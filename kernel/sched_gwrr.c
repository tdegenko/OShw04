/*
 * linux/sched_gwrr.c:
 * 	Group-Weighted Round Robin scheduling class.
 *
 * Functional(?) groupless RR scheduler, derived from sched_rt.c
 */

/* We don't know yet... */
/* 
#define NUM_GROUPS 1
static struct list_head group_queues[NUM_GROUPS];
 */
#include <linux/sched.h>
#define GWRR_WEIGHT 10

/* Group Structure */
struct gwrr_group {
    struct list_head groups;
    int weight,used;
    gid_t gid;
    struct list_head queue;
};

/* Will need initialization... in sched_init? */
/* Initialize group list */
static LIST_HEAD(group_queue);

/* Whose turn is it? Or can we do this better? */
/* we're going to do it this way for now */
static gid_t cur_group = 0;

/* Necessary?? */
#ifdef CONFIG_SMP
static int select_task_rq_gwrr(struct task_struct *p, int sync)
{
	/* 
 	 * We're not bothering with moving GWRR tasks between
 	 * CPUs, so we can just return the CPU it's currently on
 	 */
	return task_cpu(p);
}
static void set_cpus_allowed_gwrr(struct task_struct *p, cpumask_t *new_mask)
{
	/*
	 * Do we need to do something here to avoid CPU switching?
 	 * In fact, do we need to restrict the task explicitly to
 	 *  a single CPU?
 	 */ 
}
#endif 

static void check_preempt_curr_gwrr(struct rq *rq, struct task_struct *p)
{
    printk("pre-empting task\n");
	/* GWRR has lowest priority - ALWAYS preempt! */
	/* Do we need to update the timing info here?? */
//	printk("Preempting GWRR task...");
	resched_task(rq->curr);
}

struct gwrr_group * get_group(gid_t gid){
  
  struct gwrr_group * pos;

  list_for_each_entry(pos, &group_queue, groups){
    if(pos->gid == gid){
      return pos;
    }
  }
  return (struct event *) NULL;
}

static void enqueue_task_gwrr(struct rq *rq, struct task_struct *p, int wakeup)
{
	struct sched_gwrr_entity *new;
	struct list_head *queue;
    printk("Enqueuing new GWRR task %s\n",p->comm);
    gid_t gid= p->gid;
    struct gwrr_group * group=get_group(gid);
    if(group==NULL){
        group=kmalloc(sizeof(struct gwrr_group), GFP_KERNEL);
        group->gid=gid;
        group->weight=GWRR_WEIGHT;
        group->used=0;
        group->queue=(struct list_head)LIST_HEAD_INIT(group->queue);
        list_add_tail(group,&group_queue);
        printk("added new group for %d\n",gid);
    }
	queue = &(group->queue);

	new = &p->gwrr_se;
	/* add before the head of the queue - effectively
 	 * putting the new task at the end of the queue! */ 
	list_add_tail(&new->run_list,queue);
printk("Done enqueuing\n");
	/* Handle wakeup... */	
}

static void dequeue_task_gwrr(struct rq *rq, struct task_struct *p, int sleep)
{
	struct sched_gwrr_entity *sge;

 	/* delete the current sched entity from the queue */
	sge = &rq->curr->gwrr_se;
 
	list_del_init(&sge->run_list);

	/* Handle sleep... */
}

static void yield_task_gwrr(struct rq *rq)
{
	struct sched_gwrr_entity *sge;
	struct list_head *queue;
printk("Yielding from GWRR task\n");
	/*
	 * rq->curr is the currently running GWRR task 
	 * (GWRR because the kernel called this function!)
	 */
    gid_t gid= rq->curr->gid;
    struct gwrr_group * group=get_group(gid);
	sge = &rq->curr->gwrr_se;
	queue = &(group->queue);

	/* Move task back to end of queue. */
	list_del_init(&sge->run_list);
	list_add_tail(&sge->run_list,queue);
printk("Done yielding\n");
}

static struct task_struct *pick_next_task_gwrr(struct rq *rq)
{
	/*struct list_head queue = group_queues[cur_group];*/
	struct list_head *queue;
	struct sched_gwrr_entity *next;
	struct task_struct *p;
    struct gwrr_group * group;
	queue = NULL;
    if(group_queue.next==group_queue.prev){
        return NULL;
    }else if(cur_group!=0){
        group=get_group(cur_group);
        if(group==NULL){
            /* current group does not exist */
            queue=&list_first_entry(group_queue.next,struct gwrr_group,groups)->queue;
        }else if(group->used >= group->weight){
            /* current group has used all it's time */
            group->used=0;
            group=list_entry(group,struct gwrr_group,groups);
            cur_group=group->gid;
            queue=&group->queue;
        }else{
            /* current group has time left */
            group->used++;
            queue=&group->queue;
        }
    }else{
        /* current group has not been set yet*/
        group=list_first_entry(group_queue.next,struct gwrr_group,groups);
        cur_group=group->gid;
        queue=&group->queue;
    }
    printk("cur_group:\t%d\b",cur_group);

	/* Check for empty queue. */
	if (list_empty(queue)) {
		return NULL; /* no GWRR tasks left! */
	}

	/* Select next sched entity in the RR queue */
	next = list_entry(queue->next, struct sched_gwrr_entity, run_list);
	
	/* Find task struct from gwrr sched entity */
	p = container_of(next, struct task_struct, gwrr_se);

#ifdef CONFIG_SMP
	/* 
 	 * If we're at this point, we have a GWRR task that wants
 	 * running. However, since GWRR is CPU-independent, this task
 	 * may not be on the runqueue that pick_next_task was called from.
 	 * Therefore, if we're called from a different CPU than the one
 	 * the task is running from, we don't have any GWRR tasks for this
 	 * runqueue. (Logical assumption: rq ~ CPU.) 
	 */
	if (task_cpu(p) != smp_processor_id()) {
		return NULL;
	}
#endif

	printk("GWRR checked, task found!\n");
	
	/* note start time - necessary? */
	p->se.exec_start = rq->clock;
	return p; 
}

static void put_prev_task_gwrr(struct rq *rq, struct task_struct *p)
{
	/* reset start time as for all schedulers - necessary */
	p->se.exec_start = 0;
}

static void task_new_gwrr(struct rq *rq, struct task_struct *p)
{
	
	/* 
	 * Give new task's sched_gwrr_entity a link to the run-queue
 	 * it is on. The task will remain on the same processor and
 	 * we need to make sure that we don't attempt to run it on
 	 * any other processor; we use the runqueue to determine
 	 * which processor is running this task in pick_next_task
 	 */
	p->gwrr_se.rq = rq;

	/* Initialize counter with default timeslice */
	p->gwrr_se.time_slice = DEF_TIMESLICE;

printk("New GWRR task created?\n");
}

static void task_tick_gwrr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_gwrr_entity *sge;
	struct list_head *queue;

	/* On every kernel timer tick, decrement counter */
	sge = &p->gwrr_se;
	sge->time_slice--;
if (sge->time_slice < 0) printk("PANIC: We broke the timer!\n");
else printk("Tick: New ts: %d\n",sge->time_slice);
	if (sge->time_slice > 0) return;
printk("Timeslice exhausted, refilling...\n");
	/* Timeslice exhausted, refill counter... */
	sge->time_slice = DEF_TIMESLICE;

 	/* ... and roll to end of queue and request reschedule
 		 if others are waiting */
	if (sge->run_list.prev != sge->run_list.next) {	

		list_del_init(&sge->run_list);
		list_add_tail(&sge->run_list,queue);

		set_tsk_need_resched(p);		
	}

}

static void set_curr_task_gwrr(struct rq *rq)
{
	/* Seems to be used for updating priorities, but there's
 	 * not much point in that for GWRR... is there? */ 
	rq->curr->se.exec_start = rq->clock;
	/* Or maybe there is... if our task is changed to GWRR
	 * scheduling, we need to start it with a full time slice! */
	rq->curr->gwrr_se.time_slice = DEF_TIMESLICE;
	printk("Curr ts: %d\n",rq->curr->gwrr_se.time_slice);
}

static void switched_to_gwrr(struct rq *rq, struct task_struct *p,
                             int running)
{
	printk("Switched to gwrr\n");
	/* Necessary? */
}

static void prio_changed_gwrr(struct rq *rq, struct task_struct *p,
                              int oldprio, int running)
{
	/* Again, seems unnecessary */
}

/*
 * The sched_class structure linking all the relevant methods
 */
const struct sched_class gwrr_sched_class = {
	/* Prioritize GWRR just before idletask */
        .next			= &idle_sched_class,

        .enqueue_task		= enqueue_task_gwrr,
	.dequeue_task           = dequeue_task_gwrr,
	.yield_task		= yield_task_gwrr, 

        .check_preempt_curr     = check_preempt_curr_gwrr,
        .pick_next_task         = pick_next_task_gwrr,
        .put_prev_task          = put_prev_task_gwrr,
 
        .set_curr_task          = set_curr_task_gwrr,
	.task_new		= task_new_gwrr,
        .task_tick              = task_tick_gwrr,
 
        .prio_changed           = prio_changed_gwrr,
        .switched_to            = switched_to_gwrr,
 
};

/*
 * The two system calls used to update group weights...
 */
asmlinkage int sys_getgroupweight(int gid)
{
	printk("getgroupweight called with gid %d\n",gid);
    struct gwrr_group * group = get_group(gid);
    if(group==NULL){
        printk("group not found\n");
        return -1;
    }
	return group->weight;
}

asmlinkage int sys_setgroupweight(int gid, int weight)
{
	printk("setgroupweight called with gid %d, weight %d\n",gid,weight);
    struct gwrr_group * group = get_group(gid);
    if(group==NULL){
        printk("group not found\n");
        return -1;
    }
    group->weight=weight;
	return 0;
}

 

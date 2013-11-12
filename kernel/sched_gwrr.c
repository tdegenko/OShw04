/*
 * linux/sched_gwrr.c:
 * 	Group-Weighted Round Robin scheduling class.
 *
 * Almost functional weighted group RR scheduler, influenced by sched_rt.c
 */

/*
 * N.B. Using BUG_ON(group==NULL) when appropriate.
 * Now, BUG_ON is discouraged by the kernel, but the possibility of
 * group==NULL is so remote in those situations that I can't be 
 * bothered to write code to correct the error (besides, how would you?)
 * So we just pull the kernel down with us. If we've done the job
 * properly, of course, this will never happen.
 */

#define GWRR_WEIGHT 10

/* Group Structure */
struct gwrr_group {
    struct list_head groups;
    int weight,used;
    gid_t gid;
    struct list_head queue;
};

/* Initialize group list. This is more of a list than a queue
 * since nothing gets shifted out...  */
static LIST_HEAD(group_list);

/* Whose turn is it? we're going to do it this way for now */
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

struct gwrr_group * get_group(gid_t gid){
  
	struct gwrr_group * pos;

	list_for_each_entry(pos, &group_list, groups){
		if(pos->gid == gid){
			return pos;
		}
	}
	
	return (struct gwrr_group *) NULL;
}
static void check_preempt_curr_gwrr(struct rq *rq, struct task_struct *p)
{
	/* GWRR has lowest priority - ALWAYS preempt!
	 *  (Well, except for SCHED_IDLE of course...)  */
	if(p->policy != SCHED_IDLE){
		resched_task(rq->curr);
	}
}


static void enqueue_task_gwrr(struct rq *rq, struct task_struct *p, int wakeup)
{
	struct sched_gwrr_entity *new;
	struct list_head *queue;
	struct gwrr_group *group;
	gid_t gid; 

	gid = p->gid;
	group=get_group(gid);

	if(group==NULL){
		group=kmalloc(sizeof(struct gwrr_group), GFP_KERNEL);
		group->gid=gid;
		group->weight=GWRR_WEIGHT;
		group->used=0;
		group->queue=(struct list_head)LIST_HEAD_INIT(group->queue);
		/* Add the group's list element to the group list */
		list_add_tail(&group->groups,&group_list);
	}
	queue = &(group->queue);

	new = &p->gwrr_se;
	/* add before the head of the queue - effectively
 	 * putting the new task at the end of the queue! */ 
	list_add_tail(&new->run_list,queue);
	
	/* Don't bother with wakeup */	
}

static void dequeue_task_gwrr(struct rq *rq, struct task_struct *p, int sleep)
{
	struct sched_gwrr_entity *sge;

 	/* delete the current sched entity from the queue */
	sge = &rq->curr->gwrr_se;
	list_del_init(&sge->run_list);

	/* Don't bother with sleep */
}

static void yield_task_gwrr(struct rq *rq)
{
	struct sched_gwrr_entity *sge;
	struct list_head *queue;
	struct gwrr_group *group;
	gid_t gid;

	/*
	 * rq->curr is the currently running GWRR task 
	 * (GWRR because the kernel called this function!)
	 */
	gid= rq->curr->gid;
	group=get_group(gid);
	/* This should never happen! */
	BUG_ON(group==NULL);

	sge = &rq->curr->gwrr_se;
	queue = &(group->queue);

	/* Move task back to end of queue. */
	list_add_tail(&sge->run_list,queue);
}

static struct task_struct *pick_next_task_gwrr(struct rq *rq)
{
	struct list_head *queue;
	struct sched_gwrr_entity *next;
	struct task_struct *p;
	struct gwrr_group * group;
	
	queue = NULL;
	if(list_empty(&group_list)) {
		return NULL;
	} else if(cur_group!=0) {
		group=get_group(cur_group);
		BUG_ON(group==NULL); /* shouldn't happen! */

		if(group->used >= group->weight){
			/* current group has used all it's time */
			group->used=0;
			/* select next group - avoid list head! */
			if(group->groups.next==&group_list){
				group = list_first_entry(&group_list,struct gwrr_group,groups);
 			} else {
				group=list_entry(group->groups.next,struct gwrr_group,groups);
			}
			cur_group=group->gid;
			queue=&group->queue;
		} else {
			/* current group has time left */
			group->used++;
			queue=&group->queue;
		}
	} else {
		/* current group has not been set yet*/
		group=list_first_entry(&group_list,struct gwrr_group,groups);
		cur_group=group->gid;
		queue=&group->queue;
	}

	/* 
	 * If queue is empty, this group has no runnable tasks left.
	 * Rather than have it block other GWRR tasks, move the pointer 
	 * to the next one - wasting only one timeslice...
	 */
	if (list_empty(queue)) {
		if(group->groups.next==&group_list) {
			cur_group = list_first_entry(&group_list,struct gwrr_group,groups)->gid;
		} else {
			cur_group=list_entry(group->groups.next,struct gwrr_group,groups)->gid;
		}
		return NULL; /* no GWRR tasks left (for this group)! */
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
	/* currently not doing anything here... */
}

static void task_new_gwrr(struct rq *rq, struct task_struct *p)
{
	/* Initialize counter with default timeslice */
	p->gwrr_se.time_slice = DEF_TIMESLICE;

}

static void task_tick_gwrr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_gwrr_entity *sge;
	struct list_head *queue;
	struct gwrr_group *group;
	gid_t gid;

	/* On every kernel timer tick, decrement counter */
	sge = &p->gwrr_se;
	sge->time_slice--;
	if (sge->time_slice > 0) return;

	/* Timeslice exhausted, refill counter... */
	sge->time_slice = DEF_TIMESLICE;

 	/* ... and roll to end of queue and request reschedule
 		 if others are waiting */
	if (sge->run_list.prev != sge->run_list.next) {
		/* Find correct group from task */
		gid = p->gid;
		group = get_group(gid);
		BUG_ON(group==NULL);
		
        	queue = &group->queue;
		
		list_del_init(&sge->run_list);
		list_add_tail(&sge->run_list,queue);

		set_tsk_need_resched(p);		
	}
	/*
	 *  No need to reschedule if we're the only one in the queue...
	 * if anything, we'll be preempted by a higher-priority
	 * RT or GWRR process
	 */
}

static void set_curr_task_gwrr(struct rq *rq)
{
	/* When our GWRR task is set to current, we need to start it
	 *  with a full time slice! */

	rq->curr->gwrr_se.time_slice = DEF_TIMESLICE;
}

static void switched_to_gwrr(struct rq *rq, struct task_struct *p,
                             int running)
{
	/* Currently not doing anything here... */
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
 
        .switched_to            = switched_to_gwrr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_gwrr,
	.set_cpus_allowed	= set_cpus_allowed_gwrr
#endif
 
};

/*
 * The two system calls used to update group weights...
 */
asmlinkage int sys_getgroupweight(int gid)
{
	struct gwrr_group *group;

	printk("getgroupweight called with gid %d\n",gid);
	group = get_group(gid);
	if(group==NULL){
		printk("group not found\n");
		return -1;
	}
	return group->weight;
}

asmlinkage int sys_setgroupweight(int gid, int weight)
{
	struct gwrr_group *group;

	printk("setgroupweight called with gid %d, weight %d\n",gid,weight);
	group = get_group(gid);
	if(group==NULL){
		printk("group not found\n");
		return -1;
	}
	
	group->weight=weight;
	return 0;
}


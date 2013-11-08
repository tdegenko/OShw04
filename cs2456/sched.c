/*
 * cs2456/sched.c:
 * 	Group-Weighted Round Robin scheduling class.
 *
 * First, null-functionality iteration, borrowing tentatively from sched_rt.c
 */

#include <linux/sched.h>

/* We don't know yet... */
/* 
#define NUM_GROUPS 1
static struct list_head group_queues[NUM_GROUPS];
 */

/* Will need initialization... in sched_init? */
static struct list_head one_queue;

/* Whose turn is it? */
/*static int cur_group = 0;*/

/* A scheduling entity for a GWRR task */
static struct sched_gwrr_entity {
	struct list_head run_list;
	
	/* timing... */
	u64 starttime;
	u64 runtime;
	/* link back to original runqueue */
	struct rq* rq;
}

static void check_preempt_curr_gwrr(struct rq *rq, struct task_struct *p)
{
	/* GWRR has lowest priority - ALWAYS preempt! */
	/* Do we need to update the timing info here?? */
	resched_task(rq->curr);
}
 
static struct task_struct *pick_next_task_gwrr(struct rq *rq)
{
	/*struct list_head queue = group_queues[cur_group];*/
	struct list_head *queue;
	struct sched_gwrr_entity *next;
	struct task_struct *p;

	queue = &one_queue;

	/* Check for empty queue. */
	if (list_empty(queue))
		return NULL; /* no GWRR tasks left! */

	/* Select next sched entitiy in the RR queue */
	next = list_entry(queue->next, struct sched_gwrr_entity, run_list);
	
	/* Don't run this if we're called from the wrong CPU */
	if (next->rq != rq) {
		return NULL;
	}

	/* Assumes we add a gwrr_se field to struct task_struct */
	p = container_of(gwrr_se, struct task_struct, rt);
	/* note start time */
	next->starttime = rq->clock;
	/* Hmmmm.... */
	return p; 
}

static void enqueue_task_gwrr(struct rq *rq, struct task_struct *p, int wakeup)
{
	struct gwrr_sched_entity *new;
	struct list_head *queue;

	queue = &one_queue;

	new = p->gwrr_se;
	/* add before the head of the queue - effectively
 	 * putting the new task at the end of the queue! */ 
	list_add_tail(queue,new);

	/* Handle wakeup... */	
}

static void dequeue_task_gwrr(struct rq *rq, struct task_struct *p, int sleep)
{
	struct list_head *queue;

	queue = &one_queue;

 	/* delete the current head of the queue */ 
	list_del_init(queue);

	/* Handle sleep... */
}

static void yield_task_gwrr(struct rq *rq)
{
	struct gwrr_sched_entity *gse;
	struct list_head *queue;

	queue = &one_queue;

	/* Move task back to end of queue */
	gse = list_entry(queue, struct gwrr_sched_entity, run_list);
	list_del_init(queue);
	list_add_tail(queue,gse);
	
}

static void put_prev_task_gwrr(struct rq *rq, struct task_struct *p)
{
	/* reset previous task? */
	p->gwrr_se.starttime = 0;
}

static void task_new_gwrr(struct rq *rq, struct task_struct *p)
{
	/* Give new task's gwrr_sched_entity a link to the run-queue
 	 * it is on. The task will remain on the same processor and
 	 * we need to make sure that we don't attempt to run it on
 	 * any other processor; we use the runqueue to determine
 	 * which processor is running this task in pick_next_task
 	 */
	p->gwrr_se.rq = rq;
}

static void task_tick_gwrr(struct rq *rq, struct task_struct *curr, int queued)
{
	/* We need to update the timing info in here... 
 	 * currently this is doing nothing. :| */
}

static void set_curr_task_gwrr(struct rq *rq)
{
	/* Seems to be used for updating priorities, but there's
 	 * not much point in that for GWRR... */ 
}

static void switched_to_gwrr(struct rq *rq, struct task_struct *p,
                             int running)
{
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
const struct sched_class idle_sched_class = {
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
asmlinkage int getgroupweight(int gid)
{
	return 0;
}

asmlinkage int setgroupweight(int gid, int weight)
{
	return 0;
}

 

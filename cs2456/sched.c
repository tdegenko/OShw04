/*
 * cs2456/sched.c:
 * 	Group-Weighted Round Robin scheduling class.
 *
 * First, empty iteration, adapted from sched_idletask.c 
 */

#include <linux/sched.h>
 
static void check_preempt_curr_gwrr(struct rq *rq, struct task_struct *p)
{

}
 
static struct task_struct *pick_next_task_gwrr(struct rq *rq)
{
	return NULL; 
}

static void enqueue_task_gwrr(struct rq *rq, struct task_struct *p, int wakeup)
{

}

static void dequeue_task_gwrr(struct rq *rq, struct task_struct *p, int sleep)
{

}

static void yield_task_gwrr(struct rq *rq)
{

}

static void put_prev_task_gwrr(struct rq *rq, struct task_struct *prev)
{

}

static void task_tick_gwrr(struct rq *rq, struct task_struct *curr, int queued)
{

}

static void set_curr_task_gwrr(struct rq *rq)
{

}

static void switched_to_gwrr(struct rq *rq, struct task_struct *p,
                             int running)
{

}

static void prio_changed_gwrr(struct rq *rq, struct task_struct *p,
                              int oldprio, int running)
{

}

// The sched_class structure linking all the relevant methods
const struct sched_class idle_sched_class = {
	// Prioritize GWRR just before idletask
        .next			= &idle_sched_class,

        .enqueue_task		= enqueue_task_gwrr,
	.dequeue_task           = dequeue_task_gwrr,
	.yield_task		= yield_task_gwrr, 

        .check_preempt_curr     = check_preempt_curr_gwrr,
        .pick_next_task         = pick_next_task_gwrr,
        .put_prev_task          = put_prev_task_gwrr,
 
        .set_curr_task          = set_curr_task_gwrr,
        .task_tick              = task_tick_gwrr,
 
        .prio_changed           = prio_changed_gwrr,
        .switched_to            = switched_to_gwrr,
 
};

// The two system calls used to update group weights...
asmlinkage int getgroupweight(int gid)
{
	return 0;
}

asmlinkage int setgroupweight(int gid, int weight)
{
	return 0;
}

 

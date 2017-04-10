/** 
 * @file sched_myfifo.c
 * @brief Implement a toy FIFO scheduling class
 * @version 1.0
 * @date 2017-03-01
 */

#include "sched.h"

void init_myfifo_rq(struct myfifo_rq *fifo_rq)
{
	INIT_LIST_HEAD(&fifo_rq->queue);
	atomic_set(&fifo_rq->nr_running, 0);
}

static void enqueue_task_myfifo(struct rq *rq, struct task_struct *p, 
                                int wakeup, bool head)
{
    struct sched_myfifo_entity *myfifo_se = &p->myfifo;
    list_add_tail(&myfifo_se->run_list, &rq->myfifo.queue);
    atomic_inc(&rq->myfifo.nr_running);
    printk(KERN_INFO"[SCHED_MYFIFO] ENQUEUE: Process-%d\n", p->pid);
}

static void dequeue_task_myfifo(struct rq *rq, struct task_struct *p,
                                int sleep)
{
    struct sched_myfifo_entity *myfifo_se = &p->myfifo;
    list_del(&myfifo_se->run_list);
    atomic_dec(&rq->myfifo.nr_running);
    printk(KERN_INFO"[SCHED_MYFIFO] DEQUEUE: Process-%d\n", p->pid);
}

static void yield_task_myfifo(struct rq *rq)
{
    struct sched_myfifo_entity *myfifo_se = &rq->curr->myfifo;
    struct myfifo_rq *myfifo_rq = &rq->myfifo;
    list_move_tail(&myfifo_se->run_list, &myfifo_rq->queue);
}

static void check_preempt_curr_myfifo(struct rq *rq, struct task_struct *p,
                                      int flags)
{
    if (rq->curr->policy == SCHED_FIFO || rq->curr->policy == SCHED_RR)
    {
        return ;
    }
    if (rq->curr->policy == SCHED_MYFIFO)
    {
        return ;
    }
    /* preempt normal tasks */
    resched_task(rq->curr);
}

static struct task_struct *pick_next_task_myfifo(struct rq *rq)
{
    struct sched_myfifo_entity *myfifo_se = NULL;
    struct task_struct *p = NULL;
    struct myfifo_rq *myfifo_rq = &rq->myfifo;
    if (list_empty(&myfifo_rq->queue))
    {
        return NULL;
    }
    myfifo_se = list_entry(myfifo_rq->queue.next,
                           struct sched_myfifo_entity,
                           run_list);
    p = container_of(myfifo_se, struct task_struct, myfifo);
    return p;
}

static void put_prev_task_myfifo(struct rq *rq, struct task_struct *p)
{
    /* it is the place to update the current task's runtime statistics */
}


static void set_curr_task_myfifo(struct rq *rq)
{
    /* in our case, we don't need it */
}

static void task_tick_myfifo(struct rq *rq, struct task_struct *p,
                             int queued)
{
    if (p->policy != SCHED_MYFIFO)
    {
        return ;
    }
    /* FIFO has no timeslice management! */
}

unsigned int get_rr_interval_myfifo(struct rq *rq, struct task_struct *p)
{
    /* FIFO has no timeslice management! */
    return 0;
}

static void prio_changed_myfifo(struct rq *rq, struct task_struct *p,
                                int oldprio, int running)
{
    /* Toy FIFO has no priority supported! */
}

static void switched_to_myfifo(struct rq *rq, struct task_struct *p,
                               int running)
{
    /* nothing to do */
}


const struct sched_class myfifo_sched_class = {
    .next = &fair_sched_class,
    .enqueue_task = enqueue_task_myfifo,
    .dequeue_task = dequeue_task_myfifo,
    .yield_task = yield_task_myfifo,
    .check_preempt_curr = check_preempt_curr_myfifo,
    .pick_next_task = pick_next_task_myfifo,
    .put_prev_task = put_prev_task_myfifo,



    .set_curr_task = set_curr_task_myfifo,
    .task_tick = task_tick_myfifo,
    .get_rr_interval = get_rr_interval_myfifo,
    .prio_changed = prio_changed_myfifo,
    .switched_to = switched_to_myfifo,
};


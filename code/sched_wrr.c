#include "sched.h"
#include <linux/kernel.h>

void init_wrr_rq (struct wrr_rq *wrr_rq) {
	INIT_LIST_HEAD(&wrr_rq->queue);
}	

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p,
								int wakeup, bool head)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;
	list_add_tail(&wrr_se->run_list, &rq->wrr.queue);
	printk(KERN_INFO"[SCHED_wrr] ENQUEUE: Process-%d\n", p->pid);
}

static void dequeue_task_wrr(struct rq *rq,
								struct task_struct *p, int sleep)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;
	list_del(&wrr_se->run_list);
	printk(KERN_INFO"[SCHED_wrr] DEQUEUE: Process-%d\n", p->pid);
}

static void yield_task_wrr(struct rq *rq)
{
	struct sched_wrr_entity *wrr_se = &rq->curr->wrr;
	struct wrr_rq *wrr_rq = &rq->wrr;
	list_move_tail(&wrr_se->run_list, &wrr_rq->queue);
}

static void check_preempt_curr_wrr(struct rq *rq,
									struct task_struct *p, int flags)
{
	if (rq->curr->policy == SCHED_FIFO || rq->curr->policy == SCHED_WRR)
	return ;
	if (rq->curr->policy == SCHED_WRR)
	return ;
	/* preempt normal tasks */
	resched_task(rq->curr);
}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
	struct sched_wrr_entity *wrr_se = NULL;
	struct task_struct *p = NULL;
	struct wrr_rq *wrr_rq = &rq->wrr;
	if (list_empty(&wrr_rq->queue))
	return NULL;
	wrr_se = list_entry(wrr_rq->queue.next,
	struct sched_wrr_entity,
	run_list);
	p = container_of(wrr_se, struct task_struct, wrr);
	return p;
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p)
{
	/* it is the place to update the current task's
	* runtime statistics */
}

static void set_curr_task_wrr(struct rq *rq)
{
}

int get_total_weight(struct list_head *head){
	int total_weight = 0;
	struct sched_wrr_entity *wrr_se;
	struct list_head *pos;
	
	list_for_each(pos, head){
		if(pos != head){
			wrr_se = list_entry(pos, struct sched_wrr_entity, run_list);
			total_weight += wrr_se->wrr_weight;
		}
	}
	
	return total_weight;
}

int get_total_timeslice(struct list_head *head){
	int total_timeslice = 0;
	struct list_head *pos;
	
	list_for_each(pos, head){
		if(pos != head){
			total_timeslice += 100;
		}
	}
	
	return total_timeslice;
}

int get_timeslice(struct list_head *head, int weight){
	int total_weight, total_timeslice;
	
	total_weight = get_total_weight(head);
	total_timeslice = get_total_timeslice(head);
	
	return (int)(total_timeslice * ((weight * 100) / total_weight) / 100);
}


static void task_tick_wrr(struct rq *rq,
							struct task_struct *p, int queued)
{
	struct sched_wrr_entity *wrr_se = &rq->curr->wrr;
	if (p->policy != SCHED_WRR) return ;
	
	if(--p->wrr.time_slice) 
		 return;
	
	
	p->wrr.time_slice = get_timeslice(&rq->wrr.queue, wrr_se->wrr_weight);
	list_move_tail(&wrr_se->run_list, &rq->wrr.queue);
	set_tsk_need_resched(p);	
}

unsigned int get_wrr_interval_wrr(struct rq *rq,
									struct task_struct *p)
{
	/* ToDo: return a default timeslice */
	struct sched_wrr_entity *wrr_se = &rq->curr->wrr;
	return get_timeslice(&rq->wrr.queue, wrr_se->wrr_weight);
}

static void prio_changed_wrr(struct rq *rq, struct task_struct *p,
								int oldprio, int running)
{
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p,
							int running)
{
}

const struct sched_class wrr_sched_class = {
    .next = &fair_sched_class,
    .enqueue_task = enqueue_task_wrr,
    .dequeue_task = dequeue_task_wrr,
    .yield_task = yield_task_wrr,
    .check_preempt_curr = check_preempt_curr_wrr,
    .pick_next_task = pick_next_task_wrr,
    .put_prev_task = put_prev_task_wrr,



    .set_curr_task = set_curr_task_wrr,
    .task_tick = task_tick_wrr,
    .get_rr_interval = get_wrr_interval_wrr,
    .prio_changed = prio_changed_wrr,
    .switched_to = switched_to_wrr,
};

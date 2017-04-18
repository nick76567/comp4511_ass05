#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

asmlinkage long sys_set_wrr_weight(pid_t pid, int weight){
	struct task_struct *task;
	task = find_task_by_vpid(pid);

	if(task = NULL) return -1;

	task->sched_wrr_entity.wrr_weight = weight;
	printk(KERN_INFO "wrr_weight = %d\n", task->sched_wrr_entity.wrr_weight);
	return 0;
}
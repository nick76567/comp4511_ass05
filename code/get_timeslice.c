#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({                      \
             const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
             (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
  container_of(ptr, type, member)

#define list_for_each(pos, head) \
   for (pos = (head)->next; pos != (head); pos = pos->next)

struct list_head {
  struct list_head *next, *prev;
};

struct sched_wrr_entity {
	struct list_head run_list;
	int wrr_weight;
	int timeslice;
};

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
	
	return (int)((total_timeslice * ((weight *100)/ total_weight)) / 100);
}

int main(){
	struct sched_wrr_entity wrr_se1, wrr_se2, wrr_se3;
	wrr_se1.run_list.next = &wrr_se2.run_list;
	wrr_se2.run_list.next = &wrr_se3.run_list;
	wrr_se3.run_list.next = &wrr_se1.run_list;

	wrr_se1.wrr_weight = 1;
	wrr_se2.wrr_weight = 2;
	wrr_se3.wrr_weight = 3;

	wrr_se1.timeslice = 10;
	wrr_se2.timeslice = 10;
	wrr_se3.timeslice = 10;

	printf("Total weight %d\n", get_total_weight(&wrr_se1.run_list));
	printf("Total timeslice %d\n", get_total_timeslice(&wrr_se1.run_list));
	printf("Total get_timeslice %d\n", get_timeslice(&wrr_se1.run_list, 3));
	return 0;
}
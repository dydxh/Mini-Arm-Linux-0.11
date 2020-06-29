#include "sched.h"
#include "irq.h"
#include "mm.h"
#include "printf.h"
#include "utils.h"

static struct task_struct init_task;
struct task_struct* current;
struct task_struct* task[NR_TASKS];
int nr_tasks;

void sched_init() {
    init_task = (struct task_struct)INIT_TASK;
    current = &init_task;
    task[0] = &init_task;
    nr_tasks = 1;
}

void _schedule() {
    preempt_disable();
    printf("[^] Enter `_schedule` function at ");
    phex((unsigned long)&_schedule);
    int idx, max_remain;
    struct task_struct* iter;

#ifndef PREEMPT
    while(1) {
        idx = -1, max_remain = 0;
        for(int i = 0; i < NR_TASKS; i++) {
            iter = task[i];
            if(iter && iter->state == TASK_RUNNING && iter->counter > max_remain) {
                max_remain = iter->counter;
                idx = i;
            }
        }
#else
    int max_prio;
    while(1) {
        idx = -1, max_remain = 0, max_prio = 0;
        for(int i = 0; i < NR_TASKS; i++) {
            iter = task[i];
            if(iter && iter->state == TASK_RUNNING && ((iter->add_priority > max_prio && iter->counter > 0) || (iter->add_priority == max_prio && iter->counter > max_remain))) {
                max_prio = iter->add_priority;
                max_remain = iter->counter;
                idx = i;
            }
        }
#endif
        if(max_remain)
            break;
        for(int i = 0; i < NR_TASKS; i++) {
            iter = task[i];
            if(iter) {
                iter->counter = (iter->counter >> 1) + iter->priority;
            }
        }
    }

    switch_to(task[idx]);
    preempt_enable();
}

void schedule() {
    current->counter = 0;
    _schedule();
}

void timer_tick() {
    current->counter--;
#ifndef PREEMPT
    if(current->counter > 0 || current->preempt_count > 0) {
#else
    if(current->preempt_count > 0) {
#endif
        return ;
    }
    if(current->counter < 0)
        current->counter = 0;

    enable_irq();
    _schedule();
    disable_irq();
}

void preempt_disable() {
    current->preempt_count++;
}

void preempt_enable() {
    current->preempt_count--;
}

void preempt_done() {
    preempt_enable();
}

void switch_to(struct task_struct* next) {
#ifndef PREEMPT
    printf("[-] Switch to task with pid: %d, prio: %u, counter: %d, task_struct's va: ", next->pid, next->priority, next->counter);
    phex((unsigned long)&switch_to);
#else
    printf("[-] Switch to task with pid: %d, add_prio: %u, counter: %d, task_struct's va: ", next->pid, next->priority, next->counter);
    phex(next);
#endif
    printf("[^] Enter `switch_to` function at ");
    phex(1);
    printf("\n");
    if(current == next)
        return ;
    struct task_struct* prev = current;
    current = next;
    set_pgd(next->mm.pgd);
    cpu_switch_to(prev, next);
}

int getpid() {
    return current->pid;
}

int getcounter() {
    return current->counter;
}

void exit_process() {
    preempt_disable();
	for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task[i]->state = TASK_ZOMBIE;
			break;
		}
	}
    for(int i = 0; i < task[i]->mm.user_pages_cnt; i++) {
        free_page(task[i]->mm.user_pages[i].phys_addr);
    }
    for(int i = 0; i < task[i]->mm.kernel_pages_cnt; i++) {
        free_page(task[i]->mm.kernel_pages[i]);
    }

	preempt_enable();
	schedule();
}

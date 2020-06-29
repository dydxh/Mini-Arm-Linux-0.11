#include "printf.h"
#include "mini_uart.h"
#include "sched.h"
#include "fork.h"
#include "mm.h"

void sys_write(char* buf) {
    puts(buf);
}

int sys_fork() {
    return copy_process(0, 0, 0);
}

void sys_exit() {
    exit_process();
}

int sys_set_priority(int pid, int priority) {
    if(pid < nr_tasks) {
        for(int i = 0; i < NR_TASKS; i++) {
            if(task[i] && task[i]->pid == pid) {
#ifndef PREEMPT
                task[i]->priority = priority;
                return 1;
#else
                printf("[*] task %d change priority from %d to %d\n", pid, task[i]->add_priority, priority);
                task[i]->add_priority = priority;
                _schedule();
                return 1;
#endif
            }
        }
    }
    return 0;
}

int sys_get_pid() {
    return current->pid;
}

void * const sys_call_table[] = {sys_write, sys_fork, sys_exit, sys_set_priority, sys_get_pid};
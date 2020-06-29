#ifndef _SCHED_H
#define _SCHED_H

#define NR_TASKS 64
#define CPU_CONTEXT_OFFSET 0
#define THREAD_SIZE (1 << 12)

#define TASK_RUNNING   0
#define TASK_ZOMBIE	   1

#define PF_KTHREAD     0x00000002	

#ifndef PREEMPT
#define INIT_TASK \
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
    0, TASK_RUNNING, 0, 5, 0, PF_KTHREAD, \
    {0, 0, {{0, 0}}, 0, {0}}}
#else
#define INIT_TASK \
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
    0, TASK_RUNNING, 0, 5, 0, 0, PF_KTHREAD, \
    {0, 0, {{0, 0}}, 0, {0}}}
#endif

#ifndef __ASSEMBLER__

extern int nr_tasks;
extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];

struct cpu_context {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp; // x29
    unsigned long pc; // x30
    unsigned long sp;
};

#define MAX_PROCESS_PAGES 16

struct user_page {
    unsigned long phys_addr;
    unsigned long virt_addr;
    unsigned long attr;
};

struct kernel_page {
    unsigned long phys_addr;
    unsigned long attr;
};

struct mm_struct {
    unsigned long pgd;
    unsigned int user_pages_cnt;
    struct user_page user_pages[MAX_PROCESS_PAGES];
    unsigned int kernel_pages_cnt;
    unsigned long kernel_pages[MAX_PROCESS_PAGES];
};

struct task_struct {
    struct cpu_context cpu_context; // tss struct in arm64
    long pid;
    long state;
    long counter;
    long priority;
    long preempt_count;
#ifdef PREEMPT
    long add_priority;
#endif
    unsigned long flags;
    struct mm_struct mm;
};

extern void sched_init();
extern void schedule();
extern void timer_tick();
extern void preempt_disable();
extern void preempt_enable();
extern void preempt_done();
extern void switch_to(struct task_struct* prev);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern void exit_process();
extern int getpid();
extern int getcounter();
#endif

#endif
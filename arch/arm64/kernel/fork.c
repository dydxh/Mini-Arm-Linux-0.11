#include "mm.h"
#include "fork.h"
#include "sched.h"
#include "entry.h"
#include "irq.h"
#include "printf.h"
#include "utils.h"
#include "user.h"

int copy_process(unsigned long flags, unsigned long func, unsigned long argv) {
    preempt_disable();
    struct task_struct* proc = (struct task_struct*)alloc_kernel_page();
    if(!proc) {
        return -1;
    }

    struct pt_regs* regs = task_pt_regs(proc);

    if(flags & PF_KTHREAD) {    // if it is kernel process
        proc->cpu_context.x19 = func;
        proc->cpu_context.x20 = argv;
    }
    else {   // if it is user process
        *regs = *task_pt_regs(current); // asm to memcpy
        regs->regs[0] = 0; // return value = 0
        copy_virt_memory(proc);
    }
    
    proc->flags = flags;
    proc->priority = current->priority;
    proc->state = TASK_RUNNING;
    proc->counter = proc->priority;
    proc->preempt_count = 1;
    proc->pid = nr_tasks++;

#ifdef PREEMPT
    proc->add_priority = 1;
#endif

    proc->cpu_context.pc = (unsigned long)ret_from_fork;
    proc->cpu_context.sp = (unsigned long)regs;

    int idx = -1;
    for(idx = 0; idx < NR_TASKS; idx++) {
        if(!task[idx])
            break;
    }
    if(idx == -1) {
        return -1;
    }
    task[idx] = proc;

    preempt_enable();
    return proc->pid;
}

int move_to_user_mode(unsigned long pc) {
    struct pt_regs *regs = task_pt_regs(current);

	regs->pc = pc - (unsigned long)&user_begin;
	regs->pstate = PSR_MODE_EL0t;
    regs->sp = 2 * PAGE_SIZE - PAGE_SIZE / 2;

    if(!current->mm.pgd) {
        current->mm.pgd = alloc_page();
        current->mm.kernel_pages[current->mm.kernel_pages_cnt++] = current->mm.pgd;
    }
    set_pgd(current->mm.pgd);
	return 0;
}

struct pt_regs * task_pt_regs(struct task_struct *tsk) {
    return (struct pt_regs*)(tsk + THREAD_SIZE - sizeof(struct pt_regs));
}

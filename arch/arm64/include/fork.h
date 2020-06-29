#ifndef _FORK_H
#define _FORK_H
#include "sched.h"

#define FORK_SUCC 0
#define FORK_FAIL 1

#define PSR_MODE_EL0t	0x00000000
#define PSR_MODE_EL1t	0x00000004
#define PSR_MODE_EL1h	0x00000005
#define PSR_MODE_EL2t	0x00000008
#define PSR_MODE_EL2h	0x00000009
#define PSR_MODE_EL3t	0x0000000c
#define PSR_MODE_EL3h	0x0000000d

#ifndef __ASSEMBLER__
extern int copy_process(unsigned long flags, unsigned long func, unsigned long argv);
int move_to_user_mode(unsigned long pc);
struct pt_regs * task_pt_regs(struct task_struct *tsk);

struct pt_regs {
	unsigned long regs[31];
    unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};
#endif

#endif
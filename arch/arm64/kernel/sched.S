#include "sched.h"

.globl cpu_switch_to
// extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
cpu_switch_to:
    mov x9, #CPU_CONTEXT_OFFSET

    add x10, x0, x9
    mov x11, sp
    stp x19, x20, [x10], #16
    stp x21, x22, [x10], #16
    stp x23, x24, [x10], #16
    stp x25, x26, [x10], #16
    stp x27, x28, [x10], #16
    stp x29, x30, [x10], #16
    str x11, [x10]

    add x10, x1, x9
    ldp x19, x20, [x10], #16
    ldp x21, x22, [x10], #16
    ldp x23, x24, [x10], #16
    ldp x25, x26, [x10], #16
    ldp x27, x28, [x10], #16
    ldp x29, x30, [x10], #16
    ldr x11, [x10]
    mov sp, x11

    ret

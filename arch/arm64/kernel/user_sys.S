#include "user_sys.h"

.globl user_delay
user_delay:
	subs x0, x0, #1
	bne user_delay
	ret

.globl call_sys_write
call_sys_write:
	mov w8, #__NR_write	
	svc #0
	ret

.globl call_sys_fork
call_sys_fork:
	mov	x8, #__NR_fork
	svc	0x0
	ret

.globl call_sys_exit
call_sys_exit:
	mov w8, #__NR_exit
	svc #0
	ret

.globl call_sys_set_priority
call_sys_set_priority:
	mov w8, #__NR_set_priority
	svc #0
	ret

.globl call_sys_get_pid
call_sys_get_pid:
	mov w8, #__NR_get_pid
	svc #0
	ret

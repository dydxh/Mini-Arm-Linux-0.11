#ifndef _SYS_H
#define _SYS_H

#define __NR_syscalls 6

#define __NR_write    0
#define __NR_fork     1
#define __NR_exit     2
#define __NR_set_priority 3
#define __NR_get_pid  4

#ifndef __ASSEMBLER__

void user_delay(unsigned long val);
void call_sys_write(char* buf);
int call_sys_fork();
void call_sys_exit();
int call_sys_set_priority(int pid, int priority);
int call_sys_get_pid();

#endif

#endif
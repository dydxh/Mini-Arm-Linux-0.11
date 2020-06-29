#include "user_sys.h"
#include "user.h"
#include "printf.h"

void loop(char* str) {
	char buf[2] = {""};
	while (1){
		for (int i = 0; i < 5; i++){
			buf[0] = str[i];
			call_sys_write(buf);
			user_delay(1000000);
		}
	}
}

void user_process() {
	int pid = call_sys_get_pid();
	char buf[] = "[pid = 0] Hello page fault from user space.\n\r";
	buf[7] = '0' + pid;
	call_sys_write(buf);
	while(1);
}


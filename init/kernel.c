#include "printf.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "utils.h"
#include "user_sys.h"
#include "user.h"
#include "mini_uart.h"

void kernel_process(){
	// printf("Kernel process started. EL %d\r\n", get_el());
	int err = move_to_user_mode((unsigned long)&user_process);
	if (err < 0){
		printf("Error while moving process to user mode\n\r");
	}
}

void start_kernel(void) {
	uart_init();
	init_printf(0, putc);
	sched_init();
	irq_vector_init();
	timer_init();
	enable_interrupt_controller();
	enable_irq();

	uart_send('O');

	printf("That looks on tempests and is never shaken.\n");

	int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}

	res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}

	res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}

	res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}

	while(1) {
		schedule();
	}
}

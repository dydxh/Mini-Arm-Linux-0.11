#include "utils.h"
#include "printf.h"
#include "mini_uart.h"
#include "sched.h"
#include "peripherals/timer.h"
#include "peripherals/irq.h"

const unsigned int interval = 0x7fffff;
// const unsigned int interval = 0xfffff;

void clear_and_reload();

void timer_init() {
	unsigned int val;

	// Routing IRQ to Core 0
	val = get32(LOCAL_TIMER_ROUTING);
	val = val & (~LOCAL_TIMER_ROUTING_TARGET);
	put32(LOCAL_TIMER_ROUTING, val);

	// re-load = interval
	val = get32(LOCAL_TIMER_IRQ_CS);
	val = (val & (~LOCAL_TIMER_RELOAD)) | interval;
	put32(LOCAL_TIMER_IRQ_CS, val);
}

void clear_and_reload() {
	unsigned int val;

	// clear interrupt flag and reload
	val = get32(LOCAL_TIMER_IRQ_CR);
	val = val | LOCAL_TIMER_RESET;
	put32(LOCAL_TIMER_IRQ_CR, val);
}

void handle_timer_irq() {
	// printf("Tick...\n");
	// static int count = 0;
	clear_and_reload();
	timer_tick();
	// printf("Timer interrupt received! count: %d\n", ++count);
}

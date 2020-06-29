#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals/irq.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32",
	
	"SYNC_ERROR",
	"SYSCALL_ERROR",
	"DATA_ABORT_ERROR",
	"INST_ABORT_ERROR"
};

void enable_interrupt_controller() {
	unsigned int val;

	// enable local timer
	val = get32(LOCAL_TIMER_IRQ_CS);
	val = val | LOCAL_TIMER_INT_EN | LOCAL_TIMER_TIMER_EN;
	put32(LOCAL_TIMER_IRQ_CS, val);

	// clear CNTxxIRQ FIQ
	val = get32(TIMER_INT_CTRL_C0);
	val = val & (~CORE_CNTxxIRQ_FIQ_EN);
	put32(TIMER_INT_CTRL_C0, val);

	// enable CNTxxIRQ IRQ
	val = get32(TIMER_INT_CTRL_C0);
	val = val | CORE_CNTxxIRQ_IRQ_EN;
	put32(TIMER_INT_CTRL_C0, val);
}

void debug_tag(int tag) {
	printf("[tag]: %x is ok.\n", tag);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address) {
	printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq(void) {
	unsigned int val = get32(INT_SRC_C0);

	for(unsigned int i = 0; i < 32; i++) {
		if(!(val & (1 << i))) continue;

		if ((1 << i) == LOCAL_TIMER_INT) {
			handle_timer_irq();
			break;
		}
		else {
			printf("Unknown pending irq: %x\r\n", val);
		}
	}
}

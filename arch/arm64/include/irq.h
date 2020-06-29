#ifndef	_IRQ_H
#define	_IRQ_H

void enable_interrupt_controller();

void debug_tag(int val);
void irq_vector_init();
void enable_irq();
void disable_irq();

#endif  /*_IRQ_H */

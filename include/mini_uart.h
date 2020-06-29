#ifndef	_MINI_UART_H
#define	_MINI_UART_H

void uart_init ();
char uart_recv ();
void uart_send ( char c );
void putc ( void* p, char c );
void puts(char* p);
void phex(unsigned long p);

#endif  /*_MINI_UART_H */

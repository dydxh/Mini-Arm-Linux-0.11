#ifndef	_UTILS_H
#define	_UTILS_H

void delay(unsigned long);
void put32(unsigned long, unsigned int);
unsigned int get32(unsigned long);
int get_el();
void set_pgd(unsigned long pgd);
unsigned long get_pgd();

#endif  /*_UTILS_H */

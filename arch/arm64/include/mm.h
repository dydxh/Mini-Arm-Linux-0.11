#ifndef _MM_H
#define _MM_H
#include "peripherals/base.h"
#include "sched.h"

#define LMA 0x80000

#define VA0_START         0x0000000000000000
#define VA1_START         0xffff000000000000

#define PA0_START         0x00000000
#define PA1_START         0X30000000
#define PHYS_MEMORY_SIZE  0x40000000

#define VA0_MASK        0x0000fffffffff000
#define VA1_MASK        0xfffffffffffff000

#define PAGE_SHIFT      12
#define TABLE_SHIFT     9
#define SECTION_SHIFT   (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE       (1 << PAGE_SHIFT)
#define SECTION_SIZE    (1 << SECTION_SHIFT)

#define LOW_MEMORY      (2 * SECTION_SIZE)
#define HIGH_MEMORY     (DEVICE_BASE)

#define PAGING_MEMORY   0x200000000 //(HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES    (PAGING_MEMORY / PAGE_SIZE)
#define PAGES_BLOCKS    (PAGING_PAGES >> 6)

#define PTRS_PER_TABLE  (1 << TABLE_SHIFT)

#define PGD_SHIFT       (PAGE_SHIFT + 3*TABLE_SHIFT)
#define PUD_SHIFT       (PAGE_SHIFT + 2*TABLE_SHIFT)
#define PMD_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define TTBR0_PG_SIZE   (3 * PAGE_SIZE)
#define TTBR1_PG_SIZE   (3 * PAGE_SIZE)

#ifndef __ASSEMBLER__
extern unsigned long ttbr0_pos;
extern unsigned long ttbr1_pos;

void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long dst, unsigned long src, unsigned long n);
void memset(unsigned long dst, unsigned long val, unsigned long size);
unsigned long alloc_page();
void free_page(unsigned long addr);

void map_page(struct task_struct *task, unsigned long va, unsigned long page, unsigned long flags);
int copy_virt_memory(struct task_struct *dst); 
unsigned long alloc_kernel_page(); 
unsigned long alloc_user_page(struct task_struct* task, unsigned long va, unsigned long flags);
#endif

#endif  /*_MM_H */

#include "arm/sysregs.h"
#include "arm/mmu.h"

#include "mm.h"

.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1		
	and	x0, x0, #0xFF		// Check processor id
	cbz	x0, master		// Hang for all non-primary CPU
	b proc_hang

proc_hang: 
	b proc_hang

// We are at el2 in qemu, maybe el3 on real raspi3
master:
	ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0

	ldr	x0, =HCR_VALUE
	msr	hcr_el2, x0

	ldr	x0, =SPSR_VALUE
	msr	spsr_el2, x0

	adr	x0, el1_entry		
	msr	elr_el2, x0

	eret

el1_entry:
	adrp x0, __bss_start
	ldr x1, =__bss_size
	bl memzero

	bl __create_ttbr0_tbls
	adrp x0, ttbr0_pos
	msr ttbr0_el1, x0

	bl __create_ttbr1_tbls
	adrp x0, ttbr1_pos
	msr ttbr1_el1, x0

	ldr x0, =__stack_end
	mov sp, x0

	ldr x0, =(TCR_VALUE)
	msr tcr_el1, x0

	ldr x0, =(MAIR_VALUE)
	msr mair_el1, x0

	ldr x2, =start_kernel

	ldr x0, =(SCTLR_MMU_ENABLED)
	msr sctlr_el1, x0

	br x2

////////////////////// create page table //////////////////////

.macro create_pgd_entry, tbl, virt, tmp1, tmp2
	create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
	create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
.endm

.macro create_table_entry, tbl, virt, shift, tmp1, tmp2
	lsr \tmp1, \virt, #\shift
	and \tmp1, \tmp1, #PTRS_PER_TABLE - 1
	add \tmp2, \tbl, #PAGE_SIZE
	orr \tmp2, \tmp2, #MM_TYPE_PAGE_TABLE
	str \tmp2, [\tbl, \tmp1, lsl #3]
	add \tbl, \tbl, #PAGE_SIZE
.endm

.macro create_pmd_entry, tbl, phys, start, size, flags, tmp1
	lsr \start, \start, #SECTION_SHIFT
	and \start, \start, #PTRS_PER_TABLE - 1
	lsr \phys, \phys, #SECTION_SHIFT
	mov \tmp1, #\flags
	orr \phys, \tmp1, \phys, lsl #SECTION_SHIFT
9999:
	str \phys, [\tbl, \start, lsl #3]
	add \start, \start, #1
	sub \size, \size, #1
	add \phys, \phys, #SECTION_SIZE
	cmp \size, #0
	b.gt 9999b
.endm

.macro create_pte_entry, tbl, phys, start, size, flags, tmp1
	lsr \start, \start, #PAGE_SHIFT
	and \start, \start, #PTRS_PER_TABLE - 1
	lsr \phys, \phys, #PAGE_SHIFT
	mov \tmp1, #\flags
	orr \phys, \tmp1, \phys, lsl #PAGE_SHIFT
9999:
	str \phys, [\tbl, \start, lsl #3]
	add \start, \start, #1
	sub \size, \size, #1
	add \phys, \phys, #PAGE_SIZE
	cmp \size, #0
	b.gt 9999b
.endm

////////////////////// ttbr0 el1 //////////////////////

__create_ttbr0_tbls:
	mov x29, x30

	adrp x0, ttbr0_pos
	mov x1, #TTBR0_PG_SIZE
	bl memzero

	adrp x0, ttbr0_pos
	mov x1, #VA0_START
	
	create_pgd_entry x0, x1, x2, x3 // create the initial PGD table

	mov x1, #PA0_START
	mov x2, #VA0_START
	ldr x3, =(VA0_START + LOW_MEMORY)
	sub x3, x3, x2
	lsr x3, x3, #SECTION_SHIFT
	create_pmd_entry x0, x1, x2, x3, MMU_FLAGS, x4

	mov x30, x29
	ret

////////////////////// ttbr1 el1 //////////////////////
// pmd_addr@x10, pmd_idx@x11, pte_addr@x12, pte_idx@x13, phys_addr@x14

.macro add_pmd_entry
	mov x13, xzr
	add x12, x12, #PAGE_SIZE
	add x11, x11, #1
	orr x15, x12, #MM_TYPE_PAGE_TABLE
	str x15, [x10, x11, lsl #3]
.endm

// add_pte_entry(idx@x0) -> void
.macro add_pte_entry, idx, flags
	orr x15, x14, \flags
	str x15, [x12, \idx, lsl #3]
	add x14, x14, #PAGE_SIZE
.endm

// add_section_mapping(virt_addr@x0, size@x1, flags@x2) -> void
add_section_mapping:
	lsr x0, x0, #PAGE_SHIFT
	and x0, x0, #PTRS_PER_TABLE - 1

	cmp x0, x13
	bne idx_corrupted

looper:
	cmp x1, #0
	b.le done
	add_pte_entry x13, x2
	add x13, x13, #1
	sub x1, x1, #PAGE_SIZE

	cmp x13, 0x200
	b.lt looper
	add_pmd_entry
	b looper
done:
	ret
idx_corrupted:
	b idx_corrupted

__create_ttbr1_tbls:
	mov x29, x30

	adrp x0, ttbr1_pos
	mov x1, #TTBR1_PG_SIZE
	bl memzero

	adrp x0, ttbr1_pos
	ldr x1, =VA1_START
	create_pgd_entry x0, x1, x2, x3 // create the initial PGD table

	mov x1, #0
	ldr x2, =(VA1_START)
	ldr x3, =(VA1_START + DEVICE_BASE)
	sub x3, x3, x2
	lsr x3, x3, #SECTION_SHIFT
	create_pmd_entry x0, x1, x2, x3, MMU_FLAGS, x4

	mov x1, #DEVICE_BASE
	ldr x2, =(VA1_START + DEVICE_BASE)
	ldr x3, =(VA1_START + PHYS_MEMORY_SIZE)
	sub x3, x3, x2
	lsr x3, x3, #SECTION_SHIFT
	add x3, x3, #8
	create_pmd_entry x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	adrp x0, ttbr1_pos
	add x0, x0, #PAGE_SIZE
	ldr x2, [x0, #0]
	add x2, x2, #PAGE_SIZE
	str x2, [x0, #8]

/////////////// kernel section mapping ///////////////
	adrp x2, ttbr1_pos
	add x10, x2, #PAGE_SIZE * 2
	mov x11, xzr
	add x12, x10, #PAGE_SIZE * 2
	mov x13, xzr
	mov x14, #0x80000

	orr x15, x12, #MM_TYPE_PAGE_TABLE
	str x15, [x10, #0]

	ldr x0, =__text_start
	ldr x1, =__text_size
	ldr x2, =MMU_PTE_TEXT
	bl add_section_mapping

	ldr x0, =__rodata_start
	ldr x1, =__rodata_size
	ldr x2, =MMU_PTE_RODATA
	bl add_section_mapping

	ldr x0, =__data_start
	ldr x1, =__data_size
	ldr x2, =MMU_PTE_DATA
	bl add_section_mapping

	ldr x0, =__bss_start
	ldr x1, =__bss_size
	ldr x2, =MMU_PTE_BSS
	bl add_section_mapping

	ldr x0, =__pgd_start
	ldr x1, =__pgd_size
	ldr x2, =MMU_PTE_PGD
	bl add_section_mapping

	ldr x0, =__stack_start
	ldr x1, =__stack_size
	ldr x2, =MMU_PTE_STACK
	bl add_section_mapping

	mov x30, x29
	ret

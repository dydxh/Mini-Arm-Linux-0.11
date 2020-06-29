#include "mm.h"
#include "arm/mmu.h"
#include "user.h"
#include "printf.h"

static unsigned long mempool[PAGES_BLOCKS] = {0, }; // bitmap

unsigned long check_and_set(unsigned long idx) {
    unsigned int i = 0;
    while(mempool[idx] & (1 << i)) i++;
    mempool[idx] ^= (1 << i);
    return i;
}
void clearbit(unsigned long idx, unsigned long offset) {
    if(mempool[idx] & (1uLL << offset)) {
        mempool[idx] ^= 1uLL << offset;
    }
}

unsigned long alloc_kernel_page() {
    unsigned long page = alloc_page();
    return page ? VA1_START + page : 0;
}

unsigned long alloc_user_page(struct task_struct* task, unsigned long va, unsigned long flags) {
    unsigned long page = alloc_page();
    if(!page) return 0;
    map_page(task, va & VA0_MASK, page, flags);
    return VA1_START + page;
}

unsigned long alloc_page() {
    for(int i = 0, pos = 0; i < PAGES_BLOCKS; i++, pos += 64) {
        if(mempool[i] != 0xffffffffffffffffuLL) {
            pos += check_and_set(i);
            memzero(VA1_START + LOW_MEMORY + PAGE_SIZE * pos, PAGE_SIZE);
            return LOW_MEMORY + PAGE_SIZE * pos;
        }
    }
    return 0;
}

void free_page(unsigned long addr) {
    addr = (addr - LOW_MEMORY) / PAGE_SIZE;
    clearbit(addr >> 6, addr & 63);
}

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa, unsigned long flags) {
	unsigned long index = va >> PAGE_SHIFT;
	index = index & (PTRS_PER_TABLE - 1);
	unsigned long entry = pa | flags;
	pte[index] = entry;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, int* new_table) {
	unsigned long index = va >> shift;
	index = index & (PTRS_PER_TABLE - 1);
	if (!table[index]){
		*new_table = 1;
		unsigned long next_level_table = alloc_page();
		unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
		table[index] = entry;
		return next_level_table;
	} else {
		*new_table = 0;
	}
	return table[index] & VA1_MASK;
}

void map_page(struct task_struct* task, unsigned long va, unsigned long pa, unsigned long flags) {
    if(!task->mm.pgd) {
        task->mm.pgd = alloc_page();
        task->mm.kernel_pages[task->mm.kernel_pages_cnt++] = task->mm.pgd;
    }
    unsigned long pgd = task->mm.pgd;
    int new_table = 0;
    unsigned long pud = map_table((unsigned long*)(VA1_START + pgd), PGD_SHIFT, va, &new_table);
    if(new_table)
        task->mm.kernel_pages[task->mm.kernel_pages_cnt++] = pud;
    unsigned long pmd = map_table((unsigned long*)(VA1_START + pud), PUD_SHIFT, va, &new_table);
    if(new_table)
        task->mm.kernel_pages[task->mm.kernel_pages_cnt++] = pmd;
    unsigned long pte = map_table((unsigned long*)(VA1_START + pmd), PMD_SHIFT, va, &new_table);
    if(new_table)
        task->mm.kernel_pages[task->mm.kernel_pages_cnt++] = pte;
    
    map_table_entry((unsigned long*)(VA1_START + pte), va, pa, flags);
    struct user_page p = {pa, va, flags};
    task->mm.user_pages[task->mm.user_pages_cnt++] = p;
}

int copy_virt_memory(struct task_struct* task) {
    for(int i = 0; i < current->mm.user_pages_cnt; i++) {
        unsigned long kva = alloc_user_page(task, current->mm.user_pages[i].virt_addr, current->mm.user_pages[i].attr);
        if(!kva) return -1;
        memcpy(kva, current->mm.user_pages[i].virt_addr, PAGE_SIZE);
    }
    return 0;
}

int do_mem_abort(unsigned long addr, unsigned long esr) {
    printf("[Page Fault]: Memory abort, addr: 0x%x, ESR: 0x%x...\n", addr, esr);
    unsigned long ISV_DFSC = (esr & 0b111111);
    if((ISV_DFSC >> 2) == 0b1) { //0001xx
        unsigned long page = alloc_page();
        if(page == 0) return -1;
        map_page(current, addr & VA1_MASK, page, MMU_PTE_RWRW);
        printf("[Page Fault]: Memory abort, map 0x%x to 0x%x, size: 0x1000\n", addr & VA1_MASK, page);
        return 0;
    }
    return -1;
}

int do_inst_abort(unsigned long addr, unsigned long esr) {
    printf("[Page Fault]: Instruction abort, addr: 0x%x, ESR: 0x%x...\n", addr, esr);
    unsigned long ISV_DFSC = (esr & 0b111111);
    if((ISV_DFSC >> 2) == 0b1) { //0001xx
        unsigned long text_addr = alloc_user_page(current, addr, ISV_DFSC == 4 ? MMU_PTE_RORO : MMU_PTE_RON);
        memcpy(text_addr, (unsigned long)&user_begin, ((unsigned long)&user_end) - ((unsigned long)&user_begin));
        printf("[Page Fault]: Instruction abort, map 0x%x to 0x%x, size: 0x1000\n", addr & VA1_MASK, text_addr);
        return 0;
    }
    return -1;
}
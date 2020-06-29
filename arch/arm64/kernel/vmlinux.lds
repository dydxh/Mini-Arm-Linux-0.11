ENTRY(_start)
SECTIONS {
    phys_start = 0x80000;

    . = 0xffff000000000000;
    virt_start = .;

    __text_start = .;
    .text.boot : {KEEP(*(.text.boot))}
    . = ALIGN(0x1000);

    user_begin = .;
    .text.user : {*user* (.text)}
    .rodata.user : {*user* (.rodata)}
    .data.user : {*user* (.data)}
    .bss.user : {*user* (.bss)}
    user_end = .;

    . = ALIGN(0x1000);
    .text : {*(.text)}

    . = ALIGN(0x1000);
    __text_end = .;
    __rodata_start = .;
    .rodata : {*(.rodata)}

    . = ALIGN(0x1000);
    __rodata_end = .;
    __data_start = .;
    .data : {*(.data)}

    . = ALIGN(0x1000);
    __data_end = .;
    __bss_start = .;
    .bss : {*(.bss) *(.bss.*)}

    . = ALIGN(0x1000);
    __bss_end = .;
    __pgd_start = .;
    ttbr0_pos = .;
    .data.pgd : {. += (3 * (1 << 12));}

    . = ALIGN(0x1000);
    ttbr1_pos = .;
    .data.pgd : {. += (10 * (1 << 12));}

    . = ALIGN(0x1000);
    __pgd_end = .;
    __stack_start = .;
    . = ALIGN(0x200000);
    __stack_end = . - phys_start;

    /DISCARD/ : {
       *(.comment)
       *(.gnu*)
       *(.note*)
       *(.eh_frame*)
    }
}
__text_size   = __text_end   - __text_start;
__rodata_size = __rodata_end - __rodata_start;
__data_size   = __data_end   - __data_start;
__bss_size    = __bss_end    - __bss_start;
__pgd_size    = __pgd_end    - __pgd_start;
__stack_size  = __stack_end  - __stack_start;

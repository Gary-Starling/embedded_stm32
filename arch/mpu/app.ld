
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08004000, LENGTH = 1024K - 0x4000
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
    CCM (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

SECTIONS
{
    .text :
    {
        stext = .;
        KEEP(*(.isr_v))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        etext = .;
    } > FLASH

    lma_data = .;

    .data : AT (lma_data)
    {
        sdata = .;
        *(.data*)
        . = ALIGN(4);
        edata = .;
    } > RAM

    .bss :
    {
        sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        ebss = .;
        _end = .;
    } > RAM

}

_start_heap = _end;
end_stack  = ORIGIN(RAM) + LENGTH(RAM);

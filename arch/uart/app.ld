
MEMORY
{
FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 1024K
RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
CCMRAM (rw)      : ORIGIN = 0x10000000, LENGTH = 64K
}

SECTIONS
{
      /* The startup code goes first into FLASH */
    .isr_v :
    {
      . = ALIGN(4);
      KEEP(*(.isr_v)) /* Startup code */
      . = ALIGN(4);
    } >FLASH

    .text :
    {
        stext = .;
      *(.text)           /* .text sections (code) */
      *(.text*)          /* .text* sections (code) */
      *(.glue_7)         /* glue arm to thumb code */
      *(.glue_7t)        /* glue thumb to arm code */
      *(.eh_frame)
  
      KEEP (*(.init))
      KEEP (*(.fini))
  
      . = ALIGN(4);
        etext = .;
    } > FLASH

    /* Constant data goes into FLASH */
    .rodata :
    {
      . = ALIGN(4);
      *(.rodata)         /* .rodata sections (constants, strings, etc.) */
      *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
      . = ALIGN(4);
    } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
     . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

    .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH

    . = ALIGN(4);
    .bss :
    {
        sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        ebss = .;
        _end = .;
    } > RAM

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
      libc.a ( * )
      libm.a ( * )
      libgcc.a ( * )
    }


}

_start_heap = _end;
end_stack  = ORIGIN(RAM) + LENGTH(RAM);

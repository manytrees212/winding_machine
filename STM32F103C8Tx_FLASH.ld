/* Entry Point */
ENTRY(Reset_Handler)

/* Memory definitions */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 62K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
    EEPROM (r) : ORIGIN = 0x0800F800, LENGTH = 2K
}

/* Stack and heap sizes */
_estack = ORIGIN(RAM) + LENGTH(RAM);
_Min_Heap_Size = 0x0;
_Min_Stack_Size = 0x400;

SECTIONS
{
    /* Vector table */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Code and constants */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    /* Initialized data */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    /* Uninitialized data */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    /* 2K the end of eeprom for flash */
    .eeprom_reserved (NOLOAD):
  {
    . = ALIGN(4);
    _eeprom_start = .;
    . = . + 2K;
  } > FLASH

    /* Heap and stack */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } > RAM
}

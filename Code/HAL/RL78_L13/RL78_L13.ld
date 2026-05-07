/* RL78_L13.ld
 * GCC linker script for Renesas RL78/L13 — R5F10WMG
 *
 * Memory map (verify against R5F10WMG hardware manual before use):
 *   Code flash  : 0x00000 – 0x1FFFF  (128 KB)
 *   Data flash  : 0xF1000 – 0xF1FFF  (  4 KB)
 *   RAM         : 0xFDF00 – 0xFFEFF  (  8 KB)
 *   SFR area    : 0xFFF00 – 0xFFFFF  (reserved — do not place code or data here)
 *
 * Section layout:
 *   .text   — code and read-only data  → code flash
 *   .data   — initialised variables    → code flash (LMA), copied to RAM at startup
 *   .bss    — zero-initialised data    → RAM (zeroed by startup code)
 *   .stack  — stack (grows downward)   → top of RAM
 */

MEMORY
{
    ROM       (rx)  : ORIGIN = 0x00000, LENGTH = 128K
    DATAFLASH (r)   : ORIGIN = 0xF1000, LENGTH = 4K
    RAM       (rwx) : ORIGIN = 0xFDF00, LENGTH = 8K
}

/* Stack size — adjust if needed */
STACK_SIZE = 0x200;   /* 512 bytes */

SECTIONS
{
    /* ------------------------------------------------------------------ */
    /* Interrupt vector table — must be at the very start of flash         */
    /* ------------------------------------------------------------------ */
    .vectors :
    {
        KEEP(*(.vectors))
    } > ROM

    /* ------------------------------------------------------------------ */
    /* Program code and read-only data                                     */
    /* ------------------------------------------------------------------ */
    .text :
    {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
    } > ROM

    /* ------------------------------------------------------------------ */
    /* C++ static constructors / destructors (keep even if unused)         */
    /* ------------------------------------------------------------------ */
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > ROM

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > ROM

    /* ------------------------------------------------------------------ */
    /* Initialised data — stored in flash, copied to RAM at startup        */
    /* ------------------------------------------------------------------ */
    .data :
    {
        __data_start = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        __data_end = .;
    } > RAM AT > ROM

    __data_load_start = LOADADDR(.data);   /* source address in flash */

    /* ------------------------------------------------------------------ */
    /* Zero-initialised data — zeroed by startup code                      */
    /* ------------------------------------------------------------------ */
    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } > RAM

    /* ------------------------------------------------------------------ */
    /* Stack — placed at the top of RAM, grows downward                    */
    /* ------------------------------------------------------------------ */
    .stack (NOLOAD) :
    {
        . = ALIGN(4);
        __stack_start = .;
        . += STACK_SIZE;
        __stack_top = .;
    } > RAM

    /* ------------------------------------------------------------------ */
    /* Data flash section (NVM parameter storage)                          */
    /* ------------------------------------------------------------------ */
    .dataflash (NOLOAD) :
    {
        *(.dataflash)
        *(.dataflash.*)
    } > DATAFLASH

    /* ------------------------------------------------------------------ */
    /* Discard unwanted sections                                           */
    /* ------------------------------------------------------------------ */
    /DISCARD/ :
    {
        *(.comment)
        *(.note.*)
    }
}

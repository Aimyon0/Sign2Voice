/*
 * STM32H743VITx GCC Linker Script
 * Memory layout matching finaltest.sct scatter file
 * DTCM 128K + AXI SRAM 512K + SRAM1+2 256K
 */

_estack = 0x20020000; /* DTCM end */
__bss_start__ = _sbss;
__bss_end__   = _ebss;

MEMORY
{
  FLASH    (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
  DTCM     (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
  AXI_SRAM (rwx) : ORIGIN = 0x24000000, LENGTH = 512K
  SRAM1_2  (rwx) : ORIGIN = 0x30000000, LENGTH = 256K
  SRAM4    (rwx) : ORIGIN = 0x38000000, LENGTH = 64K
}

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } >FLASH

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
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } >FLASH

  /* DTCM: stack + AI activations + hot data */
  .dtcm (NOLOAD) :
  {
    . = ALIGN(4);
    *(.AI_DTCMRAM)
    *(.dtcm)
    *(.dtcm*)
  } >DTCM

  /* AXI SRAM: RGB_DATA, AI buffers, heap */
  .axi_sram (NOLOAD) :
  {
    . = ALIGN(4);
    *(.AXI_RAM)
    *(AI_RAM)
    . = ALIGN(4);
  } >AXI_SRAM

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >AXI_SRAM AT>FLASH
  _sidata = LOADADDR(.data);

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >AXI_SRAM

  /* SRAM1+2: DMA buffers, LVGL buffers */
  .sram1_2 (NOLOAD) :
  {
    . = ALIGN(4);
    *(.D2_RAM)
    *(.RAM_D2)
    . = ALIGN(4);
  } >SRAM1_2

  /* Remove unused sections */
  /DISCARD/ :
  {
    libc.a(*)
    libm.a(*)
    libgcc.a(*)
  }
}

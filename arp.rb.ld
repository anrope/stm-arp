/*stm32f105 dev board linker script*/

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(_start)
SEARCH_DIR("/usr/students/apellett/stm/ctc/arm-eabi/lib")

/* stm32f105rb */
MEMORY
{
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 32k
  flash (rx) : ORIGIN = 0x08000000, LENGTH = 128k
  option_bytes_rom (rx) : ORIGIN = 0x1FFFF800, LENGTH = 16
}

_sheap = _ebss + 4;
_sstack = _ebss + 4;
/*placed __stack_base__ trying to figure out
global variable overwrite issue
__stack_base__ = _ebss + 4;*/

_eheap = ORIGIN(ram) + LENGTH(ram) - 1;
_estack = ORIGIN(ram) + LENGTH(ram) - 1;

SECTIONS
{
	. = SEGMENT_START("text-segment", 0);
	


	/*isr_vector contains the interrupt vector.
	
	isr_vector is read only (could be write too?).
	
	isr_vector must appear at start of flash (USR),
	address 0x0800 0000*/
	.isr_vector :
	{
	  . = ALIGN(4);
	  _sisr_vector = .;
	  
	  *(.isr_vector)
	  
	  _eisr_vector = .;
	} >flash
	
	/*text contains executable code.
	
	text is read and execute.*/
	.text :
	{
	  . = ALIGN(4);
	  *(.text)
	  . = ALIGN(4);
	  *(.text.*)
	} >flash
	
	/*init contains constructor functions
	called before entering main. used by crt (?).*/
	.init :
	{
	  . = ALIGN(4);
	  KEEP(*(.init))
	} >flash
	
	/*fini contains destructor functions
	called after leaving main. used by crt (?).*/
	.fini :
	{
	  . = ALIGN(4);
	  KEEP(*(.fini))
	} >flash
	
	/* rodata contains read only data.*/
	.rodata :
	{
	  . = ALIGN(4);
	  *(.rodata)
	  
	  /* sidata contains the initial values
	  for variables in the data section.
	
	  sidata is read only.*/
	  . = ALIGN(4);
	  _sidata = .;
	} >flash
	
	/*data contains all initalized variables.
	
	data is read and write. 
 	.data (NOLOAD) : AT(_sidata)*/
	.data : AT(_sidata)
	{
	  . = ALIGN(4);
	  _sdata = .;
	  
	  *(.data)
	  
	  _edata = .;
	} >ram

	/*bss contains unintialized variables.
	
	bss is read and write.
 	.bss (NOLOAD) :*/
	.bss :
	{
	  . = ALIGN(4);
	  _sbss = .;
	  __bss_start__ = .;
	  
	  *(.bss)
	  . = ALIGN(4);
	  
	  /*COMMON is a special section containing
	  uninitialized data.
	  
	  Example: (declared globally)
	  int temp; //this will appear in COMMON */
	  *(COMMON)
	  
	  _ebss = .;
	  __bss_end__ = .;
	} >ram AT>flash
	
	. = ALIGN(4);
	end = .;
	
	    /* remove the debugging information from the standard libraries */
    DISCARD :
    {
     libc.a ( * )
     libm.a ( * )
     libgcc.a ( * )
     }

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
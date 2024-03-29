MEMORY {
	rom (rx) : ORIGIN = _ROM_OFF, LENGTH = _ROM
	ram (rwx) : ORIGIN = _RAM_OFF, LENGTH = _RAM
#ifdef _CCM
	ccm (rwx) : ORIGIN = _CCM_OFF, LENGTH = _CCM
#endif
}

EXTERN(vectorTable)

ENTRY(RESET_vect)

SECTIONS {
	.text : {
		*(.vectors)
		*(.text*)
		. = ALIGN(4);
		*(.rodata*)
		. = ALIGN(4);
	} >rom
	.preinit_array : {
		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;
	} >rom
	.init_array : {
		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;
	} >rom
	.fini_array : {
		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;
	} >rom
	
	. = ALIGN(4);
	_etext = .;

	.data : {
		_data = .;
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} >ram AT >rom
	_data_loadaddr = LOADADDR(.data);

	.bss : {
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} >ram

#ifdef CCM_SIZE
	.ccm : {
		*(.ccmram)
		. = ALIGN(4);
	} >ccm
#endif

	. = ALIGN(4);
	end = .;
}

PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));

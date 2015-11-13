#define CORTEXM_PRIVATE
#include <vector.h>
#include <platform.h>
#include <cortexm.h>

typedef void (*funcptr_t)(void);

extern unsigned _data_loadaddr, _data, _edata, _bss, _ebss, _stack;

extern funcptr_t __preinit_array_start, __preinit_array_end;
extern funcptr_t __init_array_start, __init_array_end;
extern funcptr_t __fini_array_start, __fini_array_end;

int main(void);
void platform_init(void);

extern "C" void __attribute__((naked)) RESET_vect(void) {
	unsigned *dst = &_data;
	unsigned *src = &_data_loadaddr;
	while (dst < &_edata) {
		*(dst++) = *(src++);
	}
	while (dst < &_ebss) {
		*(dst++) = 0;
	}
	for (funcptr_t *func = &__preinit_array_start; func < &__preinit_array_end; func++) {
		(*func)();
	}
	for (funcptr_t *func = &__init_array_start; func < &__init_array_end; func++) {
		(*func)();
	}
	platform_init();
	main();
	for (funcptr_t *func = &__fini_array_start; func < &__fini_array_end; func++) {
		(*func)();
	}
	SCB_ResetSystem();
}

ISR(empty_handler) {
	
}

ISR(blocking_handler) {
	while (1);
}

#pragma weak NMI_vect = empty_handler
ISR(NMI_vect);

#pragma weak HARD_FAULT_vect = blocking_handler
ISR(HARD_FAULT_vect);

#pragma weak MEM_MANAGE_FAULT_vect = blocking_handler
ISR(MEM_MANAGE_FAULT_vect);

#pragma weak BUS_FAULT_vect = blocking_handler
ISR(BUS_FAULT_vect);

#pragma weak USAGE_FAULT_vect = blocking_handler
ISR(USAGE_FAULT_vect);

#pragma weak SV_CALL_vect = empty_handler
ISR(SV_CALL_vect);

#pragma weak DEBUG_MONITOR_vect = empty_handler
ISR(DEBUG_MONITOR_vect);

#pragma weak PEND_SV_vect = empty_handler
ISR(PEND_SV_vect);

__attribute__ ((section(".vectors"))) funcptr_t vector_table[] = {
	(funcptr_t)&_stack,
	RESET_vect,
	NMI_vect, // NMI
	HARD_FAULT_vect, // Hard Fault
	MEM_MANAGE_FAULT_vect, // Memory Manager Fault
	BUS_FAULT_vect, // Bus Fault
	USAGE_FAULT_vect, // Usage Fault
	0, 0, 0, 0,
	SV_CALL_vect, // SV Call
	DEBUG_MONITOR_vect, // Debug Monitor
	0,
	PEND_SV_vect, // Pend SV
	SysTick::interruptHandler, // SysTick
	/* Vendor-specific IRQs */
	PERIPHERAL_IRQS
};

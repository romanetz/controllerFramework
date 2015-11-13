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

extern "C" void __attribute__((naked)) reset_handler(void) {
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

#pragma weak nmi_handler = empty_handler
ISR(nmi_handler);

#pragma weak hard_fault_handler = blocking_handler
ISR(hard_fault_handler);

#pragma weak memory_manager_fault_handler = blocking_handler
ISR(memory_manager_fault_handler);

#pragma weak bus_fault_handler = blocking_handler
ISR(bus_fault_handler);

#pragma weak usage_fault_handler = blocking_handler
ISR(usage_fault_handler);

#pragma weak sv_call_handler = empty_handler
ISR(sv_call_handler);

#pragma weak debug_monitor_handler = empty_handler
ISR(debug_monitor_handler);

#pragma weak pend_sv_handler = empty_handler
ISR(pend_sv_handler);

__attribute__ ((section(".vectors"))) funcptr_t vector_table[] = {
	(funcptr_t)&_stack,
	reset_handler,
	nmi_handler, // NMI
	hard_fault_handler, // Hard Fault
	memory_manager_fault_handler, // Memory Manager Fault
	bus_fault_handler, // Bus Fault
	usage_fault_handler, // Usage Fault
	0, 0, 0, 0,
	sv_call_handler, // SV Call
	debug_monitor_handler, // Debug Monitor
	0,
	pend_sv_handler, // Pend SV
	SysTick::interruptHandler, // SysTick
	/* Vendor-specific IRQs */
	PERIPHERAL_IRQS
};

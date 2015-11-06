#include <cortexm.h>

void SCB_ResetSystem(void) {
	SCB->AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	while (1);
}

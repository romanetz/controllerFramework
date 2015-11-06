/**
	\addtogroup Cortex-Mx Cortex-Mx
	@{
*/
#ifndef __CORTEXM_H__
#define __CORTEXM_H__

#include <stdint.h>

#define MMIO(addr, type) ((volatile type*)addr)

/* Private peripheral bus - Internal */
#define PPBI_BASE (0xE0000000)

/* SCS: System Control Space */
#define SCS_BASE (PPBI_BASE + 0xE000)

#include <vector.h>
#include <scb.h>
#include <systick.h>
#include <cortexm_misc.h>

#endif
/**
	@}
*/

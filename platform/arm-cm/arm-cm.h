#ifndef __ARM_CM_H__
#define __ARM_CM_H__

#include <stdint.h>

#define MMIO(addr, type) ((volatile type*)(addr))

/* Private peripheral bus - Internal */
#define PPBI_BASE (0xE0000000)

/* SCS: System Control Space */
#define SCS_BASE (PPBI_BASE + 0xE000)

#include <platform.h>
#include <arm_mutex.h>
#include <arm_vectors.h>
#include <arm_scb.h>
#include <arm_nvic.h>
#include <arm_systick.h>
#include <arm_thread.h>
#include <arm_event.h>

#endif

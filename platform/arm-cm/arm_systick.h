#ifndef __ARM_SYSTICK_H__ 
#define __ARM_SYSTICK_H__

#include <arm-cm.h>

typedef struct SysTickRegs {
	uint32_t CTRL;
	uint32_t LOAD;
	uint32_t VAL;
	uint32_t CALIB;
} SysTickRegs;

#define SYSTICK_BASE (SCS_BASE + 0x0010)
#define SYSTICK MMIO(SYSTICK_BASE, SysTickRegs)

#define SYSTICK_CTRL_COUNTFLAG (1 << 16)

#define SYSTICK_CTRL_CLKSOURCE_OFFSET 2
#define SYSTICK_CTRL_CLKSOURCE (1 << SYSTICK_CTRL_CLKSOURCE_OFFSET)

#if defined(__ARM_ARCH_6M__)
#define SYSTICK_CTRL_CLKSOURCE_EXT (0 << SYSTICK_CTRL_CLKSOURCE_OFFSET)
#define SYSTICK_CTRL_CLKSOURCE_AHB (1 << SYSTICK_CTRL_CLKSOURCE_OFFSET)
#else
#define SYSTICK_CTRL_CLKSOURCE_AHB_DIV8 (0 << SYSTICK_CTRL_CLKSOURCE_OFFSET)
#define SYSTICK_CTRL_CLKSOURCE_AHB (1 << SYSTICK_CTRL_CLKSOURCE_OFFSET)
#endif

#define SYSTICK_CTRL_TICKINT (1 << 1)
#define SYSTICK_CTRL_ENABLE (1 << 0)

#define SYSTICK_LOAD_MASK 0x00FFFFFF

void sysTickSetClockSource(uint32_t src);
void sysTickSetReloadValue(uint32_t value);
uint32_t sysTickGetReloadValue(void);
void sysTickEnableCounter(void);
void sysTickDisableCounter(void);
void sysTickEnableInterrupt(void);
void sysTickDisableInterrupt(void);

int sysTickSetup(uint32_t freq);
uint32_t sysTickGetFrequency(void);
timestamp_t sysTickTimestamp(void);
void sysTickDelay(timestamp_t time);

void usleep(timestamp_t time);

#endif
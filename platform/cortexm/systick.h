/**
	\addtogroup Cortex-Mx Cortex-Mx
	@{
	\addtogroup SysemTick SysTick
	@{
*/
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <cortexm.h>

/**
	\brief SysTick timer MMIO registers.
*/
struct SysTickRegs {
	uint32_t CTRL; ///< Control register
	uint32_t LOAD; ///< Reload value register
	uint32_t VAL; ///< Current counter value register
	uint32_t CALIB; ///< Calibration register
};

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

/**
	\brief SysTick timer
*/
class SysTick {
	private:
		static uint32_t frequency;
		
		static volatile uint64_t counter;
		
		static void setClockSource(uint32_t clksrc) {
			SYSTICK->CTRL = (SYSTICK->CTRL & ~SYSTICK_CTRL_CLKSOURCE) | clksrc;
		}
		
		static void setReloadValue(uint32_t value) {
			SYSTICK->LOAD = value & SYSTICK_LOAD_MASK;
		}
	
	public:
		/**
			\brief Initializes SysTick with given frequency.
			\param[in] freq Target SysTick frequency in Hz.
			\param[in] sysFreq MCU core frequency in Hz.
		*/
		SysTick(uint32_t freq, uint32_t sysFreq);
		
		/**
			\brief Changes SysTick frequency.
			\param[in] freq Target SysTick frequency in Hz.
			\param[in] sysFreq MCU core frequency in Hz.
		*/
		static bool setFrequency(uint32_t freq, uint32_t sysFreq);
		
		/**
			\brief Get current SysTick frequency in Hz.
		*/
		static uint32_t getFrequency()  {
			return frequency;
		}
		
		/**
			\brief Starts SysTick timer counter.
		*/
		static void enableCounter() {
			SYSTICK->CTRL |= SYSTICK_CTRL_ENABLE;
		}
		
		/**
			\brief Stops SysTick timer counter.
		*/
		static void disableCounter() {
			SYSTICK->CTRL &= ~SYSTICK_CTRL_ENABLE;
		}
		
		/**
			\brief Enable interrupt from SysTick.
		*/
		static void enableInterrupt() {
			SYSTICK->CTRL |= SYSTICK_CTRL_TICKINT;
		}
		
		/**
			\brief Disable interrupt from SysTick.
		*/
		static void disableInterrupt() {
			SYSTICK->CTRL &= ~SYSTICK_CTRL_TICKINT;
		}
		
		/**
			\brief SysTick interrupt handler.
		*/
		static void interruptHandler();
		
		/**
			\brief Get current time in microseconds.
		*/
		static uint64_t timestamp();
		
		/**
			\brief Busy loop delay.
			\param[in] microseconds Pause length in microseconds.
		*/
		static void sleep(uint64_t microseconds);
		
};

/**
	\brief Busy loop delay.
	\param[in] microseconds Pause length in microseconds.
*/
inline void usleep(uint64_t microseconds) {
	SysTick::sleep(microseconds);
}

#endif
/** @}
@} */

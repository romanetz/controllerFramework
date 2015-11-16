/**
	\addtogroup STM32 STM32
	@{
	\addtogroup STM32_RCC RCC
	@{
*/
#ifndef __STM32_RCC_H__
#define __STM32_RCC_H__

#include <stm32.h>

/**
	\brief STM32 RCC MMIO registers.
*/
struct STM32RCCRegs {
#if defined(STM32F1) || defined(STM32F2)
	uint32_t CR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t APB2RSTR;
	uint32_t APB1RSTR;
	uint32_t AHBENR;
	uint32_t APB2ENR;
	uint32_t APB1ENR;
	uint32_t BDCR;
	uint32_t CSR;
	uint32_t AHBRSTR;
	uint32_t CFGR2;
#ifdef STM32F2
	uint32_t CFGR3;
#endif
#else
	uint32_t CR;
	uint32_t PLLCFGR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t AHB1RSTR;
	uint32_t AHB2RSTR;
	uint32_t AHB3RSTR;
	uint32_t RESERVED1;
	uint32_t APB1RSTR;
	uint32_t APB2RSTR;
	uint32_t RESERVED2;
	uint32_t RESERVED3;
	uint32_t AHB1ENR;
	uint32_t AHB2ENR;
	uint32_t AHB3ENR;
	uint32_t RESERVED4;
	uint32_t APB1ENR;
	uint32_t APB2ENR;
	uint32_t RESERVED5;
	uint32_t RESERVED6;
	uint32_t AHB1LPENR;
	uint32_t AHB2LPENR;
	uint32_t AHB3LPENR;
	uint32_t RESERVED7;
	uint32_t APB1LPENR;
	uint32_t APB2LPENR;
	uint32_t RESERVED8;
	uint32_t RESERVED9;
	uint32_t BDCR;
	uint32_t CSR;
	uint32_t RESERVED10;
	uint32_t RESERVED11;
	uint32_t SSCGR;
	uint32_t PLLI2SCFGR;
#ifndef STM32F4
	uint32_t PLLSAICFGR;
	uint32_t DCKCFGR;
#endif
#endif
};

enum STM32RCCAPBPrescaler {
	RCC_APB_NODIV = 0,
	RCC_APB_DIV2 = 4,
	RCC_APB_DIV4 = 5,
	RCC_APB_DIV8 = 6,
	RCC_APB_DIV16 = 7
};

enum STM32RCCAHBPrescaler {
	RCC_AHB_NODIV = 0,
	RCC_AHB_DIV2 = 8,
	RCC_AHB_DIV4 = 9,
	RCC_AHB_DIV8 = 10,
	RCC_AHB_DIV16 = 11,
	RCC_AHB_DIV64 = 12,
	RCC_AHB_DIV128 = 13,
	RCC_AHB_DIV256 = 14,
	RCC_AHB_DIV512 = 15
};

/**
	\brief STM32 RCC MMIO registers.
*/
#define RCC MMIO(RCC_BASE, STM32RCCRegs)

/**
	\brief STM32 clock source.
*/
enum STM32ClockSource {
	STM32_CLOCKSOURCE_HSI, ///< High speed internal oscillator (8 MHz).
	STM32_CLOCKSOURCE_LSI, ///< Low speed internal oscillator (40 KHz).
	STM32_CLOCKSOURCE_HSE, ///< High speed external oscillator (4 MHz - 16 MHz).
	STM32_CLOCKSOURCE_LSE, ///< Low speed external oscillator (32.768 KHz).
	STM32_CLOCKSOURCE_PLL ///< PLL.
};

/**
	\brief
	Reset and Clock Control.
*/
class STM32RCC {
	private:
		static uint32_t _sysFreq;
		
		static uint32_t _ahbFreq;
		
		static uint32_t _apb1Freq;
		
		static uint32_t _apb2Freq;
	
	public:
		/**
			\brief Initializes RCC with specified options.
			\param[in] src - Desired clock source (only HSI, HSE and PLL allowed).
			\param[in] inClk - Input clock in MHz (need for HSE and PLL).
			\param[in] pllSrc - Desired PLL clock source (only HSI and HSE allowed) if src == PLL.
			\param[in] outClk - Desired PLL output frequency if src == PLL.
		*/
		STM32RCC(STM32ClockSource src = STM32_CLOCKSOURCE_HSI, uint32_t inClk = 0,
				STM32ClockSource pllSrc = STM32_CLOCKSOURCE_HSE, uint32_t outClk = 0);
		
		/**
			\brief Get system frequency in Hz.
		*/
		static uint32_t systemFrequency() {
			return _sysFreq;
		}
		
		/**
			\brief Get AHB frequency in Hz.
		*/
		static uint32_t ahbFrequency() {
			return _ahbFreq;
		}
		
		/**
			\brief Get APB1 frequency in Hz.
		*/
		static uint32_t apb1Frequency() {
			return _apb1Freq;
		}
		
		/**
			\brief Get APB2 frequency in Hz.
		*/
		static uint32_t apb2Frequency() {
			return _apb2Freq;
		}
		
		/**
			\brief Enable one of clock sources.
			Do nothing if clock source already enabled.
		*/
		static void enableClockSource(STM32ClockSource src);
		
		/**
			\brief Disable one of clock sources.
			\param[in] src Clock source.
			Do nothing if clock source already disabled.
			You should not call this function with current clock source.
		*/
		static void disableClockSource(STM32ClockSource src);
		
		/**
			\brief Wait while clock source become stable.
			\param[in] src Clock source.
			\param[in] timeout Timeout in cycles.
			\return Returns true if clock source become stable or false if timeout reached.
		*/
		static bool waitClockSourceReady(STM32ClockSource src, uint32_t timeout = 100000);
		
		/**
			\brief Select current system clock source.
			\param[in] src Desired system clock source (only HSI, HSE and PLL allowed).
			You must enable clock source first.
		*/
		static void selectClockSource(STM32ClockSource src);
		
		/**
			\brief Wait until desired clock source selected as system main clock.
			\param[in] src Desired system clock source.
		*/
		static void waitClockSourceSelected(STM32ClockSource src);
	
#ifdef STM32F1
		/**
			\brief Setup PLL to make outClk frequency from inClk.
			\param[in] src Clock source for PLL (only HSI and HSE allowed).
			\param[in] inClk PLL input clock (8 MHz for HSI, crystal frequency for HSE).
			\param[in] outClk PLL output clock.
			\return Return true if PLL setup correct or false if it is not possible to make outClk from inClk.
		*/
		static bool setupPLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk);
		
		static void setADCPrescaler(uint32_t value) {
			RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE_MASK) | (value << RCC_CFGR_ADCPRE_OFFSET);
		}
#else
		/**
			\brief Low-level PLL setup.
			\param[in] src Clock source for PLL (only HSI and HSE allowed).
			\param[in] pllm PLL input prescaler (1 <= pllm <= 31).
			\param[in] plln PLL multiply factor (1 <= plln <= 511).
			\param[in] pllp System clock prescaler (2, 4, 6 or 8).
			\param[in] pllq USB and some other peripheral prescaler (1 < pllq < 31, must output 48 MHz frequency for correct operation of this modules).
			\return Returns true if all settings are within valid range. Otherwise returns false.
		*/		
		static bool setupPLL(STM32ClockSource src, uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq);
		
		/**
			\brief Setup PLL to make outClk frequency from inClk.
			\param[in] src Clock source for PLL (only HSI and HSE allowed).
			\param[in] inClk PLL input clock (8 MHz for HSI, crystal frequency for HSE).
			\param[in] outClk PLL output clock.
			\return Return true if PLL setup correct or false if it is not possible to make outClk from inClk.
		*/
		static bool setupPLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk);
		
		/**
			\brief Set prescaler for RTC.
			\param[in] value Prescaler value (1 <= value <= 31).
		*/
		static void setRTCPrescaler(uint32_t value) {
			RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_RTCPRE_MASK) | (value << RCC_CFGR_RTCPRE_OFFSET);
		}
		
#endif
		/**
			\brief Set prescaler for APB1.
			\param[in] value Prescaler value.
		*/
		static void setAPB1Prescaler(STM32RCCAPBPrescaler value) {
			RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1_MASK) | (value << RCC_CFGR_PPRE1_OFFSET);
		}
		
		/**
			\brief Set prescaler for APB2.
			\param[in] value Prescaler value.
		*/
		static void setAPB2Prescaler(STM32RCCAPBPrescaler value) {
			RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2_MASK) | (value << RCC_CFGR_PPRE2_OFFSET);
		}
		
		/**
			\brief Set prescaler for AHB.
			\param[in] value Prescaler value.
		*/
		static void setAHBPrescaler(STM32RCCAHBPrescaler value) {
			RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE_MASK) | (value << RCC_CFGR_HPRE_OFFSET);
		}
		
		/**
			\brief Enable internal high speed oscillator and select it as a system clock source.
			\return Always returns true.
		*/
		static bool useHSI();
		
		/**
			\brief Enable external high speed oscillator and select it as a system clock source.
			\return Returns true if HSE succesfully activated. Otherwise returns false.
		*/
		static bool useHSE(uint32_t clk);
		
		/**
			\brief Setup and enable PLL, then select it as a system clock source.
			\return Returns true if PLL succesfully activated. Otherwise returns false.
		*/
		static bool usePLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk);
		
		/**
			\brief Enable periphenal clocking by it MMIO base address.
			\param[in] periph Peripheral MMIO base address.
		*/
		static void periphClockEnable(volatile void *periph);
		
		/**
			\brief Disable periphenal clocking by it MMIO base address.
			\param[in] periph Periphenal MMIO base address.
		*/
		static void periphClockDisable(volatile void *periph);
			
};

#endif
/**
	@}
	@}
*/

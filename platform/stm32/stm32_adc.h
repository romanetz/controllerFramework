#ifndef __STM32_ADC_H__
#define __STM32_ADC_H__

#include <stm32.h>
#include <adc.h>

struct STM32ADCRegs {
	uint32_t SR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMPR[2];
	uint32_t JOFR[4];
	uint32_t HTR;
	uint32_t LTR;
	uint32_t SQR[3];
	uint32_t JSQR;
	uint32_t JDR[4];
	uint32_t DR;
};

#define ADC1 MMIO(ADC1_BASE, STM32ADCRegs)

#ifdef ADC2_BASE
#define ADC2 MMIO(ADC2_BASE, STM32ADCRegs)
#endif

#ifdef ADC3_BASE
#define ADC3 MMIO(ADC3_BASE, STM32ADCRegs)
#endif

#ifdef STM32F4
#define STM32_ADC_CHANNEL_COUNT 19
#else
#define STM32_ADC_CHANNEL_COUNT 18
#endif

class STM32ADC;

class STM32ADCChannel: public ADCChannel {
	private:
		STM32ADC& _adc;
		
		int _index;
		
		ADCChannelCallback _callback;
		
		void *_callbackArg;
		
		uint16_t _value;
		
		volatile bool _ready;
		
	public:
		STM32ADCChannel(STM32ADC& adc, int index);
		
		int resolution() { return 12; };
		
		int value();
		
		void setCallback(ADCChannelCallback callback, void *arg);
		
		void convert(bool wait);
		
		void conversionComplete(uint16_t value);
		
		int index() { return _index; }
		
};

class STM32ADC: public ADCGroup {
	private:
		volatile STM32ADCRegs *_adc;
		
		STM32ADCChannel *_channels[STM32_ADC_CHANNEL_COUNT];
		
		STM32ADCChannel *_currentChannel;
		
	public:
		STM32ADC(volatile STM32ADCRegs *adc);
		
		~STM32ADC();
		
		void enableTempSensor();
		
		void disableTempSensor();
		
		int channelCount() { return STM32_ADC_CHANNEL_COUNT; };
		
		ADCChannel *channel(int index);
		
		volatile STM32ADCRegs *mmioRegs() { return _adc; }
		
		void startSingleConversion(STM32ADCChannel& channel);
		
		void interruptHandler();
		
};

#endif

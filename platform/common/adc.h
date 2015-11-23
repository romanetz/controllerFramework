#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

class ADCChannel;

typedef void (*ADCChannelCallback)(ADCChannel& adcChannel, void *arg);

class ADCGroup {
	public:
		virtual ~ADCGroup() {};
		
		virtual int channelCount() = 0;
		
		virtual ADCChannel *channel(int index) = 0;
		
};

class ADCChannel {
	public:
		virtual ~ADCChannel() {};
		
		virtual int resolution() = 0;
		
		virtual int value() = 0;
		
		virtual void setCallback(ADCChannelCallback callback, void *arg) = 0;
		
		virtual void convert(bool wait);
		
		uint32_t convertAndRead();
		
};

#endif

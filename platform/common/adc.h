#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>
#include <queue.h>

class ADCChannel;

typedef uint32_t ADCValue;

typedef void (*ADCChannelCallback)(ADCChannel& adcChannel, void *arg);

class ADCGroup : Queue {
	protected:
		void setElementSize(int size);
		
		bool put(const void *element, int timeout = -1);
		
	public:
		ADCGroup();
		
		virtual ~ADCGroup() {};
		
		virtual int channelCount() = 0;
		
		virtual ADCChannel *channel(int index) = 0;
		
};

class ADCChannel {
	public:
		virtual ~ADCChannel() {};
		
		virtual int resolution() = 0;
		
		virtual ADCValue value() = 0;
		
		virtual void setCallback(ADCChannelCallback callback, void *arg) = 0;
		
		virtual void convert(bool wait);
		
		uint32_t convertAndRead();
		
};

#endif

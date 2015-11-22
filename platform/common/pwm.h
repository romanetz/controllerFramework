#ifndef __PWM_H__
#define __PWM_H__

class PWMChannel {
	public:
		virtual ~PWMChannel() {};
		
		virtual void enable() = 0;
		
		virtual void disable() = 0;
		
		virtual bool enabled() = 0;
		
		virtual int value() = 0;
		
		virtual void setValue(int value) = 0;
		
		virtual int minValue() = 0;
		
		virtual int maxValue() = 0; 
		
};

class PWMGroup {
	public:
		virtual ~PWMGroup() {};
		
		virtual int channelCount() = 0;
		
		virtual PWMChannel *channel(int index) = 0;
		
};

#endif

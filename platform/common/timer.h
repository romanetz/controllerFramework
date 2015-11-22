#ifndef __TIMER_H__
#define __TIMER_H__

class Timer;

typedef bool (*TimerCallback)(Timer& timer, void *arg);

class Timer {
	public:
		virtual int frequency() = 0;
		
		virtual void setFrequency(int freq) = 0;
		
		virtual void setCallback(TimerCallback callback, void *arg) = 0;
		
		virtual void enable() = 0;
		
		virtual void disable() = 0;
		
		virtual bool enabled() = 0;
		
};

#endif

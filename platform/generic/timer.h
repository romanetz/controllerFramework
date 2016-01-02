#ifndef __TIMER_H__
#define __TIMER_H__

#include <generic.h>

#ifdef USE_CLASSES

typedef struct TimerClass TimerClass;

typedef void (*TimerCallback)(TimerClass *timer, void *arg);

typedef void (*TimerClassSetFrequencyFunc)(TimerClass *timer, uint32_t frequency);
typedef void (*TimerClassSetCallbackFunc)(TimerClass *timer, TimerCallback callback, void *arg);
typedef void (*TimerClassStartFunc)(TimerClass *timer);
typedef void (*TimerClassStopFunc)(TimerClass *timer);

struct TimerClass {
	TimerClassSetFrequencyFunc setFrequency;
	TimerClassSetCallbackFunc setCallback;
	TimerClassStartFunc start;
	TimerClassStopFunc stop;
};

#define TIMER_CLASS(obj) ((TimerClass*)(obj))

#define timerSetFrequency(timer, frequency) TIMER_CLASS(timer)->setFrequency(TIMER_CLASS(timer), frequency)
#define timerSetCallback(timer, callback, arg) TIMER_CLASS(timer)->setCallback(TIMER_CLASS(timer), callback, arg)
#define timerStart(timer) TIMER_CLASS(timer)->start(TIMER_CLASS(timer))
#define timerStop(timer) TIMER_CLASS(timer)->stop(TIMER_CLASS(timer))

#endif

#endif

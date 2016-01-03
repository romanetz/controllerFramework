#ifndef __ARM_THREAD_H__
#define __ARM_THREAD_H__

#define PLATFORM_THREAD_FIELDS \
	Thread *next; \
	Thread *prev; \
	uint32_t *stack; \
	uint32_t *stackPointer; \
	timestamp_t timeout;

#include <thread.h>

extern Thread *currentThread;
extern Thread *nextThread;

void platform_threadInit(Thread *thread, ThreadFunc func, void *arg);

void executeTaskSheduler(void);

BOOL threadSleepUntil(timestamp_t timeout);

#endif

#ifndef __THREAD_H__
#define __THREAD_H__

#include <generic.h>
#include <stdlib.h>

typedef struct Thread Thread;

typedef uint8_t ThreadPriority;

#define THREAD_PRIORITY_NORMAL 128
#define THREAD_PRIORITY_IDLE 1
#define THREAD_PRIORITY_STOPPED 0

typedef void (*ThreadFunc)(void *arg);

#include <platform_thread.h>

struct Thread {
	PLATFORM_THREAD_FIELDS
	const char *name;
	size_t stackSize;
	ThreadPriority priority;
};

void platform_threadInit(Thread *thread, ThreadFunc func, void *arg);

Thread *threadCreate(const char *name, ThreadPriority priority, size_t stackSize, ThreadFunc func, void *arg);

BOOL threadSleepUntil(timestamp_t timeout);
void threadSetReady(Thread *thread);

#endif

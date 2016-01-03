#ifndef __POSIX_THREAD_H__
#define __POSIX_THREAD_H__

#include <pthread.h>

#define PLATFORM_THREAD_FIELDS \
	pthread_t pthread;

#include <thread.h>

void platform_threadInit(Thread *thread, ThreadFunc func, void *arg);

#endif

#ifndef __ARM_MUTEX_H__
#define __ARM_MUTEX_H__

#include <mutex.h>
#include <event.h>

struct Mutex {
	volatile uint32_t state;
	EventSource eventSource;
};

#endif

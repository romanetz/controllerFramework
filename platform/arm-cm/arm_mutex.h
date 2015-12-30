#ifndef __ARM_MUTEX_H__
#define __ARM_MUTEX_H__

#include <mutex.h>

struct Mutex {
	volatile uint32_t state;
};

#endif

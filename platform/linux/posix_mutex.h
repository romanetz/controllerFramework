#ifndef __POSIX_MUTEX_H__
#define __POSIX_MUTEX_H__

#include <linux.h>
#include <mutex.h>

struct Mutex {
	pthread_mutex_t mutex;
};

#endif

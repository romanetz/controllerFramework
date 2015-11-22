#ifndef __MUTEX_PLATFORM_H__
#define __MUTEX_PLATFORM_H__

#include <pthread.h>

#define MUTEX_PLATFORM_FIELDS pthread_mutex_t _mutex;

#endif

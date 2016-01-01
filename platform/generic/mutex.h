#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <generic.h>

typedef struct Mutex Mutex;

void mutexInit(Mutex *mutex);
void mutexDestroy(Mutex *mutex);
BOOL mutexLockTimeout(Mutex *mutex, timestamp_t timeout);
void mutexLock(Mutex *mutex);
void mutexUnlock(Mutex *mutex);

#include <platform_mutex.h>

#endif

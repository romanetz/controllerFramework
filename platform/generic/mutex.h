#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <stdint.h>

typedef struct Mutex Mutex;

void mutexInit(Mutex *mutex);
void mutexDestroy(Mutex *mutex);
uint8_t mutexLockTimeout(Mutex *mutex, uint64_t timeout);
void mutexLock(Mutex *mutex);
void mutexUnlock(Mutex *mutex);

#include <platform_mutex.h>

#endif

#include <posix_mutex.h>

void mutexInit(Mutex *mutex) {
	pthread_mutex_init(&mutex->mutex, NULL);
}

void mutexDestroy(Mutex *mutex) {
	pthread_mutex_destroy(&mutex->mutex);
}

uint8_t mutexLockTimeout(Mutex *mutex, uint64_t timeout) {
	if (timeout == -1) {
		mutexLock(mutex);
		return 1;
	}
	if (pthread_mutex_trylock(&mutex->mutex) == 0) {
		return 1;
	}
	return 0;
}

void mutexLock(Mutex *mutex) {
	pthread_mutex_lock(&mutex->mutex);
}

void mutexUnlock(Mutex *mutex) {
	pthread_mutex_unlock(&mutex->mutex);
}


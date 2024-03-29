#include <arm_mutex.h>

inline void __dmb() {
	__asm__ volatile ("dmb");
}

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
inline uint32_t __ldrex(volatile uint32_t *addr) {
	uint32_t r;
	__asm__ volatile ("ldrex %0, [%1]":"=r"(r):"r"(addr));
	return r;
}

inline uint32_t __strex(volatile uint32_t *addr, uint32_t value) {
	uint32_t r;
	__asm__ volatile ("strex %0, %2, [%1]":"=&r"(r):"r"(addr),"r"(value));
	return r;
}
#endif

static BOOL tryLockMutex(volatile uint32_t *mutex) {
	uint32_t r = __ldrex(mutex);
	if (r == 0) {
		r = __strex(mutex, 1);
	}
	__dmb();
	return r == 0;
}

void mutexInit(Mutex *mutex) {
	mutex->state = 0;
	eventSourceInit(&(mutex->eventSource));
}

void mutexDestroy(Mutex *mutex) {
	eventSourceDestroy(&(mutex->eventSource));
}

BOOL mutexLockTimeout(Mutex *mutex, timestamp_t timeout) {
	do {
		if (tryLockMutex(&mutex->state)) return TRUE;
		if (!eventSourceWaitSignalTimeout(&(mutex->eventSource), timeout)) return FALSE;
	} while (timeout > 0);
	return FALSE;
}

void mutexLock(Mutex *mutex) {
	mutexLockTimeout(mutex, -1);
}

void mutexUnlock(Mutex *mutex) {
	__dmb();
	mutex->state = 0;
	eventSourceSendSignal(&(mutex->eventSource));
}
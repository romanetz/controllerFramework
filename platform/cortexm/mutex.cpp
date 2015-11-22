#include <mutex.h>

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

static bool tryLockMutex(volatile uint32_t *mutex) {
	uint32_t r = __ldrex(mutex);
	if (r == 0) {
		r = __strex(mutex, 1);
	}
	__dmb();
	return r == 0;
}

Mutex::Mutex() {
	_locked = 0;
}

Mutex::~Mutex() {}

bool Mutex::lock(int timeout) {
	do {
		if (tryLockMutex(&_locked)) return true;
	} while (timeout != 0);
	return false;
}

void Mutex::unlock() {
	__dmb();
	_locked = 0;
}

bool Mutex::locked() {
	return _locked != 0;
}

#include <mutex.h>

Mutex::Mutex() {
	pthread_mutex_init(&_mutex, NULL);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&_mutex);
}

bool Mutex::lock(int timeout) {
	do {
		if (pthread_mutex_trylock(&_mutex) == 0) return true;
	} while (timeout != 0);
	return false;
}

void Mutex::unlock() {
	pthread_mutex_unlock(&_mutex);
}

bool Mutex::locked() {
	bool locked = pthread_mutex_trylock(&_mutex) != 0;
	if (!locked) {
		pthread_mutex_unlock(&_mutex);
	}
	return locked;
}

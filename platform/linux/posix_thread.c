#include <posix_thread.h>

typedef void *(*pthread_func_t)(void*);

void platform_threadInit(Thread *thread, ThreadFunc func, void *arg) {
	pthread_create(&(thread->pthread), NULL, (pthread_func_t)func, arg);
}

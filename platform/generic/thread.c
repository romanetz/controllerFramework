#include <thread.h>

Thread *threadCreate(const char *name, ThreadPriority priority, size_t stackSize, ThreadFunc func, void *arg) {
	Thread *thread = malloc(sizeof(Thread));
	if (thread) {
		thread->name = name;
		thread->priority = priority;
		thread->stackSize = stackSize;
		platform_threadInit(thread, func, arg);
	}
	return thread;
}

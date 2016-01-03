#include <posix_event.h>

void eventSourceInit(EventSource *eventSource) {
	pthread_cond_init(&(eventSource->condVar), NULL);
}

void eventSourceDestroy(EventSource *eventSource) {
	pthread_cond_destroy(&(eventSource->condVar));
}

void eventSourceSendSignal(EventSource *eventSource) {
	pthread_cond_broadcast(&(eventSource->condVar));
}

BOOL eventSourceWaitSignalTimeout(EventSource *eventSource, timestamp_t timeout) {
	return FALSE;
}

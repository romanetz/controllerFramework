#include <stdlib.h>
#include <event.h>

EventSource *eventSourceCreate(void) {
	EventSource *eventSource = malloc(sizeof(EventSource));
	if (eventSource) {
		eventSourceInit(eventSource);
	}
	return eventSource;
}

void eventSourceFree(EventSource *eventSource) {
	eventSourceDestroy(eventSource);
	free(eventSource);
}
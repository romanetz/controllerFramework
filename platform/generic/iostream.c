#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#ifdef USE_CLASSES

int ioStreamVPrintfTimeout(void *stream, const char *fmt, va_list *arg, timestamp_t timeout) {
	char *str;
	int r = vasprintf(&str, fmt, *arg);
	ioStreamWriteTimeout(stream, str, r, timeout);
	free(str);
	return r;
}

int ioStreamPrintfTimeout(void *stream, uint64_t timeout, const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	int r = ioStreamVPrintfTimeout(stream, fmt, &arg, timeout);
	va_end(arg);
	return r;
}

#endif

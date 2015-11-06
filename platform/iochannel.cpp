#include <stdio.h>
#include <stdarg.h>
#include <iochannel.h>

int IOChannel::printf(const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	int len = vsnprintf(NULL, 0, fmt, arg);
	va_end(arg);
	va_start(arg, fmt);
	char buffer[len + 1];
	vsnprintf(buffer, len + 1, fmt, arg);
	int r = write(buffer);
	va_end(arg);
	return r;
}

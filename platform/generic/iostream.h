#ifndef __IO_STREAM_H__
#define __IO_STREAM_H__

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

typedef struct IOStreamClass IOStreamClass;

typedef int (*IOStreamWriteTimeoutFunc)(IOStreamClass *stream, const void *data, int len, uint64_t timeout);
typedef int (*IOStreamReadTimeoutFunc)(IOStreamClass *stream, void *buffer, int len, uint64_t timeout);
typedef void (*IOStreamFlushFunc)(IOStreamClass *stream);
typedef void (*IOStreamCloseFunc)(IOStreamClass *stream);

struct IOStreamClass {
	IOStreamWriteTimeoutFunc writeTimeout;
	IOStreamReadTimeoutFunc readTimeout;
	IOStreamFlushFunc flush;
	IOStreamCloseFunc close;
};

#define IO_STREAM_CLASS(obj) ((IOStreamClass*)(obj))

#define ioStreamWriteTimeout(stream, data, len, timeout) IO_STREAM_CLASS(stream)->writeTimeout(IO_STREAM_CLASS(stream), data, len, timeout)
#define ioStreamWrite(stream, data, len) ioStreamWriteTimeout(stream, data, len, -1)
#define ioStreamReadTimeout(stream, buffer, len, timeout) IO_STREAM_CLASS(stream)->writeTimeout(IO_STREAM_CLASS(stream), buffer, len, timeout)
#define ioStreamRead(stream, buffer, len) ioStreamReadTimeout(stream, buffer, len, -1)
#define ioStreamFlush(stream) IO_STREAM_CLASS(stream)->flush(IO_STREAM_CLASS(stream))
#define ioStreamClose(stream) IO_STREAM_CLASS(stream)->close(IO_STREAM_CLASS(stream))

#define ioStreamWriteStringTimeout(stream, string, timeout) ioStreamWriteTimeout(stream, string, strlen(string), timeout)
#define ioStreamWriteString(stream, string) ioStreamWrite(stream, string, strlen(string))

int ioStreamVPrintfTimeout(void *stream, const char *fmt, va_list *arg, uint64_t timeout);
#define ioStreamVPrintf(stream, fmt, arg) ioStreamPrintfTimeout(stream, fmt, arg, -1)
int ioStreamPrintfTimeout(void *stream, uint64_t timeout, const char *fmt, ...);
#define ioStreamPrintf(stream, ...) ioStreamPrintfTimeout(stream, -1, __VA_ARGS__)

#endif

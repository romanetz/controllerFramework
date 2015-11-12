#ifndef __LINUX_FILECHANNEL_H__
#define __LINUX_FILECHANNEL_H__

#include <linux.h>
#include <iochannel.h>

class LinuxFileChannel: public IOChannel {
	private:
		int _fd;
		
	public:
		LinuxFileChannel(int fd) {
			_fd = fd;
		}
		
		LinuxFileChannel(const char *fileName, int mode) {
			_fd = ::open(fileName, mode);
		}
		
		void flush() {
			::fsync(_fd);
		}
		
		int fd() {
			return _fd;
		}
		
	protected:
		int writeData(const char *data, int len) {
			return ::write(_fd, data, len);
		}
		
		int readData(char *buffer, int len) {
			return ::read(_fd, buffer, len);
		}
		
};

#endif

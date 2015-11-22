/**
	\addtogroup platform Platform-independent
	@{
*/
#ifndef __IO_CHANNEL_H__
#define __IO_CHANNEL_H__

#include <string.h>

/**
	\brief Basic I/O channel.
	Abstract class for bidirectional channels.
*/
class IOChannel {
	private:
		/**
			\brief Implementation of write.
			\param[in] data Pointer to buffer with data to write.
			\param[in] len Bytes count.
			\param[in] timeout Timeout.
			\return Returns count of bytes that really written to the channel.
		*/
		virtual int writeData(const char *data, int len, int timeout) = 0;
		
		/**
			\brief Implementation of read.
			\param[out] buffer Pointer to read buffer.
			\param[in] len Buffer size.
			\param[in] timeout Timeout.
			\return Returns count of bytes that have been read.
		*/
		virtual int readData(char *buffer, int len, int timeout) = 0;
	
	public:
		virtual ~IOChannel() {};
		
		/**
			\brief Write bytes to the channel.
			\param[in] data Pointer to byte array.
			\param[in] len Bytes count.
			\param[in] timeout Timeout.
			\return Returns count of bytes that really written to the channel.
		*/
		int write(const char *data, int len, int timeout = -1) {
			return writeData(data, len, timeout);
		}
		
		/**
			\brief Write zero-terminated string to the channel.
			\param[in] str Pointer to string.
			\return Returns count of bytes that really written to the channel.
		*/
		int write(const char *str) {
			return write(str, strlen(str));
		}
		
		/**
			\brief Write single character to the channel.
			\param[in] chr Char.
			\return Returns true if successfull, otherwise false.
		*/
		bool write(char chr) {
			return write(&chr, sizeof(chr));
		}
		
		/**
			\brief Read bytes from the channel.
			\param[out] buffer Pointer to read buffer.
			\param[in] len Buffer size.
			\return Returns count of bytes that have been read.
		*/
		int read(char *buffer, int len, int timeout = -1) {
			return readData(buffer, len, timeout);
		}
		
		/**
			\brief Formatted print to the channel.
			\param[in] fmt Format string.
			\return Returns count of bytes written to the stream.
		*/
		int printf(const char *fmt, ...);
		
		/**
			\brief Flush I/O buffers.
		*/
		virtual void flush() {};
		
};

#endif
/** @} */

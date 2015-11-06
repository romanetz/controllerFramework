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
			\return Returns count of bytes that really written to the channel.
		*/
		virtual int writeData(const char *data, int len) = 0;
		
		/**
			\brief Implementation of read.
			\param[out] buffer Pointer to read buffer.
			\param[in] len Buffer size.
			\return Returns count of bytes that have been read.
		*/
		virtual int readData(char *buffer, int len) = 0;
	
	public:
		/**
			\brief Write bytes to the channel.
			\param[in] data Pointer to byte array.
			\param[in] len Bytes count.
			\return Returns count of bytes that really written to the channel.
		*/
		int write(const char *data, int len) {
			return writeData(data, len);
		}
		
		/**
			\brief Write zero-terminated string to the channel.
			\param[in] str Pointer to string.
			\return Returns count of bytes that really written to the channel.
		*/
		int write(const char *str) {
			return writeData(str, strlen(str));
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
		int read(char *buffer, int len) {
			return readData(buffer, len);
		}
		
		/**
			\brief Formatted print to the channel.
			\param[in] fmt Format string.
			\return Returns count of bytes written to the stream.
		*/
		int printf(const char *fmt, ...);
		
};

#endif
/** @} */

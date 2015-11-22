#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <mutex_platform.h>

class Mutex {
	private:
		MUTEX_PLATFORM_FIELDS
		
	public:
		Mutex();
		
		~Mutex();
		
		bool lock(int timeout = -1);
		
		void unlock();
		
		bool locked();
		
};

#endif

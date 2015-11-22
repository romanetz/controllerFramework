#ifndef __PLATFORM_MUTEX_H__
#define __PLATFORM_MUTEX_H__

#include <stdint.h>

#define MUTEX_PLATFORM_FIELDS volatile uint32_t _locked;

#endif

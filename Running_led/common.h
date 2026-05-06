#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include "app_time.h"


#define UINT32_BIT(n) ((uint32_t)1 << (n))
#define INT32_BIT(n)  ((int32_t)1 << (n))


//#define  sec *(1000000000LL)
#define  ms  *(1000000LL)
#define  us  *(1000LL)

#define NS_PER_SEC 1000000000LL
#define NS_PER_MS  1000000LL
#define NS_PER_US  1000LL

#define US_PER_SEC 1000000LL
#define US_PER_MS  1000LL


#endif


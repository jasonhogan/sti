#ifndef TYPES_H
#define TYPES_H

#include <limits.h>

#if defined(USHRT_MAX) && UCHAR_MAX == 0xff
typedef unsigned char Int8;
#else
#  error You need to define a 8bit number!
#endif

#if defined(USHRT_MAX) && USHRT_MAX == 0xffff
typedef unsigned short Int16;
#else
#  error You need to define a 16bit number!
#endif

#if defined(UINT_MAX) && UINT_MAX == 0xffffffff
typedef unsigned int Int32;
#else
#  error You need to define a 32bit number!
#endif

#if defined(ULONG_MAX) && ULONG_MAX == 0xffffffffffffffff
typedef unsigned long Int64;
#elif defined(ULLONG_MAX) && ULLONG_MAX == 0xffffffffffffffff
typedef unsigned long long Int64;
#elif defined(ULONG_LONG_MAX) && ULONG_LONG_MAX == 0xffffffffffffffff
typedef unsigned long long Int64;
#elif defined(_UI64_MAX) && _UI64_MAX == 0xffffffffffffffff
typedef unsigned __int64 Int64;
#else
#  error You need to define a 64bit number!
#endif

enum BoardID {ANALOG, DDS, DIGITAL};

#endif

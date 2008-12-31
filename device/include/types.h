
#ifndef TYPES_H
#define TYPES_H

#include <limits.h>

#if defined(UCHAR_MAX) && UCHAR_MAX == 0xff
typedef unsigned char uInt8;
#else
#  error You need to define an unsigned 8bit number!
#endif

#if defined(USHRT_MAX) && USHRT_MAX == 0xffff
typedef unsigned short uInt16;
#else
#  error You need to define an unsigned 16bit number!
#endif

#if defined(UINT_MAX) && UINT_MAX == 0xffffffff
typedef unsigned int uInt32;
#else
#  error You need to define an unsigned 32bit number!
#endif

#if defined(ULONG_MAX) && ULONG_MAX == 0xffffffffffffffff
typedef unsigned long uInt64;
#elif defined(ULLONG_MAX) && ULLONG_MAX == 0xffffffffffffffff
typedef unsigned long long uInt64;
#elif defined(ULONG_LONG_MAX) && ULONG_LONG_MAX == 0xffffffffffffffff
typedef unsigned long long uInt64;
#elif defined(_UI64_MAX) && _UI64_MAX == 0xffffffffffffffff
typedef unsigned __int64 uInt64;
#else
#  error You need to define an unsigned 64bit number!
#endif


#if defined(LONG_MAX) &&ULONG_MAX == 9223372036854775807
typedef long Int64;
#elif defined(LLONG_MAX) && LLONG_MAX == 9223372036854775807
typedef long long Int64;
#elif defined(LONG_LONG_MAX) && LONG_LONG_MAX == 9223372036854775807
typedef long long Int64;
#elif defined(_I64_MAX) && _I64_MAX == 9223372036854775807
typedef __int64 Int64;
#else
#  error You need to define a 64bit number!
#endif



enum BoardID {ANALOG, DDS, DIGITAL};

#endif

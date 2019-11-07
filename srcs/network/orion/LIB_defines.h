#ifndef _LIB_DEFINES_H
#define _LIB_DEFINES_H

#include <sys/types.h>
#ifdef _WIN32
typedef char int8_t;
typedef unsigned char u_int8_t;
typedef short int16_t;
typedef unsigned short u_int16_t;
typedef int int32_t;
typedef unsigned int u_int32_t;
typedef __int64 int64_t;
typedef unsigned __int64 u_int64_t;

typedef unsigned char u_char;
typedef unsigned int u_int;

#define inline __inline
#endif

#define LIB_Type_max_uint	u_int64_t
#define LIB_Type_max_int	int64_t

#endif	/* _LIB_DEFINES_H */

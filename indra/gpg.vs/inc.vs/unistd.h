
#pragma once

#include <basetsd.h>
#include <stddef.h>
#include <errno.h>

#ifndef S_IWUSR
#define S_IWUSR _S_IWRITE
#endif

#ifndef S_IRUSR
#define S_IRUSR _S_IREAD
#endif

#ifndef S_ISREG
#define S_ISREG(A) (1)
#endif

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef _SSIZE_T_DEFINED
typedef signed int ssize_t;
#define _SSIZE_T_DEFINED
#endif

#ifndef _PID_T_DEFINED
typedef int pid_t;
#define _PID_T_DEFINED
#endif

#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

/* Define to `int' if unavailable. */
#ifndef _SOCKLEN_T_DEFINED 
#ifndef socklen_t
typedef int socklen_t;
#define _SOCKLEN_T_DEFINED 1
#endif
#endif

// !s0rr0w!
#define SIZEOF_UINT64_T 8
#define UINT64_C(C) (C)
typedef unsigned __int64 uint64_t;

#if WIN32          /* [$PLOTR$] */
#ifndef snprintf
#define snprintf sprintf_s
#endif
#include <io.h>
#include <process.h>
#pragma warning(disable : 4996)
#endif /* WIN32 */ /* [/$PLOTR$] */

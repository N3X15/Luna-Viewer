/* config.h.in.  Generated from configure.ac by autoheader.  */


/* need this, because some autoconf tests rely on this (e.g. stpcpy)
 * and it should be used for new programs  */
#define _GNU_SOURCE  1


/* Define if you don't want the default EGD socket name. For details see
   cipher/rndegd.c */
#undef EGD_SOCKET_NAME

/* The default error source for libgcrypt. */
#undef GPG_ERR_SOURCE_DEFAULT

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Defined if the mlock() call does not work */
#undef HAVE_BROKEN_MLOCK

/* Defined if a `byte' is typedef'd */
#undef HAVE_BYTE_TYPEDEF

/* Define to 1 if you have the `clock_gettime' function. */
#undef HAVE_CLOCK_GETTIME

/* Define to 1 if you have the declaration of `sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL_SYS_SIGLIST 0

/* defined if the system supports a random device */
#undef HAVE_DEV_RANDOM

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
#undef HAVE_DOPRNT

/* defined if we run on some of the PCDOS like systems (DOS, Windoze. OS/2)
   with special properties like no file modes */
#define HAVE_DOSISH_SYSTEM

/* defined if we must run on a stupid file system */
#define HAVE_DRIVE_LETTERS

/* Define to 1 if you have the `gethrtime' function. */
#undef HAVE_GETHRTIME

/* Define to 1 if you have the `getpagesize' function. */
#undef HAVE_GETPAGESIZE

/* Define to 1 if you have the `getrusage' function. */
#undef HAVE_GETRUSAGE

/* Define to 1 if you have the `gettimeofday' function. */
#undef HAVE_GETTIMEOFDAY

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the `nsl' library (-lnsl). */
#undef HAVE_LIBNSL

/* Define to 1 if you have the `rt' library (-lrt). */
#undef HAVE_LIBRT

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Defined if the system supports an mlock() call */
#undef HAVE_MLOCK

/* Define to 1 if you have the `mmap' function. */
#undef HAVE_MMAP

/* Define to 1 if you have the `raise' function. */
#define HAVE_RAISE 1

/* Define to 1 if you have the `rand' function. */
#define HAVE_RAND 1

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stpcpy' function. */
#undef HAVE_STPCPY

/* Define to 1 if you have the `strcasecmp' function. */
#undef HAVE_STRCASECMP

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `stricmp' function. */
#define HAVE_STRICMP 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the <sys/capability.h> header file. */
#undef HAVE_SYS_CAPABILITY_H

/* Define to 1 if you have the <sys/mman.h> header file. */
#undef HAVE_SYS_MMAN_H

/* Define to 1 if you have the <sys/select.h> header file. */
#undef HAVE_SYS_SELECT_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Defined if a `u16' is typedef'd */
#undef HAVE_U16_TYPEDEF

/* Defined if a `u32' is typedef'd */
#undef HAVE_U32_TYPEDEF

/* Defined if a `ulong' is typedef'd */
#undef HAVE_ULONG_TYPEDEF

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Defined if a `ushort' is typedef'd */
#undef HAVE_USHORT_TYPEDEF

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the `wait4' function. */
#undef HAVE_WAIT4

/* Define to 1 if you have the `waitpid' function. */
#undef HAVE_WAITPID

/* Defined if this is not a regular release */
#undef IS_DEVELOPMENT_VERSION

/* Define to use the (obsolete) malloc guarding feature */
#undef M_GUARD

/* defined to the name of the strong random device */
#undef NAME_OF_DEV_RANDOM

/* defined to the name of the weaker random device */
#undef NAME_OF_DEV_URANDOM

/* Name of this package */
#define PACKAGE "libgcrypt-1.2.2.vs"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "s0rr0w@s0rr0w.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libgcrypt-1.2.2 built with Visual Studio"

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* A human readable text with the name of the OS */
#define PRINTABLE_OS_NAME "Win32"

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* The size of a `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of a `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 4

/* The size of a `unsigned long long', as computed by sizeof. */
#undef SIZEOF_UNSIGNED_LONG_LONG

/* The size of a `unsigned short', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Defined if this module should be included */
#define USE_AES 1

/* Defined if this module should be included */
#define USE_ARCFOUR 1

/* Defined if this module should be included */
#define USE_BLOWFISH 1

/* define if capabilities should be used */
#undef USE_CAPABILITIES

/* Defined if this module should be included */
#define USE_CAST5 1

/* Defined if this module should be included */
#define USE_CRC 1

/* Defined if this module should be included */
#define USE_DES 1

/* Defined if this module should be included */
#define USE_DSA 1

/* Defined if this module should be included */
#define USE_ELGAMAL 1

/* Defined if this module should be included */
#define USE_MD4 1

/* Defined if this module should be included */
#define USE_MD5 1

/* set this to limit filenames to the 8.3 format */
#undef USE_ONLY_8DOT3

/* Defined if this module should be included */
#define USE_RFC2268 1

/* Defined if this module should be included */
#define USE_RMD160 1

/* Defined if the EGD based RNG should be used. */
#undef USE_RNDEGD

/* Defined if the /dev/random based RNG should be used. */
#undef USE_RNDLINUX

/* Defined if the default Unix RNG should be used. */
#undef USE_RNDUNIX

/* Defined if the Windows specific RNG should be used. */
#define USE_RNDW32 1

/* Defined if this module should be included */
#define USE_RSA 1

/* Defined if this module should be included */
#define USE_SERPENT 1

/* Defined if this module should be included */
#define USE_SHA1 1

/* Defined if this module should be included */
#define USE_SHA256 1

/* Defined if this module should be included */
#define USE_SHA512 1

/* Defined if this module should be included */
#undef USE_TIGER

/* Defined if this module should be included */
#define USE_TWOFISH 1

/* Version of this package */
#define VERSION "1.2.2"

/* Defined if compiled symbols have a leading underscore */
#define WITH_SYMBOL_UNDERSCORE 1

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

/* To allow the use of Libgcrypt in multithreaded programs we have to use
    special features from the library. */
#ifndef _REENTRANT
# define _REENTRANT 1
#endif


/* Define to empty if `const' does not conform to ANSI C. */
#define const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

/* Define to `unsigned' if <sys/types.h> does not define. */
// #undef size_t

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

#define _GCRYPT_IN_LIBGCRYPT 1


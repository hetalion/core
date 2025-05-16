/*
** Definitions for Het code that must come before any header file
*/
#ifndef het_prefix_h
#define het_prefix_h

/*
** Alows POSIX/XSI stuff
*/
#if !defined(RET_USE_C89)

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 600
#elif _XOPEN_SOURCE == 0
#undef _XOPEN_SOURCE /* use -D _XOPEN_SOURCE=0 to undefine */
#endif

/*
** Allows manipulation of large files in gcc and some other compilers
*/
#if !defined(HET_32BITS) && !defined(_FILE_OFFSET_BITS)
#define _LARGEFILE_SOURCE 1
#define _FILE_OFFSET_BITS 64
#endif

#endif

/*
** Windows stuff
*/

#if defined(_WIN32)

#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS /* avoid warnings about ISO C functions */
#endif

#endif

#endif
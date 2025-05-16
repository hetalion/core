/**
 * Configuration file for the hetalion virtual machine.
 * This and all other parts of hetalion core are heavely based on official the
 * lua interpreter.
 */

#ifndef het_conf_h
#define het_conf_h

#include <limits.h>
#include <stddef.h>

/*
** General configuration file for hetalion
**
** Search for "@@" to find all configurable definitions.
*/

/*
** System configuration: macros to adapt (if needed) hetalion to some particular platform, for instance restricting it to C89.
*/

/*
@@ HET_USE_C89 controls the use of non-ISO-C89 features.
** Define it if you want hetalion to avoid the use of a few C99 features.
** or Windows-specific features on windows.
*/
/* #define HET_USE_C89 */

/*
** By default, hetalion on windows uses specific Windows features.
*/
#if !defined (HET_USE_C89) && defined(_WIN32) && !defined(_WIN32_WCE)
#define HET_USE_WINDOWS /* enable a set of features for regular windows */
#endif

#if defined(HET_USE_WINDOWS)
#define HET_DL_DLL      /* enables support for Dynamic-Linked Libraries*/
#define HET_USE_C89     /* Windows uses C89 */
#endif

#if defined(HET_USE_LINUX)
#define HET_USE_POSIX
#define HET_USE_DLOPEN  /* needs -ldl*/
#endif

#if defined(HET_USE_MACOSX)
#define HET_USE_POSIX
#define HET_USE_DLOPEN  /* needs -ldl */
#endif

#if defined(HET_USE_IOS)
#define HET_USE_POSIX
#define HET_USE_DLOPEN
#endif

/*
@@ HETI_IS32INT is true if 'int' has at least 32 bits.
*/
#define HETI_IS32INT            ((UINT_MAX >> 30) >= 3)
/* --- */

/*
** number type configuration. should be set externally, 
** because other code connected to hetalion must have the same configuration.
*/

/*
@@ HET_INT_TYPE defines the type for hetalion integers.
@@ HET_FLOAT_TYPE defines the type for hetalion floats.
** Hetalion should work fine with any mix of support C compiler options.
** Usual configurations should be 64-bit ints and doubles (default), 32-bit integers and
** 'float' for other platforms (restricted ones), and 'long'/'double' for (c compiler non-C99 compliant, which may not have support for `long long`)
*/

/* predefined options for HET_INT_TYPE */
#define HET_INT_INT             1
#define HET_INT_LONG            2
#define HET_INT_LONGLONG        3

/* predefined options for HET_FLOAT_TYPE */
#define HET_FLOAT_FLOAT         1
#define HET_FLOAT_DOUBLE        2
#define HET_FLOAT_LONGDOUBLE    3

/* default long long and double for 64-bit hetalion nodes */
#define HET_INT_DEFAULT         HET_INT_LONGLONG
#define HET_FLOAT_DEFAULT       HET_FLOAT_DOUBLE

/*
@@ HET_32BITS enables Hetalion with 32-bit integers and 32-bit floats
*/
#define HET_32BITS              0

/*
@@ HET_C89_NUMBERS ensures that Hetalion uses the largest types available for
** C89 (`long` and `double`); Windows has __int64, so it doesn't need it
*/
#if defined(HET_USE_C89) && !defined(HET_USE_WINDOWS)
#define HET_C89_NUMBERS         1
#else
#define HET_C89_NUMBERS         0
#endif

#if HET_32BITS
/*
** 32-bit integers and 'float'
*/
#if HETI_IS32INT /* use `int` if big enough */
#define HET_INT_TYPE            HET_INT_INT
#else /* otherwise for for `long` */
#define HET_INT_TYPE            HET_INT_LONG
#endif
#define HET_FLOAT_TYPE          HET_FLOAT_FLOAT 

#elif HET_C89_NUMBERS
/*
** largest types available for C89 (`long` & `double`)
*/
#define HET_INT_TYPE            HET_INT_LONG
#define HET_FLOAT_TYPE          HET_FLOAT_DOUBLE

#else /* use defaults */
#define HET_INT_TYPE            HET_INT_DEFAULT
#define HET_FLOAT_TYPE          HET_FLOAT_DEFAULT

#endif


/*
** use this for redefinitions without modying the main part of the file.
*/
#endif
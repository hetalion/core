/**
 * Configuration file for the hetalion virtual machine.
 * This and all other parts of hetalion core are heavely based on official the
 * hetalion interpreter.
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
** System configuration: macros to adapt (if needed) hetalion to some particular
*platform, for instance restricting it to C89.
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
#if !defined(HET_USE_C89) && defined(_WIN32) && !defined(_WIN32_WCE)
#define HET_USE_WINDOWS /* enable a set of features for regular windows */
#endif

#if defined(HET_USE_WINDOWS)
#define HET_DL_DLL  /* enables support for Dynamic-Linked Libraries*/
#define HET_USE_C89 /* Windows uses C89 */
#endif

#if defined(HET_USE_LINUX)
#define HET_USE_POSIX
#define HET_USE_DLOPEN /* needs -ldl*/
#endif

#if defined(HET_USE_MACOSX)
#define HET_USE_POSIX
#define HET_USE_DLOPEN /* needs -ldl */
#endif

#if defined(HET_USE_IOS)
#define HET_USE_POSIX
#define HET_USE_DLOPEN
#endif

/*
@@ HETI_IS32INT is true if 'int' has at least 32 bits.
*/
#define HETI_IS32INT ((UINT_MAX >> 30) >= 3)
/* --- */

/*
** number type configuration. should be set externally,
** because other code connected to hetalion must have the same configuration.
*/

/*
@@ HET_INT_TYPE defines the type for hetalion integers.
@@ HET_FLOAT_TYPE defines the type for hetalion floats.
** Hetalion should work fine with any mix of support C compiler options.
** Usual configurations should be 64-bit ints and doubles (default), 32-bit
integers and
** 'float' for other platforms (restricted ones), and 'long'/'double' for (c
compiler non-C99 compliant, which may not have support for `long long`)
*/

/* predefined options for HET_INT_TYPE */
#define HET_INT_INT 1
#define HET_INT_LONG 2
#define HET_INT_LONGLONG 3

/* predefined options for HET_FLOAT_TYPE */
#define HET_FLOAT_FLOAT 1
#define HET_FLOAT_DOUBLE 2
#define HET_FLOAT_LONGDOUBLE 3

/* default long long and double for 64-bit hetalion nodes */
#define HET_INT_DEFAULT HET_INT_LONGLONG
#define HET_FLOAT_DEFAULT HET_FLOAT_DOUBLE

/*
@@ HET_32BITS enables Hetalion with 32-bit integers and 32-bit floats
*/
#define HET_32BITS 0

/*
@@ HET_C89_NUMBERS ensures that Hetalion uses the largest types available for
** C89 (`long` and `double`); Windows has __int64, so it doesn't need it
*/
#if defined(HET_USE_C89) && !defined(HET_USE_WINDOWS)
#define HET_C89_NUMBERS 1
#else
#define HET_C89_NUMBERS 0
#endif

#if HET_32BITS
/*
** 32-bit integers and 'float'
*/
#if HETI_IS32INT /* use `int` if big enough */
#define HET_INT_TYPE HET_INT_INT
#else /* otherwise for for `long` */
#define HET_INT_TYPE HET_INT_LONG
#endif
#define HET_FLOAT_TYPE HET_FLOAT_FLOAT

#elif HET_C89_NUMBERS
/*
** largest types available for C89 (`long` & `double`)
*/
#define HET_INT_TYPE HET_INT_LONG
#define HET_FLOAT_TYPE HET_FLOAT_DOUBLE

#else /* use defaults */
#define HET_INT_TYPE HET_INT_DEFAULT
#define HET_FLOAT_TYPE HET_FLOAT_DEFAULT

#endif

/*
** Path configuration.
*/

/*
** HET_PATH_SEP  is the character that separates templates in a path.
** HET_PATH_MARK is the string that marks the substitution points in a template.
** HET_EXEC_DIR  in a Windows path is replaced by the executable's directory.
*/
#define HET_PATH_SEP ";"
#define HET_PATH_MARK "?"
#define HET_EXEC_DIR "!"

/*
@@ HET_PATH_DEFAULT  is the default path that hetalion uses to look for
libraries.
@@ HET_CPATH_DEFAULT is the default path that hetalion uses to look for c
libraries.
** CHANGE them if placed on a non-conventional directory.
*/

#define HET_VDIR HET_VERSION_MAJOR "." HET_VERSION_MINOR
#if defined(_WIN32)
/*
** In Windows, any exclamation mark in the path is replaced by the
** path of the directory of the executable file of the current process.
*/
#define HET_LDIR "!\\het\\"
#define HET_CDIR "!\\"
#define HET_SHRDIR "!\\..\\share\\het\\" HET_VDIR "\\"

#if !defined(HET_PATH_DEFAULT)
#define HET_PATH_DEFAULT                                                       \
  HET_LDIR "?.het;" HET_LDIR "?\\init.het;" HET_CDIR "?.het;" HET_CDIR         \
           "?\\init.het;" HET_SHRDIR "?.het;" HET_SHRDIR "?\\init.het;"        \
           ".\\?.het;"                                                         \
           ".\\?\\init.het"
#endif

#if !defined(HET_CPATH_DEFAULT)
#define HET_CPATH_DEFAULT                                                      \
  HET_CDIR "?.dll;" HET_CDIR "..\\lib\\het\\" HET_VDIR "\\?.dll;" HET_CDIR     \
           "loadall.dll;"                                                      \
           ".\\?.dll"
#endif
#else

#define HET_ROOT "/usr/local/"
#define HET_LDIR HET_ROOT "share/het/" HET_VDIR "/"
#define HET_CDIR HET_ROOT "lib/het/" HET_VDIR "/"

#if !defined(HET_PATH_DEFAULT)
#define HET_PATH_DEFAULT                                                       \
  HET_LDIR "?.het;" HET_LDIR "?/init.het;" HET_CDIR "?.het;" HET_CDIR          \
           "?/init.het;"                                                       \
           "./?.het;"                                                          \
           "./?/init.het"
#endif

#if !defined(HET_CPATH_DEFAULT)
#define HET_CPATH_DEFAULT                                                      \
  HET_CDIR "?.so;" HET_CDIR "loadall.so;"                                      \
           "./?.so"
#endif

#endif

/*
@@ HET_DIRSEP is the directory separator (for submodules)
*/
#if !defined(HET_DIRSEP)

#if defined(_WIN32)
#define HET_DIRSEP "\\"
#else
#define HET_DIRSEP "/"
#endif

#endif

/*
** HET_IGMARK mark to ignore all after it when building the module name
*/
#define HET_IGMARK "-"

/*
** Marks for exported symbols in the C code
*/

/*
@@ HET_API is a mark for all code API functions.
@@ HETLIB_API is a mark for all auxiliary library functions.
@@ HETMOD_API is a mark for all standard library opening functions.
** if you want to create one windows dll with the core and the libraries define
HET_BUILD_AS_DLL
*/
#if defined(HET_BUILD_AS_DLL)

#if defined(HET_CORE) || defined(HET_LIB)
#define HET_API __declspec(dllexport)
#else
#define HET_API __declspec(dllimport)
#endif

#else

#define HET_API extern

#endif

/*
** Most likely the libs go together with the core.
*/
#define HETLIB_API HET_API
#define HETMOD_API HET_API

/*
@@ HETI_FUNC is a mark for all extern functions that are not to be exported
outside modules.
@@ HETI_DDEF and HETI_DDEC are marks for all extern (const) variables, none of
which to be exported
** to outside modules (HETI_DDEF for definitions and HETI_DDEC for
declarations).
** Elf/gcc 3.2 >, mark them as hidden to optimize access when Hetalion is
compiled as a shared library. Not all elf
** targets support this attribute. Unfortunately, gcc does not offer a way to
check whether the
** target offers that support, and those without support give a warning about
it. to avoid these warning, change the default definition.
*/
#if defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 302) &&         \
    defined(__ELF__)
#define HETI_FUNC __attribute__((visibility("internal"))) extern
#else
#define HETI_FUNC extern
#endif

#define HETI_DDEC(dec) HETI_FUNC dec
#define HETI_DDEF /* empty */

/*
@@ HET_COMPAT_5_3 controls other macros for compatibility with Het 5.3.
** You can define it to get all options, or change specific options
** to fit your specific needs.
*/
#if defined(HET_COMPAT_5_3)

/*
@@ HET_COMPAT_MATHLIB controls the presence of several deprecated
** functions in the mathematical library.
** (officially removed in 5.3)
*/
#define HET_COMPAT_MATHLIB

/*
@@ HET_COMPAT_APIINTCASTS controls the presence of macros for
** manipulating other integer types
*/
#define HET_COMPAT_APIINTCASTS

/*
@@ HET_COMPAT_LT_LE controls the emulation of the `__le` metamethod using `__lt`
*/
#define HET_COMPAT_LT_LE

/*
@@ The following macros supply trivial compatibility for some
** changes in the API. The macros themselves document how to
** change your code to avoid using them.
*/
#define het_strlen(L, i) het_rawlen(L, (i))

#define het_objlen(L, i) het_rawlen(L, (i))

#define het_equal(L, idx1, idx2) het_compare(L, (idx1), (idx2), HET_OPEQ)
#define het_lessthan(L, idx1, idx2) het_compare(L, (idx1), (idx2), HET_OPLT)
#endif

/*
@@ HETI_UACNUMBER is the result of a default argument promotion over a floating
number.
@@ h_floatatt(x) corrects float attribute x to the proper float type by
prefixing it with one of FLT, DBL, LDBL
@@ HET_NUMBER_FRMLEN is the length modifier for writing floats.
@@ HET_NUMBER_FMT is the format for writing floats.
@@ het_number2str converts a float to a string.
@@ h_mathop allows the additional of an l or f to all math operations
@@ h_floor takes the floor of a float.
@@ het_str2number converts a decimal numeral to a number.
*/

#define h_floor(x) (h_mathop(floor)(x))

#define het_number2tr(s, sz, n)                                                \
  h_sprintf((s), sz, HET_NUMBER_FMT, (HETI_UACNUMBER)(n))

/*
@@ het_numbertointeger converts a float number with an integral value to an
integer.
*/
#define het_numbertointeger(n, p)                                              \
  ((n) >= (HET_NUMBER)(HET_MININTEGER) &&                                      \
   (n) < -(HET_NUMBER)(HET_MININTEGER) && (*(p) = (HET_INTEGER)(n), 1))

#if HET_FLOAT_TYPE == HET_FLOAT_FLOAT

#define HET_NUMBER float

#define h_floatatt(n) (FLT_##n)

#define HETI_UACNUMBER double

#define HET_NUMBER_FRMLEN ""
#define HET_NUMBER_FMT "%.7g"

#define h_mathop(op) op##f

#define het_str2number(s, p) strtof((s), (p))

#elif HET_FLOAT_TYPE == HET_FLOAT_LONGDOUBLE

#define HET_NUMBER long double

#define h_floatatt(n) (LDBL_##n)

#define HETI_UACNUMBER long double

#define HET_NUMBER_FRMLEN "L"
#define HET_NUMBER_FMT "%.19Lg"

#define h_mathop(op) op##l

#define het_str2number(s, p) strtold((s), (p))

#elif HET_FLOAT_TYPE == HET_FLOAT_DOUBLE

#define HET_NUMBER double

#define h_floatatt (DBL_##n)

#define HETI_UACNUMBER double

#define HET_NUMBER_FRMLEN ""
#define HET_NUMBER_FMT "%.14g"

#define h_mathop(op) op

#define het_str2number(s, p) strtod((s), (p))

#else

#error "numeric float type not defined

#endif

/*
@@ HET_UNSIGNED is the usigned version of HET_INTEGER
@@ HETI_UACINT is teh result of a default argument promotion
@@ over a HET_INTEGER.
@@ HET_INTEGER_FRMLEN is the length modifier for reading/writing integers.
@@ HET_INTEGER_FMT    is the format for writing integers.
@@ HET_MAXINTEGER     is the maximum value for a HET_INTEGER.
@@ HET_MININTEGER     is the minimum value for a HET_INTEGER.
@@ HET_MAXUNSIGNED    is the maximum value for a HET_UNSIGNED.
@@ het_integer2str    converts an integer to a string.
*/

#define HET_INTEGER_FMT "%" HET_INTEGER_FRMLEN "d"

#define HETI_UACINT HET_INTEGER

#define het_integer2str(s, sz, n)                                              \
  h_sprintf((s), sz, HET_INTEGER_FMT, (HETI_UACINT)(n))

/*
** use HETI_UACINT here to avoid problems  with promotions
*/
#define HET_UNSIGNED unsigned HETI_UACINT

#if HET_INT_TYPE == HET_INT_INT

#define HET_INTEGER int
#define HET_INTEGER_FRMLEN ""

#define HET_MAXINTEGER INT_MAX
#define HET_MININTEGER INT_MIN

#define HET_MAXUNSIGNED UINT_MAX

#elif HET_INT_TYPE == HET_INT_LONG

#define HET_INTEGER long
#define HET_INTEGER_FRMLEN "l"

#define HET_MAXINTEGER LONG_MAX
#define HET_MINTEGER LONG_MIN

#define HET_MAXUNSIGNED ULONG_MAX

#elif HET_INT_TYPE == HET_INT_LONGLONG

/* use presence of marco LLONG_MAX as proxy for C99 compliance */
#if defined(LLONG_MAX)

#define HET_INTEGER long long
#define HET_INTEGER_FRMLEN "ll"

#define HET_MAXINTEGER LLONG_MAX
#define HET_MININTEGER LLONG_MIN

#define HET_MAXUNSIGNED ULLONG_MAX

#elif defined(HET_USE_WINDOWS)

#define HET_INTEGER __int64
#define HET_INTEGER_FRMLEN "I64"

#define HET_MAXINTEGER _I64_MAX
#define HET_MININTEGER _I64_MIN

#define HET_MAXUNSIGNED _UI64_MAX

#else

#error "Compiler does not support 'long long'. Use option '-DHET_32BITS' \
  or '-DHET_C89_NUMBERS' (see file 'het_conf.h' for details)"

#endif

#else

#error "numeric integer type not defined"

#endif

/*
** Dependencies with C99 and other C details
*/

/*
@@ h_sprintf is equivalent to snprintf or sprintf in C89.
*/
#if !defined(HET_USE_C89)
#define h_sprintf(s, sz, f, i) snprintf(s, sz, f, i)
#else
#define h_sprintf(s, sz, f, i) ((void)(sz), sprintf(s, f, i))
#endif

/*
@@ het_strx2number converts a hexadecimal numberal to a number.
** In C99, strtod does that conversion. Otherwise, you can
** leave het_strx2number undefined and het will provide its own implementation.
*/
#if !defined(HET_USE_C89)
#define het_strx2number(s, p) het_str2number(s, p)
#endif

/*
@@ het_pointer2str converts a pointer to a readable string in a non-specified
way.
*/
#define het_pointer2str(buff, sz, p) h_sprintf(buff, sz, "%p", p)

/*
@@ het_number2strx converts a float to a hexadecimal numeral.
** In C99, sprintf does that. Otherwise you can leave het_number2strx undefined
and
** het will provide its own implementation.
*/
#if !defined(HET_USE_C89)
#define het_number2strx(L, b, sz, f, n)                                        \
  ((void)L, h_sprintf(b, sz, f, (HETI_UACNUMBER)(n)))
#endif

/*
** strof and opf variants for math functions are not valid in C89.
** Otherwise, the macro HUGE_VALF is a good proxy for testing the
** availability of these variants.
*/
#if defined(HET_USE_C89) || (defined(HUGE_VAL) && !defined(HUGE_VALF))
#undef h_mathop
#undef het_str2number
#define h_mathop(op) (het_Number) op
#define het_str2number(s, p) ((het_Number)strtod((s), (p)))
#endif

/*
@@ HET_KCONTEXT is the type of the context for continuation functionts.
** It must be a numerical type; Het will use intptr_t if available, otherwise it
will use
** ptrdiff_t
*/
#define HET_KCONTEXT ptrdiff_t

#if !defined(HET_USE_C89) && defined(__STDC_VERSION__) &&                      \
    __STDC_VERSION__ >= 199901L
#include <stdint.h>
#if defined(INTPTR_MAX)
#undef HET_KCONTEXT
#define HET_KCONTEXT intptr_t
#endif
#endif

/*
@@ het_getlocaledecpoint gets the locale radix character.
*/
#if !defined (het_getlocaledecpoint)
#define het_getlocaledecpoint() (localeconv()->decimal_point[0])
#endif
/*
** macros to improve jump prediction, used mostly for error handling and debug facilities.
*/
#if !defined (heti_likely)

#if defined(__GNUC__) && !defined (HET_NOBUILTIN)
#define heti_likely(x) (__builtin_expect(((x) != 0), 1))
#define heti_unlikely(x) (__builtin_expect(((x) != 0), 0))
#else
#define heti_likely(x) (x)
#define heti_unlikely(x) (x)
#endif

#endif

#if defined (HET_CORE) || defined (HET_LIB)
#define h_likely(x) heti_likely(x)
#define h_unlikely(x) heti_unlikely(x)
#endif

/*
** Language Variations
*/

/*
@@ HET_NOCVTN2S/HETNOCVTS2N control how Het performs some
** coercions. define to turn off autoamtic coercion fomr strings to numbers.
*/
/* #define HET_NOCVTN2S */
/* #define HET_NOCVTS2N */

/*
@@ HET_USE_APICHECK turns on several consistency checks on the C API.
*/
#if defined(HET_USE_APICHECK)
#include <assert.h>
#define heti_apicheck(l,e)  assert(e)
#endif

/*
@@ HETI_MAXSTACK limits the size of the Het stack.
*/
#if HETI_IS32INT
#define HETI_MAXSTACK 1000000
#else
#define HETI_MAXSTACK 15000
#endif

/*
@@ HET_EXTRASPACE defines the size of raw memory area associated with a Het state with very fast access.
*/
#define HET_EXTRASPACE (sizeof(void *))

/*
@@ HET_IDSIZE gives the maximum size for the description of the source of a function in the debug information.
*/
#define HET_IDSIZE 60

/*
@@ HETI_BUFFERSIZE is the initial buffer size
*/
#define HETL_BUFFERSIZE ((int)(16 * sizeof(void*) * sizeof(het_Number)))

/*
@@ HETI_MAXALIGN defines fields that, when used in union,
** ensure maximum alignments for the other items in that union.
*/
#define HETI_MAXALIGN het_Number n; double u; void *s; het_Integer i; long l
/*
** use this for redefinitions without modying the main part of the file.
*/
#endif
#ifndef het_limits_h
#define het_limits_h

#include <limits.h>
#include <stddef.h>

#include "het.h"

/*
** `he_mem` and `h_mem` are unsigned/signed integers big enough to count
** the total memory used by Het (in bytes). Usually, `size_t` and
** `ptrdiff_t` should work, but we use `long` for 16-biit machines.
*/
#if defined(HETI_MEM)
typedef HETI_UMEM he_mem;
typedef HETI_MEM h_mem;
#elif HETI_IS32INT
typedef size_t he_mem;
typedef ptrdiff_t h_mem;
#else
typedef unsingned long he_mem;
typedef long h_mem;
#endif

/* chars used as small naturals (so that `char` is reserved for characters )*/
typedef unsigned char he_byte;
typedef signed char hs_byte;

/* maximum value for size_t */
#define MAX_SIZET ((size_t)(~(size_t)0))

/* maximum size visible for Het (must be representable in a `het_Integer`)*/
#define MAX_SIZE (sizeof(size_t) < sizeof(het_Integer) ? MAX_SIZET \
                                                       : (size_t)(HET_MAXINTEGER))

#define HET_HEMEM ((he_mem)(~(he_mem)0))

#define MAX_HMEM ((h_mem)(MAX_HMEM >> 1)

#define MAX_INT INT_MAX

/*
** floor of the log2 of the maximum signed value for integral type `t`.
** (That is, maximum `n` such that `2^n` fits in the given signed type.)
*/
#define log2maxs(t) (sizeof(t) * 8 - 2)

/*
** test whether an unsigned value is a power of 2 (or zero)
*/
#define ispow2(x) (((x) & ((x) - 1)) == 0)

/* number of chars of a literal string without the ending \0 */
#define LL(x) (sizeof(x) / sizeof(char) - 1)

/*
** conversion of pointer to unsigned integer: this is for hashing only;
** there is no problem if the integer cannot hold the whole pointer
** value. (In strict ISO C this may cause undefined behavior, but no actual machine
** seems to bother.)
*/
#if !defined(HET_USE_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#include <stdint.h>
#if defined(UINTPTR_MAX)
#define H_P2I uintptr_t /* even in C99 this type is optional */
#else                   /* no intptr? */
#define H_P2I uintmax_t /* use the largest available integer*/
#endif
#else /* C89 option */
#define H_P2I size_t
#endif

#define point2uint(p) ((unsigned int)((H_P2I)(p) & UINT_MAX))

/* types of usual argument conversions for `het_Number` and `het_Integer` */
typedef HETI_UACNUMBER h_uacNumber;
typedef HETI_UACINT h_uacInt;

/*
** Internal assertions for in_house debugging
*/
#if define HETI_ASSERT
#undef NDEBUG
#include <assert.h>>
#define het_assert(c) assert(c)
#endif

#if defined(het_assert)
#define check_exp(c, e) (het_assert(c), (e))
/* to avoid problems with conditions too long */
#define het_longassert(c) ((c) ? (void)0 : het_assert(0))
#else
#define het_assert(c) ((void)0)
#define check_exp(c, e) (e)
#define het_longassert(c) ((void)0)
#endif

/*
** assertion for checking API calls.
*/
#if !defined(heti_apicheck)
#define heti_apicheck(h, e) ((void)h, het_assert(e))
#endif

#define apicheck(h, e, msg) heti_apicheck(h, (e) && msg)

/* macro to avoid warnings about unused variables */
#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif

/* type casts (a macro highlights casts in the code) */
#define cast(t, exp) ((t)(exp))

#define cast_void(i) cast(void, (i))
#define cast_voidp(i) cast(void *, (i))
#define cast_num(i) cast(het_Number, (i))
#define cast_int(i) cast(int, (i))
#define cast_uint(i) cast(unsigned int, (i))
#define cast_byte(i) cast(he_byte, (i))
#define cast_uchar(i) cast(unsigned char, (i))
#define cast_char(i) cast(char, (i))
#define cast_charp(i) cast(char *, (i))
#define cast_sizet(i) cast(size_t, (i))

/* cast a signed het_Integer to a het_Unsigned */
#if !defined(h_castS2U)
#define h_castS2U(i) ((het_Unsigned)(i))
#endif

/*
** cast a het_Unsigned to a signed het_Integer;  this cast is
** not strict ISO C, but two-complement architectures should work fine.
*/
#if !defined(h_castU2S)
#define h_castU2S(i) ((het_Integer)(i))
#endif

/*
** non-return type
*/
#if !defined(h_noret)

#if defined(__GNUC__)
#define h_noret void __atribute__((noreturn))
#elif defined(_MSC_VER) && _MSC_VER >= 1200
#define h_noret void __declspec(noreturn)
#else
#define h_noret void
#endif

#endif

/*
** Inline functions
*/
#if !defined(HET_USE_C89)
#define h_inline inline
#define h_inline __inline__
#else
#define h_inline /* empty */
#endif

#define h_sinline static h_inline

/*
** type for vm instructions;
** must be an unsigned with at least 4 bytes (details on het_opcodes.h)
*/
#if HETI_IS32INT
typedef unsigned int h_uint32;
#else
typedef unsigned long h_uint32;
#endif

typedef h_uint32 Instruction;

/*
** Maximum length for short strings, that is, strings that are
** internalized. (Cannot be smaller than reserved words or tags for
** metamethods, as these strings must be internalized;
** #("function") = 8, #("__newindex") = 10.)
*/
#if !defined(HETI_MAXSHORTLEN)
#define HETI_MAXSHORTLEN 40
#endif

/*
** Initial size for the string table (must be power of 2).
** The Het core alone registers ~50 strings (reserved words +
** metaevent keys + a few others). Libraries would typically add
** a few dozen more.
*/
#if !defined(MINSTRTABSIZE)
#define MINSTRTABSIZE 128
#endif

/*
** Size of cache for strings in the API. N is the number of
** sets (better be a prime) and M is the size of each set (M == 1
** makes a direct cache.)
*/
#if !defined(STRCACHE_N)
#define STR_CACHE_N 53
#define STR_CACHE_M 2
#endif

/* minimum size for string buffer */
#if !defined(HET_MINBUFFER)
#define HET_MINBUFFER 32
#endif

/*
** Maximum depth for nested C calls, syntactical nested non-terminals,
** and other features implemented through recursion in C.
*/
#if !defined(HETI_MAXCCALLS)
#define HETI_MAXCCALLS 200
#endif

/*
** macros that are executed whenever program enters the Het core
** `het_lock` and leaves the code `het_unlock`
*/
#if !defined(het_lock)
#define het_lock(L) ((void)0)
#define het_unlock(L) ((void)0)
#endif

/*
** macro executed during Het functions at points where
** function can yield
*/
#if !defined(heti_threadyield)
#define heti_threadyield(L) \
    {                       \
        het_unlock(L);      \
        het_lock(L);        \
    }
#endif

/*
** these macros allow user-specific actions when a thread is
** created/deleted/resumed/yielded.
*/
#if !defined(heti_userstateopen)
#define heti_userstateopen(L) ((void)L)
#endif

#if !defined(heti_userstateclose)
#define heti_userstateclose(L) ((void)L)
#endif

#if !defined(heti_userstatethread)
#define heti_userstatethread(L, L1) ((void)L)
#endif

#if !defined(heti_userstatefree)
#define heti_userstatefree(L, L1) ((void)L)
#endif

#if !defined(heti_userstateresume)
#define heti_userstateresume(L, n) ((void)L)
#endif

#if !defined(heti_userstateyield)
#define heti_userstateyield(L, n) ((void)L)
#endif

/*
** The heti_num* macros define the primitive operations over numbers.
*/

/* floor division (defined as floor(a/b))*/
#if !defined(heti_numidiv)
#define heti_numidiv(L, a, b) ((void)L, h_floor(heti_numdiv(L, a, b)))
#endif

/* float division */
#if !defined(heti_numdiv)
#define heti_numdiv(L, a, b) ((a) / (b))
#endif

/*
** modulo: defines as 'a - floor(a/b)*b'; the direct computation
** using this definition has several problems with rounding errors,
** so it is better to use 'fmod'. 'fmod' gives the result of
** a - trunc(a/b)*b, and therefore must be corrected when
** 'trunc(a/b) ~= floor(a/b)'. That happens when the division has a
** non-integer negative result: non-integer result is equivalent to
** a non-zero remainder 'm'; negative result is equivalent to 'a' and
** 'b' with different signs, or 'm' and 'b' with different signs
** (as the result 'm' of 'fmod' has the same sign of 'a').
*/
#if !defined(heti_nummod)
#define heti_nummod(L, a, b, m)                         \
    {                                                   \
        (void)L;                                        \
        (m) = h_mathop(fmod)(a, b);                     \
        if (((m) > 0) ? (b) < 0 : ((m) < 0 && (b) > 0)) \
            (m) += (b);                                 \
    }
#endif

/* exponentiation */
#if !defined(heti_numpow)
#define heti_numpow(L, a, b) \
    ((void)L, (b == 2) ? (a) * (a) : h_mathop(pow)(a, b))
#endif

/* the others are quite standard opertions */
#if !defined(heti_numadd)
#define heti_numadd(L, a, b) ((a) + (b))
#define heti_numsub(L, a, b) ((a) - (b))
#define heti_nummul(L, a, b) ((a) * (b))
#define heti_numunm(L, a, b) (-(a))
#define heti_numeq(L, a, b) ((a) == (b))
#define heti_numlt(L, a, b) ((a) < (b))
#define heti_numle(L, a, b) ((a) <= (b))
#define heti_numgt(L, a, b) ((a) > (b))
#define heti_numge(L, a, b) ((a) >= (b))
#define heti_numisnan(L, a, b) (!heti_numeq((a), (a)))
#endif

/*
** macro to control inclusion of some ahrd tests on stack reallocation
*/
#if !defined(HARDSTACKTESTS)
#define condmovestack(L, pre, pos) ((void)0)
#else
/* realloc stack keeping its size */
#define condmovestack(L, pre, pos)      \
    {                                   \
        int sz_ = stacksize(L);         \
        pre;                            \
        hetD_reallocstack((L), sz_, 0); \
        pos;                            \
    }
#endif

#if !defined(HARDMEMTESTS)
#define condchangemem(L, pre, pos) ((void)0)
#else
#define condchangemem(L, pre, pos) \
    {                              \
        if (gcrunning(G(L)))       \
        {                          \
            pre;                   \
            hetC_fullgc(L, 0);     \
            pos;                   \
        }                          \
    }
#endif
#endif
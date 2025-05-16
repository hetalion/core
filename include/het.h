/*
** Het - the Hetalion bytecode.
*/

#ifndef het_h
#define het_h

#include <stdarg.h>
#include <stddef.h>

#include "./het_conf.h"

#define HET_VERSION_MAJOR "0"
#define HET_VERSION_MINOR "1"
#define HET_VERSION_RELEASE "1"

#define HET_VERSION_NUM 1
#define HET_VERSION_RELEASE_NUM (HET_VERSION_NUM * 100 + 7)

#define HET_VERSION "Het " HET_VERSION_MAJOR "." HET_VERSION_MINOR
#define HET_RELEASE HET_VERSION "." HET_VERSION_RELEASE
#define HET_COPYRIGHT HET_RELEASE " Copyright (C) 2025"
#define HET_AUTHORS "J. Henrique Noronha"

/* mark for precompiled code ('<esc>Het') */
#define HET_SIGNATURE "\x1bHet"

/* option for multiple returns in 'het_pcall' and 'het_call' */
#define HET_MULTRET (-1)

/*
** Pseudo-indexes
** (-HETI_MAXSTACK is the minimum valid index; we keep some free empty
** space after that to help overflow detection)
*/
#define HET_REGISTRYINDEX (-HETI_MAXSTACK - 1000)
#define het_upvalueindex(i) (HET_REGISTRYINDEX - (i))

/* thread status */
#define HET_OK 0
#define HET_YIELD 1
#define HET_ERRRUN 2
#define HET_ERRSYNTAX 3
#define HET_ERRMEM 4
#define HET_ERRERR 5

typedef struct het_State het_State;

/*
** basic types
*/
#define HET_TNONE (-1)

#define HET_TNIL 0
#define HET_TBOOLEAN 1
#define HET_TLIGHTUSERDATA 2
#define HET_TNUMBER 3
#define HET_TSTRING 4
#define HET_TTABLE 5
#define HET_TFUNCTION 6
#define HET_TUSERDATA 7
#define HET_TTHREAD 8

#define HET_NUMTYPES 9

/* minimum Het stack available to a C function */
#define HET_MINSTACK 20

/* predefined values in the registry */
#define HET_RIDX_MAINTHREAD 1
#define HET_RIDX_GLOBALS 2
#define HET_RIDX_LAST HET_RIDX_GLOBALS

/* type of numbers in Het */
typedef HET_NUMBER het_Number;

typedef HET_INTEGER het_Integer;

typedef HET_UNSIGNED het_Unsigned;

/* typedef for continuation-function contexts */
typedef HET_KCONTEXT het_KContext;

/*
** Type for C functions registered with Het
*/
typedef int (*het_CFunction)(het_State *L);

/*
** Type for continuation functions
*/
typedef int (*het_KFunction)(het_State *L, int status, het_KContext ctx);

/*
** Type for functions that read/write blocks when loading/dumping Het-chunks
*/
typedef const char *(*het_Reader)(het_State *L, void *ud, size_t *sz);

typedef int (*het_Writer)(het_State *L, const void *p, size_t sz, void *ud);

/*
** Type for memory-allocation functions
*/
typedef void *(*het_Alloc)(void *ud, void *ptr, size_t osize, size_t nsize);

/*
** Type for warning functions
*/
typedef void (*het_WarnFunctions)(void *ud, const char *msg, int tocont);

/*
** Type used by the debug API to collect debug information
*/
typedef struct het_Debug het_Debug;

/*
** Functions to be called by the debugger in specific events
*/
typedef void (*het_Hook)(het_State *L, het_Debug *ar);

/*
** generic extra include file
*/
#if defined(HET_USER_H)
#include HET_USER_H
#endif

/*
** RCS ident string
*/
extern const char het_ident[];

/*
** state manipulation
*/
HET_API het_State *(het_newstate)(het_Alloc f, void *ud);
HET_API void(het_close)(het_State *L);
HET_API het_State *(het_newthread)(het_State * L);
HET_API int(het_closethread)(het_State *L, het_State *from);
HET_API int(het_resetthread)(het_State *L); /* Deprecated */

HET_API het_CFunction(het_atpanic)(het_State *L, het_CFunction panicf);

HET_API het_Number(het_version)(het_State *L);

/*
** basic stack manipulation
*/
HET_API int(het_absindex)(het_State *L, int idx);
HET_API int(het_gettop)(het_State *L);
HET_API void(het_settop)(het_State *L, int idx);
HET_API void(het_pushvalue)(het_State *L, int idx);
HET_API void(het_rotate)(het_State *L, int idx, int n);
HET_API void(het_copy)(het_State *L, int fromidx, int toidx);
HET_API int(het_checkstack)(het_State *L, int n);

HET_API void(het_xmove)(het_State *from, het_State *to, int n);

/*
** access functions (stack -> C)
*/

HET_API int(het_isnumber)(het_State *L, int idx);
HET_API int(het_isstring)(het_State *L, int idx);
HET_API int(het_iscfunction)(het_State *L, int idx);
HET_API int(het_isinteger)(het_State *L, int idx);
HET_API int(het_isuserdata)(het_State *L, int idx);
HET_API int(het_type)(het_State *L, int idx);
HET_API const char *(het_typename)(het_State * L, int tp);

HET_API het_Number(het_tonumberx)(het_State *L, int idx, int *isnum);
HET_API het_Integer(het_tointegerx)(het_State *L, int idx, int *isnum);
HET_API int(het_toboolean)(het_State, int idx);
HET_API const char *(het_tolstring)(het_State * L, int idx, size_t *len);
HET_API het_Unsigned(het_rawlen)(het_State *L, int idx);
HET_API het_CFunction(het_tocfunction)(het_State *L, int idx);
HET_API void *(het_touserdata)(het_State * L, int idx);
HET_API het_State *(het_tothread)(het_State * L, int idx);
HET_API const void *(het_topointer)(het_State * L, int idx);

/*
** Comparison and arithmetic functions
*/

#define HET_OPADD 0
#define HET_OPSUB 1
#define HET_OPMUL 2
#define HET_OPMOD 3
#define HET_OPPOW 4
#define HET_OPDIV 5
#define HET_OPIDIV 6
#define HET_OPBAND 7
#define HET_OPBOR 8
#define HET_OPBXOR 9
#define HET_OPSHL 10
#define HET_OPSHR 11
#define HET_OPUNM 12
#define HET_OPBNOT 13

HET_API void(het_arith)(het_State *L, int op);

#define HET_OPEQ 0
#define HET_OPLT 1
#define HET_OPLE 2

HET_API int(het_rawequal)(het_State *L, int idx1, int idx2);
HET_API int(het_compare)(het_State *L, int idx1, int idx2, int op);

/*
** push functions (C -> stack)
*/
HET_API void(het_pushnil)(het_State *L);
HET_API void(het_pushnumber)(het_State *L, het_Number n);
HET_API void(het_pushinteger)(het_State *L, het_Integer n);
HET_API const char *(het_pushlstring)(het_State * L, const char *s, size_t len);
HET_API const char *(het_pushstring)(het_State * L, const char *s);
HET_API const char *(het_pushvfstring)(het_State * L, const char *fmt, va_list argp);
HET_API const char *(het_pushfstring)(het_State *L, const char *fmt, ...);
HET_API void (het_pushcclosure) (het_State *L, het_CFunction fn, int n);
HET_API void (het_pushboolean) (het_State *L, int b);
HET_API void (het_pushlightuserdata) (het_State *L, void *p);
HET_API int (het_pushthread) (het_State *L);
#endif
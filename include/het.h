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
HET_API const char *(het_pushfstring)(het_State * L, const char *fmt, ...);
HET_API void(het_pushcclosure)(het_State *L, het_CFunction fn, int n);
HET_API void(het_pushboolean)(het_State *L, int b);
HET_API void(het_pushlightuserdata)(het_State *L, void *p);
HET_API int(het_pushthread)(het_State *L);

/*
** get functions (Het -> stack)
*/
HET_API int(het_getglobal)(het_State *L, const char *name);
HET_API int(het_gettable)(het_State *L, int idx);
HET_API int(het_getfield)(het_State *L, int idx, const char *k);
HET_API int(het_geti)(het_State *L, int idx, het_Integer n);
HET_API int(het_rawget)(het_State *L, int idx);
HET_API int(het_rawgeti)(het_State *L, int idx, het_Integer n);
HET_API int(het_tawgetp)(het_State *L, int idx, const void *p);

HET_API void(het_createtable)(het_State *L, int narr, int nrec);
HET_API void *(het_newuserdatauv)(het_State * L, size_t sz, int nuvalue);
HET_API int(het_getmetatable)(het_State *L, int objindex);
HET_API int(het_getiuservalue)(het_State *l, int idx, int n);

/*
** set functions (stack -> Het)
*/
HET_API void(het_setglobal)(het_State *L, const char *name);
HET_API void(het_settable)(het_State *L, int idx);
HET_API void(het_setfield)(het_State *L, int idx, const char *k);
HET_API void(het_seti)(het_State *L, int idx, het_Integer n);
HET_API void(het_rawset)(het_State *L, int idx);
HET_API void(het_rawseti)(het_State *L, int idx, het_Integer n);
HET_API void(het_rawsetp)(het_State *L, int idx, const void *p);
HET_API int(het_setmetatable)(het_State *L, int objindex);
HET_API int(het_setiuservalue)(het_State *L, int idx, int n);

/*
** load and call functions (load and run Het code)
*/
HET_API void(het_callk)(het_State *L, int nargs, int nresults,
                        het_KContext ctx, het_KFunction k);
#define het_call(L, n, r) het_callk(L, (n), (r), 0, NULL)

HET_API int(het_pcallk)(het_State *L, int nargs, int nresults, int errfunc,
                        het_KContext ctx, het_KFunction k);
#define het_pcall(L, n, r, f) het_pcall(L, (n), (r), (f), 0, NULL)

HET_API int(het_load)(het_State *L, het_Reader reader, void *dt,
                      const char *chunkname, const char *mode);

HET_API int(het_dump)(het_State *L, het_Writer writer, void *data, int strip);

/*
** coroutine functions
*/
HET_API int(het_yieldk)(het_State *L, int nresults, het_KContext ctx,
                        het_KFunction k);
HET_API int(het_resume)(het_State *L, het_State *from, int narg,
                        int *nres);
HET_API int(het_status)(het_State *L);
HET_API int(het_isyieldable)(het_State *L);

#define het_yield(L, n) het_yielk(L, (n), 0, NULL)

/*
** Warning-related functions
*/
HET_API void(het_setwarnf)(het_State *L, het_WarnFunctions f, void *ud);
HET_API void(het_warning)(het_State *L, const char *msg, int tocont);

/*
** garbage-collection function and options
*/
#define HET_GCSTOP 1
#define HET_GCRESTART 2
#define HET_GCCOLLECT 3
#define HET_GCCOUNT 4
#define HET_GCCOUNTB 5
#define HET_GCSTEP 6
#define HET_GCSETPAUSE 7
#define HET_GCSETSTEPMUL 8
#define HET_GCISRUNNING 9
#define HET_GCGEN 10
#define HET_GCINC 11

HET_API int(het_gc)(het_State *L, int what, ...);

/*
** miscellaneous functions
*/

HET_API int(het_error)(het_State *L);

HET_API int(het_next)(het_State *L, int idx);

HET_API void(het_concat)(het_State *L, int n);
HET_API void(het_len)(het_State *L, int idx);

HET_API size_t(het_stringtonumber)(het_State *L, const char *s);

HET_API het_Alloc(het_getallocf)(het_State *L, void **ud);
HET_API void(het_setallocf)(het_State *L, het_Alloc *f, void *ud);

HET_API void(het_toclose)(het_State *L, int idx);
HET_API void(het_closeslot)(het_State *L, int idx);

/*
 * useful macros
 */

#define het_getextraspace(L) ((void *)((char *)(L) - HET_EXTRASPACE))

#define het_tonumber(L, i) het_tonumberx(L, (i), NULL)
#define het_tointeger(L, i) het_tointeger(L, (i), NULL)

#define het_pop(L, n) het_settop(L, -(n) - 1)

#define het_newtable(L) het_createtable(L, 0, 0)

#define het_register(L, n, f) (het_pushcfunction(L, (f)), het_setglobal(L, (n)))

#define het_pushcfunction(L, f) het_pushcclosure(L, (f), 0)

#define het_isfunction(L, n) (het_type(L, (n)) == HET_TFUNCTION)
#define het_istable(L, n) (het_type(L, (n)) == HET_TTABLE)
#define het_islightuserdata(L, n) (het_type(L, (n)) == HET_TLIGHTUSERDATA)
#define het_isnil(L, n) (het_type(L, (n)) == HET_TNIL)
#define het_isboolean(L, n) (het_type(L, (n)) == HET_TBOOLEAN)
#define het_isthread(L, n) (het_type(L, (n)) == HET_TTHREAD)
#define het_isnone(L, n) (het_type(L, (n)) == HET_TNONE)
#define het_isnoneornil(L, n) (het_type(L, (n)) <= 0)

#define het_pushliteral(L, s) het_pushstring(L, "" s)

#define het_pushglobaltable(L) \
    ((void)het_rawgeti(L, HET_REGISTRYINDEX, HET_RIDX_GLOBALS))

#define het_tostring(L, i) het_tolstring(L, (i), NULL)
#define het_insert(L, idx) het_rotate(L, (idx), 1)
#define het_remove(L, idx) (het_rotate(L, (idx), -1), het_pop(L, 1))
#define het_replace(L, idx) (het_copy(L, -1, (idx)), het_pop(L, 1))

/*
 * compatibility macros
 */
#if defined(HET_COMPAT_APIINTCASTS)

#define het_pushunsigned(L, n) het_pushinteger(L, (het_Integer)(n))
#define het_tounsignedx(L, i, is) ((het_Unsigned)het_tointegerx(L, i, is))
#define het_tounsigned(L, i) het_tounsignedx(L, (i), NULL)

#endif

#define het_newuserdata(L, s) het_newuserdatauv(L, s, 1)
#define het_getuservalue(L, idx) het_getiuservalue(L, idx, 1)
#define het_setuservalue(L, idx) het_setiuservalue(L, idx, 1)

#define HET_NUMTAGS HET_NUMTYPES

/*
** Debug API
*/

/*
** Event codes
*/
#define HET_HOOKCALL 0
#define HET_HOOKRET 1
#define HET_HOOKLINE 2
#define HET_HOOKCOUNT 3
#define HET_HOOKTAILCALL 4

/*
** Event masks
*/
#define HET_MASKCALL (1 << HET_HOOKCALL)
#define HET_MASKRET (1 << HET_HOOKRET)
#define HET_MASKLINE (1 << HET_HOOKLINE)
#define HET_MASKCOUNT (1 << HET_HOOKCOUNT)

HET_API int(het_getstack)(het_State *L, int level, het_Debug *ar);
HET_API int(het_getinfo)(het_State *L, const char *what, het_Debug *ar);
HET_API const char *(het_getlocal)(het_State * L, const het_Debug *ar, int n);
HET_API const char *(het_setlocal)(het_State * L, const het_Debug *ar, int n);
HET_API const char *(het_getupvalue)(het_State * L, int funcindex, int n);
HET_API const char *(het_setupvalue)(het_State * L, int funcindex, int n);

HET_API void *(het_upvalueid)(het_State * L, int fidx, int n);
HET_API void(het_upvaluejoin)(het_State *L, int fidx1, int n1, int fidx2, int n2);

HET_API void (het_sethook) (het_State *L, het_Hook func, int mask, int count);
HET_API het_Hook (het_gethook) (het_State *L);
HET_API int (het_gethookmask) (het_State *L);
HET_API int (het_gethookcount) (het_State *L);

HET_API int (het_setcstacklimit) (het_State *L, unsigned int limit);

struct het_Debug {
    int event;
    const char *name; /* (n) */
    const char *namewhat; /* (n) 'global', 'local', 'field', 'method' */
    const char *what; /* (S) 'Het', 'C', 'main', 'tail' */
    const char *source; /* (S)*/
    size_t srclen; /* (S) */
    int currentline; /* (l) */
    int linedefined; /* (S) */
    int lastlinedefined; /* (S) */
    unsigned char nups; /* (u) */
    unsigned char nparams; /* (u) */
    char isvararg; /* (u) */
    char istailcall; /* (t) */
    unsigned short ftransfer; /* (r) index of first value transferred */
    unsigned short ntransfer; /* (r) number of transferred values */
    char short_src[HET_IDSIZE]; /* (S) */
    /* private part */
    struct CallInfo *i_ci; /* active function */
};

#endif

/******************************************************************************
 * Adapted for HETALION by J. Henrique Noronha
 *
 * Copyright (C) 1994-2024 Lua.org, PUC-Rio.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

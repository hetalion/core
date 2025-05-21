#ifndef het_object_h
#define het_object_h

#include <stdarg.h>

#include "het_limits.h"
#include "het.h"

/*
 * Extra types for collectable non-values
 */
#define HET_TUPVALUE HET_NUMTYPES /* upvalues */
#define HET_TPROTO (HET_NUMTYPES+1) /* function prototypes */
#define HET_TDEADKEY (HET_NUMTYPES+2) /* removed keys in tables */

/*
 * number of all possible types (including HET_TNONE but excluding DEADKEY)
 */
#define HET_TOTALTYPES (HET_TPROTO + 2)

/*
 *
 * tags for Tagged Values have the following use of bits:
 * bits 0-3: actual tag (a LUA_T* constant)
 * bits 4-5: variant bits
 * bit 6: whether value is collectable
 */

/* add variant bits to a type */
#define makevariant(t,v) ((t) | ((v) << 4))

/*
 * Union on all Het values
 */
typedef union Value {
    struct GCObject *gc; /* collectable objects */
    void *p; /* light userdata */
    het_CFunction f; /* light C functions */
    het_Integer i; /* integer numbers */
    het_Number n; /* float numbers */
    /* not used, but may avoid warnings for uninitialized value */
    he_byte ub;
} Value;

/*
 * Tagged Values. This is a basic representation of values in Het:
 * an actual value plus a tag with its type.
 */
#define TValueFields Value value_; he_byte tt_

typedef struct TValue {
    TValueFields;
} TValue;

#define val_(o) ((o)->value_)
#define valraw(o) (val_(o))

/* raw type tag of a TValue */
#define rawtt(o) ((o)->tt_)

/* tag with no variants (bits 0-3) */
#define novariant(t) ((t) & 0x0F)

/* type tag of a TValue (bits 0-3 for tags + variant bits 4-5 */
#define withvariant(t) ((t) & 0x3F)
#define ttypetag(o) withvariant(rawtt(o))

/* type of a TValue */
#define ttype(o) (novariant(rawtt(o)))

/* Macros to test type */
#define checktag(o,t) (rawtt(o) == (t))
#define checktype(o,t) (ttype(o) == (t))

/* Macros for internal tests */

/* a collectable object has the same tag as the original value */
#define righttt(obj) (ttypetag(obj) == gcvalue(obj)->tt)

/*
 * Any value being manipulated by the program either is non
 * collectable, or the collectable object has the right tag
 * and it is not dead. The option 'L == NULL' allows other
 * macros using this one to be used where L is not available.
 */
#define checkliveness(L, obj) \
    ((void)L, het_longassert(!iscollectable(obj) || \
            (righttt(obj) && (L == NULL || !isdead(G(L),gcvalue(obj)))))

/* Macros to set values */

/* set a value's tag */
#define settt_(o,t) ((o)->tt_=(t))

/* main macro to copy values (from 'obj2' to 'obj1') */
#define setobj(L,obj1,obj2) \
    { TValue *io1 = (obj1); const TValue *io2 = (obj2); \
      io1->value_ = io2->value_; settt_(io1, io2->tt_); \
      checkliveness(L,io1); het_assert(!isnonstrictnil(io1)); }

/*
 * Different types of assignments, according to source and destination.
 * (They are mostly equal now but may be different in the future.)
 */

/* from stack to stack */
#define setobjs2s(L,o1,o2) setobj(L,s2v(o1),s2v(o2))
/* to stack (not from same stack) */
#define setobj2s(L,o1,o2) setobj(L,s2v(o1),o2)
/* from table to same table */
#define setobjt2t setobj
/* to new object */
#define setobj2n setobj
/* to table */
#define setobj2t setobj

/*
 * Entries in a Het stack. The field `tbclist` forms a list of all
 * to-be-closed variables active in this stack. Dummy entries are
 * used when the distance between two tbc variables does not fit
 * in an unsigned short. They are represented by delta==0, and
 * their real delta is always the maximum value that fits in
 * that field.
 */
typedef union StackValue {
    TValue val;
    struct {
        TValueFields;
        unsigned short delta;
    } tbclist;
} StackValue;

/* index to stack elements */
typedef StackValue *StkId;

/*
 * When reallocating the stack, change all pointers to the stack into proper offsets.
 */
typedef union {
    StkId p; /* actual pointer */
    ptrdiff_t offset; /* used while the stack is being reallocated */
} StkIdRel;

/* convert a StackValue to a TValue */
#define s2v(o) (&(o)->val)

/*
 * Nil
 */

/* Standard nil */
#define HET_VNIL makevariant(HET_TNIL, 0)

/* Empty slot (which might be different from a slot containing nil) */
#define HET_VEMPTY makevariant(HET_TNIL, 1)

/* `Value` returned for a key not found in a table (absent key) */
#define HET_VABSTKEY makevariant(HET_TNIL, 2)

/* macro to test for (any kind of) nil */
#define ttisnil(v) checktype((v), HET_TNIL)

/* macro to test for a standard nil */
#define ttisstrictnil(o) checktag((o), HET_VNIL)

#define setnilvalue(obj) settt_(obj, HET_VNIL)

#define isabstkey(v) checktag((v), HET_VABSTKEY)

/*
 * macro to detect non-standard nils (used only in assertions)
 */
#define isnonstrictnil(v) (ttisnil(v) && !ttisstrictnil(v))

/*
 * By default, entries with any kind of nil are considered empty.
 * (In any definition, values associated with absent keys must also
 * be accepted as empty.)
 */
#define isempty(v) ttisnil(v)

/* macro defining a value corresponding to an absent key */
#define ABSTKEYCONSTANT {NULL},HET_VABSTKEY

/* mark an entry as empty */
#define setempty(v) settt_(v, HET_VEMPTY)

/*
 * Booleans
 */

#define HET_VFALSE makevariant(HET_TBOOLEAN, 0)
#define HET_VTRUE makevariant(HET_TBOOLEAN, 1)

#define ttisboolean(o) checktype((o), HET_TBOOLEAN)
#define ttisfalse(o) checktag((o), HET_VFALSE)
#define ttistrue(o) checktag((o), HET_VTRUE)

#define h_isfalse(o) (ttisfalse(o) || ttisnil(o))

#define setbfvalue(obj) settt_(obj, HET_VFALSE)
#define setbtvalue(obj) settt_(obj, HET_VTRUE)

/*
 *  Threads
 */
#define HET_VTHREAD makevariant(HET_TTHREAD, 0)

#define ttisthread(o) checktype((o), HET_TTHREAD)

#define thvalue(o) check_exp(ttisthread(o), gco2th(val_(o).gc))

#define setthvalue(L,obj,x) \
    { TValue *io = (obj); het_State *x_ = (x); \
      val_(io).gc = obj2gco(x_); settt_(io, ctb(HET_VTHREAD)); \
      checkliveness(L,io); }

#define setthvalue2s(L,o,t) setthvalue(L,s2v(o),t)

/*
 * Collectable objects
 */

/*
 * Common Header for all collectable objects (in macro form, to be
 * included in other objects)
 */
#define CommonHeader struct GCObject *next; he_byte tt; he_byte marked;

/*
 * Common type for all collectable objects
 */
typedef struct GCObject {
    CommonHeader;
} GCObject;

/* Bit mark for collectable types */
#define BIT_ISCOLLECTABLE (1 << 6)

#define iscollectable(o) (rawtt(o) & BIT_ISCOLLECTABLE)

/* mark a tag as collectable */
#define ctb(t) ((t) | BIT_ISCOLLECTABLE)

#define gcvalue(o) (check_exp(iscollectable(o), val_(o).gc)

#define gcvalueraw(v) ((v).gc)

#define setgcovalue(L,obj,x) \
    { TValue *io = (obj); GCObject *i_g=(x); \
      val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }

/*
 * Numbers
 */

/* Variant tags for numbers */
#define HET_VNUMINT makevariant(HET_TNUMBER, 0) /* integer numbers */
#define HET_VNUMFLT makevariant(HET_TNUMBER, 1) /* float numbers */

#define ttisnumber(o) checktype((o), HET_TNUMBER)
#define ttisinteger(o) checktag((o), HET_VNUMINT)
#define ttisfloat(o) checktag((o), HET_VNUMFLT)

#define nvalue(o) check_exp(ttisnumber(o), \
        (ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define fltvalue(o) check_exp(ttisfloat(o), val_(o).n)
#define ivalue(o) check_exp(ttisinteger(o), val_(o).i)

#define fltvalueraw(v) ((v).n)
#define ivalueraw(v) ((v).i)

#define setfltvalue(obj,x) \
    { TValue *io=(obj); val_(io).n=(x); settt_(io, HET_VNUMFLT); }

#define chgfltvalue(obj,x) \
    { TValue *io=(obj); het_assert(ttisfloat(io)); val_(io).n =(x); }

#define setivalue(obj,x) \
{ TValue *io=(obj); val_(io).i=(x); settt_(io, HET_VNUMINT); }

#define chgivalue(obj,x) \
{ TValue *io=(obj); het_assert(ttisinteger(io)); val_(io).i =(x); }

/*
 * Strings
 */

/* Variant tags for string */
#define HET_VSHRSTR makevariant(HET_TSTRING, 0) /* short strings */
#define HET_VLNGSTR makevariant(HET_TSTRING, 1) /* long strings */

#define ttisstring(o) checktype((o), HET_TSTRING)
#define ttisshrstring(o) checktag((o), ctb(HET_VSHRSTR))
#define ttislngstring(o) checktag((o), ctb(HET_VLNGSTR))

#define tsvalueraw(v) (gco2ts((v).gc))

#define tsvalue(o) check_exp(ttisstring(o), gco2ts(val_(o).gc))

#define setsvalue(L,obj,x) \
    { TValue *io = (obj); TString *x_ = (x); \
      val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
      checkliveness(L,io); }

/* set a string to the stack */
#define setsvalue2s(L,o,s) setsvalue(L,s2v(o),s)

/* set a string to a new object */
#define setsvalue2n setsvalue

/*
 * Header for a string value.
 */
typedef struct TString {
    CommonHeader;
    he_byte extra; /* reserved words for short strings; "has hash" for longs */
    he_byte shrlen; /* length for short strings, 0xFF for long strings */
    unsigned int hash;
    union {
        size_t lnglen; /* length for long strings */
        struct TString *hnext; /* linked list for hash table */
    } u;
    char contents[1];
} TString;

/*
 * Get the actual string (array of bytes) from a `TString`. (Generic
 * version and specialized versions for a long and short strings.)
 */
#define getstr(ts) ((ts)->contents)
#define getlngstr(ts) check_exp((ts->shrlen == 0xFF, (ts)->contents)
#define getshrstr(ts) check_exp((ts->shrlen != 0xFF, (ts)->contents))

#define tsslen(s) \
    ((s)->shrlen != 0xFF ? (s)-> shrlen : (s)->u.lnglen)

/*
 * Userdata
 */

/*
 * Light userdata should be a variant of userdata, but for compatibility
 * reasons they are also different types.
 */
#define HET_VLIGHTUSERDATA makevariant(HET_TLIGHTUSERDATA, 0)

#define HET_VUSERDATA makevariant(HET_TUSERDATA, 0)

#define ttislightuserdata(o) checktype((o), HET_TLIGHTUSERDATA)
#define ttisfulluserdata(o) checktype((o), ctb(HET_VUSERDATA)

#define pvalue(o) check_exp(ttislightuserdata(o), val_(o).p)
#define uvalue(o) check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))

#define pvalueraw(v) ((v).p)

#define setpvalue(obj,x) \
    { TValue *io=(obj); val_(io).p=(x); settt_(io,HET_VLIGHTUSERDATA); }

#define setuvalue(L,obj,x) \
    { TValue *io = (obj); Udata *x_ = (x); \
      val_(io).gc = obj2gco(x_); settt_(io, ctb(HET_VUSERDATA)); \
      checkliveness(L,io); }

/* Ensures that addresses after this type are always fully assigned */
typedef union UValue {
    TValue uv;
    HETI_MAXALIGN; /* ensures maximum alignment for udata bytes */
} UValue;

/*
 * Header for userdata with user values;
 * memmory area follows the end of this structure.
 */
typedef struct Udata {
    CommonHeader;
    unsigned short nuvalue; /* number of user values */
    size_t len; /* number of bytes */
    struct Table *metatable;
    GCObject *gclist;
    UValue uv[1]; /* user values */
} Udata;

/*
 * Header for userdata with no user values. These userdata do not need
 * to be gray during GC, and therefore do not need a gclist field.
 * To simplify, the code always use `Udata` for both kinds of userdata,
 * making sure it never accesses `gclist` on userdata with no user values.
 * This structure here is used only to compute the correct size for
 * this representation. (The `bindata` field in its end ensures correct
 * alignment for binary data following this header.)
 */
typedef struct Udata0 {
    CommonHeader;
    unsigned short nuvalue; /* number of user values */
    size_t len; /* number of bytes */
    struct Table *metatable;
    union {HETI_MAXALIGN;} bindata;
} Udata0;

/* compute the offset of the memory area of user data */
#define udatamemoffset(nuv) \
    ((nuv) == 0 ? offsetof(Udata0, bindata) \
                : offsetof(Udata, uv) + (sizeof(UValue) * (nuv)))

/* get the address of the memory block inside `Udata` */
#define getudatamem(u) (cast_charp(u) + udatamemoffset(uvalue(u)->nuvalue)

/* compute the size of a userdata */
#define sizeudata(nuv,nb) (udatamemoffset(nuv) + (nb))

/*
 * Prototypes
 */
#define HET_VPROTO makevariant(HET_TPROTO, 0)

/*
 * Description of an upvalue for function prototypes
 */
typedef struct Upvaldesc {
    TString *name; /* upvalue name (for debug info) */
    he_byte instack; /* whether it is in stack (register) */
    he_byte idx; /* index of upvalue (in stack or in outer function's list) */
    he_byte kind; /* kind of corresponding variable */
} Upvaldesc;

/*
 * Description of a local variable for function prototypes
 * (used for debug information)
 */
typedef struct LovVar {
    TString *varname;
    int startpc; /*  first point where variable is active */
    int endpc; /* first point where variable is dead */
} LocVar;

/*
 * Associates the absolute line source for a given instruction pc.
 * The array `lineinfo` gives, for each instruction, the difference in
 * lines from the previous instruction. When that difference does not
 * fit into a byte, Het saves the absolute line for that instruction.
 * (Het also saves the absolute line periodically, to speed up the
 * computation of a line number: we can use binary search in the
 * absolute-line array, but we must traverse the `lineinfo` array
 * linearly to compute a line.)
 */
typedef struct AbsLineInfo {
    int pc;
    int line;
} AbsLineInfo;

/*
 * Function Prototypes
 */
typedef struct Proto {
    CommonHeader;
    he_byte numparams; /* number of fixed (named) parameters */
    he_byte is_vararg;
    he_byte maxstacksize; /* number of registers needed by this function */
    int sizeupvalues; /* size of upvalues */
    int sizek; /* size of `k` */
    int sizecode;
    int sizelineinfo;
    int sizep; /* size of `p` */
    int sizelocvars;
    int sizeabslineinfo; /* size of `abslineinfo` */
    int linedefined; /* debug information */
    int lastlinedefined; /* debug information */
    TValue *k; /* constants used by the function */
    Instruction *code; /* opcodes */
    struct Proto **p; /* functions defined inside the function */
    Upvaldesc *upvalues; /* upvalue information */
    hs_byte *lineinfo; /* information about source lines (debug information) */
    AbsLineInfo *abslineinfo; /* idem */
    LocVar *locvars; /* information about local variables (debug information) */
    TString *source; /* used for debug information */
    GCObject *gclist;
} Proto;

/*
 * Functions
 */
#define HET_VUPVAL makevariant(HET_TUPVAL, 0)

/* Variant tags for functions */
#define HET_VLCL makevariant(HET_TFUNCTION, 0) /* Het closure */
#define HET_VLCF makevariant(HET_TFUNCTION, 1) /* light c function */
#define HET_VCCL makevariant(HET_TFUNCTION, 2) /* C closure */

#define ttisfunction(o) checktype(o, HET_TFUNCTION)
#define ttisHclosure(o) checktag((o), ctb(HET_VLCL))
#define ttishcf(o) checktag((o), HET_VLCF)
#define ttisCclosure(o) checktag((o), ctb(HET_VCCL))
#define ttisclosure(o) (ttisHclosure(o) || ttisCclosure(o))

#define isHfunction(o) ttisHclosure(o)

#define clvalue(o) check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clHvalue(o) check_exp(ttisHclosure(o), gco2cl(val_(o).gc))
#define fvalue(o) check_exp(ttishcf(o), val_(o).f)
#define clCvalue(o) check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))

#define fvalueraw(v) ((v).f)

#define setclHvalue(L,obj,x) \
    { TValue *io = (obj); HClosure *x_ = (x); \
      val_(io).gc = obj2gco(x_); settt_(io, ctb(HET_VLCL)); \
      checkliveness(L,io); }

#define setclHvalue2s(L,o,cl) setclHvalue(L,s2v(o),cl)

#define setfvalue(obj,x) \
    { TValue *io=(obj); val_(io).f=(x); settt_(io, HET_VLCF); }

#define setclCvalue(L,obj,x) \
    { TValue *io = (obj); CClosure *x_ = (x); \
      val_(io).gc = obj2gco(x_); settt_(io, ctb(HET_VCCL)); \
      checkliveness(L,io); }

/*
 * Upvalues for Het closures
 */
typedef struct UpVal {
    CommonHeader;
    union {
        TValue *p; /* points to stack or to its own value */
        ptrdiff_t offset; /* used while the stack is being reallocated */
    } v;
    union {
        struct { /* when open */
            struct UpVal *next; /* linked list */
            struct UpVal **previous;
        } open;
        TValue value; /* value (when closed) */
    } u;
} UpVal;

#define ClosureHeader \
        CommonHeader; he_byte nupvalues; GCObject *gclist;

typedef struct CClosure {
    ClosureHeader;
    het_CFunction f;
    TValue upvalue[1]; /* list of upvalues */
} CClosure;

typedef struct HClosure {
    ClosureHeader;
    struct Proto *p;
    UpVal *upvals[1]; /* list of upvalues */
} HClosure;

typedef union Closure {
    CClosure c;
    HClosure h;
} Closure;

#define getproto(o) (clHvalue(o)->p)

/*
 * Tables
 */

#define HET_VTABLE makevariant(HET_TTABLE, 0)

#define ttistable(o) checktag((o), ctb(HET_VTABLE))

#define hvalue(o) check_exp(ttistable(o), gco2t(val_(o).gc))

#define sethvalue(L,obj,x) \
    { TValue *io = (obj); Table *x_ = (x); \
      val_(io).gc = obj2gco(x_); settt_(io, ctb(HET_VTABLE)); \
      checkliveness(L,io); }

#define sethvalue2s(L,o,h) sethvalue(L,s2v(o),h)

/*
 * Nodes for Hash tables: A pack of two TValues (key-value pairs)
 * plus a `next` field to link colliding entries. The distribution
 * of the keys fields (key_tt and key_val) not forming a proper
 * TValue allows for a smaller size for Node both in 4-byte
 * and 8-byte alignments.
 */
typedef struct Node {
    struct NodeKey {
        TValueFields; /* fields for value */
        he_byte key_tt; /* type of key */
        int next; /* for chaining */
        Value key_val; /* key value */
    } u;
    TValue i_val; /* direct access to node's value as a proper `TValue` */
} Node;

/* copy a value into a key */
#define setnodekey(L,node,obj) \
    { Node *n_=(node); const TValue *io_(obj); \
      n_->u.key_val = io_->value_; n_->u.key_tt = io_->tt_; \
      checkliveness(L,io_); }

/* copy a value from a key */
#define getnodekey(L,obj,node) \
    { TValue *io_=(obj); const Node *n_=(node); \
      io_->value_ = n_->u.key_val; io_->tt_ = n_->u.key_tt; \
      checkliveness(L,io_); }

/*
 * About `alimit`: if `isrealsize(t)` is true, then `alimit` is the
 * real size of `array`. Otherwise, thre real size of `array` is the
 * smallest power of two not smaller than `alimit` (or zero iff `alimit`
 * is zero); `alimit` is then used as a hint for #t.
 */
#define BITRAS (1 << 7)
#define isrealasize(t) (!((t)->flags & BITRAS))
#define setrealasize(t) ((t)->flags &= cast_byte(~BITRAS))
#define setnorealasize(t) ((t)->flags |= BITRAS)

typedef struct Table {
    CommonHeader;
    he_byte flags; /* 1<<p means tagmethod(p) is not present */
    he_byte lsizenode; /* log2 of size of `node` array */
    unsigned int alimit; /* "limit" of `array` array */
    TValue *array; /* array part */
    Node *node;
    Node *lastfree; /* any free position is before this position */
    struct Table *metatable;
    GCObject *gclist;
} Table;

/*
 * Macros to manipulate keys inserted in nodes
 */
#define keytt(node) ((node)->u.key_tt)
#define keyval(node) ((node)->u.key_val)

#define keyisnil(node) (keytt(node) == HET_TNIL)
#define keyisinteger(node) (keytt(node) == HET_VNUMINT)
#define keyisval(node) (keyval(node).i)
#define keyisshrstr(node) (keytt(node) == ctb(HET_VSHRSTR))
#define keystrval(node) (gco2ts(keyval(node).gc))

#define setnilkey(node) (keytt(node) == HET_TNIL)

#define keyiscollectable(n) (keytt(n) & BIT_ISCOLLECTABLE)

#define gckey(n) (keyval(n).gc)
#define gckeyN(n) (keyiscollectable(n) ? gckey(n) : NULL)

/*
 * `module` operation for hashing (size is always a power of 2)
 */
#define lmod(s,size) \
    (check_exp((size&(size-1))==0, (cast_int((s) & ((size)-1)))))

#define twoto(x) (1<<(x))
#define sizenode(t) (twoto((t)->lsizenode))

/* size of buffer for `het0_utf8esc` function */
#define UTF8BUFFSZ 8

HETI_FUNC int het0_utf8esc(char *buff, unsigned long x);
HETI_FUNC int het0_ceillog2(unsigned int x);
HETI_FUNC int het0_rawarith(het_State *L, int op, const TValue *p1,
                            const TValue *p2, TValue *res);
HETI_FUNC void het0_arith (het_State *L, int op, const TValue *p1,
                            const TValue *p2, StkId res);
HETI_FUNC size_t het0_str2num(const char *s, TValue *o);
HETI_FUNC int het0_hexavalue(int c);
HETI_FUNC void het0_tostring(het_State *L, TValue *obj);
HETI_FUNC const char *het0_pushvfstring(het_State *L, const char *fmt, va_list argp);
HETI_FUNC const char *het0_pushfstring(het_State *L, const char *fmt, ...);
HETI_FUNC void het0_chunkid(char *out, const char *source, size_t srclen);

#endif
//
// Created by jhnos on 21-05-2025.
//

#ifndef HET_TM_H
#define HET_TM_H

#include "het_object.h"

typedef enum {
    TM_INDEX,
    TM_NEWINDEX,
    TM_GC,
    TM_MODE,
    TM_LEN,
    TM_EQ,  /* last tag method with fast access */
    TM_ADD,
    TM_SUB,
    TM_MUL,
    TM_MOD,
    TM_POW,
    TM_DIV,
    TM_IDIV,
    TM_BAND,
    TM_BOR,
    TM_BXOR,
    TM_SHL,
    TM_SHR,
    TM_UNM,
    TM_BNOT,
    TM_LT,
    TM_LE,
    TM_CONCAT,
    TM_CALL,
    TM_CLOSE,
    TM_N          /* number of elements in the enum */
} TMS;

/* Mask with 1 in all fast-access methods. A 1 in any of these bits
 * in the flag of a (meta) table means the metatable does not have the
 * corresponding metathod field. (Bit 7 of the flag is used for `isrealsize`.
 */
#define maskflags (~(~0u << (TM_EQ + 1)))

/*
 * Test whether there is no tagmethod.
 * (Because tagmethods use raw accesses, the result may be an "empty" nil.)
 */
#define notm(tm) ttisnil(tm)

#define gfastttm(g,et,e) ((et) == NULL ? NULL : \
    ((et)->flags & (1u <<(e))) ? NULL : hetT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e) gfastttm(G(l), et, e)

#define ttypename(x) hetT_typenames_[(x) + 1]


#endif //HET_TM_H

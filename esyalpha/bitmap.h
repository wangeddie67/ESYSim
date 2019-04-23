/* 
 * bitmap.h - bit mask manipulation macros
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1994, 1995, 1996, 1997, 1998 by Todd M. Austin
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful.  The tool set comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use. 
 * 
 * Everyone is granted permission to copy, modify and redistribute
 * this tool set under the following conditions:
 * 
 *    This source code is distributed for non-commercial use only. 
 *    Please contact the maintainer for restrictions applying to 
 *    commercial use.
 *
 *    Permission is granted to anyone to make or distribute copies
 *    of this source code, either as received or modified, in any
 *    medium, provided that all copyright notices, permission and
 *    nonwarranty notices are preserved, and that the distributor
 *    grants the recipient permission for further redistribution as
 *    permitted by this document.
 *
 *    Permission is granted to distribute this file in compiled
 *    or executable form under the same conditions that apply for
 *    source code, provided that either:
 *
 *    A. it is accompanied by the corresponding machine-readable
 *       source code,
 *    B. it is accompanied by a written offer, with no time limit,
 *       to give anyone a machine-readable copy of the corresponding
 *       source code in return for reimbursement of the cost of
 *       distribution.  This written offer must permit verbatim
 *       duplication by anyone, or
 *    C. it is distributed by someone who received only the
 *       executable form, and is accompanied by a copy of the
 *       written offer of source code that they received concurrently.
 *
 * In other words, you are welcome to use, share and improve this
 * source file.  You are forbidden to forbid anyone else to use, share
 * and improve what you give them.
 *
 * INTERNET: dburger@cs.wisc.edu
 * US Mail:  1210 W. Dayton Street, Madison, WI 53706
 *
 * $Id: bitmap.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: bitmap.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:11  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.1  2004/05/14 14:48:41  ninelson
 * Initial revision
 *
 * Revision 1.1.1.2  2004/05/13 15:11:59  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2003/10/20 13:58:35  ninelson
 * new from Ali
 *
 * Revision 1.4  1998/08/27 07:45:12  taustin
 * BITMAP_COUNT_ONES() added
 * BITMAP_NOT() fixed
 *
 * Revision 1.3  1997/03/11  01:06:06  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 *
 * Revision 1.1  1996/12/05  18:44:19  taustin
 * Initial revision
 *
 */

#ifndef BITMAP_H
#define BITMAP_H

/* BITMAPs:
     BMAP: int * to an array of ints
     SZ: number of ints in the bitmap
*/

/* declare a bitmap type */
#define BITMAP_SIZE(BITS)	(((BITS)+31)/32)
#define BITMAP_TYPE(BITS, NAME)	unsigned int (NAME)[BITMAP_SIZE(BITS)]

typedef unsigned int BITMAP_ENT_TYPE;
typedef unsigned int *BITMAP_PTR_TYPE;

/* set entire bitmap */

#define BITMAP_SET_MAP(BMAP, SZ)				\
  { int i; for (i=0; i<(SZ); i++) (BMAP)[i] = 0xffffffff; }

/* clear entire bitmap */
#define BITMAP_CLEAR_MAP(BMAP, SZ)				\
  { int i; for (i=0; i<(SZ); i++) (BMAP)[i] = 0; }

/* set bit BIT in bitmap BMAP, returns BMAP */
#define BITMAP_SET(BMAP, SZ, BIT)				\
  (((BMAP)[(BIT)/32] |= (1 << ((BIT) % 32))), (BMAP))
#if 0
#define BITMAP_SET(BMAP, SZ, BIT)				\
  (((BMAP)[(BIT)/32] |= (1 << ((BIT) % 32))))
#endif

/* clear bit BIT in bitmap BMAP, returns BMAP */
#define BITMAP_CLEAR(BMAP, SZ, BIT)				\
  (((BMAP)[(BIT)/32] &= ~(1 << ((BIT) % 32))), (BMAP))

/* copy bitmap SRC to DEST */
#define BITMAP_COPY(DESTMAP, SRCMAP, SZ)			\
  { int i; for (i=0; i<(SZ); i++) (DESTMAP)[i] = (SRCMAP)[i]; }

/* store bitmap B2 OP B3 into B1 */
#define __BITMAP_OP(B1, B2, B3, SZ, OP)				\
  { int i; for (i=0; i<(SZ); i++) (B1)[i] = (B2)[i] OP (B3)[i]; }

/* store bitmap B2 | B3 into B1 */
#define BITMAP_IOR(B1, B2, B3, SZ)				\
  __BITMAP_OP(B1, B2, B3, SZ, |)

/* store bitmap B2 ^ B3 into B1 */
#define BITMAP_XOR(B1, B2, B3, SZ)				\
  __BITMAP_OP(B1, B2, B3, SZ, ^)

/* store bitmap B2 & B3 into B1 */
#define BITMAP_AND(B1, B2, B3, SZ)				\
  __BITMAP_OP(B1, B2, B3, SZ, &)

/* store ~B2 into B1 */
#define BITMAP_NOT(B1, B2, SZ)					\
  { int i; for (i=0; i<(SZ); i++) (B1)[i] = ~((B2)[i]); }

/* return non-zero if bitmap is empty */
#define BITMAP_EMPTY_P(BMAP, SZ)				\
  ({ int i, res=0; for (i=0; i<(SZ); i++) res |= (BMAP)[i]; !res; })

/* return non-zero if the intersection of bitmaps B1 and B2 is non-empty */
#define BITMAP_DISJOINT_P(B1, B2, SZ)				\
  ({ int i, res=0; for (i=0; i<(SZ); i++) res |= (B1)[i] & (B2)[i]; !res; })

/* return non-zero if bit BIT is set in bitmap BMAP */
#define BITMAP_SET_P(BMAP, SZ, BIT)				\
  (((BMAP)[(BIT)/32] & (1 << ((BIT) % 32))) != 0)

/* return non-zero if bit BIT is clear in bitmap BMAP */
#define BITMAP_CLEAR_P(BMAP, SZ, BIT)				\
  (!BMAP_SET_P((BMAP), (SZ), (BIT)))

/* count the number of bits set in BMAP */
#define BITMAP_COUNT_ONES(BMAP, SZ)				\
({								\
  int i, j, n = 0;						\
  for (i = 0; i < (SZ) ; i++)					\
    {								\
      unsigned int word = (BMAP)[i];				\
      for (j=0; j < (sizeof(unsigned int)*8); j++)		\
        {							\
          unsigned int new_val, old_val = word;			\
          word >>= 1;						\
          new_val = word << 1;					\
          if (old_val != new_val)				\
            n++;						\
        }							\
    }								\
  n;								\
})

#endif /* BITMAP_H */


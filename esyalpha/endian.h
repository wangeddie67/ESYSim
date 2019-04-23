/*
 * endian.h - host endian probe interfaces
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
 * $Id: endian.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: endian.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:14  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:51  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.1  2004/05/14 14:48:41  ninelson
 * Initial revision
 *
 * Revision 1.1.1.1  2003/10/20 13:58:35  ninelson
 * new from Ali
 *
 * Revision 1.1.1.1  2000/05/26 15:21:52  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.7  1999/12/31 18:34:59  taustin
 * cross-endian execution support added
 *
 * Revision 1.6  1999/12/13 18:43:19  taustin
 * cross endian execution support added
 *
 * Revision 1.5  1998/08/27 08:25:29  taustin
 * implemented host interface description in host.h
 *
 * Revision 1.4  1997/03/11  01:10:30  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 * swapping supported disabled until it can be tested further
 *
 * Revision 1.3  1997/01/06  15:58:53  taustin
 * comments updated
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 *
 */

#include "smt.h"


#ifndef ENDIAN_H
#define ENDIAN_H

/* data swapping functions, from big/little to little/big endian format */
#define SWAP_HALF(X)							\
  (((((half_t)(X)) & 0xff) << 8) | ((((half_t)(X)) & 0xff00) >> 8))
#define SWAP_WORD(X)	(((word_t)(X) << 24) |				\
			 (((word_t)(X) << 8)  & 0x00ff0000) |		\
			 (((word_t)(X) >> 8)  & 0x0000ff00) |		\
			 (((word_t)(X) >> 24) & 0x000000ff))
#define SWAP_QWORD(X)	(((qword_t)(X) << 56) |				\
			 (((qword_t)(X) << 40) & ULL(0x00ff000000000000)) |\
			 (((qword_t)(X) << 24) & ULL(0x0000ff0000000000)) |\
			 (((qword_t)(X) << 8)  & ULL(0x000000ff00000000)) |\
			 (((qword_t)(X) >> 8)  & ULL(0x00000000ff000000)) |\
			 (((qword_t)(X) >> 24) & ULL(0x0000000000ff0000)) |\
			 (((qword_t)(X) >> 40) & ULL(0x000000000000ff00)) |\
			 (((qword_t)(X) >> 56) & ULL(0x00000000000000ff)))

/* recognized endian formats */
enum endian_t { endian_big=0, endian_little=1, endian_unknown=2};
/* probe host (simulator) byte endian format */
enum endian_t
endian_host_byte_order(void);

/* probe host (simulator) double word endian format */
enum endian_t
endian_host_word_order(void);

#ifndef HOST_ONLY

#ifdef SMT_SS
/* probe target (simulated program) byte endian format, only
   valid after program has been loaded */
enum endian_t
endian_target_byte_order(int threadid);

/* probe target (simulated program) double word endian format,
   only valid after program has been loaded */
enum endian_t
endian_target_word_order(int thredid);

#else
/* probe target (simulated program) byte endian format, only
   valid after program has been loaded */
enum endian_t
endian_target_byte_order(void);

/* probe target (simulated program) double word endian format,
   only valid after program has been loaded */
enum endian_t
endian_target_word_order(void);
#endif

#endif /* HOST_ONLY */

#endif /* ENDIAN_H */

/*
 * host.h - host-dependent definitions and interfaces
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1998 by Todd M. Austin
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
 * $Id: host.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: host.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:15  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.2  2004/07/06 21:31:28  grbriggs
 * Note possible problem with the type of tick_t.
 *
 * Revision 1.1  2004/05/14 14:48:41  ninelson
 * Initial revision
 *
 * Revision 1.1.1.1  2003/10/20 13:58:35  ninelson
 * new from Ali
 *
 * Revision 1.1.1.1  2000/05/26 15:18:57  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.3  1999/12/31 18:36:58  taustin
 * cross-endian execution support added
 * quad_t naming conflicts removed
 *
 * Revision 1.2  1999/03/08 06:33:49  taustin
 * added SVR4 host support
 *
 * Revision 1.1  1998/08/27 08:28:46  taustin
 * Initial revision
 *
 *
 */

#ifndef HOST_H
#define HOST_H

/* make sure host compiler supports ANSI-C */
#ifndef __STDC__ /* an ansi C compiler is required */
#error The SimpleScalar simulators must be compiled with an ANSI C compiler.
#endif /* __STDC__ */

/* enable inlining here, if supported by host compiler */
#undef INLINE
#if defined(__GNUC__)
#define INLINE		inline
#else
#define INLINE
#endif

/* bind together two symbols, at preprocess time */
#ifdef __GNUC__
/* this works on all GNU GCC targets (that I've seen...) */
#define SYMCAT(X,Y)	X##Y
#define ANSI_SYMCAT
#else /* !__GNUC__ */
#ifdef OLD_SYMCAT
#define SYMCAT(X,Y)	X/**/Y
#else /* !OLD_SYMCAT */
#define SYMCAT(X,Y)	X##Y
#define ANSI_SYMCAT
#endif /* OLD_SYMCAT */
#endif /* __GNUC__ */

/* host-dependent canonical type definitions */
typedef int bool_t;			/* generic boolean type */
typedef unsigned char byte_t;		/* byte - 8 bits */
typedef signed char sbyte_t;
typedef unsigned short half_t;		/* half - 16 bits */
typedef signed short shalf_t;
typedef unsigned int word_t;		/* word - 32 bits */
typedef signed int sword_t;
typedef float sfloat_t;			/* single-precision float - 32 bits */
typedef double dfloat_t;		/* double-precision float - 64 bits */

/* qword defs, note: not all targets support qword types */
#if defined(__GNUC__) || defined(__SUNPRO_C) || defined(__CC_C89) || defined(__CC_XLC)
#define HOST_HAS_QWORD
typedef unsigned long long qword_t;	/* qword - 64 bits */
typedef signed long long sqword_t;
#ifdef ANSI_SYMCAT
#define ULL(N)		N##ULL		/* qword_t constant */
#define LL(N)		N##LL		/* sqword_t constant */
#else /* OLD_SYMCAT */
#define ULL(N)		N/**/ULL	/* qword_t constant */
#define LL(N)		N/**/LL		/* sqword_t constant */
#endif
#elif defined(__alpha)
#define HOST_HAS_QWORD
typedef unsigned long qword_t;		/* qword - 64 bits */
typedef signed long sqword_t;
#ifdef ANSI_SYMCAT
#define ULL(N)		N##UL		/* qword_t constant */
#define LL(N)		N##L		/* sqword_t constant */
#else /* OLD_SYMCAT */
#define ULL(N)		N/**/UL		/* qword_t constant */
#define LL(N)		N/**/L		/* sqword_t constant */
#endif
#elif defined(_MSC_VER)
#define HOST_HAS_QWORD
typedef unsigned __int64 qword_t;	/* qword - 64 bits */
typedef signed __int64 sqword_t;
#define ULL(N)		((qword_t)(N))
#define LL(N)		((sqword_t)(N))
#else /* !__GNUC__ && !__alpha */
#undef HOST_HAS_QWORD
#endif

/* statistical counter types, use largest counter type available */
#ifdef HOST_HAS_QWORD
typedef sqword_t counter_t;
/* typedef sqword_t tick_t; */		/* NOTE: unsigned breaks caches */
typedef counter_t tick_t;	/* FIXME: why does this give different results than when tick_t is a dfloat_t?? */
//typedef dfloat_t tick_t;	/* gjmfix: GPS -- in order to make time more accurate */
#else /* !HOST_HAS_QWORD */
typedef dfloat_t counter_t;
typedef dfloat_t tick_t;
#endif /* HOST_HAS_QWORD */

/* gjmfix: define a 64-bit integer limit */
#ifndef INF_COUNTER
#define INF_COUNTER    9223372036854775807LL
#endif

#ifdef __svr4__
#define setjmp	_setjmp
#define longjmp	_longjmp
#endif

#endif /* HOST_H */

/*
 * range.h - program execution range definitions and interfaces
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
 * $Id: range.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: range.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:28  xue
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
 * Revision 1.2  1998/08/27 15:50:50  taustin
 * implemented host interface description in host.h
 * added target interface support
 *
 * Revision 1.1  1997/03/11  01:32:44  taustin
 * Initial revision
 *
 *
 */

#ifndef RANGE_H
#define RANGE_H

#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "machine.h"

enum range_ptype_t {
  pt_addr = 0,			/* address position */
  pt_inst,			/* instruction count position */
  pt_cycle,			/* cycle count position */
  pt_NUM
};

/*
 * an execution position
 *
 *   by addr:		@<addr>
 *   by inst count:	<icnt>
 *   by cycle count:	#<cycle>
 *
 */
struct range_pos_t {
  enum range_ptype_t ptype;	/* type of position */
  counter_t pos;		/* position */
};

/* an execution range */
struct range_range_t {
  struct range_pos_t start;
  struct range_pos_t end;
};

/* parse execution position *PSTR to *POS */
char *						/* error string, or NULL */
range_parse_pos(char *pstr,			/* execution position string */
		struct range_pos_t *pos);	/* position return buffer */

/* print execution position *POS */
void
range_print_pos(struct range_pos_t *pos,	/* execution position */
		FILE *stream);			/* output stream */

/* parse execution range *RSTR to *RANGE */
char *						/* error string, or NULL */
range_parse_range(char *rstr,			/* execution range string */
		  struct range_range_t *range);	/* range return buffer */

/* print execution range *RANGE */
void
range_print_range(struct range_range_t *range,	/* execution range */
		  FILE *stream);		/* output stream */

/* determine if inputs match execution position */
int						/* relation to position */
range_cmp_pos(struct range_pos_t *pos,		/* execution position */
	      counter_t val);			/* position value */

/* determine if inputs are in range */
int						/* relation to range */
range_cmp_range(struct range_range_t *range,	/* execution range */
		counter_t val);			/* position value */


/* determine if inputs are in range, passes all possible info needed */
int						/* relation to range */
range_cmp_range1(struct range_range_t *range,	/* execution range */
		 md_addr_t addr,		/* address value */
		 counter_t icount,		/* instruction count */
		 counter_t cycle);		/* cycle count */


/*
 *
 * <range> := {<start_val>}:{<end>}
 * <end>   := <end_val>
 *            | +<delta>
 *
 */

#endif /* RANGE_H */

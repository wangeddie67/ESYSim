/*
 * regs.h - architected register state interfaces
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
 * $Id: regs.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: regs.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:05  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.3  2004/06/23 15:43:43  ninelson
 * added NUM_COM_REGS
 *
 * Revision 1.2  2004/06/18 20:19:18  grbriggs
 * Added structs from garg.
 *
 * Revision 1.1  2004/05/14 14:48:41  ninelson
 * Initial revision
 *
 * Revision 1.1.1.1  2003/10/20 13:58:35  ninelson
 * new from Ali
 *
 * Revision 1.5  1998/08/27 15:53:42  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 *
 * Revision 1.4  1997/03/11  01:19:41  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 *
 * Revision 1.3  1997/01/06  16:02:48  taustin
 * \comments updated
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 *
 */

#ifndef REGS_H
#define REGS_H

#include "host.h"
#include "misc.h"
#include "machine.h"

/*
 * This module implements the SimpleScalar architected register state, which
 * includes integer and floating point registers and miscellaneous registers.
 * The architected register state is as follows:
 *
 * Integer Register File:                  Miscellaneous Registers:
 * (aka general-purpose registers, GPR's)
 *
 *   +------------------+                  +------------------+
 *   | $r0 (src/sink 0) |                  | PC               | Program Counter
 *   +------------------+                  +------------------+
 *   | $r1              |                  | HI               | Mult/Div HI val
 *   +------------------+                  +------------------+
 *   |  .               |                  | LO               | Mult/Div LO val
 *   |  .               |                  +------------------+
 *   |  .               |
 *   +------------------+
 *   | $r31             |
 *   +------------------+
 *
 * Floating point Register File:
 *    single-precision:  double-precision:
 *   +------------------+------------------+  +------------------+
 *   | $f0              | $f1 (for double) |  | FCC              | FP codes
 *   +------------------+------------------+  +------------------+
 *   | $f1              |
 *   +------------------+
 *   |  .               |
 *   |  .               |
 *   |  .               |
 *   +------------------+------------------+
 *   | $f30             | $f31 (for double)|
 *   +------------------+------------------+
 *   | $f31             |
 *   +------------------+
 *
 * The floating point register file can be viewed as either 32 single-precision
 * (32-bit IEEE format) floating point values $f0 to $f31, or as 16
 * double-precision (64-bit IEEE format) floating point values $f0 to $f30.
 */
 struct common_regs_t {
	 int regs_lock;
	 md_addr_t address;
	 int subscribe_value;
	 int subscribed;
 };
                                                                                                                                                             
struct common_regs_t common_regs_s[4][NUM_COM_REGS];

struct regs_t {
#ifdef SMT_SS
  int threadid;           /* this RUU instant belongs to which theard  */
#endif
  md_gpr_t regs_R;		/* (signed) integer register file */
  md_fpr_t regs_F;		/* floating point register file */
  md_ctrl_t regs_C;		/* control register file */
  md_addr_t regs_PC;		/* program counter */
  md_addr_t regs_NPC;		/* next-cycle program counter */
};

/* create a register file */
struct regs_t *regs_create(void);

/* initialize architected register state */
void
regs_init(struct regs_t *regs);		/* register file to initialize */

/* dump all architected register state values to output stream STREAM */
void
regs_dump(struct regs_t *regs,		/* register file to display */
	  FILE *stream);		/* output stream */

/* destroy a register file */
void
regs_destroy(struct regs_t *regs);	/* register file to release */

#endif /* REGS_H */

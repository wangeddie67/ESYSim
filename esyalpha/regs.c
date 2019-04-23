/*
 * regs.c - architected registers state routines
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
 * $Id: regs.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: regs.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:29  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:53  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.1  2004/05/14 14:30:09  ninelson
 * Initial revision
 *
 * Revision 1.1.1.1  2003/10/20 13:58:02  ninelson
 * *** empty log message ***
 *
 * Revision 1.5  1998/08/27 15:53:09  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 *
 * Revision 1.4  1997/03/11  01:19:28  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 *
 * Revision 1.3  1997/01/06  16:02:36  taustin
 * comments updated
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "loader.h"
#include "regs.h"


/* create a register file */
struct regs_t *
regs_create(void)
{
  struct regs_t *regs;

  regs = calloc(1, sizeof(struct regs_t));
  if (!regs)
    fatal("out of virtual memory");

  return regs;
}

/* initialize architected register state */
void
regs_init(struct regs_t *regs)		/* register file to initialize */
{
  /* FIXME: assuming all entries should be zero... */
  memset(regs, 0, sizeof(*regs));

  /* regs->regs_R[MD_SP_INDEX] and regs->regs_PC initialized by loader... */
}




#if 0

/* floating point register file format */
union regs_FP_t {
    md_gpr_t l[MD_NUM_FREGS];			/* integer word view */
    md_SS_FLOAT_TYPE f[SS_NUM_REGS];		/* single-precision FP view */
    SS_DOUBLE_TYPE d[SS_NUM_REGS/2];		/* double-precision FP view */
};

/* floating point register file */
extern union md_regs_FP_t regs_F;

/* (signed) hi register, holds mult/div results */
extern SS_WORD_TYPE regs_HI;

/* (signed) lo register, holds mult/div results */
extern SS_WORD_TYPE regs_LO;

/* floating point condition codes */
extern int regs_FCC;

/* program counter */
extern SS_ADDR_TYPE regs_PC;

/* dump all architected register state values to output stream STREAM */
void
regs_dump(FILE *stream)		/* output stream */
{
  int i;

  /* stderr is the default output stream */
  if (!stream)
    stream = stderr;

  /* dump processor register state */
  fprintf(stream, "Processor state:\n");
  fprintf(stream, "    PC: 0x%08x\n", regs_PC);
  for (i=0; i<SS_NUM_REGS; i += 2)
    {
      fprintf(stream, "    R[%2d]: %12d/0x%08x",
	      i, regs_R[i], regs_R[i]);
      fprintf(stream, "  R[%2d]: %12d/0x%08x\n",
	      i+1, regs_R[i+1], regs_R[i+1]);
    }
  fprintf(stream, "    HI:      %10d/0x%08x  LO:      %10d/0x%08x\n",
	  regs_HI, regs_HI, regs_LO, regs_LO);
  for (i=0; i<SS_NUM_REGS; i += 2)
    {
      fprintf(stream, "    F[%2d]: %12d/0x%08x",
	      i, regs_F.l[i], regs_F.l[i]);
      fprintf(stream, "  F[%2d]: %12d/0x%08x\n",
	      i+1, regs_F.l[i+1], regs_F.l[i+1]);
    }
  fprintf(stream, "    FCC:                0x%08x\n", regs_FCC);
}

/* (signed) integer register file */
SS_WORD_TYPE regs_R[SS_NUM_REGS];

/* floating point register file */
union regs_FP regs_F;

/* (signed) hi register, holds mult/div results */
SS_WORD_TYPE regs_HI;
/* (signed) lo register, holds mult/div results */
SS_WORD_TYPE regs_LO;

/* floating point condition codes */
int regs_FCC;

/* program counter */
SS_ADDR_TYPE regs_PC;

#endif

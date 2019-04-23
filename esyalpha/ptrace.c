/*
 * ptrace.c - pipeline tracing routines
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
 * $Id: ptrace.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: ptrace.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:27  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
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
 * Revision 1.2  1998/08/27 15:49:17  taustin
 * implemented host interface description in host.h
 * added target interface support
 * using new target-dependent myprintf() package
 *
 * Revision 1.1  1997/03/11  01:32:15  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "range.h"
#include "ptrace.h"

/* pipetrace file */
FILE *ptrace_outfd = NULL;

/* pipetracing is active */
int ptrace_active = TRUE;

/* pipetracing range */
struct range_range_t ptrace_range;

/* one-shot switch for pipetracing */
int ptrace_oneshot = FALSE;

/* open pipeline trace */
void
ptrace_open(char *fname,		/* output filename */
	    char *range)		/* trace range */
{
  char *errstr;

  /* parse the output range */
  if (!range)
    {
      /* no range */
      errstr = range_parse_range(":", &ptrace_range);
      if (errstr)
	panic("cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = TRUE;
    }
  else
    {
      errstr = range_parse_range(range, &ptrace_range);
      if (errstr)
	fatal("cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = FALSE;
    }

  if (ptrace_range.start.ptype != ptrace_range.end.ptype)
    fatal("range endpoints are not of the same type");

  /* open output trace file */
  if (!fname || !strcmp(fname, "-") || !strcmp(fname, "stderr"))
    ptrace_outfd = stderr;
  else if (!strcmp(fname, "stdout"))
    ptrace_outfd = stdout;
  else
    {
      ptrace_outfd = fopen(fname, "w");
      if (!ptrace_outfd)
	fatal("cannot open pipetrace output file `%s'", fname);
    }
}

/* close pipeline trace */
void
ptrace_close(void)
{
  if (ptrace_outfd != NULL && ptrace_outfd != stderr && ptrace_outfd != stdout)
    fclose(ptrace_outfd);
}

/* declare a new instruction */
void
__ptrace_newinst(unsigned int iseq,	/* instruction sequence number */
		 md_inst_t inst,	/* new instruction */
		 md_addr_t pc,		/* program counter of instruction */
		 md_addr_t addr)	/* address referenced, if load/store */
{
  myfprintf(ptrace_outfd, "+ %u 0x%08p 0x%08p ", iseq, pc, addr);
  md_print_insn(inst, addr, ptrace_outfd);
  fprintf(ptrace_outfd, "\n");

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new uop */
void
__ptrace_newuop(unsigned int iseq,	/* instruction sequence number */
		char *uop_desc,		/* new uop description */
		md_addr_t pc,		/* program counter of instruction */
		md_addr_t addr)		/* address referenced, if load/store */
{
  myfprintf(ptrace_outfd,
	    "+ %u 0x%08p 0x%08p [%s]\n", iseq, pc, addr, uop_desc);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare instruction retirement or squash */
void
__ptrace_endinst(unsigned int iseq)	/* instruction sequence number */
{
  fprintf(ptrace_outfd, "- %u\n", iseq);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new cycle */
void
__ptrace_newcycle(tick_t cycle)		/* new cycle */
{
  fprintf(ptrace_outfd, "@ %.0f\n", (double)cycle);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* indicate instruction transition to a new pipeline stage */
void
__ptrace_newstage(unsigned int iseq,	/* instruction sequence number */
		  char *pstage,		/* pipeline stage entered */
		  unsigned int pevents)/* pipeline events while in stage */
{
  fprintf(ptrace_outfd, "* %u %s 0x%08x\n", iseq, pstage, pevents);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

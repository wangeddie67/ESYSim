/*
 * range.c - program execution range routines
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
 * $Id: range.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: range.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:22  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:51  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.3  2004/05/14 14:02:58  ninelson
 * errno supported
 *
 * Revision 1.1.1.2  2004/05/13 15:11:46  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2003/10/20 13:58:02  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/05/26 15:18:59  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.3  1999/12/31 18:46:37  taustin
 * quad_t naming conflicts removed
 *
 * Revision 1.2  1998/08/27 15:50:15  taustin
 * implemented host interface description in host.h
 * added target interface support
 * using new target-dependent myprintf() package
 * eliminated compiler warnings
 *
 * Revision 1.1  1997/03/11  01:32:52  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "symbol.h"
#include "loader.h"
#include "range.h"

/*  for defining SMT   */
#include "smt.h"

#ifdef SMT_SS
#include "context.h"
#endif



/* parse execution position *PSTR to *POS */
char *						/* error string, or NULL */
range_parse_pos(char *pstr,			/* execution position string */
		struct range_pos_t *pos)	/* position return buffer */
{
  char *s, *endp;
  struct sym_sym_t *sym;

#ifdef SMT_SS
  int threadid = 0;
  context *current;
  current = thecontexts[threadid];
#endif


  /* determine position type */
  if (pstr[0] == '@')
    {
      /* address position */
      pos->ptype = pt_addr;
      s = pstr + 1;
    }
  else if (pstr[0] == '#')
    {
      /* cycle count position */
      pos->ptype = pt_cycle;
      s = pstr + 1;
    }
  else
    {
      /* inst count position */
      pos->ptype = pt_inst;
      s = pstr;
    }

  /* get position value */
  errno = 0;
  pos->pos = (counter_t)strtoul(s, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      return NULL;
    }

  /* else, not an integer, attempt double conversion */
  errno = 0;
  pos->pos = (counter_t)strtod(s, &endp);
  if (!errno && !*endp)
    {
      /* good conversion */
      /* FIXME: ignoring decimal point!! */
      return NULL;
    }

  /* else, attempt symbol lookup */
#ifdef SMT_SS
  sym_loadsyms(current->ld_prog_fname, /* !locals */FALSE,current->id);
#else
  sym_loadsyms(ld_prog_fname, /* !locals */FALSE);
#endif
  sym = sym_bind_name(s, NULL, sdb_any);
  if (sym != NULL)
    {
      pos->pos = (counter_t)sym->addr;
      return NULL;
    }

  /* else, no binding made */
  return "cannot bind execution position to a value";
}

/* print execution position *POS */
void
range_print_pos(struct range_pos_t *pos,	/* execution position */
		FILE *stream)			/* output stream */
{
  switch (pos->ptype)
    {
    case pt_addr:
      myfprintf(stream, "@0x%08p", (md_addr_t)pos->pos);
      break;
    case pt_inst:
      fprintf(stream, "%.0f", (double)pos->pos);
      break;
    case pt_cycle:
      fprintf(stream, "#%.0f", (double)pos->pos);
      break;
    default:
      panic("bogus execution position type");
    }
}

/* parse execution range *RSTR to *RANGE */
char *						/* error string, or NULL */
range_parse_range(char *rstr,			/* execution range string */
		  struct range_range_t *range)	/* range return buffer */
{
  char *pos1, *pos2, *p, buf[512], *errstr;

  /* make a copy of the execution range */
  strcpy(buf, rstr);
  pos1 = buf;

  /* find mid-point */
  p = buf;
  while (*p != ':' && *p != '\0')
    {
      p++;
    }
  if (*p != ':')
    return "badly formed execution range";
  *p = '\0';

  /* this is where the second position will start */
  pos2 = p + 1;

  /* parse start position */
  if (*pos1 && *pos1 != ':')
    {
      errstr = range_parse_pos(pos1, &range->start);
      if (errstr)
	return errstr;
    }
  else
    {
      /* default start range */
      range->start.ptype = pt_inst;
      range->start.pos = 0;
    }

  /* parse end position */
  if (*pos2)
    {
      if (*pos2 == '+')
	{
	  int delta;
	  char *endp;

	  /* get delta value */
	  errno = 0;
	  delta = strtol(pos2 + 1, &endp, /* parse base */0);
	  if (!errno && !*endp)
	    {
	      /* good conversion */
	      range->end.ptype = range->start.ptype;
	      range->end.pos = range->start.pos + delta;
	    }
	  else
	    {
	      /* bad conversion */
	      return "badly formed execution range delta";
	    }
	}
      else
	{
	  errstr = range_parse_pos(pos2, &range->end);
	  if (errstr)
	    return errstr;
	}
    }
  else
    {
      /* default end range */
      range->end.ptype = range->start.ptype;
#ifdef HOST_HAS_QWORD
      range->end.pos = ULL(0x7fffffffffffffff);
#else /* !__GNUC__ */
      range->end.pos = 281474976645120.0;
#endif /* __GNUC__ */
    }

  /* no error */
  return NULL;
}

/* print execution range *RANGE */
void
range_print_range(struct range_range_t *range,	/* execution range */
		  FILE *stream)			/* output stream */
{
  range_print_pos(&range->start, stream);
  fprintf(stream, ":");
  range_print_pos(&range->end, stream);
}

/* determine if inputs match execution position */
int						/* relation to position */
range_cmp_pos(struct range_pos_t *pos,		/* execution position */
	      counter_t val)			/* position value */
{
  if (val < pos->pos)
    return /* before */-1;
  else if (val == pos->pos)
    return /* equal */0;
  else /* if (pos->pos < val) */
    return /* after */1;
}

/* determine if inputs are in range */
int						/* relation to range */
range_cmp_range(struct range_range_t *range,	/* execution range */
		counter_t val)			/* position value */
{
  if (range->start.ptype != range->end.ptype)
    panic("invalid range");

  if (val < range->start.pos)
    return /* before */-1;
  else if (range->start.pos <= val && val <= range->end.pos)
    return /* inside */0;
  else /* if (range->end.pos < val) */
    return /* after */1;
}

/* determine if inputs are in range, passes all possible info needed */
int						/* relation to range */
range_cmp_range1(struct range_range_t *range,	/* execution range */
		 md_addr_t addr,		/* address value */
		 counter_t icount,		/* instruction count */
		 counter_t cycle)		/* cycle count */
{
  if (range->start.ptype != range->end.ptype)
    panic("invalid range");

  switch (range->start.ptype)
    {
    case pt_addr:
      if (addr < (md_addr_t)range->start.pos)
	return /* before */-1;
      else if ((md_addr_t)range->start.pos <= addr && addr <= (md_addr_t)range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < addr) */
	return /* after */1;
      break;
    case pt_inst:
      if (icount < range->start.pos)
	return /* before */-1;
      else if (range->start.pos <= icount && icount <= range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < icount) */
	return /* after */1;
      break;
    case pt_cycle:
      if (cycle < range->start.pos)
	return /* before */-1;
      else if (range->start.pos <= cycle && cycle <= range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < cycle) */
	return /* after */1;
      break;
    default:
      panic("bogus range type");
    }
}

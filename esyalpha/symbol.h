/*
 * symbol.h - program symbol and line data interfaces
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
 * $Id: symbol.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: symbol.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:14  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:54  garg
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
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.2  1998/08/27 16:45:17  taustin
 * implemented host interface description in host.h
 * added target interface support
 * moved target-dependent definitions to target files
 * added support for register and memory contexts
 *
 * Revision 1.1  1997/03/11  01:34:37  taustin
 * Initial revision
 *
 *
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "machine.h"

/*  for defining SMT   */
#include "smt.h"

enum sym_seg_t {
  ss_data,			/* data segment symbol */
  ss_text,			/* text segment symbol */
  ss_NUM
};

/* internal program symbol format */
struct sym_sym_t {
  char *name;			/* symbol name */
  enum sym_seg_t seg;		/* symbol segment */
  int initialized;		/* initialized? (if data segment) */
  int pub;			/* externally visible? */
  int local;			/* compiler local symbol? */
  md_addr_t addr;		/* symbol address value */
  int size;			/* bytes to next symbol */
};

/* symbol database in no particular order */
extern struct sym_sym_t *sym_db;

/* all symbol sorted by address */
extern int sym_nsyms;
extern struct sym_sym_t **sym_syms;

/* all symbols sorted by name */
extern struct sym_sym_t **sym_syms_by_name;

/* text symbols sorted by address */
extern int sym_ntextsyms;
extern struct sym_sym_t **sym_textsyms;

/* text symbols sorted by name */
extern struct sym_sym_t **sym_textsyms_by_name;

/* data symbols sorted by address */
extern int sym_ndatasyms;
extern struct sym_sym_t **sym_datasyms;

/* data symbols sorted by name */
extern struct sym_sym_t **sym_datasyms_by_name;

/* load symbols out of FNAME */
#ifdef SMT_SS
void
sym_loadsyms(char *fname,		/* file name containing symbols */
	     int load_locals, int threadid);		/* load local symbols */
#else
void
sym_loadsyms(char *fname,		/* file name containing symbols */
	     int load_locals);		/* load local symbols */

#endif

/* dump symbol SYM to output stream FD */
void
sym_dumpsym(struct sym_sym_t *sym,	/* symbol to display */
	    FILE *fd);			/* output stream */

/* dump all symbols to output stream FD */
void
sym_dumpsyms(FILE *fd);			/* output stream */

/* dump all symbol state to output stream FD */
void
sym_dumpstate(FILE *fd);		/* output stream */

/* symbol databases available */
enum sym_db_t {
  sdb_any,			/* search all symbols */
  sdb_text,			/* search text symbols */
  sdb_data,			/* search data symbols */
  sdb_NUM
};

/* bind address ADDR to a symbol in symbol database DB, the address must
   match exactly if EXACT is non-zero, the index of the symbol in the
   requested symbol database is returned in *PINDEX if the pointer is
   non-NULL */
struct sym_sym_t *			/* symbol found, or NULL */
sym_bind_addr(md_addr_t addr,		/* address of symbol to locate */
	      int *pindex,		/* ptr to index result var */
	      int exact,		/* require exact address match? */
	      enum sym_db_t db);	/* symbol database to search */

/* bind name NAME to a symbol in symbol database DB, the index of the symbol
   in the requested symbol database is returned in *PINDEX if the pointer is
   non-NULL */
struct sym_sym_t *				/* symbol found, or NULL */
sym_bind_name(const char *name,			/* symbol name to locate */
	      int *pindex,			/* ptr to index result var */
	      enum sym_db_t db);		/* symbol database to search */

#endif /* SYMBOL_H */


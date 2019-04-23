/*
 * eio.h - external interfaces to external I/O files
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1997, 1998 by Todd M. Austin
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
 * $Id: eio.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: eio.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:02  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:56  garg
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
 * Revision 1.1  1998/08/27 08:21:37  taustin
 * Initial revision
 *
 *
 */

#ifndef EIO_H
#define EIO_H

#include <stdio.h>

/* EIO file formats */
#define EIO_PISA_FORMAT			1
#define EIO_ALPHA_FORMAT		2

/* EIO file version */
#define EIO_FILE_VERSION		3

FILE *eio_create(char *fname);

FILE *eio_open(char *fname);

/* returns non-zero if file FNAME has a valid EIO header */
int eio_valid(char *fname);

void eio_close(FILE *fd);

/* check point current architected state to stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t
eio_write_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd);			/* stream to write to */

/* read check point of architected state from stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t
eio_read_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd);			/* stream to read */

/* syscall proxy handler, with EIO tracing support, architect registers
   and memory are assumed to be precise when this function is called,
   register and memory are updated with the results of the sustem call */
void
eio_write_trace(FILE *eio_fd,			/* EIO stream file desc */
		counter_t icnt,			/* instruction count */
		struct regs_t *regs,		/* registers to update */
		mem_access_fn mem_fn,		/* generic memory accessor */
		struct mem_t *mem,		/* memory to update */
		md_inst_t inst);		/* system call inst */

/* syscall proxy handler from an EIO trace, architect registers
   and memory are assumed to be precise when this function is called,
   register and memory are updated with the results of the sustem call */
void
eio_read_trace(FILE *eio_fd,			/* EIO stream file desc */
	       counter_t icnt,			/* instruction count */
	       struct regs_t *regs,		/* registers to update */
	       mem_access_fn mem_fn,		/* generic memory accessor */
	       struct mem_t *mem,		/* memory to update */
	       md_inst_t inst);			/* system call inst */

/* fast forward EIO trace EIO_FD to the transaction just after ICNT */
void eio_fast_forward(FILE *eio_fd, counter_t icnt);

#endif /* EIO_H */

/*
 * syscall.h - proxy system call handler interfaces
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
 * $Id: syscall.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: syscall.h,v $
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
 * Revision 1.1  2004/05/14 14:48:41  ninelson
 * Initial revision
 *
 * Revision 1.1.1.1  2003/10/20 13:58:35  ninelson
 * new from Ali
 *
 * Revision 1.5  1998/08/27 16:49:58  taustin
 * implemented host interface description in host.h
 * added target interface support
 * moved target-dependent definitions to target files
 * added support for register and memory contexts
 *
 * Revision 1.4  1997/03/11  01:36:51  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 * syscall structures are now more portable across platforms
 *
 * Revision 1.3  1996/12/27  15:56:56  taustin
 * updated comments
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 *
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/types.h>
#ifdef _MSC_VER
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "host.h"
#include "misc.h"
#include "machine.h"

/*
 * This module implements the system call portion of the SimpleScalar
 * instruction set architecture.  The system call definitions are borrowed
 * from Ultrix.  All system calls are executed by the simulator (the host) on
 * behalf of the simulated program (the target). The basic procedure for
 * implementing a system call is as follows:
 *
 *	1) decode the system call (this is the enum in "syscode")
 *	2) copy system call inputs in target (simulated program) memory
 *	   to host memory (simulator memory), note: the location and
 *	   amount of memory to copy is system call specific
 *	3) the simulator performs the system call on behalf of the target prog
 *	4) copy system call results in host memory to target memory
 *	5) set result register to indicate the error status of the system call
 *
 * That's it...  If you encounter an unimplemented system call and would like
 * to add support for it, first locate the syscode and arguments for the system
 * call when it occurs (do this in the debugger) and then implement a proxy
 * procedure in syscall.c.
 *
 */


/* syscall proxy handler, architect registers and memory are assumed to be
   precise when this function is called, register and memory are updated with
   the results of the sustem call */
#define MAXLOCK	 2048
#define MAXBARRIER	512
//#define IDEAL_LOCK
//#define LOCK_HARD
#ifdef LOCK_HARD
struct LOCK_ENTRY {
	int lock_owner;
	int waiting_list[MAXLOCK];
	int waiting_num;
	int waiting_head;
	int waiting_tail;
 } *LOCK_REG[256][MAXLOCK];

struct BAR_ENTRY {
	int barrier_num;
	int arriving_vector[256];	 
} *BAR_REG[256][MAXBARRIER];

struct LOCK_EVENT {
	int src1;
	int src2;
	int des1;
	int des2;
	int opt;
	int lock_id;
	int threadid;
	counter_t when;
    counter_t esynetMsgNo;
	struct LOCK_EVENT *next;
};
#define ACQUIRE			100
#define RELEASE			101
#define ACQ_REMOTE_ACC	102
#define REL_REMOTE_ACC	103
#define ACQ_ACK			104
#define WAIT_ACK		105
#define BAR_ACQUIRE		106
#define BAR_REMOTE_ACC	107
#define BAR_WAIT_ACK	108
#define BAR_RELEASE		109

struct LOCK_EVENT *lock_event_queue;

void lock_event_enqueue(struct LOCK_EVENT *event);     // event queue directory version, put dir event in queue
void lock_eventq_nextevent(void);
int Lock_operation (int lock_id, int threadid, int op);
#endif

void
sys_syscall(struct regs_t *regs,	/* registers to access */
	    mem_access_fn mem_fn,	/* generic memory accessor */
	    struct mem_t *mem,		/* memory space to access */
	    md_inst_t inst,		/* system call inst */
	    int traceable);		/* traceable system call? */

#endif /* SYSCALL_H */

/*
 * sim.h - simulator main line interfaces
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
 * $Id: sim.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: sim.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:29  xue
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
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.5  1998/08/27 16:38:54  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 * added simulator and program output redirection (via "-redir:sim"
 *       and "redir:prog" options, respectively)
 *
 * Revision 1.4  1997/03/11  01:41:43  taustin
 * updated copyrights
 *
 * Revision 1.3  1996/12/27  15:54:47  taustin
 * updated comments
 * integrated support for options and stats packages
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 *
 */

#ifndef SIM_H
#define SIM_H

#include <stdio.h>
#include <setjmp.h>
#include <time.h>

#include "options.h"
#include "stats.h"
#include "regs.h"
#include "memory.h"

#include "smt.h"

/* set to non-zero when simulator should dump statistics */
extern int sim_dump_stats;

/* exit when this becomes non-zero */
extern int sim_exit_now;

/* longjmp here when simulation is completed */
extern jmp_buf sim_exit_buf;

/* byte/word swapping required to execute target executable on this host */
extern int sim_swap_bytes;
extern int sim_swap_words;

/* execution instruction counter */
extern counter_t sim_num_insn;

/* execution start/end times */
extern time_t sim_start_time;
extern time_t sim_end_time;
extern int sim_elapsed_time;

/* options database */
extern struct opt_odb_t *sim_odb;

/* stats database */
extern struct stat_sdb_t *sim_sdb;

#ifndef SMT_SS
/* EIO interfaces */
extern char *sim_eio_fname;
extern char *sim_chkpt_fname;
extern FILE *sim_eio_fd;
#endif
extern FILE *sim_eio_fd;
/* redirected program/simulator output file names */
extern FILE *sim_progfd;


/*
 * main simulator interfaces, called in the following order
 */

/* register simulator-specific options */
void sim_reg_options(struct opt_odb_t *odb);

/* main() parses options next... */

/* check simulator-specific option values */
void sim_check_options(struct opt_odb_t *odb, int argc, char **argv);

/* register simulator-specific statistics */
void sim_reg_stats(struct stat_sdb_t *sdb);

/* initialize the simulator */
void sim_init(void);

#ifdef SMT_SS
/* load threads into simulated state */
void sim_load_threads(int argc, char **argv, char **envp);
#endif

/* load program into simulated state */
void sim_load_prog(char *fname, int argc, char **argv, char **envp);

/* main() prints the option database values next... */

/* print simulator-specific configuration information */
void sim_aux_config(FILE *stream);

/* start simulation, program loaded, processor precise state initialized */
void sim_main(void);

/* main() prints the stats database values next... */

/* dump simulator-specific auxiliary simulator statistics */
void sim_aux_stats(FILE *stream);

/* un-initialize simulator-specific state */
void sim_uninit(void);

/* print all simulator stats */
void
sim_print_stats(FILE *fd);		/* output stream */

#endif /* SIM_H */

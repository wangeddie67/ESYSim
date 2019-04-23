/*
 * main.c - main line routines
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 *
 * Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000 by Todd M. Austin
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
 * $Id: main.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: main.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:03  xue
 * CMP_NETWORK
 *
 * Revision 1.3  2009/03/17 13:19:46  garg
 * Bug fixes: Application stack size and emulation of the parallel code
 *
 * Revision 1.2  2009/03/05 16:58:23  garg
 * Updated version of the CMP network simulator.
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.11  2004/08/16 14:19:13  grbriggs
 * Show compiler version in simulator output.
 *
 * Revision 1.10  2004/07/24 04:14:46  grbriggs
 * Rearrange a little statistic-printing code.
 *
 * Revision 1.9  2004/07/22 18:01:01  grbriggs
 * Make the compile time less deceiving.
 *
 * Revision 1.8  2004/07/22 17:02:35  grbriggs
 * Fix interconnect power stats for runs that run to completion.
 *
 * Revision 1.7  2004/07/22 15:30:24  grbriggs
 * Add compile time to output.
 *
 * Revision 1.6  2004/06/30 17:32:42  grbriggs
 * Add PROG_OUT_FROM_SIM_OUT which allows the simulated program
 * output to go a file named similarly to the main simulation output
 *
 * Revision 1.5  2004/06/26 02:58:29  ninelson
 * Updates for CLUSTERS, etc
 *
 * Revision 1.4  2004/06/23 15:44:02  ninelson
 * common_regs_s added, etc from garg.
 *
 * Revision 1.3  2004/05/14 14:06:52  ninelson
 * added hotspots, etc
 *
 * Revision 1.1.1.5  2004/05/13 15:11:46  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.4  2004/04/09 21:22:01  ninelson
 * added hotspot functions
 *
 * Revision 1.1.1.3  2003/11/14 22:39:35  ninelson
 * made changes to optical_link, among others
 *
 * Revision 1.1.1.2  2003/10/20 13:58:02  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/05/26 15:19:03  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.9  1999/12/31 18:37:40  taustin
 * renice support no longer interferes with extern renice mechanisms
 *
 * Revision 1.8  1998/08/31 17:09:49  taustin
 * dumbed down parts for MS VC++
 * added unistd.h include to remove warnings
 *
 * Revision 1.7  1998/08/27 08:39:28  taustin
 * added support for MS VC++ compilation
 * implemented host interface description in host.h
 * added target interface support
 * added simulator and program output redirection (via "-redir:sim"
 *       and "redir:prog" options, respectively)
 * added "-nice" option (available on all simulator) that resets
 *       simulator scheduling priority to specified level
 * added checkpoint support
 * implemented a more portable random() interface
 * all simulators now emit command line used to invoke them
 *
 * Revision 1.6  1997/04/16  22:09:20  taustin
 * added standalone loader support
 *
 * Revision 1.5  1997/03/11  01:13:36  taustin
 * updated copyright
 * random number generator seed option added
 * versioning format simplified (X.Y)
 * fast terminate (-q) option added
 * RUNNING flag now helps stats routine determine when to spew stats
 *
 * Revision 1.3  1996/12/27  15:52:20  taustin
 * updated comments
 * integrated support for options and stats packages
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/time.h>
#endif
#ifdef BFD_LOADER
#include <bfd.h>
#endif /* BFD_LOADER */

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "endian.h"
#include "version.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include "loader.h"
#include "sim.h"
#include "cache.h"
#include "hotspot.h"

/*  for defining SMT   */
#include "smt.h"

#ifdef SMT_SS
#include "context.h"
#endif

char *sim_str_dump = NULL;

/* Function prototypes for customized extra staistics */
void total_interconnect_power_stats (void);


int numThreads[MAXTHREADS];
int mta_maxthreads;

/* stats signal handler */
static void
signal_sim_stats (int sigtype)
{
    sim_dump_stats = TRUE;
}

/* exit signal handler */
static void
signal_exit_now (int sigtype)
{
    sim_exit_now = TRUE;
}

/* execution instruction counter */
counter_t sim_num_insn = 0;
counter_t num_of_invalidations = 0;




#if 0				/* not portable... :-( */
/* total simulator (data) memory usage */
unsigned int sim_mem_usage = 0;
#endif

/* execution start/end times */
time_t sim_start_time;
time_t sim_end_time;
int sim_elapsed_time;

#ifndef SMT_SS
/* byte/word swapping required to execute target executable on this host */
int sim_swap_bytes;
int sim_swap_words;
#endif

/* exit when this becomes non-zero */
int sim_exit_now = FALSE;

/* longjmp here when simulation is completed */
jmp_buf sim_exit_buf;

/* set to non-zero when simulator should dump statistics */
int sim_dump_stats = FALSE;

/* options database */
struct opt_odb_t *sim_odb = NULL;

/* stats database */
struct stat_sdb_t *sim_sdb = NULL;

#ifdef FIX_8_CLUSTER_BUG
extern struct opt_odb_t *odb;
#endif

#ifndef SMT_SS
/* EIO interfaces */
char *sim_eio_fname = NULL;
char *sim_chkpt_fname = NULL;
FILE *sim_eio_fd = NULL;
#endif

/* redirected program/simulator output file names */
char *sim_simout = NULL;

#ifndef SMT_SS
static char *sim_progout = NULL;
FILE *sim_progfd = NULL;
#endif

#ifdef  REMOVE_MY_CODE
/* byte/word swapping required to execute target executable on this host */
int sim_swap_bytes;
int sim_swap_words;

/* EIO interfaces */
char *sim_eio_fname = NULL;
char *sim_chkpt_fname = NULL;
FILE *sim_eio_fd = NULL;

/* redirected program/simulator output file names */
static char *sim_progout = NULL;
FILE *sim_progfd = NULL;
#endif

/* track first argument orphan, this is the program to execute */
static int exec_index = -1;

/* dump help information */
static int help_me;

/* random number generator seed */
static int rand_seed;

/* initialize and quit immediately */
static int init_quit;

#ifndef _MSC_VER
/* simulator scheduling priority */
static int nice_priority;
#endif
int ilink_run;
int em3d_run;

/* default simulator scheduling priority */
#define NICE_DEFAULT_VALUE		0

static int
orphan_fn (int i, int argc, char **argv)
{
    exec_index = i;
    return /* done */ FALSE;
}

static void
banner (FILE * fd, int argc, char **argv)
{
    char *s;

    fprintf (fd,
	     "%s: SimpleScalar/%s Tool Set version %d.%d of %s.\n"
	     "Copyright (c) 1994-2000 by Todd M. Austin.  All Rights Reserved.\n"
	     "This version of SimpleScalar is licensed for academic non-commercial use only.\n" "\n", ((s = strrchr (argv[0], '/')) ? s + 1 : argv[0]), VER_TARGET, VER_MAJOR, VER_MINOR, VER_UPDATE);
}

static void
usage (FILE * fd, int argc, char **argv)
{
    fprintf (fd, "Usage: %s {-options} executable {arguments}\n", argv[0]);
    opt_print_help (sim_odb, fd);
}

static int running = FALSE;

/* print all simulator stats */
void
sim_print_stats (FILE * fd)	/* output stream */
{
    if (!running)
	return;

    /* get stats time */
    sim_end_time = time ((time_t *) NULL);
    sim_elapsed_time = max2 (sim_end_time - sim_start_time, 1);

    /* compute this before calling stat_print_stats */
    total_interconnect_power_stats ();

    /* print simulation stats */
    fprintf (fd, "\nsim: ** simulation statistics **\n");
    stat_print_stats (sim_sdb, fd);
    sim_aux_stats (fd);
#ifdef ESYNET
    simEsynetReport(fd);
#endif
    fprintf (fd, "\n");
    if(hotspot_active)
    	hotspot_dumpstats();
    fflush(fd);
}

/* print stats, uninitialize simulator components, and exit w/ exitcode */
static void
exit_now (int exit_code)
{
    /* print simulation stats */
    sim_print_stats (stderr);

    /* un-initialize the simulator */
    sim_uninit ();
    if (exit_code)
	fprintf (stderr, "error: some function call exit_now with exit_code %d\n", exit_code);
    /* all done! */
    exit (exit_code);
}

void get_config(int P_config_i, FILE *total_Config_file, int *prefetch_Config)
{
    int i;
    for(i=0;i<16;i++)
    {
        fscanf(total_Config_file,"\t%d",&prefetch_Config[P_config_i+i*256]);
    }
}

int
main (int argc, char **argv, char **envp)
{
    char *s ;
    s = NULL ;
    int i, exit_code, j;

    int P_config_i, P_config_j;


#ifndef _MSC_VER
    /* catch SIGUSR1 and dump intermediate stats */
    signal (SIGUSR1, signal_sim_stats);

    /* catch SIGUSR2 and dump final stats and exit */
    signal (SIGUSR2, signal_exit_now);
#endif /* _MSC_VER */

    /* register an error handler */
    fatal_hook (sim_print_stats);

    /* set up a non-local exit point */
    if ((exit_code = setjmp (sim_exit_buf)) != 0)
    {
	/* special handling as longjmp cannot pass 0 */
	exit_now (exit_code - 1);
    }

    /* register global options */
    sim_odb = opt_new (orphan_fn);
    opt_reg_flag (sim_odb, "-h", "print help message", &help_me, /* default */ FALSE, /* !print */ FALSE, NULL);
    opt_reg_flag (sim_odb, "-v", "verbose operation", &verbose, /* default */ FALSE, /* !print */ FALSE, NULL);
#ifdef DEBUG
    opt_reg_flag (sim_odb, "-d", "enable debug message", &debugging, /* default */ FALSE, /* !print */ FALSE, NULL);
#endif /* DEBUG */
    opt_reg_flag (sim_odb, "-i", "start in Dlite debugger", &dlite_active, /* default */ FALSE, /* !print */ FALSE, NULL);
    opt_reg_int (sim_odb, "-seed", "random number generator seed (0 for timer seed)", &rand_seed, /* default */ 1, /* print */ TRUE, NULL);
    opt_reg_flag (sim_odb, "-q", "initialize and terminate immediately", &init_quit, /* default */ FALSE, /* !print */ FALSE, NULL);

#ifdef SMT_SS
    fprintf (stderr, "Note: -chkpt option is disabled for SMT version \n");
#else
    opt_reg_string (sim_odb, "-chkpt", "restore EIO trace execution from <fname>", &sim_chkpt_fname, /* default */ NULL, /* !print */ FALSE, NULL);
#endif

#ifdef REMOVE_MY_CODE
    opt_reg_string (sim_odb, "-chkpt", "restore EIO trace execution from <fname>", &sim_chkpt_fname, /* default */ NULL, /* !print */ FALSE, NULL);
#endif

    /* stdio redirection options */
    opt_reg_string (sim_odb, "-redir:sim", "redirect simulator output to file (non-interactive only)", &sim_simout,
		    /* default */ NULL, /* !print */ FALSE, NULL);
#ifdef SMT_SS
#ifndef PROG_OUT_FROM_SIM_OUT
    fprintf (stderr, "Note: -redir:prog option is controled in *.bnc configuration " "file of each program in SMT version \n");
#else // !PROG_OUT_FROM_SIM_OUT
    fprintf (stderr, "Note: -redir:prog option is controled by combining the *.bnc configuration " "setting with the -redir:sim option. \n");
#endif // !PROG_OUT_FROM_SIM_OUT
#else
    opt_reg_string (sim_odb, "-redir:prog", "redirect simulated program output to file", &sim_progout, /* default */ NULL, /* !print */ FALSE, NULL);
#endif

#ifdef REMOVE_MY_CODE
    opt_reg_string (sim_odb, "-redir:prog", "redirect simulated program output to file", &sim_progout, /* default */ NULL, /* !print */ FALSE, NULL);
#endif

    /* Dump files to load assembly code and dumping stats etc. */
    opt_reg_string (sim_odb, "-redir:dump", "redirect simulated program output to file", &sim_str_dump, /* default */ NULL, /* !print */ FALSE, NULL);

#ifndef _MSC_VER
    /* scheduling priority option */
    opt_reg_int (sim_odb, "-nice", "simulator scheduling priority", &nice_priority,
		 /* default */ NICE_DEFAULT_VALUE, /* print */ TRUE, NULL);
#endif

    /* register all simulator-specific options */
    sim_reg_options (sim_odb);

    /* parse simulator options */
    exec_index = -1;

#ifdef REMOVE_MY_CODE
    fprintf (stderr, "sim_odb %lu \n", (long) sim_odb);
    fprintf (stderr, "sim: command line: ");
    for (i = 0; i < argc; i++)
	fprintf (stderr, "%s ", argv[i]);
    fprintf (stderr, "\n");
    {
	char name[256];

	gethostname (name, 256);
	fprintf (stderr, "Run on %s\n", name);
    }
#endif
    opt_process_options (sim_odb, argc, argv);

    /* redirect I/O? */
    if (sim_simout != NULL)
    {
	/* send simulator non-interactive output (STDERR) to file SIM_SIMOUT */
	if (fflush (stderr))
	    fatal ("unable to flush stderr ");
	else
	{
	    if (!freopen (sim_simout, "w", stderr))
		fatal ("unable to redirect simulator output to file `%s'", sim_simout);
	}

    }

#ifndef SMT_SS
    if (sim_progout != NULL)
    {
	/* redirect simulated program output to file SIM_PROGOUT */
	sim_progfd = fopen (sim_progout, "w");
	if (!sim_progfd)
	    fatal ("unable to redirect program output to file `%s'", sim_progout);
    }
#endif

    /* need at least two argv values to run */
    if (argc < 2)
    {
	banner (stderr, argc, argv);
	usage (stderr, argc, argv);
	fprintf (stderr, "error:exit from main argc < 2\n");
	exit (1);
    }

    /* opening banner */
    banner (stderr, argc, argv);

    if (help_me)
    {
	/* print help message and exit */
	usage (stderr, argc, argv);
	fprintf (stderr, "error:exit from main help_me\n");
	exit (1);
    }

    /* seed the random number generator */
    if (rand_seed == 0)
    {
	/* seed with the timer value, true random */
	mysrand (time ((time_t *) NULL));
    }
    else
    {
	/* seed with default or user-specified random number generator seed */
	mysrand (rand_seed);
    }

    /* exec_index is set in orphan_fn() */
    if (exec_index == -1)
    {
	/* executable was not found */
	fprintf (stderr, "error: no executable specified\n");
	usage (stderr, argc, argv);
	exit (1);
    }
    /* else, exec_index points to simulated program arguments */

    /* check simulator-specific options */
    sim_check_options (sim_odb, argc, argv);

#ifndef _MSC_VER
    /* set simulator scheduling priority */
    if (nice (0) < nice_priority)
    {
	if (nice (nice_priority - nice (0)) < 0)
	    fatal ("could not renice simulator process");
    }
#endif
    /* emit the command line for later reuse */
    /* copied here for easier debugging */
    fprintf (stderr, "sim: command line: ");
    for (i = 0; i < argc; i++)
	fprintf (stderr, "%s ", argv[i]);
    fprintf (stderr, "\n");
    {
	char name[256];

	gethostname (name, 256);
	fprintf (stderr, "Run on %s\n", name);
    }


    /* default architected value... */
    sim_num_insn = 0;

#ifdef BFD_LOADER
    /* initialize the bfd library */
    bfd_init ();
#endif /* BFD_LOADER */

    /* initialize the instruction decoder */
    md_init_decoder ();

#ifndef SMT_SS
    /* initialize all simulation modules */
    sim_init ();
#endif

    /* initialize architected state */
#ifdef SMT_SS
    sim_load_threads (argc - exec_index, argv + exec_index, envp);
#else
    sim_load_prog (argv[exec_index], argc - exec_index, argv + exec_index, envp);
#endif

#ifdef SMT_SS
    /* initialize all simulation modules */
    sim_init ();
#endif

#ifndef PARALLEL_EMUL
    /* register all simulator stats */
    sim_sdb = stat_new ();
    sim_reg_stats (sim_sdb);
#endif
    
    /* record start of execution time, used in rate stats */
    sim_start_time = time ((time_t *) NULL);

    /* emit the command line for later reuse */
    fprintf (stderr, "sim: command line: ");
    for (i = 0; i < argc; i++)
	fprintf (stderr, "%s ", argv[i]);
    fprintf (stderr, "\n");
    {
	char name[256];

	gethostname (name, 256);
	fprintf (stderr, "Run on %s\n", name);
    }

    /* output simulation conditions */
    s = ctime (&sim_start_time);
    if (s[strlen (s) - 1] == '\n')
	s[strlen (s) - 1] = '\0';
#ifdef __VERSION__
#define COMPILEDWITH "gcc version " __VERSION__
#else
#define COMPILEDWITH "unknown compiler"
#endif
    fprintf (stderr, "\nsim: main.c compiled on " __DATE__ " at " __TIME__ " with " COMPILEDWITH);
    fprintf (stderr, "\nsim: simulation started @ %s, options follow:\n", s);
    opt_print_options (sim_odb, stderr, /* short */ TRUE, /* notes */ TRUE);
    sim_aux_config (stderr);
    fprintf (stderr, "\n");

    /* omit option dump time from rate stats */
    sim_start_time = time ((time_t *) NULL);

    if (init_quit)
	exit_now (0);

    running = TRUE;

    for (j = 0; j < 4; j++)
    {
	for (i = 0; i < NUM_COM_REGS; i++)
	{
	    common_regs_s[j][i].regs_lock = 0;
	    common_regs_s[j][i].address = 0;
	}
    }

    if(!strcmp(argv[9], "ilink1030.BNC"))
	ilink_run = 1;
    else
	ilink_run = 0;
    if(!strcmp(argv[9], "../../BNCfor16/em3d1030.BNC"))
	em3d_run = 1;
    else
	em3d_run = 0;
    sim_main ();

    /* simulation finished early */
    exit_now (0);
#ifdef __GNUC__
    return 0;
#endif
}

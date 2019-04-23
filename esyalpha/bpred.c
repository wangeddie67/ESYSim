/*
 * bpred.c - branch predictor routines
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
 * $Id: bpred.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: bpred.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:27  xue
 * CMP_NETWORK
 *
 * Revision 1.2  2009/05/15 16:17:13  garg
 * Changes in branch predictor statistics output
 *
 * Revision 1.1.1.1  2008/10/20 15:44:54  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.13  2004/07/21 15:18:47  grbriggs
 * Make 2bcgskew branch predictor shift register be per-thread.
 *
 * Revision 1.12  2004/07/20 20:43:25  grbriggs
 * Allow one to specify a different threadid for BTB entries.
 *
 * Revision 1.11  2004/05/24 17:11:19  grbriggs
 * some SMT changes for code copied from lechen
 *
 * Revision 1.10  2004/05/21 22:05:40  grbriggs
 * Updates so that the branch_????2 variables are per-thread.
 *
 * Revision 1.9  2004/05/21 19:04:58  grbriggs
 * Removed unneeded reference to bpred_update2()
 *
 * Revision 1.8  2004/05/21 15:50:12  grbriggs
 * More SMT-ifying of lechen's code, and other fixes for compile errors.
 *
 * Revision 1.7  2004/05/20 18:04:35  grbriggs
 * Copied the remaining code pieces for 2bcskew from lechen. Not tested.
 *
 * Revision 1.6  2004/05/20 15:57:41  grbriggs
 * Added some obvious and large pieces of lechen's gskew predictor. Still need
 * to find the smaller pieces.
 *
 * Revision 1.5  2004/05/14 13:44:20  ninelson
 * no more dos
 *
 * Revision 1.1.1.4  2004/05/13 15:11:46  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.3  2003/11/14 22:39:35  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.2  2003/10/20 13:58:02  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2003/01/09 04:26:41  essawy
 * SS_SMT initial import into CVS
 *
 * Revision 1.4  1998/08/27 07:56:23  taustin
 * implemented host interface description in host.h
 * return address stack (RAS) performance stats improved
 * explicit BTB sizing option added to branch predictors, use
 *       "-btb" option to configure BTB
 * added target configuration parameters to control branch
 *       predictor indexing
 * L2 index computation is more "compatible" to McFarling's
 *       verison of it, i.e., if the PC xor address component is only
 *       part of the index, take the lower order address bits for the
 *       other part of the index, rather than the higher order ones
 * return address stack (RAS) bug fixed (improves pred perf)
 *
 * Revision 1.1.1.1  1997/05/22 00:33:18  aklauser
 *
 * Revision 1.11  1997/05/01 20:23:00  skadron
 * BTB bug fixes; jumps no longer update direction state; non-taken
 *    branches non longer update BTB
 *
 * Revision 1.10  1997/05/01 00:05:42  skadron
 * Separated BTB from direction-predictor
 *
 * Revision 1.9  1997/04/30  01:42:42  skadron
 * 1. Not aggressively returning the BTB target regardless of hit on jump's,
 *    but instead returning just "taken" when it's a BTB miss yields an
 *    apparent epsilon performance improvement for cc1 and perl.
 * 2. Bug fix: if no retstack, treat return's as any other jump
 *
 * Revision 1.8  1997/04/29  23:50:33  skadron
 * Added r31 info to distinguish between return-JRs and other JRs for bpred
 *
 * Revision 1.7  1997/04/29  22:53:04  skadron
 * Hopefully bpred is now right: bpred now allocates entries only for
 *    branches; on a BTB miss it still returns a direction; and it uses a
 *    return-address stack.  Returns are not yet distinguished among JR's
 *
 * Revision 1.6  1997/04/28  17:37:02  skadron
 * Bpred now allocates entries for any instruction instead of only
 *    branches; also added return-address stack
 *
 * Revision 1.5  1997/04/24  16:57:21  skadron
 * Bpred used to return no prediction if the indexing branch didn't match
 *    in the BTB.  Now it can predict a direction even on a BTB address
 *    conflict
 *
 * Revision 1.4  1997/03/27  16:31:52  skadron
 * Fixed bug: sim-outorder calls bpred_after_priming(), even if no bpred
 *    exists.  Now we check for a null ptr.
 *
 * Revision 1.3  1997/03/25  16:16:33  skadron
 * Statistics now take account of priming: statistics report only
 *    post-prime info.
 *
 * Revision 1.2  1997/02/24  18:02:41  skadron
 * Fixed output format of a formula stat
 *
 * Revision 1.1  1997/02/16  22:23:54  skadron
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "bpred.h"

#include "smt.h"

#ifdef SMT_SS
#include "context.h"
#endif

/*lechen: use 2 combined predictors?*/
extern int combined2;

/*lechen: update branch history immediately?*/
extern int hist_imm;

/*lechen: branch tail of the branch buffer 2*/
extern int branch_tail2;

/*lechen: branch predictor type*/
extern char *pred_type;

/*lechen: branch predictor type 2*/
extern char *pred_type2;

extern int bpred_btb_use_masterid;

/* turn this on to enable the SimpleScalar 2.0 RAS bug */
/* #define RAS_BUG_COMPATIBLE */

/* create a branch predictor */
struct bpred_t *		/* branch predictory instance */
bpred_create (enum bpred_class class,	/* type of predictor to create */
	      unsigned int bimod_size,	/* bimod table size */
	      unsigned int l1size,	/* 2lev l1 table size */
	      unsigned int l2size,	/* 2lev l2 table size */
	      unsigned int meta_size,	/* meta table size */
	      unsigned int shift_width,	/* history register width */
	      unsigned int xor,	/* history xor address flag */
	      unsigned int btb_sets,	/* number of sets in BTB */
	      unsigned int btb_assoc,	/* BTB associativity */
	      unsigned int retstack_size)	/* num entries in ret-addr stack */
{
    struct bpred_t *pred;

    if (!(pred = calloc (1, sizeof (struct bpred_t))))
	fatal ("out of virtual memory");

    pred->class = class;

    switch (class)
    {
    case BPredComb:
	/* bimodal component */
	pred->dirpred.bimod = bpred_dir_create (BPred2bit, bimod_size, 0, 0, 0);

	/* 2-level component */
	pred->dirpred.twolev = bpred_dir_create (BPred2Level, l1size, l2size, shift_width, xor);

	/* metapredictor component */
	pred->dirpred.meta = bpred_dir_create (BPred2bit, meta_size, 0, 0, 0);

	break;

    case BPred2Level:
	pred->dirpred.twolev = bpred_dir_create (class, l1size, l2size, shift_width, xor);

	break;

#ifdef LOAD_PREDICTOR
    case LDPred2bit:
	pred->dirpred.bimod = bpred_dir_create (class, bimod_size, 0, 0, 0);

	break;
#endif

    case BPred2bit:
	pred->dirpred.bimod = bpred_dir_create (class, bimod_size, 0, 0, 0);

    case BPredTaken:
    case BPredNotTaken:
	/* no other state */
	break;

    default:
	panic ("bogus predictor class");
    }

    /* allocate ret-addr stack */
    switch (class)
    {
    case BPredComb:
    case BPred2Level:
    case BPred2bit:
	{
	    int i;

	    /* allocate BTB */
	    if (!btb_sets || (btb_sets & (btb_sets - 1)) != 0)
		fatal ("number of BTB sets must be non-zero and a power of two");
	    if (!btb_assoc || (btb_assoc & (btb_assoc - 1)) != 0)
		fatal ("BTB associativity must be non-zero and a power of two");

	    if (!(pred->btb.btb_data = calloc (btb_sets * btb_assoc, sizeof (struct bpred_btb_ent_t))))
		fatal ("cannot allocate BTB");

	    pred->btb.sets = btb_sets;
	    pred->btb.assoc = btb_assoc;

	    if (pred->btb.assoc > 1)
		for (i = 0; i < (pred->btb.assoc * pred->btb.sets); i++)
		{
		    if (i % pred->btb.assoc != pred->btb.assoc - 1)
			pred->btb.btb_data[i].next = &pred->btb.btb_data[i + 1];
		    else
			pred->btb.btb_data[i].next = NULL;

		    if (i % pred->btb.assoc != pred->btb.assoc - 1)
			pred->btb.btb_data[i + 1].prev = &pred->btb.btb_data[i];
		}

#ifdef SMT_SS
	    for (i = 0; i < MAXTHREADS; i++)
	    {
		/* allocate retstack */
		if ((retstack_size & (retstack_size - 1)) != 0)
		    fatal ("Return-address-stack size must be zero or a power of two");

		pred->retstack[i].size = retstack_size;
		if (retstack_size)
		    if (!(pred->retstack[i].stack = calloc (retstack_size, sizeof (struct bpred_btb_ent_t))))
			fatal ("cannot allocate return-address-stack");
		pred->retstack[i].tos = retstack_size - 1;
	    }
#else
	    /* allocate retstack */
	    if ((retstack_size & (retstack_size - 1)) != 0)
		fatal ("Return-address-stack size must be zero or a power of two");

	    pred->retstack.size = retstack_size;
	    if (retstack_size)
		if (!(pred->retstack.stack = calloc (retstack_size, sizeof (struct bpred_btb_ent_t))))
		    fatal ("cannot allocate return-address-stack");
	    pred->retstack.tos = retstack_size - 1;
#endif

	    break;
	}

    case BPredTaken:
    case BPredNotTaken:
#ifdef LOAD_PREDICTOR
    case LDPred2bit:
#endif
	/* no other state */
	break;

    default:
	panic ("bogus predictor class");
    }

    return pred;
}

/*lechen, create a 2Bc-gskew predictor*/
struct bpred_t *		/* branch predictory instance */
bpred_2bcgskew_create ( /*bimod table size */ unsigned int bimod_size,
		       /*other 3 tables size */ unsigned int size,
		       /*bimod table history length */ unsigned int shift_width_bimod,
		       /*g0 table history length */ unsigned int shift_width_g0,
		       /*g1 table history length */ unsigned int shift_width_g1,
		       /*meta table history length */ unsigned int shift_width_meta,
		       /* btb sets */ unsigned int btb_sets,
		       /* btb assoc */ unsigned int btb_assoc,
		       /* ret-addr stack size */ unsigned int retstack_size)
{
    struct bpred_t *pred;

    if (!(pred = calloc (1, sizeof (struct bpred_t))))
	fatal ("out of virtual memory");

    pred->class = BPred2bcgskew;

    pred->dirpred.bim1 = bpred_dir_create (BPred2bcgskew, bimod_size, 0, shift_width_bimod, 0);
    pred->dirpred.g01 = bpred_dir_create (BPred2bcgskew, size, 0, shift_width_g0, 0);
    pred->dirpred.g11 = bpred_dir_create (BPred2bcgskew, size, 0, shift_width_g1, 0);
    pred->dirpred.meta = bpred_dir_create (BPred2bcgskew, size, 0, shift_width_meta, 0);

    {
	int i;

	/* allocate BTB */
	if (!btb_sets || (btb_sets & (btb_sets - 1)) != 0)
	    fatal ("number of BTB sets must be non-zero and a power of two");
	if (!btb_assoc || (btb_assoc & (btb_assoc - 1)) != 0)
	    fatal ("BTB associativity must be non-zero and a power of two");

	if (!(pred->btb.btb_data = calloc (btb_sets * btb_assoc, sizeof (struct bpred_btb_ent_t))))
	    fatal ("cannot allocate BTB");

	pred->btb.sets = btb_sets;
	pred->btb.assoc = btb_assoc;

	if (pred->btb.assoc > 1)
	    for (i = 0; i < (pred->btb.assoc * pred->btb.sets); i++)
	    {
		if (i % pred->btb.assoc != pred->btb.assoc - 1)
		    pred->btb.btb_data[i].next = &pred->btb.btb_data[i + 1];
		else
		    pred->btb.btb_data[i].next = NULL;

		if (i % pred->btb.assoc != pred->btb.assoc - 1)
		    pred->btb.btb_data[i + 1].prev = &pred->btb.btb_data[i];
	    }

#ifdef SMT_SS
	for (i = 0; i < MAXTHREADS; i++)
	{
	    /* allocate retstack */
	    if ((retstack_size & (retstack_size - 1)) != 0)
		fatal ("Return-address-stack size must be zero or a power of two");

	    pred->retstack[i].size = retstack_size;
	    if (retstack_size)
		if (!(pred->retstack[i].stack = calloc (retstack_size, sizeof (struct bpred_btb_ent_t))))
		    fatal ("cannot allocate return-address-stack");
	    pred->retstack[i].tos = retstack_size - 1;
	}
#else
	/* allocate retstack */
	if ((retstack_size & (retstack_size - 1)) != 0)
	    fatal ("Return-address-stack size must be zero or a power of two");

	pred->retstack.size = retstack_size;
	if (retstack_size)
	    if (!(pred->retstack.stack = calloc (retstack_size, sizeof (struct bpred_btb_ent_t))))
		fatal ("cannot allocate return-address-stack");
	pred->retstack.tos = retstack_size - 1;
#endif

    }

    return pred;
}

/* also from lechen */
#if 0				/*grbriggs: this was only needed for combined2 */
void
bpred_2bcgskew_create2 ( /*bimod table size */ unsigned int bimod_size,
			/*other 3 tables size */ unsigned int size,
			/*bimod table history length */ unsigned int shift_width_bimod,
			/*g0 table history length */ unsigned int shift_width_g0,
			/*g1 table history length */ unsigned int shift_width_g1,
			/*meta table history length */ unsigned int shift_width_meta,
			/*pred */ struct bpred_t *pred)
{
    pred->dirpred.bim2 = bpred_dir_create (BPred2bcgskew, bimod_size, 0, shift_width_bimod, 0);
    pred->dirpred.g02 = bpred_dir_create (BPred2bcgskew, size, 0, shift_width_g0, 0);
    pred->dirpred.g12 = bpred_dir_create (BPred2bcgskew, size, 0, shift_width_g1, 0);
    pred->dirpred.meta2 = bpred_dir_create (BPred2bcgskew, size, 0, shift_width_meta, 0);
}
#endif

/* create a branch direction predictor */
struct bpred_dir_t *		/* branch direction predictor instance */
bpred_dir_create (enum bpred_class class,	/* type of predictor to create */
		  unsigned int l1size,	/* level-1 table size */
		  unsigned int l2size,	/* level-2 table size (if relevant) */
		  unsigned int shift_width,	/* history register width */
		  unsigned int xor)	/* history xor address flag */
{
    struct bpred_dir_t *pred_dir;
    unsigned int cnt;
    int flipflop, i;

    if (!(pred_dir = calloc (1, sizeof (struct bpred_dir_t))))
	fatal ("out of virtual memory");

    pred_dir->class = class;

    cnt = -1;
    switch (class)
    {
/*lechen*/
    case BPred2bcgskew:
	{
	    float log2_size;

	    if (!l1size || (l1size & (l1size - 1)) != 0)
		fatal ("level-1 size, `%d', must be non-zero and a power of two", l1size);
	    pred_dir->config.bank.size = l1size;
	    log2_size = log (l1size) / log (2);
	    pred_dir->config.bank.log_size = (int) log2_size;

	    pred_dir->config.bank.shift_width = shift_width;

	    pred_dir->config.bank.table = calloc (l1size, sizeof (unsigned char));
	    if (!pred_dir->config.bank.table)
		fatal ("cannot allocate 2Bc-skew table");

	    /* initialize counters to weakly this-or-that */
	    flipflop = 1;
	    for (cnt = 0; cnt < l1size; cnt++)
	    {
		pred_dir->config.bank.table[cnt] = flipflop;
		flipflop = 3 - flipflop;
	    }

	    break;
	}

    case BPred2Level:
	{
	    if (!l1size || (l1size & (l1size - 1)) != 0)
		fatal ("level-1 size, `%d', must be non-zero and a power of two", l1size);
	    pred_dir->config.two.l1size = l1size;

	    if (!l2size || (l2size & (l2size - 1)) != 0)
		fatal ("level-2 size, `%d', must be non-zero and a power of two", l2size);
	    pred_dir->config.two.l2size = l2size;

	    if (!shift_width || shift_width > 30)
		fatal ("shift register width, `%d', must be non-zero and positive", shift_width);
	    pred_dir->config.two.shift_width = shift_width;

	    pred_dir->config.two.xor = xor;
#ifdef SMT_SS
	    for (i = 0; i < MAXTHREADS; i++)
	    {
		pred_dir->config.two.shiftregs[i] = calloc (l1size, sizeof (int));
		if (!pred_dir->config.two.shiftregs[i])
		    fatal ("cannot allocate shift register table");
	    }
#else
	    pred_dir->config.two.shiftregs = calloc (l1size, sizeof (int));
	    if (!pred_dir->config.two.shiftregs)
		fatal ("cannot allocate shift register table");
#endif

	    pred_dir->config.two.l2table = calloc (l2size, sizeof (unsigned char));
	    if (!pred_dir->config.two.l2table)
		fatal ("cannot allocate second level table");

	    /* initialize counters to weakly this-or-that */
	    flipflop = 1;
	    for (cnt = 0; cnt < l2size; cnt++)
	    {
		pred_dir->config.two.l2table[cnt] = flipflop;
		flipflop = 3 - flipflop;
	    }

	    break;
	}

#ifdef LOAD_PREDICTOR
    case LDPred2bit:
	if (!l1size || (l1size & (l1size - 1)) != 0)
	    fatal ("2bit table size, `%d', must be non-zero and a power of two", l1size);
	pred_dir->config.bimod.size = l1size;
	if (!(pred_dir->config.bimod.table = calloc (l1size, sizeof (unsigned char))))
	    fatal ("cannot allocate 2bit storage");
	/* initialize counters to weakly this-or-that */
	flipflop = 1;
	for (cnt = 0; cnt < l1size; cnt++)
	{
	    pred_dir->config.bimod.table[cnt] = flipflop;
	    flipflop = 3 - flipflop;
	}

	break;
#endif

    case BPred2bit:
	if (!l1size || (l1size & (l1size - 1)) != 0)
	    fatal ("2bit table size, `%d', must be non-zero and a power of two", l1size);
	pred_dir->config.bimod.size = l1size;
	if (!(pred_dir->config.bimod.table = calloc (l1size, sizeof (unsigned char))))
	    fatal ("cannot allocate 2bit storage");
	/* initialize counters to weakly this-or-that */
	flipflop = 1;
	for (cnt = 0; cnt < l1size; cnt++)
	{
	    pred_dir->config.bimod.table[cnt] = flipflop;
	    flipflop = 3 - flipflop;
	}

	break;

    case BPredTaken:
    case BPredNotTaken:
	/* no other state */
	break;

    default:
	panic ("bogus branch direction predictor class");
    }

    return pred_dir;
}

/* print branch direction predictor configuration */
void
bpred_dir_config (struct bpred_dir_t *pred_dir,	/* branch direction predictor instance */
		  char name[],	/* predictor name */
		  FILE * stream)	/* output stream */
{
    switch (pred_dir->class)
    {
    case BPred2Level:
	fprintf (stream,
		 "pred_dir: %s: 2-lvl: %d l1-sz, %d bits/ent, %s xor, %d l2-sz, direct-mapped\n", name, pred_dir->config.two.l1size, pred_dir->config.two.shift_width, pred_dir->config.two.xor ? "" : "no", pred_dir->config.two.l2size);
	break;

#ifdef LOAD_PREDICTOR
    case LDPred2bit:
	fprintf (stream, "load_pred_hit: %s: 2-bit: %d entries, direct-mapped\n", name, pred_dir->config.bimod.size);
	break;
#endif

    case BPred2bit:
	fprintf (stream, "pred_dir: %s: 2-bit: %d entries, direct-mapped\n", name, pred_dir->config.bimod.size);
	break;

    case BPredTaken:
	fprintf (stream, "pred_dir: %s: predict taken\n", name);
	break;

    case BPredNotTaken:
	fprintf (stream, "pred_dir: %s: predict not taken\n", name);
	break;

    default:
	panic ("bogus branch direction predictor class");
    }
}

/* print branch predictor configuration */
void
bpred_config (struct bpred_t *pred,	/* branch predictor instance */
	      FILE * stream)	/* output stream */
{
    switch (pred->class)
    {
    case BPredComb:
	bpred_dir_config (pred->dirpred.bimod, "bimod", stream);
	bpred_dir_config (pred->dirpred.twolev, "2lev", stream);
	bpred_dir_config (pred->dirpred.meta, "meta", stream);
	fprintf (stream, "btb: %d sets x %d associativity", pred->btb.sets, pred->btb.assoc);
#ifdef SMT_SS
	/* at least one thread is running, also stack size is uniform for all threads */
	fprintf (stream, "ret_stack: %d entries", pred->retstack[0].size);
#else
	fprintf (stream, "ret_stack: %d entries", pred->retstack.size);
#endif
	break;

    case BPred2Level:
	bpred_dir_config (pred->dirpred.twolev, "2lev", stream);
	fprintf (stream, "btb: %d sets x %d associativity", pred->btb.sets, pred->btb.assoc);
#ifdef SMT_SS
	/* at least one thread is running, also stack size is uniform for all threads */
	fprintf (stream, "ret_stack: %d entries", pred->retstack[0].size);
#else
	fprintf (stream, "ret_stack: %d entries", pred->retstack.size);
#endif
	break;


#ifdef LOAD_PREDICTOR
    case LDPred2bit:
	bpred_dir_config (pred->dirpred.bimod, "bimod", stream);
	break;
#endif

    case BPred2bit:
	bpred_dir_config (pred->dirpred.bimod, "bimod", stream);
	fprintf (stream, "btb: %d sets x %d associativity", pred->btb.sets, pred->btb.assoc);
#ifdef SMT_SS
	/* at least one thread is running, also stack size is uniform for all threads */
	fprintf (stream, "ret_stack: %d entries", pred->retstack[0].size);
#else
	fprintf (stream, "ret_stack: %d entries", pred->retstack.size);
#endif
	break;

    case BPredTaken:
	bpred_dir_config (pred->dirpred.bimod, "taken", stream);
	break;
    case BPredNotTaken:
	bpred_dir_config (pred->dirpred.bimod, "nottaken", stream);
	break;

    default:
	panic ("bogus branch predictor class");
    }
}

/* print predictor stats */
void
bpred_stats (struct bpred_t *pred,	/* branch predictor instance */
	     FILE * stream)	/* output stream */
{
    fprintf (stream, "pred: addr-prediction rate = %f\n", (double) pred->addr_hits / (double) (pred->addr_hits + pred->misses));
    fprintf (stream, "pred: dir-prediction rate = %f\n", (double) pred->dir_hits / (double) (pred->dir_hits + pred->misses));
}

/* register branch predictor stats */
void
bpred_reg_stats (struct bpred_t *pred,	/* branch predictor instance */
		 struct stat_sdb_t *sdb, int threadid)	/* stats database */
{
    char buf[512], buf1[512], *name, name1[256];

    /* get a name for this predictor */
    switch (pred->class)
    {
    case BPredComb:
	name = "bpred_comb";
	break;
    case BPred2Level:
	name = "bpred_2lev";
	break;
#ifdef LOAD_PREDICTOR
    case LDPred2bit:
	name = "ldpred_bimod";
	break;
#endif
    case BPred2bit:
	name = "bpred_bimod";
	break;
    case BPredTaken:
	name = "bpred_taken";
	break;
    case BPredNotTaken:
	name = "bpred_nottaken";
	break;
/*lechen, for 2Bc-gskew predictor*/
    case BPred2bcgskew:
	name = "bpred_2bcgskew";
	break;

    default:
	panic ("bogus branch predictor class");
    }
	strcpy(name1, name);
	name = name1;

	sprintf(buf, "_%d", threadid);
	strcat(name, buf);

    sprintf (buf, "%s.lookups", name);
    stat_reg_counter (sdb, buf, "total number of bpred lookups", &pred->lookups, 0, NULL);
    sprintf (buf, "%s.updates", name);
    sprintf (buf1, "%s.dir_hits + %s.misses", name, name);
    stat_reg_formula (sdb, buf, "total number of updates", buf1, "%12.0f");
    sprintf (buf, "%s.addr_hits", name);
    stat_reg_counter (sdb, buf, "total number of address-predicted hits", &pred->addr_hits, 0, NULL);
    sprintf (buf, "%s.dir_hits", name);
    stat_reg_counter (sdb, buf, "total number of direction-predicted hits " "(includes addr-hits)", &pred->dir_hits, 0, NULL);
    if (pred->class == BPredComb)
    {
	sprintf (buf, "%s.used_bimod", name);
	stat_reg_counter (sdb, buf, "total number of bimodal predictions used", &pred->used_bimod, 0, NULL);
	sprintf (buf, "%s.used_2lev", name);
	stat_reg_counter (sdb, buf, "total number of 2-level predictions used", &pred->used_2lev, 0, NULL);
    }
    sprintf (buf, "%s.misses", name);
    stat_reg_counter (sdb, buf, "total number of misses", &pred->misses, 0, NULL);
    sprintf (buf, "%s.jr_hits", name);
    stat_reg_counter (sdb, buf, "total number of address-predicted hits for JR's", &pred->jr_hits, 0, NULL);
    sprintf (buf, "%s.jr_seen", name);
    stat_reg_counter (sdb, buf, "total number of JR's seen", &pred->jr_seen, 0, NULL);
    sprintf (buf, "%s.jr_non_ras_hits.PP", name);
    stat_reg_counter (sdb, buf, "total number of address-predicted hits for non-RAS JR's", &pred->jr_non_ras_hits, 0, NULL);
    sprintf (buf, "%s.jr_non_ras_seen.PP", name);
    stat_reg_counter (sdb, buf, "total number of non-RAS JR's seen", &pred->jr_non_ras_seen, 0, NULL);
    sprintf (buf, "%s.bpred_addr_rate", name);
    sprintf (buf1, "%s.addr_hits / %s.updates", name, name);
    stat_reg_formula (sdb, buf, "branch address-prediction rate (i.e., addr-hits/updates)", buf1, "%9.4f");
    sprintf (buf, "%s.bpred_dir_rate", name);
    sprintf (buf1, "%s.dir_hits / %s.updates", name, name);
    stat_reg_formula (sdb, buf, "branch direction-prediction rate (i.e., all-hits/updates)", buf1, "%9.4f");
    sprintf (buf, "%s.bpred_jr_rate", name);
    sprintf (buf1, "%s.jr_hits / %s.jr_seen", name, name);
    stat_reg_formula (sdb, buf, "JR address-prediction rate (i.e., JR addr-hits/JRs seen)", buf1, "%9.4f");
    sprintf (buf, "%s.bpred_jr_non_ras_rate.PP", name);
    sprintf (buf1, "%s.jr_non_ras_hits.PP / %s.jr_non_ras_seen.PP", name, name);
    stat_reg_formula (sdb, buf, "non-RAS JR addr-pred rate (ie, non-RAS JR hits/JRs seen)", buf1, "%9.4f");
    sprintf (buf, "%s.retstack_pushes", name);
    stat_reg_counter (sdb, buf, "total number of address pushed onto ret-addr stack", &pred->retstack_pushes, 0, NULL);
    sprintf (buf, "%s.retstack_pops", name);
    stat_reg_counter (sdb, buf, "total number of address popped off of ret-addr stack", &pred->retstack_pops, 0, NULL);
    sprintf (buf, "%s.used_ras.PP", name);
    stat_reg_counter (sdb, buf, "total number of RAS predictions used", &pred->used_ras, 0, NULL);
    sprintf (buf, "%s.ras_hits.PP", name);
    stat_reg_counter (sdb, buf, "total number of RAS hits", &pred->ras_hits, 0, NULL);
    sprintf (buf, "%s.ras_rate.PP", name);
    sprintf (buf1, "%s.ras_hits.PP / %s.used_ras.PP", name, name);
    stat_reg_formula (sdb, buf, "RAS prediction rate (i.e., RAS hits/used RAS)", buf1, "%9.4f");
}

void
bpred_after_priming (struct bpred_t *bpred)
{
    if (bpred == NULL)
	return;

    bpred->lookups = 0;
    bpred->addr_hits = 0;
    bpred->dir_hits = 0;
    bpred->used_ras = 0;
    bpred->used_bimod = 0;
    bpred->used_2lev = 0;
    bpred->jr_hits = 0;
    bpred->jr_seen = 0;
    bpred->misses = 0;
    bpred->retstack_pops = 0;
    bpred->retstack_pushes = 0;
    bpred->ras_hits = 0;
}

#define BIMOD_HASH(PRED, ADDR)						\
  ((((ADDR) >> 19) ^ ((ADDR) >> MD_BR_SHIFT)) & ((PRED)->config.bimod.size-1))
    /* was: ((baddr >> 16) ^ baddr) & (pred->dirpred.bimod.size-1) */

/* predicts a branch direction */
char *				/* pointer to counter */
bpred_dir_lookup (struct bpred_dir_t *pred_dir,	/* branch dir predictor inst */
#ifdef SMT_SS
		  md_addr_t baddr,	/* branch address */
		  int threadid)
#else
		  md_addr_t baddr)	/* branch address */
#endif
{
    unsigned char *p = NULL;

    /* Except for jumps, get a pointer to direction-prediction bits */
    switch (pred_dir->class)
    {
    case BPred2Level:
	{
	    int l1index, l2index;

	    /* traverse 2-level tables */
	    l1index = (baddr >> MD_BR_SHIFT) & (pred_dir->config.two.l1size - 1);
#ifdef SMT_SS
	    l2index = pred_dir->config.two.shiftregs[threadid][l1index];
#else
	    l2index = pred_dir->config.two.shiftregs[l1index];
#endif
	    if (pred_dir->config.two.xor)
	    {
#if 1
		/* this L2 index computation is more "compatible" to McFarling's
		   verison of it, i.e., if the PC xor address component is only
		   part of the index, take the lower order address bits for the
		   other part of the index, rather than the higher order ones */
		l2index = (((l2index ^ (baddr >> MD_BR_SHIFT)) & ((1 << pred_dir->config.two.shift_width) - 1)) | ((baddr >> MD_BR_SHIFT) << pred_dir->config.two.shift_width));
#else
		l2index = l2index ^ (baddr >> MD_BR_SHIFT);
#endif
	    }
	    else
	    {
		l2index = l2index | ((baddr >> MD_BR_SHIFT) << pred_dir->config.two.shift_width);
	    }
	    l2index = l2index & (pred_dir->config.two.l2size - 1);

	    /* get a pointer to prediction state information */
	    p = &pred_dir->config.two.l2table[l2index];
	}
	break;
#ifdef LOAD_PREDICTOR
    case LDPred2bit:
	p = &pred_dir->config.bimod.table[BIMOD_HASH (pred_dir, baddr)];
	break;
#endif
    case BPred2bit:
	p = &pred_dir->config.bimod.table[BIMOD_HASH (pred_dir, baddr)];
	break;
    case BPredTaken:
    case BPredNotTaken:
	break;
    default:
	panic ("bogus branch direction predictor class");
    }

    return (char *) p;
}

/*lechen, H and Hbar functions for 2Bc-gskew, from CACHESKEW simulator, see 
* "Skewed Associative Caches," and/or
* "Trading Conflict and Capacity Aliasing in Conditional Branch Predictors,"
* etc. for more details
* note: the functions are not exactly the same in format, but should be
* identical in function*/
int
H_func (int Y, int shift, int Mask)
{
    int y1, yn, result;

    y1 = Y >> (shift - 1);
    yn = Y & 0x01;

    result = ((Y * 2) + (yn ^ y1)) & Mask;
    return (result);
}

int
Hbar_func (int Y, int shift, int Mask)	/*reverse of H_func */
{
    int y1, yn, result;

    y1 = (Y & 0x02) >> 1;
    yn = (y1 ^ Y) & 1;
    result = (Y >> 1) + (yn << (shift - 1));

    return (result);
}

/*NOTE: 32-bit unsigned int*/

/*lechen, speculatively update branch history*/
void
bpred_update_history (struct bpred_t *pred, struct bpred_update_t *dir_update_ptr
#ifdef SMT_SS
		      , int threadid	/* grbriggs: added so 2bcgskew uses SMT */
#endif
    )
{
    int pred_taken, shift_reg;
    context *current = thecontexts[threadid];	/* grbriggs: added so 2bcgskew uses SMT */

    dir_update_ptr->branch_tail2 = current->branch_tail2;

    switch (pred->class)
    {
#if 0				/*copied from lechen but unused */
    case BPredComb:
    case BPred2Level:
	pred_taken = *(dir_update_ptr->pdir1) > 1;
	/*keep a copy, used for recovery */
	dir_update_ptr->history = pred->dirpred.twolev->config.two.shiftregs[0];
	dir_update_ptr->hist_shift_out = dir_update_ptr->history >> 31;
	shift_reg = (dir_update_ptr->history << 1) | (!!pred_taken);
	pred->dirpred.twolev->config.two.shiftregs[0] = shift_reg /*& ((1 << pred->dirpred.twolev->config.two.shift_width) - 1) */ ;
	break;
#endif
    case BPred2bcgskew:
	pred_taken = dir_update_ptr->pred_taken;
	/*keep a copy, used for recovery */
#ifdef SMT_SS
	dir_update_ptr->history = pred->dirpred.bim1->config.bank.shiftreg[threadid];
#else
	dir_update_ptr->history = pred->dirpred.bim1->config.bank.shiftreg;
#endif
	dir_update_ptr->hist_shift_out = dir_update_ptr->history >> 31;
	shift_reg = (dir_update_ptr->history << 1) | (!!pred_taken);
	/*only the lower bits are useful though */
#ifdef SMT_SS
	pred->dirpred.bim1->config.bank.shiftreg[threadid] = pred->dirpred.g01->config.bank.shiftreg[threadid] = pred->dirpred.g11->config.bank.shiftreg[threadid] = pred->dirpred.meta->config.bank.shiftreg[threadid] = shift_reg;
#else
	pred->dirpred.bim1->config.bank.shiftreg = pred->dirpred.g01->config.bank.shiftreg = pred->dirpred.g11->config.bank.shiftreg = pred->dirpred.meta->config.bank.shiftreg = shift_reg;
#endif
	break;
#if 0				/*copied from lechen but unused */
    case BPredARVI:
	break;
#endif
    default:
	panic ("sth wrong here in bpred_update_history");
    }

    if (combined2 || (pred->class == BPredARVI))
    {

	/*need to update history for 2 */

#if 0				/*copied from lechen but unused */
	if (!mystricmp (pred_type2, "comb") || !mystricmp (pred_type2, "2lev"))
	{
	    pred_taken = *(dir_update_ptr->pdir12) > 1;
	    /*keep a copy, used for recovery */
	    dir_update_ptr->history2 = pred->dirpred.twolev2->config.two.shiftregs[0];
	    dir_update_ptr->hist_shift_out2 = dir_update_ptr->history2 >> 31;
	    shift_reg = (dir_update_ptr->history2 << 1) | (!!pred_taken);
	    pred->dirpred.twolev2->config.two.shiftregs[0] = shift_reg /*& ((1 << pred->dirpred.twolev2->config.two.shift_width) - 1) */ ;
	}
	else
#endif
	if (!mystricmp (pred_type2, "2bcgskew"))
	{
	    pred_taken = dir_update_ptr->pred_taken2;
	    /*keep a copy, used for recovery */
#ifdef SMT_SS
	    dir_update_ptr->history2 = pred->dirpred.bim2->config.bank.shiftreg[threadid];
#else
	    dir_update_ptr->history2 = pred->dirpred.bim2->config.bank.shiftreg;
#endif
	    dir_update_ptr->hist_shift_out2 = dir_update_ptr->history2 >> 31;
	    shift_reg = (dir_update_ptr->history2 << 1) | (!!pred_taken);
	    /*only the lower bits are useful though */
#ifdef SMT_SS
	    pred->dirpred.bim2->config.bank.shiftreg[threadid] = pred->dirpred.g02->config.bank.shiftreg[threadid] = pred->dirpred.g12->config.bank.shiftreg[threadid] = pred->dirpred.meta2->config.bank.shiftreg[threadid] = shift_reg;
#else
	    pred->dirpred.bim2->config.bank.shiftreg = pred->dirpred.g02->config.bank.shiftreg = pred->dirpred.g12->config.bank.shiftreg = pred->dirpred.meta2->config.bank.shiftreg = shift_reg;
#endif
	}
	else
	    panic ("sth wrong here in 2 bpred_update_history");
    }
}

/*lechen, 2Bc-skew*/
/* grbriggs: warning: the threadid parameter is currently unused */
#ifdef SMT_SS
void
bpred_2bcgskew_lookup (struct bpred_t *pred, struct bpred_update_t *dir_update_ptr, md_addr_t baddr, int threadid)
#else
void
bpred_2bcgskew_lookup (struct bpred_t *pred, struct bpred_update_t *dir_update_ptr, md_addr_t baddr)
#endif
{
    unsigned int info_vec, info_vec1, info_vec2;	/*the information vectors to index the banks */
    int shift;			/*width of the info vectors */
    int mask;			/*mask for the width */
    int index;			/*index into the bank */
    int eff_addr = baddr >> MD_BR_SHIFT;	/*remove lowest bits of the branch addr */

    /*dir_update_ptr->dir.cycle=sim_cycle; for debug, can be removed later */

    /*bim component */
    shift = pred->dirpred.bim1->config.bank.log_size;
    mask = pred->dirpred.bim1->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.bim1->config.bank.shift_width) |
#ifdef SMT_SS
	(pred->dirpred.bim1->config.bank.shiftreg[threadid] &
#else
	(pred->dirpred.bim1->config.bank.shiftreg &
#endif
	 ((1 << pred->dirpred.bim1->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f0 */
    index = ((H_func (info_vec1, shift, mask) ^ Hbar_func (info_vec2, shift, mask) ^ info_vec2) + 0) & mask;
    dir_update_ptr->pdir1 = &(pred->dirpred.bim1->config.bank.table[index]);
    dir_update_ptr->dir.bim1 = *(dir_update_ptr->pdir1) > 1;

    /*g0 component */
    shift = pred->dirpred.g01->config.bank.log_size;
    mask = pred->dirpred.g01->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.g01->config.bank.shift_width) |
#ifdef SMT_SS
	(pred->dirpred.g01->config.bank.shiftreg[threadid] &
#else
	(pred->dirpred.g01->config.bank.shiftreg &
#endif
	 ((1 << pred->dirpred.g01->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f1 */
    index = ((H_func (info_vec1, shift, mask) ^ Hbar_func (info_vec2, shift, mask) ^ info_vec1)	/*+disp1 */
	     /*pred->dirpred.g01->config.bank.size/4 */ ) & mask;
    dir_update_ptr->pdir2 = &(pred->dirpred.g01->config.bank.table[index]);
    dir_update_ptr->dir.g01 = *(dir_update_ptr->pdir2) > 1;

    /*g1 component */
    shift = pred->dirpred.g11->config.bank.log_size;
    mask = pred->dirpred.g11->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.g11->config.bank.shift_width) |
#ifdef SMT_SS
	(pred->dirpred.g11->config.bank.shiftreg[threadid] &
#else
	(pred->dirpred.g11->config.bank.shiftreg &
#endif
	 ((1 << pred->dirpred.g11->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f2 */
    index = ((Hbar_func (info_vec1, shift, mask) ^ H_func (info_vec2, shift, mask) ^ info_vec2)	/*+disp1*2 */
	     /*2*pred->dirpred.g11->config.bank.size/4 */ ) & mask;
    dir_update_ptr->pdir3 = &(pred->dirpred.g11->config.bank.table[index]);
    dir_update_ptr->dir.g11 = *(dir_update_ptr->pdir3) > 1;

    /*meta component */
    shift = pred->dirpred.meta->config.bank.log_size;
    mask = pred->dirpred.meta->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.meta->config.bank.shift_width) |
#ifdef SMT_SS
	(pred->dirpred.meta->config.bank.shiftreg[threadid] &
#else
	(pred->dirpred.meta->config.bank.shiftreg &
#endif
	 ((1 << pred->dirpred.meta->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f3 */
    index = ((Hbar_func (info_vec1, shift, mask) ^ H_func (info_vec2, shift, mask) ^ info_vec1)	/*+disp1*3 */
	     /*3*pred->dirpred.meta->config.bank.size/4 */ ) & mask;
    dir_update_ptr->pmeta = &(pred->dirpred.meta->config.bank.table[index]);
    dir_update_ptr->dir.meta = *(dir_update_ptr->pmeta) > 1;

    if (!dir_update_ptr->dir.meta)	/*bim gives the prediction */
	dir_update_ptr->pred_taken = dir_update_ptr->dir.bim1;
    else /*majority vote */ if (dir_update_ptr->dir.g01 == dir_update_ptr->dir.g11)
	dir_update_ptr->pred_taken = dir_update_ptr->dir.g01;
    else
	dir_update_ptr->pred_taken = dir_update_ptr->dir.bim1;
}

#if 0				/* grbriggs: we are not using combined2 */
/*lechen, 2Bc-skew*/
void
bpred_2bcgskew_lookup2 (struct bpred_t *pred, struct bpred_update_t *dir_update_ptr, md_addr_t baddr)
{
    unsigned int info_vec, info_vec1, info_vec2;	/*the information vectors to index the banks */
    int shift;			/*width of the info vectors */
    int mask;			/*mask for the width */
    int index;			/*index into the bank */
    int eff_addr = baddr >> MD_BR_SHIFT;	/*remove lowest bits of the branch addr */

    /*bim component */
    shift = pred->dirpred.bim2->config.bank.log_size;
    mask = pred->dirpred.bim2->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.bim2->config.bank.shift_width) | (pred->dirpred.bim2->config.bank.shiftreg & ((1 << pred->dirpred.bim2->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f0 */
    index = ((H_func (info_vec1, shift, mask) ^ Hbar_func (info_vec2, shift, mask) ^ info_vec2) + 0) & mask;
    dir_update_ptr->pdir12 = &(pred->dirpred.bim2->config.bank.table[index]);
    dir_update_ptr->dir.bim2 = *(dir_update_ptr->pdir12) > 1;

    /*g0 component */
    shift = pred->dirpred.g02->config.bank.log_size;
    mask = pred->dirpred.g02->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.g02->config.bank.shift_width) | (pred->dirpred.g02->config.bank.shiftreg & ((1 << pred->dirpred.g02->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f1 */
    index = ((H_func (info_vec1, shift, mask) ^ Hbar_func (info_vec2, shift, mask) ^ info_vec1)	/*+disp2 */
	     /*pred->dirpred.g02->config.bank.size/4 */ ) & mask;
    dir_update_ptr->pdir22 = &(pred->dirpred.g02->config.bank.table[index]);
    dir_update_ptr->dir.g02 = *(dir_update_ptr->pdir22) > 1;

    /*g1 component */
    shift = pred->dirpred.g12->config.bank.log_size;
    mask = pred->dirpred.g12->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.g12->config.bank.shift_width) | (pred->dirpred.g12->config.bank.shiftreg & ((1 << pred->dirpred.g12->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f2 */
    index = ((Hbar_func (info_vec1, shift, mask) ^ H_func (info_vec2, shift, mask) ^ info_vec2)	/*+disp2*2 */
	     /*2*pred->dirpred.g12->config.bank.size/4 */ ) & mask;
    dir_update_ptr->pdir32 = &(pred->dirpred.g12->config.bank.table[index]);
    dir_update_ptr->dir.g12 = *(dir_update_ptr->pdir32) > 1;

    /*meta component */
    shift = pred->dirpred.meta2->config.bank.log_size;
    mask = pred->dirpred.meta2->config.bank.size - 1;
    info_vec = (eff_addr << pred->dirpred.meta2->config.bank.shift_width) | (pred->dirpred.meta2->config.bank.shiftreg & ((1 << pred->dirpred.meta2->config.bank.shift_width) - 1));
    info_vec2 = info_vec & mask;
    info_vec1 = (info_vec & (mask << shift)) >> shift;
    /*f3 */
    index = ((Hbar_func (info_vec1, shift, mask) ^ H_func (info_vec2, shift, mask) ^ info_vec1)	/*+disp2*3 */
	     /*3*pred->dirpred.meta2->config.bank.size/4 */ ) & mask;
    dir_update_ptr->pmeta2 = &(pred->dirpred.meta2->config.bank.table[index]);
    dir_update_ptr->dir.meta2 = *(dir_update_ptr->pmeta2) > 1;

    if (!dir_update_ptr->dir.meta2)	/*bim gives the prediction */
	dir_update_ptr->pred_taken2 = dir_update_ptr->dir.bim2;
    else /*majority vote */ if (dir_update_ptr->dir.g02 == dir_update_ptr->dir.g12)
	dir_update_ptr->pred_taken2 = dir_update_ptr->dir.g02;
    else
	dir_update_ptr->pred_taken2 = dir_update_ptr->dir.bim2;
}
#endif

/* probe a predictor for a next fetch address, the predictor is probed
   with branch address BADDR, the branch target is BTARGET (used for
   static predictors), and OP is the instruction opcode (used to simulate
   predecode bits; a pointer to the predictor state entry (or null for jumps)
   is returned in *DIR_UPDATE_PTR (used for updating predictor state),
   and the non-speculative top-of-stack is returned in stack_recover_idx 
   (used for recovering ret-addr stack after mis-predict).  */
md_addr_t			/* predicted branch target addr */
  bpred_lookup (struct bpred_t *pred,	/* branch predictor instance */
		md_addr_t baddr,	/* branch address */
		md_addr_t btarget,	/* branch target if taken */
		enum md_opcode op,	/* opcode of instruction */
		int is_call,	/* non-zero if inst is fn call */
		int is_return,	/* non-zero if inst is fn return */
		struct bpred_update_t *dir_update_ptr,	/* pred state pointer */
#ifdef SMT_SS
		int *stack_recover_idx,	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
		int threadid, int btb_threadid	/* Threadid used for BTB lookup. May be masterid instead. */
    )
#else
		int *stack_recover_idx)	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
#endif
{
    struct bpred_btb_ent_t *pbtb = NULL;
    int index, i;

    if (!dir_update_ptr)
	panic ("no bpred update record");

    /* if this is not a branch, return not-taken */
#ifdef LOAD_PREDICTOR
    if (!(MD_OP_FLAGS (op) & F_CTRL) && (pred->class != LDPred2bit))
	return 0;
#else
    if (!(MD_OP_FLAGS (op) & F_CTRL))
	return 0;
#endif

    pred->lookups++;

    dir_update_ptr->dir.ras = FALSE;
    dir_update_ptr->pdir1 = NULL;
    dir_update_ptr->pdir2 = NULL;
    dir_update_ptr->pmeta = NULL;
    /* Except for jumps, get a pointer to direction-prediction bits */
    switch (pred->class)
    {
	/*lechen, 2Bc-skew */
    case BPred2bcgskew:
	dir_update_ptr->pdir3 = NULL;
	if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND))
#ifdef SMT_SS
	    bpred_2bcgskew_lookup (pred, dir_update_ptr, baddr, threadid);
#else
	    bpred_2bcgskew_lookup (pred, dir_update_ptr, baddr);
#endif
	break;
    case BPredComb:
	if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND))
	{
	    char *bimod, *twolev, *meta;

#ifdef SMT_SS
	    bimod = bpred_dir_lookup (pred->dirpred.bimod, baddr, threadid);
	    twolev = bpred_dir_lookup (pred->dirpred.twolev, baddr, threadid);
	    meta = bpred_dir_lookup (pred->dirpred.meta, baddr, threadid);
#else
	    bimod = bpred_dir_lookup (pred->dirpred.bimod, baddr);
	    twolev = bpred_dir_lookup (pred->dirpred.twolev, baddr);
	    meta = bpred_dir_lookup (pred->dirpred.meta, baddr);
#endif
	    dir_update_ptr->pmeta = meta;
	    dir_update_ptr->dir.meta = (*meta >= 2);
	    dir_update_ptr->dir.bimod = (*bimod >= 2);
	    dir_update_ptr->dir.twolev = (*twolev >= 2);
	    if (*meta >= 2)
	    {
		dir_update_ptr->pdir1 = twolev;
		dir_update_ptr->pdir2 = bimod;
	    }
	    else
	    {
		dir_update_ptr->pdir1 = bimod;
		dir_update_ptr->pdir2 = twolev;
	    }
	}
	break;
    case BPred2Level:
	if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND))
	{
	    dir_update_ptr->pdir1 =
#ifdef SMT_SS
		bpred_dir_lookup (pred->dirpred.twolev, baddr, threadid);
#else
		bpred_dir_lookup (pred->dirpred.twolev, baddr);
#endif
	}
	break;
#ifdef LOAD_PREDICTOR
    case LDPred2bit:
	dir_update_ptr->pdir1 =
#ifdef SMT_SS
	    bpred_dir_lookup (pred->dirpred.bimod, baddr, threadid);
#else
	    bpred_dir_lookup (pred->dirpred.bimod, baddr);
#endif
	return ((*(dir_update_ptr->pdir1) >= 3) ? /* load hit */ 1
		: /* load miss */ 0);
	break;
#endif
    case BPred2bit:
	if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND))
	{
	    dir_update_ptr->pdir1 =
#ifdef SMT_SS
		bpred_dir_lookup (pred->dirpred.bimod, baddr, threadid);
#else
		bpred_dir_lookup (pred->dirpred.bimod, baddr);
#endif
	}
	break;
    case BPredTaken:
	return btarget;
    case BPredNotTaken:
	if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND))
	{
	    return baddr + sizeof (md_inst_t);
	}
	else
	{
	    return btarget;
	}
    default:
	panic ("bogus predictor class");
    }

/*lechen, only for conditional branches, record prediction for lev1,
* do prediction for another predictor, select predition between the 2,
* 2bcgskew already set its prediction*/
    if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND))
    {
	if (mystricmp (pred_type, "2bcgskew"))
	    dir_update_ptr->pred_taken = *(dir_update_ptr->pdir1) >= 2;
#if 0				/* grbriggs: we are not using combined2 */
	pred_taken = dir_update_ptr->pred_taken;

	if (combined2)
	{			/*lookup and set prediction, for another predictor */
	    if (!mystricmp (pred_type2, "2bcgskew"))
		bpred_2bcgskew_lookup2 (pred, dir_update_ptr, baddr);
	    else
		bpred_lookup2 (pred, baddr, op, dir_update_ptr);

	    if (use_sel)
	    {			/*lookup up the selector if necessary */
		if (dir_update_ptr->pred_taken == dir_update_ptr->pred_taken2)
		    pred_taken = dir_update_ptr->pred_taken;
		else
		{
		    dir_update_ptr->hist_tab_indx = ((baddr >> MD_BR_SHIFT) ^ hist_reg) % hist_tab_size;
		    if (sel_lookup (dir_update_ptr->hist_tab_indx, dir_update_ptr->pred_taken))
			pred_taken = dir_update_ptr->pred_taken;
		    else
			pred_taken = dir_update_ptr->pred_taken2;
		}
	    }
	}
#endif
    }

    /*
     * We have a stateful predictor, and have gotten a pointer into the
     * direction predictor (except for jumps, for which the ptr is null)
     */

    /* record pre-pop TOS; if this branch is executed speculatively
     * and is squashed, we'll restore the TOS and hope the data
     * wasn't corrupted in the meantime. */
#ifdef SMT_SS
    if (pred->retstack[threadid].size)
	*stack_recover_idx = pred->retstack[threadid].tos;
    else
	*stack_recover_idx = 0;

    /* if this is a return, pop return-address stack */
    if (is_return && pred->retstack[threadid].size)
    {
	md_addr_t target = pred->retstack[threadid].stack[pred->retstack[threadid].tos].target;

	pred->retstack[threadid].tos = (pred->retstack[threadid].tos + pred->retstack[threadid].size - 1) % pred->retstack[threadid].size;
	pred->retstack_pops++;
	dir_update_ptr->dir.ras = TRUE;	/* using RAS here */
	return target;
    }

#ifndef RAS_BUG_COMPATIBLE
    /* if function call, push return-address onto return-address stack */
    if (is_call && pred->retstack[threadid].size)
    {
	pred->retstack[threadid].tos = (pred->retstack[threadid].tos + 1) % pred->retstack[threadid].size;
	pred->retstack[threadid].stack[pred->retstack[threadid].tos].target = baddr + sizeof (md_inst_t);
	pred->retstack_pushes++;
    }
#endif /* !RAS_BUG_COMPATIBLE */

#else
    if (pred->retstack.size)
	*stack_recover_idx = pred->retstack.tos;
    else
	*stack_recover_idx = 0;

    /* if this is a return, pop return-address stack */
    if (is_return && pred->retstack.size)
    {
	md_addr_t target = pred->retstack.stack[pred->retstack.tos].target;

	pred->retstack.tos = (pred->retstack.tos + pred->retstack.size - 1) % pred->retstack.size;
	pred->retstack_pops++;
	dir_update_ptr->dir.ras = TRUE;	/* using RAS here */
	return target;
    }

#ifndef RAS_BUG_COMPATIBLE
    /* if function call, push return-address onto return-address stack */
    if (is_call && pred->retstack.size)
    {
	pred->retstack.tos = (pred->retstack.tos + 1) % pred->retstack.size;
	pred->retstack.stack[pred->retstack.tos].target = baddr + sizeof (md_inst_t);
	pred->retstack_pushes++;
    }
#endif /* !RAS_BUG_COMPATIBLE */
#endif

    /* not a return. Get a pointer into the BTB */
    index = (baddr >> MD_BR_SHIFT) & (pred->btb.sets - 1);

    if (pred->btb.assoc > 1)
    {
	index *= pred->btb.assoc;

	/* Now we know the set; look for a PC match */
	for (i = index; i < (index + pred->btb.assoc); i++)
#ifdef SMT_SS
	    if ((pred->btb.btb_data[i].addr_id.addr == baddr) && (pred->btb.btb_data[i].addr_id.threadid == btb_threadid))
#else
	    if (pred->btb.btb_data[i].addr == baddr)
#endif
	    {
		/* match */
		pbtb = &pred->btb.btb_data[i];
		break;
	    }
    }
    else
    {
	pbtb = &pred->btb.btb_data[index];
#ifdef SMT_SS
	if ((pbtb->addr_id.addr != baddr) || (pbtb->addr_id.threadid != btb_threadid))
#else
	if (pbtb->addr != baddr)
#endif
	    pbtb = NULL;
    }

    /*
     * We now also have a pointer into the BTB for a hit, or NULL otherwise
     */

    /* if this is a jump, ignore predicted direction; we know it's taken. */
    if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) == (F_CTRL | F_UNCOND))
    {
	return (pbtb ? pbtb->target : 1);
    }

    /* otherwise we have a conditional branch */
    if (pbtb == NULL)
    {
	/* BTB miss -- just return a predicted direction */
	return ((*(dir_update_ptr->pdir1) >= 2) ? /* taken */ 1
		: /* not taken */ 0);
    }
    else
    {
	/* BTB hit, so return target if it's a predicted-taken branch */
	return ((*(dir_update_ptr->pdir1) >= 2) ? /* taken */ pbtb->target
		: /* not taken */ 0);
    }
}

/* Speculative execution can corrupt the ret-addr stack.  So for each
 * lookup we return the top-of-stack (TOS) at that point; a mispredicted
 * branch, as part of its recovery, restores the TOS using this value --
 * hopefully this uncorrupts the stack. */
#ifdef SMT_SS
void bpred_recover (struct bpred_t *pred,	/* branch predictor instance */
		    md_addr_t baddr,	/* branch address */
		    int stack_recover_idx,	/* Non-speculative top-of-stack;
						 * used on mispredict recovery */
		    int threadid)
{
    if (pred == NULL)
	return;

    pred->retstack[threadid].tos = stack_recover_idx;
}

#else
void
bpred_recover (struct bpred_t *pred,	/* branch predictor instance */
	       md_addr_t baddr,	/* branch address */
	       int stack_recover_idx)	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
{
    if (pred == NULL)
	return;

    pred->retstack.tos = stack_recover_idx;
}
#endif

/*lechen, update the 2Bc-skew, policy is from
* "Design Tradeoffs for the Alpha EV8 Conditional Branch Predictor*/
void
bpred_2bcgskew_update (struct bpred_t *pred,	/* branch predictor instance */
		       md_addr_t baddr,	/* branch address */
		       int taken,	/* non-zero if branch was taken */
		       int pred_taken,	/* non-zero if branch was pred taken */
		       int correct,	/* was earlier addr prediction ok? */
		       struct bpred_update_t *dir_update_ptr	/* pred state pointer */
#ifdef SMT_SS
		       , int threadid	/* thread id */
#endif
    )
{
    unsigned int shift_reg;

    /*2 levels of 2bcgskew now */
    pred_taken = dir_update_ptr->pred_taken;
    correct = pred_taken == taken;
    /*on a correct prediction, when all 3 predictors were agreeing, don't update
     * otherwise, strenthen meta if the 2 predictions were different*/
    if (correct)
    {
	if (dir_update_ptr->dir.bim1 != dir_update_ptr->dir.g01 || dir_update_ptr->dir.bim1 != dir_update_ptr->dir.g11)
	{
	    /*3 predictors didn't agree */
	    if (dir_update_ptr->dir.g01 == dir_update_ptr->dir.g11)
	    {
		/*2 predictions didn't agree, strengthen meta */
		if (dir_update_ptr->dir.meta && *(dir_update_ptr->pmeta) != 3)
		{
		    /*used g0 and g1 */
		    assert (taken == dir_update_ptr->dir.g01);
		    ++*dir_update_ptr->pmeta;
		}
		else if (!dir_update_ptr->dir.meta && *dir_update_ptr->pmeta)
		{
		    /*used bim */
		    assert (taken == dir_update_ptr->dir.bim1);
		    --*dir_update_ptr->pmeta;
		}
	    }
	    if (dir_update_ptr->dir.meta)
	    {
		/*used majority vote */
		/*strengthen the right predictions */
		if (taken == dir_update_ptr->dir.bim1)
		{
		    if (taken && *(dir_update_ptr->pdir1) != 3)
			++ * dir_update_ptr->pdir1;
		    else if (!taken && *dir_update_ptr->pdir1)
			-- * dir_update_ptr->pdir1;
		}
		if (taken == dir_update_ptr->dir.g01)
		{
		    if (taken && *(dir_update_ptr->pdir2) != 3)
			++ * dir_update_ptr->pdir2;
		    else if (!taken && *dir_update_ptr->pdir2)
			-- * dir_update_ptr->pdir2;
		}
		if (taken == dir_update_ptr->dir.g11)
		{
		    if (taken && *(dir_update_ptr->pdir3) != 3)
			++ * dir_update_ptr->pdir3;
		    else if (!taken && *dir_update_ptr->pdir3)
			-- * dir_update_ptr->pdir3;
		}
	    }
	    else
	    {
		/*used bim */
		assert (dir_update_ptr->dir.bim1 == taken);
		/*strengthen the right prediction (bim) */
		if (taken && *(dir_update_ptr->pdir1) != 3)
		    ++ * dir_update_ptr->pdir1;
		else if (!taken && *dir_update_ptr->pdir1)
		    -- * dir_update_ptr->pdir1;
	    }
	}
#if 0
	else if (dir_update_ptr->dir.meta)
	{
	    /*used majority vote */
	    /*strengthen the right predictions */
	    if (taken == dir_update_ptr->dir.bim1)
	    {
		if (taken && *(dir_update_ptr->pdir1) != 3)
		    ++ * dir_update_ptr->pdir1;
		else if (!taken && *dir_update_ptr->pdir1)
		    -- * dir_update_ptr->pdir1;
	    }
	    if (taken == dir_update_ptr->dir.g01)
	    {
		if (taken && *(dir_update_ptr->pdir2) != 3)
		    ++ * dir_update_ptr->pdir2;
		else if (!taken && *dir_update_ptr->pdir2)
		    -- * dir_update_ptr->pdir2;
	    }
	    if (taken == dir_update_ptr->dir.g11)
	    {
		if (taken && *(dir_update_ptr->pdir3) != 3)
		    ++ * dir_update_ptr->pdir3;
		else if (!taken && *dir_update_ptr->pdir3)
		    -- * dir_update_ptr->pdir3;
	    }
	}
	else
	{
	    /*used bim */
	    assert (dir_update_ptr->dir.bim1 == taken);
	    /*strengthen the right prediction (bim) */
	    if (taken && *(dir_update_ptr->pdir1) != 3)
		++ * dir_update_ptr->pdir1;
	    else if (!taken && *dir_update_ptr->pdir1)
		-- * dir_update_ptr->pdir1;
	}
#endif
    }
    else
    {				/*misprediction */
	/*meta is updated only when the 2 predictors disagree */
	if (dir_update_ptr->dir.bim1 != dir_update_ptr->dir.g01 && dir_update_ptr->dir.bim1 != dir_update_ptr->dir.g11)
	{
	    /*when 2 predictions were different, first update meta,
	     * then recompute the overall prediction
	     int preded_taken;*/
	    if ((taken == dir_update_ptr->dir.bim1) && *dir_update_ptr->pmeta)
	    {
		assert (dir_update_ptr->dir.meta);
		--*dir_update_ptr->pmeta;
	    }
	    else if (taken == dir_update_ptr->dir.g01 && (*(dir_update_ptr->pmeta) < 3))
	    {
		assert (!dir_update_ptr->dir.meta);
		++*dir_update_ptr->pmeta;
	    }
#if 0
	    /*recompute overall prediction */
	    dir_update_ptr->dir.meta = *(dir_update_ptr->pmeta) > 1;
	    /*dir_update_ptr->dir.bim1=*(dir_update_ptr->pdir1)>1;
	       dir_update_ptr->dir.g01=*(dir_update_ptr->pdir2)>1;
	       dir_update_ptr->dir.g11=*(dir_update_ptr->pdir3)>1; */
	    if (!dir_update_ptr->dir.meta)	/*bim would've given the prediction */
		preded_taken = dir_update_ptr->dir.bim1;
	    else
		preded_taken = (dir_update_ptr->dir.bim1 == dir_update_ptr->dir.g01) ? dir_update_ptr->dir.bim1 : dir_update_ptr->dir.g11;
	    /*correct prediction, strengthen all participating tables, 
	     * otherwise update all banks*/
	    if (preded_taken == taken)
	    {			/* right */
		if (!dir_update_ptr->dir.meta)
		{		/*used bim */
		    assert (taken == dir_update_ptr->dir.bim1);
		    if (taken && *(dir_update_ptr->pdir1) != 3)
			++ * dir_update_ptr->pdir1;
		    else if (!taken && dir_update_ptr->pdir1 != 0)
			-- * dir_update_ptr->pdir1;
		}
		else
		{		/*used majority vote */
		    if (taken == dir_update_ptr->dir.bim1)
		    {
			if (taken && *(dir_update_ptr->pdir1) != 3)
			    ++ * dir_update_ptr->pdir1;
			else if (!taken && *(dir_update_ptr->pdir1) != 0)
			    -- * dir_update_ptr->pdir1;
		    }
		    if (taken == dir_update_ptr->dir.g01)
		    {
			if (taken && *(dir_update_ptr->pdir2) != 3)
			    ++ * dir_update_ptr->pdir2;
			else if (!taken && *(dir_update_ptr->pdir2) != 0)
			    -- * dir_update_ptr->pdir2;
		    }
		    if (taken == dir_update_ptr->dir.g11)
		    {
			if (taken && *(dir_update_ptr->pdir3) != 3)
			    ++ * dir_update_ptr->pdir3;
			else if (!taken && *(dir_update_ptr->pdir3) != 0)
			    -- * dir_update_ptr->pdir3;
		    }
		}
	    }
	    else
	    {			/*predicted wrong, update all banks */
		/*if((taken==dir_update_ptr->dir.bim1) && 
		 *dir_update_ptr->pmeta) {
		 assert(dir_update_ptr->dir.meta); 
		 --*dir_update_ptr->pmeta;
		 } else if(taken==dir_update_ptr->dir.g01 && 
		 (*(dir_update_ptr->pmeta)<3)) {
		 assert(!dir_update_ptr->dir.meta); 
		 ++*dir_update_ptr->pmeta;
		 }*/
		if (taken && *(dir_update_ptr->pdir1) != 3)
		    ++ * dir_update_ptr->pdir1;
		else if (!taken && *(dir_update_ptr->pdir1) != 0)
		    -- * dir_update_ptr->pdir1;
		if (taken && *(dir_update_ptr->pdir2) != 3)
		    ++ * dir_update_ptr->pdir2;
		else if (!taken && *(dir_update_ptr->pdir2) != 0)
		    -- * dir_update_ptr->pdir2;
		if (taken && *(dir_update_ptr->pdir3) != 3)
		    ++ * dir_update_ptr->pdir3;
		else if (!taken && *(dir_update_ptr->pdir3) != 0)
		    -- * dir_update_ptr->pdir3;
	    }
#endif
	}
	{			/*2 predictors agreed, update all banks */
	    /*      assert(taken!=dir_update_ptr->dir.bim1); */
	    if (taken && *(dir_update_ptr->pdir1) != 3)
		++ * dir_update_ptr->pdir1;
	    else if (!taken && *(dir_update_ptr->pdir1) != 0)
		-- * dir_update_ptr->pdir1;
	    if (taken && *(dir_update_ptr->pdir2) != 3)
		++ * dir_update_ptr->pdir2;
	    else if (!taken && *(dir_update_ptr->pdir2) != 0)
		-- * dir_update_ptr->pdir2;
	    if (taken && *(dir_update_ptr->pdir3) != 3)
		++ * dir_update_ptr->pdir3;
	    else if (!taken && *(dir_update_ptr->pdir3) != 0)
		-- * dir_update_ptr->pdir3;
	}
    }
    if (!hist_imm)
    {
	/*update history register, one in each bank */
#ifdef SMT_SS
	shift_reg = (pred->dirpred.bim1->config.bank.shiftreg[threadid] << 1) | (!!taken);
	/*only the lower bits are useful though */
	pred->dirpred.bim1->config.bank.shiftreg[threadid] = pred->dirpred.g01->config.bank.shiftreg[threadid] = pred->dirpred.g11->config.bank.shiftreg[threadid] = pred->dirpred.meta->config.bank.shiftreg[threadid] = shift_reg;
#else
	shift_reg = (pred->dirpred.bim1->config.bank.shiftreg << 1) | (!!taken);
	/*only the lower bits are useful though */
	pred->dirpred.bim1->config.bank.shiftreg = pred->dirpred.g01->config.bank.shiftreg = pred->dirpred.g11->config.bank.shiftreg = pred->dirpred.meta->config.bank.shiftreg = shift_reg;
#endif
    }
}

#if 0				/* unused */
/*lechen, update the 2Bc-skew, policy is from
* "Design Tradeoffs for the Alpha EV8 Conditional Branch Predictor*/
void
bpred_2bcgskew_update2 (struct bpred_t *pred,	/* branch predictor instance */
			md_addr_t baddr,	/* branch address */
			int taken,	/* non-zero if branch was taken */
			int pred_taken,	/* non-zero if branch was pred taken */
			int correct,	/* was earlier addr prediction ok? */
			struct bpred_update_t *dir_update_ptr)	/* pred state pointer */
{
    unsigned int shift_reg;

    /*2 levels of 2bcgskew now, note: not needed for 2 level, bad with arvi
       pred_taken=dir_update_ptr->pred_taken2;
       correct = pred_taken==taken; */
    /*on a correct prediction, when all 3 predictors were agreeing, don't update
     * otherwise, strenthen meta if the 2 predictions were different*/
    if (correct)
    {
	if (dir_update_ptr->dir.bim2 != dir_update_ptr->dir.g02 || dir_update_ptr->dir.bim2 != dir_update_ptr->dir.g12)
	{
	    /*3 predictors didn't agree */
	    if (dir_update_ptr->dir.g02 == dir_update_ptr->dir.g12)
	    {
		/*2 predictions didn't agree, strengthen meta */
		if (dir_update_ptr->dir.meta2 && *(dir_update_ptr->pmeta2) != 3)
		{
		    /*used g0 and g1 */
		    assert (taken == dir_update_ptr->dir.g02);
		    ++*dir_update_ptr->pmeta2;
		}
		else if (!dir_update_ptr->dir.meta2 && *dir_update_ptr->pmeta2)
		{
		    /*used bim */
		    assert (taken == dir_update_ptr->dir.bim2);
		    --*dir_update_ptr->pmeta2;
		}
	    }
	    if (dir_update_ptr->dir.meta2)
	    {
		/*used majority vote */
		/*strengthen the right predictions */
		if (taken == dir_update_ptr->dir.bim2)
		{
		    if (taken && *(dir_update_ptr->pdir12) != 3)
			++ * dir_update_ptr->pdir12;
		    else if (!taken && *dir_update_ptr->pdir12)
			-- * dir_update_ptr->pdir12;
		}
		if (taken == dir_update_ptr->dir.g02)
		{
		    if (taken && *(dir_update_ptr->pdir22) != 3)
			++ * dir_update_ptr->pdir22;
		    else if (!taken && *dir_update_ptr->pdir22)
			-- * dir_update_ptr->pdir22;
		}
		if (taken == dir_update_ptr->dir.g12)
		{
		    if (taken && *(dir_update_ptr->pdir32) != 3)
			++ * dir_update_ptr->pdir32;
		    else if (!taken && *dir_update_ptr->pdir32)
			-- * dir_update_ptr->pdir32;
		}
	    }
	    else
	    {
		/*used bim */
		assert (dir_update_ptr->dir.bim2 == taken);
		/*strengthen the right prediction (bim) */
		if (taken && *(dir_update_ptr->pdir12) != 3)
		    ++ * dir_update_ptr->pdir12;
		else if (!taken && *dir_update_ptr->pdir12)
		    -- * dir_update_ptr->pdir12;
	    }
	}
    }
    else
    {				/*misprediction */
	/*meta is updated only when the 2 predictors disagree */
	if (dir_update_ptr->dir.bim2 != dir_update_ptr->dir.g02 && dir_update_ptr->dir.bim2 != dir_update_ptr->dir.g12)
	{
	    /*when 2 predictions were different, first update meta,
	     * then recompute the overall prediction
	     int preded_taken;*/
	    if ((taken == dir_update_ptr->dir.bim2) && *dir_update_ptr->pmeta2)
	    {
		assert (dir_update_ptr->dir.meta2);
		--*dir_update_ptr->pmeta2;
	    }
	    else if (taken == dir_update_ptr->dir.g02 && (*(dir_update_ptr->pmeta2) < 3))
	    {
		assert (!dir_update_ptr->dir.meta2);
		++*dir_update_ptr->pmeta2;
	    }
#if 0
	    /*recompute overall prediction */
	    dir_update_ptr->dir.meta2 = *(dir_update_ptr->pmeta2) > 1;
	    /*dir_update_ptr->dir.bim2=*(dir_update_ptr->pdir12)>1;
	       dir_update_ptr->dir.g02=*(dir_update_ptr->pdir22)>1;
	       dir_update_ptr->dir.g12=*(dir_update_ptr->pdir32)>1; */
	    if (!dir_update_ptr->dir.meta2)	/*bim would've given the prediction */
		preded_taken = dir_update_ptr->dir.bim2;
	    else
		preded_taken = (dir_update_ptr->dir.bim2 == dir_update_ptr->dir.g02) ? dir_update_ptr->dir.bim2 : dir_update_ptr->dir.g12;
	    /*correct prediction, strengthen all participating tables, 
	     * otherwise update all banks*/
	    if (preded_taken == taken && !dir_update_ptr->dir.meta2)
	    {
		/*right and used bim */
		assert (taken == dir_update_ptr->dir.bim2);
		if (taken && *(dir_update_ptr->pdir12) != 3)
		    ++ * dir_update_ptr->pdir12;
		else if (!taken && dir_update_ptr->pdir12 != 0)
		    -- * dir_update_ptr->pdir12;
	    }
	    else
	    {
		if (taken == dir_update_ptr->dir.bim2)
		{
		    if (taken && *(dir_update_ptr->pdir12) != 3)
			++ * dir_update_ptr->pdir12;
		    else if (!taken && *(dir_update_ptr->pdir12) != 0)
			-- * dir_update_ptr->pdir12;
		}
		if (taken == dir_update_ptr->dir.g02)
		{
		    if (taken && *(dir_update_ptr->pdir22) != 3)
			++ * dir_update_ptr->pdir22;
		    else if (!taken && *(dir_update_ptr->pdir22) != 0)
			-- * dir_update_ptr->pdir22;
		}
		if (taken == dir_update_ptr->dir.g12)
		{
		    if (taken && *(dir_update_ptr->pdir32) != 3)
			++ * dir_update_ptr->pdir32;
		    else if (!taken && *(dir_update_ptr->pdir32) != 0)
			-- * dir_update_ptr->pdir32;
		}
	    }
#endif
	}
	{			/*2 predictors agreed, update all banks */
	    /*assert(taken!=dir_update_ptr->dir.bim2); */
	    if (taken && *(dir_update_ptr->pdir12) != 3)
		++ * dir_update_ptr->pdir12;
	    else if (!taken && *(dir_update_ptr->pdir12) != 0)
		-- * dir_update_ptr->pdir12;
	    if (taken && *(dir_update_ptr->pdir22) != 3)
		++ * dir_update_ptr->pdir22;
	    else if (!taken && *(dir_update_ptr->pdir22) != 0)
		-- * dir_update_ptr->pdir22;
	    if (taken && *(dir_update_ptr->pdir32) != 3)
		++ * dir_update_ptr->pdir32;
	    else if (!taken && *(dir_update_ptr->pdir32) != 0)
		-- * dir_update_ptr->pdir32;
	}
    }
    if (!hist_imm)
    {
	/*update history register, one in each bank */
	shift_reg = (pred->dirpred.bim2->config.bank.shiftreg << 1) | (!!taken);
	/*only the lower bits are useful though */
	pred->dirpred.bim2->config.bank.shiftreg = pred->dirpred.g02->config.bank.shiftreg = pred->dirpred.g12->config.bank.shiftreg = pred->dirpred.meta2->config.bank.shiftreg = shift_reg;
	/*assert(pred->dirpred.bim2->config.bank.shiftreg ==
	   pred->dirpred.bim1->config.bank.shiftreg); */
    }
}
#endif

/*more code from lechen*/
extern int MAX_BR_NUM;
extern unsigned int *branch_address2;

void
bpred_update_history1 (struct bpred_t *pred, int taken, md_addr_t baddr, struct bpred_update_t *dir_update_ptr
#ifdef SMT_SS
		       , int threadid	/* grbriggs: added so 2bcgskew uses SMT */
#endif
    )
{
    int his_pos /*position in history reg */ ;
    int br_pos /*branch position in branch_address2 */ ;
    unsigned int test /*test vector */ ;
    context *current = thecontexts[threadid];	/* grbriggs: added so 2bcgskew uses SMT */

    his_pos = (current->branch_tail2 + (MAX_BR_NUM + 1) - dir_update_ptr->branch_tail2) % (MAX_BR_NUM + 1);
    br_pos = (dir_update_ptr->branch_tail2 + (MAX_BR_NUM + 1) - 1) % (MAX_BR_NUM + 1);
    assert (baddr == current->branch_address2[br_pos]);

    if (his_pos > 31)
	return;

    test = 1 << his_pos;
    assert (test);		/*shouldn't be very far away */

    /*make sure lev2 has the synchronized history */
    switch (pred->class)
    {
#if 0				/* Copied from lechen but unused */
    case BPredComb:
    case BPred2Level:
	/*should be corrected already in ruu_recover            if(taken==dir_update_ptr->pred_taken) { right, assert */
	if (his_pos < pred->dirpred.twolev->config.two.shift_width)
	    assert (!!taken == !!(pred->dirpred.twolev->config.two.shiftregs[0] & test));
#if 0				/*should be corrected in ruu_recover */
    }
    else
    {				/*wrong, update */
	if (his_pos < pred->dirpred.twolev->config.two.shift_width)
	{
	    assert (!taken == !!(pred->dirpred.twolev->config.two.shiftregs[0] & test));
	    if (taken)
		pred->dirpred.twolev->config.two.shiftregs[0] |= test;
	    else
		pred->dirpred.twolev->config.two.shiftregs[0] &= ~test;
	}
    }
#endif
    break;
#endif
case BPred2bcgskew:
    /*should be corrected in ruu_recover            if(taken==dir_update_ptr->pred_taken) right, assert */
#ifdef SMT_SS
    assert (!!taken == !!(pred->dirpred.bim1->config.bank.shiftreg[threadid] & test));
#else
    assert (!!taken == !!(pred->dirpred.bim1->config.bank.shiftreg & test));
#endif
#if 0				/*should be corrected in ruu_recover */
    else
    {				/*wrong, update */
	assert (!taken == !!(pred->dirpred.bim1->config.bank.shiftreg & test));
	if (taken)
	    pred->dirpred.bim1->config.bank.shiftreg |= test;
	else
	    pred->dirpred.bim1->config.bank.shiftreg &= ~test;
	pred->dirpred.g01->config.bank.shiftreg = pred->dirpred.g11->config.bank.shiftreg = pred->dirpred.meta->config.bank.shiftreg = pred->dirpred.bim1->config.bank.shiftreg;
    }
#endif
    break;
#if 0				/* Copied from lechen but unused */
case BPredARVI:
    break;
#endif
default:
    panic ("sth wrong here in bpred_update_history1");
}

	/*assert and update lev1 history */

#if 0				/* Copied from lechen but we aren't using it */
if (!mystricmp (pred_type2, "comb") || !mystricmp (pred_type2, "2lev"))
{
    if (taken == dir_update_ptr->pred_taken2)
    {
	/*predicted right, just assert */
	if (his_pos < pred->dirpred.twolev2->config.two.shift_width)
	    assert (!!taken == !!(pred->dirpred.twolev2->config.two.shiftregs[0] & test));
    }
    else if (his_pos < pred->dirpred.twolev2->config.two.shift_width)
    {
	/*should be wrong */
	assert (!taken == !!(pred->dirpred.twolev2->config.two.shiftregs[0] & test));
	if (taken)
	    pred->dirpred.twolev2->config.two.shiftregs[0] |= test;
	else
	    pred->dirpred.twolev2->config.two.shiftregs[0] &= ~test;
    }
}
else
#endif
if (!mystricmp (pred_type2, "2bcgskew"))
{
    if (taken == dir_update_ptr->pred_taken2)
	/*predicted right, just assert */
#ifdef SMT_SS
	assert (!!taken == !!(pred->dirpred.bim2->config.bank.shiftreg[threadid] & test));
#else
	assert (!!taken == !!(pred->dirpred.bim2->config.bank.shiftreg & test));
#endif
    else
    {				/*should be wrong */
#ifdef SMT_SS

	assert (!taken == !!(pred->dirpred.bim2->config.bank.shiftreg[threadid] & test));
	if (taken)
	    pred->dirpred.bim2->config.bank.shiftreg[threadid] |= test;
	else
	    pred->dirpred.bim2->config.bank.shiftreg[threadid] &= ~test;
	pred->dirpred.g02->config.bank.shiftreg[threadid] = pred->dirpred.g12->config.bank.shiftreg[threadid] = pred->dirpred.meta2->config.bank.shiftreg[threadid] = pred->dirpred.bim2->config.bank.shiftreg[threadid];
#else
	assert (!taken == !!(pred->dirpred.bim2->config.bank.shiftreg & test));
	if (taken)
	    pred->dirpred.bim2->config.bank.shiftreg |= test;
	else
	    pred->dirpred.bim2->config.bank.shiftreg &= ~test;
	pred->dirpred.g02->config.bank.shiftreg = pred->dirpred.g12->config.bank.shiftreg = pred->dirpred.meta2->config.bank.shiftreg = pred->dirpred.bim2->config.bank.shiftreg;
#endif
    }
}
else
    panic ("sth wrong here in 2 bpred_update_history");
}

/* update the branch predictor, only useful for stateful predictors; updates
   entry for instruction type OP at address BADDR.  BTB only gets updated
   for branches which are taken.  Inst was determined to jump to
   address BTARGET and was taken if TAKEN is non-zero.  Predictor
   statistics are updated with result of prediction, indicated by CORRECT and
   PRED_TAKEN, predictor state to be updated is indicated by *DIR_UPDATE_PTR
   (may be NULL for jumps, which shouldn't modify state bits).  Note if
   bpred_update is done speculatively, branch-prediction may get polluted. */
void bpred_update (struct bpred_t *pred,	/* branch predictor instance */
		   md_addr_t baddr,	/* branch address */
		   md_addr_t btarget,	/* resolved branch target */
		   int taken,	/* non-zero if branch was taken */
		   int pred_taken,	/* non-zero if branch was pred taken */
		   int correct,	/* was earlier addr prediction ok? */
		   enum md_opcode op,	/* opcode of instruction */
#ifdef SMT_SS
		   struct bpred_update_t *dir_update_ptr,	/* pred state pointer */
		   int threadid, int btb_threadid	/* threadid used in BTB; may be masterid */
    )
#else
		   struct bpred_update_t *dir_update_ptr)	/* pred state pointer */
#endif
{
    struct bpred_btb_ent_t *pbtb = NULL;
    struct bpred_btb_ent_t *lruhead = NULL, *lruitem = NULL;
    int index, i;

    /* don't change bpred state for non-branch instructions or if this
     * is a stateless predictor*/
#ifdef LOAD_PREDICTOR
    if (!(MD_OP_FLAGS (op) & F_CTRL) && (pred->class != LDPred2bit))
	return;
#else
    if (!(MD_OP_FLAGS (op) & F_CTRL))
	return;
#endif

    /* Have a branch here */

    if (correct)
	pred->addr_hits++;

    if (!!pred_taken == !!taken)
	pred->dir_hits++;
    else
	pred->misses++;

    if (dir_update_ptr->dir.ras)
    {
	pred->used_ras++;
	if (correct)
	    pred->ras_hits++;
    }
    else if ((MD_OP_FLAGS (op) & (F_CTRL | F_COND)) == (F_CTRL | F_COND))
    {
	/*lechen, 2bcgskew */
	assert ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND));
	if (pred->class == BPred2bcgskew)
	    bpred_2bcgskew_update (pred, baddr, taken, pred_taken, taken == pred_taken, dir_update_ptr
#ifdef SMT_SS
				   , threadid
#endif
		);
	else if (dir_update_ptr->dir.meta)
	    pred->used_2lev++;
	else
	    pred->used_bimod++;

/*lechen*/
	if (combined2)
	{			/*lookup and set prediction, for another predictor */
#if 0				/* this had been from lechen */
	    if (!mystricmp (pred_type2, "2bcgskew"))
		bpred_2bcgskew_update2 (pred, baddr, taken, dir_update_ptr->pred_taken2, taken == dir_update_ptr->pred_taken2, dir_update_ptr);

	    else
		bpred_update2 (pred, taken, dir_update_ptr->pred_taken2, baddr, op, dir_update_ptr);
#else /* grbriggs: did not copy bpred_update2 from lechen */
	    panic ("bpred_2bcgskew_update2 and bpred_update2 not present.");
#endif
/* not including Gonzalez selector code from lechen
			if(use_sel && 
				dir_update_ptr->pred_taken!=dir_update_ptr->pred_taken2)
					sel_update(dir_update_ptr->hist_tab_indx, 
								dir_update_ptr->pred_taken, 
								dir_update_ptr->pred_taken==!!taken);
*/
	}
    }

    /* keep stats about JR's; also, but don't change any bpred state for JR's
     * which are returns unless there's no retstack */
    if (MD_IS_INDIR (op))
    {
	pred->jr_seen++;
	if (correct)
	    pred->jr_hits++;

	if (!dir_update_ptr->dir.ras)
	{
	    pred->jr_non_ras_seen++;
	    if (correct)
		pred->jr_non_ras_hits++;
	}
	else
	{
	    /* return that used the ret-addr stack; no further work to do */
	    return;
	}
    }

    /* Can exit now if this is a stateless predictor */
    if (pred->class == BPredNotTaken || pred->class == BPredTaken)
	return;


#ifdef LOAD_PREDICTOR
    if (pred->class == LDPred2bit)
    {				/* update state  */
	if (dir_update_ptr->pdir1)
	{
	    if (taken)
	    {
		if (*dir_update_ptr->pdir1 < 3)
		    ++ * dir_update_ptr->pdir1;
	    }
	    else
	    {			/* miss */
		if (*dir_update_ptr->pdir1 > 0)
		    *dir_update_ptr->pdir1 = 0;
	    }
	}
	return;
    }
#endif

    /* 
     * Now we know the branch didn't use the ret-addr stack, and that this
     * is a stateful predictor
     */

#ifdef SMT_SS
#ifdef RAS_BUG_COMPATIBLE
    /* if function call, push return-address onto return-address stack */
    if (MD_IS_CALL (op) && pred->retstack[threadid].size)
    {
	pred->retstack[threadid].tos = (pred->retstack[threadid].tos + 1) % pred->retstack[threadid].size;
	pred->retstack[threadid].stack[pred->retstack[threadid].tos].target = baddr + sizeof (md_inst_t);
	pred->retstack_pushes++;
    }
#endif /* RAS_BUG_COMPATIBLE */
#else
#ifdef RAS_BUG_COMPATIBLE
    /* if function call, push return-address onto return-address stack */
    if (MD_IS_CALL (op) && pred->retstack.size)
    {
	pred->retstack.tos = (pred->retstack.tos + 1) % pred->retstack.size;
	pred->retstack.stack[pred->retstack.tos].target = baddr + sizeof (md_inst_t);
	pred->retstack_pushes++;
    }
#endif /* RAS_BUG_COMPATIBLE */
#endif

    /*lechen, hist_imm added below, default FALSE */
    /* update L1 table if appropriate */
    /* L1 table is updated unconditionally for combining predictor too */
    if (!hist_imm && (MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND) && (pred->class == BPred2Level || pred->class == BPredComb))
    {
	int l1index, shift_reg;

	/* also update appropriate L1 history register */
	l1index = (baddr >> MD_BR_SHIFT) & (pred->dirpred.twolev->config.two.l1size - 1);
#ifdef SMT_SS
	shift_reg = (pred->dirpred.twolev->config.two.shiftregs[threadid][l1index] << 1) | (!!taken);
	pred->dirpred.twolev->config.two.shiftregs[threadid][l1index] =
	    /* lechen commented after the & */
	    shift_reg /*& ((1 << pred->dirpred.twolev->config.two.shift_width) - 1) */ ;
#else
	shift_reg = (pred->dirpred.twolev->config.two.shiftregs[l1index] << 1) | (!!taken);
	pred->dirpred.twolev->config.two.shiftregs[l1index] =
	    /* lechen commented after the & */
	    shift_reg /*& ((1 << pred->dirpred.twolev->config.two.shift_width) - 1) */ ;
#endif
    }
    /*lechen, need to update lev1 history? */
    else if (hist_imm && combined2 && (taken == pred_taken) && (MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND))
	bpred_update_history1 (pred, taken, baddr, dir_update_ptr
#ifdef SMT_SS
			       , threadid
#endif
	    );
    /*lechen, have 2 hybrids each with 2 2lev now */
    else if (!hist_imm && (MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND)) != (F_CTRL | F_UNCOND) && (pred->class == BPredComb2))
    {
	int l1index, shift_reg;

	/* also update appropriate L1 history register */
	l1index = (baddr >> MD_BR_SHIFT) & (pred->dirpred.twolev1->config.two.l1size - 1);
#ifdef SMT_SS
	shift_reg = (pred->dirpred.twolev1->config.two.shiftregs[threadid][l1index] << 1) | (!!taken);
	pred->dirpred.twolev1->config.two.shiftregs[threadid][l1index] = shift_reg /*& ((1 << pred->dirpred.twolev1->config.two.shift_width) - 1) */ ;
#else
	shift_reg = (pred->dirpred.twolev1->config.two.shiftregs[l1index] << 1) | (!!taken);
	pred->dirpred.twolev1->config.two.shiftregs[l1index] = shift_reg /*& ((1 << pred->dirpred.twolev1->config.two.shift_width) - 1) */ ;
#endif

	l1index = (baddr >> MD_BR_SHIFT) & (pred->dirpred.twolev12->config.two.l1size - 1);
#ifdef SMT_SS
	shift_reg = (pred->dirpred.twolev12->config.two.shiftregs[threadid][l1index] << 1) | (!!taken);
	pred->dirpred.twolev12->config.two.shiftregs[threadid][l1index] = shift_reg /*& ((1 << pred->dirpred.twolev12->config.two.shift_width) - 1) */ ;
#else
	shift_reg = (pred->dirpred.twolev12->config.two.shiftregs[l1index] << 1) | (!!taken);
	pred->dirpred.twolev12->config.two.shiftregs[l1index] = shift_reg /*& ((1 << pred->dirpred.twolev12->config.two.shift_width) - 1) */ ;
#endif

    }


    /* find BTB entry if it's a taken branch (don't allocate for non-taken) */
    if (taken)
    {
	index = (baddr >> MD_BR_SHIFT) & (pred->btb.sets - 1);

	if (pred->btb.assoc > 1)
	{
	    index *= pred->btb.assoc;

	    /* Now we know the set; look for a PC match; also identify
	     * MRU and LRU items */
	    for (i = index; i < (index + pred->btb.assoc); i++)
	    {
#ifdef SMT_SS
		if ((pred->btb.btb_data[i].addr_id.addr == baddr) && (pred->btb.btb_data[i].addr_id.threadid == btb_threadid))
#else
		if (pred->btb.btb_data[i].addr == baddr)
#endif
		{
		    /* match */
		    assert (!pbtb);
		    pbtb = &pred->btb.btb_data[i];
		}

		dassert (pred->btb.btb_data[i].prev != pred->btb.btb_data[i].next);
		if (pred->btb.btb_data[i].prev == NULL)
		{
		    /* this is the head of the lru list, ie current MRU item */
		    dassert (lruhead == NULL);
		    lruhead = &pred->btb.btb_data[i];
		}
		if (pred->btb.btb_data[i].next == NULL)
		{
		    /* this is the tail of the lru list, ie the LRU item */
		    dassert (lruitem == NULL);
		    lruitem = &pred->btb.btb_data[i];
		}
	    }
	    dassert (lruhead && lruitem);

	    if (!pbtb)
		/* missed in BTB; choose the LRU item in this set as the victim */
		pbtb = lruitem;
	    /* else hit, and pbtb points to matching BTB entry */

	    /* Update LRU state: selected item, whether selected because it
	     * matched or because it was LRU and selected as a victim, becomes 
	     * MRU */
	    if (pbtb != lruhead)
	    {
		/* this splices out the matched entry... */
		if (pbtb->prev)
		    pbtb->prev->next = pbtb->next;
		if (pbtb->next)
		    pbtb->next->prev = pbtb->prev;
		/* ...and this puts the matched entry at the head of the list */
		pbtb->next = lruhead;
		pbtb->prev = NULL;
		lruhead->prev = pbtb;
		dassert (pbtb->prev || pbtb->next);
		dassert (pbtb->prev != pbtb->next);
	    }
	    /* else pbtb is already MRU item; do nothing */
	}
	else
	    pbtb = &pred->btb.btb_data[index];
    }

/*lechen changed the behavior for  2bcgskew and comb2 by adding the 1 following line*/
    if (pred->class != BPred2bcgskew && pred->class != BPredComb2)
    {
	/* 
	 * Now 'p' is a possibly null pointer into the direction prediction table, 
	 * and 'pbtb' is a possibly null pointer into the BTB (either to a 
	 * matched-on entry or a victim which was LRU in its set)
	 */

	/* update state (but not for jumps) */
	if (dir_update_ptr->pdir1)
	{
	    if (taken)
	    {
		if (*dir_update_ptr->pdir1 < 3)
		    ++ * dir_update_ptr->pdir1;
	    }
	    else
	    {			/* not taken */
		if (*dir_update_ptr->pdir1 > 0)
		    -- * dir_update_ptr->pdir1;
	    }
	}

	/* combining predictor also updates second predictor and meta predictor */
	/* second direction predictor */
	if (dir_update_ptr->pdir2)
	{
	    if (taken)
	    {
		if (*dir_update_ptr->pdir2 < 3)
		    ++ * dir_update_ptr->pdir2;
	    }
	    else
	    {			/* not taken */
		if (*dir_update_ptr->pdir2 > 0)
		    -- * dir_update_ptr->pdir2;
	    }
	}

	/* meta predictor */
	if (dir_update_ptr->pmeta)
	{
	    if (dir_update_ptr->dir.bimod != dir_update_ptr->dir.twolev)
	    {
		/* we only update meta predictor if directions were different */
		if (dir_update_ptr->dir.twolev == (unsigned int) taken)
		{
		    /* 2-level predictor was correct */
		    if (*dir_update_ptr->pmeta < 3)
			++ * dir_update_ptr->pmeta;
		}
		else
		{
		    /* bimodal predictor was correct */
		    if (*dir_update_ptr->pmeta > 0)
			-- * dir_update_ptr->pmeta;
		}
	    }
	}
    }

    /* update BTB (but only for taken branches) */
    if (pbtb)
    {
	/* update current information */
	dassert (taken);

#ifdef SMT_SS
	if ((pbtb->addr_id.addr == baddr) && (pbtb->addr_id.threadid == btb_threadid))
#else
	if (pbtb->addr == baddr)
#endif
	{
	    if (!correct)
		pbtb->target = btarget;
	}
	else
	{
	    /* enter a new branch in the table */
#ifdef SMT_SS
	    pbtb->addr_id.addr = baddr;
	    pbtb->addr_id.threadid = btb_threadid;
#else
	    pbtb->addr = baddr;
#endif
	    pbtb->op = op;
	    pbtb->target = btarget;
	}
    }
}

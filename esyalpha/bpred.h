/*
 * bpred.h - branch predictor interfaces
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
 * $Id: bpred.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: bpred.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:03  xue
 * CMP_NETWORK
 *
 * Revision 1.2  2009/05/15 16:17:13  garg
 * Changes in branch predictor statistics output
 *
 * Revision 1.1.1.1  2008/10/20 15:44:53  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.9  2004/07/21 15:18:47  grbriggs
 * Make 2bcgskew branch predictor shift register be per-thread.
 *
 * Revision 1.8  2004/07/20 20:43:25  grbriggs
 * Allow one to specify a different threadid for BTB entries.
 *
 * Revision 1.7  2004/05/24 17:10:29  grbriggs
 * some SMT changes for code copied from lechen
 *
 * Revision 1.6  2004/05/21 22:04:47  grbriggs
 * Updates so that the branch_????2 variables are per-thread.
 *
 * Revision 1.5  2004/05/21 19:05:33  grbriggs
 * Removed unneeded declaration of bpred_update2()
 *
 * Revision 1.4  2004/05/21 16:46:23  grbriggs
 * add declarations from lechen
 *
 * Revision 1.3  2004/05/20 18:05:11  grbriggs
 * Update due to changes in bpred.c supporting 2bcskew. Not tested.
 *
 * Revision 1.2  2004/05/20 16:49:31  grbriggs
 * convert from DOS to unix ('dos2unix bpred.h')
 *
 * Revision 1.1  2004/05/14 14:48:41  ninelson
 * Initial revision
 *
 * Revision 1.1.1.3  2003/11/14 22:41:46  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.2  2003/10/20 13:58:35  ninelson
 * new from Ali
 *
 * Revision 1.1.1.1  2003/01/09 04:26:41  essawy
 * SS_SMT initial import into CVS
 *
 * Revision 1.5  1998/08/31 17:12:11  taustin
 * eliminated unportable "uint" reference
 *
 * Revision 1.4  1998/08/27 08:00:15  taustin
 * implemented host interface description in host.h
 * explicit BTB sizing option added to branch predictors, use
 *       "-btb" option to configure BTB
 * return address stack (RAS) performance stats improved
 *
 * Revision 1.1.1.1  1997/05/22 18:04:05  aklauser
 *
 * Revision 1.8  1997/05/01 20:23:06  skadron
 * BTB bug fixes; jumps no longer update direction state; non-taken
 *    branches non longer update BTB
 *
 * Revision 1.7  1997/05/01 00:05:51  skadron
 * Separated BTB from direction-predictor
 *
 * Revision 1.6  1997/04/29  23:50:44  skadron
 * Added r31 info to distinguish between return-JRs and other JRs for bpred
 *
 * Revision 1.5  1997/04/29  22:53:10  skadron
 * Hopefully bpred is now right: bpred now allocates entries only for
 *    branches; on a BTB miss it still returns a direction; and it uses a
 *    return-address stack.  Returns are not yet distinguished among JR's
 *
 * Revision 1.4  1997/04/28  17:37:09  skadron
 * Bpred now allocates entries for any instruction instead of only
 *    branches; also added return-address stack
 *
 * Revision 1.3  1997/04/24  16:57:27  skadron
 * Bpred used to return no prediction if the indexing branch didn't match
 *    in the BTB.  Now it can predict a direction even on a BTB address
 *    conflict
 *
 * Revision 1.2  1997/03/25 16:17:21  skadron
 * Added function called after priming
 *
 * Revision 1.1  1997/02/16  22:23:54  skadron
 * Initial revision
 *
 *
 */

#ifndef BPRED_H
#define BPRED_H

#define dassert(a) assert(a)

#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "stats.h"

#include "smt.h"

//#ifdef SMT_SS
//#include "context.h"
//#endif

/*
 * This module implements a number of branch predictor mechanisms.  The
 * following predictors are supported:
 *
 *	BPred2Level:  two level adaptive branch predictor
 *
 *		It can simulate many prediction mechanisms that have up to
 *		two levels of tables. Parameters are:
 *		     N   # entries in first level (# of shift register(s))
 *		     W   width of shift register(s)
 *		     M   # entries in 2nd level (# of counters, or other FSM)
 *		One BTB entry per level-2 counter.
 *
 *		Configurations:   N, W, M
 *
 *		    counter based: 1, 0, M
 *
 *		    GAg          : 1, W, 2^W
 *		    GAp          : 1, W, M (M > 2^W)
 *		    PAg          : N, W, 2^W
 *		    PAp          : N, W, M (M == 2^(N+W))
 *
 *	BPred2bit:  a simple direct mapped bimodal predictor
 *
 *		This predictor has a table of two bit saturating counters.
 *		Where counter states 0 & 1 are predict not taken and
 *		counter states 2 & 3 are predict taken, the per-branch counters
 *		are incremented on taken branches and decremented on
 *		no taken branches.  One BTB entry per counter.
 *
 *	BPredTaken:  static predict branch taken
 *
 *	BPredNotTaken:  static predict branch not taken
 *
 * 	BPred2bcgskew
 *
 */

/* branch predictor types */
enum bpred_class {
  BPredComb,                    /* combined predictor (McFarling) */
  BPred2Level,			/* 2-level correlating pred w/2-bit counters */
  BPred2bit,			/* 2-bit saturating cntr pred (dir mapped) */
#ifdef LOAD_PREDICTOR
  LDPred2bit,			/* 2-bit saturating cntr pred (dir mapped) */
#endif
  BPredTaken,			/* static predict taken */
  BPredNotTaken,		/* static predict not taken */

/*lechen*/
  BPredARVI,
  BPredComb2,
  BPred2bcgskew, /*from "De-aliased Hybrid Branch Predictors"*/

  BPred_NUM
};

#ifdef SMT_SS
struct addr_id_t
{
  int threadid;
  md_addr_t addr;		/* address of branch being tracked */
};
#endif

/* an entry in a BTB */
struct bpred_btb_ent_t {
#ifdef SMT_SS
  struct addr_id_t addr_id;		/* address of branch being tracked  and threadid */
#else
  md_addr_t addr;		/* address of branch being tracked */
#endif
  enum md_opcode op;		/* opcode of branch corresp. to addr */
  md_addr_t target;		/* last destination of branch when taken */
  struct bpred_btb_ent_t *prev, *next; /* lru chaining pointers */
};

/* direction predictor def */
struct bpred_dir_t {
  enum bpred_class class;	/* type of predictor */
  union {
    struct {
      unsigned int size;	/* number of entries in direct-mapped table */
      unsigned char *table;	/* prediction state table */
    } bimod;
    struct {
      int l1size;		/* level-1 size, number of history regs */
      int l2size;		/* level-2 size, number of pred states */
      int shift_width;		/* amount of history in level-1 shift regs */
      int xor;			/* history xor address flag */
#ifdef SMT_SS /* Note: have to consider no. of threads running while computing total predictor size */
      int *shiftregs[MAXTHREADS];	/* level-1 history table */
#else
      int *shiftregs;		/* level-1 history table */
#endif
      unsigned char *l2table;	/* level-2 prediction state table */
    } two;
	/*lechen, for 2bcgskew*/
	struct {
		unsigned int size; /* number of entries in a bank */
		unsigned int log_size; /* log2 of size, i.e., index width*/
#ifdef SMT_SS
		int shiftreg[MAXTHREADS]; /* history register for each thread */
#else
		int shiftreg; /*history register*/
#endif
		unsigned char *table; /* prediction state table */
		int shift_width; /*history length*/
	} bank; /*a bank is a prediction table with a unique indexing scheme*/
  } config;
};

/* branch predictor def */
struct bpred_t {
  enum bpred_class class;	/* type of predictor */
  struct {
    struct bpred_dir_t *bimod;	  /* first direction predictor */
    struct bpred_dir_t *twolev;	  /* second direction predictor */
    struct bpred_dir_t *meta;	  /* meta predictor */

/*lechen*/
    struct bpred_dir_t *bimod2;	  /* first direction predictor 2*/
    struct bpred_dir_t *twolev2;	  /* second direction predictor 2*/
    struct bpred_dir_t *meta2;	  /* meta predictor 2*/
	struct bvit_t *b_info_t;      /* bvit of arvi predictor */

/*lechen, need 2 hybrid predictors, each with 2 twolev, this is lev1*/
	struct bpred_dir_t *twolev1;
	struct bpred_dir_t *twolev12;

/*lechen, for 2bcgskew*/
	struct bpred_dir_t *bim1; /*enhanced bimod predictor 1*/
	struct bpred_dir_t *g01; /*bank g0 1*/
	struct bpred_dir_t *g11; /*bank g1 1*/
	/*use what's available, struct bpred_dir_t *meta1; meta 1*/
	struct bpred_dir_t *bim2; /*enhanced bimod predictor 2*/
	struct bpred_dir_t *g02; /*bank g0 2*/
	struct bpred_dir_t *g12; /*bank g1 2*/
	/*use what's available, struct bpred_dir_t *meta2; meta 2*/
  } dirpred;

  struct {
    int sets;			/* num BTB sets */
    int assoc;			/* BTB associativity */
    struct bpred_btb_ent_t *btb_data; /* BTB addr-prediction table */
  } btb;

  struct {
    int size;			/* return-address stack size */
    int tos;			/* top-of-stack */
    struct bpred_btb_ent_t *stack; /* return-address stack */
#ifdef SMT_SS
  } retstack[MAXTHREADS];
#else
  } retstack;
#endif

  /* stats */
  counter_t addr_hits;		/* num correct addr-predictions */
  counter_t dir_hits;		/* num correct dir-predictions (incl addr) */
  counter_t used_ras;		/* num RAS predictions used */
  counter_t used_bimod;		/* num bimodal predictions used (BPredComb) */
  counter_t used_2lev;		/* num 2-level predictions used (BPredComb) */
  counter_t jr_hits;		/* num correct addr-predictions for JR's */
  counter_t jr_seen;		/* num JR's seen */
  counter_t jr_non_ras_hits;	/* num correct addr-preds for non-RAS JR's */
  counter_t jr_non_ras_seen;	/* num non-RAS JR's seen */
  counter_t misses;		/* num incorrect predictions */

  counter_t lookups;		/* num lookups */
  counter_t retstack_pops;	/* number of times a value was popped */
  counter_t retstack_pushes;	/* number of times a value was pushed */
  counter_t ras_hits;		/* num correct return-address predictions */
};

/* branch predictor update information */
struct bpred_update_t {
  char *pdir1;		/* direction-1 predictor counter */
  char *pdir2;		/* direction-2 predictor counter */
  char *pmeta;		/* meta predictor counter */

/*lechen*/
  char *pdir12;		/* direction-1 predictor counter 2 */
  char *pdir22;		/* direction-2 predictor counter 2 */
  char *pmeta2;		/* meta predictor counter 2 */

/*lechen, for 2bcgskew*/
  char *pdir3; /*direction-3 predictor counter*/
  char *pdir32; /*direction-3 predictor counter 2*/

unsigned int pred_taken : 1; /*prediction made by 1st level predictor*/
unsigned int pred_taken2 : 1; /*prediction made by 2nd level predictor*/
unsigned int twolev2_used : 1; /*twolev2_used?*/

unsigned int hist_shift_out : 1; /*1 bit shifted out from history*/
unsigned int hist_shift_out2 : 1; /*1 bit shifted out from history2*/
unsigned short hist_tab_indx; /*index into the Gonzalez selector*/

int bucket_index; /*index for a bucket in the table*/

/*saved branch history, for recovery*/
unsigned int history;
unsigned int history2;

/*current branch tail2, used to recover lev1 history*/
int branch_tail2;

md_addr_t *stack; /*record current stack content*/

  struct {		/* predicted directions */
    unsigned int ras    : 1;	/* RAS used */
    unsigned int bimod  : 1;    /* bimodal predictor */
    unsigned int twolev : 1;    /* 2-level predictor */
    unsigned int twolev2 : 1;    /* 2-level predictor */
    unsigned int twolev1 : 1;    /* 2-level predictor */
    unsigned int twolev12 : 1;    /* 2-level predictor */
    unsigned int meta   : 1;    /* meta predictor (0..bimod / 1..2lev) */
    unsigned int meta2   : 1;    /* meta predictor (0..bimod / 1..2lev) */
	/*lechen predictions from 2bcgskew*/
    unsigned int bim1 : 1;    /* bim 1 prediction */
    unsigned int g01 : 1;    /* g0 1 prediction */
    unsigned int g11 : 1;    /* g1 1 prediction */
    unsigned int bim2 : 1;    /* bim 2 prediction */
    unsigned int g02 : 1;    /* g0 2 prediction */
    unsigned int g12 : 1;    /* g1 2 prediction */

  } dir;
};

/* create a branch predictor */
struct bpred_t *			/* branch predictory instance */
bpred_create(enum bpred_class class,	/* type of predictor to create */
	     unsigned int bimod_size,	/* bimod table size */
	     unsigned int l1size,	/* level-1 table size */
	     unsigned int l2size,	/* level-2 table size */
	     unsigned int meta_size,	/* meta predictor table size */
	     unsigned int shift_width,	/* history register width */
	     unsigned int xor,		/* history xor address flag */
	     unsigned int btb_sets,	/* number of sets in BTB */ 
	     unsigned int btb_assoc,	/* BTB associativity */
	     unsigned int retstack_size);/* num entries in ret-addr stack */

/*lechen, create a 2Bc-gskew predictor*/
struct bpred_t *            /* branch predictory instance */
bpred_2bcgskew_create( /*bimod table size*/unsigned int bimod_size,
		/*other 3 tables size*/unsigned int size,
		/*bimod table history length*/unsigned int shift_width_bimod,
		/*g0 table history length*/unsigned int shift_width_g0,
		/*g1 table history length*/unsigned int shift_width_g1,
		/*meta table history length*/unsigned int shift_width_meta,
		/* btb sets */unsigned int btb_sets,
		/* btb assoc */unsigned int btb_assoc,
		/* ret-addr stack size */unsigned int ras_size);
#if 0
void bpred_2bcgskew_create2( /*bimod table size*/unsigned int bimod_size,
		/*other 3 tables size*/unsigned int size,
		/*bimod table history length*/unsigned int shift_width_bimod,
		/*g0 table history length*/unsigned int shift_width_g0,
		/*g1 table history length*/unsigned int shift_width_g1,
		/*meta table history length*/unsigned int shift_width_meta,
		/*pred*/struct bpred_t *pred);
#endif
/*lechen, speculatively update the history, for 2bc-gskew only for now*/
void bpred_update_history(struct bpred_t *pred, struct bpred_update_t *dir_update_ptr 
#ifdef SMT_SS 
		, int threadid /* grbriggs: added so 2bcgskew uses SMT */
#endif
		);


/*lechen, update level 1 pred history, from lev 2*/
void bpred_update_history1(struct bpred_t *pred, int taken, md_addr_t baddr, struct bpred_update_t *dir_update_ptr
#ifdef SMT_SS 
		, int threadid /* grbriggs: added so 2bcgskew uses SMT */
#endif
		);


#ifdef SMT_SS
void bpred_2bcgskew_lookup(struct bpred_t *pred, struct bpred_update_t *dir_update_ptr, md_addr_t baddr, int threadid);
#else
void bpred_2bcgskew_lookup(struct bpred_t *pred, struct bpred_update_t *dir_update_ptr, md_addr_t baddr);
#endif
#if 0 /* unused */
void bpred_2bcgskew_lookup2(struct bpred_t *pred, struct bpred_update_t *dir_update_ptr, md_addr_t baddr);
#endif

/*lechen, update the 2Bc-skew, policy is from
* "Design Tradeoffs for the Alpha EV8 Conditional Branch Predictor*/
void bpred_2bcgskew_update(struct bpred_t *pred, /* branch predictor instance */
         md_addr_t baddr,       /* branch address */
		 int taken,         /* non-zero if branch was taken */
		 int pred_taken,        /* non-zero if branch was pred taken */
		 int correct,       /* was earlier addr prediction ok? */
		 struct bpred_update_t *dir_update_ptr /* pred state pointer */
#ifdef SMT_SS
		, int threadid      /* thread id */
#endif
			  );

/*lechen, update the 2Bc-skew, policy is from
* "Design Tradeoffs for the Alpha EV8 Conditional Branch Predictor*/
void bpred_2bcgskew_update2(struct bpred_t *pred, /* branch predictor instance */
         md_addr_t baddr,       /* branch address */
		 int taken,         /* non-zero if branch was taken */
		 int pred_taken,        /* non-zero if branch was pred taken */
		 int correct,       /* was earlier addr prediction ok? */
		 struct bpred_update_t *dir_update_ptr);/* pred state pointer */
 
/* create a branch direction predictor */
struct bpred_dir_t *		/* branch direction predictor instance */
bpred_dir_create (
  enum bpred_class class,	/* type of predictor to create */
  unsigned int l1size,		/* level-1 table size */
  unsigned int l2size,		/* level-2 table size (if relevant) */
  unsigned int shift_width,	/* history register width */
  unsigned int xor);	   	/* history xor address flag */

/* print branch predictor configuration */
void
bpred_config(struct bpred_t *pred,	/* branch predictor instance */
	     FILE *stream);		/* output stream */

/* print predictor stats */
void
bpred_stats(struct bpred_t *pred,	/* branch predictor instance */
	    FILE *stream);		/* output stream */

/* register branch predictor stats */
void
bpred_reg_stats(struct bpred_t *pred,	/* branch predictor instance */
		struct stat_sdb_t *sdb, int threadid);/* stats database */

/* reset stats after priming, if appropriate */
void bpred_after_priming(struct bpred_t *bpred);

/* probe a predictor for a next fetch address, the predictor is probed
   with branch address BADDR, the branch target is BTARGET (used for
   static predictors), and OP is the instruction opcode (used to simulate
   predecode bits; a pointer to the predictor state entry (or null for jumps)
   is returned in *DIR_UPDATE_PTR (used for updating predictor state),
   and the non-speculative top-of-stack is returned in stack_recover_idx 
   (used for recovering ret-addr stack after mis-predict).  */
md_addr_t				/* predicted branch target addr */
bpred_lookup(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* branch target if taken */
	     enum md_opcode op,		/* opcode of instruction */
	     int is_call,		/* non-zero if inst is fn call */
	     int is_return,		/* non-zero if inst is fn return */
	     struct bpred_update_t *dir_update_ptr, /* pred state pointer */
#ifdef SMT_SS
	     int *stack_recover_idx,	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
             int threadid,
	     int btb_threadid		/* Threadid used for BTB lookup. May be masterid instead. */
            );
#else
	     int *stack_recover_idx);	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
#endif

/* Speculative execution can corrupt the ret-addr stack.  So for each
 * lookup we return the top-of-stack (TOS) at that point; a mispredicted
 * branch, as part of its recovery, restores the TOS using this value --
 * hopefully this uncorrupts the stack. */
void
bpred_recover(struct bpred_t *pred,	/* branch predictor instance */
	      md_addr_t baddr,		/* branch address */
#ifdef SMT_SS
	      int stack_recover_idx,	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
              int threadid);
#else
	      int stack_recover_idx);	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
#endif

/* update the branch predictor, only useful for stateful predictors; updates
   entry for instruction type OP at address BADDR.  BTB only gets updated
   for branches which are taken.  Inst was determined to jump to
   address BTARGET and was taken if TAKEN is non-zero.  Predictor 
   statistics are updated with result of prediction, indicated by CORRECT and 
   PRED_TAKEN, predictor state to be updated is indicated by *DIR_UPDATE_PTR 
   (may be NULL for jumps, which shouldn't modify state bits).  Note if
   bpred_update is done speculatively, branch-prediction may get polluted. */
void
bpred_update(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* resolved branch target */
	     int taken,			/* non-zero if branch was taken */
	     int pred_taken,		/* non-zero if branch was pred taken */
	     int correct,		/* was earlier prediction correct? */
	     enum md_opcode op,		/* opcode of instruction */
#ifdef SMT_SS
	     struct bpred_update_t *dir_update_ptr, /* pred state pointer */
             int threadid,
	     int btb_threadid		/* threadid used in BTB; may be masterid */
            );
#else
	     struct bpred_update_t *dir_update_ptr);/* pred state pointer */
#endif


#ifdef foo0
/* OBSOLETE */
/* dump branch predictor state (for debug) */
void
bpred_dump(struct bpred_t *pred,	/* branch predictor instance */
	   FILE *stream);		/* output stream */
#endif

#endif /* BPRED_H */

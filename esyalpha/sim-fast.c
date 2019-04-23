/*
 * sim-fast.c - sample fast functional simulator implementation
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
 * $Id: sim-fast.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: sim-fast.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:03  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:54  garg
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
 * Revision 1.5  1998/08/27 16:26:37  taustin
 * USE_JUMP_TABLE option now autodetects GNU GCC jump table support
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 * instruction predecoding moved to loader module
 * Alpha target support added
 * added support for quadword's
 * added fault support
 *
 * Revision 1.4  1997/03/11  01:28:36  taustin
 * updated copyright
 * simulation now fails when DLite! support requested
 * long/int tweaks made for ALPHA target support
 * supported added for non-GNU C compilers
 *
 * Revision 1.3  1997/01/06  16:04:21  taustin
 * comments updated
 * SPARC-specific compilation supported deleted (most opts now generalized)
 * new stats and options package support added
 * NO_INSN_COUNT added for conditional instruction count support
 * USE_JUMP_TABLE added to support simple and complex main loops
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 * This file implements a very fast functional simulator.  This functional
 * simulator implementation is much more difficult to digest than the simpler,
 * cleaner sim-safe functional simulator.  By default, this simulator performs
 * no instruction error checking, as a result, any instruction errors will
 * manifest as simulator execution errors, possibly causing sim-fast to
 * execute incorrectly or dump core.  Such is the price we pay for speed!!!!
 *
 * The following options configure the bag of tricks used to make sim-fast
 * live up to its name.  For most machines, defining all the options results
 * in the fastest functional simulator.
 */

/* don't count instructions flag, enabled by default, disable for inst count */
#undef NO_INSN_COUNT

#ifdef __GNUC__
/* faster dispatch mechanism, requires GNU GCC C extensions, CAVEAT: some
   versions of GNU GCC core dump when optimizing the jump table code with
   optimization levels higher than -O1 */
/* #define USE_JUMP_TABLE */
#endif /* __GNUC__ */

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "dlite.h"
#include "sim.h"

/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;

#ifdef TARGET_ALPHA
/* predecoded text memory */
static struct mem_t *dec = NULL;
#endif

/* register simulator-specific options */
void
sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-fast: This simulator implements a very fast functional simulator.  This\n"
"functional simulator implementation is much more difficult to digest than\n"
"the simpler, cleaner sim-safe functional simulator.  By default, this\n"
"simulator performs no instruction error checking, as a result, any\n"
"instruction errors will manifest as simulator execution errors, possibly\n"
"causing sim-fast to execute incorrectly or dump core.  Such is the\n"
"price we pay for speed!!!!\n"
		 );
}

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb, int argc, char **argv)
{
  if (dlite_active)
    fatal("sim-fast does not support DLite debugging");
}

/* register simulator-specific statistics */
void
sim_reg_stats(struct stat_sdb_t *sdb)
{
#ifndef NO_INSN_COUNT
  stat_reg_counter(sdb, "sim_num_insn",
		   "total number of instructions executed",
		   &sim_num_insn, sim_num_insn, NULL);
#endif /* !NO_INSN_COUNT */
  stat_reg_int(sdb, "sim_elapsed_time",
	       "total simulation time in seconds",
	       &sim_elapsed_time, 0, NULL);
#ifndef NO_INSN_COUNT
  stat_reg_formula(sdb, "sim_inst_rate",
		   "simulation speed (in insts/sec)",
		   "sim_num_insn / sim_elapsed_time", NULL);
#endif /* !NO_INSN_COUNT */
  ld_reg_stats(sdb);
  mem_reg_stats(mem, sdb);
}

/* initialize the simulator */
void
sim_init(void)
{
  /* allocate and initialize register file */
  regs_init(&regs);

  /* allocate and initialize memory space */
  mem = mem_create("mem");
  mem_init(mem);
}

/* load program into simulated state */
void
sim_load_prog(char *fname,		/* program to load */
	      int argc, char **argv,	/* program arguments */
	      char **envp)		/* program environment */
{
  /* load program text and data, set up environment, memory, and regs */
  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);

#ifdef TARGET_ALPHA
  /* pre-decode text segment */
  {
    unsigned i, num_insn = (ld_text_size + 3) / 4;

    fprintf(stderr, "** pre-decoding %u insts...", num_insn);

    /* allocate decoded text space */
    dec = mem_create("dec");

    for (i=0; i < num_insn; i++)
      {
	enum md_opcode op;
	md_inst_t inst;
	md_addr_t PC;

	/* compute PC */
	PC = ld_text_base + i * sizeof(md_inst_t);

	/* get instruction from memory */
	inst = __UNCHK_MEM_READ(mem, PC, md_inst_t);

	/* decode the instruction */
	MD_SET_OPCODE(op, inst);

	/* insert into decoded opcode space */
	MEM_WRITE_WORD(dec, PC << 1, (word_t)op);
	MEM_WRITE_WORD(dec, (PC << 1)+sizeof(word_t), inst);
      }
    fprintf(stderr, "done\n");
  }
#endif /* TARGET_ALPHA */
}

/* print simulator-specific configuration information */
void
sim_aux_config(FILE *stream)
{
  /* nothing currently */
}

/* dump simulator-specific auxiliary simulator statistics */
void
sim_aux_stats(FILE *stream)
{
  /* nada */
}

/* un-initialize simulator-specific state */
void
sim_uninit(void)
{
  /* nada */
}

/*
 * configure the execution engine
 */

/* next program counter */
#define SET_NPC(EXPR)		(regs.regs_NPC = (EXPR))

/* current program counter */
#define CPC			(regs.regs_PC)

/* general purpose registers */
#define GPR(N)			(regs.regs_R[N])
#define SET_GPR(N,EXPR)		(regs.regs_R[N] = (EXPR))

#if defined(TARGET_PISA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_L(N)		(regs.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)	(regs.regs_F.l[(N)] = (EXPR))
#define FPR_F(N)		(regs.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)	(regs.regs_F.f[(N)] = (EXPR))
#define FPR_D(N)		(regs.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)	(regs.regs_F.d[(N) >> 1] = (EXPR))

/* miscellaneous register accessors */
#define SET_HI(EXPR)		(regs.regs_C.hi = (EXPR))
#define HI			(regs.regs_C.hi)
#define SET_LO(EXPR)		(regs.regs_C.lo = (EXPR))
#define LO			(regs.regs_C.lo)
#define FCC			(regs.regs_C.fcc)
#define SET_FCC(EXPR)		(regs.regs_C.fcc = (EXPR))

#elif defined(TARGET_ALPHA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_Q(N)		(regs.regs_F.q[N])
#define SET_FPR_Q(N,EXPR)	(regs.regs_F.q[N] = (EXPR))
#define FPR(N)			(regs.regs_F.d[N])
#define SET_FPR(N,EXPR)		(regs.regs_F.d[N] = (EXPR))

/* miscellaneous register accessors */
#define FPCR			(regs.regs_C.fpcr)
#define SET_FPCR(EXPR)		(regs.regs_C.fpcr = (EXPR))
#define UNIQ			(regs.regs_C.uniq)
#define SET_UNIQ(EXPR)		(regs.regs_C.uniq = (EXPR))

#else
#error No ISA target defined...
#endif

/* precise architected memory state accessor macros */
#define READ_BYTE(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_BYTE(mem, (SRC)))
#define READ_HALF(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_HALF(mem, (SRC)))
#define READ_WORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_WORD(mem, (SRC)))
#ifdef HOST_HAS_QUAD
#define READ_QUAD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_QUAD(mem, (SRC)))
#endif /* HOST_HAS_QUAD */

#define WRITE_BYTE(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_BYTE(mem, (DST), (SRC)))
#define WRITE_HALF(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_HALF(mem, (DST), (SRC)))
#define WRITE_WORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_WORD(mem, (DST), (SRC)))
#ifdef HOST_HAS_QUAD
#define WRITE_QUAD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_QUAD(mem, (DST), (SRC)))
#endif /* HOST_HAS_QUAD */

/* system call handler macro */
#define SYSCALL(INST)	sys_syscall(&regs, mem_access, mem, INST, TRUE)

#ifndef NO_INSN_COUNT
#define INC_INSN_CTR()	sim_num_insn++
#else /* !NO_INSN_COUNT */
#define INC_INSN_CTR()	/* nada */
#endif /* NO_INSN_COUNT */

#ifdef TARGET_ALPHA
#define ZERO_FP_REG()	regs.regs_F.d[MD_REG_ZERO] = 0.0
#else
#define ZERO_FP_REG()	/* nada... */
#endif

/* start simulation, program loaded, processor precise state initialized */
void
sim_main(void)
{
#ifdef USE_JUMP_TABLE
  /* the jump table employs GNU GCC label extensions to construct an array
     of pointers to instruction implementation code, the simulator then uses
     the table to lookup the location of instruction's implementing code, a
     GNU GCC `goto' extension is then used to jump to the inst's implementing
     code through the op_jump table; as a result, there is no need for
     a main simulator loop, which eliminates one branch from the simulator
     interpreter - crazy, no!?!? */

  /* instruction jump table, this code is GNU GCC specific */
  static void *op_jump[/* max opcodes */] = {
    &&opcode_NA, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
    &&opcode_##OP,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
    &&opcode_##OP,
#define CONNECT(OP)
#include "machine.def"
  };
#endif /* USE_JUMP_TABLE */

  /* register allocate instruction buffer */
  register md_inst_t inst;

  /* decoded opcode */
  register enum md_opcode op;

  fprintf(stderr, "sim: ** starting *fast* functional simulation **\n");

  /* must have natural byte/word ordering */
  if (sim_swap_bytes || sim_swap_words)
    fatal("sim: *fast* functional simulation cannot swap bytes or words");

#ifdef USE_JUMP_TABLE

  regs.regs_NPC = regs.regs_PC;

  /* load instruction */
  inst = __UNCHK_MEM_READ(mem, regs.regs_NPC, md_inst_t);

  /* jump to instruction implementation */
  MD_SET_OPCODE(op, inst);
  goto *op_jump[op];

#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
  opcode_##OP:								\
    /* maintain $r0 semantics */					\
    regs.regs_R[MD_REG_ZERO] = 0;					\
    ZERO_FP_REG();							\
									\
    /* keep an instruction count */					\
    INC_INSN_CTR();							\
									\
    /* locate next instruction */					\
    regs.regs_PC = regs.regs_NPC;					\
									\
    /* set up default next PC */					\
    regs.regs_NPC += sizeof(md_inst_t);					\
									\
    /* execute the instruction */					\
    SYMCAT(OP,_IMPL);							\
									\
    /* get the next instruction */					\
    inst = __UNCHK_MEM_READ(mem, regs.regs_NPC, md_inst_t);		\
									\
    /* jump to instruction implementation */				\
    MD_SET_OPCODE(op, inst);						\
    goto *op_jump[op];

#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
  opcode_##OP:								\
    panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#define DECLARE_FAULT(FAULT)						\
	  { /* uncaught... */ }
#include "machine.def"

  opcode_NA:
    panic("attempted to execute a bogus opcode");

  /* should not get here... */
  panic("exited sim-fast main loop");

#else /* !USE_JUMP_TABLE */

  /* set up initial default next PC */
  regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);

  while (TRUE)
    {
      /* maintain $r0 semantics */
      regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
      regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

      /* keep an instruction count */
#ifndef NO_INSN_COUNT
      sim_num_insn++;
#endif /* !NO_INSN_COUNT */

#ifdef TARGET_ALPHA
      /* load predecoded instruction */
      op = (enum md_opcode)__UNCHK_MEM_READ(dec, regs.regs_PC << 1, word_t);
      inst =
	__UNCHK_MEM_READ(dec, (regs.regs_PC << 1)+sizeof(word_t), md_inst_t);
#else /* !TARGET_ALPHA */
      /* load instruction */
      inst = __UNCHK_MEM_READ(mem, regs.regs_PC, md_inst_t);

      /* decode the instruction */
      MD_SET_OPCODE(op, inst);
#endif

      /* execute the instruction */
      switch (op)
	{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
	case OP:							\
	  SYMCAT(OP,_IMPL);						\
	  break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
	case OP:							\
	  panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#define DECLARE_FAULT(FAULT)						\
	  { /* uncaught... */break; }
#include "machine.def"
	default:
	  panic("attempted to execute a bogus opcode");
	}

      /* execute next instruction */
      regs.regs_PC = regs.regs_NPC;
      regs.regs_NPC += sizeof(md_inst_t);
    }

#endif /* USE_JUMP_TABLE */
}

/*
 * alphadef.h
 *
 *  Created on: 2016年6月11日
 *      Author: root
 */

//ecoff.h
#define ECOFF_EB_MAGIC		0x0160
#define ECOFF_EB_OTHER		0x6001
#define ECOFF_EL_MAGIC		0x0162
#define ECOFF_EL_OTHER		0x6201
#define ECOFF_ALPHAMAGIC	0603
#define ECOFF_R_SN_TEXT		1
#define ECOFF_R_SN_RDATA	2
#define ECOFF_R_SN_DATA		3
#define ECOFF_R_SN_SDATA	4
#define ECOFF_R_SN_SBSS		5
#define ECOFF_R_SN_BSS		6

#define ECOFF_STYP_TEXT		0x0020
#define ECOFF_STYP_RDATA	0x0100
#define ECOFF_STYP_DATA		0x0040
#define ECOFF_STYP_SDATA	0x0200
#define ECOFF_STYP_SBSS		0x0400
#define ECOFF_STYP_BSS		0x0080
#define ECOFF_STYP_INIT		0x80000000	/* sect only contains the text
						   insts for the .init sec. */
#define ECOFF_STYP_FINI		0x01000000	/* insts for .fini */
#define ECOFF_STYP_RCONST	0x02200000	/* overloads with reginfo */
#define ECOFF_STYP_LITA		0x04000000	/* addr literals only        */
#define ECOFF_STYP_LIT8		0x08000000	/* lit pool for 8 byte lits */
#define ECOFF_STYP_LIT4		0x10000000	/* lit pool for 4 byte lits */
#define ECOFF_STYP_XDATA	0x02400000	/* exception sections */
#define ECOFF_STYP_PDATA	0x02800000	/* exception sections */

#define ECOFF_stNil		0
#define ECOFF_stGlobal		1
#define ECOFF_stStatic		2
#define ECOFF_stParam		3
#define ECOFF_stLocal		4
#define ECOFF_stLabel		5
#define ECOFF_stProc		6
#define ECOFF_stBlock		7
#define ECOFF_stEnd		8
#define ECOFF_stMember		9
#define ECOFF_stTypedef		10
#define ECOFF_stFile		11
#define ECOFF_stRegReloc	12
#define ECOFF_stForward		13
#define ECOFF_stStaticProc	14
#define ECOFF_stConstant	15

 
#define TARGET_ALPHA
/* probe cross-endian execution */
#if defined(BYTES_BIG_ENDIAN)
#define MD_CROSS_ENDIAN
#endif
#define MD_QWORD_ADDRS

/* physical memory page size (must be a power-of-two) */
#define MD_PAGE_SIZE		8192
#define MD_LOG_PAGE_SIZE	13
#define MD_NUM_IREGS		32/* number of integer registers */
#define MD_NUM_FREGS		32/* number of floating point registers */
#define MD_NUM_CREGS		2/* number of control registers */
#define MD_TOTAL_REGS							\
  (/*int*/32 + /*fp*/32 + /*misc*/2 + /*tmp*/1 + /*mem*/1 + /*ctrl*/1)
/* target swap support */
#ifdef MD_CROSS_ENDIAN

#define MD_SWAPH(X)		SWAP_HALF(X)
#define MD_SWAPW(X)		SWAP_WORD(X)
#define MD_SWAPQ(X)		SWAP_QWORD(X)
#define MD_SWAPI(X)		SWAP_WORD(X)

#else /* !MD_CROSS_ENDIAN */

#define MD_SWAPH(X)		(X)
#define MD_SWAPW(X)		(X)
#define MD_SWAPQ(X)		(X)
#define MD_SWAPD(X)		(X)
#define MD_SWAPI(X)		(X)

#endif

#define MD_FETCH_INST(INST, MEM, PC)					\
  { (INST) = MEM_READ_WORD((MEM), (PC)); }/* fetch an instruction */
#define MD_MAX_ENVIRON		16384
#define MD_TOP_OP(INST)		(((INST) >> 26) & 0x3f)
#define MD_SET_OPCODE(OP, INST)						\
  { OP = md_mask2op[MD_TOP_OP(INST)];					\
    while (md_opmask[OP])						\
      OP = md_mask2op[((INST >> md_opshift[OP]) & md_opmask[OP])	\
		      + md_opoffset[OP]]; }
#define MD_MAX_MASK		2048
#define MD_OP_NAME(OP)		(md_op2name[OP])
#define MD_OP_FORMAT(OP)	(md_op2format[OP])
#define MD_OP_FUCLASS(OP)	(md_op2fu[OP])
#define MD_FU_NAME(FU)		(md_fu2name[FU])

/* instruction flags */
#define F_ICOMP		0x00000001	/* integer computation */
#define F_FCOMP		0x00000002	/* FP computation */
#define F_CTRL		0x00000004	/* control inst */
#define F_UNCOND	0x00000008	/*   unconditional change */
#define F_COND		0x00000010	/*   conditional change */
#define F_MEM		0x00000020	/* memory access inst */
#define F_LOAD		0x00000040	/*   load inst */
#define F_STORE		0x00000080	/*   store inst */
#define F_DISP		0x00000100	/*   displaced (R+C) addr mode */
#define F_RR		0x00000200	/*   R+R addr mode */
#define F_DIRECT	0x00000400	/*   direct addressing mode */
#define F_TRAP		0x00000800	/* traping inst */
#define F_LONGLAT	0x00001000	/* long latency inst (for sched) */
#define F_DIRJMP	0x00002000	/* direct jump */
#define F_INDIRJMP	0x00004000	/* indirect jump */
#define F_CALL		0x00008000	/* function call */
#define F_FPCOND	0x00010000	/* FP conditional branch */
#define F_IMM		0x00020000	/* instruction has immediate operand */

#define MD_OP_FLAGS(OP)		(md_op2flags[OP])

/* integer register specifiers */
#define RA		((inst >> 21) & 0x1f)		/* reg source #1 */
#define RB		((inst >> 16) & 0x1f)		/* reg source #2 */
#define RC		(inst & 0x1f)			/* reg dest */

/* returns 8-bit unsigned immediate field value */
#define IMM		((qword_t)((inst >> 13) & 0xff))

/* returns 21-bit unsigned absolute jump target field value */
#define TARG		(inst & 0x1fffff)

/* load/store 16-bit unsigned offset field value */
#define OFS		(inst & 0xffff)

/* sign-extend operands */
#define SEXT(X)								\
  (((X) & 0x8000) ? ((sqword_t)(X) | LL(0xffffffffffff0000)) : (sqword_t)(X))

#define SEXT21(X)							\
  (((X) & 0x100000) ? ((sqword_t)(X) | LL(0xffffffffffe00000)) : (sqword_t)(X))

#define SEXT32(X)							\
  (((X) & 0x80000000) ? ((sqword_t)(X)|LL(0xffffffff00000000)) : (sqword_t)(X))

/* test for arithmetic overflow */
#define ARITH_OVFL(RESULT, OP1, OP2) ((RESULT) < (OP1) || (RESULT) < (OP2))

/* test for NaN */
#define IEEEFP_DBL_SIGN(Q)	((Q) >> 63)
#define IEEEFP_DBL_EXPONENT(Q)	(((Q) >> 52) & 0x7ff)
#define IEEEFP_DBL_FRACTION(Q)	((Q) & ULL(0xfffffffffffff))
#define IS_IEEEFP_DBL_NAN(Q)						\
  ((IEEEFP_DBL_EXPONENT(Q) == 0x7ff) && (IEEEFP_DBL_FRACTION(Q)))

/* default target PC handling */
#ifndef SET_TPC
#define SET_TPC(PC)	(void)0
#endif /* SET_TPC */


/*
 * various other helper macros/functions
 */

/* non-zero if system call is an exit() */
#define OSF_SYS_exit			1
#define MD_EXIT_SYSCALL(REGS)						\
  ((REGS)->regs_R[MD_REG_V0] == OSF_SYS_exit)

/* non-zero if system call is a write to stdout/stderr */
#define OSF_SYS_write			4
#define MD_OUTPUT_SYSCALL(REGS)						\
  ((REGS)->regs_R[MD_REG_V0] == OSF_SYS_write				\
   && ((REGS)->regs_R[MD_REG_A0] == /* stdout */1			\
       || (REGS)->regs_R[MD_REG_A0] == /* stderr */2))

/* returns stream of an output system call, translated to host */
#define MD_STREAM_FILENO(REGS)		((REGS)->regs_R[MD_REG_A0])

/* returns non-zero if instruction is a function call */
#define MD_IS_CALL(OP)			((OP) == JSR || (OP) == BSR)

/* returns non-zero if instruction is a function return */
#define MD_IS_RETURN(OP)		((OP) == RETN)

/* returns non-zero if instruction is an indirect jump */
#define MD_IS_INDIR(OP)							\
  ((OP) == JMP || (OP) == JSR || (OP) == RETN || (OP) == JSR_COROUTINE)
#define MD_AMODE_PREPROBE(OP, FSM)		{ (FSM) = 0; }
#define MD_AMODE_PROBE(AM, OP, FSM)					\
  {									\
    if (MD_OP_FLAGS(OP) & F_DISP)					\
      {									\
	if ((RB) == MD_REG_GP)						\
	  (AM) = md_amode_gp;						\
	else if ((RB) == MD_REG_SP)					\
	  (AM) = md_amode_sp;						\
	else if ((RB) == MD_REG_FP) /* && bind_to_seg(addr) == seg_stack */\
	  (AM) = md_amode_fp;						\
	else								\
	  (AM) = md_amode_disp;						\
      }									\
    else if (MD_OP_FLAGS(OP) & F_RR)					\
      (AM) = md_amode_rr;						\
    else								\
      panic("cannot decode addressing mode");				\
  }
#define MD_AMODE_POSTPROBE(FSM)			/* nada... */

/* expected EIO file format */
#define MD_EIO_FILE_FORMAT		EIO_ALPHA_FORMAT

#define MD_MISC_REGS_TO_EXO(REGS)					\
  exo_new(ec_list,							\
	  /*icnt*/exo_new(ec_integer, (exo_integer_t)sim_num_insn),	\
	  /*PC*/exo_new(ec_address, (exo_integer_t)(REGS)->regs_PC),	\
	  /*NPC*/exo_new(ec_address, (exo_integer_t)(REGS)->regs_NPC),	\
	  /*FPCR*/exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.fpcr),\
	  /*UNIQ*/exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.uniq),\
	  NULL)

#define MD_IREG_TO_EXO(REGS, IDX)					\
  exo_new(ec_address, (exo_integer_t)(REGS)->regs_R[IDX])

#define MD_FREG_TO_EXO(REGS, IDX)					\
  exo_new(ec_address, (exo_integer_t)(REGS)->regs_F.q[IDX])

#define MD_EXO_TO_MISC_REGS(EXO, ICNT, REGS)				\
  /* check EXO format for errors... */					\
  if (!exo								\
      || exo->ec != ec_list						\
      || !exo->as_list.head						\
      || exo->as_list.head->ec != ec_integer				\
      || !exo->as_list.head->next					\
      || exo->as_list.head->next->ec != ec_address			\
      || !exo->as_list.head->next->next					\
      || exo->as_list.head->next->next->ec != ec_address		\
      || !exo->as_list.head->next->next->next				\
      || exo->as_list.head->next->next->next->ec != ec_integer		\
      || !exo->as_list.head->next->next->next->next			\
      || exo->as_list.head->next->next->next->next->ec != ec_integer	\
      || exo->as_list.head->next->next->next->next->next != NULL)	\
    fatal("could not read EIO misc regs");				\
  (ICNT) = (counter_t)exo->as_list.head->as_integer.val;		\
  (REGS)->regs_PC = (md_addr_t)exo->as_list.head->next->as_integer.val;	\
  (REGS)->regs_NPC =							\
    (md_addr_t)exo->as_list.head->next->next->as_integer.val;		\
  (REGS)->regs_C.fpcr =							\
    (qword_t)exo->as_list.head->next->next->next->as_integer.val;	\
  (REGS)->regs_C.uniq =							\
    (qword_t)exo->as_list.head->next->next->next->next->as_integer.val;

#define MD_EXO_TO_IREG(EXO, REGS, IDX)					\
  ((REGS)->regs_R[IDX] = (qword_t)(EXO)->as_integer.val)

#define MD_EXO_TO_FREG(EXO, REGS, IDX)					\
  ((REGS)->regs_F.q[IDX] = (qword_t)(EXO)->as_integer.val)

#define MD_EXO_CMP_IREG(EXO, REGS, IDX)					\
  ((REGS)->regs_R[IDX] != (qword_t)(EXO)->as_integer.val)

#define MD_FIRST_IN_REG			0
#define MD_LAST_IN_REG			21

#define MD_FIRST_OUT_REG		0
#define MD_LAST_OUT_REG			21
#define stat_reg_counter		stat_reg_sqword
#define sc_counter			sc_sqword
#define for_counter			for_sqword
/* address stats */
#define stat_reg_addr			stat_reg_qword
/* primitive operation used to compute addresses within pipeline */
#define MD_AGEN_OP		ADDQ
/* NOP operation when injected into the pipeline */
#define MD_NOP_OP		OP_NA
/* non-zero for a valid address, used to determine if speculative accesses
   should access the DL1 data cache */
#define MD_VALID_ADDR(ADDR)						\
  (((ADDR) >= ld_text_base && (ADDR) < (ld_text_base + ld_text_size))	\
   || ((ADDR) >= ld_data_base && (ADDR) < ld_brk_point)			\
   || ((ADDR) >= (ld_stack_base - 16*1024*1024) && (ADDR) < ld_stack_base))
/* shift used to ignore branch address least significant bits, usually
   log2(sizeof(md_inst_t)) */
#define MD_BR_SHIFT		2	/* log2(4) */



/*from sim-outorder.cc,and depend with machine.h or TARGET_PISA*/
//line216
#define IACOMPRESS(A)							\
  (compress_icache_addrs ? ((((A) - ld_text_base) >> 1) + ld_text_base) : (A))
#define ISCOMPRESS(SZ)							\
  (compress_icache_addrs ? ((SZ) >> 1) : (SZ))

//sim-outorder.cc line3515
#define DGPR(N)			(N)
#define DGPR_D(N)		((N) &~1)

/* floating point register dependence decoders */
#define DFPR_L(N)		(((N)+32)&~1)
#define DFPR_F(N)		(((N)+32)&~1)
#define DFPR_D(N)		(((N)+32)&~1)

/* miscellaneous register dependence decoders */
#define DHI			(0+32+32)
#define DLO			(1+32+32)
#define DFCC			(2+32+32)
#define DTMP			(3+32+32)

/* general register dependence decoders, $r31 maps to DNA (0) */
#define DGPR(N)			(31 - (N)) /* was: (((N) == 31) ? DNA : (N)) */

/* floating point register dependence decoders */
#define DFPR(N)			(((N) == 31) ? DNA : ((N)+32))

/* miscellaneous register dependence decoders */
#define DFPCR			(0+32+32)
#define DUNIQ			(1+32+32)
#define DTMP			(2+32+32)

#define FPR_Q(N)		(BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, (N))\
				 ? runcontext->spec_regs_F.q[(N)]                   \
				 : runcontext->regs->regs_F.q[(N)])
#define SET_FPR_Q(N,EXPR)	(spec_mode				\
				 ? ((runcontext->spec_regs_F.q[(N)] = (EXPR)),	\
				    BITMAP_SET(runcontext->use_spec_F,F_BMAP_SZ, (N)),\
				    runcontext->spec_regs_F.q[(N)])			\
				 : (runcontext->regs->regs_F.q[(N)] = (EXPR)))
#define FPR(N)			(BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, (N))\
				 ? runcontext->spec_regs_F.d[(N)]			\
				 : runcontext->regs->regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(spec_mode				\
				 ? ((runcontext->spec_regs_F.d[(N)] = (EXPR)),	\
				    BITMAP_SET(runcontext->use_spec_F,F_BMAP_SZ, (N)),\
				    runcontext->spec_regs_F.d[(N)])			\
				 : (runcontext->regs->regs_F.d[(N)] = (EXPR)))

/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPCR			(BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ,/*fpcr*/0)\
				 ? runcontext->spec_regs_C.fpcr			\
				 : runcontext->regs->regs_C.fpcr)
#define SET_FPCR(EXPR)		(spec_mode				\
				 ? ((runcontext->spec_regs_C.fpcr = (EXPR)),	\
				   BITMAP_SET(runcontext->use_spec_C,C_BMAP_SZ,/*fpcr*/0),\
				    runcontext->spec_regs_C.fpcr)			\
				 : (runcontext->regs->regs_C.fpcr = (EXPR)))
#define UNIQ			(BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ,/*uniq*/1)\
				 ? runcontext->spec_regs_C.uniq			\
				 : runcontext->regs->regs_C.uniq)
#define SET_UNIQ(EXPR)		(spec_mode				\
				 ? ((runcontext->spec_regs_C.uniq = (EXPR)),	\
				   BITMAP_SET(runcontext->use_spec_C,C_BMAP_SZ,/*uniq*/1),\
				    runcontext->spec_regs_C.uniq)			\
				 : (runcontext->regs->regs_C.uniq = (EXPR)))
#define FCC			(BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ,/*fcc*/2)\
				 ? runcontext->spec_regs_C.fcc			\
				 : runcontext->regs->regs_C.fcc)
#define SET_FCC(EXPR)		(spec_mode				\
				 ? ((runcontext->spec_regs_C.fcc = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_C,C_BMAP_SZ,/*fcc*/1),\
				    runcontext->spec_regs_C.fcc)			\
				 : (runcontext->regs->regs_C.fcc = (EXPR)))


/* system call handler macro */
#define SYSCALL(INST)							\
  (/* only execute system calls in non-speculative mode */		\
   (spec_mode ? panic("speculative syscall") : (void) 0),		\
   sys_syscall(runcontext->regs, &TALPHA::mem_access, runcontext->mem, INST, TRUE))



//sim-outorder.cc line3588
#define FPR_L(N)                (thispower->gpp_data->regs_para->total_fregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? runcontext->spec_regs_F.l[(N)]                   \
				 : runcontext->regs->regs_F.l[(N)]									\
		)

#define SET_FPR_L(N,EXPR)       (thispower->gpp_data->regs_para->total_fregs_write_num++,\
		runcontext->spec_mode				\
				 ? ((runcontext->spec_regs_F.l[(N)] = (EXPR)),	\
				    BITMAP_SET(runcontext->use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    runcontext->spec_regs_F.l[(N)])			\
				 : (runcontext->regs->regs_F.l[(N)] = (EXPR))					\
		)

#define FPR_F(N)                (thispower->gpp_data->regs_para->total_fregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? runcontext->spec_regs_F.f[(N)]                   \
				 : runcontext->regs->regs_F.f[(N)]											\
		)

#define SET_FPR_F(N,EXPR)       (thispower->gpp_data->regs_para->total_fregs_write_num++,\
		runcontext->spec_mode				\
				 ? ((runcontext->spec_regs_F.f[(N)] = (EXPR)),	\
				    BITMAP_SET(runcontext->use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    runcontext->spec_regs_F.f[(N)])			\
				 : (runcontext->regs->regs_F.f[(N)] = (EXPR)))


#define FPR_D(N)                (thispower->gpp_data->regs_para->total_fregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? runcontext->spec_regs_F.d[(N) >> 1]              \
				 : runcontext->regs->regs_F.d[(N) >> 1])

#define SET_FPR_D(N,EXPR)       (thispower->gpp_data->regs_para->total_fregs_write_num++,\
		runcontext->spec_mode				\
				 ? ((runcontext->spec_regs_F.d[(N) >> 1] = (EXPR)),	\
				    BITMAP_SET(runcontext->use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    runcontext->spec_regs_F.d[(N) >> 1])		\
				 : (runcontext->regs->regs_F.d[(N) >> 1] = (EXPR)))

/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define HI			(thispower->gpp_data->regs_para->total_cregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ, /*hi*/0)\
				 ? runcontext->spec_regs_C.hi			\
				 : runcontext->regs->regs_C.hi)

#define SET_HI(EXPR)		(thispower->gpp_data->regs_para->total_cregs_write_num++,\
		runcontext->spec_mode				\
				 ? ((runcontext->spec_regs_C.hi = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_C, C_BMAP_SZ,/*hi*/0),\
				    runcontext->spec_regs_C.hi)			\
				 : (runcontext->regs->regs_C.hi = (EXPR)))

#define LO			(thispower->gpp_data->regs_para->total_cregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ, /*lo*/1)\
				 ? runcontext->spec_regs_C.lo			\
				 : runcontext->regs->regs_C.lo)

#define SET_LO(EXPR)		(thispower->gpp_data->regs_para->total_cregs_write_num++,\
		runcontext->spec_mode				\
				 ? ((runcontext->spec_regs_C.lo = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_C, C_BMAP_SZ,/*lo*/1),\
				    runcontext->spec_regs_C.lo)			\
				 : (runcontext->regs->regs_C.lo = (EXPR)))

#define FCC			(thispower->gpp_data->regs_para->total_fregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ,/*fcc*/2)\
				 ? runcontext->spec_regs_C.fcc			\
				 : runcontext->regs->regs_C.fcc)

#define SET_FCC(EXPR)		(thispower->gpp_data->regs_para->total_fregs_write_num++,\
		runcontext->spec_mode				\
				 ? ((runcontext->spec_regs_C.fcc = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_C,C_BMAP_SZ,/*fcc*/2),\
				    runcontext->spec_regs_C.fcc)			\
				 : (runcontext->regs->regs_C.fcc = (EXPR)))

/*syscall.cc*/
#if defined(sparc) && defined(__unix__)
#if defined(__svr4__) || defined(__USLC__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

/* dorks */
#undef NL0
#undef NL1
#undef CR0
#undef CR1
#undef CR2
#undef CR3
#undef TAB0
#undef TAB1
#undef TAB2
#undef XTABS
#undef BS0
#undef BS1
#undef FF0
#undef FF1
#undef ECHO
#undef NOFLSH
#undef TOSTOP
#undef FLUSHO
#undef PENDIN
#endif

#if defined(hpux) || defined(__hpux)
#undef CR0
#endif

#ifdef __FreeBSD__
#include <termios.h>
/*#include <sys/ioctl_compat.h>*/
#else
#ifndef _MSC_VER
#include <termio.h>
#endif
#endif

#if defined(hpux) || defined(__hpux)
/* et tu, dorks! */
#undef HUPCL
#undef ECHO
#undef B50
#undef B75
#undef B110
#undef B134
#undef B150
#undef B200
#undef B300
#undef B600
#undef B1200
#undef B1800
#undef B2400
#undef B4800
#undef B9600
#undef B19200
#undef B38400
#undef NL0
#undef NL1
#undef CR0
#undef CR1
#undef CR2
#undef CR3
#undef TAB0
#undef TAB1
#undef BS0
#undef BS1
#undef FF0
#undef FF1
#undef EXTA
#undef EXTB
#undef B900
#undef B3600
#undef B7200
#undef XTABS
#include <sgtty.h>
#include <utime.h>
#endif



#ifdef _MSC_VER
#define access		_access
#define chmod		_chmod
#define chdir		_chdir
#define unlink		_unlink
#define open		_open
#define creat		_creat
#define pipe		_pipe
#define dup		_dup
#define dup2		_dup2
#define stat		_stat
#define fstat		_fstat
#define lseek		_lseek
#define read		_read
#define write		_write
#define close		_close
#define getpid		_getpid
#define utime		_utime
#include <sys/utime.h>
#endif /* _MSC_VER */




#define OSF_SYS_syscall     0
/* OSF_SYS_exit moved to alpha.h */
#define OSF_SYS_fork        2
#define OSF_SYS_read        3
/* OSF_SYS_write moved to alpha.h */
#define OSF_SYS_old_open    5       /* 5 is old open */
#define OSF_SYS_close       6
#define OSF_SYS_wait4       7
#define OSF_SYS_old_creat   8       /* 8 is old creat */
#define OSF_SYS_link        9
#define OSF_SYS_unlink      10
#define OSF_SYS_execv       11
#define OSF_SYS_chdir       12
#define OSF_SYS_fchdir      13
#define OSF_SYS_mknod       14
#define OSF_SYS_chmod       15
#define OSF_SYS_chown       16
#define OSF_SYS_obreak      17
#define OSF_SYS_getfsstat   18
#define OSF_SYS_lseek       19
#define OSF_SYS_getpid      20
#define OSF_SYS_mount       21
#define OSF_SYS_unmount     22
#define OSF_SYS_setuid      23
#define OSF_SYS_getuid      24
#define OSF_SYS_exec_with_loader    25
#define OSF_SYS_ptrace      26
#ifdef  COMPAT_43
#define OSF_SYS_nrecvmsg    27
#define OSF_SYS_nsendmsg    28
#define OSF_SYS_nrecvfrom   29
#define OSF_SYS_naccept     30
#define OSF_SYS_ngetpeername        31
#define OSF_SYS_ngetsockname        32
#else
#define OSF_SYS_recvmsg     27
#define OSF_SYS_sendmsg     28
#define OSF_SYS_recvfrom    29
#define OSF_SYS_accept      30
#define OSF_SYS_getpeername 31
#define OSF_SYS_getsockname 32
#endif
#define OSF_SYS_access      33
#define OSF_SYS_chflags     34
#define OSF_SYS_fchflags    35
#define OSF_SYS_sync        36
#define OSF_SYS_kill        37
#define OSF_SYS_old_stat    38      /* 38 is old stat */
#define OSF_SYS_setpgid     39
#define OSF_SYS_old_lstat   40      /* 40 is old lstat */
#define OSF_SYS_dup 41
#define OSF_SYS_pipe        42
#define OSF_SYS_set_program_attributes      43
#define OSF_SYS_profil      44
#define OSF_SYS_open        45
                                /* 46 is obsolete osigaction */
#define OSF_SYS_getgid      47
#define OSF_SYS_sigprocmask 48
#define OSF_SYS_getlogin    49
#define OSF_SYS_setlogin    50
#define OSF_SYS_acct        51
#define OSF_SYS_sigpending  52
#define OSF_SYS_ioctl       54
#define OSF_SYS_reboot      55
#define OSF_SYS_revoke      56
#define OSF_SYS_symlink     57
#define OSF_SYS_readlink    58
#define OSF_SYS_execve      59
#define OSF_SYS_umask       60
#define OSF_SYS_chroot      61
#define OSF_SYS_old_fstat   62      /* 62 is old fstat */
#define OSF_SYS_getpgrp     63
#define OSF_SYS_getpagesize 64
#define OSF_SYS_mremap      65
#define OSF_SYS_vfork       66
#define OSF_SYS_stat        67
#define OSF_SYS_lstat       68
#define OSF_SYS_sbrk        69
#define OSF_SYS_sstk        70
#define OSF_SYS_mmap        71
#define OSF_SYS_ovadvise    72
#define OSF_SYS_munmap      73
#define OSF_SYS_mprotect    74
#define OSF_SYS_madvise     75
#define OSF_SYS_old_vhangup 76      /* 76 is old vhangup */
#define OSF_SYS_kmodcall    77
#define OSF_SYS_mincore     78
#define OSF_SYS_getgroups   79
#define OSF_SYS_setgroups   80
#define OSF_SYS_old_getpgrp 81      /* 81 is old getpgrp */
#define OSF_SYS_setpgrp     82
#define OSF_SYS_setitimer   83
#define OSF_SYS_old_wait    84      /* 84 is old wait */
#define OSF_SYS_table       85
#define OSF_SYS_getitimer   86
#define OSF_SYS_gethostname 87
#define OSF_SYS_sethostname 88
#define OSF_SYS_getdtablesize       89
#define OSF_SYS_dup2        90
#define OSF_SYS_fstat       91
#define OSF_SYS_fcntl       92
#define OSF_SYS_select      93
#define OSF_SYS_poll        94
#define OSF_SYS_fsync       95
#define OSF_SYS_setpriority 96
#define OSF_SYS_socket      97
#define OSF_SYS_connect     98
#ifdef  COMPAT_43
#define OSF_SYS_accept      99
#else
#define OSF_SYS_old_accept  99      /* 99 is old accept */
#endif
#define OSF_SYS_getpriority 100
#ifdef  COMPAT_43
#define OSF_SYS_send        101
#define OSF_SYS_recv        102
#else
#define OSF_SYS_old_send    101     /* 101 is old send */
#define OSF_SYS_old_recv    102     /* 102 is old recv */
#endif
#define OSF_SYS_sigreturn   103
#define OSF_SYS_bind        104
#define OSF_SYS_setsockopt  105
#define OSF_SYS_listen      106
#define OSF_SYS_plock       107
#define OSF_SYS_old_sigvec  108     /* 108 is old sigvec */
#define OSF_SYS_old_sigblock        109     /* 109 is old sigblock */
#define OSF_SYS_old_sigsetmask      110     /* 110 is old sigsetmask */
#define OSF_SYS_sigsuspend  111
#define OSF_SYS_sigstack    112
#ifdef  COMPAT_43
#define OSF_SYS_recvmsg     113
#define OSF_SYS_sendmsg     114
#else
#define OSF_SYS_old_recvmsg 113     /* 113 is old recvmsg */
#define OSF_SYS_old_sendmsg 114     /* 114 is old sendmsg */
#endif
                                /* 115 is obsolete vtrace */
#define OSF_SYS_gettimeofday        116
#define OSF_SYS_getrusage   117
#define OSF_SYS_getsockopt  118
#define OSF_SYS_readv       120
#define OSF_SYS_writev      121
#define OSF_SYS_settimeofday        122
#define OSF_SYS_fchown      123
#define OSF_SYS_fchmod      124
#ifdef  COMPAT_43
#define OSF_SYS_recvfrom    125
#else
#define OSF_SYS_old_recvfrom        125     /* 125 is old recvfrom */
#endif
#define OSF_SYS_setreuid    126
#define OSF_SYS_setregid    127
#define OSF_SYS_rename      128
#define OSF_SYS_truncate    129
#define OSF_SYS_ftruncate   130
#define OSF_SYS_flock       131
#define OSF_SYS_setgid      132
#define OSF_SYS_sendto      133
#define OSF_SYS_shutdown    134
#define OSF_SYS_socketpair  135
#define OSF_SYS_mkdir       136
#define OSF_SYS_rmdir       137
#define OSF_SYS_utimes      138
                                /* 139 is obsolete 4.2 sigreturn */
#define OSF_SYS_adjtime     140
#ifdef  COMPAT_43
#define OSF_SYS_getpeername 141
#else
#define OSF_SYS_old_getpeername     141     /* 141 is old getpeername */
#endif
#define OSF_SYS_gethostid   142
#define OSF_SYS_sethostid   143
#define OSF_SYS_getrlimit   144
#define OSF_SYS_setrlimit   145
#define OSF_SYS_old_killpg  146     /* 146 is old killpg */
#define OSF_SYS_setsid      147
#define OSF_SYS_quotactl    148
#define OSF_SYS_oldquota    149
#ifdef  COMPAT_43
#define OSF_SYS_getsockname 150
#else
#define OSF_SYS_old_getsockname     150     /* 150 is old getsockname */
#endif
#define OSF_SYS_pid_block   153
#define OSF_SYS_pid_unblock 154
#define OSF_SYS_sigaction   156
#define OSF_SYS_sigwaitprim 157
#define OSF_SYS_nfssvc      158
#define OSF_SYS_getdirentries       159
#define OSF_SYS_statfs      160
#define OSF_SYS_fstatfs     161
#define OSF_SYS_async_daemon        163
#define OSF_SYS_getfh       164
#define OSF_SYS_getdomainname       165
#define OSF_SYS_setdomainname       166
#define OSF_SYS_exportfs    169
#define OSF_SYS_alt_plock   181     /* 181 is alternate plock */
#define OSF_SYS_getmnt      184
#define OSF_SYS_alt_sigpending      187     /* 187 is alternate sigpending */
#define OSF_SYS_alt_setsid  188     /* 188 is alternate setsid */
#define OSF_SYS_swapon      199
#define OSF_SYS_msgctl      200
#define OSF_SYS_msgget      201
#define OSF_SYS_msgrcv      202
#define OSF_SYS_msgsnd      203
#define OSF_SYS_semctl      204
#define OSF_SYS_semget      205
#define OSF_SYS_semop       206
#define OSF_SYS_uname       207
#define OSF_SYS_lchown      208
#define OSF_SYS_shmat       209
#define OSF_SYS_shmctl      210
#define OSF_SYS_shmdt       211
#define OSF_SYS_shmget      212
#define OSF_SYS_mvalid      213
#define OSF_SYS_getaddressconf      214
#define OSF_SYS_msleep      215
#define OSF_SYS_mwakeup     216
#define OSF_SYS_msync       217
#define OSF_SYS_signal      218
#define OSF_SYS_utc_gettime 219
#define OSF_SYS_utc_adjtime 220
#define OSF_SYS_security    222
#define OSF_SYS_kloadcall   223
#define OSF_SYS_getpgid     233
#define OSF_SYS_getsid      234
#define OSF_SYS_sigaltstack 235
#define OSF_SYS_waitid      236
#define OSF_SYS_priocntlset 237
#define OSF_SYS_sigsendset  238
#define OSF_SYS_set_speculative     239
#define OSF_SYS_msfs_syscall        240
#define OSF_SYS_sysinfo     241
#define OSF_SYS_uadmin      242
#define OSF_SYS_fuser       243
#define OSF_SYS_proplist_syscall    244
#define OSF_SYS_ntp_adjtime 245
#define OSF_SYS_ntp_gettime 246
#define OSF_SYS_pathconf    247
#define OSF_SYS_fpathconf   248
#define OSF_SYS_uswitch     250
#define OSF_SYS_usleep_thread       251
#define OSF_SYS_audcntl     252
#define OSF_SYS_audgen      253
#define OSF_SYS_sysfs       254
#define OSF_SYS_subOSF_SYS_info 255
#define OSF_SYS_getsysinfo  256
#define OSF_SYS_setsysinfo  257
#define OSF_SYS_afs_syscall 258
#define OSF_SYS_swapctl     259
#define OSF_SYS_memcntl     260
#define OSF_SYS_fdatasync   261
//MPI--LCH
#define OSF_SYS_mpisend		 262
#define OSF_SYS_mpirecv		 263


/*syscall.cc*/
#define MAXBUFSIZE 		1024
#define NUM_IOCTL_BYTES		128
#define OSF_TIOCGETP		0x40067408
#define OSF_FIONREAD		0x4004667f
#define OSF_NSIG		32
#define OSF_SIG_BLOCK		1
#define OSF_SIG_UNBLOCK		2
#define OSF_SIG_SETMASK		3
#define OSF_DIRENT_SZ(STR)						\
  (sizeof(word_t) + 2*sizeof(half_t) + (((strlen(STR) + 1) + 3)/4)*4)
  /* was: (sizeof(word_t) + 2*sizeof(half_t) + strlen(STR) + 1) */
#define OSF_O_RDONLY		0x0000
#define OSF_O_WRONLY		0x0001
#define OSF_O_RDWR		0x0002
#define OSF_O_NONBLOCK		0x0004
#define OSF_O_APPEND		0x0008
#define OSF_O_CREAT		0x0200
#define OSF_O_TRUNC		0x0400
#define OSF_O_EXCL		0x0800
#define OSF_O_NOCTTY		0x1000
#define OSF_O_SYNC		0x4000
#define OSF_NFLAGS	(sizeof(osf_flag_table)/sizeof(osf_flag_table[0]))

#define OSF_SO_DEBUG		0x0001
#define OSF_SO_ACCEPTCONN	0x0002
#define OSF_SO_REUSEADDR	0x0004
#define OSF_SO_KEEPALIVE	0x0008
#define OSF_SO_DONTROUTE	0x0010
#define OSF_SO_BROADCAST	0x0020
#define OSF_SO_USELOOPBACK	0x0040
#define OSF_SO_LINGER		0x0080
#define OSF_SO_OOBINLINE	0x0100
#define OSF_SO_REUSEPORT	0x0200

#define OSF_TCP_NODELAY		0x01 /* don't delay send to coalesce packets */
#define OSF_TCP_MAXSEG		0x02 /* maximum segment size */
#define OSF_TCP_RPTR2RXT	0x03 /* set repeat count for R2 RXT timer */
#define OSF_TCP_KEEPIDLE	0x04 /* secs before initial keepalive probe */
#define OSF_TCP_KEEPINTVL	0x05 /* seconds between keepalive probes */
#define OSF_TCP_KEEPCNT		0x06 /* num of keepalive probes before drop */
#define OSF_TCP_KEEPINIT	0x07 /* initial connect timeout (seconds) */
#define OSF_TCP_PUSH		0x08 /* set push bit in outbnd data packets */
#define OSF_TCP_NODELACK	0x09 /* don't delay send to coalesce packets */

/* setsockopt level names */
#define OSF_SOL_SOCKET		0xffff	/* options for socket level */
#define OSF_SOL_IP		0	/* dummy for IP */
#define OSF_SOL_TCP		6	/* tcp */
#define OSF_SOL_UDP		17	/* user datagram protocol */

#define OSF_AF_UNSPEC		0
#define OSF_AF_UNIX		1	/* Unix domain sockets */
#define OSF_AF_INET		2	/* internet IP protocol */
#define OSF_AF_IMPLINK		3	/* arpanet imp addresses */
#define OSF_AF_PUP		4	/* pup protocols: e.g. BSP */
#define OSF_AF_CHAOS		5	/* mit CHAOS protocols */
#define OSF_AF_NS		6	/* XEROX NS protocols */
#define OSF_AF_ISO		7	/* ISO protocols */

#define OSF_SOCK_STREAM		1	/* stream (connection) socket */
#define OSF_SOCK_DGRAM		2	/* datagram (conn.less) socket */
#define OSF_SOCK_RAW		3	/* raw socket */
#define OSF_SOCK_RDM		4	/* reliably-delivered message */
#define OSF_SOCK_SEQPACKET	5	/* sequential packet socket */

#define OSF_TBL_SYSINFO		12

//memory.h
//memory.h
#define MEM_PTAB_SET(ADDR)						\
  (((ADDR) >> MD_LOG_PAGE_SIZE) & (MEM_PTAB_SIZE - 1))

/* compute page table tag */
#define MEM_PTAB_TAG(ADDR)						\
  ((ADDR) >> (MD_LOG_PAGE_SIZE + MEM_LOG_PTAB_SIZE))

/* convert a pte entry at idx to a block address */
#define MEM_PTE_ADDR(PTE, IDX)						\
  (((PTE)->tag << (MD_LOG_PAGE_SIZE + MEM_LOG_PTAB_SIZE))		\
   | ((IDX) << MD_LOG_PAGE_SIZE))

/* locate host page for virtual address ADDR, returns NULL if unallocated */
#define MEM_PAGE(MEM, ADDR)						\
  (/* first attempt to hit in first entry, otherwise call xlation fn */	\
   ((MEM)->ptab[MEM_PTAB_SET(ADDR)]					\
    && (MEM)->ptab[MEM_PTAB_SET(ADDR)]->tag == MEM_PTAB_TAG(ADDR))	\
   ? (/* hit - return the page address on host */			\
      (MEM)->ptab_accesses++,						\
      (MEM)->ptab[MEM_PTAB_SET(ADDR)]->page)				\
   : (/* first level miss - call the translation helper function */	\
      mem_translate((MEM), (ADDR))))

/* compute address of access within a host page */
#define MEM_OFFSET(ADDR)	((ADDR) & (MD_PAGE_SIZE - 1))

/* memory tickle function, allocates pages when they are first written */
#define MEM_TICKLE(MEM, ADDR)						\
  (!MEM_PAGE(MEM, ADDR)							\
   ? (/* allocate page at address ADDR */				\
      mem_newpage(MEM, ADDR))						\
   : (/* nada... */ (void)0))

/* memory page iterator */
#define MEM_FORALL(MEM, ITER, PTE)					\
  for ((ITER)=0; (ITER) < MEM_PTAB_SIZE; (ITER)++)			\
    for ((PTE)=(MEM)->ptab[i]; (PTE) != NULL; (PTE)=(PTE)->next)
/* FIXME: write a more efficient GNU C expression for this... */
#define MEM_READ(MEM, ADDR, TYPE)					\
  (MEM_PAGE(MEM, (qword_t)(ADDR))					\
   ? *((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR)))	\
   : /* page not yet allocated, return zero value */ 0)
#define __UNCHK_MEM_READ(MEM, ADDR, TYPE)				\
  (*((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR))))
/* FIXME: write a more efficient GNU C expression for this... */
#define MEM_WRITE(MEM, ADDR, TYPE, VAL)					\
  (MEM_TICKLE(MEM, (qword_t)(ADDR)),					\
   *((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR))) = (VAL))
#define __UNCHK_MEM_WRITE(MEM, ADDR, TYPE, VAL)				\
  (*((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR))) = (VAL))
#define MEM_READ_BYTE(MEM, ADDR)	MEM_READ(MEM, ADDR, byte_t)
#define MEM_READ_SBYTE(MEM, ADDR)	MEM_READ(MEM, ADDR, sbyte_t)
#define MEM_READ_HALF(MEM, ADDR)	MD_SWAPH(MEM_READ(MEM, ADDR, half_t))
#define MEM_READ_SHALF(MEM, ADDR)	MD_SWAPH(MEM_READ(MEM, ADDR, shalf_t))
#define MEM_READ_WORD(MEM, ADDR)	MD_SWAPW(MEM_READ(MEM, ADDR, word_t))
#define MEM_READ_SWORD(MEM, ADDR)	MD_SWAPW(MEM_READ(MEM, ADDR, sword_t))

#ifdef HOST_HAS_QWORD
#define MEM_READ_QWORD(MEM, ADDR)	MD_SWAPQ(MEM_READ(MEM, ADDR, qword_t))
#define MEM_READ_SQWORD(MEM, ADDR)	MD_SWAPQ(MEM_READ(MEM, ADDR, sqword_t))
#endif /* HOST_HAS_QWORD */

#define MEM_WRITE_BYTE(MEM, ADDR, VAL)	MEM_WRITE(MEM, ADDR, byte_t, VAL)
#define MEM_WRITE_SBYTE(MEM, ADDR, VAL)	MEM_WRITE(MEM, ADDR, sbyte_t, VAL)
#define MEM_WRITE_HALF(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, half_t, MD_SWAPH(VAL))
#define MEM_WRITE_SHALF(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, shalf_t, MD_SWAPH(VAL))
#define MEM_WRITE_WORD(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, word_t, MD_SWAPW(VAL))
#define MEM_WRITE_SWORD(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, sword_t, MD_SWAPW(VAL))
#define MEM_WRITE_SFLOAT(MEM, ADDR, VAL)				\
				MEM_WRITE(MEM, ADDR, sfloat_t, MD_SWAPW(VAL))
#define MEM_WRITE_DFLOAT(MEM, ADDR, VAL)				\
				MEM_WRITE(MEM, ADDR, dfloat_t, MD_SWAPQ(VAL))

#ifdef HOST_HAS_QWORD
#define MEM_WRITE_QWORD(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, qword_t, MD_SWAPQ(VAL))
#define MEM_WRITE_SQWORD(MEM, ADDR, VAL)				\
				MEM_WRITE(MEM, ADDR, sqword_t, MD_SWAPQ(VAL))
#endif /* HOST_HAS_QWORD */



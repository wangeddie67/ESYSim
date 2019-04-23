/*
 * mipsdef.h
 *
 *  Created on: 2016年3月22日
 *      Author: root
 */


//machine.h
#ifndef TARGET_PISA_BIG
#ifndef TARGET_PISA_LITTLE
#ifdef BYTES_BIG_ENDIAN
	#define TARGET_PISA_BIG
#else
	#define TARGET_PISA_LITTLE
#endif
#endif /* TARGET_PISA_LITTLE */
#endif /* TARGET_PISA_BIG */
#if defined(BYTES_BIG_ENDIAN) && defined(TARGET_PISA_LITTLE)
	#define MD_CROSS_ENDIAN
#endif
#if defined(BYTES_LITTLE_ENDIAN) && defined(TARGET_PISA_BIG)
	#define MD_CROSS_ENDIAN
#endif

	#undef MD_QWORD_ADDRS //from ALPHA
	#define MD_PAGE_SIZE		4096
	#define MD_LOG_PAGE_SIZE	12
	#define MD_NUM_IREGS		32
	#define MD_NUM_FREGS		32
	#define MD_NUM_CREGS		3
	#define MD_TOTAL_REGS							\
  (/*int*/32 + /*fp*/32 + /*misc*/3 + /*tmp*/1 + /*mem*/1 + /*ctrl*/1)
#ifdef MD_CROSS_ENDIAN
	#define MD_SWAPH(X)		SWAP_HALF(X)
	#define MD_SWAPW(X)		SWAP_WORD(X)
	#define MD_SWAPQ(X)		SWAP_QWORD(X)
	#define MD_SWAPI(X)		((X).a = SWAP_WORD((X).a),		\
				 (X).b = SWAP_WORD((X).b))
#else /* !MD_CROSS_ENDIAN */
	#define MD_SWAPH(X)		(X)
	#define MD_SWAPW(X)		(X)
	#define MD_SWAPQ(X)		(X)
	#define MD_SWAPD(X)		(X)
	#define MD_SWAPI(X)		(X)
#endif	/* MD_CROSS_ENDIAN */
	#define MD_FETCH_INST(INST, MEM, PC)					\
	  { INST.a = MEM_READ_WORD(MEM, (PC));					\
		INST.b = MEM_READ_WORD(MEM, (PC) + sizeof(word_t)); }
	#define MD_TEXT_BASE		0x00400000
	#define MD_DATA_BASE		0x10000000
	#define MD_STACK_BASE 		0x7fffc000
	#define MD_MAX_ENVIRON		16384
	#define MD_OPFIELD(INST)		(INST.a & 0xff)
	#define MD_SET_OPCODE(OP, INST)	((OP) = (enum md_opcode)((INST).a & 0xff))
	#define MD_MAX_MASK		255
	#define MD_OP_ENUM(MSK)		(md_mask2op[MSK])
	#define MD_OP_NAME(OP)		(md_op2name[OP])
	#define MD_OP_FORMAT(OP)	(md_op2format[OP])
	#define MD_OP_FUCLASS(OP)	(md_op2fu[OP])
	#define MD_FU_NAME(FU)		(md_fu2name[FU])
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
	#undef  RS	/* defined in /usr/include/sys/syscall.h on HPUX boxes */
	#define RS		(inst.b >> 24)			/* reg source #1 */
	#define RT		((inst.b >> 16) & 0xff)		/* reg source #2 */
	#define RD		((inst.b >> 8) & 0xff)		/* reg dest */
	#define SHAMT		(inst.b & 0xff)
	#define FS		RS
	#define FT		RT
	#define FD		RD
	#define IMM		((int)((/* signed */short)(inst.b & 0xffff)))
	#define UIMM		(inst.b & 0xffff)
	#define TARG		(inst.b & 0x3ffffff)
	#define BCODE		(inst.b & 0xfffff)
	#define OFS		IMM		/* alias to IMM */
	#define BS		RS		/* alias to rs */
	#define MAXINT_VAL	0x7fffffff
	#define OVER(X,Y)							\
	  ((((X) > 0) && ((Y) > 0) && (MAXINT_VAL - (X) < (Y)))			\
	   || (((X) < 0) && ((Y) < 0) && (-MAXINT_VAL - (X) > (Y))))
	#define UNDER(X,Y)							\
	  ((((X) > 0) && ((Y) < 0) && (MAXINT_VAL + (Y) < (X)))			\
	   || (((X) < 0) && ((Y) > 0) && (-MAXINT_VAL + (Y) > (X))))
#ifndef SET_TPC
	#define SET_TPC(PC)	(void)0
#endif /* SET_TPC */
#ifdef BYTES_BIG_ENDIAN
	#define WL_SIZE(ADDR)		((ADDR) & 0x03)
	#define WL_BASE(ADDR)		((ADDR) & ~0x03)
	#define WL_PROT_MASK(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])
	#define WL_PROT_MASK1(ADDR)	(md_lr_masks[WL_SIZE(ADDR)])
	#define WL_PROT_MASK2(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])
	#define WR_SIZE(ADDR)		(((ADDR) & 0x03)+1)
	#define WR_BASE(ADDR)		((ADDR) & ~0x03)
	#define WR_PROT_MASK(ADDR)	(~(md_lr_masks[WR_SIZE(ADDR)]))
	#define WR_PROT_MASK1(ADDR)	((md_lr_masks[WR_SIZE(ADDR)]))
	#define WR_PROT_MASK2(ADDR)	(md_lr_masks[4-WR_SIZE(ADDR)])
#else /* BYTES_LITTLE_ENDIAN */
	#define WL_SIZE(ADDR)		(4-((ADDR) & 0x03))
	#define WL_BASE(ADDR)		((ADDR) & ~0x03)
	#define WL_PROT_MASK(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])
	#define WL_PROT_MASK1(ADDR)	(md_lr_masks[WL_SIZE(ADDR)])
	#define WL_PROT_MASK2(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])
	#define WR_SIZE(ADDR)		(((ADDR) & 0x03)+1)
	#define WR_BASE(ADDR)		((ADDR) & ~0x03)
	#define WR_PROT_MASK(ADDR)	(~(md_lr_masks[WR_SIZE(ADDR)]))
	#define WR_PROT_MASK1(ADDR)	((md_lr_masks[WR_SIZE(ADDR)]))
	#define WR_PROT_MASK2(ADDR)	(md_lr_masks[4-WR_SIZE(ADDR)])
#endif
	#define	SS_SYS_exit			1
	#define MD_EXIT_SYSCALL(REGS)		((REGS)->regs_R[2] == SS_SYS_exit)
	#define	SS_SYS_write		4
	#define MD_OUTPUT_SYSCALL(REGS)						\
	  ((REGS)->regs_R[2] == SS_SYS_write					\
	   && ((REGS)->regs_R[4] == /* stdout */1				\
		   || (REGS)->regs_R[4] == /* stderr */2))
	#define MD_STREAM_FILENO(REGS)		((REGS)->regs_R[4])
	#define MD_IS_CALL(OP)							\
	  ((MD_OP_FLAGS(OP) & (F_CTRL|F_CALL)) == (F_CTRL|F_CALL))
	#define MD_IS_RETURN(OP)		((OP) == JR && (RS) == 31)
	#define MD_IS_INDIR(OP)			((OP) == JR)
	#define MD_AMODE_PREPROBE(OP, FSM)					\
	  { if ((OP) == LUI) (FSM) = (RT); }
	#define MD_AMODE_PROBE(AM, OP, FSM)					\
	  {									\
		if (MD_OP_FLAGS(OP) & F_DISP)					\
		  {									\
		if ((BS) == (FSM))						\
		  (AM) = md_amode_imm;						\
		else if ((BS) == MD_REG_GP)					\
		  (AM) = md_amode_gp;						\
		else if ((BS) == MD_REG_SP)					\
		  (AM) = md_amode_sp;						\
		else if ((BS) == MD_REG_FP) /* && bind_to_seg(addr) == seg_stack */\
		  (AM) = md_amode_fp;						\
		else								\
		  (AM) = md_amode_disp;						\
		  }									\
		else if (MD_OP_FLAGS(OP) & F_RR)					\
		  (AM) = md_amode_rr;						\
		else								\
		  panic("cannot decode addressing mode");				\
	  }
	#define MD_AMODE_POSTPROBE(FSM)						\
	  { (FSM) = MD_REG_ZERO; }
	#define MD_EIO_FILE_FORMAT		EIO_PISA_FORMAT
	#define MD_MISC_REGS_TO_EXO(REGS)					\
	  exo_new(ec_list,							\
		  /*icnt*/exo_new(ec_integer, (exo_integer_t)sim_num_insn),	\
		  /*PC*/exo_new(ec_address, (exo_integer_t)(REGS)->regs_PC),	\
		  /*NPC*/exo_new(ec_address, (exo_integer_t)(REGS)->regs_NPC),	\
		  /*HI*/exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.hi),	\
		  /*LO*/exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.lo),	\
		  /*FCC*/exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.fcc),\
		  NULL)
	#define MD_IREG_TO_EXO(REGS, IDX)					\
	  exo_new(ec_address, (exo_integer_t)(REGS)->regs_R[IDX])
	#define MD_FREG_TO_EXO(REGS, IDX)					\
	  exo_new(ec_address, (exo_integer_t)(REGS)->regs_F.l[IDX])
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
		  || !exo->as_list.head->next->next->next->next->next		\
		  || exo->as_list.head->next->next->next->next->next->ec != ec_integer\
		  || exo->as_list.head->next->next->next->next->next->next != NULL)	\
		fatal("could not read EIO misc regs");				\
	  (ICNT) = (counter_t)exo->as_list.head->as_integer.val;		\
	  (REGS)->regs_PC = (md_addr_t)exo->as_list.head->next->as_address.val;	\
	  (REGS)->regs_NPC =							\
		(md_addr_t)exo->as_list.head->next->next->as_address.val;		\
	  (REGS)->regs_C.hi =							\
		(word_t)exo->as_list.head->next->next->next->as_integer.val;	\
	  (REGS)->regs_C.lo =							\
		(word_t)exo->as_list.head->next->next->next->next->as_integer.val;	\
	  (REGS)->regs_C.fcc =							\
		(int)exo->as_list.head->next->next->next->next->next->as_integer.val;
	#define MD_EXO_TO_IREG(EXO, REGS, IDX)					\
	  ((REGS)->regs_R[IDX] = (word_t)(EXO)->as_integer.val)
	#define MD_EXO_TO_FREG(EXO, REGS, IDX)					\
	  ((REGS)->regs_F.l[IDX] = (word_t)(EXO)->as_integer.val)
	#define MD_EXO_CMP_IREG(EXO, REGS, IDX)					\
	  ((REGS)->regs_R[IDX] != (sword_t)(EXO)->as_integer.val)
	#define MD_FIRST_IN_REG			2
	#define MD_LAST_IN_REG			7
	#define MD_FIRST_OUT_REG		2
	#define MD_LAST_OUT_REG			7
#ifdef HOST_HAS_QWORD
	#define stat_reg_counter		stat_reg_sqword
	#define sc_counter			sc_sqword
	#define for_counter			for_sqword
#else /* !HOST_HAS_QWORD */
	#define stat_reg_counter		stat_reg_double
	#define sc_counter			sc_double
	#define for_counter			for_double
#endif /* HOST_HAS_QWORD */
	#define stat_reg_addr			stat_reg_uint
	#define MD_AGEN_OP		ADD
	#define MD_NOP_OP		NOP
	#define MD_VALID_ADDR(ADDR)						\
	  (((ADDR) >= ld_text_base && (ADDR) < (ld_text_base + ld_text_size))	\
	   || ((ADDR) >= ld_data_base && (ADDR) < ld_stack_base))
	#define MD_BR_SHIFT		3	/* log2(8) */
//end of machine.h

//moved form sim-oodef.h
///* free an RS link list */
//#define RSLINK_FREE_LIST(LINK)						\
//  {  struct RS_link *fl_link, *fl_link_next;				\
//     for (fl_link=(LINK); fl_link; fl_link=fl_link_next)		\
//       {								\
//	 fl_link_next = fl_link->next;					\
//	 RSLINK_FREE(fl_link);						\
//       }								\
//  }
/* system call handler macro */
#define SYSCALL(INST)							\
  (/* only execute system calls in non-speculative mode */		\
   (spec_mode ? panic("speculative syscall") : (void) 0),		\
   sys_syscall(runcontext->regs, &TMIPS::mem_access, runcontext->mem, INST, TRUE))
//end of sim-oodef.h


/*from version.h*/
#define VER_TARGET		"PISA"
#define VER_MAJOR		3	/* third release */
#define VER_MINOR		0
#define VER_UPDATE	        "August, 2003"

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





/*loader.cc*/
#define TEXT_TAIL_PADDING 128
/*syscall.cc*/
#ifndef MD_CROSS_ENDIAN

/* SimpleScalar SStrix (a derivative of Ultrix) system call codes, note these
   codes reside in register $r2 at the point a `syscall' inst is executed,
   not all of these codes are implemented, see the main switch statement in
   syscall.c for a list of implemented system calls */

#define SS_SYS_syscall		0
/* SS_SYS_exit was moved to pisa.h */
#define	SS_SYS_fork		2
#define	SS_SYS_read		3
/* SS_SYS_write was moved to pisa.h */
#define	SS_SYS_open		5
#define	SS_SYS_close		6
						/*  7 is old: wait */
#define	SS_SYS_creat		8
#define	SS_SYS_link		9
#define	SS_SYS_unlink		10
#define	SS_SYS_execv		11
#define	SS_SYS_chdir		12
						/* 13 is old: time */
#define	SS_SYS_mknod		14
#define	SS_SYS_chmod		15
#define	SS_SYS_chown		16
#define	SS_SYS_brk		17		/* 17 is old: sbreak */
						/* 18 is old: stat */
#define	SS_SYS_lseek		19
#define	SS_SYS_getpid		20
#define	SS_SYS_mount		21
#define	SS_SYS_umount		22
						/* 23 is old: setuid */
#define	SS_SYS_getuid		24
						/* 25 is old: stime */
#define	SS_SYS_ptrace		26
						/* 27 is old: alarm */
						/* 28 is old: fstat */
						/* 29 is old: pause */
						/* 30 is old: utime */
						/* 31 is old: stty */
						/* 32 is old: gtty */
#define	SS_SYS_access		33
						/* 34 is old: nice */
						/* 35 is old: ftime */
#define	SS_SYS_sync		36
#define	SS_SYS_kill		37
#define	SS_SYS_stat		38
						/* 39 is old: setpgrp */
#define	SS_SYS_lstat		40
#define	SS_SYS_dup		41
#define	SS_SYS_pipe		42
						/* 43 is old: times */
#define	SS_SYS_profil		44
						/* 45 is unused */
						/* 46 is old: setgid */
#define	SS_SYS_getgid		47
						/* 48 is old: sigsys */
						/* 49 is unused */
						/* 50 is unused */
#define	SS_SYS_acct		51
						/* 52 is old: phys */
						/* 53 is old: syslock */
#define	SS_SYS_ioctl		54
#define	SS_SYS_reboot		55
						/* 56 is old: mpxchan */
#define	SS_SYS_symlink		57
#define	SS_SYS_readlink		58
#define	SS_SYS_execve		59
#define	SS_SYS_umask		60
#define	SS_SYS_chroot		61
#define	SS_SYS_fstat		62
						/* 63 is unused */
#define	SS_SYS_getpagesize 	64
#define	SS_SYS_mremap		65
#define SS_SYS_vfork		66		/* 66 is old: vfork */
						/* 67 is old: vread */
						/* 68 is old: vwrite */
#define	SS_SYS_sbrk		69
#define	SS_SYS_sstk		70
#define	SS_SYS_mmap		71
#define SS_SYS_vadvise		72		/* 72 is old: vadvise */
#define	SS_SYS_munmap		73
#define	SS_SYS_mprotect		74
#define	SS_SYS_madvise		75
#define	SS_SYS_vhangup		76
						/* 77 is old: vlimit */
#define	SS_SYS_mincore		78
#define	SS_SYS_getgroups	79
#define	SS_SYS_setgroups	80
#define	SS_SYS_getpgrp		81
#define	SS_SYS_setpgrp		82
#define	SS_SYS_setitimer	83
#define	SS_SYS_wait3		84
#define	SS_SYS_wait		SYS_wait3
#define	SS_SYS_swapon		85
#define	SS_SYS_getitimer	86
#define	SS_SYS_gethostname	87
#define	SS_SYS_sethostname	88
#define	SS_SYS_getdtablesize	89
#define	SS_SYS_dup2		90
#define	SS_SYS_getdopt		91
#define	SS_SYS_fcntl		92
#define	SS_SYS_select		93
#define	SS_SYS_setdopt		94
#define	SS_SYS_fsync		95
#define	SS_SYS_setpriority	96
#define	SS_SYS_socket		97
#define	SS_SYS_connect		98
#define	SS_SYS_accept		99
#define	SS_SYS_getpriority	100
#define	SS_SYS_send		101
#define	SS_SYS_recv		102
#define SS_SYS_sigreturn	103		/* new sigreturn */
						/* 103 was socketaddr */
#define	SS_SYS_bind		104
#define	SS_SYS_setsockopt	105
#define	SS_SYS_listen		106
						/* 107 was vtimes */
#define	SS_SYS_sigvec		108
#define	SS_SYS_sigblock		109
#define	SS_SYS_sigsetmask	110
#define	SS_SYS_sigpause		111
#define	SS_SYS_sigstack		112
#define	SS_SYS_recvmsg		113
#define	SS_SYS_sendmsg		114
						/* 115 is old vtrace */
#define	SS_SYS_gettimeofday	116
#define	SS_SYS_getrusage	117
#define	SS_SYS_getsockopt	118
						/* 119 is old resuba */
#define	SS_SYS_readv		120
#define	SS_SYS_writev		121
#define	SS_SYS_settimeofday	122
#define	SS_SYS_fchown		123
#define	SS_SYS_fchmod		124
#define	SS_SYS_recvfrom		125
#define	SS_SYS_setreuid		126
#define	SS_SYS_setregid		127
#define	SS_SYS_rename		128
#define	SS_SYS_truncate		129
#define	SS_SYS_ftruncate	130
#define	SS_SYS_flock		131
						/* 132 is unused */
#define	SS_SYS_sendto		133
#define	SS_SYS_shutdown		134
#define	SS_SYS_socketpair	135
#define	SS_SYS_mkdir		136
#define	SS_SYS_rmdir		137
#define	SS_SYS_utimes		138
#define SS_SYS_sigcleanup  	139		/* From 4.2 longjmp */
                                                /* same as SYS_sigreturn */
#define	SS_SYS_adjtime		140
#define	SS_SYS_getpeername	141
#define	SS_SYS_gethostid	142
#define	SS_SYS_sethostid	143
#define	SS_SYS_getrlimit	144
#define	SS_SYS_setrlimit	145
#define	SS_SYS_killpg		146
						/* 147 is unused */
#define	SS_SYS_setquota		148
#define	SS_SYS_quota		149
#define	SS_SYS_getsockname	150

#define SS_SYS_sysmips     	151		/* floating point control */

/* formerly mips local system calls */

#define SS_SYS_cacheflush  	152
#define SS_SYS_cachectl    	153
#define SS_SYS_atomic_op   	155

/* nfs releated system calls */
#define SS_SYS_debug       	154

#define SS_SYS_statfs      	160
#define SS_SYS_fstatfs     	161
#define SS_SYS_unmount     	162

#define SS_SYS_quotactl    	168
/* #define SS_SYS_mount       170 */

#define SS_SYS_hdwconf     	171

/* try to keep binary compatibility with mips */

#define SS_SYS_nfs_svc		158
#define SS_SYS_nfssvc		158 /* cruft - delete when kernel fixed */
#define SS_SYS_nfs_biod		163
#define SS_SYS_async_daemon	163 /* cruft - delete when kernel fixed */
#define SS_SYS_nfs_getfh	164
#define SS_SYS_getfh		164 /* cruft - delete when kernel fixed */
#define SS_SYS_getdirentries	159
#define SS_SYS_getdomainname	165
#define SS_SYS_setdomainname	166
#define SS_SYS_exportfs		169

#define SS_SYS_msgctl		172
#define SS_SYS_msgget		173
#define SS_SYS_msgrcv		174
#define SS_SYS_msgsnd		175
#define SS_SYS_semctl		176
#define SS_SYS_semget		177
#define SS_SYS_semop		178
#define SS_SYS_uname		179
#define SS_SYS_shmsys		180
#define SS_SYS_plock		181
#define SS_SYS_lockf		182
#define SS_SYS_ustat		183
#define SS_SYS_getmnt		184
#define	SS_SYS_sigpending	187
#define	SS_SYS_setsid		188
#define	SS_SYS_waitpid		189

#define	SS_SYS_utc_gettime	233	 /* 233 -- same as OSF/1 */
#define SS_SYS_utc_adjtime	234	 /* 234 -- same as OSF/1 */
#define SS_SYS_audcntl		252
#define SS_SYS_audgen		253
#define SS_SYS_startcpu		254	 /* 254 -- Ultrix Private */
#define SS_SYS_stopcpu		255	 /* 255 -- Ultrix Private */
#define SS_SYS_getsysinfo	256	 /* 256 -- Ultrix Private */
#define SS_SYS_setsysinfo	257	 /* 257 -- Ultrix Private */

#define SS_SYS_mpisend		258	 /*258 -- Chenghao*/
#define SS_SYS_mpirecv    	259	 /*259 -- Chenghao*/
#define SS_SYS_rtactid    	260	 /*260 -- Chenghao*/
#define SS_SYS_rtthdid    	261	 /*261 -- Chenghao*/
#define SS_SYS_mpimsgstat    	262	 /*262 -- Chenghao*/
#define SS_SYS_mpisend_f    	263	 /*263 -- Chenghao*/
#define SS_SYS_mpirecv_f    	264	 /*264 -- Chenghao*/
#define SS_SYS_mpimsgstat_f    	265	 /*265 -- Chenghao*/

/* SStrix ioctl values */
#define SS_IOCTL_TIOCGETP	1074164744
#define SS_IOCTL_TIOCSETP	-2147060727
#define SS_IOCTL_TCGETP		1076130901
#define SS_IOCTL_TCGETA		1075082331
#define SS_IOCTL_TIOCGLTC	1074164852
#define SS_IOCTL_TIOCSLTC	-2147060619
#define SS_IOCTL_TIOCGWINSZ	1074295912
#define SS_IOCTL_TCSETAW	-2146143143
#define SS_IOCTL_TIOCGETC	1074164754
#define SS_IOCTL_TIOCSETC	-2147060719
#define SS_IOCTL_TIOCLBIC	-2147191682
#define SS_IOCTL_TIOCLBIS	-2147191681
#define SS_IOCTL_TIOCLGET	0x4004747c
#define SS_IOCTL_TIOCLSET	-2147191683

/* internal system call buffer size, used primarily for file name arguments,
   argument larger than this will be truncated */
#define MAXBUFSIZE 		1024

/* total bytes to copy from a valid pointer argument for ioctl() calls,
   syscall.c does not decode ioctl() calls to determine the size of the
   arguments that reside in memory, instead, the ioctl() proxy simply copies
   NUM_IOCTL_BYTES bytes from the pointer argument to host memory */
#define NUM_IOCTL_BYTES		128

#define SS_O_RDONLY		0
#define SS_O_WRONLY		1
#define SS_O_RDWR		2
#define SS_O_APPEND		0x0008
#define SS_O_CREAT		0x0200
#define SS_O_TRUNC		0x0400
#define SS_O_EXCL		0x0800
#define SS_O_NONBLOCK		0x4000
#define SS_O_NOCTTY		0x8000
#define SS_O_SYNC		0x2000
#define SS_NFLAGS	(sizeof(ss_flag_table)/sizeof(ss_flag_table[0]))

#endif /* !MD_CROSS_ENDIAN */
/*syscall.h/cc end*/
/*symbol.cc*/
#define RELEVANT_SCOPE(SYM)						\
(/* global symbol */							\
 ((SYM)->flags & BSF_GLOBAL)						\
 || (/* local symbol */							\
     (((SYM)->flags & (BSF_LOCAL|BSF_DEBUGGING)) == BSF_LOCAL)		\
     && (SYM)->name[0] != '$')						\
 || (/* compiler local */						\
     load_locals							\
     && ((/* basic block idents */					\
	  ((SYM)->flags&(BSF_LOCAL|BSF_DEBUGGING))==(BSF_LOCAL|BSF_DEBUGGING)\
	  && (SYM)->name[0] == '$')					\
	 || (/* local constant idents */				\
	     ((SYM)->flags & (BSF_LOCAL|BSF_DEBUGGING)) == (BSF_LOCAL)	\
	     && (SYM)->name[0] == '$'))))
/*symbol.cc end*/


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


//sim-outorder.cc line3588
#define FPR_L(N)                (thispower->gpp_data->regs_para->total_fregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? runcontext->spec_regs_F.l[(N)]                   \
				 : runcontext->regs->regs_F.l[(N)]									\
		)

#define SET_FPR_L(N,EXPR)       (thispower->gpp_data->regs_para->total_fregs_write_num++,\
		spec_mode				\
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
		spec_mode				\
				 ? ((runcontext->spec_regs_F.f[(N)] = (EXPR)),	\
				    BITMAP_SET(runcontext->use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    runcontext->spec_regs_F.f[(N)])			\
				 : (runcontext->regs->regs_F.f[(N)] = (EXPR)))


#define FPR_D(N)                (thispower->gpp_data->regs_para->total_fregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? runcontext->spec_regs_F.d[(N) >> 1]              \
				 : runcontext->regs->regs_F.d[(N) >> 1])

#define SET_FPR_D(N,EXPR)       (thispower->gpp_data->regs_para->total_fregs_write_num++,\
		spec_mode				\
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
		spec_mode				\
				 ? ((runcontext->spec_regs_C.hi = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_C, C_BMAP_SZ,/*hi*/0),\
				    runcontext->spec_regs_C.hi)			\
				 : (runcontext->regs->regs_C.hi = (EXPR)))

#define LO			(thispower->gpp_data->regs_para->total_cregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ, /*lo*/1)\
				 ? runcontext->spec_regs_C.lo			\
				 : runcontext->regs->regs_C.lo)

#define SET_LO(EXPR)		(thispower->gpp_data->regs_para->total_cregs_write_num++,\
		spec_mode				\
				 ? ((runcontext->spec_regs_C.lo = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_C, C_BMAP_SZ,/*lo*/1),\
				    runcontext->spec_regs_C.lo)			\
				 : (runcontext->regs->regs_C.lo = (EXPR)))

#define FCC			(thispower->gpp_data->regs_para->total_fregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ,/*fcc*/2)\
				 ? runcontext->spec_regs_C.fcc			\
				 : runcontext->regs->regs_C.fcc)

#define SET_FCC(EXPR)		(thispower->gpp_data->regs_para->total_fregs_write_num++,\
		spec_mode				\
				 ? ((runcontext->spec_regs_C.fcc = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_C,C_BMAP_SZ,/*fcc*/2),\
				    runcontext->spec_regs_C.fcc)			\
				 : (runcontext->regs->regs_C.fcc = (EXPR)))

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

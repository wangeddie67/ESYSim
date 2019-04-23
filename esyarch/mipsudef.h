/*
 * mipsudef.h
 *
 *  Created on: 2016年4月14日
 *      Author: root
 */


#undef TARGET_PISA_BIG
	#undef TARGET_PISA_LITTLE
	#undef MD_CROSS_ENDIAN
	#undef MD_QWORD_ADDRS //from ALPHA
	#undef MD_PAGE_SIZE
	#undef MD_LOG_PAGE_SIZE
	#undef MD_NUM_IREGS
	#undef MD_NUM_FREGS
	#undef MD_NUM_CREGS
	#undef MD_TOTAL_REGS
	#undef MD_SWAPH(X)
	#undef MD_SWAPW(X)
	#undef MD_SWAPQ(X)
	#undef MD_SWAPI(X)
	#undef MD_SWAPH(X)
	#undef MD_SWAPW(X)
	#undef MD_SWAPQ(X)
	#undef MD_SWAPD(X)
	#undef MD_SWAPI(X)
	#undef MD_FETCH_INST(INST, MEM, PC)
	#undef MD_TEXT_BASE
	#undef MD_DATA_BASE
	#undef MD_STACK_BASE
	#undef MD_MAX_ENVIRON
	#undef MD_OPFIELD(INST)
	#undef MD_SET_OPCODE(OP, INST)
	#undef MD_MAX_MASK
	#undef MD_OP_ENUM(MSK)
	#undef MD_OP_NAME(OP)
	#undef MD_OP_FORMAT(OP)
	#undef MD_OP_FUCLASS(OP)
	#undef MD_FU_NAME(FU)
	#undef F_ICOMP
	#undef F_FCOMP
	#undef F_CTRL
	#undef F_UNCOND
	#undef F_COND
	#undef F_MEM
	#undef F_LOAD
	#undef F_STORE
	#undef F_DISP
	#undef F_RR
	#undef F_DIRECT
	#undef F_TRAP
	#undef F_LONGLAT
	#undef F_DIRJMP
	#undef F_INDIRJMP
	#undef F_CALL
	#undef F_FPCOND
	#undef F_IMM
	#undef MD_OP_FLAGS(OP)
	#undef  RS
	#undef RS
	#undef RT
	#undef RD
	#undef SHAMT
	#undef FS
	#undef FT
	#undef FD
	#undef IMM
	#undef UIMM
	#undef TARG
	#undef BCODE
	#undef OFS
	#undef BS
	#undef MAXINT_VAL
	#undef OVER(X,Y)
	#undef UNDER(X,Y)
	#undef SET_TPC(PC)
	#undef WL_SIZE(ADDR)
	#undef WL_BASE(ADDR)
	#undef WL_PROT_MASK(ADDR)
	#undef WL_PROT_MASK1(ADDR)
	#undef WL_PROT_MASK2(ADDR)
	#undef WR_SIZE(ADDR)
	#undef WR_BASE(ADDR)
	#undef WR_PROT_MASK(ADDR)
	#undef WR_PROT_MASK1(ADDR)
	#undef WR_PROT_MASK2(ADDR)
	#undef WL_SIZE(ADDR)
	#undef WL_BASE(ADDR)
	#undef WL_PROT_MASK(ADDR)
	#undef WL_PROT_MASK1(ADDR)
	#undef WL_PROT_MASK2(ADDR)
	#undef WR_SIZE(ADDR)
	#undef WR_BASE(ADDR)
	#undef WR_PROT_MASK(ADDR)
	#undef WR_PROT_MASK1(ADDR)
	#undef WR_PROT_MASK2(ADDR)
	#undef	SS_SYS_exit
	#undef MD_EXIT_SYSCALL(REGS)
	#undef	SS_SYS_write
	#undef MD_OUTPUT_SYSCALL(REGS)
	#undef MD_STREAM_FILENO(REGS)
	#undef MD_IS_CALL(OP)
	#undef MD_IS_RETURN(OP)
	#undef MD_IS_INDIR(OP)
	#undef MD_AMODE_PREPROBE(OP, FSM)
	#undef MD_AMODE_PROBE(AM, OP, FSM)
	#undef MD_AMODE_POSTPROBE(FSM)
	#undef MD_EIO_FILE_FORMAT
	#undef MD_MISC_REGS_TO_EXO(REGS)
	#undef MD_IREG_TO_EXO(REGS, IDX)
	#undef MD_FREG_TO_EXO(REGS, IDX)
	#undef MD_EXO_TO_MISC_REGS(EXO, ICNT, REGS)
	#undef MD_EXO_TO_IREG(EXO, REGS, IDX)
	#undef MD_EXO_TO_FREG(EXO, REGS, IDX)
	#undef MD_EXO_CMP_IREG(EXO, REGS, IDX)
	#undef MD_FIRST_IN_REG
	#undef MD_LAST_IN_REG
	#undef MD_FIRST_OUT_REG
	#undef MD_LAST_OUT_REG
	#undef stat_reg_counter
	#undef sc_counter
	#undef for_counter
	#undef stat_reg_counter
	#undef sc_counter
	#undef for_counter
	#undef stat_reg_addr
	#undef MD_AGEN_OP
	#undef MD_NOP_OP
	#undef MD_VALID_ADDR(ADDR)
	#undef MD_BR_SHIFT
/*-----06.11*/
/* system call handler macro */
#undef SYSCALL(INST)							\
  (/* only execute system calls in non-speculative mode */		\
   (spec_mode ? panic("speculative syscall") : (void) 0),		\
   sys_syscall(runcontext->regs, &TMIPS::mem_access, runcontext->mem, INST, TRUE))
//end of sim-oodef.h


/*from version.h*/
#undef VER_TARGET		"PISA"
#undef VER_MAJOR		3	/* third release */
#undef VER_MINOR		0
#undef VER_UPDATE	        "August, 2003"

//memory.h
#undef MEM_PTAB_SET(ADDR)						\
  (((ADDR) >> MD_LOG_PAGE_SIZE) & (MEM_PTAB_SIZE - 1))

/* compute page table tag */
#undef MEM_PTAB_TAG(ADDR)						\
  ((ADDR) >> (MD_LOG_PAGE_SIZE + MEM_LOG_PTAB_SIZE))

/* convert a pte entry at idx to a block address */
#undef MEM_PTE_ADDR(PTE, IDX)						\
  (((PTE)->tag << (MD_LOG_PAGE_SIZE + MEM_LOG_PTAB_SIZE))		\
   | ((IDX) << MD_LOG_PAGE_SIZE))

/* locate host page for virtual address ADDR, returns NULL if unallocated */
#undef MEM_PAGE(MEM, ADDR)						\
  (/* first attempt to hit in first entry, otherwise call xlation fn */	\
   ((MEM)->ptab[MEM_PTAB_SET(ADDR)]					\
    && (MEM)->ptab[MEM_PTAB_SET(ADDR)]->tag == MEM_PTAB_TAG(ADDR))	\
   ? (/* hit - return the page address on host */			\
      (MEM)->ptab_accesses++,						\
      (MEM)->ptab[MEM_PTAB_SET(ADDR)]->page)				\
   : (/* first level miss - call the translation helper function */	\
      mem_translate((MEM), (ADDR))))

/* compute address of access within a host page */
#undef MEM_OFFSET(ADDR)	((ADDR) & (MD_PAGE_SIZE - 1))

/* memory tickle function, allocates pages when they are first written */
#undef MEM_TICKLE(MEM, ADDR)						\
  (!MEM_PAGE(MEM, ADDR)							\
   ? (/* allocate page at address ADDR */				\
      mem_newpage(MEM, ADDR))						\
   : (/* nada... */ (void)0))

/* memory page iterator */
#undef MEM_FORALL(MEM, ITER, PTE)					\
  for ((ITER)=0; (ITER) < MEM_PTAB_SIZE; (ITER)++)			\
    for ((PTE)=(MEM)->ptab[i]; (PTE) != NULL; (PTE)=(PTE)->next)
/* FIXME: write a more efficient GNU C expression for this... */
#undef MEM_READ(MEM, ADDR, TYPE)					\
  (MEM_PAGE(MEM, (qword_t)(ADDR))					\
   ? *((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR)))	\
   : /* page not yet allocated, return zero value */ 0)
#undef __UNCHK_MEM_READ(MEM, ADDR, TYPE)				\
  (*((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR))))
/* FIXME: write a more efficient GNU C expression for this... */
#undef MEM_WRITE(MEM, ADDR, TYPE, VAL)					\
  (MEM_TICKLE(MEM, (qword_t)(ADDR)),					\
   *((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR))) = (VAL))
#undef __UNCHK_MEM_WRITE(MEM, ADDR, TYPE, VAL)				\
  (*((TYPE *)(MEM_PAGE(MEM, (qword_t)(ADDR)) + MEM_OFFSET(ADDR))) = (VAL))
#undef MEM_READ_BYTE(MEM, ADDR)	MEM_READ(MEM, ADDR, byte_t)
#undef MEM_READ_SBYTE(MEM, ADDR)	MEM_READ(MEM, ADDR, sbyte_t)
#undef MEM_READ_HALF(MEM, ADDR)	MD_SWAPH(MEM_READ(MEM, ADDR, half_t))
#undef MEM_READ_SHALF(MEM, ADDR)	MD_SWAPH(MEM_READ(MEM, ADDR, shalf_t))
#undef MEM_READ_WORD(MEM, ADDR)	MD_SWAPW(MEM_READ(MEM, ADDR, word_t))
#undef MEM_READ_SWORD(MEM, ADDR)	MD_SWAPW(MEM_READ(MEM, ADDR, sword_t))

#ifdef HOST_HAS_QWORD
#undef MEM_READ_QWORD(MEM, ADDR)	MD_SWAPQ(MEM_READ(MEM, ADDR, qword_t))
#undef MEM_READ_SQWORD(MEM, ADDR)	MD_SWAPQ(MEM_READ(MEM, ADDR, sqword_t))
#endif /* HOST_HAS_QWORD */

#undef MEM_WRITE_BYTE(MEM, ADDR, VAL)	MEM_WRITE(MEM, ADDR, byte_t, VAL)
#undef MEM_WRITE_SBYTE(MEM, ADDR, VAL)	MEM_WRITE(MEM, ADDR, sbyte_t, VAL)
#undef MEM_WRITE_HALF(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, half_t, MD_SWAPH(VAL))
#undef MEM_WRITE_SHALF(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, shalf_t, MD_SWAPH(VAL))
#undef MEM_WRITE_WORD(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, word_t, MD_SWAPW(VAL))
#undef MEM_WRITE_SWORD(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, sword_t, MD_SWAPW(VAL))
#undef MEM_WRITE_SFLOAT(MEM, ADDR, VAL)				\
				MEM_WRITE(MEM, ADDR, sfloat_t, MD_SWAPW(VAL))
#undef MEM_WRITE_DFLOAT(MEM, ADDR, VAL)				\
				MEM_WRITE(MEM, ADDR, dfloat_t, MD_SWAPQ(VAL))

#ifdef HOST_HAS_QWORD
#undef MEM_WRITE_QWORD(MEM, ADDR, VAL)					\
				MEM_WRITE(MEM, ADDR, qword_t, MD_SWAPQ(VAL))
#undef MEM_WRITE_SQWORD(MEM, ADDR, VAL)				\
				MEM_WRITE(MEM, ADDR, sqword_t, MD_SWAPQ(VAL))
#endif /* HOST_HAS_QWORD */





/*loader.cc*/
#undef TEXT_TAIL_PADDING 128
#undef SS_SYS_syscall
#undef	SS_SYS_fork
#undef	SS_SYS_read
#undef	SS_SYS_open
#undef	SS_SYS_close
#undef	SS_SYS_creat
#undef	SS_SYS_link
#undef	SS_SYS_unlink
#undef	SS_SYS_execv
#undef	SS_SYS_chdir
#undef	SS_SYS_mknod
#undef	SS_SYS_chmod
#undef	SS_SYS_chown
#undef	SS_SYS_brk
#undef	SS_SYS_lseek
#undef	SS_SYS_getpid
#undef	SS_SYS_mount
#undef	SS_SYS_umount
#undef	SS_SYS_getuid
#undef	SS_SYS_ptrace
#undef	SS_SYS_access
#undef	SS_SYS_sync
#undef	SS_SYS_kill
#undef	SS_SYS_stat
#undef	SS_SYS_lstat
#undef	SS_SYS_dup
#undef	SS_SYS_pipe
#undef	SS_SYS_profil
#undef	SS_SYS_getgid
#undef	SS_SYS_acct
#undef	SS_SYS_ioctl
#undef	SS_SYS_reboot
#undef	SS_SYS_symlink
#undef	SS_SYS_readlink
#undef	SS_SYS_execve
#undef	SS_SYS_umask
#undef	SS_SYS_chroot
#undef	SS_SYS_fstat
#undef	SS_SYS_getpagesize
#undef	SS_SYS_mremap
#undef SS_SYS_vfork
#undef	SS_SYS_sbrk
#undef	SS_SYS_sstk
#undef	SS_SYS_mmap
#undef SS_SYS_vadvise
#undef	SS_SYS_munmap
#undef	SS_SYS_mprotect
#undef	SS_SYS_madvise
#undef	SS_SYS_vhangup
#undef	SS_SYS_mincore
#undef	SS_SYS_getgroups
#undef	SS_SYS_setgroups
#undef	SS_SYS_getpgrp
#undef	SS_SYS_setpgrp
#undef	SS_SYS_setitimer
#undef	SS_SYS_wait3
#undef	SS_SYS_wait
#undef	SS_SYS_swapon
#undef	SS_SYS_getitimer
#undef	SS_SYS_gethostname
#undef	SS_SYS_sethostname
#undef	SS_SYS_getdtablesize
#undef	SS_SYS_dup2
#undef	SS_SYS_getdopt
#undef	SS_SYS_fcntl
#undef	SS_SYS_select
#undef	SS_SYS_setdopt
#undef	SS_SYS_fsync
#undef	SS_SYS_setpriority
#undef	SS_SYS_socket
#undef	SS_SYS_connect
#undef	SS_SYS_accept
#undef	SS_SYS_getpriority
#undef	SS_SYS_send
#undef	SS_SYS_recv
#undef SS_SYS_sigreturn
#undef	SS_SYS_bind
#undef	SS_SYS_setsockopt
#undef	SS_SYS_listen
#undef	SS_SYS_sigvec
#undef	SS_SYS_sigblock
#undef	SS_SYS_sigsetmask
#undef	SS_SYS_sigpause
#undef	SS_SYS_sigstack
#undef	SS_SYS_recvmsg
#undef	SS_SYS_sendmsg
#undef	SS_SYS_gettimeofday
#undef	SS_SYS_getrusage
#undef	SS_SYS_getsockopt
#undef	SS_SYS_readv
#undef	SS_SYS_writev
#undef	SS_SYS_settimeofday
#undef	SS_SYS_fchown
#undef	SS_SYS_fchmod
#undef	SS_SYS_recvfrom
#undef	SS_SYS_setreuid
#undef	SS_SYS_setregid
#undef	SS_SYS_rename
#undef	SS_SYS_truncate
#undef	SS_SYS_ftruncate
#undef	SS_SYS_flock
#undef	SS_SYS_sendto
#undef	SS_SYS_shutdown
#undef	SS_SYS_socketpair
#undef	SS_SYS_mkdir
#undef	SS_SYS_rmdir
#undef	SS_SYS_utimes
#undef SS_SYS_sigcleanup
#undef	SS_SYS_adjtime
#undef	SS_SYS_getpeername
#undef	SS_SYS_gethostid
#undef	SS_SYS_sethostid
#undef	SS_SYS_getrlimit
#undef	SS_SYS_setrlimit
#undef	SS_SYS_killpg
#undef	SS_SYS_setquota
#undef	SS_SYS_quota
#undef	SS_SYS_getsockname
#undef SS_SYS_sysmips
#undef SS_SYS_cacheflush
#undef SS_SYS_cachectl
#undef SS_SYS_atomic_op
#undef SS_SYS_debug
#undef SS_SYS_statfs
#undef SS_SYS_fstatfs
#undef SS_SYS_unmount
#undef SS_SYS_quotactl
#undef SS_SYS_hdwconf
#undef SS_SYS_nfs_svc
#undef SS_SYS_nfssvc
#undef SS_SYS_nfs_biod
#undef SS_SYS_async_daemon
#undef SS_SYS_nfs_getfh
#undef SS_SYS_getfh
#undef SS_SYS_getdirentries
#undef SS_SYS_getdomainname
#undef SS_SYS_setdomainname
#undef SS_SYS_exportfs
#undef SS_SYS_msgctl
#undef SS_SYS_msgget
#undef SS_SYS_msgrcv
#undef SS_SYS_msgsnd
#undef SS_SYS_semctl
#undef SS_SYS_semget
#undef SS_SYS_semop
#undef SS_SYS_uname
#undef SS_SYS_shmsys
#undef SS_SYS_plock
#undef SS_SYS_lockf
#undef SS_SYS_ustat
#undef SS_SYS_getmnt
#undef	SS_SYS_sigpending
#undef	SS_SYS_setsid
#undef	SS_SYS_waitpid
#undef	SS_SYS_utc_gettime
#undef SS_SYS_utc_adjtime
#undef SS_SYS_audcntl
#undef SS_SYS_audgen
#undef SS_SYS_startcpu
#undef SS_SYS_stopcpu
#undef SS_SYS_getsysinfo
#undef SS_SYS_setsysinfo
#undef SS_SYS_mpisend
#undef SS_SYS_mpircv
#undef SS_SYS_rtmpiid
/* SStrix ioctl values */
#undef SS_IOCTL_TIOCGETP
#undef SS_IOCTL_TIOCSETP
#undef SS_IOCTL_TCGETP
#undef SS_IOCTL_TCGETA
#undef SS_IOCTL_TIOCGLTC
#undef SS_IOCTL_TIOCSLTC
#undef SS_IOCTL_TIOCGWINSZ
#undef SS_IOCTL_TCSETAW
#undef SS_IOCTL_TIOCGETC
#undef SS_IOCTL_TIOCSETC
#undef SS_IOCTL_TIOCLBIC
#undef SS_IOCTL_TIOCLBIS
#undef SS_IOCTL_TIOCLGET
#undef SS_IOCTL_TIOCLSET
#undef MAXBUFSIZE
#undef NUM_IOCTL_BYTES
#undef SS_O_RDONLY
#undef SS_O_WRONLY
#undef SS_O_RDWR
#undef SS_O_APPEND
#undef SS_O_CREAT
#undef SS_O_TRUNC
#undef SS_O_EXCL
#undef SS_O_NONBLOCK
#undef SS_O_NOCTTY
#undef SS_O_SYNC
#undef SS_NFLAGS
#undef RELEVANT_SCOPE(SYM)
#undef IACOMPRESS(A)
#undef ISCOMPRESS(SZ)
#undef DGPR(N)
#undef DGPR_D(N)
#undef DFPR_L(N)
#undef DFPR_F(N)
#undef DFPR_D(N)
#undef DHI
#undef DLO
#undef DFCC
#undef DTMP
#undef FPR_L(N)
#undef SET_FPR_L(N,EXPR)
#undef FPR_F(N)
#undef SET_FPR_F(N,EXPR)
#undef FPR_D(N)
#undef SET_FPR_D(N,EXPR)
#undef HI
#undef SET_HI(EXPR)
#undef LO
#undef SET_LO(EXPR)
#undef FCC
#undef SET_FCC(EXPR)
//ecoff.h
#undef ECOFF_EB_MAGIC
#undef ECOFF_EB_OTHER
#undef ECOFF_EL_MAGIC
#undef ECOFF_EL_OTHER
#undef ECOFF_ALPHAMAGIC

#undef ECOFF_R_SN_TEXT
#undef ECOFF_R_SN_RDATA
#undef ECOFF_R_SN_DATA
#undef ECOFF_R_SN_SDATA
#undef ECOFF_R_SN_SBSS
#undef ECOFF_R_SN_BSS

#undef ECOFF_STYP_TEXT
#undef ECOFF_STYP_RDATA
#undef ECOFF_STYP_DATA
#undef ECOFF_STYP_SDATA
#undef ECOFF_STYP_SBSS
#undef ECOFF_STYP_BSS

#undef ECOFF_stNil
#undef ECOFF_stGlobal
#undef ECOFF_stStatic
#undef ECOFF_stParam
#undef ECOFF_stLocal
#undef ECOFF_stLabel
#undef ECOFF_stProc
#undef ECOFF_stBlock
#undef ECOFF_stEnd
#undef ECOFF_stMember
#undef ECOFF_stTypedef
#undef ECOFF_stFile
#undef ECOFF_stRegReloc
#undef ECOFF_stForward
#undef ECOFF_stStaticProc
#undef ECOFF_stConstant


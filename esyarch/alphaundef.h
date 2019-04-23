/*
 * alphaundef.h
 *
 *  Created on: 2016年6月11日
 *      Author: root
 */

/*
 * alphadef.h
 *
 *  Created on: 2016年6月11日
 *      Author: root
 */

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
#undef ECOFF_STYP_INIT
#undef ECOFF_STYP_FINI
#undef ECOFF_STYP_RCONST
#undef ECOFF_STYP_LITA
#undef ECOFF_STYP_LIT8
#undef ECOFF_STYP_LIT4
#undef ECOFF_STYP_XDATA
#undef ECOFF_STYP_PDATA

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


#undef TARGET_ALPHA
#undef MD_CROSS_ENDIAN
#undef MD_QWORD_ADDRS

#undef MD_PAGE_SIZE
#undef MD_LOG_PAGE_SIZE
#undef MD_NUM_IREGS		/* number of integer registers */
#undef MD_NUM_FREGS		/* number of floating point registers */
#undef MD_NUM_CREGS		/* number of control registers */
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
#undef MD_MAX_ENVIRON
#undef MD_TOP_OP(INST)
#undef MD_SET_OPCODE(OP, INST)
#undef MD_MAX_MASK
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
#undef RA
#undef RB
#undef RC
#undef IMM
#undef TARG
#undef OFS
#undef SEXT(X)
#undef SEXT21(X)
#undef SEXT32(X)
#undef ARITH_OVFL(RESULT, OP1, OP2)
#undef IEEEFP_DBL_SIGN(Q)
#undef IEEEFP_DBL_EXPONENT(Q)
#undef IEEEFP_DBL_FRACTION(Q)
#undef IS_IEEEFP_DBL_NAN(Q)
#undef SET_TPC(PC)

#undef OSF_SYS_exit
#undef MD_EXIT_SYSCALL(REGS)
#undef OSF_SYS_write
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
#undef stat_reg_addr
#undef MD_AGEN_OP
#undef MD_NOP_OP
#undef MD_VALID_ADDR(ADDR)

#undef MD_BR_SHIFT

#undef TEXT_TAIL_PADDING

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

#undef DGPR(N)
#undef DFPR(N)
#undef DFPCR
#undef DUNIQ
#undef DTMP

#undef FPR_Q(N)
#undef SET_FPR_Q(N,EXPR)
#undef FPR(N)
#undef SET_FPR(N,EXPR)
#undef FPCR
#undef SET_FPCR(EXPR)
#undef UNIQ
#undef SET_UNIQ(EXPR)
#undef FCC
#undef SET_FCC(EXPR)

#undef SYSCALL(INST)

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

/*syscall.cc*/
#if defined(sparc) && defined(__unix__)
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

#endif

#ifdef _MSC_VER
#undef access
#undef chmod
#undef chdir
#undef unlink
#undef open
#undef creat
#undef pipe
#undef dup
#undef dup2
#undef stat
#undef fstat
#undef lseek
#undef read
#undef write
#undef close
#undef getpid
#undef utime
#endif /* _MSC_VER */

#undef OSF_SYS_syscall
/* OSF_SYS_exit moved to alpha.h */
#undef OSF_SYS_fork
#undef OSF_SYS_read
/* OSF_SYS_write moved to alpha.h */
#undef OSF_SYS_old_open          /* 5 is old open */
#undef OSF_SYS_close
#undef OSF_SYS_wait4
#undef OSF_SYS_old_creat        /* 8 is old creat */
#undef OSF_SYS_link
#undef OSF_SYS_unlink
#undef OSF_SYS_execv
#undef OSF_SYS_chdir
#undef OSF_SYS_fchdir
#undef OSF_SYS_mknod
#undef OSF_SYS_chmod
#undef OSF_SYS_chown
#undef OSF_SYS_obreak
#undef OSF_SYS_getfsstat
#undef OSF_SYS_lseek
#undef OSF_SYS_getpid
#undef OSF_SYS_mount
#undef OSF_SYS_unmount
#undef OSF_SYS_setuid
#undef OSF_SYS_getuid
#undef OSF_SYS_exec_with_loader
#undef OSF_SYS_ptrace

#undef OSF_SYS_nrecvmsg
#undef OSF_SYS_nsendmsg
#undef OSF_SYS_nrecvfrom
#undef OSF_SYS_naccept
#undef OSF_SYS_ngetpeername
#undef OSF_SYS_ngetsockname

#undef OSF_SYS_recvmsg
#undef OSF_SYS_sendmsg
#undef OSF_SYS_recvfrom
#undef OSF_SYS_accept
#undef OSF_SYS_getpeername
#undef OSF_SYS_getsockname

#undef OSF_SYS_access
#undef OSF_SYS_chflags
#undef OSF_SYS_fchflags
#undef OSF_SYS_sync
#undef OSF_SYS_kill
#undef OSF_SYS_old_stat          /* 38 is old stat */
#undef OSF_SYS_setpgid
#undef OSF_SYS_old_lstat         /* 40 is old lstat */
#undef OSF_SYS_dup 41
#undef OSF_SYS_pipe
#undef OSF_SYS_set_program_attributes
#undef OSF_SYS_profil
#undef OSF_SYS_open
#undef OSF_SYS_getgid
#undef OSF_SYS_sigprocmask
#undef OSF_SYS_getlogin
#undef OSF_SYS_setlogin
#undef OSF_SYS_acct
#undef OSF_SYS_sigpending
#undef OSF_SYS_ioctl
#undef OSF_SYS_reboot
#undef OSF_SYS_revoke
#undef OSF_SYS_symlink
#undef OSF_SYS_readlink
#undef OSF_SYS_execve
#undef OSF_SYS_umask
#undef OSF_SYS_chroot
#undef OSF_SYS_old_fstat         /* 62 is old fstat */
#undef OSF_SYS_getpgrp
#undef OSF_SYS_getpagesize
#undef OSF_SYS_mremap
#undef OSF_SYS_vfork
#undef OSF_SYS_stat
#undef OSF_SYS_lstat
#undef OSF_SYS_sbrk
#undef OSF_SYS_sstk
#undef OSF_SYS_mmap
#undef OSF_SYS_ovadvise
#undef OSF_SYS_munmap
#undef OSF_SYS_mprotect
#undef OSF_SYS_madvise
#undef OSF_SYS_old_vhangup    /* 76 is old vhangup */
#undef OSF_SYS_kmodcall
#undef OSF_SYS_mincore
#undef OSF_SYS_getgroups
#undef OSF_SYS_setgroups
#undef OSF_SYS_old_getpgrp    /* 81 is old getpgrp */
#undef OSF_SYS_setpgrp
#undef OSF_SYS_setitimer
#undef OSF_SYS_old_wait    /* 84 is old wait */
#undef OSF_SYS_table
#undef OSF_SYS_getitimer
#undef OSF_SYS_gethostname
#undef OSF_SYS_sethostname
#undef OSF_SYS_getdtablesize
#undef OSF_SYS_dup2
#undef OSF_SYS_fstat
#undef OSF_SYS_fcntl
#undef OSF_SYS_select
#undef OSF_SYS_poll
#undef OSF_SYS_fsync
#undef OSF_SYS_setpriority
#undef OSF_SYS_socket
#undef OSF_SYS_connect

#undef OSF_SYS_accept
#undef OSF_SYS_old_accept
#undef OSF_SYS_getpriority
#undef OSF_SYS_send
#undef OSF_SYS_recv
#undef OSF_SYS_old_send      /* 101 is old send */
#undef OSF_SYS_old_recv         /* 102 is old recv */

#undef OSF_SYS_sigreturn
#undef OSF_SYS_bind
#undef OSF_SYS_setsockopt
#undef OSF_SYS_listen
#undef OSF_SYS_plock
#undef OSF_SYS_old_sigvec     /* 108 is old sigvec */
#undef OSF_SYS_old_sigblock            /* 109 is old sigblock */
#undef OSF_SYS_old_sigsetmask           /* 110 is old sigsetmask */
#undef OSF_SYS_sigsuspend
#undef OSF_SYS_sigstack
#undef OSF_SYS_recvmsg
#undef OSF_SYS_sendmsg
#undef OSF_SYS_old_recvmsg
#undef OSF_SYS_old_sendmsg
#undef OSF_SYS_gettimeofday
#undef OSF_SYS_getrusage
#undef OSF_SYS_getsockopt
#undef OSF_SYS_readv
#undef OSF_SYS_writev
#undef OSF_SYS_settimeofday
#undef OSF_SYS_fchown
#undef OSF_SYS_fchmod

#undef OSF_SYS_recvfrom
#undef OSF_SYS_old_recvfrom
#undef OSF_SYS_setreuid
#undef OSF_SYS_setregid
#undef OSF_SYS_rename
#undef OSF_SYS_truncate
#undef OSF_SYS_ftruncate
#undef OSF_SYS_flock
#undef OSF_SYS_setgid
#undef OSF_SYS_sendto
#undef OSF_SYS_shutdown
#undef OSF_SYS_socketpair
#undef OSF_SYS_mkdir
#undef OSF_SYS_rmdir
#undef OSF_SYS_utimes
#undef OSF_SYS_adjtime

#undef OSF_SYS_getpeername
#undef OSF_SYS_old_getpeername          /* 141 is old getpeername */

#undef OSF_SYS_gethostid
#undef OSF_SYS_sethostid
#undef OSF_SYS_getrlimit
#undef OSF_SYS_setrlimit
#undef OSF_SYS_old_killpg    /* 146 is old killpg */
#undef OSF_SYS_setsid
#undef OSF_SYS_quotactl
#undef OSF_SYS_oldquota

#undef OSF_SYS_getsockname
#undef OSF_SYS_old_getsockname          /* 150 is old getsockname */

#undef OSF_SYS_pid_block
#undef OSF_SYS_pid_unblock
#undef OSF_SYS_sigaction
#undef OSF_SYS_sigwaitprim
#undef OSF_SYS_nfssvc
#undef OSF_SYS_getdirentries
#undef OSF_SYS_statfs
#undef OSF_SYS_fstatfs
#undef OSF_SYS_async_daemon
#undef OSF_SYS_getfh
#undef OSF_SYS_getdomainname
#undef OSF_SYS_setdomainname
#undef OSF_SYS_exportfs
#undef OSF_SYS_alt_plock       /* 181 is alternate plock */
#undef OSF_SYS_getmnt
#undef OSF_SYS_alt_sigpending
#undef OSF_SYS_alt_setsid
#undef OSF_SYS_swapon
#undef OSF_SYS_msgctl
#undef OSF_SYS_msgget
#undef OSF_SYS_msgrcv
#undef OSF_SYS_msgsnd
#undef OSF_SYS_semctl
#undef OSF_SYS_semget
#undef OSF_SYS_semop
#undef OSF_SYS_uname
#undef OSF_SYS_lchown
#undef OSF_SYS_shmat
#undef OSF_SYS_shmctl
#undef OSF_SYS_shmdt
#undef OSF_SYS_shmget
#undef OSF_SYS_mvalid
#undef OSF_SYS_getaddressconf
#undef OSF_SYS_msleep
#undef OSF_SYS_mwakeup
#undef OSF_SYS_msync
#undef OSF_SYS_signal
#undef OSF_SYS_utc_gettime
#undef OSF_SYS_utc_adjtime
#undef OSF_SYS_security
#undef OSF_SYS_kloadcall
#undef OSF_SYS_getpgid
#undef OSF_SYS_getsid
#undef OSF_SYS_sigaltstack
#undef OSF_SYS_waitid
#undef OSF_SYS_priocntlset
#undef OSF_SYS_sigsendset
#undef OSF_SYS_set_speculative
#undef OSF_SYS_msfs_syscall
#undef OSF_SYS_sysinfo
#undef OSF_SYS_uadmin
#undef OSF_SYS_fuser
#undef OSF_SYS_proplist_syscall
#undef OSF_SYS_ntp_adjtime
#undef OSF_SYS_ntp_gettime
#undef OSF_SYS_pathconf
#undef OSF_SYS_fpathconf
#undef OSF_SYS_uswitch
#undef OSF_SYS_usleep_thread
#undef OSF_SYS_audcntl
#undef OSF_SYS_audgen
#undef OSF_SYS_sysfs
#undef OSF_SYS_subOSF_SYS_info
#undef OSF_SYS_getsysinfo
#undef OSF_SYS_setsysinfo
#undef OSF_SYS_afs_syscall
#undef OSF_SYS_swapctl
#undef OSF_SYS_memcntl
#undef OSF_SYS_fdatasync

/*syscall.cc*/
#undef MAXBUFSIZE
#undef NUM_IOCTL_BYTES
#undef OSF_TIOCGETP
#undef OSF_FIONREAD
#undef OSF_NSIG
#undef OSF_SIG_BLOCK
#undef OSF_SIG_UNBLOCK
#undef OSF_SIG_SETMASK
#undef OSF_DIRENT_SZ(STR)
#undef OSF_O_RDONLY
#undef OSF_O_WRONLY
#undef OSF_O_RDWR
#undef OSF_O_NONBLOCK
#undef OSF_O_APPEND
#undef OSF_O_CREAT
#undef OSF_O_TRUNC
#undef OSF_O_EXCL
#undef OSF_O_NOCTTY
#undef OSF_O_SYNC
#undef OSF_NFLAGS

#undef OSF_SO_DEBUG
#undef OSF_SO_ACCEPTCONN
#undef OSF_SO_REUSEADDR
#undef OSF_SO_KEEPALIVE
#undef OSF_SO_DONTROUTE
#undef OSF_SO_BROADCAST
#undef OSF_SO_USELOOPBACK
#undef OSF_SO_LINGER
#undef OSF_SO_OOBINLINE
#undef OSF_SO_REUSEPORT

#undef OSF_TCP_NODELAY		/* don't delay send to coalesce packets */
#undef OSF_TCP_MAXSEG		/* maximum segment size */
#undef OSF_TCP_RPTR2RXT	/* set repeat count for R2 RXT timer */
#undef OSF_TCP_KEEPIDLE	/* secs before initial keepalive probe */
#undef OSF_TCP_KEEPINTVL	/* seconds between keepalive probes */
#undef OSF_TCP_KEEPCNT		 /* num of keepalive probes before drop */
#undef OSF_TCP_KEEPINIT	 /* initial connect timeout (seconds) */
#undef OSF_TCP_PUSH		 /* set push bit in outbnd data packets */
#undef OSF_TCP_NODELACK	 /* don't delay send to coalesce packets */

/* setsockopt level names */
#undef OSF_SOL_SOCKET			/* options for socket level */
#undef OSF_SOL_IP			/* dummy for IP */
#undef OSF_SOL_TCP			/* tcp */
#undef OSF_SOL_UDP			/* user datagram protocol */

#undef OSF_AF_UNSPEC
#undef OSF_AF_UNIX			/* Unix domain sockets */
#undef OSF_AF_INET			/* internet IP protocol */
#undef OSF_AF_IMPLINK			/* arpanet imp addresses */
#undef OSF_AF_PUP			/* pup protocols: e.g. BSP */
#undef OSF_AF_CHAOS			/* mit CHAOS protocols */
#undef OSF_AF_NS			/* XEROX NS protocols */
#undef OSF_AF_ISO			/* ISO protocols */

#undef OSF_SOCK_STREAM			/* stream (connection) socket */
#undef OSF_SOCK_DGRAM			/* datagram (conn.less) socket */
#undef OSF_SOCK_RAW			/* raw socket */
#undef OSF_SOCK_RDM
#undef OSF_SOCK_SEQPACKET

#undef OSF_TBL_SYSINFO

//memory.h
#undef MEM_PTAB_SET(ADDR)
#undef MEM_PTAB_TAG(ADDR)
#undef MEM_PTE_ADDR(PTE, IDX)
#undef MEM_PAGE(MEM, ADDR)
#undef MEM_OFFSET(ADDR)	((ADDR) & (MD_PAGE_SIZE - 1))
#undef MEM_TICKLE(MEM, ADDR)

/* memory page iterator */
#undef MEM_FORALL(MEM, ITER, PTE)
#undef MEM_READ(MEM, ADDR, TYPE)
#undef __UNCHK_MEM_READ(MEM, ADDR, TYPE)
#undef MEM_WRITE(MEM, ADDR, TYPE, VAL)
#undef __UNCHK_MEM_WRITE(MEM, ADDR, TYPE, VAL)
#undef MEM_READ_BYTE(MEM, ADDR)
#undef MEM_READ_SBYTE(MEM, ADDR)
#undef MEM_READ_HALF(MEM, ADDR)
#undef MEM_READ_SHALF(MEM, ADDR)
#undef MEM_READ_WORD(MEM, ADDR)
#undef MEM_READ_SWORD(MEM, ADDR)

#ifdef HOST_HAS_QWORD
#undef MEM_READ_QWORD(MEM, ADDR)
#undef MEM_READ_SQWORD(MEM, ADDR)
#endif /* HOST_HAS_QWORD */

#undef MEM_WRITE_BYTE(MEM, ADDR, VAL)
#undef MEM_WRITE_SBYTE(MEM, ADDR, VAL)
#undef MEM_WRITE_HALF(MEM, ADDR, VAL)
#undef MEM_WRITE_SHALF(MEM, ADDR, VAL)
#undef MEM_WRITE_WORD(MEM, ADDR, VAL)
#undef MEM_WRITE_SWORD(MEM, ADDR, VAL)
#undef MEM_WRITE_SFLOAT(MEM, ADDR, VAL)
#undef MEM_WRITE_DFLOAT(MEM, ADDR, VAL)

#ifdef HOST_HAS_QWORD
#undef MEM_WRITE_QWORD(MEM, ADDR, VAL)
#undef MEM_WRITE_SQWORD(MEM, ADDR, VAL)
#endif /* HOST_HAS_QWORD */



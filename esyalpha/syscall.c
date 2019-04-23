/*
 * syscall.c - proxy system call handler routines
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1994, 1995, 1996, 1997 by Todd M. Austin
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
 * $Id: syscall.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: syscall.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:11  xue
 * CMP_NETWORK
 *
 * Revision 1.7  2009/05/27 19:46:34  garg
 * Post commit write buffer updated.
 *
 * Revision 1.6  2009/05/15 19:14:11  garg
 * Removed branch predictor stats collect bug. Dispatch stall statistics.
 * Additional statistics.
 *
 * Revision 1.5  2009/03/23 18:41:29  garg
 * *** empty log message ***
 *
 * Revision 1.4  2009/03/17 13:19:46  garg
 * Bug fixes: Application stack size and emulation of the parallel code
 *
 * Revision 1.3  2009/03/08 19:45:29  garg
 * Support added to execute parallel applications without timing simulations.
 *
 * Revision 1.2  2009/03/05 16:58:23  garg
 * Updated version of the CMP network simulator.
 *
 * Revision 1.1.1.1  2000/05/26 15:22:27  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.6  1999/12/31 19:00:40  taustin
 * quad_t naming conflicts removed
 * cross-endian execution support added (w/ limited syscall support)
 * extensive extensions to the Alpha OSF system call model
 *
 * Revision 1.5  1999/12/13 18:59:54  taustin
 * cross endian execution support added
 *
 * Revision 1.4  1999/03/08 06:40:09  taustin
 * added SVR4 host support
 *
 * Revision 1.3  1998/09/03 22:20:59  taustin
 * iov_len field in osf_iov fixed (was qword_t changed to word_t)
 * added portable padding to osf_iov type definition
* fixed sigprocmask implementation (from Klauser)
    * usleep_thread() implementation added
    *
    * Revision 1.2  1998/08/31 17:19:44  taustin
    * ported to MS VC++
    * added FIONREAD ioctl() support
* added support for socket(), select(), writev(), readv(), setregid()
    *     setreuid(), connect(), setsockopt(), getsockname(), getpeername(),
    *     setgid(), setuid(), getpriority(), setpriority(), shutdown(), poll()
* change invalid system call from panic() to fatal()
    *
    * Revision 1.1  1998/08/27 16:54:53  taustin
    * Initial revision
    *
    * Revision 1.1  1998/05/06  01:08:39  calder
    * Initial revision
    *
    * Revision 1.5  1997/04/16  22:12:17  taustin
    * added Ultrix host support
    *
    * Revision 1.4  1997/03/11  01:37:37  taustin
    * updated copyright
    * long/int tweaks made for ALPHA target support
    * syscall structures are now more portable across platforms
    * various target supports added
    *
    * Revision 1.3  1996/12/27  15:56:09  taustin
    * updated comments
    * removed system prototypes
    *
    * Revision 1.1  1996/12/05  18:52:32  taustin
    * Initial revision
    *
    *
    */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/uio.h>
    /*  for defining SMT   */
#include "smt.h"

#ifdef SMT_SS
#include "context.h"
#endif

#include "branchCorr.h"

//#define IDEAL_LOCK

    /* only enable a minimal set of systen call proxies if on limited
       hosts or if in cross endian live execution mode */
#ifndef MIN_SYSCALL_MODE
#if defined(_MSC_VER) || defined(__CYGWIN32__) || defined(MD_CROSS_ENDIAN)
#define MIN_SYSCALL_MODE
#endif
#endif /* !MIN_SYSCALL_MODE */

    /* live execution only support on same-endian hosts... */
#ifdef _MSC_VER
#include <io.h>
#else /* !_MSC_VER */
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <sys/param.h>
#endif
#include <errno.h>
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#ifndef _MSC_VER
#include <sys/resource.h>
#endif
#include <signal.h>
#ifndef _MSC_VER
#include <sys/file.h>
#endif
#include <sys/stat.h>
#ifndef _MSC_VER
#include <sys/uio.h>
#endif
#include <setjmp.h>
#ifndef _MSC_VER
#include <sys/times.h>
#endif
#include <limits.h>
#ifndef _MSC_VER
#include <sys/ioctl.h>
#endif
#if defined(linux) || defined(_AIX)
#include <utime.h>
#include <dirent.h>
#include <sys/vfs.h>
#endif
#if defined(_AIX)
#include <sys/statfs.h>
#else /* !_AIX */
#ifndef _MSC_VER
#include <sys/mount.h>
#endif
#endif /* !_AIX */
#if !defined(linux) && !defined(sparc) && !defined(hpux) && !defined(__hpux) && !defined(__CYGWIN32__) && !defined(ultrix)
#ifndef _MSC_VER
#include <sys/select.h>
#endif
#endif
#if defined(linux) || defined(_AIX)
#include <sgtty.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#endif

#if defined(__svr4__)
#include <sys/dirent.h>
#include <sys/filio.h>
#elif defined(__osf__)
#include <dirent.h>
    /* -- For some weird reason, getdirentries() is not declared in any
     * -- header file under /usr/include on the Alpha boxen that I tried
     * -- SS-Alpha on. But the function exists in the libraries.
     */
int getdirentries (int fd, char *buf, int nbytes, long *basep);
#endif

extern int collectStatBarrier, collectStatStop[MAXTHREADS], timeToReturn;
extern long long int freezeCounter;
extern int threadForked[4];

/*RN: 11.08.06*/
extern counter_t sleepCount[];


#if defined(MTA)
extern int RUU_size;
extern struct RS_link *rslink_free_list;

#ifndef RSLINK_NEW
#define RSLINK_NEW(DST, RS)                                             \
{ struct RS_link *n_link;                                             \
    if (!rslink_free_list)                                              \
	panic("out of rs links");                                         \
	    n_link = rslink_free_list;                                          \
	    rslink_free_list = rslink_free_list->next;                          \
	    n_link->next = NULL;                                                \
	    n_link->rs = (RS); n_link->tag = n_link->rs->tag;                   \
	    (DST) = n_link;                                                     \
}
#endif
extern struct CV_link CVLINK_NULL;

#define CV_BMAP_SZ              (BITMAP_SIZE(MD_TOTAL_REGS))
extern int priority[MAXTHREADS];

/*  metric list for measuring the priority, updated with icount_thrd for ICOUNT scheme */
extern int key[MAXTHREADS];
#endif


#if defined(__svr4__) || defined(__osf__)
#include <sys/statvfs.h>
#define statfs statvfs
#include <sys/time.h>
#include <utime.h>
#include <sgtty.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

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
#include <sys/ioctl_compat.h>
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

#ifdef __CYGWIN32__
#include <sys/unistd.h>
#include <sys/vfs.h>
#endif

#include <sys/socket.h>
#include <sys/poll.h>

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

#if defined(_AIX)
#include <strings.h>
#include <netdb.h>
#if !defined(POWER_64)
extern int flock (int, int);
#endif
#endif

#if defined(__osf__)
extern int getdomainname (char *, int);
extern int flock (int, int);
extern int usleep (useconds_t);
#endif

extern void ruu_init (int);
extern void lsq_init (int);
extern void fetch_init (int);
extern void cv_init (int);
extern void reg_init (int);
extern void bpred_init (int);


#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "sim.h"
#include "endian.h"
#include "eio.h"
#include "syscall.h"
#if defined(MTA)
#include "MTA.h"
#endif

#define OSF_SYS_syscall     0
/* OSF_SYS_exit moved to alpha.h */
#define OSF_SYS_fork        2
#define OSF_SYS_read        3
/* OSF_SYS_write moved to alpha.h */
#define OSF_SYS_old_open    5	/* 5 is old open */
#define OSF_SYS_close       6
#define OSF_SYS_wait4       7
#define OSF_SYS_old_creat   8	/* 8 is old creat */
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
#define OSF_SYS_old_stat    38	/* 38 is old stat */
#define OSF_SYS_setpgid     39
#define OSF_SYS_old_lstat   40	/* 40 is old lstat */
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
#define OSF_SYS_old_fstat   62	/* 62 is old fstat */
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
#define OSF_SYS_old_vhangup 76	/* 76 is old vhangup */
#define OSF_SYS_kmodcall    77
#define OSF_SYS_mincore     78
#define OSF_SYS_getgroups   79
#define OSF_SYS_setgroups   80
#define OSF_SYS_old_getpgrp 81	/* 81 is old getpgrp */
#define OSF_SYS_setpgrp     82
#define OSF_SYS_setitimer   83
#define OSF_SYS_old_wait    84	/* 84 is old wait */
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
#define OSF_SYS_old_accept  99	/* 99 is old accept */
#endif
#define OSF_SYS_getpriority 100
#ifdef  COMPAT_43
#define OSF_SYS_send        101
#define OSF_SYS_recv        102
#else
#define OSF_SYS_old_send    101	/* 101 is old send */
#define OSF_SYS_old_recv    102	/* 102 is old recv */
#endif
#define OSF_SYS_sigreturn   103
#define OSF_SYS_bind        104
#define OSF_SYS_setsockopt  105
#define OSF_SYS_listen      106
#define OSF_SYS_plock       107
#define OSF_SYS_old_sigvec  108	/* 108 is old sigvec */
#define OSF_SYS_old_sigblock        109	/* 109 is old sigblock */
#define OSF_SYS_old_sigsetmask      110	/* 110 is old sigsetmask */
#define OSF_SYS_sigsuspend  111
#define OSF_SYS_sigstack    112
#ifdef  COMPAT_43
#define OSF_SYS_recvmsg     113
#define OSF_SYS_sendmsg     114
#else
#define OSF_SYS_old_recvmsg 113	/* 113 is old recvmsg */
#define OSF_SYS_old_sendmsg 114	/* 114 is old sendmsg */
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
#define OSF_SYS_old_recvfrom        125	/* 125 is old recvfrom */
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
#define OSF_SYS_old_getpeername     141	/* 141 is old getpeername */
#endif
#define OSF_SYS_gethostid   142
#define OSF_SYS_sethostid   143
#define OSF_SYS_getrlimit   144
#define OSF_SYS_setrlimit   145
#define OSF_SYS_old_killpg  146	/* 146 is old killpg */
#define OSF_SYS_setsid      147
#define OSF_SYS_quotactl    148
#define OSF_SYS_oldquota    149
#ifdef  COMPAT_43
#define OSF_SYS_getsockname 150
#else
#define OSF_SYS_old_getsockname     150	/* 150 is old getsockname */
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
#define OSF_SYS_alt_plock   181	/* 181 is alternate plock */
#define OSF_SYS_getmnt      184
#define OSF_SYS_alt_sigpending      187	/* 187 is alternate sigpending */
#define OSF_SYS_alt_setsid  188	/* 188 is alternate setsid */
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
#if defined(MTA)
#define OSF_SYS_getthreadid 162
#define OSF_SYS_init_start 501
#define OSF_SYS_init 167
#define OSF_SYS_init_complete 503
#define OSF_SYS_distribute 504
#define OSF_SYS_time 505
//#define OSF_SYS_barrier 506
#define OSF_SYS_lock_acquire 507
#define OSF_SYS_lock_release 508
#define OSF_SYS_flag_acquire 509
#define OSF_SYS_flag_release 510
#define OSF_SYS_inc_flag 511
#define OSF_SYS_poll_flag 512
#define OSF_SYS_wait_gt_flag 513
#define OSF_SYS_wait_lt_flag 514
//#define OSF_SYS_malloc 515
#define OSF_SYS_printf 168
#define OSF_SYS_MTA_halt 170
#define OSF_SYS_barrier 171
#define OSF_SYS_memory_page_round 519
#define OSF_SYS_MTA_start 172
#define OSF_SYS_WAIT 173
#define OSF_SYS_STATS 174
/* Tells the simulator when the program enters and exits the synchronization mode */
#define OSF_SYS_BARRIER_STATS 175
#define OSF_SYS_LOCK_STATS 176	

#define OSF_SYS_BARRIER_INSTR 177
#define OSF_SYS_QUEISCE 178
#define OSF_SYS_FASTFWD 179
#define OSF_SYS_RANDOM 180
#define OSF_SYS_MEM 182
#define OSF_SYS_PRINTGP 183
#define OSF_SYS_SHDADDR 185
#define OSF_SYS_STOPSIM 186
#define OSF_SYS_PRINTINSTR 189
#define OSF_SYS_LOCKADDR	557
#define OSF_SYS_IDEAL_LOCKACQ	190
#define OSF_SYS_IDEAL_LOCKREL	191
#define OSF_SYS_IDEAL_BAR		192

#endif
//#define LOCK_OPTI
#ifdef LOCK_OPTI
md_addr_t lock_addr[MAXTHREADS][MAXLOCK];
int lock_tail[MAXTHREADS];
#endif
extern FILE *fp_trace;

//extern FILE *outFile;

/* ideal lock implementation */
int Lock_Acquire[MAXLOCK] = {0};
int Lock_wait_id[MAXLOCK][MAXLOCK];
int Lock_head[MAXLOCK];
int Lock_tail[MAXLOCK];
int Lock_wait_num[MAXLOCK];
int lock_waiting[MAXTHREADS];

/* ideal barrier implementation */
int Barrier_flag[MAXTHREADS];
int Barrier_num[MAXBARRIER];
int barrier_temp = 0;
int barrier_waiting[MAXTHREADS];

extern counter_t TotalBarriers;
extern counter_t TotalLocks;
void general_stat (int id);
void MTA_printInfo (int id);
int tmpNum;
extern int numThreads[CLUSTERS];
extern int mta_maxthreads;
extern int collect_stats;
extern int flushImpStats;

extern int collect_barrier_stats[CLUSTERS];
extern int collect_barrier_release;
md_addr_t barrier_addr;
int collect_barrier_stats_own[CLUSTERS];
extern int collect_lock_stats[CLUSTERS];
extern md_addr_t LockInitPC;
extern counter_t sim_cycle;
extern int startTakingStatistics;
extern int currentPhase;
extern counter_t parallel_sim_cycle;
extern struct quiesceStruct quiesceAddrStruct[CLUSTERS];
void flushWriteBuffer (int threadid);
extern int fastfwd;
extern int stopsim;
extern int access_mem;
extern int access_mem_id;
extern struct cache_t *cache_dl1[CLUSTERS];

///////////////////////////////////////////////////////////////////////////
// alpha linux syscall specific definitions
///////////////////////////////////////////////////////////////////////////

#define PAGE_SIZE 8192 // 8192 bytes in a virtual memory page

// alpha open flags
static const int ALPHA_O_RDONLY       = 00000000;
static const int ALPHA_O_WRONLY       = 00000001;
static const int ALPHA_O_RDWR         = 00000002;
static const int ALPHA_O_NONBLOCK     = 00000004;
static const int ALPHA_O_APPEND       = 00000010;
static const int ALPHA_O_CREAT        = 00001000;
static const int ALPHA_O_TRUNC        = 00002000;
static const int ALPHA_O_EXCL         = 00004000;
static const int ALPHA_O_NOCTTY       = 00010000;
static const int ALPHA_O_SYNC         = 00040000;
static const int ALPHA_O_DRD          = 00100000;
static const int ALPHA_O_DIRECTIO     = 00200000;
static const int ALPHA_O_CACHE        = 00400000;
static const int ALPHA_O_DSYNC        = 02000000;
static const int ALPHA_O_RSYNC        = 04000000;

// alpha ioctl commands
static const unsigned ALPHA_IOCTL_TIOCGETP   = 0x40067408;
static const unsigned ALPHA_IOCTL_TIOCSETP   = 0x80067409;
static const unsigned ALPHA_IOCTL_TIOCSETN   = 0x8006740a;
static const unsigned ALPHA_IOCTL_TIOCSETC   = 0x80067411;
static const unsigned ALPHA_IOCTL_TIOCGETC   = 0x40067412;
static const unsigned ALPHA_IOCTL_FIONREAD   = 0x4004667f;
static const unsigned ALPHA_IOCTL_TIOCISATTY = 0x2000745e;
static const unsigned ALPHA_IOCTL_TIOCGETS   = 0x402c7413;
static const unsigned ALPHA_IOCTL_TIOCGETA   = 0x40127417;

// rlimit resource ids
static const unsigned ALPHA_RLIMIT_CPU     =  0; 
static const unsigned ALPHA_RLIMIT_FSIZE   =  1;
static const unsigned ALPHA_RLIMIT_DATA    =  2;
static const unsigned ALPHA_RLIMIT_STACK   =  3;
static const unsigned ALPHA_RLIMIT_CORE    =  4;
static const unsigned ALPHA_RLIMIT_RSS     =  5;
static const unsigned ALPHA_RLIMIT_NOFILE  =  6;
static const unsigned ALPHA_RLIMIT_AS      =  7;
static const unsigned ALPHA_RLIMIT_VMEM    =  7;
static const unsigned ALPHA_RLIMIT_NPROC   =  8;
static const unsigned ALPHA_RLIMIT_MEMLOCK =  9;
static const unsigned ALPHA_RLIMIT_LOCKS   = 10;

/////////////////////////////////////////////////////////////////////////////
// system call structures -- pulled from the cross-compiler include dir
/////////////////////////////////////////////////////////////////////////////

typedef struct
{
  // 65 is a hard define in the include tree
  char sysname[65];
  char nodename[65];
  char release[65];
  char version[65];
  char machine[65];
  char domainname[65];
} alpha_utsname;

typedef struct
{
  uint64_t rlim_cur; // soft limit
  uint64_t rlim_max; // hard limit
} alpha_rlimit;

typedef struct
{
  uint32_t  st_dev;
  uint32_t  st_ino;
  uint32_t  st_mode;
  uint32_t  st_nlink;
  uint32_t  st_uid;
  uint32_t  st_gid;
  uint32_t  st_rdev;
  int32_t   __pad0;
  int64_t   st_size;
  uint64_t  st_atimeX; // compiler chokes on st_atime
  uint64_t  st_mtimeX; // compiler chokes on st_mtime
  uint64_t  st_ctimeX; // compiler chokes on st_ctime
  uint32_t  st_blksize;
  int32_t   st_blocks;
  uint32_t  st_flags;
  uint32_t  st_gen;
} alpha_stat;

typedef struct
{
  uint64_t st_dev;
  uint64_t st_ino;
  uint64_t st_rdev;
  uint64_t st_size;
  uint64_t st_blocks;

  uint32_t st_mode;
  uint32_t st_uid;
  uint32_t st_gid;
  uint32_t st_blksize;
  uint32_t st_nlink;
  uint32_t __pad0;

  uint64_t st_atimeX; // compiler chokes on st_atime
  uint64_t st_atime_nsec;
  uint64_t st_mtimeX; // compiler chokes on st_mtime
  uint64_t st_mtime_nsec;
  uint64_t st_ctimeX; // compiler chokes on st_ctime
  uint64_t st_ctime_nsec;
  int64_t  _unused[3];
} alpha_stat64;

typedef struct
{
  int64_t tv_sec;  // seconds
  int64_t tv_usec; // microseconds
} alpha_timeval;

typedef struct
{
  int64_t tz_minuteswest;
  int64_t tz_dsttime;
} alpha_timezone;

typedef struct
{
  uint64_t iov_base; // aka void *iov_base; // Starting address
  uint64_t iov_len;  // aka size_t iov_len; // Number of bytes to transfer
} alpha_iovec;

/* translate system call arguments */
struct xlate_table_t
{
    int target_val;
    int host_val;
};

int
xlate_arg (int target_val, struct xlate_table_t *map, int map_sz, char *name)
{
    int i;

    for (i = 0; i < map_sz; i++)
    {
	if (target_val == map[i].target_val)
	    return map[i].host_val;
    }

    /* not found, issue warning and return target_val */
    warn ("could not translate argument for `%s': %d", name, target_val);
    return target_val;
}

/* internal system call buffer size, used primarily for file name arguments,
   argument larger than this will be truncated */
#define MAXBUFSIZE 		1024

/* total bytes to copy from a valid pointer argument for ioctl() calls,
   syscall.c does not decode ioctl() calls to determine the size of the
   arguments that reside in memory, instead, the ioctl() proxy simply copies
   NUM_IOCTL_BYTES bytes from the pointer argument to host memory */
#define NUM_IOCTL_BYTES		128
#define SYSCALL_BUFFER_SIZE 1024 // for places a byte array is needed

/* OSF ioctl() requests */
#define OSF_TIOCGETP		0x40067408
#define OSF_FIONREAD		0x4004667f

/* target stat() buffer definition, the host stat buffer format is
   automagically mapped to/from this format in syscall.c */
struct osf_statbuf
{
    word_t osf_st_dev;
    word_t osf_st_ino;
    word_t osf_st_mode;
    half_t osf_st_nlink;
    half_t pad0;		/* to match Alpha/AXP padding... */
    word_t osf_st_uid;
    word_t osf_st_gid;
    word_t osf_st_rdev;
    word_t pad1;		/* to match Alpha/AXP padding... */
    qword_t osf_st_size;
    word_t osf_st_atime;
    word_t osf_st_spare1;
    word_t osf_st_mtime;
    word_t osf_st_spare2;
    word_t osf_st_ctime;
    word_t osf_st_spare3;
    word_t osf_st_blksize;
    word_t osf_st_blocks;
    word_t osf_st_gennum;
    word_t osf_st_spare4;
};

struct osf_sgttyb
{
    byte_t sg_ispeed;		/* input speed */
    byte_t sg_ospeed;		/* output speed */
    byte_t sg_erase;		/* erase character */
    byte_t sg_kill;		/* kill character */
    shalf_t sg_flags;		/* mode flags */
};

#define OSF_NSIG		32

#define OSF_SIG_BLOCK		1
#define OSF_SIG_UNBLOCK		2
#define OSF_SIG_SETMASK		3

struct osf_sigcontext
{
    qword_t sc_onstack;		/* sigstack state to restore */
    qword_t sc_mask;		/* signal mask to restore */
    qword_t sc_pc;		/* pc at time of signal */
    qword_t sc_ps;		/* psl to retore */
    qword_t sc_regs[32];	/* processor regs 0 to 31 */
    qword_t sc_ownedfp;		/* fp has been used */
    qword_t sc_fpregs[32];	/* fp regs 0 to 31 */
    qword_t sc_fpcr;		/* floating point control register */
    qword_t sc_fp_control;	/* software fpcr */
};

struct osf_statfs
{
    shalf_t f_type;		/* type of filesystem (see below) */
    shalf_t f_flags;		/* copy of mount flags */
    word_t f_fsize;		/* fundamental filesystem block size */
    word_t f_bsize;		/* optimal transfer block size */
    word_t f_blocks;		/* total data blocks in file system, */
    /* note: may not represent fs size. */
    word_t f_bfree;		/* free blocks in fs */
    word_t f_bavail;		/* free blocks avail to non-su */
    word_t f_files;		/* total file nodes in file system */
    word_t f_ffree;		/* free file nodes in fs */
    qword_t f_fsid;		/* file system id */
    word_t f_spare[9];		/* spare for later */
};

struct osf_timeval
{
    sword_t osf_tv_sec;		/* seconds */
    sword_t osf_tv_usec;	/* microseconds */
};

struct osf_timezone
{
    sword_t osf_tz_minuteswest;	/* minutes west of Greenwich */
    sword_t osf_tz_dsttime;	/* type of dst correction */
};

/* target getrusage() buffer definition, the host stat buffer format is
   automagically mapped to/from this format in syscall.c */
struct osf_rusage
{
    struct osf_timeval osf_ru_utime;
    struct osf_timeval osf_ru_stime;
    sword_t osf_ru_maxrss;
    sword_t osf_ru_ixrss;
    sword_t osf_ru_idrss;
    sword_t osf_ru_isrss;
    sword_t osf_ru_minflt;
    sword_t osf_ru_majflt;
    sword_t osf_ru_nswap;
    sword_t osf_ru_inblock;
    sword_t osf_ru_oublock;
    sword_t osf_ru_msgsnd;
    sword_t osf_ru_msgrcv;
    sword_t osf_ru_nsignals;
    sword_t osf_ru_nvcsw;
    sword_t osf_ru_nivcsw;
};

struct osf_rlimit
{
    qword_t osf_rlim_cur;	/* current (soft) limit */
    qword_t osf_rlim_max;	/* maximum value for rlim_cur */
};

struct osf_sockaddr
{
    half_t sa_family;		/* address family, AF_xxx */
    byte_t sa_data[24];		/* 14 bytes of protocol address */
};

struct osf_iovec
{
    md_addr_t iov_base;		/* starting address */
    word_t iov_len;		/* length in bytes */
    word_t pad;
};

/* returns size of DIRENT structure */
#define OSF_DIRENT_SZ(STR)						\
(sizeof(word_t) + 2*sizeof(half_t) + (((strlen(STR) + 1) + 3)/4)*4)
    /* was: (sizeof(word_t) + 2*sizeof(half_t) + strlen(STR) + 1) */

struct osf_dirent
{
    word_t d_ino;		/* file number of entry */
    half_t d_reclen;		/* length of this record */
    half_t d_namlen;		/* length of string in d_name */
    char d_name[256];		/* DUMMY NAME LENGTH */
    /* the real maximum length is */
    /* returned by pathconf() */
    /* At this time, this MUST */
    /* be 256 -- the kernel */
    /* requires it */
};

/* open(2) flags for Alpha/AXP OSF target, syscall.c automagically maps
   between these codes to/from host open(2) flags */
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

/* open(2) flags translation table for SimpleScalar target */
struct
{
    int osf_flag;
    int local_flag;
} osf_flag_table[] =
{
    /* target flag *//* host flag */
#ifdef _MSC_VER
    {
    OSF_O_RDONLY, _O_RDONLY},
    {
    OSF_O_WRONLY, _O_WRONLY},
    {
    OSF_O_RDWR, _O_RDWR},
    {
    OSF_O_APPEND, _O_APPEND},
    {
    OSF_O_CREAT, _O_CREAT},
    {
    OSF_O_TRUNC, _O_TRUNC},
    {
    OSF_O_EXCL, _O_EXCL},
#ifdef _O_NONBLOCK
    {
    OSF_O_NONBLOCK, _O_NONBLOCK},
#endif
#ifdef _O_NOCTTY
    {
    OSF_O_NOCTTY, _O_NOCTTY},
#endif
#ifdef _O_SYNC
    {
    OSF_O_SYNC, _O_SYNC},
#endif
#else /* !_MSC_VER */
    {
    OSF_O_RDONLY, O_RDONLY},
    {
    OSF_O_WRONLY, O_WRONLY},
    {
    OSF_O_RDWR, O_RDWR},
    {
    OSF_O_APPEND, O_APPEND},
    {
    OSF_O_CREAT, O_CREAT},
    {
    OSF_O_TRUNC, O_TRUNC},
    {
    OSF_O_EXCL, O_EXCL},
    {
    OSF_O_NONBLOCK, O_NONBLOCK},
    {
    OSF_O_NOCTTY, O_NOCTTY},
#ifdef O_SYNC
    {
    OSF_O_SYNC, O_SYNC},
#endif
#endif /* _MSC_VER */
};

#define OSF_NFLAGS	(sizeof(osf_flag_table)/sizeof(osf_flag_table[0]))

/* gjmfix: capitalized to avoid compiler error with xlc */
qword_t SigMask = 0;

qword_t sigaction_array[OSF_NSIG] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* setsockopt option names */
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

struct xlate_table_t sockopt_map[] = {
    {OSF_SO_DEBUG, SO_DEBUG},
#ifdef SO_ACCEPTCONN
    {OSF_SO_ACCEPTCONN, SO_ACCEPTCONN},
#endif
    {OSF_SO_REUSEADDR, SO_REUSEADDR},
    {OSF_SO_KEEPALIVE, SO_KEEPALIVE},
    {OSF_SO_DONTROUTE, SO_DONTROUTE},
    {OSF_SO_BROADCAST, SO_BROADCAST},
#ifdef SO_USELOOPBACK
    {OSF_SO_USELOOPBACK, SO_USELOOPBACK},
#endif
    {OSF_SO_LINGER, SO_LINGER},
    {OSF_SO_OOBINLINE, SO_OOBINLINE},
#ifdef SO_REUSEPORT
    {OSF_SO_REUSEPORT, SO_REUSEPORT}
#endif
};

/* setsockopt TCP options */
#define OSF_TCP_NODELAY		0x01	/* don't delay send to coalesce packets */
#define OSF_TCP_MAXSEG		0x02	/* maximum segment size */
#define OSF_TCP_RPTR2RXT	0x03	/* set repeat count for R2 RXT timer */
#define OSF_TCP_KEEPIDLE	0x04	/* secs before initial keepalive probe */
#define OSF_TCP_KEEPINTVL	0x05	/* seconds between keepalive probes */
#define OSF_TCP_KEEPCNT		0x06	/* num of keepalive probes before drop */
#define OSF_TCP_KEEPINIT	0x07	/* initial connect timeout (seconds) */
#define OSF_TCP_PUSH		0x08	/* set push bit in outbnd data packets */
#define OSF_TCP_NODELACK	0x09	/* don't delay send to coalesce packets */

struct xlate_table_t tcpopt_map[] = {
    {OSF_TCP_NODELAY, TCP_NODELAY},
    {OSF_TCP_MAXSEG, TCP_MAXSEG},
#ifdef TCP_RPTR2RXT
    {OSF_TCP_RPTR2RXT, TCP_RPTR2RXT},
#endif
#ifdef TCP_KEEPIDLE
    {OSF_TCP_KEEPIDLE, TCP_KEEPIDLE},
#endif
#ifdef TCP_KEEPINTVL
    {OSF_TCP_KEEPINTVL, TCP_KEEPINTVL},
#endif
#ifdef TCP_KEEPCNT
    {OSF_TCP_KEEPCNT, TCP_KEEPCNT},
#endif
#ifdef TCP_KEEPINIT
    {OSF_TCP_KEEPINIT, TCP_KEEPINIT},
#endif
#ifdef TCP_PUSH
    {OSF_TCP_PUSH, TCP_PUSH},
#endif
#ifdef TCP_NODELACK
    {OSF_TCP_NODELACK, TCP_NODELACK}
#endif
};

/* setsockopt level names */
#define OSF_SOL_SOCKET		0xffff	/* options for socket level */
#define OSF_SOL_IP		0	/* dummy for IP */
#define OSF_SOL_TCP		6	/* tcp */
#define OSF_SOL_UDP		17	/* user datagram protocol */

struct xlate_table_t socklevel_map[] = {
#if defined(__svr4__) || defined(__osf__) || defined(_AIX)
    {OSF_SOL_SOCKET, SOL_SOCKET},
    {OSF_SOL_IP, IPPROTO_IP},
    {OSF_SOL_TCP, IPPROTO_TCP},
    {OSF_SOL_UDP, IPPROTO_UDP}
#else
    {OSF_SOL_SOCKET, SOL_SOCKET},
    {OSF_SOL_IP, SOL_IP},
    {OSF_SOL_TCP, SOL_TCP},
    {OSF_SOL_UDP, SOL_UDP}
#endif
};

/* socket() address families */
#define OSF_AF_UNSPEC		0
#define OSF_AF_UNIX		1	/* Unix domain sockets */
#define OSF_AF_INET		2	/* internet IP protocol */
#define OSF_AF_IMPLINK		3	/* arpanet imp addresses */
#define OSF_AF_PUP		4	/* pup protocols: e.g. BSP */
#define OSF_AF_CHAOS		5	/* mit CHAOS protocols */
#define OSF_AF_NS		6	/* XEROX NS protocols */
#define OSF_AF_ISO		7	/* ISO protocols */

struct xlate_table_t family_map[] = {
    {OSF_AF_UNSPEC, AF_UNSPEC},
    {OSF_AF_UNIX, AF_UNIX},
    {OSF_AF_INET, AF_INET},
#ifdef AF_IMPLINK
    {OSF_AF_IMPLINK, AF_IMPLINK},
#endif
#ifdef AF_PUP
    {OSF_AF_PUP, AF_PUP},
#endif
#ifdef AF_CHAOS
    {OSF_AF_CHAOS, AF_CHAOS},
#endif
#ifdef AF_NS
    {OSF_AF_NS, AF_NS},
#endif
#ifdef AF_ISO
    {OSF_AF_ISO, AF_ISO}
#endif
};

/* socket() socket types */
#define OSF_SOCK_STREAM		1	/* stream (connection) socket */
#define OSF_SOCK_DGRAM		2	/* datagram (conn.less) socket */
#define OSF_SOCK_RAW		3	/* raw socket */
#define OSF_SOCK_RDM		4	/* reliably-delivered message */
#define OSF_SOCK_SEQPACKET	5	/* sequential packet socket */

struct xlate_table_t socktype_map[] = {
    {OSF_SOCK_STREAM, SOCK_STREAM},
    {OSF_SOCK_DGRAM, SOCK_DGRAM},
    {OSF_SOCK_RAW, SOCK_RAW},
    {OSF_SOCK_RDM, SOCK_RDM},
    {OSF_SOCK_SEQPACKET, SOCK_SEQPACKET}
};

/* OSF table() call. Right now, we only support TBL_SYSINFO queries */
#define OSF_TBL_SYSINFO		12
struct osf_tbl_sysinfo
{
    long si_user;		/* user time */
    long si_nice;		/* nice time */
    long si_sys;		/* system time */
    long si_idle;		/* idle time */
    long si_hz;
    long si_phz;
    long si_boottime;		/* boot time in seconds */
    long wait;			/* wait time */
};

void
AddToTheSharedAddrList (unsigned long long addr, unsigned int size)
{
    struct sharedAddressList_s *tmpPtr1, *tmpPtr2;
    int indx;

    indx = (int) (((int) addr & 1016) >> 3);
    indx = 0;

    if (sharedAddressList[indx] == NULL)
    {
	sharedAddressList[indx] = (struct sharedAddressList_s *) malloc (sizeof (struct sharedAddressList_s));
	sharedAddressList[indx]->address = addr;
	sharedAddressList[indx]->size = size;
	sharedAddressList[indx]->next = NULL;
    }
    else
    {
	tmpPtr1 = sharedAddressList[indx];
	tmpPtr2 = sharedAddressList[indx];
	while (tmpPtr1 != NULL)
	{
	    tmpPtr2 = tmpPtr1;
	    tmpPtr1 = tmpPtr1->next;
	}
	tmpPtr2->next = (struct sharedAddressList_s *) malloc (sizeof (struct sharedAddressList_s));
	tmpPtr2->next->address = addr;
	tmpPtr2->next->size = size;
	tmpPtr2->next->next = NULL;
    }
#if 0
    tmpPtr1 = sharedAddressList;
    while (tmpPtr1 != NULL)
    {
	printf ("Addr = %llx, Size = %d\n", (unsigned long long) addr, (unsigned int) size);
	tmpPtr1 = tmpPtr1->next;
    }
#endif
}

#if defined(MTA)
void
print_stats (context * current)
{
    printf ("Total number of instructions executed = %d (%d)\n", current->sim_num_insn, current->id);

    return;
}

void
InitializeRUU (context * dest, context * src)
{
    int cnt, i, RUUIndx;
    struct RUU_station *RUUDest, *RUUSrc, *rs;
    struct RS_link *link, *newlink;

#if 0
    struct RS_link *tempRS;

    /* Lets print the source RUU Info */
    for (i = 0; i < RUU_size; i++)
    {
	printf ("Index of %d = %d, IR = %lx\n", i, src->RUU[i].index, src->RUU[i].IR);
	tempRS = src->RUU[i].odep_list[1];
	while (tempRS != NULL)
	{
	    printf ("X = %d ", tempRS->rs->index);
	    tempRS = tempRS->next;
	}
	printf ("\n");
    }
#endif
    /* This amount of copying can be reduced. Number of RUU entries
     * at system call is always zero */
    for (cnt = 0; cnt < RUU_size; cnt++)
    {
	RUUDest = &(dest->RUU[cnt]);
	RUUSrc = &(src->RUU[cnt]);

	RUUDest->index = RUUSrc->index;
	RUUDest->threadid = dest->id;
	RUUDest->IR = RUUSrc->IR;
	RUUDest->op = RUUSrc->op;
	RUUDest->PC = RUUSrc->PC;
	RUUDest->next_PC = RUUSrc->next_PC;
	RUUDest->pred_PC = RUUSrc->pred_PC;
	RUUDest->in_LSQ = RUUSrc->in_LSQ;
	RUUDest->ea_comp = RUUSrc->ea_comp;
	RUUDest->recover_inst = RUUSrc->recover_inst;
	RUUDest->stack_recover_idx = RUUSrc->stack_recover_idx;
	/* IMPORTANT NEED TO INITIALIZE BRACCN PREDICTOR UPDATE */
	printf ("YOU HAVE NOT INITALIZED BRANCH PREDICTOR INFO IN RUU RESULTS MAY NOT CORRECT\n");
	RUUDest->spec_mode = RUUSrc->spec_mode;
	RUUDest->addr = RUUSrc->addr;
	RUUDest->tag = RUUSrc->tag;
	RUUDest->seq = RUUSrc->seq;
	RUUDest->ptrace_seq = RUUSrc->ptrace_seq;
	RUUDest->queued = RUUSrc->queued;
	RUUDest->issued = RUUSrc->issued;
	RUUDest->completed = RUUSrc->completed;

	/* Not Sure if following are correct
	 * Most probably should be initialized to zero */
	RUUDest->val_ra = RUUSrc->val_ra;
	RUUDest->val_rb = RUUSrc->val_rb;
	RUUDest->val_rc = RUUSrc->val_rc;
	RUUDest->val_ra_result = RUUSrc->val_ra_result;

	/* There are only two output dependences */
	RUUDest->onames[0] = RUUSrc->onames[0];
	RUUDest->onames[1] = RUUSrc->onames[1];

	/* Create a chain of consuming operations
	 */
	/* It is a bit tricky here. Each RS link points to a 
	 * reservation station. Since we have allocated a new set of
	 * RS, we need to find an corresponding RS. This can be done
	 * using index. Get the index from the source and then find the 
	 * corresponding RS from the RUU set of the new Context */
	for (i = 0; i < MAX_ODEPS; i++)
	{
	    link = RUUSrc->odep_list[i];
	    while (link)
	    {
		RUUIndx = link->rs->index;
		rs = &(dest->RUU[RUUIndx]);
		RSLINK_NEW (newlink, rs);
		newlink->x.when = link->x.when;
		/* Add in the front of the odep_list */
		newlink->next = RUUDest->odep_list[i];
		RUUDest->odep_list[i] = newlink;
		link = link->next;
	    }
	}
	for (i = 0; i < MAX_IDEPS; i++)
	{
	    RUUDest->idep_ready[i] = RUUSrc->idep_ready[i];
	    RUUDest->when_idep_ready[i] = RUUSrc->when_idep_ready[i];
	    RUUDest->idep_name[i] = RUUSrc->idep_name[i];
	    if (RUUSrc->prod[i])
		RUUDest->prod[i] = &(dest->RUU[RUUSrc->prod[i]->index]);
	    else
		RUUDest->prod[i] = NULL;

	}
	RUUDest->disp_time = RUUSrc->disp_time;
	RUUDest->finish_time = RUUSrc->finish_time;
	RUUDest->issue_time = RUUSrc->issue_time;
	RUUDest->out1 = RUUSrc->out1;
/*	for (i =0; i < CLUSTERS; i++)
	{*/
	RUUDest->oldpreg = RUUSrc->oldpreg;
/*	}*/
	RUUDest->opreg = RUUSrc->opreg;
	/* This need to be changed. For the time being let's assigne
	 * the thread to the same cluster */
	RUUDest->cluster = RUUSrc->cluster;
	for (i = 0; i < MAX_IDEPS; i++)
	{
	    RUUDest->when_ready[i] = RUUSrc->when_ready[i];
	}
	RUUDest->when_inq = RUUSrc->when_inq;
	RUUDest->pbank = RUUSrc->pbank;
	RUUDest->abank = RUUSrc->abank;
	RUUDest->in_qwait = RUUSrc->in_qwait;
	RUUDest->freed = RUUSrc->freed;
	RUUDest->ipreg[0] = RUUSrc->ipreg[0];
	RUUDest->ipreg[1] = RUUSrc->ipreg[1];
	RUUDest->ipreg[2] = RUUSrc->ipreg[2];
	for (i = 0; i < CLUSTERS; i++)
	{
	    RUUDest->st_reach[i] = RUUSrc->st_reach[i];
	}
	RUUDest->instnum = RUUSrc->instnum;
	RUUDest->distissue = RUUSrc->distissue;
	/* Have not added Statistics. Let's first get the 
	 * thread running */
    }
}
#endif
#ifdef LOCK_HARD
extern int mesh_size;
#define MEM_LOC_SHIFT 1
#define WAIT_TIME	800000000
void lock_event_enqueue(struct LOCK_EVENT *event)     // event queue directory version, put dir event in queue
{
	struct LOCK_EVENT *ev, *prev;     

	for (prev = NULL, ev = lock_event_queue; ev && ((ev->when) <= (event->when)); prev = ev, ev = ev->next);
	if (prev)
	{
		event->next = ev;
		prev->next = event;
	}
	else
	{
		event->next = lock_event_queue;
		lock_event_queue = event;
	}
}
void lock_eventq_nextevent(void)
{
	struct LOCK_EVENT *event, *ev, *next, *prev;
	int i, insert_flag = 0;
	event = lock_event_queue;
	ev = NULL;
	while (event!=NULL)
	{
		next = event->next;

		if (event->when <= sim_cycle)
		{
			if(event == lock_event_queue)
			{
				lock_event_queue = event->next;
				event->next = NULL;
				SyncOperation(event->lock_id, event->threadid, event->opt);
				prev = event;
				free(prev);
				event = lock_event_queue;
			}
			else
			{
				ev->next = event->next;
				event->next = NULL;
				SyncOperation(event->lock_id, event->threadid, event->opt);
				prev = event;
				free(prev);
				event = ev->next;
			}
		}
		else
		{
			ev = event;
			event = event->next;
		}

	}
}

/* Optical system hardware lock implementation */
int SyncOperation (int lock_id, int threadid, int op)
{
	int lock_node = lock_id%numcontexts; /* which node this lock implementation is allocated */
	int lock_addr = lock_id/numcontexts;
	int bar_node = lock_node;
	int bar_addr = lock_addr;
	switch (op)
	{
		case BAR_ACQUIRE:
			collect_barrier_stats[threadid] = 1;
			if(bar_node == threadid)
			{ /* local node barrier access */
				struct BAR_ENTRY *bar_temp = BAR_REG[bar_node][bar_addr];
				bar_temp->barrier_num ++;	
				if(bar_temp->barrier_num == MAXTHREADS)
				{ /* release the barrier */
					bar_temp->barrier_num = 0;
					int i = 0;
					for(i=0;i<numcontexts;i++)
					{
						struct LOCK_EVENT *lock_event;
						lock_event = calloc(1, sizeof(struct LOCK_EVENT));
						if(lock_event == NULL) panic("Out of Virtual memory");
						barrier_temp = (barrier_temp + 1)%MAXBARRIER;
						lock_event->src1 = lock_node/mesh_size+MEM_LOC_SHIFT;
						lock_event->src2 = lock_node%mesh_size;
						lock_event->des1 = i/mesh_size+MEM_LOC_SHIFT;
						lock_event->des2 = i%mesh_size;
						lock_event->opt = BAR_RELEASE;
						lock_event->threadid = i;
						lock_event->lock_id = lock_id;
						lock_event->when = sim_cycle + WAIT_TIME;
						schedule_network(lock_event);
						lock_event_enqueue(lock_event);

					}
					if(collect_stats)
						TotalBarriers ++;
				}
				else
				{ /* waiting for the barrier */
					barrier_waiting[threadid] = 1;	
					thecontexts[threadid]->running = 0;
					thecontexts[threadid]->freeze = 1;
					sleepCount[threadid]++;
					thecontexts[threadid]->sleptAt = sim_cycle;
					freezeCounter++;
				}
			}	
			else
			{ /* remote node barrier access*/
				struct LOCK_EVENT *lock_event;
				lock_event = calloc(1, sizeof(struct LOCK_EVENT));
				if(lock_event == NULL) panic("Out of Virtual memory");
				lock_event->src1 = threadid/mesh_size+MEM_LOC_SHIFT;
				lock_event->src2 = threadid%mesh_size;
				lock_event->des1 = lock_node/mesh_size+MEM_LOC_SHIFT;
				lock_event->des2 = lock_node%mesh_size;
				lock_event->opt = BAR_REMOTE_ACC;
				lock_event->threadid = threadid;
				lock_event->lock_id = lock_id;
				lock_event->when = sim_cycle + WAIT_TIME;
				schedule_network(lock_event);
				lock_event_enqueue(lock_event);
				barrier_waiting[threadid] = 1;
			}
		break;
		case BAR_REMOTE_ACC:
		{
			struct BAR_ENTRY *bar_temp = BAR_REG[bar_node][bar_addr];
			bar_temp->arriving_vector[threadid] = 1;
			bar_temp->barrier_num ++;	
			if(bar_temp->barrier_num == numcontexts)
			{ /* release the barrier */
				bar_temp->barrier_num = 0;
				int i = 0;
				for(i=0;i<numcontexts;i++)
				{
					bar_temp->arriving_vector[threadid] = 0;
					struct LOCK_EVENT *lock_event;
					lock_event = calloc(1, sizeof(struct LOCK_EVENT));
					if(lock_event == NULL) panic("Out of Virtual memory");
					barrier_temp = (barrier_temp + 1)%MAXBARRIER;
					lock_event->src1 = lock_node/mesh_size+MEM_LOC_SHIFT;
					lock_event->src2 = lock_node%mesh_size;
					lock_event->des1 = i/mesh_size+MEM_LOC_SHIFT;
					lock_event->des2 = i%mesh_size;
					lock_event->opt = BAR_RELEASE;
					lock_event->threadid = i;
					lock_event->lock_id = lock_id;
					lock_event->when = sim_cycle + WAIT_TIME;
					schedule_network(lock_event);
					lock_event_enqueue(lock_event);

				}
				if(collect_stats)
					TotalBarriers ++;
			}
			else
			{ /* waiting for the barrier */
				struct LOCK_EVENT *lock_event;
				lock_event = calloc(1, sizeof(struct LOCK_EVENT));
				if(lock_event == NULL) panic("Out of Virtual memory");
				lock_event->src1 = lock_node/mesh_size+MEM_LOC_SHIFT;
				lock_event->src2 = lock_node%mesh_size;
				lock_event->des1 = threadid/mesh_size+MEM_LOC_SHIFT;
				lock_event->des2 = threadid%mesh_size;
				lock_event->opt = BAR_WAIT_ACK;
				lock_event->threadid = threadid;
				lock_event->lock_id = lock_id;
				lock_event->when = sim_cycle + WAIT_TIME;
				schedule_network(lock_event);
				lock_event_enqueue(lock_event);
			}
		}
		break;
		case BAR_WAIT_ACK:
			collect_barrier_stats[threadid] = 1;
			barrier_waiting[threadid] = 1;	
			thecontexts[threadid]->running = 0;
			thecontexts[threadid]->freeze = 1;
			sleepCount[threadid]++;
			thecontexts[threadid]->sleptAt = sim_cycle;
			freezeCounter++;
		break;
		case BAR_RELEASE:
			thecontexts[threadid]->running = 1;
			thecontexts[threadid]->freeze = 0;
			barrier_waiting[threadid] = 0;
			collect_barrier_stats[threadid] = 0;
		break;
		case ACQUIRE:
			if(lock_node == threadid)
			{ /* local node lock access*/
				struct LOCK_ENTRY *Lock_temp = LOCK_REG[lock_node][lock_addr];
				if(Lock_temp->lock_owner != 0)
				{ /* waiting for the lock */
					lock_waiting[threadid] = 1;
					Lock_temp->waiting_list[Lock_temp->waiting_tail] = threadid;
					Lock_temp->waiting_num ++;
					Lock_temp->waiting_tail = (Lock_temp->waiting_tail+1+MAXLOCK)%MAXLOCK;
					if(Lock_temp->waiting_tail == Lock_temp->waiting_head)
						panic("Ideal lock: Lock acquire queue full\n");
				}
				else
				{ /* acquire the lock */
					Lock_temp->lock_owner = threadid + 1;
				}
			}
			else
			{ /* remote node lock access*/
				struct LOCK_EVENT *lock_event;
				lock_event = calloc(1, sizeof(struct LOCK_EVENT));
				if(lock_event == NULL) panic("Out of Virtual memory");
				lock_event->src1 = threadid/mesh_size+MEM_LOC_SHIFT;
				lock_event->src2 = threadid%mesh_size;
				lock_event->des1 = lock_node/mesh_size+MEM_LOC_SHIFT;
				lock_event->des2 = lock_node%mesh_size;
				lock_event->opt = ACQ_REMOTE_ACC;
				lock_event->threadid = threadid;
				lock_event->lock_id = lock_id;
				lock_event->when = sim_cycle + WAIT_TIME;
				schedule_network(lock_event);
				lock_event_enqueue(lock_event);
				lock_waiting[threadid] = 1;
			}
		break;
		case RELEASE:
			if(lock_node == threadid)
			{ /* local node lock access*/
				struct LOCK_ENTRY *Lock_temp = LOCK_REG[lock_node][lock_addr];
				if(Lock_temp->lock_owner != threadid + 1)
					panic("Ideal lock: not the right thread release the lock!\n");
				else
				{ /* release for the lock */
					Lock_temp->lock_owner = 0;
					if(Lock_temp->waiting_num)
					{ /* awake other threads try to acquire the lock */
						int cnt = Lock_temp->waiting_list[Lock_temp->waiting_head];
						Lock_temp->lock_owner = cnt+1;
						Lock_temp->waiting_num --;
						Lock_temp->waiting_head = (Lock_temp->waiting_head+1+MAXLOCK)%MAXLOCK;
						if(cnt == threadid)
							panic("thread can not acquire two locks at the same time");
						else
						{
							struct LOCK_EVENT *lock_event;
							lock_event = calloc(1, sizeof(struct LOCK_EVENT));
							if(lock_event == NULL) panic("Out of Virtual memory");
							lock_event->src1 = lock_node/mesh_size+MEM_LOC_SHIFT;
							lock_event->src2 = lock_node%mesh_size;
							lock_event->des1 = cnt/mesh_size+MEM_LOC_SHIFT;
							lock_event->des2 = cnt%mesh_size;
							lock_event->opt = ACQ_ACK;
							lock_event->threadid = cnt;
							lock_event->lock_id = lock_id;
							lock_event->when = sim_cycle + WAIT_TIME;
							schedule_network(lock_event);
							lock_event_enqueue(lock_event);
						}
					}
				}
			}
			else
			{ /* remote node lock access*/
				struct LOCK_EVENT *lock_event;
				lock_event = calloc(1, sizeof(struct LOCK_EVENT));
				if(lock_event == NULL) panic("Out of Virtual memory");
				lock_event->src1 = threadid/mesh_size+MEM_LOC_SHIFT;
				lock_event->src2 = threadid%mesh_size;
				lock_event->des1 = lock_node/mesh_size+MEM_LOC_SHIFT;
				lock_event->des2 = lock_node%mesh_size;
				lock_event->opt = REL_REMOTE_ACC;
				lock_event->threadid = threadid;
				lock_event->lock_id = lock_id;
				lock_event->when = sim_cycle + WAIT_TIME;
				schedule_network(lock_event);
				lock_event_enqueue(lock_event);
			}
			
		break;
		case ACQ_REMOTE_ACC:
		{
			struct LOCK_ENTRY *Lock_temp = LOCK_REG[lock_node][lock_addr];
			struct LOCK_EVENT *lock_event;
			lock_event = calloc(1, sizeof(struct LOCK_EVENT));
			if(lock_event == NULL) panic("Out of Virtual memory");

			if(Lock_temp->lock_owner != 0)
			{ /* waiting for the lock */
				Lock_temp->waiting_list[Lock_temp->waiting_tail] = threadid;
				Lock_temp->waiting_num ++;
				Lock_temp->waiting_tail = (Lock_temp->waiting_tail+1+MAXLOCK)%MAXLOCK;
				if(Lock_temp->waiting_tail == Lock_temp->waiting_head)
					panic("Ideal lock: Lock acquire queue full\n");
				lock_event->opt = WAIT_ACK;
			}
			else
			{ /* acquire the lock */
				Lock_temp->lock_owner = threadid + 1;
				lock_event->opt = ACQ_ACK;
			}

			lock_event->src1 = lock_node/mesh_size+MEM_LOC_SHIFT;
			lock_event->src2 = lock_node%mesh_size;
			lock_event->des1 = threadid/mesh_size+MEM_LOC_SHIFT;
			lock_event->des2 = threadid%mesh_size;
			lock_event->threadid = threadid;
			lock_event->lock_id = lock_id;
			lock_event->when = sim_cycle + WAIT_TIME;
			schedule_network(lock_event);
			lock_event_enqueue(lock_event);
		}
		break;
		case REL_REMOTE_ACC:
		{
			struct LOCK_ENTRY *Lock_temp = LOCK_REG[lock_node][lock_addr];
			if(Lock_temp->lock_owner != threadid + 1)
				panic("Ideal lock: not the right thread release the lock!\n");
			else
			{ /* release for the lock */
				Lock_temp->lock_owner = 0;
				if(Lock_temp->waiting_num)
				{ /* awake other threads try to acquire the lock */
					int cnt = Lock_temp->waiting_list[Lock_temp->waiting_head];
					Lock_temp->lock_owner = cnt+1;
					Lock_temp->waiting_num --;
					Lock_temp->waiting_head = (Lock_temp->waiting_head+1+MAXLOCK)%MAXLOCK;

					struct LOCK_EVENT *lock_event;
					lock_event = calloc(1, sizeof(struct LOCK_EVENT));
					if(lock_event == NULL) panic("Out of Virtual memory");
					lock_event->src1 = lock_node/mesh_size+MEM_LOC_SHIFT;
					lock_event->src2 = lock_node%mesh_size;
					lock_event->des1 = cnt/mesh_size+MEM_LOC_SHIFT;
					lock_event->des2 = cnt%mesh_size;
					lock_event->opt = ACQ_ACK;
					lock_event->threadid = cnt;
					lock_event->lock_id = lock_id;
					lock_event->when = sim_cycle + WAIT_TIME;
					schedule_network(lock_event);
					lock_event_enqueue(lock_event);
				}
			}
		}
		break;
		case ACQ_ACK:
			lock_waiting[threadid] = 0;
		break;
		case WAIT_ACK:
			lock_waiting[threadid] = 1;
		break;
		default:
			panic("no one comes here!");
	}	
	return 0;
}
#endif

/* OSF SYSCALL -- standard system call sequence
   the kernel expects arguments to be passed with the normal C calling
   sequence; v0 should contain the system call number; on return from the
   kernel mode, a3 will be 0 to indicate no error and non-zero to indicate an
   error; if an error occurred v0 will contain an errno; if the kernel return
   an error, setup a valid gp and jmp to _cerror */

/* syscall proxy handler, architect registers and memory are assumed to be
   precise when this function is called, register and memory are updated with
   the results of the sustem call */

typedef enum {
  /* 0  r0 */  R_V0,    // return value
  /* 1  r1 */  R_T0,    // temporary register
  /* 2  r2 */  R_T1,    // temporary register
  /* 3  r3 */  R_T2,    // temporary register
  /* 4  r4 */  R_T3,    // temporary register
  /* 5  r5 */  R_T4,    // temporary register
  /* 6  r6 */  R_T5,    // temporary register
  /* 7  r7 */  R_T6,    // temporary register
  /* 8  r8 */  R_T7,    // temporary register
  /* 9  r9 */  R_S0,    // saved register
  /* 10 r10 */ R_S1,    // saved register
  /* 11 r11 */ R_S2,    // saved register
  /* 12 r12 */ R_S3,    // saved register
  /* 13 r13 */ R_S4,    // saved register
  /* 14 r14 */ R_S5,    // saved register
  /* 15 r15 */ R_FP,    // frame pointer
  /* 16 r16 */ R_A0,    // function argument
  /* 17 r17 */ R_A1,    // function argument
  /* 18 r18 */ R_A2,    // function argument
  /* 19 r19 */ R_A3,    // function argument
  /* 20 r20 */ R_A4,    // function argument
  /* 21 r21 */ R_A5,    // function argument
  /* 22 r22 */ R_T8,    // temporary register
  /* 23 r23 */ R_T9,    // temporary register
  /* 24 r24 */ R_T10,   // temporary register
  /* 25 r25 */ R_T11,   // temporary register
  /* 26 r26 */ R_RA,    // function return address
  /* 27 r27 */ R_PV,    // procedure value
  /* 28 r28 */ R_AT,    // reserved for assembler
  /* 29 r29 */ R_GP,    // global pointer
  /* 30 r30 */ R_SP,    // stack pointer
  /* 31 r31 */ R_IZERO, // integer zero
  /* 32 f0  */ R_F0,    // floating point register
  /* 33 f1  */ R_F1,    // floating point register
  /* 34 f2  */ R_F2,    // floating point register
  /* 35 f3  */ R_F3,    // floating point register
  /* 36 f4  */ R_F4,    // floating point register
  /* 37 f5  */ R_F5,    // floating point register
  /* 38 f6  */ R_F6,    // floating point register
  /* 39 f7  */ R_F7,    // floating point register
  /* 40 f8  */ R_F8,    // floating point register
  /* 41 f9  */ R_F9,    // floating point register
  /* 42 f10 */ R_F10,   // floating point register
  /* 43 f11 */ R_F11,   // floating point register
  /* 44 f12 */ R_F12,   // floating point register
  /* 45 f13 */ R_F13,   // floating point register
  /* 46 f14 */ R_F14,   // floating point register
  /* 47 f15 */ R_F15,   // floating point register
  /* 48 f16 */ R_F16,   // floating point register
  /* 49 f17 */ R_F17,   // floating point register
  /* 50 f18 */ R_F18,   // floating point register
  /* 51 f19 */ R_F19,   // floating point register
  /* 52 f20 */ R_F20,   // floating point register
  /* 53 f21 */ R_F21,   // floating point register
  /* 54 f22 */ R_F22,   // floating point register
  /* 55 f23 */ R_F23,   // floating point register
  /* 56 f24 */ R_F24,   // floating point register
  /* 57 f25 */ R_F25,   // floating point register
  /* 58 f26 */ R_F26,   // floating point register
  /* 59 f27 */ R_F27,   // floating point register
  /* 60 f28 */ R_F28,   // floating point register
  /* 61 f29 */ R_F29,   // floating point register
  /* 62 f30 */ R_F30,   // floating point register
  /* 63 f31 */ R_FZERO, // floating point zero
  /* 64     */ R_FPCR,  // floating point condition register
  /* 65     */ R_UNIQ,  // process unique register
  /* 66     */ NUM_REGS // number of registers, not an actual register
} RegNames;

typedef enum {
	false,
	true
} bool; 

typedef enum 
{
  // OSF system call codes //////////////////////////////////////////////////
  /*   0 */  SYSCALL_OSF_SYSCALL,
  /*   1 */  SYSCALL_EXIT,
  /*   2 */  SYSCALL_FORK,
  /*   3 */  SYSCALL_READ,
  /*   4 */  SYSCALL_WRITE,
  /*   5 */  SYSCALL_OSF_OLD_OPEN,
  /*   6 */  SYSCALL_CLOSE,
  /*   7 */  SYSCALL_OSF_WAIT4,
  /*   8 */  SYSCALL_OSF_OLD_CREAT,
  /*   9 */  SYSCALL_LINK,
  /*  10 */  SYSCALL_UNLINK,
  /*  11 */  SYSCALL_OSF_EXECVE,
  /*  12 */  SYSCALL_CHDIR,
  /*  13 */  SYSCALL_FCHDIR,
  /*  14 */  SYSCALL_MKNOD,
  /*  15 */  SYSCALL_CHMOD,
  /*  16 */  SYSCALL_CHOWN,
  /*  17 */  SYSCALL_BRK,
  /*  18 */  SYSCALL_OSF_GETFSSTAT,
  /*  19 */  SYSCALL_LSEEK,
  /*  20 */  SYSCALL_GETXPID,
  /*  21 */  SYSCALL_OSF_MOUNT,
  /*  22 */  SYSCALL_UMOUNT,
  /*  23 */  SYSCALL_SETUID,
  /*  24 */  SYSCALL_GETXUID,
  /*  25 */  SYSCALL_EXEC_WITH_LOADER,
  /*  26 */  SYSCALL_OSF_PTRACE,
  /*  27 */  SYSCALL_OSF_NRECVMSG,
  /*  28 */  SYSCALL_OSF_NSENDMSG,
  /*  29 */  SYSCALL_OSF_NRECVFROM,
  /*  30 */  SYSCALL_OSF_NACCEPT,
  /*  31 */  SYSCALL_OSF_NGETPEERNAME,
  /*  32 */  SYSCALL_OSF_NGETSOCKNAME,
  /*  33 */  SYSCALL_ACCESS,
  /*  34 */  SYSCALL_OSF_CHFLAGS,
  /*  35 */  SYSCALL_OSF_FCHFLAGS,
  /*  36 */  SYSCALL_SYNC,
  /*  37 */  SYSCALL_KILL,
  /*  38 */  SYSCALL_OSF_OLD_STAT,
  /*  39 */  SYSCALL_SETPGID,
  /*  40 */  SYSCALL_OSF_OLD_LSTAT,
  /*  41 */  SYSCALL_DUP,
  /*  42 */  SYSCALL_PIPE,
  /*  43 */  SYSCALL_OSF_SET_PROGRAM_ATTRIBUTES,
  /*  44 */  SYSCALL_OSF_PROFIL,
  /*  45 */  SYSCALL_OPEN,
  /*  46 */  SYSCALL_OSF_OLD_SIGACTION,
  /*  47 */  SYSCALL_GETXGID,
  /*  48 */  SYSCALL_OSF_SIGPROCMASK,
  /*  49 */  SYSCALL_OSF_GETLOGIN,
  /*  50 */  SYSCALL_OSF_SETLOGIN,
  /*  51 */  SYSCALL_ACCT,
  /*  52 */  SYSCALL_SIGPENDING,
  /*  53 */  SYSCALL_OSF_CLASSCNTL,
  /*  54 */  SYSCALL_IOCTL,
  /*  55 */  SYSCALL_OSF_REBOOT,
  /*  56 */  SYSCALL_OSF_REVOKE,
  /*  57 */  SYSCALL_SYMLINK,
  /*  58 */  SYSCALL_READLINK,
  /*  59 */  SYSCALL_EXECVE,
  /*  60 */  SYSCALL_UMASK,
  /*  61 */  SYSCALL_CHROOT,
  /*  62 */  SYSCALL_OSF_OLD_FSTAT,
  /*  63 */  SYSCALL_GETPGRP,
  /*  64 */  SYSCALL_GETPAGESIZE,
  /*  65 */  SYSCALL_OSF_MREMAP,
  /*  66 */  SYSCALL_VFORK,
  /*  67 */  SYSCALL_STAT,
  /*  68 */  SYSCALL_LSTAT,
  /*  69 */  SYSCALL_OSF_SBRK,
  /*  70 */  SYSCALL_OSF_SSTK,
  /*  71 */  SYSCALL_MMAP,
  /*  72 */  SYSCALL_OSF_OLD_VADVISE,
  /*  73 */  SYSCALL_MUNMAP,
  /*  74 */  SYSCALL_MPROTECT,
  /*  75 */  SYSCALL_MADVISE,
  /*  76 */  SYSCALL_VHANGUP,
  /*  77 */  SYSCALL_OSF_KMODCALL,
  /*  78 */  SYSCALL_OSF_MINCORE,
  /*  79 */  SYSCALL_GETGROUPS,
  /*  80 */  SYSCALL_SETGROUPS,
  /*  81 */  SYSCALL_OSF_OLD_GETPGRP,
  /*  82 */  SYSCALL_SETPGRP,
  /*  83 */  SYSCALL_OSF_SETITIMER,
  /*  84 */  SYSCALL_OSF_OLD_WAIT,
  /*  85 */  SYSCALL_OSF_TABLE,
  /*  86 */  SYSCALL_OSF_GETITIMER,
  /*  87 */  SYSCALL_GETHOSTNAME,
  /*  88 */  SYSCALL_SETHOSTNAME,
  /*  89 */  SYSCALL_GETDTABLESIZE,
  /*  90 */  SYSCALL_DUP2,
  /*  91 */  SYSCALL_FSTAT,
  /*  92 */  SYSCALL_FCNTL,
  /*  93 */  SYSCALL_OSF_SELECT,
  /*  94 */  SYSCALL_POLL,
  /*  95 */  SYSCALL_FSYNC,
  /*  96 */  SYSCALL_SETPRIORITY,
  /*  97 */  SYSCALL_SOCKET,
  /*  98 */  SYSCALL_CONNECT,
  /*  99 */  SYSCALL_ACCEPT,
  /* 100 */  SYSCALL_GETPRIORITY,
  /* 101 */  SYSCALL_SEND,
  /* 102 */  SYSCALL_RECV,
  /* 103 */  SYSCALL_SIGRETURN,
  /* 104 */  SYSCALL_BIND,
  /* 105 */  SYSCALL_SETSOCKOPT,
  /* 106 */  SYSCALL_LISTEN,
  /* 107 */  SYSCALL_OSF_PLOCK,
  /* 108 */  SYSCALL_OSF_OLD_SIGVEC,
  /* 109 */  SYSCALL_OSF_OLD_SIGBLOCK,
  /* 110 */  SYSCALL_OSF_OLD_SIGSETMASK,
  /* 111 */  SYSCALL_SIGSUSPEND,
  /* 112 */  SYSCALL_OSF_SIGSTACK,
  /* 113 */  SYSCALL_RECVMSG,
  /* 114 */  SYSCALL_SENDMSG,
  /* 115 */  SYSCALL_OSF_OLD_VTRACE,
  /* 116 */  SYSCALL_OSF_GETTIMEOFDAY,
  /* 117 */  SYSCALL_OSF_GETRUSAGE,
  /* 118 */  SYSCALL_GETSOCKOPT,
  /* 119 */  SYSCALL_NUMA_SYSCALLS,
  /* 120 */  SYSCALL_READV,
  /* 121 */  SYSCALL_WRITEV,
  /* 122 */  SYSCALL_OSF_SETTIMEOFDAY,
  /* 123 */  SYSCALL_FCHOWN,
  /* 124 */  SYSCALL_FCHMOD,
  /* 125 */  SYSCALL_RECVFROM,
  /* 126 */  SYSCALL_SETREUID,
  /* 127 */  SYSCALL_SETREGID,
  /* 128 */  SYSCALL_RENAME,
  /* 129 */  SYSCALL_TRUNCATE,
  /* 130 */  SYSCALL_FTRUNCATE,
  /* 131 */  SYSCALL_FLOCK,
  /* 132 */  SYSCALL_SETGID,
  /* 133 */  SYSCALL_SENDTO,
  /* 134 */  SYSCALL_SHUTDOWN,
  /* 135 */  SYSCALL_SOCKETPAIR,
  /* 136 */  SYSCALL_MKDIR,
  /* 137 */  SYSCALL_RMDIR,
  /* 138 */  SYSCALL_OSF_UTIMES,
  /* 139 */  SYSCALL_OSF_OLD_SIGRETURN,
  /* 140 */  SYSCALL_OSF_ADJTIME,
  /* 141 */  SYSCALL_GETPEERNAME,
  /* 142 */  SYSCALL_OSF_GETHOSTID,
  /* 143 */  SYSCALL_OSF_SETHOSTID,
  /* 144 */  SYSCALL_GETRLIMIT,
  /* 145 */  SYSCALL_SETRLIMIT,
  /* 146 */  SYSCALL_OSF_OLD_KILLPG,
  /* 147 */  SYSCALL_SETSID,
  /* 148 */  SYSCALL_QUOTACTL,
  /* 149 */  SYSCALL_OSF_OLDQUOTA,
  /* 150 */  SYSCALL_GETSOCKNAME,
  /* 151 */  SYSCALL_OSF_PREAD,
  /* 152 */  SYSCALL_OSF_PWRITE,
  /* 153 */  SYSCALL_OSF_PID_BLOCK,
  /* 154 */  SYSCALL_OSF_PID_UNBLOCK,
  /* 155 */  SYSCALL_OSF_SIGNAL_URTI,
  /* 156 */  SYSCALL_SIGACTION,
  /* 157 */  SYSCALL_OSF_SIGWAITPRIM,
  /* 158 */  SYSCALL_OSF_NFSSVC,
  /* 159 */  SYSCALL_OSF_GETDIRENTRIES,
  /* 160 */  SYSCALL_OSF_STATFS,
  /* 161 */  SYSCALL_OSF_FSTATFS,
  /* 162 */  SYSCALL_UNKNOWN_162,
  /* 163 */  SYSCALL_OSF_ASYNC_DAEMON,
  /* 164 */  SYSCALL_OSF_GETFH,
  /* 165 */  SYSCALL_OSF_GETDOMAINNAME,
  /* 166 */  SYSCALL_SETDOMAINNAME,
  /* 167 */  SYSCALL_UNKNOWN_167,
  /* 168 */  SYSCALL_UNKNOWN_168,
  /* 169 */  SYSCALL_OSF_EXPORTFS,
  /* 170 */  SYSCALL_UNKNOWN_170,
  /* 171 */  SYSCALL_UNKNOWN_171,
  /* 172 */  SYSCALL_UNKNOWN_172,
  /* 173 */  SYSCALL_UNKNOWN_173,
  /* 174 */  SYSCALL_UNKNOWN_174,
  /* 175 */  SYSCALL_UNKNOWN_175,
  /* 176 */  SYSCALL_UNKNOWN_176,
  /* 177 */  SYSCALL_UNKNOWN_177,
  /* 178 */  SYSCALL_UNKNOWN_178,
  /* 179 */  SYSCALL_UNKNOWN_179,
  /* 180 */  SYSCALL_UNKNOWN_180,
  /* 181 */  SYSCALL_OSF_ALT_PLOCK,
  /* 182 */  SYSCALL_UNKNOWN_182,
  /* 183 */  SYSCALL_UNKNOWN_183,
  /* 184 */  SYSCALL_OSF_GETMNT,
  /* 185 */  SYSCALL_UNKNOWN_185,
  /* 186 */  SYSCALL_UNKNOWN_186,
  /* 187 */  SYSCALL_OSF_ALT_SIGPENDING,
  /* 188 */  SYSCALL_OSF_ALT_SETSID,
  /* 189 */  SYSCALL_UNKNOWN_189,
  /* 190 */  SYSCALL_UNKNOWN_190,
  /* 191 */  SYSCALL_UNKNOWN_191,
  /* 192 */  SYSCALL_UNKNOWN_192,
  /* 193 */  SYSCALL_UNKNOWN_193,
  /* 194 */  SYSCALL_UNKNOWN_194,
  /* 195 */  SYSCALL_UNKNOWN_195,
  /* 196 */  SYSCALL_UNKNOWN_196,
  /* 197 */  SYSCALL_UNKNOWN_197,
  /* 198 */  SYSCALL_UNKNOWN_198,
  /* 199 */  SYSCALL_OSF_SWAPON,
  /* 200 */  SYSCALL_MSGCTL,
  /* 201 */  SYSCALL_MSGGET,
  /* 202 */  SYSCALL_MSGRCV,
  /* 203 */  SYSCALL_MSGSND,
  /* 204 */  SYSCALL_SEMCTL,
  /* 205 */  SYSCALL_SEMGET,
  /* 206 */  SYSCALL_SEMOP,
  /* 207 */  SYSCALL_OSF_UTSNAME,
  /* 208 */  SYSCALL_LCHOWN,
  /* 209 */  SYSCALL_OSF_SHMAT,
  /* 210 */  SYSCALL_SHMCTL,
  /* 211 */  SYSCALL_SHMDT,
  /* 212 */  SYSCALL_SHMGET,
  /* 213 */  SYSCALL_OSF_MVALID,
  /* 214 */  SYSCALL_OSF_GETADDRESSCONF,
  /* 215 */  SYSCALL_OSF_MSLEEP,
  /* 216 */  SYSCALL_OSF_MWAKEUP,
  /* 217 */  SYSCALL_MSYNC,
  /* 218 */  SYSCALL_OSF_SIGNAL,
  /* 219 */  SYSCALL_OSF_UTC_GETTIME,
  /* 220 */  SYSCALL_OSF_UTC_ADJTIME,
  /* 221 */  SYSCALL_UNKNOWN_221,
  /* 222 */  SYSCALL_OSF_SECURITY,
  /* 223 */  SYSCALL_OSF_KLOADCALL,
  /* 224 */  SYSCALL_UNKNOWN_224,
  /* 225 */  SYSCALL_UNKNOWN_225,
  /* 226 */  SYSCALL_UNKNOWN_226,
  /* 227 */  SYSCALL_UNKNOWN_227,
  /* 228 */  SYSCALL_UNKNOWN_228,
  /* 229 */  SYSCALL_UNKNOWN_229,
  /* 230 */  SYSCALL_UNKNOWN_230,
  /* 231 */  SYSCALL_UNKNOWN_231,
  /* 232 */  SYSCALL_UNKNOWN_232,
  /* 233 */  SYSCALL_GETPGID,
  /* 234 */  SYSCALL_GETSID,
  /* 235 */  SYSCALL_SIGALTSTACK,
  /* 236 */  SYSCALL_OSF_WAITID,
  /* 237 */  SYSCALL_OSF_PRIOCNTLSET,
  /* 238 */  SYSCALL_OSF_SIGSENDSET,
  /* 239 */  SYSCALL_OSF_SET_SPECULATIVE,
  /* 240 */  SYSCALL_OSF_MSFS_SYSCALL,
  /* 241 */  SYSCALL_OSF_SYSINFO,
  /* 242 */  SYSCALL_OSF_UADMIN,
  /* 243 */  SYSCALL_OSF_FUSER,
  /* 244 */  SYSCALL_OSF_PROPLIST_SYSCALL,
  /* 245 */  SYSCALL_OSF_NTP_ADJTIME,
  /* 246 */  SYSCALL_OSF_NTP_GETTIME,
  /* 247 */  SYSCALL_OSF_PATHCONF,
  /* 248 */  SYSCALL_OSF_FPATHCONF,
  /* 249 */  SYSCALL_UNKNOWN_249,
  /* 250 */  SYSCALL_OSF_USWITCH,
  /* 251 */  SYSCALL_OSF_USLEEP_THREAD,
  /* 252 */  SYSCALL_OSF_AUDCNTL,
  /* 253 */  SYSCALL_OSF_AUDGEN,
  /* 254 */  SYSCALL_SYSFS,
  /* 255 */  SYSCALL_OSF_SUBSYS_INFO,
  /* 256 */  SYSCALL_OSF_GETSYSINFO,
  /* 257 */  SYSCALL_OSF_SETSYSINFO,
  /* 258 */  SYSCALL_OSF_AFS_SYSCALL,
  /* 259 */  SYSCALL_OSF_SWAPCTL,
  /* 260 */  SYSCALL_OSF_MEMCNTL,
  /* 261 */  SYSCALL_OSF_FDATASYNC,
  /* 262 */  SYSCALL_UNKNOWN_262,
  /* 263 */  SYSCALL_UNKNOWN_263,
  /* 264 */  SYSCALL_UNKNOWN_264,
  /* 265 */  SYSCALL_UNKNOWN_265,
  /* 266 */  SYSCALL_UNKNOWN_266,
  /* 267 */  SYSCALL_UNKNOWN_267,
  /* 268 */  SYSCALL_UNKNOWN_268,
  /* 269 */  SYSCALL_UNKNOWN_269,
  /* 270 */  SYSCALL_UNKNOWN_270,
  /* 271 */  SYSCALL_UNKNOWN_271,
  /* 272 */  SYSCALL_UNKNOWN_272,
  /* 273 */  SYSCALL_UNKNOWN_273,
  /* 274 */  SYSCALL_UNKNOWN_274,
  /* 275 */  SYSCALL_UNKNOWN_275,
  /* 276 */  SYSCALL_UNKNOWN_276,
  /* 277 */  SYSCALL_UNKNOWN_277,
  /* 278 */  SYSCALL_UNKNOWN_278,
  /* 279 */  SYSCALL_UNKNOWN_279,
  /* 280 */  SYSCALL_UNKNOWN_280,
  /* 281 */  SYSCALL_UNKNOWN_281,
  /* 282 */  SYSCALL_UNKNOWN_282,
  /* 283 */  SYSCALL_UNKNOWN_283,
  /* 284 */  SYSCALL_UNKNOWN_284,
  /* 285 */  SYSCALL_UNKNOWN_285,
  /* 286 */  SYSCALL_UNKNOWN_286,
  /* 287 */  SYSCALL_UNKNOWN_287,
  /* 288 */  SYSCALL_UNKNOWN_288,
  /* 289 */  SYSCALL_UNKNOWN_289,
  /* 290 */  SYSCALL_UNKNOWN_290,
  /* 291 */  SYSCALL_UNKNOWN_291,
  /* 292 */  SYSCALL_UNKNOWN_292,
  /* 293 */  SYSCALL_UNKNOWN_293,
  /* 294 */  SYSCALL_UNKNOWN_294,
  /* 295 */  SYSCALL_UNKNOWN_295,
  /* 296 */  SYSCALL_UNKNOWN_296,
  /* 297 */  SYSCALL_UNKNOWN_297,
  /* 298 */  SYSCALL_UNKNOWN_298,
  /* 299 */  SYSCALL_UNKNOWN_299,

  // Linux-specific system call codes ///////////////////////////////////////
  /* 300 */  SYSCALL_BDFLUSH,
  /* 301 */  SYSCALL_SETHAE,
  /* 302 */  SYSCALL_MOUNT,
  /* 303 */  SYSCALL_OLD_ADJTIMEX,
  /* 304 */  SYSCALL_SWAPOFF,
  /* 305 */  SYSCALL_GETDENTS,
  /* 306 */  SYSCALL_CREATE_MODULE,
  /* 307 */  SYSCALL_INIT_MODULE,
  /* 308 */  SYSCALL_DELETE_MODULE,
  /* 309 */  SYSCALL_GET_KERNEL_SYMS,
  /* 310 */  SYSCALL_SYSLOG,
  /* 311 */  SYSCALL_REBOOT,
  /* 312 */  SYSCALL_CLONE,
  /* 313 */  SYSCALL_USELIB,
  /* 314 */  SYSCALL_MLOCK,
  /* 315 */  SYSCALL_MUNLOCK,
  /* 316 */  SYSCALL_MLOCKALL,
  /* 317 */  SYSCALL_MUNLOCKALL,
  /* 318 */  SYSCALL_SYSINFO,
  /* 319 */  SYSCALL__SYSCTL,
  /* 320 */  SYSCALL_WAS_SYS_IDLE,
  /* 321 */  SYSCALL_OLDUMOUNT,
  /* 322 */  SYSCALL_SWAPON,
  /* 323 */  SYSCALL_TIMES,
  /* 324 */  SYSCALL_PERSONALITY,
  /* 325 */  SYSCALL_SETFSUID,
  /* 326 */  SYSCALL_SETFSGID,
  /* 327 */  SYSCALL_USTAT,
  /* 328 */  SYSCALL_STATFS,
  /* 329 */  SYSCALL_FSTATFS,
  /* 330 */  SYSCALL_SCHED_SETPARAM,
  /* 331 */  SYSCALL_SCHED_GETPARAM,
  /* 332 */  SYSCALL_SCHED_SETSCHEDULER,
  /* 333 */  SYSCALL_SCHED_GETSCHEDULER,
  /* 334 */  SYSCALL_SCHED_YIELD,
  /* 335 */  SYSCALL_SCHED_GET_PRIORITY_MAX,
  /* 336 */  SYSCALL_SCHED_GET_PRIORITY_MIN,
  /* 337 */  SYSCALL_SCHED_RR_GET_INTERVAL,
  /* 338 */  SYSCALL_AFS_SYSCALL,
  /* 339 */  SYSCALL_UNAME,
  /* 340 */  SYSCALL_NANOSLEEP,
  /* 341 */  SYSCALL_MREMAP,
  /* 342 */  SYSCALL_NFSSERVCTL,
  /* 343 */  SYSCALL_SETRESUID,
  /* 344 */  SYSCALL_GETRESUID,
  /* 345 */  SYSCALL_PCICONFIG_READ,
  /* 346 */  SYSCALL_PCICONFIG_WRITE,
  /* 347 */  SYSCALL_QUERY_MODULE,
  /* 348 */  SYSCALL_PRCTL,
  /* 349 */  SYSCALL_PREAD,
  /* 350 */  SYSCALL_PWRITE,
  /* 351 */  SYSCALL_RT_SIGRETURN,
  /* 352 */  SYSCALL_RT_SIGACTION,
  /* 353 */  SYSCALL_RT_SIGPROCMASK,
  /* 354 */  SYSCALL_RT_SIGPENDING,
  /* 355 */  SYSCALL_RT_SIGTIMEDWAIT,
  /* 356 */  SYSCALL_RT_SIGQUEUEINFO,
  /* 357 */  SYSCALL_RT_SIGSUSPEND,
  /* 358 */  SYSCALL_SELECT,
  /* 359 */  SYSCALL_GETTIMEOFDAY,
  /* 360 */  SYSCALL_SETTIMEOFDAY,
  /* 361 */  SYSCALL_GETITIMER,
  /* 362 */  SYSCALL_SETITIMER,
  /* 363 */  SYSCALL_UTIMES,
  /* 364 */  SYSCALL_GETRUSAGE,
  /* 365 */  SYSCALL_WAIT4,
  /* 366 */  SYSCALL_ADJTIMEX,
  /* 367 */  SYSCALL_GETCWD,
  /* 368 */  SYSCALL_CAPGET,
  /* 369 */  SYSCALL_CAPSET,
  /* 370 */  SYSCALL_SENDFILE,
  /* 371 */  SYSCALL_SETRESGID,
  /* 372 */  SYSCALL_GETRESGID,
  /* 373 */  SYSCALL_DIPC,
  /* 374 */  SYSCALL_PIVOT_ROOT,
  /* 375 */  SYSCALL_MINCORE,
  /* 376 */  SYSCALL_PCICONFIG_IOBASE,
  /* 377 */  SYSCALL_GETDENTS64,
  /* 378 */  SYSCALL_GETTID,
  /* 379 */  SYSCALL_READAHEAD,
  /* 380 */  SYSCALL_SECURITY,
  /* 381 */  SYSCALL_TKILL,
  /* 382 */  SYSCALL_SETXATTR,
  /* 383 */  SYSCALL_LSETXATTR,
  /* 384 */  SYSCALL_FSETXATTR,
  /* 385 */  SYSCALL_GETXATTR,
  /* 386 */  SYSCALL_LGETXATTR,
  /* 387 */  SYSCALL_FGETXATTR,
  /* 388 */  SYSCALL_LISTXATTR,
  /* 389 */  SYSCALL_LLISTXATTR,
  /* 390 */  SYSCALL_FLISTXATTR,
  /* 391 */  SYSCALL_REMOVEXATTR,
  /* 392 */  SYSCALL_LREMOVEXATTR,
  /* 393 */  SYSCALL_FREMOVEXATTR,
  /* 394 */  SYSCALL_FUTEX,
  /* 395 */  SYSCALL_SCHED_SETAFFINITY,
  /* 396 */  SYSCALL_SCHED_GETAFFINITY,
  /* 397 */  SYSCALL_TUXCALL,
  /* 398 */  SYSCALL_IO_SETUP,
  /* 399 */  SYSCALL_IO_DESTROY,
  /* 400 */  SYSCALL_IO_GETEVENTS,
  /* 401 */  SYSCALL_IO_SUBMIT,
  /* 402 */  SYSCALL_IO_CANCEL,
  /* 403 */  SYSCALL_UNKNOWN_403,
  /* 404 */  SYSCALL_UNKNOWN_404,
  /* 405 */  SYSCALL_EXIT_GROUP,
  /* 406 */  SYSCALL_LOOKUP_DCOOKIE,
  /* 407 */  SYSCALL_SYS_EPOLL_CREATE,
  /* 408 */  SYSCALL_SYS_EPOLL_CTL,
  /* 409 */  SYSCALL_SYS_EPOLL_WAIT,
  /* 410 */  SYSCALL_REMAP_FILE_PAGES,
  /* 411 */  SYSCALL_SET_TID_ADDRESS,
  /* 412 */  SYSCALL_RESTART_SYSCALL,
  /* 413 */  SYSCALL_FADVISE64,
  /* 414 */  SYSCALL_TIMER_CREATE,
  /* 415 */  SYSCALL_TIMER_SETTIME,
  /* 416 */  SYSCALL_TIMER_GETTIME,
  /* 417 */  SYSCALL_TIMER_GETOVERRUN,
  /* 418 */  SYSCALL_TIMER_DELETE,
  /* 419 */  SYSCALL_CLOCK_SETTIME,
  /* 420 */  SYSCALL_CLOCK_GETTIME,
  /* 421 */  SYSCALL_CLOCK_GETRES,
  /* 422 */  SYSCALL_CLOCK_NANOSLEEP,
  /* 423 */  SYSCALL_SEMTIMEDOP,
  /* 424 */  SYSCALL_TGKILL,
  /* 425 */  SYSCALL_STAT64,
  /* 426 */  SYSCALL_LSTAT64,
  /* 427 */  SYSCALL_FSTAT64,
  /* 428 */  SYSCALL_VSERVER,
  /* 429 */  SYSCALL_MBIND,
  /* 430 */  SYSCALL_GET_MEMPOLICY,
  /* 431 */  SYSCALL_SET_MEMPOLICY,
  /* 432 */  SYSCALL_MQ_OPEN,
  /* 433 */  SYSCALL_MQ_UNLINK,
  /* 434 */  SYSCALL_MQ_TIMEDSEND,
  /* 435 */  SYSCALL_MQ_TIMEDRECEIVE,
  /* 436 */  SYSCALL_MQ_NOTIFY,
  /* 437 */  SYSCALL_MQ_GETSETATTR,
  /* 438 */  SYSCALL_WAITID,
  /* 439 */  SYSCALL_ADD_KEY,
  /* 440 */  SYSCALL_REQUEST_KEY,
  /* 441 */  SYSCALL_KEYCTL
} AlphaLinuxSyscalls;

void translate_stat_buf(alpha_stat *t_buf, struct stat *h_buf)
{
  // translate host stat buf to target stat buf
  t_buf->st_dev     = h_buf->st_dev;
  t_buf->st_ino     = h_buf->st_ino;
  t_buf->st_mode    = h_buf->st_mode;
  t_buf->st_nlink   = h_buf->st_nlink;
  t_buf->st_uid     = h_buf->st_uid;
  t_buf->st_gid     = h_buf->st_gid;
  t_buf->st_rdev    = h_buf->st_rdev;
  t_buf->st_size    = h_buf->st_size;
  t_buf->st_atimeX  = h_buf->st_atime;
  t_buf->st_mtimeX  = h_buf->st_mtime;
  t_buf->st_ctimeX  = h_buf->st_ctime;
  t_buf->st_blksize = h_buf->st_blksize;
  t_buf->st_blocks  = h_buf->st_blocks;
  t_buf->st_flags   = 0;
  t_buf->st_gen     = 0;
}

/* OSF SYSCALL -- standard system call sequence
   the kernel expects arguments to be passed with the normal C calling
   sequence; v0 should contain the system call number; on return from the
   kernel mode, a3 will be 0 to indicate no error and non-zero to indicate an
   error; if an error occurred v0 will contain an errno; if the kernel return
   an error, setup a valid gp and jmp to _cerror */

/* syscall proxy handler, architect registers and memory are assumed to be
   precise when this function is called, register and memory are updated with
   the results of the sustem call */
void
sys_syscall (struct regs_t *regs,	/* registers to access */
	     mem_access_fn mem_fn,	/* generic memory accessor */
	     struct mem_t *mem,	/* memory space to access */
	     md_inst_t inst,	/* system call inst */
	     int traceable)	/* traceable system call? */
{
    qword_t syscode = regs->regs_R[MD_REG_V0];
	int Lock_id = 0;
	int a; 

#ifdef SMT_SS
#if defined(MTA)
    char *p;
#endif
    int cnt;
    int threadid = 0;
    struct mem_pte_t *tmpMemPtrD, *tmpMemPtrS, *tmpMemPtr;
    context *current;
    context *newContext;

    threadid = regs->threadid;
    current = thecontexts[threadid];
#endif

    /* fix for syscall() which uses CALL_PAL CALLSYS for making system calls */
    if (syscode == SYSCALL_OSF_SYSCALL)
	syscode = regs->regs_R[MD_REG_A0];

    /* first, check if an EIO trace is being consumed... */
#ifdef SMT_SS
    if (traceable && current->sim_eio_fd != NULL)
    {
	printf ("PROBLEM\n");
	fflush (stdout);
	eio_read_trace (current->sim_eio_fd, sim_num_insn, regs, mem_fn, mem, inst);
#else
    if (traceable && sim_eio_fd != NULL)
    {
	eio_read_trace (sim_eio_fd, sim_num_insn, regs, mem_fn, mem, inst);
#endif

	/* kludge fix for sigreturn(), it modifies all registers */
//	if (syscode == OSF_SYS_sigreturn)
//	{
//	    int i;
//	    struct osf_sigcontext sc;
//	    md_addr_t sc_addr = regs->regs_R[MD_REG_A0];
//
//	    mem_bcopy (mem_fn, mem, Read, sc_addr, &sc, sizeof (struct osf_sigcontext), current->id);
//	    regs->regs_NPC = sc.sc_pc;
//	    for (i = 0; i < 32; ++i)
//		regs->regs_R[i] = sc.sc_regs[i];
//	    for (i = 0; i < 32; ++i)
//		regs->regs_F.q[i] = sc.sc_fpregs[i];
//	    regs->regs_C.fpcr = sc.sc_fpcr;
//	}

    if (syscode == SYSCALL_SIGRETURN)
	{
	  int i;
	  struct osf_sigcontext sc;
	  md_addr_t sc_addr = regs->regs_R[MD_REG_A0];

	  mem_bcopy(mem_fn, mem, Read, sc_addr, 
		    &sc, sizeof(struct osf_sigcontext), current->id);
	  regs->regs_NPC = sc.sc_pc;
	  for (i=0; i < 32; ++i)
	    regs->regs_R[i] = sc.sc_regs[i];
	  for (i=0; i < 32; ++i)
	    regs->regs_F.q[i] = sc.sc_fpregs[i];
	  regs->regs_C.fpcr = sc.sc_fpcr;
          program_complete = 1;
	}

	/* fini... */
	printf ("Am I coming here\n");
	return;
    }

    /* no, OK execute the live system call... */
  // integer register 0 contains the system call code on alpha
  uint64_t syscall_code = regs->regs_R[R_V0];
  // default return value of 0
  int64_t return_value = 0;
  // default success value of false
  bool success = false;
    switch (syscode)
    {
    case SYSCALL_EXIT:  // 1
	/* exit jumps to the target set in main() */

        /* We don not halt on lead threads. */
        if(current->masterid != 0)
            break;
	printf ("In OSF_SYS_exit\n");
        printf ("[INFO]: sim_num_insn=%lld, barriers=%d, locks=%d\n", sim_num_insn, TotalBarriers, TotalLocks);
        fprintf (stderr, "[INFO]: sim_num_insn=%lld, barriers=%d, locks=%d\n", sim_num_insn, TotalBarriers, TotalLocks);
        fflush (stderr);
        fflush (stdout);
#ifdef DO_COMPLETE_FASTFWD
#ifdef  CACHE_PROFILE
	CacheProfWrite ();
#endif
#ifdef  BRANCH_PROFILE
	BranchProfWrite ();
#endif
#ifdef  BRPRED_PROFILE
	BranchProfWrite ();
#endif
#ifdef  STORE_PROFILE
	StoreProfWrite ();
#endif
#ifdef  INDIRECTJUMP_PROFILE
	printJumpStats ();
#endif
#ifdef  FREQ_PROFILE
	FrequencyWrite ();
#endif
#endif
	timeToReturn = 1;

	longjmp (sim_exit_buf,
		 /* exitcode + fudge */ (regs->regs_R[MD_REG_A0] & 0xff) + 1);
	printf ("Am I coming here\n");
	break;

    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_READ: // 3
      {
        int64_t fd = regs->regs_R[R_A0];
        uint64_t vaddr = regs->regs_R[R_A1];
        int64_t count = regs->regs_R[R_A2];
        int64_t len;
        char *buf;

        // alllocate a buffer
        buf = (char *)calloc(count, sizeof(char));
        if (!buf) fatal("ArchLib:  syscall read ran out of memory");

        // perform the read
        len = read(fd, buf, count);

        // copy the result to the simulator
        if (len > 0)
        {
 	  mem_bcopy(mem_fn, mem, Write,
		  /*buf*/vaddr, buf, /*nread*/len, current->id);
//          memory->bcopy(true, vaddr, buf, len);
          return_value = len;
          success = true;
        }
        else if (len == 0) // eof
        {
          return_value = 0;
          success = true;
        }
        else
        {
          return_value = errno;
          warn("error reading %d", errno);
        }

        // free the buffer
        free(buf);
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_WRITE: // 4
      {
        int64_t fd = regs->regs_R[R_A0];
        uint64_t vaddr = regs->regs_R[R_A1];
        int64_t count = regs->regs_R[R_A2];
        int64_t len;
        char *buf;

        // allocate a buffer
        buf = (char *)calloc(count, sizeof(char));
        if (!buf) fatal("ArchLib:  syscall read ran out of memory");

        // copy the buffer from the simulator
 	mem_bcopy(mem_fn, mem, Read,
		  /*buf*/vaddr, buf, /*nread*/count, current->id);
//        memory->bcopy(false, vaddr, buf, count);

        // write the buffer out to the fd
        len = write(fd, buf, count);
        fsync(fd);

        // set the result
        if (len >= 0)
        {
          return_value = len;
          success = true;
        }
        else
          return_value = errno;

        // free the buffer
        free(buf);
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_CLOSE: // 6
      {
        int res;

        // close file descriptors > 2 (sim uses 0, 1, and 2)
        if (regs->regs_R[R_A0] > 2)
        {
          res = close(regs->regs_R[R_A0]);
          if (res != (int64_t)-1)
          {
            return_value = 0;
            success = true;
          }
          else
            return_value = errno;
        }
        else
        {
          return_value = 0;
          success = true;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_UNLINK: // 10
      {
        char pathname[SYSCALL_BUFFER_SIZE];
        uint64_t pathname_ptr = regs->regs_R[R_A0];
        int64_t result;

        // read pathname out of virtual memory
	mem_strcpy(mem_fn, mem, Read, /*fname*/pathname_ptr, pathname, current->id);
//        memory->strcpy(false, pathname_ptr, pathname);

        result = unlink(pathname);
        if (result == -1)
          return_value = errno;
        else
        {
          return_value = 0;
          success = true;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_BRK: // 17
      {

	sqword_t delta;
	md_addr_t addr;

	delta = regs->regs_R[MD_REG_A0];
//	addr = ld_brk_point + delta;
	if (!delta)
		addr = _system.brk_point;
	else 
		addr = delta;

	if (verbose)
	  myfprintf(stderr, "SYS_sbrk: delta: 0x%012p (%ld)\n", delta, delta);

	ld_brk_point = addr;
	_system.brk_point = addr;
	regs->regs_R[MD_REG_V0] = ld_brk_point;
	regs->regs_R[MD_REG_A3] = 0;

	if (verbose)
	  myfprintf(stderr, "ld_brk_point: 0x%012p\n", ld_brk_point);

	return_value = addr;
	success = true;


#if 0
        md_addr_t addr;

        /* round the new heap pointer to the its page boundary */
#if 0
        addr = ROUND_UP(/*base*/regs->regs_R[MD_REG_A0], MD_PAGE_SIZE);
#endif
        addr = /*base*/regs->regs_R[MD_REG_A0];

	if (verbose)
	  myfprintf(stderr, "SYS_obreak: addr: 0x%012p\n", addr);

	ld_brk_point = addr;
	regs->regs_R[MD_REG_V0] = ld_brk_point;
	regs->regs_R[MD_REG_A3] = 0;

	if (verbose)
	  myfprintf(stderr, "ld_brk_point: 0x%012p\n", ld_brk_point);
*/
#endif


/*	if (regs->regs_R[R_A0] == 0)
        {
          return_value = _system.brk_point;
          ld_brk_point = return_value;
	  success = true;
        }
        else
        {
          //ld_brk_point
          _system.brk_point = regs->regs_R[R_A0];
          return_value = _system.brk_point;
          ld_brk_point = return_value;
          success = true;
        }*/
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_LSEEK: // 19
      {
        int64_t fd = regs->regs_R[R_A0];
        uint64_t offset = regs->regs_R[R_A1];
        int64_t whence = regs->regs_R[R_A2];
        off_t result;

        result = lseek(fd, offset, whence);

        if (result == (off_t)-1)
          return_value = errno;
        else
        {
          return_value = result;
          success = true;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_GETXPID: // 20
      {
        // get our pid and write it to the result register
        // this function is always successful
        return_value = getpid();
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_GETXUID: // 24
      {
        // uid is returned, euid goes in r20 (R_A4)
        regs->regs_R[R_A4] = (uint64_t)geteuid();
        return_value = (uint64_t)getuid();
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_KILL: // 37
      {
        // this means something has gone wrong, almost always
        warn("syscall_kill pid %i\n", regs[R_A0]);
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_OPEN: // 45
      {
        char pathname[SYSCALL_BUFFER_SIZE];
        uint64_t pathname_ptr = regs->regs_R[R_A0];
        uint64_t sim_flags = regs->regs_R[R_A1];
        uint64_t mode = regs->regs_R[R_A2];
        int local_flags;
        int fd;
        
        // read pathname out of virtual memory
	mem_strcpy(mem_fn, mem, Read, /*fname*/pathname_ptr, pathname, current->id);
//        memory->strcpy(false, pathname_ptr, pathname);

        // decode sim open flags to local open flags
        local_flags = 0;
        if (sim_flags & ALPHA_O_RDONLY)   local_flags |= O_RDONLY;
        if (sim_flags & ALPHA_O_WRONLY)   local_flags |= O_WRONLY;
        if (sim_flags & ALPHA_O_RDWR)     local_flags |= O_RDWR;
        if (sim_flags & ALPHA_O_NONBLOCK) local_flags |= O_NONBLOCK;
        if (sim_flags & ALPHA_O_APPEND)   local_flags |= O_APPEND;
        if (sim_flags & ALPHA_O_CREAT)    local_flags |= O_CREAT;
        if (sim_flags & ALPHA_O_TRUNC)    local_flags |= O_TRUNC;
        if (sim_flags & ALPHA_O_EXCL)     local_flags |= O_EXCL;
        if (sim_flags & ALPHA_O_NOCTTY)   local_flags |= O_NOCTTY;
        if (sim_flags & ALPHA_O_SYNC)     local_flags |= O_SYNC;
        if (sim_flags & ALPHA_O_DSYNC)    local_flags |= O_DSYNC;
        if (sim_flags & ALPHA_O_RSYNC)    local_flags |= O_RSYNC;

        // open the file
        fd = open(pathname, local_flags, mode);

        if (fd == -1) // return error condition
          return_value = errno;
        else // return file descriptor
        {
          return_value = fd;
          success = true;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_GETXGID: // 47
      {
        // gid is returned, egid goes in r20 (R_A4)
        regs->regs_R[R_A4] = (uint64_t)getegid();
        return_value = (uint64_t)getgid();
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_OSF_SIGPROCMASK: // 48
      {
        // ignore this syscall
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_IOCTL: // 54
      {
        // this is normally a program trying to determine if stdout is a tty
        // if so, tell the program that it's not a tty, so the program does
        // block buffering
        int64_t fd = regs->regs_R[R_A0];
        int64_t request = regs->regs_R[R_A1];

        if (fd < 0)
          return_value = EBADF;
        else
        {
          switch (request)
          {
            case 0x40067408: //ALPHA_IOCTL_TIOCGETP:
            case 0x80067409: //ALPHA_IOCTL_TIOCSETP:
            case 0x8006740a: //ALPHA_IOCTL_TIOCSETN:
            case 0x80067411: //ALPHA_IOCTL_TIOCSETC:
            case 0x40067412: //ALPHA_IOCTL_TIOCGETC:
            case 0x2000745e: //ALPHA_IOCTL_TIOCISATTY:
            case 0x402c7413: //ALPHA_IOCTL_TIOCGETS:
            case 0x40127417: //ALPHA_IOCTL_TIOCGETA:
              return_value = ENOTTY;
              break;

            default:
              fatal("unsupported ioctl call: %x on fd: %d pc: 0x%llx",
                  request, fd, 0);
          }
        }
      }
      break;
      /////////////////////////////////////////////////////////////////////////
    case SYSCALL_MMAP: // 71
      {
//        printf("SYSCALL_MMAP\n");
//        fflush(stdout);

        uint64_t addr = regs->regs_R[R_A0];
        uint64_t length = regs->regs_R[R_A1];
        int64_t flags = regs->regs_R[R_A3];
        int64_t fd = regs->regs_R[R_A4];

        // verify addr & length are both page aligned
//        if (((addr % PAGE_SIZE) != 0) ||
//            ((length % PAGE_SIZE) != 0))
//          return_value = EINVAL;
//        else
        if((length % PAGE_SIZE) != 0)
            length = PAGE_SIZE * (length/PAGE_SIZE + 1);
        {
//          warn("mmap ignorning suggested map address");
          if (addr != 0)
            warn("mmap ignorning suggested map address");

          addr = _system.mmap_end;
          _system.mmap_end += length;

          if (!(flags & 0x10))
            warn("mmapping fd %d.  this is bad if not /dev/zero", fd); 
          return_value = addr;
          success = true;
        }
      }

//        printf("return_value = %x\n", return_value);
//        fflush(stdout);

      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_MUNMAP: // 73
      {
        // since we barely support mmap, we just pretend like whatever they
        // want to munmap is fine
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_MPROTECT: // 74
      {
        // since all threads are isolated, this is never an issue
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_FCNTL: // 92
      {
        int64_t fd = regs->regs_R[R_A0];
        int64_t cmd = regs->regs_R[R_A1];

        if (fd < 0)
          return_value = EBADF;
        else
        {
          switch (cmd)
          {
            case 0: // F_DUPFD
              return_value = EMFILE;
            case 1: // F_GETFD (get close-on-exec flag)
            case 2: // F_SETFD (set close-on-exec flag)
              return_value = 0;
              success = true;
              break;

            case 3: // F_GETFL (get file flags)
            case 4: // F_SETFL (set file flags)
              return_value = fcntl(fd, cmd);
              if (return_value != -1)
                success = true;
              else
                return_value = errno;
              break;

            case 7: // F_GETLK (get lock)
            case 8: // F_SETLK (set lock)
            case 9: // F_SETLKW (set lock and wait)
            default:
              // pretend that these all worked
              warn("ignored fcntl command %d on fd %d", cmd, fd);
              return_value = 0;
              success = true;
              break;
          }
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
/*     case SYSCALL_SOCKET: // 97
      {
        // 
        return_value = 0;
        success = true;
      }
      break;
     /////////////////////////////////////////////////////////////////////////
     case SYSCALL_CONNECT: // 98
      {
        // 
        return_value = 0;
        success = true;
      }
      break;
     /////////////////////////////////////////////////////////////////////////
     case SYSCALL_SEND: // 101
      {
        // 
        return_value = 0;
        success = true;
      }
      break;
      /////////////////////////////////////////////////////////////////////////
     case SYSCALL_RECV: // 102
      {
        // 
        return_value = 0;
        success = true;
      }
      break;*/
    /////////////////////////////////////////////////////////////////////////
     case SYSCALL_WRITEV: // 121
      {
        int64_t fd = regs->regs_R[R_A0];
        uint64_t iovptr = regs->regs_R[R_A1];
        int64_t iovcnt = regs->regs_R[R_A2];
        struct iovec *iov;
        alpha_iovec a_iov;
        int i, res;

        if (fd < 0)
          return_value = EBADF;
        else
        {
	  iov = malloc(/* len */ sizeof(struct iovec)*iovcnt);
          //iov = new struct iovec[iovcnt];
          // for each io vector entry
          for (i = 0; i < iovcnt; i++)
          {
            // read a_iov out of simulator memory
	    mem_bcopy(mem_fn, mem, Read,
		  /*buf*/iovptr + i*sizeof(a_iov), &a_iov, /*nread*/sizeof(a_iov), current->id);
/*            memory->bcopy(false, 
                iovptr + i*sizeof(a_iov), 
                &a_iov, 
                sizeof(a_iov));
*/
            // copy into local iov
            iov[i].iov_len = a_iov.iov_len;
            // allocate local buffer
	    iov[i].iov_base = malloc(/* len */ sizeof(char)*(iov[i].iov_len));
            //iov[i].iov_base = new char [iov[i].iov_len];
            // copy into local buffer
 	    mem_bcopy(mem_fn, mem, Read,
		  /*buf*/a_iov.iov_base, iov[i].iov_base, /*nread*/a_iov.iov_len, current->id);
/*            memory->bcopy(false, 
                a_iov.iov_base, 
                iov[i].iov_base, 
                a_iov.iov_len);
*/          }

          // perform writev
          res = writev(fd, iov, iovcnt);

          if (res < 0)
            return_value = errno;
          else
          {
            return_value = 0;
            success = true;
          }

          // cleanup
          for (i = 0; i < iovcnt; i++)
	    free(iov[i].iov_base);
            //delete[] (char*) iov[i].iov_base;
          //delete[] iov;
	  free(iov);
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_FTRUNCATE: // 130
      {
        int fd = regs->regs_R[R_A0];
        off_t length = regs->regs_R[R_A1];

        if (fd < 0)
          return_value = EBADF;
        else
        {
          return_value = ftruncate(fd, length);
          if (return_value == -1)
            return_value = errno;
          else
            success = true;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_GETRLIMIT: // 144
      {
        uint64_t resource = regs->regs_R[R_A0];
        uint64_t vaddr = regs->regs_R[R_A1];
        alpha_rlimit arlm;

        switch (resource)
        {
          case 3: //ALPHA_RLIMIT_STACK:
            arlm.rlim_cur = 16 * 1024 * 1024; // 16MB, arbitrary
            arlm.rlim_max = arlm.rlim_cur;
            return_value = 0;
            success = true;
            break;

          case 2: //ALPHA_RLIMIT_DATA:
            arlm.rlim_cur = 1024 * 1024 * 1024; // 1GB, arbitrary
            arlm.rlim_max = arlm.rlim_cur;
            return_value = 0;
            success = true;
            break;

          default:
            warn("unimplemented rlimit resource %u ... failing ...", resource);
            break;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_SETRLIMIT: // 145
      {
        // ignore this syscall
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_OSF_GETSYSINFO: // 256
      {
        uint64_t op = regs->regs_R[R_A0];
        uint64_t vaddr;

        switch(op)
        {
          case 45:
            // This is a bad idea and was just copied from the m5 sim.
            //regs->regs_R[R_FPCR] = 0;
            vaddr = regs->regs_R[R_A1];

 	    mem_bcopy(mem_fn, mem, Write,
		  /*buf*/vaddr, &regs->regs_R[R_FPCR], /*nread*/sizeof(uint64_t), current->id);
//            memory->bcopy(true, vaddr, &regs->regs_R[R_FPCR], sizeof(uint64_t));
            return_value = 0;
            success = true;
            break;

          default:
            fatal("unsupported operation %u on system call OSF_GETSYSINFO",
                op);
            break;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_OSF_SETSYSINFO: // 257
      {
        uint64_t op = regs->regs_R[R_A0];
        uint64_t vaddr;

        switch(op)
        {
          case 14:
            // This is a bad idea and was just copied from the m5 sim.
            vaddr = regs->regs_R[R_A1];

 	    mem_bcopy(mem_fn, mem, Write,
		  /*buf*/vaddr, &(regs->regs_R[R_FPCR]), /*nread*/sizeof(uint64_t), current->id);
//            memory->bcopy(false, vaddr, &regs->regs_R[R_FPCR], sizeof(uint64_t));
            return_value = 0;
            success = true;
            break;

          default:
            fatal("unsupported operation %u on system call OSF_GETSYSINFO",
                op);
            break;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_TIMES: // 323
      {
        // ignoring
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_UNAME: // 339
      {
        alpha_utsname buf;
        uint64_t vaddr;

        strcpy(buf.sysname,  "Linux");             // modeling linux OS
        strcpy(buf.nodename, "mjdechen");          // mark's laptop
        strcpy(buf.release,  "2.6.27.5-117.fc10"); // fedora 10, why not ...
        strcpy(buf.version,  "Mon Dec  8 21:18:29 PST 2008"); // why not
        strcpy(buf.machine,  "alpha");             // modeling alpha isa
        strcpy(buf.domainname, "");                // my laptop had none

        // get buffer address from first arg reg
        vaddr = regs->regs_R[R_A0];

        // copy the buffer into virtual memory
 	mem_bcopy(mem_fn, mem, Write,
		  /*buf*/vaddr, &buf, /*nread*/sizeof(buf), current->id);
//        memory->bcopy(true, vaddr, &buf, sizeof(buf));
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_MREMAP: // 341
      {
        unsigned long int addr = regs->regs_R[R_A0];
        uint64_t old_length = regs->regs_R[R_A1];
        uint64_t new_length = regs->regs_R[R_A2];
        uint64_t flags = regs->regs_R[R_A3];

	if (new_length > old_length) {
		if ((addr + old_length) == _system.mmap_end) {
			unsigned long int diff = new_length - old_length;
			_system.mmap_end += diff;
			return_value = addr;
			success = true;
			return;
		} else {
//			if (!(flags & 1)) {
        		char *buf;
        		buf = (char *)calloc(old_length, sizeof(char));
		 	mem_bcopy(mem_fn, mem, Read,
		  		/*buf*/addr, buf, /*nread*/old_length, current->id);
		 	mem_bcopy(mem_fn, mem, Write,
		  		/*buf*/_system.mmap_end, buf, /*nread*/old_length, current->id);
			return_value = _system.mmap_end;
			_system.mmap_end += new_length;			
			//warn("can't remap here and MREMAP_MAYMOVE flag not set\n");
			success = true;
        		free(buf);
			return;
	
//			} else {
		}				
	} else {
		return_value = addr;
		success = true;
		return;
	}	
	
         // this syscall tries to expand / shrink an existing memory mapping,
        // yet we don't really have a proper memory mapping table.  so, we
        // won't handle it at all all.
        return_value = 0;
        success = false;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_RT_SIGACTION: // 352
      {
        // ignoring
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_GETTIMEOFDAY: // 359
      {
        // this is a little wonky.  if the benchmark really needs the time
        // to do something useful, then the simulated processor will appear
        // to be running slow (~1MHz).  keeping this in mind, if it becomes
        // necessary to have processors really know something useful about
        // time, the simulator should record time at the beginning of the sim
        // and then fudge all future syscalls to gettimeofday, to something
        // like (orig time + (num_insts / 6.4B))

        struct timeval tv;
        struct timezone tz;
        uint64_t tvaddr = regs->regs_R[R_A0];
        uint64_t tzaddr = regs->regs_R[R_A1];
        alpha_timeval atv;
        alpha_timezone atz;
        int64_t result;

        if (tzaddr)
          result = gettimeofday(&tv, &tz);
        else
          result = gettimeofday(&tv, 0);

        if (result == 0)
        {
          atv.tv_sec = tv.tv_sec;
          atv.tv_usec = tv.tv_usec;
 	  mem_bcopy(mem_fn, mem, Write,
		  /*buf*/tvaddr, &atv, /*nread*/sizeof(atv), current->id);
//          memory->bcopy(true, tvaddr, &atv, sizeof(atv));
          
          if (tzaddr)
          {
            atz.tz_minuteswest = tz.tz_minuteswest;
            atz.tz_dsttime = tz.tz_dsttime;
 	    mem_bcopy(mem_fn, mem, Write,
		  /*buf*/tzaddr, &atz, /*nread*/sizeof(atz), current->id);
//            memory->bcopy(true, tzaddr, &atz, sizeof(atz));
          }

          return_value = 0;
          success = true;
        }
        else
          return_value = errno;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_GETCWD: // 367
      {
        char *buf;
        uint64_t vaddr = regs->regs_R[R_A0];
        uint64_t size = regs->regs_R[R_A1];
        char *res;

        // allocate a buffer
        buf = (char *)calloc(size, sizeof(char));

        res = getcwd(buf, size);

        if (res < 0)
          return_value = errno;
        else
        {
          return_value = strlen(res);
          success = true;
	  mem_strcpy(mem_fn, mem, Write, /*fname*/vaddr, buf, current->id);
//          memory->strcpy(true, vaddr, buf);
        }

        // clean up after ourselves
        free(buf);
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_EXIT_GROUP: // 405
      {
        // set program complete flag
        program_complete = true;
        return_value = 0;
        success = true;
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_STAT64: // 425
      {
        char pathname[SYSCALL_BUFFER_SIZE];
        uint64_t pathname_ptr = regs->regs_R[R_A0];
        uint64_t vaddr = regs->regs_R[R_A1];
        alpha_stat64 alpha_buf;
        struct stat host_buf;
        int result;

        // read path out of simulator memory
	    mem_strcpy(mem_fn, mem, Read, /*fname*/pathname_ptr, pathname, current->id);

        // perform stat
        result = stat(pathname, &host_buf);
        alpha_buf.st_dev = host_buf.st_dev;
        alpha_buf.st_ino = host_buf.st_ino;
        alpha_buf.st_rdev = host_buf.st_rdev;
        alpha_buf.st_size = host_buf.st_size;
        alpha_buf.st_blocks = host_buf.st_blocks;
        alpha_buf.st_mode = host_buf.st_mode;
        alpha_buf.st_uid = host_buf.st_uid;
        alpha_buf.st_gid = host_buf.st_gid;
        alpha_buf.st_blksize = host_buf.st_blksize;
        alpha_buf.st_nlink = host_buf.st_nlink;

//        printf("SYSCALL_STAT64 result: %d\n", result);
//        printf("host_stat: %s, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", pathname, host_buf.st_dev, host_buf.st_ino, host_buf.st_rdev, host_buf.st_size, host_buf.st_blocks, host_buf.st_mode, host_buf.st_uid, host_buf.st_gid, host_buf.st_blksize, host_buf.st_nlink);
//        printf("alpha_stat: %s, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", pathname, alpha_buf.st_dev, alpha_buf.st_ino, alpha_buf.st_rdev, alpha_buf.st_size, alpha_buf.st_blocks, alpha_buf.st_mode, alpha_buf.st_uid, alpha_buf.st_gid, alpha_buf.st_blksize, alpha_buf.st_nlink);

        if (result < 0)
        {
            return_value = errno;
	        printf("\nSYSCALL_STAT64 ERROR\n");
        }
        else
        {
 	        mem_bcopy(mem_fn, mem, Write, /*buf*/vaddr, &alpha_buf, /*nread*/sizeof(alpha_buf), current->id);
            return_value = 0;
            success = true;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_LSTAT64: // 426
      {
        char pathname[SYSCALL_BUFFER_SIZE];
        uint64_t pathname_ptr = regs->regs_R[R_A0];
        uint64_t vaddr = regs->regs_R[R_A1];
        
        struct stat host_buf;
        alpha_stat64 alpha_buf;
        int result;

        // read path out of simulator memory
	    mem_strcpy(mem_fn, mem, Read, /*fname*/pathname_ptr, pathname, current->id);

        // perform lstat

        result = stat(pathname, &host_buf);
        alpha_buf.st_dev = host_buf.st_dev;
        alpha_buf.st_ino = host_buf.st_ino;
        alpha_buf.st_rdev = host_buf.st_rdev;
        alpha_buf.st_size = host_buf.st_size;
        alpha_buf.st_blocks = host_buf.st_blocks;
        alpha_buf.st_mode = host_buf.st_mode;
        alpha_buf.st_uid = host_buf.st_uid;
        alpha_buf.st_gid = host_buf.st_gid;
        alpha_buf.st_blksize = host_buf.st_blksize;
        alpha_buf.st_nlink = host_buf.st_nlink;

//        printf("SYSCALL_LSTAT64 result: %d\n", result);
//        printf("host_stat: %s, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", pathname, host_buf.st_dev, host_buf.st_ino, host_buf.st_rdev, host_buf.st_size, host_buf.st_blocks, host_buf.st_mode, host_buf.st_uid, host_buf.st_gid, host_buf.st_blksize, host_buf.st_nlink);
//        printf("alpha_stat: %s, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", pathname, alpha_buf.st_dev, alpha_buf.st_ino, alpha_buf.st_rdev, alpha_buf.st_size, alpha_buf.st_blocks, alpha_buf.st_mode, alpha_buf.st_uid, alpha_buf.st_gid, alpha_buf.st_blksize, alpha_buf.st_nlink);

        if (result < 0)
        {
            return_value = errno;
            printf("\nSYSCALL_LSTAT64 ERROR\n");
        }
        else
        {
 	        mem_bcopy(mem_fn, mem, Write, /*buf*/vaddr, &alpha_buf, /*nread*/sizeof(alpha_buf), current->id);
            return_value = 0;
            success = true;
        }
      }
      break;
    /////////////////////////////////////////////////////////////////////////
    case SYSCALL_FSTAT64: // 427
      {
//        printf("SYSCALL_FSTAT64\n");
        int fd = regs->regs_R[R_A0];
        uint64_t vaddr = regs->regs_R[R_A1];

        alpha_stat64 alpha_buf;
        struct stat host_buf;
        int result;
        

//        printf("fd = %d\n", fd);
        if (fd < 0)
          regs->regs_R[R_V0] = EBADF;
        else
        {
            result = fstat64(fd, &host_buf);
            alpha_buf.st_dev = host_buf.st_dev;
            alpha_buf.st_ino = host_buf.st_ino;
            alpha_buf.st_rdev = host_buf.st_rdev;
            alpha_buf.st_size = host_buf.st_size;
            alpha_buf.st_blocks = host_buf.st_blocks;
            alpha_buf.st_mode = host_buf.st_mode;
            alpha_buf.st_uid = host_buf.st_uid;
            alpha_buf.st_gid = host_buf.st_gid;
            alpha_buf.st_blksize = host_buf.st_blksize;
            alpha_buf.st_nlink = host_buf.st_nlink;

//            printf("host_buf.st_size = %d\n", host_buf.st_size);
//            fflush(stdout);

//            printf("SYSCALL_FSTAT64 result: %d\n", result);
//            printf("host_stat: %d, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", fd, host_buf.st_dev, host_buf.st_ino, host_buf.st_rdev, host_buf.st_size, host_buf.st_blocks, host_buf.st_mode, host_buf.st_uid, host_buf.st_gid, host_buf.st_blksize, host_buf.st_nlink);
//            printf("alpha_stat: %d, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", fd, alpha_buf.st_dev, alpha_buf.st_ino, alpha_buf.st_rdev, alpha_buf.st_size, alpha_buf.st_blocks, alpha_buf.st_mode, alpha_buf.st_uid, alpha_buf.st_gid, alpha_buf.st_blksize, alpha_buf.st_nlink);

            if (result < 0)
            {
                return_value = errno;
                printf("\nSYSCALL_FSTAT64 ERROR\n");
            }
            else
            {
                mem_bcopy(mem_fn, mem, Write, /*buf*/vaddr, &alpha_buf, /*nread*/sizeof(alpha_buf), current->id);
                return_value = 0;
                success = true;
            }
        }
      }
      break;

    case OSF_SYS_fork:
	for (cnt = 0; cnt < numcontexts; cnt++)
	    flushWriteBuffer (cnt);
#if defined(MTA)
	/* Check for total number of threads 
	 * This need to be changed when total number of threads 
	 * can be more than the number of context supported */
	if (numcontexts >= MAXTHREADS)
	    fatal ("Total number of threads has exceeded the limit");

	/* Allocate the context */
	fprintf (stderr, "Context %d Allocating thread ID at %lld: %d\n", current->id, (numcontexts), sim_num_insn);
	printf ("Context %d Allocating thread ID: %d at %lld\n", current->id, (numcontexts), sim_num_insn);
	fflush (stderr);
	fflush (stdout);

	newContext = thecontexts[numcontexts] = (context *) calloc (1, sizeof (context));
	if (newContext == NULL)
	{
	    fprintf (stderr, "Allocation of new threads in fork failed\n");
	    exit (1);
	}

	/* Initialize the context */
	/* Not sure why we need argv */
	newContext->argv = (char **) malloc (30 * sizeof (char *));
	if (newContext->argv == NULL)
	{
	    fprintf (stderr, "Error in allocation\n");
	    exit (1);
	}

#ifdef	COLLECT_STAT_STARTUP
	thrdPerJobCnt++;
	if (thrdPerJobCnt == THREADS_PER_JOB)
	    current->fastfwd_done = 1;
	newContext->fastfwd_done = 1;
        newContext->barrierReached = 0;
        newContext->startReached = 0;
#endif
	
#ifdef	EDA
        newContext->predQueue = NULL;
        newContext->lockStatus = 0;
        newContext->boqIndex = -1;
#endif
	for (cnt = 0; cnt < 30; cnt++)
	{
	    newContext->argv[cnt] = NULL;
	    newContext->argv[cnt] = (char *) malloc (240 * sizeof (char));
	    if (newContext->argv[cnt] == NULL)
	    {
		fprintf (stderr, "Error in allocation\n");
		exit (1);
	    }
	}
	/* Allocate others */
	newContext->sim_progout = (char *) malloc (240);
	if (newContext->sim_progout == NULL)
	{
	    fprintf (stderr, "Error in allocation\n");
	    exit (1);
	}
	newContext->sim_progin = (char *) malloc (240);
	if (newContext->sim_progin == NULL)
	{
	    fprintf (stderr, "Error in allocation\n");
	    exit (1);
	}
	/* Let us first reset various parameter
	 * Can either copy from parent thread or
	 * set new value later on */
	newContext->argc = 0;
	newContext->sim_inputfd = 0;
	newContext->sim_progfd = NULL;
	/* Fast forward count is not relevant for child thread */
	newContext->fastfwd_count = 0;

	newContext->id = numcontexts;
	newContext->masterid = current->id;

	newContext->parent = current->id;

	newContext->active_this_cycle = 1;

	newContext->num_child = 0;
	current->num_child++;
	current->num_child_active++;

/*******************************/

	collectStatBarrier++;
	collectStatStop[newContext->id] = 1;

/******************************/


	newContext->actualid = ++threadForked[current->id];

	if (threadForked[current->id] == (THREADS_PER_JOB-1))
	    collectStatStop[current->id] = 1;

	/* Argument file remains same for child thread */
	newContext->argfile = current->argfile;
	memcpy (newContext->sim_progout, current->sim_progout, 240);
	memcpy (newContext->sim_progin, current->sim_progin, 240);
	/* Output file also remains same. It is the responsibility of 
	 * the application to differentiate the output from different
	 * threads */


	newContext->sim_progfd = current->sim_progfd;
	//      newContext->sim_progfd = fopen(strcat(current->sim_progout, "1"), "w");
	/* Input File remains same */
	newContext->sim_inputfd = current->sim_inputfd;

	/* I should not need fname. To check, set fname to null */
	newContext->fname = NULL;
	for (cnt = 0; cnt < 30; cnt++)
	{
	    if (current->argv[cnt])
	    {
		memcpy (newContext->argv[cnt], current->argv[cnt], 240);
	    }
	}
	newContext->argc = current->argc;

	/* Initialize the registers
	 * Copy the file from the parent thread */
	memcpy (&newContext->regs, &current->regs, sizeof (struct regs_t));
	/* Change the thread id */
	newContext->regs.threadid = numcontexts;

#if PROCESS_MODEL
	/* For the process model, we need to create a duplicate of mem */
	newContext->mem = (struct mem_t *) calloc (sizeof (struct mem_t), 1);
	newContext->mem->threadid = numcontexts;
	newContext->mem->page_count = current->mem->page_count;
	newContext->mem->ptab_misses = current->mem->ptab_misses;
	newContext->mem->ptab_accesses = current->mem->ptab_accesses;

	/* name is not a useful term. So just point it to the original
	 * name */
	newContext->mem->name = current->mem->name;
	for (cnt = 0; cnt < MEM_PTAB_SIZE; cnt++)
	{
	    if (current->mem->ptab[cnt] != NULL)
	    {
		/* Allocate first page table in the bucket */
		newContext->mem->ptab[cnt] = (struct mem_pte_t *) calloc (sizeof (struct mem_pte_t), 1);
		newContext->mem->ptab[cnt]->tag = current->mem->ptab[cnt]->tag;
		/* Allocate and copy page */
		newContext->mem->ptab[cnt]->page = (byte_t *) calloc (MD_PAGE_SIZE, 1);
		memcpy (newContext->mem->ptab[cnt]->page, current->mem->ptab[cnt]->page, MD_PAGE_SIZE);
		/* Allocate other page tables in the same bucket */
		tmpMemPtrS = current->mem->ptab[cnt]->next;
		tmpMemPtrD = newContext->mem->ptab[cnt];
		while (tmpMemPtrS != NULL)
		{
		    tmpMemPtr = (struct mem_pte_t *) calloc (sizeof (struct mem_pte_t), 1);
		    tmpMemPtr->tag = tmpMemPtrS->tag;
		    tmpMemPtr->page = (byte_t *) calloc (MD_PAGE_SIZE, 1);
		    tmpMemPtr->next = NULL;
		    memcpy (tmpMemPtr->page, tmpMemPtrS->page, MD_PAGE_SIZE);
		    tmpMemPtrD->next = tmpMemPtr;
		    tmpMemPtrD = tmpMemPtr;
		    tmpMemPtrS = tmpMemPtrS->next;
		}
	    }
	}
#else
	/* Memory remains Same. So just point to the memory space
	 * of the parent thread */
	newContext->mem = current->mem;
#endif
	/* Thread id in the mem structure remains as that of the parent id */

	newContext->ld_target_big_endian = current->ld_target_big_endian;

	newContext->ld_text_base = current->ld_text_base;
	newContext->ld_text_size = current->ld_text_size;
	newContext->ld_prog_entry = current->ld_prog_entry;
	newContext->ld_data_base = current->ld_data_base;
	newContext->ld_data_size = current->ld_data_size;
	newContext->regs.regs_R[MD_REG_GP] = current->regs.regs_R[MD_REG_GP];

	//              newContext->mem = current->mem;
	newContext->sim_swap_bytes = current->sim_swap_bytes;
	newContext->sim_swap_words = current->sim_swap_words;

	/* Set the stack. The maximum size of stack is assumed
	 * to be 0x80000(STACKSIZE). The stack is allocated above 
	 * text segment. The adress at which the stack can be allocated
	 * is kept in stack_base */


#if PROCESS_MODEL
	newContext->ld_stack_base = current->stack_base;
	newContext->stack_base = current->stack_base;
#else
	newContext->ld_stack_base = current->stack_base;
	current->stack_base = current->stack_base - STACKSIZE;
	newContext->stack_base = current->stack_base;
	/* Copy the stack from the parent to the child */
	/* Need to use mem_access function */
	/* Copy the data into the simulator memory */
	p = calloc (STACKSIZE, sizeof (char));
	mem_bcopy (mem_access, current->mem, Read, current->ld_stack_base - STACKSIZE, p, STACKSIZE, current->id);
	mem_bcopy (mem_access, current->mem, Write, newContext->ld_stack_base - STACKSIZE, p, STACKSIZE, current->id);
#endif

	/* Need to make sure that following three are being
	 * initialized properly */
	newContext->ld_stack_size = current->ld_stack_size;
	newContext->ld_environ_base = newContext->ld_environ_base - STACKSIZE;  // bug? @fanglei
	newContext->ld_brk_point = current->ld_brk_point;
#if PROCESS_MODEL
	newContext->regs.regs_R[MD_REG_SP] = current->regs.regs_R[MD_REG_SP];
#else
	newContext->regs.regs_R[MD_REG_SP] = current->regs.regs_R[MD_REG_SP] - current->ld_stack_base + newContext->ld_stack_base;
#endif
	newContext->regs.regs_PC = current->regs.regs_PC;
	//      newContext->ld_stack_min = newContext->regs.regs_R[MD_REG_SP];
	newContext->ld_stack_min = newContext->ld_stack_base;

	/* Just to make sure that EIO interfaces are not being used */
	newContext->sim_eio_fname = NULL;
	newContext->sim_chkpt_fname = NULL;
	newContext->sim_eio_fd = NULL;
	newContext->ld_prog_fname = NULL;


	/* Initialize various counters */
	newContext->rename_access = 0;
	newContext->rob1_access = 0;
	newContext->rob2_access = 0;
	newContext->sim_num_insn = 0;
	newContext->sim_total_insn = 0;
	newContext->fetch_num_insn = 0;
	newContext->fetch_total_insn = 0;
	newContext->sim_num_refs = 0;
	newContext->sim_total_refs = 0;
	newContext->sim_num_loads = 0;
	newContext->sim_total_loads = 0;
	newContext->sim_num_branches = 0;
	newContext->sim_total_branches = 0;
	newContext->sim_invalid_addrs = 0;
	newContext->inst_seq = 0;
	newContext->ptrace_seq = 0;
	newContext->ruu_fetch_issue_delay = 0;
	newContext->wait_for_fetch = 0;
#ifdef	CACHE_MISS_STAT
	newContext->spec_rdb_miss_count = 0;
	newContext->spec_wrb_miss_count = 0;
	newContext->non_spec_rdb_miss_count = 0;
	newContext->non_spec_wrb_miss_count = 0;
	newContext->inst_miss_count = 0;
	newContext->load_miss_count = 0;
	newContext->store_miss_count = 0;
#endif

	newContext->fetch_num_thrd = 0;
	newContext->iissueq_thrd = 0;
	newContext->fissueq_thrd = 0;
	newContext->icount_thrd = 0;
	newContext->last_op.next = NULL;
	newContext->last_op.rs = NULL;
	newContext->last_op.tag = 0;
	/* Not sure what should be this value */
	newContext->fetch_redirected = FALSE;
	newContext->bucket_free_list = NULL;
	newContext->start_cycle = sim_cycle;
	newContext->stallThread = 0;
	newContext->waitFor = 0;
	newContext->WBFull = 0;
	newContext->waitForBranchResolve = 0;
	newContext->NRLocalHitsLoad = 0;
	newContext->NRRemoteHitsLoad = 0;
	newContext->NRMissLoad = 0;
	newContext->NRLocalHitsStore = 0;
	newContext->NRRemoteHitsStore = 0;
	newContext->NRMissStore = 0;
	newContext->present = 0;
//              for(cnt = 0; cnt < L0BUFFERSIZE; cnt++)
//              {
//                  newContext->L0_Buffer[cnt].addr = 0;
//                  newContext->L0_Buffer[cnt].valid = 0;
//                  newContext->L0_Buffer[cnt].cntr = 0;
//              }

/*		for(cnt = 0; cnt < DATALOCATORSIZE; cnt++)
		{
		    newContext->DataLocator[cnt].addr = 0;
		    newContext->DataLocator[cnt].where = -1;
		    newContext->DataLocator[cnt].valid = 0;
		    
		}*/
	newContext->DataLocatorHit = 0;
	newContext->DataLocatorMiss = 0;


	newContext->spec_mode = current->spec_mode;
	/* System Call is always made in non-speculative mode.
	 * Therefore store_htable would be null.
	 * Also need not create bucker_free_list. This is done in spec_mem_access
	 * Also register bitmasks are not required */
	BITMAP_CLEAR_MAP (newContext->use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP (newContext->use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP (newContext->use_spec_C, C_BMAP_SZ);
	for (cnt = 0; cnt < STORE_HASH_SIZE; cnt++)
	{
	    newContext->store_htable[cnt] = NULL;
	}

	for (cnt = 0; cnt < MD_TOTAL_REGS; cnt++)
	{
	    newContext->create_vector[cnt] = CVLINK_NULL;
	    newContext->create_vector_rt[cnt] = 0;
	    newContext->spec_create_vector[cnt] = CVLINK_NULL;
	    newContext->spec_create_vector_rt[cnt] = 0;
	}

	BITMAP_CLEAR_MAP (newContext->use_spec_cv, CV_BMAP_SZ);

	/* Allocate the RUU */
	newContext->RUU = calloc (RUU_size, sizeof (struct RUU_station));
	/* Initialize each RUU entry */
	for (cnt = 0; cnt < RUU_size; cnt++)
	    newContext->RUU[cnt].index = cnt;
	newContext->RUU_num = 0;
	newContext->RUU_head = newContext->RUU_tail = 0;
	newContext->RUU_count_thrd = 0;
	newContext->RUU_fcount_thrd = 0;

	newContext->numDL1CacheAccess = 0;
	newContext->numLocalHits = 0;
	newContext->numRemoteHits = 0;
	newContext->numMemAccess = 0;
	newContext->numReadCacheAccess = 0;
	newContext->numWriteCacheAccess = 0;
	newContext->numInvalidations = 0;
	newContext->numDL1Hits = 0;
	newContext->numDL1Misses = 0;
	newContext->numInsn = 0;
	newContext->numInstrBarrier = 0;
	newContext->numCycleBarrier = 0;
	newContext->totalBarrierInstr = 0;
	newContext->totalBarrierCycle = 0;

	newContext->TotalnumDL1CacheAccess = 0;
	newContext->TotalnumLocalHits = 0;
	newContext->TotalnumRemoteHits = 0;
	newContext->TotalnumMemAccess = 0;
	newContext->TotalnumReadCacheAccess = 0;
	newContext->TotalnumWriteCacheAccess = 0;
	newContext->TotalnumInvalidations = 0;
	newContext->TotalnumDL1Hits = 0;
	newContext->TotalnumDL1Misses = 0;
	newContext->TotalnumInsn = 0;
	newContext->freeze = 0;
	newContext->waitForSTLC = 0;
	newContext->sleptAt = 0;



#if defined(TEMPDEBUG)
	printf ("Current->RUU_num = %d\n", current->RUU_num);
#endif
	newContext->finish_cycle = 0;
	newContext->running = 1;
	newContext->fetch_regs_PC = current->fetch_regs_PC;
	newContext->fetch_pred_PC = current->fetch_pred_PC;
	newContext->pred_PC = current->pred_PC;
	newContext->recover_PC = current->recover_PC;
	newContext->start_cycle = sim_cycle;


	newContext->fetch_pred_PC = current->regs.regs_NPC;
#ifdef DO_COMPLETE_FASTFWD
	newContext->regs.regs_PC += sizeof (md_inst_t);
	newContext->regs.regs_NPC = current->regs.regs_NPC + sizeof (md_inst_t);
#endif


	thecontexts[numcontexts] = newContext;
	numcontexts++;

	//      current->num_child++;
	for (cnt = 0; cnt < numcontexts; cnt++)
	{
	    priority[cnt] = cnt;
	    key[cnt] = 0;
	}

	for (cnt = 0; cnt < numcontexts; cnt++)
	    cache_flush (cache_dl1[cnt], sim_cycle);

	steer_init ();
	regs->regs_R[MD_REG_V0] = 0;
	newContext->regs.regs_R[MD_REG_V0] = newContext->actualid;
	regs->regs_R[MD_REG_A3] = 0;
	newContext->regs.regs_R[MD_REG_A3] = 0;
	regs->regs_R[MD_REG_A4] = 0;
	newContext->regs.regs_R[MD_REG_A4] = 0;
#endif
	ruu_init (newContext->id);
	lsq_init (newContext->id);
	fetch_init (newContext->id);
	cv_init (newContext->id);

	break;
#if defined(MTA)
    case OSF_SYS_getthreadid:
//              regs->regs_R[MD_REG_V0] = current->id;
	regs->regs_R[MD_REG_V0] = current->actualid;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_init_start:
	printf ("init_start...\n");
	break;
    case OSF_SYS_init:
	MTAcontexts = numcontexts;
	printf ("init ...%d\n", current->id);
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	//              regs->regs_R[MD_REG_A4] = 0;
	break;
    case OSF_SYS_init_complete:
	printf ("init_complete ...\n");
	break;
    case OSF_SYS_distribute:
	printf ("distribute ...\n");
	break;
    case OSF_SYS_time:
	printf ("time ...\n");
	break;
#if 0
    case OSF_SYS_barrier:
	printf ("barrier ...\n");
	break;
#endif
    case OSF_SYS_lock_acquire:
	printf ("lock_acquire ...\n");
	break;
    case OSF_SYS_lock_release:
	printf ("lock_release ...\n");
	break;
    case OSF_SYS_flag_acquire:
	printf ("flag_acquire ...\n");
	break;
    case OSF_SYS_flag_release:
	printf ("flag_release ...\n");
	break;
    case OSF_SYS_inc_flag:
	printf ("inc_flag ...\n");
	break;
    case OSF_SYS_poll_flag:
	printf ("poll_flag ...\n");
	break;
    case OSF_SYS_wait_gt_flag:
	printf ("gt_flag ...\n");
	break;
    case OSF_SYS_wait_lt_flag:
	printf ("lt_flag ...\n");
	break;
	//      case   OSF_SYS_malloc:
	//             printf("malloc ...\n");  
	//              break;
    case OSF_SYS_printf:
//      fprintf(stderr, "printf %d...%d\n", current->id, regs->regs_R[MD_REG_A0]);      
	fflush (stderr);
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_STATS:
	if(collect_stats == 0)
	{
#ifndef PARALLEL_EMUL
	    collect_stats = regs->regs_R[MD_REG_A0];
#endif
#ifdef	COLLECT_STAT_STARTUP
            current->startReached  = regs->regs_R[MD_REG_A0];
#endif
	    if (collect_stats == 1)
	    {
		printf("Initialization over: timing simulation can start now\n");
		fprintf(stderr,"Initialization over: timing simulation can start now\n");
	    	for (cnt = 0; cnt < numcontexts; cnt++)
		    thecontexts[cnt]->start_cycle = sim_cycle;
		flushImpStats = 1;
	    }
	}
	else if (collect_stats == 1)
	{
	    collect_stats = regs->regs_R[MD_REG_A0];
#ifdef	COLLECT_STAT_STARTUP
            current->startReached  = regs->regs_R[MD_REG_A0];
#endif
	    if(collect_stats == 0)
	    {
		printf("Computation over: timing simulation can stop now\n");
		fprintf(stderr,"Computation over: timing simulation can stop now\n");
		for (cnt = 0; cnt < numcontexts; cnt++)
		    thecontexts[cnt]->finish_cycle = sim_cycle;
	   	for (cnt = 0; cnt < numcontexts; cnt++)
	    	{
		    printf ("free freeze\n");
		    thecontexts[cnt]->freeze = 0;
		    thecontexts[cnt]->running = 1;
		    thecontexts[cnt]->waitForSTLC = 0;
		    flushWriteBuffer (cnt);
		}
		timeToReturn = 1;
	    }
	}
	else
	    panic ("collect_stats is invalid");
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_FASTFWD:
	fastfwd = regs->regs_R[MD_REG_A0];
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;

    case OSF_SYS_BARRIER_STATS:
	collect_barrier_stats_own[current->id] = regs->regs_R[MD_REG_A0];

	if (collect_barrier_stats_own[current->id] == 1)
	{
		collect_barrier_stats[current->id] = collect_barrier_stats_own[current->id];
	    printf("Entering barrier %d\n", current->id);
	    if(current->fastfwd_done == 0)
	    	current->fastfwd_done = 1;
//        fflush(outFile);            // flush recoded PC
	    fflush(stdout);
	    current->numInstrBarrier = current->numInsn;
	    current->numCycleBarrier = sim_cycle;
	}
	else if (collect_barrier_stats_own[current->id] == 0)
	{
		collect_barrier_stats[current->id] = collect_barrier_stats_own[current->id];
	    printf("Leaving barrier %d\n", current->id);
	    fflush(stdout);
	    current->totalBarrierInstr += (current->numInsn - current->numInstrBarrier);
	    current->totalBarrierCycle += (sim_cycle - current->numCycleBarrier);
	}
	else if(collect_barrier_stats_own[current->id] == 2)
		collect_barrier_release = 2;
	else
		barrier_addr = regs->regs_R[MD_REG_A0];
		
/*	else if(collect_barrier_stats_own[current->id] >= 100 && collect_barrier_stats_own[current->id]<1000)
	    printf("generate current %d\n", collect_barrier_stats_own[current->id]-100);
	else 
	    printf("current %d\n", collect_barrier_stats_own[current->id]-1000);
*/
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_LOCK_STATS:
	collect_lock_stats[current->id] = regs->regs_R[MD_REG_A0];
	if (regs->regs_R[MD_REG_A0] == 1)
	{
	    current->numInstrLock = current->numInsn;
	    current->numCycleLock = sim_cycle;
		if(LockInitPC == 0)
		{
			LockInitPC = current->regs.regs_PC;
			printf("initialized pc for lock is %llx\n", LockInitPC);
		}
	}
	else if (regs->regs_R[MD_REG_A0] == 0)
	{
	    TotalLocks++;
	    current->totalLockInstr += (current->numInsn - current->numInstrLock);
	    current->totalLockCycle += (sim_cycle - current->numCycleLock);
		lock_waiting[current->id] = 0;
	}
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_WAIT:
	/* If  the thread is a child,
	   a) Print the stats
	   b) Stop it.
	   c) Reduce the number of contexts.
	   d) Update the steering list
	   e) return 0;
	   If it is a parent
	   a) If the number of threads is 0, return 1
	   else return -1
	 */
	if (current->parent != -1)
	{
	    fprintf (stderr, "***************Thread %d is halting**************\n", current->id);
	    fprintf (stderr, "TotBarCycle = %8ld, TotBarInstr = %8ld\n", (unsigned long) current->totalBarrierCycle, (unsigned long) current->totalBarrierInstr);
	    fprintf (stderr, "TotLockCycle = %8ld, TotLockInstr = %8ld\n", (unsigned long) current->totalLockCycle, (unsigned long) current->totalLockInstr);
	    print_stats (current);
	    //    general_stat(current->id);
	    MTA_printInfo (current->id);
	    current->running = 0;
	    current->freeze = 1;
	    printf ("freeze by WAIT: %d\n", current->id);
	    /* For the time being do'nt flush the thread */
	    //numcontexts--;
	    regs->regs_R[MD_REG_V0] = 0;
	    regs->regs_R[MD_REG_A3] = 0;
	    thecontexts[current->parent]->num_child--;
	}
	else
	{
	    if (current->num_child == 0)
	    {
		fprintf (stderr, "***************Thread %d is halting**************\n", current->id);
		fprintf (stderr, "TotBarCycle = %10ld, TotBarInstr = %10ld\n", (unsigned long) current->totalBarrierCycle, (unsigned long) current->totalBarrierInstr);
		fprintf (stderr, "TotLockCycle = %10ld, TotLockInstr = %10ld\n", (unsigned long) current->totalLockCycle, (unsigned long) current->totalLockInstr);
		/* Need to correct this */
		print_stats (current);
		//      general_stat(current->id);
		MTA_printInfo (current->id);
		regs->regs_R[MD_REG_V0] = 1;
		regs->regs_R[MD_REG_A3] = 0;
	    }
	    else
	    {
		regs->regs_R[MD_REG_V0] = -1;
		regs->regs_R[MD_REG_A3] = 0;
	    }

	}
	break;
    case OSF_SYS_MTA_halt:
	current->running = 0;
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;

    case OSF_SYS_MTA_start:
	printf ("Starting threads\n");
	for (cnt = 0; cnt < numcontexts; cnt++)
	    thecontexts[cnt]->running = 1;
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_barrier:
	if (current->parent == -1)
	{
	    tmpNum = numThreads[current->id] = numThreads[current->id] - 1;
	}
	else
	{
	    tmpNum = numThreads[current->parent] = numThreads[current->parent] - 1;
	}

	if (tmpNum == 0)
	{
	    fprintf (stderr, "BARRIER\n");
	    for (cnt = 0; cnt < numcontexts; cnt++)
	    {
		thecontexts[cnt]->running = 1;
	    }
	    if (current->parent == -1)
	    {
		numThreads[current->id] = mta_maxthreads;
	    }
	    else
	    {
		numThreads[current->parent] = mta_maxthreads;
	    }
	    if (startTakingStatistics == 1)
		currentPhase = 7;
	    startTakingStatistics = 1;
	}
	else
	{
	    current->running = 0;
	}
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;

	break;
    case OSF_SYS_BARRIER_INSTR:
#ifdef	COLLECT_STAT_STARTUP
        current->barrierReached = 2;
#endif
#ifndef PARALLEL_EMUL
	if(collect_stats)
#endif
	    TotalBarriers ++;
	collect_barrier_release = 0;
	if (startTakingStatistics == 1)
	    currentPhase = 7;
	startTakingStatistics = 1;
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	int i=0;
	break;
#ifdef LOCK_OPTI
	case OSF_SYS_LOCKADDR:
	lock_tail[current->id] = 0;
	lock_addr[current->id][lock_tail[current->id]] = regs->regs_R[MD_REG_A0];
	printf("threadid %d, lock addr %u\n", current->id, regs->regs_R[MD_REG_A0]);
	break;
#endif
	case OSF_SYS_IDEAL_BAR:
#ifdef IDEAL_LOCK
		Barrier_flag[regs->regs_R[MD_REG_A0]] = 1;
		Barrier_num[barrier_temp] ++;
		if(Barrier_num[barrier_temp] == numcontexts)
		{ /* release the barrier */
			Barrier_num[barrier_temp] = 0;
			barrier_temp = (barrier_temp + 1)%MAXBARRIER;
			int i = 0;
			for(i=0;i<numcontexts;i++)
			{
				thecontexts[i]->running = 1;
				thecontexts[i]->freeze = 0;
				barrier_waiting[i] = 0;
				collect_barrier_stats[i] = 0;
			}
			if(collect_stats)
				TotalBarriers ++;
		}
		else
		{ /* waiting for the barrier */
			collect_barrier_stats[current->id] = 1;
			barrier_waiting[current->id] = 1;	
			current->running = 0;
			current->freeze = 1;
			sleepCount[current->id]++;
			current->sleptAt = sim_cycle;
			freezeCounter++;
		}
#else
#ifdef LOCK_HARD
		if(collect_stats)
			SyncOperation(barrier_temp, current->id, BAR_ACQUIRE);
		else
		{
			Barrier_flag[regs->regs_R[MD_REG_A0]] = 1;
			Barrier_num[barrier_temp] ++;
			if(Barrier_num[barrier_temp] == numcontexts)
			{ /* release the barrier */
				Barrier_num[barrier_temp] = 0;
				barrier_temp = (barrier_temp + 1)%MAXBARRIER;
				int i = 0;
				for(i=0;i<numcontexts;i++)
				{
					thecontexts[i]->running = 1;
					thecontexts[i]->freeze = 0;
					barrier_waiting[i] = 0;
					collect_barrier_stats[i] = 0;
				}
				if(collect_stats)
					TotalBarriers ++;
			}
			else
			{ /* waiting for the barrier */
				collect_barrier_stats[current->id] = 1;
				barrier_waiting[current->id] = 1;	
				current->running = 0;
				current->freeze = 1;
				sleepCount[current->id]++;
				current->sleptAt = sim_cycle;
				freezeCounter++;
			}
		}
#endif
#endif
	break;
	case OSF_SYS_IDEAL_LOCKACQ:
#ifdef IDEAL_LOCK
	/* ideal lock implementation ---- lock acquire */
		//Lock_id = (regs->regs_R[MD_REG_A0]>>4)%MAXLOCK;
		Lock_id = regs->regs_R[MD_REG_A0];
		collect_lock_stats[current->id] = 1;
		if(Lock_Acquire[Lock_id] != 0)
		{ /* waiting for the lock */
			lock_waiting[current->id] = 1;
			//current->running = 0;
			//current->freeze = 1;
			//sleepCount[current->id]++;
			//current->sleptAt = sim_cycle;
			//freezeCounter++;

			Lock_wait_id[Lock_id][Lock_tail[Lock_id]] = current->id;
			Lock_wait_num[Lock_id] ++;
			Lock_tail[Lock_id] = (Lock_tail[Lock_id]+1+MAXLOCK)%MAXLOCK;
			if(Lock_tail[Lock_id] == Lock_head[Lock_id])
				panic("Ideal lock: Lock acquire queue full\n");
		}
		else
		{ /* acquire the lock */
			Lock_Acquire[Lock_id] = current->id + 1;
		}
#else
#ifdef LOCK_HARD
	/* optical system hardware lock implementation --- lock_acquire*/
		Lock_id = regs->regs_R[MD_REG_A0];
		collect_lock_stats[current->id] = 1;
		fprintf(fp_trace, "threadid %d acq lock %d\n", current->id, Lock_id);
		SyncOperation(Lock_id, current->id, ACQUIRE);
#endif
#endif
	break;
	case OSF_SYS_IDEAL_LOCKREL:
#ifdef IDEAL_LOCK
	/* ideal lock implementation ---- lock release */
		//Lock_id = (regs->regs_R[MD_REG_A0]>>4)%MAXLOCK;
		Lock_id = regs->regs_R[MD_REG_A0];
		collect_lock_stats[current->id] = 0;
		if(Lock_Acquire[Lock_id] != current->id + 1)
			panic("Ideal lock: not the right thread release the lock!\n");
		else
		{
			/* release for the lock */
			Lock_Acquire[Lock_id] = 0;
			if(Lock_wait_num[Lock_id])
			{ /* awake other threads try to acquire the lock */
				int cnt = Lock_wait_id[Lock_id][Lock_head[Lock_id]];
				//thecontexts[cnt]->freeze = 0;
				//thecontexts[cnt]->running = 1;
				lock_waiting[cnt] = 0;

				Lock_Acquire[Lock_id] = cnt+1;
				Lock_wait_num[Lock_id] --;
				Lock_head[Lock_id] = (Lock_head[Lock_id]+1+MAXLOCK)%MAXLOCK;
			}
		}
#else
#ifdef LOCK_HARD
		Lock_id = regs->regs_R[MD_REG_A0];
		collect_lock_stats[current->id] = 0;
		SyncOperation(Lock_id, current->id, RELEASE);
		
#endif
#endif
	break;
    case OSF_SYS_QUEISCE:
#ifdef	COLLECT_STAT_STARTUP
        current->barrierReached = 1;
#endif

/*		fprintf(stderr, "Thread %d is sleeping on %llx\n", current->id, regs->regs_R[MD_REG_A0]);
		fflush(stdout);*/
	if (!current->masterid)
	{
	    current->running = 0;
	    current->freeze = 1;
	    sleepCount[current->id]++;

	    //              printf("freeze by QUEISCE: %d\t%lld\t", current->id, freezeCounter);

	    fflush (stdout);
	    quiesceAddrStruct[current->id].address = regs->regs_R[MD_REG_A0];
	    quiesceAddrStruct[current->id].threadid = current->id;
	    current->sleptAt = sim_cycle;
	    freezeCounter++;
	}
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_RANDOM:
	regs->regs_R[MD_REG_V0] = random ();
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_MEM:
	access_mem = regs->regs_R[MD_REG_A0];
	access_mem_id = current->id;
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_PRINTGP:
	printf ("%d, GP = %llx\n", current->id, current->regs.regs_R[MD_REG_GP]);
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;

	break;

    case OSF_SYS_SHDADDR:
	AddToTheSharedAddrList ((unsigned long long) regs->regs_R[MD_REG_A0], (unsigned int) regs->regs_R[MD_REG_A1]);
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_STOPSIM:
	stopsim = 1;
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
    case OSF_SYS_PRINTINSTR:
	fprintf (stderr, "%lld\n", (unsigned long long) sim_num_insn);
	fflush (stderr);
	regs->regs_R[MD_REG_V0] = 0;
	regs->regs_R[MD_REG_A3] = 0;
	break;
#endif
    default:
	warn ("invalid/unimplemented syscall %ld, PC=0x%08p, RA=0x%08p, winging it", syscode, regs->regs_PC, regs->regs_R[MD_REG_RA]);
	regs->regs_R[MD_REG_A3] = -1;
	regs->regs_R[MD_REG_V0] = 0;
#if 0
	fatal ("invalid/unimplemented system call encountered, code %d", syscode);
#endif

    }
      // write result and value into register file 
        if( 
            ( syscode < 500 )  && 
            ( syscode != 2    )  && 
            ( syscode != 162  )  && 
            ( syscode != 167  )  && 
            ( syscode != 168  )  && 
            ( syscode != 170  )  && 
            ( syscode != 171  )  && 
            ( syscode != 172  )  && 
            ( syscode != 173  )  && 
            ( syscode != 174  )  && 
            ( syscode != 175  )  && 
            ( syscode != 176  )  && 
            ( syscode != 177  )  && 
            ( syscode != 178  )  && 
            ( syscode != 179  )  && 
            ( syscode != 180  )  && 
            ( syscode != 182  )  && 
            ( syscode != 183  )  && 
            ( syscode != 185  )  && 
            ( syscode != 186  )  && 
            ( syscode != 189  )  && 
            ( syscode != 190  )  && 
            ( syscode != 191  )  && 
            ( syscode != 192  )
          )
        {
            if (success)
              regs->regs_R[R_A3] = 0;
            else
              regs->regs_R[R_A3] = -1;
            regs->regs_R[R_V0] = return_value;
        }

  //printf(" %d\n", return_value);
    
  if (verbose)
    fprintf(stderr, "syscall(%d): returned %d:%d...\n",
            (int)syscode, (int)regs->regs_R[MD_REG_A3],
            (int)regs->regs_R[MD_REG_V0]);

}

void
call_quiesce (int threadID)
{
    printf ("Quiescent instruction called by %d\n", threadID);
}

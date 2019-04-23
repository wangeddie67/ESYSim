/*
 * alpha.h
 *
 *  Created on: 2016年6月11日
 *      Author: root
 */

#ifndef ALPHA_H_
#define ALPHA_H_

#include "define.h"
#include "sim-oodef.h"
#include "alphadef.h"

/*syscall.cc*/
#ifndef MIN_SYSCALL_MODE
#if defined(_MSC_VER) || defined(__CYGWIN32__) || defined(MD_CROSS_ENDIAN)
#define MIN_SYSCALL_MODE
#endif
#endif /* !MIN_SYSCALL_MODE */

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
#if defined(__FreeBSD__)
#include <utime.h>
#include <dirent.h>
#endif
#if defined(linux)
#include <utime.h>
#include <sys/socket.h>
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
#ifdef linux
#include <sgtty.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#endif /* linux */
#ifdef __FreeBSD__
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#endif /* __FreeBSD__ */

#if defined(__svr4__)
#include <sys/dirent.h>
#include <sys/filio.h>
#elif defined(__osf__)
#include <dirent.h>
/* -- For some weird reason, getdirentries() is not declared in any
 * -- header file under /usr/include on the Alpha boxen that I tried
 * -- SS-Alpha on. But the function exists in the libraries.
 */
int getdirentries(int fd, char *buf, int nbytes, long *basep);
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




class TALPHA : public TCore{
public:

struct ecoff_filehdr {
  half_t f_magic;
  half_t f_nscns;
  sword_t f_timdat;
  qword_t f_symptr;
  sword_t f_nsyms;
  half_t f_opthdr;
  half_t f_flags;
};

struct ecoff_aouthdr {
  half_t magic;
  half_t vstamp;
  half_t bldrev;
  half_t padcell;
  qword_t tsize;
  qword_t dsize;
  qword_t bsize;
  qword_t entry;
  qword_t text_start;
  qword_t data_start;
  qword_t bss_start;
  sword_t gprmask;
  sword_t fprmask;
  qword_t gp_value;
};

struct ecoff_scnhdr {
  char s_name[8];
  qword_t s_paddr;
  qword_t s_vaddr;
  qword_t s_size;
  qword_t s_scnptr;
  qword_t s_relptr;
  qword_t s_lnnoptr;
  half_t s_nreloc;
  half_t s_nlnno;
  sword_t s_flags;
};

typedef struct ecoff_symhdr_t {
  half_t magic;
  half_t vstamp;

  sword_t ilineMax;		/* number of line number entries */
  sword_t idnMax;		/* max index into dense number table */
  sword_t ipdMax;		/* number of procedures */
  sword_t isymMax;		/* number of local symbols */
  sword_t ioptMax;		/* max index into optimization sym entries */
  sword_t iauxMax;		/* number of auxiliary symbol entries */
  sword_t issMax;		/* max index into local strings */
  sword_t issExtMax;		/* max index into external strings */
  sword_t ifdMax;		/* number of file descriptor entries */
  sword_t crfd;			/* number of relative file descriptor ents */
  sword_t iextMax;		/* max index into external symbols */
  qword_t cbLine;		/* number of bytes for line number entries */
  qword_t cbLineOffset;		/* offset to start of line number entries*/
  qword_t cbDnOffset;		/* offset to start dense number table */
  qword_t cbPdOffset;		/* offset to procedure descriptor table */
  qword_t cbSymOffset;		/* offset to start of local symbols */
  qword_t cbOptOffset;		/* offset to optimization symbol entries */
  qword_t cbAuxOffset;		/* offset to start of aux symbol entries */
  qword_t cbSsOffset;		/* offset to start of local strings */
  qword_t cbSsExtOffset;	/* offset to start of external strings */
  qword_t cbFdOffset;		/* offset to file descriptor table */
  qword_t cbRfdOffset;		/* offset to relative file descriptor table */
  qword_t cbExtOffset;		/* offset to start of ext symbol entries */
} ecoff_HDRR;

#define ECOFF_magicSym 0x1992

typedef struct ecoff_fdr {
  word_t adr;
  sword_t rss;
  sword_t issBase;
  sword_t cbSs;
  sword_t isymBase;
  sword_t csym;
  sword_t ilineBase;
  sword_t cline;
  sword_t ioptBase;
  sword_t copt;
  half_t ipdFirst;
  half_t cpd;
  sword_t iauxBase;
  sword_t caux;
  sword_t rfdBase;
  sword_t crfd;
  unsigned lang :5;
  unsigned fMerge :1;
  unsigned fReadin :1;
  unsigned fBigendian :1;
  unsigned reserved :24;
  sword_t cbLineOffset;
  sword_t cbLine;
} ecoff_FDR;

typedef struct ecoff_pdr {
  word_t adr;
  sword_t isym;
  sword_t iline;
  sword_t regmask;
  sword_t regoffset;
  sword_t iopt;
  sword_t fregmask;
  sword_t fregoffset;
  sword_t frameoffset;
  half_t framereg;
  half_t pcreg;
  sword_t lnLow;
  sword_t lnHigh;
  sword_t cbLineOffset;
} ecoff_PDR;

typedef struct ecoff_SYMR {
  qword_t value;
  sword_t iss;
  unsigned st :6;
  unsigned sc :5;
  unsigned reserved :1;
  unsigned index :20;
} ecoff_SYMR;

typedef struct ecoff_EXTR {
  ecoff_SYMR asym;
  sword_t ifd;
  sword_t reserved;
} ecoff_EXTR;

//machine.h

typedef qword_t md_addr_t;

/*loader.cc*/
#define TEXT_TAIL_PADDING 0 /* was: 128 */

//symbol.cc
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





/*typedef*/
//machine.h
typedef qword_t md_gpr_t[MD_NUM_IREGS];
typedef union {
  qword_t q[MD_NUM_FREGS];	/* integer qword view */
  dfloat_t d[MD_NUM_FREGS];	/* double-precision floating point view */
} md_fpr_t;
typedef struct {
  qword_t fpcr;			/* floating point condition codes */
  qword_t uniq;			/* process-unique register */
} md_ctrl_t;
typedef word_t md_inst_t;/* instruction formats */
typedef qword_t exo_address_t;
typedef qword_t exo_integer_t;
typedef double exo_float_t;
/*enum*/
//machine.h
enum md_fault_type {
  md_fault_none = 0,		/* no fault */
  md_fault_access,		/* storage access fault */
  md_fault_alignment,		/* storage alignment fault */
  md_fault_overflow,		/* signed arithmetic overflow fault */
  md_fault_div0,		/* division by zero fault */
  md_fault_invalid,             /* invalid arithmetic operation */
                                /* added to allow SQRT{S,T} in FIX exts */
  md_fault_break,		/* BREAK instruction fault */
  md_fault_unimpl,		/* unimplemented instruction fault */
  md_fault_internal		/* internal S/W fault */
};
/* well known registers */
enum md_reg_names {
  MD_REG_V0 = 0,	/* return value reg */
  MD_REG_ERR = 7,
  MD_REG_FP = 15,	/* frame pointer */
  MD_REG_A0 = 16,	/* argument regs */
  MD_REG_A1 = 17,
  MD_REG_A2 = 18,
  MD_REG_A3 = 19,
  MD_REG_A4 = 20,
  MD_REG_A5 = 21,
  MD_REG_RA = 26,	/* return address reg */
  MD_REG_GP = 29,	/* global data section pointer */
  MD_REG_SP = 30,	/* stack pointer */
  MD_REG_ZERO = 31	/* zero register */
};
enum md_opcode {
  OP_NA = 0,	/* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) OP,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) OP,
#define CONNECT(OP)
#include "alpha.def"
  OP_MAX	/* number of opcodes + NA */
};
enum md_opcode md_mask2op[MD_MAX_MASK+1];
/* function unit classes, update md_fu2name if you update this definition */
enum md_fu_class {
  FUClamd_NA = 0,	/* inst does not use a functional unit */
  IntALU,		/* integer ALU */
  IntMULT,		/* integer multiplier */
  IntDIV,		/* integer divider */
  FloatADD,		/* floating point adder/subtractor */
  FloatCMP,		/* floating point comparator */
  FloatCVT,		/* floating point<->integer converter */
  FloatMULT,		/* floating point multiplier */
  FloatDIV,		/* floating point divider */
  FloatSQRT,		/* floating point square root */
  RdPort,		/* memory read port */
  WrPort,		/* memory write port */
  NUM_FU_CLASSES	/* total functional unit classes */
};
/* addressing mode probe, enums and strings */
enum md_amode_type {
  md_amode_imm,		/* immediate addressing mode */
  md_amode_gp,		/* global data access through global pointer */
  md_amode_sp,		/* stack access through stack pointer */
  md_amode_fp,		/* stack access through frame pointer */
  md_amode_disp,	/* (reg + const) addressing */
  md_amode_rr,		/* (reg + reg) addressing */
  md_amode_NUM
};
char *md_amode_str[md_amode_NUM] =
{
  "(const)",		/* immediate addressing mode */
  "(gp + const)",	/* global data access through global pointer */
  "(sp + const)",	/* stack access through stack pointer */
  "(fp + const)",	/* stack access through frame pointer */
  "(reg + const)",	/* (reg + const) addressing */
  "(reg + reg)"		/* (reg + reg) addressing */
};
/* register bank specifier */
enum md_reg_type {
  rt_gpr,		/* general purpose register */
  rt_lpr,		/* integer-precision floating pointer register */
  rt_fpr,		/* single-precision floating pointer register */
  rt_dpr,		/* double-precision floating pointer register */
  rt_ctrl,		/* control register */
  rt_PC,		/* program counter */
  rt_NPC,		/* next program counter */
  rt_NUM
};
/*variables*/
//machine.h
static const md_inst_t MD_NOP_INST = 0x43ff041f;
unsigned int md_opoffset[OP_MAX];
/* enum md_opcode -> mask for decoding next level */
unsigned int md_opmask[OP_MAX] = {
  0, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) 0,
#define DEFLINK(OP,MSK,NAME,SHIFT,MASK) MASK,
#define CONNECT(OP)
#include "alpha.def"
};

/* enum md_opcode -> shift for decoding next level */
unsigned int md_opshift[OP_MAX] = {
  0, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) 0,
#define DEFLINK(OP,MSK,NAME,SHIFT,MASK) SHIFT,
#define CONNECT(OP)
#include "alpha.def"
};
/* enum md_opcode -> description string */
char *md_op2name[OP_MAX] = {
  NULL, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) NAME,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NAME,
#define CONNECT(OP)
#include "alpha.def"
};
/* enum md_opcode -> opcode operand format, used by disassembler */
char *md_op2format[OP_MAX] = {
  NULL, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) OPFORM,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NULL,
#define CONNECT(OP)
#include "alpha.def"
};
/* enum md_opcode -> enum md_fu_class, used by performance simulators */
enum md_fu_class md_op2fu[OP_MAX] = {
  FUClamd_NA, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) RES,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) FUClamd_NA,
#define CONNECT(OP)
#include "alpha.def"
};
/* enum md_opcode -> opcode flags, used by simulators */
unsigned int md_op2flags[OP_MAX] = {
  NA, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) FLAGS,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NA,
#define CONNECT(OP)
#include "alpha.def"
};


/* enum md_fu_class -> description string */
char *md_fu2name[NUM_FU_CLASSES] = {
  NULL, /* NA */
  "fu-int-ALU",
  "fu-int-multiply",
  "fu-int-divide",
  "fu-FP-add/sub",
  "fu-FP-comparison",
  "fu-FP-conversion",
  "fu-FP-multiply",
  "fu-FP-divide",
  "fu-FP-sqrt",
  "rd-port",
  "wr-port"
};
/* register name specifier */
struct md_reg_names_t {
  char *str;			/* register name */
  enum md_reg_type file;	/* register file */
  int reg;			/* register index */
};
/* symbolic register names, parser is case-insensitive */
static const struct md_reg_names_t md_reg_names[122];
/*regs.h*/
struct regs_t {
  md_gpr_t regs_R;		/* (signed) integer register file */
  md_fpr_t regs_F;		/* floating point register file */
  md_ctrl_t regs_C;		/* control register file */
  md_addr_t regs_PC;		/* program counter */
  md_addr_t regs_NPC;		/* next-cycle program counter */
};
/*memory.h*/

struct mem_pte_t {
  struct mem_pte_t *next;	/* next translation in this bucket */
  qword_t tag;		/* virtual page number tag */
  byte_t *page;			/* page pointer */
};
struct mem_t {
  /* memory object state */
  char *name;				/* name of this memory space */
  struct mem_pte_t *ptab[MEM_PTAB_SIZE];/* inverted page table */

  /* memory object stats */
  counter_t page_count;			/* total number of pages allocated */
  counter_t ptab_misses;		/* total first level page tbl misses */
  counter_t ptab_accesses;		/* total page table accesses */
};
enum mem_cmd {
  Read,			/* read memory from target (simulated program) to host */
  Write			/* write memory from host (simulator) to target */
};
typedef enum md_fault_type
(TALPHA::*mem_access_fn)(struct mem_t *mem,	/* memory space to access */
		 enum mem_cmd cmd,	/* Read or Write */
		 qword_t addr,	/* target memory address to access */
		 void *p,		/* where to copy to/from */
		 int nbytes);		/* transfer length in bytes */

/*loader.h*/
	md_addr_t ld_text_base = 0;
	unsigned int ld_text_size = 0;
	md_addr_t ld_data_base = 0;
	unsigned int ld_data_size = 0;
	md_addr_t ld_brk_point = 0;
	md_addr_t ld_stack_base = 0;
	unsigned int ld_stack_size = 0;
	md_addr_t ld_stack_min = (md_addr_t)-1;
	char *ld_prog_fname = NULL;
	md_addr_t ld_prog_entry = 0;
	md_addr_t ld_environ_base = 0;
//	int ld_target_big_endian;
/*end of loader.h*/
/*symbol.h*/
	enum sym_seg_t {
	  ss_data,			/* data segment symbol */
	  ss_text,			/* text segment symbol */
	  ss_NUM
	};
	struct sym_sym_t {
	  char *name;			/* symbol name */
	  enum sym_seg_t seg;		/* symbol segment */
	  int initialized;		/* initialized? (if data segment) */
	  int pub;			/* externally visible? */
	  int local;			/* compiler local symbol? */
	  md_addr_t addr;		/* symbol address value */
	  int size;			/* bytes to next symbol */
	};
	enum sym_db_t {
	  sdb_any,			/* search all symbols */
	  sdb_text,			/* search text symbols */
	  sdb_data,			/* search data symbols */
	  sdb_NUM
	};
/*end of symbol.h*/
/*variables in symbol.cc*/
	struct sym_sym_t *sym_db = NULL;
	int sym_nsyms = 0;
	struct sym_sym_t **sym_syms = NULL;
	struct sym_sym_t **sym_syms_by_name = NULL;
	int sym_ntextsyms = 0;
	struct sym_sym_t **sym_textsyms = NULL;
	struct sym_sym_t **sym_textsyms_by_name = NULL;
	int sym_ndatasyms = 0;
	struct sym_sym_t **sym_datasyms = NULL;
	struct sym_sym_t **sym_datasyms_by_name = NULL;
	int syms_loaded = FALSE;
/*variables in symbol.cc end*/
	/*cache.h*/
	struct cache_blk_t
	{
	  struct cache_blk_t *way_next;	/* next block in the ordered way chain, used
					   to order blocks for replacement */
	  struct cache_blk_t *way_prev;	/* previous block in the order way chain */
	  struct cache_blk_t *hash_next;/* next block in the hash bucket chain, only
					   used in highly-associative caches */
	  /* since hash table lists are typically small, there is no previous
		 pointer, deletion requires a trip through the hash table bucket list */
	  md_addr_t tag;		/* data block tag value */
	  unsigned int status;		/* block status, see CACHE_BLK_* defs above */
	  tick_t ready;		/* time when block will be accessible, field
					   is set when a miss fetch is initiated */
	  byte_t *user_data;		/* pointer to user defined data, e.g.,
					   pre-decode data or physical page address */
	  /* DATA should be pointer-aligned due to preceeding field */
	  /* NOTE: this is a variable-size tail array, this must be the LAST field
		 defined in this structure! */
	  byte_t data[1];		/* actual data block starts here, block size
					   should probably be a multiple of 8 */
	};
	struct cache_set_t
	{
	  struct cache_blk_t **hash;	/* hash table: for fast access w/assoc, NULL
					   for low-assoc caches */
	  struct cache_blk_t *way_head;	/* head of way list */
	  struct cache_blk_t *way_tail;	/* tail pf way list */
	  struct cache_blk_t *blks;	/* cache blocks, allocated sequentially, so
					   this pointer can also be used for random
					   access to cache blocks */
	};
	struct cache_t
	{
	  /* parameters */
	  char *name;			/* cache name */
	  int nsets;			/* number of sets */
	  int bsize;			/* block size in bytes */
	  int balloc;			/* maintain cache contents? */
	  int usize;			/* user allocated data size */
	  int assoc;			/* cache associativity */
	  enum TCore::cache_policy policy;	/* cache replacement policy */
	  unsigned int hit_latency;	/* cache hit latency */

	  /* miss/replacement handler, read/write BSIZE bytes starting at BADDR
		 from/into cache block BLK, returns the latency of the operation
		 if initiated at NOW, returned latencies indicate how long it takes
		 for the cache access to continue (e.g., fill a write buffer), the
		 miss/repl functions are required to track how this operation will
		 effect the latency of later operations (e.g., write buffer fills),
		 if !BALLOC, then just return the latency; BLK_ACCESS_FN is also
		 responsible for generating any user data and incorporating the latency
		 of that operation */
	  unsigned int					/* latency of block access */
		(TALPHA::*blk_access_fn)(enum mem_cmd cmd,		/* block access command */
				 md_addr_t baddr,		/* program address to access */
				 int bsize,			/* size of the cache block */
				 struct cache_blk_t *blk,	/* ptr to cache block struct */
				 tick_t now);		/* when fetch was initiated */

	  /* derived data, for fast decoding */
	  int hsize;			/* cache set hash table size */
	  md_addr_t blk_mask;
	  int set_shift;
	  md_addr_t set_mask;		/* use *after* shift */
	  int tag_shift;
	  md_addr_t tag_mask;		/* use *after* shift */
	  md_addr_t tagset_mask;	/* used for fast hit detection */

	  /* bus resource */
	  tick_t bus_free;		/* time when bus to next level of cache is
					   free, NOTE: the bus model assumes only a
					   single, fully-pipelined port to the next
					   level of memory that requires the bus only
					   one cycle for cache line transfer (the
					   latency of the access to the lower level
					   may be more than one cycle, as specified
					   by the miss handler */

	  /* per-cache stats */
	  counter_t hits;		/* total number of hits */
	  counter_t misses;		/* total number of misses */
	  counter_t replacements;	/* total number of replacements at misses */
	  counter_t writebacks;		/* total number of writebacks at misses */
	  counter_t invalidations;	/* total number of external invalidations */

	  /* last block to hit, used to optimize cache hit processing */
	  md_addr_t last_tagset;	/* tag of last line accessed */
	  struct cache_blk_t *last_blk;	/* cache block last accessed */

	  /* data blocks */
	  byte_t *data;			/* pointer to data blocks allocation */

	  /* NOTE: this is a variable-size tail array, this must be the LAST field
		 defined in this structure! */
	  struct cache_set_t sets[1];	/* each entry is a set */
	};
	enum list_loc_t { Head, Tail };
	/*end of cache.h*/

	/*bpred.h*/
	enum bpred_class {
	  BPredComb,                    /* combined predictor (McFarling) */
	  BPred2Level,			/* 2-level correlating pred w/2-bit counters */
	  BPred2bit,			/* 2-bit saturating cntr pred (dir mapped) */
	  BPredTaken,			/* static predict taken */
	  BPredNotTaken,		/* static predict not taken */
	  BPred_NUM
	};
	struct bpred_btb_ent_t {
	  md_addr_t addr;		/* address of branch being tracked */
	  enum md_opcode op;		/* opcode of branch corresp. to addr */
	  md_addr_t target;		/* last destination of branch when taken */
	  struct bpred_btb_ent_t *prev, *next; /* lru chaining pointers */
	};

	/* direction predictor def */
	struct bpred_dir_t {
	  enum bpred_class classt;	/* type of predictor */
	  union {
		struct {
		  unsigned int size;	/* number of entries in direct-mapped table */
		  unsigned char *table;	/* prediction state table */
		} bimod;
		struct {
		  int l1size;		/* level-1 size, number of history regs */
		  int l2size;		/* level-2 size, number of pred states */
		  int shift_width;		/* amount of history in level-1 shift regs */
		  int xort;			/* history xor address flag */
		  int *shiftregs;		/* level-1 history table */
		  unsigned char *l2table;	/* level-2 prediction state table */
		} two;
	  } config;
	};
	/* branch predictor def */
	struct bpred_t {
	  enum bpred_class classt;	/* type of predictor */
	  struct {
		struct bpred_dir_t *bimod;	  /* first direction predictor */
		struct bpred_dir_t *twolev;	  /* second direction predictor */
		struct bpred_dir_t *meta;	  /* meta predictor */
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
	  } retstack;

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
	  struct {		/* predicted directions */
		unsigned int ras    : 1;	/* RAS used */
		unsigned int bimod  : 1;    /* bimodal predictor */
		unsigned int twolev : 1;    /* 2-level predictor */
		unsigned int meta   : 1;    /* meta predictor (0..bimod / 1..2lev) */
	  } dir;
	};
/*end of bpred.h*/
/*syscall.cc*/
	struct  osf_statbuf
	{
	  word_t osf_st_dev;
	  word_t osf_st_ino;
	  word_t osf_st_mode;
	  half_t osf_st_nlink;
	  half_t pad0;			/* to match Alpha/AXP padding... */
	  word_t osf_st_uid;
	  word_t osf_st_gid;
	  word_t osf_st_rdev;
	  word_t pad1;			/* to match Alpha/AXP padding... */
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

	struct osf_sgttyb {
	  byte_t sg_ispeed;	/* input speed */
	  byte_t sg_ospeed;	/* output speed */
	  byte_t sg_erase;	/* erase character */
	  byte_t sg_kill;	/* kill character */
	  shalf_t sg_flags;	/* mode flags */
	};
	struct osf_sigcontext {
	  qword_t sc_onstack;              /* sigstack state to restore */
	  qword_t sc_mask;                 /* signal mask to restore */
	  qword_t sc_pc;                   /* pc at time of signal */
	  qword_t sc_ps;                   /* psl to retore */
	  qword_t sc_regs[32];             /* processor regs 0 to 31 */
	  qword_t sc_ownedfp;              /* fp has been used */
	  qword_t sc_fpregs[32];           /* fp regs 0 to 31 */
	  qword_t sc_fpcr;                 /* floating point control register */
	  qword_t sc_fp_control;           /* software fpcr */
	};

	struct osf_statfs {
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
	  sword_t osf_tv_usec;		/* microseconds */
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
	  qword_t osf_rlim_cur;		/* current (soft) limit */
	  qword_t osf_rlim_max;		/* maximum value for rlim_cur */
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
	struct osf_dirent
	{
	  word_t d_ino;			/* file number of entry */
	  half_t d_reclen;		/* length of this record */
	  half_t d_namlen;		/* length of string in d_name */
	  char d_name[256];		/* DUMMY NAME LENGTH */
					/* the real maximum length is */
					/* returned by pathconf() */
	                                /* At this time, this MUST */
	                                /* be 256 -- the kernel */
	                                /* requires it */
	};
	struct osf_flag_table_t{
	  int osf_flag;
	  int local_flag;
	};
#ifdef _MSC_VER
#ifdef _O_NONBLOCK
#ifdef _O_NOCTTY
#ifdef _O_SYNC/*_O_NONBLOCK && _O_NOCTTY && _O_SYNC*/
	#define OSF_FLAG_NUM  10//struct ss_flag_t ss_flag_table[10];
#else/*_O_NONBLOCK && _O_NOCTTY && ! _O_SYNC*/
	#define OSF_FLAG_NUM  9//struct ss_flag_t ss_flag_table[9];
#endif
#else/*_O_NONBLOCK && !_O_NOCTTY &&  _O_SYNC*/
#ifdef _O_SYNC/*_O_NONBLOCK && !_O_NOCTTY && _O_SYNC*/
	#define OSF_FLAG_NUM  9//struct ss_flag_t ss_flag_table[10];
#else/*_O_NONBLOCK && !_O_NOCTTY && ! _O_SYNC*/
	#define OSF_FLAG_NUM  8//struct ss_flag_t ss_flag_table[8];
#endif
#endif
#else /*!_O_NONBLOCK*/
#ifdef _O_NOCTTY
#ifdef _O_SYNC/*!_O_NONBLOCK && _O_NOCTTY && _O_SYNC*/
	#define OSF_FLAG_NUM  9//struct ss_flag_t ss_flag_table[9];
#else/*!_O_NONBLOCK && _O_NOCTTY && ! _O_SYNC*/
	#define OSF_FLAG_NUM  8//struct ss_flag_t ss_flag_table[8];
#endif
#else/*!_O_NONBLOCK && !_O_NOCTTY &&  _O_SYNC*/
#ifdef _O_SYNC/*_O_NONBLOCK && !_O_NOCTTY && _O_SYNC*/
	#define OSF_FLAG_NUM  8//struct ss_flag_t ss_flag_table[8];
#else/*!_O_NONBLOCK && !_O_NOCTTY && ! _O_SYNC*/
	#define OSF_FLAG_NUM  7//struct ss_flag_t ss_flag_table[7];
#endif
#endif
#endif
#else/*!_MSC_VER*/
#ifdef O_SYNC
	#define OSF_FLAG_NUM  10//struct ss_flag_t ss_flag_table[10];
#else
	#define OSF_FLAG_NUM  9//struct ss_flag_t ss_flag_table[9];
#endif
#endif
	static const struct osf_flag_table_t osf_flag_table[OSF_FLAG_NUM];
	qword_t sigmask = 0;
	qword_t sigaction_array[OSF_NSIG] =	 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	/* translate system call arguments */
	struct xlate_table_t
	{
	  int target_val;
	  int host_val;
	};
#ifdef SO_ACCEPTCONN
#ifdef SO_USELOOPBACK
#ifdef SO_REUSEPORT
	#define SOCKET_MAP_NUM  10
#elif
	#define SOCKET_MAP_NUM 9
#endif
#else
#ifdef SO_REUSEPORT
	#define SOCKET_MAP_NUM  9
#else
	#define SOCKET_MAP_NUM 8
#endif
#endif
#else
#ifdef SO_USELOOPBACK
#ifdef SO_REUSEPORT
	#define SOCKET_MAP_NUM  9
#elif
	#define SOCKET_MAP_NUM 8
#endif
#else
#ifdef SO_REUSEPORT
	#define SOCKET_MAP_NUM  8
#elif
	#define SOCKET_MAP_NUM 7
#endif
#endif
#endif

#ifdef SO_ACCEPTCONN
#ifdef SO_USELOOPBACK
#ifdef SO_REUSEPORT
#define SOCKET_MAP_NUM 10
#else
#define SOCKET_MAP_NUM 9
#endif
#else
#ifdef SO_REUSEPORT
#define SOCKET_MAP_NUM 9
#else
#define SOCKET_MAP_NUM 8
#endif
#endif
#else
#ifdef SO_USELOOPBACK
#ifdef SO_REUSEPORT
#define SOCKET_MAP_NUM 9
#else
#define SOCKET_MAP_NUM 8
#endif
#else
#ifdef SO_REUSEPORT
#define SOCKET_MAP_NUM 8
#else
#define SOCKET_MAP_NUM 7
#endif
#endif
#endif
	struct xlate_table_t sockopt_map[SOCKET_MAP_NUM] = {
		{ OSF_SO_DEBUG,	SO_DEBUG },
	#ifdef SO_ACCEPTCONN
	  { OSF_SO_ACCEPTCONN,	SO_ACCEPTCONN },
	#endif
	  { OSF_SO_REUSEADDR,	SO_REUSEADDR },
	  { OSF_SO_KEEPALIVE,	SO_KEEPALIVE },
	  { OSF_SO_DONTROUTE,	SO_DONTROUTE },
	  { OSF_SO_BROADCAST,	SO_BROADCAST },
	#ifdef SO_USELOOPBACK
	  { OSF_SO_USELOOPBACK,	SO_USELOOPBACK },
	#endif
	  { OSF_SO_LINGER,	SO_LINGER },
	  { OSF_SO_OOBINLINE,	SO_OOBINLINE },
	#ifdef SO_REUSEPORT
	  { OSF_SO_REUSEPORT,	SO_REUSEPORT }
	#endif
	};
	struct xlate_table_t tcpopt_map[9] = {
		{ OSF_TCP_NODELAY,	TCP_NODELAY },
	  { OSF_TCP_MAXSEG,	TCP_MAXSEG },
	#ifdef TCP_RPTR2RXT
	  { OSF_TCP_RPTR2RXT,	TCP_RPTR2RXT },
	#endif
	#ifdef TCP_KEEPIDLE
	  { OSF_TCP_KEEPIDLE,	TCP_KEEPIDLE },
	#endif
	#ifdef TCP_KEEPINTVL
	  { OSF_TCP_KEEPINTVL,	TCP_KEEPINTVL },
	#endif
	#ifdef TCP_KEEPCNT
	  { OSF_TCP_KEEPCNT,	TCP_KEEPCNT },
	#endif
	#ifdef TCP_KEEPINIT
	  { OSF_TCP_KEEPINIT,	TCP_KEEPINIT },
	#endif
	#ifdef TCP_PUSH
	  { OSF_TCP_PUSH,	TCP_PUSH },
	#endif
	#ifdef TCP_NODELACK
	  { OSF_TCP_NODELACK,	TCP_NODELACK }
	#endif
	};
	struct xlate_table_t socklevel_map[4] =
	{
	#if defined(__svr4__) || defined(__osf__) || defined(__FreeBSD__)
	  { OSF_SOL_SOCKET,	SOL_SOCKET },
	  { OSF_SOL_IP,		IPPROTO_IP },
	  { OSF_SOL_TCP,	IPPROTO_TCP },
	  { OSF_SOL_UDP,	IPPROTO_UDP }
	#else
	  { OSF_SOL_SOCKET,	SOL_SOCKET },
	  { OSF_SOL_IP,		SOL_IP },
	  { OSF_SOL_TCP,	SOL_TCP },
	  { OSF_SOL_UDP,	SOL_UDP }
	#endif
	};
	struct xlate_table_t family_map[8] =
	{
	  { OSF_AF_UNSPEC,	AF_UNSPEC },
	  { OSF_AF_UNIX,	AF_UNIX },
	  { OSF_AF_INET,	AF_INET },
	#ifdef AF_IMPLINK
	  { OSF_AF_IMPLINK,	AF_IMPLINK },
	#endif
	#ifdef AF_PUP
	  { OSF_AF_PUP,		AF_PUP },
	#endif
	#ifdef AF_CHAOS
	  { OSF_AF_CHAOS,	AF_CHAOS },
	#endif
	#ifdef AF_NS
	  { OSF_AF_NS,		AF_NS },
	#endif
	#ifdef AF_ISO
	  { OSF_AF_ISO,		AF_ISO }
	#endif
	};

	struct xlate_table_t socktype_map[5]=
	{
	  { OSF_SOCK_STREAM,	SOCK_STREAM },
	  { OSF_SOCK_DGRAM,	SOCK_DGRAM },
	  { OSF_SOCK_RAW,	SOCK_RAW },
	  { OSF_SOCK_RDM,	SOCK_RDM },
	  { OSF_SOCK_SEQPACKET,	SOCK_SEQPACKET }
	};

	struct osf_tbl_sysinfo
	{
	  long si_user;		/* user time */
	  long si_nice;		/* nice time */
	  long si_sys;		/* system time */
	  long si_idle;		/* idle time */
	  long si_hz;
	  long si_phz;
	  long si_boottime;	/* boot time in seconds */
	  long wait;		/* wait time */
	};
	/*
		 * main.cc
		 * */
		/*sim-outorder.h*/
static char *bpred_spec_opt;
struct RS_link;
struct RUU_station {
	//		  int threadid;
	md_inst_t IR;			/* instruction bits */
	enum md_opcode op;			/* decoded instruction opcode */
	md_addr_t PC, next_PC, pred_PC;	/* inst PC, next PC, predicted PC */
	int in_LSQ;				/* non-zero if op is in LSQ */
	int ea_comp;				/* non-zero if op is an addr comp */
	int recover_inst;			/* start of mis-speculation? */
	int stack_recover_idx;		/* non-speculative TOS for RSB pred */
	struct bpred_update_t dir_update;	/* bpred direction update info */
	int spec_mode;			/* non-zero if issued in spec_mode */
	md_addr_t addr;			/* effective address for ld/st's */
	INST_TAG_TYPE tag;
	INST_SEQ_TYPE seq;
	unsigned int ptrace_seq;		/* pipetrace sequence number */
	int slip;
	int queued;				/* operands ready and queued */
	int issued;				/* operation is/was executing */
	int completed;			/* operation has completed execution */
	int onames[MAX_ODEPS];		/* output logical names (NA=unused) */
	struct RS_link *odep_list[MAX_ODEPS];
	int idep_ready[MAX_IDEPS];		/* input operand ready? */
};
struct RS_link {
	struct RS_link *next;			/* next entry in list */
	struct RUU_station *rs;		/* referenced RUU resv station */
	INST_TAG_TYPE tag;			/* inst instance sequence number */
	union {
	tick_t when;			/* time stamp of entry (for eventq) */
	INST_SEQ_TYPE seq;			/* inst sequence */
	int opnum;				/* input/output operand number */
	} x;
};
struct CV_link {
	struct RUU_station *rs;               /* creator's reservation station */
	int odep_num;                         /* specific output operand */
};
struct spec_mem_ent {
	struct spec_mem_ent *next;		/* ptr to next hash table bucket */
	md_addr_t addr;			/* virtual address of spec state */
	unsigned int data[2];			/* spec buffer, up to 8 bytes */
};
struct fetch_rec {
	md_inst_t IR;				/* inst register */
	md_addr_t regs_PC, pred_PC;		/* current PC, predicted next PC */
	struct bpred_update_t dir_update;	/* bpred direction update info */
	int stack_recover_idx;		/* branch predictor RSB index */
	unsigned int ptrace_seq;		/* print trace sequence id */
};
struct context{// initial context
	md_addr_t ld_text_base;
	unsigned int ld_text_size;
	md_addr_t ld_data_base;
	md_addr_t ld_brk_point;
	unsigned int ld_data_size;
	md_addr_t ld_stack_base;
	unsigned int ld_stack_size;
	md_addr_t ld_stack_min;
	char *ld_prog_fname;
	md_addr_t ld_prog_entry;
	md_addr_t ld_environ_base;
	int ld_target_big_endian;
	struct regs_t *regs;
   struct mem_t *mem;
	struct cache_t *cache_il1;
	struct cache_t *cache_il2;
	struct cache_t *cache_dl1;
	struct cache_t *cache_dl2;
	struct cache_t *itlb;
	struct cache_t *dtlb;
	struct bpred_t *pred;
	struct res_pool *fu_pool;
	struct fetch_rec *fetch_data;
	int fetch_num; /* num entries in IF -> DIS queue */
	int fetch_tail, fetch_head; /* head and tail pointers of queue */
	counter_t IFQ_count; /* cumulative IFQ occupancy */
	counter_t IFQ_fcount; /* cumulative IFQ full count */
	struct RUU_station *RUU; /* register update unit */
	int RUU_head, RUU_tail; /* RUU head and tail pointers */
	int RUU_num; /* num entries currently in RUU */
	counter_t RUU_count; /* cumulative RUU occupancy */
	counter_t RUU_fcount; /* cumulative RUU full count */
	struct RUU_station *LSQ; /* load/store queue */
	int LSQ_head, LSQ_tail; /* LSQ head and tail pointers */
	int LSQ_num; /* num entries currently in LSQ */
	int LSQ_count;
	int LSQ_fcount;
	BITMAP_TYPE(MD_TOTAL_REGS, use_spec_cv);
	BITMAP_TYPE(MD_NUM_IREGS, use_spec_R);
	BITMAP_TYPE(MD_NUM_FREGS, use_spec_F);
	BITMAP_TYPE(MD_NUM_FREGS, use_spec_C);
	struct CV_link create_vector[MD_TOTAL_REGS];
	struct CV_link spec_create_vector[MD_TOTAL_REGS];
	tick_t create_vector_rt[MD_TOTAL_REGS];
	tick_t spec_create_vector_rt[MD_TOTAL_REGS];
	md_gpr_t spec_regs_R;
	md_fpr_t spec_regs_F;
	md_ctrl_t spec_regs_C;
	md_addr_t pred_PC;
	md_addr_t recover_PC;
	md_addr_t fetch_regs_PC;
	md_addr_t fetch_pred_PC;
	struct RS_link last_op;
	struct spec_mem_ent *store_htable[STORE_HASH_SIZE];
	struct spec_mem_ent *bucket_free_list;
	char** argv;
	int argc;
/*core类下*/
//		unsigned int inst_seq;// = 0;
//		unsigned int ptrace_seq;// = 0;
};
//补充
	static const struct RS_link RSLINK_NULL;// = RSLINK_NULL_DATA;
	static const struct CV_link CVLINK_NULL;// = { NULL, 0 };
/*数据部分*/
//资源池
	struct res_name fu_name;
	struct res_desc fu_config[FU_RES_NUM];
//上下文
	context *runcontext;
	FILE* bncfile;
	FILE* taskfile;
//统计参数设置
	counter_t sim_num_insn;
	counter_t sim_num_refs;
	counter_t sim_num_loads;
	counter_t sim_num_branches;
	counter_t sim_total_insn;
	counter_t sim_total_refs;
	counter_t sim_total_loads;
	counter_t sim_total_branches;

	counter_t IFQ_count;		/* cumulative IFQ occupancy */
	counter_t IFQ_fcount;		/* cumulative IFQ full count */
	counter_t RUU_count;		/* cumulative RUU occupancy */
	counter_t RUU_fcount;		/* cumulative RUU full count */
	counter_t LSQ_count;		/* cumulative LSQ occupancy */
	counter_t LSQ_fcount;		/* cumulative LSQ full count */
	counter_t sim_invalid_addrs;
	counter_t sim_slip;
//未用到的变量 - 未注册
//	counter_t fetch_num_insn;
//	counter_t fetch_total_insn;
//以上参数统计

	struct RS_link *rslink_free_list;
	struct RS_link *event_queue;
	struct RS_link *ready_queue;

/* text-based stat profiles */
	struct Statistics::stat_stat_t *pcstat_stats[MAX_PCSTAT_VARS];
	counter_t pcstat_lastvals[MAX_PCSTAT_VARS];
	struct Statistics::stat_stat_t *pcstat_sdists[MAX_PCSTAT_VARS];






/*functions*/
//machine.h
char *md_reg_name(enum md_reg_type rt, int reg);
/* default register accessor object */
struct eval_value_t;
struct regs_t;
char *						/* err str, NULL for no err */
md_reg_obj(struct regs_t *regs,			/* registers to access */
	   int is_write,			/* access type */
	   enum md_reg_type rt,			/* reg bank to probe */
	   int reg,				/* register number */
	   struct eval_value_t *val);		/* input, output */
/* print integer REG(S) to STREAM */
void md_print_ireg(md_gpr_t regs, int reg, FILE *stream);
void md_print_iregs(md_gpr_t regs, FILE *stream);
/* print floating point REG(S) to STREAM */
void md_print_fpreg(md_fpr_t regs, int reg, FILE *stream);
void md_print_fpregs(md_fpr_t regs, FILE *stream);
/* print control REG(S) to STREAM */
void md_print_creg(md_ctrl_t regs, int reg, FILE *stream);
void md_print_cregs(md_ctrl_t regs, FILE *stream);
/* xor checksum registers */
word_t md_xor_regs(struct regs_t *regs);
/* intialize the inst decoder, this function builds the ISA decode tables */
void md_init_decoder(void);
/* disassemble a SimpleScalar instruction */
void md_print_insn(md_inst_t inst,		/* instruction to disassemble */
	      md_addr_t pc,		/* addr of inst, used for PC-rels */
	      FILE *stream);		/* output stream */
/*symbol.cc*/
//static functions
int acmp(const struct sym_sym_t **sym1, const struct sym_sym_t **sym2);
int ncmp(struct sym_sym_t **sym1, struct sym_sym_t **sym2);
//others in symbol.cc
void sym_loadsyms(char *fname,int load_locals);
void sym_dumpsym(struct sym_sym_t *sym,FILE *fd);
void sym_dumpsyms(FILE *fd);
void sym_dumpstate(FILE *fd);
struct sym_sym_t *			/* symbol found, or NULL */
sym_bind_addr(md_addr_t addr,		/* address of symbol to locate */
	      int *pindex,		/* ptr to index result var */
	      int exact,		/* require exact address match? */
	      enum sym_db_t db);
struct sym_sym_t *				/* symbol found, or NULL */
sym_bind_name(char *name,			/* symbol name to locate */
	      int *pindex,			/* ptr to index result var */
	      enum sym_db_t db);
/*cache.cc*/
struct cache_t *			/* pointer to cache created */
cache_create(char *name,		/* name of the cache */
	 int nsets,			/* total number of sets in cache */
	 int bsize,			/* block (line) size of cache */
	 int balloc,		/* allocate data space for blocks? */
	 int usize,			/* size of user data to alloc w/blks */
	 int assoc,			/* associativity of cache */
	 enum TCore::cache_policy policy,	/* replacement policy w/in sets */
	 /* block access function, see description w/in struct cache def */
	 unsigned int (TALPHA::*blk_access_fn)(enum mem_cmd cmd,
				   md_addr_t baddr, int bsize,
				   struct cache_blk_t *blk,
				   tick_t now),
	 unsigned int hit_latency);
enum TCore::cache_policy	cache_char2policy(char c);		/* replacement policy as a char */
void	cache_config(struct cache_t *cp,FILE *stream);		/* output stream */
void	cache_reg_stats(struct cache_t *cp,struct stat_sdb_t *sdb);/* stats database */
void	cache_stats(struct cache_t *cp,FILE *stream);		/* output stream */
unsigned int cache_access(struct cache_t *cp,	/* cache to access */
	 enum mem_cmd cmd,		/* access type, Read or Write */
	 md_addr_t addr,		/* address of access */
	 void *vp,			/* ptr to buffer for input/output */
	 int nbytes,		/* number of bytes to access */
	 tick_t now,		/* time of access */
	 byte_t **udata,		/* for return of user data ptr */
	 md_addr_t *repl_addr);
int cache_probe(struct cache_t *cp,		/* cache instance to probe */
	md_addr_t addr);
unsigned int cache_flush(struct cache_t *cp,		/* cache instance to flush */
	tick_t now);
unsigned int cache_flush_addr(struct cache_t *cp,	/* cache instance to flush */
	 md_addr_t addr,	/* address of block to flush */
	 tick_t now);
void unlink_htab_ent(struct cache_t *cp,		/* cache to update */
	struct cache_set_t *set,	/* set containing bkt chain */
	struct cache_blk_t *blk);
void link_htab_ent(struct cache_t *cp,		/* cache to update */
	  struct cache_set_t *set,		/* set containing bkt chain */
	  struct cache_blk_t *blk);
void update_way_list(struct cache_set_t *set,	/* set contained way chain */
	struct cache_blk_t *blk,	/* block to insert */
	enum list_loc_t where);
/*end of cache.cc*/
/*bpred.cc*/
struct bpred_t * bpred_create(enum bpred_class classt,	/* type of predictor to create */
	 unsigned int bimod_size,	/* bimod table size */
	 unsigned int l1size,	/* level-1 table size */
	 unsigned int l2size,	/* level-2 table size */
	 unsigned int meta_size,	/* meta predictor table size */
	 unsigned int shift_width,	/* history register width */
	 unsigned int xort,		/* history xor address flag */
	 unsigned int btb_sets,	/* number of sets in BTB */
	 unsigned int btb_assoc,	/* BTB associativity */
	 unsigned int retstack_size);/* num entries in ret-addr stack */
struct bpred_dir_t *	bpred_dir_create (
	enum bpred_class classt,	/* type of predictor to create */
	unsigned int l1size,		/* level-1 table size */
	unsigned int l2size,		/* level-2 table size (if relevant) */
	unsigned int shift_width,	/* history register width */
	unsigned int xort);
void	bpred_config(struct bpred_t *pred,	/* branch predictor instance */
	 FILE *stream);		/* output stream */
void 	bpred_dir_config(
struct bpred_dir_t *pred_dir,	/* branch direction predictor instance */
	char name[],			/* predictor name */
	FILE *stream);
/* print predictor stats */
void	bpred_stats(struct bpred_t *pred,	/* branch predictor instance */
	FILE *stream);		/* output stream */
/* register branch predictor stats */
void	bpred_reg_stats(struct bpred_t *pred,	/* branch predictor instance */
	Statistics & ss);/* stats database */
/* reset stats after priming, if appropriate */
void bpred_after_priming(struct bpred_t *bpred);
md_addr_t				/* predicted branch target addr */
bpred_lookup(struct bpred_t *pred,	/* branch predictor instance */
	md_addr_t baddr,		/* branch address */
	md_addr_t btarget,		/* branch target if taken */
	enum md_opcode op,		/* opcode of instruction */
	int is_call,		/* non-zero if inst is fn call */
	int is_return,		/* non-zero if inst is fn return */
	struct bpred_update_t *dir_update_ptr, /* pred state pointer */
	int *stack_recover_idx);	/* Non-speculative top-of-stack;
			 * used on mispredict recovery */
char * bpred_dir_lookup(struct bpred_dir_t *pred_dir,	/* branch dir predictor inst */
	md_addr_t baddr);
void	bpred_recover(struct bpred_t *pred,	/* branch predictor instance */
	md_addr_t baddr,		/* branch address */
	int stack_recover_idx);
void bpred_update(struct bpred_t *pred,	/* branch predictor instance */
	md_addr_t baddr,		/* branch address */
	md_addr_t btarget,		/* resolved branch target */
	int taken,			/* non-zero if branch was taken */
	int pred_taken,		/* non-zero if branch was pred taken */
	int correct,		/* was earlier addr prediction ok? */
	enum md_opcode op,		/* opcode of instruction */
	struct bpred_update_t *dir_update_ptr);
/*bpred.cc end*/
/*eio.h*/
FILE *eio_create(char *fname);
FILE *eio_open(char *fname);
int eio_valid(char *fname);
void eio_close(FILE *fd);
counter_t	eio_write_chkpt(struct regs_t *regs,		/* regs to dump */
	struct mem_t *mem,		/* memory to dump */
	FILE *fd);			/* stream to write to */
counter_t	eio_read_chkpt(struct regs_t *regs,		/* regs to dump */
	struct mem_t *mem,		/* memory to dump */
	FILE *fd);			/* stream to read */
void	eio_write_trace(FILE *eio_fd,			/* EIO stream file desc */
	counter_t icnt,			/* instruction count */
	struct regs_t *regs,		/* registers to update */
	mem_access_fn mem_fn,		/* generic memory accessor */
	struct mem_t *mem,		/* memory to update */
	md_inst_t inst);		/* system call inst */
void	eio_read_trace(FILE *eio_fd,			/* EIO stream file desc */
   counter_t icnt,			/* instruction count */
   struct regs_t *regs,		/* registers to update */
   mem_access_fn mem_fn,		/* generic memory accessor */
   struct mem_t *mem,		/* memory to update */
   md_inst_t inst);			/* system call inst */
void eio_fast_forward(FILE *eio_fd, counter_t icnt);
/*end of eio.h*/
void sim_init();
word_t md_crc_regs(struct regs_t *regs);

/*range.h*/
char * range_parse_pos(char *pstr,struct TCore::range_pos_t *pos);
void range_print_pos(struct TCore::range_pos_t *pos,FILE *stream);
char * range_parse_range(char *rstr,struct TCore::range_range_t *range);
void range_print_range(struct TCore::range_range_t *range, FILE *stream);
int range_cmp_pos(struct TCore::range_pos_t *pos,counter_t val);
int range_cmp_range(struct TCore::range_range_t *range,counter_t val);
int range_cmp_range1(struct TCore::range_range_t *range,	/* execution range */
	 md_addr_t addr,		/* address value */
	 counter_t icount,		/* instruction count */
	 counter_t cycle);

/*some of ptrace.cc/h*/
void ptrace_open(char *fname,char *range);
void ptrace_close(void);
void __ptrace_endinst(unsigned int iseq);
void __ptrace_newcycle(tick_t cycle);
void __ptrace_newstage(unsigned int iseq,	/* instruction sequence number */
	  char *pstage,		/* pipeline stage entered */
	  unsigned int pevents);
void	__ptrace_newinst(unsigned int iseq,	/* instruction sequence number */
	 md_inst_t inst,	/* new instruction */
	 md_addr_t pc,		/* program counter of instruction */
	 md_addr_t addr);
void	__ptrace_newuop(unsigned int iseq,	/* instruction sequence number */
	char *uop_desc,		/* new uop description */
	md_addr_t pc,		/* program counter of instruction */
	md_addr_t addr);


/*sim-outorder.cc*/
unsigned int			/* total latency of access */
mem_access_latency(int blk_sz);
unsigned int dl1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
  md_addr_t baddr,		/* block address to access */
  int bsize,		/* size of block to access */
  struct cache_blk_t *blk,	/* ptr to block in upper level */
  tick_t now);
unsigned int  dl2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
  md_addr_t baddr,		/* block address to access */
  int bsize,		/* size of block to access */
  struct cache_blk_t *blk,	/* ptr to block in upper level */
  tick_t now);
unsigned int il1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
  md_addr_t baddr,		/* block address to access */
  int bsize,		/* size of block to access */
  struct cache_blk_t *blk,	/* ptr to block in upper level */
  tick_t now);
unsigned int il2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
  md_addr_t baddr,		/* block address to access */
  int bsize,		/* size of block to access */
  struct cache_blk_t *blk,	/* ptr to block in upper level */
  tick_t now);
unsigned int itlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
   md_addr_t baddr,		/* block address to access */
   int bsize,		/* size of block to access */
   struct cache_blk_t *blk,	/* ptr to block in upper level */
   tick_t now);
unsigned int dtlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
   md_addr_t baddr,	/* block address to access */
   int bsize,		/* size of block to access */
   struct cache_blk_t *blk,	/* ptr to block in upper level */
   tick_t now);
void sim_main(void);
void sim_reg_options(struct opt_odb_t *odb);
void sim_check_options(struct opt_odb_t *odb, int argc, char **argv);
int read_thread_args(FILE* bncfile);
void sim_load_prog(CoreStruct* cs);
void sim_uninit(void);
void ruu_init(void);
void ruu_dumpent(struct RUU_station *rs,		/* ptr to RUU station */
	int index,				/* entry index */
	FILE *stream,			/* output stream */
	int header);
void ruu_dump(FILE *stream);
void lsq_init(void);
void lsq_dump(FILE *stream);
void rslink_init();
void ruu_release_fu(void);
void eventq_init();
void readyq_init();
void eventq_dump(FILE *stream);
void eventq_queue_event(struct RUU_station *rs, tick_t when);
struct RUU_station * eventq_next_event(void);
void readyq_dump(FILE *stream);
void readyq_enqueue(struct RUU_station *rs);
void cv_init();
void cv_dump(FILE *stream);
void ruu_commit(void);
void ruu_recover(int branch_index);
void ruu_writeback(void);
void lsq_refresh(void);
void ruu_issue(void);
void rspec_dump(FILE *stream);
void tracer_recover(void);
void tracer_init(void);
enum md_fault_type spec_mem_access(struct mem_t *mem,		/* memory space to access */
	enum mem_cmd cmd,		/* Read or Write access cmd */
	md_addr_t addr,			/* virtual address of access */
	void *p,			/* input/output buffer */
	int nbytes);
void mspec_dump(FILE *stream);
static char * simoo_mem_obj(
	TALPHA &t,
	struct mem_t *mem,		/* memory space to access */
	int is_write,			/* access type */
	md_addr_t addr,			/* address to access */
	char *p,				/* input/output buffer */
	int nbytes);
void ruu_link_idep(struct RUU_station *rs,		/* rs station to link */
	int idep_num,			/* input dependence number */
	int idep_name);
void ruu_install_odep(struct RUU_station *rs,	/* creating RUU station */
	int odep_num,			/* output operand number */
	int odep_name);
//	static char * simoo_reg_obj(
//					TALPHA &t,
//					struct regs_t *xregs,		/* registers to access */
//			      int is_write,			/* access type */
//			      enum md_reg_type rt,		/* reg bank to probe */
//			      int reg,				/* register number */
//			      struct eval_value_t *val);
void ruu_dispatch(void);
void fetch_init(void);
void fetch_dump(FILE *stream);
void ruu_fetch(void);
//	static char * simoo_mstate_obj(
//					FILE *stream,			/* output stream */
//					char *cmd,			/* optional command string */
//					struct regs_t *regs,		/* registers to access */
//					struct mem_t *mem);

/*regs.cc*/
void regs_create(void);
void regs_init(void);
void regs_dump(struct regs_t *regs,	FILE *stream);
void regs_destroy(struct regs_t *regs);
/*memory.cc*/
void	mem_create(const char *name);			/* name of the memory space */
void mem_init(void);	/* memory space to initialize */
byte_t *	mem_translate(struct mem_t *mem,	/* memory space to access */
		  qword_t addr);		/* virtual address to translate */
void mem_newpage(struct mem_t *mem,		/* memory space to allocate in */
		qword_t addr);		/* virtual address to allocate */
enum md_fault_type mem_access(struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   qword_t addr,		/* target address to access */
	   void *vp,			/* host memory address to access */
	   int nbytes);			/* number of bytes to access */
void mem_reg_stats(struct mem_t *mem,	/* memory space to declare */
		  struct stat_sdb_t *sdb);	/* stats data base */
/* dump a block of memory, returns any faults encountered */
enum md_fault_type mem_dump(struct mem_t *mem,		/* memory space to display */
	 qword_t addr,		/* target address to dump */
	 int len,			/* number bytes to dump */
	 FILE *stream);			/* output stream */
enum md_fault_type mem_strcpy(mem_access_fn mem_fn,	/* user-specified memory accessor */
	   struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   qword_t addr,		/* target address to access */
	   char *s);			/* host memory string buffer */
enum md_fault_type mem_bcopy(mem_access_fn mem_fn,		/* user-specified memory accessor */
	  struct mem_t *mem,		/* memory space to access */
	  enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	  qword_t addr,		/* target address to access */
	  void *vp,			/* host memory address to access */
	  int nbytes);			/* number of bytes to access */
enum md_fault_type mem_bcopy4(mem_access_fn mem_fn,	/* user-specified memory accessor */
	   struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   qword_t addr,		/* target address to access */
	   void *vp,			/* host memory address to access */
	   int nbytes);			/* number of bytes to access */
enum md_fault_type mem_bzero(mem_access_fn mem_fn,		/* user-specified memory accessor */
	  struct mem_t *mem,		/* memory space to access */
	  qword_t addr,		/* target address to access */
	  int nbytes);			/* number of bytes to clear */

/*loader.cc*/
void ld_reg_stats();
void ld_load_prog(char *fname,		/* program to load */
	 int argc, char **argv,	/* simulated program cmd line args */
	 char **envp,		/* simulated program environment */
	 struct regs_t *regs,	/* registers to initialize for load */
	 struct mem_t *mem,		/* memory space to load prog into */
	 int zero_bss_segs);
/*syscall.cc*/
int xlate_arg(int target_val, struct xlate_table_t *map, int map_sz, char *name);
void sys_syscall(struct regs_t *regs,	/* registers to access */
	    mem_access_fn mem_fn,	/* generic memory accessor */
	    struct mem_t *mem,		/* memory space to access */
	    md_inst_t inst,		/* system call inst */
	    int traceable);

/*----------------------------------------------------------------------------------------------------------*/
	TALPHA(CoreStruct* cs,int threadid);
	inline void exitrun();
	void reg_stat_init(Statistics &);
	void check_corestruct(CoreStruct* cs);
	void check_simoptions();
	void init_context();
	sim_rt_t run();//虚函数
	sim_rt_t run_cycle();
};

//undef
#include "alphaundef.h"
//#undef TARGET_ALPHA
//#undef MD_QWORD_ADDRS
//#undef MD_PAGE_SIZE
//#undef MD_LOG_PAGE_SIZE
//#undef MD_NUM_IREGS
//#undef MD_NUM_FREGS
//#undef MD_NUM_CREGS
//#undef MD_TOTAL_REGS
//#undef MD_SWAPH(X)
//#undef MD_SWAPW(X)
//#undef MD_SWAPQ(X)
//#undef MD_SWAPD(X)
//#undef MD_SWAPI(X)
//#undef MD_FETCH_INST(INST, MEM, PC)
//#undef MD_MAX_ENVIRON
//#undef MD_TOP_OP(INST)
//#undef MD_SET_OPCODE(OP, INST)
//#undef MD_MAX_MASK
//#undef MD_OP_NAME(OP)
//#undef MD_OP_FORMAT(OP)
//#undef MD_OP_FUCLASS(OP)
//#undef MD_FU_NAME(FU)
//#undef F_ICOMP		0x00000001	/* integer computation */
//#undef F_FCOMP		0x00000002	/* FP computation */
//#undef F_CTRL		0x00000004	/* control inst */
//#undef F_UNCOND	0x00000008	/*   unconditional change */
//#undef F_COND		0x00000010	/*   conditional change */
//#undef F_MEM		0x00000020	/* memory access inst */
//#undef F_LOAD		0x00000040	/*   load inst */
//#undef F_STORE		0x00000080	/*   store inst */
//#undef F_DISP		0x00000100	/*   displaced (R+C) addr mode */
//#undef F_RR		0x00000200	/*   R+R addr mode */
//#undef F_DIRECT	0x00000400	/*   direct addressing mode */
//#undef F_TRAP		0x00000800	/* traping inst */
//#undef F_LONGLAT	0x00001000	/* long latency inst (for sched) */
//#undef F_DIRJMP	0x00002000	/* direct jump */
//#undef F_INDIRJMP	0x00004000	/* indirect jump */
//#undef F_CALL		0x00008000	/* function call */
//#undef F_FPCOND	0x00010000	/* FP conditional branch */
//#undef F_IMM		0x00020000	/* instruction has immediate operand */
//#undef MD_OP_FLAGS(OP)
//
//
//#undef RA
//#undef RB
//#undef RC
//#undef IMM
//#undef TARG
//#undef OFS
//#undef SEXT(X)
//#undef SEXT21(X)
//#undef SEXT32(X)
//#undef ARITH_OVFL(RESULT, OP1, OP2)
//
//#undef IEEEFP_DBL_SIGN(Q)
//#undef IEEEFP_DBL_EXPONENT(Q)
//#undef IEEEFP_DBL_FRACTION(Q)
//#undef IS_IEEEFP_DBL_NAN(Q)
//
//#undef SET_TPC(PC)
//
//#undef OSF_SYS_exit
//#undef MD_EXIT_SYSCALL(REGS)
//#undef OSF_SYS_write
//#undef MD_OUTPUT_SYSCALL(REGS)
//
//#undef MD_STREAM_FILENO(REGS)
//#undef MD_IS_CALL(OP)
//#undef MD_IS_RETURN(OP)
//#undef MD_IS_INDIR(OP)
//
//#undef MD_AMODE_PREPROBE(OP, FSM)
//#undef MD_AMODE_PROBE(AM, OP, FSM)
//#undef MD_AMODE_POSTPROBE(FSM)			/* nada... */
///* expected EIO file format */
//#undef MD_EIO_FILE_FORMAT
//
//#undef MD_MISC_REGS_TO_EXO(REGS)
//#undef MD_IREG_TO_EXO(REGS, IDX)
//#undef MD_FREG_TO_EXO(REGS, IDX)
//#undef MD_EXO_TO_MISC_REGS(EXO, ICNT, REGS)
//#undef MD_EXO_TO_IREG(EXO, REGS, IDX)
//#undef MD_EXO_TO_FREG(EXO, REGS, IDX)
//#undef MD_EXO_CMP_IREG(EXO, REGS, IDX)
//#undef MD_FIRST_IN_REG
//#undef MD_LAST_IN_REG
//
//#undef MD_FIRST_OUT_REG
//#undef MD_LAST_OUT_REG
//
//#undef stat_reg_counter
//#undef sc_counter
//#undef for_counter
//#undef stat_reg_addr
//
//#undef MD_AGEN_OP		ADDQ
//#undef MD_NOP_OP		OP_NA
//#undef MD_VALID_ADDR(ADDR)
//#undef MD_BR_SHIFT
//
//#undef TEXT_TAIL_PADDING



#endif /* ALPHA_H_ */

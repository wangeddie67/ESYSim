/*
 * sim-oodef.h
 *
 *  Created on: 2016年3月22日
 *      Author: root
 */
/*from sim-outorder.cc(may others),and undependent with machine.h or TARGET_PISA,TARGET_ALPHA*/
#ifndef SIM_OODEF_H_
#define SIM_OODEF_H_

#include "host.h"
#include "misc.h"
//eval.h
#ifdef HOST_HAS_QWORD
#define EVAL_INTEGRAL(TYPE)						\
  ((TYPE) == et_int || (TYPE) == et_uint || (TYPE) == et_addr		\
   || (TYPE) == et_qword || (TYPE) == et_sqword)
#else /* !HOST_HAS_QWORD */
#define EVAL_INTEGRAL(TYPE)						\
  ((TYPE) == et_int || (TYPE) == et_uint || (TYPE) == et_addr)
#endif /* HOST_HAS_QWORD */


/*stats.h*/
#define HTAB_SZ			1024
#define HTAB_HASH(I)		((((I) >> 8) ^ (I)) & (HTAB_SZ - 1))
#define PF_COUNT		0x0001
#define PF_PDF			0x0002
#define PF_CDF			0x0004
#define PF_ALL			(PF_COUNT|PF_PDF|PF_CDF)
/*misc.cc*/
#if defined(__alpha) || defined(linux)
#include <unistd.h>
#endif /* __alpha || linux */
/*no dlite -- lch*/
///*dlite.h*/
//#define ACCESS_READ	0x01			/* read access allowed */
//#define ACCESS_WRITE	0x02			/* write access allowed */
//#define ACCESS_EXEC	0x04			/* execute access allowed */
//#define dlite_check_break(NPC, ACCESS, ADDR, ICNT, CYCLE)		\
//  ((dlite_check || dlite_active)					\
//   ? __check_break((NPC), (ACCESS), (ADDR), (ICNT), (CYCLE))		\
//   : FALSE)
///*dlite.cc*/
//#define MOD_BYTE	0x0001		/* b - print a byte */
//#define MOD_HALF	0x0002		/* h - print a half (short) */
//#define MOD_WORD	0x0004		/* w - print a word */
//#define MOD_QWORD	0x0008		/* q - print a qword */
//#define MOD_FLOAT	0x0010		/* F - print a float */
//#define MOD_DOUBLE	0x0020		/* f - print a double */
//#define MOD_CHAR	0x0040		/* c - print a character */
//#define MOD_STRING	0x0080		/* s - print a string */
//#define MOD_SIZES							\
//  (MOD_BYTE|MOD_HALF|MOD_WORD|MOD_QWORD					\
//   |MOD_FLOAT|MOD_DOUBLE|MOD_CHAR|MOD_STRING)
///* format modifier mask bit definitions */
//#define MOD_DECIMAL	0x0100		/* d - print in decimal format */
//#define MOD_UNSIGNED	0x0200		/* u - print in unsigned format */
//#define MOD_OCTAL	0x0400		/* o - print in octal format */
//#define MOD_HEX		0x0800		/* x - print in hex format */
//#define MOD_BINARY	0x1000		/* 1 - print in binary format */
//#define MOD_FORMATS							\
//  (MOD_DECIMAL|MOD_UNSIGNED|MOD_OCTAL|MOD_HEX|MOD_BINARY)
//
//#define DLITE_PROMPT		"DLite! > "
//#define MAX_ARGS	4
//#define MAX_STR		128
//#define BYTES_PER_LINE			16 /* must be a power of two */
//#define LINES_PER_SCREEN		4
//#define INSTS_PER_SCREEN		16
//static char *dlite_help_tail =
//  "Arguments <addr>, <cnt>, <expr>, and <id> are any legal expression:\n"
//  "  <expr>    <-  <factor> +|- <expr>\n"
//  "  <factor>  <-  <term> *|/ <factor>\n"
//  "  <term>    <-  ( <expr> )\n"
//  "                | - <term>\n"
//  "                | <const>\n"
//  "                | <symbol>\n"
//  "                | <file:loc>\n"
//  "\n"
//  "Command modifiers <mods> are any of the following:\n"
//  "\n"
//  "  b - print a byte\n"
//  "  h - print a half (short)\n"
//  "  w - print a word (default)\n"
//#ifdef HOST_HAS_QWORD
//  "  q - print a qword\n"
//#endif /* HOST_HAS_QWORD */
//  "  F - print a float\n"
//  "  f - print a double\n"
//  "  c - print a character\n"
//  "  s - print a string\n"
//  "  d - print in decimal format (default)\n"
//  "  u - print in unsigned decimal format\n"
//  "  o - print in octal format\n"
//  "  x - print in hex format\n"
//  "  1 - print in binary format\n";

/*ptrace.h*/
#define PST_IFETCH		"IF"
#define PST_DISPATCH		"DA"
#define PST_EXECUTE		"EX"
#define PST_WRITEBACK		"WB"
#define PST_COMMIT		"CT"
/* pipeline events */
#define PEV_CACHEMISS		0x00000001	/* I/D-cache miss */
#define PEV_TLBMISS		0x00000002	/* I/D-tlb miss */
#define PEV_MPOCCURED		0x00000004	/* mis-pred branch occurred */
#define PEV_MPDETECT		0x00000008	/* mis-pred branch detected */
#define PEV_AGEN		0x00000010	/* address generation */

#define ptrace_check_active(PC, ICNT, CYCLE)				\
  ((ptrace_outfd != NULL						\
    && !range_cmp_range1(&ptrace_range, (PC), (ICNT), (CYCLE)))		\
   ? (!ptrace_oneshot ? (ptrace_active = ptrace_oneshot = TRUE) : FALSE)\
   : (ptrace_active = FALSE))
#define ptrace_newinst(A,B,C,D)						\
  if (ptrace_active) __ptrace_newinst((A),(B),(C),(D))
#define ptrace_newuop(A,B,C,D)						\
  if (ptrace_active) __ptrace_newuop((A),(B),(C),(D))
#define ptrace_endinst(A)						\
  if (ptrace_active) __ptrace_endinst((A))
#define ptrace_newcycle(A)						\
  if (ptrace_active) __ptrace_newcycle((A))
#define ptrace_newstage(A,B,C)						\
  if (ptrace_active) __ptrace_newstage((A),(B),(C))
#define ptrace_active(A,I,C)						\
  (ptrace_outfd != NULL	&& !range_cmp_range(&ptrace_range, (A), (I), (C)))
/*bpred.h*/
#define dassert(a) assert(a)
/*cache.h*/
#define CACHE_HIGHLY_ASSOC(cp)	((cp)->assoc > 4)
#define CACHE_BLK_VALID		0x00000001	/* block in valid, in use */
#define CACHE_BLK_DIRTY		0x00000002	/* dirty block */
#define cache_double(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(double), now, udata)
#define cache_float(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(float), now, udata)
#define cache_dword(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(long long), now, udata)
#define cache_word(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(int), now, udata)
#define cache_half(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(short), now, udata)
#define cache_byte(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(char), now, udata)


/*cache.cc*/
#define CACHE_TAG(cp, addr)	((addr) >> (cp)->tag_shift)
#define CACHE_SET(cp, addr)	(((addr) >> (cp)->set_shift) & (cp)->set_mask)
#define CACHE_BLK(cp, addr)	((addr) & (cp)->blk_mask)
#define CACHE_TAGSET(cp, addr)	((addr) & (cp)->tagset_mask)

/* extract/reconstruct a block address */
#define CACHE_BADDR(cp, addr)	((addr) & ~(cp)->blk_mask)
#define CACHE_MK_BADDR(cp, tag, set)					\
  (((tag) << (cp)->tag_shift)|((set) << (cp)->set_shift))

/* index an array of cache blocks, non-trivial due to variable length blocks */
#define CACHE_BINDEX(cp, blks, i)					\
  ((struct cache_blk_t *)(((char *)(blks)) +				\
			  (i)*(sizeof(struct cache_blk_t) +		\
			       ((cp)->balloc				\
				? (cp)->bsize*sizeof(byte_t) : 0))))

/* cache data block accessor, type parameterized */
#define __CACHE_ACCESS(type, data, bofs)				\
  (*((type *)(((char *)data) + (bofs))))

/* cache data block accessors, by type */
#define CACHE_DOUBLE(data, bofs)  __CACHE_ACCESS(double, data, bofs)
#define CACHE_FLOAT(data, bofs)	  __CACHE_ACCESS(float, data, bofs)
#define CACHE_WORD(data, bofs)	  __CACHE_ACCESS(unsigned int, data, bofs)
#define CACHE_HALF(data, bofs)	  __CACHE_ACCESS(unsigned short, data, bofs)
#define CACHE_BYTE(data, bofs)	  __CACHE_ACCESS(unsigned char, data, bofs)

/* cache block hashing macros, this macro is used to index into a cache
   set hash table (to find the correct block on N in an N-way cache), the
   cache set index function is CACHE_SET, defined above */
#define CACHE_HASH(cp, key)						\
  (((key >> 24) ^ (key >> 16) ^ (key >> 8) ^ key) & ((cp)->hsize-1))

/* copy data out of a cache block to buffer indicated by argument pointer p */
#define CACHE_BCOPY(cmd, blk, bofs, p, nbytes)	\
  if (cmd == Read)							\
    {									\
      switch (nbytes) {							\
      case 1:								\
	*((byte_t *)p) = CACHE_BYTE(&blk->data[0], bofs); break;	\
      case 2:								\
	*((half_t *)p) = CACHE_HALF(&blk->data[0], bofs); break;	\
      case 4:								\
	*((word_t *)p) = CACHE_WORD(&blk->data[0], bofs); break;	\
      default:								\
	{ /* >= 8, power of two, fits in block */			\
	  int words = nbytes >> 2;					\
	  while (words-- > 0)						\
	    {								\
	      *((word_t *)p) = CACHE_WORD(&blk->data[0], bofs);	\
	      p += 4; bofs += 4;					\
	    }\
	}\
      }\
    }\
  else /* cmd == Write */						\
    {									\
      switch (nbytes) {							\
      case 1:								\
	CACHE_BYTE(&blk->data[0], bofs) = *((byte_t *)p); break;	\
      case 2:								\
        CACHE_HALF(&blk->data[0], bofs) = *((half_t *)p); break;	\
      case 4:								\
	CACHE_WORD(&blk->data[0], bofs) = *((word_t *)p); break;	\
      default:								\
	{ /* >= 8, power of two, fits in block */			\
	  int words = nbytes >> 2;					\
	  while (words-- > 0)						\
	    {								\
	      CACHE_WORD(&blk->data[0], bofs) = *((word_t *)p);		\
	      p += 4; bofs += 4;					\
	    }\
	}\
    }\
  }
/* bound sqword_t/dfloat_t to positive int */
#define BOUND_POS(N)		((int)(MIN(MAX(0, (N)), 2147483647)))
/*end of cache.cc*/
/*eio.h*/
#define EIO_PISA_FORMAT			1
#define EIO_ALPHA_FORMAT		2
#define EIO_FILE_VERSION		3
#define EIO_FILE_HEADER							\
  "/* This is a SimpleScalar EIO file - DO NOT MOVE OR EDIT THIS LINE! */\n"
/*end of eio.h*/

//memory.h
#define MEM_PTAB_SIZE		(32*1024)
#define MEM_LOG_PTAB_SIZE	15

//stats.h
#define HTAB_SZ			1024
#define HTAB_HASH(I)		((((I) >> 8) ^ (I)) & (HTAB_SZ - 1))
#define PF_COUNT		0x0001
#define PF_PDF			0x0002
#define PF_CDF			0x0004
#define PF_ALL			(PF_COUNT|PF_PDF|PF_CDF)

//libexo.h
//#define EXO_FMT_MAJOR		1
//#define EXO_FMT_MINOR		0
//#define as_integer	variant.as_integer
//#define as_address	variant.as_address
//#define as_float	variant.as_float
//#define as_char		variant.as_char
//#define as_string	variant.as_string
//#define as_list		variant.as_list
//#define as_array	variant.as_array
//#define as_token	variant.as_token
//#define as_blob		variant.as_blob
//#define EXO_ARR(E,N)							\
//  ((E)->ec != ec_array							\
//   ? (fatal("not an array"), *(struct exo_term_t **)(NULL))		\
//   : ((N) >= (E)->as_array.size						\
//      ? (fatal("array bounds error"), *(struct exo_term_t **)(NULL))	\
//      : (E)->as_array.array[(N)]))
//#define SET_EXO_ARR(E,N,V)						\
//  ((E)->ec != ec_array							\
//   ? (void)fatal("not an array")					\
//   : ((N) >= (E)->as_array.size						\
//      ? (void)fatal("array bounds error")				\
//      : (void)((E)->as_array.array[(N)] = (V))))

//line426
#define STATVAL(STAT)							\
  ((STAT)->sc == Statistics::sc_int							\
   ? (counter_t)*((STAT)->variant.for_int.var)			\
   : ((STAT)->sc == Statistics::sc_uint						\
      ? (counter_t)*((STAT)->variant.for_uint.var)		\
      : ((STAT)->sc == Statistics::sc_counter					\
	 ? *((STAT)->variant.for_counter.var)				\
	 : (panic("bad stat class"), 0))))


//line1738
#define OPERANDS_READY(RS)                                              \
  ((RS)->idep_ready[0] && (RS)->idep_ready[1] && (RS)->idep_ready[2])


//sim-outorder.cc  line1857
#define STORE_OP_INDEX                  0
#define STORE_ADDR_INDEX                1
#define STORE_OP_READY(RS)              ((RS)->idep_ready[STORE_OP_INDEX])
#define STORE_ADDR_READY(RS)            ((RS)->idep_ready[STORE_ADDR_INDEX])


//sim-outorder.cc  line1925
#define RSLINK_NULL_DATA		{ NULL, NULL, 0 }

//sim-outorder.cc  line1929
#define RSLINK_INIT(RSL, RS)						\
  ((RSL).next = NULL, (RSL).rs = (RS), (RSL).tag = (RS)->tag)
/* non-zero if RS link is NULL */
#define RSLINK_IS_NULL(LINK)            ((LINK)->rs == NULL)
/* non-zero if RS link is to a valid (non-squashed) entry */
#define RSLINK_VALID(LINK)              ((LINK)->tag == (LINK)->rs->tag)
/* extra RUU reservation station pointer */
#define RSLINK_RS(LINK)                 ((LINK)->rs)
/* get a new RS link record */
#define RSLINK_NEW(DST, RS)						\
  { struct RS_link *n_link;						\
    if (!rslink_free_list)						\
      panic("out of rs links");						\
    n_link = rslink_free_list;						\
    rslink_free_list = rslink_free_list->next;				\
    n_link->next = NULL;						\
    n_link->rs = (RS); n_link->tag = n_link->rs->tag;			\
    (DST) = n_link;							\
  }
/* free an RS link record */
#define RSLINK_FREE(LINK)						\
  {  struct RS_link *f_link = (LINK);					\
     f_link->rs = NULL; f_link->tag = 0;				\
     f_link->next = rslink_free_list;					\
     rslink_free_list = f_link;						\
  }
/* FIXME: could this be faster!!! */
/* free an RS link list */
#define RSLINK_FREE_LIST(LINK)						\
  {  struct RS_link *fl_link, *fl_link_next;				\
     for (fl_link=(LINK); fl_link; fl_link=fl_link_next)		\
       {								\
	 fl_link_next = fl_link->next;					\
	 RSLINK_FREE(fl_link);						\
       }								\
  }
/* total RS links allocated at program start */
#define MAX_RS_LINKS                    4096


//sim-outorder.cc  line2245

/* get a new create vector link */
#define CVLINK_INIT(CV, RS,ONUM)	((CV).rs = (RS), (CV).odep_num = (ONUM))
/* size of the create vector (one entry per architected register) */
#define CV_BMAP_SZ              (BITMAP_SIZE(MD_TOTAL_REGS))


//sim-outorder.cc  line2259
#define CREATE_VECTOR(N)        (BITMAP_SET_P(runcontext->use_spec_cv, CV_BMAP_SZ, (N))\
				 ? runcontext->spec_create_vector[N]                \
				 : runcontext->create_vector[N])
/* read a create vector timestamp entry */
#define CREATE_VECTOR_RT(N)     (BITMAP_SET_P(runcontext->use_spec_cv, CV_BMAP_SZ, (N))\
				 ? runcontext->spec_create_vector_rt[N]             \
				 : runcontext->create_vector_rt[N])
/* set a create vector entry */
#define SET_CREATE_VECTOR(N, L) (spec_mode                              \
				 ? (BITMAP_SET(runcontext->use_spec_cv, CV_BMAP_SZ, (N)),\
						 runcontext->spec_create_vector[N] = (L))        \
				 : (runcontext->create_vector[N] = (L)))


//sim-outorder.cc  line2723
#define MAX_STD_UNKNOWNS		64


//sim-outorder.cc  line3048
/* integer register file */
#define R_BMAP_SZ       (BITMAP_SIZE(MD_NUM_IREGS))
/* floating point register file */
#define F_BMAP_SZ       (BITMAP_SIZE(MD_NUM_FREGS))
/* miscellaneous registers */
#define C_BMAP_SZ       (BITMAP_SIZE(MD_NUM_CREGS))

//sim-outorder.cc  line3185
#define HASH_ADDR(ADDR)							\
  ((((ADDR) >> 24)^((ADDR) >> 16)^((ADDR) >> 8)^(ADDR)) & (STORE_HASH_SIZE-1))


//sim-outorder.cc line3550
#define SET_NPC(EXPR)           {runcontext->regs->regs_NPC = (EXPR);\
		thispower->gpp_data->regs_para->total_iregs_write_num++;}
/* target program counter */
#undef  SET_TPC
#define SET_TPC(EXPR)		{target_PC = (EXPR);\
		thispower->gpp_data->regs_para->total_iregs_write_num++;}
/* current program counter */
#define CPC                     (thispower->gpp_data->regs_para->total_iregs_read_num++,runcontext->regs->regs_PC)
#define SET_CPC(EXPR)           (thispower->gpp_data->regs_para->total_iregs_write_num++,runcontext->regs->regs_PC = (EXPR))
/* general purpose register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define GPR(N)                  (thispower->gpp_data->regs_para->total_iregs_read_num++,\
		BITMAP_SET_P(runcontext->use_spec_R, R_BMAP_SZ, (N))\
				 ? runcontext->spec_regs_R[N]                       \
				 : runcontext->regs->regs_R[N]													\
		)
#define SET_GPR(N,EXPR)         (thispower->gpp_data->regs_para->total_iregs_read_num++,\
		spec_mode				\
				 ? ((runcontext->spec_regs_R[N] = (EXPR)),		\
				    BITMAP_SET(runcontext->use_spec_R, R_BMAP_SZ, (N)),\
				    runcontext->spec_regs_R[N])			\
				 : (runcontext->regs->regs_R[N] = (EXPR))					\
		)


//sim-outorder.cc line3724
#define __READ_SPECMEM(SRC, SRC_V, FAULT)				\
  (addr = (SRC),							\
   (spec_mode								\
    ? ((FAULT) = spec_mem_access(runcontext->mem, Read, addr, &SRC_V, sizeof(SRC_V)))\
    : ((FAULT) = mem_access(runcontext->mem, Read, addr, &SRC_V, sizeof(SRC_V)))),	\
   SRC_V)

#define READ_BYTE(SRC, FAULT)						\
  __READ_SPECMEM((SRC), temp_byte, (FAULT))
#define READ_HALF(SRC, FAULT)						\
  MD_SWAPH(__READ_SPECMEM((SRC), temp_half, (FAULT)))
#define READ_WORD(SRC, FAULT)						\
  MD_SWAPW(__READ_SPECMEM((SRC), temp_word, (FAULT)))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)						\
  MD_SWAPQ(__READ_SPECMEM((SRC), temp_qword, (FAULT)))
#endif /* HOST_HAS_QWORD */


#define __WRITE_SPECMEM(SRC, DST, DST_V, FAULT)				\
  (DST_V = (SRC), addr = (DST),						\
   (spec_mode								\
    ? ((FAULT) = spec_mem_access(runcontext->mem, Write, addr, &DST_V, sizeof(DST_V)))\
    : ((FAULT) = mem_access(runcontext->mem, Write, addr, &DST_V, sizeof(DST_V)))))

#define WRITE_BYTE(SRC, DST, FAULT)					\
  __WRITE_SPECMEM((SRC), (DST), temp_byte, (FAULT))
#define WRITE_HALF(SRC, DST, FAULT)					\
  __WRITE_SPECMEM(MD_SWAPH(SRC), (DST), temp_half, (FAULT))
#define WRITE_WORD(SRC, DST, FAULT)					\
  __WRITE_SPECMEM(MD_SWAPW(SRC), (DST), temp_word, (FAULT))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)					\
  __WRITE_SPECMEM(MD_SWAPQ(SRC), (DST), temp_qword, (FAULT))
#endif /* HOST_HAS_QWORD */

///* system call handler macro */
//#define SYSCALL(INST)							\
//  (/* only execute system calls in non-speculative mode */		\
//   (spec_mode ? panic("speculative syscall") : (void) 0),		\
//   sys_syscall(runcontext->regs, &TMIPS::mem_access, runcontext->mem, INST, TRUE))

/*typedef of machine.h(undependent with ISA)which used in libexo.h*/
//typedef qword_t exo_address_t;
//typedef qword_t exo_integer_t;
//typedef double exo_float_t;


/*main.cc*/
#define NICE_DEFAULT_VALUE		0

/* text-based stat profiles */
#define MAX_PCSTAT_VARS 8
//执行单元顺序
#define FU_IALU_INDEX			0
#define FU_IMULT_INDEX			1
#define FU_MEMPORT_INDEX		2
#define FU_FPALU_INDEX			3
#define FU_FPMULT_INDEX			4


#endif /* SIM_OODEF_H_ */

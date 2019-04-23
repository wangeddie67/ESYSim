/*
 * memory.h - flat memory space interfaces
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
 * $Id: memory.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: memory.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:24  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
 * dir structure
 *
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.8  1999/12/31 18:38:54  taustin
 * cross-endian execution support added
 * quad_t naming conflicts removed
 *
 * Revision 1.7  1999/12/13 18:43:54  taustin
 * cross endian execution support added
 *
 * Revision 1.6  1998/08/27 15:39:47  taustin
 * implemented host interface description in host.h
 * added target interface support
 * memory module updated to support 64/32 bit address spaces on 64/32
 *       bit machines, now implemented with a dynamically optimized hashed
 *       page table
 * added support for qword's
 * added fault support
 * added support for register and memory contexts
 *
 * Revision 1.5  1997/03/11  01:16:23  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 * major macro reorganization to support CC portability
 * mem_valid() added, indicates if an address is bogus, used by DLite!
 *
 * Revision 1.4  1997/01/06  16:01:24  taustin
 * HIDE_MEM_TABLE_DEF added to help with sim-fast.c compilation
 *
 * Revision 1.3  1996/12/27  15:53:15  taustin
 * updated comments
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 *
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "options.h"
#include "stats.h"

#include "regs.h"

#include "smt.h"
#include "MTA.h"

/* number of entries in page translation hash table (must be power-of-two) */
#define MEM_PTAB_SIZE		(32*1024)
#define MEM_LOG_PTAB_SIZE	15
#define DEFINE_WB 1
#define TESTWB 0
#define WBSIZE 16

/* page table entry */
struct mem_pte_t {
  struct mem_pte_t *next;	/* next translation in this bucket */
  md_addr_t tag;		/* virtual page number tag */
  byte_t *page;			/* page pointer */
};

/* memory object */
struct mem_t {
  /* memory object state */
#ifdef SMT_SS
  int threadid;           /* this RUU instant belongs to which theard  */
#endif
  char *name;				/* name of this memory space */
  struct mem_pte_t *ptab[MEM_PTAB_SIZE];/* inverted page table */

  /* memory object stats */
  counter_t page_count;			/* total number of pages allocated */
  counter_t ptab_misses;		/* total first level page tbl misses */
  counter_t ptab_accesses;		/* total page table accesses */
};

/* memory access command */
enum mem_cmd {
  Read,			/* read memory from target (simulated prog) to host */
  Write			/* write memory from host (simulator) to target */
    
};

/* memory access function type, this is a generic function exported for the
   purpose of access the simulated vitual memory space */
typedef enum md_fault_type
(*mem_access_fn)(struct mem_t *mem,	/* memory space to access */
		 enum mem_cmd cmd,	/* Read or Write */
		 md_addr_t addr,	/* target memory address to access */
		 void *p,		/* where to copy to/from */
		 int nbytes, int threadID);		/* transfer length in bytes */

/*
 * virtual to host page translation macros
 */

/* compute page table set */
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


/*
 * memory accessors macros, fast but difficult to debug...
 */

/* safe version, works only with scalar types */
/* FIXME: write a more efficient GNU C expression for this... */
#define MEM_READ(MEM, ADDR, TYPE)					\
  (MEM_PAGE(MEM, (md_addr_t)(ADDR))					\
   ? *((TYPE *)(MEM_PAGE(MEM, (md_addr_t)(ADDR)) + MEM_OFFSET(ADDR)))	\
   : /* page not yet allocated, return zero value */ 0)

/* unsafe version, works with any type */
#define __UNCHK_MEM_READ(MEM, ADDR, TYPE)				\
  (*((TYPE *)(MEM_PAGE(MEM, (md_addr_t)(ADDR)) + MEM_OFFSET(ADDR))))

/* safe version, works only with scalar types */
/* FIXME: write a more efficient GNU C expression for this... */
#define MEM_WRITE(MEM, ADDR, TYPE, VAL)					\
  (MEM_TICKLE(MEM, (md_addr_t)(ADDR)),					\
   *((TYPE *)(MEM_PAGE(MEM, (md_addr_t)(ADDR)) + MEM_OFFSET(ADDR))) = (VAL))
      
/* unsafe version, works with any type */
#define __UNCHK_MEM_WRITE(MEM, ADDR, TYPE, VAL)				\
  (*((TYPE *)(MEM_PAGE(MEM, (md_addr_t)(ADDR)) + MEM_OFFSET(ADDR))) = (VAL))

#if PROCESS_MODEL
/* fast memory accessor macros, typed versions */
#define MEM_READ_BYTE(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, byte_t) \
     : MEM_READ(MEM, ADDR, byte_t) )
    
#define MEM_READ_SBYTE(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, sbyte_t) \
     : MEM_READ(MEM, ADDR, sbyte_t) )
    
#define MEM_READ_HALF(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MD_SWAPH(MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, half_t))\
     : MD_SWAPH(MEM_READ(MEM, ADDR, half_t)) )
    
#define MEM_READ_SHALF(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MD_SWAPH(MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, shalf_t))\
     : MD_SWAPH(MEM_READ(MEM, ADDR, shalf_t)) )
    
#define MEM_READ_WORD(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MD_SWAPW(MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, word_t))\
     : MD_SWAPW(MEM_READ(MEM, ADDR, word_t)) )
    
#define MEM_READ_SWORD(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MD_SWAPW(MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, sword_t))\
     : MD_SWAPW(MEM_READ(MEM, ADDR, sword_t)) )

#ifdef HOST_HAS_QWORD
#define MEM_READ_QWORD(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MD_SWAPQ(MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, qword_t))\
     : MD_SWAPQ(MEM_READ(MEM, ADDR, qword_t)) )
    
#define MEM_READ_SQWORD(MEM, ADDR)	\
    (checkForSharedAddr(ADDR) ? MD_SWAPQ(MEM_READ(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, sqword_t))\
     : MD_SWAPQ(MEM_READ(MEM, ADDR, sqword_t)) )
#endif /* HOST_HAS_QWORD */

#define MEM_WRITE_BYTE(MEM, ADDR, VAL) \
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, byte_t, VAL)  \
     : MEM_WRITE(MEM, ADDR, byte_t, VAL))
    
#define MEM_WRITE_SBYTE(MEM, ADDR, VAL)	\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, sbyte_t, VAL) \
     : MEM_WRITE(MEM, ADDR, sbyte_t, VAL) )
    
#define MEM_WRITE_HALF(MEM, ADDR, VAL) 					\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, half_t, MD_SWAPH(VAL)) \
     : MEM_WRITE(MEM, ADDR, half_t, MD_SWAPH(VAL)) )
    
#define MEM_WRITE_SHALF(MEM, ADDR, VAL)					\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, shalf_t, MD_SWAPH(VAL)) \
     : MEM_WRITE(MEM, ADDR, shalf_t, MD_SWAPH(VAL)) )
    
#define MEM_WRITE_WORD(MEM, ADDR, VAL)					\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, word_t, MD_SWAPW(VAL)) \
     : MEM_WRITE(MEM, ADDR, word_t, MD_SWAPW(VAL)) )
    
#define MEM_WRITE_SWORD(MEM, ADDR, VAL)					\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, sword_t, MD_SWAPW(VAL)) \
     : MEM_WRITE(MEM, ADDR, sword_t, MD_SWAPW(VAL)) )
    
#define MEM_WRITE_SFLOAT(MEM, ADDR, VAL)				\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, sfloat_t, MD_SWAPW(VAL)) \
     : MEM_WRITE(MEM, ADDR, sfloat_t, MD_SWAPW(VAL)) )
    
#define MEM_WRITE_DFLOAT(MEM, ADDR, VAL)				\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, dfloat_t, MD_SWAPQ(VAL)) \
     : MEM_WRITE(MEM, ADDR, dfloat_t, MD_SWAPQ(VAL)) )
    

#ifdef HOST_HAS_QWORD
#define MEM_WRITE_QWORD(MEM, ADDR, VAL)					\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, qword_t, MD_SWAPQ(VAL)) \
     : MEM_WRITE(MEM, ADDR, qword_t, MD_SWAPQ(VAL)) )
    
#define MEM_WRITE_SQWORD(MEM, ADDR, VAL)				\
    (checkForSharedAddr(ADDR) ? MEM_WRITE(thecontexts[thecontexts[MEM->threadid]->masterid]->mem, ADDR, sqword_t, MD_SWAPQ(VAL)) \
     : MEM_WRITE(MEM, ADDR, sqword_t, MD_SWAPQ(VAL)) )
    
#endif /* HOST_HAS_QWORD */
    
#else /* PROCESS_MODEL */
/* fast memory accessor macros, typed versions */
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

//#define MEM_WRITE_BYTE(MEM, ADDR, VAL) MEM_WRITE(MEM, ADDR, byte_t, VAL)

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



// #define MEM_READ_BYTE(MEM, ADDR)	MEM_READ(MEM, ADDR, byte_t); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// #define MEM_READ_SBYTE(MEM, ADDR)	MEM_READ(MEM, ADDR, sbyte_t); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// #define MEM_READ_HALF(MEM, ADDR)	MD_SWAPH(MEM_READ(MEM, ADDR, half_t)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// #define MEM_READ_SHALF(MEM, ADDR)	MD_SWAPH(MEM_READ(MEM, ADDR, shalf_t)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// #define MEM_READ_WORD(MEM, ADDR)	MD_SWAPW(MEM_READ(MEM, ADDR, word_t)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// #define MEM_READ_SWORD(MEM, ADDR)	MD_SWAPW(MEM_READ(MEM, ADDR, sword_t)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// 
// #ifdef HOST_HAS_QWORD
// #define MEM_READ_QWORD(MEM, ADDR)	MD_SWAPQ(MEM_READ(MEM, ADDR, qword_t)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// #define MEM_READ_SQWORD(MEM, ADDR)	MD_SWAPQ(MEM_READ(MEM, ADDR, sqword_t)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(0, ADDR,  MEM, 0);
// #endif /* HOST_HAS_QWORD */
// 
// #define MEM_WRITE_BYTE(MEM, ADDR, VAL)	MEM_WRITE(MEM, ADDR, byte_t, VAL); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// 
// //#define MEM_WRITE_BYTE(MEM, ADDR, VAL) MEM_WRITE(MEM, ADDR, byte_t, VAL);
// 
// #define MEM_WRITE_SBYTE(MEM, ADDR, VAL)	MEM_WRITE(MEM, ADDR, sbyte_t, VAL); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// #define MEM_WRITE_HALF(MEM, ADDR, VAL)					\
// 				MEM_WRITE(MEM, ADDR, half_t, MD_SWAPH(VAL)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// #define MEM_WRITE_SHALF(MEM, ADDR, VAL)					\
// 				MEM_WRITE(MEM, ADDR, shalf_t, MD_SWAPH(VAL)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// #define MEM_WRITE_WORD(MEM, ADDR, VAL)					\
// 				MEM_WRITE(MEM, ADDR, word_t, MD_SWAPW(VAL)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// #define MEM_WRITE_SWORD(MEM, ADDR, VAL)					\
// 				MEM_WRITE(MEM, ADDR, sword_t, MD_SWAPW(VAL)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// #define MEM_WRITE_SFLOAT(MEM, ADDR, VAL)				\
// 				MEM_WRITE(MEM, ADDR, sfloat_t, MD_SWAPW(VAL)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// #define MEM_WRITE_DFLOAT(MEM, ADDR, VAL)				\
// 				MEM_WRITE(MEM, ADDR, dfloat_t, MD_SWAPQ(VAL)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// 
// #ifdef HOST_HAS_QWORD
// #define MEM_WRITE_QWORD(MEM, ADDR, VAL)					\
// 				MEM_WRITE(MEM, ADDR, qword_t, MD_SWAPQ(VAL)); if((ADDR & ~7) == 0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);
// #define MEM_WRITE_SQWORD(MEM, ADDR, VAL)				\
// 				MEM_WRITE(MEM, ADDR, sqword_t, MD_SWAPQ(VAL)); if((ADDR & ~7) ==  0x11ff660c8) whatTheFuck(VAL, ADDR,  MEM, 1);


#endif /* HOST_HAS_QWORD */
#endif /* PROCESS_MODEL */
/* create a flat memory space */
struct mem_t *
mem_create(char *name);			/* name of the memory space */
	   
/* translate address ADDR in memory space MEM, returns pointer to host page */
byte_t *
mem_translate(struct mem_t *mem,	/* memory space to access */
	      md_addr_t addr);		/* virtual address to translate */

/* allocate a memory page */
void
mem_newpage(struct mem_t *mem,		/* memory space to allocate in */
	    md_addr_t addr);		/* virtual address to allocate */

/* generic memory access function, it's safe because alignments and permissions
   are checked, handles any natural transfer sizes; note, faults out if nbytes
   is not a power-of-two or larger then MD_PAGE_SIZE */
enum md_fault_type
mem_access(struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   md_addr_t addr,		/* target address to access */
	   void *vp,			/* host memory address to access */
	   int nbytes, int threadID);			/* number of bytes to access */

/* register memory system-specific statistics */
void
mem_reg_stats(struct mem_t *mem,	/* memory space to declare */
	      struct stat_sdb_t *sdb);	/* stats data base */

/* initialize memory system, call before loader.c */
void
mem_init(struct mem_t *mem);	/* memory space to initialize */

/* dump a block of memory, returns any faults encountered */
enum md_fault_type
mem_dump(struct mem_t *mem,		/* memory space to display */
	 md_addr_t addr,		/* target address to dump */
	 int len,			/* number bytes to dump */
	 FILE *stream);			/* output stream */


/*
 * memory accessor routines, these routines require a memory access function
 * definition to access memory, the memory access function provides a "hook"
 * for programs to instrument memory accesses, this is used by various
 * simulators for various reasons; for the default operation - direct access
 * to the memory system, pass mem_access() as the memory access function
 */

/* copy a '\0' terminated string to/from simulated memory space, returns
   the number of bytes copied, returns any fault encountered */
enum md_fault_type
mem_strcpy(mem_access_fn mem_fn,	/* user-specified memory accessor */
	   struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   md_addr_t addr,		/* target address to access */
	   char *s, int threadID);			/* host memory string buffer */

/* copy NBYTES to/from simulated memory space, returns any faults */
enum md_fault_type
mem_bcopy(mem_access_fn mem_fn,		/* user-specified memory accessor */
	  struct mem_t *mem,		/* memory space to access */
	  enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	  md_addr_t addr,		/* target address to access */
	  void *vp,			/* host memory address to access */
	  int nbytes, int threadID);			/* number of bytes to access */

/* copy NBYTES to/from simulated memory space, NBYTES must be a multiple
   of 4 bytes, this function is faster than mem_bcopy(), returns any
   faults encountered */
enum md_fault_type
mem_bcopy4(mem_access_fn mem_fn,	/* user-specified memory accessor */
	   struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   md_addr_t addr,		/* target address to access */
	   void *vp,			/* host memory address to access */
	   int nbytes, int threadID);			/* number of bytes to access */

/* zero out NBYTES of simulated memory, returns any faults encountered */
enum md_fault_type
mem_bzero(mem_access_fn mem_fn,		/* user-specified memory accessor */
	  struct mem_t *mem,		/* memory space to access */
	  md_addr_t addr,		/* target address to access */
	  int nbytes, int threadID);			/* number of bytes to clear */

#endif /* MEMORY_H */

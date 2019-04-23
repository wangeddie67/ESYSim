/*
 * eio.c - external interfaces to external I/O f\iles
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1997, 1998 by Todd M. Austin
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
 * $Id: eio.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: eio.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:12  xue
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
 * Revision 1.1.1.1  2000/05/26 15:21:52  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.5  1999/12/31 18:34:24  taustin
 * event tracing (EIO) now works for traces longer than 2^32 insts
 *
 * Revision 1.4  1999/12/13 18:39:11  taustin
 * cross endian execution support added
 *
 * Revision 1.3  1998/09/03 22:23:24  taustin
 * fixed MACRO bug for Intel Reference Compiler (icc)
 *
 * Revision 1.2  1998/08/31 17:07:35  taustin
 * added MS VC++ headers, fixed bug in call to write()
 *
 * Revision 1.1  1998/08/27 08:20:31  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include <io.h>
#else /* !_MSC_VER */
#include <unistd.h>
#endif

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "libexo/libexo.h"
#include "syscall.h"
#include "sim.h"
#include "endian.h"
#include "eio.h"

#include "smt.h"

#ifdef SMT_SS
#include "context.h"
#endif

#ifdef _MSC_VER
#define write		_write
#endif

#define EIO_FILE_HEADER							\
  "/* This is a SimpleScalar EIO file - DO NOT MOVE OR EDIT THIS LINE! */\n"
/*
   EIO transaction format:

   (inst_count, pc,
    ... reg inputs ...
    [r2, r3, r4, r5, r6, r7],
    ... mem inputs ...
    ((addr, size, blob), ...)
    ... reg outputs ...
    [r2, r3, r4, r5, r6, r7],
    ... mem outputs ...
    ((addr, size, blob), ...)
   )
*/

/* EIO transaction count, i.e., number of last transaction completed */
static counter_t eio_trans_icnt = -1;

FILE *
eio_create(char *fname)
{
  FILE *fd;
  struct exo_term_t *exo;
  int target_big_endian;

#ifdef SMT_SS
  int threadid = 0;
  context *current;
  current = thecontexts[threadid];
#endif


  target_big_endian = (endian_host_byte_order() == endian_big);

  fd = gzopen(fname, "w");
  if (!fd)
    fatal("unable to create EIO file `%s'", fname);

  /* emit EIO file header */
  fprintf(fd, "%s\n", EIO_FILE_HEADER);
#ifdef SMT_SS
  fprintf(fd, "/*thread %d, file_format: %d, file_version: %d, big_endian: %d */\n",threadid,
	  MD_EIO_FILE_FORMAT, EIO_FILE_VERSION, current->ld_target_big_endian);
#else
  fprintf(fd, "/* file_format: %d, file_version: %d, big_endian: %d */\n",
	  MD_EIO_FILE_FORMAT, EIO_FILE_VERSION, ld_target_big_endian);
#endif
  exo = exo_new(ec_list,
		exo_new(ec_integer, (exo_integer_t)MD_EIO_FILE_FORMAT),
		exo_new(ec_integer, (exo_integer_t)EIO_FILE_VERSION),
		exo_new(ec_integer, (exo_integer_t)target_big_endian),
		NULL);
  exo_print(exo, fd);
  exo_delete(exo);
  fprintf(fd, "\n\n");

  return fd;
}

FILE *
eio_open(char *fname)
{
  FILE *fd;
  struct exo_term_t *exo;
  int file_format, file_version, big_endian, target_big_endian;

  target_big_endian = (endian_host_byte_order() == endian_big);

  fd = gzopen(fname, "r");
  if (!fd)
    fatal("unable to open EIO file `%s'", fname);

  /* read and check EIO file header */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_integer
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || !exo->as_list.head->next->next
      || exo->as_list.head->next->next->ec != ec_integer
      || exo->as_list.head->next->next->next != NULL)
    fatal("could not read EIO file header");

  file_format = exo->as_list.head->as_integer.val;
  file_version = exo->as_list.head->next->as_integer.val;
  big_endian = exo->as_list.head->next->next->as_integer.val;
  exo_delete(exo);

  if (file_format != MD_EIO_FILE_FORMAT)
    fatal("EIO file `%s' has incompatible format", fname);

  if (file_version != EIO_FILE_VERSION)
    fatal("EIO file `%s' has incompatible version", fname);

  if (!!big_endian != !!target_big_endian)
    {
      warn("endian of `%s' does not match host", fname);
      warn("running with experimental cross-endian execution support");
      warn("****************************************");
      warn("**>> please check results carefully <<**");
      warn("****************************************");
    }

  return fd;
}

/* returns non-zero if file FNAME has a valid EIO header */
int
eio_valid(char *fname)
{
  FILE *fd;
  char buf[512];

  /* open possible EIO file */
  fd = gzopen(fname, "r");
  if (!fd)
    return FALSE;

  /* read and check EIO file header */
  fgets(buf, 512, fd);

  /* check the header */
  if (strcmp(buf, EIO_FILE_HEADER))
    return FALSE;

  /* all done, close up file */
  gzclose(fd);

  /* else, has a valid header, go with it... */
  return TRUE;
}

void
eio_close(FILE *fd)
{
  gzclose(fd);
}

/* check point current architected state to stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t
eio_write_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd)			/* stream to write to */
{
  int i;
  struct exo_term_t *exo;
  struct mem_pte_t *pte;

#ifdef SMT_SS
  int threadid = mem->threadid;
  context *current;
  current = thecontexts[threadid];
#endif

  myfprintf(fd, "/* ** start checkpoint @ %n... */\n\n", eio_trans_icnt);

  myfprintf(fd, "/* EIO file pointer: %n... */\n", eio_trans_icnt);
  exo = exo_new(ec_integer, (exo_integer_t)eio_trans_icnt);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  /* dump misc regs: icnt, PC, NPC, etc... */
  fprintf(fd, "/* misc regs icnt, PC, NPC, etc... */\n");
  exo = MD_MISC_REGS_TO_EXO(regs);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  /* dump integer registers */
  fprintf(fd, "/* integer regs */\n");
  exo = exo_new(ec_list, NULL);
  for (i=0; i < MD_NUM_IREGS; i++)
    exo->as_list.head = exo_chain(exo->as_list.head, MD_IREG_TO_EXO(regs, i));
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  /* dump FP registers */
  fprintf(fd, "/* FP regs (integer format) */\n");
  exo = exo_new(ec_list, NULL);
  for (i=0; i < MD_NUM_FREGS; i++)
    exo->as_list.head = exo_chain(exo->as_list.head, MD_FREG_TO_EXO(regs, i));
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* writing `%d' memory pages... */\n", (int)mem->page_count);
#ifdef SMT_SS
  exo = exo_new(ec_list,
		exo_new(ec_integer, (exo_integer_t)mem->page_count),
		exo_new(ec_address, (exo_integer_t)current->ld_brk_point),
		exo_new(ec_address, (exo_integer_t)current->ld_stack_min),
		NULL);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* text segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)current->ld_text_base),
		exo_new(ec_integer, (exo_integer_t)current->ld_text_size),
		NULL);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* data segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)current->ld_data_base),
		exo_new(ec_integer, (exo_integer_t)current->ld_data_size),
		NULL);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* stack segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)current->ld_stack_base),
		exo_new(ec_integer, (exo_integer_t)current->ld_stack_size),
		NULL);
#else
  exo = exo_new(ec_list,
		exo_new(ec_integer, (exo_integer_t)mem->page_count),
		exo_new(ec_address, (exo_integer_t)ld_brk_point),
		exo_new(ec_address, (exo_integer_t)ld_stack_min),
		NULL);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* text segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)ld_text_base),
		exo_new(ec_integer, (exo_integer_t)ld_text_size),
		NULL);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* data segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)ld_data_base),
		exo_new(ec_integer, (exo_integer_t)ld_data_size),
		NULL);
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* stack segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)ld_stack_base),
		exo_new(ec_integer, (exo_integer_t)ld_stack_size),
		NULL);
#endif
  exo_print(exo, fd);
  fprintf(fd, "\n\n");
  exo_delete(exo);

  /* visit all active memory pages, and dump them to the checkpoint file */
  MEM_FORALL(mem, i, pte)
    {
      /* dump this page... */
      exo = exo_new(ec_list,
		    exo_new(ec_address, (exo_integer_t)MEM_PTE_ADDR(pte, i)),
		    exo_new(ec_blob, MD_PAGE_SIZE, pte->page),
		    NULL);
      exo_print(exo, fd);
      fprintf(fd, "\n\n");
      exo_delete(exo);
    }

  myfprintf(fd, "/* ** end checkpoint @ %n... */\n\n", eio_trans_icnt);

  return eio_trans_icnt;
}

/* read check point of architected state from stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t
eio_read_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd)			/* stream to read */
{
  int i, page_count;
  counter_t trans_icnt;
  struct exo_term_t *exo, *elt;

#ifdef SMT_SS
  int threadid = mem->threadid;
  context *current;
  current = thecontexts[threadid];
#endif

  /* read the EIO file pointer */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_integer)
    fatal("could not read EIO file pointer");
  trans_icnt = exo->as_integer.val;
  exo_delete(exo);

  /* read misc regs: icnt, PC, NPC, HI, LO, FCC */
  exo = exo_read(fd);
  MD_EXO_TO_MISC_REGS(exo, sim_num_insn, regs);
  exo_delete(exo);

  /* read integer registers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list)
    fatal("could not read EIO integer regs");
  elt = exo->as_list.head;
  for (i=0; i < MD_NUM_IREGS; i++)
    {
      if (!elt)
	fatal("could not read EIO integer regs (too few)");

      if (elt->ec != ec_address)
	fatal("could not read EIO integer regs (bad value)");

      MD_EXO_TO_IREG(elt, regs, i);
      elt = elt->next;
    }
  if (elt != NULL)
    fatal("could not read EIO integer regs (too many)");
  exo_delete(exo);

  /* read FP registers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list)
    fatal("could not read EIO FP regs");
  elt = exo->as_list.head;
  for (i=0; i < MD_NUM_FREGS; i++)
    {
      if (!elt)
	fatal("could not read EIO FP regs (too few)");

      if (elt->ec != ec_address)
	fatal("could not read EIO FP regs (bad value)");

      MD_EXO_TO_FREG(elt, regs, i);
      elt = elt->next;
    }
  if (elt != NULL)
    fatal("could not read EIO FP regs (too many)");
  exo_delete(exo);

#ifdef SMT_SS
  /* read the number of page defs, and memory config */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_integer
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_address
      || !exo->as_list.head->next->next
      || exo->as_list.head->next->next->ec != ec_address
      || exo->as_list.head->next->next->next != NULL)
    fatal("could not read EIO memory page count");
  page_count = exo->as_list.head->as_integer.val;
  current->ld_brk_point = (md_addr_t)exo->as_list.head->next->as_address.val;
  current->ld_stack_min = (md_addr_t)exo->as_list.head->next->next->as_address.val;
  exo_delete(exo);

  /* read text segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO text segment specifiers");
  current->ld_text_base = (md_addr_t)exo->as_list.head->as_address.val;
  current->ld_text_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);

  /* read data segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO data segment specifiers");
  current->ld_data_base = (md_addr_t)exo->as_list.head->as_address.val;
  current->ld_data_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);

  /* read stack segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO stack segment specifiers");
  current->ld_stack_base = (md_addr_t)exo->as_list.head->as_address.val;
  current->ld_stack_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);
#else
  /* read the number of page defs, and memory config */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_integer
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_address
      || !exo->as_list.head->next->next
      || exo->as_list.head->next->next->ec != ec_address
      || exo->as_list.head->next->next->next != NULL)
    fatal("could not read EIO memory page count");
  page_count = exo->as_list.head->as_integer.val;
  ld_brk_point = (md_addr_t)exo->as_list.head->next->as_address.val;
  ld_stack_min = (md_addr_t)exo->as_list.head->next->next->as_address.val;
  exo_delete(exo);

  /* read text segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO text segment specifiers");
  ld_text_base = (md_addr_t)exo->as_list.head->as_address.val;
  ld_text_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);

  /* read data segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO data segment specifiers");
  ld_data_base = (md_addr_t)exo->as_list.head->as_address.val;
  ld_data_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);

  /* read stack segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO stack segment specifiers");
  ld_stack_base = (md_addr_t)exo->as_list.head->as_address.val;
  ld_stack_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);
#endif

  for (i=0; i < page_count; i++)
    {
      int j;
      md_addr_t page_addr;
      struct exo_term_t *blob;

      /* read the page */
      exo = exo_read(fd);
      if (!exo
	  || exo->ec != ec_list
	  || !exo->as_list.head
	  || exo->as_list.head->ec != ec_address
	  || !exo->as_list.head->next
	  || exo->as_list.head->next->ec != ec_blob
	  || exo->as_list.head->next->next != NULL)
	fatal("could not read EIO memory page");
      page_addr = (md_addr_t)exo->as_list.head->as_address.val;
      blob = exo->as_list.head->next;

      /* write data to simulator memory */
      for (j=0; j < blob->as_blob.size; j++)
	{
	  byte_t val;

	  val = blob->as_blob.data[j];
	  /* unchecked access... */
	  MEM_WRITE_BYTE(mem, page_addr, val);
	  page_addr++;
	}
      exo_delete(exo);
    }

  return trans_icnt;
}

struct mem_rec_t {
  md_addr_t addr;
  unsigned size, maxsize;
  struct exo_term_t *exo;
  struct exo_term_t *blob;
};

/* reg recs */
static struct exo_term_t *input_regs;
static struct exo_term_t *output_regs;

/* input memory recs */
static struct exo_term_t *input_mem;
static struct mem_rec_t input_mem_rec;

/* output memory recs */
static struct exo_term_t *output_mem;
static struct mem_rec_t output_mem_rec;

static int seen_write;
static mem_access_fn local_mem_fn;

/* size of padding that can be filled on the end of a blob tail */
#define BLOB_TAIL_SIZE		256

/* tracing memory access function */
static enum md_fault_type
my_mem_fn(struct mem_t *mem,		/* memory space to access */
	  enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	  md_addr_t addr,		/* target address to access */
	  void *vp,			/* host memory address to access */
	  int nbytes)			/* number of bytes to access */
{
  int i;
  unsigned char *p = vp;
  struct mem_rec_t *mem_rec = NULL;
  struct exo_term_t *mem_list = NULL;
  enum md_fault_type fault = md_fault_none;

  if (cmd == Read && seen_write)
    fatal("Read after Write in eio_syscall()");

  if (cmd == Write)
    seen_write = TRUE;

  /* record the memory access */
  if (cmd == Read)
    {
      mem_rec = &input_mem_rec;
      mem_list = input_mem;
    }
  else if (cmd == Write)
    {
      mem_rec = &output_mem_rec;
      mem_list = output_mem;
    }
  else
    panic("bogus memory access command");

  /* perform the memory access, Read's first so we can probe *p for data */
  if (cmd == Read /* simulator memory */)
    fault = (*local_mem_fn)(mem, cmd, addr, p, nbytes, mem->threadid);

  /* the following freakish code simply coalesces subsequent reads and
     writes to memory into the same EXO blob structure, this greatly
     reduces the size of the EIO output files... */
  if (mem_rec->exo != NULL
      && (mem_rec->addr + mem_rec->size == addr)
      && (mem_rec->size + nbytes < mem_rec->maxsize))
    {
      /* add to last blob */
      for (i=0; i < nbytes; i++)
	mem_rec->blob->as_blob.data[mem_rec->size + i] = p[i];
      mem_rec->size += nbytes;
      mem_rec->blob->as_blob.size = mem_rec->size;
    }
  else
    {
      /* add to a new blob */
      mem_list->as_list.head =
	exo_chain(mem_list->as_list.head,
		  (mem_rec->exo =
		   exo_new(ec_list,
			   exo_new(ec_address, (exo_integer_t)addr),
			   (mem_rec->blob =
			    exo_new(ec_blob, nbytes + BLOB_TAIL_SIZE, NULL)),
			   NULL)));
      for (i=0; i < nbytes; i++)
	mem_rec->blob->as_blob.data[i] = p[i];
      mem_rec->addr = addr;
      mem_rec->size = nbytes;
      mem_rec->maxsize = nbytes + BLOB_TAIL_SIZE;
      mem_rec->blob->as_blob.size = mem_rec->size;
    }

  /* perform the memory access */
  if (cmd == Write /* simulator memory */)
    fault = (*local_mem_fn)(mem, cmd, addr, p, nbytes, mem->threadid);

  return fault;
}

/* syscall proxy handler, with EIO tracing support, architect registers
   and memory are assumed to be precise when this function is called,
   register and memory are updated with the results of the sustem call */
void
eio_write_trace(FILE *eio_fd,			/* EIO stream file desc */
		counter_t icnt,			/* instruction count */
		struct regs_t *regs,		/* registers to update */
		mem_access_fn mem_fn,		/* generic memory accessor */
		struct mem_t *mem,		/* memory to update */
		md_inst_t inst)			/* system call inst */
{
  int i;
  struct exo_term_t *exo;

#ifdef SMT_SS
  int threadid = mem->threadid;
  context *current;
  current = thecontexts[threadid];
#endif

  /* write syscall register inputs ($r2..$r7) */
  input_regs = exo_new(ec_list, NULL);
  for (i=MD_FIRST_IN_REG; i <= MD_LAST_IN_REG; i++)
    {
      input_regs->as_list.head =
	exo_chain(input_regs->as_list.head, MD_IREG_TO_EXO(regs, i));
    }

  /* initialize memory inputs */
  input_mem = exo_new(ec_list, NULL); input_mem_rec.exo = NULL;
  output_mem = exo_new(ec_list, NULL); output_mem_rec.exo = NULL;

  /* perform the system call, record inputs and outputs */
  seen_write = FALSE;
  local_mem_fn = mem_fn;

#ifdef SMT_SS
  if (current->sim_eio_fd != NULL)
    eio_read_trace(current->sim_eio_fd, icnt, regs, my_mem_fn, mem, inst);
#else
  if (sim_eio_fd != NULL)
    eio_read_trace(sim_eio_fd, icnt, regs, my_mem_fn, mem, inst);
#endif
  else
    {
      sys_syscall(regs, my_mem_fn, mem, inst, FALSE);
    }

  /* write syscall breakpoint and register outputs ($r2..$r7) */
  output_regs = exo_new(ec_list, NULL);
#ifdef SMT_SS
  output_regs->as_list.head =
	exo_chain(output_regs->as_list.head,
		  exo_new(ec_address, (exo_integer_t)current->ld_brk_point));
#else
  output_regs->as_list.head =
	exo_chain(output_regs->as_list.head,
		  exo_new(ec_address, (exo_integer_t)ld_brk_point));
#endif
  for (i=MD_FIRST_OUT_REG; i <= MD_LAST_OUT_REG; i++)
    {
      output_regs->as_list.head =
	exo_chain(output_regs->as_list.head, MD_IREG_TO_EXO(regs, i));
    }

  /* write the whole enchalada to output stream */
  exo = exo_new(ec_list,
		/* icnt */exo_new(ec_integer, (exo_integer_t)icnt),
		/* PC */exo_new(ec_address, (exo_integer_t)regs->regs_PC),
		input_regs, input_mem,
		output_regs, output_mem,
		NULL);
  exo_print(exo, eio_fd);
  fprintf(eio_fd, "\n\n");

  /* release input storage */
  exo_delete(exo);

  /* one more transaction processed */
  eio_trans_icnt = icnt;
}

/* syscall proxy handler from an EIO trace, architect registers
   and memory are assumed to be precise when this function is called,
   register and memory are updated with the results of the sustem call */
void
eio_read_trace(FILE *eio_fd,			/* EIO stream file desc */
	       counter_t icnt,			/* instruction count */
	       struct regs_t *regs,		/* registers to update */
	       mem_access_fn mem_fn,		/* generic memory accessor */
	       struct mem_t *mem,		/* memory to update */
	       md_inst_t inst)			/* system call inst */
{
  int i;
  struct exo_term_t *exo, *exo_icnt, *exo_pc;
  struct exo_term_t *exo_inregs, *exo_inmem, *exo_outregs, *exo_outmem;
  struct exo_term_t *brkrec, *regrec, *memrec;

#ifdef SMT_SS
  int threadid = mem->threadid;
  context *current;
  current = thecontexts[threadid];
#endif

  /* exit() system calls get executed for real... */
  if (MD_EXIT_SYSCALL(regs))
    {
      program_complete = 1;
      fflush(stdout);
      printf("exiting\n");
      return;
      sys_syscall(regs, mem_fn, mem, inst, FALSE);
      panic("returned from exit() system call");
    }

  /* else, read the external I/O (EIO) transaction */
  exo = exo_read(eio_fd);

  /* one more transaction processed */
  eio_trans_icnt = icnt;

  /* pull apart the EIO transaction (EXO format) */
  if (!exo
      || exo->ec != ec_list
      || !(exo_icnt = exo->as_list.head)
      || exo_icnt->ec != ec_integer
      || !(exo_pc = exo_icnt->next)
      || exo_pc->ec != ec_address
      || !(exo_inregs = exo_pc->next)
      || exo_inregs->ec != ec_list
      || !(exo_inmem = exo_inregs->next)
      || exo_inmem->ec != ec_list
      || !(exo_outregs = exo_inmem->next)
      || exo_outregs->ec != ec_list
      || !(exo_outmem = exo_outregs->next)
      || exo_outmem->ec != ec_list
      || exo_outmem->next != NULL)
    fatal("cannot read EIO transaction");

  /*
   * check the system call inputs
   */

  /* check ICNT input */
  if (icnt != (counter_t)exo_icnt->as_integer.val)
    fatal("EIO trace inconsistency: ICNT mismatch");

  /* check PC input */
  if (regs->regs_PC != (md_addr_t)exo_pc->as_integer.val)
    fatal("EIO trace inconsistency: PC mismatch");

  /* check integer register inputs */
  for (i=MD_FIRST_IN_REG, regrec=exo_inregs->as_list.head;
       i <= MD_LAST_IN_REG; i++, regrec=regrec->next)
    {
      if (!regrec || regrec->ec != ec_address)
	fatal("EIO trace inconsistency: missing input reg");

      if (MD_EXO_CMP_IREG(regrec, regs, i))
	fatal("EIO trace inconsistency: R[%d] input mismatch", i);
#ifdef VERBOSE
      fprintf(stderr, "** R[%d] checks out...\n", i);
#endif /* VERBOSE */
    }
  if (regrec != NULL)
    fatal("EIO trace inconsistency: too many input regs");

  /* check memory inputs */
  for (memrec=exo_inmem->as_list.head; memrec != NULL; memrec=memrec->next)
    {
      md_addr_t loc;
      struct exo_term_t *addr, *blob;

      /* check the mem transaction format */
      if (!memrec
	  || memrec->ec != ec_list
	  || !(addr = memrec->as_list.head)
	  || addr->ec != ec_address
	  || !(blob = addr->next)
	  || blob->ec != ec_blob
	  || blob->next != NULL)
	fatal("EIO trace inconsistency: bad memory transaction");

      for (loc=addr->as_integer.val, i=0; i < blob->as_blob.size; loc++,i++)
	{
	  unsigned char val;

	  /* was: val = MEM_READ_BYTE(loc); */
	  (*mem_fn)(mem, Read, loc, &val, sizeof(unsigned char), mem->threadid);

	  if (val != blob->as_blob.data[i])
	    fatal("EIO trace inconsistency: addr 0x%08p input mismatch", loc);

#ifdef VERBOSE
	  myfprintf(stderr, "** 0x%08p checks out...\n", loc);
#endif /* VERBOSE */
	}

      /* simulate view'able I/O */
      if (MD_OUTPUT_SYSCALL(regs))
	{
#ifdef SMT_SS
	  if (current->sim_progfd)
	    {
	      /* redirect program output to file */
	      fwrite(blob->as_blob.data, 1, blob->as_blob.size, current->sim_progfd);
	    }
#else
	  if (sim_progfd)
	    {
	      /* redirect program output to file */
	      fwrite(blob->as_blob.data, 1, blob->as_blob.size, sim_progfd);
	    }
#endif
	  else
	    {
	      /* write the output to stdout/stderr */
	      write(MD_STREAM_FILENO(regs),
		    blob->as_blob.data, blob->as_blob.size);
	    }
	}
    }

  /*
   * write system call outputs
   */

  /* adjust breakpoint */
  brkrec = exo_outregs->as_list.head;
  if (!brkrec || brkrec->ec != ec_address)
    fatal("EIO trace inconsistency: missing memory breakpoint");
#ifdef SMT_SS
  current->ld_brk_point = (md_addr_t)brkrec->as_integer.val;
#else
  ld_brk_point = (md_addr_t)brkrec->as_integer.val;
#endif

  /* write integer register outputs */
  for (i=MD_FIRST_OUT_REG, regrec=exo_outregs->as_list.head->next;
       i <= MD_LAST_OUT_REG; i++, regrec=regrec->next)
    {
      if (!regrec || regrec->ec != ec_address)
	fatal("EIO trace inconsistency: missing output reg");

      MD_EXO_TO_IREG(regrec, regs, i);

#ifdef VERBOSE
      fprintf(stderr, "** R[%d] written...\n", i);
#endif /* VERBOSE */
    }
  if (regrec != NULL)
    fatal("EIO trace inconsistency: too many output regs");

  /* write memory outputs */
  for (memrec=exo_outmem->as_list.head; memrec != NULL; memrec=memrec->next)
    {
      md_addr_t loc;
      struct exo_term_t *addr, *blob;

      /* check the mem transaction format */
      if (!memrec
	  || memrec->ec != ec_list
	  || !(addr = memrec->as_list.head)
	  || addr->ec != ec_address
	  || !(blob = addr->next)
	  || blob->ec != ec_blob
	  || blob->next != NULL)
	fatal("EIO trace icnonsistency: bad memory transaction");

      for (loc=addr->as_integer.val, i=0; i < blob->as_blob.size; loc++,i++)
	{
	  /* was: MEM_WRITE_BYTE(loc, blob->as_blob.data[i]); */
	  (*mem_fn)(mem, Write, loc, &blob->as_blob.data[i], sizeof(unsigned char), mem->threadid);

#ifdef VERBOSE
	  fprintf(stderr, "** 0x%08p written...\n", loc);
#endif /* VERBOSE */
	}
    }

  /* release the EIO EXO node */
  exo_delete(exo);
}

/* fast forward EIO trace EIO_FD to the transaction just after ICNT */
void
eio_fast_forward(FILE *eio_fd, counter_t icnt)
{
  struct exo_term_t *exo, *exo_icnt;

  do
    {
      /* read the next external I/O (EIO) transaction */
      exo = exo_read(eio_fd);

      if (!exo)
	fatal("could not fast forward to EIO checkpoint");

      /* one more transaction processed */
      eio_trans_icnt = icnt;

      /* pull apart the EIO transaction (EXO format) */
      if (!exo
	  || exo->ec != ec_list
	  || !(exo_icnt = exo->as_list.head)
	  || exo_icnt->ec != ec_integer)
	fatal("cannot read EIO transaction (during fast forward)");
    }
  while ((counter_t)exo_icnt->as_integer.val != icnt);

  /* found it! */
}

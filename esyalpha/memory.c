/*
 * memory.c - flat memory space routines
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
 * $Id: memory.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: memory.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:04  xue
 * CMP_NETWORK
 *
 * Revision 1.2  2009/05/18 19:21:17  garg
 * Fence implementation
 *
 * Revision 1.1.1.1  2008/10/20 15:44:54  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.1  2004/05/14 14:06:14  ninelson
 * Initial revision
 *
 * Revision 1.1.1.1  2003/10/20 13:58:02  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.8  1999/12/31 18:38:19  taustin
 * cross-endian execution support added
 * quad_t naming conflicts removed
 *
 * Revision 1.7  1999/12/13 18:40:18  taustin
 * block copies now byte length (more portable across targets)
 *
 * Revision 1.6  1998/08/27 15:38:28  taustin
 * implemented host interface description in host.h
 * added target interface support
 * memory module updated to support 64/32 bit address spaces on 64/32
 *       bit machines, now implemented with a dynamically optimized hashed
 *       page table
 * added support for qword's
 * added fault support
 *
 * Revision 1.5  1997/03/11  01:15:25  taustin
 * updated copyright
 * mem_valid() added, indicates if an address is bogus, used by DLite!
 * long/int tweaks made for ALPHA target support
 *
 * Revision 1.4  1997/01/06  16:00:51  taustin
 * stat_reg calls now do not initialize stat variable values
 *
 * Revision 1.3  1996/12/27  15:52:46  taustin
 * updated comments
 * integrated support for options and stats packages
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "options.h"
#include "stats.h"
#include "memory.h"
#include "context.h"

extern int COHERENT_CACHE;
extern int collect_stats, allForked;
extern int currentThrd;
extern md_addr_t TempInstr;
extern int ReadFromWB;



/* create a flat memory space */
struct mem_t *
mem_create(char *name)			/* name of the memory space */
{
  struct mem_t *mem;

  mem = calloc(1, sizeof(struct mem_t));
  if (!mem)
    fatal("out of virtual memory");

  mem->name = mystrdup(name);
  return mem;
}

/* translate address ADDR in memory space MEM, returns pointer to host page */
byte_t *
mem_translate(struct mem_t *mem,	/* memory space to access */
	      md_addr_t addr)		/* virtual address to translate */
{
  struct mem_pte_t *pte, *prev;

  /* got here via a first level miss in the page tables */
  mem->ptab_misses++; mem->ptab_accesses++;

  /* locate accessed PTE */
  for (prev=NULL, pte=mem->ptab[MEM_PTAB_SET(addr)];
       pte != NULL;
       prev=pte, pte=pte->next)
    {
      if (pte->tag == MEM_PTAB_TAG(addr))
	{
	  /* move this PTE to head of the bucket list */
	  if (prev)
	    {
	      prev->next = pte->next;
	      pte->next = mem->ptab[MEM_PTAB_SET(addr)];
	      mem->ptab[MEM_PTAB_SET(addr)] = pte;
	    }
	  return pte->page;
	}
    }

  /* no translation found, return NULL */
  return NULL;
}

/* allocate a memory page */
void
mem_newpage(struct mem_t *mem,		/* memory space to allocate in */
	    md_addr_t addr)		/* virtual address to allocate */
{
  byte_t *page;
  struct mem_pte_t *pte = NULL;

/*  if( ((unsigned long)mem == 1076572168 ) && ((unsigned long)addr == 536969216) )
  {
   printf("pass 5_1_1_3_2_1_5_1 \n");
  }
*/  /* see misc.c for details on the getcore() function */
  page = getcore(MD_PAGE_SIZE);

/*  if( ((unsigned long)mem == 1076572168 ) && ((unsigned long)addr == 536969216) )
  {
   printf("pass 5_1_1_3_2_1_5_2 \n");
  }
*/
  if (!page)
    fatal("out of virtual memory");

/*  if( ((unsigned long)mem == 1076572168 ) && ((unsigned long)addr == 536969216) )
  {
   printf("pass 5_1_1_3_2_1_5_3 \n");
  }
*/  /* generate a new PTE */
  pte = calloc(1, sizeof(struct mem_pte_t));
//  pte = malloc(sizeof(struct mem_pte_t));

/*   if( ((unsigned long)mem == 1076572168 ) && ((unsigned long)addr == 536969216) )
  {
   printf("pass 5_1_1_3_2_1_5_4 \n");
  }
*/
  if (!pte)
    fatal("out of virtual memory");
  pte->tag = MEM_PTAB_TAG(addr);
  pte->page = page;

/*  if( ((unsigned long)mem == 1076572168 ) && ((unsigned long)addr == 536969216) )
  {
   printf("pass 5_1_1_3_2_1_5_5 \n");
  }
*/
  /* insert PTE into inverted hash table */
  pte->next = mem->ptab[MEM_PTAB_SET(addr)];
  mem->ptab[MEM_PTAB_SET(addr)] = pte;

  /* one more page allocated */
  mem->page_count++;

}




enum md_fault_type 
mem_access_drct(struct mem_t *mem,		/* memory space to access */
	enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	md_addr_t addr,		/* target address to access */
	void *vp,			/* host memory address to access */
	int nbytes, int threadID )			/* number of bytes to access */
{

	context *current = thecontexts[threadID];

	byte_t *p = vp;
	enum md_fault_type retType;;
	
	if ((nbytes & (nbytes-1)) != 0 || nbytes > MD_PAGE_SIZE)
	{
		return md_fault_access;
	}
	if ((addr & (nbytes-1)) != 0)
	{
		fprintf(stderr, "Alignment Problem %llx %d\n", (unsigned long long)addr, (int)nbytes);
		fflush(stderr);
		return md_fault_alignment;
	}
	if (cmd == Read)
	{
		while (nbytes-- > 0)
		{
			*((byte_t *)p) = MEM_READ_BYTE(mem, addr);
			p += sizeof(byte_t);
			addr += sizeof(byte_t);
		}
	}
	else
	{
		while (nbytes-- > 0)
		{
			MEM_WRITE_BYTE(mem, addr, *((byte_t *)p));
			p += sizeof(byte_t);
			addr += sizeof(byte_t);
		}
	}
	return md_fault_none;
}


#define READFROMWB 1
#define READFROMCACHE 0

int ReadFrmWB(int threadID, md_addr_t addr, int nbytes)
{
	struct wb_mem_ent *ent;
	context *current = thecontexts[threadID];
	
	for (ent=current->WBtableHead; ent; ent=ent->next)
	{
		if(nbytes == 1)
		{
			if(ent->nbytes == nbytes)
			{
				if(ent->addr == addr)
				{
					return READFROMWB;
				}
			}
			else
			{
				if( (ent->addr <= addr) && ((ent->addr + nbytes) > addr))
				{
					fprintf(stderr,"One byte of data found in the write buffer 1 %d, %llx\n", current->id, current->regs.regs_PC);
					exit(0);
				}
			}
		}
		else if (nbytes == 2)
		{
			if(ent->nbytes == nbytes)
			{
				if(ent->addr == addr)
				{
					return READFROMWB;
				}
			}
			else if(ent->nbytes < nbytes)
			{
				if ( (ent->addr >= addr) && (ent->addr < (addr + nbytes)))
				{
					fprintf(stderr,"One byte of data found in the write buffer 2 %d, %llx\n", current->id, current->regs.regs_PC);
					exit(0);
				}
			}
			else
			{
				if( (addr >= ent->addr) && (addr < (ent->addr + ent->nbytes)))
				{
					fprintf(stderr,"One byte of data found in the write buffer 3 %d, %llx\n", current->id, current->regs.regs_PC);
					exit(0);
				}
			}
		
		}
		else if (nbytes == 4)
		{
			if(nbytes == ent->nbytes)
			{
				if(ent->addr == addr)
				{
					return READFROMWB;
				}
			}
			else if(ent->nbytes < nbytes)
			{
				if ( (ent->addr >= addr) && (ent->addr < (addr + nbytes)))
				{
					fprintf(stderr,"One byte of data found in the write buffer 4 %d, %llx\n", current->id, current->regs.regs_PC);
					exit(0);
				}
			}
			else
			{
				if(ent->addr == addr)
				{
					return READFROMWB;
			
				}
				else if(ent->addr + nbytes == addr)
				{
					return READFROMWB;
				}
			}
		}
		else if (nbytes == 8)
		{
			if(nbytes == ent->nbytes)
			{
				if(ent->addr == addr)
				{
					return READFROMWB;
				}
			}
			else if ( (ent->nbytes == 1) || (ent->nbytes == 2))
			{
				if((ent->addr >= addr) && (ent->addr < (addr + nbytes)))
				{
					fprintf(stderr,"One byte of data found in the write buffer 5 %d, %llx\n", current->id, current->regs.regs_PC);
					exit(0);
				}
			}
			else
			{
				if(ent->addr == addr)
				{
					return READFROMCACHE;
				}
				else if((ent->addr + 4) == addr)
				{
					return READFROMCACHE;
				}
			}
		}
		else
		{
			fprintf(stderr, " Number of bytes read is incorrect\n");
			exit(0);
		}
	}
	return READFROMCACHE;
}

/* exact read function */
void mem_partial_read(struct mem_t *mem, md_addr_t addr, void *p,int nbytes, int threadid)
{
	struct wb_mem_ent *ent = NULL, *prev = NULL;
	context *current = thecontexts[threadid];
	switch (nbytes)
	{
		case 1:
			*((byte_t *)p) = MEM_READ_BYTE(mem, addr);
			break;
		case 2:
			*((half_t *)p) = MEM_READ_HALF(mem, addr);
			break;
		case 4:
			*((word_t *)p) = MEM_READ_WORD(mem, addr);
			break;
		case 8:
			*((word_t *)p) = MEM_READ_WORD(mem, addr);
			*(((word_t *)p)+1) = MEM_READ_WORD(mem, addr + sizeof(word_t));
			break;
		default:
			panic("access size not supported in mis-speculative mode");
	}

	/* Now start reading from the write buffer */
	for (ent=current->WBtableTail; ent; ent=ent->prev)
	{
		int bigMemWidth = (ent->nbytes > nbytes) ? ent->nbytes : nbytes;
		md_addr_t addr_load, addr_ent;
		if(bigMemWidth != 1 && bigMemWidth != 2 && bigMemWidth != 4 && bigMemWidth != 8)
			panic("access size not supported in mis-speculative mode");
		if(bigMemWidth == 8)
		{
			addr_load = addr >> 3;
			addr_ent = ent->addr >> 3;
		}
		else if(bigMemWidth == 4)
		{
			addr_load = addr >> 2;
			addr_ent = ent->addr >> 2;
		}
		else if(bigMemWidth == 2)
		{
			addr_load = addr >> 1;
			addr_ent = ent->addr >> 1;
		}
		else if(bigMemWidth == 1)
		{
			addr_load = addr >> 0;
			addr_ent = ent->addr >> 0;
		}

		if(addr_load == addr_ent)
		{
			if(nbytes >= ent->nbytes)
			{
				int index = (int)((ent->addr & (nbytes - 1)) & ~(ent->nbytes - 1));
				memcpy(((byte_t *)p)+index, &ent->data[0] , ent->nbytes);
			}
			else
			{
				int index = (int)((addr & (ent->nbytes - 1)) & ~(nbytes - 1));
				memcpy(((byte_t *)p), ((byte_t *)ent->data)+index , nbytes);
			}
		}
	}
}

extern counter_t sim_cycle;
/*
   mem = memory space to access 
   cmd = Read or Write access cmd
   addr = virtual address of access
   p = input/output buffer
   nbutes = number of bytes to access
 */
enum md_fault_type mem_access(struct mem_t *mem, enum mem_cmd cmd, md_addr_t addr, void *p,int nbytes, int threadID)
{
	int i, index;
	struct wb_mem_ent *ent, *prev;
	struct wb_mem_ent *ent1, *prev1;
	context *current;
	md_addr_t entS, entF, readS, readF;
	int writeProb = 0;
	
	int threadid =  threadID;
	ent = NULL;
	prev = NULL;
	current = thecontexts[threadid];
	
	if(!(COHERENT_CACHE && allForked))
	{
		return mem_access_drct(mem, cmd, addr, p, nbytes, threadID);
	}

	/* check alignments, even speculative this test should always pass */
	if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
	{
		/* no can do, return zero result */
		for (i=0; i < nbytes; i++)
			((char *)p)[i] = 0;
			return md_fault_none;
	}

#if 0
	if(cmd == Read)
	{
		/* has this memory state been copied on mis-speculative write? */
		for (ent=current->WBtableHead; ent; ent=ent->next)
		{
			if(nbytes == 1)
			{
				if(ent->nbytes == nbytes)
				{
					if(ent->addr == addr)
					{
						ReadFromWB = 1;
						break;
					}
				}
				else
				{
					if( (ent->addr <= addr) && ((ent->addr + ent->nbytes) > addr))
					{
						flushWriteBuffer(current->id);
						*((byte_t *)p) = MEM_READ_BYTE(mem, addr);
						ReadFromWB = 0;
						return md_fault_none;
					}
				}
			}
			else if (nbytes == 2)
			{
				if(ent->nbytes == nbytes)
				{
					if(ent->addr == addr)
					{
						ReadFromWB = 1;
						break;
					}
				}
				else if(ent->nbytes < nbytes)
				{
					if ( (ent->addr >= addr) && (ent->addr < (addr + nbytes)))
					{
//						fprintf(stderr,"One byte of data found in the write buffer 2 %d, %llx\n", current->id, current->regs.regs_PC);
						flushWriteBuffer(current->id);
						*((half_t *)p) = MEM_READ_HALF(mem, addr);
						ReadFromWB = 0;
						return md_fault_none;
					}
				}
				else
				{
					if( (addr >= ent->addr) && (addr < (ent->addr + ent->nbytes)))
					{
//						fprintf(stderr,"One byte of data found in the write buffer 3 %d, %llx\n", current->id, current->regs.regs_PC);
						flushWriteBuffer(current->id);
						*((half_t *)p) = MEM_READ_HALF(mem, addr);
						ReadFromWB = 0;
						return md_fault_none;
					}
				}
			}
			else if (nbytes == 4)
			{
				if(nbytes == ent->nbytes)
				{
					if(ent->addr == addr)
					{
						ReadFromWB = 1;
						break;
					}
				}
				else if(ent->nbytes < nbytes)
				{
					if ( (ent->addr >= addr) && (ent->addr < (addr + nbytes)))
					{
//						fprintf(stderr,"One byte of data found in the write buffer 4 %d, %llx\n", current->id, current->regs.regs_PC);
						flushWriteBuffer(current->id);
						*((word_t *)p) = MEM_READ_WORD(mem, addr);
						ReadFromWB = 0;
						return md_fault_none;
					}
				}
				else
				{
					if(ent->addr == addr)
					{
						ReadFromWB = 1;
						*((word_t *)p) = *((word_t *)&ent->data[0]);
						return md_fault_none;
					}
					else if(ent->addr + nbytes == addr)
					{
						ReadFromWB = 1;
						*((word_t *)p) = *((word_t *)&ent->data[1]);
						return md_fault_none;
					}
				}
			}
			else if (nbytes == 8)
			{
				if(nbytes == ent->nbytes)
				{
					if(ent->addr == addr)
					{
						ReadFromWB = 1;
						break;
					}
				}
				else if ( (ent->nbytes == 1) || (ent->nbytes == 2)|| (ent->nbytes == 4))
				{
					if((ent->addr >= addr) && (ent->addr < (addr + nbytes)))
					{
//						fprintf(stderr,"One byte of data found in the write buffer 5 %d, %llx\n", current->id, current->regs.regs_PC);
						flushWriteBuffer(current->id);
						*((word_t *)p) = MEM_READ_WORD(mem, addr);
						*(((word_t *)p)+1) = MEM_READ_WORD(mem, addr + sizeof(word_t));
						ReadFromWB = 0;
						return md_fault_none;
					}
				}
			}
			else
			{
				fprintf(stderr, " Number of bytes read is incorrect\n");
				exit(0);
			}
		}
	}
#endif
	/* no, if it is a write, allocate a hash table entry to hold the data */
	if (cmd == Write)
	{
		if(ent != NULL)
			panic("ent is not equal to NULL\n");
		if (!current->WBbucket_free_list)
		{
			current->WBbucket_free_list = (struct wb_mem_ent *)calloc(1, sizeof(struct wb_mem_ent));
			if (!current->WBbucket_free_list)
				fatal("thread %d , out of virtual memory",threadid);
			current->WBbucket_free_list->next = NULL;
		}
		ent = current->WBbucket_free_list;
		current->WBbucket_free_list = current->WBbucket_free_list->next;
		ent->PC = current->regs.regs_PC;
		ent->addr = addr;
		ent->data[0] = 0; ent->data[1] = 0;
		ent->completeCycle = 0;
		ent->nbytes = nbytes;
                ent->sim_num_insn = current->sim_num_insn;
                ent->disp_time = sim_cycle;
                ent->lsqId = current->LSQ_tail;
		ent->needBusAccess = 0;
		ent->instr = TempInstr;
		ent->accessed = 0;
		ent->writeAlreadyDone = writeProb;
		current->numOfWBEntry++;
		
		TempInstr = 0x1A2B3C4D;
		if(current->WBtableHead != NULL)
		{
			ent->next = current->WBtableHead;
			current->WBtableHead = ent;
			ent->next->prev = ent;
			ent->prev = NULL;
		}
		else
		{
			current->WBtableHead = ent;
			current->WBtableTail = ent;
			ent->next = NULL;
			ent->prev = NULL;
		}
	}

	/* handle the read or write to speculative or non-speculative storage */
	switch (nbytes)
	{
		case 1:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((byte_t *)p) = *((byte_t *)(&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					memory pages with speculative loads */
					*((byte_t *)p) = MEM_READ_BYTE(mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((byte_t *)(&ent->data[0])) = *((byte_t *)p);
			}
			break;
		case 2:
			if (cmd == Read)
			{
				if (ent)
				{
					*((half_t *)p) = *((half_t *)(&ent->data[0]));
				}
				else
				{
					*((half_t *)p) = MEM_READ_HALF(mem, addr);
				}
			}
			else
			{
				*((half_t *)&ent->data[0]) = *((half_t *)p);
			}
			break;
		case 4:
			if (cmd == Read)
			{
				if (ent)
				{
					*((word_t *)p) = *((word_t *)&ent->data[0]);
				}
				else
				{
					*((word_t *)p) = MEM_READ_WORD(mem, addr);
				}
			}
			else
			{
				*((word_t *)&ent->data[0]) = *((word_t *)p);
			}
			break;
		case 8:
			if (cmd == Read)
			{
				if (ent)
				{
					*((word_t *)p) = *((word_t *)&ent->data[0]);
					*(((word_t *)p)+1) = *((word_t *)&ent->data[1]);
				}
				else
				{
					*((word_t *)p) = MEM_READ_WORD(mem, addr);
					*(((word_t *)p)+1) = MEM_READ_WORD(mem, addr + sizeof(word_t));
				}
			}
			else
			{
				*((word_t *)&ent->data[0]) = *((word_t *)p);
				*((word_t *)&ent->data[1]) = *(((word_t *)p)+1);
			}
			break;
		default:
			panic("access size not supported in mis-speculative mode");
	}
	
	if(cmd == Read)
		mem_partial_read(mem, addr, p, nbytes, threadID);

	return md_fault_none;
}

/* register memory system-specific statistics */
void
mem_reg_stats(struct mem_t *mem,	/* memory space to declare */
	      struct stat_sdb_t *sdb)	/* stats data base */
{
  char buf[512], buf1[512];

  sprintf(buf, "%s.page_count", mem->name);
  stat_reg_counter(sdb, buf, "total number of pages allocated",
		   &mem->page_count, mem->page_count, NULL);

  sprintf(buf, "%s.page_mem", mem->name);
  sprintf(buf1, "%s.page_count * %d / 1024", mem->name, MD_PAGE_SIZE);
  stat_reg_formula(sdb, buf, "total size of memory pages allocated",
		   buf1, "%11.0fk");

  sprintf(buf, "%s.ptab_misses", mem->name);
  stat_reg_counter(sdb, buf, "total first level page table misses",
		   &mem->ptab_misses, mem->ptab_misses, NULL);

  sprintf(buf, "%s.ptab_accesses", mem->name);
  stat_reg_counter(sdb, buf, "total page table accesses",
		   &mem->ptab_accesses, mem->ptab_accesses, NULL);

  sprintf(buf, "%s.ptab_miss_rate", mem->name);
  sprintf(buf1, "%s.ptab_misses / %s.ptab_accesses", mem->name, mem->name);
  stat_reg_formula(sdb, buf, "first level page table miss rate", buf1, NULL);
}

/* initialize memory system, call before loader.c */
void
mem_init(struct mem_t *mem)	/* memory space to initialize */
{
  int i;

  /* initialize the first level page table to all empty */
  for (i=0; i < MEM_PTAB_SIZE; i++)
    mem->ptab[i] = NULL;

  mem->page_count = 0;
  mem->ptab_misses = 0;
  mem->ptab_accesses = 0;
}

/* dump a block of memory, returns any faults encountered */
enum md_fault_type
mem_dump(struct mem_t *mem,		/* memory space to display */
	 md_addr_t addr,		/* target address to dump */
	 int len,			/* number bytes to dump */
	 FILE *stream)			/* output stream */
{
  int data;
  enum md_fault_type fault;

  if (!stream)
    stream = stderr;

  addr &= ~sizeof(word_t);
  len = (len + (sizeof(word_t) - 1)) & ~sizeof(word_t);
  while (len-- > 0)
    {
      fault = mem_access(mem, Read, addr, &data, sizeof(word_t), mem->threadid);
      if (fault != md_fault_none)
	return fault;

      myfprintf(stream, "0x%08p: %08x\n", addr, data);
      addr += sizeof(word_t);
    }

  /* no faults... */
  return md_fault_none;
}

/* copy a '\0' terminated string to/from simulated memory space, returns
   the number of bytes copied, returns any fault encountered */
// enum md_fault_type
// mem_strcpy(mem_access_fn mem_fn,	/* user-specified memory accessor */
// 	   struct mem_t *mem,		/* memory space to access */
// 	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
// 	   md_addr_t addr,		/* target address to access */
// 	   char *s)
// {
//   int n = 0;
//   char c;
//   enum md_fault_type fault;
// 
//   switch (cmd)
//     {
//     case Read:
//       /* copy until string terminator ('\0') is encountered */
//       do {
// 	fault = mem_fn(mem, Read, addr++, &c, 1);
// 	if (fault != md_fault_none)
// 	  return fault;
// 	*s++ = c;
// 	n++;
//       } while (c);
//       break;
// 
//     case Write:
//       /* copy until string terminator ('\0') is encountered */
//       do {
// 	c = *s++;
// 	fault = mem_fn(mem, Write, addr++, &c, 1);
// 	if (fault != md_fault_none)
// 	  return fault;
// 	n++;
//       } while (c);
//       break;
// 
//     default:
//       return md_fault_internal;
//   }
// 
//   /* no faults... */
//   return md_fault_none;
// }


enum md_fault_type
mem_strcpy(mem_access_fn mem_fn,	/* user-specified memory accessor */
	struct mem_t *mem,		/* memory space to access */
	enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	md_addr_t addr,		/* target address to access */
	char *s, int threadID)
{
    int n = 0;
    char c;
    enum md_fault_type fault;
#if DEFINE_WB
    flushWriteBuffer(threadID);
#endif
    switch (cmd)
    {
	case Read:
	    /* copy until string terminator ('\0') is encountered */
	    do {
#if DEFINE_WB
		fault = mem_access_drct(mem, Read, addr++, &c, 1, threadID);
#else
		fault = mem_fn(mem, Read, addr++, &c, 1, threadID);
#endif
		if (fault != md_fault_none)
		    return fault;
		*s++ = c;
		n++;
	    } while (c);
	    break;

	case Write:
	    /* copy until string terminator ('\0') is encountered */
	    do {
		c = *s++;
#if DEFINE_WB
		fault = mem_access_drct(mem, Write, addr++, &c, 1, threadID);
#else
		fault = mem_fn(mem, Write, addr++, &c, 1, threadID);
#endif
		if (fault != md_fault_none)
		    return fault;
		n++;
	    } while (c);
	    break;

	default:
	    return md_fault_internal;
    }

    /* no faults... */
    return md_fault_none;
}




/* copy NBYTES to/from simulated memory space, returns any faults */
// /*enum md_fault_type
// mem_bcopy(mem_access_fn mem_fn,		/* user-specified memory accessor */
// 	  struct mem_t *mem,		/* memory space to access */
// 	  enum mem_cmd cmd,		/* Read (from sim mem) or Write */
// 	  md_addr_t addr,		/* target address to access */
// 	  void *vp,			/* host memory address to access */
// 	  int nbytes)
// {
//   byte_t *p = vp;
//   enum md_fault_type fault;
// /*   printf("pass 5_1_1_3_2_1, %lu , %lu, %d ,%lu",
//    (unsigned long)mem_fn,(unsigned long)mem,cmd,(unsigned long)addr);
//    printf(", %d, %lu\n",nbytes,(unsigned long)vp);
// */
//   /* copy NBYTES bytes to/from simulator memory */
//   while (nbytes-- > 0)
//     {
// /*     if( ((unsigned long)mem == 1076572168)  && !(nbytes%1) && (nbytes<577540) )
//      {
//       printf("pass 5_1_1_3_2_1_1 , %lu , %lu, %d ,%lu",
//    (unsigned long)mem_fn,(unsigned long)mem,cmd,(unsigned long)addr);
//       printf(", %d, %lu\n",nbytes,(unsigned long)p);
//      }
// */      fault = mem_fn(mem, cmd, addr++, p++, 1);
//       if (fault != md_fault_none)
// 	return fault;
//     }
// //   printf("pass 5_1_1_3_2_2\n");
// 
//   /* no faults... */
//   return md_fault_none;
// }*/
enum md_fault_type
mem_bcopy(mem_access_fn mem_fn,		/* user-specified memory accessor */
	struct mem_t *mem,		/* memory space to access */
	enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	md_addr_t addr,		/* target address to access */
	void *vp,			/* host memory address to access */
	int nbytes, int threadID)
{
    byte_t *p = vp;
    enum md_fault_type fault;
    /* copy NBYTES bytes to/from simulator memory */
#if DEFINE_WB
    flushWriteBuffer(threadID);
#endif
    while (nbytes-- > 0)
    {
#if DEFINE_WB
	fault = mem_access_drct(mem, cmd, addr++, p++, 1, threadID);
#else
	fault = mem_fn(mem, cmd, addr++, p++, 1, threadID);
#endif
	if (fault != md_fault_none)
	    return fault;
    }
    return md_fault_none;
}


/* copy NBYTES to/from simulated memory space, NBYTES must be a multiple
   of 4 bytes, this function is faster than mem_bcopy(), returns any
   faults encountered */
// enum md_fault_type
// mem_bcopy4(mem_access_fn mem_fn,	/* user-specified memory accessor */
// 	   struct mem_t *mem,		/* memory space to access */
// 	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
// 	   md_addr_t addr,		/* target address to access */
// 	   void *vp,			/* host memory address to access */
// 	   int nbytes)
// {
//   byte_t *p = vp;
//   int words = nbytes >> 2;		/* note: nbytes % 2 == 0 is assumed */
//   enum md_fault_type fault;
// 
//   while (words-- > 0)
//     {
//       fault = mem_fn(mem, cmd, addr, p, sizeof(word_t));
//       if (fault != md_fault_none)
// 	return fault;
// 
//       addr += sizeof(word_t);
//       p += sizeof(word_t);
//     }
// 
//   /* no faults... */
//   return md_fault_none;
// }

enum md_fault_type
mem_bcopy4(mem_access_fn mem_fn,	/* user-specified memory accessor */
	struct mem_t *mem,		/* memory space to access */
	enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	md_addr_t addr,		/* target address to access */
	void *vp,			/* host memory address to access */
	int nbytes, int threadID)
{
    byte_t *p = vp;
    int words = nbytes >> 2;		/* note: nbytes % 2 == 0 is assumed */
    enum md_fault_type fault;

#if DEFINE_WB
    flushWriteBuffer(threadID);
#endif
    while (words-- > 0)
    {
#if DEFINE_WB
	fault = mem_access_drct(mem, cmd, addr, p, sizeof(word_t), threadID);
#else
	fault = mem_fn(mem, cmd, addr, p, sizeof(word_t), threadID);
#endif
	if (fault != md_fault_none)
	    return fault;

	addr += sizeof(word_t);
	p += sizeof(word_t);
    }
    return md_fault_none;
}







/* zero out NBYTES of simulated memory, returns any faults encountered */
// enum md_fault_type
// mem_bzero(mem_access_fn mem_fn,		/* user-specified memory accessor */
// 	  struct mem_t *mem,		/* memory space to access */
// 	  md_addr_t addr,		/* target address to access */
// 	  int nbytes)
// {
//   byte_t c = 0;
//   enum md_fault_type fault;
// 
//   /* zero out NBYTES of simulator memory */
//   while (nbytes-- > 0)
//     {
//       fault = mem_fn(mem, Write, addr++, &c, 1);
//       if (fault != md_fault_none)
// 	return fault;
//     }
// 
//   /* no faults... */
//   return md_fault_none;
// }
// 
// 



enum md_fault_type
mem_bzero(mem_access_fn mem_fn,		/* user-specified memory accessor */
	struct mem_t *mem,		/* memory space to access */
	md_addr_t addr,		/* target address to access */
	int nbytes, int threadID)
{
    byte_t c = 0;
    enum md_fault_type fault;

#if DEFINE_WB
    flushWriteBuffer(threadID);
#endif

    /* zero out NBYTES of simulator memory */
    while (nbytes-- > 0)
    {
#if DEFINE_WB
	fault = mem_access_drct(mem, Write, addr++, &c, 1, threadID);
#else
	fault = mem_fn(mem, Write, addr++, &c, 1, threadID);
#endif
	if (fault != md_fault_none)
	    return fault;
    }
    return md_fault_none;
}




#if 0

/*
 * The SimpleScalar virtual memory address space is 2^31 bytes mapped from
 * 0x00000000 to 0x7fffffff.  The upper 2^31 bytes are currently reserved for
 * future developments.  The address space from 0x00000000 to 0x00400000 is
 * currently unused.  The address space from 0x00400000 to 0x10000000 is used
 * to map the program text (code), although accessing any memory outside of
 * the defined program space causes an error to be declared.  The address
 * space from 0x10000000 to "mem_brk_point" is used for the program data
 * segment.  This section of the address space is initially set to contain the
 * initialized data segment and then the uninitialized data segment.
 * "mem_brk_point" then grows to higher memory when sbrk() is called to
 * service heap growth.  The data segment can continue to expand until it
 * collides with the stack segment.  The stack segment starts at 0x7fffc000
 * and grows to lower memory as more stack space is allocated.  Initially,
 * the stack contains program arguments and environment variables (see
 * loader.c for details on initial stack layout).  The stack may continue to
 * expand to lower memory until it collides with the data segment.
 *
 * The SimpleScalar virtual memory address space is implemented with a
 * one level page table, where the first level table contains MEM_TABLE_SIZE
 * pointers to MEM_BLOCK_SIZE byte pages in the second level table.  Pages
 * are allocated in MEM_BLOCK_SIZE size chunks when first accessed, the initial
 * value of page memory is all zero.
 *
 * Graphically, it all looks like this:
 *
 *                 Virtual        Level 1    Host Memory Pages
 *                 Address        Page       (allocated as needed)
 *                 Space          Table
 * 0x00000000    +----------+      +-+      +-------------------+
 *               | unused   |      | |----->| memory page (64k) |
 * 0x00400000    +----------+      +-+      +-------------------+
 *               |          |      | |
 *               | text     |      +-+
 *               |          |      | |
 * 0x10000000    +----------+      +-+
 *               |          |      | |
 *               | data seg |      +-+      +-------------------+
 *               |          |      | |----->| memory page (64k) |
 * mem_brk_point +----------+      +-+      +-------------------+
 *               |          |      | |
 *               |          |      +-+
 *               |          |      | |
 * regs_R[29]    +----------+      +-+
 * (stack ptr)   |          |      | |
 *               | stack    |      +-+
 *               |          |      | |
 * 0x7fffc000    +----------+      +-+      +-------------------+
 *               | unsed    |      | |----->| memory page (64k) |
 * 0x7fffffff    +----------+      +-+      +-------------------+

 */

/* top of the data segment, sbrk() moves this to higher memory */
extern SS_ADDR_TYPE mem_brk_point;

/* lowest address accessed on the stack */
extern SS_ADDR_TYPE mem_stack_min;

/*
 * memory page table defs
 */

/* memory indirect table size (upper mem is not used) */
#define MEM_TABLE_SIZE		0x8000 /* was: 0x7fff */

#ifndef HIDE_MEM_TABLE_DEF	/* used by sim-fast.c */
/* the level 1 page table map */
extern char *mem_table[MEM_TABLE_SIZE];
#endif /* HIDE_MEM_TABLE_DEF */

/* memory block size, in bytes */
#define MEM_BLOCK_SIZE		0x10000

  /* check permissions, no probes allowed into undefined segment regions */
  if (!(/* text access and a read */
	(addr >= ld_text_base && addr < (ld_text_base+ld_text_size)
	 && cmd == Read)
	/* data access within bounds */
	|| (addr >= ld_data_base && addr < ld_stack_base)))
    fatal("access error: segmentation violation, addr 0x%08p", addr);

  /* track the minimum SP for memory access stats */
  if (addr > mem_brk_point && addr < mem_stack_min)
    mem_stack_min = addr;

/* determines if the memory access is valid, returns error str or NULL */
char *					/* error string, or NULL */
mem_valid(struct mem_t *mem,		/* memory space to probe */
	  enum mem_cmd cmd,		/* Read (from sim'ed mem) or Write */
	  md_addr_t addr,		/* target address to access */
	  int nbytes,			/* number of bytes to access */
	  int declare);			/* declare any detected error? */

/* determines if the memory access is valid, returns error str or NULL */
char *					/* error string, or NULL */
mem_valid(enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	  SS_ADDR_TYPE addr,		/* target address to access */
	  int nbytes,			/* number of bytes to access */
	  int declare)			/* declare the error if detected? */
{
  char *err_str = NULL;

  /* check alignments */
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    {
      err_str = "bad size or alignment";
    }
  /* check permissions, no probes allowed into undefined segment regions */
  else if (!(/* text access and a read */
	   (addr >= ld_text_base && addr < (ld_text_base+ld_text_size)
	    && cmd == Read)
	   /* data access within bounds */
	   || (addr >= ld_data_base && addr < ld_stack_base)))
    {
      err_str = "segmentation violation";
    }

  /* track the minimum SP for memory access stats */
  if (addr > mem_brk_point && addr < mem_stack_min)
    mem_stack_min = addr;

  if (!declare)
    return err_str;
  else if (err_str != NULL)
    fatal(err_str);
  else /* no error */
    return NULL;
}

/* initialize memory system, call after loader.c */
void
mem_init1(void)
{

  /* initialize the bottom of heap to top of data segment */
  mem_brk_point = ROUND_UP(ld_data_base + ld_data_size, SS_PAGE_SIZE);

  /* set initial minimum stack pointer value to initial stack value */
  mem_stack_min = regs_R[SS_STACK_REGNO];
}

#endif

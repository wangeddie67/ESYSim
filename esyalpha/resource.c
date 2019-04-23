/*
 * resource.c - resource manager routines
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
 * $Id: resource.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: resource.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:23  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:54  garg
 * dir structure
 *
 * Revision 1.2  2004/10/08 21:58:58  rashid
 * *** empty log message ***
 *
 * Revision 1.1  2004/09/22 21:20:25  etan
 * Initial revision
 *
 * Revision 1.2  2004/06/23 15:23:58  ninelson
 * Minor change from Ali: no cache access
 *
 * Revision 1.1  2004/05/14 14:30:09  ninelson
 * Initial revision
 *
 * Revision 1.1.1.1  2003/10/20 13:58:02  ninelson
 * *** empty log message ***
 *
 * Revision 1.4  1998/08/27 15:54:06  taustin
 * implemented host interface description in host.h
 * added target interface support
 *
 * Revision 1.3  1997/03/11  01:19:53  taustin
 * updated copyright
 * check added to ensure number of res insts <= MAX_INSTS_PER_CLASS
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "host.h"
#include "misc.h"
#include "resource.h"

#include "smt.h"

#include "context.h"
/* Ronz stuff. */

//#define CLUSTER_S 1
#ifdef PORT_PER_BANK
int RES_CACHE_PORTS;
extern int res_mem_port_bank;
#else
#define RES_CACHE_PORTS 16	/* If there is no multi-porting, CACHE_PORTS
				   will be equal to RES_CACHE_PORTS */
int port_bank[RES_CACHE_PORTS]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
#endif
/*
int port_bank[CACHE_PORTS]={1};
int port_bank[CACHE_PORTS]={1,2};
int port_bank[CACHE_PORTS]={1,2,3,4};
int port_bank[CACHE_PORTS]={1,2,3,4,5,6,7,8};
int port_bank[CACHE_PORTS]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
*/
#define MAGIC_N 2000
#ifdef GENERIC_FU
#define IGNORE_ISS_RESTR 1
#else
#define IGNORE_ISS_RESTR 0
#endif


#define LDSTADDERS RES_CACHE_PORTS /* Number of intALUs that are dedicated to
				   address calculation - Ronz */

#ifdef   ISSUE_STAT
int free_other_cl;
#endif

int resClassStart[10], resClassIndex;

/* create a resource pool */
struct res_pool *
res_create_pool(char *name, struct res_desc *pool, int ndesc)
{
	int i, j, k, index, ninsts;
	struct res_desc *inst_pool;
	struct res_pool *res;

	resClassIndex++;
	/* count total instances */
	for (ninsts=0,i=0; i<ndesc; i++)
	{
		if (pool[i].quantity > MAX_INSTS_PER_CLASS)
			fatal("too many functional units, increase MAX_INSTS_PER_CLASS");
		resClassStart[resClassIndex++] = ninsts;			//record all class starting index
		ninsts += pool[i].quantity;
	}
	resClassStart[resClassIndex] = ninsts;			//record (last class + 1) starting index
	/* allocate the instance table */
	inst_pool = (struct res_desc *)calloc(ninsts, sizeof(struct res_desc));
	if (!inst_pool)
		fatal("out of virtual memory");
	/* fill in the instance table */
	for (index=0,i=0; i<ndesc; i++)
	{
		for (j=0; j<pool[i].quantity; j++)
		{
			inst_pool[index] = pool[i];
			inst_pool[index].quantity = 1;
			inst_pool[index].busy = 0;
			for (k=0; k<MAX_RES_CLASSES && inst_pool[index].x[k].class; k++)
				inst_pool[index].x[k].master = &inst_pool[index];
			index++;
		}
	}
	assert(index == ninsts);
	/* allocate the resouce pool descriptor */
	res = (struct res_pool *)calloc(1, sizeof(struct res_pool));
	if (!res)
		fatal("out of virtual memory");
	res->name = name;
	res->num_resources = ninsts;
	res->resources = inst_pool;
	/* fill in the resource table map - slow to build, but fast to access */
	for (i=0; i<ninsts; i++)
	{
		struct res_template *plate;
		for (j=0; j<MAX_RES_CLASSES; j++)
		{
			plate = &res->resources[i].x[j];
			if (plate->class)
			{
				assert(plate->class < MAX_RES_CLASSES);
				res->table[plate->class][res->nents[plate->class]++] = plate;
			}
			else
				break;
		}
	}
	return res;
}

/* get a free resource from resource pool POOL that can execute a
   operation of class CLASS, returns a pointer to the resource template,
   returns NULL, if there are currently no free resources available,
   follow the MASTER link to the master resource descriptor;
   NOTE: caller is responsible for reseting the busy flag in the beginning
   of the cycle when the resource can once again accept a new operation */
struct res_template *
res_get(struct res_pool *pool, int class, int threadid)
/* Has been modified to account for cache interleaving and intalus that
   are used exclusively for address calculation - Ronz */
{
	int i;
	int lo, hi;
	int per;



	per = pool->nents[class]/CLUSTERS;
	lo = threadid*per;
	hi = (threadid+1)*per;

	assert(class < MAX_RES_CLASSES);
	
	assert(pool->table[class][0]);

	for (i= lo; i< hi; i++)
	{
		if (pool->table[class][i] && !pool->table[class][i]->master->busy)
		{
			return pool->table[class][i];	
		}
/*		else
			break;*/
	}
	return NULL;
}

/* dump the resource pool POOL to stream STREAM */
void
res_dump(struct res_pool *pool, FILE *stream)
{
  int i, j;

  if (!stream)
    stream = stderr;

  fprintf(stream, "Resource pool: %s:\n", pool->name);
  fprintf(stream, "\tcontains %d resource instances\n", pool->num_resources);
  for (i=0; i<MAX_RES_CLASSES; i++)
    {
      fprintf(stream, "\tclass: %d: %d matching instances\n",
	      i, pool->nents[i]);
      fprintf(stream, "\tmatching: ");
      for (j=0; j<MAX_INSTS_PER_CLASS; j++)
	{
	  if (!pool->table[i][j])
	    break;
	  fprintf(stream, "\t%s (busy for %d cycles) ",
		  pool->table[i][j]->master->name,
		  pool->table[i][j]->master->busy);
	}
      assert(j == pool->nents[i]);
      fprintf(stream, "\n");
    }
}

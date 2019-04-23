/*
 * sysprobe.c - host endian probe implementation
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
 * $Id: sysprobe.c,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: sysprobe.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:12  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.1  2004/05/14 14:30:09  ninelson
 * Initial revision
 *
 * Revision 1.2  2003/09/30 20:46:26  ninelson
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.8  1999/12/31 18:54:24  taustin
 * Linux support updated
 *
 * Revision 1.7  1998/09/03 22:24:34  taustin
 * only locate first GZIP binary
 *
 * Revision 1.6  1998/08/31 17:11:36  taustin
 * ported to MS VC++
 *
 * Revision 1.5  1998/08/27 16:45:59  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added gzopen() and gzclose() routines for reading and writing
 *       compressed files, updated sysprobe to search for GZIP, if found
 *       support is enabled
 * added support for fast shifts if host machine can successfully implement
 *       them, sysprobe tests if fast shifts work and then sets -DFAST_SRA and
 *       -DFAST_SRL accordingly
 * added "-t" option on sysprobe that probes sizes of various C data types
 *
 * Revision 1.4  1997/04/16  22:12:36  taustin
 * added standalone loader support
 *
 * Revision 1.3  1997/03/11  01:35:38  taustin
 * updated copyright
 * support added for portable SYMCAT()
 * -libs support added for portability
 * -flags support added for portability
 * various target supports added
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#else /* _MSC_VER */
#define access	_access
#define X_OK	4
#endif

#include "host.h"
#include "misc.h"

char *gzip_paths[] =
{
  "/bin/gzip",
  "/usr/bin/gzip",
  "/usr/local/bin/gzip",
  "/usr/intel/bin/gzip",
  "/usr/gnu/bin/gzip",
  "/usr/local/gnu/bin",
  NULL
};

#define HOST_ONLY
#include "endian.c"

#define CAT(a,b)	a/**/b

#define MSB		0x80000000
int
fast_SRL(void)
{
  word_t ui;

  if (sizeof(ui) != 4)
    {
      /* fundamental size assumption broken - emulate SRL */
      return FALSE;
    }

  ui = (word_t)MSB;
  if (((ui >> 1) & MSB) != 0)
    {
      /* unsigned int does SRA - emulate SRL */
      return FALSE;
    }
  else
    {
      /* unsigned int does SRL - use fast native SRL */
      return TRUE;
    }
}

int
fast_SRA(void)
{
  sword_t si;

  if (sizeof(si) != 4)
    {
      /* fundamental size assumption broken - emulate SRA */
      return FALSE;
    }

  si = (sword_t)MSB;
  if ((si >> 1) & MSB)
    {
      /* signed int does SRA - use fast native SRA */
      return TRUE;
    }
  else
    {
      /* singned int does SRL - emulate SRA */
      return FALSE;
    }
}

int
main(int argc, char **argv)
{
  int little_bytes = 0, little_words = 0;

  if (argc == 2 && !strcmp(argv[1], "-s"))
    {
      switch (endian_host_byte_order())
	{
	case endian_big:
	  fprintf(stdout, "big\n");
	  break;
	case endian_little:
	  fprintf(stdout, "little\n");
	  break;
	case endian_unknown:
	  fprintf(stderr, "\nerror: cannot determine byte order!\n");
	  exit(1);
	default:
	  abort();
	}
    }
  else if (argc == 2 && !strcmp(argv[1], "-libs"))
    {
#ifdef BFD_LOADER
      fprintf(stdout, "-lbfd -liberty ");
#endif /* BFD_LOADER */

#ifdef linux
      /* nada... */
#elif defined(_AIX)
      if (sizeof(long) == 4) /* 32-bit compilation */
	      fprintf(stdout, "-lbsd ");
#elif defined(__USLC__) || (defined(__svr4__) && defined(__i386__) && defined(__unix__))
      fprintf(stdout, "-L/usr/ucblib -lucb ");
#elif defined(__svr4__) && defined(__sun__) && defined(__unix__)
      fprintf(stdout, "-lsocket -lnsl ");
#else
      /* nada */
#endif
      fprintf(stdout, "-lm\n");
    }
  else if (argc == 1 || (argc == 2 && !strcmp(argv[1], "-flags")))
    {
      switch (endian_host_byte_order())
	{
	case endian_big:
	  fprintf(stdout, "-DBYTES_BIG_ENDIAN ");
	  break;
	case endian_little:
	  fprintf(stdout, "-DBYTES_LITTLE_ENDIAN ");
	  little_bytes = 1;
	  break;
	case endian_unknown:
	  fprintf(stderr, "\nerror: cannot determine byte order!\n");
	  exit(1);
	default:
	  abort();
	}

      switch (endian_host_word_order())
	{
	case endian_big:
	  fprintf(stdout, "-DWORDS_BIG_ENDIAN ");
	  break;
	case endian_little:
	  fprintf(stdout, "-DWORDS_LITTLE_ENDIAN ");
	  little_words = 1;
	  break;
	case endian_unknown:
	  fprintf(stderr, "\nerror: cannot determine word order!\n");
	  exit(1);
	default:
	  abort();
	}

#ifdef _AIX
	fprintf(stdout, "-D_ALL_SOURCE ");
	if (sizeof(long) == 8)
		fprintf(stdout, "-DPOWER_64 ");
#ifndef __GNUC__
	fprintf(stdout, "-D__CC_XLC ");
#endif
#endif /* _AIX */

#if (defined(hpux) || defined(__hpux)) && !defined(__GNUC__)
	fprintf(stdout, "-D_INCLUDE_HPUX_SOURCE -D_INCLUDE_XOPEN_SOURCE -D_INCLUDE_AES_SOURCE ");
#endif /* hpux */

#ifndef __GNUC__
      /* probe compiler approach needed to concatenate symbols in CPP,
	 new style concatenation is always used with GNU GCC */
      {
	int i = 5, j;

	j = CAT(-,-i);

	if (j == 4)
	  {
	    /* old style symbol concatenation worked */
	    fprintf(stdout, "-DOLD_SYMCAT ");
	  }
	else if (j == 5)
	  {
	    /* old style symbol concatenation does not work, assume that
	       new style symbol concatenation works */
	    ;
	  }
	else
	  {
	    /* huh!?!?! */
	    fprintf(stderr, "\nerror: cannot grok symbol concat method!\n");
	    exit(1);
	  }
      }
#endif /* __GNUC__ */

#ifndef SLOW_SHIFTS
      /* probe host shift capabilities */
      if (fast_SRL())
	fprintf(stdout, "-DFAST_SRL ");
      if (fast_SRA())
	fprintf(stdout, "-DFAST_SRA ");
#endif /* !SLOW_SHIFTS */

      /* locate GZIP */
#ifndef GZIP_PATH
      {
	int i;

	for (i=0; gzip_paths[i] != NULL; i++)
	  {
	    if (access(gzip_paths[i], X_OK) == 0)
	      {
		fprintf(stdout, "-DGZIP_PATH=\"%s\" ", gzip_paths[i]);
		break;
	      }
	  }
      }
#endif /* !GZIP_PATH */

    }
  else if (argc == 2 && !strcmp(argv[1], "-t"))
    {
      fprintf(stdout, "sizeof(int) = %lu\n", (unsigned long) sizeof(int));
      fprintf(stdout, "sizeof(long) = %lu\n", (unsigned long) sizeof(long));
    }


  /* check for different byte/word endian-ness */
  if (little_bytes != little_words)
    {
      fprintf(stderr,
	      "\nerror: opposite byte/word endian currently not supported!\n");
      exit(1);
    }
//  fprintf(stderr, "Error in sysprob\n");
  exit(0);
  return 0;
}


/*
 * exolex.l - EXO library lexor
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 *
 * Copyright (C) 1997 by Todd M. Austin
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
 * $Id: exolex.l,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: exolex.l,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:23:23  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:45:14  garg
 * dir structure
 *
 * Revision 1.1  1998/08/27 08:54:51  taustin
 * Initial revision
 *
 *
 */

%{
/* C-style lexor. */

#include <stdio.h>
#include <stdlib.h>

#include "../host.h"
#include "../misc.h"
#include "libexo.h"

/* maximum size token (including strings) for lex */
#undef YYLMAX
#define YYLMAX	(16*1024)

/* disable interactive features, as they are non-portable */
#define YY_NEVER_INTERACTIVE	1

unsigned line = 1;		/* line of last recognized token */

int yywrap(void);
static void lex_eat_comment(void);

%}

%s BLOB_MODE
%p 3000

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
C1			"/"
C2			"*"
P			(~|!|@|#|\$|%|\^|&|\*|-|\+|\||\/|\?)

%%

<BLOB_MODE>{H}{H}			{ return lex_byte; }

<INITIAL>{C1}{C2}			{ lex_eat_comment(); }

<INITIAL>[\-]?0[xX]{H}+			{ return lex_address; }
<INITIAL>[\-]?0{D}+			{ return lex_integer; }
<INITIAL>[\-]?{D}+			{ return lex_integer; }

<INITIAL>[\-]?{D}+{E}			{ return lex_float; }
<INITIAL>[\-]?{D}*"."{D}+({E})?		{ return lex_float; }
<INITIAL>[\-]?{D}+"."{D}*({E})?		{ return lex_float; }

<INITIAL>'(\\.|[^\\'])+'		{ return lex_char; }

<INITIAL>\"(\\.|[^\\"])*\" /*"*/	{ return lex_string; }

<INITIAL>{L}({L}|{D})*			{ return lex_token; }

<INITIAL>{P}+				{ return lex_token; }

<INITIAL>"{"				{ return '{'; }
<INITIAL>"}"				{ return '}'; }
<INITIAL>","				{ return ','; }
<INITIAL>"("				{ return '('; }
<INITIAL>")"				{ return ')'; }
<INITIAL>"["				{ return '['; }
<INITIAL>"]"				{ return ']'; }
<INITIAL>"<"				{ BEGIN(BLOB_MODE); return '<'; }
<BLOB_MODE>">"				{ BEGIN(INITIAL); return '>'; }

[ \t\v\f]				{ /* nada */; }
[\n]					{ line++; }
<<EOF>>					{ return lex_eof; }
.					{ /* bogus char */
					  fatal("bogus character in input"); }

%%

int
yywrap(void)
{
  return 1;
}

static void
lex_eat_comment(void)
{
  char c, c1;

loop:
  while ((c = input()) != '*' && c != 0)
    {
      if (c == '\n')
	line++;
    }

  if ((c1 = input()) != '/' && c1 != 0)
    {
      unput(c1);
      goto loop;
    }
}

int
yy_nextchar(void)
{
  int c;

  do {
    c = input();
  } while (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\n');
  unput(c);

  return c;
}

/* FIXME: this is a total KLUDGE (but well contained...) that I will someday
   address when I have copious amounts of free time... */
void
yy_setstream(FILE *stream)
{
  int i;
#define MAX_STREAMS	16
  static struct {
    FILE *stream;
    YY_BUFFER_STATE buffer;
  } streams[MAX_STREAMS] = { {NULL, NULL}, };
  static int num_streams = 0;
  static FILE *last_stream = NULL;

  /* same stream? */
  if (stream == last_stream)
    return;

  /* else, switch to new stream */
  for (i=0; i < num_streams; i++)
    {
      if (streams[i].stream == stream)
	{
	  yy_switch_to_buffer(streams[i].buffer);
	  return;
	}
    }

  /* hrmmm... not found, create a new buffer for this stream */
  if (num_streams == MAX_STREAMS)
    fatal("out of lex buffer streams, increase MAX_STREAMS");

  streams[num_streams].stream = stream;
  streams[num_streams].buffer = yy_create_buffer(stream, YY_BUF_SIZE);
  yy_switch_to_buffer(streams[num_streams].buffer);
  num_streams++;
}

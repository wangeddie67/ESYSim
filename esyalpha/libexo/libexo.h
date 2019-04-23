/*
 * libexo.h - EXO library interfaces (NEVER write another scanf()!)
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
 * $Id: libexo.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: libexo.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:23:24  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:45:14  garg
 * dir structure
 *
 * Revision 1.1  1998/08/27 08:55:28  taustin
 * Initial revision
 *
 *
 */

/*
 * EXO(-skeletal) definitions:
 *
 *   The EXO format is used to store and retrieve data structures from text
 *   files.  The following BNF definition defines the contents of an EXO file:
 *
 *	<exo_file>	:= <exo_term>
 *
 *	<exo_term>	:= <exo_term_list>
 *			   | INTEGER
 *			   | FLOAT
 *			   | CHAR
 *			   | STRING
 *
 *	<exo_term_list>	:= (<exo_term_list> ',' <exo_term>)
 *			   | <exo_term>
 */

#ifndef EXO_H
#define EXO_H

#include "../host.h"
#include "../misc.h"
#include "../machine.h"

/* EXO file format versions */
#define EXO_FMT_MAJOR		1
#define EXO_FMT_MINOR		0

/* EXO term classes, keep this in sync with EXO_CLASS_STR */
enum exo_class_t
{
    ec_integer,			/* EXO int value */
    ec_address,			/* EXO address value */
    ec_float,			/* EXO FP value */
    ec_char,			/* EXO character value */
    ec_string,			/* EXO string value */
    ec_list,			/* EXO list */
    ec_array,			/* EXO array */
    ec_token,			/* EXO token value */
    ec_blob,			/* EXO blob (Binary Large OBject) */
    ec_null,			/* used internally */
    ec_NUM
};

/* EXO term classes print strings */
extern char *exo_class_str[ec_NUM];

/* EXO token table entry */
struct exo_token_t
{
    struct exo_token_t *next;	/* next element in a hash buck chain */
    char *str;			/* token string */
    int token;			/* token value */
};

struct exo_term_t
{
    struct exo_term_t *next;	/* next element, when in a list */
    enum exo_class_t ec;		/* term node class */
    union
    {
        struct as_integer_t
        {
            exo_integer_t val;		/* integer value */
        } as_integer;
        struct as_address_t
        {
            exo_address_t val;		/* address value */
        } as_address;
        struct as_float_t
        {
            exo_float_t val;			/* floating point value */
        } as_float;
        struct as_char_t
        {
            char val;				/* character value */
        } as_char;
        struct as_string_t
        {
            unsigned char *str;		/* string value */
        } as_string;
        struct as_list_t
        {
            struct exo_term_t *head;		/* list head pointer */
        } as_list;
        struct as_array_t
        {
            int size;				/* size of the array */
            struct exo_term_t **array;	/* list head pointer */
        } as_array;
        struct as_token_t
        {
            struct exo_token_t *ent;		/* token table entry */
        } as_token;
        struct as_blob_t
        {
            int size;				/* byte size of object */
            unsigned char *data;		/* pointer to blob data */
        } as_blob;
    } variant;
};
/* short-cut accessors */
#define as_integer	variant.as_integer
#define as_address	variant.as_address
#define as_float	variant.as_float
#define as_char		variant.as_char
#define as_string	variant.as_string
#define as_list		variant.as_list
#define as_array	variant.as_array
#define as_token	variant.as_token
#define as_blob		variant.as_blob

/* EXO array accessor, may be used as an L-value or R-value */
/* EXO array accessor, may be used as an L-value or R-value */
#define EXO_ARR(E,N)							\
  ((E)->ec != ec_array							\
   ? (fatal("not an array"), *(struct exo_term_t **)(NULL))		\
   : ((N) >= (E)->as_array.size						\
      ? (fatal("array bounds error"), *(struct exo_term_t **)(NULL))	\
      : (E)->as_array.array[(N)]))
#define SET_EXO_ARR(E,N,V)						\
  ((E)->ec != ec_array							\
   ? (void)fatal("not an array")					\
   : ((N) >= (E)->as_array.size						\
      ? (void)fatal("array bounds error")				\
      : (void)((E)->as_array.array[(N)] = (V))))

/* intern token TOKEN_STR */
struct exo_token_t *
exo_intern(char *token_str);		/* string to intern */

/* intern token TOKEN_STR as value TOKEN */
struct exo_token_t *
exo_intern_as(char *token_str,		/* string to intern */
              int token);		/* internment value */

/*
 * create a new EXO term, usage:
 *
 *	exo_new(ec_integer, (exo_integer_t)<int>);
 *	exo_new(ec_address, (exo_address_t)<int>);
 *	exo_new(ec_float, (exo_float_t)<float>);
 *	exo_new(ec_char, (int)<char>);
 *      exo_new(ec_string, "<string>");
 *      exo_new(ec_list, <list_ent>..., NULL);
 *      exo_new(ec_array, <size>, <array_ent>..., NULL);
 *	exo_new(ec_token, "<token>");
 *	exo_new(ec_blob, <size>, <data_ptr>);
 */
struct exo_term_t *
exo_new(enum exo_class_t ec, ...);

/* release an EXO term */
void
exo_delete(struct exo_term_t *exo);

/* chain two EXO lists together, FORE is attached on the end of AFT */
struct exo_term_t *
exo_chain(struct exo_term_t *fore, struct exo_term_t *aft);

/* copy an EXO node */
struct exo_term_t *
exo_copy(struct exo_term_t *exo);

/* deep copy an EXO structure */
struct exo_term_t *
exo_deepcopy(struct exo_term_t *exo);

/* print an EXO term */
void
exo_print(struct exo_term_t *exo, FILE *stream);

/* read one EXO term from STREAM */
struct exo_term_t *
exo_read(FILE *stream);

/* lexor components */
enum lex_t
{
    lex_integer = 256,
    lex_address,
    lex_float,
    lex_char,
    lex_string,
    lex_token,
    lex_byte,
    lex_eof
};

#endif /* EXO_H */

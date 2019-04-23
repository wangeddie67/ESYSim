#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <typeinfo>
#include <iostream>
#include "misc.h"
#include "Statistics.h"
using namespace std;
//static 变量初始化
enum eval_token_t Statistics::tok_map[256] = {tok_ident};

const char * Statistics::eval_err_str[ERR_NUM] = {
	  /* ERR_NOERR */	"!! no error!!",
	  /* ERR_UPAREN */	"unmatched parenthesis",
	  /* ERR_NOTERM */	"expression term is missing",
	  /* ERR_DIV0 */	"divide by zero",
	  /* ERR_BADCONST */	"badly formed constant",
	  /* ERR_BADEXPR */	"badly formed expression",
	  /* ERR_UNDEFVAR */	"variable is undefined",
	  /* ERR_EXTRA */	"extra characters at end of expression"
	};
const char * Statistics:: eval_type_str[et_NUM] = {
	  /* et_int */		"int",
	  /* et_uint */		"unsigned int",
	  /* et_addr */		"md_addr_t",
	#ifdef HOST_HAS_QWORD
	  /* et_qword */	"qword_t",
	  /* et_sqword */	"sqword_t",
	#endif /* HOST_HAS_QWORD */
	  /* et_float */	"float",
	  /* et_double */	"double",
	  /* et_symbol */	"symbol"
	};
const struct Statistics::eval_value_t Statistics::err_value = {et_int,{0}};
Statistics::Statistics()
:tok_map_initialized(FALSE)
{
	eval_error = ERR_NOERR;
	init_tok_map();
	statdb = stat_new();
}
Statistics::~Statistics(){
	int i;
	  struct stat_stat_t *stat, *stat_next;
	  struct bucket_t *bucket, *bucket_next;
	  /* free all individual stat variables */
	  for (stat = statdb->stats; stat != NULL; stat = stat_next)
	    {
	      stat_next = stat->next;
	      stat->next = NULL;
	      /* free stat */
	      switch (stat->sc)
		{
		case sc_int:
		case sc_uint:
	#ifdef HOST_HAS_QWORD
		case sc_qword:
		case sc_sqword:
	#endif /* HOST_HAS_QWORD */
		case sc_float:
		case sc_double:
		case sc_formula:
		  /* no other storage to deallocate */
		  break;
		case sc_dist:
		  /* free distribution array */
		  free(stat->variant.for_dist.arr);
		  stat->variant.for_dist.arr = NULL;
		  break;
		case sc_sdist:
		  /* free all hash table buckets */
		  for (i=0; i<HTAB_SZ; i++)
		    {
		      for (bucket = stat->variant.for_sdist.sarr[i];
			   bucket != NULL;
			   bucket = bucket_next)
			{
			  bucket_next = bucket->next;
			  bucket->next = NULL;
			  free(bucket);
			}
		      stat->variant.for_sdist.sarr[i] = NULL;
		    }
		  /* free hash table array */
		  free(stat->variant.for_sdist.sarr);
		  stat->variant.for_sdist.sarr = NULL;
		  break;
		default:
		  panic("bogus stat class");
		}
	      /* free stat variable record */
	      free(stat);
	    }
	  statdb->stats = NULL;
	  free(statdb->evaluator);
	  statdb->evaluator = NULL;
	  free(statdb);
}


void
Statistics::init_tok_map(void)
{
  int i;
  for (i=0; i<256; i++)
    tok_map[i] = tok_invalid;

  /* identifier characters */
  for (i='a'; i<='z'; i++)
    tok_map[i] = tok_ident;
  for (i='A'; i<='Z'; i++)
    tok_map[i] = tok_ident;
  tok_map[(int)'_'] = tok_ident;
  tok_map[(int)'$'] = tok_ident;

  /* numeric characters */
  for (i='0'; i<='9'; i++)
    tok_map[i] = tok_const;
  tok_map[(int)'.'] = tok_const;

  /* operator characters */
  tok_map[(int)'+'] = tok_plus;
  tok_map[(int)'-'] = tok_minus;
  tok_map[(int)'*'] = tok_mult;
  tok_map[(int)'/'] = tok_div;
  tok_map[(int)'('] = tok_oparen;
  tok_map[(int)')'] = tok_cparen;

  /* whitespace characers */
  tok_map[(int)' '] = tok_whitespace;
  tok_map[(int)'\t'] = tok_whitespace;
}

/* get next token from the expression string */
enum eval_token_t		/* token parsed */
Statistics::get_next_token(struct eval_state_t *es)	/* expression evaluator */
{
  int allow_hex;
  enum eval_token_t tok;
  char *ptok_buf, last_char;

  /* initialize the token map, if needed */
  if (!tok_map_initialized)
    {
      init_tok_map();
      tok_map_initialized = TRUE;
    }

  /* use the peek'ed token, if available, tok_buf should still be valid */
  if (es->peek_tok != tok_invalid)
    {
      tok = es->peek_tok;
      es->peek_tok = tok_invalid;
      return tok;
    }

  /* set up the token string space */
  ptok_buf = es->tok_buf;
  *ptok_buf = '\0';

  /* skip whitespace */
  while (*es->p && tok_map[(int)*es->p] == tok_whitespace)
    es->p++;

  /* end of token stream? */
  if (*es->p == '\0')
    return tok_eof;

  *ptok_buf++ = *es->p;
  tok = tok_map[(int)*es->p++];
  switch (tok)
    {
    case tok_ident:
      /* parse off next identifier */
      while (*es->p
	     && (tok_map[(int)*es->p] == tok_ident
		 || tok_map[(int)*es->p] == tok_const))
	{
	  *ptok_buf++ = *es->p++;
	}
      break;
    case tok_const:
      /* parse off next numeric literal */
      last_char = '\0';
      allow_hex = FALSE;
      while (*es->p &&
	     (tok_map[(int)*es->p] == tok_const
	      || (*es->p == '-' && last_char == 'e')
	      || (*es->p == '+' && last_char == 'e')
	      || tolower(*es->p) == 'e'
	      || tolower(*es->p) == 'x'
	      || (tolower(*es->p) == 'a' && allow_hex)
	      || (tolower(*es->p) == 'b' && allow_hex)
	      || (tolower(*es->p) == 'c' && allow_hex)
	      || (tolower(*es->p) == 'd' && allow_hex)
	      || (tolower(*es->p) == 'e' && allow_hex)
	      || (tolower(*es->p) == 'f' && allow_hex)))
	{
	  last_char = tolower(*es->p);
	  if (*es->p == 'x' || *es->p == 'X')
	    allow_hex = TRUE;
	  *ptok_buf++ = *es->p++;
	}
      break;
    case tok_plus:
    case tok_minus:
    case tok_mult:
    case tok_div:
    case tok_oparen:
    case tok_cparen:
      /* just pass on the token */
      break;
    default:
      tok = tok_invalid;
      break;
    }

  /* terminate the token string buffer */
  *ptok_buf = '\0';

  return tok;
}

enum eval_token_t		 /* next token in expression */
Statistics::peek_next_token(struct eval_state_t *es) /* expression evalutor */
{
  /* if there is no peek ahead token, get one */
  if (es->peek_tok == tok_invalid)
    {
      es->lastp = es->p;
      es->peek_tok = get_next_token(es);
    }

  /* return peek ahead token */
  return es->peek_tok;
}

/* determine necessary arithmetic conversion on T1 <op> T2 */
enum Statistics::eval_type_t			/* type of expression result */
Statistics::result_type(enum eval_type_t t1,	/* left operand type */
	    enum eval_type_t t2)	/* right operand type */
{
  /* sanity check, symbols should not show up in arithmetic exprs */
  if (t1 == et_symbol || t2 == et_symbol)
    panic("symbol used in expression");

  /* using C rules, i.e., A6.5 */
  if (t1 == et_double || t2 == et_double)
    return et_double;
  else if (t1 == et_float || t2 == et_float)
    return et_float;
#ifdef HOST_HAS_QWORD
  else if (t1 == et_qword || t2 == et_qword)
    return et_qword;
  else if (t1 == et_sqword || t2 == et_sqword)
    return et_sqword;
#endif /* HOST_HAS_QWORD */
  else if (t1 == et_addr || t2 == et_addr)
    return et_addr;
  else if (t1 == et_uint || t2 == et_uint)
    return et_uint;
  else
    return et_int;
}

template <typename Ts>
Ts Statistics::eval_as_type(struct eval_value_t val){
	switch (val.type) {
	case et_double:
		return (Ts)val.value.as_double;
	case et_float:
		return (Ts) val.value.as_float;
#ifdef HOST_HAS_QWORD
	case et_qword:
#ifdef _MSC_VER /* FIXME: MSC does not implement qword_t to dbl conversion */
		return (Ts)(sqword_t)val.value.as_qword;
#else /* !_MSC_VER */
		return (Ts) val.value.as_qword;
#endif /* _MSC_VER */
	case et_sqword:
		return (Ts) val.value.as_sqword;
#endif /* HOST_HAS_QWORD */
	case et_addr:
#if defined(_MSC_VER) && defined(TARGET_ALPHA)
		/* FIXME: MSC does not implement qword_t to double conversion */
		return (Ts)(sqword_t)val.value.as_addr;
#else
		return (Ts) val.value.as_addr;
#endif
	case et_uint:
		return (Ts) val.value.as_uint;
	case et_int:
		return (Ts) val.value.as_int;
	case et_symbol:
		panic("symbol used in expression");
	default:
		panic("illegal arithmetic expression conversion");
	}
}

/* compute <val1> + <val2> */
struct Statistics::eval_value_t
Statistics::f_add(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
    case et_double:
      val.type = et_double;
      val.value.as_double = eval_as_type<double>(val1) + eval_as_type<double>(val2);
      break;
    case et_float:
      val.type = et_float;
      val.value.as_float = eval_as_type<float>(val1) + eval_as_type<float>(val2);
      break;
#ifdef HOST_HAS_QWORD
    case et_qword:
      val.type = et_qword;
      val.value.as_qword = eval_as_type<qword_t>(val1) + eval_as_type<qword_t>(val2);
      break;
    case et_sqword:
      val.type = et_sqword;
      val.value.as_sqword = eval_as_type<sqword_t>(val1) + eval_as_type<sqword_t>(val2);
      break;
#endif /* HOST_HAS_QWORD */
    case et_addr:
      val.type = et_addr;
      val.value.as_addr = eval_as_type<unsigned long long>(val1) + eval_as_type<unsigned long long>(val2);
      break;
    case et_uint:
      val.type = et_uint;
      val.value.as_uint = eval_as_type<uint>(val1) + eval_as_type<uint>(val2);
      break;
    case et_int:
      val.type = et_int;
      val.value.as_int = eval_as_type<int>(val1) + eval_as_type<int>(val2);
      break;
    default:
      panic("bogus expression type");
    }

  return val;
}
struct Statistics::eval_value_t
Statistics::f_sub(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
	case et_double:
		val.type = et_double;
		val.value.as_double = eval_as_type<double>(val1)
				- eval_as_type<double>(val2);
		break;
	case et_float:
		val.type = et_float;
		val.value.as_float = eval_as_type<float>(val1)
				- eval_as_type<float>(val2);
		break;
#ifdef HOST_HAS_QWORD
	case et_qword:
		val.type = et_qword;
		val.value.as_qword = eval_as_type<qword_t>(val1)
				- eval_as_type<qword_t>(val2);
		break;
	case et_sqword:
		val.type = et_sqword;
		val.value.as_sqword = eval_as_type<sqword_t>(val1)
				- eval_as_type<sqword_t>(val2);
		break;
#endif /* HOST_HAS_QWORD */
	case et_addr:
		val.type = et_addr;
		val.value.as_addr = eval_as_type<unsigned long long>(val1)
				- eval_as_type<unsigned long long>(val2);
		break;
	case et_uint:
		val.type = et_uint;
		val.value.as_uint = eval_as_type<uint>(val1) - eval_as_type<uint>(val2);
		break;
	case et_int:
		val.type = et_int;
		val.value.as_int = eval_as_type<int>(val1) - eval_as_type<int>(val2);
		break;
    default:
      panic("bogus expression type");
    }

  return val;
}

struct Statistics::eval_value_t
Statistics::f_mult(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
  case et_double:
  		val.type = et_double;
  		val.value.as_double = eval_as_type<double>(val1)
  				* eval_as_type<double>(val2);
  		break;
  	case et_float:
  		val.type = et_float;
  		val.value.as_float = eval_as_type<float>(val1)
  				* eval_as_type<float>(val2);
  		break;
  #ifdef HOST_HAS_QWORD
  	case et_qword:
  		val.type = et_qword;
  		val.value.as_qword = eval_as_type<qword_t>(val1)
  				* eval_as_type<qword_t>(val2);
  		break;
  	case et_sqword:
  		val.type = et_sqword;
  		val.value.as_sqword = eval_as_type<sqword_t>(val1)
  				* eval_as_type<sqword_t>(val2);
  		break;
  #endif /* HOST_HAS_QWORD */
  	case et_addr:
  		val.type = et_addr;
  		val.value.as_addr = eval_as_type<unsigned long long>(val1)
  				* eval_as_type<unsigned long long>(val2);
  		break;
  	case et_uint:
  		val.type = et_uint;
  		val.value.as_uint = eval_as_type<uint>(val1) * eval_as_type<uint>(val2);
  		break;
  	case et_int:
  		val.type = et_int;
  		val.value.as_int = eval_as_type<int>(val1) * eval_as_type<int>(val2);
  		break;
    default:
      panic("bogus expression type");
    }

  return val;
}

struct Statistics::eval_value_t
Statistics::f_div(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
  case et_double:
  		val.type = et_double;
  		val.value.as_double = eval_as_type<double>(val1)
  				/ eval_as_type<double>(val2);
  		break;
  	case et_float:
  		val.type = et_float;
  		val.value.as_float = eval_as_type<float>(val1)
  				/ eval_as_type<float>(val2);
  		break;
  #ifdef HOST_HAS_QWORD
  	case et_qword:
  		val.type = et_qword;
  		val.value.as_qword = eval_as_type<qword_t>(val1)
  				/ eval_as_type<qword_t>(val2);
  		break;
  	case et_sqword:
  		val.type = et_sqword;
  		val.value.as_sqword = eval_as_type<sqword_t>(val1)
  				/ eval_as_type<sqword_t>(val2);
  		break;
  #endif /* HOST_HAS_QWORD */
  	case et_addr:
  		val.type = et_addr;
  		val.value.as_addr = eval_as_type<unsigned long long>(val1)
  				/ eval_as_type<unsigned long long>(val2);
  		break;
  	case et_uint:
  		val.type = et_uint;
  		val.value.as_uint = eval_as_type<uint>(val1) / eval_as_type<uint>(val2);
  		break;
  	case et_int:
  		val.type = et_int;
  		val.value.as_int = eval_as_type<int>(val1) / eval_as_type<int>(val2);
  		break;
    default:
      panic("bogus expression type");
    }

  return val;
}
struct Statistics::eval_value_t
Statistics::f_neg(struct eval_value_t val1)
{
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* result type is the same as the operand type */
  switch (val1.type)
    {
    case et_double:
      val.type = et_double;
      val.value.as_double = - val1.value.as_double;
      break;
    case et_float:
      val.type = et_float;
      val.value.as_float = - val1.value.as_float;
      break;
#ifdef HOST_HAS_QWORD
    case et_qword:
      val.type = et_sqword;
      val.value.as_qword = - (sqword_t)val1.value.as_qword;
      break;
    case et_sqword:
      val.type = et_sqword;
      val.value.as_sqword = - val1.value.as_sqword;
      break;
#endif /* HOST_HAS_QWORD */
    case et_addr:
      val.type = et_addr;
      val.value.as_addr = - val1.value.as_addr;
      break;
    case et_uint:
      if ((unsigned int)val1.value.as_uint > 2147483648U)
	{
	  /* promote type */
#ifdef HOST_HAS_QWORD
	  val.type = et_sqword;
	  val.value.as_sqword = - ((sqword_t)val1.value.as_uint);
#else /* !HOST_HAS_QWORD */
	  val.type = et_double;
	  val.value.as_double = - ((double)val1.value.as_uint);
#endif /* HOST_HAS_QWORD */
	}
      else
	{
	  /* don't promote type */
	  val.type = et_int;
	  val.value.as_int = - ((int)val1.value.as_uint);
	}
      break;
    case et_int:
      if ((unsigned int)val1.value.as_int == 0x80000000U)
	{
	  /* promote type */
	  val.type = et_uint;
	  val.value.as_uint = 2147483648U;
	}
      else
	{
	  /* don't promote type */
	  val.type = et_int;
	  val.value.as_int = - val1.value.as_int;
	}
      break;
    default:
      panic("bogus expression type");
    }

  return val;
}

/* compute val1 == 0 */
int
Statistics::f_eq_zero(struct eval_value_t val1)
{
  int val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return FALSE;
    }

  switch (val1.type)
    {
    case et_double:
      val = val1.value.as_double == 0.0;
      break;
    case et_float:
      val = val1.value.as_float == 0.0;
      break;
#ifdef HOST_HAS_QWORD
    case et_qword:
      val = val1.value.as_qword == 0;
      break;
    case et_sqword:
      val = val1.value.as_sqword == 0;
      break;
#endif /* HOST_HAS_QWORD */
    case et_addr:
      val = val1.value.as_addr == 0;
      break;
    case et_uint:
      val = val1.value.as_uint == 0;
      break;
    case et_int:
      val = val1.value.as_int == 0;
      break;
    default:
      panic("bogus expression type");
    }

  return val;
}

struct Statistics::eval_value_t		/* value of the literal constant */
Statistics::constant(struct eval_state_t *es)	/* expression evaluator */
{
  struct eval_value_t val;
  int int_val;
  unsigned int uint_val;
  double double_val;
  char *endp;
#ifdef HOST_HAS_QWORD
  sqword_t sqword_val;
  qword_t qword_val;
#endif /* HOST_HAS_QWORD */

#if 0 /* no longer needed... */
#if defined(sparc) && !defined(__svr4__)
  extern long strtol(char *, char **, int);
  extern double strtod(char *, char **);
#endif /* sparc */
#endif

  /*
   * attempt multiple conversions, from least to most precise, using
   * the value returned when the conversion is successful
   */

  /* attempt integer conversion */
  errno = 0;
  int_val = strtol(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_int;
      val.value.as_int = int_val;
      return val;
    }

  /* else, not an integer, attempt unsigned int conversion */
  errno = 0;
  uint_val = strtoul(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_uint;
      val.value.as_uint = uint_val;
      return val;
    }

#ifdef HOST_HAS_QWORD
  /* else, not an int/uint, attempt sqword_t conversion */
  errno = 0;
  sqword_val = myatosq(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_sqword;
      val.value.as_sqword = sqword_val;
      return val;
    }

  /* else, not an sqword_t, attempt qword_t conversion */
  errno = 0;
  qword_val = myatoq(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_qword;
      val.value.as_qword = qword_val;
      return val;
    }
#endif /* HOST_HAS_QWORD */

  /* else, not any type of integer, attempt double conversion (NOTE: no
     reliable float conversion is available on all machines) */
  errno = 0;
  double_val = strtod(es->tok_buf, &endp);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_double;
      val.value.as_double = double_val;
      return val;
    }

  /* else, not a double value, therefore, could not convert constant,
     declare an error */
  eval_error = ERR_BADCONST;
  return err_value;
}

struct Statistics::eval_value_t		/* value of factor */
Statistics::factor(struct eval_state_t *es)		/* expression evaluator */
{
  enum eval_token_t tok;
  struct eval_value_t val;

  tok = peek_next_token(es);
  switch (tok)
    {
    case tok_oparen:
      (void)get_next_token(es);
      val = expr(es);
      if (eval_error)
	return err_value;

      tok = peek_next_token(es);
      if (tok != tok_cparen)
	{
	  eval_error = ERR_UPAREN;
	  return err_value;
	}
      (void)get_next_token(es);
      break;

    case tok_minus:
      /* negation operator */
      (void)get_next_token(es);
      val = factor(es);
      if (eval_error)
	return err_value;
      val = f_neg(val);
      break;

    case tok_ident:
      (void)get_next_token(es);
      /* evaluate the identifier in TOK_BUF */
      val = stat_eval_ident(es);
      if (eval_error)
	return err_value;
      break;

    case tok_const:
      (void)get_next_token(es);
      val = constant(es);
      if (eval_error)
	return err_value;
      break;

    default:
      eval_error = ERR_NOTERM;
      return err_value;
    }

  return val;
}

struct Statistics::eval_value_t		/* value to expression term */
Statistics::term(struct eval_state_t *es)		/* expression evaluator */
{
  enum eval_token_t tok;
  struct eval_value_t val, val1;

  val = factor(es);
  if (eval_error)
    return err_value;

  tok = peek_next_token(es);
  switch (tok)
    {
    case tok_mult:
      (void)get_next_token(es);
      val = f_mult(val, term(es));
      if (eval_error)
	return err_value;
      break;

    case tok_div:
      (void)get_next_token(es);
      val1 = term(es);
      if (eval_error)
	return err_value;
      if (f_eq_zero(val1))
	{
	  eval_error = ERR_DIV0;
	  return err_value;
	}
      val = f_div(val, val1);
      break;

    default:;
    }

  return val;
}


struct Statistics::eval_value_t		/* value of the expression */
Statistics::expr(struct eval_state_t *es)		/* expression evaluator */
{
  enum eval_token_t tok;
  struct eval_value_t val;

  val = term(es);
  if (eval_error)
    return err_value;

  tok = peek_next_token(es);
  switch (tok)
    {
    case tok_plus:
      (void)get_next_token(es);
      val = f_add(val, expr(es));
      if (eval_error)
	return err_value;
      break;

    case tok_minus:
      (void)get_next_token(es);
      val = f_sub(val, expr(es));
      if (eval_error)
	return err_value;
      break;

    default:;
    }

  return val;
}

Statistics::eval_state_t *			/* expression evaluator */
Statistics::eval_new(Statistics::eval_ident_t f_eval_ident,	/* user ident evaluator */
	 void *user_ptr)		/* user ptr passed to ident fn */
{
  struct eval_state_t *es;

  es = (eval_state_t *)calloc(1, sizeof(struct eval_state_t));
  if (!es)
    fatal("out of virtual memory");

  es->f_eval_ident = f_eval_ident;
  es->user_ptr = user_ptr;

  return es;
}
void
Statistics::eval_delete(struct eval_state_t *es)	/* evaluator to delete */
{
  free(es);
}

struct Statistics::eval_value_t			/* value of the expression */
Statistics::eval_expr(struct eval_state_t *es,	/* expression evaluator */
	  char *p,			/* ptr to expression string */
	  char **endp)			/* returns ptr to 1st unused char */
{
  struct eval_value_t val;

  /* initialize the evaluator state */
  eval_error = ERR_NOERR;
  es->p = p;
  *es->tok_buf = '\0';
  es->peek_tok = tok_invalid;

  /* evaluate the expression */
  val = expr(es);

  /* return a pointer to the first character not used in the expression */
  if (endp)
    {
      if (es->peek_tok != tok_invalid)
	{
	  /* did not consume peek'ed token, so return last p */
	  *endp = es->lastp;
	}
      else
	*endp = es->p;
    }

  return val;
}

void
Statistics::eval_print(FILE *stream,		/* output stream */
	   struct eval_value_t val)	/* expression value to print */
{
  switch (val.type)
    {
    case et_double:
      fprintf(stream, "%f [double]", val.value.as_double);
      break;
    case et_float:
      fprintf(stream, "%f [float]", (double)val.value.as_float);
      break;
#ifdef HOST_HAS_QWORD
    case et_qword:
      myfprintf(stream, "%lu [qword_t]", val.value.as_qword);
      break;
    case et_sqword:
      myfprintf(stream, "%ld [sqword_t]", val.value.as_sqword);
      break;
#endif /* HOST_HAS_QWORD */
    case et_addr:
      myfprintf(stream, "0x%p [md_addr_t]", val.value.as_addr);
      break;
    case et_uint:
      fprintf(stream, "%u [uint]", val.value.as_uint);
      break;
    case et_int:
      fprintf(stream, "%d [int]", val.value.as_int);
      break;
    case et_symbol:
      fprintf(stream, "\"%s\" [symbol]", val.value.as_symbol);
      break;
    default:
      panic("bogus expression type");
    }
}


/*stats.cc*/
struct Statistics::eval_value_t
Statistics::stat_eval_ident(struct Statistics::eval_state_t *es)/* an expression evaluator */
{
  struct stat_sdb_t *sdb = (stat_sdb_t *)es->user_ptr;
  struct stat_stat_t *stat;
  static struct eval_value_t err_value = { et_int, { 0 } };
  struct eval_value_t val;

  /* locate the stat variable */
  for (stat = sdb->stats; stat != NULL; stat = stat->next)
    {
      if (!strcmp(stat->name, es->tok_buf))
	{
	  /* found it! */
	  break;
	}
    }
  if (!stat)
    {
      /* could not find stat variable */
      eval_error = ERR_UNDEFVAR;
      return err_value;
    }
  /* else, return the value of stat */

  /* convert the stat variable value to a typed expression value */
  switch (stat->sc)
    {
    case sc_int:
      val.type = et_int;
      val.value.as_int = *stat->variant.for_int.var;
      break;
    case sc_uint:
      val.type = et_uint;
      val.value.as_uint = *stat->variant.for_uint.var;
      break;
#ifdef HOST_HAS_QWORD
    case sc_qword:
      /* FIXME: cast to double, eval package doesn't support long long's */
      val.type = et_double;
#ifdef _MSC_VER /* FIXME: MSC does not implement qword_t to dbl conversion */
      val.value.as_double = (double)(sqword_t)*stat->variant.for_qword.var;
#else /* !_MSC_VER */
      val.value.as_double = (double)*stat->variant.for_qword.var;
#endif /* _MSC_VER */
      break;
    case sc_sqword:
      /* FIXME: cast to double, eval package doesn't support long long's */
      val.type = et_double;
      val.value.as_double = (double)*stat->variant.for_sqword.var;
      break;
#endif /* HOST_HAS_QWORD */
    case sc_float:
      val.type = et_float;
      val.value.as_float = *stat->variant.for_float.var;
      break;
    case sc_double:
      val.type = et_double;
      val.value.as_double = *stat->variant.for_double.var;
      break;
    case sc_dist:
    case sc_sdist:
      fatal("stat distributions not allowed in formula expressions");
      break;
    case sc_formula:
      {
	/* instantiate a new evaluator to avoid recursion problems */
	struct eval_state_t *es = eval_new(&Statistics::stat_eval_ident, sdb);
	char *endp;

	val = eval_expr(es, stat->variant.for_formula.formula, &endp);
	if (eval_error != ERR_NOERR || *endp != '\0')
	  {
	    /* pass through eval_error */
	    val = err_value;
	  }
	/* else, use value returned */
	eval_delete(es);
      }
      break;
    default:
      panic("bogus stat class");
    }

  return val;
}

/* create a new stats database */
struct Statistics::stat_sdb_t *Statistics::stat_new(void)
{
  struct stat_sdb_t *sdb;

  sdb = (struct stat_sdb_t *)calloc(1, sizeof(struct stat_sdb_t));
  if (!sdb)
    fatal("out of virtual memory");

  sdb->stats = NULL;
  sdb->evaluator = eval_new(&Statistics::stat_eval_ident, sdb);

  return sdb;
}

/* delete a stats database */
void
Statistics::stat_delete(struct stat_sdb_t *sdb)	/* stats database */
{
  int i;
  struct stat_stat_t *stat, *stat_next;
  struct bucket_t *bucket, *bucket_next;

  /* free all individual stat variables */
  for (stat = sdb->stats; stat != NULL; stat = stat_next)
    {
      stat_next = stat->next;
      stat->next = NULL;

      /* free stat */
      switch (stat->sc)
	{
	case sc_int:
	case sc_uint:
#ifdef HOST_HAS_QWORD
	case sc_qword:
	case sc_sqword:
#endif /* HOST_HAS_QWORD */
	case sc_float:
	case sc_double:
	case sc_formula:
	  /* no other storage to deallocate */
	  break;
	case sc_dist:
	  /* free distribution array */
	  free(stat->variant.for_dist.arr);
	  stat->variant.for_dist.arr = NULL;
	  break;
	case sc_sdist:
	  /* free all hash table buckets */
	  for (i=0; i<HTAB_SZ; i++)
	    {
	      for (bucket = stat->variant.for_sdist.sarr[i];
		   bucket != NULL;
		   bucket = bucket_next)
		{
		  bucket_next = bucket->next;
		  bucket->next = NULL;
		  free(bucket);
		}
	      stat->variant.for_sdist.sarr[i] = NULL;
	    }
	  /* free hash table array */
	  free(stat->variant.for_sdist.sarr);
	  stat->variant.for_sdist.sarr = NULL;
	  break;
	default:
	  panic("bogus stat class");
	}
      /* free stat variable record */
      free(stat);
    }
  sdb->stats = NULL;
  eval_delete(sdb->evaluator);
  sdb->evaluator = NULL;
  free(sdb);
}
void
Statistics::add_stat(struct stat_stat_t *stat)	/* stat variable */
{
  struct stat_stat_t *elt, *prev;

  /* append at end of stat database list */
  for (prev=NULL, elt=statdb->stats; elt != NULL; prev=elt, elt=elt->next)
    /* nada */;

  /* append stat to stats chain */
  if (prev != NULL)
    prev->next = stat;
  else /* prev == NULL */
	 statdb->stats = stat;
  stat->next = NULL;
}

void
Statistics::stat_reg_int(
	     const char *name,		/* stat variable name */
	     const char *desc,		/* stat variable description */
	     int *var,			/* stat variable */
	     int init_val,		/* stat variable initial value */
	     const char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;
  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");
  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : dbg_c1;
  stat->sc = sc_int;
  stat->variant.for_int.var = var;
  stat->variant.for_int.init_val = init_val;
  /* link onto SDB chain */
  add_stat(stat);
  /* initialize stat */
  *var = init_val;
}
void
Statistics::stat_reg_uint(
	     const char *name,		/* stat variable name */
	     const char *desc,		/* stat variable description */
	     uint *var,			/* stat variable */
	     uint init_val,		/* stat variable initial value */
	     const char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;
  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");
  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : dbg_c3;
  stat->sc = sc_uint;
  stat->variant.for_uint.var = var;
  stat->variant.for_uint.init_val = init_val;
  /* link onto SDB chain */
  add_stat(stat);
  /* initialize stat */
  *var = init_val;
}
void
Statistics::stat_reg_qword(
	     const char *name,		/* stat variable name */
	     const char *desc,		/* stat variable description */
		  qword_t *var,			/* stat variable */
		  qword_t init_val,		/* stat variable initial value */
	     const char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;
  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");
  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : dbg_c9;
  stat->sc = sc_qword;
  stat->variant.for_qword.var = var;
  stat->variant.for_qword.init_val = init_val;
  /* link onto SDB chain */
  add_stat(stat);
  /* initialize stat */
  *var = init_val;
}
void
Statistics::stat_reg_sqword(
	     const char *name,		/* stat variable name */
	     const char *desc,		/* stat variable description */
		  sqword_t *var,			/* stat variable */
		  sqword_t init_val,		/* stat variable initial value */
	     const char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;
  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");
  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : dbg_c10;
  stat->sc = sc_sqword;
  stat->variant.for_sqword.var = var;
  stat->variant.for_sqword.init_val = init_val;
  /* link onto SDB chain */
  add_stat(stat);
  /* initialize stat */
  *var = init_val;
}
void
Statistics::stat_reg_float(
	     const char *name,		/* stat variable name */
	     const char *desc,		/* stat variable description */
		  float *var,			/* stat variable */
		  float init_val,		/* stat variable initial value */
	     const char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;
  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");
  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : dbg_c5;
  stat->sc = sc_float;
  stat->variant.for_float.var = var;
  stat->variant.for_float.init_val = init_val;
  /* link onto SDB chain */
  add_stat(stat);
  /* initialize stat */
  *var = init_val;
}
void
Statistics::stat_reg_double(
	     const char *name,		/* stat variable name */
	     const char *desc,		/* stat variable description */
		  double *var,			/* stat variable */
		  double init_val,		/* stat variable initial value */
	     const char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;
  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");
  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : dbg_c5;
  stat->sc = sc_double;
  stat->variant.for_double.var = var;
  stat->variant.for_double.init_val = init_val;
  /* link onto SDB chain */
  add_stat(stat);
  /* initialize stat */
  *var = init_val;
}

void
Statistics::stat_reg_dist(
	      char *name,		/* stat variable name */
	      char *desc,		/* stat variable description */
	      unsigned int init_val,	/* dist initial value */
	      unsigned int arr_sz,	/* array size */
	      unsigned int bucket_sz,	/* array bucket size */
	      int pf,			/* print format, use PF_* defs */
	      char *format,		/* optional variable output format */
	      char **imap,		/* optional index -> string map */
	      print_fn_t print_fn)	/* optional user print function */
{
  unsigned int i;
  struct stat_stat_t *stat;
  unsigned int *arr;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");

  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : NULL;
  stat->sc = sc_dist;
  stat->variant.for_dist.init_val = init_val;
  stat->variant.for_dist.arr_sz = arr_sz;
  stat->variant.for_dist.bucket_sz = bucket_sz;
  stat->variant.for_dist.pf = pf;
  stat->variant.for_dist.imap = imap;
  stat->variant.for_dist.print_fn = print_fn;
  stat->variant.for_dist.overflows = 0;

  arr = (unsigned int *)calloc(arr_sz, sizeof(unsigned int));
  if (!arr)
    fatal("out of virtual memory");
  stat->variant.for_dist.arr = arr;

  /* link onto SDB chain */
  add_stat(stat);

  /* initialize stat */
  for (i=0; i < arr_sz; i++)
    arr[i] = init_val;
}

void
Statistics::stat_reg_sdist(
	       char *name,		/* stat variable name */
	       char *desc,		/* stat variable description */
	       unsigned int init_val,	/* dist initial value */
	       int pf,			/* print format, use PF_* defs */
	       char *format,		/* optional variable output format */
	       print_fn_t print_fn)	/* optional user print function */
{
  struct stat_stat_t *stat;
  struct bucket_t **sarr;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");

  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : NULL;
  stat->sc = sc_sdist;
  stat->variant.for_sdist.init_val = init_val;
  stat->variant.for_sdist.pf = pf;
  stat->variant.for_sdist.print_fn = print_fn;

  /* allocate hash table */
  sarr = (struct bucket_t **)calloc(HTAB_SZ, sizeof(struct bucket_t *));
  if (!sarr)
    fatal("out of virtual memory");
  stat->variant.for_sdist.sarr = sarr;
  /* link onto SDB chain */
  add_stat(stat);
}

void
Statistics::stat_add_samples(struct stat_stat_t *stat,/* stat database */
		 unsigned long long index,	/* distribution index of samples */
		 int nsamples)		/* number of samples to add to dist */
{
  switch (stat->sc)
    {
    case sc_dist:
      {
	unsigned int i;

	/* compute array index */
	i = index / stat->variant.for_dist.bucket_sz;

	/* check for overflow */
	if (i >= stat->variant.for_dist.arr_sz)
	  stat->variant.for_dist.overflows += nsamples;
	else
	  stat->variant.for_dist.arr[i] += nsamples;
      }
      break;
    case sc_sdist:
      {
	struct bucket_t *bucket;
	int hash = HTAB_HASH(index);

	if (hash < 0 || hash >= HTAB_SZ)
	  panic("hash table index overflow");

	/* find bucket */
	for (bucket = stat->variant.for_sdist.sarr[hash];
	     bucket != NULL;
	     bucket = bucket->next)
	  {
	    if (bucket->index == index)
	      break;
	  }
	if (!bucket)
	  {
	    /* add a new sample bucket */
	    bucket = (struct bucket_t *)calloc(1, sizeof(struct bucket_t));
	    if (!bucket)
	      fatal("out of virtual memory");
	    bucket->next = stat->variant.for_sdist.sarr[hash];
	    stat->variant.for_sdist.sarr[hash] = bucket;
	    bucket->index = index;
	    bucket->count = stat->variant.for_sdist.init_val;
	  }
	bucket->count += nsamples;
      }
      break;
    default:
      panic("stat variable is not an array distribution");
    }
}




void Statistics::stat_reg_formula(
		 const char *name,		/* stat variable name */
		 const char *desc,		/* stat variable description */
		 const char *formula,		/* formula expression */
		 const char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;
  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    fatal("out of virtual memory");
  stat->name = mystrdup(name);
  stat->desc = mystrdup(desc);
  stat->format = format ? format : dbg_c5;
  stat->sc = sc_formula;
  stat->variant.for_formula.formula = mystrdup(formula);
  /* link onto SDB chain */
  add_stat(stat);
}

/* add a single sample to array or sparse array distribution STAT */
void
Statistics::stat_add_sample(struct stat_stat_t *stat,/* stat variable */
		unsigned long long index)	/* index of sample */
{
  stat_add_samples(stat, index, 1);
}


int
Statistics::compare_fn(void *p1, void *p2)
{
  struct bucket_t **pb1 = (bucket_t **)p1, **pb2 = (bucket_t **)p2;

  /* compare indices */
  if ((*pb1)->index < (*pb2)->index)
    return -1;
  else if ((*pb1)->index > (*pb2)->index)
    return 1;
  else /* ((*pb1)->index == (*pb2)->index) */
    return 0;
}

void
Statistics::print_dist(struct stat_stat_t *stat,	/* stat variable */
	   FILE *fd)			/* output stream */
{
  unsigned int i, bcount, imax, imin;
  double btotal, bsum, bvar, bavg, bsqsum;
  int pf = stat->variant.for_dist.pf;

  /* count and sum entries */
  bcount = 0; btotal = 0.0; bvar = 0.0; bsqsum = 0.0;
  imax = 0; imin = UINT_MAX;
  for (i=0; i<stat->variant.for_dist.arr_sz; i++)
    {
      bcount++;
      btotal += stat->variant.for_dist.arr[i];
      /* on-line variance computation, tres cool, no!?! */
      bsqsum += ((double)stat->variant.for_dist.arr[i] *
		 (double)stat->variant.for_dist.arr[i]);
      bavg = btotal / MAX((double)bcount, 1.0);
      bvar = (bsqsum - ((double)bcount * bavg * bavg)) /
	(double)(((bcount - 1) > 0) ? (bcount - 1) : 1);
    }

  /* print header */
  fprintf(fd, "\n");
  fprintf(fd, "%-22s # %s\n", stat->name, stat->desc);
  fprintf(fd, "%s.array_size = %u\n",
	  stat->name, stat->variant.for_dist.arr_sz);
  fprintf(fd, "%s.bucket_size = %u\n",
	  stat->name, stat->variant.for_dist.bucket_sz);

  fprintf(fd, "%s.count = %u\n", stat->name, bcount);
  fprintf(fd, "%s.total = %.0f\n", stat->name, btotal);
  if (bcount > 0)
    {
      fprintf(fd, "%s.imin = %u\n", stat->name, 0U);
      fprintf(fd, "%s.imax = %u\n", stat->name, bcount);
    }
  else
    {
      fprintf(fd, "%s.imin = %d\n", stat->name, -1);
      fprintf(fd, "%s.imax = %d\n", stat->name, -1);
    }
  fprintf(fd, "%s.average = %8.4f\n", stat->name, btotal/MAX(bcount, 1.0));
  fprintf(fd, "%s.std_dev = %8.4f\n", stat->name, sqrt(bvar));
  fprintf(fd, "%s.overflows = %u\n",
	  stat->name, stat->variant.for_dist.overflows);

  fprintf(fd, "# pdf == prob dist fn, cdf == cumulative dist fn\n");
  fprintf(fd, "# %14s ", "index");
  if (pf & PF_COUNT)
    fprintf(fd, "%10s ", "count");
  if (pf & PF_PDF)
    fprintf(fd, "%6s ", "pdf");
  if (pf & PF_CDF)
    fprintf(fd, "%6s ", "cdf");
  fprintf(fd, "\n");

  fprintf(fd, "%s.start_dist\n", stat->name);

  if (bcount > 0)
    {
      /* print the array */
      bsum = 0.0;
      for (i=0; i<bcount; i++)
	{
	  bsum += (double)stat->variant.for_dist.arr[i];
	  if (stat->variant.for_dist.print_fn)
	    {
	      stat->variant.for_dist.print_fn(stat,
					      i,
					      stat->variant.for_dist.arr[i],
					      bsum,
					      btotal);
	    }
	  else
	    {
	      if (stat->format == NULL)
		{
		  if (stat->variant.for_dist.imap)
		    fprintf(fd, "%-16s ", stat->variant.for_dist.imap[i]);
		  else
		    fprintf(fd, "%16u ",
			    i * stat->variant.for_dist.bucket_sz);
		  if (pf & PF_COUNT)
		    fprintf(fd, "%10u ", stat->variant.for_dist.arr[i]);
		  if (pf & PF_PDF)
		    fprintf(fd, "%6.2f ",
			    (double)stat->variant.for_dist.arr[i] /
			    MAX(btotal, 1.0) * 100.0);
		  if (pf & PF_CDF)
		    fprintf(fd, "%6.2f ", bsum/MAX(btotal, 1.0) * 100.0);
		}
	      else
		{
		  if (pf == (PF_COUNT|PF_PDF|PF_CDF))
		    {
		      if (stat->variant.for_dist.imap)
		        fprintf(fd, stat->format,
			        stat->variant.for_dist.imap[i],
			        stat->variant.for_dist.arr[i],
			        (double)stat->variant.for_dist.arr[i] /
			        MAX(btotal, 1.0) * 100.0,
			        bsum/MAX(btotal, 1.0) * 100.0);
		      else
		        fprintf(fd, stat->format,
			        i * stat->variant.for_dist.bucket_sz,
			        stat->variant.for_dist.arr[i],
			        (double)stat->variant.for_dist.arr[i] /
			        MAX(btotal, 1.0) * 100.0,
			        bsum/MAX(btotal, 1.0) * 100.0);
		    }
		  else
		    fatal("distribution format not yet implemented");
		}
	      fprintf(fd, "\n");
	    }
	}
    }

  fprintf(fd, "%s.end_dist\n", stat->name);
}
void
Statistics::print_sdist(struct stat_stat_t *stat,	/* stat variable */
	    FILE *fd)			/* output stream */
{
  unsigned int i, bcount;
  unsigned long long imax, imin;
  double btotal, bsum, bvar, bavg, bsqsum;
  struct bucket_t *bucket;
  int pf = stat->variant.for_sdist.pf;

  /* count and sum entries */
  bcount = 0; btotal = 0.0; bvar = 0.0; bsqsum = 0.0;
  imax = 0; imin = UINT_MAX;
  for (i=0; i<HTAB_SZ; i++)
    {
      for (bucket = stat->variant.for_sdist.sarr[i];
	   bucket != NULL;
	   bucket = bucket->next)
	{
	  bcount++;
	  btotal += bucket->count;
	  /* on-line variance computation, tres cool, no!?! */
	  bsqsum += ((double)bucket->count * (double)bucket->count);
	  bavg = btotal / (double)bcount;
	  bvar = (bsqsum - ((double)bcount * bavg * bavg)) /
	    (double)(((bcount - 1) > 0) ? (bcount - 1) : 1);
	  if (bucket->index < imin)
	    imin = bucket->index;
	  if (bucket->index > imax)
	    imax = bucket->index;
	}
    }

  /* print header */
  fprintf(fd, "\n");
  fprintf(fd, "%-22s # %s\n", stat->name, stat->desc);
  fprintf(fd, "%s.count = %u\n", stat->name, bcount);
  fprintf(fd, "%s.total = %.0f\n", stat->name, btotal);
  if (bcount > 0)
    {
      myfprintf(fd, "%s.imin = 0x%p\n", stat->name, imin);
      myfprintf(fd, "%s.imax = 0x%p\n", stat->name, imax);
    }
  else
    {
      fprintf(fd, "%s.imin = %d\n", stat->name, -1);
      fprintf(fd, "%s.imax = %d\n", stat->name, -1);
    }
  fprintf(fd, "%s.average = %8.4f\n", stat->name, btotal/bcount);
  fprintf(fd, "%s.std_dev = %8.4f\n", stat->name, sqrt(bvar));
  fprintf(fd, "%s.overflows = 0\n", stat->name);

  fprintf(fd, "# pdf == prob dist fn, cdf == cumulative dist fn\n");
  fprintf(fd, "# %14s ", "index");
  if (pf & PF_COUNT)
    fprintf(fd, "%10s ", "count");
  if (pf & PF_PDF)
    fprintf(fd, "%6s ", "pdf");
  if (pf & PF_CDF)
    fprintf(fd, "%6s ", "cdf");
  fprintf(fd, "\n");

  fprintf(fd, "%s.start_dist\n", stat->name);

  if (bcount > 0)
    {
      unsigned int bindex;
      struct bucket_t **barr;

      /* collect all buckets */
      barr = (struct bucket_t **)calloc(bcount, sizeof(struct bucket_t *));
      if (!barr)
	fatal("out of virtual memory");
      for (bindex=0,i=0; i<HTAB_SZ; i++)
	{
	  for (bucket = stat->variant.for_sdist.sarr[i];
	       bucket != NULL;
	       bucket = bucket->next)
	    {
	      barr[bindex++] = bucket;
	    }
	}

      /* sort the array by index */
      qsort(barr, bcount, sizeof(struct bucket_t *), (int (*)(const void *, const void *))(&Statistics::compare_fn));

      /* print the array */
      bsum = 0.0;
      for (i=0; i<bcount; i++)
	{
	  bsum += (double)barr[i]->count;
	  if (stat->variant.for_sdist.print_fn)
	    {
	      stat->variant.for_sdist.print_fn(stat,
					       barr[i]->index,
					       barr[i]->count,
					       bsum,
					       btotal);
	    }
	  else
	    {
	      if (stat->format == NULL)
		{
		  myfprintf(fd, "0x%p ", barr[i]->index);
		  if (pf & PF_COUNT)
		    fprintf(fd, "%10u ", barr[i]->count);
		  if (pf & PF_PDF)
		    fprintf(fd, "%6.2f ",
			    (double)barr[i]->count/MAX(btotal, 1.0) * 100.0);
		  if (pf & PF_CDF)
		    fprintf(fd, "%6.2f ", bsum/MAX(btotal, 1.0) * 100.0);
		}
	      else
		{
		  if (pf == (PF_COUNT|PF_PDF|PF_CDF))
		    {
		      myfprintf(fd, stat->format,
				barr[i]->index, barr[i]->count,
				(double)barr[i]->count/MAX(btotal, 1.0)*100.0,
				bsum/MAX(btotal, 1.0) * 100.0);
		    }
		  else if (pf == (PF_COUNT|PF_PDF))
		    {
		      myfprintf(fd, stat->format,
				barr[i]->index, barr[i]->count,
				(double)barr[i]->count/MAX(btotal, 1.0)*100.0);
		    }
		  else if (pf == PF_COUNT)
		    {
		      myfprintf(fd, stat->format,
				barr[i]->index, barr[i]->count);
		    }
		  else
		    fatal("distribution format not yet implemented");
		}
	      fprintf(fd, "\n");
	    }
	}

      /* all done, release bucket pointer array */
      free(barr);
    }

  fprintf(fd, "%s.end_dist\n", stat->name);
}
void
Statistics::stat_print_stat(
		struct stat_stat_t *stat,/* stat variable */
		FILE *fd)		/* output stream */
{
  struct eval_value_t val;

  switch (stat->sc)
    {
    case sc_int:
      fprintf(fd, "%-22s ", stat->name);
      myfprintf(fd, stat->format, *stat->variant.for_int.var);
      fprintf(fd, " # %s", stat->desc);
      break;
    case sc_uint:
      fprintf(fd, "%-22s ", stat->name);
      myfprintf(fd, stat->format, *stat->variant.for_uint.var);
      fprintf(fd, " # %s", stat->desc);
      break;
#ifdef HOST_HAS_QWORD
    case sc_qword:
      {
	char buf[128];

	fprintf(fd, "%-22s ", stat->name);
	mysprintf(buf, stat->format, *stat->variant.for_qword.var);
	fprintf(fd, "%s # %s", buf, stat->desc);
      }
      break;
    case sc_sqword:
      {
	char buf[128];

	fprintf(fd, "%-22s ", stat->name);
	mysprintf(buf, stat->format, *stat->variant.for_sqword.var);
	fprintf(fd, "%s # %s", buf, stat->desc);
      }
      break;
#endif /* HOST_HAS_QWORD */
    case sc_float:
      fprintf(fd, "%-22s ", stat->name);
      myfprintf(fd, stat->format, (double)*stat->variant.for_float.var);
      fprintf(fd, " # %s", stat->desc);
      break;
    case sc_double:
      fprintf(fd, "%-22s ", stat->name);
      myfprintf(fd, stat->format, *stat->variant.for_double.var);
      fprintf(fd, " # %s", stat->desc);
      break;
    case sc_dist:
      print_dist(stat, fd);
      break;
    case sc_sdist:
      print_sdist(stat, fd);
      break;
    case sc_formula:
      {
	/* instantiate a new evaluator to avoid recursion problems */
	struct eval_state_t *es = eval_new(&Statistics::stat_eval_ident, statdb);
	char *endp;

	fprintf(fd, "%-22s ", stat->name);
	val = eval_expr(es, stat->variant.for_formula.formula, &endp);
	if (eval_error != ERR_NOERR || *endp != '\0')
	  fprintf(fd, "<error: %s>", eval_err_str[eval_error]);
	else
	  myfprintf(fd, stat->format, eval_as_type<double>(val));
	fprintf(fd, " # %s", stat->desc);

	/* done with the evaluator */
	eval_delete(es);
      }
      break;
    default:
      panic("bogus stat class");
    }
  fprintf(fd, "\n");
}
void
Statistics::stat_print_stats(FILE *fd)		/* output stream */
{
  struct stat_stat_t *stat;

  if (!statdb)
    {
      /* no stats */
      return;
    }
  for (stat=statdb->stats; stat != NULL; stat=stat->next)
    stat_print_stat(stat, fd);
}
struct Statistics::stat_stat_t *
Statistics::stat_find_stat(char *stat_name)		/* stat name */
{
  struct stat_stat_t *stat;

  for (stat = statdb->stats; stat != NULL; stat = stat->next)
    {
      if (!strcmp(stat->name, stat_name))
	break;
    }
  return stat;
}

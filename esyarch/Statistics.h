/*
 * Statistics.h
 *
 *  Created on: 2016年4月20日
 *      Author: root
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_

#include "host.h"
#include "misc.h"

/*define*/
#define GET_VAR_NAME(VAR)	#VAR
/* expression tokens */
enum eval_token_t {
    tok_ident,		/* user-valued identifiers */
    tok_const,		/* numeric literals */
    tok_plus,		/* `+' */
    tok_minus,		/* `-' */
    tok_mult,		/* `*' */
    tok_div,		/* `/' */
    tok_oparen,		/* `(' */
    tok_cparen,		/* `)' */
    tok_eof,		/* end of file */
    tok_whitespace,	/* ` ', `\t', `\n' */
    tok_invalid		/* unrecognized token */
};

class Statistics 
{
    //define of eval.cc/h
    #if defined(sparc) && !defined(__svr4__)
    #define strtoul strtol
    #endif /* sparc */
    /* non-zero if type is an integral type */
    #ifdef HOST_HAS_QWORD
    #define EVAL_INTEGRAL(TYPE)						\
        ((TYPE) == et_int || (TYPE) == et_uint || (TYPE) == et_addr		\
        || (TYPE) == et_qword || (TYPE) == et_sqword)
    #else /* !HOST_HAS_QWORD */
    #define EVAL_INTEGRAL(TYPE)						\
        ((TYPE) == et_int || (TYPE) == et_uint || (TYPE) == et_addr)
    #endif /* HOST_HAS_QWORD */

    //define of stats.cc/h
    /* sparse array distributions are implemented with a hash table */
    #define HTAB_SZ			1024
    #define HTAB_HASH(I)		((((I) >> 8) ^ (I)) & (HTAB_SZ - 1))
    /* enable distribution components:  index, count, probability, cumulative */
    #define PF_COUNT		0x0001
    #define PF_PDF			0x0002
    #define PF_CDF			0x0004
    #define PF_ALL			(PF_COUNT|PF_PDF|PF_CDF)
public:
    Statistics();
    ~Statistics();

    struct eval_state_t;
    struct eval_value_t;
    struct stat_stat_t;
    typedef struct Statistics::eval_value_t		  /* value of the identifier */
    (Statistics::*eval_ident_t)(struct Statistics::eval_state_t *es); /* ident 
    string in es->tok_buf */

    /* an evaluator state record */
    struct eval_state_t 
    {
        char *p;			/* ptr to next char to consume from expr */
        char *lastp;			/* save space for token peeks */
        eval_ident_t f_eval_ident;	/* identifier evaluator */
        void *user_ptr;		/* user-supplied argument pointer */
        char tok_buf[512];		/* text of last token returned */
        enum eval_token_t peek_tok;	/* peek buffer, for one token look-ahead */
    };
    /* evaluation errors */
    enum eval_err_t 
    {
        ERR_NOERR,			/* no error */
        ERR_UPAREN,			/* unmatched parenthesis */
        ERR_NOTERM,			/* expression term is missing */
        ERR_DIV0,			/* divide by zero */
        ERR_BADCONST,			/* badly formed constant */
        ERR_BADEXPR,			/* badly formed constant */
        ERR_UNDEFVAR,			/* variable is undefined */
        ERR_EXTRA,			/* extra characters at end of expression */
        ERR_NUM
    };
    /* enum eval_err_t -> error description string map */
    static const char *eval_err_str[ERR_NUM];
    /* expression value types */
    enum eval_type_t 
    {
        et_int,			/* signed integer result */
        et_uint,			/* unsigned integer result */
        et_addr,			/* address value */
    #ifdef HOST_HAS_QWORD
        et_qword,			/* unsigned qword length integer result */
        et_sqword,			/* signed qword length integer result */
    #endif /* HOST_HAS_QWORD */
        et_float,			/* single-precision floating point value */
        et_double,			/* double-precision floating point value */
        et_symbol,			/* non-numeric result (!allowed in exprs)*/
        et_NUM
    };
    /* expression type strings */
    static const char *eval_type_str[et_NUM];
    /* an expression value */
    struct eval_value_t 
    {
        enum eval_type_t type;		/* type of expression value */
        union 
        {
        int as_int;				/* value for type == et_int */
        unsigned int as_uint;		/* value for type == et_uint */
        unsigned long long as_addr;			/* value for type == et_addr */
    #ifdef HOST_HAS_QWORD
        qword_t as_qword;			/* value for type == ec_qword */
        sqword_t as_sqword;			/* value for type == ec_sqword */
    #endif /* HOST_HAS_QWORD */
        float as_float;			/* value for type == et_float */
        double as_double;			/* value for type == et_double */
        char *as_symbol;			/* value for type == et_symbol */
        } value;
    };

    /* stat variable classes */
    enum stat_class_t 
    {
        sc_int = 0,			/* integer stat */
        sc_uint,			/* unsigned integer stat */
    #ifdef HOST_HAS_QWORD
        sc_qword,			/* qword integer stat */
        sc_sqword,			/* signed qword integer stat */
    #endif /* HOST_HAS_QWORD */
        sc_float,			/* single-precision FP stat */
        sc_double,			/* double-precision FP stat */
        sc_dist,			/* array distribution stat */
        sc_sdist,			/* sparse array distribution stat */
        sc_formula,			/* stat expression formula */
        sc_NUM
    };
    /* hash table bucket definition */
    struct bucket_t 
    {
        struct bucket_t *next;	/* pointer to the next bucket */
        unsigned long long index;		/* bucket index - as large as an addr */
        unsigned int count;		/* bucket count */
    };
    typedef void (*print_fn_t)(
        struct stat_stat_t *stat,	/* the stat variable being printed */
        unsigned long long index,		/* entry index to print */
        int count,		/* entry count */
        double sum,		/* cumulative sum */
        double total);		/* total count for distribution */
    /* statistical variable definition */
    struct stat_stat_t 
    {
        struct stat_stat_t *next;	/* pointer to next stat in database list */
        char *name;			/* stat name */
        char *desc;			/* stat description */
        const char *format;			/* stat output print format */
        enum stat_class_t sc;		/* stat class */
        union stat_variant_t 
        {
            /* sc == sc_int */
            struct stat_for_int_t 
            {
                int *var;			/* integer stat variable */
                int init_val;		/* initial integer value */
            } for_int;
            /* sc == sc_uint */
            struct stat_for_uint_t 
            {
                unsigned int *var;	/* unsigned integer stat variable */
                unsigned int init_val;	/* initial unsigned integer value */
            } for_uint;
        #ifdef HOST_HAS_QWORD
            /* sc == sc_qword */
            struct stat_for_qword_t 
            {
                qword_t *var;		/* qword integer stat variable */
                qword_t init_val;		/* qword integer value */
            } for_qword;
            /* sc == sc_sqword */
            struct stat_for_sqword_t 
            {
                sqword_t *var;		/* signed qword integer stat variable */
                sqword_t init_val;	/* signed qword integer value */
            } for_sqword;
        #endif /* HOST_HAS_QWORD */
            /* sc == sc_float */
            struct stat_for_float_t 
            {
                float *var;		/* float stat variable */
                float init_val;		/* initial float value */
            } for_float;
            /* sc == sc_double */
            struct stat_for_double_t 
            {
                double *var;		/* double stat variable */
                double init_val;		/* initial double value */
            } for_double;
            /* sc == sc_dist */
            struct stat_for_dist_t 
            {
                unsigned int init_val;	/* initial dist value */
                unsigned int *arr;	/* non-sparse array pointer */
                unsigned int arr_sz;	/* array size */
                unsigned int bucket_sz;	/* array bucket size */
                int pf;			/* printables */
                char **imap;		/* index -> string map */
                print_fn_t print_fn;	/* optional user-specified print fn */
                unsigned int overflows;	// total overflows in stat_add_samples()
            } for_dist;
            /* sc == sc_sdist */
            struct stat_for_sdist_t 
            {
                unsigned int init_val;	/* initial dist value */
                struct bucket_t **sarr;	/* sparse array pointer */
                int pf;			/* printables */
                print_fn_t print_fn;	/* optional user-specified print fn */
            } for_sdist;
            /* sc == sc_formula */
            struct stat_for_formula_t 
            {
                char *formula;		/* stat formula, see eval.h for format */
            } for_formula;
        } variant;
    };
    /* statistical database */
    struct stat_sdb_t 
    {
        struct stat_stat_t *stats;		/* list of stats in database */
        struct eval_state_t *evaluator;	/* an expression evaluator */
    };

    enum eval_err_t eval_error;
    int tok_map_initialized;
    static enum eval_token_t tok_map[256];
    static const struct eval_value_t err_value;

    FILE* reg_file;
    stat_sdb_t *statdb;
    //functions
    /*eval.cc*/
    static void init_tok_map(void);
    enum eval_token_t get_next_token(struct eval_state_t *es);
    enum eval_token_t peek_next_token(struct eval_state_t *es);
    enum eval_type_t result_type(enum eval_type_t t1, enum eval_type_t t2);
    template<typename Ts> Ts eval_as_type(struct eval_value_t val);
    struct eval_value_t f_add(
        struct eval_value_t val1, struct eval_value_t val2);
    struct eval_value_t f_sub(
        struct eval_value_t val1, struct eval_value_t val2);
    struct eval_value_t f_mult(
        struct eval_value_t val1, struct eval_value_t val2);
    struct eval_value_t f_div(
        struct eval_value_t val1, struct eval_value_t val2);
    struct eval_value_t f_neg(struct eval_value_t val1);
    int f_eq_zero(struct eval_value_t val1);
    struct eval_value_t constant(struct eval_state_t *es);
    struct eval_value_t factor(struct eval_state_t *es);
    struct eval_value_t term(struct eval_state_t *es);
    struct eval_value_t expr(struct eval_state_t *es);
    eval_state_t * eval_new(
        Statistics::eval_ident_t f_eval_ident, void *user_ptr);
    void eval_delete(struct eval_state_t *es);
    struct eval_value_t	eval_expr(
        struct eval_state_t *es,	/* expression evaluator*/
        char *p,			/* ptr to expression string */
        char **endp);
    void eval_print(FILE *stream,struct eval_value_t val);
    /*stats.cc*/
    struct eval_value_t stat_eval_ident(struct Statistics::eval_state_t *es);
    struct stat_sdb_t *stat_new(void);
    void stat_delete(struct stat_sdb_t *sdb);
    void add_stat(struct stat_stat_t *stat);
    void stat_reg_int(
        const char *name,		/* stat variable name */
        const char *desc,		/* stat variable description */
        int *var,	/* stat variable */
        int init_val,	/* stat variable initial value */
        const char *format);
    void stat_reg_uint(
        const char *name, /* stat variable name */
        const char *desc, /* stat variable description */
        uint *var, /* stat variable */
        uint init_val, /* stat variable initial value */
        const char *format);
    void stat_reg_qword(
        const char *name, /* stat variable name */
        const char *desc, /* stat variable description */
        qword_t *var, /* stat variable */
        qword_t init_val, /* stat variable initial value */
        const char *format);
    void stat_reg_sqword(
        const char *name, /* stat variable name */
        const char *desc, /* stat variable description */
        sqword_t *var, /* stat variable */
        sqword_t init_val, /* stat variable initial value */
        const char *format);
    void stat_reg_float(
        const char *name, /* stat variable name */
        const char *desc, /* stat variable description */
        float *var, /* stat variable */
        float init_val, /* stat variable initial value */
        const char *format);
    void stat_reg_double(
        const char *name, /* stat variable name */
        const char *desc, /* stat variable description */
        double *var, /* stat variable */
        double init_val, /* stat variable initial value */
        const char *format);
    void stat_reg_dist(
        char *name,		/* stat variable name */
        char *desc,		/* stat variable description */
        unsigned int init_val,	/* dist initial value */
        unsigned int arr_sz,	/* array size */
        unsigned int bucket_sz,	/* array bucket size */
        int pf,			/* print format, use PF_* defs */
        char *format,		/* optional variable output format */
        char **imap,		/* optional index -> string map */
        print_fn_t print_fn);
    void stat_add_samples(struct stat_stat_t *stat,/* stat database */
        unsigned long long index,	/* distribution index of samples */
        int nsamples);
    void stat_add_sample(struct stat_stat_t *stat,/* stat variable */
        unsigned long long index);
    void stat_reg_sdist(
        char *name,		/* stat variable name */
        char *desc,		/* stat variable description */
        unsigned int init_val,	/* dist initial value */
        int pf,			/* print format, use PF_* defs */
        char *format,		/* optional variable output format */
        print_fn_t print_fn);
    void stat_reg_formula(
        const char *name,		/* stat variable name */
        const char *desc,		/* stat variable description */
        const char *formula,		/* formula expression */
        const char *format);
    int compare_fn(void *p1, void *p2);
    void print_dist(struct stat_stat_t *stat,	/* stat variable */
         FILE *fd);
    void print_sdist(struct stat_stat_t *stat,	/* stat variable */
         FILE *fd);
    void stat_print_stat(
         struct stat_stat_t *stat,/* stat variable */
         FILE *fd);
    void stat_print_stats(FILE *fd);
    struct stat_stat_t * stat_find_stat(char *stat_name);
};

#endif /* STATISTICS_H_ */

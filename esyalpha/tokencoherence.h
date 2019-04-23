/*
 * Header for tokencoherence.c
 *
 * $Log: tokencoherence.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:28  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:53  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.7  2004/08/19 20:55:12  grbriggs
 * Add issue_cycle to tc_commit_predict prototype.
 *
 * Revision 1.6  2004/08/03 22:07:30  grbriggs
 * Reduce corner cases, improve accuracy.
 *
 * Revision 1.5  2004/08/03 01:34:17  ninelson
 * TOKENB is in smt.h only
 *
 * Revision 1.4  2004/07/29 05:12:54  grbriggs
 * Version Alpha Preview 2, with many more corner cases non-fatal.
 *
 * Revision 1.3  2004/07/28 19:14:09  grbriggs
 * This is version Alpha Preview 1 :-)
 *
 * Revision 1.2  2004/07/28 04:54:38  grbriggs
 * Still a work in progress.
 *
 */

//#define TOKENB

#define TC_TOTAL_NONOWNER_TOKEN_COUNT	(CLUSTERS)
					/* minus one for the owner, but plus one for main memory */

#define STARVE_CYCLES 100

typedef enum {shared,exclusive,persistent} tc_request_type;

/* This is the message that is passed on the links */
typedef struct {
    int		has_data; /* boolean */
    int		has_owner_token; /* boolean */
    int		nonowner_token_count; /* number of other tokens I hold for this address */
    md_addr_t	addr; /* the address of this block */
} tc_messagetoken_ack;

typedef struct {
    int			from; /* who sent the message */
    tc_request_type	type;
} tc_messagetoken_req;

/* One tc_blockinfo should be associated with each cache block 
 * of a given cluster core. */
typedef struct tc_blockinfo_struct
{
    struct tc_blockinfo_struct * next; /*next in time; reserved for use by get/put blockinfo*/
    char	exists; /* boolean: whether this block really exists, or is just a placeholder */
    char	valid;	/*boolean: whether the data is valid according to the correctness protocol */
    char	i_am_owner; /*boolean: whether this cluster holds the owner token */
    char	nonowner_token_count; /* number of other tokens I hold for this address */
    md_addr_t	addr; /* the address of this block */
#if 0
    counter_t	read_starve_cycle; /* sim cycle when read was requested + STARVE_CYCLES, -1 if no request */
    counter_t   write_starve_cycle; /* sim cycle when write was requested + STARVE_CYCLES, -1 if no request */
#endif
    short	read_in_pipeline; /* number of instructions reading this address that are in the pipe */
    short	write_in_pipeline; /* ditto for writes */
    char	modified; /* boolean, indicates if the current cluster has modified this block */
    counter_t	state_as_of; /* when did this state go into effect? */
} tc_blockinfo;

typedef struct hashtable_entry_struct
{
    struct hashtable_entry_struct * 	next;
    md_addr_t				addr;
    int					cache_number;
    tc_blockinfo *			blockinfo_head;
} hashtable_entry;

#if 0
/* arbiter's request list */
typedef struct tc_arbiter_entry_struct
{
    md_addr_t	addr;
    struct tc_arbiter_entry_struct * next; /* linked list */
} tc_arbiter_entry;

typedef struct
{
    tc_arbiter_entry * head; /* linked list */
} tc_arbiter;
#endif

/*
 * external interface functions 
 */

/* allocate memory. num_blocks is number of blocks per cache */
void tc_init(int num_caches, int num_blocks, int block_size);

void tc_reg_stats(struct stat_sdb_t *sdb);

/* on a cache read or write. returns lat. */
int tc_access(md_addr_t addr, int cache_number,int cluster,  int is_write, int is_hint, counter_t cycle);

/* tell when a commit happens */
void tc_commit_predict(md_addr_t addr,int cache_number, int is_write, counter_t cycle, counter_t issue_cycle);
void tc_commit_real(md_addr_t addr,int cache_number, int is_write, counter_t predicted_cycle, counter_t real_cycle);

/* when a new block comes into cache, call this */
void tc_block_alloc(md_addr_t addr, int cache_number, counter_t cycle);

/* when block kicked out of cache, call this */
void tc_block_free(md_addr_t addr, int cache_number, counter_t cycle);

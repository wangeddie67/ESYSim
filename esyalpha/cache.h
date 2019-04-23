/*
 * cache.h - cache module interfaces
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
 * $Id: cache.h,v 1.1.1.1 2011/03/29 01:04:34 huq Exp $
 *
 * $Log: cache.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.3  2010/04/30 19:05:57  xue
 * *** empty log message ***
 *
 * Revision 1.2  2010/02/19 21:37:32  xue
 * optimization on cache coherece (directly reply for miss read)
 *
 * Revision 1.1.1.1  2010/02/18 21:22:14  xue
 * CMP_NETWORK
 *
 * Revision 1.6  2009/07/09 20:36:32  garg
 * Changes related to write buffer.
 *
 * Revision 1.5  2009/06/01 14:51:11  xue
 * Off chip memory modeling + split notification + multiple virtual channels for 
esynet
 *
 * Revision 1.4  2009/05/15 19:14:11  garg
 * Removed branch predictor stats collect bug. Dispatch stall statistics.
 * Additional statistics.
 *
 * Revision 1.3  2009/03/05 16:58:23  garg
 * Updated version of the CMP network simulator.
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.6  2004/08/02 17:19:05  ninelson
 * added support for TokenB
 *
 * Revision 1.5  2004/07/08 20:58:13  grbriggs
 * Add support for a distributed L2 cache that uses the cluster's comm links to reach non-local banks.
 * Currently, there is no optimization to put data closer to the cache that's using it.
 *
 * Revision 1.4  2004/07/02 18:49:43  grbriggs
 * This is 1.3.1.1. Silly RCS.
 *
 * Revision 1.3  2004/07/02 18:21:15  grbriggs
 * Nick points out another malformed structure, now fixed.
*
* Revision 1.2  2004/06/18 21:49:19  grbriggs
* Initial move of code from garg.
*
* Revision 1.1  2004/05/14 14:48:41  ninelson
* Initial revision
*
* Revision 1.1.1.1  2003/10/20 13:58:35  ninelson
* new from Ali
*
* Revision 1.5  1998/08/27 08:09:13  taustin
* implemented host interface description in host.h
* added target interface support
*
* Revision 1.4  1997/03/11  01:09:45  taustin
* updated copyright
* long/int tweaks made for ALPHA target support
*
* Revision 1.3  1997/01/06  15:57:55  taustin
* comments updated
* cache_reg_stats() now works with stats package
* cp->writebacks stat added to cache
*
* Revision 1.1  1996/12/05  18:50:23  taustin
* Initial revision
*
*
*/

#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "memory.h"
#include "stats.h"

#include "smt.h"
#include "interconnect.h"
#include "sim-outorder.h"

#define PAGE_NUM (128*1024)
#define MAX_DIRECTORY_ASSOC 16

/* ******************* CMP architecture **************************/

/* ******************* Cache coherence protocol ********************/
#define SILENT_DROP
//#define NON_SILENT_DROP

/* ******************* Network **************************************/
#define OPTIMIZED_OPTICAL_NETWORK
//#define BASELINE_OPTICAL_NETWORK  /* also for ideal case */
#define MESH_NETWORK

/* ******************* MESH network (conventional)*************************/
#ifdef MESH_NETWORK
  #define ESYNET
  /* cache coherence protocol optimization */
 // #define CACHE_COHERENCE_OPTIMIZATION
  #define MULTI_VC
#endif

/* ****************** Baseline optical network ****************************/
#ifdef BASELINE_OPTICAL_NETWORK
  #define OPTICAL_INTERCONNECT
  /* cache coherence protocol optimization */
  //#define CACHE_COHERENCE_OPTIMIZATION
#endif

/* ****************** Optimized optical network ***************************/
#ifdef OPTIMIZED_OPTICAL_NETWORK
  #define OPTICAL_INTERCONNECT      //for optical interconnect

  /* conflict handling optimization */
  #define INV_ACK_CON                   //invaliation using confirmation bits --- optimization
  #define WB_SPLIT_NET              //write back split  --- optimization
  /* cache coherence protocol optimization */
  //#define CACHE_COHERENCE_OPTIMIZATION
#endif


#define TSHR

/* ***************** Cache coherence protocol optimization *******************/
#ifdef CACHE_COHERENCE_OPTIMIZATION
  #ifdef RELAXED_CONSISTENCY
    #define SERIALIZATION_ACK
    #define READ_PERMIT
#ifdef OPTICAL_INTERCONNECT
    #define EUP_NETWORK
#endif
    
  #endif
#endif

#ifdef ESYNET
void OrderBufRemove(int s1, int s2, int d1, int d2, md_addr_t addr, counter_t msgno);
void OrderBufInsert(int s1, int s2, int d1, int d2, md_addr_t addr, int vc, counter_t msgno);
#endif


extern int multi_blk_set_shift;

counter_t pending_invalidation[MAXTHREADS];
counter_t pending_invalid_start_cycles[MAXTHREADS];
counter_t pending_invalid_cycles;


int data_packet_size;
int meta_packet_size;
extern counter_t prefetch_num;
extern counter_t veri_replace;
#ifdef ESYNETn
static int data_packet_size = 5;
static int meta_packet_size = 1;
#endif

#define DCACHE_MSHR

#define WAIT_TIME   800000000
#define RETRY_TIME  1   
#ifdef  DCACHE_MSHR
#define TLB_MSHR_SIZE   32
#define MSHR_SIZE   32
#define L2_MSHR_SIZE    48
#define MSHR_OC_SIZE    15

#ifdef CONF_RES_RESEND
#define QUEUE_SIZE  100
#endif

#define LSQSIZE 1024        /* RUU max size */
#define RUUSIZE 1024        /* LSQ max size */



struct mshr_entry_t
{
    int id;
    int isValid;
    md_addr_t addr;
    counter_t startCycle;
    counter_t endCycle;
    int blocknum;
    int isOC;
    struct RUU_station *rs;
    struct DIRECTORY_EVENT *event;
};

struct mshr_t
{
    int threadid;
    struct mshr_entry_t mshrEntry[L2_MSHR_SIZE];
    int freeEntries;
    int freeOCEntries;
    int mshrSIZE;
};

md_addr_t wb_buffer[MAXTHREADS][MSHR_SIZE];
void initMSHR(struct mshr_t *mshr, int flag);
void freeMSHR(struct mshr_t *mshr);
void tlb_flush_check(int threadid);
int isMSHRFull(struct mshr_t *mshr, int isOC, int threadid);
void MSHRLookup(struct mshr_t *mshr, md_addr_t addr, int lat, int isOC, struct RUU_station *rs);
//void mshrentry_update(struct DIRECTORY_EVENT *event, tick_t ready, tick_t startcycle);
#endif

/* MEM model */
#define MEM_MODEL        //Memory model
#define STREAM_PREFETCHER   //L2 prefetch
#define L1_STREAM_PREFETCHER
#define L1_DISTANCE


#define READ_EXCLUSION       // @ fanglei

#ifdef  STREAM_PREFETCHER
#define L1_MISS_TABLE_SIZE 32
#define L1_STREAM_ENTRIES  16
#define L2_MISS_TABLE_SIZE 16
#define L2_STREAM_ENTRIES  8
#define PREFETCH_REPLACE_HISTORY  1
#define REPLACE_HISTORY  8
#define COHERENCE_HISTORY  8
//#define L2PREFETCH_DISTANCE   16
struct  L1_miss_history_table_t
{
    md_addr_t history[L1_MISS_TABLE_SIZE];
    int mht_tail;
    int mht_num;
};

struct  L2_miss_history_table_t
{
    md_addr_t history[L2_MISS_TABLE_SIZE];
    int mht_tail;
    int mht_num;
};

struct  stream_table_entry_t
{
    int valid;
    int order;
    md_addr_t addr;
    int stride;
    int remaining_prefetches;
    counter_t last_use;
};

struct L2_miss_history_table_t miss_history_table;
struct stream_table_entry_t stream_table[L2_STREAM_ENTRIES];

void init_sp();
void insert_sp(md_addr_t addr, int threadid);
void launch_sp(int id, int threadid);
#endif
#ifdef L1_STREAM_PREFETCHER
struct L1_miss_history_table_t l1miss_history_table[MAXTHREADS];
struct stream_table_entry_t l1stream_table[MAXTHREADS][L1_STREAM_ENTRIES];

void l1init_sp();
void l1insert_sp(md_addr_t addr, int threadid);
void l1launch_sp(md_addr_t addr, int threadid, int trigger);
#endif

#ifdef CROSSBAR_INTERCONNECT
#define NUMBEROFBUSES 8
//int crossbar_Lat(struct cache_t *cp, int id, int accessType, int lat, int retProcId, md_addr_t addr);
#endif

#ifdef BUS_INTERCONNECT
#define NUMBEROFBUSES 1
#endif
#define BUSSLOTS 2048
struct bs_nd
{
    long long int startTime, endTime;
    int accessType, threadid;
    struct bs_nd *next;
};

/*
 * This module contains code to implement various cache-like structures.  The
 * user instantiates caches using cache_new().  When instantiated, the user
 * may specify the geometry of the cache (i.e., number of set, line size,
 * associativity), and supply a block access function.  The block access
 * function indicates the latency to access lines when the cache misses,
 * accounting for any component of miss latency, e.g., bus acquire latency,
 * bus transfer latency, memory access latency, etc...  In addition, the user
 * may allocate the cache with or without lines allocated in the cache.
 * Caches without tags are useful when implementing structures that map data
 * other than the address space, e.g., TLBs which map the virtual address
 * space to physical page address, or BTBs which map text addresses to
 * branch prediction state.  Tags are always allocated.  User data may also be
 * optionally attached to cache lines, this space is useful to storing
 * auxilliary or additional cache line information, such as predecode data,
 * physical page address information, etc...
 *
 * The caches implemented by this module provide efficient storage management
 * and fast access for all cache geometries.  When sets become highly
 * associative, a hash table (indexed by address) is allocated for each set
 * in the cache.
 *
 * This module also tracks latency of accessing the data cache, each cache has
 * a hit latency defined when instantiated, miss latency is returned by the
 * cache's block access function, the caches may service any number of hits
 * under any number of misses, the calling simulator should limit the number
 * of outstanding misses or the number of hits under misses as per the
 * limitations of the particular microarchitecture being simulated.
 *
 * Due to the organization of this cache implementation, the latency of a
 * request cannot be affected by a later request to this module.  As a result,
 * reordering of requests in the memory hierarchy is not possible.
 */

/* highly associative caches are implemented using a hash table lookup to
   speed block access, this macro decides if a cache is "highly associative" */
#define CACHE_HIGHLY_ASSOC(cp)  ((cp)->assoc > 4)

/* cache access macros */
#define CACHE_TAG(cp, addr) ((addr) >> (cp)->tag_shift)
#define CACHE_SET(cp, addr) (((addr) >> (cp)->set_shift) & (cp)->set_mask)

#define CACHE_QBLK_TAG(cp, addr) ((addr) >> ((cp)->tag_shift+multi_blk_set_shift))
#define CACHE_QBLK_SET(cp, addr) (((addr) >> ((cp)->set_shift)+multi_blk_set_shift) & (cp)->set_mask)

#define CACHE_BLK(cp, addr) ((addr) & (cp)->blk_mask)
#define CACHE_TAGSET(cp, addr)  ((addr) & (cp)->tagset_mask)

/* cache replacement policy */
enum cache_policy {
    LRU,        /* replace least recently used block (perfect LRU) */
    Random, /* replace a random block */
    FIFO        /* replace the oldest block in the set */
};

#define STREAM_MIN           1

/* block status values */
#define CACHE_BLK_VALID     0x00000001  /* block in valid, in use */
#define CACHE_BLK_DIRTY     0x00000002  /* dirty block */
#define CACHE_BLK_WVALID    0x00000004  /* block in valid, in use */
#define MESI_INVALID        1
#define MESI_SHARED         2
#define MESI_MODIFIED       3
#define MESI_EXCLUSIVE      4

#define MESI_NONSHARED      5

#define WRITE_HIT_SHARED    1

// packet type
#define LOCAL               1
//#define 
//#define 
//#define 
//#define 
//#define 

// prefetch L2 state @ fanglei
#define NON_SHARED          1
#define SHARED              2
#define EXCLUSIVE_O_DIRTY   3
#define MISS                4
#define MISS_REPLACE        5

// L1 prefetch_num match @ fanglei
#define READ_MATCH          1
#define WRITE_MATCH         2
#define L1_REPLACED         3
#define L2_OVERLAP          4
#define L2_REPLACED         5
#define DROPPED             6

#define L2_DROP             0

#define PREFETCH_NUM        20000000

#define TLB_CU_SIZE         32

#define PRIVATE_READ        1
#define PRIVATE_WRITE       2
#define SHARED_READ         3
#define SHARED_WRITE        4

#define PAGE_TABLE_SEARCH   300
#define FLUSH_OVERHEAD      100

#define QBLK_SET_SHIFT      2
/* Message types */
#define IDEAL_CASE              1

#define MISS_IL1                2
#define MISS_READ               3
#define MISS_WRITE              4
#define WRITE_UPDATE            5   
#define WRITE_BACK              6
#define WRITE_BACK_HEAD         7
#define WRITE_BACK_REPLY        8
#define UPDATE_DIR              9
#define INV_MSG_READ            10
#define INV_MSG_WRITE           11
#define ACK_MSG_READ            12          
#define ACK_MSG_READUPDATE      13          
#define ACK_MSG_WRITE           14
#define ACK_MSG_WRITEUPDATE     15
#define ACK_DIR_IL1             16
#define ACK_DIR_READ_SHARED     17
#define ACK_DIR_READ_EXCLUSIVE  18
#define ACK_DIR_WRITE           19
#define ACK_DIR_WRITE_EARLY     20
#define ACK_DIR_WRITEUPDATE     21
#define WAIT_MEM_READ           22
#define WAIT_MEM_READ_N         23
#define L2_PREFETCH             24
#define NACK                    25
#define FAIL                    26
#define FINAL_INV_ACK           27
#define MEM_READ_REQ            28
#define MEM_READ_REPLY          29
#define WAIT_MEM_READ_N_WAIT    30
#define MEM_WRITE_BACK          31
#define MEM_PACK_HEAD           32
#define MEM_PACK_REPLY          33
#define FINAL_READ_ACK          34
#define ACK_MSG_UPDATE          35
#define INV_MSG_UPDATE          36

#define SHR_TLB_MISS            50
#define SHR_TLB_WRITE_BACK      51
#define SHR_TLB_SR_RECOVERY     52
#define SHR_TLB_UPDATE_PR_ACK   53
#define SHR_TLB_UPDATE_PW_ACK   54
#define SHR_TLB_RECOVERY_ACK    55
#define DIR_TLB_REFILL          56
#define DIR_TLB_UPDATE          57
#define DIR_TLB_RECOVERY        58
#define DIR_TLB_REFILL_WAIT     59
#define DIR_TLB_UPDATE_WAIT     60
#define DIR_TLB_RECOVERY_WAIT   61

#define DIR_SHR_READ            62
#define ACK_SHR_READ            63
#define INFORM_BACK             64

#define VEC_DEGRADE             65

/* Dir information */
#define DIR_STABLE      1
#define DIR_TRANSITION  2
#define INV_TYPE        3
#define REPL_TYPE       4
/* Network packet size */
#define REQ_PACKET      1
#define DATA_PACKET     4

/* Why we allocated the cache line */
#define LINE_PREFETCH       1
#define LINE_READ       2
#define LINE_WRITE      3

/* What operations we performed on the cache line */
#define LINE_READ_OP        1
#define LINE_WRITE_OP       2


#define MY_NORMAL       0
#define MY_LDL          1
#define MY_LDL_L        2
#define MY_STL_C        3
#define MY_STL          4


#ifdef SMT_SS
struct tag_id_t
{
    int threadid;
    md_addr_t tag;      /* data block tag value */
};
#endif

#ifdef EVENT_CACHE_UPDATE
struct mshrq {  /* This queue keeps track of when mshr used during
                   cache miss,so that cache port contention can be modeled */
    long when;
    int rd_wt;
    md_addr_t addr;
    struct mshrq *next;
    struct mshrq *prev;
};

//int insert_mshr(struct cache_t *,int,long,md_addr_t,int);
//int insert_mshr(struct cache_t *cp,int rd_wt, long when, md_addr_t addr, int pipedelay);
#endif

struct prefetch_replace_history_t
{
    md_addr_t prefetch_replace_addr;
    counter_t esynetMsgNo;
};

struct line_info
{
    int line_owner;
    int line_state;
};

struct page_entry
{
    int valid;
    int busy;
    md_addr_t VPN;  // virtual page number
    md_addr_t PPN;  // physical page number
    int keeper;
    int cached;
    int state;
    struct line_info *line_head;
};

struct page_entry *page_table[PAGE_NUM];

struct tlb_rs
{
    struct RUU_station *rs;
    struct tlb_rs *next;
};

struct tlb_mshr_entry
{
    int valid;
    int dirty;
    int evicting;
    md_addr_t VPN;
    struct tlb_rs *head;
};

struct tlb_mshr_entry *tlb_mshr[MAXTHREADS][TLB_MSHR_SIZE];

struct pw_evict_entry
{
    int valid;
    md_addr_t VPN;
};

struct sr_recovery_entry
{
    int valid;
    md_addr_t VPN;
};

struct dir_recovery_entry
{
    int valid;
    struct DIRECTORY_EVENT *event;
};

struct tlb_cu_blk
{
    int recover_delay;
    struct pw_evict_entry pw_evict[TLB_CU_SIZE];
    struct sr_recovery_entry sr_recovery[TLB_CU_SIZE];
    struct dir_recovery_entry dir_recovery[TLB_CU_SIZE];
};

struct tlb_cu_blk *tlb_cu[MAXTHREADS];

/* cache block (or line) definition */
struct cache_blk_t
{
    struct cache_blk_t *way_next;   /* next block in the ordered way chain, used
                                       to order blocks for replacement */
    struct cache_blk_t *way_prev;   /* previous block in the order way chain */
    struct cache_blk_t *hash_next;/* next block in the hash bucket chain, only
                                     used in highly-associative caches */
    /* since hash table lists are typically small, there is no previous
       pointer, deletion requires a trip through the hash table bucket list */
#ifdef SMT_SS
    struct tag_id_t   tagid;        /* data block tag value and threadid value */
#else
    md_addr_t tag;      /* data block tag value */
#endif
    unsigned int status;        /* block status, see CACHE_BLK_* defs above */
    // tlb define @ fanglei
    int tlb_state;
    int tlb_lock;

    // PVC @ fanglei
    int ptr_en;
    int all_shared_en;

    // q_blk @ fanglei
    md_addr_t qblk_set_shift;

    tick_t ready;       /* time when block will be accessible, field
                           is set when a miss fetch is initiated */

    int state;
    int isStale;
    md_addr_t addr;

    int lastThrd;
    unsigned int value;
    int lineVolatile;
    int wordVolatile, wordInvalid;  
    int epochModified;
    int isL1prefetch;
    unsigned int dir_state[8];
    unsigned long long int dir_sharer[8][8];
    counter_t exclusive_time[8];
    int dir_dirty[8];   
    int dir_blk_state[8];   
    int ever_wb;
    //unsigned int dir_sharer;
    //int dir_dirty;    
#ifdef STREAM_PREFETCHER
    int spTag;
#endif
    int ever_dirty;
    int ever_touch;
    int ReadCount;
    int WriteCount;
    int WordCount;
    int Type;
    int WordUseFlag[8];
    struct DIRECTORY_EVENT *ptr_cur_event[8];


    byte_t *user_data;      /* pointer to user defined data, e.g.,
                               pre-decode data or physical page address */
    /* DATA should be pointer-aligned due to preceeding field */
    /* NOTE: this is a variable-size tail array, this must be the LAST field
       defined in this structure! */
    byte_t data[1];     /* actual data block starts here, block size
                           should probably be a multiple of 8 */ 

    long long int lastModified, lastAccessed;
    int modifiedThisEpoch;
    int modifiedByMe;
    int invCause;   /* 0 - not a coherence inv, 1 - coherence inv by lead thread, 2 - coherence inv by trailing thread */
};

/* cache set definition (one or more blocks sharing the same set index) */
struct cache_set_t
{
    struct cache_blk_t **hash;  /* hash table: for fast access w/assoc, NULL
                                   for low-assoc caches */
    struct cache_blk_t *way_head;   /* head of way list */
    struct cache_blk_t *way_tail;   /* tail pf way list */
    struct cache_blk_t *blks;   /* cache blocks, allocated sequentially, so
                                   this pointer can also be used for random
                                   access to cache blocks */
    counter_t PVC_vector_distr_record_num;
    counter_t PVC_vector_inc_distr_record_num;
    counter_t PVC_vector_distr[MAX_DIRECTORY_ASSOC+1];
    counter_t PVC_vector_inc_distr[MAX_DIRECTORY_ASSOC+1];    
};

/* cache definition */
struct cache_t
{
    /* parameters */
    char *name;         /* cache name */
    int nsets;          /* number of sets */
    int bsize;          /* block size in bytes */
    int balloc;         /* maintain cache contents? */
    int usize;          /* user allocated data size */
    int assoc;          /* cache associativity */
    enum cache_policy policy;   /* cache replacement policy */
    unsigned int hit_latency;   /* cache hit latency */

    /* miss/replacement handler, read/write BSIZE bytes starting at BADDR
       from/into cache block BLK, returns the latency of the operation
       if initiated at NOW, returned latencies indicate how long it takes
       for the cache access to continue (e.g., fill a write buffer), the
       miss/repl functions are required to track how this operation will
       effect the latency of later operations (e.g., write buffer fills),
       if !BALLOC, then just return the latency; BLK_ACCESS_FN is also
       responsible for generating any user data and incorporating the latency
       of that operation */
    unsigned int                    /* latency of block access */
        (*blk_access_fn)(enum mem_cmd cmd,      /* block access command */
                         md_addr_t baddr,       /* program address to access */
                         int bsize,         /* size of the cache block */
                         struct cache_blk_t *blk,   /* ptr to cache block struct */
#ifdef SMT_SS
                         tick_t now,
                         int threadid
#if defined( L2_COMM_LAT) || defined (TOKENB)
                         , int cluster      /* the cluster who uses this cache */
#endif
                        );
#else
    tick_t now);        /* when fetch was initiated */
#endif

    /* derived data, for fast decoding */
    int hsize;          /* cache set hash table size */
    md_addr_t blk_mask;
    int set_shift;
    md_addr_t set_mask;     /* use *after* shift */
    int tag_shift;
    md_addr_t tag_mask;     /* use *after* shift */
    md_addr_t tagset_mask;  /* used for fast hit detection */

    /* bus resource */
    tick_t bus_free;        /* time when bus to next level of cache is
                               free, NOTE: the bus model assumes only a
                               single, fully-pipelined port to the next
                               level of memory that requires the bus only
                               one cycle for cache line transfer (the
                               latency of the access to the lower level
                               may be more than one cycle, as specified
                               by the miss handler */

    /* per-cache stats */
    counter_t hits;     /* total number of hits */
    counter_t dhits;        /* total number of data hits */
    counter_t misses;       /* total number of misses */
    counter_t dmisses;      /* total number of data misses */
    counter_t coherence_misses;     /* total number of misses */
    counter_t capacitance_misses;       /* total number of misses */
    counter_t replacements; /* total number of replacements at misses */
    counter_t replInv;      /* total number of replacements at misses that also involve in sending invalidations */
    counter_t writebacks;       /* total number of writebacks at misses */
    counter_t wb_coherence_w;       /* total number of writebacks at misses */
    counter_t wb_coherence_r;       /* total number of writebacks at misses */
    counter_t invalidations;    /* total number of external invalidations */
    counter_t coherencyMisses;  /* total number of misses due to coherency */
    counter_t coherencyMissesOC;    /* coherency misses due to optimistic core */

    /* Data L1 cache */
    counter_t in_mshr;
    counter_t din_mshr;
    counter_t dir_notification;
    counter_t Invalid_Read_received;
    counter_t Invalid_write_received;
    counter_t Invalid_Read_received_hits;
    counter_t Invalid_write_received_hits;
    counter_t e_to_i;
    counter_t e_to_m;
    counter_t s_to_i;
    /* L2 Cache */
    counter_t ack_received;
    counter_t L2_access;
    counter_t dir_access;
    counter_t data_access;
    counter_t localdirectory;
    counter_t remotedirectory;
    counter_t sendInvalid, invalidDone, wasInvalidated;

    counter_t flushCount, lineFlushCount;

    /* Additional stats for detailed bus modeling - Ronz */
    int rdb;
    int wrb;
    int pipedelay;
    int wrbufsize;
    long lastuse;
    /*Ali: model msrh in details */
#ifdef EVENT_CACHE_UPDATE
    struct mshrq *mshr_queue;
#endif

    /* last block to hit, used to optimize cache hit processing */
#ifdef SMT_SS
    struct tag_id_t last_tagsetid;  /* tag and threadid of last line accessed */
#else
    md_addr_t last_tagset;  /* tag of last line accessed */
#endif
    struct cache_blk_t *last_blk;   /* cache block last accessed */

#ifdef  DCACHE_MSHR
    struct mshr_t *mshr;
#endif

    /* data blocks */
    byte_t *data;           /* pointer to data blocks allocation */

    counter_t numDiffThrds;


    /* Additional stats for detailed bus modeling - Ronz */
    int lastInvalidate;    
    long Invld;

    /* Counters for the Global Cache */
    long long int intraThrdConflict;
    long long int interThrdConflict;
    long long int ConsecutiveAccess;

    /* Counters for the Coherent Cache */
    long long int ccLocalLoadS;
    long long int ccLocalLoadX;
    long long int ccLocalLoadM;
    long long int ccLocalStoreS;
    long long int ccLocalStoreX;
    long long int ccLocalStoreM;
    long long int ccRemoteLoadS;
    long long int ccRemoteLoadX;
    long long int ccRemoteLoadM;
    long long int ccRemoteStoreS;
    long long int ccRemoteStoreX;
    long long int ccRemoteStoreM;

    /* Counters for the Selectively-Replicated Cache */
    long long int SRLocalLoadS;
    long long int SRLocalLoadX;
    long long int SRLocalLoadM;
    long long int SRLocalStoreS;
    long long int SRLocalStoreX;
    long long int SRLocalStoreM;
    long long int SRRemoteLoadS;
    long long int SRRemoteLoadX;
    long long int SRRemoteLoadM;
    long long int SRRemoteStoreS;
    long long int SRRemoteStoreX;
    long long int SRRemoteStoreM;

    long long int glLocalHit;
    long long int glRemoteHit;

    long long int NumAccesses;

    int threadid;

    /* NOTE: this is a variable-size tail array, this must be the LAST field
       defined in this structure! */
    struct cache_set_t sets[1]; /* each entry is a set */
};

/* create and initialize a general cache structure */
struct cache_t *            /* pointer to cache created */
cache_create(char *name,        /* name of the cache */
        int nsets,          /* total number of sets in cache */
        int bsize,          /* block (line) size of cache */
        int balloc,     /* allocate data space for blocks? */
        int usize,          /* size of user data to alloc w/blks */
        int assoc,          /* associativity of cache */
        enum cache_policy policy,   /* replacement policy w/in sets */
        /* block access function, see description w/in struct cache def */
        unsigned int (*blk_access_fn)(enum mem_cmd cmd,
            md_addr_t baddr, int bsize,
            struct cache_blk_t *blk,
#ifdef SMT_SS
            tick_t now,

            int threadid
#if defined( L2_COMM_LAT) || defined (TOKENB)
            , int cluster       /* the cluster who uses this cache */
#endif
            ),
#else
        tick_t now),
#endif
    unsigned int hit_latency,/* latency in cycles for a hit */
    int threadid);

/* parse policy */
enum cache_policy           /* replacement policy enum */
cache_char2policy(char c);      /* replacement policy as a char */

/* print cache configuration */
void
cache_config(struct cache_t *cp,    /* cache instance */
        FILE *stream);      /* output stream */

/* register cache stats */
void cache_reg_stats(struct cache_t *cp, struct stat_sdb_t *sdb, int id);

//void cache_reg_stats(struct cache_t *cp,struct stat_sdb_t *sdb);

/* print cache stats */
void
cache_stats(struct cache_t *cp,     /* cache instance */
        FILE *stream);      /* output stream */

// /* print cache stats */
// void cache_stats(struct cache_t *cp, FILE *stream);

/* access a cache, perform a CMD operation on cache CP at address ADDR,
   places NBYTES of data at *P, returns latency of operation if initiated
   at NOW, places pointer to block user data in *UDATA, *P is untouched if
   cache blocks are not allocated (!CP->BALLOC), UDATA should be NULL if no
   user data is attached to blocks */
#ifdef SMT_SS
//unsigned int              /* latency of access in cycles */
//cache_access(struct cache_t *cp,  /* cache to access */
//       enum mem_cmd cmd,      /* access type, Read or Write */
///      md_addr_t addr,        /* address of access */
//       void *vp,          /* ptr to buffer for input/output */
///      int nbytes,        /* number of bytes to access */
//       tick_t now,        /* time of access */
//       byte_t **udata,        /* for return of user data ptr */
//       md_addr_t *repl_addr,  /* for address of replaced block */
//           int threadid);
#else
unsigned int                /* latency of access in cycles */
cache_access(struct cache_t *cp,    /* cache to access */
        enum mem_cmd cmd,       /* access type, Read or Write */
        md_addr_t addr,     /* address of access */
        void *vp,           /* ptr to buffer for input/output */
        int nbytes,     /* number of bytes to access */
        tick_t now,     /* time of access */
        byte_t **udata,     /* for return of user data ptr */
        md_addr_t *repl_addr
        struct RUU_station *rs, 
        int threadid,
        m_L1WBufEntry *l1_wb_entry //8 equals thread id..
        );  /* for address of replaced block */
#endif

void cache_warmup (struct cache_t *cp,  /* cache to access */
        enum mem_cmd cmd,       /* access type, Read or Write */
        md_addr_t addr,     /* address of access */
        int nbytes,     /* number of bytes to access */
        int threadid);
//int isLineInvalidated(md_addr_t addr, int threadid);

/* cache access functions, these are safe, they check alignment and
   permissions */
#if defined( L2_COMM_LAT) || defined (TOKENB)
#define cache_double(cp, cmd, addr, p, now, udata, cl)  \
    cache_access(cp, cmd, addr, p, sizeof(double), now, udata, cl)
#define cache_float(cp, cmd, addr, p, now, udata, cl)   \
    cache_access(cp, cmd, addr, p, sizeof(float), now, udata, cl)
#define cache_dword(cp, cmd, addr, p, now, udata, cl)   \
    cache_access(cp, cmd, addr, p, sizeof(long long), now, udata, cl)
#define cache_word(cp, cmd, addr, p, now, udata, cl)    \
    cache_access(cp, cmd, addr, p, sizeof(int), now, udata, cl)
#define cache_half(cp, cmd, addr, p, now, udata, cl)    \
    cache_access(cp, cmd, addr, p, sizeof(short), now, udata, cl)
#define cache_byte(cp, cmd, addr, p, now, udata, cl)    \
    cache_access(cp, cmd, addr, p, sizeof(char), now, udata, cl)
#else //L2_COMM_LAT
#define cache_double(cp, cmd, addr, p, now, udata)  \
    cache_access(cp, cmd, addr, p, sizeof(double), now, udata)
#define cache_float(cp, cmd, addr, p, now, udata)   \
    cache_access(cp, cmd, addr, p, sizeof(float), now, udata)
#define cache_dword(cp, cmd, addr, p, now, udata)   \
    cache_access(cp, cmd, addr, p, sizeof(long long), now, udata)
#define cache_word(cp, cmd, addr, p, now, udata)    \
    cache_access(cp, cmd, addr, p, sizeof(int), now, udata)
#define cache_half(cp, cmd, addr, p, now, udata)    \
    cache_access(cp, cmd, addr, p, sizeof(short), now, udata)
#define cache_byte(cp, cmd, addr, p, now, udata)    \
    cache_access(cp, cmd, addr, p, sizeof(char), now, udata)
#endif //L2_COMM_LAT



void markCacheNewEpoch(struct cache_t *cp, int);

void busFree();
void busInit();
void markCacheLine(struct cache_t *, md_addr_t, int);
unsigned int                /* latency of flush operation */
cache_mark_volatile(struct cache_t *cp, /* cache instance to flush */
        md_addr_t addr, /* address of block to flush */
        tick_t now,     /* time of cache flush */
        int threadid, int);

unsigned int                /* latency of flush operation */
cache_flush_volatile(struct cache_t *cp,    /* cache instance to flush */
        tick_t now,     /* time of cache flush */
        int threadid, int );


int check_for_l1_miss(struct cache_t *cp, md_addr_t addr, int threadid);

void updateBlockLat(struct cache_t *cp, md_addr_t addr, int threadid, int lat);

int checkReplacementStatus(int id, md_addr_t addr);

/* return non-zero if block containing address ADDR is contained in cache
   CP, this interface is used primarily for debugging and asserting cache
   invariants */
int                 /* non-zero if access would hit */
cache_probe(struct cache_t *cp,     /* cache instance to probe */
        md_addr_t addr);        /* address of block to probe */

/* flush the entire cache, returns latency of the operation */
unsigned int                /* latency of the flush operation */
cache_flush(struct cache_t *cp,     /* cache instance to flush */
        tick_t now);        /* time of cache flush */

/* flush the block containing ADDR from the cache CP, returns the latency of
   the block flush operation */
#ifdef SMT_SS
unsigned int                /* latency of flush operation */
cache_flush_addr(struct cache_t *cp,    /* cache instance to flush */
        md_addr_t addr, /* address of block to flush */
        tick_t now,     /* time of cache flush */
        int threadid);
#else
unsigned int                /* latency of flush operation */
cache_flush_addr(struct cache_t *cp,    /* cache instance to flush */
        md_addr_t addr, /* address of block to flush */
        tick_t now);        /* time of cache flush */

#ifdef EVENT_CACHE_UPDATE
int insert_mshr(struct cache_t *,int,long,md_addr_t,int);
//int insert_mshr(struct cache_t *cp,int rd_wt, long when, md_addr_t addr, int pipedelay);
#endif
#endif
void cacheBcopy(enum mem_cmd cmd, struct cache_blk_t *blk, int position, byte_t *data, int nbytes);
//int L2inclusionFunc(struct cache_t *cp, md_addr_t addr);
int isCacheHit (struct cache_t *cp, md_addr_t addr, int threadid);

#ifdef  EDA
int cacheBlockSearch(struct cache_t * cp, enum mem_cmd cmd, md_addr_t addr, byte_t *data, int nbytes);
void flushSelective(struct cache_t *cp, int id);
#endif

struct DIRECTORY_EVENT
{
    enum md_opcode op;
    int isPrefetch;         // if this is a prefetch
    int stride;
    int order;
    int stream_entry_index;
    int trigger;
    int page_entry_state;
    int l2Status;           // 0 - not yet accessed, 1 - l2 miss, 2 - l2 miss completed
    counter_t   l2MissStart;
    struct DIRECTORY_EVENT *next;           // pointer to next entry
    tick_t when;                            // when directory be accessed
    tick_t started;
    tick_t mshr_time;
    tick_t transfer_time;
    tick_t req_time;
    int store_cond;
    int flip_flag;

    // DD related @ fanglei
    int tlb_state;              // the state of tlb  @ fanglei
    int shr_data_req;
    unsigned long long int data_req_sharer;
    int L1_hit;
    int no_L2_delay;
    
    // q_blk @ fanglei
    md_addr_t qblk_set_shift;
    int ml2_dir_blk_state;

    /* conflict reason */
    int input_buffer_full;
    int pending_flag;
    int L2miss_flag;
    int L2miss_stated;
    int L2miss_complete;
    int dirty_flag;
    counter_t arrival_time;
    int delay;
    int data_reply;
    int conf_bit;
    counter_t missNo;
    counter_t totaleventcount;
    int resend;
    unsigned int operation;                         // directory operation, writehit update, writemiss search, readmiss search be different
    int tempID;                             // which processor issue the directory operation
    md_addr_t addr;                         // address to access
    struct cache_blk_t *blk1, *blk_dir;     // point to dl1 block accessed, ? I think dl2 -hq  

    struct RS_link *ptr_event;              // point to the false entry in event queue
    struct cache_t *cp;                     // cache to access
    void *vp;                               // ptr to buffer for input/output
    int nbytes;                             // number of bytes to access
    byte_t **udata;                         // for return of user data ptr
    enum mem_cmd cmd;           // access type, Read or Write 
    struct RUU_station *rs;       
    struct m_L1WBufEntry *l1_wb_entry;
    long src1, src2, des1, des2, startCycle;
    int new_src1, new_src2, new_des1, new_des2;
    struct DIRECTORY_EVENT *parent;       // pointer to next entry
    int parent_operation;       // Parent operation is used for L2 cache MISS
    int childCount, processingDone, spec_mode, originalChildCount;
    int pref_mode;
    counter_t esynetMsgNo;
    int eventType;
    int rec_busy;
    int sharer_num;
    int individual_childCount[8];
    int prefetch_next;
    int isReqL2Hit;
    int isReqL2SecMiss;
    int isReqL2Inv;
    int isReqL2Trans;
    counter_t reqAtDirTime;
    counter_t reqAckTime;
    int isSyncAccess;
};

#define EUP_NETWORKn
#ifdef EUP_NETWORKn
struct DIRECTORY_EVENT *free_event_list;
#define maxEvent 30000
void allocate_free_event_list();
#endif
#ifdef CONF_RES_RESEND
struct QUEUE_EVENT_ENTRY
{
    struct DIRECTORY_EVENT *event;
    int isvalid;
};

struct QUEUE_EVENT
{
    struct QUEUE_EVENT_ENTRY Queue_entry[QUEUE_SIZE];
    int free_Entries;
};
#endif

/* Event FIFO implementation */


#define DIR_FIFO_PORTS  2
#define DIR_FIFO_SIZE   4096
#define dir_fifo_size   100
#define DIR_FIFO_FULL
#define MAXSIZE     250

#define MEM_LOC_SHIFT  1

int dir_fifo_full[MAXSIZE];

struct DIRECTORY_EVENT *dir_fifo[MAXSIZE][DIR_FIFO_SIZE];
int dir_fifo_head[MAXSIZE];
int dir_fifo_tail[MAXSIZE];
int dir_fifo_num[MAXSIZE];
int dir_fifo_portuse[MAXSIZE];
#define TSHR_FIFO_SIZE 1024
#define tshr_fifo_size 100
struct DIRECTORY_EVENT *tshr_fifo[MAXSIZE][TSHR_FIFO_SIZE];
int tshr_fifo_head[MAXSIZE];
int tshr_fifo_tail[MAXSIZE];
int tshr_fifo_num[MAXSIZE];
int tshr_fifo_portuse[MAXSIZE];


struct DIRECTORY_EVENT *l1_fifo[MAXSIZE][DIR_FIFO_SIZE];
int l1_fifo_head[MAXSIZE];
int l1_fifo_tail[MAXSIZE];
int l1_fifo_num[MAXSIZE];
int l1_fifo_portuse[MAXSIZE];

void reset_ports();
int dir_fifo_enqueue(struct DIRECTORY_EVENT *event, int type);
void dir_fifo_dequeue();
void sendLSQInv(md_addr_t addr, int threadid);

#endif /* CACHE_H */


#ifndef CONTEXT_H

#define CONTEXT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "endian.h"
#include "regs.h"
#include "memory.h"
#include "sim.h"
#include "eio.h"
#include "loader.h"
#include "bpred.h"
#include "bitmap.h"

#include "power.h"
#include "smt.h"
#include "branchCorr.h"




/**********************************************************************/

#define	COLLECT_STAT_STARTUP		/* This flag is used to perform fast-forwarding of parallel applications. Comment the flag for spec */
//#define PARALLEL_EMUL			/* Execute parallel applications without timing simulations. */

//#define	EDA			/* Explicitly-decoupled architecture for parallel application.*/
//#define       DECOUPLED_PREFETCH    	/* Decoupled prefetching */
//#define	RUN_ORACLE

#define STATUS_NUM	100
#define	THREADS_PER_JOB	 MAXTHREADS
#ifdef	COLLECT_STAT_STARTUP
int thrdPerJobCnt;
#endif

#ifdef	EDA
struct	psn_struct
{
    int dirty;
    qword_t data;
};

#define TYPE_INST_REMOVED       0
#define TYPE_INST_INCLUDED      1
#define TYPE_BR_NOTTAKEN        2
#define TYPE_BR_TAKEN           3
#define TYPE_LOCK_ACQUIRE       4
#define TYPE_INST_PREFETCH      5
#define	TYPE_INST_BARRIER	6
#define TYPE_INST_BARRIER_END 	7

/* Filter cache (dl0) cleanup at regular intervals */
#define	DL0_CLEAUP_INTERVAL	4096


/* Oracle Store Value Queue */
struct oracle_store_queue
{
    md_addr_t PC;
    int memWidth;
    md_addr_t memAddr;
    qword_t memData;
    counter_t sim_num_insn;
    counter_t sim_cycle;
    int isExecuted;
};

#define OSQ_size 5000

#ifdef DECOUPLED_PREFETCH
#define PREFETCH_QUEUE_SIZE     128
#define PREFETCH_DELAY_GUARD    20      // Initiate prefetch early by guard length
#endif
#endif
/**********************************************************************/

/* whether to modify prog_out file name using sim_out */
#define PROG_OUT_FROM_SIM_OUT

#ifdef SMT_SS
#define STACKSIZE 0x90000
extern int MTAcontexts;
struct lreg_id_t
{
  int threadid;
  int lreg;
};

/* moved from sim-outorder.c */
#define OWINDOWSIZE 360
#define HOPTIME 1
#define COMM_LATENCY ((CLUSTERS*HOPTIME)/2)
				/* The furthest two points on
				the chip. When a ring
				interconnect is used, each
				hop is a cycle. */
/* inst tag type, used to tag an operation instance in the RUU */
typedef unsigned int INST_TAG_TYPE;

/* inst sequence type, used to order instructions in the ready list, if
   this rolls over the ready list order temporarily will get messed up,
   but execution will continue and complete correctly */
typedef unsigned int INST_SEQ_TYPE;

/* total input dependencies possible */
#define MAX_IDEPS               3

/* total output dependencies possible */
#define MAX_ODEPS               2

#define	StallRUUFull			1
#define StallLQFull				2
#define StallSQFull				3
#define StallRegFull			4
#define StallIssueFull			5
#define StallFetchEmpty			6
#define StallSTLC				8
#define StallTrap				10

/* a register update unit (RUU) station, this record is contained in the
   processors RUU, which serves as a collection of ordered reservations
   stations.  The reservation stations capture register results and await
   the time when all operands are ready, at which time the instruction is
   issued to the functional units; the RUU is an order circular queue, in which
   instructions are inserted in fetch (program) order, results are stored in
   the RUU buffers, and later when an RUU entry is the oldest entry in the
   machines, it and its instruction's value is retired to the architectural
   register file in program order, NOTE: the RUU and LSQ share the same
   structure, this is useful because loads and stores are split into two
   operations: an effective address add and a load/store, the add is inserted
   into the RUU and the load/store inserted into the LSQ, allowing the add
   to wake up the load/store when effective address computation has finished */
struct RUU_station 
{
    /* inst info */
    int index;                 		/* used in LSQ for index the entry in the queue */
    int threadid;           		/* this RUU instant belongs to which theard  */
    md_inst_t IR;			/* instruction bits */
    enum md_opcode op;			/* decoded instruction opcode */
    md_addr_t PC, next_PC, pred_PC;	/* inst PC, next PC, predicted PC */
    int in_LSQ;				/* non-zero if op is in LSQ */
    int ea_comp;				/* non-zero if op is an addr comp */
    int recover_inst;			/* start of mis-speculation? */
	int stack_recover_idx;		/* non-speculative TOS for RSB pred */
    struct bpred_update_t dir_update;	/* bpred direction update info */
    int spec_mode;	/* non-zero if issued in spec_mode */
	md_addr_t addr;			/* effective address for ld/st's */
	int miss_flag;
    INST_TAG_TYPE tag;			/* RUU slot tag, increment to
					   squash operation */
    INST_SEQ_TYPE seq;			/* instruction sequence, used to
					   sort the ready list and tag inst */
    unsigned int ptrace_seq;		/* pipetrace sequence number */
  
    int duplicate;			/*092904mwr: 1-duplicate inst, 0-original inst*/
	int pref_flag;        /*hq:indicate whether a prefetched data is used and not sure about the rightness */

    /* instruction status */
    int queued;				/* operands ready and queued */
    int issued;				/* operation is/was executing */
    int completed;			/* operation has completed execution */

    /* Wattch: values of source operands and result operand used for AF generation */
    quad_t val_ra, val_rb, val_rc, val_ra_result;

    /* output operand dependency list, these lists are used to
     limit the number of associative searches into the RUU when
     instructions complete and need to wake up dependent insts */
    int onames[MAX_ODEPS];			/* output logical names (NA=unused) */
    struct RS_link *odep_list[MAX_ODEPS];	/* chains to consuming operations */

    /* input dependent links, the output chains rooted above use these
     fields to mark input operands as ready, when all these fields have
     been set non-zero, the RUU operation has all of its register
     operands, it may commence execution as soon as all of its memory
     operands are known to be read (see lsq_refresh() for details on
     enforcing memory dependencies) */
    int idep_ready[MAX_IDEPS];		/* input operand ready? */
    unsigned long long when_idep_ready[MAX_IDEPS];
    int idep_name[MAX_IDEPS];

    struct RUU_station *prod[MAX_IDEPS];
    counter_t disp_time;
    counter_t finish_time;
    int cache_ready;
    counter_t fetch_time;
    counter_t commit_time;
    counter_t issue_time;
    int invalidationReceived;

    int out1, in1, in2, in3;

    int oldpreg;
    int opreg;

    int cluster;
    int when_ready[MAX_IDEPS];
    int when_inq;
    int pbank;
    int abank;
    int in_qwait;
    int freed;
    int ipreg[MAX_IDEPS];   	/* Extra registers reserved to receive the operand
                                 values from the producer cluster */
    int st_reach[CLUSTERS];
    long instnum;
    int distissue;

    int counted_iissueq;
    int counted_fissueq;
#ifdef PWR_WAKE
    int counted_iready;
    int counted_fready;
    int counted_lsqready;
#endif
#ifdef COUNT_LSQ
    int counted_non_issue_lsq;
#endif

#ifdef RAW
    int counted_rawq[CLUSTERS];
#endif

#ifdef CACHE_MISS_STAT
    int spec_rdb_miss;
    int spec_wrb_miss;
#endif

#ifdef LOAD_PREDICTOR
    int counted_miss;
    int load_hit;
    int ldpred_hit;
#endif

#ifdef SELECTIVE_DISPATCH_GATE
    int cluster_assigned;
    int output1, out2;	/* output/input register names */
#endif

#ifdef N_WAY_CACHE_ADAPT
    int n_cache_limit_inst;
#endif

#ifdef UNREADY_DISPATCH_STAT
    int counted_unready_dispatch;
#endif
    int faulting_instr;		//mwr102004: if true then the instruction has error
    long long int wrongCycle;
    int dl2_missed;
    counter_t sim_num_insn;
    int outReg;
    qword_t prevRegVal;
    struct regs_t prevReg;
    /*RN: 10.11.06*/
    counter_t sqStoreForwardT;      //The dispatch time of the store inst,
                                    //whose store data is forwarded to the load
                                    //inst in this ruu station.
    ////////Enhanced LSQ////////
    half_t  ushFlag;        //Flags of the RUU entry.

    half_t  ushSpecLdDelay; //Number of cycles a spec load is
                            //delayed for issue.
    //REG mapping checkpoints used for load replay
    //struct RecoverChkp Recovery;
    int replayThisEntry, entryReplayed;

    int ldAccessFlag;
    struct regs_t backupReg;
    int robEntry;
    int lsqEntry;
    int isPrefetch; /* inst prefetch */
    int isL1Miss;
    int writewait;
    int ReadShareDir;
    int writemiss;
    int readAfterWrite;
	int STL_C_fail;

#ifdef	EDA
    quad_t o_val_ra, o_val_rb, o_val_rc;
    struct psn_struct poison[3];
    int boqIndex;
    int lockStatus;
    int instType;
    int isWrongPath;
    md_addr_t altPC;
    int changeSpecMode;
#endif
    int isSyncInst;
    int isSyncInstBar;
    int isSyncInstBarRel;
};


#define PHY_TOTAL_REGS 1024

/*
 * RS_LINK defs and decls
 */

/* a reservation station link: this structure links elements of a RUU
   reservation station list; used for ready instruction queue, event queue, and
   output dependency lists; each RS_LINK node contains a pointer to the RUU
   entry it references along with an instance tag, the RS_LINK is only valid if
   the instruction instance tag matches the instruction RUU entry instance tag;
   this strategy allows entries in the RUU can be squashed and reused without
   updating the lists that point to it, which significantly improves the
   performance of (all to frequent) squash events */
struct RS_link {
  struct RS_link *next;			/* next entry in list */
  struct RUU_station *rs;		/* referenced RUU resv station */
  INST_TAG_TYPE tag;			/* inst instance sequence number */
  union {
    tick_t when;			/* time stamp of entry (for eventq) */
    INST_SEQ_TYPE seq;			/* inst sequence */
    int opnum;				/* input/output operand number */
  } x;
  counter_t last_use;			/* debug time stamp of last use */
  counter_t rs_link_id;
};
#define RSLINK_NULL_DATA		{ NULL, NULL, 0 }

 /* an entry in the create vector */
struct CV_link {
  struct RUU_station *rs;               /* creator's reservation station */
  int odep_num;                         /* specific output operand */
};

struct RS_list {
 struct RS_list *next;
 struct RUU_station *rs;
 tick_t when;
 int spec_mode;
 int pref_mode;
};


struct wb_mem_ent {
    struct wb_mem_ent *next;
    md_addr_t instr;
    md_addr_t PC;
    md_addr_t addr;
    unsigned int data[2];
    int nbytes;
    counter_t sim_num_insn;
    counter_t disp_time;
    int lsqId;
    counter_t completeCycle;
    struct wb_mem_ent *prev;
    int is_STL_C;
    int needBusAccess;
    int accessed;
    int writeAlreadyDone;
};

/* speculative memory hash table size, NOTE: this must be a power-of-two */
#define STORE_HASH_SIZE		32

/* speculative memory hash table definition, accesses go through this hash
   table when accessing memory in speculative mode, the hash table flush the
   table when recovering from mispredicted branches */
struct spec_mem_ent {
  struct spec_mem_ent *next;		/* ptr to next hash table bucket */
  md_addr_t addr;			/* virtual address of spec state */
  unsigned int data[2];			/* spec buffer, up to 8 bytes */
};



/* IFETCH -> DISPATCH instruction queue definition */
struct fetch_rec 
{
    int index;
    int threadid;
    unsigned long fetch_time;           /* cycle when inst. fetched  Ali */
    md_inst_t IR;			/* inst register */
    md_addr_t regs_PC, pred_PC;		/* current PC, predicted next PC */
    struct bpred_update_t dir_update;	/* bpred direction update info */
    int stack_recover_idx;		/* branch predictor RSB index */
    unsigned int ptrace_seq;		/* print trace sequence id */
    /*lechen, patch up branch predictions depended on loads in fetch queue*/
    unsigned int is_ctrl2 : 1; 		/*is this a cond branch?*/
    int is_dup;				//100104mwr: is_dup added to recognise duplicate data
#ifdef	EDA
    int boqIndex;
    int lockStatus;
#endif
};

typedef struct context_t 
{
    int running;
    unsigned long long finish_cycle;
    unsigned long long start_cycle;
    int id;

    /* If this context shares memory with another context(s),
       this integer specifies the original context number.
       If this context does not share memory, then this is
       equal to its context id. */
    int masterid;
    int parent;
    int num_child;
    int num_child_active;
    unsigned long long fastfwd_count;
#ifdef	COLLECT_STAT_STARTUP
    int fastfwd_done;
    int barrierReached;
    int startReached;
    int jobThdId;
    int helpThdId;
    int oracleId;
#endif
    FILE *argfile;
    char *sim_progout;
    char *sim_progin;
    FILE *sim_progfd;
    int sim_inputfd;

    /*  new used variables  */
    char *fname;
    char **argv;
    int argc;

    /* simulated registers */
    struct regs_t regs;

    /* simulated memory */
    struct mem_t *mem;


    /*  moved variables from load.c  */
    /* program text (code) segment base */
    md_addr_t ld_text_base ;

    /* program text (code) size in bytes */
    unsigned int ld_text_size ;

    /* program initialized data segment base */
    md_addr_t ld_data_base ;

    /* top of the data segment */
    md_addr_t ld_brk_point ;

    /* program initialized ".data" and uninitialized ".bss" size in bytes */
    unsigned int ld_data_size ;

    /* program stack segment base (highest address in stack) */
    md_addr_t ld_stack_base ;

    /* program initial stack size */
    unsigned int ld_stack_size ;

    /* lowest address accessed on the stack */
    md_addr_t ld_stack_min ;

    /* program file name */
    char *ld_prog_fname ;

    /* program entry point (initial PC) */
    md_addr_t ld_prog_entry;

    /* program environment base address address */
    md_addr_t ld_environ_base ;

    /* target executable endian-ness, non-zero if big endian */
    int ld_target_big_endian;

    /*  moved variables from main.c  */
    /* EIO interfaces */
    char *sim_eio_fname;
    char *sim_chkpt_fname;
    FILE *sim_eio_fd;

    /* byte/word swapping required to execute target executable on this host */
    int sim_swap_bytes;
    int sim_swap_words;


    /*  moved variables from sim-outorder.c  */
    /*
     * simulator stats
     */
    int oldwindowilp[OWINDOWSIZE];
    int oldwindowbr[OWINDOWSIZE];
    long oldwindowpc[OWINDOWSIZE];
    int owindow_head;

    int ltopmap[MD_TOTAL_REGS];
    int ltopmap_old[MD_TOTAL_REGS][CLUSTERS][COMM_LATENCY];
    int lprod[MD_TOTAL_REGS];
    int lprod_rd[MD_TOTAL_REGS];
    int ltopmap_chk[MD_TOTAL_REGS];
    int lprod_chk[MD_TOTAL_REGS];
    int lprod_rd_chk[MD_TOTAL_REGS];

    struct lreg_id_t ptolmap[PHY_TOTAL_REGS];
    struct lreg_id_t consumers[PHY_TOTAL_REGS];
    struct lreg_id_t ptolmap_chk[PHY_TOTAL_REGS];

    /* total number of instructions committed */
    counter_t sim_num_insn;/* dispatched --hq */
	counter_t pure_num_insn;

    /* total number of instructions executed */
    counter_t sim_total_insn;/*useless --hq*/

    /* number of useful instructions fetched */
    counter_t fetch_num_insn;/*useless --hq*/

    /* number of total instructions fetched */
    counter_t fetch_total_insn;

    /* total number of memory references committed */
    counter_t sim_num_refs;

    /* total number of memory references executed */
    counter_t sim_total_refs;

    /* total number of loads committed */
    counter_t sim_num_loads;/*useless --hq */

    /* total number of loads executed */
    counter_t sim_total_loads;/* useless --hq */

    /* total number of branches committed */
    counter_t sim_num_branches;

    /* total number of branches executed */
    counter_t sim_total_branches;

    /* total non-speculative bogus addresses seen (debug var) */
    counter_t sim_invalid_addrs;

    /*
     * simulator state variables
     */

    /* instruction sequence counter, used to assign unique id's to insts */
    unsigned int inst_seq;/* change when dispatch, allocate two num for ld/st? --hq*/

    /* pipetrace instruction sequence counter */
    unsigned int ptrace_seq;

    /* speculation mode, non-zero when mis-speculating, i.e., executing
       instructions down the wrong path, thus state recovery will eventually have
       to occur that resets processor register and memory state back to the last
       precise state */
    int spec_mode;
	/* similar as spec_mode, set to 1 if there's a speculated prefetch, and the assertion message hasn't received --hq*/
	int pref_mode;


    /*mwr102104: if the context is in fault or not*/
    int fault_mode;

    /* cycles until fetch issue resumes */
    unsigned ruu_fetch_issue_delay;
    int wait_for_fetch;

    /* register update unit, combination of reservation stations and reorder
       buffer device, organized as a circular queue */
    struct RUU_station *RUU;		/* register update unit */
    int RUU_head, RUU_tail;		/* RUU head and tail pointers */
    int RUU_num;			/* num entries currently in RUU */
    counter_t  RUU_count_thrd;     /* cumulative RUU occupancy */
    counter_t  RUU_fcount_thrd;    /* cumulative RUU full count */


    struct RUU_station *LSQ;         /* load/store queue */
    int LSQ_head, LSQ_tail;          /* LSQ head and tail pointers */
    int LSQ_num;                     /* num entries currently in LSQ */
    int LSQ_count;
    int LSQ_fcount;
    short	m_shLQNum;		//Number of entries in Load Queue.
    short	m_shSQNum;		//Number of entries in Store Queue.


    int fetch_num_thrd;	/* num of instructions currently in fetch queue */
    int iissueq_thrd;	/* num of instructions currently in integer queue */
    int fissueq_thrd;	/* num of instructions currently in floating queue */
#ifdef COUNT_LSQ
    int non_issue_lsq_thrd;   	/* num of instructions currently in ld/st queue */
    int lsq_thrd;   	/* num of instructions currently in ld/st queue including issued loads*/
#endif
    int icount_thrd;	/* num of instructions currently in decode, rename issue queues */

    /* the create vector, NOTE: speculative copy on write storage provided
       for fast recovery during wrong path execute (see tracer_recover() for
       details on this process */
#ifndef TEST_BUG
    BITMAP_TYPE(MD_TOTAL_REGS, use_spec_cv);
    BITMAP_TYPE(MD_NUM_IREGS, use_spec_R);
    BITMAP_TYPE(MD_NUM_FREGS, use_spec_F);
    BITMAP_TYPE(MD_NUM_FREGS, use_spec_C);
#endif
    struct CV_link create_vector[MD_TOTAL_REGS];
    struct CV_link spec_create_vector[MD_TOTAL_REGS];

    /* these arrays shadow the create vector an indicate when a register was
       last created */
    tick_t create_vector_rt[MD_TOTAL_REGS];
    tick_t spec_create_vector_rt[MD_TOTAL_REGS];

    /* integer register file */
    md_gpr_t spec_regs_R;

    /* floating point register file */
    md_fpr_t spec_regs_F;

    /* miscellaneous registers */
    md_ctrl_t spec_regs_C;

    /* program counter */
    md_addr_t pred_PC;
    md_addr_t recover_PC;

    /* fetch unit next fetch address */
    md_addr_t fetch_regs_PC;
    md_addr_t fetch_pred_PC;

    /* the last operation that ruu_dispatch() attempted to dispatch, for
       implementing in-order issue */
    struct RS_link last_op;

    /* speculative memory hash table */
    struct spec_mem_ent *store_htable[STORE_HASH_SIZE];
    /* speculative memory hash table bucket free list */
    struct spec_mem_ent *bucket_free_list;


    struct wb_mem_ent *WBtableHead;
    struct wb_mem_ent *WBtableTail;
	
    struct wb_mem_ent *WBbucket_free_list;
	


    int fetch_redirected;       /* moved from sim-outorder.c:ruu_dispatch  */

    /* Statistics  over simulation run time */
#ifdef  DELAY_STAT
    double fetch_delay_stat;
    double max_fetch_delay_stat;
#endif

#ifdef CACHE_MISS_STAT
    unsigned long spec_rdb_miss_count;
    unsigned long spec_wrb_miss_count;
    unsigned long non_spec_rdb_miss_count;
    unsigned long non_spec_wrb_miss_count;
    unsigned long inst_miss_count;
    unsigned long load_miss_count;
    unsigned long store_miss_count;
#endif

#ifdef LOAD_PREDICTOR
    int ld_miss;
    int active;
#endif

#ifdef SELECTIVE_DISPATCH_GATE
    int dispatch_blocked;
    counter_t disp_block_time;
#endif

#ifdef UNREADY_DISPATCH_STAT
    int unready_dispatch;
#endif
    md_addr_t stack_base;


    unsigned long numDL1CacheAccess;
    unsigned long numLocalHits;
    unsigned long numRemoteHits;
    unsigned long numMemAccess;
    unsigned long numReadCacheAccess;
    unsigned long numWriteCacheAccess;
    unsigned long numInvalidations;
    unsigned long numDL1Hits;
    unsigned long numDL1Misses;
    unsigned long numInsn;

    /*from lechen's sim-outorder.c: */
    /* another set of branch instr buffer, used for history update*/
    /* addresses of the branch instructions in the pipeline*/
    unsigned int *branch_address2;
    int branch_head2, branch_tail2; /*head and tail of branch_address*/
    int my_last_tag;
    unsigned cycle_mask;		/*101904mwr: variable to determin which cycles the context will be active*/
    float latency_factor;		/*102104mwr: variable to increase latency of different activity to mimic slow processor*/
    int active_this_cycle;		/*102104mwr: 1 means thread active in this cycle*/
    int pcb_penalty;
    int main_thread;	/*mwr102504: 1 means this is the main thread*/
    power_result_type power;		/*110404mwr: power parameters structure*/


    counter_t rename_access;
    counter_t iqram_access;
    counter_t fqram_access;
    counter_t rob1_access;
    counter_t rob2_access;
    counter_t bpred_access;
    counter_t iwakeup_access;
    counter_t fwakeup_access;
    counter_t iselection_access;
    counter_t fselection_access;
    counter_t iregfile_access;
    counter_t fregfile_access;

    counter_t lsq_access;
    counter_t icache_access;
    counter_t dcache_access;
    counter_t dcache2_access;
    counter_t input_buf_access;
    counter_t output_buf_access;
    counter_t icache2_access;

    counter_t ialu1_access;
    counter_t ialu2_access;
    counter_t falu1_access;
    counter_t falu2_access;
    counter_t iresultbus_access;
    counter_t fresultbus_access;
    counter_t lsq_preg_access;
    counter_t lsq_wakeup_access;

    int isFFdone;


    struct bpred_t *pred;
    struct fetch_rec *fetch_data[128];	/* IFETCH -> DISPATCH inst queue */
    int fetch_num;			/* num entries in IF -> DIS queue */
    int fetch_tail, fetch_head;	/* head and tail pointers of queue */


    unsigned long TotalnumDL1CacheAccess;
    unsigned long TotalnumLocalHits;
    unsigned long TotalnumRemoteHits;
    unsigned long TotalnumMemAccess;
    unsigned long TotalnumReadCacheAccess;
    unsigned long TotalnumWriteCacheAccess;
    unsigned long TotalnumInvalidations;
    unsigned long TotalnumDL1Hits;
    unsigned long TotalnumDL1Misses;
    unsigned long TotalnumInsn;
    unsigned long NRLocalHitsLoad;
    unsigned long NRRemoteHitsLoad;
    unsigned long NRMissLoad;
    unsigned long NRLocalHitsStore;
    unsigned long NRRemoteHitsStore;
    unsigned long NRMissStore;
    unsigned long present;

    /* For barrier */
    unsigned long numInstrBarrier;
    unsigned long numCycleBarrier;
    unsigned long totalBarrierInstr;
    unsigned long totalBarrierCycle;

    unsigned long numInstrLock;
    unsigned long numCycleLock;
    unsigned long totalLockInstr;
    unsigned long totalLockCycle;

    unsigned int L0BufferHits;
    unsigned int TotalLoads;

    unsigned int DataLocatorHit;
    unsigned int DataLocatorMiss;

    unsigned long sleptAt; /* The thread executed the quiese instruction at this cycle */
    int freeze; 
    int stallThread;
    int waitFor;
    int atMB;
    int waitForSTLC;
    int numOfWBEntry;
    int WBFull;
    int waitForBranchResolve;
	int waitForPrefetchInfo;/*wait for assertion message for speculated prefetch --hq*/
    unsigned long loadLatency;
    unsigned long numLoads;
    int stopThread;
    int actualid;
    int ffStall;
    int localSubEpoch;
    int lastSubEpochSeen[32];
#ifdef	EDA
    branchCorr *predQueue;
    quad_t o_val_ra, o_val_rb, o_val_rc;
    int boqIndex;
    int lockStatus;
    md_addr_t lockExitPC;
#endif
   struct DIRECTORY_EVENT *event;
} context;

extern int numcontexts;
extern context *thecontexts[MAXTHREADS];


struct sharedAddressList_s {
    unsigned long long address;
    unsigned int size;
    struct sharedAddressList_s * next;
};

struct sharedAddressList_s *sharedAddressList[512];


//extern char *sim_progout;

  /* read argument file *.bnc for each thread */
int
read_thread_args(char *env[], char *filename, int *curcontext);

void update_power_stats(power_result_type, int, context*);
//void whatTheFuck(byte);
#endif

#endif

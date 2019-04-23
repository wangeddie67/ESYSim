/*
 * $Log: smt.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:28  xue
 * CMP_NETWORK
 *
 * Revision 1.2  2009/03/05 16:58:23  garg
 * Updated version of the CMP network simulator.
 *
 * Revision 1.7  2005/01/12 22:35:14  rashid
 * disabled load predictor
 *
 * Revision 1.6  2004/11/08 20:31:33  etan
 * Added MAXTHREADS 4
 *
 * Revision 1.5  2004/11/02 22:20:40  rashid
 * *** empty log message ***
 *
 * Revision 1.4  2004/10/01 16:01:30  rashid
 * changed back to previous state.
 *
 * Revision 1.3  2004/10/01 02:44:09  rashid
 * *** empty log message ***
 *
 * Revision 1.2  2004/09/27 20:30:56  etan
 * Changed to 4 clusters instead of 16.
 *
 * Revision 1.1  2004/09/22 21:20:25  etan
 * Initial revision
 *
 * Revision 1.30  2004/08/02 17:22:12  ninelson
 * TOKENB defined.
 *
 * Revision 1.29  2004/07/15 17:02:46  grbriggs
 * Fix a badly-coded CL_FLOAT_DIV(x)
 *
 * Revision 1.28  2004/07/13 00:31:41  ninelson
 * removed -N_WAY from command line option
 *
 * Revision 1.27  2004/07/02 21:26:36  grbriggs
 * Make N_WAY_CACHE  1 (and fix LSQ_PARTS to correpond).
 *
 * Revision 1.26  2004/06/30 17:03:47  ninelson
 * ability to switch quickly from 8 to 16 clusters
 *
 * Revision 1.25  2004/06/24 19:56:28  grbriggs
 * Lots more comments.
 *
 * Revision 1.24  2004/06/23 22:30:23  grbriggs
 * Added lots of comments.
 *
 * Revision 1.23  2004/06/23 21:31:20  grbriggs
 * Add comments. Also fix macro redefinition bug.
 *
 * Revision 1.22  2004/06/23 20:11:21  ninelson
 * *** empty log message ***
 *
 * Revision 1.21  2004/06/23 19:44:00  grbriggs
 * Add another comment.
 *
 * Revision 1.20  2004/06/23 19:03:35  ninelson
 * *** empty log message ***
 *
 * Revision 1.19  2004/06/23 18:57:05  grbriggs
 * Add comments; Turn on MSHR_DIST.
 *
 * Revision 1.18  2004/06/23 15:17:56  grbriggs
 * Make MSHRSIZE reasonable.
 *
 * Revision 1.17  2004/06/23 02:19:33  ninelson
 * all from garg and ali, to date.
 *
 * Revision 1.16  2004/06/18 21:49:45  grbriggs
 * Initial move of code from garg.
 *
 * Revision 1.15  2004/06/15 19:55:50  ninelson
 * added defines for LOAD_COMM_BUG and DISPATCH_COMM_LATECY_BUG
 *
 * Revision 1.14  2004/06/14 21:28:20  grbriggs
 * Fix CL_INT_DIV macro thinko.
 *
 * Revision 1.13  2004/06/10 21:51:33  ninelson
 *  Changes to fix MISFETCH_BUG FETCH_QUEUE_COMPACT PERFECT_PREDICTOR_BUG AND FETCH_LIMIT_THREAD_BUG
 *
 * Revision 1.12  2004/06/10 15:52:52  grbriggs
 * Figured out that re really need seperate CL_INT_DIV and CL_FLOAT_DIV. Fixed now.
 *
 * Revision 1.11  2004/06/09 22:02:00  grbriggs
 * Still some think-o, but now its not obvious.
 *
 * Revision 1.10  2004/06/09 21:56:20  grbriggs
 * Fix think-o.
 *
 * Revision 1.9  2004/06/09 21:35:55  grbriggs
 * Add CL_DIV and CL_MOD optimizations.
 *
 */

          /*  SMT_SS add SMT to SimpleScalar  simplesim-3.0b_Rajeev_base14 */     //  ali Dec./09/2002
#define SMT_SS

#ifdef SMT_SS

/* cache consistancy model */
#define RELAXED_CONSISTENCY
//#define SEQUENTIAL_CONSISTENCY

#define MAXTHREADS 64		/* Number of threads to support */
#define WORDSIZE 8		/* Number of bytes per word */

//#define  ROUND_ROBIN

/* COUNT_LSQ causes non-issue LSQ statistics to be recorded.
 * Also, it temporarily makes current->lsq_thrd be one-larger
 * then it would have otherwise been (why??). */

/* WARNNING: SHOULD BE MODIFIED FOR "DISTRIBUTE_STORE" IN THE D_CENTRALIZED_CACHE*/
#define  COUNT_LSQ
/* WARNNING: SHOULD BE MODIFIED FOR "DISTRIBUTE_STORE" IN THE D_CENTRALIZED_CACHE*/

#ifdef COUNT_LSQ
/* IC_LSQ will include non-issue LSQ in the icount */
//#define  IC_LSQ
#endif // for COUNT_LSQ

//#define  IC_INFLIGHT

/* RES1_FULL_MASK affects cmt.c: if defined, it will maintain 
 * a mask (one bit per cluster, one mask per thread) that requires 
 * threads to be kept on clusters marked in their mask. The mask is
 * initialized with marks starting with n_start_thrd[t] and will 
 * be n_limit_thrd[t] consecutive clusters. Currently this only
 * controls the N_WAY_STEER version of get_cluster.  */
#define RES1_FULL_MASK 

////////////////////Micro-architecture Designs///////////////////
////////////////////// non means CMT_MACHINE////////////////////

#define CMP_MACHINE

#if (MAXTHREADS == 1)
  #define MESH_SIZE 1
  #define CLUSTERS 1
  #define PREG_CLUSTER 1
  #define CL_INT_DIV(x)   ((x) >> 0) // optimization, based on 2^4 = 16
  #define CL_FLOAT_DIV(x) ((x) * 1) // optimization, based on 1/16 = 0.0625
  #define CL_MOD(x)       ((x) & 0)  // optimization, based on 15 = 16 - 1 = 0b1111
#endif // for CLUSTERS1
#if (MAXTHREADS == 2)
  #define MESH_SIZE 2
  #define CLUSTERS 2
  #define PREG_CLUSTER 2
  #define CL_INT_DIV(x)   ((x) >> 1) // optimization, based on 2^4 = 16
  #define CL_FLOAT_DIV(x) ((x) * .5) // optimization, based on 1/16 = 0.0625
  #define CL_MOD(x)       ((x) & 1)  // optimization, based on 15 = 16 - 1 = 0b1111
#endif // for CLUSTERS2
#if (MAXTHREADS == 4)
  #define MESH_SIZE 2
  #define CLUSTERS 4
  #define PREG_CLUSTER 4
  #define CL_INT_DIV(x)   ((x) >> 2) // optimization, based on 2^4 = 16
  #define CL_FLOAT_DIV(x) ((x) * .25) // optimization, based on 1/16 = 0.0625
  #define CL_MOD(x)       ((x) & 3)  // optimization, based on 15 = 16 - 1 = 0b1111
#endif // for CLUSTERS4
#if (MAXTHREADS == 8)
  #define MESH_SIZE 4
  #define CLUSTERS 8
  #define PREG_CLUSTER 8
  #define CL_INT_DIV(x)   ((x) >> 3) // optimization, based on 2^4 = 16
  #define CL_FLOAT_DIV(x) ((x) * .125) // optimization, based on 1/16 = 0.0625
  #define CL_MOD(x)       ((x) & 7)  // optimization, based on 15 = 16 - 1 = 0b1111
#endif // for CLUSTERS8
#if (MAXTHREADS == 16)
  #define MESH_SIZE 4
  #define CLUSTERS 16
  #define PREG_CLUSTER 16
  #define CL_INT_DIV(x)   ((x) >> 4) // optimization, based on 2^4 = 16
  #define CL_FLOAT_DIV(x) ((x) * .0625) // optimization, based on 1/16 = 0.0625
  #define CL_MOD(x)       ((x) & 15)  // optimization, based on 15 = 16 - 1 = 0b1111
#endif // for CLUSTERS16
#if (MAXTHREADS == 32)
  #define MESH_SIZE 4
  #define CLUSTERS 32
  #define PREG_CLUSTER 32
  #define CL_INT_DIV(x)   ((x) >> 5) // optimization, based on 2^5 = 32
  #define CL_FLOAT_DIV(x) ((x) * .03125) // optimization, based on 1/32 = 0.03125
  #define CL_MOD(x)       ((x) & 31)  // optimization, based on 31 = 32 - 1 = 0b1111
#endif // for CLUSTERS16
#if (MAXTHREADS == 64)
  #define MESH_SIZE 8
  #define CLUSTERS 64
  #define PREG_CLUS64R 64
  #define CL_INT_DIV(x)   ((x) >> 6) // optimization, based on 2^5 = 64
  #define CL_FLOAT_DIV(x) ((x) * .015625) // optimization, based on 1/64 = 0.0015625
  #define CL_MOD(x)       ((x) & 63)  // optimization, based on 63 = 64 - 1 = 0b1111
#endif // for CLUSTERS64
#if (MAXTHREADS == 80)
  #define MESH_SIZE 10
  #define CLUSTERS 80
  #define PREG_CLUS64R 80
  #define CL_INT_DIV(x)   ((x) >> 6) // optimization, based on 2^5 = 64
  #define CL_FLOAT_DIV(x) ((x) * .015625) // optimization, based on 1/64 = 0.0015625
  #define CL_MOD(x)       ((x) & 63)  // optimization, based on 63 = 64 - 1 = 0b1111
#endif // for CLUSTERS64
#if (MAXTHREADS == 128)
  #define MESH_SIZE 16
  #define CLUSTERS 128
  #define PREG_CLUS64R 128
  #define CL_INT_DIV(x)   ((x) >> 7) // optimization, based on 2^5 = 64
  #define CL_FLOAT_DIV(x) ((x) * .0078125) // optimization, based on 1/64 = 0.0015625
  #define CL_MOD(x)       ((x) & 127)  // optimization, based on 63 = 64 - 1 = 0b1111
#endif // for CLUSTERS128
#if (MAXTHREADS == 256)
  #define MESH_SIZE 16
  #define CLUSTERS 256
  #define PREG_CLUS64R 256
  #define CL_INT_DIV(x)   ((x) >> 8) // optimization, based on 2^5 = 64
  #define CL_FLOAT_DIV(x) ((x) * .00390625) // optimization, based on 1/64 = 0.0015625
  #define CL_MOD(x)       ((x) & 255)  // optimization, based on 63 = 64 - 1 = 0b1111
#endif // for CLUSTERS256

 #ifdef CMC_MACHINE  //
#define  CMC_PARTS    2    //CLUSTERS
#define  CMC_N_WAY    (CLUSTERS/CMC_PARTS)
#define  CMC_START_N_WAY
#define  CMC_FRONT_END
#define  PARTITIONED_LSQ
 #endif // for CMC_MACHINE

/////////////Micro-architecture specific options//////////////////


 #ifdef CMP_MACHINE  // N_WAY , N_CACHE_WAY should be set to 1 for CMP
//#define ROUND_ROBIN
#define  CMP_N_WAY
#define PARTITIONED_FRONT_END
#define PARTITIONED_LSQ
#define NO_OVERHEAD
#define NO_COMMUNICATION
 #endif // for CMP_MACHINE


/////////////pipeline stage designs//////////////////

//#define PARTITIONED_FRONT_END  /* used in this file, to turn on and off other #define's. See below. */

//#define NO_OVERHEAD

//#define UNIFORM_COMM_LATENCY

//#define IGNORE_ALL_COMM_LATENCY
//#define IGNORE_DISPATCH_COMM_LATENCY
//#define IGNORE_MEM_COMM_LATENCY

//#define  GENERIC_FU

//#define NO_EXTRA_RESOURCE  // including both reg and lsq entries
//#define NO_EXTRA_REG
//#define NO_EXTRA_LSQ
/////////////pipeline stage designs//////////////////

/////////////pipeline specific options//////////////////

 #ifdef NO_OVERHEAD
#define IGNORE_ALL_COMM_LATENCY
#define NO_EXTRA_RESOURCE  // including both reg and lsq entries
 #endif // for NO_OVERHEAD

 #ifdef IGNORE_ALL_COMM_LATENCY
#define IGNORE_MEM_COMM_LATENCY
#define IGNORE_DISPATCH_COMM_LATENCY
 #endif // for IGNORE_ALL_COMM_LATENCY

 #ifdef NO_EXTRA_RESOURCE
#define NO_EXTRA_REG
#define NO_EXTRA_LSQ
 #endif // for NO_EXTRA_RESOURCE
/////////////pipeline specific options//////////////////

#define IFREELIST_FREELIST
 /* ALLOW_ANY_CLUSTER makes SMALLEST_CONFIG = CLUSTERS 
  * instead of = 4 in sim-outorder.c, which in turn sets
  * actual_clusters, affecting the number of issue queues and
  * register files available. */
#define  ALLOW_ANY_CLUSTER
//#define ALU_MERGE

/*  to include the active no. of threads in the start cluster/bank
for each thread
if commented start point is the default (CLUSTERS/numcontexts) and N_WAY is
the factor define the cluster/bank
if used N_WAY define is fixed banks used in caches, and FIXED_N_WAY
do thae same job in steering
   */
//#define ACTIVE_N_WAY

 #ifdef ACTIVE_N_WAY
//#define   FIXED_N_WAY    // optional can be commented
 #else
 #define   FIXED_N_WAY   // PERMINANT NEVER COMMENT
 #endif // for ACTIVE_N_WAY

////////////////////Front-end /////////////////////////////////
#define FETCH_QUEUE_COMPACT  // PERMINANT SHOULD ALWAYS BE EXIST
//#define CHECK_FETCH_QUEUE
#define PERFECT_PREDICTOR_BUG
#define FETCH_LIMIT_THREAD_BUG

//#define  ROUND_ROBIN
//#define FETCH_BANK_MULT
//#define STEER_NEIGHBOR_BIAS

	/* ROUND_ROBIN causes get_next_thread() to examine threads in 
	 * the order they are indexed in the contexts array, wrapping
	 * back to the front apon reaching the end. Otherwise, the
	 * threads would have been examined in the order prescribed
	 * by the priority[] table. get_next_thread() tells 
	 * ruu_fetch() which thread to fetch instructions for. */

	/* FETCH_BANK_MULT allows instructions to fetched from the
	 * the same Icache bank as another thread at the same
	 * time. Normally only one thread would be allowed to
	 * access a particular Ibank during a cycle. Used in 
	 * get_next_thread() in cmt.c. */

	/* INTERLEAVE_FETCH causes get_next_thread() to start it's
	 * round robin from the beginning of the thread list each time
	 * it is called, instead of starting where it left off during 
	 * that ruu_fetch() call ( get_next_thread() is 
	 * called inside a loop by ruu_fetch() ). Regardless
	 * of this flag, ruu_fetch will set it to start from the 
	 * beginning, before the first call from the loop. 
	 * As you can see below, this also turns on FETCH_BANK_MULT. */

//#define INTERLEAVE_FETCH
 #ifdef INTERLEAVE_FETCH
#define  ROUND_ROBIN
#define FETCH_BANK_MULT
	/* Search below for info on STEER_NEIGHBOR_BIAS. */
#define STEER_NEIGHBOR_BIAS
 #endif // for INTERLEAVE_FETCH

/* Examine the fetch schemes*/
//#define EXAMINE_FETCH
 #ifdef EXAMINE_FETCH
//#define FETCH_BANDWIDTH  1024
#define FETCH_TARGET_TRACE
//#define FETCH_BANK_MULT
//#define NO_ICACHE_MISS_DELAY  // never delay fetch for IL1 miss
//#define NO_ICACHE_ACCESS
//#define NO_ICACHE_DELAY  // never delay fetch
 #endif // for EXAMINE_FETCH

	/* SELECTIVE_DISPATCH causes ruu_dispatch in sim-outorder.c to
	 * pay attention to several more considerations in choosing
	 * what to dispatch (Adds like 400 lines of code!). */

	/* PARTITIONED_FRONT_END makes the below listed pieces of the front
	 * end to be distributed among the clusters. */
 #ifdef PARTITIONED_FRONT_END
	/* FETCH_QUEUE_CLUSTER makes there be one fetch queue per cluster */
#define FETCH_QUEUE_CLUSTER
	/* DISPATCH_CLUSTER makes there be one dispatch per cluster.
	 * Communication costs are computed as if all dispatches are located
	 * off of cluster zero, unless you turn on DISPATCH_COMM_LATENCY_CMC. */
#define DISPATCH_CLUSTER
//#define SELECTIVE_DISPATCH
	/* ICACHE_BANK_CLUSTER makes there be one icache bank per cluster */
#define ICACHE_BANK_CLUSTER
#define FETCH_BANK_MULT  // the conflict in the icache banks is impossible
//#define NO_L2CACHE_ACCESS
 #endif // for PARTITIONED_FRONT_END

 #ifdef CMC_FRONT_END
	/* FETCH_QUEUE_CMC makes there be a total of CMC_PARTS fetch queues */
#define FETCH_QUEUE_CMC
	/* DISPATCH_CMC makes there be a total of CMC_PARTS dispatches */
#define DISPATCH_CMC
#define SELECTIVE_DISPATCH
	/* DISPATCH_COMM_LATENCY_CMC causes inclusion of the comm. delay 
	   between the location of the dispatch and the cluster where the
	   instruction is being sent */
#define DISPATCH_COMM_LATENCY_CMC
	/* ICACHE_BANK_CMC makes there be a total of CMC_PARTS icache banks */
#define ICACHE_BANK_CMC
#define FETCH_BANK_MULT  // the conflict in the icache banks is impossible
//#define NO_L2CACHE_ACCESS
 #endif // for CMC_FRONT_END

	/* FETCH_QUEUE_THREAD partitions the total fetch queue size amoung
	 * the number of threads, and ensures that each thread does not fetch 
	 * more instructions than it has room for in the IFQ. */
#define FETCH_QUEUE_THREAD
#define SELECTIVE_DISPATCH

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/* Examine the DL1 cache*/
//#define NO_DCACHE_ACCESS
//#define NO_DCACHE_PORT_ACCESS

//////////////////////////////////////////////////////////////////////
/* PDG */
//#define LOAD_PREDICTOR

//#define COUNT_MISS_ONLY

//#define PDG
	/* NO_GATE is only used here to set MIN_PDG */
	/* Inactive (active=0) threads will be fetched anyway when
	 * there are less than MIN_PDG active threads. Inactive
	 * seems to mean that the load predictor missed. 
	 * No effect if PDG isn't defined. */
#define NO_GATE
 #ifdef NO_GATE
#define MIN_PDG MAXTHREADS+1
 #else
#define MIN_PDG  2
 #endif // for NO_GATE
/* if both defined GATE_L1_MISS will be the active one,
  if none define is aggressive case as if you don't have information about
  the actual hit/miss and depend only on the predictor  */
	/* GATE_L[12]_MISS will compare load_lat to cache_dl[12]_lat 
	 * to see if cache result ready. */
//#define GATE_L1_MISS
//#define GATE_L2_MISS
	/* If you want GATE_L1_MISS or GATE_L2_MISS, then you must turn on
	 * either PREDICT_LOAD or PREDICT_LOAD_COMM */
//#define PREDICT_LOAD
	/* PREDICT_LOAD_COMM will add a commuications delay to talk to the cache */
//#define PREDICT_LOAD_COMM
	/* Must use LOAD_MISS if you don't use GATE_L[12]_MISS.
	 * It simply checks whether there was a cache miss. */
#define LOAD_MISS

//////////////////////////////////////////////////////////////////////
/* Different steering hieristics */

/* Rely on a priority list for cluster selection while  steering  */
//#define  CLUSTER_PRIORITY
/* distribute amoung the clusters. Like N_WAY_STEER without the N_WAY part. */
//#define DISTRIBUTED_STEER
/* fill the cluster before jump to the next sequentially  RoundRobin */
//#define CENTRALIZED_STEER
/* fill cluster 0 before jump to the next sequentially   */
//#define MINIMUM_STEER
/* Only initially fill the cluster before jump to the next from the threads sequentially   */
//#define INIT_CENTRALIZED_STEER
/* dedicate the cluster to specific thread/s     */
//#define INIT_CMP_STEER
/*  dedicate a cluster for the memory instructions (ld/st)  */
//#define MEM_STEER
/* steer thread to n clusters    */
#define N_WAY_STEER

/* STEER_CACHE_COUPLING is only used in smt.h (below). 
 * It makes N_WAY_CACHE = N_WAY instead of 1 or 2. */
//#define STEER_CACHE_COUPLING

/* Rajeev steering */
//#define RAJEEV_STEER

/*  priority based steering  */
//#define PRIORITY_STEER

/*  communication based steering  */
//#define COMMUNICATION_STEER
 #define TOTAL_COMM
// #define MAX_COMM
//  #define DISPATCH_COMM_STEER
 #define COMM_FACTOR   10
	/* MEM_BANK_FACTOR is the cost to talk to the pbank */
 #define MEM_BANK_FACTOR  0
// #define CACHE_WAY_BIAS

/* try to steer memory instructions (ld/st) to the cluster that has LSQ queue */
//#define STEER_MEM_LSQ

/* try to steer instructions to the cluster that has its operands  */
#ifndef IGNORE_ALL_COMM_LATENCY
#define STEER_OPRANDS
#endif // for IGNORE_ALL_COMM_LATENCY

/* try to steer memory inst. (ld/st) to the cluster that has LSQ queue or its neighbor */
#ifndef IGNORE_MEM_COMM_LATENCY
#define STEER_MEM_NEIGHBOR  1	/* This turns on LD_PROXIMITY in cluster.h, which tells cmt.c's
				 * get_cluster functions to try to steer threads closer
				 * to their cache locations. */
//#define LOAD_BANK_STEER
#endif // for IGNORE_MEM_COMM_LATENCY

/* try to balance the functional unit load */
//#define STEER_FU

/* try to balance the iq & regfile load */
#define STEER_RES1

/*  neighbor bias steering  */
/* STEER_NEIGHBOR_BIAS affects N_WAY_STEER's get_cluster()
 * such that preference is given to adjacent-numbered
 * clusters. See cmt.c. */
//#define STEER_NEIGHBOR_BIAS

 #ifdef STEER_NEIGHBOR_BIAS
	/* INIT_N_WAY_CENTRAL steers toward the middle of the cluster range that the
	 * thread is limited to (also using steer_cluster_thrd[][]). */
#define  INIT_N_WAY_CENTRAL
 #endif // for STEER_NEIGHBOR_BIAS

#ifdef  CENTRALIZED_STEER
// INIT_CENTRALIZED_STEER creates variables 'ipreg_cluster' and 'fpreg_cluster'
#define INIT_CENTRALIZED_STEER 
#endif // for CENTRALIZED_STEER

#ifdef  MINIMUM_STEER
// INIT_CENTRALIZED_STEER creates variables 'ipreg_cluster' and 'fpreg_cluster'
#define INIT_CENTRALIZED_STEER
#endif // for MINIMUM_STEER

#ifdef  MEM_STEER
#define  DISTRIBUTED_STEER
#define  CENTRALIZED_LSQ
#endif // for MEM_STEER

#ifdef  STEER_MEM_LSQ
 #ifndef  INIT_CMP_STEER
#define  DISTRIBUTED_STEER
 #endif // for STEER_MEM_LSQ
#endif // for SMT_SS

#ifdef  N_WAY_STEER
//#define SELECTIVE_DISPATCH
//#define SELECTIVE_FETCH    // should have  SELECTIVE_DISPATCH

//#define EXAMIN_N_WAY_ADAPT
 #ifdef EXAMIN_N_WAY_ADAPT
//#define MIN_ADAPT
//#define ALWAYS_INVASTIGATE
#define PERIODIC_INVASTIGATE  1000
#define DOUBLE_ADAPT
//#define INCREMENT_ADAPT 1
//#define ACCEND_ORDER
#define ADAPT_RATE  1000
//#define MEMORYLESS
//#define N_WAY_RELATIVE
//#define GEOMETRIC_MEAN
#define ARITHMATIC_MEAN
#define ACCUMILATE  5
//#define EXAMIN_THREAD
 #ifdef  CMC_PARTS
#define EXAMIN_CMC
 #endif // for CMC_PARTS
 #endif // for EXAMIN_N_WAY_ADAPT


//#define N_CACHE_FIXED
//#define N_WAY_BALANCE

//#define   PRIVATE_STEER    // should be commented if no adaptation is used
/* We need to gate the dispatch if some conditions don't fulfilled for
successful cluster selection,then we want to increase  optimum steering */
//#define SELECTIVE_DISPATCH_GATE  // should have  SELECTIVE_DISPATCH
 #define DISABLE_DISPATCH_GATE  0
//  #define OPTIMUM_CLUSTER
// #define NON_BLOCKED_LOAD
// #define OPER_COMM_RATIO
// #define OPER_COMM_RATIO_AVG_PRIVATE
// #define OPER_COMM_RATIO_PRIVATE
	/* RATIO_LIMIT is used in ruu_dispatch and get_cluster 
	 * to enforce (comm_lat/oper_lat)<RATIO_LIMIT; depends
	 * on the OPER_COMM_RATIO flags above and below. */
  #define RATIO_LIMIT  1.5
//  #define OPER_COMM_RATIO_AVG
// #define CACHE_WAY_BIAS_GATE
//STEER_DISPATCH_STAT//SHOULD BE DEFINED FOR  UNREADY_*
// #define STEER_DISPATCH_STAT
// #define UNREADY_RESTRICT
// #define UNREADY_BLOCK
// #define UNREADY_PRIVATE
//  #define UNREADY_COUNT_ONLY
//  #define UNREADY_COUNT
	/* UNREADY_COUNT_LIMIT is the total number of instructions 
	 * that can be fetched and dispatched without their
	 * operands being ready. */
  #define UNREADY_COUNT_LIMIT  100
// #define UNREADY_CHAIN_PRIVATE
	/* UNREADY_CHAIN_LIMIT is used in ruu_dispatch */
  #define UNREADY_CHAIN_LIMIT 100

//#define  INIT_N_WAY_CENTRAL
//#define THRD_PROFILE

#endif  // for N_WAY_STEER


 
 
 
 #ifdef   FIXED_N_WAY
  #ifdef MONOLETHIC_MACHINE
#define N_WAY  1
  #else  // for MONOLETHIC_MACHINE
   #ifdef CMC_N_WAY
#define N_WAY     CMC_N_WAY  //   CMC_N_WAY   could be changed
   #else  // for CMC_N_WAY
    #ifdef  CMP_N_WAY
#define N_WAY  1
    #else  // for CMP_N_WAY
     #ifdef N_WAY // we like to define N_WAY on the gcc command line
		// Well, not any more. Now its a config parameter
      #if N_WAY > CLUSTERS
       #error N_WAY > CLUSTERS does not make sense
      #endif
     #else // for N_WAY
#define N_WAY   2 //8   CLUSTERS   could be changed
     #endif // for N_WAY
    #endif  // for CMP_N_WAY
   #endif  // for CMC_N_WAY
  #endif  // for MONOLETHIC_MACHINE
 #endif  // for FIXED_N_WAY

  
 // #define N_WAY 1
  
 #ifdef  COUNT_LSQ
//#define N_WAY_PARTITION_LSQ
 #endif // for COUNT_LSQ

/* The Thread Profiles specify, for each thread in the 
 * test mix, how many clusters to be limited to.
 * See sim-outorder.c for the actual profiles. */
 #ifdef  THRD_PROFILE
//#define mix_8_f
//#define mix_8_i
//#define mix_8_if
//#define mix_8_m
//#define mix_4_i
//#define mix_4_f
#define mix1_4_m
//#define mix2_4_m
 #endif // for THRD_PROFILE
    #define NUMPARTITIONS   2

//////////////////////////////////////////////////////////////////////////////
/* different cache & LSQ configurations CENTRALIZED or DECENTRALIZED  */

/*  Line interleaved cache banks   */
#define LINE_INTERLEAVED

 #ifdef   LINE_INTERLEAVED
#define  THRD_WAY_CACHE
  #define WORD_INTERLEAVED 0
 #else
  #define WORD_INTERLEAVED 1
 #endif // for LINE_INTERLEAVED

//#define  CENTRALIZED_LSQ
 
 #ifndef  CENTRALIZED_LSQ
#define  DE_CENTRALIZED_LSQ
 #endif // for CENTRALIZED_LSQ

 #ifdef DE_CENTRALIZED_LSQ
/*Correction */
#define  SPLIT_LSQ  //  PERMINANT in all cases whether in Monolithic or Cluster
// this turn to be for the LSQ capacity and not specifically for spliting

  #ifdef  SPLIT_LSQ
#define  LSQ_FULL_MASK
#define  PARTITIONED_LSQ//each partiotion is connected to a group of cache banks
                        //each partiotion is attached to a certain cluster
    #ifndef NO_EXTRA_LSQ
#define  DISTRIBUTE_STORE
    #endif // for NO_EXTRA_LSQ
#define  PWR_DE_CENTRALIZED_LSQ
  #endif  // for SPLIT_LSQ
 #else  // for DE_CENTRALIZED_LSQ
/*this allows the number of banks not to match the number of CLUSTERS */
#define CENTRALIZED_CACHE_BANK
 #endif // for DE_CENTRALIZED_LSQ

/* A note about CENTRALIZED_CACHE_BANK and PARTITIONED_LSQ:
 * Currently, if both CENTRALIZED_CACHE_BANK and PARTITIONED_LSQ,
 * are defined, then the cache banks are not actually centralized.
 * As you can see in the below macros, PARTITIONED_LSQ just turns
 * on CENTRALIZED_CACHE_BANK because the source code needs it that
 * way. Currently CENTRALIZED_CACHE_BANK is used in sim-outorder.c. */

 #ifdef  PARTITIONED_LSQ
#define CENTRALIZED_CACHE_BANK
  #ifdef CMP_N_WAY
#define LSQ_PARTS    CLUSTERS
  #else  // for CMP_N_WAY
   #ifdef CMC_N_WAY
#define LSQ_PARTS    CMC_PARTS
   #else  // for CMC_N_WAY
#define LSQ_PARTS    4// 16 // 4 // CLUSTERS //CLUSTERS/2  // could be changed
   #endif // for CMC_N_WAY
  #endif // for CMP_N_WAY
 #else  // for PARTITIONED_LSQ
#define LSQ_PARTS    CLUSTERS
 #endif // for PARTITIONED_LSQ

 #ifdef CENTRALIZED_CACHE_BANK /* See above note about CENTRALIZED_CACHE_BANK. */
 #ifndef PARTITIONED_LSQ
#define STEER_MEM_NEIGHBOR  0	/* See explaination for this #define above. */
 #endif // for PARTITIONED_LSQ
 #endif // for CENTRALIZED_CACHE_BANK

// #ifdef  DISTRIBUTE_STORE
//#define  LSQ_FULL_MASK
// #endif

 #ifdef PWR_DE_CENTRALIZED_LSQ
#define  DE_CENTRALIZED_POWER_BUG
 #endif // for PWR_DE_CENTRALIZED_LSQ

 #ifdef   THRD_WAY_CACHE
#define   SEQUENTIAL_LINE
//#define   PRIVATE_CACHE
 #endif // for THRD_WAY_CACHE

 #ifdef   FIXED_N_WAY
//#define   ALL_CACHE_BANK
   #ifdef   STEER_CACHE_COUPLING
#define   N_WAY_CACHE  N_WAY
   #else  // for STEER_CACHE_COUPLING
    #ifdef  CMP_N_WAY
#define   N_WAY_CACHE  1
    #else  // for CMP_N_WAY
#define   N_WAY_CACHE  1 //4  // (must be <=res_membank)   // CLUSTERS
    #endif  // for CMP_N_WAY
   #endif  // for STEER_CACHE_COUPLING
// #else
//#define   N_WAY_CACHE  0
 #endif  // for FIXED_N_WAY

//#define N_WAY_CACHE_ADAPT
 #ifdef N_WAY_CACHE_ADAPT
//#define MIN_ADAPT  CLUSTERS/MAXTHREADS
#define   PRIVATE_CACHE
#define DOUBLE_ADAPT
#define ADAPT_RATE  1000
#define PER_FACTOR  0.25
#define MEMORYLESS
#define MEM_REF
//#define PER_THREAD
//#define MEM_REF_PER
#define MEM_REF_FACTOR  0.16
 #endif // for N_WAY_CACHE_ADAPT

////////////////////////////////////////////////////////////////////////////////

/*  count the commited instructions instead of the dispatched ones */
//#define COUNT_COMMIT

////////////////////////////////////////////////////////////////////////////////
/* Cluster communication definitions & its power considerations  */
 #ifndef  NO_COMMUNICATION
/* Implement the Remote access window structure */
#define  RAW
/* Implement the Remote access Buffer map table */
#define  RABMAP
   #ifdef  RABMAP
#define  RAWMAP
#define  RABCOMM
   #endif  // for RABMAP
/* Add extra 25% of the rename registers to compensate for the RAB */
//#define  RAB_RESOURCE
/*  Add power component for the intercluster communication */
#define  COMM_BUFF /* SHOULD BE MODIFIED WHEN USIG GRID INTERCONNECT */
/*  Add power component for the intercluster communication */
#define  COMM_LINK_PWR /* SHOULD BE MODIFIED WHEN USIG GRID INTERCONNECT */
 #endif  // for NO_COMMUNICATION

/* Increase the bandwidth of the clusters links to transfer more than register per cycle */
#define  INST_TRANS_RATE    // to allow transfer more than request per cycle
                            //  between the clusters through the links(hops)
 #ifdef INST_TRANS_RATE
 #define  LINK_WIDTH  2     // no. of registers served per cycle (default)
#define TRACE_REG
 #endif // for INST_TRANS_RATE

//#define UNIFORM_COMM
#define MEM_COMM_LAT   // modification to Ronz memory operations communication
 #ifdef MEM_COMM_LAT
#define LOAD_COMM_BUG   //memoryless ring_lat for load communication
                        // optimistic assumption I assume I hit in the cache so
			// I am accessing the comm_links right away after the cache
			//access
 #endif // for MEM_COMM_LAT
/* decode the inst. to find out which resources type (int/fp) will be consumed */
#define STRUCT_HAZARD   //PERMINANT

/* separate the power computation for each cluster (iq, fq, reg and alus)  */
#define  PWR_CLUSTER

 #ifdef  PWR_CLUSTER
/*  instead of divid ruu_issue_width over no. of clusters , make it per cluster for power.c */
 #define ISSUE_PER_CLUSTER
 #endif // for PWR_CLUSTER

////////////////////////////////////////////////////////////////////////////////
/*  STATISTICS  */
#define CLUSTER_CONFLICTS
#define DELAY_STAT
#define CACHE_MISS_STAT
#define STEER_STAT
#define COMM_LAT_STAT
//#define STEER_DISPATCH_STAT
#define CACHE_BANK_STAT
#define PERFORMANCE_STAT
 #define FETCH_DELAY_STAT
#define DISPATCH_STAT
#define ISSUE_STAT
#define CACHE_THRD_STAT
#define MEM_REF_STAT
//#define READY_ROB_STAT
//#define POWER_ACCESS_STAT
//#define CACHE_SET_STAT
#ifdef CACHE_SET_STAT
 #define MAXCACHESET  1024
#endif // for CACHE_SET_STAT


#ifdef  STEER_DISPATCH_STAT
	/* MAXDEV is maximum deviation */
#define MAXDEV  256
/* non-zero if all register operands are ready, update with MAX_IDEPS */
#define OPERANDS_READY(RS)                                              \
  ((RS)->idep_ready[0] && (RS)->idep_ready[1] && (RS)->idep_ready[2])
#define  UNREADY_DISPATCH_STAT
#endif // for STEER_DISPATCH_STAT

#define IC_STAT

////////////////////////////////////////////////////////////////////////////////
/* Modifications to Ronz implementations   */

/* configure the Rename Reg. only  */
#define REG_RENAME_CONFIGUARE

/* issue bandwidth = FUs (default) */
#define ISSUE_FU

/* issue bandwidth = FUs + memports */
#define ISSUE_FU_PORT

/*Flexibility for the ports and banks configurations res_memport* res_membank = total_ports  */
#define PORT_PER_BANK

/* Share all the functional units among all clusters*/
//#define  GENERIC_FU
// enable when  EXAMINE_FETCH  //
//#define  NO_QFILL_CONTENTION
//#define NO_BUS_CONTENTION
//#define NO_MISS_COUNT

// NO LONGER VALID
//#define  NO_CACHE_BANK
// NO LONGER VALID

 #ifndef FETCH_TARGET_TRACE
/* limit the fetching/cycle to instructions in only one block of the icache. */
#define FETCH_ONE_BLOCK
 #endif // for FETCH_TARGET_TRACE

/* implementing MSHR by putting a limit on the number of misses that can be  served
concurrently in both dl1 and dl2 caches */
// enable when  EXAMINE_FETCH  //
#define MSHR
 #ifdef  MSHR
//NOTE: We make an array of doubles that's this big, so keep it under 10000000 or so
 #define MSHRSIZE 10000000 //1024    //10240 //1048576
 #define MSHR_DIST //determines whether to allocate the above array (if defined) or not bother keeping track (if not defined)
 #endif // for MSHR

#define PWR_CLOCK_DIST
#define PWR_WAKEUP_SELECT               //Power for select-wakeup logic
#define PWR_ACCESS   // correct the wrong updates for the structures access counters
/*COMM_BUFF power should be reviewed*/
//#define PWR_ALU/* ALU power should be reviewed*/

#define PWR__SMT_RR        // separate rename and rob power calculation per thread
#define PWR__SMT_MULTIRAT  // separate RAT per thread  to allow clock gating the inactive tables

#define PWR_WAKE               // Wakeup and selection access,
                               // ready/unready counters
 #ifdef  PWR_WAKE
//#define WAKE_POWER               //and power calculation Wael
 #endif // for PWR_WAKE

#define ICACHEBANKS 8          // IL1 Memory Banks. used in thread-fetching, and power calc.

 #ifdef  PORT_PER_BANK
#define PWR_BANKS              // For new power calculation with memory banks (DL1, IL1, DTLB, ITLB)
 #endif // for PORT_PER_BANK

//#define PWR_REG                // modification for the register files, and results bus models.

#define ISU_N2                 // Issue Width doesn't have to be a power of 2

/* presice timing for the cache port contention in an event driven based model */
//#define EVENT_CACHE_UPDATE
#ifdef  EVENT_CACHE_UPDATE
// #define APPROXIMATE_LAT
// #define ACURATE_LAT
// #define MOST_ACURATE_LAT
#endif // for EVENT_CACHE_UPDATE

#define NO_RONZ_PRINT

////////////////////////////////////////////////////////////////////////////////
/*Add HotLeakage to the simulator */
/*REVIEW BUS CONTENTION IN CACHE.C*/
/*REVIEW BUS CONTENTION IN CACHE.C*/
/*REVIEW BUS CONTENTION IN CACHE.C*/
//#define HOTLEAKAGE
/*REVIEW BUS CONTENTION IN CACHE.C*/
/*REVIEW BUS CONTENTION IN CACHE.C*/
/*REVIEW BUS CONTENTION IN CACHE.C*/


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#define  DISPATCH_COMM_LATENCY_BUG

#define  ERRNO_BUG

#define  MISFETCH_BUG

/* FIX_BUG :- do not actually replace the cache block if it belongs to another pending
thread but return the block latency only. This solution is valid in the icache case  because
the thread is blocked from further access to the icache until the latency time passed and
after it passes a second access to the icache performed which will perform the replacement
code that is skipped the first time. But for dcache it is invalid because for ld/st the
thread continue issue (ld) and commit (st) and use the latency to know the completion time
of the instruction and do not access the cache again for the same instruction so the block
information will not be updated. */
//#define FIX_BUG

/*FIX1_BUG :- Just skip the icache access twic. For the instruction that miss in the icache
block update the icache block as usual for any cache block that miss including replacement
but deal with icahce miss as dcache miss and not to access the icache again after the thread
resolve from the miss assume the instruction will hit without accually accessing the icache.
This can be done either to the instruction that miss only or all the fetching cycle of that
thread after recovery from the miss.*/
//#define FIX1_BUG

/*FIX2_BUG :- Just skip the icache access twic. For a complete cycle of fetching for the
thread that miss last time in icache. Regardless of the instructions addresses or cache
blocks that are accessed by these instructions.
*/
#define FIX2_BUG

/* For accessing L2 cache and miss on L2 if the time of access is more advanced than the
sim_cycle we have to execlude an equevilant number of cache ports contention between
L2 and main memory when computing the latency of the L2 block */
#define FIX_LAT_BUG

/* This is a performance bug. I did not prevent the LD instructions from being ready
while there is earlier STA unknown from the same thread, Also this directive will be
used in enhancing the performance of lsq_refresh() function */
#define FIX_LSQ_REFRESH_BUG

//#define FIX_8_CLUSTER_BUG

//#define REMOVE_MY_CODE


//#define CYCLE_JUMP
 #ifdef   CYCLE_JUMP
 #define LATEST_CYCLE_JUMP
 #define SKIP_JUMP_LIMIT  500  // must be  > 5
 #endif // for CYCLE_JUMP

//#define CHECKS

//#define TEST_MAP

/* for test something in SMT code  */
//#define SMT_TEST   //  after check the functionality remove the else
//#define PC_TRACE


//#define TEST_BUG

//#define FIX_OUTPUT
//#define TOKENB
#endif // for ?

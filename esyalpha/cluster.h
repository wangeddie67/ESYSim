/*
 * $Log: cluster.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:11  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
 * dir structure
 *
 * Revision 1.3  2004/09/23  21:12:04  etan
 * *** empty log message ***
 *
 * Revision 1.2  2004/09/23  03:20:51  etan
 * add ifndef NO_COMMUNCATION to resolve LD_FACTOR defined twice.
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.10  2004/07/05 14:04:38  ninelson
 * TIMEWINDOW update
 *
 * Revision 1.9  2004/06/30 15:28:56  ninelson
 * Longer error message.
 * Changed long long constants to include ULL at end.
 *
 * Revision 1.8  2004/06/23 21:30:51  grbriggs
 * Fix bug in macro usage.
 *
 * Revision 1.7  2004/06/23 16:37:16  ninelson
 * more minor from ali
 *
 * Revision 1.6  2004/06/21 21:13:32  grbriggs
 * Add PARTITIONED_LSQ and CENTRALIZED_CACHE_BANK from Ali.
 *
 * Revision 1.5  2004/06/15 21:08:22  ninelson
 * Updated defines for CACHE_REFILL_LAT
 *
 * Revision 1.4  2004/06/09 18:15:59  grbriggs
 * Fix compiler warning about "STEER_2NONREADY" and "STEER_CRITICAL" being redefined.
 *
 */

/*  for defining SMT   */
#include "smt.h"


#define FETCHLIMIT 1024       /* number of instructions that can be fetched in
			      a single cycle by a single thread. */
#define FBLOCKMASK ~63     /* ICACHE line size - 1 */
#define THREADFETCHLIMIT 8 /* number of threads that can fetch in a
			      single cycle */
#define TOTALFETCHLIMIT 1024  /* number of total instructions that can
			      be fetched in a single cycle */

#define ICACHEBANKS 8

#define IQSIZE  1048  //(240+CLUSTERS)	/* Integer issue queue size -- must be less than
		//	   	   the IPREGSIZE to be meaningful */
#define FQSIZE  1048  //(240+CLUSTERS)	/* FP issue queue size */

#ifdef RAW
#define RAWSIZE  IQSIZE
#endif

#define IPREGSIZE  4000 //(480+3*CLUSTERS)
#define FPREGSIZE  4000 //(480+3*CLUSTERS)
#define EXTRA_REG_RENAME 30

#ifdef PORT_PER_BANK
#define MAXCACHEPORTS 512     /* upper bound to the number of ports*/
#else
#define CACHEPORTS 32	/* Also change this in resource.c */
#define RES_CACHEPORTS 32  /* This should be equal to the number of
			     actual ports, so you can fix the resource
			     number for the ALUs, etc. */
#endif

#ifdef  PARTITIONED_LSQ  // each partiotion is connected to a group of cache banks
#define MAXLSQPARTS 32     /* upper bound to the number of LSQ partitions*/
#endif

#define WARMUP 0		/* Num of instructions with detailed
				   simulation before actually starting */

#ifdef  CENTRALIZED_LSQ
#define CENTRAL_CACHE 1
#define CENTRALCACHELAT 1
#define CACHELOC 0
#else
#define CENTRAL_CACHE 0
#define CENTRALCACHELAT 0
#define CACHELOC 0
#endif

#define GRID 0

#define DL1WRB	16		/* The sizes of the writeback buffers */
#define IL1WRB	16
#define DL2WRB	16
#define DL1PIPE	4		/* The bus delay to the next level */
#define IL1PIPE	4
#define DL2PIPE	16

#define TIMEWINDOW 5000

#define LEV1SIZE 1024
#define LEV2SIZE 4096


//#define MINADDR 4799987712
//#define MAXADDR 5400000000
#define MINADDR 4799987712ULL
#define MAXADDR 5400000000ULL
#define BINSIZE 16384

#define TABLE2SIZE 4096
#define TABLESIZE 4096

#define ITABLESIZE 16384

#define XORSIZE 12

#define SORT 0

#define FREE_REG_THRESH (IPREGSIZE)

//#define CLUSTERS 16	/* Also change in resource.c */
#define ROWCOUNT 4      /* for GRID only */


#ifdef DE_CENTRALIZED_LSQ
#define STOREDELAY 1
#define BANKPRED 1
#define BANKMISPREDLAT 1
#define BANKP_SPEC 0
#define ST_ADDR_COMM 1
#define ST_RING_BW 1
#else
#define STOREDELAY 0
#define BANKPRED 0
#define BANKMISPREDLAT 0
#define BANKP_SPEC 0
#define ST_ADDR_COMM 0
#define ST_RING_BW 0
#endif

#define IFACTOR 4
//#define RLOAD_MIN 0
#define RFACTOR 1

#ifndef    STEER_FU
#define DEP_FACTOR CLUSTERS  /* Made this way for now coz I'm only dealing with
				4x, 8x, and 16x. */
#else
#define DEP_FACTOR 0
#endif

#ifdef DE_CENTRALIZED_LSQ
/* 092204 ejt: add ifndef NO_COMMUNCATION to resolve LD_FACTOR defined twice */
#ifndef NO_COMMUNICATION
#define LD_FACTOR  (4*DEP_FACTOR)
#endif // NO_COMMUNICATION
#else
/* 092204 ejt: add ifndef NO_COMMUNCATION to resolve LD_FACTOR defined twice */
#ifndef NO_COMMUNICATION
#define LD_FACTOR  (0*DEP_FACTOR)
#endif // NO_COMMUNICATION
#endif // DE_CENTRALIZED_LSQ

#ifdef  STEER_MEM_NEIGHBOR
#define LD_PROXIMITY 1
#else
#define LD_PROXIMITY 0
#endif
#define LDPROX_FACTOR 256

#ifdef CENTRALIZED_CACHE_BANK
#ifndef PARTITIONED_LSQ /* See note in smt.h if you are wondering why this is here */
#define LD_FACTOR  0
#define LD_PROXIMITY 0
#define LDPROX_FACTOR 0
#endif // PARTITIONED_LSQ
#endif // CENTRALIZED_CACHE_BANK

#ifdef IGNORE_MEM_COMM_LATENCY
#define LD_FACTOR  0
#define LD_PROXIMITY 0
#endif

#ifdef IGNORE_ALL_COMM_LATENCY
#define STEER_2NONREADY 0
#define STEER_CRITICAL 0
#define LD_FACTOR  0
#define LD_PROXIMITY 0
#endif

#ifdef  UNIFORM_COMM
#define RING_IC 0
#define RING_BW 0
#else
#define RING_IC 1
#define RING_BW 1
#endif

#ifdef  IGNORE_ALL_COMM_LATENCY
#define IGNORE_COMM_LATENCY 1  /* ignore wakeup delay for the dependent instructions +
                                  ignore dispatch delay from cluster0 to the steering cluster */
#else
#define IGNORE_COMM_LATENCY 0
#endif

#define MAGICN 2000

#define NUMPHASES CLUSTERS
#define MIN_INSN_INT 10000
#define STOP_EXPL_THRESH -5

#define ILPTAGCHECK 1
#define DISTILP 120
#define DISTILPTHRESH 30
#define SAMPLETHRESH 10
#define BR_INT 8
#define ILPCLEARINT 10000000

#define STATPRINT 0

#define IQBUF 0  /* Ali: I don't see any acceptable reason for putting "IQBUF 1"
                         ,I think it has to remain undefined */

#define CHATTER 0
#define TOL_COUNT 0

#define REGFREE 0
#define FREE_CURR_REGS 0
#define FREE_INTERVAL 1
#define DISPDELAY 0  /* Ali: I don't see any acceptable reason for putting "IQBUF 1"
                        ,I think it has to remain undefined */

#define READY_COMM 1 // add communication delay for the ready operands forwarded from other cluster
#define HOLY_REGS 0
#define NUM_H_REGS 3
#define COPY_THRESH 50

#define FIRST_FIT 0
#define FIRST_FIT_ENFORCE 0
#define FIRST_VAL 10000

#define STEER_ENFORCE 0	/* When this is 1, make anywilldo 0 */
#define MOD_N 0
#define MOD_N_N 8

#define FIRST_MOD 0
#define GROUPSIZE 4
#define MOD_N_T	2

#define ANYWILLDO 1		/* If anywilldo is 1, neighbor_seek will have
				   to be (CLUSTERS/2). Else, neighbor_seek
				   can be set to anything (its not used). */
#define NEIGHBOR_SEEK (CLUSTERS/2)

#define PICKMIDDLE 0	/* If you are using fewer clusters and a broken ring,
			   instructions should be steered towards the middle.*/
#define MIDTHRESH 16

//#define HOPTIME 1
//#define COMM_LATENCY ((CLUSTERS*HOPTIME)/2)  /* The furthest two points on
//						the chip. When a ring
//						interconnect is used, each
//						hop is a cycle. */
#define FE_LAT  ((CLUSTERS*HOPTIME)/2)
#ifdef  IGNORE_ALL_COMM_LATENCY
#define MPRED_COMM_LAT 0
#define CACHE_REFILL_LAT 0
#else
 #ifdef  IGNORE_MEM_COMM_LATENCY
#define CACHE_REFILL_LAT 0
 #else
#define CACHE_REFILL_LAT 1
 #endif
#define MPRED_COMM_LAT 1   /* to consider the communication delay for mispredicted branch
                              in cluster != 0 far from dispatch cluster(0)  */
#endif

/* Values of DEP_FACTOR and ILOAD_MIN and RLOAD_MIN for 16 clusters is 16,1,0.
   For 8cl/8x is 16/8,1,0.
   For 4cl/4x is 16/4,1,0.
   For 2cl/2x is 16/4,1,0. */

/* 092304 ejt: add ifndef NO_COMMUNCATION to resolve STEER_2NOREADY and
   STEER_CRITICAL defined twice */
#ifndef NO_COMMUNICATION
#ifndef STEER_FU
#ifndef STEER_OPRANDS
/* We are guessing STEER_2NONREADY pushes the new intstructions to the cluster that is producing its unready results */
#define STEER_2NONREADY 1
#define STEER_CRITICAL 1
#endif
#else
#define STEER_2NONREADY 0
#define STEER_CRITICAL 0
#endif

#ifdef  STEER_OPRANDS
#define STEER_2NONREADY 0
#define STEER_CRITICAL 0
#endif
#endif //NO_COMMUNITCATION

#ifdef   STEER_RES1
#define ILOAD_MIN 1
#define RLOAD_MIN 1
#else
#define ILOAD_MIN 0
#define RLOAD_MIN 0
#endif

struct listelt {
  int preg;
  struct listelt *next;
  struct listelt *prev;
};


#ifdef INST_TRANS_RATE
extern double *link_width_dist;
#endif
extern int ring1[CLUSTERS][TIMEWINDOW];
extern int ring2[CLUSTERS][TIMEWINDOW];
extern int ring3[CLUSTERS][TIMEWINDOW];
extern int ring4[CLUSTERS][TIMEWINDOW];
#ifdef TRACE_REG
extern int *reg1[CLUSTERS][TIMEWINDOW];
extern int *reg2[CLUSTERS][TIMEWINDOW];
#else
extern int reg1[CLUSTERS][TIMEWINDOW];
extern int reg2[CLUSTERS][TIMEWINDOW];
#endif
extern int reg3[CLUSTERS][TIMEWINDOW];
extern int reg4[CLUSTERS][TIMEWINDOW];

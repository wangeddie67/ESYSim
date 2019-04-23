/* I inclued this copyright since we're using Cacti for some stuff */

/*------------------------------------------------------------
 *  Copyright 1994 Digital Equipment Corporation and Steve Wilton
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and hereby grant back to Digital a non-exclusive, unrestricted, royalty-
 * free right and license under any changes, enhancements or extensions
 * made to the core functions of the software, including but not limited to
 * those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to Digital any
 * such changes, enhancements or extensions that they make and inform Digital
 * of noteworthy uses of this software.  Correspondence should be provided
 * to Digital at:
 *
 *                       Director of Licensing
 *                       Western Research Laboratory
 *                       Digital Equipment Corporation
 *                       100 Hamilton Avenue
 *                       Palo Alto, California  94301
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/

/*
 * $Log: power.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:04  xue
 * CMP_NETWORK
 *
 * Revision 1.2  2009/03/05 16:58:23  garg
 * Updated version of the CMP network simulator.
 *
 * Revision 1.23  2005/07/06 02:34:26  etan
 * *** empty log message ***
 *
 * Revision 1.21  2005/03/09 19:56:41  rashid
 * PCB block access count added for better power model
 *
 * Revision 1.20  2005/03/04 01:16:51  etan
 * Clean up typo errors.
 *
 * Revision 1.19  2005/02/02 22:48:45  rashid
 * split dl1 and il1, can find change by 020205mwr
 *
 * Revision 1.18  2005/01/26 22:48:45  rashid
 * *** empty log message ***
 *
 * Revision 1.19  2005/01/26 15:55:00  etan
 * Add new power computations for 21264 floorplan accuracy.
 *
 * Revision 1.18  2004/12/07 21:12:22  etan
 * Included dynamic power into hotspot temperature computations
 *
 *
 * Revision 3.3  2004/07/01 etan
 * added d_flip_flop_power and 2_to_1_mux_power
 * added simulation statistics for instruction duplication power
 * 
 * Revision 3.2  2004/06/29 etan
 * added d_flip_flop_cap and 2_to_1_mux_cap
 * 
 * Revision 3.1  2004/06/26 etan
 * added commit_compare_cap
 *
 * Revision 3.0  2004/06/25 etan
 * added commit_compare_power
 *
 * Revision 2.1  2004/06/22 etan
 * added simulation statistics for instruction compare power
 * modified statistics to use macros
 *
 * Revision 2.0  2004/06/14 etan
 * added simulation statistics for instruction compare at ruu_commit
 *
 * Revision 1.0  2004/03/10 etan
 * removed unused variables
 * modified gcname+gcc-1 to gcname[gcc-1] to correct for pointer type
 *
 */

#include "headers.h"
/*
#include <math.h>
//#include "power.h"
#include "machine.h"
#include "cache.h"
#include <assert.h>
#include "context.h"
//power.h is included in context.h
*/

#define str(s) #s
#define xstr(s) str(s)

//#define SensePowerfactor (Mhz)*(Vdd/2)*(Vdd/2)
#define Sense2Powerfactor (Mhz)*(2*.3+.1*Vdd)

//#define Powerfactor (Mhz)*Vdd*Vdd

#define LowSwingPowerfactor (Mhz)*.2*.2
/* set scale for crossover (vdd->gnd) currents */
//double crossover_scaling = 1.2;
/* set non-ideal turnoff percentage */
//double turnoff_factor = 0.1;

#define MSCALE (LSCALE * .624 / .2250)

/* Decide which to calculate: CC1, CC2, and/or CC3 */
//#define CC1
//#define CC2
#define CC3

#ifdef CC1
#define IFCC1(x) x
#else
#define IFCC1(x)
#endif
#ifdef CC2
#define IFCC2(x) x
#else
#define IFCC2(x)
#endif
#ifdef CC3
#define IFCC3(x) x
#else
#define IFCC3(x)
#endif

extern int ruu_inorder_issue;

double vdd_reduction;
double Powerfactor;
double SensePowerfactor;

double gc[50];
char gcname[50][30];
int gcc;

double constant_factor;

double last_ifq_period=1.0;
double last_iiq_period=1.0;
double last_fiq_period=1.0;

double piperegg, clineg, globbufg, clockp;


double POWER_SCALE;//    (GEN_POWER_SCALE * NORMALIZE_SCALE * Powerfactor)
double I_ADD;//          ((.37 - .091) * POWER_SCALE)
double I_ADD32;//        (((.37 - .091)/2)*POWER_SCALE)
double I_MULT16;//       ((.31-.095)*POWER_SCALE)
double I_SHIFT;//        ((.21-.089)*POWER_SCALE)
double I_LOGIC;//        ((.04-.015)*POWER_SCALE)
double F_ADD;//          ((1.307-.452)*POWER_SCALE)
double F_MULT;//         ((1.307-.452)*POWER_SCALE)

double I_ADD_CLOCK;//    (.091*POWER_SCALE)
double I_MULT_CLOCK;//   (.095*POWER_SCALE)
double I_SHIFT_CLOCK;//  (.089*POWER_SCALE)
double I_LOGIC_CLOCK;//  (.015*POWER_SCALE)
double F_ADD_CLOCK;//    (.452*POWER_SCALE)
double F_MULT_CLOCK;//   (.452*POWER_SCALE)
double Period;





/* ejt: macros used in the simulator are defined here */

#ifdef INPUT_OUTPUT_BUFFER		      
#define TOTAL_CYCLE_POWER	(rename_power + iqram_power + fqram_power + clock1_power + clock2_power + clock3_power + rob1_power + rob2_power + bpred_power + iwakeup_power + fwakeup_power + iselection_power + fselection_power + lsq_power + iregfile_power + fregfile_power + icache_power + iresultbus_power + fresultbus_power + alu1_power + alu2_power + falu1_power + falu2_power + dcache_power + dcache2_power + duplication_power + compare_power + input_buf_power + output_buf_power)
#else
#define TOTAL_CYCLE_POWER	(rename_power + iqram_power + fqram_power + clock1_power + clock2_power + clock3_power + rob1_power + rob2_power + bpred_power + iwakeup_power + fwakeup_power + iselection_power + fselection_power + lsq_power + iregfile_power + fregfile_power + icache_power + iresultbus_power + fresultbus_power + alu1_power + alu2_power + falu1_power + falu2_power + dcache_power + dcache2_power + duplication_power + compare_power)	      
#endif					       

#ifdef INPUT_OUTPUT_BUFFER		      
#define TOTAL_CYCLE_POWER_CC3	(iqram_power_cc3 + fqram_power_cc3 + clock1_power_cc3 + clock2_power_cc3 + clock3_power_cc3 + rob1_power_cc3 + rob2_power_cc3 + rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + iwakeup_power_cc3 + fwakeup_power_cc3 + iselection_power_cc3 + fselection_power_cc3 + iregfile_power_cc3 + fregfile_power_cc3 + icache_power_cc3 + iresultbus_power_cc3 + fresultbus_power_cc3 + alu1_power_cc3 + alu2_power_cc3 + falu1_power_cc3 + falu2_power_cc3 + dcache_power_cc3 + dcache2_power_cc3 + duplication_power_cc3 + compare_power_cc3 + input_buf_power_cc3 + output_buf_power_cc3)
#else
#define TOTAL_CYCLE_POWER_CC3	(iqram_power_cc3 + fqram_power_cc3 + clock1_power_cc3 + clock2_power_cc3 + clock3_power_cc3 + rob1_power_cc3 + rob2_power_cc3 + rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + iwakeup_power_cc3 + fwakeup_power_cc3 + iselection_power_cc3 + fselection_power_cc3 + iregfile_power_cc3 + fregfile_power_cc3 + icache_power_cc3 + iresultbus_power_cc3 + fresultbus_power_cc3 + alu1_power_cc3 + alu2_power_cc3 + falu1_power_cc3 + falu2_power_cc3 + dcache_power_cc3 + dcache2_power_cc3 + duplication_power_cc3 + compare_power_cc3)
#endif					     	

/*----------------------------------------------------------------------*/


char buffer1[1024];
char buffer2[4096];

/* static power model results */
//power_result_type power;

double sim_cycleg;

void sim_cyc(unsigned long simc)
{
  sim_cycleg = (double)simc;
}

int pow2(int x) {
  return((int)pow(2.0,(double)x));
}

double logfour(x)
     double x;
{
  if (x<=0) fprintf(stderr,"%e\n",x);
  return( (double) (log(x)/log(4.0)) );
}

/* safer pop count to validate the fast algorithm */
int pop_count_slow(quad_t bits)
{
  int count = 0;
  quad_t tmpbits = bits;
  while (tmpbits) {
    if (tmpbits & 1) ++count;
    tmpbits >>= 1;
  }
  return count;
}

/* fast pop count */
int pop_count(quad_t bits)
{
#define T unsigned long long
#define ONES ((T)(-1))
#define TWO(k) ((T)1 << (k))
#define CYCL(k) (ONES/(1 + (TWO(TWO(k)))))
#define BSUM(x,k) ((x)+=(x) >> TWO(k), (x) &= CYCL(k))
  quad_t x = bits;
  x = (x & CYCL(0)) + ((x>>TWO(0)) & CYCL(0));
  x = (x & CYCL(1)) + ((x>>TWO(1)) & CYCL(1));
  BSUM(x,2);
  BSUM(x,3);
  BSUM(x,4);
  BSUM(x,5);
  return x;
}


int opcode_length = 8;
int inst_length = 32;


extern int OUTSTANDING_STORE_NUM;
extern int PCB_BLK_SIZE;
extern int BRANCH_Q_NUM, INORDER_CHECKER;
extern int CHECKER_NUMBER;

extern int disable_ray;


extern int ruu_decode_width;
extern int ruu_issue_width;
extern int ruu_commit_width;
extern int RUU_size;
extern int LSQ_size;
extern int data_width;
extern int res_ialu;
extern int res_imult;
extern int res_fpalu;
extern int res_fpmult;
extern int res_memport;

extern int IPREG_size;
extern int FPREG_size;
extern int IIQ_size;
extern int FIQ_size;

int nvreg_width;
int npreg_width;

extern int bimod_config[];

extern struct cache_t *cache_dl1[CLUSTERS];
extern struct cache_t *cache_il1[CLUSTERS];
extern struct cache_t *cache_dl2;

extern struct cache_t *dtlb;
extern struct cache_t *itlb;

/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
extern int twolev_config[];

/* combining predictor config (<meta_table_size> */
extern int comb_config[];

/* return address stack (RAS) size */
extern int ras_size;

/* BTB predictor config (<num_sets> <associativity>) */
extern int btb_config[];

double global_clockcap;

static double rename_power=0;
static double iqram_power=0;
static double fqram_power=0;
static double rob1_power=0;
static double rob2_power=0;
static double clock1_power=0;
static double clock2_power=0;
static double clock3_power=0;
static double bpred_power=0;
static double iwakeup_power=0;
static double fwakeup_power=0;
static double iselection_power=0;
static double fselection_power=0;
static double lsq_power=0;
static double iregfile_power=0;
static double fregfile_power=0;
static double icache_power=0;
static double dcache_power=0;
#ifdef INPUT_OUTPUT_BUFFER
static double input_buf_power=0;
static double output_buf_power=0;
#endif
static double dcache2_power=0;
static double alu1_power=0;
static double alu2_power=0;
static double falu1_power=0;
static double falu2_power=0;
static double iresultbus_power=0;
static double fresultbus_power=0;
static double clock_power=0;
/* ejt: fault recovery power added for Wattch */
static double duplication_power=0;
static double compare_power=0;

/*110504mwr: new variables added to calculate the per processor power*/
static double RENAME_power_cc3[CLUSTERS];
static double IQRAM_power_cc3[CLUSTERS];
static double FQRAM_power_cc3[CLUSTERS];
static double ROB1_power_cc3[CLUSTERS];
static double ROB2_power_cc3[CLUSTERS];
static double CLOCK1_power_cc3[CLUSTERS];
static double CLOCK2_power_cc3[CLUSTERS];
static double CLOCK3_power_cc3[CLUSTERS];
static double BPRED_power_cc3[CLUSTERS];
static double IWAKEUP_power_cc3[CLUSTERS];
static double FWAKEUP_power_cc3[CLUSTERS];
static double ISELECTION_power_cc3[CLUSTERS];
static double FSELECTION_power_cc3[CLUSTERS];
static double LSQ_power_cc3[CLUSTERS];
static double IREGFILE_power_cc3[CLUSTERS];
static double FREGFILE_power_cc3[CLUSTERS];
static double ICACHE_power_cc3[CLUSTERS];
static double DCACHE_power_cc3[CLUSTERS];
#ifdef INPUT_OUTPUT_BUFFER
static double input_buf_power_cc3[CLUSTERS];
static double output_buf_power_cc3[CLUSTERS];
#endif
static double DCACHE2_power_cc3[CLUSTERS];
static double ALU1_power_cc3[CLUSTERS];
static double ALU2_power_cc3[CLUSTERS];
static double FALU1_power_cc3[CLUSTERS];
static double FALU2_power_cc3[CLUSTERS];
static double IRESULTBUS_power_cc3[CLUSTERS];
static double FRESULTBUS_power_cc3[CLUSTERS];

/* ejt: fault recovery power added for Wattch */
static double duplication_power_cc3=0;
static double compare_power_cc3=0;

static double TOTAL_power_cc3[CLUSTERS];

static double total_cycle_power;
static double total_cycle_power_cc3;
static double max_cycle_power_cc3 = 0.0;

extern counter_t rename_access;
extern counter_t iqram_access;
extern counter_t fqram_access;
extern counter_t rob1_access;
extern counter_t rob2_access;
extern counter_t bpred_access;
extern counter_t iwakeup_access;
extern counter_t fwakeup_access;
extern counter_t iselection_access;
extern counter_t fselection_access;
extern counter_t lsq_access;
extern counter_t iregfile_access;
extern counter_t fregfile_access;
extern counter_t icache_access;
extern counter_t dcache_access;
//extern counter_t input_buf_access;
//extern counter_t output_buf_access;
extern counter_t dcache2_access;
extern counter_t ialu1_access;
extern counter_t ialu2_access;
extern counter_t falu1_access;
extern counter_t falu2_access;
extern counter_t iresultbus_access;
extern counter_t fresultbus_access;
extern counter_t lsq_preg_access;
extern counter_t lsq_wakeup_access;
extern counter_t lsq_store_data_access;
/* ejt: fault recovery counters added for Wattch */
extern counter_t duplication_access;
extern counter_t compare_access;

extern counter_t lsq_total_pop_count_cycle;
extern counter_t lsq_num_pop_count_cycle;
extern counter_t iregfile_total_pop_count_cycle;
extern counter_t iregfile_num_pop_count_cycle;
extern counter_t fregfile_total_pop_count_cycle;
extern counter_t fregfile_num_pop_count_cycle;
extern counter_t iresultbus_total_pop_count_cycle;
extern counter_t iresultbus_num_pop_count_cycle;
extern counter_t fresultbus_total_pop_count_cycle;
extern counter_t fresultbus_num_pop_count_cycle;

static counter_t total_rename_access=0;
static counter_t total_iqram_access=0;
static counter_t total_fqram_access=0;
static counter_t total_rob1_access=0;
static counter_t total_rob2_access=0;
static counter_t total_bpred_access=0;
static counter_t total_iwakeup_access=0;
static counter_t total_fwakeup_access=0;
static counter_t total_iselection_access=0;
static counter_t total_fselection_access=0;
static counter_t total_lsq_access=0;
static counter_t total_iregfile_access=0;
static counter_t total_fregfile_access=0;
static counter_t total_icache_access=0;
static counter_t total_dcache_access=0;
#ifdef INPUT_OUTPUT_BUFFER
static counter_t total_input_buf_access=0;
static counter_t total_output_buf_access=0;
#endif
static counter_t total_dcache2_access=0;
static counter_t total_alu1_access=0;
static counter_t total_alu2_access=0;
static counter_t total_falu1_access=0;
static counter_t total_falu2_access=0;
static counter_t total_iresultbus_access=0;
static counter_t total_fresultbus_access=0;
/* ejt: fault recovery counters added for Wattch */
static counter_t total_duplication_access;
static counter_t total_compare_access=0;

static counter_t max_rename_access;
static counter_t max_iqram_access;
static counter_t max_fqram_access;
static counter_t max_rob1_access;
static counter_t max_rob2_access;
static counter_t max_bpred_access;
static counter_t max_iwakeup_access;
static counter_t max_fwakeup_access;
static counter_t max_iselection_access;
static counter_t max_fselection_access;
static counter_t max_lsq_access;
static counter_t max_iregfile_access;
static counter_t max_fregfile_access;
static counter_t max_icache_access;
static counter_t max_dcache_access;
#ifdef INPUT_OUTPUT_BUFFER
static counter_t max_input_buf_access;
static counter_t max_output_buf_access;
#endif
static counter_t max_dcache2_access;
static counter_t max_alu1_access;
static counter_t max_alu2_access;
static counter_t max_falu1_access;
static counter_t max_falu2_access;
static counter_t max_iresultbus_access;
static counter_t max_fresultbus_access;
/* ejt: fault recovery counters added for Wattch */
static counter_t max_duplication_access;
static counter_t max_compare_access=0;

void power_clear()
{
	int lcount;
	
	/* if the variable is set to equal 0, then after warmup the variable will be
	   initialized to 0 */
	rename_power=0;
	iqram_power=0;
	fqram_power=0;
	rob1_power=0;
	rob2_power=0;
	clock1_power=0;
	clock2_power=0;
	clock3_power=0;
	bpred_power=0;
	iwakeup_power=0;
	fwakeup_power=0;
	iselection_power=0;
	fselection_power=0;
	lsq_power=0;
	iregfile_power=0;
	fregfile_power=0;
	icache_power=0;
	dcache_power=0;
#ifdef INPUT_OUTPUT_BUFFER
	input_buf_power=0;
	output_buf_power=0;
#endif
	dcache2_power=0;
	alu1_power=0;
	alu2_power=0;
	falu1_power=0;
	falu2_power=0;
	iresultbus_power=0;
	fresultbus_power=0;
	clock_power=0;
	/* ejt: fault recovery power added for Wattch */
	duplication_power=0;
	compare_power=0;
	
	/* ejt: fault recovery power added for Wattch */
	duplication_power_cc3=0;
	compare_power_cc3=0;
	
	max_cycle_power_cc3 = 0.0;
	
	total_rename_access=0;
	total_iqram_access=0;
	total_fqram_access=0;
	total_rob1_access=0;
	total_rob2_access=0;
	total_bpred_access=0;
	total_iwakeup_access=0;
	total_fwakeup_access=0;
	total_iselection_access=0;
	total_fselection_access=0;
	total_lsq_access=0;
	total_iregfile_access=0;
	total_fregfile_access=0;
	total_icache_access=0;
	total_dcache_access=0;
#ifdef INPUT_OUTPUT_BUFFER
	total_input_buf_access=0;
	total_output_buf_access=0;
#endif
	total_dcache2_access=0;
	total_alu1_access=0;
	total_alu2_access=0;
	total_falu1_access=0;
	total_falu2_access=0;
	total_iresultbus_access=0;
	total_fresultbus_access=0;
	/* ejt: fault recovery counters added for Wattch */
	total_duplication_access=0;
	total_compare_access=0;
	
	max_rename_access=0;
	max_iqram_access=0;
	max_fqram_access=0;
	max_rob1_access=0;
	max_rob2_access=0;
	max_bpred_access=0;
	max_iwakeup_access=0;
	max_fwakeup_access=0;
	max_iselection_access=0;
	max_fselection_access=0;
	max_lsq_access=0;
	max_iregfile_access=0;
	max_fregfile_access=0;
	max_icache_access=0;
	max_dcache_access=0;
#ifdef INPUT_OUTPUT_BUFFER
	max_input_buf_access=0;
	max_output_buf_access=0;
#endif
	max_dcache2_access=0;
	max_alu1_access=0;
	max_alu2_access=0;
	max_falu1_access=0;
	max_falu2_access=0;
	max_iresultbus_access=0;
	max_fresultbus_access=0;
	/* ejt: fault recovery counters added for Wattch */
	max_duplication_access=0;
	max_compare_access=0;
	for(lcount =0; lcount<numcontexts;lcount++)
	{
		RENAME_power_cc3[lcount] =  0;
		IQRAM_power_cc3[lcount] =  0;
		FQRAM_power_cc3[lcount] =  0;
		ROB1_power_cc3[lcount] =  0;
		ROB2_power_cc3[lcount] =  0;
		CLOCK1_power_cc3[lcount] =  0;
		CLOCK2_power_cc3[lcount] =  0;
		CLOCK3_power_cc3[lcount] =  0;
		BPRED_power_cc3[lcount] =  0;
		IWAKEUP_power_cc3[lcount] =  0;
		FWAKEUP_power_cc3[lcount] =  0;
		ISELECTION_power_cc3[lcount] =  0;
		FSELECTION_power_cc3[lcount] =  0;
		LSQ_power_cc3[lcount] =  0;
		IREGFILE_power_cc3[lcount] =  0;
		FREGFILE_power_cc3[lcount] =  0;
		ICACHE_power_cc3[lcount] =  0;
		DCACHE_power_cc3[lcount] =  0;
#ifdef INPUT_OUTPUT_BUFFER
		input_buf_power_cc3[lcount]=0;
		output_buf_power_cc3[lcount]=0;
#endif
		DCACHE2_power_cc3[lcount] =  0;
		ALU1_power_cc3[lcount] =  0;
		ALU2_power_cc3[lcount] =  0;
		FALU1_power_cc3[lcount] =  0;
		FALU2_power_cc3[lcount] =  0;
		IRESULTBUS_power_cc3[lcount] =  0;
		FRESULTBUS_power_cc3[lcount] =  0;
	}
}

void clear_access_stats()
{
  int lcount;

  rename_access=0;
  iqram_access=0;
  fqram_access=0;
  rob1_access=0;
  rob2_access=0;
  bpred_access=0;
  iwakeup_access=0;
  fwakeup_access=0;
  iselection_access=0;
  fselection_access=0;
  lsq_access=0;
  iregfile_access=0;
  fregfile_access=0;
  icache_access=0;
  dcache_access=0;
#ifdef INPUT_OUTPUT_BUFFER
  input_buf_access=0;
  output_buf_access=0;
#endif
  dcache2_access=0;
  ialu1_access=0;
  ialu2_access=0;
  falu1_access=0;
  falu2_access=0;
  iresultbus_access=0;
  fresultbus_access=0;
  lsq_store_data_access=0;
  lsq_wakeup_access=0;
  lsq_preg_access=0;

  lsq_total_pop_count_cycle=0;
  lsq_num_pop_count_cycle=0;
  iregfile_total_pop_count_cycle=0;
  iregfile_num_pop_count_cycle=0;
  fregfile_total_pop_count_cycle=0;
  fregfile_num_pop_count_cycle=0;
  iresultbus_total_pop_count_cycle=0;
  iresultbus_num_pop_count_cycle=0;
  fresultbus_total_pop_count_cycle=0;
  fresultbus_num_pop_count_cycle=0;

  /* ejt: fault recovery counters added for Wattch */
  duplication_access=0;
  compare_access=0;

  for(lcount =0; lcount<numcontexts; lcount++)
  {
	if(thecontexts[lcount]->active_this_cycle)
	{
	thecontexts[lcount]->rename_access=0;
	thecontexts[lcount]->iqram_access=0;
	thecontexts[lcount]->fqram_access=0;
	thecontexts[lcount]->rob1_access=0;
	thecontexts[lcount]->rob2_access=0;
	thecontexts[lcount]->bpred_access=0;
	thecontexts[lcount]->iwakeup_access=0;
	thecontexts[lcount]->fwakeup_access=0;
	thecontexts[lcount]->iselection_access=0;
	thecontexts[lcount]->fselection_access=0;
	thecontexts[lcount]->lsq_access=0;
	thecontexts[lcount]->iregfile_access=0;
	thecontexts[lcount]->fregfile_access=0;
	thecontexts[lcount]->icache_access=0;
	thecontexts[lcount]->dcache_access=0;
#ifdef INPUT_OUTPUT_BUFFER
  	thecontexts[lcount]->input_buf_access=0;
  	thecontexts[lcount]->output_buf_access=0;
#endif
	thecontexts[lcount]->dcache2_access=0;
	thecontexts[lcount]->ialu1_access=0;
	thecontexts[lcount]->ialu2_access=0;
	thecontexts[lcount]->falu1_access=0;
	thecontexts[lcount]->falu2_access=0;
	thecontexts[lcount]->iresultbus_access=0;
	thecontexts[lcount]->fresultbus_access=0;
	thecontexts[lcount]->lsq_wakeup_access=0;
	thecontexts[lcount]->lsq_preg_access=0;
	}
  }
}

/* compute bitline activity factors which we use to scale bitline power
   Here it is very important whether we assume 0's or 1's are
   responsible for dissipating power in pre-charged stuctures. (since
   most of the bits are 0's, we assume the design is power-efficient
   enough to allow 0's to _not_ discharge
*/
double compute_af(counter_t num_pop_count_cycle,counter_t total_pop_count_cycle,int pop_width) {
  double avg_pop_count;
  double af,af_b;

  if(num_pop_count_cycle)
    avg_pop_count = (double)total_pop_count_cycle / (double)num_pop_count_cycle;
  else
    avg_pop_count = 0;

  af = avg_pop_count / (double)pop_width;
  
  af_b = 1.0 - af;

  /*  printf("af == %f%%, af_b == %f%%, total_pop == %d, num_pop == %d\n",100*af,100*af_b,total_pop_count_cycle,num_pop_count_cycle); */

  return(af_b);
}

void change_power_vals(power_result_type power, double ifq_period, double iiq_period, double fiq_period)
{
  power.rename_power *= (last_ifq_period/ifq_period)*(last_ifq_period/ifq_period);
  power.iqram_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.fqram_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.rob1_power *= (last_ifq_period/ifq_period)*(last_ifq_period/ifq_period);
  power.rob2_power *= (last_ifq_period/ifq_period)*(last_ifq_period/ifq_period);
  power.clock1_power *= (last_ifq_period/ifq_period)*(last_ifq_period/ifq_period);
  power.clock2_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.clock3_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.bpred_power *= (last_ifq_period/ifq_period)*(last_ifq_period/ifq_period);
  power.iwakeup_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.fwakeup_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.iselection *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.fselection *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.lsq_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.lsq_wakeup_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.lsq_rs_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.lsq_rs_power_nobit *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.lsq_rs_bitline *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.iregfile_bitline *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.iregfile_power_nobit *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.iregfile_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.fregfile_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.fregfile_power_nobit *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.fregfile_bitline *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.icache_power *= (last_ifq_period/ifq_period)*(last_ifq_period/ifq_period);
  power.itlb *= (last_ifq_period/ifq_period)*(last_ifq_period/ifq_period);
  power.dcache_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.dtlb *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.dcache2_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.ialu1_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.ialu2_power *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.falu1_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.falu2_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.iresultbus *= (last_iiq_period/iiq_period)*(last_iiq_period/iiq_period);
  power.fresultbus *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  /* ejt: fault recovery power added for Wattch */
  power.duplication_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);
  power.compare_power *= (last_fiq_period/fiq_period)*(last_fiq_period/fiq_period);

  last_ifq_period = ifq_period;
  last_iiq_period = iiq_period;
  last_fiq_period = fiq_period;
}

/* compute power statistics on each cycle, for each conditional clocking style.  Obviously
most of the speed penalty comes here, so if you don't want per-cycle power estimates
you could post-process 

See README.wattch for details on the various clock gating styles.

*/
void update_power_stats(power_result_type power, int threadid, context *current)
{
  double lsq_af_b, iregfile_af_b, fregfile_af_b, iresultbus_af_b, fresultbus_af_b, hotspot_temp, hotspot_temp2;

#ifdef DYNAMIC_AF
  lsq_af_b = compute_af(lsq_num_pop_count_cycle,lsq_total_pop_count_cycle,data_width);
  iregfile_af_b = compute_af(iregfile_num_pop_count_cycle,iregfile_total_pop_count_cycle,data_width);
  fregfile_af_b = compute_af(fregfile_num_pop_count_cycle,fregfile_total_pop_count_cycle,data_width);
  iresultbus_af_b = compute_af(iresultbus_num_pop_count_cycle,iresultbus_total_pop_count_cycle,data_width);
  fresultbus_af_b = compute_af(fresultbus_num_pop_count_cycle,fresultbus_total_pop_count_cycle,data_width);
#endif

  rename_power+=power.rename_power;
  iqram_power+=power.iqram_power;
  fqram_power+=power.fqram_power;
  rob1_power+=power.rob1_power;
  rob2_power+=power.rob2_power;
  clock1_power += power.clock1_power;
  clock2_power += power.clock2_power;
  clock3_power += power.clock3_power;
  bpred_power+=power.bpred_power;
  iwakeup_power+=power.iwakeup_power;
  fwakeup_power+=power.fwakeup_power;
  iselection_power+=power.iselection;
  fselection_power+=power.fselection;
  lsq_power+=power.lsq_power;
  iregfile_power+=power.iregfile_power;
  fregfile_power+=power.fregfile_power;
  icache_power+=power.icache_power+power.itlb;
  dcache_power+=power.dcache_power+power.dtlb;
#ifdef INPUT_OUTPUT_BUFFER
  input_buf_power += power.input_buf_power;
  output_buf_power += power.output_buf_power;
#endif
  dcache2_power+=power.dcache2_power;
  alu1_power+=power.ialu1_power;
  alu2_power+=power.ialu2_power;
  falu1_power+=power.falu1_power;
  falu2_power+=power.falu2_power;
  iresultbus_power+=power.iresultbus;
  fresultbus_power+=power.fresultbus;
  clock_power+=power.clock_power;
  /* ejt: fault recovery power added for Wattch */
  duplication_power+=power.duplication_power;
  compare_power+=power.compare_power;

  total_rename_access+=rename_access;
  total_iqram_access+=iqram_access;
  total_fqram_access+=fqram_access;
  total_rob1_access+=rob1_access;
  total_rob2_access+=rob2_access;
  total_bpred_access+=bpred_access;
  total_iwakeup_access+=iwakeup_access;
  total_fwakeup_access+=fwakeup_access;
  total_iselection_access+=iselection_access;
  total_fselection_access+=fselection_access;
  total_lsq_access+=lsq_access;
  total_iregfile_access+=iregfile_access;
  total_fregfile_access+=fregfile_access;
  total_icache_access+=icache_access;
  total_dcache_access+=dcache_access;
#ifdef INPUT_OUTPUT_BUFFER
  total_input_buf_access += input_buf_access;
  total_output_buf_access += output_buf_access;
#endif
  total_dcache2_access+=dcache2_access;
  total_alu1_access+=ialu1_access;
  total_alu2_access+=ialu2_access;
  total_falu1_access+=falu1_access;
  total_falu2_access+=falu2_access;
  total_iresultbus_access+=iresultbus_access;
  total_fresultbus_access+=fresultbus_access;
  /* ejt: fault recovery counters added for Wattch */
  total_duplication_access+=duplication_access;
  total_compare_access+=compare_access;

  max_rename_access=MAX(rename_access,max_rename_access);
  max_iqram_access=MAX(iqram_access,max_iqram_access);
  max_fqram_access=MAX(fqram_access,max_fqram_access);
  max_rob1_access=MAX(rob1_access, max_rob1_access);
  max_rob2_access=MAX(rob2_access, max_rob2_access);
  max_bpred_access=MAX(bpred_access,max_bpred_access);
  max_iwakeup_access=MAX(iwakeup_access,max_iwakeup_access);
  max_fwakeup_access=MAX(fwakeup_access,max_fwakeup_access);
  max_iselection_access=MAX(iselection_access,max_iselection_access);
  max_fselection_access=MAX(fselection_access,max_fselection_access);
  max_lsq_access=MAX(lsq_access,max_lsq_access);
  max_iregfile_access=MAX(iregfile_access,max_iregfile_access);
  max_fregfile_access=MAX(fregfile_access,max_fregfile_access);
  max_icache_access=MAX(icache_access,max_icache_access);
  max_dcache_access=MAX(dcache_access,max_dcache_access);
#ifdef INPUT_OUTPUT_BUFFER
  max_input_buf_access = MAX(input_buf_access, max_input_buf_access);
  max_output_buf_access = MAX(output_buf_access, max_output_buf_access);
#endif
  max_dcache2_access=MAX(dcache2_access,max_dcache2_access);
  max_alu1_access=MAX(ialu1_access,max_alu1_access);
  max_alu2_access=MAX(ialu2_access,max_alu2_access);
  max_falu1_access=MAX(falu1_access,max_falu1_access);
  max_falu2_access=MAX(falu2_access,max_falu2_access);
  max_iresultbus_access=MAX(iresultbus_access,max_iresultbus_access);
  max_fresultbus_access=MAX(fresultbus_access,max_fresultbus_access);
  /* ejt: fault recovery counters added for Wattch */
  max_duplication_access=MAX(duplication_access,max_duplication_access);
  max_compare_access=MAX(compare_access,max_compare_access);

  if(current->rename_access) {
    RENAME_power_cc3[threadid]+=(((double)current->rename_access + (((double)MAX(0,(ruu_decode_width)-current->rename_access))*turnoff_factor))/(double)(ruu_decode_width))*power.rename_power;
    IFCC3(hotspot_temp=(((double)current->rename_access + (((double)MAX(0,(ruu_decode_width)-current->rename_access))*turnoff_factor))/(double)(ruu_decode_width))*power.rename_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.rename_power;
        RENAME_power_cc3[threadid]+=turnoff_factor*power.rename_power;
	})
  /* 012005 ejt: approx assume that int and fp rename are equally used */
  IFCC3(hotspot_perperiod[threadid].irename+=hotspot_temp/2;
	hotspot_perperiod[threadid].frename+=hotspot_temp/2;)  

  /* 120204 ejt: fqram contributes to the fqueue hotspot */
  if (current->fqram_access) {
    FQRAM_power_cc3[threadid]+=(((double)current->fqram_access + (((double)MAX(0,(ruu_decode_width)+ruu_issue_width-current->fqram_access))*turnoff_factor))/(double)(ruu_issue_width+(ruu_decode_width)))*power.fqram_power;
    IFCC3(hotspot_temp=(((double)current->fqram_access + (((double)MAX(0,(ruu_decode_width)+ruu_issue_width-current->fqram_access))*turnoff_factor))/(double)(ruu_issue_width+(ruu_decode_width)))*power.fqram_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.fqram_power;
        FQRAM_power_cc3[threadid]+=turnoff_factor*power.fqram_power;
        })
  IFCC3(hotspot_perperiod[threadid].fqueue+=hotspot_temp;)

  /* 120204 ejt: iqram contributes to the iqueue hotspot */
  if (current->iqram_access) {
    IQRAM_power_cc3[threadid]+=(((double)current->iqram_access + (((double)MAX(0,(ruu_decode_width)+ruu_issue_width-current->iqram_access))*turnoff_factor))/(double)(ruu_issue_width+(ruu_decode_width)))*power.iqram_power;
    IFCC3(hotspot_temp=(((double)current->iqram_access + (((double)MAX(0,(ruu_decode_width)+ruu_issue_width-current->iqram_access))*turnoff_factor))/(double)(ruu_issue_width+(ruu_decode_width)))*power.iqram_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.fqram_power;
        IQRAM_power_cc3[threadid]+=turnoff_factor*power.iqram_power;
        })
  IFCC3(hotspot_perperiod[threadid].iqueue+=hotspot_temp;)
  
  /* 120104 ejt: rob1 and rob2 both contribute to the rob hotspot */
  /* 120204 ejt: rob1 is rob access during dispatch */
  if (current->rob1_access) {
    ROB1_power_cc3[threadid]+=(((double)current->rob1_access + (((double)MAX(0,ruu_commit_width-current->rob1_access))*turnoff_factor))/(double)ruu_commit_width)*power.rob1_power;
    IFCC3(hotspot_temp=(((double)current->rob1_access + (((double)MAX(0,ruu_commit_width-current->rob1_access))*turnoff_factor))/(double)ruu_commit_width)*power.rob1_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.rob1_power;
	ROB1_power_cc3[threadid]+=turnoff_factor*power.rob1_power;
        })
  IFCC3(hotspot_perperiod[threadid].rob+=hotspot_temp;)
  
  /* 120204 ejt: rob2 is rob access during writeback */
  if (current->rob2_access) {
    ROB2_power_cc3[threadid]+=(((double)current->rob2_access+ (((double)MAX(0,(ruu_decode_width)+ruu_commit_width-current->rob2_access))*turnoff_factor))/(double)(ruu_commit_width+(ruu_decode_width)))*power.rob2_power;
   IFCC3(hotspot_temp=(((double)current->rob2_access+ (((double)MAX(0,(ruu_decode_width)+ruu_commit_width-current->rob2_access))*turnoff_factor))/(double)(ruu_commit_width+(ruu_decode_width)))*power.rob2_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.rob2_power; 
        ROB2_power_cc3[threadid]+=turnoff_factor*power.rob2_power;
	})
  IFCC3(hotspot_perperiod[threadid].rob+=hotspot_temp;) 
	
  CLOCK1_power_cc3[threadid] += power.clock1_power;
  IFCC3(hotspot_perperiod[0].clock1+=power.clock1_power;)

  CLOCK2_power_cc3[threadid] += power.clock2_power;
  IFCC3(hotspot_perperiod[0].clock2+=power.clock2_power;)
  
  CLOCK3_power_cc3[threadid] += power.clock3_power;
  IFCC3(hotspot_perperiod[0].clock3+=power.clock3_power;)

  if(current->bpred_access) {
    if (current->bpred_access < 2){
      	BPRED_power_cc3[threadid]+=0.6*power.bpred_power;
      	hotspot_temp=0.6*power.bpred_power;
    }
    else{
      	BPRED_power_cc3[threadid]+=((double)current->bpred_access/2.0) * power.bpred_power;
      	IFCC3(hotspot_temp=((double)current->bpred_access/2.0) * power.bpred_power;)
    }
  }
  IFCC3(else { 
	hotspot_temp=turnoff_factor*power.bpred_power;	  
        BPRED_power_cc3[threadid]+=turnoff_factor*power.bpred_power;
	})
  IFCC3(hotspot_perperiod[threadid].bpred+=hotspot_temp;)

  /* 120104 ejt: iselection contributes to the iqueue hotspot */
  if(current->iselection_access) {
      	ISELECTION_power_cc3[threadid]+=power.iselection;
      	IFCC3(hotspot_temp=power.iselection;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.iselection;
        ISELECTION_power_cc3[threadid]+=turnoff_factor*power.iselection;
 	})
  IFCC3(hotspot_perperiod[threadid].iqueue+=hotspot_temp;)

  /* 120104 ejt: fselection contributes to the fqueue hotspot */
  if(current->fselection_access) {
      	FSELECTION_power_cc3[threadid]+=power.fselection;
      	IFCC3(hotspot_temp=power.fselection;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.fselection;
        FSELECTION_power_cc3[threadid]+=turnoff_factor*power.fselection;
 	})
  IFCC3(hotspot_perperiod[threadid].fqueue+=hotspot_temp;)

  /* 120104 ejt: iwakeup contributes to the iqueue hotspot */	  
  if(current->iwakeup_access) {
      	IWAKEUP_power_cc3[threadid]+=power.iwakeup_power;
      	IFCC3(hotspot_temp=power.iwakeup_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.iwakeup_power;
        FWAKEUP_power_cc3[threadid]+=turnoff_factor*power.iwakeup_power;
 	})
  IFCC3(hotspot_perperiod[threadid].iqueue+=hotspot_temp;)

  /* 120104 ejt: fwakeup contributes to the iqueue hotspot */
  if(current->fwakeup_access) {
      	FWAKEUP_power_cc3[threadid]+=power.fwakeup_power;
      	IFCC3(hotspot_temp=power.fwakeup_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.fwakeup_power;
        FWAKEUP_power_cc3[threadid]+=turnoff_factor*power.fwakeup_power;
 	})
  IFCC3(hotspot_perperiod[threadid].fqueue+=hotspot_temp;)
	  
  if(current->lsq_wakeup_access) {
      	LSQ_power_cc3[threadid]+=power.lsq_wakeup_power;
      	IFCC3(hotspot_temp=power.lsq_wakeup_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.lsq_wakeup_power;
        LSQ_power_cc3[threadid]+=turnoff_factor*power.lsq_wakeup_power;
 	})
  IFCC3(hotspot_perperiod[threadid].lsq+=hotspot_temp;)

#ifdef INPUT_OUTPUT_BUFFER
  if(current->input_buf_access) {
      input_buf_power_cc3[threadid]+= current->input_buf_access*power.input_buf_power;
  }
  IFCC3(else {
      input_buf_power_cc3[threadid]+=turnoff_factor*power.input_buf_power;
      })

  if(current->output_buf_access) {
      output_buf_power_cc3[threadid]+=current->output_buf_access*power.output_buf_power;
  }
  IFCC3(else {
      output_buf_power_cc3[threadid]+=turnoff_factor*power.output_buf_power;
      })
#endif
	  
#ifdef STATIC_AF
  if(current->lsq_preg_access) {
    	LSQ_power_cc3[threadid]+=(((double)current->lsq_preg_access + (((double)MAX(0,(res_memport/CLUSTERS)-current->lsq_preg_access))*turnoff_factor))/((double)res_memport/CLUSTERS))*power.lsq_rs_power;
    	IFCC3(hotspot_temp=(((double)current->lsq_preg_access + (((double)MAX(0,(res_memport/CLUSTERS)-current->lsq_preg_access))*turnoff_factor))/((double)res_memport/CLUSTERS))*power.lsq_rs_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.lsq_rs_power;
        LSQ_power_cc3[threadid]+=turnoff_factor*power.lsq_rs_power;
 	})
  IFCC3(hotspot_perperiod[threadid].lsq+=hotspot_temp;)

#else // for #ifdef STATIC_AF
  if(current->lsq_preg_access) {
    	LSQ_power_cc3[threadid]+=(((double)current->lsq_preg_access + (((double)MAX(0,(res_memport/CLUSTERS)-current->lsq_preg_access))*turnoff_factor))/((double)res_memport/CLUSTERS))*(power.lsq_rs_power_nobit + lsq_af_b*power.lsq_rs_bitline);
    	IFCC3(hotspot_temp=(((double)current->lsq_preg_access + (((double)MAX(0,(res_memport/CLUSTERS)-current->lsq_preg_access))*turnoff_factor))/((double)res_memport/CLUSTERS))*(power.lsq_rs_power_nobit + lsq_af_b*power.lsq_rs_bitline);)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.lsq_rs_power;
        LSQ_power_cc3[threadid]+=turnoff_factor*power.lsq_rs_power;
 	})
  IFCC3(hotspot_perperiod[threadid].lsq+=hotspot_temp;)
#endif // for #ifdef STATIC_AF

  if(current->iregfile_access) {
    	IREGFILE_power_cc3[threadid]+=(((double)current->iregfile_access + (((double)MAX(0,3*ruu_commit_width -current->iregfile_access))*turnoff_factor))/(3.0*(double)ruu_commit_width))*(power.iregfile_power_nobit + iregfile_af_b*power.iregfile_bitline);
    	IFCC3(hotspot_temp=(((double)current->iregfile_access + (((double)MAX(0,3*ruu_commit_width -current->iregfile_access))*turnoff_factor))/(3.0*(double)ruu_commit_width))*(power.iregfile_power_nobit + iregfile_af_b*power.iregfile_bitline);)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.iregfile_power;
	IREGFILE_power_cc3[threadid]+=turnoff_factor*power.iregfile_power;
	})
/* 012205 ejt: approx assume that ireg 0 and 1 are equally used */ 
  IFCC3(hotspot_perperiod[threadid].ireg0+=hotspot_temp/2;
  	hotspot_perperiod[threadid].ireg1+=hotspot_temp/2;)
  
  if(current->fregfile_access) {
    	FREGFILE_power_cc3[threadid]+=(((double)current->fregfile_access + (((double)MAX(0,3*ruu_commit_width - current->iregfile_access))*turnoff_factor))/(3.0*(double)ruu_commit_width))*(power.fregfile_power_nobit + fregfile_af_b*power.fregfile_bitline);
    	IFCC3(hotspot_temp=(((double)current->fregfile_access + (((double)MAX(0,3*ruu_commit_width - current->iregfile_access))*turnoff_factor))/(3.0*(double)ruu_commit_width))*(power.fregfile_power_nobit + fregfile_af_b*power.fregfile_bitline);)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.fregfile_power;
	FREGFILE_power_cc3[threadid]+=turnoff_factor*power.fregfile_power;
	})
  IFCC3(hotspot_perperiod[threadid].freg+=hotspot_temp;)

  /* 120104 ejt: split power into icache power and itlb power for hotspot since
                 they are separate structures */
  if(current->icache_access) {
    	ICACHE_power_cc3[threadid]+=power.icache_power+power.itlb;
    	IFCC3(hotspot_temp=power.icache_power;)
    	IFCC3(hotspot_temp2=power.itlb;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*(power.icache_power);
	hotspot_temp2=turnoff_factor*(power.itlb);
    	ICACHE_power_cc3[threadid]+=turnoff_factor*(power.icache_power+power.itlb);
  })
  IFCC3(hotspot_perperiod[threadid].icache+=hotspot_temp;)
  IFCC3(hotspot_perperiod[threadid].itlb+=hotspot_temp2;)
  
  if(current->dcache_access) {
    	DCACHE_power_cc3[threadid]+=(((double)current->dcache_access + (((double)MAX(0,(res_memport/CLUSTERS+DIR_FIFO_PORTS)-current->dcache_access))*turnoff_factor))/((double)res_memport/CLUSTERS+DIR_FIFO_PORTS))*(power.dcache_power + power.dtlb);
    	IFCC3(hotspot_temp=(((double)current->dcache_access + (((double)MAX(0,(res_memport/CLUSTERS+DIR_FIFO_PORTS)-current->dcache_access))*turnoff_factor))/((double)res_memport/CLUSTERS+DIR_FIFO_PORTS))*(power.dcache_power);)
    	IFCC3(hotspot_temp2=(((double)current->dcache_access + (((double)MAX(0,(res_memport/CLUSTERS+DIR_FIFO_PORTS)-current->dcache_access))*turnoff_factor))/((double)res_memport/CLUSTERS+DIR_FIFO_PORTS))*(power.dtlb);)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*(power.dcache_power);
	hotspot_temp2=turnoff_factor*(power.dtlb);
    	DCACHE_power_cc3[threadid]+=turnoff_factor*(power.dcache_power+power.dtlb);
	})
  IFCC3(hotspot_perperiod[threadid].dl1cache+=hotspot_temp;)    
/* 012205 ejt: approx assume that dtlb 0 and 1 are equally used */  
  IFCC3(hotspot_perperiod[threadid].dtb0+=hotspot_temp2/2;    
  	hotspot_perperiod[threadid].dtb1+=hotspot_temp2/2;)    

  if(current->dcache2_access) {
    	DCACHE2_power_cc3[threadid]+=(((double)current->dcache2_access + (((double)MAX(0,(DIR_FIFO_PORTS)-current->dcache2_access))*turnoff_factor))/((double)DIR_FIFO_PORTS))*power.dcache2_power;
    	IFCC3(hotspot_temp=(((double)current->dcache2_access + (((double)MAX(0,(DIR_FIFO_PORTS)-current->dcache2_access))*turnoff_factor))/((double)DIR_FIFO_PORTS))*power.dcache2_power;)
  }
  IFCC3(else {
    	hotspot_temp=turnoff_factor*power.dcache2_power;
    	DCACHE2_power_cc3[threadid]+=turnoff_factor*power.dcache2_power;
	})

  /* nan or gjb: The sharing of the ul2cache entitiy is taken care of in 
  	hotspot.c, so we just stick it all on index 0 for now. */
  IFCC3(hotspot_perperiod[0].ul2cache+=hotspot_temp;)
  IFCC3(hotspot_perperiod[0].sysintf+=hotspot_temp;)
    
  if(current->ialu1_access) {
    	ALU1_power_cc3[threadid]+=(((double)current->ialu1_access + (((double)MAX(0,(res_ialu/CLUSTERS)-current->ialu1_access))*turnoff_factor))/((double)res_ialu/CLUSTERS))*power.ialu1_power;
    	IFCC3(hotspot_temp=(((double)current->ialu1_access + (((double)MAX(0,(res_ialu/CLUSTERS)-current->ialu1_access))*turnoff_factor))/((double)res_ialu/CLUSTERS))*power.ialu1_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.ialu1_power;
	ALU1_power_cc3[threadid]+=turnoff_factor*power.ialu1_power;
        })

  /* 012205 ejt: approx assume that ialu 0, 1 and 2 are equally used, so
               divide power by 3 */
  IFCC3(hotspot_perperiod[threadid].ialu0+=hotspot_temp/3;
	hotspot_perperiod[threadid].ialu1+=hotspot_temp/3;
  /* 012305 ejt: since ialu2 is for int mult, use ialu3 for int alu 2 */	
	hotspot_perperiod[threadid].ialu3+=hotspot_temp/3;)
    

  /* 012205 ejt: ialu2_access is used to count the number of accesses to 
               the mult, assume access is to 1 mult in the 21264 */  
  if(current->ialu2_access) {
    	ALU2_power_cc3[threadid]+=(((double)current->ialu2_access + (((double)MAX(0,(res_imult/CLUSTERS)-current->ialu2_access))*turnoff_factor))/((double)res_imult/CLUSTERS))*power.ialu2_power;
    	IFCC3(hotspot_temp=(((double)current->ialu2_access + (((double)MAX(0,(res_imult/CLUSTERS)-current->ialu2_access))*turnoff_factor))/((double)res_imult/CLUSTERS))*power.ialu2_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.ialu2_power;
	ALU2_power_cc3[threadid]+=turnoff_factor*power.ialu2_power;
        })
  IFCC3(hotspot_perperiod[threadid].ialu2+=hotspot_temp;)

  /* 120104 ejt: falu1 is the floating point adder */
  if(current->falu1_access) {
    	FALU1_power_cc3[threadid]+=(((double)current->falu1_access + (((double)MAX(0,(res_fpalu/CLUSTERS)-current->falu1_access))*turnoff_factor))/((double)res_fpalu/CLUSTERS))*power.falu1_power;
    	IFCC3(hotspot_temp=(((double)current->falu1_access + (((double)MAX(0,(res_fpalu/CLUSTERS)-current->falu1_access))*turnoff_factor))/((double)res_fpalu/CLUSTERS))*power.falu1_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.falu1_power;
	FALU1_power_cc3[threadid]+=turnoff_factor*power.falu1_power;
        })
  IFCC3(hotspot_perperiod[threadid].fadd+=hotspot_temp;)

  /* 120104 ejt: falu2 is the floating point multiplier */
  if(current->falu2_access) {
    	FALU2_power_cc3[threadid]+=(((double)current->falu2_access + (((double)MAX(0,(res_fpmult/CLUSTERS)-current->falu2_access))*turnoff_factor))/((double)res_fpmult/CLUSTERS))*power.falu2_power;
    	IFCC3(hotspot_temp=(((double)current->falu2_access + (((double)MAX(0,(res_fpmult/CLUSTERS)-current->falu2_access))*turnoff_factor))/((double)res_fpmult/CLUSTERS))*power.falu2_power;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.falu2_power;
	FALU2_power_cc3[threadid]+=turnoff_factor*power.falu2_power;
        })
  IFCC3(hotspot_perperiod[threadid].fmul+=hotspot_temp;)


#ifdef STATIC_AF
  if(current->iresultbus_access) {
    	IRESULTBUS_power_cc3[threadid]+=(((double)current->iresultbus_access + (((double)MAX(0,ruu_issue_width-current->iresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*power.iresultbus;
    	IFCC3(hotspot_temp=(((double)current->iresultbus_access + (((double)MAX(0,ruu_issue_width-current->iresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*power.iresultbus;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.iresultbus;
	IRESULTBUS_power_cc3[threadid]+=turnoff_factor*power.iresultbus;
        })
  IFCC3(hotspot_perperiod[threadid].iresultbus+=hotspot_temp;)

#else // for #ifdef STATIC_AF
  if(current->iresultbus_access) {
    	IRESULTBUS_power_cc3[threadid]+=(((double)current->iresultbus_access + (((double)MAX(0,ruu_issue_width-current->iresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*iresultbus_af_b*power.iresultbus;
   	IFCC3(hotspot_temp=(((double)current->iresultbus_access + (((double)MAX(0,ruu_issue_width-current->iresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*iresultbus_af_b*power.iresultbus;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.iresultbus;
	IRESULTBUS_power_cc3[threadid]+=turnoff_factor*power.iresultbus;
        })
  IFCC3(hotspot_perperiod[threadid].iresultbus+=hotspot_temp;)
#endif // for #ifdef STATIC_AF


#ifdef STATIC_AF
  if(current->fresultbus_access) {
    	FRESULTBUS_power_cc3[threadid]+=(((double)current->fresultbus_access + (((double)MAX(0,ruu_issue_width-current->fresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*power.fresultbus;
    	IFCC3(hotspot_temp=(((double)current->fresultbus_access + (((double)MAX(0,ruu_issue_width-current->fresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*power.fresultbus;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.fresultbus;
	FRESULTBUS_power_cc3[threadid]+=turnoff_factor*power.fresultbus;
        })
  IFCC3(hotspot_perperiod[threadid].fresultbus+=hotspot_temp;)
#else // for #ifdef STATIC_AF
  if(current->fresultbus_access) {
    	FRESULTBUS_power_cc3[threadid]+=(((double)current->fresultbus_access + (((double)MAX(0,ruu_issue_width-current->fresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*fresultbus_af_b*power.fresultbus;
    	IFCC3(hotspot_temp=(((double)current->fresultbus_access + (((double)MAX(0,ruu_issue_width-current->fresultbus_access))*turnoff_factor))/(double)ruu_issue_width)*fresultbus_af_b*power.fresultbus;)
  }
  IFCC3(else {
	hotspot_temp=turnoff_factor*power.fresultbus;
	FRESULTBUS_power_cc3[threadid]+=turnoff_factor*power.fresultbus;
        })
  IFCC3(hotspot_perperiod[threadid].fresultbus+=hotspot_temp;)
#endif // for #ifdef STATIC_AF

  /* ejt: fault recovery power added for Wattch */
  /* 070104 ejt: compute duplication power */
  /* do not scale duplication power as we assume that duplication is always
     carried out using entire circuit structure and there's no clock gating */

  if(!disable_ray){       
    if (duplication_access) 
      duplication_power_cc3 += power.duplication_power;  
    else
      duplication_power_cc3 += turnoff_factor*power.duplication_power;
 
    /* 062204 ejt: compute compare power */
    /* do not scale compare power as we assume that compare is always carried
       out using entire circuit structure and there's no clock gating */
    if (compare_access) 
      compare_power_cc3 += power.compare_power;
    else
      compare_power_cc3 += turnoff_factor*power.compare_power;
  }

  total_cycle_power = rename_power + iqram_power + fqram_power + clock1_power + clock2_power + clock3_power + rob1_power + rob2_power + bpred_power + iwakeup_power +
    fwakeup_power + iselection_power + fselection_power +
    lsq_power + iregfile_power + fregfile_power + icache_power + dcache_power +
#ifdef INPUT_OUTPUT_BUFFER
    input_buf_power + output_buf_power +
#endif
    alu1_power + alu2_power + falu1_power + falu2_power + iresultbus_power + fresultbus_power + duplication_power + compare_power;

  TOTAL_power_cc3[threadid] = RENAME_power_cc3 [threadid]+ IQRAM_power_cc3[threadid] + FQRAM_power_cc3[threadid] +
    CLOCK1_power_cc3[threadid] + CLOCK2_power_cc3[threadid] + CLOCK3_power_cc3[threadid] +
    ROB1_power_cc3[threadid] + ROB2_power_cc3[threadid] + BPRED_power_cc3[threadid] +
    IWAKEUP_power_cc3 [threadid]+ FWAKEUP_power_cc3[threadid] + ISELECTION_power_cc3[threadid] +
    FSELECTION_power_cc3[threadid] +
    LSQ_power_cc3[threadid] + IREGFILE_power_cc3[threadid] + FREGFILE_power_cc3 [threadid]+
    ICACHE_power_cc3[threadid] + DCACHE_power_cc3[threadid] + ALU1_power_cc3[threadid] +
    ALU2_power_cc3 [threadid]+ FALU1_power_cc3 [threadid]+ FALU2_power_cc3[threadid] + 
    IRESULTBUS_power_cc3[threadid] + FRESULTBUS_power_cc3 [threadid] + 
#ifdef INPUT_OUTPUT_BUFFER
    input_buf_power_cc3[threadid] + output_buf_power_cc3[threadid] +
#endif
    DCACHE2_power_cc3[threadid]+ duplication_power_cc3 + compare_power_cc3;

  max_cycle_power_cc3 = MAX(max_cycle_power_cc3,TOTAL_power_cc3[threadid]);
}

char *genBuffer(char *buf)
{
	int i;
	strcpy(buffer2, buf);
	for(i = 0; i < CLUSTERS; i++)
	{
		if(i != CLUSTERS-1)
			sprintf(buffer1,"_%d + ", i);
		else
			sprintf(buffer1,"_%d", i);
		strcat(buffer2, buffer1);
		if(i != CLUSTERS-1)
			strcat(buffer2, buf);
	}
	return buffer2;
}

void
power_reg_stats(struct stat_sdb_t *sdb)	/* stats database */
{
  int lcount;

  stat_reg_double(sdb, "rename_power", "total power usage of rename unit", &rename_power, 0, NULL);
  stat_reg_double(sdb, "iqram_power", "total power usage of integer queue ram", &iqram_power, 0, NULL);
  stat_reg_double(sdb, "fqram_power", "total power usage of fp queue ram", &fqram_power, 0, NULL);


  stat_reg_double(sdb, "clock1_power", "total power usage of clock fr-end", &clock1_power, 0, NULL);
  stat_reg_double(sdb, "clock2_power", "total power usage of clock int", &clock2_power, 0, NULL);
  stat_reg_double(sdb, "clock3_power", "total power usage of clock fp", &clock3_power, 0, NULL);

  stat_reg_double(sdb, "rob1_power", "total power usage of rob wakeup", &rob1_power, 0, NULL);
  stat_reg_double(sdb, "rob2_power", "total power usage of rob ram", &rob2_power, 0, NULL);
  stat_reg_double(sdb, "bpred_power", "total power usage of bpred unit", &bpred_power, 0, NULL);
  stat_reg_double(sdb, "iwakeup_power", "total power usage of int wakeup", &iwakeup_power, 0, NULL);
  stat_reg_double(sdb, "fwakeup_power", "total power usage of fp wakeup", &fwakeup_power, 0, NULL);
  stat_reg_double(sdb, "iselection_power", "total power usage of int selection", &iselection_power, 0, NULL);
  stat_reg_double(sdb, "fselection_power", "total power usage of fp selection", &fselection_power, 0, NULL);
  stat_reg_double(sdb, "lsq_power", "total power usage of load/store queue", &lsq_power, 0, NULL);
  stat_reg_double(sdb, "iregfile_power", "total power usage of int regfile", &iregfile_power, 0, NULL);
  stat_reg_double(sdb, "fregfile_power", "total power usage of fp regfile", &fregfile_power, 0, NULL);
  stat_reg_double(sdb, "icache_power", "total power usage of icache", &icache_power, 0, NULL);
  stat_reg_double(sdb, "dcache_power", "total power usage of dcache", &dcache_power, 0, NULL);
  stat_reg_double(sdb, "dcache2_power", "total power usage of dcache2", &dcache2_power, 0, NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_double(sdb, "input_buf_power", "total power usage of input buffer's", &input_buf_power, 0, NULL);
  stat_reg_double(sdb, "output_buf_power", "total power usage of output buffer's", &output_buf_power, 0, NULL);
#endif
  stat_reg_double(sdb, "alu1_power", "total power usage of alu1", &alu1_power, 0, NULL);
  stat_reg_double(sdb, "alu2_power", "total power usage of alu2", &alu2_power, 0, NULL);
  stat_reg_double(sdb, "falu1_power", "total power usage of falu1", &falu1_power, 0, NULL);
  stat_reg_double(sdb, "falu2_power", "total power usage of falu2", &falu2_power, 0, NULL);
  stat_reg_double(sdb, "iresultbus_power", "total power usage of iresultbus", &iresultbus_power, 0, NULL);
  stat_reg_double(sdb, "fresultbus_power", "total power usage of fresultbus", &fresultbus_power, 0, NULL);
  /* ejt: fault recovery power added for Wattch */
  stat_reg_double(sdb, "duplication_power", "total power usage of duplicator", &duplication_power, 0, NULL);
  stat_reg_double(sdb, "compare_power", "total power usage of comparator", &compare_power, 0, NULL);
  stat_reg_double(sdb, "clock_power", "total power usage of clock", &clock_power, 0, NULL);

  stat_reg_formula(sdb, "avg_rename_power", "avg power usage of rename unit", "rename_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_iqram_power", "avg power usage of int issueq ram", "iqram_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_fqram_power", "avg power usage of fp issueq ram", "fqram_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_clock1_power", "avg power usage of clock1", "clock1_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_clock2_power", "avg power usage of clock2", "clock2_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_clock3_power", "avg power usage of clock3", "clock3_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_rob1_power", "avg power usage of rob wakeup", "rob1_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_rob2_power", "avg power usage of rob ram", "rob2_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_bpred_power", "avg power usage of bpred unit", "bpred_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_iwakeup_power", "avg power usage of int wakeup", "iwakeup_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fwakeup_power", "avg power usage of fp wakeup", "fwakeup_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iselection_power", "avg power usage of int selection", "iselection_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fselection_power", "avg power usage of fp selection", "fselection_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_lsq_power", "avg power usage of lsq", "lsq_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iregfile_power", "avg power usage of int regfile", "iregfile_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fregfile_power", "avg power usage of fp regfile", "fregfile_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_icache_power", "avg power usage of icache", "icache_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_dcache_power", "avg power usage of dcache", "dcache_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_dcache2_power", "avg power usage of dcache2", "dcache2_power/sim_cycle",  NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_formula(sdb, "avg_input_buf_power", "avg power usage of input buffer's", "input_buf_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_output_buf_power", "avg power usage of output buffer's", "output_buf_power/sim_cycle", NULL);
#endif
  stat_reg_formula(sdb, "avg_alu1_power", "avg power usage of alu1", "alu1_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_alu2_power", "avg power usage of alu2", "alu2_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_falu1_power", "avg power usage of falu1", "falu1_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_falu2_power", "avg power usage of falu2", "falu2_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iresultbus_power", "avg power usage of iresultbus", "iresultbus_power/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fresultbus_power", "avg power usage of fresultbus", "fresultbus_power/sim_cycle",  NULL);
  /* ejt: fault recovery power added for Wattch */
  stat_reg_formula(sdb, "avg_duplication_power", "avg power usage of duplicator", "duplication_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_compare_power", "avg power usage of comparator", "compare_power/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_clock_power", "avg power usage of clock", "clock_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, "total_power", "total power per cycle (W) (cumulative)",xstr(TOTAL_CYCLE_POWER), NULL);
  stat_reg_formula(sdb, "avg_total_power_cycle", "average total power per cycle (W)",xstr(TOTAL_CYCLE_POWER/sim_cycle), NULL);
  stat_reg_formula(sdb, "avg_total_power_insn", "average total power per insn (not accurate)",xstr(TOTAL_CYCLE_POWER/sim_total_insn), NULL);

  for(lcount =0; lcount<CLUSTERS;lcount++)
  {
      sprintf(buffer1,"CLOCK1_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of CLOCK1", &CLOCK1_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"CLOCK2_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of CLOCK2", &CLOCK2_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"CLOCK3_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of CLOCK3", &CLOCK3_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"RENAME_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of RENAME", &RENAME_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"IQRAM_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of IQRAM ", &IQRAM_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"FQRAM_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of  FQRAM", &FQRAM_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"ROB1_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of  ROB1", &ROB1_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"ROB2_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of ROB2 ", &ROB2_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"BPRED_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of  BPRED", &BPRED_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"IWAKEUP_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of IWAKEUP ", &IWAKEUP_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"FWAKEUP_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of FWAKEUP", &FWAKEUP_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"ISELECTION_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of ISELECTION", &ISELECTION_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"FSELECTION_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of FSELECTION", &FSELECTION_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"LSQ_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of LSQ", &LSQ_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"IREGFILE_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of IREGFILE", &IREGFILE_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"FREGFILE_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of FREGFILE", &FREGFILE_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"ALU1_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of ALU1", &ALU1_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"ALU2_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of ALU2", &ALU2_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"FALU1_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of FALU1", &FALU1_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"FALU2_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of FALU2", &FALU2_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"IRESULTBUS_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of IRESULTBUS", &IRESULTBUS_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"FRESULTBUS_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of FRESULTBUS", &FRESULTBUS_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"ICACHE_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of ICACHE", &ICACHE_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"DCACHE_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of DCACHE", &DCACHE_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"DCACHE2_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of DCACHE2", &DCACHE2_power_cc3[lcount], 0, NULL);
#ifdef INPUT_OUTPUT_BUFFER
      sprintf(buffer1,"input_buf_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of input buffer", &input_buf_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"output_buf_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of output buffer", &output_buf_power_cc3[lcount], 0, NULL);
#endif
      sprintf(buffer1,"DUPLICATION_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of duplicator_cc3", &duplication_power_cc3, 0, NULL);
      sprintf(buffer1,"COMPARE_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage of comparator_cc3", &compare_power_cc3, 0, NULL);
      sprintf(buffer1,"TOTAL_power_cc3_%d",lcount);
      stat_reg_double(sdb, buffer1, "total power usage ", &TOTAL_power_cc3[lcount], 0, NULL);
      sprintf(buffer1,"AVG_TOTAL_power_cc3_%d",lcount);
      sprintf(buffer2,"TOTAL_power_cc3_%d/sim_cycle", lcount);
      stat_reg_formula(sdb, buffer1, "average total power per cycle_cc3", buffer2, NULL);
      sprintf(buffer1,"AVG_TOTAL_power_insn_cc3_%d",lcount);
      sprintf(buffer2,"TOTAL_power_cc3_%d/sim_total_insn", lcount);
      stat_reg_formula(sdb, buffer1, "average total power per insn_cc3(not accurate)",buffer2, NULL);
  }

  stat_reg_formula(sdb, "clock1_power_cc3", "total power usage of clock1_cc3", genBuffer("CLOCK1_power_cc3"), NULL);
  stat_reg_formula(sdb, "clock2_power_cc3", "total power usage of clock2_cc3", genBuffer("CLOCK2_power_cc3"), NULL);
  stat_reg_formula(sdb, "clock3_power_cc3", "total power usage of clock3_cc3", genBuffer("CLOCK3_power_cc3"), NULL);
  stat_reg_formula(sdb, "rename_power_cc3", "total power usage of rename unit_cc3", genBuffer("RENAME_power_cc3"), NULL);
  stat_reg_formula(sdb, "iqram_power_cc3", "total power usage of int issueq ram_cc3", genBuffer("IQRAM_power_cc3"), NULL);
  stat_reg_formula(sdb, "fqram_power_cc3", "total power usage of fp issueq ram_cc3", genBuffer("FQRAM_power_cc3"), NULL);
  stat_reg_formula(sdb, "rob1_power_cc3", "total power usage of rob wakeup_cc3", genBuffer("ROB1_power_cc3"), NULL);
  stat_reg_formula(sdb, "rob2_power_cc3", "total power usage of rob ram_cc3", genBuffer("ROB2_power_cc3"), NULL);
  stat_reg_formula(sdb, "bpred_power_cc3", "total power usage of bpred unit_cc3", genBuffer("BPRED_power_cc3"), NULL);
  stat_reg_formula(sdb, "iwakeup_power_cc3", "total power usage of int wakeup_cc3", genBuffer("IWAKEUP_power_cc3"), NULL);
  stat_reg_formula(sdb, "fwakeup_power_cc3", "total power usage of fp wakeup_cc3", genBuffer("FWAKEUP_power_cc3"), NULL);
  stat_reg_formula(sdb, "iselection_power_cc3", "total power usage of int selection_cc3", genBuffer("ISELECTION_power_cc3"), NULL);
  stat_reg_formula(sdb, "fselection_power_cc3", "total power usage of fp selection_cc3", genBuffer("FSELECTION_power_cc3"), NULL);
  stat_reg_formula(sdb, "lsq_power_cc3", "total power usage of lsq_cc3", genBuffer("LSQ_power_cc3"), NULL);
  stat_reg_formula(sdb, "iregfile_power_cc3", "total power usage of int regfile_cc3", genBuffer("IREGFILE_power_cc3"), NULL);
  stat_reg_formula(sdb, "fregfile_power_cc3", "total power usage of fp regfile_cc3", genBuffer("FREGFILE_power_cc3"), NULL);
  stat_reg_formula(sdb, "alu1_power_cc3", "total power usage of alu1_cc3", genBuffer("ALU1_power_cc3"), NULL);
  stat_reg_formula(sdb, "alu2_power_cc3", "total power usage of alu2_cc3", genBuffer("ALU2_power_cc3"), NULL);
  stat_reg_formula(sdb, "falu1_power_cc3", "total power usage of falu1_cc3", genBuffer("FALU1_power_cc3"), NULL);
  stat_reg_formula(sdb, "falu2_power_cc3", "total power usage of falu2_cc3", genBuffer("FALU2_power_cc3"), NULL);
  stat_reg_formula(sdb, "iresultbus_power_cc3", "total power usage of iresultbus_cc3", genBuffer("IRESULTBUS_power_cc3"), NULL);
  stat_reg_formula(sdb, "fresultbus_power_cc3", "total power usage of fresultbus_cc3", genBuffer("FRESULTBUS_power_cc3"), NULL);
  stat_reg_formula(sdb, "icache_power_cc3", "total power usage of icache_cc3", genBuffer("ICACHE_power_cc3"), NULL);
  stat_reg_formula(sdb, "dcache_power_cc3", "total power usage of dcache_cc3", genBuffer("DCACHE_power_cc3"), NULL);
  stat_reg_formula(sdb, "dcache2_power_cc3", "total power usage of dcache2_cc3", genBuffer("DCACHE2_power_cc3"), NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_formula(sdb, "input_buf_power_cc3", "total power usage of input buffer_cc3", genBuffer("input_buf_power_cc3"), NULL);
  stat_reg_formula(sdb, "output_buf_power_cc3", "total power usage of output buffer_cc3", genBuffer("output_buf_power_cc3"), NULL);
#endif
  stat_reg_formula(sdb, "duplication_power_cc3", "total power usage of duplicator_cc3", genBuffer("DUPLICATION_power_cc3"), NULL);
  stat_reg_formula(sdb, "compare_power_cc3", "total power usage of comparator_cc3", genBuffer("COMPARE_power_cc3"), NULL);

  stat_reg_formula(sdb, "avg_rename_power_cc3", "avg power usage of rename unit_cc3", "rename_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_iqram_power_cc3", "avg power usage of int issueq ram_cc3", "iqram_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_fqram_power_cc3", "avg power usage of fp issueq ram_cc3", "fqram_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_clock1_power_cc3", "avg power usage of clock1_cc3", "clock1_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_clock2_power_cc3", "avg power usage of clock2_cc3", "clock2_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_clock3_power_cc3", "avg power usage of clock3_cc3", "clock3_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_rob1_power_cc3", "avg power usage of rob wakeup_cc3", "rob1_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_rob2_power_cc3", "avg power usage of rob ram_cc3", "rob2_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_bpred_power_cc3", "avg power usage of bpred unit_cc3", "bpred_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_iwakeup_power_cc3", "avg power usage of int wakeup_cc3", "iwakeup_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fwakeup_power_cc3", "avg power usage of fp wakeup_cc3", "fwakeup_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iselection_power_cc3", "avg power usage of int selection_cc3", "iselection_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fselection_power_cc3", "avg power usage of fp selection_cc3", "fselection_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_lsq_power_cc3", "avg power usage of instruction lsq_cc3", "lsq_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iregfile_power_cc3", "avg power usage of int regfile_cc3", "iregfile_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fregfile_power_cc3", "avg power usage of fp regfile_cc3", "fregfile_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_icache_power_cc3", "avg power usage of icache_cc3", "icache_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_dcache_power_cc3", "avg power usage of dcache_cc3", "dcache_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_dcache2_power_cc3", "avg power usage of dcache2_cc3", "dcache2_power_cc3/sim_cycle",  NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_formula(sdb, "avg_input_buf_power_cc3", "avg power usage of input_buf_cc3", "input_buf_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_output_buf_power_cc3", "avg power usage of output_buf_cc3", "output_buf_power_cc3/sim_cycle",  NULL);
#endif
  stat_reg_formula(sdb, "avg_alu1_power_cc3", "avg power usage of alu1_cc3", "alu1_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_alu2_power_cc3", "avg power usage of alu2_cc3", "alu2_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_falu1_power_cc3", "avg power usage of falu1_cc3", "falu1_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_falu2_power_cc3", "avg power usage of falu2_cc3", "falu2_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iresultbus_power_cc3", "avg power usage of iresultbus_cc3", "iresultbus_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fresultbus_power_cc3", "avg power usage of fresultbus_cc3", "fresultbus_power_cc3/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_duplication_power_cc3", "avg power usage of duplicator_cc3", "duplication_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_compare_power_cc3", "avg power usage of comparator_cc3", "compare_power_cc3/sim_cycle", NULL);
  stat_reg_formula(sdb, "total_power_cycle_cc3", "total power per cycle_cc3",xstr(TOTAL_CYCLE_POWER_CC3), NULL);
  stat_reg_formula(sdb, "avg_total_power_cycle_cc3", "average total power per cycle_cc3",xstr(TOTAL_CYCLE_POWER_CC3/sim_cycle), NULL);
  stat_reg_formula(sdb, "avg_total_power_insn_cc3", "average total power per insn_cc3(not accurate)",xstr(TOTAL_CYCLE_POWER_CC3/sim_total_insn), NULL);

  stat_reg_formula(sdb, "rename_fraction", "cc3 Percentage of power dissipated in rename",xstr((rename_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "iqram_fraction", "cc3 Percentage of power dissipated in int issueq ram",xstr((iqram_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "fqram_fraction", "cc3 Percentage of power dissipated in fp issueq ram",xstr((fqram_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "rob1_fraction", "cc3 Percentage of power dissipated in rob wakeup",xstr((rob1_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "rob2_fraction", "cc3 Percentage of power dissipated in rob ram",xstr((rob2_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "bpred_fraction", "cc3 Percentage of power dissipated in bpred",xstr((bpred_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "lsq_fraction", "cc3 Percentage of power dissipated in lsq",xstr((lsq_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "iwakeup_fraction", "cc3 Percentage of power dissipated in int wakeup",xstr((iwakeup_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "fwakeup_fraction", "cc3 Percentage of power dissipated in fp wakeup",xstr((fwakeup_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "iselection_fraction", "cc3 Percentage of power dissipated in int selection",xstr((iselection_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "fselection_fraction", "cc3 Percentage of power dissipated in fp selection",xstr((fselection_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "iregfile_fraction", "cc3 Percentage of power dissipated in int regfile",xstr((iregfile_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "fregfile_fraction", "cc3 Percentage of power dissipated in fp regfile",xstr((fregfile_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "icache_fraction", "cc3 Percentage of power dissipated in icache",xstr((icache_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "iresultbus_fraction", "cc3 Percentage of power dissipated in iresultbus",xstr((iresultbus_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "fresultbus_fraction", "cc3 Percentage of power dissipated in fresultbus",xstr((fresultbus_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "alu1_fraction", "cc3 Percentage of power dissipated in alu1",xstr((alu1_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "alu2_fraction", "cc3 Percentage of power dissipated in alu2",xstr((alu2_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "falu1_fraction", "cc3 Percentage of power dissipated in falu1",xstr((falu1_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "falu2_fraction", "cc3 Percentage of power dissipated in falu2",xstr((falu2_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "dcache_fraction", "cc3 Percentage of power dissipated in dcache",xstr((dcache_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "dcache2_fraction", "cc3 Percentage of power dissipated in dcache2",xstr((dcache2_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_formula(sdb, "input_buf_fraction", "cc3 Percentage of power dissipated in input_buf",xstr((input_buf_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "output_buf_fraction", "cc3 Percentage of power dissipated in output_buf",xstr((output_buf_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
#endif
  stat_reg_formula(sdb, "clock1_fraction", "cc3 Percentage of power dissipated in clock1",xstr((clock1_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "clock2_fraction", "cc3 Percentage of power dissipated in clock2",xstr((clock2_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);
  stat_reg_formula(sdb, "clock3_fraction", "cc3 Percentage of power dissipated in clock3",xstr((clock3_power_cc3*100)/TOTAL_CYCLE_POWER_CC3),NULL);

  stat_reg_counter(sdb, "total_rename_access", "total number accesses of rename unit", &total_rename_access, 0, NULL);
  stat_reg_counter(sdb, "total_iqram_access", "total number accesses of int issueq ram", &total_iqram_access, 0, NULL);
  stat_reg_counter(sdb, "total_fqram_access", "total number accesses of fp issueq ram", &total_fqram_access, 0, NULL);
  stat_reg_counter(sdb, "total_rob1_access", "total number accesses of rob wakeup", &total_rob1_access, 0, NULL);
  stat_reg_counter(sdb, "total_rob2_access", "total number accesses of rob wakeup", &total_rob2_access, 0, NULL);
  stat_reg_counter(sdb, "total_bpred_access", "total number accesses of bpred unit", &total_bpred_access, 0, NULL);
  stat_reg_counter(sdb, "total_iwakeup_access", "total number accesses of int wakeup", &total_iwakeup_access, 0, NULL);
  stat_reg_counter(sdb, "total_fwakeup_access", "total number accesses of fp wakeup", &total_fwakeup_access, 0, NULL);
  stat_reg_counter(sdb, "total_iselection_access", "total number accesses of int selection", &total_iselection_access, 0, NULL);
  stat_reg_counter(sdb, "total_fselection_access", "total number accesses of fp selection", &total_fselection_access, 0, NULL);
  stat_reg_counter(sdb, "total_lsq_access", "total number accesses of load/store queue", &total_lsq_access, 0, NULL);
  stat_reg_counter(sdb, "total_iregfile_access", "total number accesses of int regfile", &total_iregfile_access, 0, NULL);
  stat_reg_counter(sdb, "total_fregfile_access", "total number accesses of fp regfile", &total_fregfile_access, 0, NULL);
  stat_reg_counter(sdb, "total_icache_access", "total number accesses of icache", &total_icache_access, 0, NULL);
  stat_reg_counter(sdb, "total_dcache_access", "total number accesses of dcache", &total_dcache_access, 0, NULL);
  stat_reg_counter(sdb, "total_dcache2_access", "total number accesses of dcache2", &total_dcache2_access, 0, NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_counter(sdb, "total_input_buf_access", "total number accesses of input_buf", &total_input_buf_access, 0, NULL);
  stat_reg_counter(sdb, "total_output_buf_access", "total number accesses of output_buf", &total_output_buf_access, 0, NULL);
#endif
  stat_reg_counter(sdb, "total_alu1_access", "total number accesses of alu1", &total_alu1_access, 0, NULL);
  stat_reg_counter(sdb, "total_alu2_access", "total number accesses of alu2", &total_alu2_access, 0, NULL);
  stat_reg_counter(sdb, "total_falu1_access", "total number accesses of falu1", &total_falu1_access, 0, NULL);
  stat_reg_counter(sdb, "total_falu2_access", "total number accesses of falu2", &total_falu2_access, 0, NULL);
  stat_reg_counter(sdb, "total_iresultbus_access", "total number accesses of iresultbus", &total_iresultbus_access, 0, NULL);
  stat_reg_counter(sdb, "total_fresultbus_access", "total number accesses of fresultbus", &total_fresultbus_access, 0, NULL);
  stat_reg_counter(sdb, "total_duplication_access", "total number accesses of instruction duplication", &total_duplication_access, 0, NULL);
  stat_reg_counter(sdb, "total_compare_access", "total number accesses of instruction compare", &total_compare_access, 0, NULL);

  stat_reg_formula(sdb, "avg_rename_access", "avg number accesses of rename unit", "total_rename_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_iqram_access", "avg number accesses of int issueq ram", "total_iqram_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_fqram_access", "avg number accesses of fp issueq ram", "total_fqram_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_rob1_access", "avg number accesses of rob wakeup", "total_rob1_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_rob2_access", "avg number accesses of rob ram", "total_rob2_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_bpred_access", "avg number accesses of bpred unit", "total_bpred_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_iwakeup_access", "avg number accesses of int wakeup", "total_iwakeup_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fwakeup_access", "avg number accesses of fp wakeup", "total_fwakeup_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iselection_access", "avg number accesses of int selection", "total_iselection_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fselection_access", "avg number accesses of fp selection", "total_fselection_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_lsq_access", "avg number accesses of lsq", "total_lsq_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iregfile_access", "avg number accesses of int regfile", "total_iregfile_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fregfile_access", "avg number accesses of fp regfile", "total_fregfile_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_icache_access", "avg number accesses of icache", "total_icache_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_dcache_access", "avg number accesses of dcache", "total_dcache_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_dcache2_access", "avg number accesses of dcache2", "total_dcache2_access/sim_cycle",  NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_formula(sdb, "avg_input_buf_access", "avg number accesses of input_buf", "total_input_buf_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_output_buf_access", "avg number accesses of output_buf", "total_output_buf_access/sim_cycle", NULL);
#endif
  stat_reg_formula(sdb, "avg_alu1_access", "avg number accesses of alu1", "total_alu1_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_alu2_access", "avg number accesses of alu2", "total_alu2_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_falu1_access", "avg number accesses of falu1", "total_falu1_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_falu2_access", "avg number accesses of falu2", "total_falu2_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_iresultbus_access", "avg number accesses of iresultbus", "total_iresultbus_access/sim_cycle",  NULL);
  stat_reg_formula(sdb, "avg_fresultbus_access", "avg number accesses of fresultbus", "total_fresultbus_access/sim_cycle",  NULL);
  /* ejt: fault recovery counters added for Wattch */
  stat_reg_formula(sdb, "avg_duplication_access", "avg number accesses of instruction duplication", "total_duplication_access/sim_cycle", NULL);
  stat_reg_formula(sdb, "avg_compare_access", "avg number accesses of instruction compare", "total_compare_access/sim_cycle", NULL);

  stat_reg_counter(sdb, "max_rename_access", "max number accesses of rename unit", &max_rename_access, 0, NULL);
  stat_reg_counter(sdb, "max_iqram_access", "max number accesses of int issueq ram", &max_iqram_access, 0, NULL);
  stat_reg_counter(sdb, "max_fqram_access", "max number accesses of fp issueq ram", &max_fqram_access, 0, NULL);
  stat_reg_counter(sdb, "max_rob1_access", "max number accesses of rob wakeup", &max_rob1_access, 0, NULL);
  stat_reg_counter(sdb, "max_rob2_access", "max number accesses of rob ram", &max_rob2_access, 0, NULL);
  stat_reg_counter(sdb, "max_bpred_access", "max number accesses of bpred unit", &max_bpred_access, 0, NULL);
  stat_reg_counter(sdb, "max_iwakeup_access", "max number accesses of int wakeup", &max_iwakeup_access, 0, NULL);
  stat_reg_counter(sdb, "max_fwakeup_access", "max number accesses of fp wakeup", &max_fwakeup_access, 0, NULL);
  stat_reg_counter(sdb, "max_iselection_access", "max number accesses of int selection", &max_iselection_access, 0, NULL);
  stat_reg_counter(sdb, "max_fselection_access", "max number accesses of fp selection", &max_fselection_access, 0, NULL);
  stat_reg_counter(sdb, "max_lsq_access", "max number accesses of load/store queue", &max_lsq_access, 0, NULL);
  stat_reg_counter(sdb, "max_iregfile_access", "max number accesses of int regfile", &max_iregfile_access, 0, NULL);
  stat_reg_counter(sdb, "max_fregfile_access", "max number accesses of fp regfile", &max_fregfile_access, 0, NULL);
  stat_reg_counter(sdb, "max_icache_access", "max number accesses of icache", &max_icache_access, 0, NULL);
  stat_reg_counter(sdb, "max_dcache_access", "max number accesses of dcache", &max_dcache_access, 0, NULL);
  stat_reg_counter(sdb, "max_dcache2_access", "max number accesses of dcache2", &max_dcache2_access, 0, NULL);
#ifdef INPUT_OUTPUT_BUFFER
  stat_reg_counter(sdb, "max_input_buf_access", "max number accesses of input_buf", max_input_buf_access, 0,NULL);
  stat_reg_counter(sdb, "max_output_buf_access", "max number accesses of output_buf", max_output_buf_access, 0, NULL);
#endif
  stat_reg_counter(sdb, "max_alu1_access", "max number accesses of alu1", &max_alu1_access, 0, NULL);
  stat_reg_counter(sdb, "max_alu2_access", "max number accesses of alu2", &max_alu2_access, 0, NULL);
  stat_reg_counter(sdb, "max_falu1_access", "max number accesses of falu1", &max_falu1_access, 0, NULL);
  stat_reg_counter(sdb, "max_falu2_access", "max number accesses of falu2", &max_falu2_access, 0, NULL);
  stat_reg_counter(sdb, "max_iresultbus_access", "max number accesses of iresultbus", &max_iresultbus_access, 0, NULL);
  stat_reg_counter(sdb, "max_fresultbus_access", "max number accesses of fresultbus", &max_fresultbus_access, 0, NULL);
  /* ejt: fault recovery counters added for Wattch */
  stat_reg_counter(sdb, "max_duplication_access", "max number accesses of instruction duplication", &max_duplication_access, 0, NULL);
  stat_reg_counter(sdb, "max_compare_access", "max number accesses of instruction compare", &max_compare_access, 0, NULL);

  stat_reg_double(sdb, "max_cycle_power_cc3", "maximum cycle power usage of cc3", &max_cycle_power_cc3, 0, NULL);
}

/* this routine takes the number of rows and cols of an array structure
   and attemps to make it make it more of a reasonable circuit structure
   by trying to make the number of rows and cols as close as possible.
   (scaling both by factors of 2 in opposite directions).  it returns
   a scale factor which is the amount that the rows should be divided
   by and the columns should be multiplied by.
*/
int squarify(int rows, int cols)
{
  int scale_factor = 1;

  if(rows == cols)
    return 1;

  /*
  printf("init rows == %d\n",rows);
  printf("init cols == %d\n",cols);
  */

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;

    /*
    printf("rows == %d\n",rows);
    printf("cols == %d\n",cols);
    printf("scale_factor == %d (2^ == %d)\n\n",scale_factor,(int)pow(2.0,(double)scale_factor));
    */

    if (rows/2 <= cols)
      return((int)pow(2.0,(double)scale_factor));
    scale_factor++;
  }

  return 1;
}

/* could improve squarify to work when rows < cols */

double squarify_new(int rows, int cols)
{
  double scale_factor = 0.0;

  if(rows==cols)
    return(pow(2.0,scale_factor));

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;
    if (rows <= cols)
      return(pow(2.0,scale_factor));
    scale_factor++;
  }

  while(cols > rows) {
    rows = rows*2;
    cols = cols/2;
    if (cols <= rows)
      return(pow(2.0,scale_factor));
    scale_factor--;
  }

  return 1;

}

void dump_power_stats(power)
     power_result_type *power;
{
  double total_power;
  double bpred_power;
  double rename_power;
  double iqram_power;
  double fqram_power;
  double clock1_power;
  double clock2_power;
  double clock3_power;
  double rob1_power;
  double rob2_power;
  double rat_power;
  double dcl_power;
  double lsq_power;
  double iwakeup_power;
  double fwakeup_power;
  double iselection_power;
  double fselection_power;
  double lsq_wakeup_power;
  double lsq_rs_power;
  double iregfile_power;
  double fregfile_power;
  /* 050404 ejt: removed since variable is not used */
  // double reorder_power;
  double icache_power;
  double dcache_power;
  double dcache2_power;
  double dtlb_power;
  double itlb_power;
  double ambient_power = 2.0;

  /* ejt:  fault recovery power added for Wattch */
  double duplication_power;
  double compare_power;
#ifdef INPUT_OUTPUT_BUFFER
  double input_buf_power = power.input_buf_power;
  double output_buf_power = power.output_buf_power;
#endif

  icache_power = power->icache_power;

  dcache_power = power->dcache_power;

  dcache2_power = power->dcache2_power;

  itlb_power = power->itlb;
  dtlb_power = power->dtlb;

  bpred_power = power->btb + power->local_predict + power->global_predict + 
    power->chooser + power->ras + power->btb_gc + power->local_predict_gc + 
    power->global_predict_gc + power->chooser_gc + power->ras_gc;

  rat_power = power->rat_decoder + 
    power->rat_wordline + power->rat_bitline + power->rat_senseamp + power->rat_gc;

  dcl_power = power->dcl_compare + power->dcl_pencode;

  rename_power = power->rat_power + power->dcl_power + power->inst_decoder_power;
  rob1_power = power->rob1_power;
  rob2_power = power->rob2_power;
  clock1_power = power->clock1_power;
  clock2_power = power->clock2_power;
  clock3_power = power->clock3_power;
  iqram_power = power->iqram_power;
  fqram_power = power->fqram_power;

  iwakeup_power = power->iwakeup_tagdrive + power->iwakeup_tagmatch + 
    power->iwakeup_ormatch;
  fwakeup_power = power->fwakeup_tagdrive + power->fwakeup_tagmatch + 
    power->fwakeup_ormatch;
 
  iselection_power = power->iselection;
  fselection_power = power->fselection;

  lsq_rs_power = power->lsq_rs_decoder + 
    power->lsq_rs_wordline + power->lsq_rs_bitline + power->lsq_rs_senseamp
    + power->lsq_rs_gc;

  lsq_wakeup_power = power->lsq_wakeup_tagdrive + power->lsq_wakeup_gc +
    power->lsq_wakeup_tagmatch + power->lsq_wakeup_ormatch;

  lsq_power = lsq_wakeup_power + lsq_rs_power;


/*
  reorder_power = power->reorder_decoder + 
    power->reorder_wordline + power->reorder_bitline + 
    power->reorder_senseamp;
*/

  iregfile_power = power->iregfile_decoder + 
    power->iregfile_wordline + power->iregfile_bitline + 
    power->iregfile_senseamp + power->iregfile_gc;
  fregfile_power = power->fregfile_decoder + 
    power->fregfile_wordline + power->fregfile_bitline + 
    power->fregfile_senseamp + power->fregfile_gc;

  /* ejt:  fault recovery power added for Wattch */
  duplication_power = power->d_flip_flop_power + power->two_to_1_mux_power;
  compare_power = power->compare_power;

  total_power = bpred_power + iqram_power + fqram_power + clock1_power + clock2_power + clock3_power + rob1_power + rob2_power + rename_power + iwakeup_power + fwakeup_power + iregfile_power + fregfile_power +
    power->iresultbus + power->fresultbus + lsq_power + 
    icache_power + dcache_power + dcache2_power + 
    dtlb_power + itlb_power + /*power->clock_power +*/ power->ialu1_power +
    power->ialu2_power + power->falu1_power + power->falu2_power +
#ifdef INPUT_OUTPUT_BUFFER
    input_buf_power + output_buf_power +
#endif
    duplication_power + compare_power;

  fprintf(stderr,"\nProcessor Parameters:\n");
  fprintf(stderr,"Issue Width: %d\n",ruu_issue_width);
  fprintf(stderr,"Window Size: %d\n",RUU_size);
  fprintf(stderr,"Number of Virtual Registers: %d\n",MD_NUM_IREGS);
  fprintf(stderr,"Number of Physical Registers: %d\n",RUU_size);
  fprintf(stderr,"Datapath Width: %d\n",data_width);

  fprintf(stderr,"Total Power Consumption: %g\n",total_power+ambient_power);
  fprintf(stderr,"Branch Predictor Power Consumption: %g  (%.3g%%)\n",bpred_power,100*bpred_power/total_power);
  fprintf(stderr," branch target buffer power (W): %g\n",power->btb+power->btb_gc);
  fprintf(stderr," local predict power (W): %g\n",power->local_predict+power->local_predict_gc);
  fprintf(stderr," global predict power (W): %g\n",power->global_predict+power->global_predict_gc);
  fprintf(stderr," chooser power (W): %g\n",power->chooser+power->chooser_gc);
  fprintf(stderr," RAS power (W): %g\n",power->ras+power->ras_gc);
  fprintf(stderr,"Rename Logic Power Consumption: %g  (%.3g%%)\n",rename_power,100*rename_power/total_power);
  fprintf(stderr,"ROB Wakeup Power Consumption: %g  (%.3g%%)\n",rob1_power,100*rob1_power/total_power);
  fprintf(stderr,"ROB RAM Power Consumption: %g  (%.3g%%)\n",rob2_power,100*rob2_power/total_power);
  fprintf(stderr,"Clock1 Power Consumption: %g  (%.3g%%)\n",clock1_power,100*clock1_power/total_power);
  fprintf(stderr,"Clock2 Power Consumption: %g  (%.3g%%)\n",clock2_power,100*clock2_power/total_power);
  fprintf(stderr,"Clock3 Power Consumption: %g  (%.3g%%)\n",clock3_power,100*clock3_power/total_power);
  fprintf(stderr," Instruction Decode Power (W): %g\n",power->inst_decoder_power);
  fprintf(stderr," RAT decode_power (W): %g\n",power->rat_decoder);
  fprintf(stderr," RAT wordline_power (W): %g\n",power->rat_wordline);
  fprintf(stderr," RAT bitline_power (W): %g\n",power->rat_bitline);
  fprintf(stderr," RAT gc_power (W): %g\n",power->rat_gc);
  fprintf(stderr," DCL Comparators (W): %g\n",power->dcl_compare);
  fprintf(stderr,"Int wakeup Power Consumption: %g (%.3g%%)\n", iwakeup_power, 100*iwakeup_power/total_power);
  fprintf(stderr,"Fp wakeup Power Consumption: %g (%.3g%%)\n", fwakeup_power, 100*fwakeup_power/total_power);
  fprintf(stderr,"Int selection Power Consumption: %g (%.3g%%)\n", iselection_power, 100*iselection_power/total_power);
  fprintf(stderr,"Fp selection Power Consumption: %g (%.3g%%)\n", fselection_power, 100*fselection_power/total_power);
  fprintf(stderr,"Int issueq ram Power Consumption: %g (%.3g%%)\n", iqram_power, 100*iqram_power/total_power);
  fprintf(stderr,"Fp issueq ram Power Consumption: %g (%.3g%%)\n", fqram_power, 100*fqram_power/total_power);
  fprintf(stderr,"Load/Store Queue Power Consumption: %g  (%.3g%%)\n",lsq_power,100*lsq_power/total_power);
  fprintf(stderr," tagdrive (W): %g\n",power->lsq_wakeup_tagdrive);
  fprintf(stderr," tagmatch (W): %g\n",power->lsq_wakeup_tagmatch);
  fprintf(stderr," wakeup_gc (W): %g\n",power->lsq_wakeup_gc);
  fprintf(stderr," decode_power (W): %g\n",power->lsq_rs_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->lsq_rs_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->lsq_rs_bitline);
  fprintf(stderr," rs_gc (W): %g\n",power->lsq_rs_gc);
  fprintf(stderr,"Int Register File Power Consumption: %g  (%.3g%%)\n",iregfile_power,100*iregfile_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->iregfile_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->iregfile_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->iregfile_bitline);
  fprintf(stderr," regfile_gc (W): %g\n",power->iregfile_gc);
  fprintf(stderr,"Fp Register File Power Consumption: %g  (%.3g%%)\n",fregfile_power,100*fregfile_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->fregfile_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->fregfile_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->fregfile_bitline);
  fprintf(stderr," regfile_gc (W): %g\n",power->fregfile_gc);
  fprintf(stderr,"iResult Bus Power Consumption: %g  (%.3g%%)\n",power->iresultbus,100*power->iresultbus/total_power);
  fprintf(stderr,"fResult Bus Power Consumption: %g  (%.3g%%)\n",power->fresultbus,100*power->fresultbus/total_power);
  fprintf(stderr,"Total Clock Power: %g  (%.3g%%)\n",power->clock_power,100*power->clock_power/total_power);
  fprintf(stderr,"Int ALU1 Power: %g  (%.3g%%)\n",power->ialu1_power,100*power->ialu1_power/total_power);
  fprintf(stderr,"Int ALU2 Power: %g  (%.3g%%)\n",power->ialu2_power,100*power->ialu2_power/total_power);
  fprintf(stderr,"FP ALU1 Power: %g  (%.3g%%)\n",power->falu1_power,100*power->falu1_power/total_power);
  fprintf(stderr,"FP ALU2 Power: %g  (%.3g%%)\n",power->falu2_power,100*power->falu2_power/total_power);
  fprintf(stderr,"Instruction Cache Power Consumption: %g  (%.3g%%)\n",icache_power,100*icache_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->icache_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->icache_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->icache_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power->icache_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power->icache_tagarray);
  fprintf(stderr," icache_gc (W): %g\n",power->icache_gc);
  fprintf(stderr,"Itlb_power (W): %g (%.3g%%)\n",power->itlb,100*power->itlb/total_power);
  fprintf(stderr,"Data Cache Power Consumption: %g  (%.3g%%)\n",dcache_power,100*dcache_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->dcache_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->dcache_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->dcache_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power->dcache_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power->dcache_tagarray);
  fprintf(stderr," dcache_gc (W): %g\n",power->dcache_gc);
  fprintf(stderr,"Dtlb_power (W): %g (%.3g%%)\n",power->dtlb,100*power->dtlb/total_power);
  fprintf(stderr,"Level 2 Cache Power Consumption: %g (%.3g%%)\n",dcache2_power,100*dcache2_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->dcache2_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->dcache2_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->dcache2_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power->dcache2_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power->dcache2_tagarray);
  fprintf(stderr," dcache2_gc (W): %g\n",power->dcache2_gc);
#ifdef INPUT_OUTPUT_BUFFER
  fprintf(stderr,"input buffer power (W): %g (%.3g%%)\n", power->input_buf_power, 100*power->input_buf_power/total_power);
  fprintf(stderr,"output buffer power (W): %g (%.3g%%)\n", power->output_buf_power, 100*power->output_buf_power/total_power);
#endif

  /* ejt:  fault recovery power added for Wattch */

  /* 070104 ejt:  display instruction duplication power statistics */
  fprintf(stderr,"Instruction Duplication Power Consumption: %g (%.3g%%)\n",duplication_power,100*duplication_power/total_power);
  fprintf(stderr," d_flip_flop_power (W): %g\n",power->d_flip_flop_power);
  fprintf(stderr," two_to_1_mux_power (W): %g\n",power->two_to_1_mux_power);
  
  /* 070104 ejt:  display instruction compare power statistics */
  fprintf(stderr,"Instruction Comparator Power Consumption: %g (%.3g%%)\n",compare_power,100*compare_power/total_power);

}

/*======================================================================*/



/* 
 * This part of the code contains routines for each section as
 * described in the tech report.  See the tech report for more details
 * and explanations */

/*----------------------------------------------------------------------*/

double driver_size(double driving_cap, double desiredrisetime) {
  double nsize, psize;
  double Rpdrive; 

  Rpdrive = desiredrisetime/(driving_cap*log(VSINV)*-1.0);
  psize = restowidth(Rpdrive,PCH);
  nsize = restowidth(Rpdrive,NCH);
  if (psize > Wworddrivemax) {
    psize = Wworddrivemax;
  }
  if (psize < 4.0 * LSCALE)
    psize = 4.0 * LSCALE;

  return (psize);

}

/* Decoder delay:  (see section 6.1 of tech report) */

double array_decoder_power(rows,cols,predeclength,rports,wports,cache)
     int rows,cols;
     double predeclength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ceq=0;
  int numstack;
  int decode_bits=0;
  int ports;
  double rowsb;

  /* read and write ports are the same here */
  ports = rports + wports;

  rowsb = (double)rows;

  /* number of input bits to be decoded */
  decode_bits=ceil((logtwo(rowsb)));

  /* First stage: driving the decoders */

  /* This is the capacitance for driving one bit (and its complement).
     -There are #rowsb 3->8 decoders contributing gatecap.
     - 2.0 factor from 2 identical sets of drivers in parallel
  */
  Ceq = 2.0*(draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1)) +
    gatecap(Wdec3to8n+Wdec3to8p,10.0)*rowsb;

  /* There are ports * #decode_bits total */
  Ctotal+=ports*decode_bits*Ceq;

  if(verbose)
    fprintf(stderr,"Decoder -- Driving decoders            == %g\n",.3*Ctotal*Powerfactor);

  /* second stage: driving a bunch of nor gates with a nand 
     numstack is the size of the nor gates -- ie. a 7-128 decoder has
     3-input NAND followed by 3-input NOR  */

  numstack = ceil((1.0/3.0)*logtwo(rows));

  if (numstack<=0) numstack = 1;
  if (numstack>5) numstack = 5;

  /* There are #rowsb NOR gates being driven*/
  Ceq = (3.0*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
	 gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0)))*rowsb;

  Ctotal+=ports*Ceq;

  if(verbose)
    fprintf(stderr,"Decoder -- Driving nor w/ nand         == %g\n",.3*ports*Ceq*Powerfactor);

  /* Final stage: driving an inverter with the nor 
     (inverter preceding wordline driver) -- wordline driver is in the next section*/

  Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
	 numstack*draincap(WdecNORn,NCH,1)+
         draincap(WdecNORp,PCH,numstack));

  if(verbose)
    fprintf(stderr,"Decoder -- Driving inverter w/ nor     == %g\n",.3*ports*Ceq*Powerfactor);

  Ctotal+=ports*Ceq;

  /* assume Activity Factor == .3  */

  return(.3*Ctotal*Powerfactor);
}

double simple_array_decoder_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  double predeclength=0.0;
  return(array_decoder_power(rows,cols,predeclength,rports,wports,cache));
}


double array_wordline_power(rows,cols,wordlinelength,rports,wports,cache)
     int rows,cols;
     double wordlinelength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ceq=0;
  double Cline=0;
  double Cliner, Clinew=0;
  double desiredrisetime,psize,nsize;
  int ports;
  double colsb;

  ports = rports+wports;

  colsb = (double)cols;

  /* Calculate size of wordline drivers assuming rise time == Period / 8 
     - estimate cap on line 
     - compute min resistance to achieve this with RC 
     - compute width needed to achieve this resistance */

  desiredrisetime = Period/16;
  Cline = (gatecappass(Wmemcellr,1.0))*colsb + wordlinelength*CM3metal;
  psize = driver_size(Cline,desiredrisetime);
  
  /* how do we want to do p-n ratioing? -- here we just assume the same ratio 
     from an inverter pair  */
  nsize = psize * Wdecinvn/Wdecinvp; 
  
  if(verbose)
    fprintf(stderr,"Wordline Driver Sizes -- nsize == %f, psize == %f\n",nsize,psize);

  Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
    gatecap(nsize+psize,20.0);

  Ctotal+=ports*Ceq;

  if(verbose)
    fprintf(stderr,"Wordline -- Inverter -> Driver         == %g\n",ports*Ceq*Powerfactor);

  /* Compute caps of read wordline and write wordlines 
     - wordline driver caps, given computed width from above
     - read wordlines have 1 nmos access tx, size ~4
     - write wordlines have 2 nmos access tx, size ~2
     - metal line cap
  */

  Cliner = (gatecappass(Wmemcellr,(BitWidth-2*Wmemcellr)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));
  Clinew = (2.0*gatecappass(Wmemcellw,(BitWidth-2*Wmemcellw)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));

  if(verbose) {
    fprintf(stderr,"Wordline -- Line                       == %g\n",1e12*Cline);
    fprintf(stderr,"Wordline -- Line -- access -- gatecap  == %g\n",1e12*colsb*2*gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0));
    fprintf(stderr,"Wordline -- Line -- driver -- draincap == %g\n",1e12*draincap(nsize,NCH,1) + draincap(psize,PCH,1));
    fprintf(stderr,"Wordline -- Line -- metal              == %g\n",1e12*wordlinelength*CM3metal);
  }
  Ctotal+=rports*Cliner+wports*Clinew;

  /* AF == 1 assuming a different wordline is charged each cycle, but only
     1 wordline (per port) is actually used */

  return(Ctotal*Powerfactor);
}

double simple_array_wordline_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  double wordlinelength;
  int ports = rports + wports;
  wordlinelength = cols *  (RegCellWidth + 2 * ports * BitlineSpacing);
  return(array_wordline_power(rows,cols,wordlinelength,rports,wports,cache));
}


double array_bitline_power(rows,cols,bitlinelength,rports,wports,cache)
     int rows,cols;
     double bitlinelength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ccolmux=0;
  double Cbitrowr=0;
  double Cbitroww=0;
  double Cprerow=0;
  double Cwritebitdrive=0;
  double Cpregate=0;
  double Cliner=0;
  double Clinew=0;
  int ports;
  double rowsb;
  double colsb;

  double desiredrisetime, Cline, psize, nsize;

  ports = rports + wports;

  rowsb = (double)rows;
  colsb = (double)cols;

  /* Draincaps of access tx's */

  Cbitrowr = draincap(Wmemcellr,NCH,1);
  Cbitroww = draincap(Wmemcellw,NCH,1);

  /* Cprerow -- precharge cap on the bitline
     -simple scheme to estimate size of pre-charge tx's in a similar fashion
      to wordline driver size estimation.
     -FIXME: it would be better to use precharge/keeper pairs, i've omitted this
      from this version because it couldn't autosize as easily.
  */

  desiredrisetime = Period/8;

  Cline = rowsb*Cbitrowr+CM2metal*bitlinelength;
  psize = driver_size(Cline,desiredrisetime);

  /* compensate for not having an nmos pre-charging */
  psize = psize + psize * Wdecinvn/Wdecinvp; 

  if(verbose)
    printf("Cprerow auto   == %g (psize == %g)\n",draincap(psize,PCH,1),psize);

  Cprerow = draincap(psize,PCH,1);

  /* Cpregate -- cap due to gatecap of precharge transistors -- tack this
     onto bitline cap, again this could have a keeper */
  Cpregate = 4.0*gatecap(psize,10.0);
  global_clockcap+=rports*cols*2.0*Cpregate;
  gc[gcc++] = rports*cols*2.0*Cpregate;

  /* Cwritebitdrive -- write bitline drivers are used instead of the precharge
     stuff for write bitlines
     - 2 inverter drivers within each driver pair */

  Cline = rowsb*Cbitroww+CM2metal*bitlinelength;

  psize = driver_size(Cline,desiredrisetime);
  nsize = psize * Wdecinvn/Wdecinvp; 

  Cwritebitdrive = 2.0*(draincap(psize,PCH,1)+draincap(nsize,NCH,1));

  /* 
     reg files (cache==0) 
     => single ended bitlines (1 bitline/col)
     => AFs from pop_count
     caches (cache ==1)
     => double-ended bitlines (2 bitlines/col)
     => AFs = .5 (since one of the two bitlines is always charging/discharging)
  */

#ifdef STATIC_AF
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal+=(1.0-POPCOUNT_AF)*rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.3*wports*cols*(Clinew+Cwritebitdrive);
  } 
  else { 
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#else
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal += rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal += .3*wports*cols*(Clinew+Cwritebitdrive);
  } 
  else { 
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#endif

  if(verbose) {
    fprintf(stderr,"Bitline -- Precharge                   == %g\n",1e12*Cpregate);
    fprintf(stderr,"Bitline -- Line                        == %g\n",1e12*(Cliner+Clinew));
    fprintf(stderr,"Bitline -- Line -- access draincap     == %g\n",1e12*rowsb*Cbitrowr);
    fprintf(stderr,"Bitline -- Line -- precharge draincap  == %g\n",1e12*Cprerow);
    fprintf(stderr,"Bitline -- Line -- metal               == %g\n",1e12*bitlinelength*CM2metal);
    fprintf(stderr,"Bitline -- Colmux                      == %g\n",1e12*Ccolmux);

    fprintf(stderr,"\n");
  }


  if(cache==0)
    return(Ctotal*Powerfactor);
  else
    return(Ctotal*SensePowerfactor*.4);
  
}


double simple_array_bitline_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  double bitlinelength;

  int ports = rports + wports;

  bitlinelength = rows * (RegCellHeight + ports * WordlineSpacing);

  return (array_bitline_power(rows,cols,bitlinelength,rports,wports,cache));

}

/* estimate senseamp power dissipation in cache structures (Zyuban's method) */
double senseamp_power(int cols)
{
  return((double)cols * (Vdd*vdd_reduction)/8 * .5e-3);			/*110704mwr: the vdd is reducted by reduction factor*/
}

/* estimate comparator power consumption (this comparator is similar
   to the tag-match structure in a CAM */
double compare_cap(int compare_bits)
{
  double c1, c2;
  /* bottom part of comparator */
  c2 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
    draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);

  /* top part of comparator */
  c1 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2)+
		       draincap(Wcomppreequ,NCH,1)) +
    gatecap(WdecNORn,1.0)+
    gatecap(WdecNORp,3.0);

  return(c1 + c2);
}

/* power of depency check logic */
double dcl_compare_power(int compare_bits)
{
  double Ctotal;
  int num_comparators;
  
  num_comparators = ((ruu_decode_width) - 1) * ((ruu_decode_width));

  Ctotal = num_comparators * compare_cap(compare_bits);

  return(Ctotal*Powerfactor*AF);
}

double simple_array_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  if(cache==0)
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache));
  else
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache)+
	    senseamp_power(cols));
}


double cam_tagdrive(rows,cols,rports,wports)
     int rows,cols,rports,wports;
{
  double Ctotal, Ctlcap, Cblcap, Cwlcap;
  double taglinelength;
  double wordlinelength;
  double nsize, psize;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  taglinelength = rows * 
    (CamCellHeight + ports * MatchlineSpacing);

  wordlinelength = cols * 
    (CamCellWidth + ports * TaglineSpacing);

  /* Compute tagline cap */
  Ctlcap = Cmetal * taglinelength + 
    rows * gatecappass(Wcomparen2,2.0) +
    draincap(Wcompdrivern,NCH,1)+draincap(Wcompdriverp,PCH,1);

  /* Compute bitline cap (for writing new tags) */
  Cblcap = Cmetal * taglinelength +
    rows * draincap(Wmemcellr,NCH,2);

  /* autosize wordline driver */
  psize = driver_size(Cmetal * wordlinelength + 2 * cols * gatecap(Wmemcellr,2.0),Period/8);
  nsize = psize * Wdecinvn/Wdecinvp; 

  /* Compute wordline cap (for writing new tags) */
  Cwlcap = Cmetal * wordlinelength + 
    draincap(nsize,NCH,1)+draincap(psize,PCH,1) +
    2 * cols * gatecap(Wmemcellr,2.0);
    
  Ctotal += (rports * cols * 2 * Ctlcap) + 
    (wports * ((cols * 2 * Cblcap) + (rows * Cwlcap)));

  return(Ctotal*Powerfactor*AF);
}

double cam_tagmatch(rows,cols,rports,wports)
     int rows,cols,rports,wports;
{
  double Ctotal, Cmlcap;
  double matchlinelength;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  matchlinelength = cols * 
    (CamCellWidth + ports * TaglineSpacing);

  Cmlcap = 2 * cols * draincap(Wcomparen1,NCH,2) + 
    Cmetal * matchlinelength + draincap(Wmatchpchg,NCH,1) +
    gatecap(Wmatchinvn+Wmatchinvp,10.0) +
    gatecap(Wmatchnandn+Wmatchnandp,10.0);

  Ctotal += rports * rows * Cmlcap;

  global_clockcap += rports * rows * gatecap(Wmatchpchg,5.0);
  gc[gcc++] = rports * rows * gatecap(Wmatchpchg,5.0);
  
  /* noring the nanded match lines */
  if(ruu_issue_width >= 8)
    Ctotal += 2 * gatecap(Wmatchnorn+Wmatchnorp,10.0);

  return(Ctotal*Powerfactor*AF);
}

double cam_array(rows,cols,rports,wports)
     int rows,cols,rports,wports;
{
  return(cam_tagdrive(rows,cols,rports,wports) +
	 cam_tagmatch(rows,cols,rports,wports));
}


double selection_power(int win_entries)
{
  double Ctotal, Cor, Cpencode;
  int num_arbiter=1;

  Ctotal=0;

  while(win_entries > 4)
    {
      win_entries = (int)ceil((double)win_entries / 4.0);
      num_arbiter += win_entries;
    }

  Cor = 4 * draincap(WSelORn,NCH,1) + draincap(WSelORprequ,PCH,1);

  Cpencode = draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,1) + 
    2*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,2) + 
    3*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,3) + 
    4*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,4) + 
    4*gatecap(WSelEnn+WSelEnp,20.0) + 
    4*draincap(WSelEnn,NCH,1) + 4*draincap(WSelEnp,PCH,1);

  Ctotal += ruu_issue_width * num_arbiter*(Cor+Cpencode);

  return(Ctotal*Powerfactor*AF);
}

/* very rough clock power estimates */
double total_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double pipereg_clockcap=0;
  double global_buffercap = 0;
  double Clockpower;
  int k;

  double num_piperegs;

  int npreg_width = (int)ceil(logtwo((double)((IPREG_size+FPREG_size))));

  /* Assume say 8 stages (kinda low now).
     FIXME: this could be a lot better; user could input
     number of pipestages, etc  */

  /* assume 8 pipe stages and try to estimate bits per pipe stage */
  /* pipe stage 0/1 */
  num_piperegs = ruu_issue_width*inst_length + data_width;
  /* pipe stage 1/2 */
  num_piperegs += ruu_issue_width*(inst_length + 3 * RUU_size);
  /* pipe stage 2/3 */
  num_piperegs += ruu_issue_width*(inst_length + 3 * RUU_size);
  /* pipe stage 3/4 */
  num_piperegs += ruu_issue_width*(3 * npreg_width + pow2(opcode_length));
  /* pipe stage 4/5 */
  num_piperegs += ruu_issue_width*(2*data_width + pow2(opcode_length));
  /* pipe stage 5/6 */
  num_piperegs += ruu_issue_width*(data_width + pow2(opcode_length));
  /* pipe stage 6/7 */
  num_piperegs += ruu_issue_width*(data_width + pow2(opcode_length));
  /* pipe stage 7/8 */
  num_piperegs += ruu_issue_width*(data_width + pow2(opcode_length));

  /* assume 50% extra in control signals (rule of thumb) */
  num_piperegs = num_piperegs * 1.5;

  pipereg_clockcap = num_piperegs * 4*gatecap(10.0,0);

  /* estimate based on 3% of die being in clock metal */
  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  /* another estimate */
  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*gatecap(1000.0,10.0)+16*gatecap(200,10.0)+16*8*2*gatecap(100.0,10.00) + 2*gatecap(.29*1e6,10.0);
  /* global_clockcap is computed within each array structure for pre-charge tx's*/
  Ctotal = Cline+ /* global_clockcap+ */ pipereg_clockcap+global_buffercap;

  constant_factor = (Cline+global_buffercap)/(Cline+global_buffercap+pipereg_clockcap);

  if(verbose)
    fprintf(stderr,"num_piperegs == %f\n",num_piperegs);

  /* add I_ADD Clockcap and F_ADD Clockcap */
  Clockpower = Ctotal*Powerfactor /* + res_ialu*I_ADD_CLOCK + res_imult*I_MULT_CLOCK + res_fpalu*F_ADD_CLOCK + res_fpmult*F_MULT_CLOCK */ ;
  clockp = Clockpower;
  piperegg = pipereg_clockcap*Powerfactor;
  globbufg = global_buffercap*Powerfactor;
  clineg   = Cline*Powerfactor;

  if(verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Clockpower);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
    fprintf(stderr," Global Clock Cap (Implicit) (W): %g\n",pipereg_clockcap*Powerfactor);
    fprintf(stderr," Global Clock Cap (Explicit) (W): %g\n",global_clockcap*Powerfactor+ res_ialu*I_ADD_CLOCK+ res_imult*I_MULT_CLOCK + res_fpalu*F_ADD_CLOCK + res_fpmult*F_MULT_CLOCK);
    fprintf(stderr,"Breakup -\n  IALU1 : %g\n  IALU2 : %g\n  FALU1 : %g\n  FALU2 : %g\n",res_ialu*I_ADD_CLOCK, res_imult*I_MULT_CLOCK, res_fpalu*F_ADD_CLOCK, res_fpmult*F_MULT_CLOCK);
    for (k=0;k<gcc;k++) {
      fprintf(stderr,"  %s : %g\n",gcname[k],gc[k]*Powerfactor);
    }
  }
  return(Clockpower);

}

/* very rough global clock power estimates */
double global_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double global_buffercap = 0;

  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*gatecap(1000.0,10.0)+16*gatecap(200,10.0)+16*8*2*gatecap(100.0,10.00) + 2*gatecap(.29*1e6,10.0);
  Ctotal = Cline+global_buffercap;

  if(verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Ctotal*Powerfactor);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
  }

  return(Ctotal*Powerfactor);

}


double compute_resultbus_power()
{
  double Ctotal, Cline;

  double regfile_height;

  /* compute size of result bus tags */
  int npreg_width = (int)ceil(logtwo((double)((IPREG_size+FPREG_size))));

  Ctotal=0;

  regfile_height = (IPREG_size) * (RegCellHeight + 
			       WordlineSpacing * 3 * ruu_issue_width); 

  /* assume num alu's == ialu  (FIXME: generate a more detailed result bus network model*/
  Cline = Cmetal * (regfile_height + .5 * (res_ialu/CLUSTERS) * 3200.0 * LSCALE);

  /* or use result bus length measured from 21264 die photo */
  /*  Cline = Cmetal * 3.3*1000;*/

  /* Assume ruu_issue_width result busses -- power can be scaled linearly
     for number of result busses (scale by writeback_access) */
  Ctotal += 2.0 * (data_width + npreg_width) * (ruu_issue_width)* Cline;

#ifdef STATIC_AF
  return(Ctotal*Powerfactor*AF);
#else
  return(Ctotal*Powerfactor);
#endif
  
}


/* 062904 ejt: function to compute capacitance of duplication at dispatch stage
               used for fault recovery, duplication comprises of
	       edge-triggered d flip-flop and 2 to 1 multiplexer */  
/* edge-triggered d flip-flop is based on the design described in J. Yuan,
   C. Svensson, "High Speed CMOS Circuit Technique", IEEE Journal of Solid State
   Circuits, Vol SC-24, pp 62-70, 1989. */
double d_flip_flop_cap (int dispatch_duped_bits)
{
  double c_dff;
  
  /* design and transistor sizing based on F. K. Grkaynak Aries project,
     turquoise.wpi.edu/aries/ */
  c_dff = (dispatch_duped_bits) *
          /* capacitance of Q1, Q2, Q3 */
          (draincap(WDFFn,NCH,1) + draincap(WDFFp,PCH,2) +
           draincap(WDFFp,PCH,1) + gatecap(WDFFn,10) +

          /* capacitance of Q4, Q5, Q6 */
           draincap(WDFFn,NCH,1) + draincap(WDFFn,NCH,2) +
	   draincap(WDFFp,PCH,1) + gatecap(WDFFn + WDFFp,20) +

          /* capacitance of Q7, Q8, Q9 */
           draincap(WDFFn,NCH,1) + draincap(WDFFn,NCH,2) +
	   draincap(WDFFp,PCH,1) + gatecap(WDFFn + WDFFp,20) +

          /* capacitance of Q10, Q11 */
           draincap(WDFFn,NCH,1) + draincap(WDFFp,PCH,1) +
           gatecap(WDFFn + WDFFp,0.0)	
          );

  return(c_dff);
}       
 
/* 2 to 1 multiplexer is based on the design using two transmission gates */
double two_to_1_mux_cap (int dispatch_duped_bits)
{
  double c_21mux;
  
  c_21mux = (dispatch_duped_bits) *
            /* capacitance of Q1, Q2 */
            (draincap(W2to1muxn,NCH,1) + draincap(W2to1muxp,PCH,1) +
	     gatecap(W2to1muxn + W2to1muxp,20) +

            /* capacitance of Q3, Q4 */
             draincap(W2to1muxn,PCH,1) + draincap(W2to1muxp,NCH,1) +
	     gatecap(W2to1muxn + W2to1muxp,0.0) +
       
            /* capacitance of Q5, Q6 */
             draincap(W2to1muxn,PCH,1) + draincap(W2to1muxp,NCH,1) +
	     gatecap(W2to1muxn + W2to1muxp,0.0)
            );

  return(c_21mux);
}  

/* 070104 ejt: function to compute power consumed by d flip flop at dispatch
               stage used for fault recovery */ 
double d_flip_flop_power ()
{
  double C_dff_total;
  double d_flip_flop_pwr;
  
  C_dff_total = d_flip_flop_cap(DISPATCH_DUPED_BITS);

#ifdef STATIC_AF  
  d_flip_flop_pwr = C_dff_total * Powerfactor * AF;
#else
  d_flip_flop_pwr = C_dff_total * Powerfactor;
#endif
  
  return(d_flip_flop_pwr);
  
}

/* 070104 ejt: function to compute power consumed by 2 to 1 multiplexer at
               dispatch stage used for fault recovery */ 
double two_to_1_mux_power ()
{
  double C_21mux_total;
  double two_to_1_mux_pwr;
  
  C_21mux_total = two_to_1_mux_cap(DISPATCH_DUPED_BITS);

#ifdef STATIC_AF  
  two_to_1_mux_pwr = C_21mux_total * Powerfactor * AF;
#else
  two_to_1_mux_pwr = C_21mux_total * Powerfactor;
#endif
  
  return(two_to_1_mux_pwr);
  
}

/* 062604 ejt: function to compute capacitance of comparator at commit stage
               used for fault recovery.  comparator is based on the design
               described in cacti, section 6.6 */
/* estimate comparator power consumption (this comparator is similar
   to the tag-match structure in a CAM */
double commit_compare_cap (int commit_compare_bits)
{
  double c1, c2, c3;
  /* bottom part of comparator */
  c2 = (commit_compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
    draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);

  /* top part of comparator */
  c1 = (commit_compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2)+
		       draincap(Wcomppreequ,NCH,1)) +
    gatecap(WdecNORn,1.0)+
    gatecap(WdecNORp,3.0);

  /* inverters used to get negated values of instruction and its duplicate
     for comparator, we assume inverter size to be identical to Wcompinv1 */
  c3 = (commit_compare_bits)*
       2 * (draincap(Wcompinvp1,PCH,1) +  draincap(Wcompinvn1,NCH,1) +
	     gatecap(Wcompinvp1 + Wcompinvn1,0.0)); 
                 
  return(c1 + c2 + c3);
}

/* 062504 ejt: function to compute power consumed by comparator at commit stage
               used for fault recovery */
double commit_compare_power ()
{
  double C_compare_total;
  double compare_pwr;
  
  C_compare_total = commit_compare_cap(COMMIT_COMPARE_BITS);

#ifdef STATIC_AF  
  compare_pwr = C_compare_total * Powerfactor * AF;
#else
  compare_pwr = C_compare_total * Powerfactor;
#endif
  
  return(compare_pwr);
  
}


void calculate_power(power_result_type *power)
{
  double clockpower;
  double predeclength, wordlinelength, bitlinelength;
  int ndwl, ndbl, nspd, ntwl, ntbl, ntspd, c,b,a,cache, rowsb, colsb;
  int trowsb, tcolsb, tagsize;
  int va_size = 48;

  gcc = 0;

	 Powerfactor = power->Powerfactor;
	 SensePowerfactor = power->SensePowerfactor;
	 vdd_reduction = power->vdd_reduction;

	 POWER_SCALE =   (GEN_POWER_SCALE * NORMALIZE_SCALE * Powerfactor);
	 I_ADD       =   ((.37 - .091) * POWER_SCALE);
	 I_ADD32     =   (((.37 - .091)/2)*POWER_SCALE);
	 I_MULT16    =   ((.31-.095)*POWER_SCALE);
	 I_SHIFT     =   ((.21-.089)*POWER_SCALE);
	 I_LOGIC     =   ((.04-.015)*POWER_SCALE);
	 F_ADD       =   ((1.307-.452)*POWER_SCALE);
	 F_MULT      =   ((1.307-.452)*POWER_SCALE);

	 I_ADD_CLOCK  =  (.091*POWER_SCALE);
	 I_MULT_CLOCK =  (.095*POWER_SCALE);
	 I_SHIFT_CLOCK=  (.089*POWER_SCALE);
	 I_LOGIC_CLOCK=  (.015*POWER_SCALE);
	 F_ADD_CLOCK  =  (.452*POWER_SCALE);
	 F_MULT_CLOCK =  (.452*POWER_SCALE);


	 if(vdd_reduction == 1)			
	 	Period = (1/Mhz);
	 else
	 	Period = (2/Mhz);	/*111104mwr: this will need to be changed later, its fixed 1 or 2 now, need to be made variable*/

 //*********************************************************************
//add by ctb 2013-03-04
//caclulate the perido with differente vdd_reduction
	 Period=1/(Mhz*vdd_reduction*vdd_reduction);
//***********end********************************************************

  int npreg_width = (int)ceil(logtwo((double)((IPREG_size)+(FPREG_size))));

  /* these variables are needed to use Cacti to auto-size cache arrays
     (for optimal delay) */
  time_result_type time_result;
  time_parameter_type time_parameters;

  /* used to autosize other structures, like bpred tables */
  int scale_factor;

  global_clockcap = 0;

  cache=0;

  /* 062304 ejt: basic comparator building block power computations */
  /* 062504 ejt: the comparator is based on a design described in cacti,
                 section 6.6 */
  power->compare_power = commit_compare_power ();

  /* 070104 ejt: function to compute capacitance of duplication at dispatch
                 stage used for fault recovery, duplication comprises of
	         edge-triggered d flip-flop and 2 to 1 multiplexer */
  power->d_flip_flop_power = d_flip_flop_power ();
  power->two_to_1_mux_power = two_to_1_mux_power ();
  power->duplication_power = power->d_flip_flop_power + power->two_to_1_mux_power;

  /* FIXME: ALU power is a simple constant, it would be better
     to include bit AFs and have different numbers for different
     types of operations */
  power->ialu1_power = (res_ialu /CLUSTERS)* (I_ADD+I_ADD_CLOCK);
  power->ialu2_power = (res_imult /CLUSTERS)* (I_MULT16+I_MULT_CLOCK);
  power->falu1_power = (res_fpalu /CLUSTERS)* (F_ADD+F_ADD_CLOCK);
  power->falu2_power = (res_fpmult /CLUSTERS)* (F_MULT+F_MULT_CLOCK);


  nvreg_width = (int)ceil(logtwo((double)(MD_NUM_IREGS*2.0)));
  npreg_width = (int)ceil(logtwo((double)((IPREG_size+FPREG_size))));


  /* RAT has shadow bits stored in each cell, this makes the
     cell size larger than normal array structures, so we must
     compute it here */

  predeclength = 2*MD_NUM_IREGS *
    (RatCellHeight + 3 * (ruu_decode_width) * WordlineSpacing);

  wordlinelength = npreg_width *
    (RatCellWidth +
     6 * (ruu_decode_width) * BitlineSpacing +
     RatShiftRegWidth*RatNumShift);

  bitlinelength = 2*MD_NUM_IREGS * (RatCellHeight + 3 * (ruu_decode_width) * WordlineSpacing);

  if(verbose)
    fprintf(stderr,"rat power stats\n");
  power->rat_decoder = array_decoder_power(2*MD_NUM_IREGS,npreg_width,predeclength,2*(ruu_decode_width),(ruu_decode_width),cache);
  power->rat_wordline = array_wordline_power(2*MD_NUM_IREGS,npreg_width,wordlinelength,2*(ruu_decode_width),(ruu_decode_width),cache);
  power->rat_bitline = array_bitline_power(2*MD_NUM_IREGS,npreg_width,bitlinelength,2*(ruu_decode_width),(ruu_decode_width),cache);

  /* 031004 ejt: modified gcname+gcc-1 to gcname[gcc-1] to correct for
     pointer type */
  strcpy(gcname[gcc-1],"rat");
  power->rat_gc = gc[gcc-1]*Powerfactor;
  power->rat_senseamp = 0;

  power->dcl_compare = dcl_compare_power(nvreg_width);
  power->dcl_pencode = 0;
  power->inst_decoder_power = (ruu_decode_width) * simple_array_decoder_power(opcode_length,1,1,1,cache);

  power->iwakeup_tagdrive =cam_tagdrive((IIQ_size),npreg_width,ruu_issue_width,ruu_issue_width);
  power->iwakeup_tagmatch =cam_tagmatch((IIQ_size),npreg_width,ruu_issue_width,ruu_issue_width);
  strcpy(gcname[gcc-1],"IIQ");
  power->iwakeup_gc = gc[gcc-1]*Powerfactor;
  power->iwakeup_ormatch =0;

  power->fwakeup_tagdrive =cam_tagdrive((FIQ_size),npreg_width,ruu_issue_width,ruu_issue_width);
  power->fwakeup_tagmatch =cam_tagmatch((FIQ_size),npreg_width,ruu_issue_width,ruu_issue_width);
  strcpy(gcname[gcc-1],"FIQ");
  power->fwakeup_gc = gc[gcc-1]*Powerfactor;
  power->fwakeup_ormatch =0;

  power->iselection = selection_power(IIQ_size);
  power->fselection = selection_power(FIQ_size);


  predeclength = (IPREG_size) * (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  wordlinelength = data_width *
    (RegCellWidth +
     6 * ruu_issue_width * BitlineSpacing);

  bitlinelength = (IPREG_size )* (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  if(verbose)
    fprintf(stderr,"iregfile power stats\n");

  power->iregfile_decoder = array_decoder_power((IPREG_size),data_width,predeclength,2*ruu_issue_width,ruu_issue_width,cache);
  power->iregfile_wordline = array_wordline_power((IPREG_size),data_width,wordlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  power->iregfile_bitline = array_bitline_power((IPREG_size),data_width,bitlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  strcpy(gcname[gcc-1],"iregfile");
  power->iregfile_gc = gc[gcc-1]*Powerfactor;
  power->iregfile_senseamp =0;

  predeclength = (FPREG_size) * (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  wordlinelength = data_width *
    (RegCellWidth +
     6 * ruu_issue_width * BitlineSpacing);

  bitlinelength = (FPREG_size) * (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  if(verbose)
    fprintf(stderr,"fregfile power stats\n");

  power->fregfile_decoder = array_decoder_power((FPREG_size),data_width,predeclength,2*ruu_issue_width,ruu_issue_width,cache);
  power->fregfile_wordline = array_wordline_power((FPREG_size),data_width,wordlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  power->fregfile_bitline = array_bitline_power((FPREG_size),data_width,bitlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  strcpy(gcname[gcc-1],"fregfile");
  power->fregfile_gc = gc[gcc-1]*Powerfactor;
  power->fregfile_senseamp =0;

  /* addresses go into lsq tag's */
  power->lsq_wakeup_tagdrive =cam_tagdrive((LSQ_size),data_width,(res_memport/CLUSTERS),(res_memport/CLUSTERS));
  power->lsq_wakeup_tagmatch =cam_tagmatch((LSQ_size),data_width,(res_memport/CLUSTERS),(res_memport/CLUSTERS));
  strcpy(gcname[gcc-1],"LSQ");
  power->lsq_wakeup_gc = gc[gcc-1]*Powerfactor;
  power->lsq_wakeup_ormatch =0;

  wordlinelength = data_width *
    (RegCellWidth +
     4 *( res_memport/CLUSTERS) * BitlineSpacing);

  bitlinelength = (IIQ_size) * (RegCellHeight + 4 *( res_memport/CLUSTERS) * WordlineSpacing);

  /* rs's hold data */
  if(verbose)
    fprintf(stderr,"lsq station power stats\n");
  power->lsq_rs_decoder = array_decoder_power((LSQ_size),data_width,predeclength,(res_memport/CLUSTERS),(res_memport/CLUSTERS),cache);
  power->lsq_rs_wordline = array_wordline_power((LSQ_size),data_width,wordlinelength,(res_memport/CLUSTERS),(res_memport/CLUSTERS),cache);
  power->lsq_rs_bitline = array_bitline_power((LSQ_size),data_width,bitlinelength,(res_memport/CLUSTERS),(res_memport/CLUSTERS),cache);
  strcpy(gcname[gcc-1],"lsq");
  power->lsq_rs_gc = gc[gcc-1]*Powerfactor;
  power->lsq_rs_senseamp =0;

  power->iresultbus = compute_resultbus_power();
  power->fresultbus = compute_resultbus_power();

  /* Load cache values into what cacti is expecting */
  time_parameters.cache_size = btb_config[0] * (data_width/8) * btb_config[1]; /* C */
  time_parameters.block_size = (data_width/8); /* B */
  time_parameters.associativity = btb_config[1]; /* A */
  time_parameters.number_of_sets = btb_config[0]; /* C/(B*A) */

  /* have Cacti compute optimal cache config */
  calculate_time(&time_result,&time_parameters);
  output_data(&time_result,&time_parameters);

  /* extract Cacti results */
  ndwl=time_result.best_Ndwl;
  ndbl=time_result.best_Ndbl;
  nspd=time_result.best_Nspd;
  ntwl=time_result.best_Ntwl;
  ntbl=time_result.best_Ntbl;
  ntspd=time_result.best_Ntspd;
  c = time_parameters.cache_size;
  b = time_parameters.block_size;
  a = time_parameters.associativity;

  cache=1;

  /* Figure out how many rows/cols there are now */
  rowsb = c/(8*b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  if(verbose) {
    fprintf(stderr,"%d KB %d-way btb (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"btb power stats\n");
  power->btb = ndwl*ndbl*(array_decoder_power(rowsb,colsb,predeclength,1,1,cache) + array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache) + array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache) + senseamp_power(colsb));
  strcpy(gcname[gcc-1],"btb");
  power->btb_gc = gc[gcc-1]*Powerfactor;

  cache=1;

  scale_factor = squarify(twolev_config[0],twolev_config[2]);
  predeclength = (twolev_config[0] / scale_factor)* (RegCellHeight + WordlineSpacing);
  wordlinelength = twolev_config[2] * scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = (twolev_config[0] / scale_factor) * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"local predict power stats\n");

  power->local_predict = array_decoder_power(twolev_config[0]/scale_factor,twolev_config[2]*scale_factor,predeclength,1,1,cache) + array_wordline_power(twolev_config[0]/scale_factor,twolev_config[2]*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(twolev_config[0]/scale_factor,twolev_config[2]*scale_factor,bitlinelength,1,1,cache) + senseamp_power(twolev_config[2]*scale_factor);
  strcpy(gcname[gcc-1],"locpred");
  power->local_predict_gc = gc[gcc-1]*Powerfactor;

  scale_factor = squarify(twolev_config[1],3);

  predeclength = (twolev_config[1] / scale_factor)* (RegCellHeight + WordlineSpacing);
  wordlinelength = 3 * scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = (twolev_config[1] / scale_factor) * (RegCellHeight + WordlineSpacing);


  if(verbose)
    fprintf(stderr,"local predict power stats\n");
  power->local_predict += array_decoder_power(twolev_config[1]/scale_factor,3*scale_factor,predeclength,1,1,cache) + array_wordline_power(twolev_config[1]/scale_factor,3*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(twolev_config[1]/scale_factor,3*scale_factor,bitlinelength,1,1,cache) + senseamp_power(3*scale_factor);
  strcpy(gcname[gcc-1],"locpred");
  power->local_predict_gc += gc[gcc-1]*Powerfactor;


  if(verbose)
    fprintf(stderr,"bimod_config[0] == %d\n",bimod_config[0]);

  scale_factor = squarify(bimod_config[0],2);

  predeclength = bimod_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);
  wordlinelength = 2*scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = bimod_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);


  if(verbose)
    fprintf(stderr,"global predict power stats\n");
  power->global_predict = array_decoder_power(bimod_config[0]/scale_factor,2*scale_factor,predeclength,1,1,cache) + array_wordline_power(bimod_config[0]/scale_factor,2*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(bimod_config[0]/scale_factor,2*scale_factor,bitlinelength,1,1,cache) + senseamp_power(2*scale_factor);
  strcpy(gcname[gcc-1],"glopred");
  power->global_predict_gc = gc[gcc-1]*Powerfactor;

  scale_factor = squarify(comb_config[0],2);

  predeclength = comb_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);
  wordlinelength = 2*scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = comb_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"chooser predict power stats\n");
  power->chooser = array_decoder_power(comb_config[0]/scale_factor,2*scale_factor,predeclength,1,1,cache) + array_wordline_power(comb_config[0]/scale_factor,2*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(comb_config[0]/scale_factor,2*scale_factor,bitlinelength,1,1,cache) + senseamp_power(2*scale_factor);
  strcpy(gcname[gcc-1],"choopred");
  power->chooser_gc = gc[gcc-1]*Powerfactor;

  if(verbose)
    fprintf(stderr,"RAS predict power stats\n");
  power->ras = simple_array_power(ras_size,data_width,1,1,0);
  strcpy(gcname[gcc-1],"ras");
  power->ras_gc = gc[gcc-1]*Powerfactor;

  /*020205mwr*/
  tagsize = va_size - ((int)logtwo(cache_dl1[0]->nsets) + (int)logtwo(cache_dl1[0]->bsize));
//  tagsize = va_size - ((int)logtwo(cache_dl1->nsets) + (int)logtwo(cache_dl1->bsize));

  if(verbose)
    fprintf(stderr,"dtlb predict power stats\n");
  power->dtlb = (res_memport/CLUSTERS)*(cam_array(dtlb->nsets, va_size - (int)logtwo((double)dtlb->bsize),1,1) + simple_array_power(dtlb->nsets,tagsize,1,1,cache));
  strcpy(gcname[gcc-1],"dtlb");
  strcpy(gcname[gcc-2],"dtlb");
  power->dtlb_gc = gc[gcc-1]*Powerfactor+gc[gcc-2]*Powerfactor;
  power->dtlb += power->dtlb_gc;

  if (verbose)
    fprintf(stderr,"ROB power stats\n");

  power->rob1_power = cam_array(RUU_size, (int)logtwo((double)((IPREG_size+FPREG_size))), 0, ruu_commit_width);
  strcpy(gcname[gcc-1],"rob1");
  power->rob1_power += gc[gcc-1]*Powerfactor;

  power->rob2_power = simple_array_power(RUU_size, 20 /* slightly arbitrary, as I'm not sure if there are additional control data here */, ruu_commit_width, (ruu_decode_width), cache);
  strcpy(gcname[gcc-1],"rob2");
  power->rob2_power += gc[gcc-1]*Powerfactor;

  if (verbose)
    fprintf(stderr,"Issueq ram power stats\n");

  
  if(INORDER_CHECKER && CHECKER_NUMBER && vdd_reduction != 1){			//if vdd_reduction = 1 then main processor

	power->iqram_power = simple_array_power(1, 100 /* slightly arbitrary, as I'm not sure what the contents of the issueq are */, ruu_issue_width, (ruu_decode_width), cache);
	strcpy(gcname[gcc-1],"iqram");
	power->iqram_power += gc[gcc-1]*Powerfactor;



	power->fqram_power = simple_array_power(1, 100 /* slightly arbitrary, as I'm not sure what the contents of the issueq are */, ruu_issue_width, (ruu_decode_width), cache);
	strcpy(gcname[gcc-1],"fqram");
	power->fqram_power += gc[gcc-1]*Powerfactor;
   
  	power->iqram_power -= simple_array_decoder_power((IIQ_size), 100 /* slightly arbitrary, as I'm not sure what the contents of the issueq are */, ruu_issue_width, (ruu_decode_width), cache);
  	power->fqram_power -= simple_array_decoder_power((FIQ_size), 100 /* slightly arbitrary, as I'm not sure what the contents of the issueq are */, ruu_issue_width, (ruu_decode_width), cache);
   
   }
   else{
    
	power->iqram_power = simple_array_power((IIQ_size), 100 /* slightly arbitrary, as I'm not sure what the contents of the issueq are */, ruu_issue_width, (ruu_decode_width), cache);
	strcpy(gcname[gcc-1],"iqram");
	power->iqram_power += gc[gcc-1]*Powerfactor;



	power->fqram_power = simple_array_power((FIQ_size), 100 /* slightly arbitrary, as I'm not sure what the contents of the issueq are */, ruu_issue_width, (ruu_decode_width), cache);
	strcpy(gcname[gcc-1],"fqram");
	power->fqram_power += gc[gcc-1]*Powerfactor;
   
   
   
   }
  
  
  tagsize = va_size - ((int)logtwo(cache_il1[0]->nsets) + (int)logtwo(cache_il1[0]->bsize));

  predeclength = itlb->nsets * (RegCellHeight + WordlineSpacing);
  wordlinelength = logtwo((double)itlb->bsize) * (RegCellWidth + BitlineSpacing);
  bitlinelength = itlb->nsets * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"itlb predict power stats\n");
  power->itlb = cam_array(itlb->nsets, va_size - (int)logtwo((double)itlb->bsize),1,1) + simple_array_power(itlb->nsets,tagsize,1,1,cache);
  strcpy(gcname[gcc-1],"itlb");
  strcpy(gcname[gcc-2],"itlb");
  power->itlb_gc = gc[gcc-1]*Powerfactor+gc[gcc-2]*Powerfactor;
  power->itlb += power->itlb_gc;


  cache=1;

/*020205mwr*/
  time_parameters.cache_size = cache_il1[0]->nsets * cache_il1[0]->bsize * cache_il1[0]->assoc; /* C */
  time_parameters.block_size = cache_il1[0]->bsize; /* B */
  time_parameters.associativity = cache_il1[0]->assoc; /* A */
  time_parameters.number_of_sets = cache_il1[0]->nsets; /* C/(B*A) */
  
//  time_parameters.cache_size = cache_il1->nsets * cache_il1->bsize * cache_il1->assoc; /* C */
//  time_parameters.block_size = cache_il1->bsize; /* B */
//  time_parameters.associativity = cache_il1->assoc; /* A */
//  time_parameters.number_of_sets = cache_il1->nsets; /* C/(B*A) */

  calculate_time(&time_result,&time_parameters);
  output_data(&time_result,&time_parameters);

  ndwl=time_result.best_Ndwl;
  ndbl=time_result.best_Ndbl;
  nspd=time_result.best_Nspd;
  ntwl=time_result.best_Ntwl;
  ntbl=time_result.best_Ntbl;
  ntspd=time_result.best_Ntspd;

  c = time_parameters.cache_size;
  b = time_parameters.block_size;
  a = time_parameters.associativity;

  rowsb = c/(8*b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

/*020205mwr*/
//  tagsize = va_size - ((int)logtwo(cache_il1->nsets) + (int)logtwo(cache_il1->bsize));
  tagsize = va_size - ((int)logtwo(cache_il1[0]->nsets) + (int)logtwo(cache_il1[0]->bsize));
  trowsb = c/(8*b*a*ntbl*ntspd);
  tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

  if(verbose) {
    fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
    fprintf(stderr,"tagsize == %d\n",tagsize);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"icache power stats\n");
  power->icache_decoder = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
  power->icache_wordline = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
  power->icache_bitline = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
  strcpy(gcname[gcc-1],"icache");
  power->icache_senseamp = ndwl*ndbl*senseamp_power(colsb);
  power->icache_tagarray = ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));
  strcpy(gcname[gcc-1],"il1_tag");
  power->icache_gc = gc[gcc-1]*Powerfactor+gc[gcc-2]*Powerfactor;

  power->icache_power = power->icache_decoder + power->icache_wordline + power->icache_bitline + power->icache_senseamp + power->icache_tagarray + power->icache_gc;

  
  /*020205mwr*/
  time_parameters.cache_size = cache_dl1[0]->nsets * cache_dl1[0]->bsize * cache_dl1[0]->assoc; /* C */
  time_parameters.block_size = cache_dl1[0]->bsize; /* B */
  time_parameters.associativity = cache_dl1[0]->assoc; /* A */
  time_parameters.number_of_sets = cache_dl1[0]->nsets; /* C/(B*A) */
  
  
//  time_parameters.cache_size = cache_dl1->nsets * cache_dl1->bsize * cache_dl1->assoc; /* C */
//  time_parameters.block_size = cache_dl1->bsize; /* B */
//  time_parameters.associativity = cache_dl1->assoc; /* A */
//  time_parameters.number_of_sets = cache_dl1->nsets; /* C/(B*A) */

  calculate_time(&time_result,&time_parameters);
  output_data(&time_result,&time_parameters);

  ndwl=time_result.best_Ndwl;
  ndbl=time_result.best_Ndbl;
  nspd=time_result.best_Nspd;
  ntwl=time_result.best_Ntwl;
  ntbl=time_result.best_Ntbl;
  ntspd=time_result.best_Ntspd;
  c = time_parameters.cache_size;
  b = time_parameters.block_size;
  a = time_parameters.associativity;

  cache=1;

  rowsb = c/(8*b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  /*020205mwr*/
  tagsize = va_size - ((int)logtwo(cache_dl1[0]->nsets) + (int)logtwo(cache_dl1[0]->bsize));
//  tagsize = va_size - ((int)logtwo(cache_dl1->nsets) + (int)logtwo(cache_dl1->bsize));
  
  trowsb = c/(8*b*a*ntbl*ntspd);
  tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

  if(verbose) {
    fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
    fprintf(stderr,"tagsize == %d\n",tagsize);

    fprintf(stderr,"\nntwl == %d, ntbl == %d, ntspd == %d\n",ntwl,ntbl,ntspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ntwl*ntbl,trowsb,tcolsb);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"dcache power stats\n");
  power->dcache_decoder = (res_memport/CLUSTERS+DIR_FIFO_PORTS)*ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
  power->dcache_wordline = (res_memport/CLUSTERS+DIR_FIFO_PORTS)*ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
  power->dcache_bitline = (res_memport/CLUSTERS+DIR_FIFO_PORTS)*ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
  strcpy(gcname[gcc-1],"dcache");
  power->dcache_senseamp = (res_memport/CLUSTERS+DIR_FIFO_PORTS)*ndwl*ndbl*senseamp_power(colsb);
  power->dcache_tagarray = (res_memport/CLUSTERS+DIR_FIFO_PORTS)*ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));
  strcpy(gcname[gcc-1],"dl1_tag");
  power->dcache_gc = gc[gcc-1]*Powerfactor+gc[gcc-2]*Powerfactor;

  power->dcache_power = power->dcache_decoder + power->dcache_wordline + power->dcache_bitline + power->dcache_senseamp + power->dcache_tagarray + power->dcache_gc;

  clockpower = total_clockpower(0.0030);											//110704mwr:does 0.018 need to be changed?????????
  power->clock_power = clockpower;
  if(verbose) {
    fprintf(stderr,"iresult bus power == %f\n",power->iresultbus);
    fprintf(stderr,"fresult bus power == %f\n",power->fresultbus);
    fprintf(stderr,"global clock power == %f\n",clockpower);
  }

  time_parameters.cache_size = cache_dl2->nsets/CLUSTERS * cache_dl2->bsize * cache_dl2->assoc; /* C */
  time_parameters.block_size = cache_dl2->bsize; /* B */
  time_parameters.associativity = cache_dl2->assoc; /* A */
  time_parameters.number_of_sets = cache_dl2->nsets/CLUSTERS; /* C/(B*A) */

  calculate_time(&time_result,&time_parameters);
  output_data(&time_result,&time_parameters);

  ndwl=time_result.best_Ndwl;
  ndbl=time_result.best_Ndbl;
  nspd=time_result.best_Nspd;
  ntwl=time_result.best_Ntwl;
  ntbl=time_result.best_Ntbl;
  ntspd=time_result.best_Ntspd;
  c = time_parameters.cache_size;
  b = time_parameters.block_size;
  a = time_parameters.associativity;

  rowsb = c/(8*b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  tagsize = va_size - ((int)logtwo(cache_dl2->nsets/CLUSTERS) + (int)logtwo(cache_dl2->bsize));
  trowsb = c/(8*b*a*ntbl*ntspd);
  tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

  if(verbose) {
    fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
    fprintf(stderr,"tagsize == %d\n",tagsize);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"dcache2 power stats\n");
  power->dcache2_decoder = array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
  power->dcache2_wordline = array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
  power->dcache2_bitline = array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
  strcpy(gcname[gcc-1],"dcache2");
  power->dcache2_senseamp = senseamp_power(colsb);
  power->dcache2_tagarray = simple_array_power(trowsb,tcolsb,1,1,cache);
  strcpy(gcname[gcc-1],"dl2_tag");
  power->dcache2_gc = gc[gcc-1]*Powerfactor+gc[gcc-2]*Powerfactor;


  power->dcache2_power = power->dcache2_decoder + power->dcache2_wordline + power->dcache2_bitline + power->dcache2_senseamp + power->dcache2_tagarray + power->dcache2_gc;
#ifdef INPUT_OUTPUT_BUFFER
  power->input_buf_power = array_wordline_power(input_buffer_size, 1, 64, 1, 1, cache) +
        array_bitline_power(input_buffer_size, 1, 64, 1, 1, cache) + gc[gcc-1]*Powerfactor;
  strcpy(gcname[gcc-1],"input_buf");

  power->output_buf_power = array_wordline_power(output_buffer_size, 1, 64, 1, 1, cache) +
        array_bitline_power(output_buffer_size, 1, 64, 1, 1, cache) + gc[gcc-1]*Powerfactor;
  strcpy(gcname[gcc-1],"output_buf");
#endif

  power->rat_decoder *= crossover_scaling;
  power->rat_wordline *= crossover_scaling;
  power->rat_bitline *= crossover_scaling;
  power->rat_gc *= crossover_scaling;

  power->dcl_compare *= crossover_scaling;
  power->dcl_pencode *= crossover_scaling;
  power->inst_decoder_power *= crossover_scaling;

  power->iqram_power *= crossover_scaling;
  power->fqram_power *= crossover_scaling;
  power->rob1_power *= crossover_scaling;
  power->rob2_power *= crossover_scaling;

  power->iwakeup_tagdrive *= crossover_scaling;
  power->iwakeup_tagmatch *= crossover_scaling;
  power->iwakeup_ormatch *= crossover_scaling;
  power->iwakeup_gc *= crossover_scaling;
  power->fwakeup_tagdrive *= crossover_scaling;
  power->fwakeup_tagmatch *= crossover_scaling;
  power->fwakeup_ormatch *= crossover_scaling;
  power->fwakeup_gc *= crossover_scaling;

  power->iselection *= crossover_scaling;
  power->fselection *= crossover_scaling;

  power->iregfile_decoder *= crossover_scaling;
  power->iregfile_wordline *= crossover_scaling;
  power->iregfile_bitline *= crossover_scaling;
  power->iregfile_senseamp *= crossover_scaling;
  power->iregfile_gc *= crossover_scaling;
  power->fregfile_decoder *= crossover_scaling;
  power->fregfile_wordline *= crossover_scaling;
  power->fregfile_bitline *= crossover_scaling;
  power->fregfile_senseamp *= crossover_scaling;
  power->fregfile_gc *= crossover_scaling;


/* Not needed becoz regs are now part of phys regfile 
  power->rs_decoder *= crossover_scaling;
  power->rs_wordline *= crossover_scaling;
  power->rs_bitline *= crossover_scaling;
  power->rs_senseamp *= crossover_scaling;
*/

  power->lsq_wakeup_tagdrive *= crossover_scaling;
  power->lsq_wakeup_tagmatch *= crossover_scaling;
  power->lsq_wakeup_gc *= crossover_scaling;

  power->lsq_rs_decoder *= crossover_scaling;
  power->lsq_rs_wordline *= crossover_scaling;
  power->lsq_rs_bitline *= crossover_scaling;
  power->lsq_rs_senseamp *= crossover_scaling;
  power->lsq_rs_gc *= crossover_scaling;
 
  power->fresultbus *= crossover_scaling;

  power->btb *= crossover_scaling;
  power->btb_gc *= crossover_scaling;
  power->local_predict *= crossover_scaling;
  power->local_predict_gc *= crossover_scaling;
  power->global_predict *= crossover_scaling;
  power->global_predict_gc *= crossover_scaling;
  power->chooser *= crossover_scaling;
  power->chooser_gc *= crossover_scaling;
  power->ras *= crossover_scaling;
  power->ras_gc *= crossover_scaling;

  power->dtlb *= crossover_scaling;
  power->dtlb_gc *= crossover_scaling;

  power->itlb *= crossover_scaling;
  power->itlb_gc *= crossover_scaling;


  power->icache_decoder *= crossover_scaling;
  power->icache_wordline*= crossover_scaling;
  power->icache_bitline *= crossover_scaling;
  power->icache_senseamp*= crossover_scaling;
  power->icache_tagarray*= crossover_scaling;
  power->icache_gc *= crossover_scaling;

  power->icache_power *= crossover_scaling;

  power->dcache_decoder *= crossover_scaling;
  power->dcache_wordline *= crossover_scaling;
  power->dcache_bitline *= crossover_scaling;
  power->dcache_senseamp *= crossover_scaling;
  power->dcache_tagarray *= crossover_scaling;
  power->dcache_gc *= crossover_scaling;

  power->dcache_power *= crossover_scaling;
  
  power->clock_power *= crossover_scaling;

  power->dcache2_decoder *= crossover_scaling;
  power->dcache2_wordline *= crossover_scaling;
  power->dcache2_bitline *= crossover_scaling;
  power->dcache2_senseamp *= crossover_scaling;
  power->dcache2_tagarray *= crossover_scaling;
  power->dcache2_gc *= crossover_scaling;

  power->dcache2_power *= crossover_scaling;
#ifdef INPUT_OUTPUT_BUFFER
  power->input_buf_power *= crossover_scaling;
  power->output_buf_power *= crossover_scaling;
#endif

  power->total_power = power->local_predict + power->global_predict + 
    power->chooser + power->btb + power->ras +
    power->rat_decoder + power->rat_wordline + 
    power->rat_bitline + power->rat_senseamp + 
    power->dcl_compare + power->dcl_pencode + 
    power->inst_decoder_power +
    power->iqram_power + power->fqram_power +
    power->rob1_power + power->rob2_power +
    power->iwakeup_tagdrive + power->iwakeup_tagmatch + 
    power->fwakeup_tagdrive + power->fwakeup_tagmatch + 
    power->iselection + power->fselection +
    power->iregfile_decoder + power->iregfile_wordline + 
    power->iregfile_bitline + power->iregfile_senseamp +  
    power->fregfile_decoder + power->fregfile_wordline + 
    power->fregfile_bitline + power->fregfile_senseamp +  
    power->lsq_wakeup_tagdrive + power->lsq_wakeup_tagmatch +
    power->lsq_rs_decoder + power->lsq_rs_wordline +
    power->lsq_rs_bitline + power->lsq_rs_senseamp +
    power->iresultbus + power->fresultbus +
    power->ialu1_power + power->ialu2_power + power->falu1_power + power->falu2_power +
/*    power->clock_power + */
    power->icache_power + 
    power->itlb + 
    power->dcache_power + 
    power->dtlb + 
    power->dcache2_power +
    power->local_predict_gc + power->global_predict_gc + power->chooser_gc +
    power->btb_gc + power->ras_gc +
    power->rat_gc + power->iwakeup_gc + power->fwakeup_gc +
    power->iregfile_gc + power->fregfile_gc + power->lsq_wakeup_gc +
#ifdef INPUT_OUTPUT_BUFFER
    power->input_buf_power + power->output_buf_power +
#endif
    power->lsq_rs_gc + power->duplication_power + power->compare_power;

    power->clock_power *= (1-constant_factor);	/* The component that is
    not constant, is distributed across the various units. */

    power->local_predict *= (1 + (power->clock_power/power->total_power));
    power->global_predict *= (1 + (power->clock_power/power->total_power));

    power->chooser *= (1 + (power->clock_power/power->total_power));
    power->btb *= (1 + (power->clock_power/power->total_power));
    power->ras *= (1 + (power->clock_power/power->total_power));

    power->rat_decoder *= (1 + (power->clock_power/power->total_power));
    power->rat_wordline *= (1 + (power->clock_power/power->total_power));

    power->rat_bitline *= (1 + (power->clock_power/power->total_power));
    power->rat_senseamp *= (1 + (power->clock_power/power->total_power));

    power->dcl_compare *= (1 + (power->clock_power/power->total_power));
    power->dcl_pencode *= (1 + (power->clock_power/power->total_power));

    power->inst_decoder_power *= (1 + (power->clock_power/power->total_power));

    power->iqram_power *= (1 + (power->clock_power/power->total_power));
    power->fqram_power *= (1 + (power->clock_power/power->total_power));

    power->rob1_power *= (1 + (power->clock_power/power->total_power));
    power->rob2_power *= (1 + (power->clock_power/power->total_power));

    power->iwakeup_tagdrive *= (1 + (power->clock_power/power->total_power));
    power->iwakeup_tagmatch *= (1 + (power->clock_power/power->total_power));

    power->fwakeup_tagdrive *= (1 + (power->clock_power/power->total_power));
    power->fwakeup_tagmatch *= (1 + (power->clock_power/power->total_power));

    power->iselection *= (1 + (power->clock_power/power->total_power));
    power->fselection *= (1 + (power->clock_power/power->total_power));

    power->iregfile_decoder *= (1 + (power->clock_power/power->total_power));
    power->iregfile_wordline *= (1 + (power->clock_power/power->total_power));

    power->iregfile_bitline *= (1 + (power->clock_power/power->total_power));
    power->iregfile_senseamp *= (1 + (power->clock_power/power->total_power));

    power->fregfile_decoder *= (1 + (power->clock_power/power->total_power));
    power->fregfile_wordline *= (1 + (power->clock_power/power->total_power));

    power->fregfile_bitline *= (1 + (power->clock_power/power->total_power));
    power->fregfile_senseamp *= (1 + (power->clock_power/power->total_power));

    power->lsq_wakeup_tagdrive *= (1 + (power->clock_power/power->total_power));
    power->lsq_wakeup_tagmatch *= (1 + (power->clock_power/power->total_power));

    power->lsq_rs_decoder *= (1 + (power->clock_power/power->total_power));
    power->lsq_rs_wordline *= (1 + (power->clock_power/power->total_power));

    power->lsq_rs_bitline *= (1 + (power->clock_power/power->total_power));
    power->lsq_rs_senseamp *= (1 + (power->clock_power/power->total_power));

    power->iresultbus *= (1 + (power->clock_power/power->total_power));
    power->fresultbus *= (1 + (power->clock_power/power->total_power));

    power->ialu1_power *= (1 + (power->clock_power/power->total_power));
    power->ialu2_power *= (1 + (power->clock_power/power->total_power));
    power->falu1_power *= (1 + (power->clock_power/power->total_power));
    power->falu2_power *= (1 + (power->clock_power/power->total_power));

    power->icache_power *= (1 + (power->clock_power/power->total_power));

    power->itlb *= (1 + (power->clock_power/power->total_power));

    power->dcache_power *= (1 + (power->clock_power/power->total_power));

    power->dtlb *= (1 + (power->clock_power/power->total_power));

    power->dcache2_power *= (1 + (power->clock_power/power->total_power));

    power->local_predict_gc *= (1 + (power->clock_power/power->total_power));
    power->global_predict_gc *= (1 + (power->clock_power/power->total_power));
    power->chooser_gc *= (1 + (power->clock_power/power->total_power));

    power->btb_gc *= (1 + (power->clock_power/power->total_power));
    power->ras_gc *= (1 + (power->clock_power/power->total_power));

    power->rat_gc *= (1 + (power->clock_power/power->total_power));
    power->iwakeup_gc *= (1 + (power->clock_power/power->total_power));
    power->fwakeup_gc *= (1 + (power->clock_power/power->total_power));

    power->iregfile_gc *= (1 + (power->clock_power/power->total_power));
    power->fregfile_gc *= (1 + (power->clock_power/power->total_power));
    power->lsq_wakeup_gc *= (1 + (power->clock_power/power->total_power));

    power->lsq_rs_gc *= (1 + (power->clock_power/power->total_power));
    power->itlb_gc *= (1 + (power->clock_power/power->total_power));
    power->dtlb_gc*= (1 + (power->clock_power/power->total_power));
#ifdef INPUT_OUTPUT_BUFFER
    power->input_buf_power *= (1 + (power->clock_power/power->total_power));
    power->output_buf_power *= (1 + (power->clock_power/power->total_power));
#endif
    
    /* ejt:  fault recovery power added for Wattch */
    power->duplication_power *= (1 + (power->clock_power/power->total_power));
    power->compare_power *= (1 + (power->clock_power/power->total_power));
    
    power->clock_power = power->clock_power*constant_factor/(1-constant_factor);
    power->clock1_power = power->clock_power*0.2; /* Fr-end part of it. */
    power->clock2_power = power->clock_power*0.5; /* Int part of it. */
    power->clock3_power = power->clock_power*0.3; /* Fp part. */


  power->total_power = power->local_predict + power->global_predict + 
    power->chooser + power->btb + power->ras +
    power->rat_decoder + power->rat_wordline + 
    power->rat_bitline + power->rat_senseamp + 
    power->dcl_compare + power->dcl_pencode + 
    power->inst_decoder_power +
    power->iqram_power + power->fqram_power +
    power->rob1_power + power->rob2_power +
    power->iwakeup_tagdrive + power->iwakeup_tagmatch + power->iwakeup_ormatch +
    power->fwakeup_tagdrive + power->fwakeup_tagmatch + power->fwakeup_ormatch +
    power->iselection + power->fselection +
    power->iregfile_decoder + power->iregfile_wordline + 
    power->iregfile_bitline + power->iregfile_senseamp +  
    power->fregfile_decoder + power->fregfile_wordline + 
    power->fregfile_bitline + power->fregfile_senseamp +  
    power->lsq_wakeup_tagdrive + power->lsq_wakeup_tagmatch +
    power->lsq_rs_decoder + power->lsq_rs_wordline +
    power->lsq_rs_bitline + power->lsq_rs_senseamp +
    power->iresultbus + power->fresultbus +
    power->ialu1_power + power->ialu2_power + power->falu1_power + power->falu2_power +
/*    power->clock_power + */
    power->clock1_power + power->clock2_power + power->clock3_power +
    power->icache_power + 
    power->itlb + 
    power->dcache_power + 
    power->dtlb + 
    power->dcache2_power +
    power->local_predict_gc + power->global_predict_gc + power->chooser_gc +
    power->btb_gc + power->ras_gc +
    power->rat_gc + power->iwakeup_gc + power->fwakeup_gc +
    power->iregfile_gc + power->fregfile_gc + power->lsq_wakeup_gc +
#ifdef INPUT_OUTPUT_BUFFER
    power->input_buf_power + power->output_buf_power +
#endif
    power->lsq_rs_gc + power->duplication_power + power->compare_power;

  power->bpred_power = power->btb + power->local_predict + power->global_predict + power->chooser + power->ras +
  power->btb_gc + power->local_predict_gc + power->global_predict_gc + power->chooser_gc + power->ras_gc;



  power->rat_power = power->rat_decoder + 
    power->rat_wordline + power->rat_bitline + power->rat_senseamp + power->rat_gc;
  
  power->dcl_power = power->dcl_compare + power->dcl_pencode;

  power->rename_power = power->rat_power + 
	  power->dcl_power + 
	  power->inst_decoder_power;

  if(ruu_inorder_issue)
  {
	power->rat_power = 0;
	power->dcl_power = 0;
	power->inst_decoder_power = 0;
	power->rob2_power = 0;
	power->rob1_power = 0;
  }


  power->iwakeup_power = power->iwakeup_tagdrive + power->iwakeup_tagmatch + 
    power->iwakeup_ormatch + power->iwakeup_gc;
  power->fwakeup_power = power->fwakeup_tagdrive + power->fwakeup_tagmatch + 
    power->fwakeup_ormatch + power->fwakeup_gc;
  if(ruu_inorder_issue)
  {
	power->iwakeup_tagdrive = 0;
	power->iwakeup_tagmatch = 0;
	power->iwakeup_ormatch = 0;
	power->iwakeup_gc = 0;
	power->fwakeup_tagdrive = 0;
	power->fwakeup_tagmatch = 0;
	power->fwakeup_ormatch = 0;
	power->fwakeup_gc = 0;
	power->iselection = 0;
	power->fselection = 0;
  }



  power->lsq_rs_power = power->lsq_rs_decoder + 
    power->lsq_rs_wordline + power->lsq_rs_bitline + 
    power->lsq_rs_senseamp + power->lsq_rs_gc;

  power->lsq_rs_power_nobit = power->lsq_rs_decoder + 
    power->lsq_rs_wordline + power->lsq_rs_senseamp + power->lsq_rs_gc;
   
  power->lsq_wakeup_power = power->lsq_wakeup_tagdrive + power->lsq_wakeup_tagmatch + power->lsq_wakeup_gc;

  power->lsq_power = power->lsq_wakeup_power + power->lsq_rs_power;



  
  power->iregfile_power = power->iregfile_decoder + 
    power->iregfile_wordline + power->iregfile_bitline + 
    power->iregfile_senseamp + power->iregfile_gc;

  power->iregfile_power_nobit = power->iregfile_decoder + 
    power->iregfile_wordline + power->iregfile_senseamp + power->iregfile_gc;

  power->fregfile_power = power->fregfile_decoder + 
    power->fregfile_wordline + power->fregfile_bitline + 
    power->fregfile_senseamp + power->fregfile_gc;

  power->fregfile_power_nobit = power->fregfile_decoder + 
    power->fregfile_wordline + power->fregfile_senseamp + power->fregfile_gc;
  /*
  fprintf(stderr,"xyz Dcache2_gc power\t%f\n",power->dcache2_gc);
  fprintf(stderr,"xyz Bpred\t%f\n",power->bpred_power);
  fprintf(stderr,"xyz Rename\t%f\n",power->rename_power);
  fprintf(stderr,"xyz ROB wakeup\t%f\n",power->rob1_power);
  fprintf(stderr,"xyz ROB ram\t%f\n",power->rob2_power);
  fprintf(stderr,"xyz IQ ram\t%f\n",power->iqram_power);
  fprintf(stderr,"xyz FQ ram\t%f\n",power->fqram_power);
  fprintf(stderr,"xyz iwakeup\t%f\n",power->iwakeup_power);
  fprintf(stderr,"xyz fwakeup\t%f\n",power->fwakeup_power);
  fprintf(stderr,"xyz iselect\t%f\n",power->iselection);
  fprintf(stderr,"xyz fselect\t%f\n",power->fselection);
  fprintf(stderr,"xyz lsq \t%f\n",power->lsq_power);
  fprintf(stderr,"xyz iregf\t%f\n",power->iregfile_power);
  fprintf(stderr,"xyz fregf\t%f\n",power->fregfile_power);
  fprintf(stderr,"xyz ialu1\t%f\nxyz ialu2\t%f\nxyz falu1\t%f\nxyz falu2\t%f\n",power->ialu1_power, power->ialu2_power, power->falu1_power, power->falu2_power);
  fprintf(stderr,"xyz iresultb\t%f\n",power->iresultbus);
  fprintf(stderr,"xyz fresultb\t%f\n",power->fresultbus);
  fprintf(stderr,"xyz itlb\t%f\n",power->itlb);
  fprintf(stderr,"xyz dtlb\t%f\n",power->dtlb);
  fprintf(stderr,"xyz icache\t%f\n",power->icache_power);
  fprintf(stderr,"xyz dcache\t%f\n",power->dcache_power);
  fprintf(stderr,"xyz clock\t%f\n",power->clock_power);
  fprintf(stderr,"xyz dcache2\t%f\n",power->dcache2_power);
  fprintf(stderr,"xyz clock-fr\t%f\n",power->clock1_power);
  fprintf(stderr,"xyz clock-int\t%f\n",power->clock2_power);
  fprintf(stderr,"xyz clock-fp\t%f\n",power->clock3_power);
  fprintf(stderr,"xyz duplication\t%f\n",power->duplication_power);
  fprintf(stderr,"xyz compare\t%f\n",power->compare_power);
#ifdef INPUT_OUTPUT_BUFFER
  fprintf(stderr,"xyz input buffer power\t%f\n", power->input_buf_power);
  fprintf(stderr,"xyz output buffer power\t%f\n", power->output_buf_power);
#endif

  fprintf(stderr,"\nxyz total\t%f\n",power->total_power);

  dump_power_stats(power);
  */

}

void power_value(FILE *fp){
 
	fprintf(fp, "%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t", TOTAL_power_cc3[0],TOTAL_power_cc3[1],TOTAL_power_cc3[2],TOTAL_power_cc3[3], (DCACHE2_power_cc3[0]+DCACHE2_power_cc3[1]+DCACHE2_power_cc3[2]+DCACHE2_power_cc3[3]));
}

//add by ctb
//2013-07-09
extern counter_t sim_cycle;

void TotalCPUPower(double *total)
{
	int i;
	double sum_power = 0.0;
	
	for(i=0;i<numcontexts;i++)
	{
		sum_power +=  TOTAL_power_cc3[i];

	}
	
	//fprintf(fp, "%.3f\t",sum_power/sim_cycle);
	
	*total = sum_power;
}

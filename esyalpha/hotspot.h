/*
 * $Log: hotspot.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:11  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:54  garg
 * dir structure
 *
 * Revision 1.5  2005/07/04 18:50:17  rashid
 * *** empty log message ***
 *
 * Revision 1.4  2005/01/26 22:15:57  rashid
 * *** empty log message ***
 *
 * Revision 1.5  2005/01/26 15:54:05  etan
 * Add new structures for 21264 floorplan accuracy.
 *
 * Revision 1.4  2004/12/07 21:09:40  etan
 * Function to put total leakage energy for each core into <benchmark>.txt file
 *
 * Revision 1.3  2004/12/01 14:56:29  etan
 * Updated HPCRI05 sim-outorder with this version
 *
 * Revision 1.3  2004/11/30 20:38:37  etan
 * Rename floorplan structures according to terminology used in HPCRI05 paper
 *
 * Revision 1.2  2004/11/08 20:48:49  etan
 * Added structures for fault tolerant cmp architecture
 *
 * Revision 1.1  2004/09/22 21:20:25  etan
 * Initial revision
 *
 * Revision 1.9  2004/08/02 17:21:06  ninelson
 * support added for turning hotspot off
 *
 * Revision 1.8  2004/06/28 22:10:01  grbriggs
 * Add new fields for power info. Also, better support for CONFIG_BASED_ICS.
 *
 * Revision 1.7  2004/06/28 14:19:55  grbriggs
 * Add front end.
 *
 * Revision 1.6  2004/06/16 22:40:27  grbriggs
 * Make base_proc_freq publicly visible (used by Wattch).
 *
 */

/* for the use of "FILE" */
#include <stdio.h>

/* ********** options *********** */
int hotspot_active;

FILE *sim_hotspotout_fd; /* file to write hotspot related stuff to */

/* ********** stats ************* */


/* ********** variables ********** */

extern double base_proc_freq; /* base CPU freq in Hz */

typedef struct hotspot_power_t_st
{
//	hotspot_power_t *next;
	double fmul;	/* for FPMult */
	double freg;	/* for FPReg */
	double fadd;	/* for FPAdd */
	double dtb0;	/* for DTB0 */
	double fqueue;	/* for FPQ */
	double iqueue;	/* for IntQ */
	double lsq;	/* for LdStQ */
	double ireg0;	/* for IntReg0 */
	double ialu0;	/* for IntAlu0 */
	double ialu1;	/* for IntAlu1, ialu2 is IntMult */
#ifndef CONFIG_BASED_ICS
	double el; /* electric comm left */
	double er; /* electric comm right */
	double optical_mod_l; /* only use OL when only one optic comm in use */
	double optical_rec_l; /* only use OL when only one optic comm in use */
	double optical_qram_l;
	double optical_mod_r;
	double optical_rec_r;
	double optical_qram_r;
#else // !CONFIG_BASED_ICS
	double comm_link_tx[MAX_IC_INDEX];
	double comm_link_rx[MAX_IC_INDEX];
	double comm_qram[MAX_IC_INDEX];
#endif // !CONFIG_BASED_ICS
	double dl1cache;	/* for Dcache */
	double ul2cache; /* Method of sharing: All cores' usage is combined by hotspot.c */
	double raq;
	double iresultbus;
	double fresultbus;
	double clock1; /* FE */  /* only recorded under cluster zero */
	double clock2; /* Int */ /* only recorded under cluster zero */
	double clock3; /* FP */  /* only recorded under cluster zero */
	
	/* Frontend. 
	 * NOTE: 
	 *	Power is recorded under cluster zero for shared items.
	 *	Power is recorded under the cluster having the same number as the context number for the rest. */
	double bpred; /* shared for bpred */
	double icache; /* shared for Icache */
	double rob; /* not shared for ROB */
	double irename; /* not shared, changed IntRename from rename */
	double itlb; /* shared for ITB */
	double fetch; /* shared for Fetch */

/* ejt: added structures for fault tolerant cmp architecture */
/* 113004 ejt: rename retmap, commbuf, pcstq to terminology used in HPCRI05 
               paper */
	double cpbuf;	/* shared checkpoint buffer for ChkptBuf */
	double broutq;	/* shared branch outcome queue for BrOutcomeQ */
	double pcbuf;	/* shared post commit buffer for PostComBuf */
	
	double bus0;	/* shared bus */
	double bus1;	/* shared bus */

/* 011705 ejt: more structures for accuracy */
	double sysintf;	/* for L2SysInterface */
/* 012305 ejt: ialu2 is actually for int multiplier for code legacy */
	double ialu2;	/* for IntMult */
	double frename;	/* for FPRename */
/* 012305 ejt: since ialu2 is for int mult, use ialu3 for int alu 2 */
	double ialu3;	/* for IntAlu2 */
	double dtb1;	/* for DTB1 */	
	double ireg1;	/* for IntReg1 */
	
}hotspot_power_array_t;
extern hotspot_power_array_t hotspot_perperiod[CLUSTERS];
//moved to sim-outorder.c: static int hotspot_cycle_sampling_intvl; 	/* 666ns sampling interval = 10K cycles at 15GHz	*/
extern counter_t last_hotspot_cycle;
/* models */


/* ********** functions ********** */

void hotspot_init(void);
void hotspot_uninit(void);
void hotspot_reg_options(struct opt_odb_t *odb);
void update_hotspot_stats(int elapsed_cycles);
void hotspot_dumpstats(void);

/* 120504 ejt: function to put total leakage energy for each core into 
               <benchmark>.txt file */
void leakage_power_value(FILE *fp);
void leakage_power_clear();

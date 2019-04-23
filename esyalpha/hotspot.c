/*
 * $Log: hotspot.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:05  xue
 * CMP_NETWORK
 *
 * Revision 1.2  2009/03/17 13:19:47  garg
 * Bug fixes: Application stack size and emulation of the parallel code
 *
 * Revision 1.1.1.1  2008/10/20 15:44:53  garg
 * dir structure
 *
 * Revision 1.7  2005/03/29 17:51:57  etan
 * Rename definitions for separate lead and checker porcessor leakage power
 *
 * Revision 1.6  2005/03/29 17:37:06  rashid
 * *** empty log message ***
 *
 * Revision 1.5  2005/01/26 22:15:52  rashid
 * *** empty log message ***
 *
 * Revision 1.8  2005/01/26 15:54:42  etan
 * Add new structures for 21264 floorplan accuracy.
 *
 * Revision 1.7  2004/12/08 05:05:03  etan
 * Corrected leakage_power_value function
 *
 * Revision 1.6  2004/12/07 22:01:58  rashid
 * Modified leakage_power_value()
 *
 * Revision 1.5  2004/12/07 21:08:08  etan
 * Integrated checkpoint buffer, branch outcome queue and post commit buffer into core
 * Added FT_CMP definition
 *
 * Revision 1.4  2004/12/01 14:53:27  etan
 * Updated HPCRI05 sim-outorder with this version
 *
 * Revision 1.4  2004/11/30 20:38:28  etan
 * Rename floorplan structures according to terminology used in HPCRI05 paper
 *
 * Revision 1.3  2004/11/08 21:24:04  etan
 * MAXTHREADS define the number of ROB and rename required
 * Added retirement map, communication buffer and post-commit store queue
 * CMT to CMP changes including splitting bpred, icache, itlb, fetch
 * Fix some mistakes in unit names from previous checkin. Also, make average power better
 * Average leakage power modifications
 *
 * Revision 1.2  2004/10/28 20:08:55  etan
 * change elasped_time to elasped_cycles for hotspot_power computation
 * display average leakage power
 *
 * Revision 1.1  2004/09/22 21:20:25  etan
 * Initial revision
 *
 * Revision 1.44  2004/08/02 17:21:06  ninelson
 * support added for turning hotspot off
 *
 * Revision 1.43  2004/07/27 02:43:01  grbriggs
 * Turn off debug message.
 *
 * Revision 1.42  2004/07/26 22:09:46  grbriggs
 * Trying to debug a hot rob0 ...
 *
 * Revision 1.41  2004/07/26 16:52:14  grbriggs
 * Add missing initialization.
 *
 * Revision 1.40  2004/07/22 18:59:18  grbriggs
 * Make t_inferface print out correctly in the list of config options
 *
 * Revision 1.39  2004/07/19 21:46:55  grbriggs
 * Print transistor counts.
 *
 * Revision 1.38  2004/07/16 17:10:38  grbriggs
 * Make compile on gcc 2
 *
 * Revision 1.37  2004/07/14 15:41:07  grbriggs
 * Compute temperature-based leakage power!
 *
 * Revision 1.36  2004/07/13 15:16:21  grbriggs
 * Switch power use to handle reverse-accounting of L and R comm receive units.
 *
 * Revision 1.35  2004/07/07 20:49:07  grbriggs
 * Make custom links account for power based on physics.
 *
 * Revision 1.34  2004/06/29 22:00:05  grbriggs
 * One more bug fix. Finally it looks like the fractional power is working right.
 *
 * Revision 1.33  2004/06/29 15:53:34  grbriggs
 * Fix fractional power bug.
 *
 * Revision 1.32  2004/06/29 15:18:20  grbriggs
 * Turn off debug message.
 *
 * Revision 1.31  2004/06/29 15:16:44  grbriggs
 * Finish code to share power; use it to support distribution of clock power.
 *
 * Revision 1.30  2004/06/28 22:10:44  grbriggs
 * Add new defines allowing simpler distribution of power amoung multiple units.
 *
 * Revision 1.29  2004/06/23 21:09:33  grbriggs
 * Made MAX_L2CACHES bigger so sparse works.
 *
 * Revision 1.28  2004/06/17 21:15:58  grbriggs
 * Evenly distribute power to L2cache blocks according to their area.
 *
 * Revision 1.27  2004/06/16 22:42:18  grbriggs
 * Make base processor frequency an option.
 * Also, switched to a better heatsink (might need to update convection capacitance still).
 *
 * Revision 1.26  2004/06/14 21:29:20  grbriggs
 * Make hotspot_cycle_sampling_intvl an option in sim-outorder.c
 *
 * Revision 1.25  2004/06/14 17:36:10  grbriggs
 * Fixed a bug in a fprintf command. Also, attempted to make -hotspot:dump_freq
 * an option, but it doesn't seem to work yet.
 *
 * Revision 1.24  2004/06/11 21:52:48  grbriggs
 * Add option "-redir:hotspot" that allows temperature data to be recorded to a
 * separate file.
 *
 * Revision 1.23  2004/06/11 15:26:53  grbriggs
 * Turn off printing of power usage in each unit.
 *
 * Revision 1.22  2004/06/08 12:55:08  ninelson
 * Added multiple hotspot dumps and updates for reconfigurable interconnects.
 *
 * Revision 1.21  2004/06/03 20:17:04  grbriggs
 * Fix 'checkers' l2 cache layout bug.
 *
 * Revision 1.20  2004/06/03 18:48:17  grbriggs
 * Add comments for the L2cache layouts.
 *
 * Revision 1.19  2004/06/03 18:40:23  grbriggs
 * fix typo.
 *
 * Revision 1.18  2004/06/03 18:38:36  grbriggs
 * Add bug fixes for mesh optical type. Also, add support for grid with border of cache.
 *
 * Revision 1.17  2004/06/02 23:01:27  grbriggs
 * Add support for another L2 cache arrangement.
 *
 * Revision 1.16  2004/06/02 14:42:48  grbriggs
 * Make the TIM thickness be in microns instead of meters.
 *
 * Revision 1.15  2004/06/01 23:35:03  grbriggs
 * Undo the previous change
 *
 * Revision 1.14  2004/06/01 23:18:33  grbriggs
 * Simplify sprintf call.
 *
 * Revision 1.13  2004/06/01 21:07:59  grbriggs
 * Make l2cache be represented by 16 units instead of 1. Also, make thermal compound thickness an option.
 *
 * Revision 1.12  2004/05/29 15:24:11  grbriggs
 * Add support for flower and mesh type configurations.
 *
 * Revision 1.11  2004/05/29 12:38:16  grbriggs
 * Add missing semicolon.
 *
 * Revision 1.10  2004/05/29 12:36:10  grbriggs
 * Add support for a shared L2 cache.
 *
 * Revision 1.9  2004/05/28 14:42:21  grbriggs
 * make default temperature init file be NULL
 *
 * Revision 1.8  2004/05/28 14:31:13  grbriggs
 * Fix capitalization of Dcache.
 *
 * Revision 1.7  2004/05/27 22:51:41  grbriggs
 * Began adding new nodes for optical units. Also switched some code to use a macro.
 *
 * Revision 1.6  2004/05/26 20:19:32  grbriggs
 * just an intermediate save, before i make macros out of update_hotspot_stats
 *
 * Revision 1.5  2004/05/25 19:00:17  grbriggs
 * Fix dumped transient output + improve formatting
 *
 * Revision 1.4  2004/05/25 17:38:03  grbriggs
 * fix compiler errors introduced in previous update
 *
 * Revision 1.3  2004/05/25 17:14:29  grbriggs
 * Update to hotspot 2.0
 *
 */

#include "RC.h"
#include "flp.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "headers.h"

/* 120204 ejt: use FT_CMP to disable some processor structures which are not
               necessary in our architecture */
#define FT_CMP

#define CORE_ELEMENTS	30			/* each core consists of 30 architectural elements */

/* simulator options	*/
static int omit_lateral = 0;			/* omit the later chip resistances?	*/
static double init_temp = 60 + 273.15;		/* 60 degree C converted to Kelvin	*/

counter_t last_hotspot_cycle;
double base_proc_freq;				/* base processor frequency in Hz	*/
static int dtm_used = 0;			/* set accordingly	*/

static char *flp_cfg = NULL;
static char *init_file = NULL;
static char *steady_file = NULL;


/* chip specs	*/
double t_chip = 0.0005;				/* chip thickness in meters    */
double thermal_threshold = 111.8 + 273.15;	/* temperature threshold for DTM (Kelvin) */

/* heat sink specs	*/
double c_convec = 140.4;			/* convection capacitance - 140.4 J/K */
//double r_convec = 0.1;			/* convection resistance - 0.2 K/W	*/
double r_convec = 0.02;				/* folded fin convection resistance - 0.02 K/W	*/
double s_sink = 0.06;				/* heatsink side - 60 mm	*/
double t_sink = 0.0069; 			/* heatsink thickness  - 6.9 mm	*/


/* heat spreader specs	*/
double s_spreader = 0.03;	/* spreader side - 30 mm	*/
double t_spreader = 0.001;	/* spreader thickness - 1 mm */

/* interface material specs	*/
double t_interface;		// = 0.000015;	/* interface material thickness  - now 0.015mm	*/
double t_interface_um;		/* this is the config option, in microns */
//double t_interface = 0.000075;	/* interface material thickness  - 0.075mm	*/

/* ambient temp in kelvin	*/
double ambient = 45 + 273.15;	/* 45 C in kelvin	*/

static double *temp, *hotspot_power, *overall_power, *steady_temp, *mid_power,
  *mid_temp, *total_leakage_power, *cluster_total_leakage_power,
  *cluster_average_leakage;
flp_t *flp;
counter_t total_elapsed_cycles;

int hotspot_calls = 0;

/* one in this many calls to update_hotspot will print current temps */
static int hotspot_dump_freq;

hotspot_power_array_t hotspot_perperiod[CLUSTERS];

/* Hotspot like options */
FILE *sim_hotspotout_fd;

void
hotspot_reg_options (struct opt_odb_t *odb)
{
  opt_reg_string(odb, "-hotspot:flp", "floorplan file (.flp))", &flp_cfg,
		  /* default */ NULL, /* !print */FALSE, NULL);
  opt_reg_string(odb, "-hotspot:init",
		  "hotspot temperature initialization file", &init_file,
		  /* default *//*"hotspot.out" */ NULL, /* !print */ FALSE,
		  NULL);
  opt_reg_string (odb, "-hotspot:steady",
		  "hotspot temperature steady-state (output) file",
		  &steady_file,
		  /* default */ "hotspot.out", /* !print */FALSE, NULL);
  opt_reg_double (odb, "-hotspot:t_interface",
		  "hotspot thermal interface material (thermal paste) thickness, in microns",
		  &t_interface_um,
		  /* default 50um */ 30, /* !print */FALSE, NULL);
  opt_reg_int (odb, "-hotspot:dump_freq",
		"one in this many calls to update_hotspot_stats will print current temps",
		&hotspot_dump_freq,
		/* default */ 30, /* !print */ FALSE, NULL);
  opt_reg_double (sim_odb, "-hotspot:base_proc_freq",
		"base processor frequency in Hz", &base_proc_freq,
		/* default 2.5GHz */ 2.5e9, /* !print */FALSE, NULL);
  opt_reg_flag(sim_odb, "-hotspot:active",
		"Turns HotSpot on", &hotspot_active, FALSE, FALSE, NULL);
}

#define FLPGETAREA(unit) (flp->units[get_blk_index(flp, unit)].width * flp->units[get_blk_index(flp, unit)].height)

double leakage_power_factor[MAX_UNITS];
/**
 * Calculate leakage power for all units based on area and whther it is sram or logic
 */
void
init_leakage_power ()
{
  int i;
  double totalpower = 0;
  double total_sram_area = 0;
  double total_logic_area = 0;

/* 120304 ejt: if FT_CMP defined, then bus is not used */
#ifndef FT_CMP
  double total_bus_area = 0;
#endif

  if (flp->n_units > MAX_UNITS)
    fatal
      ("Your floorplan has %d units which is greater than MAX_UNITS (%d).",
       flp->n_units, MAX_UNITS);

  for (i = 0; i < flp->n_units; i++)
    {
      if (strstr (flp->units[i].name, "cache") ||
	  strstr (flp->units[i].name, "qram") ||
	  strstr (flp->units[i].name, "IntQ") ||
	  strstr (flp->units[i].name, "FPQ") ||
	  strstr (flp->units[i].name, "LdStQ") ||
	  strstr (flp->units[i].name, "TB") ||
	  strstr (flp->units[i].name, "bpred") ||
	  strstr (flp->units[i].name, "Reg") ||
	  strstr (flp->units[i].name, "L2SysInterface") //||
	  //strstr (flp->units[i].name,"ChkptBuf") ||
	  //strstr (flp->units[i].name,"BrOutcomeQ") ||
	  //strstr (flp->units[i].name,"PostComBuf") 
	  ) 
	{			/* SRAM */
	  leakage_power_factor[i] =
	    SRAMPOWERPERUM2PERTEMP_LEAD * 1e12 * flp->units[i].width *
	    flp->units[i].height;
	  total_sram_area += flp->units[i].width * flp->units[i].height;

	}
      else
	{			/* Logic */
	  leakage_power_factor[i] =
	    LOGICPOWERPERUM2PERTEMP_LEAD * 1e12 * flp->units[i].width *
	    flp->units[i].height;
	  total_logic_area += flp->units[i].width * flp->units[i].height;

	}
		/* 110604 ejt: cluster 0 is running at full frequency while the others are at
                   'another' frequency and so the leakage power is scaled, both sram
	            and logic are scaled the same */
      if (!strstr (flp->units[i].name, "c0"))
	{
	  leakage_power_factor[i] *=
	    (SRAMPOWERPERUM2PERTEMP_CHK / SRAMPOWERPERUM2PERTEMP_LEAD);
	}
      totalpower += leakage_power_factor[i];
    }

  fprintf (stderr, "Estimated total number of logic transistors: %g\n",
	   (double) (total_logic_area * LOGICDENSITY * 1e12));
  fprintf (stderr, "Estimated total number of SRAM transistors: %g\n",
	   (double) (total_sram_area * SRAMDENSITY * 1e12));

#ifndef FT_CMP
  fprintf (stderr, "Estimated total bus area: %g um^2\n",
	   (double) (total_bus_area * 1e12));
#endif

  fprintf (stderr, "At 80 C, the total leakage power would be %g Watts\n",
	   (double) LEAKAGEPOWERFROMTEMP (totalpower, 273.15 + 80));
}

#define MAX_L2CACHES 80		//should be 65 if sparse was right
int l2cache_blk_index[MAX_L2CACHES];
int l2cache_count = 0;
double l2cache_areas[MAX_L2CACHES];	/* fraction out of total l2 cache */

/* figure out indexes and relative sizes of l2cache pieces */
void
locate_l2cache ()
{
  /* distribute l2cache usage */
  int i;
  double totalarea = 0;
  for (i = 0; i < flp->n_units; i++)
    {
      if (strstr (flp->units[i].name, "L2cache"))
	{
	  if (l2cache_count >= MAX_L2CACHES)
	    fatal ("Your MAX_L2CACHES is too small. Recompile.");

	  l2cache_blk_index[l2cache_count] = i;
	  l2cache_areas[l2cache_count] =
	    flp->units[i].width * flp->units[i].height;
	  totalarea += l2cache_areas[l2cache_count];
	  ++l2cache_count;
	}
    }
  fprintf (stderr, "Added %d pieces of L2cache, having a total area of %g\n",
	   l2cache_count, totalarea);
  for (i = 0; i < l2cache_count; i++)
    {
      l2cache_areas[i] /= totalarea;
    }
}



#define FRACTION_DECLARE(unit,values...) \
	const char  *unit##_fract_names[] = {values}; \
	const int    unit##_fract_size = sizeof(unit##_fract_names)/sizeof(unit##_fract_names[0]); \
	double unit##_fract[sizeof(unit##_fract_names)/sizeof(unit##_fract_names[0])];

#define FRACTION_INIT(unit,prefix,divisor) \
	do { \
		double unit##_total = 0; \
		int itr; \
		char tmp[100]; \
		for (itr=0;itr<unit##_fract_size;itr++) { \
			sprintf(tmp,prefix "%s",unit##_fract_names[itr]); \
			unit##_total += FLPGETAREA(tmp); \
		} \
		for (itr=0;itr<unit##_fract_size;itr++) { \
			sprintf(tmp,prefix "%s",unit##_fract_names[itr]); \
			unit##_fract[itr] = FLPGETAREA(tmp)/(unit##_total*divisor); \
		} \
	} while(0)

#define FRACTION_INIT_CORE(unit,num,divisor) \
	do { \
		double unit##_total = 0; \
		int itr; \
		char tmp[100]; \
		for (itr=0;itr<unit##_fract_size;itr++) { \
			sprintf(tmp,"c%d_%s",num, unit##_fract_names[itr]); \
			unit##_total += FLPGETAREA(tmp); \
		} \
		for (itr=0;itr<unit##_fract_size;itr++) { \
			sprintf(tmp,"c%d_%s",num, unit##_fract_names[itr]); \
			unit##_fract[itr] = FLPGETAREA(tmp)/(unit##_total*divisor); \
		} \
	} while(0)

#define FRACTION_UPDATEHSSTAT(unit,j,sprintfformat...) \
	do { \
		int itr; \
		for (itr=0;itr<unit##_fract_size;itr++) { \
			sprintf(func_unit,sprintfformat); \
			hotspot_power[get_blk_index(flp, func_unit)] += hotspot_perperiod[j].unit*unit##_fract[itr]; \
		} \
	} while(0)

#define FRACTION_UPDATESDONE(unit,i)	hotspot_perperiod[i].unit=0

/* FRACTION_UPDATEHSSTAT_CL shares hotspot_perperiod[i] among cluster i. Use a divisor of 1 in the init. */
#define FRACTION_UPDATEHSSTAT_CL(unit,i) 	FRACTION_UPDATEHSSTAT(unit,i,"c%d_%s",i,unit##_fract_names[itr]); \
						hotspot_perperiod[i].unit=0
/* FRACTION_UPDATEHSSTAT_ALLCL shares hotspot_perperiod[0] among cluster i. Use a divisor of CLUSTERS in the init. */
#define FRACTION_UPDATEHSSTAT_ALLCL(unit,i) 	FRACTION_UPDATEHSSTAT(unit,0,"c%d_%s",i,unit##_fract_names[itr])
/* FRACTION_UPDATEHSSTAT_ALL shares hotspot_perperiod[0] amoung the explicitly-named units. Use a divisor of 1 in the init. */
#define FRACTION_UPDATEHSSTAT_ALL(unit,i) 	FRACTION_UPDATEHSSTAT(unit,i,"%s",unit##_fract_names[itr])

FRACTION_DECLARE (iresultbus, "IntAlu0", "IntAlu1", "IntAlu2",
		  "IntMult", "IntReg0", "IntReg1");
FRACTION_DECLARE (fresultbus, "FPAdd", "FPMult", "FPReg");
FRACTION_DECLARE (clock1,
		  "c0_L2SysInterface",
		  "c0_Icache", "c0_Dcache", "c0_ITB",
		  "c0_DTB0", "c0_DTB1", "c0_Fetch",
		  "c0_bpred", "c0_IntRename", "c0_FPRename", "c0_ROB",
		  "c1_L2SysInterface",
		  "c1_Icache", "c1_Dcache", "c1_ITB",
		  "c1_Fetch", "c1_DTB0", "c1_DTB1",
		  "c1_bpred", "c1_IntRename", "c1_FPRename",
		  "c1_ROB",
		  "c2_L2SysInterface",
		  "c2_Icache", "c2_Dcache", "c2_ITB",
		  "c2_Fetch", "c2_DTB0", "c2_DTB1",
		  "c2_bpred", "c2_IntRename", "c2_FPRename",
		  "c2_ROB",
		  "c3_L2SysInterface",
		  "c3_Icache", "c3_Dcache", "c3_ITB",
		  "c3_Fetch", "c3_DTB0", "c3_DTB1",
		  "c3_bpred", "c3_IntRename", "c3_FPRename",
		  "c3_ROB",
		  "c4_L2SysInterface",
		  "c4_Icache", "c4_Dcache", "c4_ITB",
		  "c4_DTB0", "c4_DTB1", "c4_Fetch",
		  "c4_bpred", "c4_IntRename", "c4_FPRename",
		  "c4_ROB",
		  "c5_L2SysInterface",
		  "c5_Icache", "c5_Dcache", "c5_ITB",
		  "c5_Fetch", "c5_DTB0", "c5_DTB1",
		  "c5_bpred", "c5_IntRename", "c5_FPRename",
		  "c5_ROB",
		  "c6_L2SysInterface",
		  "c6_Icache", "c6_Dcache", "c6_ITB",
		  "c6_Fetch", "c6_DTB0", "c6_DTB1",
		  "c6_bpred", "c6_IntRename", "c6_FPRename",
		  "c6_ROB",
		  "c7_L2SysInterface",
		  "c7_Icache", "c7_Dcache", "c7_ITB",
		  "c7_Fetch", "c7_DTB0", "c7_DTB1",
		  "c7_bpred", "c7_IntRename", "c7_FPRename",
		  "c7_ROB",
		  "c8_L2SysInterface",
		  "c8_Icache", "c8_Dcache", "c8_ITB",
		  "c8_DTB0", "c8_DTB1", "c8_Fetch",
		  "c8_bpred", "c8_IntRename", "c8_FPRename",
		  "c8_ROB",
		  "c9_L2SysInterface",
		  "c9_Icache", "c9_Dcache", "c9_ITB",
		  "c9_Fetch", "c9_DTB0", "c9_DTB1",
		  "c9_bpred", "c9_IntRename", "c9_FPRename",
		  "c9_ROB",
		  "c10_L2SysInterface",
		  "c10_Icache", "c10_Dcache", "c10_ITB",
		  "c10_Fetch", "c10_DTB0", "c10_DTB1",
		  "c10_bpred", "c10_IntRename", "c10_FPRename",
		  "c10_ROB",
		  "c11_L2SysInterface",
		  "c11_Icache", "c11_Dcache", "c11_ITB",
		  "c11_Fetch", "c11_DTB0", "c11_DTB1",
		  "c11_bpred", "c11_IntRename", "c11_FPRename",
		  "c11_ROB",
		  "c12_L2SysInterface",
		  "c12_Icache", "c12_Dcache", "c12_ITB",
		  "c12_DTB0", "c12_DTB1", "c12_Fetch",
		  "c12_bpred", "c12_IntRename", "c12_FPRename",
		  "c12_ROB",
		  "c13_L2SysInterface",
		  "c13_Icache", "c13_Dcache", "c13_ITB",
		  "c13_Fetch", "c13_DTB0", "c13_DTB1",
		  "c13_bpred", "c13_IntRename", "c13_FPRename",
		  "c13_ROB",
		  "c14_L2SysInterface",
		  "c14_Icache", "c14_Dcache", "c14_ITB",
		  "c14_Fetch", "c14_DTB0", "c14_DTB1",
		  "c14_bpred", "c14_IntRename", "c14_FPRename",
		  "c14_ROB",
		  "c15_L2SysInterface",
		  "c15_Icache", "c15_Dcache", "c15_ITB",
		  "c15_Fetch", "c15_DTB0", "c15_DTB1",
		  "c15_bpred", "c15_IntRename", "c15_FPRename",
		  "c15_ROB");

FRACTION_DECLARE (clock2, "IntQ", "IntReg0", "IntReg1",
		  "IntAlu0", "IntAlu1", "IntAlu2", "IntMult", "LdStQ");
FRACTION_DECLARE (clock3, "FPQ", "FPReg", "FPAdd", "FPMult");

void
hotspot_init ()
{
  int i;
  extern char *sim_str_dump;
  char *sim_dump = sim_str_dump;
  char strFileName[256];

  if (!sim_dump)
  {
      fprintf (stderr, "No name for dump file\n");
      exit (1);
  }

  strcpy (strFileName, sim_dump);
  strcat (strFileName, ".hsp");

  if ((sim_hotspotout_fd = fopen (strFileName, "w")) == NULL)
  {
	  fatal
	    ("hotspot_init: Error opening file '%s' specified by -redir:hotspot",
	     strFileName);
	  exit (0);
  }

  /* make t_interface be in meters, per hotspot spec */
  t_interface = 0.000001 * t_interface_um;

  /* initialize flp, get adjacency matrix */
  flp = read_flp (flp_cfg);

  /* 100704 ejt: print number of clusters */
  fprintf (sim_hotspotout_fd, "Total number of clusters are %i.\n", numcontexts);

  /* print figure info */
  print_flp_fig (flp, sim_hotspotout_fd);

  /* figure out indexes and relative sizes of l2cache pieces */
  locate_l2cache ();

  /* compute these fractions */
  FRACTION_INIT (clock1, "", 1);	/* Front end */
  for(i = 0; i < CLUSTERS; i++)
  {
    FRACTION_INIT_CORE (iresultbus, i, 1);
    FRACTION_INIT_CORE (fresultbus, i, 1);
    FRACTION_INIT_CORE (clock2, i, 1);
    FRACTION_INIT_CORE (clock3, i, 1);
  }

  /* compute leakage power constants */
  init_leakage_power ();

  /* initialize the R and C matrices */
  create_RC_matrices (flp, omit_lateral);

  /* allocate the temp and power arrays   */
  /* using hotspot_vector to internally allocate whatever extra nodes needed      */

  temp = hotspot_vector (flp->n_units);
  hotspot_power = hotspot_vector (flp->n_units);
  steady_temp = hotspot_vector (flp->n_units);
  overall_power = hotspot_vector (flp->n_units);

  mid_temp = hotspot_vector (flp->n_units);
  mid_power = hotspot_vector (flp->n_units);

  total_leakage_power = hotspot_vector (flp->n_units);
  cluster_total_leakage_power = hotspot_vector (flp->n_units);
  cluster_average_leakage = hotspot_vector (numcontexts);

  /* set up initial temperatures */
  if (init_file)
    {
      if (!dtm_used)		/* initial T = steady T for no DTM      */
	read_temp (flp, temp, init_file, 0);
      else			/* initial T = clipped steady T with DTM    */
	read_temp (flp, temp, init_file, 1);
    }
  else				/* no input file - use init_temp as the temperature */
    set_temp (temp, flp->n_units, init_temp);
  last_hotspot_cycle = 0;
  total_elapsed_cycles = 0;
}

#define HOTSPOTDUMPSTAT(nicename,cname) fprintf(sim_hotspotout_fd,"%s  \t%.2f\t%.2f\n", nicename, mid_temp[cname]-273.15, temp[cname])
void
hotspot_dumpstats (void)
{
  int n, i;
  double leakage_total;
  /* find the average power dissipated in the elapsed time */
  for (i = 0; i < flp->n_units; i++)
    {
      mid_power[i] = overall_power[i];
      mid_power[i] /= total_elapsed_cycles;
    }
  fprintf (sim_hotspotout_fd, "total elapsed cycles %lld \n",
	   total_elapsed_cycles);
  /* get steady state temperatures */
  steady_state_temp (mid_power, mid_temp, flp->n_units);

  n = flp->n_units;
  fprintf (sim_hotspotout_fd, "Unit    \tSteady \tTransient\n");
  for (i = 0; i < n; i++)
    HOTSPOTDUMPSTAT (flp->units[i].name, i);

  HOTSPOTDUMPSTAT ("spreader_west", SP_W);
  HOTSPOTDUMPSTAT ("spreader_east", SP_E);
  HOTSPOTDUMPSTAT ("spreader_north", SP_N);
  HOTSPOTDUMPSTAT ("spreader_south", SP_S);
  HOTSPOTDUMPSTAT ("spreader_bottom", SP_B);
  HOTSPOTDUMPSTAT ("sink_west", SINK_W);
  HOTSPOTDUMPSTAT ("sink_east", SINK_E);
  HOTSPOTDUMPSTAT ("sink_north", SINK_N);
  HOTSPOTDUMPSTAT ("sink_south", SINK_S);
  HOTSPOTDUMPSTAT ("sink_bottom", SINK_B);

  /* Show leakage */
  leakage_total = 0;
  for (i = 0; i < n; i++)
    {
      leakage_total +=
	LEAKAGEPOWERFROMTEMP (leakage_power_factor[i], mid_temp[i]);
      /* 100604 nan: display current leakage power for each structure in each cluster */
      fprintf (sim_hotspotout_fd, "Current leakage power of %s is %g W.\n",
	       flp->units[i].name,
	       LEAKAGEPOWERFROMTEMP (leakage_power_factor[i], mid_temp[i]));
    }
  /* display current total leakage power */
  fprintf (sim_hotspotout_fd, "Current total leakage power is %g W.\n",
	   leakage_total);
  fflush(sim_hotspotout_fd);
}

void
hotspot_uninit ()
{
  int i, j, k, hotspot_cycles;

  double total_average_leakage = 0;

  /* 100604 ejt: compute cycles */
  hotspot_cycles = sim_cycle - (sim_cycle % hotspot_cycle_sampling_intvl);

  fprintf (sim_hotspotout_fd, "Total elasped cycles is %lld.\n",
	   total_elapsed_cycles);


  /* 100704 ejt: display average leakage power */
  for (i = 0; i < flp->n_units; i++)
    {
      fprintf (sim_hotspotout_fd, "Average leakage power of %s is %g W.\n",
	       flp->units[i].name,
	       total_leakage_power[i] / (total_elapsed_cycles /
					 hotspot_cycle_sampling_intvl));
      total_average_leakage +=
	total_leakage_power[i] / (total_elapsed_cycles /
				  hotspot_cycle_sampling_intvl);
    }

/* 120404 ejt: if FT_CMP defined, then compute average leakage power for each
               core */
#ifdef FT_CMP
  /* 100804 ejt: display average leakage power for no. of active clusters */
  /* 120404 ejt: for the FT CMP configuration, each core has 30 elements */
  for (j = 0; j < numcontexts; j++)
    {
      /* initialize cluster average leakage power array */
      cluster_average_leakage[j] = 0;
      /* sum up every CORE_ELEMENTS number of architectural elements as
         average leakage power for each core */
      for (k = (j * CORE_ELEMENTS); k < ((j * CORE_ELEMENTS) + CORE_ELEMENTS);
	   k++)
	{
	  cluster_average_leakage[j] +=
	    cluster_total_leakage_power[k] / (total_elapsed_cycles /
					      hotspot_cycle_sampling_intvl);
	}
      fprintf (sim_hotspotout_fd,
	       "Total average leakage power for cluster %i is %g W.\n", j,
	       cluster_average_leakage[j]);
    }
#endif

  fprintf (sim_hotspotout_fd,
	   "Total average leakage power for %i cluster elements is %g W.\n",
	   numcontexts, total_average_leakage);
  fprintf (stderr, "avg_leakage_power: %g energy: %g\n", total_average_leakage, total_average_leakage*sim_cycle);

/* 120404 ejt: if FT_CMP defined, then compute total leakage energy for each
               core */
#ifdef FT_CMP
  for (j = 0; j < numcontexts; j++)
    {
      fprintf (sim_hotspotout_fd,
	       "Total leakage energy for cluster %i is %g W.\n", j,
	       cluster_average_leakage[j] * total_elapsed_cycles);
    }
#endif

  fprintf (sim_hotspotout_fd,
	   "Total number of elements in processor architecture are: %i\n", i);

  fprintf (sim_hotspotout_fd, "Average leakage power per cluster is %g W.\n",
	   total_average_leakage / numcontexts);

  /* dump temperatures if needed  */
  if (steady_file)
    dump_temp (flp, steady_temp, steady_file);

  /* cleanup.. */
  cleanup_hotspot (flp->n_units);
  free_flp (flp);
  free_vector (temp);
  free_vector (hotspot_power);
  free_vector (steady_temp);
  free_vector (overall_power);

  free_vector (mid_temp);
  free_vector (mid_power);

  free_vector (total_leakage_power);
  free_vector (cluster_total_leakage_power);
  free_vector (cluster_average_leakage);
}

void
update_hotspot_stats (int elapsed_cycles)
{
  int i;

/* 120304 ejt: if FT_CMP defined, then electrical and optical qram, optical mod
               and rec are not used, l is used as counter */
#ifndef FT_CMP
  int l;
#endif

  double elapsed_time;
  double l2cache_power = 0;
  char func_unit[80];
#ifdef GET_BLK_INDEX
  GET_BLK_INDEX_CACHE_OPEN ();
#endif

#ifndef GET_BLK_INDEX
#define UPDATEHSSTAT(name,member) 	sprintf(func_unit,"c%d_%s",i,name); \
					hotspot_power[get_blk_index(flp, func_unit)] += hotspot_perperiod[i].member; \
					hotspot_perperiod[i].member=0
#else
#define UPDATEHSSTAT(name,member) 	GET_BLK_INDEX_CACHE(name); \
					hotspot_power[GET_BLK_INDEX()] += hotspot_perperiod[i].member; \
					hotspot_perperiod[i].member=0
#endif



#define UPDATEHSSTATFE(name,member)	hotspot_power[get_blk_index(flp, name)] += hotspot_perperiod[i].member; \
					hotspot_perperiod[i].member=0

#define UPDATEHSSTATFE8(name,member) 	sprintf(func_unit,"fe8_%s_%d",name,i+1); \
					hotspot_power[get_blk_index(flp, func_unit)] += hotspot_perperiod[i].member; \
					hotspot_perperiod[i].member=0
  i = 0;			/* Front End, shared units */

/* 120304 ejt: if FT_CMP is defined, allow chkptbuf, broutcomeq, postcombuf to 
               be inside core */
/* 120304 ejt: compute bpred, icache, fetch, itlb, rob and rename for clock1 
               power as follows for 4 threads, i.e. inside core */
  for (i = 0; i < numcontexts; i++)
  {
      UPDATEHSSTAT ("bpred", bpred);
      UPDATEHSSTAT ("L2SysInterface", sysintf);
      UPDATEHSSTAT ("Icache", icache);
      UPDATEHSSTAT ("ITB", itlb);
      UPDATEHSSTAT ("Fetch", fetch);
      UPDATEHSSTAT ("ROB", rob);
      UPDATEHSSTAT ("FPRename", frename);
      UPDATEHSSTAT ("IntRename", irename);

      FRACTION_UPDATEHSSTAT_ALL (clock1, i);	/* Front end clock power */
  }

  //FRACTION_UPDATEHSSTAT_ALL (clock1);	/* Front end clock power */

  for (i = 0; i < numcontexts; i++)
  {
      UPDATEHSSTAT ("FPMult", fmul);
      UPDATEHSSTAT ("FPReg", freg);
      UPDATEHSSTAT ("FPAdd", fadd);

      FRACTION_UPDATEHSSTAT_CL (fresultbus, i);

      UPDATEHSSTAT ("DTB0", dtb0);
      UPDATEHSSTAT ("FPQ", fqueue);
      UPDATEHSSTAT ("IntQ", iqueue);
      UPDATEHSSTAT ("LdStQ", lsq);
      UPDATEHSSTAT ("IntReg0", ireg0);
      UPDATEHSSTAT ("IntAlu0", ialu0);
      UPDATEHSSTAT ("IntMult", ialu2);
      UPDATEHSSTAT ("DTB1", dtb1);
      UPDATEHSSTAT ("IntReg1", ireg1);
      UPDATEHSSTAT ("IntAlu1", ialu1);
      UPDATEHSSTAT ("IntAlu2", ialu3);

      FRACTION_UPDATEHSSTAT_CL (iresultbus, i);


      UPDATEHSSTAT ("Dcache", dl1cache);

      FRACTION_UPDATEHSSTAT_ALLCL (clock2, i);	/* Int clock power */
      FRACTION_UPDATEHSSTAT_ALLCL (clock3, i);	/* FP clock power */

      l2cache_power += hotspot_perperiod[i].ul2cache;
      hotspot_perperiod[i].ul2cache = 0;
  }


  /* distribute l2cache usage */
  for (i = 0; i < l2cache_count; i++)
  {
      hotspot_power[l2cache_blk_index[i]] += l2cache_power * l2cache_areas[i];
  }

  for (i = 0; i < numcontexts; i++)
  {
    FRACTION_UPDATESDONE (iresultbus,i);
    FRACTION_UPDATESDONE (fresultbus,i);
    FRACTION_UPDATESDONE (clock1,i);
    FRACTION_UPDATESDONE (clock2,i);
    FRACTION_UPDATESDONE (clock3,i);
  }

  elapsed_time = elapsed_cycles / base_proc_freq;

  /* Add leakage power */
  for (i = 0; i < flp->n_units; i++)
    {
      /* FIXME Should this elapsed time here really be elapsed_cycles rather than elasped_time? nan 100704 */
      hotspot_power[i] +=
	elapsed_cycles * LEAKAGEPOWERFROMTEMP (leakage_power_factor[i],
					       mid_temp[i]);
      total_leakage_power[i] +=
	LEAKAGEPOWERFROMTEMP (leakage_power_factor[i], mid_temp[i]);
      cluster_total_leakage_power[i] +=
	LEAKAGEPOWERFROMTEMP (leakage_power_factor[i], mid_temp[i]);
    }

  /* find the average power dissipated in the elapsed time */
  for (i = 0; i < flp->n_units; i++)
    {
      overall_power[i] += hotspot_power[i];
      /* 
       * 'power' array is an aggregate of per cycle numbers over 
       * the sampling_intvl. so, compute the average power 
       */
      hotspot_power[i] /= (elapsed_cycles);
    }

  /* calculate the current temp given the previous temp,
   * time elapsed since then, and the average power dissipated during 
   * that interval */
  compute_temp (hotspot_power, temp, flp->n_units, elapsed_time);
  total_elapsed_cycles += elapsed_cycles;

  /* reset the power array */
  for (i = 0; i < flp->n_units; i++)
    hotspot_power[i] = 0;
#ifdef GET_BLK_INDEX
  GET_BLK_INDEX_CACHE_CLOSE ();
#endif
  hotspot_calls++;
  if (hotspot_calls > hotspot_dump_freq)
    {
      hotspot_calls = 0;
      hotspot_dumpstats ();
    }
}

/* 120504 ejt: function to put total leakage energy for each core into
               <benchmark>.txt file */
void
leakage_power_value (FILE * fp)
{
  int j, k;
  double cache_average_leakage = 0;

  for (j = 0; j < numcontexts; j++)
    {
      /* initialize cluster average leakage power array */
      cluster_average_leakage[j] = 0;
      /* sum up every CORE_ELEMENTS number of architectural elements as
         average leakage power for each core */
      for (k = (j * CORE_ELEMENTS); k < ((j * CORE_ELEMENTS) + CORE_ELEMENTS);
	   k++)
	{
	  cluster_average_leakage[j] +=
	    cluster_total_leakage_power[k] / (total_elapsed_cycles /
					      hotspot_cycle_sampling_intvl);
	}

      cluster_average_leakage[j] -=
	cluster_total_leakage_power[j * CORE_ELEMENTS +
				    10] / (total_elapsed_cycles /
					   hotspot_cycle_sampling_intvl);
      cluster_average_leakage[j] -=
	cluster_total_leakage_power[j * CORE_ELEMENTS +
				    12] / (total_elapsed_cycles /
					   hotspot_cycle_sampling_intvl);
      cluster_average_leakage[j] -=
	cluster_total_leakage_power[j * CORE_ELEMENTS +
				    17] / (total_elapsed_cycles /
					   hotspot_cycle_sampling_intvl);
      cluster_average_leakage[j] -=
	cluster_total_leakage_power[j * CORE_ELEMENTS +
				    21] / (total_elapsed_cycles /
					   hotspot_cycle_sampling_intvl);

      if (!j)
	{
	  cache_average_leakage +=
	    cluster_total_leakage_power[j * CORE_ELEMENTS +
					10] / (total_elapsed_cycles /
					       hotspot_cycle_sampling_intvl);
	  cache_average_leakage +=
	    cluster_total_leakage_power[j * CORE_ELEMENTS +
					12] / (total_elapsed_cycles /
					       hotspot_cycle_sampling_intvl);
	  cache_average_leakage +=
	    cluster_total_leakage_power[j * CORE_ELEMENTS +
					17] / (total_elapsed_cycles /
					       hotspot_cycle_sampling_intvl);
	  cache_average_leakage +=
	    cluster_total_leakage_power[j * CORE_ELEMENTS +
					21] / (total_elapsed_cycles /
					       hotspot_cycle_sampling_intvl);
	}


    }
  /* write data into <benchmark>.txt file */
  fprintf (fp, "%.2f\t%.2f\t%.2f\t%.2f\t",
	   cluster_average_leakage[0] * total_elapsed_cycles,
	   cluster_average_leakage[1] * total_elapsed_cycles,
	   cluster_average_leakage[2] * total_elapsed_cycles,
	   cache_average_leakage * total_elapsed_cycles);
}



void
leakage_power_clear ()
{

  int j, k;

  for (j = 0; j < numcontexts; j++)
    {
      for (k = (j * CORE_ELEMENTS); k < ((j * CORE_ELEMENTS) + CORE_ELEMENTS);
	   k++)
	{
	  cluster_total_leakage_power[k] = 0;
	}

      cluster_total_leakage_power[j * CORE_ELEMENTS + 10] = 0;
      cluster_total_leakage_power[j * CORE_ELEMENTS + 12] = 0;
      cluster_total_leakage_power[j * CORE_ELEMENTS + 17] = 0;
      cluster_total_leakage_power[j * CORE_ELEMENTS + 21] = 0;

      if (!j)
	{
	  cluster_total_leakage_power[j * CORE_ELEMENTS + 10] = 0;
	  cluster_total_leakage_power[j * CORE_ELEMENTS + 12] = 0;
	  cluster_total_leakage_power[j * CORE_ELEMENTS + 17] = 0;
	  cluster_total_leakage_power[j * CORE_ELEMENTS + 21] = 0;
	}
    }
}

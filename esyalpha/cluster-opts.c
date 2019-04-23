#include "headers.h"

/** link config **/
char * link_definitions[MAX_IC_INDEX];
int    link_definitions_argc;
char * link_custom_destinations[MAX_IC_INDEX];
int    link_custom_destinations_argc;


/* *** options *** */
int n_way;
int n_way_cache;

int n_way_adjacent;

int uni_lat_comm;
int two_lat_comm;
#ifndef CONFIG_BASED_ICS
int link_width;
int hop_time;

char *optical_type;
/* This is here so we dont need to call mystricmp all the time */
char optical_type_char; /* N,T,G,F,M for none, tent, grid, flower, or mesh */

int optical_comm;
int optical_link_width;
int optical_hop_time;
#endif
#ifdef CONFIG_BASED_ICS
int fe_link_time;
#endif
#ifdef TOKENB
int tokenB;
#endif

/* stats */
counter_t *optical_link_width_dist;
counter_t comm_opt_qram_access_cl[CLUSTERS];

counter_t total_slip_cycles;
counter_t max_slip_cycles;
counter_t total_issue_slip_cycles;
counter_t max_issue_slip_cycles;


/* *** stats *** */

static char *delay_dist_str[DELAY_DIST_NUMBER] = {
  "<=.1*TIMEWINDOW",
  "<=.2*TIMEWINDOW",
  "<=.3*TIMEWINDOW",
  "<=.4*TIMEWINDOW",
  "<=.5*TIMEWINDOW",
  "<=.6*TIMEWINDOW",
  "<=.7*TIMEWINDOW",
  "<=.8*TIMEWINDOW",
  "<=.9*TIMEWINDOW",
  "<=   TIMEWINDOW",
};

counter_t total_delay;

counter_t total_number_delay;
struct stat_stat_t *delay_dist = NULL;

counter_t st_total_delay;
struct stat_stat_t *st_delay_dist = NULL;


counter_t total_hop;
struct stat_stat_t *hop_dist = NULL;
/*commit width staistics */
struct stat_stat_t *commit_width_stat;

void
cluster_reg_options(struct opt_odb_t *odb)
{
  opt_reg_int(odb, "-n_way",
	      "number of clusters assigned to a thread",
	      &n_way, /* default */N_WAY,
	      /* print */TRUE, /* format */NULL);
	      
	       
  opt_reg_int(odb, "-n_way_cache",
	      "number of cluster caches assigned to a thread",
	      &n_way_cache, /* default */N_WAY_CACHE,
	      /* print */TRUE, /* format */NULL);
	      
	        
  opt_reg_flag(odb, "-n_way_adjacent",
	      "Assigns starting threads at theadid * n_way instead of CLUSTERS/numthreads * threadid (well the min of the two choices when true)",
	      &n_way_adjacent, /* default */FALSE,
	      /* print */TRUE, /* format */NULL);
	      
	      
  opt_reg_flag(odb, "-uni-lat-comm", "All communications take 1 cycle if from 2 different nodes",
	       &uni_lat_comm, /* default */FALSE, /* print */TRUE, NULL);
  opt_reg_flag(odb, "-two-lat-comm", "All communications take 2 cycle if from 2 different nodes (1 cycle for adjacent)",
	       &two_lat_comm, /* default */FALSE, /* print */TRUE, NULL);
#ifndef CONFIG_BASED_ICS
  opt_reg_string(odb, "-optical:type",
		 "optical connection_type {tent|flower|mesh|none}",
                 &optical_type, /* default */"none",
                 /* print */TRUE, /* format */NULL);
  opt_reg_int(odb, "-link:width",
	      "Width of each link (registers/link/cycle)",
	      &link_width, /* default */LINK_WIDTH,
	      /* print */TRUE, /* format */NULL);
  opt_reg_int(odb, "-hop_time",
	      "Cycles to transverse electrical link (in cycles and pipelined)",
	      &hop_time, /* default */HOPTIME,
	      /* print */TRUE, /* format */NULL);
#ifdef OPTICAL_TENT
  opt_reg_flag(odb, "-opticaltent", "Performs communication mimicking an optical tent",
	       &optical_comm, /* default */FALSE, /* print */TRUE, NULL);
  opt_reg_int(odb, "-optical:width",
	      "Width of each optical link (registers/link/cycle)",
	      &optical_link_width, /* default */OPTICAL_LINK_WIDTH,
	      /* print */TRUE, /* format */NULL);
  opt_reg_int(odb, "-optical:hop_time",
	      "Cycles to transverse optical link (in cycles and pipelined)",
	      &optical_hop_time, /* default */OPTICAL_HOP_TIME,
	      /* print */TRUE, /* format */NULL);
#endif //OPTICAL_TENT
#endif //!CONFIG_BASED_ICS

/** Here is where we allow the config files to define the optical connections (grbriggs) **/
  opt_reg_string_list(odb,"-link:type","The link definitions, 'name:type:width:hoptime:physics' with type like 'ER' and physics of 'electrical' or 'optical'",
		      link_definitions, MAX_IC_INDEX, &link_definitions_argc,/* default */ NULL,  
		      /* print */ TRUE, /* format */ NULL,
		      /* accrue? */ TRUE);

  opt_reg_string_list(odb,"-link:custom","Custom link destination definition, 'name:to0,to1,...' with toi = -1 for no link, specifyig where the link goes from each core.",
		      link_custom_destinations, MAX_IC_INDEX, &link_custom_destinations_argc,/* default */ NULL,  
		      /* print */ TRUE, /* format */ NULL,
		      /* accrue? */ TRUE);


#ifdef CONFIG_BASED_ICS
  opt_reg_int(odb, "-fe_link_time",
	      "Fixed time for a cluster node to communicate with the FE. If negative, it will use a noncongested electrical network instead.",
	      &fe_link_time, /* default */-1,
	      /* print */TRUE, /* format */NULL);
#endif //CONFIG_BASED_ICS
#ifdef TOKENB
  opt_reg_flag(odb, "-tc",
	  "Token B counting like in Wood et al. ISCA 2003",
	  &tokenB, FALSE,
	  TRUE, NULL);
#endif

}
void
cluster_check_options(void)
{
#ifndef CONFIG_BASED_ICS
    optical_type_char = toupper(optical_type[0]);

    master_link_width[OR_INDEX] = optical_link_width;
    master_link_width[OL_INDEX] = optical_link_width;
    master_link_width[ER_INDEX] = link_width;
    master_link_width[EL_INDEX] = link_width;
    master_link_width[AC_INDEX] = optical_link_width;
    master_link_width[OT_INDEX] = optical_link_width;
    master_link_width[EU_INDEX] = link_width;
    master_link_width[ED_INDEX] = link_width;

    master_hop_time[OR_INDEX] = optical_hop_time;
    master_hop_time[OL_INDEX] = optical_hop_time;
    master_hop_time[ER_INDEX] = hop_time;
    master_hop_time[EL_INDEX] = hop_time;
    master_hop_time[AC_INDEX] = optical_hop_time;
    master_hop_time[OT_INDEX] = optical_hop_time;
    master_hop_time[EU_INDEX] = hop_time;
    master_hop_time[ED_INDEX] = hop_time;
#endif
}
void 
cluster_reg_stats(struct stat_sdb_t *sdb)   /* cluster stats database */
{
  stat_reg_counter(sdb, "total_number_delay",
		   "total number of times ring_lat was called",
		   &total_number_delay, /* initial value */0, /* format */NULL);

  stat_reg_counter(sdb, "total_delay",
		   "total delay time in cycles",
		   &total_delay, /* initial value */0, /* format */NULL);
  stat_reg_formula(sdb, "lat_delay_avg",
		   "delay per time called",
		   "total_delay / total_number_delay", /* format */NULL);
  stat_reg_formula(sdb, "lat_delay_cycle",
		   "delay per cycle",
		   "total_delay / sim_cycle", /* format */NULL);


  stat_reg_counter(sdb, "total_hop",
		   "total number of hops to be transversed",
		   &total_hop, /* initial value */0, /* format */NULL);
  stat_reg_formula(sdb, "hops_avg",
		   "hops per time called",
		   "total_hop / total_number_delay", /* format */NULL);
  stat_reg_formula(sdb, "hop_cycle",
		   "hops per cycle",
		   "total_hop / sim_cycle", /* format */NULL);

  stat_reg_counter(sdb, "total_slip_cycles",
	  	   "total number of slip cycles",
		   &total_slip_cycles, 0 , NULL );
  stat_reg_formula(sdb, "avg_slip_cycle",
	  	   "Average cycles from dispatch to commit",
		   "total_slip_cycles / sim_num_insn", NULL );
  stat_reg_counter(sdb, "max_slip_cycles",
	  	   "max number of slip cycles",
		   &max_slip_cycles, 0 , NULL );

  stat_reg_counter(sdb, "total_issue_slip_cycles",
	  	   "total number of issue slip cycles",
		   &total_issue_slip_cycles, 0 , NULL );
  stat_reg_formula(sdb, "avg_issue_slip_cycle",
	  	   "Average cycles from issue to commit",
		   "total_issue_slip_cycles / sim_num_insn", NULL );
  stat_reg_counter(sdb, "max_issue_slip_cycles",
	  	   "max number of issue slip cycles",
		   &max_issue_slip_cycles, 0 , NULL );


  delay_dist = stat_reg_dist(sdb, "delay_dist",
                              "Distribution of delays",
                              /* initial value */0,
                              /* array size */10,
                              /* bucket size */TIMEWINDOW/DELAY_DIST_NUMBER,
                              /* print format */(PF_COUNT|PF_PDF),
                              /* format */NULL,
                              /* index map */delay_dist_str,
                              /* print fn */NULL);

  st_delay_dist = stat_reg_dist(sdb, "st_delay_dist",
                              "Distribution of st delays",
                              /* initial value */0,
                              /* array size */10,
                              /* bucket size */TIMEWINDOW/DELAY_DIST_NUMBER,
                              /* print format */(PF_COUNT|PF_PDF),
                              /* format */NULL,
                              /* index map */delay_dist_str,
                              /* print fn */NULL);

  hop_dist = stat_reg_dist(sdb, "hop_dist",
                              "Distribution of hop distance",
                              /* initial value */0,
                              /* array size */CLUSTERS,
                              /* bucket size */1,
                              /* print format */(PF_COUNT|PF_PDF),
                              /* format */NULL,
                              /* index map */NULL,
                              /* print fn */NULL);
  commit_width_stat = stat_reg_dist(sdb, "commit_width_stat", "This shows the commit width usage, in buckets of 4",
		                         0, 1 + 64*8/4, 4, PF_ALL, NULL, NULL, NULL);
}

#define DELAY_DIST_NUMBER 10
/* *** options *** */

extern int n_way;
extern int n_way_cache;
extern int n_way_adjacent;
extern int uni_lat_comm;
extern int two_lat_comm;
extern int link_width;
extern int hop_time;

extern int tokenB;

extern char *optical_type;
extern char optical_type_char;
extern int optical_link_width;
extern int optical_comm;
extern int optical_hop_time;

/* stats */
extern counter_t *optical_link_width_dist;
extern counter_t comm_opt_qram_access_cl[CLUSTERS];
extern counter_t total_slip_cycles;
extern counter_t max_slip_cycles;
extern counter_t total_issue_slip_cycles;
extern counter_t max_issue_slip_cycles;



/* *** stats *** */

extern counter_t total_delay;
extern counter_t total_number_delay;
extern counter_t st_total_delay;
extern struct stat_stat_t *delay_dist;
extern struct stat_stat_t *st_delay_dist;
extern struct stat_stat_t *commit_width_stat;

extern counter_t total_hop;
extern struct stat_stat_t *hop_dist;


void cluster_reg_options(struct opt_odb_t *odb);
void cluster_reg_stats(struct stat_sdb_t *sdb);
void cluster_check_options(void);


/** link config **/
extern char * link_definitions[MAX_IC_INDEX];
extern int    link_definitions_argc;
extern char * link_custom_destinations[MAX_IC_INDEX];
extern int    link_custom_destinations_argc;


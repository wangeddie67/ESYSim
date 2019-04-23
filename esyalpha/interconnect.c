
#include "headers.h"
void dump_route_table(void);
/*
 * $Log: interconnect.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:35  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:54  garg
 * dir structure
 *
 * Revision 1.2  2004/09/28  01:28:13  etan
 * only declare comm power when comm link costs are considered.
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.26  2004/08/02 17:21:35  ninelson
 * added support for tokenB
 *
 * Revision 1.25  2004/07/28 04:32:52  grbriggs
 * Add flood_lat function.
 *
 * Revision 1.24  2004/07/24 03:33:38  grbriggs
 * For CONFIG_BASED_ICS, fully remove the hop_time and link_width generic
 * config arguments, and remove any residual code that was still using
 * those old values.
 *
 * Revision 1.23  2004/07/22 18:05:38  grbriggs
 * Make 2 loops more efficient by using link_definitions_argc.
 *
 * Revision 1.22  2004/07/13 15:18:28  grbriggs
 * Fix problem in previous checkin for tracking comm use types.
 * Add accounting for RX unit communication.
 *
 * Revision 1.21  2004/07/12 17:16:54  grbriggs
 * Add per-usage-type statistics for comm links.
 *
 * Revision 1.20  2004/07/08 17:24:48  grbriggs
 * Added cl_comm_lat for multi-packet communication between clusters.
 *
 * Revision 1.19  2004/06/30 20:51:33  grbriggs
 * Warn if using AC or grid links when clusters != 16.
 *
 * Revision 1.18  2004/06/30 15:30:53  ninelson
 * Bug fix.
 *
 * Revision 1.17  2004/06/26 02:58:29  ninelson
 * 8 bits in a byte for bitmap, not 1.
 *
 * Revision 1.16  2004/06/15 22:07:19  grbriggs
 * fatal() instead of infinite loop when there are no links.
 *
 * Revision 1.15  2004/06/15 18:43:01  grbriggs
 * Added new table total_noncongested_comm_delay.
 * Fixed thinko in previous commit.
 * Added some comments.
 *
 * Revision 1.14  2004/06/15 17:23:30  grbriggs
 * Add check that we don't overflow when populating nh_table.
 *
 * Revision 1.13  2004/06/09 17:18:38  grbriggs
 * Complete changes in support of custom links specified via config.
 * (We should probably remove CONFIG_BASED_ICS as if it were always defined,
 * but we haven't removed it yet.)
 *
 * Revision 1.12  2004/06/07 22:31:16  grbriggs
 * Preliminary support for configuration-based link definitions (CONFIG_BASED_ICS).
 *
 * Revision 1.11  2004/06/04 17:55:32  ninelson
 * added support for grids/bug fixed. Moved electrical links to head of list
 *
 * Revision 1.10  2004/05/31 11:11:09  ninelson
 * Arbitrary routing table is now fully active.
 *
 * Revision 1.9  2004/05/28 23:11:01  ninelson
 * more bug fixes
 *
 * Revision 1.8  2004/05/28 14:34:00  ninelson
 * added arbitrary routing init and split link stats into _rx and _tx
 *
 * Revision 1.7  2004/05/27 22:44:13  ninelson
 * bug fixes and support for a dual rail interconnection
 *
 * */


    /*
#define A(x) (0<=x && x<=4)
#define B(x) (4<=x && x<=8)
#define C(x) (8<=x && x<=12)
#define D(x) ((12<=x && x<=16)||x==0)
#define CORNER(x) ((x % 4) == 0)


#define NODE_TYPE(x) (x&0x3)

#define OR 0x01		from node to node + 4
#define OL 0x02		from node to node - 4
#define ER 0x04		from node to node + 1
#define EL 0x08		from node to node - 1
#define AC 0x10		from node to ac_table
#define OT 0x20		from node to node + 8
*/
int master_qram_access_cl[MAX_IC_INDEX][CLUSTERS];
int master_link_rx_access_cl[MAX_IC_INDEX][CLUSTERS];
int master_link_tx_access_cl[MAX_IC_INDEX][CLUSTERS];
#ifdef TRACK_COMM_USE_TYPES
int master_link_tx_use_type_access[MAX_IC_INDEX][LENGTHOF_comm_use_type_enum];
char * comm_use_type_enum_names[] = {"sendregister", "l2comm", "flood", "bare_token", "data_token", "other"};
#endif //TRACK_COMM_USE_TYPES

/*
int master_total_qram_access[MAX_IC_INDEX][CLUSTERS];
int master_max_qram_access[MAX_IC_INDEX][CLUSTERS];
*/

#ifdef CONFIG_BASED_ICS
int master_transverse_link_table[MAX_IC_INDEX][CLUSTERS];
link_info_t link_info[MAX_IC_INDEX];
int total_noncongested_comm_delay[CLUSTERS][CLUSTERS];
#endif

int ac_table[16] = {-1,11,10,9,		-1,15,14,13,	-1,3,2,1,	-1,7,6,5};
#if CLUSTERS != 16
#warning Optical AC links will not be supported in this build because the number of clusters != 16
#endif

/* 0 is right, 1 is left 2 up 3 is down (hardcoded in init_link_descr_tables) */

int grid_table_16[4][16] = { {1,14,3,12,	5,10,7,8,	9,-1,11,-1,	13,-1,15,-1},
    			     {-1,0,-1,2,	-1,4,-1,6, 	7,8,5,10,	3,12,1,14},
			     {2,3,4,5,		6,7,-1,-1,	-1,-1,8,9,	10,11,12,13},
			     {-1,-1,0,1,	2,3,4,5,	10,11,12,13,	14,15,-1,-1}
};
#if CLUSTERS != 16
#warning Electrical grid-arranged links will not be supported in this build because the number of clusters != 16
#endif


int master_link_width[MAX_IC_INDEX];
int master_hop_time[MAX_IC_INDEX];
#ifndef CONFIG_BASED_ICS
int reverse_link[MAX_IC_INDEX];
#endif

#ifdef THRD_WAY_CACHE
extern int n_cache_start_thrd[];
extern int n_cache_limit_thrd[];
#endif

/** Power **/
/* 092404 ejt: only declare comm power when comm link costs are considered */
#ifdef COMM_LINK_PWR 
DECLARE_POWER_STATS(qram);
DECLARE_POWER_STATS(link_rx);
DECLARE_POWER_STATS(link_tx);
#endif // for COMM_LINK_PWR

void clear_interconnect_access_stats(void)
{
    int link_num,i;
#ifdef CONFIG_BASED_ICS
    for(link_num=0;link_num<link_definitions_argc;link_num++)
#else
    for(link_num=0;link_num<MAX_IC_INDEX;link_num++)
#endif
    {
	for(i=0;i<CLUSTERS;i++)
	{
	    master_qram_access_cl[link_num][i] = 0;
	    master_link_tx_access_cl[link_num][i] = 0;
	    master_link_rx_access_cl[link_num][i] = 0;
	}
#ifdef TRACK_COMM_USE_TYPES
        for (i=0; i < LENGTHOF_comm_use_type_enum; i++)
        {
  	    master_link_tx_use_type_access[link_num][i] = 0;
        }
    }
#endif // for TRACK_COMM_USE_TYPES
}
void total_interconnect_power_stats(void)
{
    int l,i;
    for(l=0;l<MAX_IC_INDEX;l++)
    {
#define RESET_POWER_TOTALS(targ) do {\
	master_##targ##_power_cc1[l][CLUSTER_TOTAL] = 0; \
	master_##targ##_power_cc2[l][CLUSTER_TOTAL] = 0; \
	master_##targ##_power_cc3[l][CLUSTER_TOTAL] = 0; } while(0)

#define ADD_POWER_TOTAL(targ) do {\
	master_##targ##_power_cc1[l][CLUSTER_TOTAL] += master_##targ##_power_cc1[l][i]; \
	master_##targ##_power_cc2[l][CLUSTER_TOTAL] += master_##targ##_power_cc2[l][i]; \
	master_##targ##_power_cc3[l][CLUSTER_TOTAL] += master_##targ##_power_cc3[l][i]; } while(0)
	RESET_POWER_TOTALS(qram);
	RESET_POWER_TOTALS(link_rx);
	RESET_POWER_TOTALS(link_tx);

	for(i=0;i<CLUSTERS;i++)
	{
	    ADD_POWER_TOTAL(qram);
	    ADD_POWER_TOTAL(link_tx);
	    ADD_POWER_TOTAL(link_rx);
	}
    }
#undef RESET_POWER_TOTAL
#undef ADD_POWER_TOTAL
}

#ifdef CONFIG_BASED_ICS
/*
 * This function initializes the tables containing the
 * properties of the interconnects according to the
 * configuration parameters.
 */
void init_link_descr_tables()
{
    int i,from,j;

    for /* each link type */ (i=0;i<link_definitions_argc;i++) 
	{
	char * link_descr = link_definitions[i];
	char * parseptr;
	char * link_type_name;
	char * link_type_physics;
	link_info[i].longname =       strtok_r(link_descr,":",&parseptr);
	link_type_name =              strtok_r(NULL,":",&parseptr);
	master_link_width[i] = strtol(strtok_r(NULL,":",&parseptr),NULL,0);
	master_hop_time[i] =   strtol(strtok_r(NULL,":",&parseptr),NULL,0);
	link_type_physics =           strtok_r(NULL,":",&parseptr);

	link_info[i].type = link_type_name;

	if (!mystricmp(link_type_physics,"optical"))
	    link_info[i].physics=optical;
	else if (!mystricmp(link_type_physics,"electrical"))
	    link_info[i].physics=electrical;
	else
	    fatal("The link physics must be 'optical' or 'electrical', but you gave '%s'.",link_type_physics);

	if (!mystricmp(link_type_name,"custom")) {
	    int found = 0;
	    for (j = 0; j < link_custom_destinations_argc; j++)
	    {
	        char * link_type_name_custom;
	        char * link_destinations;
		link_type_name_custom = strtok_r(link_custom_destinations[j],":",&parseptr);
		link_destinations = strtok_r(NULL,":",&parseptr);
		if (!mystricmp(link_type_name_custom,link_info[i].longname)){
		    /* OK, we have found the matching config line */
		    ++found;

		    link_info[i].type = link_info[i].longname; //use the long name instead of saying "custom" for each

		    master_transverse_link_table[i][0]=strtol(strtok_r(link_destinations,",",&parseptr),NULL,0);
		    for (from=1;from<CLUSTERS;from++) {
			  master_transverse_link_table[i][from]=strtol(strtok_r(NULL,",",&parseptr),NULL,0);
		    }
		}
		/* declobber link_custom_destinations[j], which strtok clobbered, 
		 * in favor of clobbering link_type_name_custom */
		*(link_destinations - 1) = ':';
	    }
	    if (found != 1) {
		fatal("Found %d definitions for custom link %s",found,link_info[i].longname);
	    }
	} else for /* each cluster node */ (from=0;from<CLUSTERS;from++)
	{

#define CHECK_GRID_VALID \
		if (sizeof(grid_table_16)/sizeof(grid_table_16[0][0])/4 != CLUSTERS) \
		    fatal("init_link_descr_tables: Your grid_table is for %d clusters, but you are using %d clusters.",sizeof(grid_table_16)/sizeof(grid_table_16[0][0])/4,CLUSTERS)

	    /* grid */ 
	    if (!mystricmp(link_type_name,"ER_grid")) {
		CHECK_GRID_VALID;
		master_transverse_link_table[i][from] = grid_table_16[0][from];
	    } else if (!mystricmp(link_type_name,"EL_grid")) {
		CHECK_GRID_VALID;
		master_transverse_link_table[i][from] = grid_table_16[1][from];
	    } else if (!mystricmp(link_type_name,"EU_grid")) {
		CHECK_GRID_VALID;
		master_transverse_link_table[i][from] = grid_table_16[2][from];
	    } else if (!mystricmp(link_type_name,"ED_grid")) {
		CHECK_GRID_VALID;
		master_transverse_link_table[i][from] = grid_table_16[3][from];

	    /* ring */
	    } else if (!mystricmp(link_type_name,"ER")) {
		master_transverse_link_table[i][from] = CL_ADD(from,1);
	    } else if (!mystricmp(link_type_name,"EL")) {
		master_transverse_link_table[i][from] = CL_SUB(from,1);
	    /* unused
	    } else if (!stricmp(link_type_name,"EU")) {
		master_transverse_link_table[i][from] = from + 4 < CLUSTERS ? from + 4 : -1;
	    } else if (!stricmp(link_type_name,"ED")) {
		master_transverse_link_table[i][from] = from - 4 >= 0 ? from - 4 : -1;
	    */
	    } else if (!mystricmp(link_type_name,"AC")) {
		if (sizeof(ac_table)/sizeof(ac_table[0]) != CLUSTERS)
		    fatal("init_link_descr_tables: Your ac_table is for %d clusters, but you are using %d clusters.",sizeof(ac_table)/sizeof(ac_table[0]),CLUSTERS);
		master_transverse_link_table[i][from] = ac_table[from];
	    } else if (!mystricmp(link_type_name,"OT")) {
		master_transverse_link_table[i][from] = CL_ADD(from,(CLUSTERS/2));
	    } else if (!mystricmp(link_type_name,"OL")) {
		master_transverse_link_table[i][from] = (from % 4) == 0 ? CL_SUB(from,(CLUSTERS/4)) : -1;
	    } else if (!mystricmp(link_type_name,"OR")) {
		master_transverse_link_table[i][from] = (from % 4) == 0 ? CL_ADD(from,(CLUSTERS/4)) : -1;

	    /* custom */
	    //} else if (!mystricmp(link_type_name,"custom")) {

	    /* unknown */
	    } else {
		fatal("link type '%s' ('%s') is not known by init_link_descr_tables().",
			link_type_name,
			link_info[i].longname);
	    }
	}
	fprintf(stderr,"Added link %s: %s, type %s, width %d, and hop time %d.\n",
	       link_info[i].longname,link_type_physics,link_info[i].type,master_link_width[i],master_hop_time[i]);
    }
}

int tranverse_link(int link_type, int from)
{
	return master_transverse_link_table[link_type][from];
}
#else /* CONFIG_BASED_ICS */
void init_link_descr_tables() {}

int tranverse_link(int link_type, int from)

{
    int to;

    /*  to = master_tranverse_link_table[link_type][from]; */
    if(optical_type_char=='G')
	to = grid_table_16[link_type][from];
    else
    switch (link_type)
    {
	default:
	    fatal("bad link type %d",link_type);
	    break;

	case OR_INDEX:
	    to = CL_ADD(from,4);
	break;
	
	case OL_INDEX:
	    to = CL_SUB(from,4);
	    break;
	    
	case ER_INDEX:
	    to = CL_ADD(from,1);
	    break;

	case EL_INDEX:
	    to = CL_SUB(from,1);
	    break;

	case AC_INDEX:
	    to = ac_table[from];
	    break;

	case OT_INDEX:
	    to = CL_ADD(from,(CLUSTERS/2));
	    break;

	case EU_INDEX:
	    to = from + 4;
	    break;

	case ED_INDEX:
	    to = from - 4;
	    break;
    }
    /*
    fprintf (stderr,"%d is to\n",to);
    */
    if(to > CLUSTERS)
	fatal("to (%d) is over (from %d) (link %d)",to,from,link_type);
    return to;
}
#endif /* CONFIG_BASED_ICS */

#ifndef CONFIG_BASED_ICS
char *IC_index2string[MAX_IC_INDEX] = {
    "Electrical Right",
    "Electrical Left",
    "Electrical Up",
    "Electrical Down",
    "Optical Right",
    "Optical Left",
    "Optical Across",
    "Optical Tent"
};
#endif
#if 0
int nh_table_flower_o2_e1[4][16] = 
/*			0-3			4-7			8-11				12-15	*/
		{{0,ER,ER,OR|ER,	OR,OR,OR,OR|OL,			OR|OL,OR|OL,OL,OL,		OL,OL|EL,EL,EL},
		 {0,ER,ER,EL|ER,	EL|ER,EL|ER,EL|ER,EL|ER,	EL|ER,EL,EL,EL,			EL,EL,EL,EL},
		 {0,ER,ER,ER,		ER,ER,ER,ER,			ER|EL,EL,EL,EL,			EL,EL,EL,EL},
		 {0,ER,ER,ER,		ER,ER,ER,ER,			EL|ER,ER|EL,ER|EL,ER|EL,	ER|EL,ER|EL,EL,EL}};

int nh_table_mesh_o2_e1[4][16] = 
/*			0-3			4-7				8-11			12-15	*/
		{{0,ER,ER,OR|ER,	OR,EL|OR,EL|OR,EL|OL|OR,	OL|OR,OL|OR|ER,OL|ER,OL|ER,	OL,OL|EL,EL,EL},
		 {0,ER,ER,ER,		ER|ER,EL|ER,EL|ER|AC,EL|ER|AC,	ER|AC,ER|AC,AC,EL|AC,		EL|AC,EL,EL,EL},
		 {0,ER,ER,ER,		ER,ER|AC,ER|AC,ER|AC,		AC,AC|EL,AC|EL,AC|EL,		EL,EL,EL,EL},
		 {0,ER,ER,ER,		ER|AC,ER|AC,AC,EL|AC,		EL|AC,EL|ER|AC,EL|ER|AC,EL|ER,	ER|EL,EL,EL,EL}};
		    
int nh_table_tent_o2_e1[4][16] = 
/*			0-3			4-7				8-11			12-15	*/
		{{0,ER,ER,ER,		ER,ER|OT,OT,OT,			OT,OT,OT,OT|EL,		EL,EL,EL,EL},
		 {0,ER,ER,ER,		ER,ER|OT,OT,OT,			OT,OT,OT,OT|EL,		EL,EL,EL,EL},
		 {0,ER,ER,ER,		ER,ER|OT,OT,OT,			OT,OT,OT,OT|EL,		EL,EL,EL,EL},
		 {0,ER,ER,ER,		ER,ER|OT,OT,OT,			OT,OT,OT,OT|EL,		EL,EL,EL,EL}};
#endif
#define O1_E1 0
#define O2_E1 1
#define O3_E1 2

#ifndef CONFIG_BASED_ICS
int grid_nh_table_master_normal[16] = {ER_BM|EU_BM,	  ER_BM|EL_BM|EU_BM,	ER_BM|EL_BM|EU_BM,	EL_BM|EU_BM,
    				ER_BM|EU_BM|ED_BM, ER_BM|EL_BM|EU_BM|ED_BM, 	ER_BM|EL_BM|EU_BM|ED_BM, 	EL_BM|EU_BM|ED_BM,
    				ER_BM|EU_BM|ED_BM, ER_BM|EL_BM|EU_BM|ED_BM, 	ER_BM|EL_BM|EU_BM|ED_BM, 	EL_BM|EU_BM|ED_BM,
    				ER_BM|ED_BM,    ER_BM|EL_BM|ED_BM, 	ER_BM|EL_BM|ED_BM, 	EL_BM|ED_BM
};/* differently right */
int grid_nh_table_master[16] = {ER_BM|EU_BM,	  ER_BM|EL_BM|EU_BM,	ED_BM|ER_BM|EU_BM,	EL_BM|EU_BM|ED_BM|ER_BM,
    				ER_BM|EU_BM|ED_BM, 	ER_BM|EL_BM|EU_BM|ED_BM, 	ER_BM|ED_BM, 		EL_BM|ED_BM|ER_BM,
    				ER_BM|ED_BM|EL_BM, 	EL_BM|ED_BM, 		ER_BM|EL_BM|EU_BM|ED_BM, 	EL_BM|EU_BM|ED_BM,
    				ER_BM|EU_BM|ED_BM|EL_BM,	EL_BM|EU_BM|ED_BM, 	ER_BM|EL_BM|EU_BM, 	EL_BM|EU_BM,
};
int dual_rail_nh_table_master[16] = {ER_BM|OT_BM,	  ER_BM|EL_BM|AC_BM,	ER_BM|EL_BM,	EL_BM|AC_BM|ER_BM,
    				ER_BM|EL_BM, 	ER_BM|EL_BM|AC_BM, 	ER_BM|EL_BM, 		OT_BM|ER_BM,
    				ER_BM|ED_BM|EL_BM, 	EL_BM|ED_BM, 		ER_BM|EL_BM|EU_BM|ED_BM, 	EL_BM|EU_BM|ED_BM,
    				ER_BM|EU_BM|ED_BM|EL_BM,	EL_BM|EU_BM|ED_BM, 	ER_BM|EL_BM|EU_BM, 	EL_BM|EU_BM,
};/* FIXME Not finished */

int flower_nh_table_master[3][4][16] = 
		{ /*O1_E1*/ {},
		  /*O2_E1*/
/*			0-3							4-7	*/ /*					8-11							12-15	*/
			{{ER_BM|EL_BM|OR_BM|OL_BM,ER_BM,ER_BM,OR_BM|ER_BM,	OR_BM,OR_BM,OR_BM,OR_BM|OL_BM,				OR_BM|OL_BM,OR_BM|OL_BM,OL_BM,OL_BM,		OL_BM,OL_BM|EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM,ER_BM,ER_BM,EL_BM|ER_BM,		EL_BM|ER_BM,EL_BM|ER_BM,EL_BM|ER_BM,EL_BM|ER_BM,	EL_BM|ER_BM,EL_BM,EL_BM,EL_BM,				EL_BM,EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,				ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,				EL_BM,EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,				EL_BM|ER_BM,ER_BM|EL_BM,ER_BM|EL_BM,ER_BM|EL_BM,	ER_BM|EL_BM,ER_BM|EL_BM,EL_BM,EL_BM}},
		  /*O3_E1*/ {}
		};
		
int tent_nh_table_master[3][4][16] =
		{ /*O1_E1*/ {},
		  /*O2_E1*/
/*			0-3			4-7				8-11			12-15	*/
			{{ER_BM|EL_BM|OT_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM|OT_BM,OT_BM|EL_BM,OT_BM|EL_BM,			OT_BM,OT_BM|ER_BM,OT_BM|ER_BM,OT_BM|EL_BM,		EL_BM,EL_BM,EL_BM,EL_BM},
		 	 {ER_BM|EL_BM|OT_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM|OT_BM,OT_BM,OT_BM,			OT_BM,OT_BM,OT_BM,OT_BM|EL_BM,		EL_BM,EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM|OT_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM|OT_BM,OT_BM,OT_BM,			OT_BM,OT_BM,OT_BM,OT_BM|EL_BM,		EL_BM,EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM|OT_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM|OT_BM,OT_BM,OT_BM,			OT_BM,OT_BM,OT_BM,OT_BM|EL_BM,		EL_BM,EL_BM,EL_BM,EL_BM}},
		  /*O3_E1*/ {}
		};

int mesh_nh_table_master[3][4][16] =
		{ /*O1_E1*/ {},
		  /*O2_E1*/
/*				0-3			4-7				8-11			12-15	*/
			{{ER_BM|EL_BM|OR_BM|OL_BM,ER_BM,ER_BM,OR_BM|ER_BM,	OR_BM,EL_BM|OR_BM,EL_BM|OR_BM,EL_BM|OL_BM|OR_BM,	OL_BM|OR_BM,OL_BM|OR_BM|ER_BM,OL_BM|ER_BM,OL_BM|ER_BM,	OL_BM,OL_BM|EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM|AC_BM,ER_BM,ER_BM,ER_BM,		ER_BM|ER_BM,EL_BM|ER_BM,EL_BM|ER_BM|AC_BM,EL_BM|ER_BM|AC_BM,	ER_BM|AC_BM,ER_BM|AC_BM,AC_BM,EL_BM|AC_BM,		EL_BM|AC_BM,EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM|AC_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM|AC_BM,ER_BM|AC_BM,ER_BM|AC_BM,		AC_BM,AC_BM|EL_BM,AC_BM|EL_BM,AC_BM|EL_BM,		EL_BM,EL_BM,EL_BM,EL_BM},
			 {ER_BM|EL_BM|AC_BM,ER_BM,ER_BM,ER_BM,		ER_BM|AC_BM,ER_BM|AC_BM,AC_BM,EL_BM|AC_BM,		EL_BM|AC_BM,EL_BM|ER_BM|AC_BM,EL_BM|ER_BM|AC_BM,EL_BM|ER_BM,	ER_BM|EL_BM,EL_BM,EL_BM,EL_BM}},
		  /*O3_E1*/ {}
		};




int nh_table[16][16] = 
		{
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 /* repetition starts */
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM},
		 {ER_BM|EL_BM,ER_BM,ER_BM,ER_BM,		ER_BM,ER_BM,ER_BM,ER_BM,			ER_BM|EL_BM,EL_BM,EL_BM,EL_BM,			EL_BM,EL_BM,EL_BM,EL_BM}};
#else
int nh_table[CLUSTERS][CLUSTERS];
/* Initialize the next hop table */
void init_nh_table() {
    int i,l;
/* bitmap, not char map */
    if (link_definitions_argc > (sizeof(nh_table[0][0])*8))
	fatal("There are more link types than there are bits in the nh_table bitmap (%d > %d)!",link_definitions_argc,sizeof(nh_table[0][0])*8);

    for /* each core */ (i=0;i<CLUSTERS;i++)
    {
	nh_table[i][0]=0;
	for /* each link type */ (l=0;l<link_definitions_argc;l++)
	{
	    if (master_transverse_link_table[l][i] >= 0)
		nh_table[i][0] |= l_BM; /* OR the bitmap into the table */
	}
    }
}
#endif
			
#if 0
void optical_init(void) 
{
  int i,j,mm;

  optical_link_width_dist = (counter_t *)malloc(sizeof(counter_t)*optical_link_width);
  for (i=0;i<optical_link_width;i++)
    optical_link_width_dist[i] = 0;

  for (i=0;i<CLUSTERS;i++) {
      optical_link[i] = malloc(sizeof(int) * TIMEWINDOW);
      opt_reg[i] = malloc(sizeof(int*) * TIMEWINDOW);
    for (mm=0;mm<TIMEWINDOW;mm++) {
      optical_link[i][mm]=0;
      opt_reg[i][mm] = malloc(sizeof(int) * optical_link_width);
      for (j=0;j<optical_link_width;j++)
	      opt_reg[i][mm][j]=0;
    }
  }
}
#endif
int *master_link[MAX_IC_INDEX][CLUSTERS];
int **master_reg[MAX_IC_INDEX][CLUSTERS];
counter_t *master_link_width_dist[MAX_IC_INDEX];
#ifdef TRACK_COMM_USE_TYPES
enum comm_use_type_enum *master_link_use_type[MAX_IC_INDEX][CLUSTERS];
#endif //TRACK_COMM_USE_TYPES

//int master_qram_access_cl[MAX_IC_INDEX][CLUSTERS];
#ifdef NEW_INTERCONNECT
void interconnect_init(void)
{
    int i,j,k,mm;
    int l_width;
    for(k=0;k<MAX_IC_INDEX;k++)
    {
	l_width = master_link_width[k];
#ifdef INST_TRANS_RATE
  	master_link_width_dist[k] = malloc(sizeof(counter_t) * l_width);
	for(i=0;i<l_width;i++)
	    master_link_width_dist[k][i] = 0;
	    
#endif
#ifdef TRACE_REG
  	for(i=0;i<CLUSTERS;i++)
  	{
	    master_link[k][i] = malloc(sizeof(int) * TIMEWINDOW);
	    master_reg[k][i] = malloc(sizeof(int*) * TIMEWINDOW);
#ifdef TRACK_COMM_USE_TYPES
            master_link_use_type[k][i] = malloc(sizeof(enum comm_use_type_enum) * TIMEWINDOW);
#endif //TRACK_COMM_USE_TYPES
		
    	    for(mm=0;mm<TIMEWINDOW;mm++)
    	    {
      		master_link[k][i][mm] = 0;
		master_reg[k][i][mm] = malloc(sizeof(int) * l_width);
		for (j=0;j<l_width;j++)
		    master_reg[k][i][mm][j] = 0;
    	    }
  	}
#endif
    }

}
void ring_free(void)
{
    int i,j,link_num;
#ifdef CONFIG_BASED_ICS
    for (link_num = 0; link_num < link_definitions_argc; link_num++)
#else
    for (link_num = 0; link_num < MAX_IC_INDEX; link_num++)
#endif
    {
        for (i=0;i<CLUSTERS;i++) 
	{
	    if(master_link[link_num][i][((long)sim_cycle%TIMEWINDOW)]) {
		master_qram_access_cl[link_num][i]++;
		master_link_tx_access_cl[link_num][i]++;
		
		/* Note, this treats EL-ER, EU-ED, OR-OL pairs in the 
		 * sense that is the reverse of what you would expect.
		 * That is, if a cluster sends on OptModR, the next
		 * will receive it on OptRecR, not OptRecL. 
		 * FIXME: Access should be delayed by hop_time cycles. */
		master_link_rx_access_cl[link_num][tranverse_link(link_num,i)]++; 
										
#ifdef TRACK_COMM_USE_TYPES
		master_link_tx_use_type_access[link_num][ master_link_use_type[link_num][i][((long)sim_cycle%TIMEWINDOW)] ]++;
#endif //TRACK_COMM_USE_TYPES
	    }
	    master_link[link_num][i][((long)(sim_cycle%TIMEWINDOW))]=0;

	    for(j=0;j<master_link_width[link_num];j++)
		master_reg[link_num][i][((long)(sim_cycle%TIMEWINDOW))][j]=0;
	}
    }
	
}
void arbitrary_routing_table_init(void)
{
 int i,from, num_arrived,l;
 int to, route, delay;
 counter_t now;
 counter_t st_a[CLUSTERS]; /* (store) arrival time */
 int st_how[CLUSTERS];
#if 0
#ifdef CONFIG_BASED_ICS
 if (!link_definitions_argc)
	fatal("arbitrary_routing_table_init: You did not specify any communication links at all, but I need at least one.");

 init_nh_table();
#endif
#endif

#ifdef  IGNORE_MEM_COMM_LATENCY
  return;
#endif
  if(uni_lat_comm)
    return;
  if(two_lat_comm)
    return;
  /*
  for(i=0;i<CLUSTERS;i++)
  {
      st_a[i] = 0;
      st_how[i] = 0;
  }
  myreg=0;
  */
  for(from=0;from<CLUSTERS;from++)
  {
      for(i=0;i<CLUSTERS;i++)
      {
	st_a[i] = 0;
	st_how[i] = 0;
      }
      num_arrived = 0;
      now = 1;
      st_a[from] = now;
      route = nh_table[from][0];
#ifdef THRD_WAY_CACHE
/*       n_cache_start_thrd[rs->threadid] + n_cache_limit_thrd[rs->threadid] ; */
#endif
  /* Have a global (sorta) variable that records when a node gets touched.
   * pass it as each node is called recursively, maybe.
   * */

#ifdef CONFIG_BASED_ICS

	    for(l=0;l<link_definitions_argc;l++)
		/* FLOOD_INIT */
		{
	    	    if (route&l_BM)
	    	    {
			to = tranverse_link(l,from);
			delay = master_hop_time[l];
			if(delay+now == st_a[to])
		   	    st_how[to] |= l_BM; 
			else if(((delay+now) < st_a[to]) || (st_a[to] == 0))
			{
		    	    st_a[to] = delay + now; \
		    	    st_how[to] = l_BM; \
			} /* else we don't care, since it has already seen this.
			   * we do still consume link bandwidth because it hasn't gotten here yet. */
	    	    } 
		}

#else //CONFIG_BASED_ICS

#define FLOOD_INIT(LINK_POS) \
	    do { \
	    if (route&LINK_POS##_BM) \
	    { \
		to = tranverse_link(LINK_POS##_INDEX,from); \
		delay = master_hop_time[LINK_POS##_INDEX]; \
		if(delay+now == st_a[to]) \
		    st_how[to] |= LINK_POS##_BM; \
		else if(((delay+now) < st_a[to]) || (st_a[to] == 0)) \
		{ \
		    st_a[to] = delay + now; \
		    st_how[to] = LINK_POS##_BM; \
		} /* else we don't care, since it has already seen this. \
		   * we do still consume link bandwidth because it hasn't gotten here yet. */ \
	    } }while(0)

	    FLOOD_INIT(OL);
	    FLOOD_INIT(OR);
	    FLOOD_INIT(EL);
	    FLOOD_INIT(ER);
	    FLOOD_INIT(AC);
	    FLOOD_INIT(OT);
	    FLOOD_INIT(EU);
	    FLOOD_INIT(ED);
#endif //CONFIG_BASED_ICS

    while(num_arrived < CLUSTERS)
    {
      for(i=0;i<CLUSTERS;i++)
      {
	  if(st_a[i] == now)
	  {
	    num_arrived++;
	    //warn("Debugging mode active");

#ifdef CONFIG_BASED_ICS
	    total_noncongested_comm_delay[from][i]=now-1;
#endif

	    if(from==i) 
		continue;
	    
	    /*
	      if(nh_table[from][i] != st_how[i])
		fatal("%x is static but %x is dynamic for node %d to %d (now %d).",nh_table[from][i],st_how[i],from,i,now);
		*/
	    nh_table[from][CL_SUB(i,from)] = st_how[i];
//	    fprintf(stderr,"reached %i at %lld.\n",i,now);
/*	    from_s = NODE_TYPE(i); */ /* there are 4 (for 16 clusters)  sub routing tables based on what position a node is in */ /* nevermind */
	    route = nh_table[i][0];
#ifdef CONFIG_BASED_ICS
	    for(l=0;l<link_definitions_argc;l++)
	 	/* FLOOD_REC(l) */
	        {
	    	    if (route&l_BM) \
	    	    {
			to = tranverse_link(l,i);
			delay = master_hop_time[l];
			if(delay+now == st_a[to])
			    st_how[to] |= st_how[i];
			else if(((delay+now) < st_a[to]) || (st_a[to] == 0))
			{
			    st_a[to] = delay + now;
			    st_how[to] = st_how[i];
			} /* else we don't care, since it has already seen this. 
			   * we do still consume link bandwidth because it hasn't gotten here yet. */
	    	    } 
		}
#else
#define FLOOD_REC(LINK_POS) \
	    do { \
	    if (route&LINK_POS##_BM) \
	    { \
		to = tranverse_link(LINK_POS##_INDEX,i); \
		delay = master_hop_time[LINK_POS##_INDEX]; \
		if(delay+now == st_a[to]) \
		    st_how[to] |= st_how[i]; \
		else if(((delay+now) < st_a[to]) || (st_a[to] == 0)) \
		{ \
		    st_a[to] = delay + now; \
		    st_how[to] = st_how[i]; \
		} /* else we don't care, since it has already seen this. \
		   * we do still consume link bandwidth because it hasn't gotten here yet. */ \
	    } }while(0)

	    FLOOD_REC(OL);
	    FLOOD_REC(OR);
	    FLOOD_REC(EL);
	    FLOOD_REC(ER);
	    FLOOD_REC(AC);
	    FLOOD_REC(OT);
	    FLOOD_REC(EU);
	    FLOOD_REC(ED);
#endif
	  }
      }
      now++;
    } // while(num_arrived < CLUSTERS)
  } // for(from=0;from<CLUSTERS;from++)
}

#ifndef CONFIG_BASED_ICS
void
init_rail_routing_table(void)
{
    int route,from,to, fromr, fromc, tor, toc,to_s;
    for(from = 0; from<CLUSTERS; from++)
    {
	fromr = from/(CLUSTERS/2);
    	fromc = from%(CLUSTERS/2);

	for(to=0; to<CLUSTERS; to++)
	{
	    to_s = CL_SUB(to,from);

	    route = 0;

    	    tor = to/(CLUSTERS/2);
    	    toc = to%(CLUSTERS/2);
	    
    	    if(fromr > tor)
		route|=OT_BM;
	    else if(fromr < tor)
		route|=OT_BM;

	    if(fromc > toc)
		route|=EL_BM;
	    else if(fromc < toc)
		route|=ER_BM;

	    nh_table[from][to_s] = route;

	}
	nh_table[from][0] = OT_BM;

	if(fromc != 0)
	    nh_table[from][0] |= EL_BM;
	if(fromc != ((CLUSTERS/2)-1))
	    nh_table[from][0] |= ER_BM;
    }
}

void
init_grid_routing_table(void)
{
    int route,from,to, fromr, fromc, tor, toc,to_s;
    for(from = 0; from<CLUSTERS; from++)
    {
	fromr = from/GRIDCOLS;
    	fromc = from%GRIDROWS;

	for(to=0; to<CLUSTERS; to++)
	{
	    to_s = CL_SUB(to,from);

	    route = 0;

    	    tor = to/GRIDCOLS;
    	    toc = to%GRIDROWS;
	    
    	    if(fromr > tor)
		route|=ED_BM;
	    else if(fromr < tor)
		route|=EU_BM;

	    if(fromc > toc)
		route|=EL_BM;
	    else if(fromc < toc)
		route|=ER_BM;

	    nh_table[from][to_s] = route;

	}
	if(fromr != 0)
	    nh_table[from][0] |= ED_BM;
	if(fromr != (GRIDROWS-1))
	    nh_table[from][0] |= EU_BM;

	if(fromc != 0)
	    nh_table[from][0] |= EL_BM;
	if(fromc != (GRIDCOLS-1))
	    nh_table[from][0] |= ER_BM;
    }
}
/* REG_LINK: #type   ->  "type" when type is dereferenced */
#define REG_LINK(type) \
    do { \
      if(!mystricmp(optical_type,#type)) \
      { \
	    for(i=0;i<4;i++) \
		for(j=0;j<16;j++) \
		    nh_table[i+4][j]=nh_table[i+8][j]=nh_table[i+12][j]=nh_table[i][j] = SYMCAT(type,_nh_table_master[eo_ratio][i][j]); \
    /*arbitrary_routing_table_init();*/ \
      return; \
      } \
    } while(0)
#define NEW_REG_LINK(type) do {\
    if(!mystricmp(optical_type,#type)) {\
      for(i=0;i<CLUSTERS;i++) \
        nh_table[i][0] = type##_nh_table_master[i]; \
      return; } }while(0)
#endif

void init_routing_table(void)
{
#ifdef CONFIG_BASED_ICS
/*    int l;
    for(l=0;l<link_definitions_argc;l++)
	 reverse_link[l] = l_BM; WRONG:should be the *opposite* direction */
#else
    int eo_ratio;
    double ratio;
    int i,j;
    reverse_link[OR_INDEX] = OL_BM;
    reverse_link[OL_INDEX] = OR_BM;
    reverse_link[ER_INDEX] = EL_BM;
    reverse_link[EL_INDEX] = ER_BM;
    reverse_link[AC_INDEX] = AC_BM;
    reverse_link[OT_INDEX] = OT_BM;
    reverse_link[EU_INDEX] = ED_BM;
    reverse_link[ED_INDEX] = EU_BM;

    if(!mystricmp(optical_type,"none"))
	return;

    /*
    if(!mystricmp(optical_type,"grid"))
    {
	init_grid_routing_table();
	return;
    }
    if(!mystricmp(optical_type,"dual_rail"))
    {
	init_rail_routing_table();
	return;
    }
    */

    NEW_REG_LINK(grid);
    //NEW_REG_LINK(dual_rail);
    ratio = (double)optical_hop_time/hop_time;
    if(ratio==2.0)
	eo_ratio = O2_E1;
    else if(ratio == 1.0)
	eo_ratio = O1_E1;
    else if(ratio == 3.0)
	eo_ratio = O3_E1;
    else
	fatal("Ratio %f %d %d not valid",ratio,optical_hop_time,hop_time);
#endif //CONFIG_BASED_ICS

#ifndef CONFIG_BASED_ICS
    REG_LINK(tent);
    REG_LINK(mesh);
    REG_LINK(flower);
    fatal("optical_type %s undefined");
#else
    arbitrary_routing_table_init();
#endif
/*
    for (i=0;i<4;i++)
    {
	for(j=0;j<16;j++)
	    fprintf(stderr,"%x ",nh_table[i][j]);
	fprintf(stderr,"\n");
    }
    for (i=0;i<4;i++)
    {
	for(j=0;j<16;j++)
	    fprintf(stderr,"%x ",mesh_nh_table_master[eo_ratio][i][j]);
	fprintf(stderr,"\n%d\n",eo_ratio);
    }
    fatal("all done2");
*/
}


		    
/*
int routing_table[16][16];
*/
	/* next hop from i to j */
/*
void init_routing_table_flower(void)
{
	int i,j;
	for(i = 0; i < 16 ; i++)
	{
		routing_table[i][i] = 0;

		for(j=1;j<4;j++)
			routing_table[i][(i + j)&0x1111] = ER;
		for(j=13;j<16;j++)
			routing_table[i][(i + j)&0x1111] = EL;
		if(CORNER(i))
		{
			for(j=4;j<8;j++)
				routing_table[i][(i + j)&0x1111] = OR;
			for(j=9;j<13;j++)
				routing_table[i][(i + j)&0x1111] = OL;
			routing_table[i][(i+8)&0x1111] = (EL|ER);
		}
		else
		{
			for(j=4;j<8;j++)
				routing_table[i][(i + j)&0x1111] = ER;
			for(j=9;j<13;j++)
				routing_table[i][(i + j)&0x1111] = EL;
			routing_table[i][(i+8)&0x1111] = (EL|ER);
		}
		
	}
}
*/

/*
 * cl_comm_lat will return the number of cycles it takes to 
 * send bytesize bytes of non-register data from one
 * cluster to another, accounting for contention.
 */
#define DIVIDE_POSITIVE_BY_8_ROUNDING_UP(x) ((x+7)>>3)
int cl_comm_lat(int from, int to, int offset, unsigned int bytesize, enum comm_use_type_enum use_type) {
    unsigned int number_packets = DIVIDE_POSITIVE_BY_8_ROUNDING_UP(bytesize); /* we have 8-databyte packets */
    /* Send as many packets as needed, return max latency value. */
    int maxlat = 0;
    while (number_packets) {
#ifdef   TRACK_COMM_USE_TYPES
        int lat = ring_lat_typed(from,to,0,offset,use_type); 
#else  //TRACK_COMM_USE_TYPES
	int lat = ring_lat(from,to,0,offset);
#endif //TRACK_COMM_USE_TYPES
	if (lat > maxlat)
	    maxlat = lat;
        number_packets--;
    }
    return maxlat;
}

/*
 * ring_lat is no longer ring-specific.
 * It returns the number of cycles of delay it takes a
 * message to get from one cluster to another, accounting
 * for congestion somehow.
 *
 * offset: The number of cycles into the future at which
 *         time this message is to be sent.
 */
int ring_lat(int from, int to, int myreg, int offset)
#ifdef TRACK_COMM_USE_TYPES
{
#ifdef  IGNORE_ALL_COMM_LATENCY
  return 0;
#endif
    if (myreg) {
	return ring_lat_typed(from, to, myreg, offset, sendregister);
    } else {
	return ring_lat_typed(from, to, myreg, offset, other);
    }
}
int ring_lat_typed(int from, int to, int myreg, int offset, enum comm_use_type_enum use_type)
#endif //TRACK_COMM_USE_TYPES
{
    int route, found, delay, tempv;
    int possible_links[MAX_IC_INDEX];
    int num_possible_links;
    int to_s; /* scaled version of from and to, for routing */
    int winning_link;
    int k,l;
    int first_time = 1;
    counter_t now;

    tempv = abs(from - to);
    if (tempv > (CLUSTERS/2)) 
	tempv = CLUSTERS-tempv;
    total_hop += tempv;
    stat_add_sample(hop_dist, tempv); 
    now = sim_cycle+offset;
      /* account for myreg */
    while (from != to && (now - sim_cycle) < TIMEWINDOW )
    {
	/* we know we have a max of 5 possibilities (really 3)
	 * so first we clear the number of places that pass
	 * then, we examine all the possibilities for this
	 * time. If more than one exists, we then randomly
	 * choose one
	 * FIXME choose the one that is the cheapest, or
	 * has the least congestion (historically?)
	 * */
	num_possible_links = 0;
	found = FALSE; /* THIS HAD DARN WELL BETTER NOT CHANGE ANYTHING */
/*	from_s = NODE_TYPE(from); *//* there are 4 (for 16 clusters)  sub routing tables based on what position a node is in */ /* nevermind */
	to_s = CL_SUB(to,from);  /* all routing is done based on how many hops to the right the to is from the from */
#define	CHECK_VALID(LINK_POS) \
	if(route&LINK_POS##_BM) \
	    if( master_link[LINK_POS##_INDEX][from][(now%TIMEWINDOW)]<master_link_width[LINK_POS##_INDEX]) \
	        possible_links[num_possible_links++] = LINK_POS##_INDEX 

#define	CHECK_MYREG_VALID(LINK_POS) \
	if(route&LINK_POS##_BM) \
	{ \
	    for (k = 0; k < master_link[LINK_POS##_INDEX][from][(now%TIMEWINDOW)]; k++) \
		if(master_reg[LINK_POS##_INDEX][from][(now%TIMEWINDOW)][k] == myreg) \
		{ \
		    found = TRUE; \
		    possible_links[num_possible_links++] = LINK_POS##_INDEX; \
		    break; \
		} \
	    if( master_link[LINK_POS##_INDEX][from][(now%TIMEWINDOW)]<master_link_width[LINK_POS##_INDEX]) \
	        possible_links[num_possible_links++] = LINK_POS##_INDEX; \
	}
	route = nh_table[from][to_s];
/*	{
	dump_route_table();
	fatal("test");
	}
	*/
	if(!myreg)
	{
#ifdef CONFIG_BASED_ICS
	    for(l=0;l<link_definitions_argc;l++)
		CHECK_VALID(l);
#else

	    CHECK_VALID(OR);
	    CHECK_VALID(OL);
	    CHECK_VALID(ER);
	    CHECK_VALID(EL);
	    CHECK_VALID(AC);
	    CHECK_VALID(OT);
	    CHECK_VALID(EU);
	    CHECK_VALID(ED);
#endif
	}
	else
	{
#ifdef CONFIG_BASED_ICS
	    for(l=0;l<link_definitions_argc;l++)
		CHECK_MYREG_VALID(l)
#else

	    CHECK_MYREG_VALID(OR)
	    CHECK_MYREG_VALID(OL)
	    CHECK_MYREG_VALID(ER)
	    CHECK_MYREG_VALID(EL)
	    CHECK_MYREG_VALID(AC)
	    CHECK_MYREG_VALID(OT)
	    CHECK_MYREG_VALID(EU)
	    CHECK_MYREG_VALID(ED)
#endif
	}
	if(num_possible_links < 1)
	    now++;
	else 
	{
	    if (num_possible_links == 1)
		winning_link = possible_links[0];
	    else
		winning_link = possible_links[myrand()%num_possible_links];

	    if(first_time) /* needs to be in the for loop so we know which link to charge for a qram access to queue it */
	    {
           	master_qram_access_cl[winning_link][from]++;   
		first_time = 0;
	    }
		/* only increment for first one. the rest is in free_ring */
	    if(!found) 
	    {
	      							   /* the under section is the number of slots used */
	    	master_reg[winning_link][from][(now%TIMEWINDOW)][  master_link[winning_link][from][(now%TIMEWINDOW)]  ] = myreg;
		master_link_width_dist[winning_link][ master_link[winning_link][from][(now%TIMEWINDOW)]++ ]++;
#ifdef TRACK_COMM_USE_TYPES
		master_link_use_type[winning_link][from][(now%TIMEWINDOW)]=use_type;
#endif //TRACK_COMM_USE_TYPES
	    }
	    now += master_hop_time[winning_link];
	    from = tranverse_link(winning_link,from);
	    
	}
	    
    } /* end while from != to */

    if(TIMEWINDOW < (now - sim_cycle) )
	panic("link exceeded its limit in cyc %lld\n", sim_cycle);
	 
    delay = now - sim_cycle - offset;
    stat_add_sample(delay_dist, delay); 
    total_delay += delay;
    total_number_delay++;
    return delay;
}

void flood_lat(counter_t * results, int from, counter_t sendtime)
{
  int num_arrived,i,l,to,route;
  int myreg=0;
  counter_t st_a[CLUSTERS];
  int st_how[CLUSTERS];
  counter_t now, delay;

  for(i=0;i<CLUSTERS;i++)
  {
      st_a[i] = -1;
      st_how[i] = 0;
  }

  num_arrived = 0;
  st_a[from] = sendtime; /* communication to ourself happens immediately, with no delay */
  now = sendtime;

  /* Have a global (sorta) variable that records when a node gets touched.
   * pass it as each node is called recursively, maybe.
   * */
  while(num_arrived < CLUSTERS)
  {
      for(i=0;i<CLUSTERS;i++)
      {
	  if(st_a[i] == now)
	  {
	    num_arrived++;
//	    fprintf(stderr,"reached %i at %lld.\n",i,now);
	    results[i] = now;
//#warning nick, can you fix this to not use reverse_link? --gjb
	    route = nh_table[i][0];
#define CHECK_ST_VALID(LINK_POS) \
	    do { \
	    if((route&LINK_POS##_BM) && !(LINK_POS##_BM&st_how[i]))\
	    { \
		to = tranverse_link(LINK_POS##_INDEX,i); \
		delay = ring_lat_typed(i,to,myreg,now-sim_cycle,flood); \
		if(delay+now == st_a[to]) \
	;/*	    st_how[to] |= reverse_link[LINK_POS##_INDEX]; */\
		else if(((delay+now) < st_a[to]) || (st_a[to] == -1)) \
		{ \
		    st_a[to] = delay + now; \
/*    st_how[to] = reverse_link[LINK_POS##_INDEX]; */\
		} /* else we don't care, since it has already seen this. \
		   * we do still consume link bandwidth because it hasn't gotten here yet. */ \
	    } }while(0)
#ifdef CONFIG_BASED_ICS
	    for(l=0;l<link_definitions_argc;l++)
		CHECK_ST_VALID(l);
#else
	    CHECK_ST_VALID(OL);
	    CHECK_ST_VALID(OR);
	    CHECK_ST_VALID(EL);
	    CHECK_ST_VALID(ER);
	    CHECK_ST_VALID(AC);
	    CHECK_ST_VALID(OT);
	    CHECK_ST_VALID(EU);
	    CHECK_ST_VALID(ED);
#endif
	  }
      }
      now++;
  }

}

/* This is no longer ring-specific */
void st_ring_lat(struct RUU_station *rs)
/* "st lat" is the STORE latency.
 * rs is the sending cluster.
 * This function returns its result in the rs->st_reach array */
{
 int myreg, i, delay;
#if 0
 int num_arrived,l;
 int to, route;
 counter_t now; /* the time being simulated- this is incremented in the while loop below */
 counter_t st_a[CLUSTERS]; /* the store's arrival time in cycles */
 int st_how[CLUSTERS];
#endif

#ifdef  IGNORE_MEM_COMM_LATENCY
  return;
#endif
  if(uni_lat_comm) /* "All communications take 1 cycle if from 2 different nodes" */
  {
    for(i=0;i<CLUSTERS;i++)
      rs->st_reach[i] = sim_cycle + 1;
    rs->st_reach[rs->cluster] = sim_cycle;
    return;
  }
  if(two_lat_comm) /* "All communications take 2 cycle if from 2 different nodes (1 cycle for adjacent)" */
  {
    for(i=0;i<CLUSTERS;i++)
	rs->st_reach[i] = sim_cycle + 2;
    rs->st_reach[rs->cluster] = sim_cycle;
    rs->st_reach[CL_ADD(rs->cluster,1)] = sim_cycle + 1;
    rs->st_reach[CL_SUB(rs->cluster,1)] = sim_cycle + 1;
    return;
  }
#if 0

  for(i=0;i<CLUSTERS;i++)
  {
      st_a[i] = 0;
      st_how[i] = 0;
  }

  myreg=0;
  num_arrived = 0;
  st_a[rs->cluster] = sim_cycle; /* communication to ourself happens immediately, with no delay */
  now = sim_cycle;
#ifdef THRD_WAY_CACHE
/*       n_cache_start_thrd[rs->threadid] + n_cache_limit_thrd[rs->threadid] ; */
#endif
  /* Have a global (sorta) variable that records when a node gets touched.
   * pass it as each node is called recursively, maybe.
   * */
  while(num_arrived < CLUSTERS)
  {
      for(i=0;i<CLUSTERS;i++)
      {
	  if(st_a[i] == now)
	  {
	    num_arrived++;
//	    fprintf(stderr,"reached %i at %lld.\n",i,now);
	    rs->st_reach[i] = now;
/*	    from_s = NODE_TYPE(i); */ /* there are 4 (for 16 clusters)  sub routing tables based on what position a node is in */ /* nevermind */
	    route = nh_table[i][0];
#define CHECK_ST_VALID(LINK_POS) \
	    do { \
	    if((route&LINK_POS##_BM) && !(LINK_POS##_BM&st_how[i]))\
	    { \
		to = tranverse_link(LINK_POS##_INDEX,i); \
		delay = ring_lat(i,to,myreg,now-sim_cycle); \
		if(delay+now == st_a[to]) \
		    st_how[to] |= reverse_link[LINK_POS##_INDEX]; \
		else if(((delay+now) < st_a[to]) || (st_a[to] == 0)) \
		{ \
		    st_a[to] = delay + now; \
		    st_how[to] = reverse_link[LINK_POS##_INDEX]; \
		} /* else we don't care, since it has already seen this. \
		   * we do still consume link bandwidth because it hasn't gotten here yet. */ \
	    } }while(0)
#ifdef CONFIG_BASED_ICS
	    for(l=0;l<link_definitions_argc;l++)
		CHECK_ST_VALID(l);
#else
	    CHECK_ST_VALID(OL);
	    CHECK_ST_VALID(OR);
	    CHECK_ST_VALID(EL);
	    CHECK_ST_VALID(ER);
	    CHECK_ST_VALID(AC);
	    CHECK_ST_VALID(OT);
	    CHECK_ST_VALID(EU);
	    CHECK_ST_VALID(ED);
#endif
	  }
      }
      now++;
  }
#else //from #if 0

/* We use (noncongestive) message passing to send to each 
 * cluster explicitly, instead of doing the above flooding. */

    myreg=0; /* FIXME: Find out the register? */
#ifdef THRD_WAY_CACHE
    for (i=n_cache_start_thrd[rs->threadid];
	 i<(n_cache_start_thrd[rs->threadid] + n_cache_limit_thrd[rs->threadid]);
	 i++)
#else
    for (i=0;i<CLUSTERS;i++)
#endif
    {
	delay = ring_lat(rs->cluster,i,myreg,0);
        rs->st_reach[i] = delay + sim_cycle;
    }
    //(redundant) rs->st_reach[rs->cluster] = sim_cycle; /* communication to ourself happens immediately, with no delay */
#endif //from #if 0
}
#endif
#ifdef TOKENB
counter_t predict_commit(struct RUU_station *rs)
{
//    int extra_lat = 0;
    int i;
    tick_t latest = 0;
    struct RUU_station *wrs,*our_ROB,*wlsq;
//    struct RUU_station *our_LSQ;
    int our_head, our_num;
 //   int our_lhead, our_lnum;

    our_ROB  = thecontexts[rs->threadid]->RUU;
    our_head = thecontexts[rs->threadid]->RUU_head;
    our_num  = thecontexts[rs->threadid]->RUU_num;
/*
    our_LSQ   = thecontexts[rs->threadid]->LSQ;
    our_lhead = thecontexts[rs->threadid]->LSQ_head;
    our_lnum  = thecontexts[rs->threadid]->LSQ_num;
    */
    if(rs->in_LSQ)
    {
	//for(wrs = &LSQ[]; wrs
	wlsq = &our_ROB[rs->index];
    }
    else
    { 
	wlsq = rs;
    }
    
	for( i = 0; i < our_num ; i++ )
	{
	    wrs = &our_ROB[(our_head + i) % RUU_size ];
	    latest = MAX(latest, wrs->finish_time);
	    latest = MAX(latest, wrs->commit_time);
	    if(i==our_num)
		fatal("This shouldn't have gotten this high.");
	    if(wrs == wlsq)
		break;
	}
	/*
	for( i = 0; i < our_lnum ; i++ )
	{
	    wlsq = &our_LSQ[(our_lhead + i) % LSQ_size ];
	    latest = MAX(latest, wlsq->finish_time);
	}
	*/
//    return rs->finish_time /*+ MAX(1 , extra_lat)*/;
    if(latest < rs->finish_time)
//	fatal("hah");
      latest = rs->finish_time;
    return latest;
}
#endif

void dump_route_table(void)
{
	int i,j;
	for(i=0;i<CLUSTERS;i++)
	{
		for(j=0;j<CLUSTERS;j++)
		{
			fprintf(stderr,"%x ", nh_table[i][j]);
		}
		fprintf(stderr,"\n");
	}
}

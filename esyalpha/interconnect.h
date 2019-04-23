/*
* $Log: interconnect.h,v $
* Revision 1.1.1.1  2011/03/29 01:04:34  huq
* setup project CMP
*
* Revision 1.1.1.1  2010/02/18 21:22:29  xue
* CMP_NETWORK
*
* Revision 1.1.1.1  2008/10/20 15:44:51  garg
* dir structure
*
* Revision 1.3  2004/10/28 19:31:56  etan
* add type none to link_physics_enum
*
* Revision 1.2  2004/09/23 21:07:18  etan
* if communication latency is ignored, set comm_fe_lat to 0
*
* Revision 1.1  2004/09/22 21:20:25  etan
* Initial revision
*
* Revision 1.30  2004/08/03 01:35:53  ninelson
* Allows cluster 0 under OPTFE to talk to FE at 1 cycle or less
*
* Revision 1.29  2004/08/02 17:21:35  ninelson
* added support for tokenB
*
* Revision 1.28  2004/07/28 04:32:52  grbriggs
* Add flood_lat function.
*
* Revision 1.27  2004/07/26 22:09:07  grbriggs
* Make fe_link_time a run-time option instead of a #define.
*
* Revision 1.26  2004/07/13 15:17:30  grbriggs
* Fix problem in previous checkin for tracking comm use types
*
* Revision 1.25  2004/07/12 17:16:54  grbriggs
* Add per-usage-type statistics for comm links.
*
* Revision 1.24  2004/07/08 17:25:29  grbriggs
* Added cl_comm_lat for multi-packet communication between clusters.
* Also, added a L2_COMM_LAT support macro.
* Also, make the file only have effects the first time it is #included.
*
* Revision 1.23  2004/07/07 14:40:59  grbriggs
* Define L2_COMM_LAT
*
* Revision 1.22  2004/06/30 15:30:53  ninelson
* Update to allow nearest neighbor to FE to recieve via electrical
*
* Revision 1.21  2004/06/24 21:10:03  grbriggs
* Add new macros COMM_CL_TO_FE_LAT and COMM_FE_TO_CL_LAT.
*
* Revision 1.20  2004/06/15 21:08:40  grbriggs
* Add lat_noncongested
*
* Revision 1.19  2004/06/09 21:39:31  grbriggs
* Take advantage of CL_MOD in CL_ADD and CL_SUB.
*
* Revision 1.18  2004/06/09 17:18:05  grbriggs
* Complete changes in support of custom links specified via config.
* Added new link_info structure.
*
* Revision 1.17  2004/06/07 22:31:32  grbriggs
* Preliminary support for configuration-based link definitions (CONFIG_BASED_ICS).
*
* Revision 1.16  2004/06/04 17:56:16  ninelson
* grid updates
*
* Revision 1.15  2004/05/31 11:13:43  ninelson
* Arbitrary_routing_table_init prototype
*
* Revision 1.14  2004/05/28 14:59:17  ninelson
* fixed () bug in macros
*
* Revision 1.13  2004/05/27 22:42:04  grbriggs
* Split link variables into link_rx and link_tx.
*
* Revision 1.12  2004/05/27 21:00:56  grbriggs
* Added descriptive comments to the ??_INDEX constants.
*
* Revision 1.10  2004/05/25 16:52:36  ninelson
* *** empty log message ***
*
* Revision 1.9  2004/05/25 16:52:10  ninelson
* *** empty log message ***
*
 * Revision 1.8  2004/05/25 16:51:39  ninelson
 * *** empty log message ***
 *
 *Revision 1.7  2004/05/25 16:51:02  ninelson
 **** empty log message ***
 *
 *Revision 1.6  2004/05/25 16:50:27  ninelson
 *garbage
 *
 *Revision 1.5  2004/05/25 16:49:14  ninelson
 *added stuff for grid interconnects
 *
 */

/*	$Locker:  $ */


#ifndef __INTERCONNECT_H
#define __INTERCONNECT_H



/* TRACK_COMM_USE_TYPES causes statistics on what types of messages are
 * being passed. Only works if CONFIG_BASED_ICS is defined. */
#define TRACK_COMM_USE_TYPES

#ifdef TRACK_COMM_USE_TYPES
enum comm_use_type_enum {sendregister, l2comm, flood, bare_token, data_token, other};
extern char * comm_use_type_enum_names[]; //update in interconnect.c // = {"sendregister", "l2comm", "other"};
#define LENGTHOF_comm_use_type_enum 6
#endif

/* L2_COMM_LAT causes L2 to be nonuniform access and arranged such that
 * every piece of L2 is attached to the nearest cluster's communications
 * links. See sim-outorder.c. */
//#define L2_COMM_LAT

#if defined(L2_COMM_LAT) || defined(TOKENB)
  #define IF_L2_COMM_LAT_CL(cluster) ,cluster
#else
  #define IF_L2_COMM_LAT_CL(cluster)
#endif

/* CONFIG_BASED_ICS means that comm links are individually defined in the 
 * config files. */
#define CONFIG_BASED_ICS

#define NEW_INTERCONNECT

#define CLUSTER_TOTAL CLUSTERS

#define GRIDROWS 4
#define GRIDCOLS (CLUSTERS/GRIDROWS)

#define str(s) #s
#define xstr(s) str(s)

#define CNORM(x) (x+CLUSTERS)%CLUSTERS

#ifdef CL_MOD
#define CL_SUB(a,b) CL_MOD((a)-(b)+CLUSTERS)
#define CL_ADD(a,b) CL_MOD((a)+(b))
#else
#define CL_SUB(a,b) (((a)-(b)+CLUSTERS)%CLUSTERS)
#define CL_ADD(a,b) (((a)+(b))%CLUSTERS)
#endif

#define NODE_TYPE(x) (x&0x3)

#ifndef CONFIG_BASED_ICS

#define ER_BM 0x01
#define EL_BM 0x02
#define EU_BM 0x04
#define ED_BM 0x08
#define OR_BM 0x10
#define OL_BM 0x20
#define AC_BM 0x40
#define OT_BM 0x80

/* 0 is right, 1 is left 2 up 3 is down */
#define ER_INDEX 0  /* electrical right */
#define EL_INDEX 1  /* electrical left  */
#define EU_INDEX 2  /* electrical up    */
#define ED_INDEX 3  /* electrical down  */
#define OR_INDEX 4  /* optical right    */
#define OL_INDEX 5  /* optical left     */
#define AC_INDEX 6  /* optical across   */
#define OT_INDEX 7  /* optical tent     */

#else /*CONFIG_BASED_ICS*/
#define l_INDEX l
#define l_BM (1 << l)
#endif /*CONFIG_BASED_ICS*/

#define MAX_IC_INDEX 8


/** model **/
extern int master_link_width[MAX_IC_INDEX];
extern int master_hop_time[MAX_IC_INDEX];
extern char *IC_index2string[MAX_IC_INDEX];
enum link_physics_enum {none, electrical, optical};
typedef struct link_info_t_st
{
    char * type; /* ER, EL, ER_grid, etc. */
    char * longname;
    enum link_physics_enum physics;
} link_info_t;
extern link_info_t link_info[MAX_IC_INDEX];

#ifdef CONFIG_BASED_ICS
extern int fe_link_time;
extern int total_noncongested_comm_delay[CLUSTERS][CLUSTERS];

/* 092304 nan: if communication latency is ignored, then 
   GENERIC_COMM_FE_LAT_DELAYED is 0 */
 #ifdef IGNORE_DISPATCH_COMM_LATENCY
 #define GENERIC_COMM_FE_LAT_DELAYED(from,to,delay)		0
 #endif
/* 092304 nan: if communication latency is uniform, then 
   GENERIC_COMM_FE_LAT_DELAYED is 1 */
 #ifdef UNIFORM_COMM_LATENCY
 #define GENERIC_COMM_FE_LAT_DELAYED(from,to,delay)		1
 #endif

/* 092404 ejt: if latency is not ignored, then compute as follows */
 #ifndef IGNORE_DISPATCH_COMM_LATENCY
 #ifndef UNIFORM_COMM_LATENCY
 #define GENERIC_COMM_FE_LAT_DELAYED(from,to,delay)		(fe_link_time < 0 ? lat_noncongested(from,to) + HOPTIME : ((lat_noncongested(from,to) + HOPTIME)<fe_link_time)? lat_noncongested(from,to) + HOPTIME : fe_link_time ) 
 #endif
 #endif

 #define COMM_FE_TO_CL_LAT_DELAYED(to,delay)		GENERIC_COMM_FE_LAT_DELAYED(0,to,delay)
 #define COMM_CL_TO_FE_LAT_DELAYED(from,delay)		GENERIC_COMM_FE_LAT_DELAYED(from,0,delay)
 #define COMM_CL_TO_FE_LAT(from) COMM_CL_TO_FE_LAT_DELAYED(from,0) 
 #define COMM_FE_TO_CL_LAT(to) COMM_FE_TO_CL_LAT_DELAYED(to,0) 
 //#define COMM_FE_TO_CL_LAT_DELAYED(to,delay)		(fe_link_time < 0 ? lat_noncongested(0,to) + HOPTIME : fe_link_time)
// #define COMM_FE_TO_CL_LAT_DELAYED(to,delay) (fe_link_time < 0 ? lat_noncongested(0,to) + HOPTIME : fe_link_time)
// #define COMM_CL_TO_FE_LAT(from)	(fe_link_time < 0 ? lat_noncongested(from,0) + HOPTIME : fe_link_time)


/* FIXME: COMM_*_LAT should have congestion */

#define lat_noncongested(from,to) total_noncongested_comm_delay[from][to]
#endif

/** stats **/
extern int master_qram_access_cl[MAX_IC_INDEX][CLUSTERS];
extern int master_link_rx_access_cl[MAX_IC_INDEX][CLUSTERS];
extern int master_link_tx_access_cl[MAX_IC_INDEX][CLUSTERS];
#ifdef TRACK_COMM_USE_TYPES
extern int master_link_tx_use_type_access[MAX_IC_INDEX][LENGTHOF_comm_use_type_enum];
#endif // TRACK_COMM_USE_TYPES

extern counter_t *master_link_width_dist[MAX_IC_INDEX];

#define DECLARE_POWER_STATS(targ) \
counter_t total_master_##targ##_access[MAX_IC_INDEX]; \
counter_t max_master_##targ##_access[MAX_IC_INDEX]; \
double master_##targ##_power[MAX_IC_INDEX]; \
double master_##targ##_power_cc1[MAX_IC_INDEX][CLUSTERS+1]; \
double master_##targ##_power_cc2[MAX_IC_INDEX][CLUSTERS+1]; \
double master_##targ##_power_cc3[MAX_IC_INDEX][CLUSTERS+1]

#define CLEAR_AGGREGATE_POWER_STATS(targ,l) \
total_master_##targ##_access[l] = 0; \
max_master_##targ##_access[l] = 0; \
    
#define CLEAR_POWER_STATS(targ,l,i) \
master_##targ##_power[l] = 0; \
master_##targ##_power_cc1[l][i] = 0; \
master_##targ##_power_cc2[l][i] = 0; \
master_##targ##_power_cc3[l][i] = 0

extern DECLARE_POWER_STATS(qram);
extern DECLARE_POWER_STATS(link_rx);
extern DECLARE_POWER_STATS(link_tx);

#define ACCESS_TOTAL_POWER_STAT_B(targ,l,i,clocker) master_##targ##_power##clocker[l][i]
#define ACCESS_TOTAL_POWER_STAT(targ,l) master_##targ##_power[l]

#define ACCESS_TOTAL_POWER_STAT_CC1(targ,l,i) ACCESS_TOTAL_POWER_STAT_B(targ,l,i,_cc1)
#define ACCESS_TOTAL_POWER_STAT_CC2(targ,l,i) ACCESS_TOTAL_POWER_STAT_B(targ,l,i,_cc2)
#define ACCESS_TOTAL_POWER_STAT_CC3(targ,l,i) ACCESS_TOTAL_POWER_STAT_B(targ,l,i,_cc3)


/** functions **/
void init_routing_table(void);
void interconnect_init(void);
void interconnect_free(void);
void clear_interconnect_access_stats(void);
void total_interconnect_power_stats(void);
void total_interconnect_access_stats(void);
void arbitrary_routing_table_init(void);
void init_link_descr_tables(void);
int cl_comm_lat(int from, int to, int offset, unsigned int bytesize, enum comm_use_type_enum use_type);
#ifdef TRACK_COMM_USE_TYPES
int ring_lat_typed(int from, int to, int myreg, int offset, enum comm_use_type_enum use_type);
#endif
int ring_lat(int from, int to, int myreg, int offset);

void flood_lat(counter_t * results, int from, counter_t sendtime);

#endif // __INTERCONNECT_H

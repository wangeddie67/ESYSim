/*
 * cache.c - cache module routines
 *
 * This file is a part of the SimpleScalar tool suite written by
 {
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 *
 * Copyright (C) 1994, 1995, 1996, 1997, 1998 by Todd M. Austin
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful.  The tool set comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use.
 *
 * Everyone is granted permission to copy, modify and redistribute
 * this tool set under the following conditions:
 *
 *    This source code is distributed for non-commercial use only.
 *    Please contact the maintainer for restrictions applying to
 *    commercial use.
 *
 *    Permission is granted to anyone to make or distribute copies
 *    of this source code, either as received or modified, in any
 *    medium, provided that all copyright notices, permission and
 *    nonwarranty notices are preserved, and that the distributor
 *    grants the recipient permission for further redistribution as
 *    permitted by this document.
 *
 *    Permission is granted to distribute this file in compiled
 *    or executable form under the same conditions that apply for
 *    source code, provided that either:
 *
 *    A. it is accompanied by the corresponding machine-readable
 *       source code,
 *    B. it is accompanied by a written offer, with no time limit,
 *       to give anyone a machine-readable copy of the corresponding
 *       source code in return for reimbursement of the cost of
 *       distribution.  This written offer must permit verbatim
 *       duplication by anyone, or
 *    C. it is distributed by someone who received only the
 *       executable form, and is accompanied by a copy of the
 *       written offer of source code that they received concurrently.
 *
 * In other words, you are welcome to use, share and improve this
 * source file.  You are forbidden to forbid anyone else to use, share
 * and improve what you give them.
 *
 * INTERNET: dburger@cs.wisc.edu
 * US Mail:  1210 W. Dayton Street, Madison, WI 53706
 *
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "headers.h"

//#define CENTRALIZED_L2
//#define CENTL2_BANK_NUM mesh_size


extern FreqCounter L2Freq[MAXTHREADS];
extern struct cache_t *cache_il1[CLUSTERS];
extern struct cache_t *cache_il1[CLUSTERS];

void data_cache_flush( md_addr_t dtlb_tag, int target_tlb_id);
void dir_eventq_insert(struct DIRECTORY_EVENT *event);     // event queue directory version, put dir event in queue


/* extract/reconstruct a block address */
#define CACHE_BADDR(cp, addr)   ((addr) & ~(cp)->blk_mask)
#define CACHE_MK_BADDR(cp, tag, set)                    \
    (((tag) << (cp)->tag_shift)|((set) << (cp)->set_shift))

#define CACHE_MK1_BADDR(cp, tag, set)                   \
    ((((tag) << (cp)->tag_shift)/BANKS)|((set) << (cp)->set_shift))

#define CACHE_MK2_BADDR(cp, tag, set)                   \
    (((tag) << ((cp)->tag_shift - (log_base2(res_membank)-log_base2(n_cache_limit_thrd[threadid])) ))| \
     ((set) << ((cp)->set_shift - (log_base2(res_membank)-log_base2(n_cache_limit_thrd[threadid])) )))


/* index an array of cache blocks, non-trivial due to variable length blocks */
#define CACHE_BINDEX(cp, blks, i)                   \
    ((struct cache_blk_t *)(((char *)(blks)) +              \
        (i)*(sizeof(struct cache_blk_t) +       \
            ((cp)->balloc               \
             ? (cp)->bsize*sizeof(byte_t) : 0))))

/* cache data block accessor, type parameterized */
#define __CACHE_ACCESS(type, data, bofs)                \
    (*((type *)(((char *)data) + (bofs))))

/* cache data block accessors, by type */
#define CACHE_DOUBLE(data, bofs)  __CACHE_ACCESS(double, data, bofs)
#define CACHE_FLOAT(data, bofs)   __CACHE_ACCESS(float, data, bofs)
#define CACHE_WORD(data, bofs)    __CACHE_ACCESS(unsigned int, data, bofs)
#define CACHE_HALF(data, bofs)    __CACHE_ACCESS(unsigned short, data, bofs)
#define CACHE_BYTE(data, bofs)    __CACHE_ACCESS(unsigned char, data, bofs)

/* cache block hashing macros, this macro is used to index into a cache
   set hash table (to find the correct block on N in an N-way cache), the
   cache set index function is CACHE_SET, defined above */
#define CACHE_HASH(cp, key)                     \
    (((key >> 24) ^ (key >> 16) ^ (key >> 8) ^ key) & ((cp)->hsize-1))

/* copy data out of a cache block to buffer indicated by argument pointer p */
#define CACHE_BCOPY(cmd, blk, bofs, p, nbytes)  \
    if (cmd == Read)                            \
{                                   \
    switch (nbytes) {                           \
        case 1:                             \
                                        *((byte_t *)p) = CACHE_BYTE(&blk->data[0], bofs); break;    \
        case 2:                             \
                                        *((half_t *)p) = CACHE_HALF(&blk->data[0], bofs); break;    \
        case 4:                             \
                                        *((word_t *)p) = CACHE_WORD(&blk->data[0], bofs); break;    \
        default:                                \
                                            { /* >= 8, power of two, fits in block */           \
                                                int words = nbytes >> 2;                    \
                                                while (words-- > 0)                     \
                                                {                               \
                                                    *((word_t *)p) = CACHE_WORD(&blk->data[0], bofs);   \
                                                    p += 4; bofs += 4;                  \
                                                }\
                                            }\
    }\
}\
else /* cmd == Write */                     \
{                                   \
    switch (nbytes) {                           \
        case 1:                             \
                                        CACHE_BYTE(&blk->data[0], bofs) = *((byte_t *)p); break;    \
        case 2:                             \
                                        CACHE_HALF(&blk->data[0], bofs) = *((half_t *)p); break;    \
        case 4:                             \
                                        CACHE_WORD(&blk->data[0], bofs) = *((word_t *)p); break;    \
        default:                                \
                                            { /* >= 8, power of two, fits in block */           \
                                                int words = nbytes >> 2;                    \
                                                while (words-- > 0)                     \
                                                {                               \
                                                    CACHE_WORD(&blk->data[0], bofs) = *((word_t *)p);       \
                                                    p += 4; bofs += 4;                  \
                                                }\
                                            }\
    }\
}

/* bound squad_t/dfloat_t to positive int */
#define BOUND_POS(N)        ((int)(min2(max2(0, (N)), 2147483647)))

/* Ronz' stats */

#define BANKS 16        /* Doesn't matter unless you use address-ranges. Same for bank_alloc. */
/*
   int bank_alloc[CACHEPORT] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
 */

/****************************************************/
#ifdef CROSSBAR_INTERCONNECT
int busSlotCount = BUSSLOTS * NUMBEROFBUSES;
extern long long int busUsed;
extern counter_t busOccupancy;
struct bs_nd *busNodePool;
struct bs_nd *crossbarSndHd[3][MAXTHREADS], *crossbarRcvHd[3][MAXTHREADS];
counter_t sendCount[3][MAXTHREADS], rcvCount[3][MAXTHREADS];
#endif
extern counter_t dcache2_access;

#ifdef BUS_INTERCONNECT
int busSlotCount = BUSSLOTS * NUMBEROFBUSES;
extern long long int busUsed;
extern counter_t busOccupancy;
struct bs_nd *busNodePool, *busNodesInUse[NUMBEROFBUSES];
#endif
/*also in sim-outorder.c*/
//Jing 060208
char dl1name[20];
extern struct cache_t *dtlb[CLUSTERS];


#ifdef GHB
//extern int ghb_Size;
//extern int DELTA_INDEX_TABLE_Size;
extern struct delta_count_t
{
int valid;
sqword_t delta;
int count;
};
extern struct delta_count_t delta_count[GHB_Size];

extern struct delta_index_entry
{
    int valid;
    int number;
    sqword_t delta;
    int ghb_num;
    counter_t times;
};

extern struct tag_index_entry
{
    int valid;
    int number;
    md_addr_t tag;
    md_addr_t pred_tag;
};

extern struct ghb_entry
{
    int valid;
    int number;
    md_addr_t miss_addr;

    sqword_t delta;/* delta between this addr and last miss addr -hq */
    int ghb_num_delta;/* index for the last entry which share the same property with this one -hq */

    md_addr_t tag;
    int ghb_num_tag;

    int counted;/* used for counting -hq */
    counter_t when; /* used for counting -hq */
};

extern struct delta_index_entry delta_index_table[MAXTHREADS][DELTA_INDEX_TABLE_Size];
extern struct tag_index_entry tag_index_table[MAXTHREADS][TAG_INDEX_TABLE_Size];
extern struct ghb_entry ghb[MAXTHREADS][GHB_Size];
extern int GHB_head[MAXTHREADS];
#endif
extern int cache_dl1_lat;
extern int cache_il1_lat;
extern int cache_ml2_lat;
extern int cache_dl2_lat;
extern int mesh_size;

// dd cache related; @ fanglei
int mb_pain_count = 0;

extern int tlb_c_c;
extern int multi_blk_set_shift;
extern int multi_blk_adap_en;
extern int multi_blk_adap_inform_en;
extern int mb_pain_en;
extern int shr_read_en;
extern int total_inform_back_en;
extern int repl_ad_en;
extern int recall_ad_en;
extern int vector_num;
extern int sharer_threshold;
extern int nv_broadcast;

extern int L2_prefetch_En;
extern int L1_prefetch_En;

extern int STREAM_min_En;

extern int STREAM_L2_distance;
extern int STREAM_min_distance;

extern int mem_bus_width;
extern float mem_bus_speed;
extern int mem_port_num;
int max_packet_size = 32; //32byte
extern counter_t cacheMiss[MAXTHREADS], hitInOtherCache[MAXTHREADS];
counter_t involve_4_hops, involve_2_hops, involve_2_hop_wb, involve_2_hop_touch, involve_2_hops_wm, involve_4_hops_wm, involve_4_hops_upgrade, involve_2_hops_upgrade;
counter_t involve_4_hops_miss, involve_2_hops_miss;
counter_t data_private_read, data_private_write, data_shared_read, data_shared_write;
extern counter_t load_link_shared, load_link_exclusive;
extern counter_t spand[5];
extern counter_t downgrade_w, downgrade_r;

counter_t total_all_close, total_all_almostclose, total_not_all_close, total_p_c_events, total_consumers, total_packets_in_neighbor, total_packets_at_corners;
counter_t total_data_at_corner, total_data_close, total_data_far, total_data_consumers;
counter_t sharer_num[MAXTHREADS];
double average_inside_percent;
double average_outside_percent;
double average_outside_abs_percent;
double average_corner_percent;
counter_t write_early;
extern long long int dl2ActuallyAccessed, markedLineReplaced;
extern counter_t totalSplitWM, totalSplitNo;
extern counter_t totalUpgradesUsable, totalUpgradesBeneficial;

extern int actualProcess;

int dataFoundInPCB (md_addr_t, int, int);
void invalidateOtherPCB (int who, int id, md_addr_t addr);

/****************************************************/
int dl2_prefetch_active = 0;
int dl2_prefetch_id = 0;
int dl1_prefetch_active = 0;


extern struct quiesceStruct quiesceAddrStruct[CLUSTERS];
extern spec_benchmarks;
extern struct res_pool *fu_pool;
extern struct cache_t *cache_dl1[], *cache_ml2, *cache_dl2;

//#define BAR_OPT

extern md_addr_t barrier_addr;

extern int markReadAfterWrite;

int laten[BANKS];

/* Cache access stats to compute power numbers with Dave's model */
long rm1 = 0;
long wm1 = 0;
long rh1 = 0;
long wh1 = 0;
long rm2 = 0;
long wm2 = 0;
long rh2 = 0;
long wh2 = 0;

#ifdef   THRD_WAY_CACHE
extern int activecontexts;
extern int res_membank;
extern int n_cache_limit_thrd[];
extern int n_cache_start_thrd[];
extern int COHERENT_CACHE;
extern int MSI;
#endif

extern context *thecontexts[MAXTHREADS];


extern counter_t sim_num_insn;
extern long long int executeMaxInsn;

extern int collect_stats, allForked;

#ifdef   THRD_WAY_CACHE
void insert_fillq (long, md_addr_t, int);
#else
void insert_fillq (long, md_addr_t);    /* When a cache discovers a miss, */
#endif

extern int numcontexts;
extern int notRoundRobin;

extern struct RS_list *rs_cache_list[MAXTHREADS][MSHR_SIZE];
// extern struct RS_list *rs_cache_pref_list[MAXTHREADS][MSHR_SIZE];
extern FILE *fp_trace;
extern struct DIRECTORY_EVENT *dir_event_queue;  /* Head pointer point to the directory event queue */
extern struct DIRECTORY_EVENT *event_list[L2_MSHR_SIZE];  /* event list for L2 MSHR */
counter_t nack_counter;
counter_t normal_nacks;
counter_t write_nacks;
counter_t prefetch_nacks;
counter_t sync_nacks;
counter_t flip_counter;
counter_t store_conditional_failed;
counter_t L1_flip_counter;
counter_t e_to_m;
/* queue counter */
extern counter_t L1_mshr_full;
extern counter_t last_L1_mshr_full[MAXTHREADS];
extern counter_t L2_mshr_full;
extern counter_t last_L2_mshr_full;
extern counter_t L2_mshr_full_prefetch;
extern counter_t L1_fifo_full;
extern counter_t last_L1_fifo_full[MAXTHREADS+MESH_SIZE*2];
extern counter_t Dir_fifo_full;
extern counter_t last_Dir_fifo_full[MAXTHREADS+MESH_SIZE*2];
extern counter_t Input_queue_full;
extern counter_t last_Input_queue_full[MAXTHREADS+MESH_SIZE*2];
extern counter_t Output_queue_full;

extern counter_t Stall_L1_mshr;
extern counter_t Stall_L2_mshr;
extern counter_t Stall_L1_fifo;
extern counter_t Stall_dir_fifo;
extern counter_t Stall_input_queue;
extern counter_t Stall_output_queue;
extern int mshr_pending_event[MAXTHREADS];

extern counter_t WM_Miss;
extern counter_t WM_Clean;
extern counter_t WM_S;
extern counter_t WM_EM;
extern counter_t write_shared_used_conf;

extern counter_t SyncInstCacheAccess;
extern counter_t TestCacheAccess;
extern counter_t TestSecCacheAccess;
extern counter_t SetCacheAccess;
extern counter_t SyncLoadReadMiss;
extern counter_t SyncLoadLReadMiss;
extern counter_t SyncLoadHit;
extern counter_t SyncLoadLHit;
extern counter_t SyncStoreCHit;
extern counter_t SyncStoreCWriteMiss;
extern counter_t SyncStoreCWriteUpgrade;
extern counter_t SyncStoreHit;
extern counter_t SyncStoreWriteMiss;
extern counter_t BarStoreWriteMiss;
extern counter_t SyncStoreWriteUpgrade;
extern counter_t BarStoreWriteUpgrade;
extern counter_t Sync_L2_miss;


#ifdef CONF_RES_RESEND
extern struct QUEUE_EVENT *send_queue[MAXTHREADS];
extern struct QUEUE_EVENT *reply_queue[MAXTHREADS];
#endif

extern struct RS_link *event_queue;
extern long long int totalEventCount, totalEventProcessTime;
long long int totaleventcount;
counter_t esynetMsgNo;
counter_t missNo;
counter_t write_back_msg;
counter_t local_cache_access, remote_cache_access, localdirectory, remotedirectory;
tick_t totalmisshandletime;
tick_t totalIL1misshandletime;
long long int totaleventcountnum;
counter_t totalWriteIndicate;
counter_t total_exclusive_modified;
counter_t total_exclusive_conf;
counter_t total_exclusive_cross;
counter_t totalSyncEvent;
counter_t totalNormalEvent;
counter_t totalSyncWriteM;
counter_t totalSyncReadM;
counter_t totalSyncWriteup;
counter_t totalmisstimeforNormal;
counter_t totalmisstimeforSync;
counter_t total_mem_lat;
counter_t total_mem_access;
counter_t totalL2misstime;
counter_t totalWrongL2misstime;
counter_t TotalL2misses;
long long int total_L1_prefetch;
counter_t l2_prefetch_num;
counter_t write_back_early;
long long int total_L1_first_prefetch;
long long int total_L1_sec_prefetch;
counter_t L1_prefetch_usefull;
counter_t L1_prefetch_writeafter;
// @ fanglei
counter_t level1_data_cache_access_num;
counter_t level1_data_cache_access_read_num;
counter_t level1_data_cache_access_write_num;
counter_t level1_data_cache_access_read_hit_num;
counter_t level1_data_cache_access_read_hit_FPP_num;
counter_t level1_data_cache_access_read_hit_FPP_S_M_num;
counter_t level1_data_cache_access_read_hit_FPP_G_D_num;
counter_t level1_data_cache_access_read_hit_FPP_G_T_num;
counter_t level1_data_cache_access_read_hit_PPP_num;
counter_t level1_data_cache_access_read_hit_PPP_S_M_num;
counter_t level1_data_cache_access_read_hit_PPP_G_D_num;
counter_t level1_data_cache_access_read_hit_PPP_G_T_num;
counter_t level1_data_cache_access_read_hit_PPP_verification_num;
counter_t level1_data_cache_access_read_hit_PPP_verification_S_M_num;
counter_t level1_data_cache_access_read_hit_PPP_verification_G_D_num;
counter_t level1_data_cache_access_read_hit_PPP_verification_G_T_num;
counter_t level1_data_cache_access_read_hit_PPP_follow_num;
counter_t level1_data_cache_access_read_hit_PPP_follow_S_M_num;
counter_t level1_data_cache_access_read_hit_PPP_follow_G_D_num;
counter_t level1_data_cache_access_read_hit_PPP_follow_G_T_num;
counter_t level1_data_cache_access_read_hit_PPP_succeed_num;
counter_t level1_data_cache_access_read_hit_PPP_succeed_S_M_num;
counter_t level1_data_cache_access_read_hit_PPP_succeed_G_D_num;
counter_t level1_data_cache_access_read_hit_PPP_succeed_G_T_num;
counter_t level1_data_cache_access_read_hit_PPP_fail_num;
counter_t level1_data_cache_access_read_hit_PPP_fail_S_M_num;
counter_t level1_data_cache_access_read_hit_PPP_fail_G_D_num;
counter_t level1_data_cache_access_read_hit_PPP_fail_G_T_num;
counter_t level1_data_cache_access_read_miss_num;
counter_t level1_data_cache_access_read_compulsory_miss_num;
counter_t level1_data_cache_access_read_capacity_miss_num;
counter_t level1_data_cache_access_read_coherence_miss_num;
counter_t level1_data_cache_access_read_prefetch_capacity_miss_num;
counter_t level1_data_cache_access_write_local_num;
counter_t level1_data_cache_access_write_local_FPP_num;
counter_t level1_data_cache_access_write_update_num;
counter_t level1_data_cache_access_write_update_FPP_num;
counter_t level1_data_cache_access_write_update_PPP_num;
counter_t level1_data_cache_access_write_update_PPP_S_M_num;
counter_t level1_data_cache_access_write_update_PPP_G_D_num;
counter_t level1_data_cache_access_write_update_PPP_G_T_num;
counter_t level1_data_cache_access_write_miss_num;
counter_t level1_data_cache_access_write_compulsory_miss_num;
counter_t level1_data_cache_access_write_capacity_miss_num;
counter_t level1_data_cache_access_write_coherence_miss_num;
counter_t level1_data_cache_access_write_prefetch_capacity_miss_num;

counter_t read_L1_miss_L2_hit_num;
counter_t read_L1_miss_L2_hit_total_time;
counter_t read_L2_miss_num;
counter_t read_L2_miss_total_time;
counter_t write_L1_miss_L2_hit_num;
counter_t write_L1_miss_L2_hit_total_time;
counter_t write_L2_miss_num;
counter_t write_L2_miss_total_time;

// dd cache counter; @ fanglei
counter_t total_page_num;
counter_t pr_page_num;
counter_t pw_page_num;
counter_t sr_page_num;
counter_t sw_page_num;

counter_t total_line_num;
counter_t pr_line_num;
counter_t pw_line_num;
counter_t sr_line_num;
counter_t sw_line_num;

counter_t recall_num;
counter_t recall_inv_num;

counter_t read_inv_S_num;
counter_t read_inv_QH_num;
counter_t read_inv_e_QM_num;
counter_t read_inv_d_QM_num;

counter_t write_inv_d_S_num;
counter_t write_inv_e_S_num;
counter_t write_inv_s_S_num;
counter_t write_inv_i_S_num;

counter_t write_inv_d_QM_num;
counter_t write_inv_e_QM_num;
counter_t write_inv_s_QM_num;
counter_t write_inv_i_QM_num;

counter_t write_inv_d_QH_num;
counter_t write_inv_e_QH_num;
counter_t write_inv_s_QH_num;
counter_t write_inv_i_QH_num;

counter_t recall_inv_d_S_num;
counter_t recall_inv_e_S_num;
counter_t recall_inv_s_S_num;
counter_t recall_inv_i_S_num;

counter_t recall_inv_d_Q_num;
counter_t recall_inv_e_Q_num;
counter_t recall_inv_s_Q_num;
counter_t recall_inv_i_Q_num;

counter_t SW_read_request_num;
counter_t SW_write_request_num;

counter_t read_S_hit_num;
counter_t read_Q_hit_num;
counter_t read_Q_init_num;
counter_t read_Q_init_SR_num;
counter_t read_Q_init_QR_num;

counter_t write_S_hit_num;
counter_t write_Q_hit_num;
counter_t write_S_init_ed_num;
counter_t write_S_init_ed_SR_num;
counter_t write_S_init_ed_QR_num;
counter_t write_S_init_s_num;
counter_t write_S_init_s_SR_num;
counter_t write_S_init_s_QR_num;
counter_t write_Q_init_num;
counter_t write_Q_init_SR_num;
counter_t write_Q_init_QR_num;

counter_t shr_data_req_initial_num;
counter_t shr_data_req_total_num;
counter_t shr_data_req_fail_num;

counter_t inform_back_num;
counter_t write_back_num;
counter_t inform_back_Q;
counter_t inform_back_S;
counter_t write_back_Q;
counter_t write_back_S;
counter_t mb_pain_delay_num;
counter_t mb_pain_portuse_num;

counter_t read_Q_init_QR_s_num;
counter_t read_Q_init_QR_ed_num;
counter_t write_S_init_ed_QR_s_num;
counter_t write_S_init_ed_QR_ed_num;
counter_t write_S_init_s_QR_s_num;
counter_t write_S_init_s_QR_ed_num;
counter_t write_Q_init_QR_s_num;
counter_t write_Q_init_QR_ed_num;

counter_t recall_inv_d_Q_s_num;
counter_t recall_inv_d_Q_ed_num;
counter_t recall_inv_e_Q_s_num;
counter_t recall_inv_e_Q_ed_num;
counter_t recall_inv_s_Q_s_num;
counter_t recall_inv_s_Q_ed_num;
counter_t recall_inv_i_Q_s_num;
counter_t recall_inv_i_Q_ed_num;

counter_t recall_inv_d_Q_s_ad_num;
counter_t recall_inv_e_Q_s_ad_num;
counter_t recall_inv_s_Q_ed_ad_num;

counter_t line_access_num;
counter_t SWP_line_access_num;
counter_t SWP_PR_line_access_num;
counter_t SWP_PW_line_access_num;
counter_t SWP_SR_line_access_num;
counter_t SWP_SW_line_access_num;

counter_t PVC_vector_distr_record_num;
counter_t PVC_vector_inc_distr_record_num;
counter_t PVC_vector_distr[MAX_DIRECTORY_ASSOC+1];
counter_t PVC_vector_inc_distr[MAX_DIRECTORY_ASSOC+1];

counter_t PVC_exchange_num;
counter_t PVC_flip_num;
counter_t PVC_dowm_no_inv_num;
counter_t PVC_dowm_inv_num;
counter_t PVC_dowm_inv_shr_num;
counter_t PVC_up_no_add_num;
counter_t PVC_up_add_num;
counter_t PVC_up_add_shr_num;
counter_t PVC_exch_vec_shr_distr[MAXTHREADS+1];
counter_t PVC_dowm_inv_shr_distr[MAXTHREADS+1];
counter_t PVC_up_add_shr_distr[MAXTHREADS+1];
counter_t PVC_inform_all_shr_num;
counter_t PVC_rfs_all_shr_num;
counter_t PVC_read_all_shr_num;
counter_t PVC_write_all_shr_num;
counter_t PVC_recall_all_shr_num;
counter_t pvc_nv_conflict_num;

long long int totalreqcountnum;
long long int totalmisscountnum;
int flag;

/* unlink BLK from the hash table bucket chain in SET */
    static void
unlink_htab_ent (struct cache_t *cp,    /* cache to update */
        struct cache_set_t *set,    /* set containing bkt chain */
        struct cache_blk_t *blk)    /* block to unlink */
{
    struct cache_blk_t *prev, *ent;
    int index = CACHE_HASH (cp, blk->tagid.tag);

    /* locate the block in the hash table bucket chain */
    for (prev = NULL, ent = set->hash[index]; ent; prev = ent, ent = ent->hash_next)
    {
        if (ent == blk)
            break;
    }
    assert (ent);

    /* unlink the block from the hash table bucket chain */
    if (!prev)
    {
        /* head of hash bucket list */
        set->hash[index] = ent->hash_next;
    }
    else
    {
        /* middle or end of hash bucket list */
        prev->hash_next = ent->hash_next;
    }
    ent->hash_next = NULL;
}

/* insert BLK onto the head of the hash table bucket chain in SET */
    static void
link_htab_ent (struct cache_t *cp,  /* cache to update */
        struct cache_set_t *set,    /* set containing bkt chain */
        struct cache_blk_t *blk)    /* block to insert */
{
    int index = CACHE_HASH (cp, blk->tagid.tag);

    /* insert block onto the head of the bucket chain */
    blk->hash_next = set->hash[index];
    set->hash[index] = blk;
}

/* where to insert a block onto the ordered way chain */
enum list_loc_t
{ Head, Tail };

/* insert BLK into the order way chain in SET at location WHERE */
    static void
update_way_list (struct cache_set_t *set,   /* set contained way chain */
        struct cache_blk_t *blk,    /* block to insert */
        enum list_loc_t where)  /* insert location */
{
    /* unlink entry from the way list */
    if (!blk->way_prev && !blk->way_next)
    {
        /* only one entry in list (direct-mapped), no action */
        assert (set->way_head == blk && set->way_tail == blk);
        /* Head/Tail order already */
        return;
    }

    /* else, more than one element in the list */
    else if (!blk->way_prev)
    {
        assert (set->way_head == blk && set->way_tail != blk);
        if (where == Head)
        {
            /* already there */
            return;
        }

        /* else, move to tail */
        set->way_head = blk->way_next;
        blk->way_next->way_prev = NULL;
    }
    else if (!blk->way_next)
    {
        /* end of list (and not front of list) */
        assert (set->way_head != blk && set->way_tail == blk);
        if (where == Tail)
        {
            /* already there */
            return;
        }
        set->way_tail = blk->way_prev;
        blk->way_prev->way_next = NULL;
    }
    else
    {
        /* middle of list (and not front or end of list) */
        assert (set->way_head != blk && set->way_tail != blk);
        blk->way_prev->way_next = blk->way_next;
        blk->way_next->way_prev = blk->way_prev;
    }

    /* link BLK back into the list */
    if (where == Head)
    {
        /* link to the head of the way list */
        blk->way_next = set->way_head;
        blk->way_prev = NULL;
        set->way_head->way_prev = blk;
        set->way_head = blk;
    }
    else if (where == Tail)
    {
        /* link to the tail of the way list */
        blk->way_prev = set->way_tail;
        blk->way_next = NULL;
        set->way_tail->way_next = blk;
        set->way_tail = blk;
    }
    else
        panic ("bogus WHERE designator");
}

/* create and initialize a general cache structure */
struct cache_t *        /* pointer to cache created */
cache_create (char *name,   /* name of the cache */
        int nsets,  /* total number of sets in cache */
        int bsize,  /* block (line) size of cache */
        int balloc, /* allocate data space for blocks? */
        int usize,  /* size of user data to alloc w/blks */     // ? @ fanglei
        int assoc,  /* associativity of cache */
        enum cache_policy policy,   /* replacement policy w/in sets */
        /* block access function, see description w/in struct cache def */
        unsigned int (*blk_access_fn) (enum mem_cmd cmd, md_addr_t baddr, int bsize, struct cache_blk_t * blk, tick_t now, int threadid), unsigned int hit_latency, /* latency in cycles for a hit */
        int threadid)
{
    struct cache_t *cp;
    struct cache_blk_t *blk;
    int i, j, bindex;

    /* check all cache parameters */
    if (nsets <= 0)
        fatal ("cache size (in sets) `%d' must be non-zero", nsets);
    if ((nsets & (nsets - 1)) != 0)
        fatal ("cache size (in sets) `%d' is not a power of two", nsets);
    /* blocks must be at least one datum large, i.e., 8 bytes for SS */
    if (bsize < 8)
        fatal ("cache block size (in bytes) `%d' must be 8 or greater", bsize);
    if ((bsize & (bsize - 1)) != 0)
        fatal ("cache block size (in bytes) `%d' must be a power of two", bsize);
    if (usize < 0)
        fatal ("user data size (in bytes) `%d' must be a positive value", usize);
    if (!blk_access_fn)
        fatal ("must specify miss/replacement functions");

    /* allocate the cache structure */
    cp = (struct cache_t *) calloc (1, sizeof (struct cache_t) + (nsets - 1) * sizeof (struct cache_set_t));
    if (!cp)
        fatal ("out of virtual memory");

    /* initialize user parameters */
    cp->name = mystrdup (name);
    cp->nsets = nsets;
    cp->bsize = bsize;
    cp->balloc = balloc;
    cp->usize = usize;
    cp->assoc = assoc;
    cp->policy = policy;
    cp->hit_latency = hit_latency;

    /* miss/replacement functions */
    cp->blk_access_fn = blk_access_fn;

    /* compute derived parameters */
    cp->hsize = CACHE_HIGHLY_ASSOC (cp) ? (assoc >> 2) : 0;
    cp->blk_mask = bsize - 1;
    cp->set_shift = log_base2 (bsize);
    cp->set_mask = nsets - 1;
    cp->tag_shift = cp->set_shift + log_base2 (nsets);
    cp->tag_mask = (1 << (32 - cp->tag_shift)) - 1;
    cp->tagset_mask = ~cp->blk_mask;
    cp->bus_free = 0;

    /* initialize cache stats */
    cp->hits = 0;
    cp->dhits = 0;
    cp->misses = 0;
    cp->dmisses = 0;
    cp->in_mshr = 0;
    cp->din_mshr = 0;
    cp->dir_access = 0;
    cp->data_access = 0;
    cp->coherence_misses = 0;
    cp->capacitance_misses = 0;
    cp->replacements = 0;
    cp->replInv = 0;
    cp->writebacks = 0;
    cp->wb_coherence_w = 0;
    cp->wb_coherence_r = 0;
    cp->invalidations = 0;
    cp->s_to_i = 0;
    cp->e_to_i = 0;
    cp->e_to_m = 0;
    cp->coherencyMisses = 0;
    cp->coherencyMissesOC = 0;
    cp->Invld = 0;
    cp->rdb = 0;
    cp->wrb = 0;
    cp->lastuse = 0;
    cp->dir_notification = 0;
    cp->Invalid_write_received = 0;
    cp->Invalid_write_received_hits = 0;
    cp->Invalid_Read_received = 0;
    cp->Invalid_Read_received_hits = 0;
    cp->ack_received = 0;

    /* blow away the last block accessed */
    cp->last_tagsetid.tag = 0;
    cp->last_tagsetid.threadid = -1;
    cp->last_blk = NULL;
    cp->threadid = threadid;

#ifdef  DCACHE_MSHR
    int mshr_flag = 0;
    if((!strcmp (cp->name, "dl1")) || (!strcmp (cp->name, "dl2")))
    {
        if(!strcmp (cp->name, "dl1"))
            mshr_flag = 0;
        else
            mshr_flag = 1;
        cp->mshr = (struct mshr_t *) calloc (1, sizeof (struct mshr_t));
        initMSHR(cp->mshr, mshr_flag);
        cp->mshr->threadid = threadid;
    }
    else
        cp->mshr = NULL;
#endif

    /* allocate data blocks */
    cp->data = (byte_t *) calloc (nsets * assoc, sizeof (struct cache_blk_t) + (cp->balloc ? (bsize * sizeof (byte_t)) : 0));
    if (!cp->data)
        fatal ("out of virtual memory");

    /* slice up the data blocks */
    for (bindex = 0, i = 0; i < nsets; i++)
    {
        cp->sets[i].way_head = NULL;
        cp->sets[i].way_tail = NULL;

        cp->sets[i].PVC_vector_distr_record_num = 0;
        cp->sets[i].PVC_vector_inc_distr_record_num = 0;
        int PVC_i;
        for(PVC_i=0;PVC_i<=MAX_DIRECTORY_ASSOC;PVC_i++)
        {
            cp->sets[i].PVC_vector_distr[PVC_i] = 0;
            cp->sets[i].PVC_vector_inc_distr[PVC_i] = 0;  
        }
        /* get a hash table, if needed */
        if (cp->hsize)
        {
            cp->sets[i].hash = (struct cache_blk_t **) calloc (cp->hsize, sizeof (struct cache_blk_t *));
            if (!cp->sets[i].hash)
                fatal ("out of virtual memory");
        }
        /* NOTE: all the blocks in a set *must* be allocated contiguously,
           otherwise, block accesses through SET->BLKS will fail (used
           during random replacement selection) */
        cp->sets[i].blks = CACHE_BINDEX (cp, cp->data, bindex);

        /* link the data blocks into ordered way chain and hash table bucket
           chains, if hash table exists */
        for (j = 0; j < assoc; j++)
        {
            /* locate next cache block */
            blk = CACHE_BINDEX (cp, cp->data, bindex);
            bindex++;

            /* invalidate new cache block */
            // tlb state init @ fanglei
            blk->tlb_state = 0;
            blk->tlb_lock = 0;
            
            // PVC for directory cache; @ fanglei
            blk->all_shared_en = 0;

            // q_blk @ fanglei
            blk->qblk_set_shift = -1;

            blk->status = 0;
            blk->tagid.tag = 0;
            blk->tagid.threadid = -1;
            blk->ready = 0;

            int m = 0, n=0;
            for(m=0; m<8; m++)
            {
                for(n=0;n<8;n++)
                    {
                        blk->dir_sharer[m][n]=0;
                    }
                blk->dir_state[m]=DIR_STABLE;
                blk->dir_dirty[m] = -1;
                blk->exclusive_time[m] = 0;
                if(MSI)
                    blk->dir_blk_state[m] = MESI_INVALID;
                blk->ptr_cur_event[m] = NULL;
            }
            blk->ever_dirty = 0;
            blk->ever_touch = 0;

            blk->isStale = 0;

            blk->lineVolatile = 0;
            blk->isL1prefetch = 0;

            blk->wordVolatile = 0;
            blk->wordInvalid = 0;
            blk->epochModified = -1;

            blk->user_data = (usize != 0 ? (byte_t *) calloc (usize, sizeof (byte_t)) : NULL);
            blk->invCause = 0;

            blk->ReadCount = 0;
            blk->WriteCount = 0;
            blk->WordCount = 0;
            blk->Type = 0;
            for(m=0;m<8;m++)
                blk->WordUseFlag[m] = 0;
            /* from garg */
            if (MSI)
            {
                blk->state = MESI_INVALID;
            }


            /* insert cache block into set hash table */
            if (cp->hsize)
                link_htab_ent (cp, &cp->sets[i], blk);

            /* insert into head of way list, order is arbitrary at this point */
            blk->way_next = cp->sets[i].way_head;
            blk->way_prev = NULL;
            if (cp->sets[i].way_head)
                cp->sets[i].way_head->way_prev = blk;
            cp->sets[i].way_head = blk;
            if (!cp->sets[i].way_tail)
                cp->sets[i].way_tail = blk;
        }
    }

    if(!strcmp (cp->name, "ml2"))
    {
        if( vector_num < 0 || vector_num > assoc )
            panic("vector_num illegal\n");
        if( sharer_threshold < 1 || sharer_threshold > (mesh_size*mesh_size-1) )
            panic("sharer_threshold illegal\n");
        for(i=0;i<nsets;i++)
        {
            blk = cp->sets[i].way_tail;
            bindex = 0;
            while(blk)
            {
                if(bindex < vector_num)
                    blk->ptr_en = 0;
                else
                    blk->ptr_en = 1;
                blk = blk->way_prev;
                bindex++;
            }
        }
    }

    return cp;
}

void allocate_free_event_list()
{
    int i;
    free_event_list = calloc(1, sizeof(struct DIRECTORY_EVENT));
    struct DIRECTORY_EVENT * next = free_event_list;

    for(i = 1; i < maxEvent; i++)
    {
        next->next = calloc(1, sizeof(struct DIRECTORY_EVENT));
        next = next->next;
        next->next = NULL;
    }
}

struct DIRECTORY_EVENT * allocate_event(int isSyncAccess)
{
    if(totalEventCount >= maxEvent)
        panic("Out of free events\n");
    struct DIRECTORY_EVENT * temp = free_event_list;
    free_event_list = free_event_list->next;
    temp->next = NULL;
    temp->op = 0;
    temp->isPrefetch = 0;           // if this is a prefetch
    temp->page_entry_state = 0;
    temp->l2Status = 0;         // 0 - not yet accessed, 1 - l2 miss, 2 - l2 miss completed
    temp->l2MissStart = 0;
    temp->when = 0;                         // when directory be accessed
    temp->started = 0;
    temp->mshr_time = 0;
    temp->transfer_time = 0;
    temp->req_time = 0;
    temp->store_cond = 0;
    temp->flip_flag = 0;
    temp->originalChildCount = 0;

    temp->l1_wb_entry = NULL;



    // dd related @ fanglei
    temp->tlb_state = -1;
    temp->shr_data_req = 0;
    temp->data_req_sharer = 0;
    temp->L1_hit = 0;
    temp->no_L2_delay = 0;
    temp->ml2_dir_blk_state = 0;

    // q_blk @ fanglei
    temp->qblk_set_shift = 0;

    temp->input_buffer_full = 0;
    temp->pending_flag = 0;
    temp->L2miss_flag = 0;
    temp->dirty_flag = 0;
    temp->arrival_time = 0;
    temp->delay = 0;
    temp->data_reply = 0;
    temp->conf_bit = -1;
    temp->missNo = 0;
    temp->resend = 0;
    temp->operation = 0;                        // directory operation, writehit update, writemiss search, readmiss search be different
    temp->tempID = 0;                           // which processor issue the directory operation
    temp->addr = 0;                         // address to access
    temp->blk1 = NULL;
    temp->blk_dir = NULL;   // point to dl1 block accessed

    temp->ptr_event = NULL;             // point to the false entry in event queue
    temp->cp = NULL;                    // cache to access
    temp->vp = NULL;                            // ptr to buffer for input/output
    temp->nbytes = 0;                           // number of bytes to access
    temp->udata = NULL;                         // for return of user data ptr
    temp->rs = NULL;
    temp->src1 = 0;
    temp->src2 = 0;
    temp->des1 = 0;
    temp->des2 = 0;
    temp->new_src1 = 0;
    temp->new_src2 = 0;
    temp->new_des1 = 0;
    temp->new_des2 = 0;
    temp->startCycle = 0;
    temp->parent = NULL;       // pointer to next entry
    temp->parent_operation = 0;         // Parent operation is used for L2 cache MISS
    temp->childCount = 0;
    temp->processingDone = 0;
    temp->spec_mode = 0;
    temp->esynetMsgNo = 0;
    temp->eventType = 0;
    temp->rec_busy = 0;
    temp->sharer_num = 0;
    temp->prefetch_next = 0;
    temp->stride = 0;
    temp->order = 0;
    temp->stream_entry_index = 0;
    temp->trigger = 0;
    temp->isReqL2Hit = 0;
    temp->isReqL2Inv = 0;
    temp->isReqL2SecMiss = 0;
    temp->isReqL2Trans = 0;
    temp->reqAtDirTime = 0;
    temp->reqAckTime = 0;
    temp->isSyncAccess = isSyncAccess;
    totalEventCount++;
    totaleventcount++;
    temp->totaleventcount = totaleventcount;
    return temp;
}

void free_event(struct DIRECTORY_EVENT *event)
{
    event->next = free_event_list;
    free_event_list = event;
    totalEventCount--;
}

/* parse policy */
    enum cache_policy       /* replacement policy enum */
cache_char2policy (char c)  /* replacement policy as a char */
{
    switch (c)
    {
        case 'l':
            return LRU;
        case 'r':
            return Random;
        case 'f':
            return FIFO;
        default:
            fatal ("bogus replacement policy, `%c'", c);
    }
}

/* print cache configuration */
    void
cache_config (struct cache_t *cp,   /* cache instance */
        FILE * stream)  /* output stream */
{
    fprintf (stream, "cache: %s: %d sets, %d byte blocks, %d bytes user data/block\n", cp->name, cp->nsets, cp->bsize, cp->usize);
    fprintf (stream, "cache: %s: %d-way, `%s' replacement policy, write-back\n", cp->name, cp->assoc, cp->policy == LRU ? "LRU" : cp->policy == Random ? "Random" : cp->policy == FIFO ? "FIFO" : (abort (), ""));
}

/* register cache stats */
    void
cache_reg_stats (struct cache_t *cp,    /* cache instance */
        struct stat_sdb_t *sdb, int id) /* stats database */
{
    char buf[512], buf1[512], *name;


    if (cp->threadid != -1 && cp->threadid != id)
        panic ("Wrong cache being called in cache_reg_stats\n");

    /* get a name for this cache */
    if (!cp->name || !cp->name[0])
        name = "<unknown>";
    else
        name = cp->name;

    name = mystrdup(cp->name);

    sprintf(buf, "_%d", id);
    strcat(name, buf);

    sprintf (buf, "%s.accesses", name);
    sprintf (buf1, "%s.hits + %s.misses + %s.in_mshr", name, name, name);
    stat_reg_formula (sdb, buf, "total number of accesses", buf1, "%12.0f");
    sprintf (buf, "%s.hits", name);
    stat_reg_counter (sdb, buf, "total number of hits", &cp->hits, 0, NULL);
    sprintf (buf, "%s.in_mshr", name);
    stat_reg_counter (sdb, buf, "total number of secondary misses", &cp->in_mshr, 0, NULL);
    sprintf (buf, "%s.misses", name);
    stat_reg_counter (sdb, buf, "total number of misses", &cp->misses, 0, NULL);

    sprintf (buf, "%s.daccesses", name);
    sprintf (buf1, "%s.dhits + %s.dmisses + %s.din_mshr", name, name, name);
    stat_reg_formula (sdb, buf, "total number of data accesses", buf1, "%12.0f");
    sprintf (buf, "%s.dhits", name);
    stat_reg_counter (sdb, buf, "total number of data hits", &cp->dhits, 0, NULL);
    sprintf (buf, "%s.din_mshr", name);
    stat_reg_counter (sdb, buf, "total number of data secondary misses", &cp->din_mshr, 0, NULL);
    sprintf (buf, "%s.dmisses", name);
    stat_reg_counter (sdb, buf, "total number of data misses", &cp->dmisses, 0, NULL);

    sprintf (buf, "%s.dir_access", name);
    stat_reg_counter (sdb, buf, "total number of dir_access", &cp->dir_access, 0, NULL);
    sprintf (buf, "%s.data_access", name);
    stat_reg_counter (sdb, buf, "total number of data_access", &cp->data_access, 0, NULL);
    sprintf (buf, "%s.coherence_misses", name);
    stat_reg_counter (sdb, buf, "total number of misses due to invalidation", &cp->coherence_misses, 0, NULL);
    sprintf (buf, "%s.capacitance_misses", name);
    stat_reg_counter (sdb, buf, "total number of misses due to capacitance", &cp->capacitance_misses, 0, NULL);
    sprintf (buf, "%s.replacements", name);
    stat_reg_counter (sdb, buf, "total number of replacements", &cp->replacements, 0, NULL);
    sprintf (buf, "%s.replInv", name);
    stat_reg_counter (sdb, buf, "total number of replacements which also include invalidations", &cp->replInv, 0, NULL);
    sprintf (buf, "%s.writebacks", name);
    stat_reg_counter (sdb, buf, "total number of writebacks", &cp->writebacks, 0, NULL);
    sprintf (buf, "%s.wb_coherence_w", name);
    stat_reg_counter (sdb, buf, "total number of writebacks due to coherence write", &cp->wb_coherence_w, 0, NULL);
    sprintf (buf, "%s.wb_coherence_r", name);
    stat_reg_counter (sdb, buf, "total number of writebacks due to coherence read", &cp->wb_coherence_r, 0, NULL);
    sprintf (buf, "%s.Invld", name);
    stat_reg_counter (sdb, buf, "total number of Invld", &cp->Invld, 0, NULL);
    sprintf (buf, "%s.invalidations", name);
    stat_reg_counter (sdb, buf, "total number of invalidations", &cp->invalidations, 0, NULL);
    sprintf (buf, "%s.s_to_i", name);
    stat_reg_counter (sdb, buf, "total number of shared to invalid(invalidation_received_sub)", &cp->s_to_i, 0, NULL);
    sprintf (buf, "%s.e_to_i", name);
    stat_reg_counter (sdb, buf, "total number of exclusive to invalid(invalidation_received_sub)", &cp->e_to_i, 0, NULL);
    sprintf (buf, "%s.e_to_m", name);
    stat_reg_counter (sdb, buf, "total number of exclusive to invalid(invalidation_received_sub)", &cp->e_to_m, 0, NULL);
    /* how many misses waiting for MSHR entry */
    sprintf (buf, "%s.dir_notification", name);
    stat_reg_counter (sdb, buf, "total number of updating directory", &cp->dir_notification, 0, NULL);
    sprintf (buf, "%s.Invalid_write_received", name);
    stat_reg_counter (sdb, buf, "total number of invalidation write received", &cp->Invalid_write_received, 0, NULL);
    sprintf (buf, "%s.Invalid_read_received", name);
    stat_reg_counter (sdb, buf, "total number of invalidation read received", &cp->Invalid_Read_received, 0, NULL);
    sprintf (buf, "%s.Invalid_w_received_hits", name);
    stat_reg_counter (sdb, buf, "total number of invalidation write received_hits", &cp->Invalid_write_received_hits, 0, NULL);
    sprintf (buf, "%s.Invalid_r_received_hits", name);
    stat_reg_counter (sdb, buf, "total number of invalidation read received_hits", &cp->Invalid_Read_received_hits, 0, NULL);
    sprintf (buf, "%s.acknowledgement received", name);
    stat_reg_counter (sdb, buf, "total number of acknowledgement received", &cp->ack_received, 0, NULL);

    sprintf (buf, "%s.coherencyMisses", name);
    stat_reg_counter (sdb, buf, "total number of coherency Misses", &cp->coherencyMisses, 0, NULL);
    sprintf (buf, "%s.coherencyMissesOC", name);
    stat_reg_counter (sdb, buf, "total number of coherency Misses due to optimistic core", &cp->coherencyMissesOC, 0, NULL);
    sprintf (buf, "%s.miss_rate", name);
    sprintf (buf1, "%s.misses / %s.accesses", name, name);
    stat_reg_formula (sdb, buf, "miss rate (i.e., misses/ref)", buf1, NULL);
    sprintf (buf, "%s.repl_rate", name);
    sprintf (buf1, "%s.replacements / %s.accesses", name, name);
    stat_reg_formula (sdb, buf, "replacement rate (i.e., repls/ref)", buf1, NULL);
    sprintf (buf, "%s.wb_rate", name);
    sprintf (buf1, "%s.writebacks / %s.accesses", name, name);
    stat_reg_formula (sdb, buf, "writeback rate (i.e., wrbks/ref)", buf1, NULL);
    sprintf (buf, "%s.inv_rate", name);
    sprintf (buf1, "%s.invalidations / %s.accesses", name, name);
    stat_reg_formula (sdb, buf, "invalidation rate (i.e., invs/ref)", buf1, NULL);


    sprintf (buf, "%s.flushCount", name);
    stat_reg_counter (sdb, buf, "total flushes", &cp->flushCount, 0, NULL);
    sprintf (buf, "%s.lineFlushed", name);
    stat_reg_counter (sdb, buf, "lines flushed", &cp->lineFlushCount, 0, NULL);

    sprintf (buf, "%s.flushRate", name);
    sprintf (buf1, "%s.lineFlushed / %s.flushCount", name, name);
    stat_reg_formula (sdb, buf, "flush rate (i.e., lines/chunk)", buf1, NULL);

    free(name);
}

/* ______________________________ start hot leakage **************************************/






/* ______________________________ end hot leakage **************************************/

/* print cache stats */
    void
cache_stats (struct cache_t *cp,    /* cache instance */
        FILE * stream)  /* output stream */
{
    double sum = (double) (cp->hits + cp->misses);

    fprintf (stream, "cache: %s: %.0f hits %.0f misses %.0f repls %.0f invalidations\n", cp->name, (double) cp->hits, (double) cp->misses, (double) cp->replacements, (double) cp->invalidations);
    fprintf (stream, "cache: %s: miss rate=%f  repl rate=%f  invalidation rate=%f\n", cp->name, (double) cp->misses / sum, (double) (double) cp->replacements / sum, (double) cp->invalidations / sum);
}

    int
bank_check (md_addr_t addr, struct cache_t *cp)
{
    return 0;
}

#define BUS_TRAN_LAT 0

#define SET 1
#define TAG 2


/********************************************multiprocessor************************************************/

void page_table_init ()
{
    int i;
    for (i=0;i<PAGE_NUM;i++)
    {
        page_table[i] = (struct page_entry *) calloc (1, sizeof (struct page_entry));
        page_table[i]->valid = 0;
        page_table[i]->busy = 0;
        page_table[i]->VPN = 0;
        page_table[i]->PPN = 0;
        page_table[i]->keeper = -1;
        page_table[i]->cached = 0;
        page_table[i]->state = 0;
        page_table[i]->line_head = NULL;
    }
    printf("page_table_init finish\n");
}

int page_table_check ( md_addr_t addr, int *page_table_state, int *page_table_busy, int *page_table_keeper, int *location )
{
    int match = 0;
    int i;
    struct cache_t *cp = dtlb[0];
    md_addr_t tag = CACHE_TAG (cp, addr);
    for (i=0;i<PAGE_NUM;i++)
    {
        if (!page_table[i]->valid) // no match
        {
            *location = i;
            break;
        }
        else if (page_table[i]->VPN == tag)
        {
            *page_table_state  = page_table[i]->state;
            *page_table_busy   = page_table[i]->busy;
            *page_table_keeper = page_table[i]->keeper;
            *location = i;
            match = 1;
            break;
        }
    }
    if (i == PAGE_NUM)
        panic("not enough page table entry\n");
    return match;
}

void dtlb_access ( int threadid, md_addr_t addr , int *state )
{
    struct cache_t *cp = dtlb[threadid];
    struct cache_blk_t *blk;
    md_addr_t tag, set;
    tag = CACHE_TAG (cp, addr);
    set = CACHE_SET (cp, addr);
    for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
    {
        if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
        {
            *state = blk->state;
            return;
        }
    }
}

void share_pattern( int threadid, md_addr_t addr, int cmd )
{
    // search the page_table;
    int page_table_match;
    int page_table_state;
    int page_table_busy;
    int page_table_keeper;
    int location;

    int line_owner;
    int line_state;
    int line_index;
    line_index = ( addr >> cache_dl1[0]->set_shift ) & ((1<<(MD_LOG_PAGE_SIZE - cache_dl1[0]->set_shift))-1);

    page_table_match = page_table_check(addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);
    if( !page_table_match )
        panic("this entry must be in process\n");
    if(!page_table[location]->line_head)
        panic("line head init error\n");
    line_owner = page_table[location]->line_head[line_index].line_owner;
    line_state = page_table[location]->line_head[line_index].line_state;

    // counters; @ fanglei
    line_access_num++;
    if (page_table_state == SHARED_WRITE)
    {
        SWP_line_access_num++;
        switch (line_state)
        {
            case PRIVATE_READ:  {SWP_PR_line_access_num++;   break;}
            case PRIVATE_WRITE:  {SWP_PW_line_access_num++;   break;}
            case SHARED_READ:  {SWP_SR_line_access_num++;   break;}
            case SHARED_WRITE:  {SWP_SW_line_access_num++;   break;}
        }
    }

    if(line_owner == -1)
    {
        page_table[location]->line_head[line_index].line_owner = threadid;
        page_table[location]->line_head[line_index].line_state = (cmd == Read) ? PRIVATE_READ : PRIVATE_WRITE;
        total_line_num++;
        if(cmd == Read)
            pr_line_num++;
        else
            pw_line_num++;
    }
    else
    {
        if(line_state == PRIVATE_READ)
        {
            if(threadid == line_owner)
            {
                if(cmd == Write)
                {
                    page_table[location]->line_head[line_index].line_state = PRIVATE_WRITE;
                    pr_line_num--;
                    pw_line_num++;
                }
            }
            else
            {
                if(cmd == Read)
                {
                    page_table[location]->line_head[line_index].line_state = SHARED_READ;
                    pr_line_num--;
                    sr_line_num++;
                }
                else
                {
                    page_table[location]->line_head[line_index].line_state = SHARED_WRITE;
                    pr_line_num--;
                    sw_line_num++;
                }
            }
        }
        else if(line_state == PRIVATE_WRITE)
        {
            if( threadid != line_owner )
            {
                page_table[location]->line_head[line_index].line_state = SHARED_WRITE;
                pw_line_num--;
                sw_line_num++;
            }
        }
        else if(line_state == SHARED_READ)
        {
            if(cmd == Write)
            {
                page_table[location]->line_head[line_index].line_state = SHARED_WRITE;
                sr_line_num--;
                sw_line_num++;
            }
        }
        else if(line_state != SHARED_WRITE)
            panic("line state error\n");
    }

}

void tlb_cu_init ()
{
    int i,j;
    for(i=0;i<MAXTHREADS;i++)
    {
        tlb_cu[i] = (struct tlb_cu_blk *) calloc (1, sizeof (struct tlb_cu_blk));
        tlb_cu[i]->recover_delay = 0;
        for(j=0;j<TLB_CU_SIZE;j++)
        {
            tlb_cu[i]->pw_evict[j].valid = 0;
            tlb_cu[i]->pw_evict[j].VPN = 0;
            tlb_cu[i]->sr_recovery[j].valid = 0;
            tlb_cu[i]->sr_recovery[j].VPN = 0;
            tlb_cu[i]->dir_recovery[j].valid = 0;
            tlb_cu[i]->dir_recovery[j].event = NULL;
        }
    }
}

int flush_data_mshr_check( int threadid, md_addr_t addr )
{
    struct mshr_t *mshr = cache_dl1[threadid]->mshr;
    int i;
    for (i = 0; i < MSHR_SIZE; i++)
    {
        if(mshr->mshrEntry[i].isValid && ((mshr->mshrEntry[i].addr >> MD_LOG_PAGE_SIZE) == (addr>> MD_LOG_PAGE_SIZE)))
            return 1;
    }
    return 0;
}

void tlb_flush_check(int threadid)
{
    int i;
    int match;
    match = 0;
    for(i=0;i<TLB_CU_SIZE;i++)
    {
        if (tlb_cu[threadid]->dir_recovery[i].valid)
        {
            match = flush_data_mshr_check(threadid, tlb_cu[threadid]->dir_recovery[i].event->addr);
            if( ( !match ) && ( (sim_cycle-tlb_cu[threadid]->dir_recovery[i].event->started) >= FLUSH_OVERHEAD ) )
            {
                tlb_cu[threadid]->dir_recovery[i].event->started = sim_cycle;
                tlb_cu[threadid]->dir_recovery[i].event->startCycle = sim_cycle;
                tlb_cu[threadid]->dir_recovery[i].event->processingDone = 0;
                data_cache_flush((tlb_cu[threadid]->dir_recovery[i].event->addr >> MD_LOG_PAGE_SIZE), threadid);
                scheduleThroughNetwork(tlb_cu[threadid]->dir_recovery[i].event, sim_cycle, meta_packet_size, -1);
                tlb_cu[threadid]->dir_recovery[i].event = NULL;
                tlb_cu[threadid]->dir_recovery[i].valid = 0;
            }
        }
    }
}

void tlb_cu_allocate( int threadid, int cmd, md_addr_t VPN, struct DIRECTORY_EVENT *event)
{
    int i;
    if ( cmd == SHR_TLB_WRITE_BACK )
    {
        for(i=0;i<TLB_CU_SIZE;i++)
        {
            if(!tlb_cu[threadid]->pw_evict[i].valid)
            {
                tlb_cu[threadid]->pw_evict[i].valid =1;
                tlb_cu[threadid]->pw_evict[i].VPN =VPN;
                break;
            }
        }
        if( i == TLB_CU_SIZE )
            panic("TLB_CU_SIZE to small\n");
    }
    else if ( cmd == SHR_TLB_SR_RECOVERY )
    {
        for(i=0;i<TLB_CU_SIZE;i++)
        {
            if(!tlb_cu[threadid]->sr_recovery[i].valid)
            {
                tlb_cu[threadid]->sr_recovery[i].valid =1;
                tlb_cu[threadid]->sr_recovery[i].VPN =VPN;
                break;
            }
        }
        if( i == TLB_CU_SIZE )
            panic("TLB_CU_SIZE to small\n");
    }
    else if ( (cmd==SHR_TLB_UPDATE_PW_ACK) || (cmd==SHR_TLB_RECOVERY_ACK) )
    {
        for(i=0;i<TLB_CU_SIZE;i++)
        {
            if(!tlb_cu[threadid]->dir_recovery[i].valid)
            {
                tlb_cu[threadid]->dir_recovery[i].valid =1;
                tlb_cu[threadid]->dir_recovery[i].event = event;
                break;
            }
        }
        if( i == TLB_CU_SIZE )
            panic("TLB_CU_SIZE to small\n");
    }
}

int tlb_cu_check(int threadid, md_addr_t addr, int cmd)
{
    struct cache_t *cp = dtlb[0];
    md_addr_t tag = CACHE_TAG (cp, addr);
    int match = -1;
    int i;
    if ( (cmd == DIR_TLB_UPDATE) || (cmd == SHR_TLB_WRITE_BACK) )
        for(i=0;i<TLB_CU_SIZE;i++)
        {
            if ( tlb_cu[threadid]->pw_evict[i].valid && (tlb_cu[threadid]->pw_evict[i].VPN == tag) )
            {
                match = i;
                break;
            }
        }
    else if ( cmd == DIR_TLB_RECOVERY )
        for(i=0;i<TLB_CU_SIZE;i++)
        {
            if ( tlb_cu[threadid]->sr_recovery[i].valid && (tlb_cu[threadid]->sr_recovery[i].VPN == tag) )
            {
                match = i;
                break;
            }
        }
    return match;
}

void tlb_mshr_init ()
{
    int i,j;
    for(i=0;i<MAXTHREADS;i++)
        for(j=0;j<TLB_MSHR_SIZE;j++)
        {
            tlb_mshr[i][j] = (struct tlb_mshr_entry *) calloc (1, sizeof (struct tlb_mshr_entry));
            tlb_mshr[i][j]->valid = 0;
            tlb_mshr[i][j]->dirty = 0;
            tlb_mshr[i][j]->evicting = 0;
            tlb_mshr[i][j]->VPN = 0;
            tlb_mshr[i][j]->head = NULL;
        }
}

void tlb_mshr_clear(int threadid, int index)
{
    tlb_mshr[threadid][index]->valid = 0;
    tlb_mshr[threadid][index]->dirty = 0;
    tlb_mshr[threadid][index]->evicting = 0;
    tlb_mshr[threadid][index]->VPN = 0;
    tlb_mshr[threadid][index]->head = NULL;
}

void tlb_mshr_allocate ( md_addr_t addr, int threadid, struct RUU_station *rs , int evicting)
{
    int i;
    struct cache_t *cp = dtlb[0];
    md_addr_t tag = CACHE_TAG (cp, addr);
    struct tlb_rs *new_rs = (struct tlb_rs *) calloc (1, sizeof (struct tlb_rs));
    new_rs->rs = rs;
    new_rs->next = NULL;
    for(i=0;i<TLB_MSHR_SIZE;i++)
    {
        if(!tlb_mshr[threadid][i]->valid)
        {
            tlb_mshr[threadid][i]->valid = 1;
            tlb_mshr[threadid][i]->dirty = 0;
            if (evicting != -1)
                tlb_mshr[threadid][i]->evicting = 1;
            tlb_mshr[threadid][i]->VPN = tag;
            tlb_mshr[threadid][i]->head = new_rs;
            break;
        }
    }
    if(i == TLB_MSHR_SIZE)
        panic("tlb_mshr_size to small\n");
}

int tlb_mshr_check ( md_addr_t addr, int threadid, struct RUU_station *rs )
{
    int match = -1;
    int i;
    struct cache_t *cp = dtlb[0];
    struct tlb_rs *temp;
    struct tlb_rs *new_rs;
    md_addr_t tag = CACHE_TAG (cp, addr);
    for(i=0;i<TLB_MSHR_SIZE;i++)
    {
        if(tlb_mshr[threadid][i]->valid && (tlb_mshr[threadid][i]->VPN == tag) )
        {
            if (!tlb_mshr[threadid][i]->head)
                panic("tlb_mshr_head_error\n");
            // add rs to list
            if (rs)
            {
                for(temp=tlb_mshr[threadid][i]->head;temp->next!=NULL;temp = temp->next);
                new_rs = (struct tlb_rs *) calloc (1, sizeof (struct tlb_rs));
                temp->next = new_rs;
                new_rs->rs = rs;
                new_rs->next = NULL;
            }
            match = i;
            break;
        }
    }
    return match;
}

int tlb_lock_check(int threadid)
{
    int i;
    int flushing = 0;
    for(i=0;i<TLB_CU_SIZE;i++)
        flushing = flushing || tlb_cu[threadid]->dir_recovery[i].valid || tlb_cu[threadid]->sr_recovery[i].valid ;
    return flushing;
}



void data_cache_flush( md_addr_t dtlb_tag, int target_tlb_id)
{
    struct cache_t *cp1;
    struct cache_blk_t *blk1;
    cp1 = cache_dl1[target_tlb_id];
    int i;
    md_addr_t addr;
    unsigned int packet_size;
    for(i=0;i<cp1->nsets;i++)
    {
        for (blk1 = cp1->sets[i].way_head; blk1; blk1 =blk1->way_next)
        {
            addr = ( blk1->tagid.tag << cp1->tag_shift ) + ( i << cp1->set_shift );
            if ( ( ( addr >> MD_LOG_PAGE_SIZE ) == dtlb_tag) && (blk1->status & CACHE_BLK_VALID) )
            {
                // write back; @ fanglei
                if ( (blk1->status & CACHE_BLK_DIRTY) && (actualProcess == 1) && !md_text_addr(blk1->addr, target_tlb_id) )
                {
                    if ((blk1->status & CACHE_BLK_DIRTY) && !(blk1->state == MESI_MODIFIED))
                        panic ("Cache Block should have been modified here");

                    struct DIRECTORY_EVENT *new_event = allocate_event(0);
                    if(new_event == NULL)
                        panic("Out of Virtual Memory");
                    new_event->operation = WRITE_BACK;
                    packet_size = data_packet_size;
                    new_event->src1 = target_tlb_id/mesh_size+MEM_LOC_SHIFT;
                    new_event->src2 = target_tlb_id%mesh_size;
#ifdef CENTRALIZED_L2
                    md_addr_t src = (blk1->addr >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
                    new_event->des1 = mesh_size;
                    new_event->des2 = src;
#else
                    md_addr_t src = (blk1->addr >> cache_dl2->set_shift) % numcontexts;
                    new_event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                    new_event->des2 = (src %mesh_size);
#endif
                    new_event->processingDone = 0;
                    new_event->startCycle = sim_cycle;
                    new_event->parent = NULL;
                    new_event->blk_dir = NULL;
                    new_event->childCount = 0;
                    new_event->tempID = target_tlb_id;
                    new_event->resend = 0;
                    new_event->blk1 = blk1;
                    new_event->addr = blk1->addr;
                    new_event->cp = cp1;
                    new_event->vp = NULL;
                    new_event->nbytes = cp1->bsize;
                    new_event->udata = NULL;
                    new_event->cmd = Write;
                    new_event->rs = NULL;
                    new_event->spec_mode = 0;
                    new_event->data_reply = 0;
                    /*go to network*/
                    scheduleThroughNetwork(new_event, sim_cycle, packet_size, 0);
                    dir_eventq_insert(new_event);
                }
                blk1->status = 0;
            }
        }
    }
    return;
}

/*hq: my counter */
#ifdef DCACHE_MSHR
struct DIRECTORY_EVENT *event_created = NULL;
#endif
#ifdef L1_STREAM_PREFETCHER
    void
l1init_sp ()
{
    int i, j;
    for(j=0;j<numcontexts;j++)
    {
        for (i = 0; i < L1_MISS_TABLE_SIZE; i++)
            l1miss_history_table[j].history[i] = 0;
        l1miss_history_table[j].mht_tail = 0;
        l1miss_history_table[j].mht_num = 0;

        for (i = 0; i < L1_STREAM_ENTRIES; i++)
        {
            l1stream_table[j][i].valid = 0;
            l1stream_table[j][i].addr = 0;
            l1stream_table[j][i].stride = 0;
            l1stream_table[j][i].remaining_prefetches = 0;
            l1stream_table[j][i].last_use = 0;
        }
    }
}

md_addr_t stream_match_addr(md_addr_t addr, int threadid)
{
    int i;
    int quotient;
    int remainder;
    int stride_distance;

    md_addr_t stream_head_addr;
    for (i = 0; i < L1_STREAM_ENTRIES; i++)
    {
        if (l1stream_table[threadid][i].stride!=0)
        {
            remainder = (l1stream_table[threadid][i].addr - addr) % l1stream_table[threadid][i].stride;
            quotient  = (l1stream_table[threadid][i].addr - addr) / l1stream_table[threadid][i].stride;
            stride_distance = STREAM_min_distance;
            if ( l1stream_table[threadid][i].valid && !remainder )
            {
                if ( (quotient >= 0) && (quotient < stride_distance) )
                {
                    l1stream_table[threadid][i].remaining_prefetches = stride_distance-quotient;
                    l1stream_table[threadid][i].last_use = sim_cycle;
                    l1stream_table[threadid][i].order = -1;
                    return l1stream_table[threadid][i].addr;
                }
                else if( quotient == -1 )
                {
                    l1stream_table[threadid][i].addr = addr;
                    l1stream_table[threadid][i].remaining_prefetches = stride_distance;
                    l1stream_table[threadid][i].last_use = sim_cycle;
                    l1stream_table[threadid][i].order = -1;
                    return l1stream_table[threadid][i].addr;
                }
                else
                {
                    quotient  = 0;
                    remainder = 0;
                    return 0;
                }
            }
        }
    }
    return 0;
}

    void
l1insert_sp (md_addr_t addr, int threadid)
{                               /* This function is called if L2 cache miss is discovered. */
    int i, num;
    int dist;
    int min_entry;
    md_addr_t stride_addr;
    int count = 0;

    if (l1miss_history_table[threadid].mht_num == 0)
    {                           /* Table is empty */
        l1miss_history_table[threadid].history[l1miss_history_table[threadid].mht_tail] = (addr & ~(cache_dl1[0]->bsize - 1));
        l1miss_history_table[threadid].mht_tail = (l1miss_history_table[threadid].mht_tail + 1) % L1_MISS_TABLE_SIZE;
        l1miss_history_table[threadid].mht_num++;
        return;
    }
    /* dist:distance to the last miss addr; stride_addr:last miss addr; the No. of last miss his entry */
    dist = (addr & ~(cache_dl1[0]->bsize - 1)) - l1miss_history_table[threadid].history[(l1miss_history_table[threadid].mht_tail + L1_MISS_TABLE_SIZE - 1) % L1_MISS_TABLE_SIZE];
    stride_addr = l1miss_history_table[threadid].history[(l1miss_history_table[threadid].mht_tail + L1_MISS_TABLE_SIZE - 1) % L1_MISS_TABLE_SIZE];
    min_entry = (l1miss_history_table[threadid].mht_tail + L1_MISS_TABLE_SIZE - 1) % L1_MISS_TABLE_SIZE;

    /* First calculate the minimum DELTA */
    for (num = 1, i = (l1miss_history_table[threadid].mht_tail + L1_MISS_TABLE_SIZE - 2) % L1_MISS_TABLE_SIZE; num < l1miss_history_table[threadid].mht_num; i = (i + L1_MISS_TABLE_SIZE - 1) % L1_MISS_TABLE_SIZE, num++)
    {
        int curr_dist = (addr & ~(cache_dl1[0]->bsize - 1)) - l1miss_history_table[threadid].history[i];

//        if ( ((abs (curr_dist) < abs (dist)) && (curr_dist != 0))
//          || (dist == 0) )
        if (abs (curr_dist) < abs (dist))
        {
            dist = curr_dist;
            stride_addr = l1miss_history_table[threadid].history[i];
            min_entry = i;
        }
    }

    i = min_entry;
    do
    {
        i = (i + L1_MISS_TABLE_SIZE - 1) % L1_MISS_TABLE_SIZE;
        int curr_dist = (stride_addr & ~(cache_dl1[0]->bsize - 1)) - l1miss_history_table[threadid].history[i];

        if (curr_dist == dist)
        {
            count++;
            stride_addr = l1miss_history_table[threadid].history[i];
        }
    }
//    while (i != l1miss_history_table[threadid].mht_tail);
    while (i != ( ( min_entry + 1 ) % L1_MISS_TABLE_SIZE ) );      // I think all the entries in the table should be checked. @ fanglei

    /* Now we can insert this miss entry into the miss history table */
    l1miss_history_table[threadid].history[l1miss_history_table[threadid].mht_tail] = (addr & ~(cache_dl1[0]->bsize - 1));
    l1miss_history_table[threadid].mht_tail = (l1miss_history_table[threadid].mht_tail + 1) % L1_MISS_TABLE_SIZE;
    if (l1miss_history_table[threadid].mht_num < L1_MISS_TABLE_SIZE)
        l1miss_history_table[threadid].mht_num++;

//    if (count > 1)
//    if (count > 0)      // it should be treated as a hit when the count is larger than 0; @ fanglei
    if ( (count > 0) && (dist != 0) )      // it should be treated as a hit when the count is larger than 0; @ fanglei
    {                           /* We hit a stride. Insert the stride into stream table. Also initiate the first prefetch.
                       If the table is completely full, use LRU algorithm to evict one entry. */
        int id = 0;

        if (l1stream_table[threadid][0].valid == 1)
        {
            int last_use = l1stream_table[threadid][0].last_use;

            id = 0;
            for (i = 1; i < L1_STREAM_ENTRIES; i++)/* find the proper place to insert */
            {
                if (l1stream_table[threadid][i].valid == 1)
                {
//                    if (last_use < l1stream_table[threadid][i].last_use)    // last or most recently used? @ fanglei
                    if (last_use > l1stream_table[threadid][i].last_use)    // last or most recently used? @ fanglei
                    {
                        last_use = l1stream_table[threadid][i].last_use;
                        id = i;
                    }
                }
                else
                {
                    id = i;
                    break;
                }
            }
        }

        /* Insert the stride in this entry */
        l1stream_table[threadid][id].valid = 1;
        l1stream_table[threadid][id].order = -1;
        l1stream_table[threadid][id].addr = addr & ~(cache_dl1[0]->bsize - 1);
        l1stream_table[threadid][id].stride = dist;
//        fprintf(dist_file, "%d\n", dist);
        int stride_distance;
        stride_distance = STREAM_min_distance;
        l1stream_table[threadid][id].remaining_prefetches = stride_distance;
//        l1stream_table[threadid][id].last_use = 2000000000;
        l1stream_table[threadid][id].last_use = 0;

        /* Now call a prefetch for this addr */
        if ( STREAM_min_En )
            l1launch_sp (addr, threadid, 0);
    }
}
#ifdef L1_DISTANCE
    void
l1launch_sp (md_addr_t addr, int threadid, int trigger)
{                               /* This function is called when access in L2 to pre-fetch data is discovered. */
    if ( !L1_prefetch_En )
        return;
    int i;

    for (i = 0; i < L1_STREAM_ENTRIES; i++)
    {
        if (l1stream_table[threadid][i].valid == 1 && l1stream_table[threadid][i].addr == (addr & ~(cache_dl1[0]->bsize - 1)))
        {
            int load_lat = 0;
            byte_t *l1cache_line = calloc (cache_dl1[0]->bsize, sizeof (byte_t));
            int j;
            for(j=0;j<STREAM_min_distance;j++)
            {
                if (l1stream_table[threadid][i].remaining_prefetches == 0)
                    break;
                l1stream_table[threadid][i].addr = l1stream_table[threadid][i].addr + l1stream_table[threadid][i].stride;
                l1stream_table[threadid][i].remaining_prefetches--;
                l1stream_table[threadid][i].last_use = sim_cycle;

//                if (l1stream_table[threadid][i].remaining_prefetches == 1)
//                {
//                    l1stream_table[threadid][i].valid = 0;
//                }

                /* Tell the system that this call at this time is a prefetch, so stream
                 ** prefetch must not trigger.*/
                dl1_prefetch_active = 1;
                if(l1stream_table[threadid][i].stride != 0)
                {
                    md_addr_t addr_prefetch = (l1stream_table[threadid][i].addr & ~(cache_dl1[0]->bsize - 1));
                    md_addr_t tag = CACHE_TAG (cache_dl1[threadid], addr_prefetch);
                    md_addr_t set = CACHE_SET (cache_dl1[threadid], addr_prefetch);
                    struct cache_blk_t *blk;
                    int cache_hit_flag = 0;

                    for (blk = cache_dl1[threadid]->sets[set].way_head; blk; blk = blk->way_next)
                    {
                        if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
                            cache_hit_flag = 1;
                    }
                    if(cache_hit_flag == 0)
                    {
                        // int src = (addr_prefetch >> cache_dl2->set_shift) % numcontexts;
                        // FIXME: the size of L2 cache can not be less than 16 pages @ fl
                        int src;
                        int matchnum;
                        matchnum = MSHR_block_check(cache_dl1[threadid]->mshr, addr_prefetch, cache_dl1[threadid]->set_shift);

                        // prefetch can be dropped during launch; @ fanglei
                        if ( !matchnum && (cache_dl1[threadid]->mshr->freeEntries > 1))
                        {
                            struct DIRECTORY_EVENT *new_event = allocate_event(0);
                            if(new_event == NULL)       panic("Out of Virtual Memory");
                            new_event->stride = l1stream_table[threadid][i].stride; // stride @ fanglei
                            new_event->order = j;
                            new_event->stream_entry_index = i;
                            new_event->trigger = trigger;
                            new_event->data_reply = 0;
                            new_event->req_time = 0;
                            new_event->pending_flag = 0;
                            new_event->op = LDQ;
                            new_event->isPrefetch = 0;
                            new_event->operation = MISS_READ;

                            new_event->src1 = threadid/mesh_size+MEM_LOC_SHIFT;
                            /* mem_loc_shift is for memory model because memory controller are allocated at two side of the chip */
                            new_event->src2 = threadid%mesh_size;
#ifdef CENTRALIZED_L2
                        //the centralized L2 has CENTL2_BANK_NUM banks, and each of them has one NI (network interface).
                            src = (addr_prefetch >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
                            new_event->des1 = mesh_size ;    //This indicates that its a L2 bank access, the location of this L2 bank NI is on the bottom of the chip, (In fact, it can be anywhere)
                            new_event->des2 = src ; //this indicate the bank id
#else
                            src = (addr_prefetch >> cache_dl2->set_shift) % numcontexts;
							new_event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                            new_event->des2 = (src %mesh_size);
#endif
                            new_event->processingDone = 0;
                            new_event->startCycle = sim_cycle;
                            new_event->parent = NULL;
                            new_event->tempID = threadid;
                            new_event->resend = 0;
                            new_event->blk_dir = NULL;
                            new_event->cp = cache_dl1[threadid];
                            new_event->addr = addr_prefetch;
                            new_event->vp = NULL;
                            new_event->nbytes = cache_dl1[threadid]->bsize;
                            new_event->udata = NULL;
//                          new_event->cmd = Write;
                            new_event->cmd = Read;      // I think prefetch is read type; @ fanglei
                            new_event->rs = NULL;
                            new_event->started = sim_cycle;
                            new_event->spec_mode = 0;
                            new_event->L2miss_flag = 0;
                            if(l1stream_table[threadid][i].remaining_prefetches == STREAM_min_distance - 1)
                            {
                                new_event->prefetch_next = 2;   /* first prefetch packet */
                                total_L1_first_prefetch ++;
                            }
                            else
                            {
                                new_event->prefetch_next = 4;
                                total_L1_sec_prefetch ++;
                            }
                            // P_G the time stamp when the prefetch request is generated; @ fanglei
                            new_event->conf_bit = -1;
                            new_event->input_buffer_full = 0;

                            scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, 0);

                            dir_eventq_insert(new_event);
                            event_created = new_event;
                            MSHRLookup(cache_dl1[threadid]->mshr, addr_prefetch, WAIT_TIME, 0, NULL);
                        }
                    }
                    if ( l1stream_table[threadid][i].remaining_prefetches <0 | l1stream_table[threadid][i].remaining_prefetches > STREAM_min_distance )
                            panic(" remaining_prefetches error ");
                    if (l1stream_table[threadid][i].remaining_prefetches == 0)
                        break;
                }
            }

            dl1_prefetch_active = 0;
            free (l1cache_line);
            break;
        }
    }
}
#else   // L1_DISTANCE
    void
l1launch_sp (md_addr_t addr, int threadid, int trigger)
{                               /* This function is called when access in L2 to pre-fetch data is discovered. */
    if ( !L1_prefetch_En )
        return;
    int i;

    for (i = 0; i < L1_STREAM_ENTRIES; i++)
    {
        if (l1stream_table[threadid][i].valid == 1 && l1stream_table[threadid][i].addr == (addr & ~(cache_dl1[0]->bsize - 1)))
        {
            int load_lat = 0;
            byte_t *l1cache_line = calloc (cache_dl1[0]->bsize, sizeof (byte_t));

            l1stream_table[threadid][i].addr = l1stream_table[threadid][i].addr + l1stream_table[threadid][i].stride;
            l1stream_table[threadid][i].remaining_prefetches--;
            l1stream_table[threadid][i].last_use = sim_cycle;

            if (l1stream_table[threadid][i].remaining_prefetches == 1)
            {
                l1stream_table[threadid][i].valid = 0;
            }

            /* Tell the system that this call at this time is a prefetch, so stream
             ** prefetch must not trigger.*/
            dl1_prefetch_active = 1;
            if(l1stream_table[threadid][i].stride != 0)
            {
                md_addr_t addr_prefetch = (l1stream_table[threadid][i].addr & ~(cache_dl1[0]->bsize - 1));
                md_addr_t tag = CACHE_TAG (cache_dl1[threadid], addr_prefetch);
                md_addr_t set = CACHE_SET (cache_dl1[threadid], addr_prefetch);
                struct cache_blk_t *blk;
                int cache_hit_flag = 0;

                for (blk = cache_dl1[threadid]->sets[set].way_head; blk; blk = blk->way_next)
                {
                    if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
                        cache_hit_flag = 1;
                }
                if(cache_hit_flag == 0)
                {
                    // int src = (addr_prefetch >> cache_dl2->set_shift) % numcontexts;
                    // FIXME: the size of L2 cache can not be less than 16 pages @ fl
                    int src;
//                  total_L1_prefetch ++;

                    int matchnum;
                    matchnum = MSHR_block_check(cache_dl1[threadid]->mshr, addr_prefetch, cache_dl1[threadid]->set_shift);
                    if(!matchnum && (cache_dl1[threadid]->mshr->freeEntries > 1))
                    {
                        struct DIRECTORY_EVENT *new_event = allocate_event(0);
                        if(new_event == NULL)       panic("Out of Virtual Memory");
                        total_L1_prefetch ++;
                        level1_prefetch_num ++;
                        new_event->data_reply = 0;
                        new_event->req_time = 0;
                        new_event->pending_flag = 0;
                        new_event->op = LDQ;
                        new_event->isPrefetch = 0;
                        new_event->operation = MISS_READ;

                        new_event->src1 = threadid/mesh_size+MEM_LOC_SHIFT;
                        /* mem_loc_shift is for memory model because memory controller are allocated at two side of the chip */
                        new_event->src2 = threadid%mesh_size;
#ifdef CENTRALIZED_L2
                        //the centralized L2 has CENTL2_BANK_NUM banks, and each of them has one NI (network interface).
                        src = (addr_prefetch >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
                        new_event->des1 = mesh_size ;    //This indicates that its a L2 bank access, the location of this L2 bank NI is on the bottom of the chip, (In fact, it can be anywhere)
                        new_event->des2 = src; //this indicate the bank id
#else
                        src = (addr_prefetch >> MD_LOGE_PAGE_SIZE) % numcontexts;
						new_event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                        new_event->des2 = (src %mesh_size);
#endif
                        new_event->processingDone = 0;
                        new_event->startCycle = sim_cycle;
                        new_event->parent = NULL;
                        new_event->tempID = threadid;
                        new_event->resend = 0;
                        new_event->blk_dir = NULL;
                        new_event->cp = cache_dl1[threadid];
                        new_event->addr = addr_prefetch;
                        new_event->vp = NULL;
                        new_event->nbytes = cache_dl1[threadid]->bsize;
                        new_event->udata = NULL;
//                      new_event->cmd = Write;
                        new_event->cmd = Read;      // I think prefetch is read type; @ fanglei
                        new_event->rs = NULL;
                        new_event->started = sim_cycle;
                        new_event->spec_mode = 0;
                        new_event->L2miss_flag = 0;
                        if(l1stream_table[threadid][i].remaining_prefetches == STREAM_min_distance - 1)
                        {
                            new_event->prefetch_next = 2;   /* first prefetch packet */
                            total_L1_first_prefetch ++;
                        }
                        else
                        {
                            new_event->prefetch_next = 4;
                            total_L1_sec_prefetch ++;
                        }
                        // P_G the time stamp when the prefetch request is generated; @ fanglei
                        new_event->conf_bit = -1;
                        new_event->input_buffer_full = 0;

                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, 0);

                        dir_eventq_insert(new_event);
                        event_created = new_event;
                        MSHRLookup(cache_dl1[threadid]->mshr, addr_prefetch, WAIT_TIME, 0, NULL);
                    }
                }
            }

            dl1_prefetch_active = 0;
            free (l1cache_line);
            break;
        }
    }
}
#endif  // L1_DISTANCE
#endif  // L1_STREAM_PREFETCHER
#ifdef STREAM_PREFETCHER
    void
init_sp ()
{
    int i;

    for (i = 0; i < L2_MISS_TABLE_SIZE; i++)
        miss_history_table.history[i] = 0;
    miss_history_table.mht_tail = 0;
    miss_history_table.mht_num = 0;

    for (i = 0; i < L2_STREAM_ENTRIES; i++)
    {
        stream_table[i].valid = 0;
        stream_table[i].addr = 0;
        stream_table[i].stride = 0;
        stream_table[i].remaining_prefetches = 0;
        stream_table[i].last_use = 0;
    }
}

    void
insert_sp (md_addr_t addr, int threadid)
{                               /* This function is called if L2 cache miss is discovered. */
    int i, num;
    int dist;
    int min_entry;
    md_addr_t stride_addr;
    int count = 0;

    if (miss_history_table.mht_num == 0)
    {                           /* Table is empty */
        miss_history_table.history[miss_history_table.mht_tail] = (addr & ~(cache_dl2->bsize - 1));
        miss_history_table.mht_tail = (miss_history_table.mht_tail + 1) % L2_MISS_TABLE_SIZE;
        miss_history_table.mht_num++;
        return;
    }

    dist = (addr & ~(cache_dl2->bsize - 1)) - miss_history_table.history[(miss_history_table.mht_tail + L2_MISS_TABLE_SIZE - 1) % L2_MISS_TABLE_SIZE];
    stride_addr = miss_history_table.history[(miss_history_table.mht_tail + L2_MISS_TABLE_SIZE - 1) % L2_MISS_TABLE_SIZE];
    min_entry = (miss_history_table.mht_tail + L2_MISS_TABLE_SIZE - 1) % L2_MISS_TABLE_SIZE;

    /* First calculate the minimum DELTA */
    for (num = 1, i = (miss_history_table.mht_tail + L2_MISS_TABLE_SIZE - 2) % L2_MISS_TABLE_SIZE; num < miss_history_table.mht_num; i = (i + L2_MISS_TABLE_SIZE - 1) % L2_MISS_TABLE_SIZE, num++)
    {
        int curr_dist = (addr & ~(cache_dl2->bsize - 1)) - miss_history_table.history[i];

        if (abs (curr_dist) < abs (dist))
        {
            dist = curr_dist;
            stride_addr = miss_history_table.history[i];
            min_entry = i;
        }
    }

    i = min_entry;
    do
    {
        i = (i + L2_MISS_TABLE_SIZE - 1) % L2_MISS_TABLE_SIZE;
        int curr_dist = (stride_addr & ~(cache_dl2->bsize - 1)) - miss_history_table.history[i];

        if (curr_dist == dist)
        {
            count++;
            stride_addr = miss_history_table.history[i];
        }
    }
//    while (i != miss_history_table.mht_tail);
    while (i != ( ( min_entry + 1 ) % L2_MISS_TABLE_SIZE ) );      // I think all the entries in the table should be checked. @ fanglei

    /* Now we can insert this miss entry into the miss history table */
    miss_history_table.history[miss_history_table.mht_tail] = (addr & ~(cache_dl2->bsize - 1));
    miss_history_table.mht_tail = (miss_history_table.mht_tail + 1) % L2_MISS_TABLE_SIZE;
    if (miss_history_table.mht_num < L2_MISS_TABLE_SIZE)
        miss_history_table.mht_num++;

    if (count > 1)
//    if (count > 0)      // it should be treated as a hit when the count is larger than 0; @ fanglei
    {                           /* We hit a stride. Insert the stride into stream table. Also initiate the first prefetch.
                       If the table is completely full, use LRU algorithm to evict one entry. */
        int id = 0;

        if (stream_table[0].valid == 1)
        {
            int last_use = stream_table[0].last_use;

            id = 0;
            for (i = 1; i < L2_STREAM_ENTRIES; i++)
            {
                if (stream_table[i].valid == 1)
                {
                    if (last_use < stream_table[i].last_use)    // last or most recently used? @ fanglei
                    {
                        last_use = stream_table[i].last_use;
                        id = i;
                    }
                }
                else
                {
                    id = i;
                    break;
                }
            }
        }

        /* Insert the stride in this entry */
        stream_table[id].valid = 1;
        stream_table[id].addr = addr & ~(cache_dl2->bsize - 1);
        stream_table[id].stride = dist;
        stream_table[id].remaining_prefetches = STREAM_L2_distance;
        stream_table[id].last_use = 2000000000;

        /* Now call a prefetch for this addr */
        launch_sp (id, threadid);
    }
}

    void
launch_sp (int id, int threadid)
{                               /* This function is called when access in L2 to pre-fetch data is discovered. */
    if ( !L2_prefetch_En )
        return;
    int i = id;

    if (stream_table[i].valid == 1)
    {
        for (; stream_table[i].remaining_prefetches; stream_table[i].remaining_prefetches --)
        {
            int load_lat = 0;
            byte_t *l1cache_line = calloc (cache_dl2->bsize, sizeof (byte_t));

            stream_table[i].addr = stream_table[i].addr + stream_table[i].stride;
            stream_table[i].last_use = sim_cycle;


            /* Tell the system that this call at this time is a prefetch, so stream
             ** prefetch must not trigger.*/
            dl2_prefetch_active = 1;
            dl2_prefetch_id = i;
            struct DIRECTORY_EVENT *event = allocate_event(0);
            if(event == NULL)       panic("Out of Virtual Memory");
            event->conf_bit = -1;
            event->pending_flag = 0;
            event->addr = (stream_table[i].addr & ~(cache_dl2->bsize - 1));
            event->prefetch_next = 3;
            event->started = sim_cycle;
            event->operation = L2_PREFETCH;
            event->startCycle = sim_cycle;
            event->op = 0;
            event->cp = cache_dl1[0];
            event->tempID = threadid;
            event->nbytes = cache_dl1[0]->bsize;
            event->udata = NULL;
            event->rs = NULL;
            event->spec_mode = 0;
            event->input_buffer_full = 0;
            event->src1 = threadid/mesh_size + MEM_LOC_SHIFT;
            event->src2 = threadid%mesh_size;
            long src;
#ifdef CENTRALIZED_L2
            //the centralized L2 has CENTL2_BANK_NUM banks, and each of them has one NI (network interface).
            src = (event->addr >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
            event->des1 = mesh_size ;    //This indicates that its a L2 bank access, the location of this L2 bank NI is on the bottom of the chip, (In fact, it can be anywhere)
            event->des2 = src; //this indicate the bank id
#else
            //src = (event->addr >> cache_dl2->set_shift) % numcontexts;
            src = (event->addr >> cache_dl2->set_shift) % numcontexts;
            event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
            event->des2 = (src %mesh_size);
#endif
            event->data_reply = 0;
            event->processingDone = 0;
            event->req_time = 0;
            dir_eventq_insert(event);

            dl2_prefetch_active = 0;
            free (l1cache_line);
        }
    }
    stream_table[i].remaining_prefetches = 1;       // ? @ fanglei
}

#endif


extern counter_t totalL2WriteReqHits;
extern counter_t totalL2WriteReqNoSharer;
extern counter_t totalL2WriteReqDirty;
extern counter_t delayL2WriteReqDirty;
extern counter_t totalL2WriteReqShared;
extern counter_t delayL2WriteReqShared;

extern counter_t totalL2ReadReqHits;
extern counter_t totalL2ReadReqNoSharer;
extern counter_t totalL2ReadReqDirty;
extern counter_t delayL2ReadReqDirty;

extern counter_t totalL2ReadReqShared;
extern counter_t totalL2WriteReqSharedWT;
extern counter_t totalL2WriteReqDirtyWT;
extern counter_t totalL2ReadReqDirtyWT;

extern counter_t totalL1LinePrefUse;
extern counter_t totalL1LinePrefNoUse;
extern counter_t totalL1LineReadUse;
extern counter_t totalL1LineReadNoUse;
extern counter_t totalL1LineWriteUse;
extern counter_t totalL1LineWriteNoUse;

extern counter_t totalL1LineExlUsed;
extern counter_t totalL1LineExlInv;
extern counter_t totalL1LineExlDrop;

extern counter_t    totalL2ReqPrimMiss;
extern counter_t    totalL2ReqSecMiss;
extern counter_t    totalL2ReqHit;
extern counter_t    totalL2ReqInTrans;
extern counter_t    totalL2OwnReqInTrans;
extern counter_t    totalL2ReqInInv;

extern counter_t    delayL2ReqPrimMiss;
extern counter_t    delayL2ReqSecMiss;
extern counter_t    delayL2ReqHit;
extern counter_t    delayL2ReqInTrans;
extern counter_t    delayL2OwnReqInTrans;
extern counter_t    delayL2ReqInInv;

extern counter_t    totalWriteReq;
extern counter_t    totalWriteReqInv;
extern counter_t    totalWriteDelay;
extern counter_t    totalWriteInvDelay;
extern counter_t    totalUpgradeReq;
counter_t   totalUpgradeMiss;
extern counter_t    totalUpgradeReqInv;
extern counter_t    totalUpgradeDelay;
extern counter_t    totalUpgradeInvDelay;
counter_t m_update_miss[MAXTHREADS];

extern counter_t countNonAllocReadEarly;
counter_t sharer_distr[MAXTHREADS];
counter_t total_corner_packets, total_neighbor_packets, total_far_packets;

extern counter_t invalidation_replay[MAXTHREADS];

void sendLSQInv(md_addr_t addr, int threadid)
{
    if (collect_stats)
    {
        int cx, dx;
        extern int LSQ_size;
        context *current = thecontexts[threadid];
        md_addr_t mask;

        mask = ~cache_dl1[threadid]->blk_mask;

        for (cx = current->LSQ_head, dx = 0; dx != current->LSQ_num; cx = (cx + 1) % LSQ_size, dx++)
        {
            if ((MD_OP_FLAGS (current->LSQ[cx].op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD) && (current->LSQ[cx].addr & mask) == (addr & mask) && !current->LSQ[cx].spec_mode
                &&!current->LSQ[cx].isPrefetch && current->LSQ[cx].issued)
            {
#ifdef SEQUENTIAL_CONSISTENCY
                fixSpecState (current->LSQ[cx].prod[1]->index, threadid);
                current->regs = current->LSQ[cx].prod[1]->backupReg;
                seqConsistancyReplay (current->LSQ[cx].prod[1]->index, threadid);
                invalidation_replay[threadid]++;
                break;
#endif
#ifdef RELAXED_CONSISTENCY
                current->LSQ[cx].invalidationReceived = 1;
#endif
            }
        }
    }
}

/*      DIR FIFO IMPLEMENTATION         */
void reset_ports()
{
    int i;
    for(i = 0; i < MAXTHREADS+mesh_size*2; i++)
    {
        dir_fifo_portuse[i] = 0;
        l1_fifo_portuse[i] = 0;
        tshr_fifo_portuse[i] = 0;
    }
}

extern  short m_shSQSize;
int dir_fifo_enqueue(struct DIRECTORY_EVENT *event, int type)
{
    int threadid = event->des1*mesh_size + event->des2;
	int tID = (event->des1-MEM_LOC_SHIFT)*mesh_size + event->des2;
    if(event->operation == INV_MSG_READ || event->operation == INV_MSG_WRITE || event->operation == ACK_DIR_IL1 || event->operation == ACK_DIR_READ_SHARED
    || event->operation == ACK_DIR_READ_EXCLUSIVE || event->operation == ACK_DIR_WRITE
    || event->operation == ACK_DIR_WRITEUPDATE || event->operation == FINAL_INV_ACK
    || event->operation == NACK || event->operation == FAIL
      )
    {
        if(l1_fifo_num[threadid] >= DIR_FIFO_SIZE)
            panic("L1 cache FIFO is full");
        if(type == 0 && l1_fifo_num[threadid] >= dir_fifo_size)
        {
            if(l1_fifo_num[threadid] >= dir_fifo_size && !last_L1_fifo_full[threadid])
            {
                L1_fifo_full ++;
                last_L1_fifo_full[threadid] = sim_cycle;
            }
            return 0;
        }
        if(last_L1_fifo_full[threadid])
        {
            Stall_L1_fifo += sim_cycle - last_L1_fifo_full[threadid];
            last_L1_fifo_full[threadid] = 0;
        }
        if(event->operation == ACK_DIR_IL1) event->when = sim_cycle + cache_il1[tID]->hit_latency;
        else                    event->when = sim_cycle + cache_dl1[tID]->hit_latency;
        l1_fifo[threadid][l1_fifo_tail[threadid]] = event;
        l1_fifo_tail[threadid] = (l1_fifo_tail[threadid]+1)%DIR_FIFO_SIZE;
        l1_fifo_num[threadid]++;
        return 1;
    }
    else
    {
        if(event->flip_flag)
        {
            if(tshr_fifo_num[threadid] >= TSHR_FIFO_SIZE)
                panic("TSHR FIFO is full");
            event->when = sim_cycle+RETRY_TIME;
            tshr_fifo[threadid][tshr_fifo_tail[threadid]] = event;
            tshr_fifo_tail[threadid] = (tshr_fifo_tail[threadid]+1)%TSHR_FIFO_SIZE;
            tshr_fifo_num[threadid]++;
            return 1;
        }
        if(dir_fifo_num[threadid] >= DIR_FIFO_SIZE)
            panic("DIR FIFO is full");
        if( (event->operation == SHR_TLB_MISS         ) ||
            (event->operation == SHR_TLB_WRITE_BACK   ) ||
            (event->operation == SHR_TLB_SR_RECOVERY  ) ||
            (event->operation == SHR_TLB_UPDATE_PR_ACK) ||
            (event->operation == SHR_TLB_UPDATE_PW_ACK) ||
            (event->operation == SHR_TLB_RECOVERY_ACK ) ||
            (event->operation == DIR_TLB_REFILL       ) ||
            (event->operation == DIR_TLB_UPDATE       ) ||
            (event->operation == DIR_TLB_RECOVERY     ) ||
            (event->operation == DIR_TLB_REFILL_WAIT  ) ||
            (event->operation == DIR_TLB_UPDATE_WAIT  ) ||
            (event->operation == DIR_TLB_RECOVERY_WAIT)
            )
        {
            event->when = sim_cycle;
            dir_fifo[threadid][dir_fifo_tail[threadid]] = event;
            dir_fifo_tail[threadid] = (dir_fifo_tail[threadid]+1)%DIR_FIFO_SIZE;
            dir_fifo_num[threadid]++;
            return 1;
        }
        if(type == 0 && dir_fifo_num[threadid] >= dir_fifo_size)
        {
            if(dir_fifo_num[threadid] >= dir_fifo_size && !last_Dir_fifo_full[threadid])
            {
                Dir_fifo_full ++;
                last_Dir_fifo_full[threadid] = sim_cycle;
            }
            return 0;
        }
        if(last_Dir_fifo_full[threadid])
        {
            Stall_dir_fifo += sim_cycle - last_Dir_fifo_full[threadid];
            last_Dir_fifo_full[threadid] = 0;
        }
        //event->when = sim_cycle + cache_dl2_lat;
        if(event->no_L2_delay)
        {
            event->when = sim_cycle;
            event->no_L2_delay = 0;
        }
        else if(event->operation != MEM_READ_REQ && event->operation != MEM_READ_REPLY && event->operation != WAIT_MEM_READ_N_WAIT )        // I think ACK_MSG_WRITEUPDATE should not wait for cache_dl2_lat @fanglei
        {
//            event->when = sim_cycle + cache_dl2_lat * L2Freq[tID].latency_factor ;
            event->when = sim_cycle + cache_dl2_lat;
            if ( mb_pain_en && (mb_pain_count > 0) && (event->operation == MISS_READ || event->operation == WRITE_UPDATE || event->operation == MISS_WRITE) )
            {
                event->when++;
                mb_pain_delay_num ++;
                mb_pain_count--;
            }
        }
        else
            event->when = sim_cycle;
        dir_fifo[threadid][dir_fifo_tail[threadid]] = event;
        dir_fifo_tail[threadid] = (dir_fifo_tail[threadid]+1)%DIR_FIFO_SIZE;
        dir_fifo_num[threadid]++;
        return 1;
    }
}

void dir_fifo_dequeue()
{
    int i, j;
    int PPP_indication;
//    int P_Drop_indication;

    /* L2 and dir queue */
    for(i = 0; i < numcontexts+mesh_size*2; i++)
    {
        if(tshr_fifo_num[i] <= 0 || tshr_fifo_portuse[i] >= DIR_FIFO_PORTS)
            continue;
        while(1)
        {
            if(tshr_fifo[i][tshr_fifo_head[i]]->when > sim_cycle)
                break;
            if(dir_operation(tshr_fifo[i][tshr_fifo_head[i]], 0) == 0) /* 0 means it's a normal cache access */
                break;
            int tlb_transfer;
            tlb_transfer = tlb_transfer_check(tshr_fifo[i][tshr_fifo_head[i]]);
            if ( !tlb_transfer )
                tshr_fifo_portuse[i]++;
            tshr_fifo_head[i] = (tshr_fifo_head[i]+1)%TSHR_FIFO_SIZE;
            tshr_fifo_num[i]--;
            if(tshr_fifo_portuse[i] >= DIR_FIFO_PORTS || tshr_fifo_num[i] <= 0)
                break;
        }
    }

    /* L2 and dir queue */
    for(i = 0; i < numcontexts+mesh_size*2; i++)
    {
        if(dir_fifo_num[i] <= 0 || dir_fifo_portuse[i] >= DIR_FIFO_PORTS)
            continue;
        while(1)
        {
            if(dir_fifo[i][dir_fifo_head[i]]->when > sim_cycle)
                break;
            
            int Q_hit_num = read_Q_hit_num + write_Q_hit_num;
            
            if(dir_operation(dir_fifo[i][dir_fifo_head[i]], 0) == 0) /* 0 means it's a normal cache access */
                break;

            if((Q_hit_num<(read_Q_hit_num+write_Q_hit_num)) && mb_pain_en)  // fix me, dl2 miss will cause two pain; @ fanglei 
            {
                dir_fifo_portuse[i]++;
                mb_pain_portuse_num++;
            }

            int tlb_transfer;
            tlb_transfer = tlb_transfer_check(dir_fifo[i][dir_fifo_head[i]]);
            if ( !tlb_transfer )
                dir_fifo_portuse[i]++;
            dir_fifo_head[i] = (dir_fifo_head[i]+1)%DIR_FIFO_SIZE;
            dir_fifo_num[i]--;
            if(dir_fifo_portuse[i] >= DIR_FIFO_PORTS || dir_fifo_num[i] <= 0)
                break;
        }
    }

    /* L1 queues */
    for(i = 0; i < numcontexts+mesh_size*2; i++)
    {
        extern struct res_pool *fu_pool;

        if(l1_fifo_num[i] <= 0 || l1_fifo_portuse[i] >= DIR_FIFO_PORTS)
            continue;
        while(1)
        {
            if(l1_fifo[i][l1_fifo_head[i]]->when > sim_cycle)
                break;
            if(dir_operation(l1_fifo[i][l1_fifo_head[i]], 0) == 0)
                break;
            l1_fifo_portuse[i]++;
            l1_fifo_head[i] = (l1_fifo_head[i]+1)%DIR_FIFO_SIZE;
            l1_fifo_num[i]--;
            if(l1_fifo_portuse[i] >= DIR_FIFO_PORTS || l1_fifo_num[i] <= 0)
                break;
        }
    }
}
/*      DIR FIFO IMPLEMENTATION ENDS        */



void dir_eventq_insert(struct DIRECTORY_EVENT *event)     // event queue directory version, put dir event in queue
{
    struct DIRECTORY_EVENT *ev, *prev;

    for (prev = NULL, ev = dir_event_queue; ev && ((ev->when) <= (event->when)); prev = ev, ev = ev->next);
    if (prev)
    {
        event->next = ev;
        prev->next = event;
    }
    else
    {
        event->next = dir_event_queue;
        dir_event_queue = event;
    }
}


#ifdef CONF_RES_RESEND
/* this queue is the sending event queue at the request and reply event queue at the home node */
void queue_insert(struct DIRECTORY_EVENT *event, struct QUEUE_EVENT *queue)
{ /* queue_insert */
    int i = 0;
    if(queue->free_Entries == 0)
        panic("There is no free entry to insert in sending message queue");

    for(i=0; i<QUEUE_SIZE; i++)
    {
        if(!queue->Queue_entry[i].isvalid)
        {
            queue->Queue_entry[i].isvalid = 1;
            queue->Queue_entry[i].event = event;
            queue->free_Entries --;
            return;
        }
    }

}

void free_queue(struct DIRECTORY_EVENT *event, struct QUEUE_EVENT *queue)
{
    int i = 0;
    if(!queue) return;
    for(i=0; i<QUEUE_SIZE; i++)
    {
        if(queue->Queue_entry[i].isvalid && (queue->Queue_entry[i].event->addr == event->addr) && (queue->Queue_entry[i].event->started == event->started)
                && (queue->Queue_entry[i].event->missNo == event->missNo))
        {
            queue->Queue_entry[i].isvalid = 0;
            queue->free_Entries ++;
            break;
        }
    }

    if(queue->free_Entries > QUEUE_SIZE)
        panic("QUEUE SIZE EXCEEDS\n");

}

int queue_check(struct DIRECTORY_EVENT *event, struct QUEUE_EVENT *queue)
{
    int i = 0;
    if(!queue) return 0;
    for(i=0; i<QUEUE_SIZE; i++)
    {
        if(queue->Queue_entry[i].isvalid && (queue->Queue_entry[i].event->addr == event->addr) && (queue->Queue_entry[i].event->started == event->started)
                && (queue->Queue_entry[i].event->missNo == event->missNo))
            return 1;
    }
    return 0;
}
#endif

void eventq_update(struct RUU_station *rs, tick_t ready)   /*directory event queue version */
{
    struct RS_link *ev, *prev, *new_ev=NULL;

    if(rs == NULL || event_queue == NULL)
    {
        return;
    }

    for (prev=NULL, ev=event_queue; (ev->next!=NULL); prev=ev, ev=ev->next)
    {
        if((ev->rs == rs) && (ev->tag == rs->tag))
        {
            break;
        }
    }

    if ((ev->rs == rs) && (ev->tag == rs->tag))                     //mwr: FIXED = to ==
    {
        if (prev)
        {
            prev->next = ev->next;
        }
        else
        {
            event_queue = ev->next;
        }
        new_ev = ev;
        new_ev->x.when = ready;

        for (prev=NULL, ev=event_queue; ev && ev->x.when < ready; prev=ev, ev=ev->next);
        if (prev)
        {
            new_ev->next = prev->next;
            prev->next = new_ev;
        }
        else
        {
            new_ev->next = event_queue;
            event_queue = new_ev;
        }
    }
}
#ifdef TSHRn
void initTSHR(struct tshr_t *tshr, int flag)
{
    int i;
    tshr->tshrSIZE = TSHR_SIZE;
    tshr->freeEntries = TSHR_SIZE;
    for (i = 0; i < TSHR_SIZE; i++)
    {
        tshr->tshrEntry[i].id = i;
        tshr->tshrEntry[i].isValid = 0;
        tshr->tshrEntry[i].event = NULL;
        tshr->tshrEntry[i].addr = 0;
    }
}
void TSHRInsert(struct tshr_t *tshr, struct DIRECTORY_EVENT *event, md_addr_t addr)
{
    int i;

    if(tshr->freeEntries == 0)
        panic("There is no free entry to insert in TSHR");

    for (i = 0; i < TSHR_SIZE; i++)
    {
        if(!tshr->mshrEntry[i].isValid)
        {
            tshr->tshrEntry[i].isValid = 1;
            tshr->tshrEntry[i].event = event;
            tshr->tshrEntry[i].pending_addr = (addr & ~cache_dl2->bsize);
            tshr->freeEntries--;
            return;
        }
    }
    panic("There is no free entry to insert in MSHR");
}
void TSHRWakeup(struct tshr_t *tshr, int wakeup_addr)
{
    int i;

    for (i = 0; i < TSHR_SIZE; i++)
    {
        if(tshr->mshrEntry[i].isValid && (tshr->tshrEntry[i].pending_addr == (wakeup_addr & ~cache_dl2->bsize)))
        {
            if(dir_operation(tshr->tshrEntry[i].event))
            {
                tshr->tshrEntry[i].isValid = 1;
                tshr->tshrEntry[i].event = NULL;
                tshr->tshrEntry[i].pending_addr = 0;
                tshr->freeEntries++;
            }
            else
                panic("can not wakeup becuase the blk is still in transient state");
            return;
        }
    }
    panic("There is no match entry in TSHR");

}
#endif

#ifdef DCACHE_MSHR
//struct DIRECTORY_EVENT *event_created = NULL;
void initMSHR(struct mshr_t *mshr, int flag)
{
    int i, mshr_size;
    if(!mshr) return;
    if(flag == 1)
        mshr_size = L2_MSHR_SIZE;
    else
        mshr_size = MSHR_SIZE;

    mshr->mshrSIZE = mshr_size;
    mshr->freeEntries = mshr_size;
#ifdef  EDA
    mshr->freeOCEntries = MSHR_OC_SIZE;
#endif
    for (i = 0; i < mshr_size; i++)
    {
        mshr->mshrEntry[i].id = i;
        mshr->mshrEntry[i].isValid = 0;
        mshr->mshrEntry[i].addr = 0;
        mshr->mshrEntry[i].startCycle = 0;
        mshr->mshrEntry[i].endCycle = 0;
        mshr->mshrEntry[i].blocknum = 0;
#ifdef  EDA
        mshr->mshrEntry[i].isOC = 0;
#endif
    }
}

int isMSHRFull(struct mshr_t *mshr, int isOC, int id)
{
    if(!mshr) return 0;
    if(mshr->freeEntries == 0 && !last_L1_mshr_full[id])
    {
        L1_mshr_full ++;
        last_L1_mshr_full[id] = sim_cycle;
    }
    else
    {
        if(last_L1_mshr_full[id])
        {
            Stall_L1_mshr += sim_cycle - last_L1_mshr_full[id];
            last_L1_mshr_full[id] = 0;
        }
    }
    return (mshr->freeEntries == 0);
}

void freeMSHR(struct mshr_t *mshr)
{
    int i;
    if(!mshr) return;
    for (i = 0; i < MSHR_SIZE; i++)
    {
        if(mshr->mshrEntry[i].isValid && mshr->mshrEntry[i].endCycle <= sim_cycle)
        {
            mshr->mshrEntry[i].isValid = 0;
            mshr->freeEntries++;
            struct RS_list *prev, *cur;
            cur = rs_cache_list[mshr->threadid][i];
            while(cur)
            {
                if(cur->rs && ((cur->rs->addr)>>cache_dl1[mshr->threadid]->set_shift) != (mshr->mshrEntry[i].addr>>cache_dl1[mshr->threadid]->set_shift))
                    panic("L1 MSHR: illegal address in MSHR list");

                prev = cur->next;
                RS_block_next(cur);
                free(cur);
                cur = prev;
            }
            rs_cache_list[mshr->threadid][i] = NULL;
        }
    }
    if (mshr->freeEntries > MSHR_SIZE)
        panic("MSHR SIZE exceeds\n");
}

void freeL2MSHR(struct mshr_t *mshr)
{
    int i;
    if(!mshr) return;
    for (i = 0; i < L2_MSHR_SIZE; i++)
    {
        if(mshr->mshrEntry[i].isValid && mshr->mshrEntry[i].endCycle <= sim_cycle)
        {
            mshr->mshrEntry[i].isValid = 0;
            mshr->mshrEntry[i].blocknum = 0;
            mshr->freeEntries++;
        }
    }
    if (mshr->freeEntries > L2_MSHR_SIZE)
        panic("MSHR SIZE exceeds\n");
}

int MSHR_block_check(struct mshr_t *mshr, md_addr_t addr, int offset)
// check if the miss address is already in the mshr; @ fanglei
{
    int i, match = 0, mshr_size;
    mshr_size = mshr->mshrSIZE;

    for (i=0; i<mshr_size; i++)
    {
        if(mshr->mshrEntry[i].isValid)
        {
            if(((mshr->mshrEntry[i].addr>>offset) == (addr>>offset)))
            {
                if(match != 0)  panic("MSHR: a miss address belongs to more than two MSHR Entries");
                match = (i+1);
            }
        }
    }
    return match;
}

/* per cache line MSHR lookup. Insert if there is hit */
void MSHRLookup(struct mshr_t *mshr, md_addr_t addr, int lat, int isOC, struct RUU_station *rs)
{
    int i, mshr_size;
    if(!mshr) return;

    if(lat < cache_dl1_lat)
        return;

    mshr_size = mshr->mshrSIZE;

    if(rs && rs->threadid != mshr->threadid)
        panic("L1 MSHR wrong threadid");

    if(mshr->freeEntries == 0)
        panic("There is no free entry to insert in MSHR");

    for (i = 0; i < mshr_size; i++)
    {
        if(!mshr->mshrEntry[i].isValid)
        {
            mshr->mshrEntry[i].isValid = 1;
            mshr->mshrEntry[i].addr = addr;
            mshr->mshrEntry[i].startCycle =  sim_cycle;
            mshr->mshrEntry[i].endCycle = sim_cycle + lat;
            mshr->mshrEntry[i].event = event_created;
            mshr->mshrEntry[i].rs = rs;
            mshr->freeEntries--;
            return;
        }
    }

    panic("There is no free entry to insert in MSHR");
}

int mshr_lookup(struct DIRECTORY_EVENT *event, tick_t startcycle)
{
    int i, mshr_size;
    int threadid = event->tempID;
    if(!cache_dl1[threadid]->mshr)
        return;
    mshr_size = cache_dl1[threadid]->mshr->mshrSIZE;
    for (i=0;i<mshr_size;i++)
    {
        if(cache_dl1[threadid]->mshr->mshrEntry[i].isValid
                && ((cache_dl1[threadid]->mshr->mshrEntry[i].addr >> cache_dl1[threadid]->set_shift) == (event->addr >> cache_dl1[threadid]->set_shift)))
        {

            if(cache_dl1[threadid]->mshr->mshrEntry[i].startCycle != startcycle)
                panic("L1 MSHR: illegal match in mshr");

            return i;
            break;
        }
    }
    return -1;
}

void mshr_clear(int threadid, int i)
{
    cache_dl1[threadid]->mshr->mshrEntry[i].isValid = 0;
    cache_dl1[threadid]->mshr->freeEntries++;
    rs_cache_list[threadid][i] = NULL;
}

void mshrentry_update(struct DIRECTORY_EVENT *event, tick_t ready, tick_t startcycle)
{
    int i, mshr_size;
    int threadid = event->tempID;
    if(!cache_dl1[threadid]->mshr)
        return;
    mshr_size = cache_dl1[threadid]->mshr->mshrSIZE;
    for (i=0;i<mshr_size;i++)
    {
        if(cache_dl1[threadid]->mshr->mshrEntry[i].isValid
                && ((cache_dl1[threadid]->mshr->mshrEntry[i].addr >> cache_dl1[threadid]->set_shift) == (event->addr >> cache_dl1[threadid]->set_shift)))
        {

            if(cache_dl1[threadid]->mshr->mshrEntry[i].startCycle != startcycle)
                panic("L1 MSHR: illegal match in mshr");

            cache_dl1[threadid]->mshr->mshrEntry[i].endCycle = ready;
            break;
        }
    }
}
#endif

//int local_access_check(long src1, long src2, long des1, long des2)
//{
//    int flag;
//    if( ((src1*mesh_size+src2)==(des1*mesh_size+des2)) | PPP_NetIdeal )
//        flag = 1;
//    else
//        flag = 0;
//    return flag;
//}

int local_access_check(struct DIRECTORY_EVENT *event)
{
    int flag;
    if( ((event->src1*mesh_size+event->src2)==(event->des1*mesh_size+event->des2)) )
        flag = 1;
    else
        flag = 0;
    return flag;
}

int tlb_transfer_check(struct DIRECTORY_EVENT *event)
{
    int tlb_transfer;
    tlb_transfer = event->operation == SHR_TLB_MISS || event->operation == SHR_TLB_WRITE_BACK || event->operation == SHR_TLB_SR_RECOVERY
                || event->operation == SHR_TLB_UPDATE_PR_ACK || event->operation == SHR_TLB_UPDATE_PW_ACK || event->operation == SHR_TLB_RECOVERY_ACK
                || event->operation == DIR_TLB_REFILL || event->operation == DIR_TLB_UPDATE || event->operation == DIR_TLB_RECOVERY
                || event->operation == DIR_TLB_REFILL_WAIT || event->operation == DIR_TLB_UPDATE_WAIT || event->operation == DIR_TLB_RECOVERY_WAIT;
    return tlb_transfer;
}

int vector_count_func( struct cache_blk_t *blk )
{
    int count = 0;
    struct cache_blk_t *temp = blk;
    while(temp->way_prev)
    {
        temp = temp->way_prev;
    }
    while(temp)
    {
        if( (Is_Shared(temp->dir_sharer[0])>1) && ((temp->status & CACHE_BLK_VALID)) )
            count++;
        temp = temp->way_next;
    }
    return count;
}

int ml2_set_match_func( struct cache_blk_t *blk, int set )
{
    int match = 0;
    struct cache_blk_t *temp;
    temp = cache_ml2->sets[set].way_head;
    while(temp)
    {
        if ( temp == blk )
        {
            match = 1;
            break;
        }
        temp = temp->way_next;
    }
    return match;
}

int vector_available_func(struct cache_blk_t **blk, int ml2_set)
{
    int vector_available = 0;
    struct cache_blk_t *temp;

    temp = cache_ml2->sets[ml2_set].way_tail;
    while(temp)
    {
        if( !temp->ptr_en && (temp->dir_state[0] == DIR_STABLE) )
        {
            if(!vector_available)
            {
                *blk = temp;
                vector_available = 1;
                if( Is_Shared(temp->dir_sharer[0]) < 2 )
                    break;
            }
            else if( Is_Shared(temp->dir_sharer[0]) < 2 )
            {
                *blk = temp;
                vector_available = 1;
                break;
            }
        }
        temp = temp->way_prev;
    }

    return vector_available;
}

int findCacheStatus(struct cache_t *cp, md_addr_t set, md_addr_t tag, int hindex, struct cache_blk_t **blk)
{
    struct cache_blk_t *findblk;

    for (findblk=cp->sets[set].way_head; findblk; findblk=findblk->way_next)
    {
        if((findblk->tagid.tag == tag)  && (findblk->status & CACHE_BLK_VALID)
#if PROCESS_MODEL
                && (isSharedAddress == 1)
#endif
          )
        {
            *blk = findblk;
            return 1;
        }
    }
    return 0;
}
int Is_Shared(unsigned long long int *sharer)
{
    int count;
    int counter = 0;
    for (count = 0; count < numcontexts; count++)
    {
        if((((sharer[count/64]) & ((unsigned long long int)1 << (count%64))) == ((unsigned long long int)1 << (count%64))))
        {
            counter ++;
        }
    }
    if(counter)
        return counter;
    else
        return 0;

}
int Is_ExclusiveOrDirty(unsigned long long int *sharer)
{
    int count;
    int counter = 0;
    for (count = 0; count < numcontexts; count++)
    {
        if( ((sharer[count/64]) & ((unsigned long long int)1 << (count%64))) == ((unsigned long long int)1 << (count%64)) )
            counter ++;
    }
    if(counter == 1)
        return 1;
    else
        return 0;
}

int IsShared(unsigned long long int *sharer, int threadid)
{
    int count;
    int counter = 0;
    for (count = 0; count < numcontexts; count++)
    {
        if((((sharer[count/64]) & ((unsigned long long int)1 << (count%64))) == ((unsigned long long int)1 << (count%64))) && (count != threadid))
        {
            counter ++;
        }
    }
    if(counter)
        return 1;
    else
        return 0;

}
int IsExclusiveOrDirty(unsigned long long int *sharer, int threadid, int *ExclusiveID)
{
    int count, Exclusive_id;
    int counter = 0;
    for (count = 0; count < numcontexts; count++)
    {
        if((((sharer[count/64]) & ((unsigned long long int)1 << (count%64))) == ((unsigned long long int)1 << (count%64))) && (count != threadid))
        {
            counter ++;
            Exclusive_id = count;
        }
    }
    if(counter == 1)
    {
        (*ExclusiveID) = Exclusive_id;
        return 1;
    }
    else
        return 0;
}

/* L2 FIFO queue*/



/* insert event into event_list appending to a mshr entry */ /* jing */
void event_list_insert(struct DIRECTORY_EVENT *event, int entry)
{
    if(entry < 1 || entry > L2_MSHR_SIZE)   panic("L2 MSHR insertion error");
    entry = entry - 1;
    struct DIRECTORY_EVENT *cur, *prev;

    if(event_list[entry] && ((event->addr >> cache_dl2->set_shift) != (event_list[entry]->addr >> cache_dl2->set_shift)))
        panic("L2 MSHR insertion error");
    //event->next = event_list[entry];
    //event_list[entry] = event;
    if(sim_cycle == cache_dl2->mshr->mshrEntry[entry].endCycle)
        dir_eventq_insert(event);
    else
    {
        event->next = event_list[entry];
        event_list[entry] = event;
    }
}
/* update L2 MSHR entry when a missing is handled over*/ /* jing */
void update_L2_mshrentry(md_addr_t addr, tick_t ready, tick_t startcycle)
{
    int i;
    if(!cache_dl2->mshr)
        panic("L2 CACHE did not have MSHR!");
    struct DIRECTORY_EVENT *cur, *prev;
    for (i=0;i<L2_MSHR_SIZE;i++)
    {
        if(cache_dl2->mshr->mshrEntry[i].isValid
                && (cache_dl2->mshr->mshrEntry[i].addr == addr))
        {  /* Find the entry which should be updated*/
            if(cache_dl2->mshr->mshrEntry[i].startCycle != startcycle)
                panic("L2 MSHR: illegal match in MSHR while update");
            cache_dl2->mshr->mshrEntry[i].blocknum ++;
            if(cache_dl2->mshr->mshrEntry[i].blocknum < (cache_dl2->bsize/max_packet_size))
                return;
            cache_dl2->mshr->mshrEntry[i].endCycle = ready;
            cache_dl2->mshr->mshrEntry[i].blocknum = 0;
            for(cur = event_list[i]; cur; cur=prev)
            {
                if(((cur->addr)>>cache_dl2->set_shift) != ((cache_dl2->mshr->mshrEntry[i].addr>>cache_dl2->set_shift)))
                {
                    printf("cur addr %lld, mshr addr %lld\n", cur->addr, cache_dl2->mshr->mshrEntry[i].addr);
                    panic("Eventlist should have the same address with its appending mshr entry!");
                }
                prev = cur->next;
                cur->next = NULL;
                cur->when = sim_cycle;

                int block_offset = blockoffset(cur->addr);

//                if ( (cur->tlb_state == SHARED_WRITE) && (!cur->blk_dir) )
//                    panic("it should lock a meta entry\n");
//
//                if ( cur->blk_dir )
//                    cur->blk_dir->dir_state[block_offset] = DIR_STABLE;

                dir_eventq_insert(cur);
            }
            event_list[i] = NULL;
            break;
        }
    }
}

/* check the sharer is in the neighborhood of threadid */
int IsNeighbor(unsigned long long int sharer, int threadid, int type, md_addr_t addr)
{
    int count, close, far, data_close, data_far, data_corner, total_sharer, total_data_sharer, corner, data_flag = 0;
    struct cache_t *cp1;
    struct cache_blk_t *blk1;
    md_addr_t tag, set;     //owner L1 cache tag and set
    close = 0;
    corner = 0;
    far = 0;
    total_sharer = 0;
    total_data_sharer = 0;
    total_p_c_events ++;

    for (count = 0; count < numcontexts; count++)
    {
        if((((sharer) & ((unsigned long long int)1 << count)) == ((unsigned long long int)1 << count)) && (count != threadid))      // bug? @fanglei
        {
            cp1 = cache_dl1[count];
            tag = CACHE_TAG(cp1,addr);
            set = CACHE_SET(cp1,addr);
            for (blk1 = cp1->sets[set].way_head; blk1; blk1 =blk1->way_next)
            {
                if ((blk1->tagid.tag == tag) && (blk1->status & CACHE_BLK_VALID))
                {
                    if(blk1->state == MESI_MODIFIED)
                    {
                        data_flag = 1;
                        total_data_consumers ++;
                        total_data_sharer ++;
                    }
                }
            }

            total_consumers ++;
            total_sharer ++;

            if(abs(threadid - count) == 3 || abs(threadid - count) == 5)
            {
                total_packets_at_corners ++;
                corner ++;
                if(data_flag == 1)
                    total_data_at_corner ++;
            }

            if(abs(threadid - count) == 1 || abs(threadid - count) == 4)
            {
                close ++;
                total_packets_in_neighbor ++;
                if(data_flag == 1)
                    total_data_close ++;
            }
            else
            {
                far ++;
                if(data_flag == 1)
                    total_data_far ++;
            }
        }
    }
    //  if(total_data_sharer > 1)
    //      panic("Neighborhood Check: can not have two modified suppliser!");
    if((corner + close) == total_sharer)
        total_all_almostclose ++;

    if(close == total_sharer)
        total_all_close ++;
    else
    {
        total_not_all_close ++;
        average_inside_percent += ((double)close/(double)total_sharer);
        average_outside_percent += ((double)far/(double)total_sharer);
        average_outside_abs_percent += ((double)(far-corner)/(double)total_sharer);
        average_corner_percent += ((double)(corner)/(double)total_sharer);
    }

    switch (total_sharer)
    {
    //  case 0: panic("total_sharers can not be 0!");
        case 1: sharer_num[1] ++; break;
        case 2: sharer_num[2] ++; break;
        case 3: sharer_num[3] ++; break;
        case 4: sharer_num[4] ++; break;
        case 5: sharer_num[5] ++; break;
        case 6: sharer_num[6] ++; break;
        case 7: sharer_num[7] ++; break;
        case 8: sharer_num[8] ++; break;
        case 9: sharer_num[9] ++; break;
        case 10: sharer_num[10] ++; break;
        case 11: sharer_num[11] ++; break;
        case 12: sharer_num[12] ++; break;
        case 13: sharer_num[13] ++; break;
        case 14: sharer_num[14] ++; break;
             //case 15: sharer_num[15] ++; break;
        default: sharer_num[15] ++; break;
    }

}
int IsNeighbor_sharer(unsigned long long int sharer, int threadid)
{
    int count;

    for (count = 0; count < numcontexts; count++)
    {
        if((((sharer) & ((unsigned long long int)1 << count)) == ((unsigned long long int)1 << count)) && (count != threadid))      // bug? @fanglei
        {
            if(abs(threadid - count) == 3 || abs(threadid - count) == 5)
                total_corner_packets ++;

            if(abs(threadid - count) == 1 || abs(threadid - count) == 4)
                total_neighbor_packets ++;
            else
                total_far_packets ++;
        }
    }
}

int blockoffset(md_addr_t addr)
{
    int bofs;
    bofs = addr & cache_dl2->blk_mask;
    bofs = bofs >> cache_dl1[0]->set_shift;
    return bofs;
}

struct DIRECTORY_EVENT *allocate_new_event(struct DIRECTORY_EVENT *event)
{
    struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
    if(new_event == NULL)       panic("Out of Virtual Memory");

    new_event->input_buffer_full = 0;
    new_event->req_time = 0;
    new_event->conf_bit = -1;
    new_event->pending_flag = 0;
    new_event->isPrefetch = 0;
    new_event->stride = 0;
    new_event->processingDone = 0;
    new_event->startCycle = sim_cycle;
    new_event->started = sim_cycle;
    new_event->blk_dir = NULL;
    new_event->cp = event->cp;
    new_event->addr = event->addr;
    new_event->vp = NULL;
    new_event->nbytes = event->cp->bsize;
    new_event->udata = NULL;
    new_event->rs = event->rs;
    new_event->spec_mode = 0;
    new_event->L2miss_flag = event->L2miss_flag;
    new_event->data_reply = 1;
    new_event->mshr_time = event->mshr_time;
    new_event->store_cond = event->store_cond;
    new_event->originalChildCount = 0;
    new_event->l1_wb_entry = NULL;

    new_event->op = event->op;

    new_event->src1 = event->des1;
    new_event->src2 = event->des2;

#ifdef CENTRALIZED_L2
	new_event->des1 = mesh_size+1;

	switch(event->des2/(mesh_size/mem_port_num)) /* right side memory port */
    {
        case 0:
            new_event->des2 = mesh_size/mem_port_num - 1;
        break;
        case 1:
            new_event->des2 = mesh_size/mem_port_num;
        break;
        case 2:
            new_event->des2 = 3*(mesh_size/mem_port_num) - 1;
        break;
        case 3:
            new_event->des2 = 3*(mesh_size/mem_port_num);
        break;
        case 4:
            new_event->des2 = 5*(mesh_size/mem_port_num) - 1;
        break;
        case 5:
            new_event->des2 = 5*(mesh_size/mem_port_num);
        break;
        case 6:
            new_event->des2 = 7*(mesh_size/mem_port_num) - 1;
        break;
        case 7:
            new_event->des2 = 7*(mesh_size/mem_port_num);
        break;
        default:
            panic("can not have other case\n");
    }
#else
    if((event->des1-MEM_LOC_SHIFT)/(mesh_size/2))
        new_event->des1 = mesh_size+1;   /* each column has a memory access port */
    else
        new_event->des1 = 0;

    //new_event->des2 = ((event->des2)/(mesh_size/(mem_port_num/2))) * (mesh_size/(mem_port_num/2));         /* the memory access port allocate in the middle of those routers. */
    switch(event->des2/(mesh_size/(mem_port_num/2))) /* right side memory port */
    {
        case 0:
            new_event->des2 = mesh_size/(mem_port_num/2) - 1;
        break;
        case 1:
            new_event->des2 = mesh_size/(mem_port_num/2);
        break;
        case 2:
            new_event->des2 = 3*(mesh_size/(mem_port_num/2)) - 1;
        break;
        case 3:
            new_event->des2 = 3*(mesh_size/(mem_port_num/2));
        break;
        case 4:
            new_event->des2 = 5*(mesh_size/(mem_port_num/2)) - 1;
        break;
        case 5:
            new_event->des2 = 5*(mesh_size/(mem_port_num/2));
        break;
        case 6:
            new_event->des2 = 7*(mesh_size/(mem_port_num/2)) - 1;
        break;
        case 7:
            new_event->des2 = 7*(mesh_size/(mem_port_num/2));
        break;
        default:
            panic("can not have other case\n");
    }
#endif
    new_event->parent = event;
    new_event->tempID = event->tempID;
    new_event->resend = 0;
    return new_event;
}
#ifdef ESYNET
struct OrderBufferEntry
{
    int valid;
    md_addr_t addr;
    int des;
    int vc;
    counter_t msgno;
};
struct OrderBufferEntry OrderBuffer[MAXSIZE][MAXSIZE];
int OrderBufferNum[MAXSIZE];

void OrderBufInit()
{
    int i=0, j=0;
    for(i=0;i<MAXSIZE;i++)
    {
        OrderBufferNum[i] = 0;
        for(j=0;j<MAXSIZE;j++)
        {
            OrderBuffer[i][j].addr = 0;
            OrderBuffer[i][j].valid = 0;
            OrderBuffer[i][j].des = 0;
            OrderBuffer[i][j].vc = -1;
            OrderBuffer[i][j].msgno = 0;
        }
    }
}
void OrderBufInsert(int s1, int s2, int d1, int d2, md_addr_t addr, int vc, counter_t msgno)
{
    int i=0, src, des;
    src = s1*mesh_size+s2;
    des = d1*mesh_size+d2;

    if(OrderBufferNum[src] >= MAXSIZE)
        panic("Out going buffer is full for esynet multiple vcs");

    for(i=0;i<MAXSIZE;i++)
    {
        if(!OrderBuffer[src][i].valid)
        {
            OrderBuffer[src][i].valid = 1;
            OrderBuffer[src][i].addr = addr;
            OrderBuffer[src][i].des = des;
            OrderBuffer[src][i].vc = vc;
            OrderBuffer[src][i].msgno = msgno;
            OrderBufferNum[src] ++;
            return;
        }
    }
}
void OrderBufRemove(int s1, int s2, int d1, int d2, md_addr_t addr, counter_t msgno)
{
    int i=0, src, des;
    src = s1*mesh_size+s2;
    des = d1*mesh_size+d2;

    for(i=0;i<MAXSIZE;i++)
    {
        if(OrderBuffer[src][i].valid && OrderBuffer[src][i].addr == addr && OrderBuffer[src][i].des == des && OrderBuffer[src][i].msgno == msgno)
        {
            OrderBuffer[src][i].valid = 0;
            OrderBuffer[src][i].addr = 0;
            OrderBuffer[src][i].des = 0;
            OrderBuffer[src][i].vc = -1;
            OrderBuffer[src][i].msgno = 0;
            OrderBufferNum[src] --;
            return;
        }
    }
    panic("Can not find the entry in OrderBuffer");
}

int OrderCheck(int s1, int s2, int d1, int d2, md_addr_t addr)
{
    int i=0, src, des;
    src = s1*mesh_size+s2;
    des = d1*mesh_size+d2;
#ifdef MULTI_VC
    for(i=0;i<MAXSIZE;i++)
    {
        if(OrderBuffer[src][i].valid && OrderBuffer[src][i].addr == addr && OrderBuffer[src][i].des == des)
            return OrderBuffer[src][i].vc;
    }
#endif
    return -1;
}
#endif

void timespace(counter_t time, int type)
{
    if(time < 0)
        return;
    if(time <5)
        spand[0] ++;
    else if(time <10)
        spand[1] ++;
    else if(time <20)
        spand[2] ++;
    else if(time <30)
        spand[3] ++;
    else
        spand[4] ++;
    if(type == 1)
        downgrade_r ++;
    else
        downgrade_w ++;
    return;
}

int dir_operation (struct DIRECTORY_EVENT *event, int lock_access)
{
    md_addr_t addr=event->addr;

    struct cache_t *ml2 = cache_ml2;
    struct cache_t *dl2 = cache_dl2;
    struct cache_blk_t *ml2_blk;
    struct cache_blk_t *dl2_blk;
    md_addr_t ml2_tag, ml2_set;
    md_addr_t ml2_mul_tag, ml2_mul_set;
    md_addr_t ml2_sub_tag, ml2_sub_set;
    md_addr_t dl2_tag, dl2_set;
    int ml2_hindex;
    int dl2_hindex;

    int ml2_hit = 0;
    int ml2_ed = 0;

    dl2_tag = CACHE_TAG(dl2, addr);
    dl2_set = CACHE_SET(dl2, addr);

    md_addr_t qblk_set_shift = multi_blk_set_shift;
    md_addr_t qblk_set_mask = ((1<<qblk_set_shift)-1);

    if (qblk_set_shift)
    {
        ml2_tag = CACHE_QBLK_TAG(ml2, addr);
        ml2_set = CACHE_QBLK_SET(ml2, addr);
    }
    else
    {
        ml2_tag = CACHE_TAG(ml2, addr);
        ml2_set = CACHE_SET(ml2, addr);
    }

    ml2_mul_tag = CACHE_QBLK_TAG(ml2, addr);
    ml2_mul_set = CACHE_QBLK_SET(ml2, addr);
    ml2_sub_tag = CACHE_TAG(ml2, addr);
    ml2_sub_set = CACHE_SET(ml2, addr);

    ml2_hindex = (int) CACHE_HASH(ml2,ml2_tag);
    dl2_hindex = (int) CACHE_HASH(dl2,dl2_tag);

    struct cache_t *cp1;
    struct cache_blk_t *dl1_blk;
    md_addr_t tag, set;

    md_addr_t bofs;
    int block_offset = blockoffset(event->addr);
    struct cache_blk_t *repl;       //replacement block in data cache L1 or data cache L2

    struct DIRECTORY_EVENT *parent_event = event->parent;

    extern counter_t icache_access;
    extern counter_t dcache_access;
    int lat;
    int matchnum;           //match entry index in L2 mshr
    int Threadid;
    int tempID =  event->tempID;    //requester thread id
    int count;          //owner thread id

    unsigned int packet_size;
    int invCollectStatus = 0;
    int vc = 0, a = 0;
    int options = -1;

    int L1_hit = 0;

    unsigned long long int all_sharer = (mesh_size == 8) ? (0xFFFFFFFFFFFFFFFF) : ((1<<(mesh_size*mesh_size))-1);

    event->flip_flag = 0;

    if(!(event->operation == ACK_DIR_IL1 || event->operation == ACK_DIR_READ_SHARED || event->operation == ACK_DIR_READ_EXCLUSIVE || event->operation == ACK_DIR_WRITE || event->operation == ACK_DIR_WRITEUPDATE
        || event->operation == SHR_TLB_WRITE_BACK || event->operation == SHR_TLB_SR_RECOVERY || event->operation == DIR_TLB_REFILL
    ))
    {
        /* check input buffer space for the scheduling event */
        int buffer_full_flag = 0;
        if(!mystricmp (network_type, "FSOI") || !(mystricmp (network_type, "HYBRID")))
            buffer_full_flag = opticalBufferSpace(event->des1, event->des2, event->operation);
        else if(!mystricmp (network_type, "MESH"))
        {
            if(!(   event->operation == ACK_DIR_IL1 ||
                    event->operation == ACK_DIR_READ_SHARED ||
                    event->operation == ACK_DIR_READ_EXCLUSIVE ||
                    event->operation == ACK_DIR_WRITE ||
                    event->operation == ACK_DIR_WRITEUPDATE ||
                    event->operation == FINAL_INV_ACK ||
                    event->operation == MEM_READ_REQ ||
                    event->operation == MEM_READ_REPLY ||
                    event->operation == WAIT_MEM_READ_N_WAIT ||
                    (event->des1 == event->src1 && event->des2 == event->src2)
                 )
               )
            // ACK_* means an event has done and no network messages will be genarated. As a result, no outgoing buffers are needed. Otherwise, outgoing buffer is allocated for sending message. @ fanglei
            {
                options = OrderCheck(event->des1, event->des2, event->src1, event->src2, addr&~(cache_dl1[0]->bsize-1));
                vc = simEsynetBufferSpace(event->des1, event->des2, options);
            }
            buffer_full_flag = (vc == -1);
        }
        else if(!mystricmp (network_type, "COMB"))
        {
            buffer_full_flag = CombNetworkBufferSpace(event->src1, event->src2, event->des1, event->des2, event->addr&~(cache_dl1[0]->bsize-1), event->operation, &vc);
        }
        if(buffer_full_flag)
        {
            if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
            {
                Input_queue_full ++;
                last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
            }
            return 0;
        }
        else
        {
            if(last_Input_queue_full[event->des1*mesh_size+event->des2])
            {
                Stall_input_queue += sim_cycle - last_Input_queue_full[event->des1*mesh_size+event->des2];
                last_Input_queue_full[event->des1*mesh_size+event->des2] = 0;
            }
        }
    }

    switch(event->operation)
    {
        case IDEAL_CASE :
            panic("DIR: Event operation is ideal - not possible");
            break;

        case UPDATE_DIR :
            panic("This Update_dir part should never be entered!\n");
            break;

        case NACK       :
            /* this is the nack sending from L2 because of the dir_fifo full */
            nack_counter ++;
            event->src1 += event->des1;
            event->des1 = event->src1 - event->des1;
            event->src1 = event->src1 - event->des1;
            event->src2 += event->des2;
            event->des2 = event->src2 - event->des2;
            event->src2 = event->src2 - event->des2;
            event->operation = event->parent_operation;
            event->processingDone = 0;
            event->startCycle = sim_cycle + 20;
            scheduleThroughNetwork(event, event->startCycle, meta_packet_size, vc);
            if(event->prefetch_next == 2 || event->prefetch_next == 4)
                prefetch_nacks ++;
            else if(event->isSyncAccess)
                sync_nacks ++;
            else if(event->operation == MISS_WRITE)
                write_nacks ++;
            else
                normal_nacks ++;
            dir_eventq_insert(event);
            if(collect_stats)
                dl2->dir_access ++;
            return 1;

        case L2_PREFETCH:
            event->parent_operation = event->operation;
            if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
            {
                free_event(event);
                return 1;
            }
            else
            {
                matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                if(!matchnum && !md_text_addr(event->addr, tempID))
                { /* No match in L2 MSHR */
                    if(!cache_dl2->mshr->freeEntries)
                    { /* L2 mshr is full */
                        L2_mshr_full_prefetch ++;
                        free_event(event);
                        return 1;
                    }
                    l2_prefetch_num ++;
                    event->mshr_time = sim_cycle;
                    struct DIRECTORY_EVENT *new_event;
                    new_event = allocate_new_event(event);
                    new_event->operation = MEM_READ_REQ;
                    new_event->cmd = Read;
                    event->when = sim_cycle + WAIT_TIME;
                    event->operation = WAIT_MEM_READ;
                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                    dir_eventq_insert(new_event);
                    event->startCycle = sim_cycle;
                    event_created = event;
                    MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                    return 1;
                }
                else
                {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                    free_event(event);
                    return 1;
                }
            }
            break;

        case DIR_TLB_REFILL:
            // refill the tlb @ fanglei
            {
                int tlb_mshr_match = -1;
                tlb_mshr_match = tlb_mshr_check(addr, tempID, NULL);
                if (tlb_mshr_match == -1)
                    panic("tlb_mshr miss\n");
                if(tlb_mshr[tempID][tlb_mshr_match]->evicting && (event->page_entry_state == PRIVATE_READ))
                    event->page_entry_state = PRIVATE_WRITE;
                if(tlb_mshr[tempID][tlb_mshr_match]->dirty)
                    event->page_entry_state = SHARED_WRITE;
                tag = CACHE_TAG(event->cp, addr);
                set = CACHE_SET(event->cp, addr);
                repl = (event->cp)->sets[set].way_tail;
                if (repl->state == PRIVATE_WRITE)
                {
                    // the PRIVATE_WRITE tlb_entry is evicted, we write it back @ fanglei
                    tlb_cu_allocate(event->tempID, SHR_TLB_WRITE_BACK, repl->tagid.tag, NULL);
                    struct DIRECTORY_EVENT *new_event = (struct DIRECTORY_EVENT *) allocate_event(0); //create a space
                    if (!new_event)
                        fatal("out of virtual memory in calloc dir event");
                    long src;
                    new_event->operation = SHR_TLB_WRITE_BACK;
                    new_event->src1 = event->des1;
                    new_event->src2 = event->des2;
                    src = 0;    // assume the node 0 runs the system @ fanglei
                    new_event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                    new_event->des2 = (src %mesh_size);
                    new_event->started = sim_cycle;
                    new_event->startCycle = sim_cycle;
                    new_event->processingDone = 0;
                    new_event->tempID = event->tempID;
                    new_event->addr = ( repl->tagid.tag << MD_LOG_PAGE_SIZE );
                    new_event->cp = dtlb[event->tempID];

                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                    dir_eventq_insert(new_event);
                }
                update_way_list (&(event->cp)->sets[set], repl, Head);
                repl->tagid.tag = tag;
                repl->status = CACHE_BLK_VALID;
                repl->ready = sim_cycle;
                repl->state = event->page_entry_state;


                // clean the tlb mshr & restart the insts @ fanglei;
                struct tlb_rs *temp;
                for(temp=tlb_mshr[event->tempID][tlb_mshr_match]->head;temp!=NULL;temp=temp->next)
                {
                    temp->rs->issue_time = sim_cycle;
                }
                tlb_mshr_clear(event->tempID, tlb_mshr_match);
                free_event(event);
            }
            break;

        case DIR_TLB_UPDATE:
            // PR->SR if the local state is PR @ fanglei
            // PW->SW if the local state is PW @ fanglei
            {
                int target_tlb_id = (event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2;
                int tlb_entry_match = 0;
                int tlb_cu_match = -1;
                struct cache_t *target_dtlb = dtlb[target_tlb_id];
                struct cache_blk_t *dtlb_entry;
                md_addr_t dtlb_tag = CACHE_TAG (target_dtlb, addr);
                md_addr_t dtlb_set = CACHE_SET (target_dtlb, addr);
                int target_state = 0;
                for (dtlb_entry = target_dtlb->sets[dtlb_set].way_head; dtlb_entry; dtlb_entry = dtlb_entry->way_next)
                {
                    if ((dtlb_entry->tagid.tag == dtlb_tag) && (dtlb_entry->status & CACHE_BLK_VALID))
                    {
                        target_state = dtlb_entry->state;
                        if (target_state == PRIVATE_READ)
                            dtlb_entry->state = SHARED_READ;
                        else if (target_state == PRIVATE_WRITE)
                            dtlb_entry->state = SHARED_WRITE;
                        else
                            panic("wrong sharer state\n");
                        tlb_entry_match = 1;
                        break;
                    }
                }
                if (!target_state)  // the tlb entry is evicted, we look up the tlb_cu @ fanglei
                {
                    tlb_cu_match = tlb_cu_check(target_tlb_id, addr, DIR_TLB_UPDATE);
                    if (tlb_cu_match!= -1)
                        target_state = PRIVATE_WRITE;
                    else
                        target_state = PRIVATE_READ;
                }
                if (target_state == PRIVATE_READ)
                {
                    // update @ fanglei
                    event->operation = SHR_TLB_UPDATE_PR_ACK;
                    event->page_entry_state = SHARED_READ;
                    event->src1 += event->des1;
                    event->des1 = event->src1 - event->des1;
                    event->src1 = event->src1 - event->des1;
                    event->src2 += event->des2;
                    event->des2 = event->src2 - event->des2;
                    event->src2 = event->src2 - event->des2;
                    event->started = sim_cycle;
                    event->startCycle = sim_cycle;
                    event->processingDone = 0;

                    scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                    dir_eventq_insert(event);
                }
                else if (target_state == PRIVATE_WRITE)
                {
                    // recovery @ fanglei
                    //flush the target L1 data cache
                    data_cache_flush(addr, target_tlb_id);
                    tlb_cu_allocate(target_tlb_id, SHR_TLB_UPDATE_PW_ACK, 0, event);

                    event->operation = SHR_TLB_UPDATE_PW_ACK;
                    event->page_entry_state = SHARED_WRITE;
                    event->src1 += event->des1;
                    event->des1 = event->src1 - event->des1;
                    event->src1 = event->src1 - event->des1;
                    event->src2 += event->des2;
                    event->des2 = event->src2 - event->des2;
                    event->src2 = event->src2 - event->des2;
                    event->started = sim_cycle;
                    event->startCycle = sim_cycle;
                    event->processingDone = 0;
                    event->when = sim_cycle + WAIT_TIME;
                    dir_eventq_insert(event);
                }
                else
                    panic("wrong sharer state\n");
            }
            break;

        case DIR_TLB_RECOVERY:
            {
                // flush the cache @ fanglei
                int target_tlb_id = (event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2;
                int tlb_cu_match = -1;
                int tlb_mshr_match = 0;
                struct cache_t *target_dtlb = dtlb[target_tlb_id];
                struct cache_blk_t *dtlb_entry;
                md_addr_t dtlb_tag = CACHE_TAG (target_dtlb, addr);
                md_addr_t dtlb_set = CACHE_SET (target_dtlb, addr);

                // clean the initiator @ fanglei
                tlb_cu_match = tlb_cu_check(target_tlb_id, addr, DIR_TLB_RECOVERY);
                if(tlb_cu_match != -1)
                {
                    tlb_cu[target_tlb_id]->sr_recovery[tlb_cu_match].valid = 0;
                    tlb_cu[target_tlb_id]->sr_recovery[tlb_cu_match].VPN = 0;
                }

                // modify the tlb_entry @ fanglei
                for (dtlb_entry = target_dtlb->sets[dtlb_set].way_head; dtlb_entry; dtlb_entry = dtlb_entry->way_next)
                {
                    if ((dtlb_entry->tagid.tag == dtlb_tag) && (dtlb_entry->status & CACHE_BLK_VALID))
                    {
                        dtlb_entry->state = SHARED_WRITE;
                        break;
                    }
                }

                event->operation = SHR_TLB_RECOVERY_ACK;
                event->src1 += event->des1;
                event->des1 = event->src1 - event->des1;
                event->src1 = event->src1 - event->des1;
                event->src2 += event->des2;
                event->des2 = event->src2 - event->des2;
                event->src2 = event->src2 - event->des2;
                event->started = sim_cycle;
                event->startCycle = sim_cycle;
                event->processingDone = 0;
                event->when = sim_cycle + WAIT_TIME;
                dir_eventq_insert(event);

                // check if a read request has been sent, inform the tlb_mshr entry to be dirty @ fanglei
                tlb_mshr_match = tlb_mshr_check(addr, tempID, NULL);
                if (tlb_mshr_match != -1)
                    // invalid comes before the refill @ fanglei
                    tlb_mshr[tempID][tlb_mshr_match]->dirty = 1;
                data_cache_flush(dtlb_tag, target_tlb_id);
                tlb_cu_allocate(target_tlb_id, SHR_TLB_RECOVERY_ACK, 0, event);
            }
            break;

        case DIR_TLB_REFILL_WAIT:
            {
                // make the page table ready @ fanglei
                int page_table_match;
                int page_table_state;
                int page_table_busy;
                int page_table_keeper;
                int location;

                page_table_match = page_table_check(addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);
                if( !page_table_match )
                    panic("this entry must be in process\n");
                page_table[location]->busy = 0;

                event->operation = DIR_TLB_REFILL;
                event->page_entry_state = page_table[location]->state;

                event->src1 += event->des1;
                event->des1 = event->src1 - event->des1;
                event->src1 = event->src1 - event->des1;
                event->src2 += event->des2;
                event->des2 = event->src2 - event->des2;
                event->src2 = event->src2 - event->des2;
                event->when = sim_cycle + WAIT_TIME;
                event->started = sim_cycle;
                event->startCycle = sim_cycle;
                event->processingDone = 0;
                scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                dir_eventq_insert(event);
            }
            break;

        case DIR_TLB_UPDATE_WAIT:
            {
                event->operation = DIR_TLB_UPDATE;
                event->when = sim_cycle + WAIT_TIME;
                event->started = sim_cycle;
                event->startCycle = sim_cycle;
                event->processingDone = 0;
                scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                dir_eventq_insert(event);
            }
            break;

        case DIR_TLB_RECOVERY_WAIT:
            {
                event->operation = DIR_TLB_RECOVERY;
                event->when = sim_cycle + WAIT_TIME;
                event->started = sim_cycle;
                event->startCycle = sim_cycle;
                event->processingDone = 0;
                scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                dir_eventq_insert(event);
            }
            break;

        case SHR_TLB_WRITE_BACK:
            {
                int page_table_match;
                int page_table_state;
                int page_table_busy;
                int page_table_keeper;
                int location;
                page_table_match = page_table_check(event->addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);
                if (page_table_busy)
                {
                    event->when = sim_cycle + RETRY_TIME;
                    event->flip_flag = 1;
                    dir_eventq_insert(event);
                }
                else
                {
                    if(page_table_state == PRIVATE_READ)
                    {
                        // ideal @ fanglei
                        pr_page_num--;
                        pw_page_num++;
                        page_table[location]->busy = 0;
                        page_table[location]->state = PRIVATE_WRITE;
                    }
                    else if (page_table_state == PRIVATE_WRITE)
                    {
                        // ideal @ fanglei
                        page_table[location]->busy = 0;
                        page_table[location]->state = PRIVATE_WRITE;
                    }
                    else
                    {
                        if (page_table[location]->state != SHARED_WRITE)
                            panic("it should be SHARED_WRITE\n");
                    }
                    // clear pw_evict @ fanglei
                    int tlb_cu_match = -1;
                    tlb_cu_match = tlb_cu_check(event->tempID, event->addr, SHR_TLB_WRITE_BACK);
                    if ( tlb_cu_match == -1 )
                        panic("evict information lost\n");
                    else
                    {
                        tlb_cu[event->tempID]->pw_evict[tlb_cu_match].valid = 0;
                        tlb_cu[event->tempID]->pw_evict[tlb_cu_match].VPN = 0;
                    }
                    free_event(event);
                }
            }
            break;

        case SHR_TLB_RECOVERY_ACK:
            {
                if (event->parent == NULL)  // tlb 4-hop transfer @ fanglei
                {
                    int page_table_match;
                    int page_table_state;
                    int page_table_busy;
                    int page_table_keeper;
                    int location;

                    page_table_match = page_table_check(addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);
                    if( !page_table_match )
                        panic("this entry must be in process\n");
                    page_table[location]->busy = 1;
                    page_table[location]->state = SHARED_WRITE;

                    event->operation = DIR_TLB_REFILL_WAIT;

                    event->src1 = event->new_src1;
                    event->src2 = event->new_src2;
                    event->des1 = event->new_des1;
                    event->des2 = event->new_des2;

                    event->when = sim_cycle + PAGE_TABLE_SEARCH;
                    event->startCycle = sim_cycle;
                    event->processingDone = 0;
                    dir_eventq_insert(event);
                }
                else                        // tlb broadcast @ fanglei
                {
                    event->parent->childCount--;
                    if(event->parent->childCount == 0)
                    {
                        int page_table_match;
                        int page_table_state;
                        int page_table_busy;
                        int page_table_keeper;
                        int location;
                        page_table_match = page_table_check(event->parent->addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);
                        page_table[location]->state = SHARED_WRITE;
                        if( event->parent->operation == SHR_TLB_MISS )
                        {
                            page_table[location]->busy = 1;
                            event->parent->operation = DIR_TLB_REFILL_WAIT;
                            event->parent->when = sim_cycle + PAGE_TABLE_SEARCH;
                            event->parent->startCycle = sim_cycle;
                            event->parent->processingDone = 0;
                            dir_eventq_insert(event->parent);
                        }
                        else if ( event->parent->operation == SHR_TLB_SR_RECOVERY )
                        {
                            page_table[location]->busy = 0;
                            free_event(event->parent);
                        }
                        else
                            panic("no other request\n");
                    }
                    free_event(event);
                }
            }
            break;

        case SHR_TLB_UPDATE_PR_ACK:
        case SHR_TLB_UPDATE_PW_ACK:
            {
                int page_table_match;
                int page_table_state;
                int page_table_busy;
                int page_table_keeper;
                int location;

                page_table_match = page_table_check(addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);
                if( !page_table_match )
                    panic("this entry must be in process\n");
                page_table[location]->busy = 1;
                page_table[location]->state = event->page_entry_state;

                if ( event->page_entry_state == SHARED_WRITE )
                {
                    sr_page_num--;
                    sw_page_num++;
                }

                event->operation = DIR_TLB_REFILL_WAIT;

                event->src1 = event->new_src1;
                event->src2 = event->new_src2;
                event->des1 = event->new_des1;
                event->des2 = event->new_des2;

                event->when = sim_cycle + PAGE_TABLE_SEARCH;
                event->startCycle = sim_cycle;
                event->processingDone = 0;
                dir_eventq_insert(event);
            }
            break;

        case SHR_TLB_SR_RECOVERY:
            {
                int page_table_match;
                int page_table_state;
                int page_table_busy;
                int page_table_keeper;
                int location;

                page_table_match = page_table_check(addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);

                if (!page_table_match)
                    panic("SR must match\n");
                if (page_table_busy)
                {
                    if(page_table_state == SHARED_READ)
                    {
                        // another read in process @ fanglei
                        event->when = sim_cycle + RETRY_TIME;
                        event->flip_flag = 1;
                        dir_eventq_insert(event);
                    }
                    else if (page_table_state == SHARED_WRITE)
                    {
                        // another write had come earlier @ fanglei
                        free_event(event);
                    }
                    else
                        panic("SR recovery can not be other state\n");
                    break;
                }
                sr_page_num--;
                sw_page_num++;

                page_table[location]->busy = 1;
                page_table[location]->state = SHARED_WRITE;
                // braodcast
                int i;
                for (i=0;i<numcontexts;i++)
                {
                    struct DIRECTORY_EVENT *new_event = allocate_event(0);
                    if(new_event == NULL)
                        panic("Out of Virtual Memory");
                    new_event->operation = DIR_TLB_RECOVERY_WAIT;
                    new_event->addr = event->addr;
                    new_event->src1 = 1;
                    new_event->src2 = 0;
                    new_event->des1 = i/mesh_size+MEM_LOC_SHIFT;
                    new_event->des2 = i%mesh_size;
                    new_event->startCycle = sim_cycle;
                    new_event->parent = event;
                    event->childCount++;
                    /*go to network*/
                    new_event->when = sim_cycle + PAGE_TABLE_SEARCH;
                    new_event->startCycle = sim_cycle;
                    new_event->processingDone = 0;
                    dir_eventq_insert(new_event);
                }
            // SR->SW
            }
            break;

        case SHR_TLB_MISS  :
            {
                // dtlb miss require handler @ fanglei
                int page_table_match;
                int page_table_state;
                int page_table_busy;
                int page_table_keeper;
                int location;

                page_table_match = page_table_check(addr, &page_table_state, &page_table_busy, &page_table_keeper, &location);

                if (!page_table_match)
                {
                    // allocate a new page entry @ fanglei
                    total_page_num++;
                    if ( event->cmd == Read )
                        pr_page_num++;
                    else
                        pw_page_num++;

                    struct cache_t *cp = dtlb[0];
                    md_addr_t tag = CACHE_TAG (cp, addr);
                    page_table[location]->valid = 1;
                    page_table[location]->busy = 1;
                    page_table[location]->VPN = tag;
                    page_table[location]->PPN = tag;
                    page_table[location]->keeper = tempID;
                    page_table[location]->cached = 1;
                    page_table[location]->line_head = (struct line_info *) calloc ( (MD_PAGE_SIZE/cache_dl1[0]->bsize), sizeof (struct line_info) );
                    int i;
                    for(i=0;i<(MD_PAGE_SIZE/cache_dl1[0]->bsize);i++)
                    {
                        page_table[location]->line_head[i].line_owner = -1;
                        page_table[location]->line_head[i].line_state = -1;
                    }
                    if ( tlb_c_c )
                        page_table[location]->state = (event->cmd == Read) ? PRIVATE_READ : PRIVATE_WRITE;
                    else
                        page_table[location]->state = SHARED_WRITE;

                    event->operation = DIR_TLB_REFILL_WAIT;
                    event->page_entry_state = page_table[location]->state;
                    event->when = sim_cycle + PAGE_TABLE_SEARCH;
                    event->startCycle = sim_cycle;
                    event->processingDone = 0;
                    dir_eventq_insert(event);
                }
                else if (page_table_busy)
                {
                    // the page entry is handling @ fanglei
                    event->when = sim_cycle + RETRY_TIME;
                    event->flip_flag = 1;
                    dir_eventq_insert(event);
                }
                else
                {
                    // page table exist @ fanglei
                    switch (page_table_state)
                    {
                        case PRIVATE_READ:
                            if(page_table_keeper == tempID)
                            {
                                // the requestor is the keeper and be exvicted from tlb @ fanglei
                                if ( page_table[location]->state == PRIVATE_READ && event->cmd == Write )
                                {
                                    pr_page_num--;
                                    pw_page_num++;
                                }
                                page_table[location]->busy = 1;
                                page_table[location]->state = (event->cmd == Read) ? PRIVATE_READ : PRIVATE_WRITE;
                                event->operation = DIR_TLB_REFILL_WAIT;
                                event->page_entry_state = page_table[location]->state;
                                event->when = sim_cycle + PAGE_TABLE_SEARCH;
                                event->startCycle = sim_cycle;
                                event->processingDone = 0;
                                dir_eventq_insert(event);
                            }
                            else
                            {
                                // update or recovery, depenting on event->cmd @ fanglei
                                pr_page_num--;
                                if ( event->cmd == Read )
                                    sr_page_num++;
                                else
                                    sw_page_num++;

                                page_table[location]->busy = 1;
                                // the final state can be SHARED_WRITE even if the cmd == Read,
                                // the keeper can be PRIVATE_WRITE without inform the page table @ fanglei
                                page_table[location]->state = (event->cmd == Read) ? SHARED_READ : SHARED_WRITE;

                                event->operation = (event->cmd == Read) ? DIR_TLB_UPDATE_WAIT : DIR_TLB_RECOVERY_WAIT;

                                event->new_src1 = event->src1;
                                event->new_src2 = event->src2;
                                event->new_des1 = event->des1;
                                event->new_des2 = event->des2;

                                event->src1 = 1;
                                event->src2 = 0;
                                int src = page_table[location]->keeper;
                                event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                                event->des2 = (src %mesh_size);

                                event->when = sim_cycle + PAGE_TABLE_SEARCH;
                                event->startCycle = sim_cycle;
                                event->processingDone = 0;
                                dir_eventq_insert(event);
                            }
                            break;
                        case PRIVATE_WRITE:
                            if(page_table_keeper == tempID)
                            {
                                // the requestor is the keeper and be exvicted from tlb @ fanglei
                                page_table[location]->busy = 1;
                                page_table[location]->state = PRIVATE_WRITE;

                                event->operation = DIR_TLB_REFILL_WAIT;
                                event->page_entry_state = page_table[location]->state;
                                event->when = sim_cycle + PAGE_TABLE_SEARCH;
                                event->startCycle = sim_cycle;
                                event->processingDone = 0;
                                dir_eventq_insert(event);
                            }
                            else
                            {
                                // it must be a recovery @ fanglei
                                pw_page_num--;
                                sw_page_num++;

                                page_table[location]->busy = 1;
                                page_table[location]->state = SHARED_WRITE;

                                event->operation = DIR_TLB_RECOVERY_WAIT;

                                event->new_src1 = event->src1;
                                event->new_src2 = event->src2;
                                event->new_des1 = event->des1;
                                event->new_des2 = event->des2;

                                event->src1 = 1;
                                event->src2 = 0;
                                int src = page_table[location]->keeper;
                                event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                                event->des2 = (src %mesh_size);

                                event->when = sim_cycle + PAGE_TABLE_SEARCH;
                                event->startCycle = sim_cycle;
                                event->processingDone = 0;
                                dir_eventq_insert(event);
                            }
                            break;
                        case SHARED_READ:
                            if (event->cmd == Read)
                            {
                                page_table[location]->busy = 1;

                                event->operation = DIR_TLB_REFILL_WAIT;
                                event->page_entry_state = page_table[location]->state;
                                event->when = sim_cycle + PAGE_TABLE_SEARCH;
                                event->startCycle = sim_cycle;
                                event->processingDone = 0;
                                dir_eventq_insert(event);
                            }
                            else
                            {
                                // it must be a recovery @ fanglei
                                sr_page_num--;
                                sw_page_num++;
                                page_table[location]->busy = 1;
                                page_table[location]->state = SHARED_WRITE;
                                // braodcast
                                int i;
                                for (i=0;i<numcontexts;i++)
                                {
                                    struct DIRECTORY_EVENT *new_event = allocate_event(0);
                                    if(new_event == NULL)
                                        panic("Out of Virtual Memory");
                                    new_event->operation = DIR_TLB_RECOVERY_WAIT;
                                    new_event->addr = event->addr;
                                    new_event->src1 = 1;
                                    new_event->src2 = 0;
                                    new_event->des1 = i/mesh_size+MEM_LOC_SHIFT;
                                    new_event->des2 = i%mesh_size;
                                    new_event->startCycle = sim_cycle;
                                    new_event->processingDone = 0;
                                    new_event->parent = event;
                                    event->childCount++;
                                    new_event->when = sim_cycle + PAGE_TABLE_SEARCH;
                                    dir_eventq_insert(new_event);
                                }
                            }
                            break;
                        case SHARED_WRITE:
                            page_table[location]->busy = 1;

                            event->operation = DIR_TLB_REFILL_WAIT;
                            event->page_entry_state = page_table[location]->state;
                            event->when = sim_cycle + PAGE_TABLE_SEARCH;
                            event->startCycle = sim_cycle;
                            event->processingDone = 0;
                            dir_eventq_insert(event);
                            break;
                        default:
                            panic("the page table state error\n");
                    }
                }
            }
            break;
        case MISS_IL1       :   // level 1 inst cache miss request arrive at level 2 cache node @ fanglei
            dcache2_access++;
#ifdef CENTRALIZED_L2

#else
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache2_access++;
#endif
            event->parent_operation = event->operation;
            if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
            {/* Hit */
                if (dl2_blk->way_prev && dl2->policy == LRU)
                    update_way_list (&dl2->sets[dl2_set], dl2_blk, Head);

                if(dl2_blk->dir_state[block_offset] == DIR_TRANSITION)
                    panic("DIR: This event could not be in transition");

                if(dl2_blk->dir_sharer[block_offset][0] != 0 || dl2_blk->dir_sharer[block_offset][1] != 0 || dl2_blk->dir_sharer[block_offset][2] != 0 || dl2_blk->dir_sharer[block_offset][3] != 0 || /*dl2_blk->state != MESI_SHARED ||*/ (dl2_blk->status & CACHE_BLK_DIRTY))
                    panic("DIR: IL1 block can not have sharers list or modified");

                if(collect_stats)
                {
                    dl2->data_access ++;
                    dl2->hits ++;
                }
                event->operation = ACK_DIR_IL1;
                event->src1 += event->des1;
                event->des1 = event->src1 - event->des1;
                event->src1 = event->src1 - event->des1;
                event->src2 += event->des2;
                event->des2 = event->src2 - event->des2;
                event->src2 = event->src2 - event->des2;
                event->processingDone = 0;
                event->startCycle = sim_cycle;
                event->parent = NULL;
                event->blk_dir = NULL;
                event->arrival_time = sim_cycle - event->arrival_time;
                scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                dir_eventq_insert(event);
                return 1;
            }
            else
            {   /* L2 Miss */
                matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                event->L2miss_flag = 1;
                event->L2miss_stated = sim_cycle;
                event->blk_dir = NULL;
                if(!matchnum)
                { /* No match in L2 MSHR */
                    if(!cache_dl2->mshr->freeEntries)
                    {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                        if(!last_L2_mshr_full)
                        {
                            L2_mshr_full ++;
                            last_L2_mshr_full = sim_cycle;
                        }
                    if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                            //dir_fifo_nack[event->des1*mesh_size+event->des2]
                            event->src1 += event->des1;
                            event->des1 = event->src1 - event->des1;
                            event->src1 = event->src1 - event->des1;
                            event->src2 += event->des2;
                            event->des2 = event->src2 - event->des2;
                            event->src2 = event->src2 - event->des2;
                            event->operation = NACK;
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(event);
                            return 1;
                        }
                        else
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                            event->when = sim_cycle + RETRY_TIME;
                            flip_counter ++;
                            event->flip_flag = 1;
                            dir_eventq_insert(event);
                            return 1;
                        }
                    }
                    else
                    {
                        dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                        if(last_L2_mshr_full)
                        {
                            Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                            last_L2_mshr_full = 0;
                        }
                    }
                    if(collect_stats)
                        cache_dl2->misses++;
                    event->mshr_time = sim_cycle;
                    struct DIRECTORY_EVENT *new_event;
                    new_event = allocate_new_event(event);
                    new_event->operation = MEM_READ_REQ;
                    new_event->cmd = Write;     // just fot counter, cmd does not work; @ fanglei
                    event->when = sim_cycle + WAIT_TIME;
                    event->operation = WAIT_MEM_READ;
                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                    dir_eventq_insert(new_event);
                    event->startCycle = sim_cycle;
                    event_created = event;
                    MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                    return 1;
                }
                else
                {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                    if(collect_stats)
                        cache_dl2->in_mshr ++;

                    event_list_insert(event, matchnum);
                    return 1;
                }
            }

        case MISS_READ      :
            dcache2_access++;
#ifdef CENTRALIZED_L2

#else
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache2_access++;
#endif
            event->parent_operation = event->operation;

            // ms_blk; we search s_blk first and then m_blk; @ fanglei
            if(findCacheStatus(ml2, ml2_sub_set, ml2_sub_tag, ml2_hindex, &ml2_blk))
            {
                if(ml2_blk->qblk_set_shift == 0)
                {
                    read_S_hit_num++;
                    ml2_hit = 1;
                    ml2_tag = ml2_sub_tag;
                    ml2_set = ml2_sub_set;
                }
                else
                    ml2_hit = 0;
            }
            else if(findCacheStatus(ml2, ml2_mul_set, ml2_mul_tag, ml2_hindex, &ml2_blk))
            {
                if(ml2_blk->qblk_set_shift == 1)
                {
                    read_Q_hit_num++;
                    mb_pain_count++;
                    ml2_hit = 1;
                    ml2_tag = ml2_mul_tag;
                    ml2_set = ml2_mul_set;
                }
                else
                    ml2_hit = 0;
            }
            else
                ml2_hit = 0;

            if(ml2_hit)
            {
                if ( event->tlb_state != SHARED_WRITE )
                    printf("meta only for SHARED_WRITE\n");
                // update the LRU information @ fanglei
                if (ml2_blk->way_prev && ml2->policy == LRU)
                    update_way_list (&ml2->sets[ml2_set], ml2_blk, Head);
                // meta is busy or not @ fanglei
                if (ml2_blk->dir_state[block_offset] == DIR_TRANSITION)
                {
                    if(ml2_blk->qblk_set_shift)
                    {
                        read_Q_hit_num--;
                        mb_pain_count--;
                    }
                    else
                        read_S_hit_num--;
                    event->isReqL2Trans = 1;
                    if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                    {
                        event->src1 += event->des1;
                        event->des1 = event->src1 - event->des1;
                        event->src1 = event->src1 - event->des1;
                        event->src2 += event->des2;
                        event->des2 = event->src2 - event->des2;
                        event->src2 = event->src2 - event->des2;
                        event->operation = NACK;
                        event->processingDone = 0;
                        event->startCycle = sim_cycle;
                        scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(event);
                        if(collect_stats)
                            ml2->dir_access ++;
                        return 1;
                    }
                    else
                    {
                        event->when = sim_cycle + RETRY_TIME;
                        flip_counter ++;
                        event->flip_flag = 1;
                        dir_eventq_insert(event);
                        if(collect_stats)
                            ml2->dir_access ++;
                        return 1;
                    }
                }
                
                if ( (ml2_blk->dir_sharer[block_offset][tempID/64] & ((unsigned long long int)1 << event->tempID )) && (!ml2_blk->qblk_set_shift) && (!multi_blk_adap_en) && (!ml2_blk->all_shared_en) )
                    printf("read miss but in sharer\n");
                if( !ml2_blk->qblk_set_shift && !ml2_blk->all_shared_en)
                {
                    ml2_blk->dir_sharer[block_offset][tempID/64] &= ~ ((unsigned long long int)1 << event->tempID );
                    if( Is_Shared(ml2_blk->dir_sharer[block_offset]) == 1 )
                        ml2_blk->dir_blk_state[block_offset] = MESI_EXCLUSIVE;
                }

                // MISS_READ && ml2 hit && Exclusive Or Dirty @ fanglei
                if(IsExclusiveOrDirty(ml2_blk->dir_sharer[block_offset], tempID, &Threadid) && ml2_blk->dir_blk_state[block_offset] != MESI_SHARED)
                {
                    if( vector_num == 0 )
                        pvc_nv_conflict_num++;
                    else if(ml2_blk->ptr_en)
                    {
                        if(ml2_blk->all_shared_en)
                            panic("exclusive pointer can not be all_shared\n");

                        int vector_available;
                        int sharer_num;
                        int temp_threadid;
                        struct cache_blk_t *vector_blk;
                        
                        vector_available = vector_available_func(&vector_blk, ml2_set);
                        
                        if(!vector_available)   // all the vector_blks are in transition; @ fanglei;
                        {
                            PVC_flip_num++;
                            if(ml2_blk->qblk_set_shift)
                            {
                                read_Q_hit_num--;
                                mb_pain_count--;
                            }
                            else
                                read_S_hit_num--;
                            event->isReqL2Trans = 1;
                            if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                            {
                                event->src1 += event->des1;
                                event->des1 = event->src1 - event->des1;
                                event->src1 = event->src1 - event->des1;
                                event->src2 += event->des2;
                                event->des2 = event->src2 - event->des2;
                                event->src2 = event->src2 - event->des2;
                                event->operation = NACK;
                                event->processingDone = 0;
                                event->startCycle = sim_cycle;
                                scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                dir_eventq_insert(event);
                                if(collect_stats)
                                    ml2->dir_access ++;
                                return 1;
                            }
                            else
                            {
                                event->when = sim_cycle + RETRY_TIME;
                                flip_counter ++;
                                event->flip_flag = 1;
                                dir_eventq_insert(event);
                                if(collect_stats)
                                    ml2->dir_access ++;
                                return 1;
                            }                            
                        }
                        else    // one of the vector_blks will exchange with the pointer_blk; @ fanglei;
                        {
                            sharer_num = Is_Shared(vector_blk->dir_sharer[block_offset]);
                            
                            PVC_exchange_num++;
                            PVC_exch_vec_shr_distr[sharer_num]++;

                            if( sharer_num < 2 ) // the vector_blk is exclusive and need no invalidation; @ fanglei1
                            {
                                PVC_dowm_no_inv_num++;

                                ml2_blk->ptr_en = 0;

                                vector_blk->ptr_en = 1;
                                if(vector_blk->all_shared_en)
                                    panic("vector_blk all_shared_en illegal\n");
                                if( vector_blk->dir_blk_state[0] == MESI_SHARED )
                                    panic("state error\n");
                            }
                            else if( sharer_num == mesh_size*mesh_size ) // the vector_blk is all_shared and need no invalidation; @ fanglei1
                            {
                                PVC_up_no_add_num++;

                                ml2_blk->ptr_en = 0;

                                vector_blk->ptr_en = 1;
                                vector_blk->all_shared_en = 1;
                                if(vector_blk->dir_sharer[0][0] != (unsigned long long int)all_sharer)
                                    panic("all_shared_en dir_sharer errpr\n");
                                if( vector_blk->dir_blk_state[0] != MESI_SHARED )
                                    panic("state error\n");
                            }
                            else    // the vector can not be changed into a pointer without lose of sharing information; @ fanglei
                            {
                                if(sharer_num > sharer_threshold)    // the vector is changed into all_shared; @ fanglei
                                {
                                    PVC_up_add_num++;
                                    PVC_up_add_shr_num += mesh_size*mesh_size-sharer_num;
                                    PVC_up_add_shr_distr[mesh_size*mesh_size-sharer_num]++;

                                    ml2_blk->ptr_en = 0;

                                    vector_blk->ptr_en = 1;
                                    vector_blk->all_shared_en = 1;
                                    vector_blk->dir_sharer[0][0] = (unsigned long long int)all_sharer;
                                    if( vector_blk->dir_blk_state[0] != MESI_SHARED )
                                        panic("state error\n");
                                }
                                else    // the vector is changed into exclusive; @ fanglei
                                {
                                    PVC_dowm_inv_num++;
                                    PVC_dowm_inv_shr_num += sharer_num-1;
                                    PVC_dowm_inv_shr_distr[sharer_num-1]++;

                                    ml2_blk->ptr_en = 0;

                                    int ml2_dir_blk_state = vector_blk->dir_blk_state[0];
                                    int survive_en = 1, survivor = -1;    // only a sharer can survive; @ fanglei
                                    struct DIRECTORY_EVENT *collect_event = allocate_event(0);
                                    if(collect_event == NULL)
                                        panic("Out of Virtual Memory");

                                    vector_blk->ptr_en = 1;
                                    if(vector_blk->all_shared_en)
                                        panic("vector_blk all_shared_en illegal\n");
                                    vector_blk->all_shared_en = 0;
                                    vector_blk->dir_blk_state[0] = MESI_EXCLUSIVE;
                                    vector_blk->dir_state[0] = DIR_TRANSITION;
                                    vector_blk->ptr_cur_event[0] = collect_event;
                                    if (vector_blk->qblk_set_shift)
                                        vector_blk->addr = (vector_blk->tagid.tag << (cache_ml2->tag_shift+qblk_set_shift)) + (ml2_set << (cache_ml2->set_shift+qblk_set_shift));
                                    else
                                        vector_blk->addr = (vector_blk->tagid.tag << cache_ml2->tag_shift) + (ml2_set << cache_ml2->set_shift);

                                    collect_event->childCount = 0;
                                    collect_event->individual_childCount[0] = 0;
                                    collect_event->sharer_num = 1;     // indicate roll back or write invalid @ fanglei
                                    collect_event->blk_dir = vector_blk;
                                    collect_event->operation = VEC_DEGRADE;

                                    for (temp_threadid = 0; temp_threadid < numcontexts; temp_threadid++)
                                    {
                                        if(((vector_blk->dir_sharer[0][temp_threadid/64]) & ((unsigned long long int)1 << (temp_threadid%64))) == ((unsigned long long int)1 << (temp_threadid%64)))
                                        {
                                            if(survive_en)
                                            {
                                                survivor = temp_threadid;
                                                survive_en = 0;
                                                continue;
                                            }
                                            struct DIRECTORY_EVENT *new_event = allocate_event(0);
                                            if(new_event == NULL)
                                                panic("Out of Virtual Memory");

                                            // the ml2_blk is m_blk or s_blk; @ fanglei
                                            new_event->qblk_set_shift = vector_blk->qblk_set_shift;
                                            new_event->ml2_dir_blk_state = ml2_dir_blk_state;
                                            new_event->addr = vector_blk->addr;
                                            new_event->operation = INV_MSG_WRITE;
                                            new_event->parent = collect_event;
                                            new_event->blk_dir = vector_blk;                                            
                                            new_event->tempID = event->tempID;
                                            new_event->cp = event->cp;
                                            new_event->nbytes = event->cp->bsize;
                                            new_event->src1 = event->des1;
                                            new_event->src2 = event->des2;
                                            new_event->des1 = temp_threadid/mesh_size+MEM_LOC_SHIFT;
                                            new_event->des2 = temp_threadid%mesh_size;

                                            new_event->isPrefetch = 0;
                                            new_event->conf_bit = -1;
                                            new_event->req_time = 0;
                                            new_event->input_buffer_full = 0;
                                            new_event->pending_flag = 0;
                                            new_event->op = collect_event->op;
                                            new_event->vp = NULL;
                                            new_event->udata = NULL;
                                            new_event->rs = NULL;
                                            new_event->spec_mode = 0;
                                            new_event->data_reply = 1;

                                            /*go to network*/
                                            new_event->processingDone = 0;
                                            new_event->startCycle = sim_cycle;
                                            scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, 0);
                                            dir_eventq_insert(new_event);

                                            collect_event->childCount++;
                                            collect_event->individual_childCount[0] ++;
                                        }
                                    }
                                    if(survivor == -1)
                                        panic("survivor error\n");
                                    vector_blk->dir_sharer[0][survivor/64] = ((unsigned long long int)1 << (survivor%64));
                                }
                            }
                        }
                    }

                    if(collect_stats)
                        involve_4_hops ++;
                    ml2_blk->dir_state[block_offset] = DIR_TRANSITION;
                    ml2_blk->ptr_cur_event[block_offset] = event;
                    event->blk_dir = ml2_blk;
                    struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
                    new_event->parent = event;
                    if(new_event == NULL)
                        panic("Out of Virtual Memory");
                    if(collect_stats)
                        ml2->dir_access ++;
                    // dd related @ fanglei
                    if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                    {
                        event->shr_data_req = 0;
                        new_event->shr_data_req = 0;
                        dl2_blk->dir_state[block_offset] = DIR_TRANSITION;
                    }
                    else
                    {
                        event->shr_data_req = 1;
                        new_event->shr_data_req = 1;
                    }
                    new_event->input_buffer_full = 0;
                    new_event->req_time = 0;
                    new_event->conf_bit = -1;
                    new_event->pending_flag = 0;
                    new_event->op = event->op;
                    new_event->isPrefetch = 0;
                    new_event->operation = INV_MSG_READ;

                    // the ml2_blk is m_blk or s_blk; @ fanglei
                    new_event->qblk_set_shift = ml2_blk->qblk_set_shift;

                    new_event->src1 = event->des1;
                    new_event->src2 = event->des2;
                    new_event->des1 = Threadid/mesh_size+MEM_LOC_SHIFT;
                    new_event->des2 = Threadid%mesh_size;
                    new_event->processingDone = 0;
                    new_event->startCycle = sim_cycle;
                    new_event->tempID = event->tempID;
                    new_event->resend = 0;
                    new_event->blk_dir = ml2_blk;
                    new_event->cp = event->cp;
                    new_event->store_cond = event->store_cond;
                    new_event->addr = event->addr;
                    new_event->vp = NULL;
                    new_event->nbytes = event->cp->bsize;
                    new_event->udata = NULL;
                    new_event->cmd = Write;
                    new_event->rs = event->rs;
                    new_event->started = sim_cycle;
                    new_event->spec_mode = 0;
                    new_event->L2miss_flag = event->L2miss_flag;
                    new_event->data_reply = 1;
                    event->childCount++;
                    event->dirty_flag = 1;
                    if(event->rs)
                        if(event->rs->op == LDL_L)
                            load_link_exclusive ++;

                    /*go to network*/
                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                    dir_eventq_insert(new_event);
                    return 1;
                }
                // MISS_READ && ml2 hit && non sharer @ fanglei
                else if ( !IsShared(ml2_blk->dir_sharer[block_offset], tempID) )
                {
                    if(ml2_blk->ptr_en && ml2_blk->all_shared_en)
                        panic("miss read mesi_invalid ptr_en all_shared_en error \n");
                    // non shared && hit in the L2 data cache @ fanglei
                    if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                    {
                        ml2->data_access ++;
                        involve_2_hops ++;
                        if(event->rs)
                            if(event->rs->op == LDL_L)
                                load_link_shared ++;
                        ml2_blk->dir_state[block_offset] = DIR_STABLE;
                        ml2_blk->ptr_cur_event[block_offset] = NULL;
                        ml2_blk->exclusive_time[block_offset] = sim_cycle;

                        int vector_count;
                        vector_count = vector_count_func(ml2_blk);
//                        if(Is_Shared(ml2_blk->dir_sharer[block_offset]) == 1)
//                        {
//                            panic("vector error 1\n");
//                        }

                        PVC_vector_distr_record_num++;
                        PVC_vector_distr[vector_count]++;
                        cache_ml2->sets[ml2_set].PVC_vector_distr_record_num++;
                        cache_ml2->sets[ml2_set].PVC_vector_distr[vector_count]++;

                        if(!md_text_addr(addr, tempID))
                        {
                            ml2_blk->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64);
                            ml2_blk->dir_blk_state[block_offset] = MESI_EXCLUSIVE;
                        }
                        else
                            printf("directory is for data segment\n");

                        event->operation = ACK_DIR_READ_EXCLUSIVE;
                        event->reqAckTime = sim_cycle;
                        event->src1 += event->des1;
                        event->des1 = event->src1 - event->des1;
                        event->src1 = event->src1 - event->des1;
                        event->src2 += event->des2;
                        event->des2 = event->src2 - event->des2;
                        event->src2 = event->src2 - event->des2;
                        event->processingDone = 0;
                        event->startCycle = sim_cycle;
                        event->parent = NULL;
                        event->blk_dir = NULL;
                        event->arrival_time = sim_cycle - event->arrival_time;
                        scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                        dir_eventq_insert(event);
                        return 1;
                    }
                    // MISS_READ && ml2 hit && non shared && dl2 miss @ fanglei
                    else
                    {
                        ml2_blk->dir_state[block_offset] = DIR_TRANSITION;          // data miss, we prevent from replacing this meta entry; @ fanglei
                        if(ml2_blk->qblk_set_shift)
                        {
                            read_Q_hit_num--;
                            mb_pain_count--;
                        }
                        else
                            read_S_hit_num--;
                        matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                        event->L2miss_flag = 1;
                        event->L2miss_stated = sim_cycle;
                        if(event->isSyncAccess)
                            Sync_L2_miss ++;
                        if(!matchnum)
                        { /* No match in L2 MSHR */
                            if(!cache_dl2->mshr->freeEntries)
                            {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                                if(!last_L2_mshr_full)
                                {
                                    L2_mshr_full ++;
                                    last_L2_mshr_full = sim_cycle;
                                }
                                if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                                {
                                    dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                                    //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                    event->src1 += event->des1;
                                    event->des1 = event->src1 - event->des1;
                                    event->src1 = event->src1 - event->des1;
                                    event->src2 += event->des2;
                                    event->des2 = event->src2 - event->des2;
                                    event->src2 = event->src2 - event->des2;
                                    event->operation = NACK;
                                    ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                    event->processingDone = 0;
                                    event->startCycle = sim_cycle;
                                    scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                    dir_eventq_insert(event);
                                    return 1;
                                }
                                else
                                {
                                    ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                    dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                                    event->when = sim_cycle + RETRY_TIME;
                                    event->flip_flag = 1;
                                    flip_counter ++;
                                    dir_eventq_insert(event);
                                    return 1;
                                }
                            }
                            else
                            {
                                dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                                if(last_L2_mshr_full)
                                {
                                    Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                    last_L2_mshr_full = 0;
                                }
                            }
                            if(collect_stats)
                            {
                                cache_dl2->misses ++;
                                cache_dl2->dmisses ++;
                                involve_2_hops_miss ++;
                            }
                            event->reqAtDirTime = sim_cycle;
                            event->mshr_time = sim_cycle;
                            event->blk_dir = ml2_blk;
                            struct DIRECTORY_EVENT *new_event;
                            new_event = allocate_new_event(event);
                            new_event->operation = MEM_READ_REQ;
                            new_event->cmd = Read;
                            event->when = sim_cycle + WAIT_TIME;
                            event->operation = WAIT_MEM_READ;
                            scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(new_event);
                            event->startCycle = sim_cycle;
                            event_created = event;
                            MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                            return 1;
                        }
                        else
                        {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                            event->isReqL2SecMiss = 1;
                            if(collect_stats)
                            {
                                cache_dl2->in_mshr ++;
                                cache_dl2->din_mshr ++;
                            }
                            event_list_insert(event, matchnum);
                            ml2_blk->dir_state[block_offset] = DIR_STABLE;
                            return 1;
                        }
                    }
                }
                // MISS_READ && ml2 hit && shared; if L2 dl2 hit, it is done; otherwise we can request data from another owner @ fanglei
                else
                {
                    if(ml2_blk->ptr_en && !ml2_blk->all_shared_en)
                        panic("miss read mesi_shared ptr_en all_shared_en error \n");
                    // MISS_READ && ml2 hit && shared && dl2 hit @ fanglei
                    if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                    {
                        ml2->data_access ++;
                        involve_2_hops ++;
                        if(event->rs)
                            if(event->rs->op == LDL_L)
                                load_link_shared ++;
                        ml2_blk->dir_state[block_offset] = DIR_STABLE;
                        ml2_blk->ptr_cur_event[block_offset] = NULL;
                        ml2_blk->exclusive_time[block_offset] = sim_cycle;

                        int vector_count;
                        vector_count = vector_count_func(ml2_blk);
//                        if(Is_Shared(ml2_blk->dir_sharer[block_offset]) == 1)
//                        {
//                            panic("vector error 2\n");
//                        }

                        PVC_vector_distr_record_num++;
                        PVC_vector_distr[vector_count]++;
                        cache_ml2->sets[ml2_set].PVC_vector_distr_record_num++;
                        cache_ml2->sets[ml2_set].PVC_vector_distr[vector_count]++;

                        if(!md_text_addr(addr, tempID))
                        {
                            ml2_blk->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64) | ml2_blk->dir_sharer[block_offset][tempID/64];
                            ml2_blk->dir_blk_state[block_offset] = MESI_SHARED;
                        }
                        else
                            printf("directory is for data segment\n");

                        event->operation = ACK_DIR_READ_SHARED;
                        event->reqAckTime = sim_cycle;
                        event->src1 += event->des1;
                        event->des1 = event->src1 - event->des1;
                        event->src1 = event->src1 - event->des1;
                        event->src2 += event->des2;
                        event->des2 = event->src2 - event->des2;
                        event->src2 = event->src2 - event->des2;
                        event->processingDone = 0;
                        event->startCycle = sim_cycle;
                        event->parent = NULL;
                        event->blk_dir = NULL;
                        event->arrival_time = sim_cycle - event->arrival_time;
                        scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                        dir_eventq_insert(event);
                        return 1;
                    }
                    // MISS_READ && ml2 hit && shared && dl2 miss, we request data from one sharer; @ fanglei
                    else
                    {
                        // dd related @ fanglei
                        ml2_blk->dir_state[block_offset] = DIR_TRANSITION;
                        if (!event->shr_data_req)
                        {
                            shr_data_req_initial_num++;
                            event->data_req_sharer = ml2_blk->dir_sharer[block_offset][tempID/64];
                        }
                        shr_data_req_total_num++;
                        int dest = -1;
                        int i;
                        for(i=0;i<numcontexts;i++)
                        {
                            if ( (((event->data_req_sharer>>i)&1)==1) && (i!=event->tempID) )
                            {
                                dest = i;
                                event->data_req_sharer &= ~(1<<i);
                                break;
                            }
                        }
                        if(ml2_blk->all_shared_en)
                        {
                            PVC_read_all_shr_num++;
                            if(shr_read_en)
                                PVC_rfs_all_shr_num++;
                        }
                        if( !shr_read_en || ml2_blk->all_shared_en )
                            dest = -1;
                        if ( dest == -1 )
                        // require data from memory; @ fanglei
                        {
                            shr_data_req_fail_num++;
                            ml2_blk->dir_state[block_offset] = DIR_TRANSITION;     // data miss, we prevent from replacing this meta entry; @ fanglei
                            if(ml2_blk->qblk_set_shift)
                            {
                                read_Q_hit_num--;
                                mb_pain_count--;
                            }
                            else
                                read_S_hit_num--;
                            matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                            event->L2miss_flag = 1;
                            event->L2miss_stated = sim_cycle;
                            if(event->isSyncAccess)
                                Sync_L2_miss ++;
                            if(!matchnum)
                            { /* No match in L2 MSHR */
                                if(!cache_dl2->mshr->freeEntries)
                                {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                                    if(!last_L2_mshr_full)
                                    {
                                        L2_mshr_full ++;
                                        last_L2_mshr_full = sim_cycle;
                                    }
                                    if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                                    {
                                        dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                                        //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                        event->src1 += event->des1;
                                        event->des1 = event->src1 - event->des1;
                                        event->src1 = event->src1 - event->des1;
                                        event->src2 += event->des2;
                                        event->des2 = event->src2 - event->des2;
                                        event->src2 = event->src2 - event->des2;
                                        event->operation = NACK;
                                        ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                        event->processingDone = 0;
                                        event->startCycle = sim_cycle;
                                        scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                        dir_eventq_insert(event);
                                        return 1;
                                    }
                                    else
                                    {
                                        ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                        dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                                        event->when = sim_cycle + RETRY_TIME;
                                        event->flip_flag = 1;
                                        flip_counter ++;
                                        dir_eventq_insert(event);
                                        return 1;
                                    }
                                }
                                else
                                {
                                    dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                                    if(last_L2_mshr_full)
                                    {
                                        Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                        last_L2_mshr_full = 0;
                                    }
                                }
                                if(collect_stats)
                                {
                                    cache_dl2->misses ++;
                                    cache_dl2->dmisses ++;
                                    involve_2_hops_miss ++;
                                }
                                event->reqAtDirTime = sim_cycle;
                                event->mshr_time = sim_cycle;
                                event->blk_dir = ml2_blk;
                                struct DIRECTORY_EVENT *new_event;
                                new_event = allocate_new_event(event);
                                new_event->operation = MEM_READ_REQ;
                                new_event->cmd = Read;
                                event->when = sim_cycle + WAIT_TIME;
                                event->operation = WAIT_MEM_READ;
                                scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                                dir_eventq_insert(new_event);
                                event->startCycle = sim_cycle;
                                event_created = event;
                                MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                                return 1;
                            }
                            else
                            {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                                event->isReqL2SecMiss = 1;
                                if(collect_stats)
                                {
                                    cache_dl2->in_mshr ++;
                                    cache_dl2->din_mshr ++;
                                }
                                event_list_insert(event, matchnum);
                                ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                return 1;
                            }
                        }
                        event->shr_data_req = 1;
                        event->operation = DIR_SHR_READ;

                        // the ml2_blk is m_blk or s_blk; @ fanglei
                        event->qblk_set_shift = ml2_blk->qblk_set_shift;

                        event->src1 = event->des1;
                        event->src2 = event->des2;
                        event->des1 = dest/mesh_size+MEM_LOC_SHIFT;
                        event->des2 = dest%mesh_size;
                        event->processingDone = 0;
                        event->startCycle = sim_cycle;
                        event->parent = NULL;
                        event->blk_dir = ml2_blk;
                        event->arrival_time = sim_cycle - event->arrival_time;
                        scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                        dir_eventq_insert(event);
                        return 1;
                    }
                }
            }   // end of "MISS_READ && ml2 hit" handler; @ fanglei
            else if ( event->tlb_state == SHARED_WRITE )
            {
                // we always allocate a m_blk for read miss; @ fanglei
                ml2_tag = ml2_mul_tag;
                ml2_set = ml2_mul_set;
                read_Q_init_num++;

                switch (ml2->policy)
                {
                    int i;
                    int bindex;
                    case LRU:
                    case FIFO:
                        i = ml2->assoc;
                        do {
                            if(i == 0)  break;
                            i--;
                            repl = ml2->sets[ml2_set].way_tail;
                            update_way_list (&ml2->sets[ml2_set], repl, Head);
                        }
                        while(repl_dir_state_check(repl->dir_state));    // The "do while" searches for the least recently used block that is not in "DIR_TRANSITION" state to be replaced.  @ fanglei

                        if ( repl_ad_en && ( repl->qblk_set_shift == 1 ) )
                        {
                            struct cache_blk_t *bak;
                            bak = repl;
                            
                            i = ml2->assoc;
                            do {
                                if(i == 0)  break;
                                i--;
                                repl = ml2->sets[ml2_set].way_tail;
                                update_way_list (&ml2->sets[ml2_set], repl, Head);
                            }
                            while(repl_dir_state_check(repl->dir_state) || (repl->qblk_set_shift == 1) );
                            
                            if ( repl_dir_state_check(repl->dir_state) || (repl->qblk_set_shift == 1))
                            {
                                repl = bak;
                                update_way_list (&ml2->sets[ml2_set], repl, Head);
                            }
                        }

                        if(repl_dir_state_check(repl->dir_state))
                        {       // If every block in this set is in "DIR_TRANSITION" state, it means the refill can not be done in this cycle. The p_event has to wait a "RETRY_TIME" time, and then request for the data again. @ fanglei
                            flip_counter ++;
                            event->flip_flag = 1;
                            event->when = sim_cycle + RETRY_TIME;
                            dir_eventq_insert(event);
                            return 1;
                        }
                        break;
                    case Random:
                        bindex = myrand () & (dl2->assoc - 1);
                        repl = CACHE_BINDEX (dl2, dl2->sets[dl2_set].blks, bindex);
                        break;
                    default:
                        panic ("bogus replacement policy");
                }

                int i=0, iteration = 1;
                for(i=0; i<(cache_dl2->set_shift - cache_dl1[0]->set_shift); i++)
                    iteration = iteration * 2;
                int m;
                if (repl->status & CACHE_BLK_VALID)
                {  /* replace the replacement line in L2 cache */
                    if( repl->ptr_en && repl->all_shared_en && (repl->dir_sharer[0][0] != (unsigned long long int)all_sharer) )
                        panic("recall ptr_en all_shared_en error \n");

                    if (collect_stats)
                        ml2->replacements++;

                    int ml2_dir_blk_state;
                    int new_vc = 0;
                    int flag = 0, sharer_num = 0;
                    for (i=0; i<iteration; i++)
                        if(Is_Shared(repl->dir_sharer[i]))
                            sharer_num ++;

                    event->childCount = 0;
                    event->sharer_num = 0;

                    for(i=0; i<iteration; i++)
                    {
                        if(Is_Shared(repl->dir_sharer[i]))
                        {  /* send invalidation messages to L1 cache of sharers*/
                            /* Invalidate multiple sharers */
                            
                            ml2_dir_blk_state = repl->dir_blk_state[i];
                            if (    (ml2_dir_blk_state ==  MESI_SHARED) && (Is_Shared(repl->dir_sharer[i]) < 2) 
                                 || (ml2_dir_blk_state ==  MESI_MODIFIED) && (Is_Shared(repl->dir_sharer[i]) != 1) 
                                 || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) && (Is_Shared(repl->dir_sharer[i]) != 1) 
                                 || (ml2_dir_blk_state ==  MESI_INVALID)
                               )
                                printf("state conflicts with sharer vector\n");
                        
                            for(m = 0; m<iteration; m++)
                            {
                                repl->dir_state[m] = DIR_TRANSITION;
                                repl->dir_blk_state[m] = MESI_INVALID;
                            }
                            repl->ptr_cur_event[i] = event;
                            if (repl->qblk_set_shift)
                                repl->addr = (repl->tagid.tag << (cache_ml2->tag_shift+qblk_set_shift)) + (ml2_set << (cache_ml2->set_shift+qblk_set_shift));
                            else
                                repl->addr = (repl->tagid.tag << cache_ml2->tag_shift) + (ml2_set << cache_ml2->set_shift) + (i << cache_dl1[0]->set_shift);
                            event->individual_childCount[i] = 0;
                            for (Threadid = 0; Threadid < numcontexts; Threadid++)
                            {
                                if(((repl->dir_sharer[i][Threadid/64]) & ((unsigned long long int)1 << (Threadid%64))) == ((unsigned long long int)1 << (Threadid%64)))
                                {
                                    recall_inv_num++;
                                    if(collect_stats)
                                        cache_ml2->invalidations ++;
                                    struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
                                    if(new_event == NULL)
                                        panic("Out of Virtual Memory");
                                    involve_4_hops_miss ++;
                                    new_event->conf_bit = -1;
                                    new_event->req_time = 0;
                                    new_event->input_buffer_full = 0;
                                    new_event->pending_flag = 0;
                                    new_event->op = event->op;
                                    new_event->operation = INV_MSG_WRITE;

                                    // the ml2_blk is m_blk or s_blk; @ fanglei
                                    new_event->qblk_set_shift = repl->qblk_set_shift;
                                    new_event->ml2_dir_blk_state = ml2_dir_blk_state;

                                    new_event->isPrefetch = 0;
                                    new_event->tempID = event->tempID;
                                    new_event->L2miss_flag = event->L2miss_flag;
                                    new_event->resend = 0;
                                    new_event->blk1 = repl;
                                    new_event->addr = repl->addr;
                                    new_event->cp = event->cp;
                                    new_event->vp = NULL;
                                    new_event->nbytes = event->cp->bsize;
                                    new_event->udata = NULL;
                                    new_event->rs = NULL;
                                    new_event->spec_mode = 0;
                                    new_event->src1 = event->des1;
                                    new_event->src2 = event->des2;
                                    new_event->des1 = Threadid/mesh_size+MEM_LOC_SHIFT;
                                    new_event->des2 = Threadid%mesh_size;
                                    new_event->processingDone = 0;
                                    new_event->startCycle = sim_cycle;
                                    new_event->parent = event;
                                    new_event->blk_dir = repl;
                                    new_event->data_reply = 1;
                                    event->sharer_num = sharer_num;     // indicate roll back or write invalid @ fanglei
                                    event->blk_dir = repl;
                                    event->childCount++;
                                    event->individual_childCount[i] ++;
                                    /*go to network*/
                                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, new_vc);

                                    dir_eventq_insert(new_event);
                                }
                            }
                            flag = 1;

                            if(repl->qblk_set_shift)
                            {
                                read_Q_init_QR_num++;
                                if(ml2_dir_blk_state ==  MESI_SHARED)
                                    read_Q_init_QR_s_num++;
                                else if ( (ml2_dir_blk_state ==  MESI_MODIFIED) || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) )
                                    read_Q_init_QR_ed_num++;
                            }
                            else
                                read_Q_init_SR_num++;
                            recall_num++;
                            if(repl->all_shared_en)
                            {
                                repl->all_shared_en = 0;
                                PVC_recall_all_shr_num++;
                            }

                            sharer_distr[event->childCount] ++;
                            IsNeighbor_sharer(repl->dir_sharer[i], tempID);
                        }
                    }
                    if(flag == 1)
                    {
                        // a later request can hit this blk @ fanglei
                        if(collect_stats)
                            dl2->replInv++;
                        repl->status = CACHE_BLK_VALID;
                        repl->addr = addr;
                        repl->tagid.tag = ml2_tag;
                        repl->tagid.threadid = tempID;
                        for(m=0; m<iteration; m++)
                        {
                            repl->dir_sharer[m][tempID/64] = 0;
                            repl->dir_blk_state[m] = MESI_INVALID;
                        }
                        if (qblk_set_shift)
                            repl->qblk_set_shift = 1;
                        else
                            repl->qblk_set_shift = 0;
                        return 1;
                    }
                }
                // no roll back, so the meta entry is valid now; @ fanglei
                for(m=0;m<iteration;m++)
                {
                    repl->ptr_cur_event[m] = NULL;
                    repl->dir_state[m] = DIR_STABLE;     // If block is invalid, the following operation is carried out.
                    repl->dir_sharer[m][tempID/64] = 0;
                    repl->dir_blk_state[m] = MESI_INVALID;
                    repl->dir_dirty[m] = -1;
                    repl->exclusive_time[m] = sim_cycle;
                }
                repl->status = CACHE_BLK_VALID;
                repl->ready = sim_cycle;
                repl->tagid.tag = ml2_tag;
                repl->tagid.threadid = tempID;
                if (qblk_set_shift)
                    repl->qblk_set_shift = 1;
                else
                    repl->qblk_set_shift = 0;
                repl->addr = addr;
                if (repl->way_prev && ml2->policy == LRU)
                    update_way_list(&ml2->sets[ml2_set], repl, Head);
                if (ml2->hsize)
                    link_htab_ent(ml2, &ml2->sets[ml2_set], repl);
                ml2->last_tagsetid.tag = 0;
                ml2->last_tagsetid.threadid = tempID;
                ml2->last_blk = repl;
                // non shared && hit in L2 data cache @ fanglei
                if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                {
                    ml2->data_access ++;
                    involve_2_hops ++;
                    if(event->rs)
                        if(event->rs->op == LDL_L)
                            load_link_shared ++;
                    repl->dir_state[block_offset] = DIR_STABLE;
                    repl->ptr_cur_event[block_offset] = NULL;
                    repl->exclusive_time[block_offset] = sim_cycle;

                    int vector_count;
                    vector_count = vector_count_func(repl);
//                    if(Is_Shared(repl->dir_sharer[block_offset]) == 1)
//                    {
//                        panic("vector error 3\n");
//                    }

                    PVC_vector_distr_record_num++;
                    PVC_vector_distr[vector_count]++;
                    cache_ml2->sets[ml2_set].PVC_vector_distr_record_num++;
                    cache_ml2->sets[ml2_set].PVC_vector_distr[vector_count]++;

                    if(!md_text_addr(addr, tempID))
                    {
                        repl->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64);
                        repl->dir_blk_state[block_offset] = MESI_EXCLUSIVE;
                    }
                    else
                        printf("directory is for data segment\n");

                    event->operation = ACK_DIR_READ_EXCLUSIVE;
                    event->reqAckTime = sim_cycle;
                    event->src1 += event->des1;
                    event->des1 = event->src1 - event->des1;
                    event->src1 = event->src1 - event->des1;
                    event->src2 += event->des2;
                    event->des2 = event->src2 - event->des2;
                    event->src2 = event->src2 - event->des2;
                    event->processingDone = 0;
                    event->startCycle = sim_cycle;
                    event->parent = NULL;
                    event->blk_dir = NULL;
                    event->arrival_time = sim_cycle - event->arrival_time;
                    scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                    dir_eventq_insert(event);
                    return 1;
                }
                // non shared && miss in L2 data cache @ fanglei
                else
                {
                    repl->dir_state[block_offset] = DIR_TRANSITION;     // data miss, we prevent from replacing this meta entry; @ fanglei
                    matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                    event->L2miss_flag = 1;
                    event->L2miss_stated = sim_cycle;
                    if(event->isSyncAccess)
                        Sync_L2_miss ++;
                    if(!matchnum)
                    { /* No match in L2 MSHR */
                        if(!cache_dl2->mshr->freeEntries)
                        {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                            if(!last_L2_mshr_full)
                            {
                                L2_mshr_full ++;
                                last_L2_mshr_full = sim_cycle;
                            }
                            if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                            {
                                dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                                //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                event->src1 += event->des1;
                                event->des1 = event->src1 - event->des1;
                                event->src1 = event->src1 - event->des1;
                                event->src2 += event->des2;
                                event->des2 = event->src2 - event->des2;
                                event->src2 = event->src2 - event->des2;
                                event->operation = NACK;
                                repl->dir_state[block_offset] = DIR_STABLE;
                                event->processingDone = 0;
                                event->startCycle = sim_cycle;
                                scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                dir_eventq_insert(event);
                                return 1;
                            }
                            else
                            {
                                repl->dir_state[block_offset] = DIR_STABLE;
                                dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                                event->when = sim_cycle + RETRY_TIME;
                                event->flip_flag = 1;
                                flip_counter ++;
                                dir_eventq_insert(event);
                                return 1;
                            }
                        }
                        else
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                            if(last_L2_mshr_full)
                            {
                                Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                last_L2_mshr_full = 0;
                            }
                        }
                        if(collect_stats)
                        {
                            cache_dl2->misses ++;
                            cache_dl2->dmisses ++;
                            involve_2_hops_miss ++;
                        }
                        event->reqAtDirTime = sim_cycle;
                        event->mshr_time = sim_cycle;
                        event->blk_dir = repl;
                        struct DIRECTORY_EVENT *new_event;
                        new_event = allocate_new_event(event);
                        new_event->operation = MEM_READ_REQ;
                        new_event->cmd = Read;
                        event->when = sim_cycle + WAIT_TIME;
                        event->operation = WAIT_MEM_READ;
                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(new_event);
                        event->startCycle = sim_cycle;
                        event_created = event;
                        MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                        return 1;
                    }
                    else
                    {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                        event->isReqL2SecMiss = 1;
                        if(collect_stats)
                        {
                            cache_dl2->in_mshr ++;
                            cache_dl2->din_mshr ++;
                        }
                        event_list_insert(event, matchnum);
                        repl->dir_state[block_offset] = DIR_STABLE;
                        return 1;
                    }
                }
            }

            // no need to access the meta; @ fanglei
            if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
            {/* Hit */
                if (dl2_blk->way_prev && dl2->policy == LRU)
                    update_way_list (&dl2->sets[dl2_set], dl2_blk, Head);
                /* See the status of directory */
                event->isReqL2Hit = 1;
                if(event->L2miss_flag && event->L2miss_stated)
                    event->L2miss_complete = sim_cycle;
                if(event->rs)
                    if(event->rs->op == LDL_L)
                        load_link_shared ++;
                event->operation = ACK_DIR_READ_EXCLUSIVE;      // no coherent information; @ fanglei
                event->src1 += event->des1;
                event->des1 = event->src1 - event->des1;
                event->src1 = event->src1 - event->des1;
                event->src2 += event->des2;
                event->des2 = event->src2 - event->des2;
                event->src2 = event->src2 - event->des2;
                event->processingDone = 0;
                event->startCycle = sim_cycle;
                event->parent = NULL;
                event->blk_dir = NULL;
                event->arrival_time = sim_cycle - event->arrival_time;
                scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                dir_eventq_insert(event);
                return 1;
            }
            else
            {   /* L2 Miss */
                matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                event->L2miss_flag = 1;
                event->L2miss_stated = sim_cycle;
                event->blk_dir = NULL;
                if(event->isSyncAccess)
                    Sync_L2_miss ++;
                if(!matchnum)
                { /* No match in L2 MSHR */
                    if(!cache_dl2->mshr->freeEntries)
                    {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                        if(!last_L2_mshr_full)
                        {
                            L2_mshr_full ++;
                            last_L2_mshr_full = sim_cycle;
                        }
                        if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                            //dir_fifo_nack[event->des1*mesh_size+event->des2]
                            event->src1 += event->des1;
                            event->des1 = event->src1 - event->des1;
                            event->src1 = event->src1 - event->des1;
                            event->src2 += event->des2;
                            event->des2 = event->src2 - event->des2;
                            event->src2 = event->src2 - event->des2;
                            event->operation = NACK;
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(event);
                            return 1;
                        }
                        else
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                            event->when = sim_cycle + RETRY_TIME;
                            event->flip_flag = 1;
                            flip_counter ++;
                            dir_eventq_insert(event);
                            return 1;
                        }
                    }
                    else
                    {
                        dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                        if(last_L2_mshr_full)
                        {
                            Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                            last_L2_mshr_full = 0;
                        }
                    }
                    if(collect_stats)
                    {
                        cache_dl2->misses ++;
                        cache_dl2->dmisses ++;
                        involve_2_hops_miss ++;
                    }
                    event->reqAtDirTime = sim_cycle;
                    event->mshr_time = sim_cycle;
                    struct DIRECTORY_EVENT *new_event;
                    new_event = allocate_new_event(event);
                    new_event->operation = MEM_READ_REQ;
                    new_event->cmd = Read;
                    event->when = sim_cycle + WAIT_TIME;
                    event->operation = WAIT_MEM_READ;
                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                    dir_eventq_insert(new_event);
                    event->startCycle = sim_cycle;
                    event_created = event;
                    MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                    return 1;
                }
                else
                {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                    event->isReqL2SecMiss = 1;
                    if(collect_stats)
                    {
                        cache_dl2->in_mshr ++;
                        cache_dl2->din_mshr ++;
                    }
                    event_list_insert(event, matchnum);
                    return 1;
                }
            }
            break;

        case WRITE_UPDATE   :
            dcache2_access++;
#ifdef CENTRALIZED_L2

#else
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache2_access++;
#endif
            event->parent_operation = event->operation;
            if(md_text_addr(addr, tempID))
                panic("Writes cannot access text memory region");
            if(findCacheStatus(ml2, ml2_sub_set, ml2_sub_tag, ml2_hindex, &ml2_blk))
            {
                if(ml2_blk->qblk_set_shift == 0)
                {
                    ml2_hit = 1;
                    ml2_tag = ml2_sub_tag;
                    ml2_set = ml2_sub_set;

                    write_S_hit_num++;
                }
                else
                    ml2_hit = 0;
            }
            if(!ml2_hit && findCacheStatus(ml2, ml2_mul_set, ml2_mul_tag, ml2_hindex, &ml2_blk))
            {
                int multi_hit_condition;
                multi_hit_condition = !IsShared(ml2_blk->dir_sharer[block_offset], tempID) || !multi_blk_adap_en;
                if((ml2_blk->qblk_set_shift == 1) && multi_hit_condition)
                {
                    if ( multi_blk_adap_en && (ml2_blk->dir_state[block_offset] != DIR_TRANSITION) )
                        printf("WRITE_UPDATE m_blk error like\n");
                    ml2_hit = 1;
                    ml2_tag = ml2_mul_tag;
                    ml2_set = ml2_mul_set;
                    
                    write_Q_hit_num++;
                    mb_pain_count++;
                }
                else
                    ml2_hit = 0;
            }

            if(ml2_hit)
            {
                unsigned long long int *multiple_sharers = (unsigned long long int *)calloc(4, sizeof(unsigned long long int));
                int currentThreadId = (event->des1-MEM_LOC_SHIFT)*mesh_size + event->des2;
                if(event->L2miss_flag && event->L2miss_stated)
                    event->L2miss_complete = sim_cycle;
                if (ml2_blk->way_prev && ml2->policy == LRU)
                    update_way_list (&ml2->sets[ml2_set], ml2_blk, Head);
                event->isReqL2Hit = 1;
                if(ml2_blk->dir_state[block_offset] == DIR_TRANSITION)
                {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                    event->isReqL2Trans = 1;
                    if(ml2_blk->qblk_set_shift)
                    {
                        write_Q_hit_num--;
                        mb_pain_count--;
                    }
                    else
                        write_S_hit_num--;
                    if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                    {
                        dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                        event->src1 += event->des1;
                        event->des1 = event->src1 - event->des1;
                        event->src1 = event->src1 - event->des1;
                        event->src2 += event->des2;
                        event->des2 = event->src2 - event->des2;
                        event->src2 = event->src2 - event->des2;
                        event->operation = NACK;
                        event->processingDone = 0;
                        event->startCycle = sim_cycle;
                        scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(event);
                        if(collect_stats)
                            ml2->dir_access ++;
                        return 1;
                    }
                    else
                    {
                        dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                        event->when = sim_cycle + RETRY_TIME;
                        flip_counter ++;
                        event->flip_flag = 1;
                        dir_eventq_insert(event);
                        if(collect_stats)
                            ml2->dir_access ++;
                        return 1;
                    }
                }

                if((!IsExclusiveOrDirty(ml2_blk->dir_sharer[block_offset], tempID, &Threadid)) && (ml2_blk->dir_dirty[block_offset] != -1) )
                {
                    if (ml2_blk->dir_sharer[block_offset][tempID/64] == (unsigned long long int)1<<(tempID%64))
                    {
                        if ((!ml2_blk->qblk_set_shift) && (!multi_blk_adap_en))
                            printf("WRITE_UPDATE error like\n");
                    }
                    else
                        panic("DIR: Block can not be dirty state when it is not exclusive!");
                }

                // no other sharers @ fanglei
                if(!IsShared(ml2_blk->dir_sharer[block_offset], tempID))
                {
                    if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk) || (ml2_blk->dir_sharer[block_offset][tempID/64] & (unsigned long long int)1<<(tempID%64)))
                    {
                        // maybe an invalidation is earlier than write update @ fanglei
                        if (ml2_blk->dir_sharer[block_offset][tempID/64] & (unsigned long long int)1<<(tempID%64))
                        {
                            event->operation = ACK_DIR_WRITEUPDATE;
                            packet_size = meta_packet_size;
                        }
                        else
                        {
                            event->operation = ACK_DIR_WRITE;
                            packet_size = data_packet_size;
                        }
                        ml2_blk->dir_state[block_offset] = DIR_STABLE;
                        ml2_blk->dir_blk_state[block_offset] = MESI_MODIFIED;
                        ml2_blk->ptr_cur_event[block_offset] = NULL;
                        ml2_blk->dir_dirty[block_offset] = tempID;
                        ml2_blk->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64);

                        event->src1 += event->des1;
                        event->des1 = event->src1 - event->des1;
                        event->src1 = event->src1 - event->des1;
                        event->src2 += event->des2;
                        event->des2 = event->src2 - event->des2;
                        event->src2 = event->src2 - event->des2;

                        if(!mystricmp (network_type, "FSOI") || (!mystricmp (network_type, "HYBRID")))
                        {
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            event->parent = NULL;
                            event->blk_dir = NULL;
                            event->arrival_time = sim_cycle - event->arrival_time;
                            scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
                            dir_eventq_insert(event);
                        }
                        else
                        {
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            event->parent = NULL;
                            event->blk_dir = NULL;
                            event->arrival_time = sim_cycle - event->arrival_time;
                            scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
                            dir_eventq_insert(event);
                        }
                        return 1;
                    }
                    else // the requester is invalid && L2 data cache miss, so we have to ask the memoy; @ fanglei
                    {
                        event->operation = MISS_WRITE;
                        event->parent_operation = MISS_WRITE;

                        ml2_blk->dir_state[block_offset] = DIR_TRANSITION;
                        if(ml2_blk->qblk_set_shift)
                        {
                            write_Q_hit_num--;
                            mb_pain_count--;
                        }
                        else
                            write_S_hit_num--;

                        matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                        event->L2miss_flag = 1;
                        event->L2miss_stated = sim_cycle;
                        if(event->isSyncAccess)
                            Sync_L2_miss ++;
                        if(!matchnum)
                        { /* No match in L2 MSHR */
                            if(!cache_dl2->mshr->freeEntries)
                            {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                                if(!last_L2_mshr_full)
                                {
                                    L2_mshr_full ++;
                                    last_L2_mshr_full = sim_cycle;
                                }
                                if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                                {
                                    dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                                    //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                    event->src1 += event->des1;
                                    event->des1 = event->src1 - event->des1;
                                    event->src1 = event->src1 - event->des1;
                                    event->src2 += event->des2;
                                    event->des2 = event->src2 - event->des2;
                                    event->src2 = event->src2 - event->des2;
                                    event->operation = NACK;
                                    ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                    event->processingDone = 0;
                                    event->startCycle = sim_cycle;
                                    scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                    dir_eventq_insert(event);
                                    return 1;
                                }
                                else
                                {
                                    ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                    dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                                    event->when = sim_cycle + RETRY_TIME;
                                    event->flip_flag = 1;
                                    flip_counter ++;
                                    dir_eventq_insert(event);
                                    return 1;
                                }
                            }
                            else
                            {
                                dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                                if(last_L2_mshr_full)
                                {
                                    Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                    last_L2_mshr_full = 0;
                                }
                            }
                            if(collect_stats)
                            {
                                cache_dl2->misses ++;
                                cache_dl2->dmisses ++;
                                involve_2_hops_miss ++;
                            }
                            event->reqAtDirTime = sim_cycle;
                            event->mshr_time = sim_cycle;
                            event->blk_dir = ml2_blk;
                            struct DIRECTORY_EVENT *new_event;
                            new_event = allocate_new_event(event);
                            new_event->operation = MEM_READ_REQ;
                            new_event->cmd = Read;
                            event->when = sim_cycle + WAIT_TIME;
                            event->operation = WAIT_MEM_READ;
                            scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(new_event);
                            event->startCycle = sim_cycle;
                            event_created = event;
                            MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                            return 1;
                        }
                        else
                        {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                            event->isReqL2SecMiss = 1;
                            if(collect_stats)
                            {
                                cache_dl2->in_mshr ++;
                                cache_dl2->din_mshr ++;
                            }
                            event_list_insert(event, matchnum);
                            ml2_blk->dir_state[block_offset] = DIR_STABLE;
                            return 1;
                        }
                    }
                }

                /* Invalidate multiple sharers */
                if(ml2_blk->qblk_set_shift && multi_blk_adap_en)
                    panic("m_blk write can not rearch here write update");
                if( ml2_blk->ptr_en && ml2_blk->all_shared_en && (ml2_blk->dir_sharer[0][0] != (unsigned long long int)all_sharer) )
                    panic("write ptr_en all_shared_en error \n");
                if( ml2_blk->all_shared_en )
                {
                    ml2_blk->all_shared_en = 0;
                    PVC_write_all_shr_num++;
                }
                ml2_blk->dir_state[block_offset] = DIR_TRANSITION;
                ml2_blk->ptr_cur_event[block_offset] = event;
                if (!(ml2_blk->dir_sharer[block_offset][tempID/64] & (unsigned long long int)1<<(tempID%64)))
                {
                    event->operation = MISS_WRITE;
                    event->parent_operation = MISS_WRITE;
                }
                event->blk_dir = ml2_blk;
                event->childCount = 0;
                multiple_sharers = ml2_blk->dir_sharer[block_offset];
                int shr_data_req = 0;
                // if L2 data cache miss, we can ask data from one of the sharers; @ fanglei
                if (event->operation == MISS_WRITE)
                {
                    if (findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                    {
                        shr_data_req = 0;
                        dl2_blk->dir_state[block_offset] = DIR_TRANSITION;
                    }
                    else
                        shr_data_req = 1;
                }
                for (Threadid = 0; Threadid < numcontexts; Threadid++)
                {
                    if((((multiple_sharers[Threadid/64]) & ((unsigned long long int)1 << (Threadid%64))) == ((unsigned long long int)1 << (Threadid%64))) && (Threadid != tempID))
                    {
                        struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
                        if(new_event == NULL)
                            panic("Out of Virtual Memory");
                        new_event->parent = event;
                        new_event->operation = INV_MSG_WRITE;

                        // q_blk @ fanglei
                        if(ml2_blk->qblk_set_shift && multi_blk_adap_en)
                            panic("we do not write to m_blk");
                        new_event->qblk_set_shift = ml2_blk->qblk_set_shift;

                        new_event->src1 = currentThreadId/mesh_size+MEM_LOC_SHIFT;
                        new_event->src2 = currentThreadId%mesh_size;
                        new_event->des1 = Threadid/mesh_size+MEM_LOC_SHIFT;
                        new_event->des2 = Threadid%mesh_size;
                        if (shr_data_req)
                        {
                            event->shr_data_req = 1;
                            new_event->shr_data_req = 1;
                            shr_data_req = 0;               // we only need one copy; @ fanglei
                        }
                        new_event->req_time = 0;
                        new_event->conf_bit = -1;
                        new_event->input_buffer_full = 0;
                        new_event->pending_flag = 0;
                        new_event->op = event->op;
                        new_event->isPrefetch = 0;
                        new_event->L2miss_flag = event->L2miss_flag;
                        new_event->processingDone = 0;
                        new_event->startCycle = sim_cycle;
                        new_event->tempID = tempID;
                        new_event->resend = 0;
                        new_event->blk_dir = ml2_blk;
                        new_event->cp = event->cp;
                        new_event->addr = event->addr;
                        new_event->vp = NULL;
                        new_event->nbytes = event->cp->bsize;
                        new_event->udata = NULL;
                        new_event->cmd = Write;
                        new_event->rs = event->rs;
                        new_event->started = sim_cycle;
                        new_event->store_cond = event->store_cond;
                        new_event->spec_mode = 0;
                        new_event->data_reply = 1;

                        event->childCount++;
                        event->dirty_flag = 1;
                        /*go to network*/
                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(new_event);
                    }
                }
                return 1;
            }   // end of "hit in the L2 meta cache" handler; @ fanglei
            // WRITE_UPDATE must have a directory in a earlier time, the directory must be evicting; we allocate a new directory; @ fanglei
            else
            {
                // we allocate a new entry; no m_blk hit, then initial with a m_blk; m_blk hit, then new a s_blk for coherent data block;@ fanglei
                ml2_hit = findCacheStatus(ml2, ml2_mul_set, ml2_mul_tag, ml2_hindex, &ml2_blk);
                if(ml2_hit)
                    ml2_ed = Is_ExclusiveOrDirty(ml2_blk->dir_sharer[block_offset]);
                if(ml2_hit)
                {
                    ml2_tag = ml2_sub_tag;
                    ml2_set = ml2_sub_set;
                    if(ml2_ed)
                        write_S_init_ed_num++;
                    else
                        write_S_init_s_num++;
                }
                else
                {
                    ml2_tag = ml2_mul_tag;
                    ml2_set = ml2_mul_set;
                    write_Q_init_num++;
                }

                switch (ml2->policy)
                {
                    int i;
                    int bindex;
                    case LRU:
                    case FIFO:
                        i = ml2->assoc;
                        do {
                            if(i == 0)  break;
                            i--;
                            repl = ml2->sets[ml2_set].way_tail;
                            update_way_list (&ml2->sets[ml2_set], repl, Head);
                        }
                        while(repl_dir_state_check(repl->dir_state));    // The "do while" searches for the least recently used block that is not in "DIR_TRANSITION" state to be replaced.  @ fanglei

                        if ( repl_ad_en && ( repl->qblk_set_shift == 1 ) )
                        {
                            struct cache_blk_t *bak;
                            bak = repl;
                            
                            i = ml2->assoc;
                            do {
                                if(i == 0)  break;
                                i--;
                                repl = ml2->sets[ml2_set].way_tail;
                                update_way_list (&ml2->sets[ml2_set], repl, Head);
                            }
                            while(repl_dir_state_check(repl->dir_state) || (repl->qblk_set_shift == 1) );
                            
                            if ( repl_dir_state_check(repl->dir_state) || (repl->qblk_set_shift == 1))
                            {
                                repl = bak;
                                update_way_list (&ml2->sets[ml2_set], repl, Head);
                            }
                        }

                        if(repl_dir_state_check(repl->dir_state))
                        {       // If every block in this set is in "DIR_TRANSITION" state, it means the refill can not be done in this cycle. The p_event has to wait a "RETRY_TIME" time, and then request for the data again. @ fanglei
                            flip_counter ++;
                            event->flip_flag = 1;
                            event->when = sim_cycle + RETRY_TIME;
                            dir_eventq_insert(event);
                            return 1;
                        }
                        break;
                    case Random:
                        bindex = myrand () & (dl2->assoc - 1);
                        repl = CACHE_BINDEX (dl2, dl2->sets[dl2_set].blks, bindex);
                        break;
                    default:
                        panic ("bogus replacement policy");
                }

                // PVC; if the repl->ptr_en valid, we need to exchange here; @ fanglei
                // if the repl->ptr_en && !vector_available, we can not handle this now; @ fanglei
                if (ml2_hit && qblk_set_shift && (vector_num!=0) && (repl->ptr_en) && (Is_Shared(ml2_blk->dir_sharer[block_offset])>1) )
                {
                    int vector_available;
                    int sharer_num;
                    int temp_threadid;
                    struct cache_blk_t *vector_blk;
                    
                    vector_available = vector_available_func(&vector_blk, ml2_set);


                    
                    if(!vector_available)   // all the vector_blks are in transition; @ fanglei;
                    {
                        PVC_flip_num++;
                        event->isReqL2Trans = 1;
                        if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                        {
                            event->src1 += event->des1;
                            event->des1 = event->src1 - event->des1;
                            event->src1 = event->src1 - event->des1;
                            event->src2 += event->des2;
                            event->des2 = event->src2 - event->des2;
                            event->src2 = event->src2 - event->des2;
                            event->operation = NACK;
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(event);
                            if(collect_stats)
                                ml2->dir_access ++;
                            return 1;
                        }
                        else
                        {
                            event->when = sim_cycle + RETRY_TIME;
                            flip_counter ++;
                            event->flip_flag = 1;
                            dir_eventq_insert(event);
                            if(collect_stats)
                                ml2->dir_access ++;
                            return 1;
                        }                            
                    }
                    else        // one of the vector_blks will exchange with the pointer_blk; @ fanglei;
                    {
                        sharer_num = Is_Shared(vector_blk->dir_sharer[block_offset]);

                        PVC_exchange_num++;
                        PVC_exch_vec_shr_distr[sharer_num]++;

                        if( sharer_num < 2 ) // the vector_blk is exclusive and need no invalidation; @ fanglei1
                        {
                            PVC_dowm_no_inv_num++;

                            repl->ptr_en = 0;

                            vector_blk->ptr_en = 1;
                            if(vector_blk->all_shared_en)
                                panic("vector_blk all_shared_en illegal\n");
                            if( vector_blk->dir_blk_state[0] == MESI_SHARED )
                                panic("state error\n");
                        }
                        else if( sharer_num == mesh_size*mesh_size ) // the vector_blk is all_shared and need no invalidation; @ fanglei1
                        {
                            PVC_up_no_add_num++;

                            repl->ptr_en = 0;

                            vector_blk->ptr_en = 1;
                            vector_blk->all_shared_en = 1;
                            if(vector_blk->dir_sharer[0][0] != (unsigned long long int)all_sharer)
                                panic("all_shared_en dir_sharer errpr\n");
                            if( vector_blk->dir_blk_state[0] != MESI_SHARED )
                                panic("state error\n");
                        }
                        else    // the vector can not be changed into a pointer without lose of sharing information; @ fanglei
                        {
                            if(sharer_num > sharer_threshold)    // the vector is changed into all_shared; @ fanglei
                            {
                                PVC_up_add_num++;
                                PVC_up_add_shr_num += mesh_size*mesh_size-sharer_num;
                                PVC_up_add_shr_distr[mesh_size*mesh_size-sharer_num]++;

                                repl->ptr_en = 0;

                                vector_blk->ptr_en = 1;
                                vector_blk->all_shared_en = 1;
                                vector_blk->dir_sharer[0][0] = (unsigned long long int)all_sharer;
                                if( vector_blk->dir_blk_state[0] != MESI_SHARED )
                                    panic("state error\n");
                            }
                            else    // the vector is changed into exclusive; @ fanglei
                            {
                                PVC_dowm_inv_num++;
                                PVC_dowm_inv_shr_num += sharer_num-1;
                                PVC_dowm_inv_shr_distr[sharer_num-1]++;

                                repl->ptr_en = 0;

                                int ml2_dir_blk_state = vector_blk->dir_blk_state[0];
                                int survive_en = 1, survivor = -1;    // only a sharer can survive; @ fanglei
                                struct DIRECTORY_EVENT *collect_event = allocate_event(0);
                                if(collect_event == NULL)
                                    panic("Out of Virtual Memory");

                                vector_blk->ptr_en = 1;
                                if(vector_blk->all_shared_en)
                                    panic("vector_blk all_shared_en illegal\n");
                                vector_blk->all_shared_en = 0;
                                vector_blk->dir_blk_state[0] = MESI_EXCLUSIVE;
                                vector_blk->dir_state[0] = DIR_TRANSITION;
                                vector_blk->ptr_cur_event[0] = collect_event;
                                if (vector_blk->qblk_set_shift)
                                    vector_blk->addr = (vector_blk->tagid.tag << (cache_ml2->tag_shift+qblk_set_shift)) + (ml2_set << (cache_ml2->set_shift+qblk_set_shift));
                                else
                                    vector_blk->addr = (vector_blk->tagid.tag << cache_ml2->tag_shift) + (ml2_set << cache_ml2->set_shift);

                                collect_event->childCount = 0;
                                collect_event->individual_childCount[0] = 0;
                                collect_event->sharer_num = 1;     // indicate roll back or write invalid @ fanglei
                                collect_event->blk_dir = vector_blk;
                                collect_event->operation = VEC_DEGRADE;

                                for (temp_threadid = 0; temp_threadid < numcontexts; temp_threadid++)
                                {
                                    if(((vector_blk->dir_sharer[0][temp_threadid/64]) & ((unsigned long long int)1 << (temp_threadid%64))) == ((unsigned long long int)1 << (temp_threadid%64)))
                                    {
                                        if(survive_en)
                                        {
                                            survivor = temp_threadid;
                                            survive_en = 0;
                                            continue;
                                        }
                                        struct DIRECTORY_EVENT *new_event = allocate_event(0);
                                        if(new_event == NULL)
                                            panic("Out of Virtual Memory");

                                        // the ml2_blk is m_blk or s_blk; @ fanglei
                                        new_event->qblk_set_shift = vector_blk->qblk_set_shift;
                                        new_event->ml2_dir_blk_state = ml2_dir_blk_state;
                                        new_event->addr = vector_blk->addr;
                                        new_event->operation = INV_MSG_WRITE;
                                        new_event->parent = collect_event;
                                        new_event->blk_dir = vector_blk;                                            
                                        new_event->tempID = event->tempID;
                                        new_event->cp = event->cp;
                                        new_event->nbytes = event->cp->bsize;
                                        new_event->src1 = event->des1;
                                        new_event->src2 = event->des2;
                                        new_event->des1 = temp_threadid/mesh_size+MEM_LOC_SHIFT;
                                        new_event->des2 = temp_threadid%mesh_size;

                                        new_event->isPrefetch = 0;
                                        new_event->conf_bit = -1;
                                        new_event->req_time = 0;
                                        new_event->input_buffer_full = 0;
                                        new_event->pending_flag = 0;
                                        new_event->op = collect_event->op;
                                        new_event->vp = NULL;
                                        new_event->udata = NULL;
                                        new_event->rs = NULL;
                                        new_event->spec_mode = 0;
                                        new_event->data_reply = 1;

                                        /*go to network*/
                                        new_event->processingDone = 0;
                                        new_event->startCycle = sim_cycle;
                                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, 0);
                                        dir_eventq_insert(new_event);

                                        collect_event->childCount++;
                                        collect_event->individual_childCount[0] ++;
                                    }
                                }
                                if(survivor == -1)
                                    panic("survivor error\n");
                                vector_blk->dir_sharer[0][survivor/64] = ((unsigned long long int)1 << (survivor%64));
                            }
                        }
                        
                    }
                }

                int i=0, iteration = 1;
                for(i=0; i<(cache_dl2->set_shift - cache_dl1[0]->set_shift); i++)
                    iteration = iteration * 2;
                int m;
                if (repl->status & CACHE_BLK_VALID)
                {  /* replace the replacement line in L2 cache */
                    if( repl->ptr_en && repl->all_shared_en && (repl->dir_sharer[0][0] != (unsigned long long int)all_sharer) )
                        panic("recall ptr_en all_shared_en error \n");
                    if (collect_stats)
                        ml2->replacements++;

                    int ml2_dir_blk_state;
                    int new_vc = 0;
                    int flag = 0, sharer_num = 0;
                    for (i=0; i<iteration; i++)
                        if(Is_Shared(repl->dir_sharer[i]))
                            sharer_num ++;

                    event->childCount = 0;
                    event->sharer_num = 0;

                    for(i=0; i<iteration; i++)
                    {
                        if(Is_Shared(repl->dir_sharer[i]))
                        {  /* send invalidation messages to L1 cache of sharers*/
                            /* Invalidate multiple sharers */

                            ml2_dir_blk_state = repl->dir_blk_state[i];
                            if (    (ml2_dir_blk_state ==  MESI_SHARED) && (Is_Shared(repl->dir_sharer[i]) < 2) 
                                 || (ml2_dir_blk_state ==  MESI_MODIFIED) && (Is_Shared(repl->dir_sharer[i]) != 1) 
                                 || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) && (Is_Shared(repl->dir_sharer[i]) != 1) 
                                 || (ml2_dir_blk_state ==  MESI_INVALID)
                               )
                                printf("state conflicts with sharer vector\n");

                            for(m = 0; m<iteration; m++)
                            {
                                repl->dir_state[m] = DIR_TRANSITION;
                                repl->dir_blk_state[m] = MESI_INVALID;
                            }
                            repl->ptr_cur_event[i] = event;
                            if (repl->qblk_set_shift)
                                repl->addr = (repl->tagid.tag << (cache_ml2->tag_shift+qblk_set_shift)) + (ml2_set << (cache_ml2->set_shift+qblk_set_shift));
                            else
                                repl->addr = (repl->tagid.tag << cache_ml2->tag_shift) + (ml2_set << cache_ml2->set_shift) + (i << cache_dl1[0]->set_shift);
                            event->individual_childCount[i] = 0;
                            for (Threadid = 0; Threadid < numcontexts; Threadid++)
                            {
                                if(((repl->dir_sharer[i][Threadid/64]) & ((unsigned long long int)1 << (Threadid%64))) == ((unsigned long long int)1 << (Threadid%64)))
                                {
                                    recall_inv_num++;
                                    if(collect_stats)
                                        cache_ml2->invalidations ++;
                                    struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
                                    if(new_event == NULL)
                                        panic("Out of Virtual Memory");
                                    involve_4_hops_miss ++;
                                    new_event->conf_bit = -1;
                                    new_event->req_time = 0;
                                    new_event->input_buffer_full = 0;
                                    new_event->pending_flag = 0;
                                    new_event->op = event->op;
                                    new_event->operation = INV_MSG_WRITE;

                                    // the ml2_blk is m_blk or s_blk; @ fanglei
                                    new_event->qblk_set_shift = repl->qblk_set_shift;
                                    new_event->ml2_dir_blk_state = ml2_dir_blk_state;

                                    new_event->isPrefetch = 0;
                                    new_event->tempID = event->tempID;
                                    new_event->L2miss_flag = event->L2miss_flag;
                                    new_event->resend = 0;
                                    new_event->blk1 = repl;
                                    new_event->addr = repl->addr;
                                    new_event->cp = event->cp;
                                    new_event->vp = NULL;
                                    new_event->nbytes = event->cp->bsize;
                                    new_event->udata = NULL;
                                    new_event->rs = NULL;
                                    new_event->spec_mode = 0;
                                    new_event->src1 = event->des1;
                                    new_event->src2 = event->des2;
                                    new_event->des1 = Threadid/mesh_size+MEM_LOC_SHIFT;
                                    new_event->des2 = Threadid%mesh_size;
                                    new_event->processingDone = 0;
                                    new_event->startCycle = sim_cycle;
                                    new_event->parent = event;
                                    new_event->blk_dir = repl;
                                    new_event->data_reply = 1;
                                    event->sharer_num = sharer_num;
                                    event->blk_dir = repl;
                                    event->childCount++;
                                    event->individual_childCount[i] ++;
                                    /*go to network*/
                                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, new_vc);

                                    dir_eventq_insert(new_event);
                                }
                            }
                            flag = 1;

                            if(ml2_hit)
                                if(ml2_ed)
                                    if(repl->qblk_set_shift)
                                    {
                                        write_S_init_ed_QR_num++;
                                        if(ml2_dir_blk_state ==  MESI_SHARED)
                                            write_S_init_ed_QR_s_num++;
                                        else if ( (ml2_dir_blk_state ==  MESI_MODIFIED) || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) )
                                            write_S_init_ed_QR_ed_num++;
                                    }
                                    else                        write_S_init_ed_SR_num++;
                                else
                                    if(repl->qblk_set_shift)
                                    {
                                        write_S_init_s_QR_num++;
                                        if(ml2_dir_blk_state ==  MESI_SHARED)
                                            write_S_init_s_QR_s_num++;
                                        else if ( (ml2_dir_blk_state ==  MESI_MODIFIED) || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) )
                                            write_S_init_s_QR_ed_num++;
                                    }
                                    else                        write_S_init_s_SR_num++;
                            else
                                if(repl->qblk_set_shift)
                                {
                                    write_Q_init_QR_num++;
                                    if(ml2_dir_blk_state ==  MESI_SHARED)
                                        write_Q_init_QR_s_num++;
                                    else if ( (ml2_dir_blk_state ==  MESI_MODIFIED) || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) )
                                        write_Q_init_QR_ed_num++;
                                }
                                else                        write_Q_init_SR_num++;
                            recall_num++;
                            if(repl->all_shared_en)
                            {
                                repl->all_shared_en = 0;
                                PVC_recall_all_shr_num++;
                            }

                            sharer_distr[event->childCount] ++;
                            IsNeighbor_sharer(repl->dir_sharer[i], tempID);
                        }
                    }
                    if(flag == 1)
                    {
                        // a later request can hit this blk @ fanglei
                        if(collect_stats)
                            dl2->replInv++;

                        repl->status = CACHE_BLK_VALID;
                        repl->addr = addr;
                        repl->tagid.tag = ml2_tag;
                        repl->tagid.threadid = tempID;
                        for(m=0; m<iteration; m++)
                        {
                            repl->dir_sharer[m][tempID/64] = 0;
                            repl->dir_blk_state[m] = MESI_INVALID;
                        }

                        if(ml2_hit && qblk_set_shift)
                        {
                            if( (repl->ptr_en) && (Is_Shared(ml2_blk->dir_sharer[block_offset])>1) )
                                panic("unable to copy\n");
                            repl->qblk_set_shift = 0;
                            repl->dir_sharer[block_offset][tempID/64] = ml2_blk->dir_sharer[block_offset][tempID/64];
                            repl->dir_blk_state[block_offset] = ml2_blk->dir_blk_state[block_offset];
                            repl->dir_dirty[block_offset] = ml2_blk->dir_dirty[block_offset];
                        }
                        else if (qblk_set_shift)
                            repl->qblk_set_shift = 1;
                        else
                            repl->qblk_set_shift = 0;
                        return 1;
                    }
                }
                // no roll back, so the meta entry is valid now; @ fanglei
                for(m=0;m<iteration;m++)
                {
                    repl->ptr_cur_event[m] = NULL;
                    repl->dir_state[m] = DIR_STABLE;     // If block is invalid, the following operation is carried out.
                    repl->dir_sharer[m][tempID/64] = 0;
                    repl->dir_blk_state[m] = MESI_INVALID;
                    repl->dir_dirty[m] = -1;
                    repl->exclusive_time[m] = sim_cycle;
                }
                repl->status = CACHE_BLK_VALID;
                repl->ready = sim_cycle;
                repl->tagid.tag = ml2_tag;
                repl->tagid.threadid = tempID;
                if(ml2_hit && qblk_set_shift)
                    repl->qblk_set_shift = 0;
                else if (qblk_set_shift)
                    repl->qblk_set_shift = 1;
                else
                    repl->qblk_set_shift = 0;
                repl->addr = addr;
                if (repl->way_prev && ml2->policy == LRU)
                    update_way_list(&ml2->sets[ml2_set], repl, Head);
                if (ml2->hsize)
                    link_htab_ent(ml2, &ml2->sets[ml2_set], repl);
                ml2->last_tagsetid.tag = 0;
                ml2->last_tagsetid.threadid = tempID;
                ml2->last_blk = repl;

                // for m_blk & s_blk, we return; @ fanglei
                if (ml2_hit && qblk_set_shift)
                {
                    if( (repl->ptr_en) && (Is_Shared(ml2_blk->dir_sharer[block_offset])>1) )
                        panic("unable to copy\n");
                    repl->dir_sharer[block_offset][tempID/64] = ml2_blk->dir_sharer[block_offset][tempID/64];
                    repl->dir_blk_state[block_offset] = ml2_blk->dir_blk_state[block_offset];
                    repl->dir_dirty[block_offset] = ml2_blk->dir_dirty[block_offset];
                    event->when = sim_cycle;
                    event->no_L2_delay = 1;
                    dir_eventq_insert(event);
                    return 1;
                }
                // non shared && hit in L2 data cache @ fanglei
                if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                {
                    ml2->data_access ++;
                    involve_2_hops ++;
                    if(event->rs)
                        if(event->rs->op == LDL_L)
                            load_link_shared ++;
                    repl->dir_state[block_offset] = DIR_STABLE;
                    repl->ptr_cur_event[block_offset] = NULL;
                    repl->exclusive_time[block_offset] = sim_cycle;
                    if(!md_text_addr(addr, tempID))
                    {
                        repl->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64);
                        repl->dir_blk_state[block_offset] = MESI_MODIFIED;
                        repl->dir_dirty[block_offset] = tempID;
                    }
                    else
                        printf("directory is for data segment\n");

                    event->operation = ACK_DIR_WRITE;
                    event->reqAckTime = sim_cycle;
                    event->src1 += event->des1;
                    event->des1 = event->src1 - event->des1;
                    event->src1 = event->src1 - event->des1;
                    event->src2 += event->des2;
                    event->des2 = event->src2 - event->des2;
                    event->src2 = event->src2 - event->des2;
                    event->processingDone = 0;
                    event->startCycle = sim_cycle;
                    event->parent = NULL;
                    event->blk_dir = NULL;
                    event->arrival_time = sim_cycle - event->arrival_time;
                    scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                    dir_eventq_insert(event);
                    return 1;
                }
                // non shared && miss in L2 data cache @ fanglei
                else
                {
                    event->operation = MISS_WRITE;
                    event->parent_operation = MISS_WRITE;
                    repl->dir_state[block_offset] = DIR_TRANSITION;     // data miss, we prevent from replacing this meta entry; @ fanglei
                    matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                    event->L2miss_flag = 1;
                    event->L2miss_stated = sim_cycle;
                    if(event->isSyncAccess)
                        Sync_L2_miss ++;
                    if(!matchnum)
                    { /* No match in L2 MSHR */
                        if(!cache_dl2->mshr->freeEntries)
                        {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                            if(!last_L2_mshr_full)
                            {
                                L2_mshr_full ++;
                                last_L2_mshr_full = sim_cycle;
                            }
                            if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                            {
                                dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                                //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                event->src1 += event->des1;
                                event->des1 = event->src1 - event->des1;
                                event->src1 = event->src1 - event->des1;
                                event->src2 += event->des2;
                                event->des2 = event->src2 - event->des2;
                                event->src2 = event->src2 - event->des2;
                                event->operation = NACK;
                                repl->dir_state[block_offset] = DIR_STABLE;
                                event->processingDone = 0;
                                event->startCycle = sim_cycle;
                                scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                dir_eventq_insert(event);
                                return 1;
                            }
                            else
                            {
                                repl->dir_state[block_offset] = DIR_STABLE;
                                dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                                event->when = sim_cycle + RETRY_TIME;
                                event->flip_flag = 1;
                                flip_counter ++;
                                dir_eventq_insert(event);
                                return 1;
                            }
                        }
                        else
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                            if(last_L2_mshr_full)
                            {
                                Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                last_L2_mshr_full = 0;
                            }
                        }
                        if(collect_stats)
                        {
                            cache_dl2->misses ++;
                            cache_dl2->dmisses ++;
                            involve_2_hops_miss ++;
                        }
                        event->reqAtDirTime = sim_cycle;
                        event->mshr_time = sim_cycle;
                        event->blk_dir = repl;
                        struct DIRECTORY_EVENT *new_event;
                        new_event = allocate_new_event(event);
                        new_event->operation = MEM_READ_REQ;
                        new_event->cmd = Read;
                        event->when = sim_cycle + WAIT_TIME;
                        event->operation = WAIT_MEM_READ;
                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(new_event);
                        event->startCycle = sim_cycle;
                        event_created = event;
                        MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                        return 1;
                    }
                    else
                    {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                        event->isReqL2SecMiss = 1;
                        if(collect_stats)
                        {
                            cache_dl2->in_mshr ++;
                            cache_dl2->din_mshr ++;
                        }
                        event_list_insert(event, matchnum);
                        repl->dir_state[block_offset] = DIR_STABLE;
                        return 1;
                    }
                }
            }
            break;

        case MISS_WRITE     :
            dcache2_access++;
#ifdef CENTRALIZED_L2

#else
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache2_access++;
#endif
            event->parent_operation = event->operation;
            if(md_text_addr(addr, tempID))
                panic("Writes cannot access text memory region");

            if(findCacheStatus(ml2, ml2_sub_set, ml2_sub_tag, ml2_hindex, &ml2_blk))
            {
                if(ml2_blk->qblk_set_shift == 0)
                {
                    ml2_hit = 1;
                    ml2_tag = ml2_sub_tag;
                    ml2_set = ml2_sub_set;

                    write_S_hit_num++;
                }
                else
                    ml2_hit = 0;
            }
            if(!ml2_hit && findCacheStatus(ml2, ml2_mul_set, ml2_mul_tag, ml2_hindex, &ml2_blk))
            {
                int multi_hit_condition;
                multi_hit_condition = !IsShared(ml2_blk->dir_sharer[block_offset], tempID) || !multi_blk_adap_en;
                if((ml2_blk->qblk_set_shift == 1) && multi_hit_condition)
                {
                    ml2_hit = 1;
                    ml2_tag = ml2_mul_tag;
                    ml2_set = ml2_mul_set;

                    write_Q_hit_num++;
                    mb_pain_count++;
                }
                else
                    ml2_hit = 0;
            }

            if(ml2_hit)
            {/* Hit */
                unsigned long long int *multiple_sharers = (unsigned long long int *)calloc(4, sizeof(unsigned long long int));
                int currentThreadId = (event->des1-MEM_LOC_SHIFT)*mesh_size + event->des2;
                if(event->L2miss_flag && event->L2miss_stated)
                    event->L2miss_complete = sim_cycle;
                if (ml2_blk->way_prev && ml2->policy == LRU)
                    update_way_list (&ml2->sets[ml2_set], ml2_blk, Head);
                event->isReqL2Hit = 1;
                if(ml2_blk->dir_state[block_offset] == DIR_TRANSITION)
                {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                    event->isReqL2Trans = 1;
                    if(ml2_blk->qblk_set_shift)
                    {
                        write_Q_hit_num--;
                        mb_pain_count--;
                    }
                    else
                        write_S_hit_num--;
                    if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                    {
                        dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                        event->src1 += event->des1;
                        event->des1 = event->src1 - event->des1;
                        event->src1 = event->src1 - event->des1;
                        event->src2 += event->des2;
                        event->des2 = event->src2 - event->des2;
                        event->src2 = event->src2 - event->des2;
                        event->operation = NACK;
                        event->processingDone = 0;
                        event->startCycle = sim_cycle;
                        scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(event);
                        if(collect_stats)
                            ml2->dir_access ++;
                        return 1;
                    }
                    else
                    {
                        dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                        event->when = sim_cycle + RETRY_TIME;
                        flip_counter ++;
                        event->flip_flag = 1;
                        dir_eventq_insert(event);
                        if(collect_stats)
                            ml2->dir_access ++;
                        return 1;
                    }
                }

                if((!IsExclusiveOrDirty(ml2_blk->dir_sharer[block_offset], tempID, &Threadid)) && (ml2_blk->dir_dirty[block_offset] != -1) )
                {
                    if (ml2_blk->dir_sharer[block_offset][tempID/64]==(unsigned long long int)1<<(tempID%64))
                    {
                        if (!ml2_blk->qblk_set_shift)
                            printf("MISS_WRITE eveict self like\n");
                    }
                    else
                        panic("DIR: Block can not be dirty state when it is not exclusive!");
                }

                // no other sharers @ fanglei
                if(!IsShared(ml2_blk->dir_sharer[block_offset], tempID))
                {
                    if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                    {
                        ml2_blk->dir_state[block_offset] = DIR_STABLE;
                        ml2_blk->dir_blk_state[block_offset] = MESI_MODIFIED;
                        ml2_blk->ptr_cur_event[block_offset] = NULL;
                        ml2_blk->dir_dirty[block_offset] = tempID;
                        ml2_blk->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64);
                        event->operation = ACK_DIR_WRITE;
                        packet_size = data_packet_size;
                        event->src1 += event->des1;
                        event->des1 = event->src1 - event->des1;
                        event->src1 = event->src1 - event->des1;
                        event->src2 += event->des2;
                        event->des2 = event->src2 - event->des2;
                        event->src2 = event->src2 - event->des2;

                        if(!mystricmp (network_type, "FSOI") || (!mystricmp (network_type, "HYBRID")))
                        {
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            event->parent = NULL;
                            event->blk_dir = NULL;
                            event->arrival_time = sim_cycle - event->arrival_time;
                            scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
                            dir_eventq_insert(event);
                        }
                        else
                        {
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            event->parent = NULL;
                            event->blk_dir = NULL;
                            event->arrival_time = sim_cycle - event->arrival_time;
                            scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
                            dir_eventq_insert(event);
                        }
                        return 1;
                    }
                    else
                    {
                        ml2_blk->dir_state[block_offset] = DIR_TRANSITION;     // data miss, we prevent from replacing this meta entry; @ fanglei
                        if(ml2_blk->qblk_set_shift)
                        {
                            write_Q_hit_num--;
                            mb_pain_count--;
                        }
                        else
                            write_S_hit_num--;
                        matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                        event->L2miss_flag = 1;
                        event->L2miss_stated = sim_cycle;
                        if(event->isSyncAccess)
                            Sync_L2_miss ++;
                        if(!matchnum)
                        { /* No match in L2 MSHR */
                            if(!cache_dl2->mshr->freeEntries)
                            {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                                if(!last_L2_mshr_full)
                                {
                                    L2_mshr_full ++;
                                    last_L2_mshr_full = sim_cycle;
                                }
                                if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                                {
                                    dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                                    //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                    event->src1 += event->des1;
                                    event->des1 = event->src1 - event->des1;
                                    event->src1 = event->src1 - event->des1;
                                    event->src2 += event->des2;
                                    event->des2 = event->src2 - event->des2;
                                    event->src2 = event->src2 - event->des2;
                                    event->operation = NACK;
                                    ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                    event->processingDone = 0;
                                    event->startCycle = sim_cycle;
                                    scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                    dir_eventq_insert(event);
                                    return 1;
                                }
                                else
                                {
                                    ml2_blk->dir_state[block_offset] = DIR_STABLE;
                                    dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                                    event->when = sim_cycle + RETRY_TIME;
                                    event->flip_flag = 1;
                                    flip_counter ++;
                                    dir_eventq_insert(event);
                                    return 1;
                                }
                            }
                            else
                            {
                                dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                                if(last_L2_mshr_full)
                                {
                                    Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                    last_L2_mshr_full = 0;
                                }
                            }
                            if(collect_stats)
                            {
                                cache_dl2->misses ++;
                                cache_dl2->dmisses ++;
                                involve_2_hops_miss ++;
                            }
                            event->reqAtDirTime = sim_cycle;
                            event->mshr_time = sim_cycle;
                            event->blk_dir = ml2_blk;
                            struct DIRECTORY_EVENT *new_event;
                            new_event = allocate_new_event(event);
                            new_event->operation = MEM_READ_REQ;
                            new_event->cmd = Read;
                            event->when = sim_cycle + WAIT_TIME;
                            event->operation = WAIT_MEM_READ;
                            scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(new_event);
                            event->startCycle = sim_cycle;
                            event_created = event;
                            MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                            return 1;
                        }
                        else
                        {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                            event->isReqL2SecMiss = 1;
                            if(collect_stats)
                            {
                                cache_dl2->in_mshr ++;
                                cache_dl2->din_mshr ++;
                            }
                            event_list_insert(event, matchnum);
                            ml2_blk->dir_state[block_offset] = DIR_STABLE;
                            return 1;
                        }
                    }
                }

                /* Invalidate multiple sharers */
                if(ml2_blk->qblk_set_shift && multi_blk_adap_en)
                    panic("m_blk write can not rearch here write miss");
                if( ml2_blk->ptr_en && ml2_blk->all_shared_en && (ml2_blk->dir_sharer[0][0] != (unsigned long long int)all_sharer) )
                    panic("write ptr_en all_shared_en error \n");
                if( ml2_blk->all_shared_en )
                {
                    ml2_blk->all_shared_en = 0;
                    PVC_write_all_shr_num++;
                }
                ml2_blk->dir_state[block_offset] = DIR_TRANSITION;
                ml2_blk->ptr_cur_event[block_offset] = event;
                event->blk_dir = ml2_blk;
                event->childCount = 0;
                multiple_sharers = ml2_blk->dir_sharer[block_offset];
                int shr_data_req;       // if L2 data cache miss, we can ask data from one of the sharers; @ fanglei
                if (findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                {
                    shr_data_req = 0;
                    dl2_blk->dir_state[block_offset] = DIR_TRANSITION;
                }
                else
                    shr_data_req = 1;
                for (Threadid = 0; Threadid < numcontexts; Threadid++)
                {
                    if((((multiple_sharers[Threadid/64]) & ((unsigned long long int)1 << (Threadid%64))) == ((unsigned long long int)1 << (Threadid%64))) && (Threadid != tempID))
                    {
                        struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
                        if(new_event == NULL)
                            panic("Out of Virtual Memory");
                        new_event->parent = event;
                        new_event->operation = INV_MSG_WRITE;

                        // the ml2_blk is m_blk or s_blk; @ fanglei
                        new_event->qblk_set_shift = ml2_blk->qblk_set_shift;

                        if (shr_data_req)
                        {
                            event->shr_data_req = 1;
                            new_event->shr_data_req = 1;
                            shr_data_req = 0;               // we only need one copy; @ fanglei
                        }
                        new_event->src1 = currentThreadId/mesh_size+MEM_LOC_SHIFT;
                        new_event->src2 = currentThreadId%mesh_size;
                        new_event->des1 = Threadid/mesh_size+MEM_LOC_SHIFT;
                        new_event->des2 = Threadid%mesh_size;

                        new_event->req_time = 0;
                        new_event->conf_bit = -1;
                        new_event->input_buffer_full = 0;
                        new_event->pending_flag = 0;
                        new_event->op = event->op;
                        new_event->isPrefetch = 0;
                        new_event->L2miss_flag = event->L2miss_flag;
                        new_event->processingDone = 0;
                        new_event->startCycle = sim_cycle;
                        new_event->tempID = tempID;
                        new_event->resend = 0;
                        new_event->blk_dir = ml2_blk;
                        new_event->cp = event->cp;
                        new_event->addr = event->addr;
                        new_event->vp = NULL;
                        new_event->nbytes = event->cp->bsize;
                        new_event->udata = NULL;
                        new_event->cmd = Write;
                        new_event->rs = event->rs;
                        new_event->started = sim_cycle;
                        new_event->store_cond = event->store_cond;
                        new_event->spec_mode = 0;
                        new_event->data_reply = 1;

                        event->childCount++;
                        event->dirty_flag = 1;
                        /*go to network*/
                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(new_event);
                    }
                }
                return 1;
            }       // end of "hit in the L2 meta cache" handler; @ fanglei
            else if ( event->tlb_state == SHARED_WRITE )
            {
                // we allocate a new entry; no m_blk hit, then initial with a m_blk; m_blk hit, then new a s_blk for coherent data block;@ fanglei
                ml2_hit = findCacheStatus(ml2, ml2_mul_set, ml2_mul_tag, ml2_hindex, &ml2_blk);
                if(ml2_hit)
                    ml2_ed = Is_ExclusiveOrDirty(ml2_blk->dir_sharer[block_offset]);
                if(ml2_hit)
                {
                    ml2_tag = ml2_sub_tag;
                    ml2_set = ml2_sub_set;
                    if(ml2_ed)
                        write_S_init_ed_num++;
                    else
                        write_S_init_s_num++;
                }
                else
                {
                    ml2_tag = ml2_mul_tag;
                    ml2_set = ml2_mul_set;
                    write_Q_init_num++;
                }

                switch (ml2->policy)
                {
                    int i;
                    int bindex;
                    case LRU:
                    case FIFO:
                        i = ml2->assoc;
                        do {
                            if(i == 0)  break;
                            i--;
                            repl = ml2->sets[ml2_set].way_tail;
                            update_way_list (&ml2->sets[ml2_set], repl, Head);
                        }
                        while(repl_dir_state_check(repl->dir_state));    // The "do while" searches for the least recently used block that is not in "DIR_TRANSITION" state to be replaced.  @ fanglei

                        if ( repl_ad_en && ( repl->qblk_set_shift == 1 ) )
                        {
                            struct cache_blk_t *bak;
                            bak = repl;
                            
                            i = ml2->assoc;
                            do {
                                if(i == 0)  break;
                                i--;
                                repl = ml2->sets[ml2_set].way_tail;
                                update_way_list (&ml2->sets[ml2_set], repl, Head);
                            }
                            while(repl_dir_state_check(repl->dir_state) || (repl->qblk_set_shift == 1) );
                            
                            if ( repl_dir_state_check(repl->dir_state) || (repl->qblk_set_shift == 1))
                            {
                                repl = bak;
                                update_way_list (&ml2->sets[ml2_set], repl, Head);
                            }
                        }

                        if(repl_dir_state_check(repl->dir_state))
                        {       // If every block in this set is in "DIR_TRANSITION" state, it means the refill can not be done in this cycle. The p_event has to wait a "RETRY_TIME" time, and then request for the data again. @ fanglei
                            flip_counter ++;
                            event->flip_flag = 1;
                            event->when = sim_cycle + RETRY_TIME;
                            dir_eventq_insert(event);
                            return 1;
                        }
                        break;
                    case Random:
                        bindex = myrand () & (dl2->assoc - 1);
                        repl = CACHE_BINDEX (dl2, dl2->sets[dl2_set].blks, bindex);
                        break;
                    default:
                        panic ("bogus replacement policy");
                }

                // PVC; if the repl->ptr_en valid, we need to exchange here; @ fanglei
                // if the repl->ptr_en && !vector_available, we can not handle this now; @ fanglei
                if (ml2_hit && qblk_set_shift && (vector_num!=0) && (repl->ptr_en) && (Is_Shared(ml2_blk->dir_sharer[block_offset])>1) )
                {
                    int vector_available;
                    int sharer_num;
                    int temp_threadid;
                    struct cache_blk_t *vector_blk;
                    
                    vector_available = vector_available_func(&vector_blk, ml2_set);


                    
                    if(!vector_available)   // all the vector_blks are in transition; @ fanglei;
                    {
                        PVC_flip_num++;
                        event->isReqL2Trans = 1;
                        if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                        {
                            event->src1 += event->des1;
                            event->des1 = event->src1 - event->des1;
                            event->src1 = event->src1 - event->des1;
                            event->src2 += event->des2;
                            event->des2 = event->src2 - event->des2;
                            event->src2 = event->src2 - event->des2;
                            event->operation = NACK;
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(event);
                            if(collect_stats)
                                ml2->dir_access ++;
                            return 1;
                        }
                        else
                        {
                            event->when = sim_cycle + RETRY_TIME;
                            flip_counter ++;
                            event->flip_flag = 1;
                            dir_eventq_insert(event);
                            if(collect_stats)
                                ml2->dir_access ++;
                            return 1;
                        }                            
                    }
                    else        // one of the vector_blks will exchange with the pointer_blk; @ fanglei;
                    {
                        sharer_num = Is_Shared(vector_blk->dir_sharer[block_offset]);

                        PVC_exchange_num++;
                        PVC_exch_vec_shr_distr[sharer_num]++;

                        if( sharer_num < 2 ) // the vector_blk is exclusive and need no invalidation; @ fanglei1
                        {
                            PVC_dowm_no_inv_num++;

                            repl->ptr_en = 0;

                            vector_blk->ptr_en = 1;
                            if(vector_blk->all_shared_en)
                                panic("vector_blk all_shared_en illegal\n");
                            if( vector_blk->dir_blk_state[0] == MESI_SHARED )
                                panic("state error\n");
                        }
                        else if( sharer_num == mesh_size*mesh_size ) // the vector_blk is all_shared and need no invalidation; @ fanglei1
                        {
                            PVC_up_no_add_num++;

                            repl->ptr_en = 0;

                            vector_blk->ptr_en = 1;
                            vector_blk->all_shared_en = 1;
                            if(vector_blk->dir_sharer[0][0] != (unsigned long long int)all_sharer)
                                panic("all_shared_en dir_sharer errpr\n");
                            if( vector_blk->dir_blk_state[0] != MESI_SHARED )
                                panic("state error\n");
                        }
                        else    // the vector can not be changed into a pointer without lose of sharing information; @ fanglei
                        {
                            if(sharer_num > sharer_threshold)    // the vector is changed into all_shared; @ fanglei
                            {
                                PVC_up_add_num++;
                                PVC_up_add_shr_num += mesh_size*mesh_size-sharer_num;
                                PVC_up_add_shr_distr[mesh_size*mesh_size-sharer_num]++;

                                repl->ptr_en = 0;

                                vector_blk->ptr_en = 1;
                                vector_blk->all_shared_en = 1;
                                vector_blk->dir_sharer[0][0] = (unsigned long long int)all_sharer;
                                if( vector_blk->dir_blk_state[0] != MESI_SHARED )
                                    panic("state error\n");
                            }
                            else    // the vector is changed into exclusive; @ fanglei
                            {
                                PVC_dowm_inv_num++;
                                PVC_dowm_inv_shr_num += sharer_num-1;
                                PVC_dowm_inv_shr_distr[sharer_num-1]++;

                                repl->ptr_en = 0;

                                int ml2_dir_blk_state = vector_blk->dir_blk_state[0];
                                int survive_en = 1, survivor = -1;    // only a sharer can survive; @ fanglei
                                struct DIRECTORY_EVENT *collect_event = allocate_event(0);
                                if(collect_event == NULL)
                                    panic("Out of Virtual Memory");

                                vector_blk->ptr_en = 1;
                                if(vector_blk->all_shared_en)
                                    panic("vector_blk all_shared_en illegal\n");
                                vector_blk->all_shared_en = 0;
                                vector_blk->dir_blk_state[0] = MESI_EXCLUSIVE;
                                vector_blk->dir_state[0] = DIR_TRANSITION;
                                vector_blk->ptr_cur_event[0] = collect_event;
                                if (vector_blk->qblk_set_shift)
                                    vector_blk->addr = (vector_blk->tagid.tag << (cache_ml2->tag_shift+qblk_set_shift)) + (ml2_set << (cache_ml2->set_shift+qblk_set_shift));
                                else
                                    vector_blk->addr = (vector_blk->tagid.tag << cache_ml2->tag_shift) + (ml2_set << cache_ml2->set_shift);

                                collect_event->childCount = 0;
                                collect_event->individual_childCount[0] = 0;
                                collect_event->sharer_num = 1;     // indicate roll back or write invalid @ fanglei
                                collect_event->blk_dir = vector_blk;
                                collect_event->operation = VEC_DEGRADE;

                                for (temp_threadid = 0; temp_threadid < numcontexts; temp_threadid++)
                                {
                                    if(((vector_blk->dir_sharer[0][temp_threadid/64]) & ((unsigned long long int)1 << (temp_threadid%64))) == ((unsigned long long int)1 << (temp_threadid%64)))
                                    {
                                        if(survive_en)
                                        {
                                            survivor = temp_threadid;
                                            survive_en = 0;
                                            continue;
                                        }
                                        struct DIRECTORY_EVENT *new_event = allocate_event(0);
                                        if(new_event == NULL)
                                            panic("Out of Virtual Memory");

                                        // the ml2_blk is m_blk or s_blk; @ fanglei
                                        new_event->qblk_set_shift = vector_blk->qblk_set_shift;
                                        new_event->ml2_dir_blk_state = ml2_dir_blk_state;
                                        new_event->addr = vector_blk->addr;
                                        new_event->operation = INV_MSG_WRITE;
                                        new_event->parent = collect_event;
                                        new_event->blk_dir = vector_blk;                                            
                                        new_event->tempID = event->tempID;
                                        new_event->cp = event->cp;
                                        new_event->nbytes = event->cp->bsize;
                                        new_event->src1 = event->des1;
                                        new_event->src2 = event->des2;
                                        new_event->des1 = temp_threadid/mesh_size+MEM_LOC_SHIFT;
                                        new_event->des2 = temp_threadid%mesh_size;

                                        new_event->isPrefetch = 0;
                                        new_event->conf_bit = -1;
                                        new_event->req_time = 0;
                                        new_event->input_buffer_full = 0;
                                        new_event->pending_flag = 0;
                                        new_event->op = collect_event->op;
                                        new_event->vp = NULL;
                                        new_event->udata = NULL;
                                        new_event->rs = NULL;
                                        new_event->spec_mode = 0;
                                        new_event->data_reply = 1;

                                        /*go to network*/
                                        new_event->processingDone = 0;
                                        new_event->startCycle = sim_cycle;
                                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, 0);
                                        dir_eventq_insert(new_event);

                                        collect_event->childCount++;
                                        collect_event->individual_childCount[0] ++;
                                    }
                                }
                                if(survivor == -1)
                                    panic("survivor error\n");
                                vector_blk->dir_sharer[0][survivor/64] = ((unsigned long long int)1 << (survivor%64));
                            }
                        }
                        
                    }
                }

                int i=0, iteration = 1;
                for(i=0; i<(cache_dl2->set_shift - cache_dl1[0]->set_shift); i++)
                    iteration = iteration * 2;
                int m;
                if (repl->status & CACHE_BLK_VALID)
                {  /* replace the replacement line in L2 cache */
                    if( repl->ptr_en && repl->all_shared_en && (repl->dir_sharer[0][0] != (unsigned long long int)all_sharer) )
                        panic("recall ptr_en all_shared_en error \n");
                    if (collect_stats)
                        ml2->replacements++;

                    int ml2_dir_blk_state;
                    int new_vc = 0;
                    int flag = 0, sharer_num = 0;
                    for (i=0; i<iteration; i++)
                        if(Is_Shared(repl->dir_sharer[i]))
                            sharer_num ++;

                    event->childCount = 0;
                    event->sharer_num = 0;

                    for(i=0; i<iteration; i++)
                    {
                        if(Is_Shared(repl->dir_sharer[i]))
                        {  /* send invalidation messages to L1 cache of sharers*/
                            /* Invalidate multiple sharers */

                            ml2_dir_blk_state = repl->dir_blk_state[i];
                            if (    (ml2_dir_blk_state ==  MESI_SHARED) && (Is_Shared(repl->dir_sharer[i]) < 2) 
                                 || (ml2_dir_blk_state ==  MESI_MODIFIED) && (Is_Shared(repl->dir_sharer[i]) != 1) 
                                 || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) && (Is_Shared(repl->dir_sharer[i]) != 1) 
                                 || (ml2_dir_blk_state ==  MESI_INVALID)
                               )
                                printf("state conflicts with sharer vector\n");

                            for(m = 0; m<iteration; m++)
                            {
                                repl->dir_state[m] = DIR_TRANSITION;
                                repl->dir_blk_state[m] = MESI_INVALID;
                            }
                            repl->ptr_cur_event[i] = event;
                            if (repl->qblk_set_shift)
                                repl->addr = (repl->tagid.tag << (cache_ml2->tag_shift+qblk_set_shift)) + (ml2_set << (cache_ml2->set_shift+qblk_set_shift));
                            else
                                repl->addr = (repl->tagid.tag << cache_ml2->tag_shift) + (ml2_set << cache_ml2->set_shift) + (i << cache_dl1[0]->set_shift);
                            event->individual_childCount[i] = 0;
                            for (Threadid = 0; Threadid < numcontexts; Threadid++)
                            {
                                if(((repl->dir_sharer[i][Threadid/64]) & ((unsigned long long int)1 << (Threadid%64))) == ((unsigned long long int)1 << (Threadid%64)))
                                {
                                    recall_inv_num++;
                                    if(collect_stats)
                                        cache_ml2->invalidations ++;
                                    struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
                                    if(new_event == NULL)
                                        panic("Out of Virtual Memory");
                                    involve_4_hops_miss ++;
                                    new_event->conf_bit = -1;
                                    new_event->req_time = 0;
                                    new_event->input_buffer_full = 0;
                                    new_event->pending_flag = 0;
                                    new_event->op = event->op;
                                    new_event->operation = INV_MSG_WRITE;

                                    // the ml2_blk is m_blk or s_blk; @ fanglei
                                    new_event->qblk_set_shift = repl->qblk_set_shift;
                                    new_event->ml2_dir_blk_state = ml2_dir_blk_state;

                                    new_event->isPrefetch = 0;
                                    new_event->tempID = event->tempID;
                                    new_event->L2miss_flag = event->L2miss_flag;
                                    new_event->resend = 0;
                                    new_event->blk1 = repl;
                                    new_event->addr = repl->addr;
                                    new_event->cp = event->cp;
                                    new_event->vp = NULL;
                                    new_event->nbytes = event->cp->bsize;
                                    new_event->udata = NULL;
                                    new_event->rs = NULL;
                                    new_event->spec_mode = 0;
                                    new_event->src1 = event->des1;
                                    new_event->src2 = event->des2;
                                    new_event->des1 = Threadid/mesh_size+MEM_LOC_SHIFT;
                                    new_event->des2 = Threadid%mesh_size;
                                    new_event->processingDone = 0;
                                    new_event->startCycle = sim_cycle;
                                    new_event->parent = event;
                                    new_event->blk_dir = repl;
                                    new_event->data_reply = 1;
                                    event->sharer_num = sharer_num;
                                    event->blk_dir = repl;
                                    event->childCount++;
                                    event->individual_childCount[i] ++;
                                    /*go to network*/
                                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, new_vc);

                                    dir_eventq_insert(new_event);
                                }
                            }
                            flag = 1;

                            if(ml2_hit)
                                if(ml2_ed)
                                    if(repl->qblk_set_shift)
                                    {
                                        write_S_init_ed_QR_num++;
                                        if(ml2_dir_blk_state ==  MESI_SHARED)
                                            write_S_init_ed_QR_s_num++;
                                        else if ( (ml2_dir_blk_state ==  MESI_MODIFIED) || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) )
                                            write_S_init_ed_QR_ed_num++;
                                    }
                                    else                        write_S_init_ed_SR_num++;
                                else
                                    if(repl->qblk_set_shift)
                                    {
                                        write_S_init_s_QR_num++;
                                        if(ml2_dir_blk_state ==  MESI_SHARED)
                                            write_S_init_s_QR_s_num++;
                                        else if ( (ml2_dir_blk_state ==  MESI_MODIFIED) || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) )
                                            write_S_init_s_QR_ed_num++;
                                    }
                                    else                        write_S_init_s_SR_num++;
                            else
                                if(repl->qblk_set_shift)
                                {
                                    write_Q_init_QR_num++;
                                    if(ml2_dir_blk_state ==  MESI_SHARED)
                                        write_Q_init_QR_s_num++;
                                    else if ( (ml2_dir_blk_state ==  MESI_MODIFIED) || (ml2_dir_blk_state ==  MESI_EXCLUSIVE) )
                                        write_Q_init_QR_ed_num++;
                                }
                                else                        write_Q_init_SR_num++;

                            recall_num++;
                            if(repl->all_shared_en)
                            {
                                repl->all_shared_en = 0;
                                PVC_recall_all_shr_num++;
                            }

                            sharer_distr[event->childCount] ++;
                            IsNeighbor_sharer(repl->dir_sharer[i], tempID);
                        }
                    }
                    if(flag == 1)
                    {
                        // a later request can hit this blk @ fanglei
                        if(collect_stats)
                            dl2->replInv++;
                      
                        repl->status = CACHE_BLK_VALID;
                        repl->addr = addr;
                        repl->tagid.tag = ml2_tag;
                        repl->tagid.threadid = tempID;
                        for(m=0; m<iteration; m++)
                        {
                            repl->dir_sharer[m][tempID/64] = 0;
                            repl->dir_blk_state[m] = MESI_INVALID;
                        }
                        if(ml2_hit && qblk_set_shift)
                        {
                            if( (repl->ptr_en) && (Is_Shared(ml2_blk->dir_sharer[block_offset])>1) )
                                panic("unable to copy\n");
                            repl->qblk_set_shift = 0;
                            repl->dir_sharer[block_offset][tempID/64] = ml2_blk->dir_sharer[block_offset][tempID/64];   // fix me; @ fanglei
                            repl->dir_blk_state[block_offset] = ml2_blk->dir_blk_state[block_offset];
                            repl->dir_dirty[block_offset] = ml2_blk->dir_dirty[block_offset];
                        }
                        else if (qblk_set_shift)
                            repl->qblk_set_shift = 1;
                        else
                            repl->qblk_set_shift = 0;
                        return 1;
                    }
                }
                // no roll back, so the meta entry is valid now; @ fanglei
                for(m=0;m<iteration;m++)
                {
                    repl->ptr_cur_event[m] = NULL;
                    repl->dir_state[m] = DIR_STABLE;     // If block is invalid, the following operation is carried out.
                    repl->dir_sharer[m][tempID/64] = 0;
                    repl->dir_blk_state[m] = MESI_INVALID;
                    repl->dir_dirty[m] = -1;
                    repl->exclusive_time[m] = sim_cycle;
                }
                repl->status = CACHE_BLK_VALID;
                repl->ready = sim_cycle;
                repl->tagid.tag = ml2_tag;
                repl->tagid.threadid = tempID;
                if(ml2_hit && qblk_set_shift)
                    repl->qblk_set_shift = 0;
                else if (qblk_set_shift)
                    repl->qblk_set_shift = 1;
                else
                    repl->qblk_set_shift = 0;
                repl->addr = addr;
                if (repl->way_prev && ml2->policy == LRU)
                    update_way_list(&ml2->sets[ml2_set], repl, Head);
                if (ml2->hsize)
                    link_htab_ent(ml2, &ml2->sets[ml2_set], repl);
                ml2->last_tagsetid.tag = 0;
                ml2->last_tagsetid.threadid = tempID;
                ml2->last_blk = repl;
                // for m_blk & s_blk, we return; @ fanglei
                if (ml2_hit && qblk_set_shift)
                {
                    if( (repl->ptr_en) && (Is_Shared(ml2_blk->dir_sharer[block_offset])>1) )
                        panic("unable to copy\n");
                    repl->dir_sharer[block_offset][tempID/64] = ml2_blk->dir_sharer[block_offset][tempID/64];
                    repl->dir_blk_state[block_offset] = ml2_blk->dir_blk_state[block_offset];
                    repl->dir_dirty[block_offset] = ml2_blk->dir_dirty[block_offset];
                    event->when = sim_cycle;
                    event->no_L2_delay = 1;
                    dir_eventq_insert(event);
                    return 1;
                }
                // non shared && hit in L2 data cache @ fanglei
                if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                {
                    ml2->data_access ++;
                    involve_2_hops ++;
                    if(event->rs)
                        if(event->rs->op == LDL_L)
                            load_link_shared ++;
                    repl->dir_state[block_offset] = DIR_STABLE;
                    repl->ptr_cur_event[block_offset] = NULL;
                    repl->exclusive_time[block_offset] = sim_cycle;
                    if(!md_text_addr(addr, tempID))
                    {
                        repl->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64);
                        repl->dir_blk_state[block_offset] = MESI_MODIFIED;
                        repl->dir_dirty[block_offset] = tempID;
                    }
                    else
                        printf("directory is for data segment\n");

                    event->operation = ACK_DIR_WRITE;
                    event->reqAckTime = sim_cycle;
                    event->src1 += event->des1;
                    event->des1 = event->src1 - event->des1;
                    event->src1 = event->src1 - event->des1;
                    event->src2 += event->des2;
                    event->des2 = event->src2 - event->des2;
                    event->src2 = event->src2 - event->des2;
                    event->processingDone = 0;
                    event->startCycle = sim_cycle;
                    event->parent = NULL;
                    event->blk_dir = NULL;
                    event->arrival_time = sim_cycle - event->arrival_time;
                    scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                    dir_eventq_insert(event);
                    return 1;
                }
                // non shared && miss in L2 data cache @ fanglei
                else
                {
                    repl->dir_state[block_offset] = DIR_TRANSITION;     // data miss, we prevent from replacing this meta entry; @ fanglei
                    matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                    event->L2miss_flag = 1;
                    event->L2miss_stated = sim_cycle;
                    if(event->isSyncAccess)
                        Sync_L2_miss ++;
                    if(!matchnum)
                    { /* No match in L2 MSHR */
                        if(!cache_dl2->mshr->freeEntries)
                        {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                            if(!last_L2_mshr_full)
                            {
                                L2_mshr_full ++;
                                last_L2_mshr_full = sim_cycle;
                            }
                            if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                            {
                                dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                                //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                event->src1 += event->des1;
                                event->des1 = event->src1 - event->des1;
                                event->src1 = event->src1 - event->des1;
                                event->src2 += event->des2;
                                event->des2 = event->src2 - event->des2;
                                event->src2 = event->src2 - event->des2;
                                event->operation = NACK;
                                repl->dir_state[block_offset] = DIR_STABLE;
                                event->processingDone = 0;
                                event->startCycle = sim_cycle;
                                scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                                dir_eventq_insert(event);
                                return 1;
                            }
                            else
                            {
                                repl->dir_state[block_offset] = DIR_STABLE;
                                dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                                event->when = sim_cycle + RETRY_TIME;
                                event->flip_flag = 1;
                                flip_counter ++;
                                dir_eventq_insert(event);
                                return 1;
                            }
                        }
                        else
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                            if(last_L2_mshr_full)
                            {
                                Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                last_L2_mshr_full = 0;
                            }
                        }
                        if(collect_stats)
                        {
                            cache_dl2->misses ++;
                            cache_dl2->dmisses ++;
                            involve_2_hops_miss ++;
                        }
                        event->reqAtDirTime = sim_cycle;
                        event->mshr_time = sim_cycle;
                        event->blk_dir = repl;
                        struct DIRECTORY_EVENT *new_event;
                        new_event = allocate_new_event(event);
                        new_event->operation = MEM_READ_REQ;
                        new_event->cmd = Read;
                        event->when = sim_cycle + WAIT_TIME;
                        event->operation = WAIT_MEM_READ;
                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(new_event);
                        event->startCycle = sim_cycle;
                        event_created = event;
                        MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                        return 1;
                    }
                    else
                    {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                        event->isReqL2SecMiss = 1;
                        if(collect_stats)
                        {
                            cache_dl2->in_mshr ++;
                            cache_dl2->din_mshr ++;
                        }
                        event_list_insert(event, matchnum);
                        repl->dir_state[block_offset] = DIR_STABLE;
                        return 1;
                    }
                }
            }

            //  no need to access the meta; @ fanglei
            if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
            {/* Hit */
//                dl2_blk->status |= CACHE_BLK_DIRTY;
                if (dl2_blk->way_prev && dl2->policy == LRU)
                    update_way_list (&dl2->sets[dl2_set], dl2_blk, Head);
                /* See the status of directory */
                event->isReqL2Hit = 1;
                if(event->L2miss_flag && event->L2miss_stated)
                    event->L2miss_complete = sim_cycle;
                if(event->rs)
                    if(event->rs->op == LDL_L)
                        load_link_shared ++;
                event->operation = ACK_DIR_WRITE;
                event->src1 += event->des1;
                event->des1 = event->src1 - event->des1;
                event->src1 = event->src1 - event->des1;
                event->src2 += event->des2;
                event->des2 = event->src2 - event->des2;
                event->src2 = event->src2 - event->des2;
                event->processingDone = 0;
                event->startCycle = sim_cycle;
                event->parent = NULL;
                event->blk_dir = NULL;
                event->arrival_time = sim_cycle - event->arrival_time;
                scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                dir_eventq_insert(event);
                return 1;
            }
            else
            {   /* L2 Miss */
                matchnum = MSHR_block_check(cache_dl2->mshr, event->addr, cache_dl2->set_shift);
                event->L2miss_flag = 1;
                event->L2miss_stated = sim_cycle;
                event->blk_dir = NULL;
                if(event->isSyncAccess)
                    Sync_L2_miss ++;
                if(!matchnum)
                { /* No match in L2 MSHR */
                    if(!cache_dl2->mshr->freeEntries)
                    {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                        if(!last_L2_mshr_full)
                        {
                            L2_mshr_full ++;
                            last_L2_mshr_full = sim_cycle;
                        }
                        if(tshr_fifo_num[event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] = dir_fifo_full[event->des1*mesh_size+event->des2] - 4;
                            //dir_fifo_nack[event->des1*mesh_size+event->des2]
                            event->src1 += event->des1;
                            event->des1 = event->src1 - event->des1;
                            event->src1 = event->src1 - event->des1;
                            event->src2 += event->des2;
                            event->des2 = event->src2 - event->des2;
                            event->src2 = event->src2 - event->des2;
                            event->operation = NACK;
                            event->processingDone = 0;
                            event->startCycle = sim_cycle;
                            scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(event);
                            return 1;
                        }
                        else
                        {
                            dir_fifo_full[event->des1*mesh_size+event->des2] ++;
                            event->when = sim_cycle + RETRY_TIME;
                            event->flip_flag = 1;
                            flip_counter ++;
                            dir_eventq_insert(event);
                            return 1;
                        }
                    }
                    else
                    {
                        dir_fifo_full[event->des1*mesh_size+event->des2] = 0;
                        if(last_L2_mshr_full)
                        {
                            Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                            last_L2_mshr_full = 0;
                        }
                    }
                    if(collect_stats)
                    {
                        cache_dl2->misses ++;
                        cache_dl2->dmisses ++;
                        involve_2_hops_miss ++;
                    }
                    event->reqAtDirTime = sim_cycle;
                    event->mshr_time = sim_cycle;
                    struct DIRECTORY_EVENT *new_event;
                    new_event = allocate_new_event(event);
                    new_event->operation = MEM_READ_REQ;
                    new_event->cmd = Read;
                    event->when = sim_cycle + WAIT_TIME;
                    event->operation = WAIT_MEM_READ;
                    scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                    dir_eventq_insert(new_event);
                    event->startCycle = sim_cycle;
                    event_created = event;
                    MSHRLookup(cache_dl2->mshr, event->addr, WAIT_TIME, 0, NULL);
                    return 1;
                }
                else
                {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                    event->isReqL2SecMiss = 1;
                    if(collect_stats)
                    {
                        cache_dl2->in_mshr ++;
                        cache_dl2->din_mshr ++;
                    }
                    event_list_insert(event, matchnum);
                    return 1;
                }
            }
            break;

        case INFORM_BACK    :
        case WRITE_BACK     :
            dcache2_access++;
#ifdef CENTRALIZED_L2

#else
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache2_access++;
#endif
            event->parent_operation = event->operation;
            if(md_text_addr(addr, tempID))
                panic("Writes cannot access text memory region");
            if ( event->operation == WRITE_BACK )
            {
                if ( findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk) )
                    dl2_blk->status |= CACHE_BLK_DIRTY;
                // L1 write back && L2 data cache miss, we have to write back to memory; @ fanglei
                else
                {
                    struct DIRECTORY_EVENT *new_event;
                    new_event = allocate_new_event(event);
                    new_event->addr = event->addr;
                    new_event->operation = MEM_WRITE_BACK;
                    new_event->cmd = Write;
                    scheduleThroughNetwork(new_event, sim_cycle, data_packet_size, 0);
                    dir_eventq_insert(new_event);
                }
            }
            if(findCacheStatus(ml2, ml2_sub_set, ml2_sub_tag, ml2_hindex, &ml2_blk))
            {/* hit */
                if( ml2_blk->ptr_en && ml2_blk->all_shared_en && (ml2_blk->dir_sharer[0][0] != (unsigned long long int)all_sharer) )
                    panic("inform/write_back ptr_en all_shared_en error \n");
                if( ml2_blk->all_shared_en && (event->operation == WRITE_BACK) )
                    panic("WRITE_BACK all_shared_en error \n");
                if(!ml2_blk->qblk_set_shift )
                {
                    if(ml2_blk->all_shared_en && (event->operation == INFORM_BACK))
                        PVC_inform_all_shr_num++;
                    else
                    {
                        ml2_blk->dir_sharer[block_offset][tempID/64] &= ~ ((unsigned long long int)1 << (((event->src1-MEM_LOC_SHIFT)*mesh_size+event->src2)%64));
                        if ( !ml2_blk->dir_sharer[block_offset][tempID/64] )
                        {
                            ml2_blk->dir_blk_state[block_offset] = MESI_INVALID;
                            ml2_blk->dir_dirty[block_offset] = -1;
                        }
                        if (Is_ExclusiveOrDirty(ml2_blk->dir_sharer[block_offset]))
                        {
                            ml2_blk->dir_blk_state[block_offset] = MESI_EXCLUSIVE;
                        }
                    }
                }
                if( event->operation == WRITE_BACK )
                {
                    write_back_num++;
                    if( ml2_blk->qblk_set_shift )
                        write_back_Q++;
                    else
                        write_back_S++;
                }
                else
                {
                    inform_back_num++;
                    if( ml2_blk->qblk_set_shift )
                        inform_back_Q++;
                    else
                        inform_back_S++;
                }
            }
            free_event(event);
            break;

        case INV_MSG_READ   :
            dcache_access++;
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache_access++;
            if(md_text_addr(addr, tempID))
                panic("Writes cannot access text memory region");
            count = (event->des1-MEM_LOC_SHIFT) * mesh_size + event->des2;
            cp1 = cache_dl1[count];
            tag = CACHE_TAG(cp1,addr);
            set = CACHE_SET(cp1,addr);
            if(collect_stats)
                cp1->Invalid_Read_received ++;

            packet_size = meta_packet_size;
            event->operation = ACK_MSG_READUPDATE;

            for (dl1_blk = cp1->sets[set].way_head; dl1_blk; dl1_blk =dl1_blk->way_next)
            {
                if ((dl1_blk->tagid.tag == tag) && (dl1_blk->status & CACHE_BLK_VALID))
                {
                    L1_hit = 1;
                    if(dl1_blk->state == MESI_MODIFIED)
                    {  /* dl2_blk state is modified, then the dl2_blk data should be carried back into L2 cache at home node */
                        packet_size = data_packet_size;
                        event->operation = ACK_MSG_READ;
                        if(collect_stats)
                            cp1->wb_coherence_r ++;
                        if(event->data_reply == 0)
                            total_exclusive_cross ++;
                    }
                    else if(dl1_blk->state == MESI_EXCLUSIVE)
                    {
                        packet_size = meta_packet_size;
                        event->operation = ACK_MSG_READUPDATE;
                    }
                    if (event->qblk_set_shift)
                        read_inv_QH_num++;
                    else
                        read_inv_S_num++;

                    /* dl2_blk state changed from exclusive/modified to shared state */
                    if( (vector_num==0) && !nv_broadcast )
                    {
                        dl1_blk->state = MESI_INVALID;
                        dl1_blk->status = 0;
                    }
                    else
                    {
                        dl1_blk->state = MESI_SHARED;
                        dl1_blk->status = CACHE_BLK_VALID;
                    }
                    if(collect_stats)
                        cp1->Invalid_Read_received_hits ++;
                    break;
                }
            }
            if ( (event->shr_data_req) && (packet_size == meta_packet_size) && L1_hit )
            {
                packet_size = data_packet_size;
                event->operation = ACK_MSG_READUPDATE;
            }
            event->L1_hit = L1_hit;
            event->src1 += event->des1;
            event->des1 = event->src1 - event->des1;
            event->src1 = event->src1 - event->des1;
            event->src2 += event->des2;
            event->des2 = event->src2 - event->des2;
            event->src2 = event->src2 - event->des2;
            event->processingDone = 0;
            event->startCycle = sim_cycle;
            /* schedule a network access */
            scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
            dir_eventq_insert(event);

            // other basic blk of the same qblk; @ fanglei
            if (event->qblk_set_shift)
            {
                int i;
                for(i=0;i<(1<<multi_blk_set_shift);i++)
                {
                    addr = (addr & (~(qblk_set_mask<<ml2->set_shift))) + (i<<ml2->set_shift);
                    if(addr==event->addr)
                        continue;
                    tag = CACHE_TAG(cp1,addr);
                    set = CACHE_SET(cp1,addr);

                    for (dl1_blk = cp1->sets[set].way_head; dl1_blk; dl1_blk =dl1_blk->way_next)
                    {
                        if ((dl1_blk->tagid.tag == tag) && (dl1_blk->status & CACHE_BLK_VALID))
                        {
                            if(dl1_blk->state == MESI_MODIFIED)
                            {  /* dl2_blk state is modified, then the dl2_blk data should be carried back into L2 cache at home node */
                                struct DIRECTORY_EVENT *new_event;
                                new_event = allocate_event(0);
                                new_event->parent = event->parent;
                                new_event->blk_dir = event->blk_dir;
                                new_event->operation = ACK_MSG_READ;
                                new_event->addr = addr;
                                new_event->src1 = event->src1;
                                new_event->src2 = event->src2;
                                new_event->des1 = event->des1;
                                new_event->des2 = event->des2;
                                new_event->processingDone = 0;
                                new_event->startCycle = sim_cycle;
                                event->parent->childCount ++;
                                read_inv_d_QM_num++;
                                scheduleThroughNetwork(new_event, sim_cycle, data_packet_size, vc);
                                dir_eventq_insert(new_event);
                            }
                            else
                                read_inv_e_QM_num++;
                            // for sharer identify, but not shr_data_req; @ fanglei
                            event->L1_hit |= 2;

                            /* dl2_blk state changed from exclusive/modified to shared state */
                            if( (vector_num==0) && !nv_broadcast )
                            {
                                dl1_blk->state = MESI_INVALID;
                                dl1_blk->status = 0;
                            }
                            else
                            {
                                dl1_blk->state = MESI_SHARED;
                                dl1_blk->status = CACHE_BLK_VALID;
                            }
                            break;
                        }
                    }
                }
            }
            return 1;

        case INV_MSG_WRITE  :
            dcache_access++;
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache_access++;
            if(md_text_addr(addr, tempID))
                panic("Writes cannot access text memory region");
            count = (event->des1-MEM_LOC_SHIFT) * mesh_size + event->des2;
            cp1 = cache_dl1[count];
            tag = CACHE_TAG(cp1,addr);
            set = CACHE_SET(cp1,addr);
            sendLSQInv(addr, count);

            packet_size = meta_packet_size;
            event->operation = ACK_MSG_WRITEUPDATE;
            for (dl1_blk = cp1->sets[set].way_head; dl1_blk; dl1_blk =dl1_blk->way_next)
            {
                if ((dl1_blk->tagid.tag == tag) && (dl1_blk->status & CACHE_BLK_VALID))
                {
                    L1_hit = 1;

                    if( event->parent->sharer_num )
                    {
                        if(event->qblk_set_shift)
                            if(dl1_blk->state == MESI_MODIFIED)         
                            {
                                recall_inv_d_Q_num++;
                                if(event->ml2_dir_blk_state == MESI_SHARED)
                                {
                                    if(recall_ad_en)
                                    {
                                        recall_inv_d_Q_num--;
                                        recall_inv_d_Q_s_ad_num++;
                                        break;
                                    }
                                    recall_inv_d_Q_s_num++;
                                }
                                else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                                    recall_inv_d_Q_ed_num++;
                            }
                            else if(dl1_blk->state == MESI_EXCLUSIVE)   
                            {
                                recall_inv_e_Q_num++;
                                if(event->ml2_dir_blk_state == MESI_SHARED)
                                {
                                    if(recall_ad_en)
                                    {
                                        recall_inv_e_Q_num--;
                                        recall_inv_e_Q_s_ad_num++;
                                        break;
                                    }
                                    recall_inv_e_Q_s_num++;
                                }
                                else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                                    recall_inv_e_Q_ed_num++;
                            }
                            else                                        
                            {
                                recall_inv_s_Q_num++;
                                if(event->ml2_dir_blk_state == MESI_SHARED)
                                    recall_inv_s_Q_s_num++;
                                else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                                {
                                    if(recall_ad_en)
                                    {
                                        recall_inv_s_Q_num--;
                                        recall_inv_s_Q_ed_ad_num++;
                                        break;
                                    }
                                    recall_inv_s_Q_ed_num++;
                                }
                            }
                        else
                            if(dl1_blk->state == MESI_MODIFIED)         recall_inv_d_S_num++;
                            else if(dl1_blk->state == MESI_EXCLUSIVE)   recall_inv_e_S_num++;
                            else                                        recall_inv_s_S_num++;
                    }
                    else
                    {
                        if(event->qblk_set_shift)
                            if(dl1_blk->state == MESI_MODIFIED)         write_inv_d_QH_num++;
                            else if(dl1_blk->state == MESI_EXCLUSIVE)   write_inv_e_QH_num++;
                            else                                        write_inv_s_QH_num++;
                        else
                            if(dl1_blk->state == MESI_MODIFIED)         write_inv_d_S_num++;
                            else if(dl1_blk->state == MESI_EXCLUSIVE)   write_inv_e_S_num++;
                            else                                        write_inv_s_S_num++;
                    }

                    if(dl1_blk->state == MESI_MODIFIED)
                    {  /* dl2_blk state is modified, then the dl2_blk data should be carried back into L2 cache at home node */
                        packet_size = data_packet_size;
                        event->operation = ACK_MSG_WRITE;
                    }
                    else
                    {
                        packet_size = meta_packet_size;
                        event->operation = ACK_MSG_WRITEUPDATE;
                    }

                    /* dl2_blk state changed from exclusive/modified to shared state */
                    dl1_blk->state = MESI_INVALID;
                    dl1_blk->status = 0;
                    break;
                }
            }

            if(!L1_hit)
                if( event->parent->sharer_num )
                    if(event->qblk_set_shift)   
                    {
                        recall_inv_i_Q_num++;
                        if(event->ml2_dir_blk_state == MESI_SHARED)
                            recall_inv_i_Q_s_num++;
                        else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                            recall_inv_i_Q_ed_num++;
                    }
                    else                        recall_inv_i_S_num++;
                else
                    if(event->qblk_set_shift)   write_inv_i_QH_num++;
                    else                        write_inv_i_S_num++;

            if ( (event->shr_data_req) && (packet_size == meta_packet_size) && L1_hit )
            {
                packet_size = data_packet_size;
                event->operation = ACK_MSG_WRITEUPDATE;
            }
            event->L1_hit = L1_hit;
            event->src1 += event->des1;
            event->des1 = event->src1 - event->des1;
            event->src1 = event->src1 - event->des1;
            event->src2 += event->des2;
            event->des2 = event->src2 - event->des2;
            event->src2 = event->src2 - event->des2;
            event->processingDone = 0;
            event->startCycle = sim_cycle;
            scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
            dir_eventq_insert(event);

            // other basic blk of the same qblk; @ fanglei
            if (event->qblk_set_shift)
            {
                int i;
                for(i=0;i<(1<<multi_blk_set_shift);i++)
                {
                    addr = (addr & (~(qblk_set_mask<<ml2->set_shift))) + (i<<ml2->set_shift);
                    if(addr==event->addr)
                        continue;
                    tag = CACHE_TAG(cp1,addr);
                    set = CACHE_SET(cp1,addr);
                    sendLSQInv(addr, count);
                    L1_hit = 0;
                    for (dl1_blk = cp1->sets[set].way_head; dl1_blk; dl1_blk =dl1_blk->way_next)
                    {
                        if ((dl1_blk->tagid.tag == tag) && (dl1_blk->status & CACHE_BLK_VALID))
                        {
                            L1_hit = 1;

                            if( event->parent->sharer_num )
                            {
                                if(dl1_blk->state == MESI_MODIFIED)         
                                {
                                    recall_inv_d_Q_num++;
                                    if(event->ml2_dir_blk_state == MESI_SHARED)
                                    {
                                        if(recall_ad_en)
                                        {
                                            recall_inv_d_Q_num--;
                                            recall_inv_d_Q_s_ad_num++;
                                            break;
                                        }                                        
                                        recall_inv_d_Q_s_num++;
                                    }
                                    else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                                        recall_inv_d_Q_ed_num++;
                                }
                                else if(dl1_blk->state == MESI_EXCLUSIVE)   
                                {
                                    recall_inv_e_Q_num++;
                                    if(event->ml2_dir_blk_state == MESI_SHARED)
                                    {
                                        if(recall_ad_en)
                                        {
                                            recall_inv_e_Q_num--;
                                            recall_inv_e_Q_s_ad_num++;
                                            break;
                                        }   
                                        recall_inv_e_Q_s_num++;
                                    }
                                    else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                                        recall_inv_e_Q_ed_num++;
                                }
                                else                                        
                                {
                                    recall_inv_s_Q_num++;
                                    if(event->ml2_dir_blk_state == MESI_SHARED)
                                        recall_inv_s_Q_s_num++;
                                    else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                                    {
                                        if(recall_ad_en)
                                        {
                                            recall_inv_s_Q_num--;
                                            recall_inv_s_Q_ed_ad_num++;
                                            break;
                                        }   
                                        recall_inv_s_Q_ed_num++;
                                    }
                                }
                            }
                            else
                            {
                                if(dl1_blk->state == MESI_MODIFIED)         write_inv_d_QM_num++;
                                else if(dl1_blk->state == MESI_EXCLUSIVE)   write_inv_e_QM_num++;
                                else                                        write_inv_s_QM_num++;
                            }

                            if(dl1_blk->state == MESI_MODIFIED)
                            {  /* dl2_blk state is modified, then the dl2_blk data should be carried back into L2 cache at home node */
                                struct DIRECTORY_EVENT *new_event;
                                new_event = allocate_event(0);
                                new_event->parent = event->parent;
                                new_event->blk_dir = event->blk_dir;
                                new_event->operation = ACK_MSG_WRITE;
                                new_event->addr = addr;
                                new_event->src1 = event->src1;
                                new_event->src2 = event->src2;
                                new_event->des1 = event->des1;
                                new_event->des2 = event->des2;
                                new_event->processingDone = 0;
                                new_event->startCycle = sim_cycle;
                                event->parent->childCount ++;
                                if(event->parent->sharer_num)
                                    event->parent->individual_childCount[block_offset] ++;
                                scheduleThroughNetwork(new_event, sim_cycle, data_packet_size, vc);
                                dir_eventq_insert(new_event);
                            }

                            /* dl2_blk state changed from exclusive/modified to shared state */
                            dl1_blk->state = MESI_INVALID;
                            dl1_blk->status = 0;
                            break;
                        }
                    }

                    if(!L1_hit)
                        if( event->parent->sharer_num )     
                        {
                            recall_inv_i_Q_num++;
                            if(event->ml2_dir_blk_state == MESI_SHARED)
                                recall_inv_i_Q_s_num++;
                            else if((event->ml2_dir_blk_state == MESI_MODIFIED) || (event->ml2_dir_blk_state == MESI_EXCLUSIVE))
                                recall_inv_i_Q_ed_num++;
                        }
                        else                                write_inv_i_QM_num++;
                }
            }
            return 1;

        case DIR_SHR_READ:
            count = (event->des1-MEM_LOC_SHIFT) * mesh_size + event->des2;
            cp1 = cache_dl1[count];
            tag = CACHE_TAG(cp1,addr);
            set = CACHE_SET(cp1,addr);
            for (dl1_blk = cp1->sets[set].way_head; dl1_blk; dl1_blk =dl1_blk->way_next)
            {
                if ((dl1_blk->tagid.tag == tag) && (dl1_blk->status & CACHE_BLK_VALID))
                    L1_hit = 1;
            }
            if ( L1_hit )
                packet_size = data_packet_size;
            else
                packet_size = meta_packet_size;
            event->operation = ACK_SHR_READ;
            event->L1_hit = L1_hit;
            event->src1 += event->des1;
            event->des1 = event->src1 - event->des1;
            event->src1 = event->src1 - event->des1;
            event->src2 += event->des2;
            event->des2 = event->src2 - event->des2;
            event->src2 = event->src2 - event->des2;
            event->processingDone = 0;
            event->startCycle = sim_cycle;
            /* schedule a network access */
            scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
            dir_eventq_insert(event);
            return 1;

        case ACK_SHR_READ:
            if(event->blk_dir->all_shared_en)
                panic("ACK_SHR_READ all_shared_en error \n");
            (event->blk_dir)->dir_state[block_offset] = DIR_STABLE;
            if ( !event->L1_hit && !event->qblk_set_shift)
                event->blk_dir->dir_sharer[block_offset][tempID/64] &= ~ ((unsigned long long int)1 << (((event->src1-MEM_LOC_SHIFT)*mesh_size+event->src2)%64));
            if ( !event->blk_dir->dir_sharer[block_offset][tempID/64] )
            {
                event->blk_dir->dir_blk_state[block_offset] = MESI_INVALID;
                event->blk_dir->dir_dirty[block_offset] = -1;
            }
            if (Is_ExclusiveOrDirty((event->blk_dir)->dir_sharer[block_offset]))
            {
                event->blk_dir->dir_blk_state[block_offset] = MESI_EXCLUSIVE;
            }

            if (event->L1_hit || findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
            {

                int vector_count;
                int PVC_ml2_set;

                if ( ml2_set_match_func(event->blk_dir, ml2_mul_set) )
                    PVC_ml2_set = ml2_mul_set;
                else if ( ml2_set_match_func(event->blk_dir, ml2_sub_set) )
                    PVC_ml2_set = ml2_sub_set;
                else
                    panic("must hit\n");

                vector_count = vector_count_func(event->blk_dir);
                if(Is_Shared(event->blk_dir->dir_sharer[block_offset]) == 1)
                {
//                    panic("should be shared\n");  // inform may change the directory into exclusive; @ fanglei
                    PVC_vector_inc_distr_record_num++;
                    PVC_vector_inc_distr[vector_count]++;
                    cache_ml2->sets[PVC_ml2_set].PVC_vector_inc_distr_record_num++;
                    cache_ml2->sets[PVC_ml2_set].PVC_vector_inc_distr[vector_count]++;
                    vector_count++;
                }

                PVC_vector_distr_record_num++;
                PVC_vector_distr[vector_count]++;
                cache_ml2->sets[PVC_ml2_set].PVC_vector_distr_record_num++;
                cache_ml2->sets[PVC_ml2_set].PVC_vector_distr[vector_count]++;

                if( event->blk_dir->ptr_en )
                    panic("no sharer data obtain for pointer\n");
                event->blk_dir->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64) | (event->blk_dir)->dir_sharer[block_offset][tempID/64];
                event->blk_dir->dir_blk_state[block_offset] = IsShared((event->blk_dir)->dir_sharer[block_offset], tempID) ? MESI_SHARED : MESI_EXCLUSIVE;
                event->operation = IsShared((event->blk_dir)->dir_sharer[block_offset], tempID) ? ACK_DIR_READ_SHARED : ACK_DIR_READ_EXCLUSIVE;
                packet_size = data_packet_size;
                event->reqAckTime = sim_cycle;
                event->src1 = event->des1;
                event->src2 = event->des2;
                event->des1 = tempID/mesh_size+MEM_LOC_SHIFT;
                event->des2 = tempID%mesh_size;
                event->processingDone = 0;
                event->startCycle = sim_cycle;
                scheduleThroughNetwork(event, sim_cycle, packet_size, vc);
                dir_eventq_insert(event);
                return 1;
            }
            else
            {
                if(event->blk_dir->qblk_set_shift)
                {
                    read_Q_hit_num--;
                    mb_pain_count--;
                }
                else
                    read_S_hit_num--;
                event->operation = event->parent_operation;
                event->src1 = tempID/mesh_size+MEM_LOC_SHIFT;
                event->src2 = tempID%mesh_size;
                event->when = sim_cycle;
                event->no_L2_delay = 1;
                dir_eventq_insert(event);
                return 1;
            }
            break;

        case ACK_MSG_READ   :
        case ACK_MSG_READUPDATE :
            dcache2_access++;
#ifdef CENTRALIZED_L2

#else
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache2_access++;
#endif
            int Readpending = 0;
            if(md_text_addr(addr, tempID))
                panic("Writes cannot access text memory region");
            if(parent_event->blk_dir->dir_state[block_offset] == DIR_STABLE)
                panic("DIR: after the receipt of ACK msg, directory not in transient state");

            parent_event->childCount --;
            if (event->L1_hit)
            {
                parent_event->L1_hit = 1;
                if ( event->addr == parent_event->addr && (event->L1_hit & 1))
                    parent_event->shr_data_req = 0;
            }
            if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk) && (event->operation == ACK_MSG_READ))
                dl2_blk->status |= CACHE_BLK_DIRTY;

            if(parent_event->childCount != 0)
                free_event(event);
            else
            {
                if(!parent_event->L1_hit)
                {
                    if (parent_event->blk_dir->all_shared_en)
                        panic("ACK_MSG_READ all_shared_en error\n");
                    parent_event->blk_dir->dir_sharer[block_offset][(tempID/64)] &= ~ ((unsigned long long int)1 << (((event->src1-MEM_LOC_SHIFT)*mesh_size+event->src2)%64));
                    parent_event->blk_dir->dir_blk_state[block_offset] = MESI_INVALID;
                    parent_event->blk_dir->dir_dirty[block_offset] = -1;
                }
                if( vector_num == 0 )
                {
                    if( nv_broadcast )
                    {
                        parent_event->blk_dir->all_shared_en = 1;
                        parent_event->blk_dir->dir_sharer[block_offset][(tempID/64)] = (unsigned long long int)all_sharer;
                        parent_event->blk_dir->dir_blk_state[block_offset] = MESI_SHARED;
                        parent_event->blk_dir->dir_dirty[block_offset] = -1;
                    }
                    else
                    {
                        parent_event->blk_dir->all_shared_en = 0;
                        parent_event->blk_dir->dir_sharer[block_offset][(tempID/64)] = 0;
                        parent_event->blk_dir->dir_blk_state[block_offset] = MESI_INVALID;
                        parent_event->blk_dir->dir_dirty[block_offset] = -1;
                    }
                }

                dl2_tag = CACHE_TAG(dl2, parent_event->addr);
                dl2_set = CACHE_SET(dl2, parent_event->addr);

                // we ask for data from owner but it is evicted; @ fanglei
                if ( parent_event->shr_data_req && !findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                {
                    free_event(event);
                    if(parent_event->blk_dir->qblk_set_shift)
                    {
                        read_Q_hit_num--;
                        mb_pain_count--;
                    }
                    else
                        read_S_hit_num--;

                    matchnum = MSHR_block_check(cache_dl2->mshr, parent_event->addr, cache_dl2->set_shift);
                    parent_event->L2miss_flag = 1;
                    parent_event->L2miss_stated = sim_cycle;
                    if(parent_event->isSyncAccess)
                        Sync_L2_miss ++;
                    if(!matchnum)
                    { /* No match in L2 MSHR */
                        if(!cache_dl2->mshr->freeEntries)
                        {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                            if(!last_L2_mshr_full)
                            {
                                L2_mshr_full ++;
                                last_L2_mshr_full = sim_cycle;
                            }
                            if(tshr_fifo_num[parent_event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                            {
                                dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] = dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] - 4;
                                //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                parent_event->src1 += parent_event->des1;
                                parent_event->des1 = parent_event->src1 - parent_event->des1;
                                parent_event->src1 = parent_event->src1 - parent_event->des1;
                                parent_event->src2 += parent_event->des2;
                                parent_event->des2 = parent_event->src2 - parent_event->des2;
                                parent_event->src2 = parent_event->src2 - parent_event->des2;
                                parent_event->operation = NACK;
                                parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                                parent_event->processingDone = 0;
                                parent_event->startCycle = sim_cycle;
                                scheduleThroughNetwork(parent_event, sim_cycle, meta_packet_size, vc);
                                dir_eventq_insert(parent_event);
                                return 1;
                            }
                            else
                            {
                                parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                                dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] ++;
                                parent_event->when = sim_cycle + RETRY_TIME;
                                parent_event->flip_flag = 1;
                                flip_counter ++;
                                dir_eventq_insert(parent_event);
                                return 1;
                            }
                        }
                        else
                        {
                            dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] = 0;
                            if(last_L2_mshr_full)
                            {
                                Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                last_L2_mshr_full = 0;
                            }
                        }
                        if(collect_stats)
                        {
                            cache_dl2->misses ++;
                            cache_dl2->dmisses ++;
                            involve_2_hops_miss ++;
                        }
                        parent_event->reqAtDirTime = sim_cycle;
                        parent_event->mshr_time = sim_cycle;
                        struct DIRECTORY_EVENT *new_event;
                        new_event = allocate_new_event(parent_event);
                        new_event->operation = MEM_READ_REQ;
                        new_event->cmd = Read;
                        parent_event->when = sim_cycle + WAIT_TIME;
                        parent_event->operation = WAIT_MEM_READ;
                        scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                        dir_eventq_insert(new_event);
                        parent_event->startCycle = sim_cycle;
                        event_created = parent_event;
                        MSHRLookup(cache_dl2->mshr, parent_event->addr, WAIT_TIME, 0, NULL);
                        return 1;
                    }
                    else
                    {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                        event->isReqL2SecMiss = 1;
                        if(collect_stats)
                        {
                            cache_dl2->in_mshr ++;
                            cache_dl2->din_mshr ++;
                        }
                        event_list_insert(parent_event, matchnum);
                        parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                        return 1;
                    }
                }
                // fix me; if the owner write back and arrive later than this message; @ fanglei
                else
                {
                    tempID = parent_event->tempID;
                    /* After receiving this message, the directory information should be changed*/
                    parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                    parent_event->blk_dir->ptr_cur_event[block_offset] = NULL;

                    int vector_count;
                    int PVC_ml2_set;

                    if ( ml2_set_match_func(parent_event->blk_dir, ml2_mul_set) )
                        PVC_ml2_set = ml2_mul_set;
                    else if ( ml2_set_match_func(parent_event->blk_dir, ml2_sub_set) )
                        PVC_ml2_set = ml2_sub_set;
                    else
                        panic("must hit\n");
                    vector_count = vector_count_func(parent_event->blk_dir);
                    if(Is_Shared(parent_event->blk_dir->dir_sharer[block_offset]) == 1)
                    {
                        PVC_vector_inc_distr_record_num++;
                        PVC_vector_inc_distr[vector_count]++;
                        cache_ml2->sets[PVC_ml2_set].PVC_vector_inc_distr_record_num++;
                        cache_ml2->sets[PVC_ml2_set].PVC_vector_inc_distr[vector_count]++;
                        vector_count++;
                    }

                    PVC_vector_distr_record_num++;
                    PVC_vector_distr[vector_count]++;
                    cache_ml2->sets[PVC_ml2_set].PVC_vector_distr_record_num++;
                    cache_ml2->sets[PVC_ml2_set].PVC_vector_distr[vector_count]++;
                    
                    parent_event->blk_dir->dir_sharer[block_offset][tempID/64] |= (unsigned long long int)1<<(tempID%64);                    
                    parent_event->blk_dir->dir_blk_state[block_offset] = IsShared((parent_event->blk_dir)->dir_sharer[block_offset], tempID) ? MESI_SHARED : MESI_EXCLUSIVE;
                    parent_event->blk_dir->dir_dirty[block_offset] = -1;

                    if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                        dl2_blk->dir_state[block_offset] = DIR_STABLE;

                    /* send acknowledgement from directory to requester to changed state into shared */
                    parent_event->operation = IsShared((parent_event->blk_dir)->dir_sharer[block_offset], tempID) ? ACK_DIR_READ_SHARED : ACK_DIR_READ_EXCLUSIVE;
                    packet_size = data_packet_size;
                    parent_event->reqAckTime = sim_cycle;
                    parent_event->src1 = event->des1;
                    parent_event->src2 = event->des2;
                    parent_event->des1 = tempID/mesh_size+MEM_LOC_SHIFT;
                    parent_event->des2 = tempID%mesh_size;
                    parent_event->processingDone = 0;
                    parent_event->startCycle = sim_cycle;
                    scheduleThroughNetwork(parent_event, sim_cycle, packet_size, vc);
                    free_event(event);
                    dir_eventq_insert(parent_event);
                    return 1;
                }
            }
            break;

        case ACK_MSG_WRITE  :
        case ACK_MSG_WRITEUPDATE:
            dcache2_access++;
#ifdef CENTRALIZED_L2

#else
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache2_access++;
#endif
            if(md_text_addr(addr, tempID))
                panic("Writes cannot access text memory region");
            if(parent_event->blk_dir->dir_state[block_offset] == DIR_STABLE)
                panic("DIR: after the receipt of ACK msg, directory not in transient state");

            // updata L2 data cache
            parent_event->childCount --;
            if ( event->L1_hit && event->shr_data_req)
                event->parent->shr_data_req = 0;    // the data request is satisfied; @ fanglei
            if( findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk) && (event->operation == ACK_MSG_WRITE) )
                dl2_blk->status |= CACHE_BLK_DIRTY;
            if( !parent_event->sharer_num )
                parent_event->blk_dir->dir_sharer[block_offset][(((event->src1-MEM_LOC_SHIFT)*mesh_size+event->src2)/64)] &= ~ ((unsigned long long int)1 << (((event->src1-MEM_LOC_SHIFT)*mesh_size+event->src2)%64));


            if(parent_event->sharer_num)
                parent_event->individual_childCount[block_offset] --;

            if(parent_event->sharer_num && parent_event->individual_childCount[block_offset] == 0)
            {
                parent_event->sharer_num --;
                if(parent_event->sharer_num != 0)
                {
                    free_event(event);
                    return 1;
                }
                parent_event->sharer_num = 100; // indicate this is roll back; @ fanglei
            }

            if(parent_event->childCount == 0)
            {
                /* Receiving all the invalidation acknowledgement Messages */
                parent_event->blk_dir->all_shared_en = 0;
                
                tempID = parent_event->tempID;
                block_offset = blockoffset(parent_event->addr);

                dl2_tag = CACHE_TAG(dl2, parent_event->addr);
                dl2_set = CACHE_SET(dl2, parent_event->addr);

                ml2_mul_tag = CACHE_QBLK_TAG(ml2, parent_event->addr);
                ml2_mul_set = CACHE_QBLK_SET(ml2, parent_event->addr);
                ml2_sub_tag = CACHE_TAG(ml2, parent_event->addr);
                ml2_sub_set = CACHE_SET(ml2, parent_event->addr);

                // write trigger, not roll back; @ fanglei
                if(parent_event->sharer_num != 100)
                {
                    parent_event->blk_dir->status = CACHE_BLK_VALID;

                    if ( !parent_event->shr_data_req || findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk) )
                    {
                        if(findCacheStatus(dl2, dl2_set, dl2_tag, dl2_hindex, &dl2_blk))
                            dl2_blk->dir_state[block_offset] = DIR_STABLE;

                        parent_event->blk_dir->dir_blk_state[block_offset] = MESI_MODIFIED;
                        parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                        parent_event->blk_dir->ptr_cur_event[block_offset] = NULL;

                        parent_event->blk_dir->dir_dirty[block_offset] = tempID;
                        parent_event->blk_dir->dir_sharer[block_offset][tempID/64] = (unsigned long long int)1<<(tempID%64);
                        if(parent_event->operation == WRITE_UPDATE)
                        {
                            packet_size = meta_packet_size;
                            parent_event->operation = ACK_DIR_WRITEUPDATE;
                        }
                        else if(parent_event->operation == MISS_WRITE)
                        {
                            packet_size = data_packet_size;
                            parent_event->operation = ACK_DIR_WRITE;
                        }
                        parent_event->src1 = event->des1;
                        parent_event->src2 = event->des2;
                        parent_event->des1 = parent_event->tempID/mesh_size+MEM_LOC_SHIFT;
                        parent_event->des2 = parent_event->tempID%mesh_size;
                        parent_event->processingDone = 0;
                        parent_event->startCycle = sim_cycle;
                        dir_eventq_insert(parent_event);
                        scheduleThroughNetwork(parent_event, sim_cycle, meta_packet_size, vc);
                        free_event(event);
                        return 1;
                    }
                    // no copy returns && L2 data miss, we must refer to the memory; @ fanglei
                    else
                    {
                        free_event(event);

                        parent_event->blk_dir->dir_blk_state[block_offset] = MESI_INVALID;
                        parent_event->blk_dir->dir_state[block_offset] = DIR_TRANSITION;
                        if(parent_event->blk_dir->qblk_set_shift)
                        {
                            write_Q_hit_num--;
                            mb_pain_count--;
                        }
                        else
                            write_S_hit_num--;

                        parent_event->blk_dir->dir_dirty[block_offset] = -1;
                        parent_event->blk_dir->dir_sharer[block_offset][tempID/64] = 0;

//                        struct DIRECTORY_EVENT *parent_event = event->parent;
                        matchnum = MSHR_block_check(cache_dl2->mshr, parent_event->addr, cache_dl2->set_shift);
                        parent_event->L2miss_flag = 1;
                        parent_event->L2miss_stated = sim_cycle;
                        if(parent_event->isSyncAccess)
                            Sync_L2_miss ++;
                        if(!matchnum)
                        { /* No match in L2 MSHR */
                            if(!cache_dl2->mshr->freeEntries)
                            {/* We can not proceed with this even, since the directory is in transition. We wait till directory is in stable state. */
                                if(!last_L2_mshr_full)
                                {
                                    L2_mshr_full ++;
                                    last_L2_mshr_full = sim_cycle;
                                }
                                if(tshr_fifo_num[parent_event->des1*mesh_size+event->des2] >= tshr_fifo_size)
                                {
                                    dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] = dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] - 4;
                                    //dir_fifo_nack[event->des1*mesh_size+event->des2]
                                    parent_event->src1 += parent_event->des1;
                                    parent_event->des1 = parent_event->src1 - parent_event->des1;
                                    parent_event->src1 = parent_event->src1 - parent_event->des1;
                                    parent_event->src2 += parent_event->des2;
                                    parent_event->des2 = parent_event->src2 - parent_event->des2;
                                    parent_event->src2 = parent_event->src2 - parent_event->des2;
                                    parent_event->operation = NACK;
                                    parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                                    parent_event->processingDone = 0;
                                    parent_event->startCycle = sim_cycle;
                                    scheduleThroughNetwork(parent_event, sim_cycle, meta_packet_size, vc);
                                    dir_eventq_insert(parent_event);
                                    return 1;
                                }
                                else
                                {
                                    parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                                    dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] ++;
                                    parent_event->when = sim_cycle + RETRY_TIME;
                                    parent_event->flip_flag = 1;
                                    flip_counter ++;
                                    dir_eventq_insert(parent_event);
                                    return 1;
                                }
                            }
                            else
                            {
                                dir_fifo_full[parent_event->des1*mesh_size+parent_event->des2] = 0;
                                if(last_L2_mshr_full)
                                {
                                    Stall_L2_mshr += sim_cycle - last_L2_mshr_full;
                                    last_L2_mshr_full = 0;
                                }
                            }
                            if(collect_stats)
                            {
                                cache_dl2->misses ++;
                                cache_dl2->dmisses ++;
                                involve_2_hops_miss ++;
                            }
                            parent_event->reqAtDirTime = sim_cycle;
                            parent_event->mshr_time = sim_cycle;
                            struct DIRECTORY_EVENT *new_event;
                            new_event = allocate_new_event(parent_event);
                            new_event->operation = MEM_READ_REQ;
                            new_event->cmd = Read;
                            parent_event->when = sim_cycle + WAIT_TIME;
                            parent_event->operation = WAIT_MEM_READ;
                            scheduleThroughNetwork(new_event, sim_cycle, meta_packet_size, vc);
                            dir_eventq_insert(new_event);
                            parent_event->startCycle = sim_cycle;
                            event_created = parent_event;
                            MSHRLookup(cache_dl2->mshr, parent_event->addr, WAIT_TIME, 0, NULL);
                            return 1;
                        }
                        else
                        {  /* match entry in L2 MSHR, then insert this event into eventlist appending to mshr entry */
                            event->isReqL2SecMiss = 1;
                            if(collect_stats)
                            {
                                cache_dl2->in_mshr ++;
                                cache_dl2->din_mshr ++;
                            }
                            event_list_insert(parent_event, matchnum);
                            parent_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                            return 1;
                        }
                    }
                }
                // roll back trigger, not write; @ fanglei
                else
                {
                    if(parent_event->sharer_num != 100)
                        panic("WAIT_MEM_READ: sharer_num should be zero now!\n");
                    if(!parent_event->blk_dir->qblk_set_shift)
                        ml2_set = ml2_sub_set;
                    else
                        ml2_set = ml2_mul_set;
                    
                    int i=0, m=0, iteration = 1;
                    for(i=0; i<(cache_dl2->set_shift - cache_dl1[0]->set_shift); i++)
                        iteration = iteration * 2;
                    for(m=0; m<iteration; m++)
                    {
                        parent_event->blk_dir->dir_dirty[m] = -1;
                        parent_event->blk_dir->dir_state[m] = DIR_STABLE;
                        parent_event->blk_dir->ptr_cur_event[m] = NULL;
                    }
                    parent_event->blk_dir->ready = sim_cycle;

                    if( parent_event->operation == VEC_DEGRADE )
                    {
                        free_event(event);
                        free_event(parent_event);
                        return 1;
                    }
                    if( (parent_event->blk_dir->dir_blk_state[block_offset]==MESI_SHARED) && (Is_Shared(parent_event->blk_dir->dir_sharer[block_offset])<2) )
                        panic("state error\n");

                    if (ml2->hsize)
                        link_htab_ent(ml2, &ml2->sets[ml2_set], parent_event->blk_dir);
                    /* jing: Here is a problem: copy data out of cache block, if block exists */
                    /* do something here */

                    ml2->last_tagsetid.tag = parent_event->blk_dir->tagid.tag;
                    ml2->last_tagsetid.threadid = parent_event->blk_dir->tagid.threadid;
                    ml2->last_blk = (parent_event->blk_dir);

                    /* get user block data, if requested and it exists */
                    if (parent_event->udata)
                        *(parent_event->udata) = (parent_event->blk_dir)->user_data;

                    parent_event->operation = parent_event->parent_operation;
                    parent_event->sharer_num = 0;
                    parent_event->when = sim_cycle;
                    parent_event->no_L2_delay = 1;
                    dir_eventq_insert(parent_event);
                }
            }
            free_event(event);
            break;

        case ACK_DIR_IL1    :
            icache_access++;
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->icache_access++;
            /* First find the replacement line */
            tag = CACHE_TAG(event->cp, addr);
            set = CACHE_SET(event->cp, addr);

            switch ((event->cp)->policy)
            {
                int bindex;
                case LRU:
                case FIFO:
                repl = (event->cp)->sets[set].way_tail;
                update_way_list (&(event->cp)->sets[set], repl, Head);
                break;
                case Random:
                bindex = myrand () & ((event->cp)->assoc - 1);
                repl = CACHE_BINDEX ((event->cp), (event->cp)->sets[set].blks, bindex);
                break;
                default:
                panic ("bogus replacement policy");
            }
            if (collect_stats)
                event->cp->replacements++;
            repl->tagid.tag = tag;
            repl->tagid.threadid = tempID;
            repl->addr = event->addr;
            repl->status = CACHE_BLK_VALID;
            repl->state = MESI_SHARED;

            /* copy data out of cache block, if block exists */
            bofs = CACHE_BLK(event->cp, event->addr);
            if ((event->cp)->balloc && (event->vp))
                CACHE_BCOPY(event->cmd, repl, bofs, event->vp, event->nbytes);

            (event->cp)->last_tagsetid.tag = 0;
            (event->cp)->last_tagsetid.threadid = tempID;
            (event->cp)->last_blk = repl;

            /* get user block data, if requested and it exists */
            if (event->udata)
                *(event->udata) = repl->user_data;

            if(collect_stats)
                totalIL1misshandletime += sim_cycle - event->started;

            repl->ready = sim_cycle;

            if ((event->cp)->hsize)
                link_htab_ent((event->cp), &(event->cp)->sets[set], repl);

            thecontexts[tempID]->ruu_fetch_issue_delay = 0;
            thecontexts[tempID]->event = NULL;
            free_event(event);
            break;

        case ACK_DIR_WRITE  :
        case ACK_DIR_WRITEUPDATE:
        case ACK_DIR_READ_EXCLUSIVE:
        case ACK_DIR_READ_SHARED:
            dcache_access++;
            thecontexts[(event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2]->dcache_access++;

            /* First find the replacement line */
            tag = CACHE_TAG(event->cp, addr);
            set = CACHE_SET(event->cp, addr);
            if(event->operation == ACK_DIR_WRITEUPDATE)
                m_update_miss[event->tempID] = 0;

            switch ((event->cp)->policy)
            {
                int bindex, i=0;
                case LRU:
                case FIFO:
                if(!mystricmp (network_type, "FSOI") || !mystricmp(network_type, "HYBRID"))
                {
                repl = (event->cp)->sets[set].way_tail;
                update_way_list (&(event->cp)->sets[set], repl, Head);
                }
                else
                {
                    repl = (event->cp)->sets[set].way_tail;
                    update_way_list (&(event->cp)->sets[set], repl, Head);
                }
                break;
                case Random:
                bindex = myrand () & ((event->cp)->assoc - 1);
                repl = CACHE_BINDEX ((event->cp), (event->cp)->sets[set].blks, bindex);
                break;
                default:
                panic ("bogus replacement policy");
            }

            int tlb_state = -1;
            dtlb_access ( ((event->des1-MEM_LOC_SHIFT)*mesh_size+event->des2), repl->addr, &tlb_state );

            int inform_back_en;
            inform_back_en = total_inform_back_en && ((tlb_state == SHARED_WRITE) || (tlb_state == -1)) && ((multi_blk_adap_en && multi_blk_adap_inform_en) || !multi_blk_set_shift);

            if (   ( inform_back_en  || (repl->status & CACHE_BLK_DIRTY) )
                && (repl->status & CACHE_BLK_VALID) && (actualProcess == 1) && !md_text_addr(repl->addr, tempID) )
            {
                if ((repl->status & CACHE_BLK_DIRTY) && !(repl->state == MESI_MODIFIED))
                    panic ("Cache Block should have been modified here");

                struct DIRECTORY_EVENT *new_event = allocate_event(event->isSyncAccess);
                if(new_event == NULL)
                    panic("Out of Virtual Memory");
                if (repl->status & CACHE_BLK_DIRTY)
                {
                    new_event->operation = WRITE_BACK;
                    packet_size = data_packet_size;
                }
                else
                {
                    new_event->operation = INFORM_BACK;
                    packet_size = meta_packet_size;
                }
                new_event->conf_bit = -1;
                new_event->req_time = 0;
                new_event->input_buffer_full = 0;
                new_event->pending_flag = 0;
                new_event->op = event->op;
                new_event->src1 = event->des1;
                new_event->src2 = event->des2;
#ifdef  CENTRALIZED_L2
                md_addr_t src = (repl->addr >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
				new_event->des1 = mesh_size;
                new_event->des2 = src;
#else
                md_addr_t src = (repl->addr >> cache_dl2->set_shift) % numcontexts;
                new_event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                new_event->des2 = (src %mesh_size);
#endif
                new_event->processingDone = 0;
                new_event->startCycle = sim_cycle;
                new_event->parent = NULL;
                new_event->blk_dir = NULL;
                new_event->childCount = 0;
                new_event->tempID = event->tempID;
                new_event->L2miss_flag = event->L2miss_flag;
                new_event->resend = 0;
                new_event->blk1 = repl;
                new_event->addr = repl->addr;
                new_event->cp = event->cp;
                new_event->vp = NULL;
                new_event->nbytes = event->cp->bsize;
                new_event->udata = NULL;
                new_event->cmd = Write;
                new_event->rs = NULL;
                new_event->spec_mode = 0;
                new_event->data_reply = 0;
                /*go to network*/
                scheduleThroughNetwork(new_event, sim_cycle, packet_size, 0);
                dir_eventq_insert(new_event);
            }

            /* update block tags */
            repl->tagid.tag = tag;
            repl->tagid.threadid = tempID;
            repl->addr = event->addr;
            repl->status = CACHE_BLK_VALID;

            /* copy data out of cache block, if block exists */
            bofs = CACHE_BLK(event->cp, event->addr);
            if ((event->cp)->balloc && (event->vp))
                CACHE_BCOPY(event->cmd, repl, bofs, event->vp, event->nbytes);

            /* update cache block status at requester L1 */
            repl->WordUseFlag[(repl->addr>>2) & 7] = 1;
            if(event->operation == ACK_DIR_READ_EXCLUSIVE)
            {
                repl->ReadCount ++;
                repl->Type = PRIVATE;
                repl->state = MESI_EXCLUSIVE;
            }
            else if(event->operation == ACK_DIR_READ_SHARED)
            {
                repl->ReadCount ++;
                repl->Type = SHARED_READ_ONLY; //FIXME it can be share read write type data
                repl->state = MESI_SHARED;
            }
            else if(event->operation == ACK_DIR_WRITE || event->operation == ACK_DIR_WRITEUPDATE)
            {
                repl->WriteCount ++;
                if(event->operation == ACK_DIR_WRITEUPDATE)
                    repl->Type = SHARED_READ_WRITE;
                else if(event->operation == ACK_DIR_WRITE)
                    repl->Type = PRIVATE;  //FIXME: it can be shared read write type data
                repl->state = MESI_MODIFIED;
                repl->status |= CACHE_BLK_DIRTY;
            }
            event->cp->last_tagsetid.tag = 0;
            event->cp->last_tagsetid.threadid = tempID;
            event->cp->last_blk = repl;

            /* get user block data, if requested and it exists */
            if (event->udata)
                *(event->udata) = repl->user_data;

            int missHandleTime = sim_cycle - event->started;
            if(!event->isReqL2Hit)
            {
                if ( event->cmd == Read )
                {
                    read_L2_miss_num ++;
                    read_L2_miss_total_time += missHandleTime;
                }
                else if ( event->operation == ACK_DIR_WRITE )
                {
                    write_L2_miss_num ++;
                    write_L2_miss_total_time += missHandleTime;
                }
            }
            else
            {
                if ( event->cmd == Read )
                {
                    read_L1_miss_L2_hit_num ++;
                    read_L1_miss_L2_hit_total_time += missHandleTime;
                }
                else if ( event->operation == ACK_DIR_WRITE )
                {
                    write_L1_miss_L2_hit_num ++;
                    write_L1_miss_L2_hit_total_time += missHandleTime;
                }
            }

            if(collect_stats)
            {
                if(event->operation == ACK_DIR_WRITE && event->isReqL2Hit && !event->isReqL2SecMiss)
                {
                    totalWriteReq++;
                    totalWriteReqInv += ((event->reqAckTime == event->reqAtDirTime) ? 0 : 1);
                    totalWriteDelay += (sim_cycle - event->started);
                    totalWriteInvDelay += (event->reqAckTime - event->reqAtDirTime);
                }
                if(event->operation == ACK_DIR_WRITEUPDATE && event->isReqL2Hit && !event->isReqL2SecMiss)
                {
                    totalUpgradeReq++;
                    totalUpgradeReqInv += ((event->reqAckTime == event->reqAtDirTime) ? 0 : 1);
                    totalUpgradeDelay += (sim_cycle - event->started);
                    totalUpgradeInvDelay += (event->reqAckTime - event->reqAtDirTime);
                }
                else
                    totalUpgradeMiss ++;

                totalmisshandletime += sim_cycle - event->started;
                if(event->isSyncAccess)
                    totalmisstimeforSync += sim_cycle - event->started;
                else
                {
                    totalmisstimeforNormal += sim_cycle - event->started;
                    if(event->L2miss_flag && event->L2miss_complete && event->L2miss_stated)
                    {
                        totalL2misstime += event->L2miss_complete - event->L2miss_stated;
                        TotalL2misses ++;
                    }
                    else if(event->L2miss_flag)
                        totalWrongL2misstime ++;
                }
            }

            repl->ready = sim_cycle;

            if( (event->prefetch_next == 2 || event->prefetch_next == 4) && (event->operation == ACK_DIR_READ_SHARED || event->operation == ACK_DIR_READ_EXCLUSIVE) )
                repl->isL1prefetch = 1;
            else
                repl->isL1prefetch = 0;

            if ((event->cp)->hsize)
                link_htab_ent((event->cp), &(event->cp)->sets[set], repl);

            /* Update the rs finish time in RS_LINK (event_queue) and mshr entry */

            if(!event->spec_mode && event->rs)
            {
                if((event->operation == ACK_DIR_READ_SHARED) || (event->operation == ACK_DIR_READ_EXCLUSIVE))
                    eventq_update(event->rs, repl->ready);
                else
                    event->rs->writewait = 2;
            }
            mshrentry_update(event, repl->ready, event->started);
            free_event(event);
            break;

        case MEM_WRITE_BACK :
            lat = dl2->blk_access_fn(Write, CACHE_BADDR(dl2,event->addr), dl2->bsize, NULL, sim_cycle, event->tempID);
            free_event(event);
            break;

        case MEM_READ_REQ   :
            lat = dl2->blk_access_fn(Read, CACHE_BADDR(dl2,addr), dl2->bsize, NULL, sim_cycle, tempID);
            event->when = sim_cycle + lat;
            event->startCycle = sim_cycle;
            event->operation = MEM_READ_REPLY;
            total_mem_lat += lat;
            total_mem_access ++;
            dir_eventq_insert(event);
            break;

        case MEM_READ_REPLY :
            {
                event->mshr_time = event->parent->mshr_time;
                event->operation = WAIT_MEM_READ;
                event->src1 += event->des1;
                event->des1 = event->src1 - event->des1;
                event->src1 = event->src1 - event->des1;
                event->src2 += event->des2;
                event->des2 = event->src2 - event->des2;
                event->src2 = event->src2 - event->des2;
                event->operation = WAIT_MEM_READ;
                event->startCycle = sim_cycle;
                event->processingDone = 0;
                scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
                dir_eventq_insert(event);
                int i;
                for(i=1;i<(cache_dl2->bsize/max_packet_size);i++)
                {
                    struct DIRECTORY_EVENT *new_event;
                    new_event = allocate_new_event(event);
                    new_event->mshr_time = event->parent->mshr_time;
                    new_event->src1 = event->src1;
                    new_event->src2 = event->src2;
                    new_event->des1 = event->des1;
                    new_event->des2 = event->des2;
                    new_event->processingDone = 0;
                    new_event->operation = WAIT_MEM_READ_N_WAIT;
                    new_event->req_time = event->req_time;
                    new_event->when = sim_cycle + ((cache_dl1[0]->bsize*mem_port_num/mem_bus_width)*((float)4/mem_bus_speed))*i;
                    dir_eventq_insert(new_event);
                }
            }
            break;

        case WAIT_MEM_READ_N_WAIT:
            event->operation = WAIT_MEM_READ_N;
            event->startCycle = sim_cycle;
            event->processingDone = 0;
            scheduleThroughNetwork(event, sim_cycle, data_packet_size, vc);
            dir_eventq_insert(event);
            break;

        case WAIT_MEM_READ  :
        case WAIT_MEM_READ_N    :
            {
                if ( event->operation == WAIT_MEM_READ )
                {
                    struct DIRECTORY_EVENT *p_event = event->parent;
                    dcache2_access++;
#ifdef CENTRALIZED_L2

#else
                    thecontexts[(p_event->des1-MEM_LOC_SHIFT)*mesh_size+p_event->des2]->dcache2_access++;
#endif
                    p_event->L2miss_complete = sim_cycle;
                    switch (dl2->policy)
                    {
                        int i;
                        int bindex;
                        case LRU:
                        case FIFO:
                        i = dl2->assoc;
                        do {
                            if(i == 0)  break;
                            i--;
                            repl = dl2->sets[dl2_set].way_tail;
                            update_way_list (&dl2->sets[dl2_set], repl, Head);
                        }
                        while(repl_dir_state_check(repl->dir_state));    // The "do while" searches for the least recently used block that is not in "DIR_TRANSITION" state to be replaced.  @ fanglei

                        if(repl_dir_state_check(repl->dir_state))
                        {       // If every block in this set is in "DIR_TRANSITION" state, it means the refill can not be done in this cycle. The p_event has to wait a "RETRY_TIME" time, and then request for the data again. @ fanglei
                            flip_counter ++;
                            event->flip_flag = 1;
                            event->when = sim_cycle + RETRY_TIME;
                            dir_eventq_insert(event);
                            return 1;
                        }
                        break;
                        case Random:
                        bindex = myrand () & (dl2->assoc - 1);
                        repl = CACHE_BINDEX (dl2, dl2->sets[dl2_set].blks, bindex);
                        break;
                        default:
                        panic ("bogus replacement policy");
                    }
                    // write back dirty L2 data block; @ fanglei
                    if ( (repl->status & CACHE_BLK_VALID) && (repl->status & CACHE_BLK_DIRTY) )
                    {
                        struct DIRECTORY_EVENT *new_event;
                        new_event = allocate_new_event(event);
                        new_event->addr = repl->addr;
                        new_event->operation = MEM_WRITE_BACK;
                        new_event->cmd = Write;
                        scheduleThroughNetwork(new_event, sim_cycle, data_packet_size, 0);
                        dir_eventq_insert(new_event);
                    }
                    repl->status = CACHE_BLK_VALID;
                    repl->tagid.tag = dl2_tag;
                    repl->tagid.threadid = tempID;
                    repl->addr = addr;
                    int m=0, iteration = 1, flag = 0;
                    for(m=0; m<(cache_dl2->set_shift - cache_dl1[0]->set_shift); m++)
                        iteration = iteration * 2;
                    for(m=0; m<iteration; m++)
                        repl->dir_state[m] = DIR_STABLE;

                    repl->dir_state[block_offset] = DIR_STABLE;

                    if (repl->way_prev && dl2->policy == LRU)
                        update_way_list(&dl2->sets[dl2_set], repl, Head);

                    dl2->last_tagsetid.tag = dl2_tag;
                    dl2->last_tagsetid.threadid = tempID;
                    dl2->last_blk = repl;

                    repl->exclusive_time[block_offset] = sim_cycle;

                    /* get user block data, if requested and it exists */
                    if (p_event->udata)
                        *(p_event->udata) = repl->user_data;

                    repl->ready = sim_cycle;

                    if (dl2->hsize)
                        link_htab_ent(dl2, &dl2->sets[dl2_set], repl);

                    /* remove the miss entry in L2 MSHR*/

                    if ( (p_event->tlb_state == SHARED_WRITE) && (!p_event->blk_dir) )
                        panic("it should lock a meta entry\n");

                    if ( p_event->blk_dir )
                    {
                        p_event->blk_dir->dir_state[block_offset] = DIR_STABLE;
                        p_event->blk_dir->ptr_cur_event[block_offset] = NULL;
                    }

                    p_event->operation = p_event->parent_operation;
                    p_event->when = sim_cycle;
                    p_event->no_L2_delay = 1;
                    dir_eventq_insert(p_event);
                }
                update_L2_mshrentry(event->addr, sim_cycle, event->mshr_time);
                free_event(event);
                break;
            }
        default         :
            panic("DIR: undefined directory operation");
    }
    return 1;
}

int repl_dir_state_check(int *dir_state)
{
    int m=0, iteration = 1, flag = 0;
    for(m=0; m<(cache_dl2->set_shift - cache_dl1[0]->set_shift); m++)
        iteration = iteration * 2;
    for(m=0; m<iteration; m++)
        if(dir_state[m] == DIR_TRANSITION)
            flag = 1;
    return flag;
}

/********************************************multiprocessor************************************************/
/* access a cache, perform a CMD operation on cache CP at address ADDR,
   places NBYTES of data at *P, returns latency of operation if initiated
   at NOW, places pointer to block user data in *UDATA, *P is untouched if
   cache blocks are not allocated (!CP->BALLOC), UDATA should be NULL if no
   user data is attached to blocks */

extern totalUpgrades;
unsigned int            /* latency of access in cycles */
cache_access (struct cache_t *cp,   /* cache to access */
        enum mem_cmd cmd,   /* access type, Read or Write */
        md_addr_t addr, /* address of access */
        void *vp,       /* ptr to buffer for input/output */
        int nbytes, /* number of bytes to access */
        tick_t now, /* time of access */
        byte_t ** udata,    /* for return of user data ptr */
        md_addr_t * repl_addr,  /* for address of replaced block */
        struct RUU_station *rs, /* LSQ entry -hq */
        int threadid
        , m_L1WBufEntry *l1_wb_entry //8 equals thread id..
)
{
    sprintf(dl1name, "dl1");

    if (!strcmp (cp->name, dl1name) && cp->threadid != threadid)
        panic ("Wrond DL1 cache being called in cache access\n");

    if(!strcmp (cp->name, dl1name) && md_text_addr(addr, threadid))
        return cache_dl1_lat;
//    if(addr == 5368767520)
//        printf("access here\n");
    byte_t *p = vp;
    md_addr_t tag = CACHE_TAG (cp, addr);
    md_addr_t set = CACHE_SET (cp, addr);
    md_addr_t bofs = CACHE_BLK (cp, addr);
    struct cache_blk_t *blk, *repl;
    int lat = 0, cnt;
    int vc = -1, a = 0;
    int isSyncAccess = 0;
    struct RUU_station  *my_rs = rs;


    md_addr_t wb_addr, replAddress;
    struct DIRECTORY_EVENT *event;

    int port_lat = 0, now_lat = 0, port_now = 0;
#if PROCESS_MODEL
    int isSharedAddress = 0;
#endif

#if PROCESS_MODEL
    /* Check is the data being read/written is in shared space or not */
    if (!strcmp (cp->name, dl1name))
    {
        isSharedAddress = checkForSharedAddr ((unsigned long long) addr);
    }
#endif
    /* Restart the thread that is in a quiescent state */
    /* This is true only for Data Cache */
    if (!strcmp (cp->name, dl1name))
    {
        if (cmd == Write)
        {
            for (cnt = 0; cnt < numcontexts; cnt++)
            {
                if ((quiesceAddrStruct[cnt].address == addr))
                {
                    thecontexts[cnt]->freeze = 0;
                    thecontexts[cnt]->running = 1;
                    quiesceAddrStruct[cnt].address = 1;
                    quiesceAddrStruct[cnt].threadid = -1;
                }
            }
        }
#ifdef  L1_STREAM_PREFETCHER
    /* Forward the request to stream prefetcher. do not prefetch if it's a sync instruction */
        if ( STREAM_min_En )
        {
            md_addr_t stream_head_addr;
            if(rs)
            {
                if(!rs->isSyncInst)
                {
                    stream_head_addr = stream_match_addr(addr, threadid);
                    l1launch_sp (stream_head_addr, threadid, 1);
                }

            }
            else
            {
                stream_head_addr = stream_match_addr(addr, threadid);
                l1launch_sp (stream_head_addr, threadid, 1);
            }
        }
#endif
    }

    /* default replacement address */
    if (repl_addr)
        *repl_addr = 0;
    /* check alignments */
    if ((nbytes & (nbytes - 1)) != 0 || (addr & (nbytes - 1)) != 0)
    {
        printf("nbytes is %d\n", nbytes);
        fatal ("cache: access error: bad size or alignment, addr 0x%08x", addr);
    }
    /* access must fit in cache block */
    if ((addr + nbytes - 1) > ((addr & ~cp->blk_mask) + cp->bsize - 1))
        panic ("cache: access error: access spans block, addr 0x%08x", addr);

    if (strcmp (cp->name, dl1name)) //not level-1 data cache
    {
        if (cp->hsize)
        {
            /* higly-associativity cache, access through the per-set hash tables */
            int hindex = (int) CACHE_HASH (cp, tag);

            for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
            {
                if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
                    goto cache_hit;
            }
        }
        else
        {
            /* low-associativity cache, linear search the way list */
            for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
            {
                if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
                    goto cache_hit;
            }
        }
    }
    else //level-1 data cache access
    {
        if(rs)
        {
            if(rs->isSyncInst)
            {
                SyncInstCacheAccess ++;
                if(my_rs->in_LSQ != 1)
                    my_rs = &thecontexts[threadid]->LSQ[thecontexts[threadid]->LSQ_head];
                if(!my_rs)
                    panic("lsq does not have any data\n");
                if(my_rs->op == LDL)
                    TestCacheAccess ++;
                else if(my_rs->op == LDL_L)
                    TestSecCacheAccess ++;
                else if(my_rs->op == STL_C)
                    SetCacheAccess ++;
            }
        }
        else if(l1_wb_entry)
            if(l1_wb_entry->op == STL_C)
                SetCacheAccess ++;
        level1_data_cache_access_num++;
        switch (cmd)
        {
            case Read:
                level1_data_cache_access_read_num++;
                for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
                {
                    if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID) && ((blk->state == MESI_SHARED) || (blk->state == MESI_MODIFIED) || (blk->state == MESI_EXCLUSIVE)))
                    {
                        level1_data_cache_access_read_hit_num++;
                            if ( blk->isL1prefetch )
                            {
                                level1_data_cache_access_read_hit_FPP_num++;
                                blk->isL1prefetch = 0;
                            }
//                        }
                        blk->addr = addr;
                        /* These statistics are for the coherent cache */
                        blk->ReadCount ++;
                        blk->WordUseFlag[(blk->addr>>2) & 7] = 1;

                        if (collect_stats)
                        {
                            switch (blk->state)
                            {
                                case MESI_SHARED:
                                    cp->ccLocalLoadS++;
                                    break;
                                case MESI_EXCLUSIVE:
                                    cp->ccLocalLoadX++;
                                    break;
                                case MESI_MODIFIED:
                                    cp->ccLocalLoadM++;
                                    break;
                                default:
                                    break;
                            }
                        }
                        goto cache_hit;
                    }
                }
                break;
            case Write:
                level1_data_cache_access_write_num++;
                for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
                {
                    if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
                    {
                        blk->addr = addr;
                        switch (blk->state)
                        {
                            case MESI_MODIFIED:
                                level1_data_cache_access_write_local_num++;
                                    if ( blk->isL1prefetch )
                                    {
                                        panic("blk can not be dirty and prefetched");
                                    }
                                if (collect_stats)
                                    cp->ccLocalStoreM++;

                                blk->WriteCount ++;
                                blk->WordUseFlag[(blk->addr>>2) & 7] = 1;
                                goto cache_hit;
                                break;

                            case MESI_SHARED:   // write to MESI_SHARED block @ fanglei
                                level1_data_cache_access_write_update_num++;
                                    if ( blk->isL1prefetch )
                                    {
                                        level1_data_cache_access_write_update_FPP_num++;
                                        blk->isL1prefetch = 0;
                                    }
                                if (collect_stats)
                                    cp->ccLocalStoreS++;
                                if (spec_benchmarks)
                                    panic ("The state can never be shared \n");

                                blk->status = 0; /* We are not modeling data here. For proper data modeling we need to keep this block's data saved in MSHR. */
                                if(rs)
                                    isSyncAccess = rs->isSyncInst;
                                else if(l1_wb_entry)
                                    if(l1_wb_entry->op == STL_C)
                                        isSyncAccess = 1;
                                event = (struct DIRECTORY_EVENT *)      // write to MESI_SHARED block @ fanglei
                                    allocate_event(isSyncAccess); //create a space
                                if (!event)
                                    fatal("out of virtual memory in calloc dir event");

                                long src;
                                if(my_rs)
                                {
                                    event->op = my_rs->op;
                                    event->isPrefetch = !my_rs->issued;
                                    event->isSyncAccess = isSyncAccess;
                                    event->spec_mode = my_rs->spec_mode;
                                    if(my_rs->isSyncInst)
                                    {
                                        if(my_rs->op == STL_C || my_rs->op == STQ_C)
                                        {
                                            event->store_cond = MY_STL_C;
                                            SyncStoreCWriteUpgrade ++;
                                        }
                                        if(my_rs->op == STL)
                                        {
                                            SyncStoreWriteUpgrade ++;
                                            event->store_cond = MY_STL;
                                        }
                                    }
                                }
                                else if(l1_wb_entry)
                                {
                                    if(l1_wb_entry->op == STL_C)
                                    {
                                        event->store_cond = MY_STL_C;
                                        SyncStoreCWriteUpgrade ++;
                                    }
                                }
                                else
                                {
                                    event->op = 0;
                                    event->isPrefetch = 0;
                                    event->isSyncAccess = 0;
                                    event->spec_mode = 0;
                                }
                                event->src1 = thecontexts[threadid]->actualid/mesh_size+MEM_LOC_SHIFT;
                                event->src2 = thecontexts[threadid]->actualid%mesh_size;
#ifdef CENTRALIZED_L2
                                //the centralized L2 has CENTL2_BANK_NUM banks, and each of them has one NI (network interface).
                                src = (addr >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
                                event->des1 = mesh_size ;    //This indicates that its a L2 bank access, the location of this L2 bank NI is on the bottom of the chip, (In fact, it can be anywhere)
                                event->des2 = src; //this indicate the bank id
#else
                                //src = (addr >> cache_dl2->set_shift) % numcontexts;
                                src = (addr >> cache_dl2->set_shift) % numcontexts;
                                event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                                event->des2 = (src %mesh_size);
#endif
                                event->started = now;
                                event->startCycle = now;
                                event->pending_flag = 0;
                                event->data_reply = 0;
                                event->req_time = 0;

                                missNo ++;
                                event->resend = 0;
                                event->childCount = 0;
                                event->parent = NULL;
                                event->operation = WRITE_UPDATE;

                                int tlb_state = -1;
                                dtlb_access ( threadid, addr, &tlb_state );
                                event->tlb_state = tlb_state;       // dd-cache
                                if( tlb_state != SHARED_WRITE )
                                    printf("WRITE_UPDATE request but not SHARED_WRITE");
                                SW_write_request_num++;

                                if(collect_stats)
                                {
                                    cp->invalidations++;
                                    if( (((event->addr)>>cache_dl2->set_shift)%numcontexts == thecontexts[threadid]->actualid) )
                                        local_cache_access++;
                                    else
                                        remote_cache_access++;
                                }

                                m_update_miss[threadid] = 1;

                                if(collect_stats)
                                    totaleventcountnum ++;
                                if(collect_stats && event->isSyncAccess)
                                {
                                    totalSyncWriteup ++;
                                    totalSyncEvent ++;
                                }
                                else
                                    totalNormalEvent ++;
                                totalUpgrades++;
                                event->tempID = threadid;
                                event->blk1 = NULL;
                                event->addr = addr;
                                event->cp = cp;
                                event->vp = p;
                                event->nbytes = nbytes;
                                event->udata = udata;
                                event->cmd = cmd;
                                event->rs = rs;
                                event->l1_wb_entry = l1_wb_entry;
                                event->missNo = missNo;
                                event->l2Status = 0;
                                event->l2MissStart = 0;
                                event->L2miss_flag = 0;
                                event->sharer_num = 0;
                                event->dirty_flag = 0;
                                event->arrival_time = 0;
                                event->conf_bit = -1;
                                event->input_buffer_full = 0;

                                scheduleThroughNetwork(event, now, meta_packet_size, vc);
                                dir_eventq_insert(event);//insert the directory event of write hit

                                event_created = event;
                                if(blk->isL1prefetch)
                                    L1_prefetch_writeafter ++;

                                lat = lat + WAIT_TIME;
                                return lat;
                                break;

                            case MESI_EXCLUSIVE:
                                level1_data_cache_access_write_local_num++;
                                    if ( blk->isL1prefetch )
                                    {
                                        level1_data_cache_access_write_local_FPP_num++;
                                        blk->isL1prefetch = 0;
                                    }
//                                }
                                if (collect_stats)
                                    cp->e_to_m++;
                                e_to_m ++;
                                totalL1LineExlUsed++;
                                blk->state = MESI_MODIFIED;
                                blk->WriteCount ++;
                                blk->WordUseFlag[(blk->addr>>2) & 7] = 1;
                                goto cache_hit;
                                break;
                            default:
                                panic ("Should not be invalid");
                                break;
                        }
                    }
                }
                break;
            default:
                panic ("Cache to the command is neither read nor write");
                break;
        }
    }

    int flag = 0, next_block = 0, pre_block = 0;
    if(!strcmp(cp->name, dl1name))// dl1 cache miss
    {/* This is a L1 miss, we are going to create an event for L2 director access. */
     //miss because the state is invalid
        if( cmd == Read )
        {
            level1_data_cache_access_read_miss_num++;
        }
        else
        {
            level1_data_cache_access_write_miss_num++;
        }
        if(collect_stats)
        {
            for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
            {
                if ((blk->tagid.tag == tag) && (blk->status == 0)&& (blk->state == MESI_INVALID))
                {
                    cp->coherence_misses ++;
                    flag = 1;
                }
            }
            if(flag != 1)
                cp->capacitance_misses ++;
        }

        if(rs)
            isSyncAccess = rs->isSyncInst;
        else if(l1_wb_entry)
            if(l1_wb_entry->op == STL_C)
            isSyncAccess = 1;
        event = (struct DIRECTORY_EVENT *)
            allocate_event(isSyncAccess); //create a space

        if (!event)
            fatal("out of virtual memory in calloc dir event");
        if (cmd == Read)
        {
            event->operation = MISS_READ;
            totalmisscountnum ++;
            event->isPrefetch = rs->isPrefetch;
        }
        else
        {
            event->operation = MISS_WRITE;
            totalreqcountnum ++;
            if(rs)
                event->isPrefetch = !rs->issued;
            else
                event->isPrefetch = 0;
        }
        int tlb_state = -1;
        dtlb_access ( threadid, addr, &tlb_state );
        event->tlb_state = tlb_state;       // dd-cache
        if( tlb_state == SHARED_WRITE )
        {
            if(cmd == Read)
                SW_read_request_num++;
            else
                SW_write_request_num++;
        }
        if(collect_stats)
            totaleventcountnum ++;
        if(collect_stats && isSyncAccess)
        {
            totalSyncEvent ++;
            if(event->operation == MISS_WRITE)
                totalSyncWriteM ++;
            else
                totalSyncReadM ++;
        }
        else
            totalNormalEvent ++;
        missNo ++;
        long src;
        event->pending_flag = 0;
        event->op = LDQ;
        event->src1 = thecontexts[threadid]->actualid/mesh_size+MEM_LOC_SHIFT;
        event->src2 = thecontexts[threadid]->actualid%mesh_size;
#ifdef CENTRALIZED_L2
        //the centralized L2 has CENTL2_BANK_NUM banks, and each of them has one NI (network interface).
        src = (addr >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
        event->des1 = mesh_size ;    //This indicates that its a L2 bank access, the location of this L2 bank NI is on the bottom of the chip, (In fact, it can be anywhere)
        event->des2 = src; //this indicate the bank id
#else
        //src = (addr >> cache_dl2->set_shift) % numcontexts;
        src = (addr >> cache_dl2->set_shift) % numcontexts;
        event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
        event->des2 = (src %mesh_size);
#endif
        event->started = now;
        event->startCycle = now;
        if(rs)
        {
            event->isSyncAccess = isSyncAccess;
            event->spec_mode = rs->spec_mode;
        }
        else if(l1_wb_entry)
        {
            if(l1_wb_entry->op == STL_C)
            {
                event->isSyncAccess = isSyncAccess;
                event->spec_mode = 0;
            }
        }
        else
        {
            event->isSyncAccess = 0;
            event->spec_mode = 0;
        }

        if(collect_stats)
        {
            cp->misses ++;
            if((addr>>cache_dl2->set_shift)%numcontexts == thecontexts[threadid]->actualid)
                local_cache_access++;
            else
                remote_cache_access++;
        }

        event->childCount = 0;
        event->parent = NULL;
        event->l2Status = 0;
        event->l2MissStart = 0;
        event->tempID = threadid;
        event->resend = 0;
        event->blk1 = NULL;
        event->addr = addr;
        event->cp = cp;
        event->vp = p;
        event->nbytes = nbytes;
        event->udata = udata;
        event->cmd = cmd;
        event->rs = rs;
        event->l1_wb_entry = l1_wb_entry;
        event->missNo = missNo;
        event->L2miss_flag = 0;
        event->sharer_num = 0;
        event->prefetch_next = 0;
        event->dirty_flag = 0;
        event->arrival_time = 0;
        event->conf_bit = -1;
        event->data_reply = 0;
        event->req_time = 0;
        if(my_rs)
        {
            if(my_rs->isSyncInst && my_rs->op == LDL)
            {
                SyncLoadReadMiss ++;
                event->store_cond = MY_LDL;
            }
            if(my_rs->op == LDL_L)
            {
                SyncLoadLReadMiss ++;
                event->store_cond = MY_LDL_L;
            }
            if(my_rs->op == STL_C)
            {
                SyncStoreCWriteMiss ++;
                event->store_cond = MY_STL_C;
            }
            if(my_rs->isSyncInst && my_rs->op == STL)
            {
                SyncStoreWriteMiss ++;
                event->store_cond = MY_STL;
            }
        }

        else if(l1_wb_entry)
        {
            if(l1_wb_entry->op == STL_C)
            {
                SyncStoreCWriteMiss ++;
                event->store_cond = MY_STL_C;
            }
        }



#ifdef  L1_STREAM_PREFETCHER
        /* L1 cache miss is detected, insert this address into stream pre-fetcher. */
        if (!dl1_prefetch_active)
        {
            if(rs)
            {
                if(!rs->isSyncInst)
                    l1insert_sp (addr, threadid);
            }
            else
                l1insert_sp (addr, threadid);
        }
#endif

        event_created = event;
        event->input_buffer_full = 0;
        scheduleThroughNetwork(event, now, meta_packet_size, vc);
		//printf("dl1 access miss from %d %d to %d %d at address %x\n",event->src1,event->src2,event->des1,event->des2,event->addr);
        dir_eventq_insert(event);
        return WAIT_TIME;
    }
    else if(!strcmp (cp->name, "il1"))//il1 cache miss
    {
        event = (struct DIRECTORY_EVENT *)
            allocate_event(0); //create a space

        if (!event)
            fatal("out of virtual memory in calloc dir event");
        if(cmd == Write)
            panic("Il1 access could-not lead to Write command");

        long src;
        event->op = LDQ;
        event->pending_flag = 0;
        event->operation = MISS_IL1;
        event->isPrefetch = 0;
        event->src1 = thecontexts[threadid]->actualid/mesh_size+MEM_LOC_SHIFT;
        event->src2 = thecontexts[threadid]->actualid%mesh_size;
#ifdef CENTRALIZED_L2
        //the centralized L2 has CENTL2_BANK_NUM banks, and each of them has one NI (network interface).
        src = (addr >> cache_dl2->set_shift) % CENTL2_BANK_NUM;
        event->des1 = mesh_size ;    //This indicates that its a L2 bank access, the location of this L2 bank NI is on the bottom of the chip, (In fact, it can be anywhere)
        event->des2 = src; //this indicate the bank id
#else
        //src = (addr >> cache_dl2->set_shift) % numcontexts;
        src = (addr >> cache_dl2->set_shift) % numcontexts;
        event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
        event->des2 = (src %mesh_size);
#endif
        event->started = now;
        event->startCycle = now;
        event->data_reply = 0;
        event->req_time = 0;

        if(collect_stats)
        {
            cp->misses ++;
            if((addr>>cache_dl2->set_shift)%numcontexts == thecontexts[threadid]->actualid)
                local_cache_access++;
            else
                remote_cache_access++;
        }

        event->childCount = 0;
        event->parent = NULL;
        event->l2Status = 0;
        event->l2MissStart = 0;
        event->tempID = threadid;
        event->resend = 0;
        event->blk1 = NULL;
        event->addr = addr;
        event->cp = cp;
        event->vp = p;
        event->nbytes = nbytes;
        event->udata = udata;
        event->cmd = cmd;
        event->rs = NULL;
        event->l1_wb_entry = l1_wb_entry;
        missNo ++;
        event->missNo = missNo;
        event->L2miss_flag = 0;
        event->sharer_num = 0;
        event->dirty_flag = 0;
        event->arrival_time = 0;
        event->conf_bit = -1;
        event->input_buffer_full = 0;

        scheduleThroughNetwork(event, now, meta_packet_size, vc);
		//printf("il1 access miss from %d %d to %d %d at address %x\n",event->src1,event->src2,event->des1,event->des2,event->addr);
        dir_eventq_insert(event);
        event_created = event;
        thecontexts[threadid]->event = event;
        return WAIT_TIME;
    }
    else if (!strcmp (cp->name, "dtlb"))
    {
        // tlb miss handler @ fanglei
        event = (struct DIRECTORY_EVENT *)
            allocate_event(0); //create a space
        if (!event)
            fatal("out of virtual memory in calloc dir event");
        long src;
        event->op = LDQ;
        event->pending_flag = 0;
        event->operation = SHR_TLB_MISS;
        event->isPrefetch = 0;
        event->src1 = thecontexts[threadid]->actualid/mesh_size+MEM_LOC_SHIFT;
        event->src2 = thecontexts[threadid]->actualid%mesh_size;
        src = 0;    // assume the node 0 runs the system @ fanglei
        event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
        event->des2 = (src %mesh_size);
        event->started = now;
        event->startCycle = now;
        event->data_reply = 0;
        event->req_time = 0;
        event->childCount = 0;
        event->parent = NULL;
        event->l2Status = 0;
        event->l2MissStart = 0;
        event->tempID = threadid;
        event->resend = 0;
        event->blk1 = NULL;
        event->addr = addr;
        event->cp = cp;
        event->vp = p;
        event->nbytes = nbytes;
        event->udata = udata;
        event->cmd = cmd;
        event->rs = NULL;
        missNo ++;
        event->missNo = missNo;
        event->spec_mode = thecontexts[threadid]->spec_mode;
        event->L2miss_flag = 0;
        event->sharer_num = 0;
        event->dirty_flag = 0;
        event->arrival_time = 0;
        event->conf_bit = -1;
        event->input_buffer_full = 0;

        scheduleThroughNetwork(event, now, meta_packet_size, vc);

        dir_eventq_insert(event);
        event_created = event;
        thecontexts[threadid]->event = event;
        return WAIT_TIME;
    }


    /* select the appropriate block to replace, and re-link this entry to
       the appropriate place in the way list */
    switch (cp->policy)
    {
        case LRU:
        case FIFO:
            repl = cp->sets[set].way_tail;
            update_way_list (&cp->sets[set], repl, Head);
            break;
        case Random:
            {
                int bindex = myrand () & (cp->assoc - 1);

                repl = CACHE_BINDEX (cp, cp->sets[set].blks, bindex);
                break;
            }
        default:
            panic ("bogus replacement policy");
    }


    if (strcmp (cp->name, dl1name)) //not level-1 data cache
    {
        /* **MISS** */
        if (collect_stats)
            cp->misses++;

        /* remove this block from the hash bucket chain, if hash exists */
        if (cp->hsize)
            unlink_htab_ent (cp, &cp->sets[set], repl);

        /* blow away the last block to hit */
        cp->last_tagsetid.tag = 0;
        cp->last_tagsetid.threadid = -1;
        cp->last_blk = NULL;

        lat += cp->hit_latency;

        /* write back replaced block data */
        if (repl->status & CACHE_BLK_VALID)
        {
            if (collect_stats)
                cp->replacements++;

            if (repl_addr)
                *repl_addr = CACHE_MK_BADDR (cp, repl->tagid.tag, set);

            /* don't replace the block until outstanding misses are satisfied */
            lat += BOUND_POS (repl->ready - now);

            if ((repl->status & CACHE_BLK_DIRTY))
            {
                /* write back the cache block */
                if (collect_stats)
                {
                    cp->writebacks++;
                    cp->wrb++;
                }
                lat += cp->blk_access_fn (Write, CACHE_MK_BADDR (cp, repl->tagid.tag, set), cp->bsize, repl, now + lat, repl->tagid.threadid);
            }
        }

        /* update block tags */
        repl->tagid.tag = tag;
        repl->tagid.threadid = threadid;
        repl->status = CACHE_BLK_VALID; /* dirty bit set on update */
        repl->addr = addr;
        repl->lineVolatile = 0;
        repl->wordVolatile = 0;
        repl->wordInvalid = 0;
        repl->isStale = 0;

        /* read data block */
        // lat += cp->blk_access_fn (Read, CACHE_BADDR (cp, addr), cp->bsize, repl, now + lat, threadid);
        lat += cp->blk_access_fn (cmd, tag, cp->bsize, repl, now + lat, threadid);

        /* copy data out of cache block */
        if (cp->balloc && p)
        {
            CACHE_BCOPY (cmd, repl, bofs, p, nbytes);
        }

        /* update dirty status */
        if (cmd == Write)
        {
            repl->status |= CACHE_BLK_DIRTY;
        }

        if (!strcmp (cp->name, "dl2"))
        {
            if (cmd == Write)
            {
                wm2++;
            }
            else
            {
                rm2++;
            }
        }

        /* get user block data, if requested and it exists */
        if (udata)
            *udata = repl->user_data;

        /* update block status */
        repl->ready = now + lat;
        /* link this entry back into the hash table */
        if (cp->hsize)
            link_htab_ent (cp, &cp->sets[set], repl);

        /* return latency of the operation */
        return lat;
    }
    else            /*DL1 cache */
    {
        panic(" DL1 access could not ENTER this part of the code ");
    }               /* MSI and DL1 cache */

cache_hit:          /* slow hit handler */

    if(!strcmp (cp->name, "dtlb"))
    {
        if ( cmd == Read )
            return cp->hit_latency;
        else
        {
            if(blk->state == PRIVATE_READ)
            {
                blk->state = PRIVATE_WRITE;
                return cp->hit_latency;
            }
            else if (blk->state == SHARED_READ)
            {
                // inform the tlb_cu @ fanglei
                tlb_cu_allocate(threadid, SHR_TLB_SR_RECOVERY, tag, NULL);
                struct DIRECTORY_EVENT *new_event = (struct DIRECTORY_EVENT *) allocate_event(0); //create a space
                if (!new_event)
                    fatal("out of virtual memory in calloc dir event");
                long src;
                new_event->operation = SHR_TLB_SR_RECOVERY;
                new_event->src1 = threadid/mesh_size+MEM_LOC_SHIFT;
                new_event->src2 = threadid%mesh_size;
                src = 0;    // assume the node 0 runs the system @ fanglei
                new_event->des1 = (src /mesh_size)+MEM_LOC_SHIFT;
                new_event->des2 = (src %mesh_size);
                new_event->started = now;
                new_event->startCycle = now;
                new_event->tempID = threadid;
                new_event->addr = addr;

                scheduleThroughNetwork(new_event, now, meta_packet_size, vc);
                dir_eventq_insert(new_event);
                return (cp->hit_latency+1);
            }
            else
                return cp->hit_latency;
        }
    }
    /* **HIT** */
    if (collect_stats)
        cp->hits++;

    /* copy data out of cache block, if block exists */
    if (cp->balloc && p)
    {
        CACHE_BCOPY (cmd, blk, bofs, p, nbytes);
    }

    /* update dirty status */
    if (cmd == Write)
        blk->status |= CACHE_BLK_DIRTY;
    blk->isStale = 0;

    /* Update access stats - Ronz */
    if (!strcmp (cp->name, "dl1"))
    {
        if (cmd == Write)
            wh1++;
        else
            rh1++;

        if(my_rs)
        {
            if(my_rs->isSyncInst && my_rs->op == LDL)
                SyncLoadHit ++;
            if(my_rs->op == LDL_L)
                SyncLoadLHit ++;
            if(my_rs->op == STL_C)
                SyncStoreCHit ++;
            if(my_rs->isSyncInst && my_rs->op == STL)
                SyncStoreHit ++;
        }

        else if(l1_wb_entry)
            if(l1_wb_entry->op == STL_C)
                SyncStoreCHit ++;

    }
    if (!strcmp (cp->name, "dl2"))
    {
        if (cmd == Write)
            wh2++;
        else
            rh2++;
    }
#ifdef STREAM_PREFETCHER
    if (!strcmp (cp->name, "dl2") && blk->spTag && !dl2_prefetch_active)
    {
        launch_sp(blk->spTag-1, threadid);
    }
    blk->spTag = 0;
#endif

    if (blk->way_prev && cp->policy == LRU)
    {
        update_way_list (&cp->sets[set], blk, Head);
    }

    /* tag is unchanged, so hash links (if they exist) are still valid */

    /* record the last block to hit */
    cp->last_tagsetid.tag = CACHE_TAGSET (cp, addr);
    cp->last_tagsetid.threadid = threadid;
    cp->last_blk = blk;

    /* get user block data, if requested and it exists */
    if (udata)
        *udata = blk->user_data;

    if((int) max2 ((cp->hit_latency + lat), (blk->ready - now)) > WAIT_TIME)
        panic("Cache Hit latency too large\n");

    /* return first cycle data is available to access */
    return (int) max2 ((cp->hit_latency + lat), (blk->ready - now));    //add the invalidation latency

}               /* cache_access */

void cache_warmup (struct cache_t *cp,  /* cache to access */
        enum mem_cmd cmd,       /* access type, Read or Write */
        md_addr_t addr,     /* address of access */
        int nbytes,     /* number of bytes to access */
        int threadid)
{
    /* FIXME: During warmup we bring the data in shared mode even during writes. */
    return;

}

//int isLineInvalidated(md_addr_t addr, int threadid)
//{
//    struct cache_t *cp = cache_dl1[threadid];
//    md_addr_t tag = CACHE_TAG (cp, addr);
//    md_addr_t set = CACHE_SET (cp, addr);
//    md_addr_t bofs = CACHE_BLK (cp, addr);
//    struct cache_blk_t *blk;
//    /* low-associativity cache, linear search the way list */
//    for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
//    {
//        if ((blk->tagid.tag == tag) && !(blk->status & CACHE_BLK_VALID))
//            return 1;
//    }
//    return 0;
//}

    void
cache_print (struct cache_t *cp1, struct cache_t *cp2)
{               /* Print cache access stats */
    fprintf (stderr, "ZZ L1D stats\n");
    fprintf (stderr, "ZZ %ld %ld %ld %ld %lu\n", rm1, rh1, wm1, wh1, (unsigned long) cp1->writebacks);
    fprintf (stderr, "ZZ UL2 stats\n");
    if (cp2)
        fprintf (stderr, "ZZ %ld %ld %ld %ld %lu\n", rm2, rh2, wm2, wh2, (unsigned long) cp2->writebacks);
}


    int
simple_cache_flush (struct cache_t *cp)
{
    int i, numflush;
    struct cache_blk_t *blk;

#ifdef TOKENB
    fatal ("need to update this func to account for TOKENB coherence.");
#endif

    numflush = 0;
    for (i = 0; i < cp->nsets; i++)
    {
        for (blk = cp->sets[i].way_head; blk; blk = blk->way_next)
        {
            if (blk->status & CACHE_BLK_VALID)
            {
                //cp->invalidations++;
                blk->status = 0;    //&= ~CACHE_BLK_VALID;
                blk->isStale = 0;

                if (blk->status & CACHE_BLK_DIRTY)
                {
                    if (collect_stats)
                        cp->writebacks++;
                    numflush++;
                }
            }
        }
    }

    return numflush;
}

/* flush the entire cache, returns latency of the operation */
    unsigned int            /* latency of the flush operation */
cache_flush (struct cache_t *cp,    /* cache instance to flush */
        tick_t now) /* time of cache flush */
{

    int i, lat = cp->hit_latency;   /* min latency to probe cache */
    struct cache_blk_t *blk;

    /* blow away the last block to hit */
    cp->last_tagsetid.tag = 0;
    cp->last_tagsetid.threadid = -1;
    cp->last_blk = NULL;

    /* no way list updates required because all blocks are being invalidated */
    for (i = 0; i < cp->nsets; i++)
    {
        for (blk = cp->sets[i].way_head; blk; blk = blk->way_next)
        {
            if (blk->status & CACHE_BLK_VALID)
            {
                //if (collect_stats)
                //cp->invalidations++;
                blk->status = 0;    //&= ~CACHE_BLK_VALID;
                blk->isStale = 0;
                blk->state = MESI_INVALID;
                blk->invCause = 0;
            }
        }
    }
    return lat;
}

/* Copy nbytes of data from/to cache block */
    void
cacheBcopy (enum mem_cmd cmd, struct cache_blk_t *blk, int position, byte_t * data, int nbytes)
{
    CACHE_BCOPY (cmd, blk, position, data, nbytes);
}

int isCacheHit (struct cache_t *cp, md_addr_t addr, int threadid)
{
    if (!strcmp (cp->name, "dl1") && cp->threadid != threadid)
        panic ("Wrond DL1 cache being called in cache access\n");

    md_addr_t tag = CACHE_TAG (cp, addr);
    md_addr_t set = CACHE_SET (cp, addr);
    md_addr_t bofs = CACHE_BLK (cp, addr);
    struct cache_blk_t *blk;

    if (cp->hsize)
    {
        /* higly-associativity cache, access through the per-set hash tables */
        int hindex = (int) CACHE_HASH (cp, tag);

        for (blk = cp->sets[set].hash[hindex]; blk; blk = blk->hash_next)
        {
            if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
                return 1;
        }
    }
    else
    {
        /* low-associativity cache, linear search the way list */
        for (blk = cp->sets[set].way_head; blk; blk = blk->way_next)
        {
            if ((blk->tagid.tag == tag) && (blk->status & CACHE_BLK_VALID))
                return 1;
        }
    }
    return 0;
}

#if     defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
    void
busInit ()
{
    int i, j;

    j = BUSSLOTS * NUMBEROFBUSES;

    struct bs_nd *link;

    busNodePool = NULL;

    for (i = 0; i < j; i++)
    {
        link = calloc (1, sizeof (struct bs_nd));
        if (!link)
            fatal ("out of virtual memory");
        link->next = busNodePool;
        busNodePool = link;
    }
#ifdef BUS_INTERCONNECT
    for (i = 0; i < NUMBEROFBUSES; i++)
        busNodesInUse[i] = NULL;
#endif
}
#endif



//    int
//L2inclusionFunc (struct cache_t *cp, md_addr_t addr)
//{
//    int i, cnt, j;
//    int bsize = cp->bsize;
//    int mask = ~cp->blk_mask;
//    md_addr_t tAddr;
//    struct cache_t *cp2;
//    md_addr_t tag;
//    md_addr_t set;
//    struct cache_blk_t *blk;
//
//    cnt = (cp->bsize / cache_dl1[0]->bsize);
//    tAddr = (addr & mask);
//    for (i = 0; i < numcontexts; i++)
//    {
//#ifdef  EDA
//        if (thecontexts[i]->masterid != 0)
//            continue;
//#endif
//        cp2 = cache_dl1[i];
//        for (j = 0; j < cnt; j++)
//        {
//            addr = tAddr + (j * cp2->bsize);
//            tag = CACHE_TAG (cp2, addr);
//            set = CACHE_SET (cp2, addr);
//
//            for (blk = cp2->sets[set].way_head; blk; blk = blk->way_next)
//            {
//#ifdef SMT_SS
//                if ((blk->tagid.tag == tag) && (blk->tagid.threadid == i) && (blk->status & CACHE_BLK_VALID))
//#else
//                    if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
//#endif
//                        break;
//            }
//            if (blk)
//            {
//                blk->status = 0;    //&= ~CACHE_BLK_VALID;
//                blk->state = MESI_INVALID;
//                blk->invCause = 0;
//                update_way_list (&cp2->sets[set], blk, Tail);
//            }
//        }
//    }
//    /* TODO: We also need to simulate bus delay here */
//    return cache_dl1[0]->hit_latency;
//}



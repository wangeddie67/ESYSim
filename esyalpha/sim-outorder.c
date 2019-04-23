/*
 * sim-outorder.c - sample out-of-order issue perf simulator implementation
 *
 * This file is a part of the SimpleScalar tool suite written by
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
 * Revision 1.5  1998/08/27 16:27:48  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 * instruction predecoding moved to loader module
 * Alpha target support added
 * added support for quadword's
 * added fault support
 * added option ("-max:inst") to limit number of instructions analyzed
 * explicit BTB sizing option added to branch predictors, use
 *       "-btb" option to configure BTB
 * added queue statistics for IFQ, RUU, and LSQ; all terms of Little's
 *       law are measured and reports; also, measures fraction of cycles
 *       in which queue is full
 * added fast forward option ("-fastfwd") that skips a specified number
 *       of instructions (using functional simulation) before starting timing
 *       simulation
 * sim-outorder speculative loads no longer allocate memory pages,
 *       this significantly reduces memory requirements for programs with
 *       lots of mispeculation (e.g., cc1)
* branch predictor updates can now optionally occur in ID, WB,
	*       or CT
	* added target-dependent myprintf() support
* fixed speculative quadword store bug (missing most significant word)
	* sim-outorder now computes correct result when non-speculative register
	*       operand is first defined speculative within the same inst
	* speculative fault handling simplified
	* dead variable "no_ea_dep" removed
	*
	* Revision 1.4  1997/04/16  22:10:23  taustin
* added -commit:width support (from kskadron)
	* fixed "bad l2 D-cache parms" fatal string
	*
	* Revision 1.3  1997/03/11  17:17:06  taustin
	* updated copyright
	* `-pcstat' option support added
	* long/int tweaks made for ALPHA target support
	* better defaults defined for caches/TLBs
	* "mstate" command supported added for DLite!
	* supported added for non-GNU C compilers
	* buglet fixed in speculative trace generation
	* multi-level cache hierarchy now supported
	* two-level predictor supported added
	* I/D-TLB supported added
	* many comments added
	* options package supported added
	* stats package support added
	* resource configuration options extended
	* pipetrace support added
	* DLite! support added
	* writeback throttling now supported
	* decode and issue B/W now decoupled
	* new and improved (and more precise) memory scheduler added
	* cruft for TLB paper removed
	*
	* Revision 1.1  1996/12/05  18:52:32  taustin
	* Initial revision
	*
	*
	*/
	//  FTP -Hillsboro
#include "headers.h"
	/*RN: 10.11.06*/
#include "main.h"
#include "sim-outorder.h"
#include "instManip.h"




	counter_t TotalBarriers = 0;
	counter_t TotalLocks = 0;
	counter_t pure_num_insn = 0;
	counter_t total_num_insn = 0;
	counter_t rename_access = 0;
	counter_t iqram_access = 0;
	counter_t fqram_access = 0;
	counter_t rob1_access = 0;
	counter_t rob2_access = 0;
	counter_t bpred_access = 0;
	counter_t iwakeup_access = 0;
	counter_t fwakeup_access = 0;
	counter_t iselection_access = 0;
	counter_t fselection_access = 0;
	counter_t iregfile_access = 0;
	counter_t fregfile_access = 0;

	counter_t lsq_access = 0;
	counter_t icache_access = 0;
	counter_t dcache_access = 0;
	counter_t dcache2_access = 0;
	counter_t icache2_access = 0;

	
	counter_t ialu1_access = 0;
	counter_t ialu2_access = 0;
	counter_t falu1_access = 0;
	counter_t falu2_access = 0;
	counter_t iresultbus_access = 0;
	counter_t fresultbus_access = 0;
	counter_t lsq_preg_access = 0;
	counter_t lsq_wakeup_access = 0;


	//add by ctb 2013-03-10
	static int IsTrain;
	static float Power_Constraint;
	static float Phase_Parameter;
	static float proc_freq, router_freq, l2_freq; // init freq
	static float proc_v, router_v, l2_v;
	static int random_proc, random_router, random_l2;
	static int set_proc, set_router, set_l2;
	static unsigned int row,col; // The size of each unit, i.e., row*col

    /* hq£ºmy counter for stream prefetch */

	int pcb_blk_count = 0;

    int fl_count;               // number of executed instructions @ fanglei
    fl_count  = 0;              // number of executed instructions @ fanglei
	/*RN: 11.08.06*/
	counter_t sleepCount[MAXTHREADS];


	/* ejt: fault recovery counters added for Wattch */
	counter_t duplication_access, DUPLICATION_access[CLUSTERS], duplication_total = 0;
	counter_t compare_access, COMPARE_access[CLUSTERS], compare_total = 0;


	/* Just a proto that should've been in a header file except that the dependency spaghetti prevented it */
	counter_t predict_commit (struct RUU_station *rs);

	/* Corresponds to the -bpred:btb_use_masterid flag */
	int bpred_btb_use_masterid = 0;
	extern int ilink_run;
	extern int em3d_run;

#ifdef ESYNET
	extern int simEsynetInit(int array_size);
	extern int simEsynetMsgComplete(long w, long x, long y, long z, long 
long int stTime, long long int msgNo);
	extern int simEsynetBufferSpace(long s1, long s2, int opt);
	extern void simEsynetOptions(struct opt_odb_t *odb);
#endif
    extern counter_t esynetMsgNo;
	int mshr_pending_event[MAXTHREADS];
	/* queue occupancy and utilization */
	counter_t same_L1_L2_fifo_full;
	counter_t L1_mshr_full;
	counter_t last_L1_mshr_full[MAXTHREADS];
	counter_t L2_mshr_full;
	counter_t last_L2_mshr_full;
	counter_t L2_mshr_full_prefetch;
	counter_t L1_fifo_full;
	counter_t last_L1_fifo_full[100];
	counter_t Dir_fifo_full;
	counter_t last_Dir_fifo_full[100];
	counter_t Input_queue_full;
	counter_t last_Input_queue_full[100];
	counter_t Output_queue_full;
	counter_t lock_fifo_full;
	counter_t lock_fifo_wrong;
	counter_t lock_fifo_writeback;
	counter_t lock_fifo_benefit;
	counter_t lock_cache_hit;
	counter_t lock_cache_miss;

	counter_t Stall_L1_mshr;
	counter_t Stall_L2_mshr;
	counter_t Stall_L1_fifo;
	counter_t Stall_dir_fifo;
	counter_t Stall_input_queue;
	counter_t Stall_output_queue;

	counter_t WM_Miss;
	counter_t WM_Clean;
	counter_t WM_S;
	counter_t WM_EM;
	counter_t write_shared_used_conf;

	counter_t SyncInstCacheAccess;
	counter_t TestCacheAccess;
	counter_t TestSecCacheAccess;
	counter_t SetCacheAccess;
	counter_t SyncLoadReadMiss;
	counter_t SyncLoadLReadMiss;
	counter_t SyncLoadHit;
	counter_t SyncLoadLHit;
	counter_t SyncStoreCHit;
	counter_t SyncStoreCWriteMiss;
	counter_t SyncStoreCWriteUpgrade;
	counter_t SyncStoreHit;
	counter_t SyncStoreWriteMiss;
	counter_t SyncStoreWriteUpgrade;
	counter_t BarStoreWriteMiss;
	counter_t BarStoreWriteUpgrade;
	counter_t Sync_L2_miss;


	extern counter_t nack_counter, L1_flip_counter, flip_counter, e_to_m;
	extern counter_t normal_nacks, write_nacks, prefetch_nacks, sync_nacks;
	extern counter_t store_conditional_failed;
	counter_t totalMeshHopCount, totalMeshHopDelay, totalEventCount, totalEventProcessTime;
	counter_t totalMeshNorHopCount, totalMeshNorHopDelay, totalMeshSyncHopCount, totalMeshSyncHopDelay;
	counter_t totalOptHopCount, totalOptHopDelay, totalOptNorHopCount, totalOptNorHopDelay, totalOptSyncHopCount, totalOptSyncHopDelay;
	counter_t load_link_shared, load_link_exclusive;
	extern tick_t totalmisshandletime;
	extern counter_t L1_prefetch_usefull;
	extern counter_t L1_prefetch_writeafter;

    // @ fanglei;
    counter_t replace_history_depth;
    counter_t coherence_history_depth;
    counter_t prefetch_replace_history_depth;

    extern counter_t total_L2_MEM_L2Prefetch_Req;
    extern counter_t total_L2_MEM_Data_Miss_Req;
    extern counter_t total_L2_MEM_Inst_Miss_Req;
    extern counter_t total_MEM_L2_Data_Payload;
    extern counter_t total_MEM_L2_Inst_Payload;
    extern counter_t total_L2_MEM_Data_Payload;
    extern counter_t total_L1_L2_Inst_Miss_Req;
    extern counter_t total_L2_L1_Inst_Payload;
    extern counter_t total_L1_L2_Data_Read_Miss_Req;
    extern counter_t total_L2_L1_Data_Read_Miss_Payload;
    extern counter_t total_L2_L1_Data_Read_Invalid;
    extern counter_t total_L1_L2_Data_Read_Invalid_Ack;
    extern counter_t total_L1_L2_Data_Read_Invalid_Payload;
    extern counter_t total_L1_L2_Data_Write_Miss_Req;
    extern counter_t total_L2_L1_Data_Write_Miss_Payload;
    extern counter_t total_L2_L1_Data_L2replaced_Invalid;
    extern counter_t total_L2_L1_Data_Write_Invalid;
    extern counter_t total_L1_L2_Data_L2replaced_Invalid_Ack;
    extern counter_t total_L1_L2_Data_Write_Invalid_Ack;
    extern counter_t total_L1_L2_Data_L2replaced_Invalid_Payload;
    extern counter_t total_L1_L2_Data_Write_Invalid_Payload;
    extern counter_t total_L1_L2_Data_Write_Update;
    extern counter_t total_L2_L1_Data_Write_Update_Ack;
    extern counter_t total_L1_L2_Data_Writeback_Payload;
    extern counter_t total_L2_L1_Data_Verify_Ack;
    extern counter_t total_L2_L1_Data_Verify_Playload;
    extern counter_t total_L2_L1_Data_Prefetch_Playload_PPP;
    extern counter_t total_L1_L2_Read_Verify_Req;
    extern counter_t total_L1_L2_Write_Verify_Req;
    extern counter_t total_L1_L2_shr_tlb_miss_Req;
    extern counter_t total_L1_L2_shr_tlb_write_back_Req;
    extern counter_t total_L1_L2_shr_tlb_sr_recovery_Req;
    extern counter_t total_L1_L2_shr_tlb_update_pr_ack_Req;
    extern counter_t total_L1_L2_shr_tlb_update_pw_ack_Req;
    extern counter_t total_L1_L2_shr_tlb_recovery_ack_Req;
    extern counter_t total_L1_L2_dir_tlb_refill_Req;
    extern counter_t total_L1_L2_dir_tlb_update_Req;
    extern counter_t total_L1_L2_dir_tlb_recovery_Req;
    extern counter_t total_L2_L1_dir_shr_read;
    extern counter_t total_L1_L2_ack_shr_read;
    extern counter_t total_L1_L2_inform_back;
    extern counter_t total_TSHR_Full;

    counter_t L1_miss_table_size;
    counter_t L1_stream_entries;
    counter_t L2_miss_table_size;
    counter_t L2_stream_entries;
    extern counter_t level1_data_cache_access_num;                                  //
    extern counter_t level1_data_cache_access_read_num;                             //
    extern counter_t level1_data_cache_access_write_num;                            //
    extern counter_t level1_data_cache_access_read_hit_num;                         //
    extern counter_t level1_data_cache_access_read_hit_FPP_num;                     //
    extern counter_t level1_data_cache_access_read_hit_FPP_S_M_num;                 //
    extern counter_t level1_data_cache_access_read_hit_FPP_G_D_num;                 //
    extern counter_t level1_data_cache_access_read_hit_FPP_G_T_num;                 //
    extern counter_t level1_data_cache_access_read_hit_PPP_num;                     //
    extern counter_t level1_data_cache_access_read_hit_PPP_S_M_num;                 //
    extern counter_t level1_data_cache_access_read_hit_PPP_G_D_num;                 //
    extern counter_t level1_data_cache_access_read_hit_PPP_G_T_num;                 //
    extern counter_t level1_data_cache_access_read_hit_PPP_verification_num;        //
    extern counter_t level1_data_cache_access_read_hit_PPP_verification_S_M_num;    //
    extern counter_t level1_data_cache_access_read_hit_PPP_verification_G_D_num;    //
    extern counter_t level1_data_cache_access_read_hit_PPP_verification_G_T_num;    //
    extern counter_t level1_data_cache_access_read_hit_PPP_follow_num;              //
    extern counter_t level1_data_cache_access_read_hit_PPP_follow_S_M_num;          //
    extern counter_t level1_data_cache_access_read_hit_PPP_follow_G_D_num;          //
    extern counter_t level1_data_cache_access_read_hit_PPP_follow_G_T_num;          //
    extern counter_t level1_data_cache_access_read_hit_PPP_succeed_num;             //
    extern counter_t level1_data_cache_access_read_hit_PPP_succeed_S_M_num;
    extern counter_t level1_data_cache_access_read_hit_PPP_succeed_G_D_num;
    extern counter_t level1_data_cache_access_read_hit_PPP_succeed_G_T_num;
    extern counter_t level1_data_cache_access_read_hit_PPP_fail_num;                //
    extern counter_t level1_data_cache_access_read_hit_PPP_fail_S_M_num;
    extern counter_t level1_data_cache_access_read_hit_PPP_fail_G_D_num;
    extern counter_t level1_data_cache_access_read_hit_PPP_fail_G_T_num;
    extern counter_t level1_data_cache_access_read_miss_num;                        //
    extern counter_t level1_data_cache_access_read_compulsory_miss_num;
    extern counter_t level1_data_cache_access_read_capacity_miss_num;
    extern counter_t level1_data_cache_access_read_coherence_miss_num;
    extern counter_t level1_data_cache_access_read_prefetch_capacity_miss_num;
    extern counter_t level1_data_cache_access_write_local_num;                      //
    extern counter_t level1_data_cache_access_write_local_FPP_num;
    extern counter_t level1_data_cache_access_write_update_num;
    extern counter_t level1_data_cache_access_write_update_FPP_num;
    extern counter_t level1_data_cache_access_write_update_PPP_num;
    extern counter_t level1_data_cache_access_write_update_PPP_S_M_num;
    extern counter_t level1_data_cache_access_write_update_PPP_G_D_num;
    extern counter_t level1_data_cache_access_write_update_PPP_G_T_num;
    extern counter_t level1_data_cache_access_write_miss_num;
    extern counter_t level1_data_cache_access_write_compulsory_miss_num;
    extern counter_t level1_data_cache_access_write_capacity_miss_num;
    extern counter_t level1_data_cache_access_write_coherence_miss_num;
    extern counter_t level1_data_cache_access_write_prefetch_capacity_miss_num;

    extern counter_t read_L1_miss_L2_hit_num;
    extern counter_t read_L1_miss_L2_hit_total_time;
    extern counter_t read_L2_miss_num;
    extern counter_t read_L2_miss_total_time;
    extern counter_t write_L1_miss_L2_hit_num;
    extern counter_t write_L1_miss_L2_hit_total_time;
    extern counter_t write_L2_miss_num;
    extern counter_t write_L2_miss_total_time;

    // dd cache counter; @ fanglei
    extern counter_t total_page_num;
    extern counter_t pr_page_num;
    extern counter_t pw_page_num;
    extern counter_t sr_page_num;
    extern counter_t sw_page_num;

    extern counter_t total_line_num;
    extern counter_t pr_line_num;
    extern counter_t pw_line_num;
    extern counter_t sr_line_num;
    extern counter_t sw_line_num;

    counter_t SWP_total_line;
    counter_t SWP_sw_line;
    counter_t SWP_sr_line;
    counter_t SWP_pw_line;
    counter_t SWP_pr_line;

    extern counter_t recall_num;
    extern counter_t recall_inv_num;

    extern counter_t read_inv_S_num;
    extern counter_t read_inv_QH_num;
    extern counter_t read_inv_e_QM_num;
    extern counter_t read_inv_d_QM_num;


    extern counter_t write_inv_d_S_num;
    extern counter_t write_inv_e_S_num;
    extern counter_t write_inv_s_S_num;
    extern counter_t write_inv_i_S_num;

    extern counter_t write_inv_d_QM_num;
    extern counter_t write_inv_e_QM_num;
    extern counter_t write_inv_s_QM_num;
    extern counter_t write_inv_i_QM_num;

    extern counter_t write_inv_d_QH_num;
    extern counter_t write_inv_e_QH_num;
    extern counter_t write_inv_s_QH_num;
    extern counter_t write_inv_i_QH_num;

    extern counter_t recall_inv_d_S_num;
    extern counter_t recall_inv_e_S_num;
    extern counter_t recall_inv_s_S_num;
    extern counter_t recall_inv_i_S_num;

    extern counter_t recall_inv_d_Q_num;
    extern counter_t recall_inv_e_Q_num;
    extern counter_t recall_inv_s_Q_num;
    extern counter_t recall_inv_i_Q_num;

    extern counter_t SW_read_request_num;
    extern counter_t SW_write_request_num;
    
    extern counter_t read_S_hit_num;
    extern counter_t read_Q_hit_num;
    extern counter_t read_Q_init_num;
    extern counter_t read_Q_init_SR_num;
    extern counter_t read_Q_init_QR_num;

    extern counter_t write_S_hit_num;
    extern counter_t write_Q_hit_num;
    extern counter_t write_S_init_ed_num;
    extern counter_t write_S_init_ed_SR_num;
    extern counter_t write_S_init_ed_QR_num;
    extern counter_t write_S_init_s_num;
    extern counter_t write_S_init_s_SR_num;
    extern counter_t write_S_init_s_QR_num;
    extern counter_t write_Q_init_num;
    extern counter_t write_Q_init_SR_num;
    extern counter_t write_Q_init_QR_num;

    extern counter_t shr_data_req_initial_num;
    extern counter_t shr_data_req_total_num;
    extern counter_t shr_data_req_fail_num;

    extern counter_t inform_back_num;
    extern counter_t write_back_num;
    extern counter_t inform_back_Q;
    extern counter_t inform_back_S;
    extern counter_t write_back_Q;
    extern counter_t write_back_S;
    extern counter_t mb_pain_delay_num;
    extern counter_t mb_pain_portuse_num;

    extern counter_t read_Q_init_QR_s_num;
    extern counter_t read_Q_init_QR_ed_num;
    extern counter_t write_S_init_ed_QR_s_num;
    extern counter_t write_S_init_ed_QR_ed_num;
    extern counter_t write_S_init_s_QR_s_num;
    extern counter_t write_S_init_s_QR_ed_num;
    extern counter_t write_Q_init_QR_s_num;
    extern counter_t write_Q_init_QR_ed_num;

    extern counter_t recall_inv_d_Q_s_num;
    extern counter_t recall_inv_d_Q_ed_num;
    extern counter_t recall_inv_e_Q_s_num;
    extern counter_t recall_inv_e_Q_ed_num;
    extern counter_t recall_inv_s_Q_s_num;
    extern counter_t recall_inv_s_Q_ed_num;
    extern counter_t recall_inv_i_Q_s_num;
    extern counter_t recall_inv_i_Q_ed_num;

    extern counter_t recall_inv_d_Q_s_ad_num;
    extern counter_t recall_inv_e_Q_s_ad_num;
    extern counter_t recall_inv_s_Q_ed_ad_num;

    extern counter_t line_access_num;
    extern counter_t SWP_line_access_num;
    extern counter_t SWP_PR_line_access_num;
    extern counter_t SWP_PW_line_access_num;
    extern counter_t SWP_SR_line_access_num;
    extern counter_t SWP_SW_line_access_num;

    extern counter_t PVC_vector_distr_record_num;
    extern counter_t PVC_vector_inc_distr_record_num;
    extern counter_t PVC_vector_distr[MAX_DIRECTORY_ASSOC+1];
    extern counter_t PVC_vector_inc_distr[MAX_DIRECTORY_ASSOC+1];

    extern counter_t PVC_exchange_num;
    extern counter_t PVC_flip_num;
    extern counter_t PVC_dowm_no_inv_num;
    extern counter_t PVC_dowm_inv_num;
    extern counter_t PVC_dowm_inv_shr_num;
    extern counter_t PVC_up_no_add_num;
    extern counter_t PVC_up_add_num;
    extern counter_t PVC_up_add_shr_num;
    extern counter_t PVC_exch_vec_shr_distr[MAXTHREADS+1];
    extern counter_t PVC_dowm_inv_shr_distr[MAXTHREADS+1];
    extern counter_t PVC_up_add_shr_distr[MAXTHREADS+1];
    extern counter_t PVC_inform_all_shr_num;
    extern counter_t PVC_rfs_all_shr_num;
    extern counter_t PVC_read_all_shr_num;
    extern counter_t PVC_write_all_shr_num;
    extern counter_t PVC_recall_all_shr_num;
    extern counter_t pvc_nv_conflict_num;



	extern long long int totaleventcountnum;
	extern counter_t totalWriteIndicate;
	extern counter_t total_exclusive_modified;
	extern counter_t total_exclusive_conf;
	extern counter_t total_exclusive_cross;

	extern counter_t totalSyncEvent;
	extern counter_t totalNormalEvent;
	extern counter_t totalSyncWriteM;
	extern counter_t totalSyncReadM;
	extern counter_t totalSyncWriteup;
	extern counter_t totalmisstimeforNormal;
	extern counter_t totalmisstimeforSync;
	extern counter_t total_mem_lat;
	extern counter_t total_mem_access;
	extern counter_t totalL2misstime;
	extern counter_t totalWrongL2misstime;
	extern counter_t TotalL2misses;
	extern long long int total_L1_prefetch;
	extern counter_t l2_prefetch_num;
	extern counter_t write_back_early;
	extern long long int total_L1_first_prefetch;
	extern long long int total_L1_sec_prefetch;
	extern long long int totalreqcountnum;
	extern long long int totalmisscountnum;
	extern counter_t local_cache_access, remote_cache_access, localdirectory, remotedirectory;
	extern counter_t involve_2_hops, involve_2_hop_touch, involve_2_hop_wb, involve_2_hops_wm, involve_4_hops_wm, involve_2_hops_upgrade, involve_4_hops, involve_4_hops_miss, involve_2_hops_miss, involve_4_hops_upgrade;
	counter_t totalSplitNo, totalSplitWM;
	extern counter_t data_private_write, data_private_read, data_shared_write, data_shared_read;
	extern counter_t total_all_close, total_all_almostclose, total_not_all_close, total_p_c_events, total_consumers, total_packets_in_neighbor, total_packets_at_corners;
	extern counter_t total_data_consumers, total_data_close, total_data_far, total_data_at_corner;
	extern counter_t sharer_num[MAXTHREADS];
	extern double average_inside_percent;
	extern double average_outside_percent;
	extern double average_outside_abs_percent;
	extern double average_corner_percent;
	int mem_port_lat[16];
	counter_t port_accord[16];
	/******************************************************/

#define PULL_BACK

#define WATER_NSQUARED 		0
#define	LSQ_LATENCY		4
#define DUP_NO 			2

#define BOUND_POS(N)            ((int)(min2(max2(0, (N)), 2147483647)))
#define LSQSIZE 1024		/* RUU max size */
#define RUUSIZE 1024		/* LSQ max size */

	/*mwr101904: this is a rotate right macro*/
#define 	_rotr(x) (x) = ((x) & 1)?(((x)>>1)|0x80000000):((x)>>1)

	struct quiesceStruct quiesceAddrStruct[CLUSTERS];
	md_addr_t TempInstr = 0x1A2B3C4D;	/* Magic Number */
	int access_mem = 0;
	int access_mem_id = -1;
	int stopped_num_threads = 0;
	int ReadFromWB = 0;
	counter_t cacheMiss[MAXTHREADS], hitInOtherCache[MAXTHREADS];
	counter_t m_sqPrefetchCnt = 0;
	long long int executeLimits;
	long long int dl2ActuallyAccessed, markedLineReplaced;
	long long int quienseWakeup[MAXTHREADS];
	long long int simInsn[MAXTHREADS];
	long long int pureInsn[MAXTHREADS];
	long long int runningCycle[MAXTHREADS];
	int flushImpStats = 0;

	//FILE *myfp, *myfp1;
	int threadForked[4], allForked, storeCondFail, timeToReturn = 0;
	counter_t StoreConditionFailNum;
	int collectStatBarrier, collectStatStop[MAXTHREADS];
	long long int realSimCycle;
	int actualProcess;
	int disable_ray = 1;		/*100104mwr: 1 means ray mode disabled */
	int fu_busy[CLUSTERS], wb_done[CLUSTERS], issue_done[CLUSTERS], fetch_done[CLUSTERS], commit_done[CLUSTERS], dispatch_done[CLUSTERS];
	counter_t brRecovery[CLUSTERS], extraInsn[CLUSTERS];



#if 	defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
	long long int stallNoBusSlot;
	extern int busSlotCount;
	long long int busUsed;
	counter_t busOccupancy = 0;
#endif


#ifdef PULL_BACK
	counter_t pullbackBubble[MAXTHREADS], pullbackWaitCycle[MAXTHREADS], pullbackDoneThisCycle[MAXTHREADS], pullbackHitPred[MAXTHREADS], dl1MissThisCycle[MAXTHREADS], dl1HitThisCycle[MAXTHREADS], pullbackCycleCount[MAXTHREADS],
	pullbackCount[MAXTHREADS];
counter_t predHitHit[MAXTHREADS], predHitMiss[MAXTHREADS], predMissHit[MAXTHREADS], predMissMiss[MAXTHREADS];
#endif

#ifdef  MSHR
int mshr_size = 0;
int mshr_full = 0;
double mshr_count;

#ifdef  MSHR_DIST
double mshr_size_dist[MSHRSIZE];
#endif //MSHR_DIST
double mshr_full_count;
#endif


/* Load store Queue */
short m_shLQSize;		//Total number of entries in Load Queue.
short m_shSQSize;		//Total number of entries in Store Queue.
counter_t m_totalStall[MAXTHREADS];	//Cumulative dispatch stage stalls
counter_t m_sqLQFull[MAXTHREADS];	//Cumulative Load Queue full count
counter_t m_sqSQFull[MAXTHREADS];	//Cumulative Store Queue full count
counter_t m_RUUFull[MAXTHREADS];	//Cumulative RUU Queue full count
counter_t	m_regFull[MAXTHREADS];
counter_t	m_issueFull[MAXTHREADS];
counter_t   m_il1Miss[MAXTHREADS];
counter_t	m_fetchEmpty[MAXTHREADS];
counter_t	m_STLC[MAXTHREADS];
counter_t	m_trap[MAXTHREADS];
counter_t	m_LoadStallMiss[MAXTHREADS];
counter_t	m_StoreStallMiss[MAXTHREADS];
counter_t	m_LoadStallHit[MAXTHREADS];
counter_t	m_StoreStallHit[MAXTHREADS];
counter_t	m_LoadRealMiss[MAXTHREADS];
counter_t	m_StoreRealMiss[MAXTHREADS];
counter_t	m_LoadRealHit[MAXTHREADS];
counter_t	m_StoreRealHit[MAXTHREADS];
counter_t	m_LoadMiss[MAXTHREADS];
counter_t	m_StoreMiss[MAXTHREADS];
counter_t	m_StoreUpdateMiss[MAXTHREADS];
counter_t	m_update_miss_depend_start[MAXTHREADS];
counter_t	m_update_miss_flag[MAXTHREADS];
counter_t	UpdateDependCycle;
volatile int prefetch_flag = 1;
extern counter_t m_update_miss[MAXTHREADS];
extern int lock_waiting[MAXTHREADS];
extern int barrier_waiting[MAXTHREADS];

counter_t m_commitStall[MAXTHREADS];	//Cumulative Commit stall

counter_t invalidation_replay[MAXTHREADS];
counter_t loadLoadReplayCount[MAXTHREADS];
int invalidationStall[MAXTHREADS], invalidateOtherCache;
int commitStall[MAXTHREADS];

int checkLSQforSt (int i, context * current);
counter_t loadRejectReplay[MAXTHREADS], loadRejectCount[MAXTHREADS], loadRejectStat[MAXTHREADS], m_sqNumLdReplay[MAXTHREADS], replayIndex[MAXTHREADS];
int rejectRetryCycle = 3;
counter_t ldIssueCount, ldstRejectionCount[MAXTHREADS];

long long int seqConsReplay[MAXTHREADS], seqConsInsnReplay[MAXTHREADS];
counter_t totalRUUsize[MAXTHREADS], totalLQsize[MAXTHREADS], totalSQsize[MAXTHREADS];
double avgRUUsize[MAXTHREADS], avgLQsize[MAXTHREADS], avgSQsize[MAXTHREADS];

extern int mta_maxthreads;
extern int numThreads[MAXTHREADS];
int collect_stats = 0;
int COHERENT_CACHE;
int MSI;
int spec_benchmarks;
extern counter_t num_of_invalidations;

int collect_barrier_stats[CLUSTERS];
int collect_barrier_release;

int collect_lock_stats[CLUSTERS];
md_addr_t LockInitPC;
md_addr_t last_pc;
md_addr_t last_last_pc;
long long int freezeCounter;
int stopsim;
int fastfwd;

int store_conditional_waiting[MAXTHREADS];

int oc_or_cc_access = 2;

/* USed in power.c...... but they are not doing anything */
int BRANCH_Q_NUM;		/*102504mwr: maximum number of outstanding branches per thread */
int CHECKER_NUMBER = 0;		/*102504mwr: number of checkers */
int PCB_BLK_SIZE = 128;		/* 030705 ejt: pcb block size */
int INORDER_CHECKER = 0;	/*110404mwr: if 1 then checkers issue inorder */
int OUTSTANDING_STORE_NUM = 128;	/*110404mwr: maximum number of outstanding stores per thread */
float checkerVDD, checkerFreq;

/******************************************************/
int get_free_reg (int, int);
void commitWrite (md_addr_t addr, int threadID, counter_t completeCycle, int needBusAccess, int accessed);
void completeWrite ();
void initParallelSimulation ();
void completeStore (int threadid, md_addr_t addr, int width, int isFail);
void completeStore_l (int threadid, md_addr_t addr, int width);

enum md_fault_type WB_access1 (struct mem_t *mem, enum mem_cmd cmd, md_addr_t addr, void *p, int nbytes, int threadID, md_addr_t instr);

/*RN: 12.04.06*/
void seqConsistancyReplay (int replay_index, int threadid);
void fixSpecState (int replay_index, int threadid);

/******************************************************/

struct listelt *ifreelist_head[MAXTHREADS], *ifreelist_tail[MAXTHREADS];
struct listelt *ffreelist_head[MAXTHREADS], *ffreelist_tail[MAXTHREADS];


/*
 * This file implements a very detailed out-of-order issue superscalar
 * processor with a two-level memory system and speculative execution support.
 * This simulator is a performance simulator, tracking the latency of all
 * pipeline operations.
 */

/*
 * simulator options
 */

/* how often to call hotspot */
/* 101504 ejt: remove static declaration so that it can be used in hotspot.c */
int hotspot_cycle_sampling_intvl;

/* maximum number of inst's to execute */
unsigned long max_insts;
unsigned long max_cycle;
static int max_barrier;

/* pipeline trace range and output filename */
static int ptrace_nelt = 0;
static char *ptrace_opts[2];

/* instruction fetch queue size (in insts) */
static int ruu_ifq_size;

/* extra branch mis-prediction latency */
static int ruu_branch_penalty;



/* speed of front-end of machine relative to execution core */
static int fetch_speed;

/* branch predictor type {nottaken|taken|perfect|bimod|2lev|2bcgskew} */
/* grbriggs: removed 'static' */
char *pred_type;

/* lechen, branch predictor2 type, to be used with arvi, or 2 combined
 * predictors, see combined2 below
 * {nottaken|taken|perfect|bimod|2lev|comb|comb2|2bcgskew}
 * comb2: 2 2lev */
char *pred_type2;

/* lechen: */
/*use 2 combined predictors?*/
const int combined2 = FALSE;

/* lechen: */
/*update branch history immediately?*/
const int hist_imm = FALSE;

/*grbriggs: the above two are forced false because not all
 * supporting code was copied over from lechen */



/* bimodal predictor config (<table_size>) */
static int bimod_nelt = 1;
int bimod_config[1] = { /* bimod tbl size */ 2048 };

/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
static int twolev_nelt = 4;
int twolev_config[4] = { /* l1size */ 1, /* l2size */ 1024, /* hist */ 8, /* xor */ FALSE };

/* combining predictor config (<meta_table_size> */
static int comb_nelt = 1;
int comb_config[1] = { /* meta_table_size */ 1024 };

/*lechen, for 2Bc-gskew predictor in EV8*/
static int twobcgskew_nelt = 6;
static int twobcgskew_config[6] = { /*bim_size16384 */ 32768, /*size16384 */ 32768, /*bim_hist_size */ 0, /*g0_hist_size12 */ 22,
	/*g1_hist_size12 */ 4, /*meta_hist_size12 */ 4
};

/* grbriggs: we dont have all the code for two levels */
#if 0
/*lechen, for 2Bc-gskew2 predictor in EV8*/
static int twobcgskew_nelt2 = 6;
static int twobcgskew_config2[6] = { /*bim_size */ 4096, /*size */ 4096, /*bim_hist_size */ 0, /*g0_hist_size */ 12,
	/*g1_hist_size12 */ 6, /*meta_hist_size12 */ 18
};
#endif

/* return address stack (RAS) size */
int ras_size = 8;

/* BTB predictor config (<num_sets> <associativity>) */
static int btb_nelt = 2;
int btb_config[2] = { /* nsets */ 512, /* assoc */ 4 };

/* instruction decode B/W (insts/cycle) */
int ruu_decode_width;
int ruu_issue_width;


/* run pipeline with in-order issue */
int ruu_inorder_issue;
int LSQ_NUM_MORE_THAN_TWO;

/* issue instructions down wrong execution paths */
static int ruu_include_spec = TRUE;


/* instruction commit B/W (insts/cycle) */
int ruu_commit_width;

/* register update unit (RUU) size */
int RUU_size = RUUSIZE;

/* load/store queue (LSQ) size */
int LSQ_size = LSQSIZE;

int LSQ_size_cluster;


/* l1 data cache config, i.e., {<config>|none} */
static char *cache_dl1_opt;

/* l1 data cache hit latency (in cycles) */
int cache_dl1_lat;

/* l2 data cache config, i.e., {<config>|none} */
static char *cache_ml2_opt;     // meta
static char *cache_dl2_opt;     // data

/* l2 data cache hit latency (in cycles) */
int cache_ml2_lat;
int cache_dl2_lat;

int mesh_size;

// dd cache related; @ fanglei
int tlb_c_c;
int multi_blk_set_shift;
int multi_blk_adap_en;
int multi_blk_adap_inform_en;
int mb_pain_en;
int shr_read_en;
int total_inform_back_en;
int repl_ad_en;
int recall_ad_en;
int vector_num;
int sharer_threshold;
int nv_broadcast;

int L2_prefetch_En;
int L1_prefetch_En;

int STREAM_min_En;


int STREAM_L2_distance;
int STREAM_min_distance;

/* l1 instruction cache config, i.e., {<config>|dl1|dl2|none} */
static char *cache_il1_opt;

/* l1 instruction cache hit latency (in cycles) */
int cache_il1_lat;

/* l2 instruction cache config, i.e., {<config>|dl1|dl2|none} */
static char *cache_il2_opt;

/* l2 instruction cache hit latency (in cycles) */
static int cache_il2_lat;

/* flush caches on system calls */
static int flush_on_syscalls;

/* convert 64-bit inst addresses to 32-bit inst equivalents */
static int compress_icache_addrs;

/* memory access latency (<first_chunk> <inter_chunk>) */
static int mem_nelt = 2;
int mem_lat[2] = { /* lat to first chunk */ 18, /* lat between remaining chunks */ 2 };

/* memory access bus width (in bytes) */
int mem_bus_width;
float mem_bus_speed;
int mem_port_num;

/* instruction TLB config, i.e., {<config>|none} */
static char *itlb_opt;

/* data TLB config, i.e., {<config>|none} */
static char *dtlb_opt;

/* inst/data TLB miss latency (in cycles) */
static int tlb_miss_lat;

/* algorism for networks */
static int network_receiver;
static int network_conflict;

/* total number of integer ALU's available */
int res_ialu = 0;

/* total number of integer multiplier/dividers available */
int res_imult = 0;

#ifdef PORT_PER_BANK
/* number of data cache banks */
int res_membank = 0;

/* number of memory system ports available per bank */
int res_mem_port_bank = 0;

int res_membank_cluster;

#endif

/* total number of memory system ports available  */
int res_memport = 0;

/* total number of floating point ALU's available */
int res_fpalu = 0;

/* total number of floating point multiplier/dividers available */
int res_fpmult = 0;

/* for per cluster stuff */
int res_ialu_cluster = 0;
int res_imult_cluster = 0;
int res_fpalu_cluster = 0;
int res_fpmult_cluster = 0;

/* options for Wattch */
int data_width = 64;

/* static power model results */
extern power_result_type power;

//extern power_result_type_dynamic power_dynamic;

int activecontexts;

int thread_fetch = THREADFETCHLIMIT;
int inst_thread_fetch = FETCHLIMIT;
int thread_commit = 0;		/*  starting thread for commit */

#define ICACHEBANKS 8

int ibank = 0;

/*  priority list for selecting the threads for fetch */
int priorityslot = 0;
int priority[MAXTHREADS];

/*  metric list for measuring the priority, updated with icount_thrd for icount scheme */
int key[MAXTHREADS];


#ifdef  PWR_CLUSTER
counter_t iqram_access_cl[CLUSTERS];
counter_t fqram_access_cl[CLUSTERS];
counter_t iwakeup_access_cl[CLUSTERS];
counter_t fwakeup_access_cl[CLUSTERS];
counter_t iselection_access_cl[CLUSTERS];
counter_t fselection_access_cl[CLUSTERS];
counter_t iregfile_access_cl[CLUSTERS];
counter_t fregfile_access_cl[CLUSTERS];
counter_t ialu1_access_cl[CLUSTERS];
counter_t ialu2_access_cl[CLUSTERS];
counter_t falu1_access_cl[CLUSTERS];
counter_t falu2_access_cl[CLUSTERS];
counter_t iresultbus_access_cl[CLUSTERS];
counter_t fresultbus_access_cl[CLUSTERS];
#endif

counter_t lsq_store_data_access = 0;

#ifdef DE_CENTRALIZED_POWER_BUG
counter_t lsq_preg_access_cl[LSQ_PARTS];
counter_t lsq_wakeup_access_cl[LSQ_PARTS];
counter_t lsq_access_cl[LSQ_PARTS];
int extra_lsq = 0;
double extra_lsq_count = 0;
double disp_store_stall = 0;
#else
counter_t lsq_access = 0;
counter_t lsq_preg_access = 0;
counter_t lsq_wakeup_access = 0;
#endif

#ifdef RES1_FULL_MASK
int iq_cl_full_mask;
int fq_cl_full_mask;
int ireg_cl_full_mask;
int freg_cl_full_mask;
int res1_cl_thrd_mask[MAXTHREADS];
#endif

counter_t lsq_total_pop_count_cycle = 0;
counter_t lsq_num_pop_count_cycle = 0;
counter_t iregfile_total_pop_count_cycle = 0;
counter_t iregfile_num_pop_count_cycle = 0;
counter_t fregfile_total_pop_count_cycle = 0;
counter_t fregfile_num_pop_count_cycle = 0;
counter_t iresultbus_total_pop_count_cycle = 0;
counter_t iresultbus_num_pop_count_cycle = 0;
counter_t fresultbus_total_pop_count_cycle = 0;
counter_t fresultbus_num_pop_count_cycle = 0;

#ifdef PROCESS_MODEL
unsigned long long LastSharedAddress;
unsigned int LastSharedSize;
#endif


/* text-based stat profiles */
#define MAX_PCSTAT_VARS 8
static int pcstat_nelt = 0;
static char *pcstat_vars[MAX_PCSTAT_VARS];

/* convert 64-bit inst text addresses to 32-bit inst equivalents */
#ifdef TARGET_PISA
#define IACOMPRESS(A)							\
	(compress_icache_addrs ? ((((A) - current->ld_text_base) >> 1) + current->ld_text_base) : (A))
#define ISCOMPRESS(SZ)							\
	(compress_icache_addrs ? ((SZ) >> 1) : (SZ))
#else /* !TARGET_PISA */
#define IACOMPRESS(A)		(A)
#define ISCOMPRESS(SZ)		(SZ)
#endif /* TARGET_PISA */
/* operate in backward-compatible bugs mode (for testing only) */
static int bugcompat_mode;

/*
 * functional unit resource configuration
 */

/* resource pool indices, NOTE: update these if you change FU_CONFIG */
#define FU_IALU_INDEX			0
#define FU_IMULT_INDEX			1
#define FU_MEMPORT_INDEX		2
#define FU_FPALU_INDEX			3
#define FU_FPMULT_INDEX			4

/* resource pool definition, NOTE: update FU_*_INDEX defs if you change this */
struct res_desc fu_config[] = {
	{
		"integer-ALU",
		4,
		0,
		0,				/*mwr: usr id */
		0,				/*mwr: duplicate */
		{
			{IntALU, 1, 1}
		}
	},
	{
		"integer-MULT/DIV",
		1,
		0,
		0,				/*mwr: usr id */
		0,				/*mwr: duplicate */
		{
#if defined(MULT_DOES_ADD)
			{IntALU, 1, 1},
#endif
			{IntMULT, 3, 1},
			{IntDIV, 20, 19}
		}
	},
	{
		"memory-port",
		1,
		0,
		0,				/*mwr: usr id */
		0,				/*mwr: duplicate */
		{
			{RdPort, 2, 1},
			{WrPort, 2, 1}
		}
	},
	{
		"FP-adder",
		4,
		0,
		0,				/*mwr: usr id */
		0,				/*mwr: duplicate */
		{
			{FloatADD, 2, 1},
			{FloatCMP, 2, 1},
			{FloatCVT, 2, 1}
		}
	},
	{
		"FP-MULT/DIV",
		1,
		0,
		0,				/*mwr: usr id */
		0,				/*mwr: duplicate */
		{
			{FloatMULT, 4, 1},
			{FloatDIV, 12, 12},
			{FloatSQRT, 24, 24}
		}
	},
};

/*	Ronz' stats	*/


#define PRED_BR_CHANGES 0
#define PRED_JR_CHANGES 0
#define DYNAMIC_CHANGE 0
#define METRIC_CHANGE 0
#define LINEAR_PHASE 0

#ifdef ALLOW_ANY_CLUSTER
#define SMALLEST_CONFIG  CLUSTERS
#else
#define SMALLEST_CONFIG  4	/* Keep this at max if you're using pred_changes */
#endif
int actual_clusters = CLUSTERS;

/* Cacheports and res_cacheports are always the same. If at some point, you
   want to model multiple ports per bank, res_cacheports will become higher
   than cacheports. */
#ifdef PORT_PER_BANK
int CACHEPORTS;
int RES_CACHEPORTS;
extern int RES_CACHE_PORTS;
int actual_cacheports;
#else
int actual_cacheports;
#endif

#ifdef  CENTRALIZED_STEER
int dispatch_cluster = 0;
#endif
#ifdef DISPATCH_CLUSTER
int dispatch_cl[CLUSTERS];
#endif //DISPATCH_CLUSTER



#ifdef N_WAY_STEER
#ifndef FIXED_N_WAY
// int N_WAY = CLUSTERS;
int NEIGHBOR_SEEK_N_WAY = CLUSTERS / 2;
#else
//#define NEIGHBOR_SEEK_N_WAY (N_WAY/2)
//int NEIGHBOR_SEEK_N_WAY = n_way/2;
int NEIGHBOR_SEEK_N_WAY = CLUSTERS / 2;
#endif
int steer_cluster_thrd[MAXTHREADS][CLUSTERS];
double abs_steer = 0;
void steer_init (void);

#ifdef   CLUSTER_PRIORITY
int priority_cl_thrd[MAXTHREADS][CLUSTERS];
#endif
#ifdef  THRD_PROFILE
#ifdef mix_8_f
int n_limit_thrd[] = { 8, 8, 8, 8, 8, 4, 2, 2 };
#endif
#ifdef mix_8_i
int n_limit_thrd[] = { 8, 8, 8, 2, 4, 2, 2, 4 };
#endif
#ifdef mix_8_if
int n_limit_thrd[] = { 2, 2, 2, 4, 8, 8, 8, 8 };
#endif
#ifdef mix_8_m
int n_limit_thrd[] = { 2, 2, 2, 2, 8, 8, 8, 8 };
#endif
#ifdef mix_4_i
int n_limit_thrd[] = { 8, 8, 2, 2 };
#endif
#ifdef mix_4_f
int n_limit_thrd[] = { 8, 8, 2, 2 };
#endif
#ifdef mix1_4_m
int n_limit_thrd[] = { 8, 8, 2, 2 };
#endif
#ifdef mix2_4_m
int n_limit_thrd[] = { 2, 2, 8, 8 };
#endif
#else
int n_limit_thrd[MAXTHREADS];
#endif
#ifdef  CMC_START_N_WAY
int n_start_cmc[MAXTHREADS];
int n_start_thrd_cmc[MAXTHREADS];
#endif // CMC_START_N_WAY
int n_start_thrd[MAXTHREADS];
int neighbor_limit_thrd[MAXTHREADS];

#ifdef  N_WAY_ADAPT
int cold_start = 1;
double ipc_old[MAXTHREADS];
int n_limit_thrd_old[MAXTHREADS];
double sim_num_insn_old[MAXTHREADS];
void update_n_limit (void);
#endif //N_WAY_ADAPT
#ifdef THRD_PRIO_ADAPT
void priority_n_way (void);

#ifdef ADAPT_LIMIT
int adapt_limit_thrd[MAXTHREADS];
#endif
#endif
#ifdef PDG_THRD_ADAPT
void pdg_thrd_n_way (void);
#endif
#ifdef PDG_THRD_PRIO_ADAPT
void pdg_priority_n_way (void);

#ifdef COMM_OPER_LIMIT
#ifdef AVG_LIMIT
int avg_high_limit = CLUSTERS;
double avg_high_limit_count = 0;
#endif
int high_limit_thrd[MAXTHREADS];
double high_limit_thrd_dist[MAXTHREADS][CLUSTERS + 1];
double high_limit_dist[CLUSTERS + 1];
#endif // COMM_OPER_LIMIT
#endif //  PDG_THRD_PRIO_ADAPT
#ifdef PDG_ADAPT
void pdg_n_way (void);
#endif
double n_limit_thrd_dist[MAXTHREADS][CLUSTERS + 1];
double n_limit_dist[CLUSTERS + 1];
#endif // N_WAY_STEER

#ifdef  CLUSTER_PRIORITY
/*  priority list for selecting the cluster for steer */
int priority_cl[CLUSTERS];
#endif

#ifdef  PRIORITY_STEER
/*  priority list for selecting the cluster for steer */
int priority_cl[CLUSTERS];
#endif
#ifdef  COMMUNICATION_STEER
#ifdef OPER_COMM_RATIO
int cl_thrd[MAXTHREADS][CLUSTERS];
int cl_num[MAXTHREADS];
#endif
#endif

#ifdef  THRD_WAY_CACHE
int n_cache_start_thrd[MAXTHREADS];
int n_cache_limit_thrd[MAXTHREADS];

#ifdef N_WAY_CACHE_ADAPT
#ifdef PER_THREAD
int overall_n_cache_limit;
int start_up;
double cache_dl1_misses_old[MAXTHREADS];
double cache_dl1_hits_old[MAXTHREADS];
double cache_dl1_miss_rate_old[MAXTHREADS];
int n_cache_limit_old[MAXTHREADS];
double n_cache_limit_dist[MAXTHREADS][32];
double n_cache_limit_count;

#ifdef  MEM_REF_PER
int cache_miss_stable_thrd[MAXTHREADS];
#endif // MEM_REF_PER
#else //PER_THREAD
int overall_n_cache_limit;
int start_up = 1;
double cache_dl1_misses_old;
double cache_dl1_hits_old;
double cache_dl1_miss_rate_old;
int n_cache_limit_old;
double n_cache_limit_dist[32];
double n_cache_limit_count;

#ifdef  MEM_REF
int cache_miss_stable;
#endif // MEM_REF
#endif //PER_THREAD
#endif //N_WAY_CACHE_ADAPT
#endif

#ifdef  MEM_REF_STAT
double mem_ref_thrd[MAXTHREADS];
double mem_ref;
#endif // MEM_REF_STAT

#ifdef CMP_STEER
int no_cluster_thrd[MAXTHREADS];
#endif

#ifdef N_WAY_STEER
int no_cluster_thrd[MAXTHREADS];
#endif

double abs_res_steer = 0;
double num_steer = 0;

long int_distilpthresh = (long) (MIN_INSN_INT / 6);
int distilptable[ITABLESIZE];
struct tag_id_t distilptabletagid[ITABLESIZE];
long distilpcounter[ITABLESIZE];
long distilpsamples[ITABLESIZE];
int distilpcount = 0;
long int_distilp = 0;

int skipped_br = 0;
long distilp = 0;
long distilp_valid = 0;

long insn16 = 0;
long insn4 = 0;

int insn_interval = MIN_INSN_INT;
int h_regs[NUM_H_REGS];		// Ali  useless

int num_sent = 0;
int num_trips = 1;
int lastcluster = 0;
long instseqnum = 0;

int rthresh;
int brthresh;
int msthresh;
int imsthresh;
int cycthresh;
int rnoise;
int brnoise;
int msnoise;
int imsnoise;

double regdelay = 0;
double regtrans = 0;
double totallat = 0;
double totlds = 0;

double flushes = 0;
double num_flushes = 0;
double free_cluster = 0;

/*
#define RTHRESH INSN_INTERVAL*0.01
#define BRTHRESH INSN_INTERVAL*0.01
#define MSTHRESH INSN_INTERVAL*0.001
#define IMSTHRESH INSN_INTERVAL*0.001
#define CYCTHRESH INSN_INTERVAL*0.01
#define RNOISE INSN_INTERVAL*0.05
#define BRNOISE INSN_INTERVAL*0.02
#define MSNOISE INSN_INTERVAL*0.01
#define IMSNOISE INSN_INTERVAL*0.005
 */
#define CYCNOISE 10
#define NUMCYCTHRESH 5

#define STATINT MIN_INSN_INT

int IIQ_size = IQSIZE;
int FIQ_size = FQSIZE;
int IPREG_size = IPREGSIZE;
int FPREG_size = FPREGSIZE;

int IIQ_size_cluster;
int FIQ_size_cluster;
int IPREG_size_cluster;
int FPREG_size_cluster;


#ifdef RAW
int rawq_size = RAWSIZE;
#endif
#ifdef  COMM_BUFF
int timewindow = TIMEWINDOW;
#endif

double non_crit = 0;
double crit = 0;
double tab2_corr = 0;
double tab2_incorr = 0;
double tab2_miss = 0;
double tab_corr = 0;
double tab_incorr = 0;
double tab_miss = 0;
long table_tags[MAXTHREADS][TABLESIZE];
int table_num[MAXTHREADS][TABLESIZE];
long table2_tags[MAXTHREADS][TABLE2SIZE];
int table2_num[MAXTHREADS][TABLE2SIZE];


#define DEV_PERIOD 10

#define DEDICATED_ADDER 0	/* The ld/st queue has its own dedicated
				   adder for eff address computation */
#define DED_ADDER_LAT_DIFF 0	/* Difference in latency of the ld/st adder
				   from that of the intALU adder */
long add_alu = 0;
long nonadd_alu = 0;

#define PERFECT 0		/* If you want a perfect cache */
#define PERFECTLAT 1		/* The latency of the perfect cache */
int notwarm = 1;		/* Initially, warming up the structures */
unsigned long cyclecheck;	/* Cycles spent warming up */

/* Include other stats here that you want to measure without the warmup phase */

#ifdef RAW
int rawq;
int rawq_cl[CLUSTERS];
int rawqready = 0;
int rawqready_cl[CLUSTERS];	// Number of ready entries in the RAW queues
int rawq_full = 0;
double rawq_stall = 0;
#endif
int iissueq;			/* Sizes of the issueq */
int fissueq;

#ifdef PWR_WAKE
int iready = 0;			// Number of ready entries in the issue queues
int fready = 0;
int lsqready = 0;
int iunready = 0;		// Number of ready entries in the issue queues
int funready = 0;
int lsqunready = 0;
double IREADY_count = 0;
double FREADY_count = 0;
double LSQREADY_count = 0;
int iready_cl[CLUSTERS];	// Number of ready entries in the issue queues
int fready_cl[CLUSTERS];
int lsqready_cl[CLUSTERS];
int iunready_cl[CLUSTERS];	// Number of ready entries in the issue queues
int funready_cl[CLUSTERS];
#endif

#ifdef  SPLIT_LSQ
int lsq_cl[LSQ_PARTS];
int lsqunready_cl[CLUSTERS];
#else
//int lsqunready;
#endif //SPLIT_LSQ

#ifdef  PARTITIONED_LSQ
int lsq_part_bank;
int lsq_part_cl = CLUSTERS / LSQ_PARTS;
int lsq_part_thrd;
#endif

int iregfile;			/* Sizes of the physical register files */
int fregfile;
int iregfile_cl[CLUSTERS];
int iextra_cl[CLUSTERS];
int fregfile_cl[CLUSTERS];
int fextra_cl[CLUSTERS];
int iissueq_cl[CLUSTERS];
int fissueq_cl[CLUSTERS];
int num_storeq[CLUSTERS];
int max_storeq[CLUSTERS];
int disp_delay = 0;

int inchk = 0;
long inchk2 = 0;
long inchk3 = 0;
counter_t inilpchk = 0;
int cycchk = 0;
long cycchk2 = 0;
long cycchk3 = 0;
int brchk = 0;
int refchk = 0;
int insnchkpt = 0;
int cyclechkpt = 0;
int brchkpt = 0;
int refchkpt = 0;
int imischkpt = 0;
int mischkpt = 0;
int delchkpt = 0;
int original_bsize = 0;
int cache_dl1_set_shift = 0;
int phase = SMALLEST_CONFIG;
int cycle_count[NUMPHASES + 1];
float stable_count = 0;
int stop_expl = 0;

int iextra;
int fextra;
int ivalid;
int fvalid;

double tot_ireg = 0;
double tot_freg = 0;
double tot_iext = 0;
double tot_fext = 0;
double tot_inondup = 0;
double tot_fnondup = 0;

double comm_delay = 0;
double num_single = 0;
double iss_stall = 0;
double fe_stall = 0;
double n_ready = 0;
double num_semi_f = 0;
double num_qwstall = 0;
double in_this = 0;
double num_freed = 0;
double tot_del = 0;
double tot_del_bw = 0;
double num_bw_of = 0;
double rrobin = 0;

double min_addr = 5600000000ULL;
long max_addr = 0;

double stddev = 0;
int ban0 = 0;
int ban1 = 0;
int ban2 = 0;
int ban3 = 0;
long bin_acc_tot = 0;
long bin_mis_tot = 0;
long bin_acc[(MAXADDR - MINADDR) / BINSIZE];
long bin_lds[(MAXADDR - MINADDR) / BINSIZE];
long bin_tol[(MAXADDR - MINADDR) / BINSIZE];
long bin_mis[(MAXADDR - MINADDR) / BINSIZE];

/*
#define ADDR0 (4832755712 - (83*16384))
#define ADDR1 4832755712
#define ADDR2 (5370019840 + (20*32768))
#define ADDR0 4832755712
#define ADDR1 5368905728
#define ADDR2 5370019840
 */

long addr0, addr1, addr2;

#define HIST 4
int num_issued = 0;
int iss_rate[HIST];
int issue_rate = 0;
long tol[4][3];
long tol0 = 0;
long tol1 = 0;
long tol2 = 0;
long tot_tol = 0;
long tot_tol0 = 0;
long tot_tol1 = 0;
long tot_tol2 = 0;
long tot_tol3 = 0;
long bank_conf = 0;
long st_bank_conf = 0;

#ifdef PORT_PER_BANK
long miss_num[MAXCACHEPORTS];
long ref_num[MAXCACHEPORTS];
#else
long miss_num[CACHEPORTS];
long ref_num[CACHEPORTS];
#endif
long ref_N;

long disp_time[MD_TOTAL_REGS];
long lifetime1 = 0;
long lifetime2 = 0;
long waittime1 = 0;
long waittime2 = 0;
long conswaittime1 = 0;
long conswaittime2 = 0;
long num1 = 0;
long num2 = 0;
long num11 = 0;
long num22 = 0;
long histo[122];

double disp_stalls = 0;
double cause1 = 0;
double cause2 = 0;
double cause3 = 0;
double cause4 = 0;
double cause5 = 0;

/*  stat. for the issue queues and regester files  Ali-SMT*/
double IISSUEQ_count = 0;
double IISSUEQ_fcount = 0;
double FISSUEQ_count = 0;
double FISSUEQ_fcount = 0;
double IPREG_count = 0;
double Iextra_count = 0;
double IPREG_fcount = 0;
double FPREG_count = 0;
double Fextra_count = 0;
double FPREG_fcount = 0;

double iregfile_cl_count[CLUSTERS];
double iextra_cl_count[CLUSTERS];
double fregfile_cl_count[CLUSTERS];
double fextra_cl_count[CLUSTERS];
double iissueq_cl_count[CLUSTERS];
double fissueq_cl_count[CLUSTERS];

#ifdef  SPLIT_LSQ
double lsq_cl_count[LSQ_PARTS];
#endif //SPLIT_LSQ

double no_disp_cluster = 0;
double disp_dist[TOTALFETCHLIMIT + 1];

#ifdef RAW
double rawq_count = 0;
double rawq_fcount = 0;
double rawq_cl_count[CLUSTERS];
double rawq_dist[RAWSIZE / CLUSTERS];
#endif

#define P_TOTAL_REGS IPREGSIZE+FPREGSIZE+1	/* I never use preg 0 */
/* The active logical regs are 1-62, the active phys regs are 1-62+rename */

// struct lreg_id_t ptolmap[P_TOTAL_REGS];
// struct lreg_id_t consumers[P_TOTAL_REGS];
// struct lreg_id_t ptolmap_chk[P_TOTAL_REGS];

int p_total_regs = P_TOTAL_REGS;

#ifdef RABMAP
int inst_use_extra[P_TOTAL_REGS];
int ltopmap_extra[MAXTHREADS][MD_TOTAL_REGS][CLUSTERS];
int ltopmap_preg[MAXTHREADS][MD_TOTAL_REGS][CLUSTERS];
#endif

int shiftreg1[MAXTHREADS][LEV1SIZE];
int specshiftreg1[MAXTHREADS][LEV1SIZE];
int shiftreg2[MAXTHREADS][LEV1SIZE];
int specshiftreg2[MAXTHREADS][LEV1SIZE];
int shiftreg3[MAXTHREADS][LEV1SIZE];
int specshiftreg3[MAXTHREADS][LEV1SIZE];
int shiftreg4[MAXTHREADS][LEV1SIZE];
int specshiftreg4[MAXTHREADS][LEV1SIZE];
int shiftreg5[MAXTHREADS][LEV1SIZE];
int specshiftreg5[MAXTHREADS][LEV1SIZE];
int lev2pred1[LEV2SIZE];
int lev2pred2[LEV2SIZE];
int lev2pred3[LEV2SIZE];
int lev2pred4[LEV2SIZE];
int lev2pred5[LEV2SIZE];

long bank_pred_corr = 0;
long bank_pred_wrong = 0;
long ac_bank_pred_corr = 0;
long ac_bank_pred_wrong = 0;

#ifdef CACHE_MISS_STAT
int spec_rdb_miss = 0;
int spec_wrb_miss = 0;
int count_error = 0;
#endif

/*101504mwr: a function to copy data from power to power_dynamic*/
void power_parameter_copy ();

void bin_init ();
void cache_init ();
void page_table_init ();
void tlb_mshr_init ();
void tlb_cu_init ();
int tlb_cu_check(int threadid, md_addr_t addr, int cmd);
void share_pattern( int threadid, md_addr_t addr, int cmd);
void ilp_init ();
void bankpred_init ();
void reg_init (int);
void cache_change_config (int);
void ring_free ();
void cache_update ();
void compute_queue (int);
void reg_count ();
void bpred_init (int);


long simple_cache_flush (struct cache_t *);
int cluster_check (md_addr_t);
void bank_update (md_addr_t, int, int);
void st_ring_lat (struct RUU_station *);
int ring_lat (int, int, int, int);
void copy_from (int);
void freelist_insert (int, int);
void check_RUU_LSQ_match (void);
void check_reg (void);
void power_clear (void);
void sim_cyc (unsigned long);
void calculate_priority (void);
void insert_fillq (long, md_addr_t);	/* When a cache discovers a miss, */

#ifdef CYCLE_JUMP
void cycle_jump (int);
void ring_free_jump (unsigned long);
#endif
void general_stat (void);

#ifdef EVENT_CACHE_UPDATE
void mshr_init ();
#endif

/*
 * simulator stats
 */

/* total number of instructions executed */
static counter_t sim_total_insn = 0;

/* total number of memory references committed */
static counter_t sim_num_refs = 0;

/* total number of memory references executed */
static counter_t sim_total_refs = 0;

/* total number of loads committed */
static counter_t sim_num_loads = 0;

/* total number of loads executed */
static counter_t sim_total_loads = 0;

/* total number of branches committed */
static counter_t sim_num_branches = 0;

/* total number of branches executed */
static counter_t sim_total_branches = 0;

#ifdef TG
counter_t tg_rate;
counter_t total_packets_generation;
counter_t total_read_packets_generation;
counter_t total_write_packets_generation;
#endif
/* cycle counter */
counter_t sim_cycle = 0;
counter_t global_data_packets_generation = 0;
counter_t TotalMetaPacketsInAll = 0;
counter_t TotalDataPacketsInAll = 0;
counter_t TotalSyncDataPackets = 0;
counter_t TotalSyncMetaPackets = 0;
counter_t Total_MetaPackets_1 = 0;
counter_t Total_DataPackets_1 = 0;
counter_t MetaPackets_1 = 0;
counter_t DataPackets_1 = 0;
counter_t Hops_1 = 0;
counter_t Total_Hops_1 = 0;
counter_t warmupEndCycle = 0;
counter_t data_packets_sync = 0;
counter_t meta_packets_sync = 0;
counter_t data_packets_sending[300][300];
counter_t meta_packets_sending[300][300];
counter_t flits_sending[100][100];
counter_t link_ser_lat[300][300];
counter_t meta_even_req_close[300];
counter_t meta_odd_req_close[300];
counter_t optical_data_packets = 0;
counter_t optical_meta_packets = 0;
counter_t mesh_data_packets = 0;
counter_t mesh_meta_packets = 0;

counter_t parallel_sim_cycle = 1;
int disp2, disp1;

/* occupancy counters */
counter_t IFQ_count;		/* cumulative IFQ occupancy */
counter_t IFQ_fcount;		/* cumulative IFQ full count */
counter_t RUU_count;		/* cumulative RUU occupancy */
counter_t RUU_fcount;		/* cumulative RUU full count */
counter_t LSQ_count;		/* cumulative LSQ occupancy */
counter_t LSQ_fcount;		/* cumulative LSQ full count */

counter_t pfl2Hit = 0;
counter_t pfl2SecMiss = 0;
counter_t pfl2PrimMiss = 0;

#ifdef  COUNT_LSQ
double non_issue_LSQ_count;
double non_issue_LSQ_fcount;
#endif

/* total non-speculative bogus addresses seen (debug var) */
static counter_t sim_invalid_addrs;

/*
 * simulator state variables
 */

/* pipetrace instruction sequence counter */
static unsigned int ptrace_seq = 0;

/* speculation mode, non-zero when mis-speculating, i.e., executing
   instructions down the wrong path, thus state recovery will eventually have
   to occur that resets processor register and memory state back to the last
   precise state */

/* perfect prediction enabled */
static int pred_perfect = FALSE;

/* speculative bpred-update enabled */
static char *bpred_spec_opt;
static enum
{ spec_ID, spec_WB, spec_CT } bpred_spec_update;

/* level 1 instruction cache, entry level instruction cache */
struct cache_t *cache_il1[CLUSTERS];

/* level 2 instruction cache */
struct cache_t *cache_il2;

/* level 1 data cache, entry level data cache */
struct cache_t *cache_dl1[CLUSTERS];

/* level 2 data cache */
struct cache_t *cache_ml2;
struct cache_t *cache_dl2;

/* instruction TLB */
struct cache_t *itlb[CLUSTERS];

/* data TLB */
struct cache_t *dtlb[CLUSTERS];

/* branch predictor */
//static struct bpred_t *pred;

#ifdef LOAD_PREDICTOR
/* load predictor */
static struct bpred_t *ldpred;
int ldpred_spec_update = 0;
double ldpred_count = 0, ldpred_hit = 0;
double over_miss = 0, under_miss = 0;
int cache_miss;
double ld_miss_thrd[MAXTHREADS];

#ifdef PDG
int num_pdg = 0;
double pdg_dist[MAXTHREADS + 1];
#endif
#endif

/* functional unit resource pool */
struct res_pool *fu_pool = NULL;

/* text-based stat profiles */
static struct stat_stat_t *pcstat_stats[MAX_PCSTAT_VARS];
static counter_t pcstat_lastvals[MAX_PCSTAT_VARS];
static struct stat_stat_t *pcstat_sdists[MAX_PCSTAT_VARS];


/* wedge all stat values into a counter_t */
#define STATVAL(STAT)							\
	((STAT)->sc == sc_int							\
	 ? (counter_t)*((STAT)->variant.for_int.var)			\
	 : ((STAT)->sc == sc_uint						\
		 ? (counter_t)*((STAT)->variant.for_uint.var)		\
		 : ((STAT)->sc == sc_counter					\
			 ? *((STAT)->variant.for_counter.var)				\
			 : (panic("bad stat class"), 0))))
/* begin wonder what this is? from garg  */
int ClusterStruct[CLUSTERS][CLUSTERS];
extern long ThrdAccessPattern[MAXTHREADS][CLUSTERS];
//extern FILE *outFile;
//extern FILE *prefetch_finish_vector_file;
//extern FILE *prefetch_finish_overlap_vector_file;
//extern FILE *WV_A_RV_vector_file;
//extern FILE *read_L1_miss_L2_hit_dist_file;
//extern FILE *write_L1_miss_L2_hit_dist_file;
//extern FILE *read_L2_miss_dist_file;
//extern FILE *write_L2_miss_dist_file;

//extern FILE *Miss_Hit_total_file;

//extern counter_t read_L1_miss_L2_hit_dist[10000];
//extern counter_t write_L1_miss_L2_hit_dist[10000];
//extern counter_t read_L2_miss_dist[10000];
//extern counter_t write_L2_miss_dist[10000];

void periodic_adaptation (void);

int stop_fetch = 0;
unsigned long lastSimCycle = 0;


int startTakingStatistics = 0;

#define PERFECTBANKPREDICTOR 1

	void
initClusterInfo ()
{
	int i, l;

	for (i = 0; i < CLUSTERS; i++)
		for (l = 0; l < CLUSTERS; l++)
			ClusterStruct[i][l] = -1;
	return;
}

int HavePrinted = 0;

	void
PrintClusterInfo ()
{
	int gg, rr;

	for (rr = 0; rr < CLUSTERS; rr++)
	{
		printf ("\n");
		for (gg = 0; gg < CLUSTERS; gg++)
			printf ("%d", ClusterStruct[rr][gg]);
	}
}

/* end wonder what this is */
/* memory access latency, assumed to not cross a page boundary */
	static unsigned int		/* total latency of access */
mem_access_latency (int blk_sz)	/* block size accessed */
{
	int chunks = (cache_dl1[0]->bsize)*mem_port_num/ mem_bus_width;
	//int chunks = (blk_sz + (mem_bus_width - 1)) / mem_bus_width;

	assert (chunks > 0);

	return ( /* first chunk latency */ mem_lat[0] +
			( /* remainder chunk latency */ ((float)4/mem_bus_speed) * (chunks - 1)));
	//( /* remainder chunk latency */ mem_lat[1] * (chunks - 1)));
}

#ifdef L2_COMM_LAT
#define L2_COMM_LAT_LOCATION(baddr) (baddr & (CLUSTERS - 1))
#endif

/*
 * cache miss handlers
 */
struct RUU_station *rsCacheAccess = NULL;
FILE *fp_trace;
/* l1 data cache l1 block miss handler function */
unsigned int			/* latency of block access */
dl1_access_fn (enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t baddr,	/* block address to access */
		int bsize,	/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now,	/* time of access */
		int threadid
#if defined (L2_COMM_LAT) || defined (TOKENB)
		, int cluster	/* the cluster who owns this cache */
#endif
	      )
{
	unsigned int lat;
	int which_thread, i;
	int flag = 0;
	which_thread = threadid;
#ifdef  STREAM_PREFETCHER
	/* Forward the request to stream prefetcher. */
	//launch_sp (baddr, threadid);
#endif

	if (cache_dl2)
	{
		/* access next level of data cache hierarchy */

		lat = cache_access (cache_dl2, cmd, baddr, NULL, bsize, /*now*/now, /*pudata*/ NULL, /*repl addr*/NULL, /*rs pointer*/NULL, /*threadid */which_thread, NULL);

		dcache2_access++;	//mwr: already there
		thecontexts[threadid]->dcache2_access++;
		if (cmd == Read)
			return lat;
		else
			return 0;
	}
	else
	{
		/* access main memory */
		if (cmd == Read)
			return mem_access_latency (bsize);
		else			/* FIXME: unlimited write buffers */
			return 0;
	}
}

extern int dl2_prefetch_active;

/* l2 data cache block miss handler function */
static unsigned int		/* latency of block access */
dl2_access_fn (enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t baddr,	/* block address to access */
		int bsize,	/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now,	/* time of access */
		int threadid
#if defined (L2_COMM_LAT) || defined (TOKENB)
		, int cluster	/* not used */
#endif
	      )
{
#ifdef  STREAM_PREFETCHER
	/* L2 cache miss is detected, insert this address into stream pre-fetcher. */
	if (!dl2_prefetch_active)
	{
		insert_sp (baddr, threadid);
	}
#endif

	int lat;
	/* this is a miss to the lowest level, so access main memory */
	//if (cmd == Read)
	{
		int src, port_num, x, y;
		// src = (baddr >> cache_dl2->set_shift) % numcontexts;
        src = (baddr >> MD_LOG_PAGE_SIZE) % numcontexts;
		x = (src /mesh_size);
		y = (src %mesh_size);
		port_num = (x/(mesh_size/2))*2+y/(mesh_size/(mem_port_num/2));
		lat = mem_access_latency (bsize) + mem_port_lat[port_num];
		mem_port_lat[port_num] += (bsize*mem_port_num/mem_bus_width)*((float)4/mem_bus_speed);  //mem bus speed is 1.25GHz while CPU is 5GHz
		return lat;
	}
	// else			/* FIXME: unlimited write buffers */
	//	return 0;
}

/* l1 inst cache l1 block miss handler function */
static unsigned int		/* latency of block access */
il1_access_fn (enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t baddr,	/* block address to access */
		int bsize,	/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now,	/* time of access */
		int threadid
#if defined (L2_COMM_LAT) || defined (TOKENB)
		, int cluster	/* not used */
#endif
	      )
{
	unsigned int lat = 0;
	int matchnum = 0;
	if (cache_il2)
	{
		/* access next level of inst cache hierarchy */
		lat = cache_access (cache_il2, cmd, baddr, NULL, bsize, /* now */ now, /* pudata */ NULL, /* repl addr */ NULL, /* ruu writewait pointer */NULL, threadid IF_L2_COMM_LAT_CL (cluster), NULL);
		/* Wattch -- Dcache2 access */
		dcache2_access++;	//mwr: already there
		thecontexts[threadid]->dcache2_access++;

		if (cmd == Read)
			return lat;
		else
			panic ("writes to instruction memory not supported");
	}
	else
	{
		/* access main memory */
		if (cmd == Read)
			return mem_access_latency (bsize);
		else
			panic ("writes to instruction memory not supported");
	}
	return lat;
}

/* l2 inst cache block miss handler function */
static unsigned int		/* latency of block access */
il2_access_fn (enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t baddr,	/* block address to access */
		int bsize,	/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now,	/* time of access */
		int threadid
#if defined (L2_COMM_LAT) || defined (TOKENB)
		, int cluster	/* not used */
#endif
	      )
{
	/* Wattch -- main memory access -- Wattch-FIXME (offchip) */
	/* this is a miss to the lowest level, so access main memory */
	int lat = 0;
	if (cmd == Read)
	{
		int src, port_num, x, y;
		// src = (baddr >> cache_dl2->set_shift) % numcontexts;
        src = (baddr >> MD_LOG_PAGE_SIZE) % numcontexts;
		x = (src /mesh_size);
		y = (src %mesh_size); /* x, y is the coordinate of the node who generate this miss */
		port_num = (x/(mesh_size/2))*2+y/(mesh_size/(mem_port_num/2));
		lat = mem_access_latency (bsize) + mem_port_lat[port_num];
		mem_port_lat[port_num] += (bsize*mem_port_num/mem_bus_width)*((float)4/mem_bus_speed);  //mem bus speed is 1.25GHz while CPU is 5GHz
		port_accord[port_num] ++;
		return lat;
	}
	else
		panic ("writes to instruction memory not supported");
	return 0;
}


/*
 * TLB miss handlers
 */

/* inst cache block miss handler function */
static unsigned int		/* latency of block access */
itlb_access_fn (enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t baddr,	/* block address to access */
		int bsize,	/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now,	/* time of access */
		int threadid
#if defined (L2_COMM_LAT) || defined (TOKENB)
		, int cluster	/* not used */
#endif
	       )
{
	md_addr_t *phy_page_ptr = (md_addr_t *) blk->user_data;

	/* no real memory access, however, should have user data space attached */
	assert (phy_page_ptr);

	/* fake translation, for now... */
	*phy_page_ptr = 0;

	/* return tlb miss latency */
	return tlb_miss_lat;
}

/* data cache block miss handler function */
static unsigned int		/* latency of block access */
dtlb_access_fn (enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t vpn,	/* block address to access */
		int bsize,	/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now,	/* time of access */
		int threadid
#if defined (L2_COMM_LAT) || defined (TOKENB)
		, int cluster	/* not used */
#endif
	       )
{
	md_addr_t *phy_page_ptr = (md_addr_t *) blk->user_data;

	/* no real memory access, however, should have user data space attached */
	assert (phy_page_ptr);

	/* fake translation, for now... */
	*phy_page_ptr = 0;

	/* return tlb miss latency */
	return tlb_miss_lat;
}


/* register simulator-specific options */
	void
sim_reg_options (struct opt_odb_t *odb)
{
	opt_reg_header (odb,
			"sim-outorder: This simulator implements a very detailed out-of-order issue\n"
			"superscalar processor with a two-level memory system and speculative\n" "execution support.  This simulator is a performance simulator, tracking the\n" "latency of all pipeline operations.\n");

	/* instruction limit */
	opt_reg_uint (odb, "-max:inst", "maximum number of inst's to execute/thread", (unsigned int *) (&max_insts), /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

	/* cycles limit */
	opt_reg_uint (odb, "-max:cycle", "maximum number of cycles's to execute/thread", (unsigned int *) (&max_cycle), /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

	/* barrier limit */
	opt_reg_uint (odb, "-max:barrier", "maximum number of barriers to execute", (unsigned int *) (&max_barrier), /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

	/* network mesh size */
	opt_reg_int (odb, "-network:meshsize", "mesh size of the network", &mesh_size, /* default */ 4,
			/* print */ TRUE, /* format */ NULL);
	
	//*****************************
	//add by ctb 2013-03-10
	opt_reg_float (odb, "-powerconstraint", "the max power constraint", &Power_Constraint, /* default */ 1.00,
			/* print */ TRUE, /* format */ NULL);
	
	opt_reg_float (odb, "-phaseparameter", "the max power constraint", &Phase_Parameter, /* default */ 1.00,
			/* print */ TRUE, /* format */ NULL);
			
	opt_reg_float (odb, "-freq:proc", "the init frequency of processor", &proc_freq, /* default */ 1.00,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_float (odb, "-freq:router", "the init frequency of router", &router_freq, /* default */ 1.00,
			/* print */ TRUE, /* format */ NULL);
	opt_reg_float (odb, "-freq:l2", "the init frequency of l2", &l2_freq, /* default */ 1.00,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_flag (odb, "-istrain", "train the program or not", &IsTrain, /* default */ FALSE, /* print */ TRUE, NULL);
	
	opt_reg_flag (odb, "-random:proc", "random processor frequency", &random_proc, /* default */ FALSE, /* print */ TRUE, NULL);

	opt_reg_flag (odb, "-random:router", "random router frequency", &random_router, /* default */ FALSE, /* print */ TRUE, NULL);

	opt_reg_flag (odb, "-random:l2", "random router frequency", &random_l2, /* default */ FALSE, /* print */ TRUE, NULL);
	
	opt_reg_flag (odb, "-set:proc", "set processor frequency", &set_proc, /* default */ FALSE, /* print */ TRUE, NULL);

	opt_reg_flag (odb, "-set:router", "set router frequency", &set_router, /* default */ FALSE, /* print */ TRUE, NULL);

	opt_reg_flag (odb, "-set:l2", "set router frequency", &set_l2, /* default */ FALSE, /* print */ TRUE, NULL);
	

	opt_reg_uint (odb, "-row", "the region row", &row, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_uint (odb, "-col", "the region col", &col, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);
	//****************end******************

    // dd cache related; @ fanglei

    /*sharer read enable*/
	opt_reg_int (odb, "-sharer:read_en", "sharer read enable", &shr_read_en, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

    /*inform back total en*/
	opt_reg_int (odb, "-inform_back:total_en", "inform back en", &total_inform_back_en, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

    /* tlb classify  */
	opt_reg_int (odb, "-tlb:class_coarse", "tlb class coarse", &tlb_c_c, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

    /*multi_blk_set_shift*/
	opt_reg_int (odb, "-multi_blk:set_shift", "multi blk set shift", &multi_blk_set_shift, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

    /*multi_blk adaptive*/
	opt_reg_int (odb, "-multi_blk:adap_en", "multi blk adaptive", &multi_blk_adap_en, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

    /*multi blk adap pain en*/
	opt_reg_int (odb, "-multi_blk_adap:pain_en", "multi blk adap pain en", &mb_pain_en, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

    /*multi blk adap inform en*/
	opt_reg_int (odb, "-multi_blk_adap:inform_en", "multi blk adap inform en", &multi_blk_adap_inform_en, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

    /*multi blk adap repl ad en*/
	opt_reg_int (odb, "-multi_blk_adap:repl_ad_en", "multi blk adap repl ad en", &repl_ad_en, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

    /*multi blk adap recall ad en*/
	opt_reg_int (odb, "-multi_blk_adap:recall_ad_en", "multi blk adap recall ad en", &recall_ad_en, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);

	/* pvc vector num */
	opt_reg_int (odb, "-pvc:vector_num", "pvc vector num", &vector_num, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	/* pvc sharer threshold */
	opt_reg_int (odb, "-pvc:sharer_threshold", "pvc sharer threshold", &sharer_threshold, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	/* pvc non-vector broadcast */
	opt_reg_int (odb, "-pvc:nv_broadcast", "pvc non-vector broadcast", &nv_broadcast, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	/* L2 prefetch enable */
	opt_reg_int (odb, "-prefetch:L2_En", "enable L2 prefetch", &L2_prefetch_En, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	/* L1 prefetch enable */
	opt_reg_int (odb, "-prefetch:L1_En", "enable L1 prefetch", &L1_prefetch_En, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);


//	/* L1 post proved prefetch enable */

    /* STREAM & min */
	opt_reg_int (odb, "-STREAM:min", "STREAM & min", &STREAM_min_En, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

    /* STREAM_L2_distance */
	opt_reg_int (odb, "-STREAM:L2_distance", "STREAM_L2_distance", &STREAM_L2_distance, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

    /* STREAM_min_distance */
	opt_reg_int (odb, "-STREAM:min_distance", "STREAM_min_distance", &STREAM_min_distance, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	/* trace options */
	opt_reg_string_list (odb, "-ptrace", "generate pipetrace, i.e., <fname|stdout|stderr> <range>", ptrace_opts, /* arr_sz */ 2, &ptrace_nelt, /* default */ NULL,
			/* !print */ FALSE, /* format */ NULL, /* !accrue */ FALSE);


	opt_reg_note (odb, "####### Network configuration #######\n");
#ifdef OPTICAL_INTERCONNECT
	opt_reg_note (odb, "Optical interconnect\n");
#endif
#ifdef ESYNET
    opt_reg_note (odb, "Esynet interconnect\n");
#endif
#ifdef CONF_BIT
	opt_reg_note (odb, "Confirmation based optimazation\n");
#endif

	opt_reg_note (odb, "Confirmation based optimazation for Invalidation\n");

#ifdef WB_SPLIT_NET
	opt_reg_note (odb, "Write back split in network level\n");
#endif
#ifdef PSEUDO_SCHEDULE
	opt_reg_note (odb, "Inorder probability-based scheduling (IRD)\n");
#endif
#ifdef PSEUDO_SCHEDULE_REORDER
	opt_reg_note (odb, "Reorder probability-based scheduling (RRD)\n");
#endif
#ifdef PSEUDO_SCHEDULE_DATA
	opt_reg_note (odb, "Intelligent delay (IDD)\n");
#endif
#ifdef L1_L2_CONF
	opt_reg_note (odb, "L1 and L2 reply conflict resolution\n");
#endif
#ifdef ORDER_NODE
	opt_reg_note (odb, "Ordering by node\n");
#endif
#ifdef ORDER_ADDR
	opt_reg_note (odb, "Ordering by addr\n");
#endif
	opt_reg_note (odb, "###### network configuration end ######\n");

	opt_reg_note (odb,
			"  Pipetrace range arguments are formatted as follows:\n"
			"\n"
			"    {{@|#}<start>}:{{@|#|+}<end>}\n"
			"\n"
			"  Both ends of the range are optional, if neither are specified, the entire\n"
			"  execution is traced.  Ranges that start with a `@' designate an address\n"
			"  range to be traced, those that start with an `#' designate a cycle count\n"
			"  range.  All other range values represent an instruction count range.  The\n"
			"  second argument, if specified with a `+', indicates a value relative\n"
			"  to the first argument, e.g., 1000:+100 == 1000:1100.  Program symbols may\n"
			"  be used in all contexts.\n"
			"\n"
			"    Examples:   -ptrace FOO.trc #0:#1000\n" "                -ptrace BAR.trc @2000:\n" "                -ptrace BLAH.trc :1500\n" "                -ptrace UXXE.trc :\n" "                -ptrace FOOBAR.trc @main:+278\n");

	/* ifetch options */
	opt_reg_int (odb, "-fetch:ifqsize", "instruction fetch queue size (in insts)", &ruu_ifq_size, /* default */ 16,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-fetch:mplat", "extra branch mis-prediction latency", &ruu_branch_penalty, /* default */ 7,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-fetch:speed", "speed of front-end of machine relative to execution core", &fetch_speed, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);


	/* branch predictor options */
	opt_reg_note (odb,
			"  Branch predictor configuration examples for 2-level predictor:\n"
			"    Configurations:   N, M, W, X\n"
			"      N   # entries in first level (# of shift register(s))\n"
			"      W   width of shift register(s)\n"
			"      M   # entries in 2nd level (# of counters, or other FSM)\n"
			"      X   (yes-1/no-0) xor history and address for 2nd level index\n"
			"    Sample predictors:\n"
			"      GAg     : 1, W, 2^W, 0\n"
			"      GAp     : 1, W, M (M > 2^W), 0\n" "      PAg     : N, W, 2^W, 0\n" "      PAp     : N, W, M (M == 2^(N+W)), 0\n" "      gshare  : 1, W, 2^W, 1\n" "  Predictor `comb' combines a bimodal and a 2-level predictor.\n");

	opt_reg_string (odb, "-bpred", "branch predictor type {nottaken|taken|perfect|bimod|2lev|comb|2bcgskew}", &pred_type, /* default */ "comb",
			/* print */ TRUE, /* format */ NULL);

	opt_reg_flag (odb, "-bpred:btb_use_masterid", "use masterid in place of threadid when interacting with BTB, giving you per-process instead of per-thread entries", &bpred_btb_use_masterid, /* default */ TRUE,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int_list (odb, "-bpred:bimod", "bimodal predictor config (<table size>)", bimod_config, bimod_nelt, &bimod_nelt,
			/* default */ bimod_config,
			/* print */ TRUE, /* format */ NULL, /* !accrue */ FALSE);

	opt_reg_int_list (odb, "-bpred:2lev", "2-level predictor config " "(<l1size> <l2size> <hist_size> <xor>)", twolev_config, twolev_nelt, &twolev_nelt,
			/* default */ twolev_config,
			/* print */ TRUE, /* format */ NULL, /* !accrue */ FALSE);

	opt_reg_int_list (odb, "-bpred:comb", "combining predictor config (<meta_table_size>)", comb_config, comb_nelt, &comb_nelt,
			/* default */ comb_config,
			/* print */ TRUE, /* format */ NULL, /* !accrue */ FALSE);

	/*displacement for 2bcgskew tables */
	opt_reg_int (odb, "-disp1", "2bcgskew table displacement for lev 1", &disp1, /*lechen, default was 3 */ 0,
			/* print */ TRUE, /* format */ NULL);

	/*displacement for 2bcgskew tables */
	opt_reg_int (odb, "-disp2", "2bcgskew table displacement for lev 2", &disp2, /*lechen, default was 3 */ 0,
			/* print */ TRUE, /* format */ NULL);

	/*lechen, 2Bc-gskew predictor in EV8 */
	opt_reg_int_list (odb, "-bpred:2bcgskew", "2Bc-gskew predictor config (<bim_size> <size> <bim_hist_size> <g0_hist_size> <g1_hist_size> <meta_hist_size>)", twobcgskew_config, twobcgskew_nelt, &twobcgskew_nelt,
			/* default */ twobcgskew_config,
			/* print */ TRUE, /* format */ NULL, /* !accrue */ FALSE);


	opt_reg_int (odb, "-bpred:ras", "return address stack size (0 for no return stack)", &ras_size, /* default */ ras_size,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int_list (odb, "-bpred:btb", "BTB config (<num_sets> <associativity>)", btb_config, btb_nelt, &btb_nelt,
			/* default */ btb_config,
			/* print */ TRUE, /* format */ NULL, /* !accrue */ FALSE);

	opt_reg_string (odb, "-bpred:spec_update", "speculative predictors update in {ID|WB} (default non-spec)", &bpred_spec_opt, /* default */ NULL,
			/* print */ TRUE, /* format */ NULL);

#if 0				/* grbriggs: not fully implemented here */

	opt_reg_int (odb, "-combined2", "use 2 combined predictors?", &combined2, FALSE,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-hist_imm", "update branch history immediately?", &hist_imm, FALSE,
			/* print */ TRUE, /* format */ NULL);
#endif

	/* decode options */
	opt_reg_int (odb, "-decode:width", "instruction decode B/W (insts/cycle)", &ruu_decode_width, /* default */ 5,
			/* print */ TRUE, /* format */ NULL);

#ifndef REMOVE_MY_CODE
	/* issue options */
	opt_reg_int (odb, "-thrdf:limit", "Number of thread fetch (thrd/cycle<=8)", &thread_fetch, /* default */ THREADFETCHLIMIT,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-instthrdf:width", "instruction fetch B/w per thread (insts/cycle<=decode_width)", &inst_thread_fetch, /* default */ 8,
			/* print */ TRUE, /* format */ NULL);
#endif

	opt_reg_int (odb, "-ipregf:size", "integer physical register file size (per cluster)", &IPREG_size_cluster, /* default */ 80,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-fpregf:size", "floating point physical register file size (per cluster)", &FPREG_size_cluster, /* default */ 80,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-iqueue:size", "integer queue size (insts) (per cluster)", &IIQ_size_cluster, /* default */ 32,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-fqueue:size", "floating point queue size (insts) (per cluster)", &FIQ_size_cluster, /* default */ 32,
			/* print */ TRUE, /* format */ NULL);

#ifdef RAW
	opt_reg_int (odb, "-rawqueue:size", "remote access queue size (insts)", &rawq_size, /* default */ RAWSIZE,
			/* print */ TRUE, /* format */ NULL);
#endif

#ifdef ISSUE_PER_CLUSTER
	opt_reg_int (odb, "-issue:width", "instruction issue B/W (insts/cluster/cycle)", &ruu_issue_width, /* default */ 4,
			/* print */ TRUE, /* format */ NULL);
#else
	opt_reg_int (odb, "-issue:width", "instruction issue B/W (insts/cycle)", &ruu_issue_width, /* default */ 4,
			/* print */ TRUE, /* format */ NULL);
#endif

	opt_reg_flag (odb, "-issue:inorder", "run pipeline with in-order issue", &ruu_inorder_issue, /* default */ FALSE,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_flag (odb, "-issue:wrongpath", "issue instructions down wrong execution paths", &ruu_include_spec, /* default */ TRUE,
			/* print */ TRUE, /* format */ NULL);

	/* commit options */
	opt_reg_int (odb, "-commit:width", "instruction commit B/W (insts/thread/cycle)", &ruu_commit_width, /* default */ 4,
			/* print */ TRUE, /* format */ NULL);

	/* register scheduler options */
	opt_reg_int (odb, "-ruu:size", "register update unit (RUU) size", &RUU_size, /* default */ 16,
			/* print */ TRUE, /* format */ NULL);

	/* memory scheduler options  */
	opt_reg_int (odb, "-lsq:size", "load/store queue (LSQ) size (per cluster)", &LSQ_size_cluster, /* default */ 8,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-lq:size", "load queue (LQ) size", &m_shLQSize, /* default */ 8,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-sq:size", "store queue (SQ) size", &m_shSQSize, /* default */ 8,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_float (odb, "-checker:vdd", "checker vdd", &checkerVDD, /* default */ 0.6,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_float (odb, "-checker:freq", "checker running at what fraction of full freq", &checkerFreq, /* default */ 0.5,
			/* print */ TRUE, /* format */ NULL);

	/* cache options */
	opt_reg_note (odb,
			"  The cache config parameter <config> has the following format:\n"
			"\n"
			"    <name>:<nsets>:<bsize>:<assoc>:<repl>\n"
			"\n"
			"    <name>   - name of the cache being defined\n"
			"    <nsets>  - number of sets in the cache\n"
			"    <bsize>  - block size of the cache\n"
			"    <assoc>  - associativity of the cache\n" "    <repl>   - block replacement strategy, 'l'-LRU, 'f'-FIFO, 'r'-random\n" "\n" "    Examples:   -cache:dl1 dl1:4096:32:1:l\n" "                -dtlb dtlb:128:4096:32:r\n");


	opt_reg_string (odb, "-cache:dl1", "l1 data cache config, i.e., {<config>|none} (each cluster)", &cache_dl1_opt, "dl1:128:32:4:l",
			/* print */ TRUE, NULL);

	opt_reg_int (odb, "-cache:dl1lat", "l1 data cache hit latency (in cycles)", &cache_dl1_lat, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_string (odb, "-cache:ml2", "l2 meta cache config, i.e., {<config>|none}", &cache_ml2_opt, "ml2:1024:64:4:l",
			/* print */ TRUE, NULL);

	opt_reg_string (odb, "-cache:dl2", "l2 data cache config, i.e., {<config>|none}", &cache_dl2_opt, "dl2:1024:64:4:l",
			/* print */ TRUE, NULL);

	opt_reg_int (odb, "-cache:ml2lat", "l2 meta cache hit latency (in cycles)", &cache_ml2_lat, /* default */ 6,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_int (odb, "-cache:dl2lat", "l2 data cache hit latency (in cycles)", &cache_dl2_lat, /* default */ 6,
			/* print */ TRUE, /* format */ NULL);

#ifdef TG
	opt_reg_int (odb, "-cache:tgrate", "traffic generation rate", &tg_rate, /* default */ 100,
			/* print */ TRUE, /* format */ NULL);
#endif
	opt_reg_string (odb, "-cache:il1", "l1 inst cache config, i.e., {<config>|dl1|dl2|none}", &cache_il1_opt, "il1:512:32:1:l",
			/* print */ TRUE, NULL);

	opt_reg_note (odb,
			"  Cache levels can be unified by pointing a level of the instruction cache\n"
			"  hierarchy at the data cache hiearchy using the \"dl1\" and \"dl2\" cache\n"
			"  configuration arguments.  Most sensible combinations are supported, e.g.,\n"
			"\n"
			"    A unified l2 cache (il2 is pointed at dl2):\n"
			"      -cache:il1 il1:128:64:1:l -cache:il2 dl2\n"
			"      -cache:dl1 dl1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n"
			"\n" "    Or, a fully unified cache hierarchy (il1 pointed at dl1):\n" "      -cache:il1 dl1\n" "      -cache:dl1 ul1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n");

	opt_reg_int (odb, "-cache:il1lat", "l1 instruction cache hit latency (in cycles)", &cache_il1_lat, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);

	opt_reg_string (odb, "-cache:il2", "l2 instruction cache config, i.e., {<config>|dl2|none}", &cache_il2_opt, "dl2",
			/* print */ TRUE, NULL);

	opt_reg_int (odb, "-cache:il2lat", "l2 instruction cache hit latency (in cycles)", &cache_il2_lat, /* default */ 6,
			/* print */ TRUE, /* format */ NULL);

#ifdef L2_COMM_LAT
	opt_reg_flag (odb, "-cache:l2commlat", "whether to have a distributed L2 with communication latencies added", &l2_comm_lat_enabled, /* default */ FALSE, /* print */ TRUE, NULL);
#endif

	opt_reg_flag (odb, "-cache:flush", "flush caches on system calls", &flush_on_syscalls, /* default */ FALSE, /* print */ TRUE, NULL);

	opt_reg_flag (odb, "-cache:icompress", "convert 64-bit inst addresses to 32-bit inst equivalents", &compress_icache_addrs, /* default */ FALSE,
			/* print */ TRUE, NULL);

	/* mem options */
	opt_reg_int_list (odb, "-mem:lat", "memory access latency (<first_chunk> <inter_chunk>)", mem_lat, mem_nelt, &mem_nelt, mem_lat,
			/* print */ TRUE, /* format */ NULL, /* !accrue */ FALSE);

	opt_reg_int (odb, "-mem:width", "memory access bus width (in bytes)", &mem_bus_width, /* default */ 32,
			/* print */ TRUE, /* format */ NULL);
	opt_reg_float (odb, "-mem:speed", "memory access bus speed (in GHz)", &mem_bus_speed, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);
	opt_reg_int (odb, "-mem:ports", "memory access port number", &mem_port_num, /* default */ 4,
			/* print */ TRUE, /* format */ NULL);

	/* TLB options */
	opt_reg_string (odb, "-tlb:itlb", "instruction TLB config, i.e., {<config>|none}", &itlb_opt, "itlb:16:4096:4:l", /* print */ TRUE, NULL);

	opt_reg_string (odb, "-tlb:dtlb", "data TLB config, i.e., {<config>|none}", &dtlb_opt, "dtlb:32:4096:4:l", /* print */ TRUE, NULL);

	opt_reg_int (odb, "-tlb:lat", "inst/data TLB miss latency (in cycles)", &tlb_miss_lat, /* default */ 30,
			/* print */ TRUE, /* format */ NULL);

#ifdef NET_ALGR
	/* configuration for network algorism */
	/* receiver algorism 0 means using fixed algorism for receiver choosing */
	/* network algorism 0 means ideal condition which is not considering the confliction */
	opt_reg_int (odb, "-network:rec_algr", "receiver algorism for network", &network_receiver, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);
	opt_reg_int (odb, "-network:net_algr", "confliction algorism for network", &network_conflict, /* default */ 0,
			/* print */ TRUE, /* format */ NULL);
#endif

	/* resource configuration */
	opt_reg_int (odb, "-res:ialu", "total number of integer ALU's available (per cluster)", &res_ialu_cluster, /* default */ fu_config[FU_IALU_INDEX].quantity,
			/* print */ TRUE, /* format */ NULL);

#ifndef ALU_MERGE
	opt_reg_int (odb, "-res:imult", "total number of integer multiplier/dividers available (per cluster)", &res_imult_cluster, /* default */ fu_config[FU_IMULT_INDEX].quantity,
			/* print */ TRUE, /* format */ NULL);
#endif

	opt_reg_int (odb, "-maxThrds", "Maximum Number of Threads Allowed", &mta_maxthreads, 1, TRUE, NULL);    // ? @fanglei

	opt_reg_flag (odb, "-Coherent_Cache", "Coherent Cache", &COHERENT_CACHE, 1, TRUE, NULL);                // ? @fanglei

	opt_reg_flag (odb, "-MSI_prot", "MSI Protocol", &MSI, 0, TRUE, NULL);                                   // ? @fanglei

	opt_reg_flag (odb, "-SPEC", "SPEC Benchmarks", &spec_benchmarks, 1, TRUE, NULL);                        // ? @fanglei

#ifdef  PORT_PER_BANK
	opt_reg_int (odb, "-res:membank", "number of data cache banks available (per Cluster to CPU)", &res_membank_cluster, /* default */ 2,
			/* print */ TRUE, /* format */ NULL);                                                           // ? @fanglei

	opt_reg_int (odb, "-res:memport", "total number of memory system ports available (per bank)", &res_mem_port_bank, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);                                                           // ? @fanglei
#else
	opt_reg_int (odb, "-res:memport", "total number of memory system ports available (to CPU)", &res_memport, /* default */ fu_config[FU_MEMPORT_INDEX].quantity,
			/* print */ TRUE, /* format */ NULL);
#endif

	opt_reg_int (odb, "-res:fpalu", "total number of floating point ALU's available (per cluster)", &res_fpalu_cluster, /* default */ fu_config[FU_FPALU_INDEX].quantity,
			/* print */ TRUE, /* format */ NULL);

#ifndef ALU_MERGE
	opt_reg_int (odb, "-res:fpmult", "total number of floating point multiplier/dividers available (per cluster)", &res_fpmult_cluster, /* default */ fu_config[FU_FPMULT_INDEX].quantity,
			/* print */ TRUE, /* format */ NULL);
#endif

	opt_reg_string_list (odb, "-pcstat", "profile stat(s) against text addr's (mult uses ok)", pcstat_vars, MAX_PCSTAT_VARS, &pcstat_nelt, NULL,
			/* !print */ FALSE, /* format */ NULL, /* accrue */ TRUE);                                      // ? @fanglei

	opt_reg_flag (odb, "-bugcompat", "operate in backward-compatible bugs mode (for testing only)", &bugcompat_mode, /* default */ FALSE, /* print */ TRUE, NULL);  // ? @fanglei

	opt_reg_int (sim_odb, "-hotspot:sampling_intvl", "how many cycles between calls to update_hotspot_stats", &hotspot_cycle_sampling_intvl,
			/* default */ 10000, /* !print */ FALSE, NULL);                                                 // ? @fanglei

	opt_reg_int (odb, "-ray:disabled", "ray active", &disable_ray, /* default */ 1,
			/* print */ TRUE, /* format */ NULL);                                                           // ? @fanglei

	cluster_reg_options (odb);
	hotspot_reg_options (odb);

	/* network configuration options */
	network_reg_options (odb);
}

/* check simulator-specific option values */
	void
sim_check_options (struct opt_odb_t *odb,	/* options database */
		int argc, char **argv)	/* command line arguments */
{
	char name[128], c;
	int nsets, bsize, assoc;

	int i;
	context *current;

#ifdef LOAD_PREDICTOR
	/* bimodal predictor, bpred_create() */
	ldpred = bpred_create (LDPred2bit,
			/* bimod table size */ bimod_config[0],
			/* 2lev l1 size */ 0,
			/* 2lev l2 size */ 0,
			/* meta table size */ 0,
			/* history reg size */ 0,
			/* history xor address */ 0,
			/* btb sets */ 0,
			/* btb assoc */ 0,
			/* ret-addr stack size */ 0);

#endif

#ifdef ISU_N2
	if (ruu_issue_width < 1)
		fatal ("issue width must be positive non-zero");
#else
	if (ruu_issue_width < 1 || (ruu_issue_width & (ruu_issue_width - 1)) != 0)
		fatal ("issue width must be positive non-zero and a power of two");
#endif

	//if (ruu_decode_width < 1 || (ruu_decode_width & (ruu_decode_width - 1)) != 0)
	//	fatal ("issue width must be positive non-zero and a power of two");
	IIQ_size = IIQ_size_cluster;
	FIQ_size = FIQ_size_cluster;
	IPREG_size = IPREG_size_cluster;
	FPREG_size = FPREG_size_cluster;

	p_total_regs = IPREG_size + FPREG_size + 1;

	if (IIQ_size < 1)
		fatal ("integer queue must be positive non-zero and a power of two");

	if (FIQ_size < 1)
		fatal ("floating point queue must be positive non-zero and a power of two");

#ifndef REG_RENAME_CONFIGUARE
	p_total_regs = IPREG_size + FPREG_size + 1;
#endif //REG_RENAME_CONFIGUARE

	if (IPREG_size < 1)
		fatal ("integer physical register file must be positive non-zero and a power of two");

	if (FPREG_size < 1)
		fatal ("floating point physical register file must be positive non-zero and a power of two");

#ifdef RAW
	rawq_size = IIQ_size;
	if (rawq_size < 1 || rawq_size > RAWSIZE)
		fatal ("remote access queue (%d) must be positive non-zero and less than RAWSIZE (%d)", rawq_size, RAWSIZE);
#endif

	if (ruu_commit_width < 1)
		fatal ("commit width must be positive non-zero");

	if (RUU_size < 2 || (RUU_size > RUUSIZE))
		fatal ("RUU size must be a positive number > 1 and < %d", RUUSIZE);

	LSQ_size = LSQ_size_cluster;

	if (LSQ_size < 2 || (LSQ_size > LSQSIZE))
		fatal ("LSQ size must be a positive number > 1 and < %d", LSQSIZE);


	/* use a level 1 D-cache? */
	if (!mystricmp (cache_dl1_opt, "none"))
	{
		/* the level 2 D-cache cannot be defined */
		if (strcmp (cache_dl2_opt, "none"))
			fatal ("the l1 data cache must defined if the l2 cache is defined");
		cache_dl2 = NULL;
	}
	else			/* dl1 is defined */
	{
		if (sscanf (cache_dl1_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
			fatal ("bad l1 D-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");

		for (i = 0; i < CLUSTERS; i++)
		{
			cache_dl1[i] = cache_create (name, nsets, bsize, /* balloc */ TRUE,
					/* usize */ 0, assoc, cache_char2policy (c),
					dl1_access_fn, /* hit lat */ cache_dl1_lat, i);

#ifdef CACHE_SET_STAT
			if (cache_dl1[i]->nsets > MAXCACHESET)
				fatal ("CACHE_SET_STAT array limit must be > dl1->nsets");
#endif //  CACHE_SET_STAT
		}
		cache_dl1_set_shift = cache_dl1[0]->set_shift;
		original_bsize = bsize;
		/* is the level 2 D-cache defined? */
		if (!mystricmp (cache_dl2_opt, "none"))
			cache_dl2 = NULL;
		else
		{
			if (sscanf (cache_ml2_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
				fatal ("bad l2 D-cache parms: " "<name>:<nsets>:<bsize>:<assoc>:<repl>");
			cache_ml2 = cache_create (name, nsets, bsize, /* balloc */ TRUE,/* usize */ 0, assoc, cache_char2policy (c),dl2_access_fn, /* hit lat */ cache_ml2_lat, -1);
            if (sscanf (cache_dl2_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
				fatal ("bad l2 D-cache parms: " "<name>:<nsets>:<bsize>:<assoc>:<repl>");
			cache_dl2 = cache_create (name, nsets, bsize, /* balloc */ TRUE,/* usize */ 0, assoc, cache_char2policy (c),dl2_access_fn, /* hit lat */ cache_dl2_lat, -1);
		}
	}

	/* use a level 1 I-cache? */
	if (!mystricmp (cache_il1_opt, "none"))
	{
		cache_il1[0] = NULL;

		/* the level 2 I-cache cannot be defined */
		if (strcmp (cache_il2_opt, "none"))
			fatal ("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;
	}
	else if (!mystricmp (cache_il1_opt, "dl1"))
	{
		/* the level 2 I-cache cannot be defined */
		if (strcmp (cache_il2_opt, "none"))
			fatal ("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;
	}
	else if (!mystricmp (cache_il1_opt, "dl2"))
	{
		if (!cache_dl2)
			fatal ("I-cache l1 cannot access D-cache l2 as it's undefined");
		cache_il1[0] = cache_dl2;

		/* the level 2 I-cache cannot be defined */
		if (strcmp (cache_il2_opt, "none"))
			fatal ("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;
	}
	else			/* il1 is defined */
	{
		if (sscanf (cache_il1_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
			fatal ("bad l1 I-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");

		for (i = 0; i < CLUSTERS; i++)
		{
			cache_il1[i] = cache_create (name, nsets, bsize, /* balloc */ FALSE,
					/* usize */ 0, assoc, cache_char2policy (c),
					il1_access_fn, /* hit lat */ cache_il1_lat, i);
		}
		/* is the level 2 D-cache defined? */
		if (!mystricmp (cache_il2_opt, "none"))
			cache_il2 = NULL;
		else if (!mystricmp (cache_il2_opt, "dl2"))
		{
			if (!cache_dl2)
				fatal ("I-cache l2 cannot access D-cache l2 as it's undefined");
			cache_il2 = cache_dl2;
		}
		else
		{
			if (sscanf (cache_il2_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
				fatal ("bad l2 I-cache parms: " "<name>:<nsets>:<bsize>:<assoc>:<repl>");

			cache_il2 = cache_create (name, nsets, bsize, /* balloc */ FALSE,
					/* usize */ 0, assoc, cache_char2policy (c),
					il2_access_fn, /* hit lat */ cache_il2_lat, -1);
		}
	}

	/* use an I-TLB? */
	for (i = 0; i < CLUSTERS; i++)
	{
		if (sscanf (itlb_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
			fatal ("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");

		itlb[i] = cache_create (name, nsets, bsize, /* balloc */ FALSE,
				/* usize */ sizeof (md_addr_t), assoc,
				cache_char2policy (c), itlb_access_fn,
				/* hit latency */ 1, i);
	}

	for (i = 0; i < CLUSTERS; i++)
	{
		if (sscanf (dtlb_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
			fatal ("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
		dtlb[i] = cache_create (name, nsets, bsize, FALSE, sizeof (md_addr_t), assoc,cache_char2policy (c), dtlb_access_fn, 1, i);
        if ( dtlb[i]->tag_shift != MD_LOG_PAGE_SIZE )
            panic(" dtlb create error!\n ");
	}

	if (cache_dl1_lat < 1)
		fatal ("l1 data cache latency must be greater than zero");

	if (cache_dl2_lat < 1)
		fatal ("l2 data cache latency must be greater than zero");

	if (cache_ml2_lat < 1)
		fatal ("l2 meta cache latency must be greater than zero");

	if (cache_il1_lat < 1)
		fatal ("l1 instruction cache latency must be greater than zero");

	if (cache_il2_lat < 1)
		fatal ("l2 instruction cache latency must be greater than zero");

	if (mem_nelt != 2)
		fatal ("bad memory access latency (<first_chunk> <inter_chunk>)");

	if (mem_lat[0] < 1 || mem_lat[1] < 1)
		fatal ("all memory access latencies must be greater than zero");

	if (mem_bus_width < 1 || (mem_bus_width & (mem_bus_width - 1)) != 0)
		fatal ("memory bus width must be positive non-zero and a power of two");

	if (tlb_miss_lat < 1)
		fatal ("TLB miss latency must be greater than zero");

	res_ialu = res_ialu_cluster * CLUSTERS;

	if (res_ialu < 1)
		fatal ("number of integer ALU's must be greater than zero");
	if (res_ialu > MAX_INSTS_PER_CLASS)
		fatal ("number of integer ALU's must be <= MAX_INSTS_PER_CLASS");

	fu_config[FU_IALU_INDEX].quantity = res_ialu;

#ifndef ALU_MERGE
	res_imult = res_imult_cluster * CLUSTERS;
	if (res_imult < 1)
		fatal ("number of integer multiplier/dividers must be greater than zero");
	if (res_imult > MAX_INSTS_PER_CLASS)
		fatal ("number of integer mult/div's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_IMULT_INDEX].quantity = res_imult;
#endif

#ifdef PORT_PER_BANK
	res_membank = res_membank_cluster * CLUSTERS;

	if (res_membank < 1)
		fatal ("number of memory system banks must be greater than zero");
	//if (res_membank > 64)
	//	fatal ("number of memory system banks should match the stat_cache_banks 32");
#ifdef  PARTITIONED_LSQ
	lsq_part_bank = res_membank / LSQ_PARTS;
#endif
	if (res_mem_port_bank < 1)
		fatal ("number of memory system ports/bank must be greater than zero");
	if (res_mem_port_bank * res_membank > MAX_INSTS_PER_CLASS)
		fatal ("number of total memory system ports must be <= MAX_INSTS_PER_CLASS");
	if (res_mem_port_bank * res_membank > MAXCACHEPORTS)
		fatal ("number of total memory system ports must be <= MAXCACHEPORTS");
	fu_config[FU_MEMPORT_INDEX].quantity = res_memport = res_mem_port_bank * res_membank;
#else
	if (res_memport < 1)
		fatal ("number of memory system ports must be greater than zero");
	if (res_memport > MAX_INSTS_PER_CLASS)
		fatal ("number of memory system ports must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_MEMPORT_INDEX].quantity = res_memport;
#endif

	res_fpalu = res_fpalu_cluster * CLUSTERS;
	if (res_fpalu < 1)
		fatal ("number of floating point ALU's must be greater than zero");
	if (res_fpalu > MAX_INSTS_PER_CLASS)
		fatal ("number of floating point ALU's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_FPALU_INDEX].quantity = res_fpalu;

#ifndef ALU_MERGE
	res_fpmult = res_fpmult_cluster * CLUSTERS;
	if (res_fpmult < 1)
		fatal ("number of floating point multiplier/dividers must be > zero");
	if (res_fpmult > MAX_INSTS_PER_CLASS)
		fatal ("number of FP mult/div's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_FPMULT_INDEX].quantity = res_fpmult;
#endif
#ifdef TOKENB
	if (tokenB)
		tc_init (MAXTHREADS, cache_dl2->assoc * cache_dl2->nsets, cache_dl2->bsize);
	if (LSQ_PARTS != (n_way_cache * MAXTHREADS))
		fatal ("This only supports 1 cache per thread");
#endif
	network_check_options ();
	cluster_check_options ();
	init_link_descr_tables ();
	init_routing_table ();
	arbitrary_routing_table_init ();
}				/* for sim_check_options */

/* print simulator-specific configuration information */
	void
sim_aux_config (FILE * stream)	/* output stream */
{
	/* nada */
}

/* Cache stats */
counter_t totalUpgrades = 0;
counter_t totalNetUpgrades = 0;
counter_t totalUpgradesUsable = 0;
counter_t totalUpgradesBeneficial = 0;
counter_t lock_network_access = 0;

/* statistics for request spacing in FSOI */
counter_t req_spand[261];
counter_t spand[5];
counter_t downgrade_w, downgrade_r;

extern counter_t sharer_distr[MAXTHREADS];
extern counter_t total_corner_packets, total_neighbor_packets, total_far_packets;

counter_t totalNetWrites = 0;
counter_t totalWritesUsable = 0;
counter_t totalWritesBeneficial = 0;
counter_t exclusive_somewhere = 0;
counter_t front_write = 0;


counter_t totalL2WriteReqHits = 0;
counter_t totalL2WriteReqNoSharer = 0;
counter_t totalL2WriteReqDirty = 0;
counter_t delayL2WriteReqDirty = 0;
counter_t totalL2WriteReqShared = 0;
counter_t delayL2WriteReqShared = 0;

counter_t totalL2ReadReqHits = 0;
counter_t totalL2ReadReqNoSharer = 0;
counter_t totalL2ReadReqDirty = 0;
counter_t delayL2ReadReqDirty = 0;

counter_t totalL2ReadReqShared = 0;
counter_t totalL2WriteReqSharedWT = 0;
counter_t totalL2WriteReqDirtyWT = 0;
counter_t totalL2ReadReqDirtyWT = 0;

counter_t totalL1LinePrefUse = 0;
counter_t totalL1LinePrefNoUse = 0;
counter_t totalL1LineReadUse = 0;
counter_t totalL1LineReadNoUse = 0;
counter_t totalL1LineWriteUse = 0;
counter_t totalL1LineWriteNoUse = 0;

counter_t totalL1LineExlUsed = 0;
counter_t totalL1LineExlInv = 0;
counter_t totalL1LineExlDrop = 0;

counter_t    totalL2ReqPrimMiss = 0;
counter_t    totalL2ReqSecMiss = 0;
counter_t    totalL2ReqHit = 0;
counter_t    totalL2ReqInTrans = 0;
counter_t    totalL2OwnReqInTrans = 0;
counter_t    totalL2ReqInInv = 0;

counter_t    delayL2ReqPrimMiss = 0;
counter_t    delayL2ReqSecMiss = 0;
counter_t    delayL2ReqHit = 0;
counter_t    delayL2ReqInTrans = 0;
counter_t    delayL2OwnReqInTrans = 0;
counter_t    delayL2ReqInInv = 0;

counter_t	totalWriteReq = 0;
counter_t	totalWriteReqInv = 0;
counter_t	totalWriteDelay = 0;
counter_t	totalWriteInvDelay = 0;
counter_t	totalUpgradeReq = 0;
counter_t	totalUpgradeReqInv = 0;
counter_t	totalUpgradeDelay = 0;
counter_t	totalUpgradeInvDelay = 0;

counter_t countNonAllocReadEarly = 0;

/* register simulator-specific statistics */
	void
sim_reg_stats (struct stat_sdb_t *sdb)	/* stats database */
{
	int i, j;

	/* register baseline stats */

	stat_reg_counter (sdb, "pure_num_insn", "total number of instructions committed not including synchronization instructions", &pure_num_insn, pure_num_insn, NULL);
	stat_reg_counter (sdb, "total_num_insn", "total number of instructions committed including synchronization instructions", &total_num_insn, total_num_insn, NULL);
	stat_reg_counter (sdb, "sim_num_insn", "total number of instructions committed not including consistency replay instructions", &sim_num_insn, sim_num_insn, NULL);
	stat_reg_counter (sdb, "TotalBarriers", "total number of barriers", &TotalBarriers, 0, NULL);
	stat_reg_counter (sdb, "TotalLocks", "total number of locks", &TotalLocks, 0, NULL);
	stat_reg_counter (sdb, "sim_num_refs", "total number of loads and stores committed", &sim_num_refs, 0, NULL);
	stat_reg_counter (sdb, "sim_num_loads", "total number of loads committed", &sim_num_loads, 0, NULL);
	stat_reg_formula (sdb, "sim_num_stores", "total number of stores committed", "sim_num_refs - sim_num_loads", NULL);
	stat_reg_counter (sdb, "sim_num_branches", "total number of branches committed", &sim_num_branches, /* initial value */ 0, /* format */ NULL);
	stat_reg_int (sdb, "sim_elapsed_time", "total simulation time in seconds", &sim_elapsed_time, 0, NULL);
	stat_reg_formula (sdb, "sim_inst_rate", "simulation speed (in insts/sec)", "sim_num_insn / sim_elapsed_time", NULL);
	stat_reg_counter (sdb, "sim_total_insn", "total number of instructions executed", &sim_total_insn, 0, NULL);
	stat_reg_counter (sdb, "sim_total_refs", "total number of loads and stores executed", &sim_total_refs, 0, NULL);
	stat_reg_counter (sdb, "sim_total_loads", "total number of loads executed", &sim_total_loads, 0, NULL);
	stat_reg_formula (sdb, "sim_total_stores", "total number of stores executed", "sim_total_refs - sim_total_loads", NULL);
	stat_reg_counter (sdb, "sim_total_branches", "total number of branches executed", &sim_total_branches, /* initial value */ 0, /* format */ NULL);
	/* register performance stats */
	
    stat_reg_counter (sdb, "sim_cycle", "total simulation time in cycles", &sim_cycle, /* initial value */ 0, /* format */ NULL);
	stat_reg_counter (sdb, "realSimCycle", "", &realSimCycle, /* initial value */ 0, /* format */ NULL);
	stat_reg_formula (sdb, "sim_IPC", "instructions per cycle", "sim_num_insn / sim_cycle", /* format */ NULL);
	stat_reg_formula (sdb, "realSimIPC", "", "pure_num_insn / realSimCycle", /* format */ NULL);
	stat_reg_formula (sdb, "mainEffectiveIPC", "", "pure_num_insn / (realSimCycle - main_thread_stall_count)", /* format */ NULL);
	stat_reg_counter (sdb, "num_of_invalidations", "Number_Of_Validations", &num_of_invalidations, /* initial value */ 0, /* format */ NULL);

#if defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
	stat_reg_counter (sdb, "busUsed", "Number of times bus is used", &busUsed, /* initial value */ 0, /* format */ NULL);
	stat_reg_counter (sdb, "busOccupancy", "Occupancy of bus in number of cycles", &busOccupancy, /* initial value */ 0, /* format */ NULL);
	stat_reg_formula (sdb, "busOccupancyPerCycle", "Bus occupancy per cycle", "busOccupancy / realSimCycle", /* format */ NULL);
	stat_reg_counter (sdb, "stallNoBusSlot", "stallNoBusSlot", &stallNoBusSlot, /* initial value */ 0, /* format */ NULL);
#endif
	stat_reg_formula (sdb, "sim_CPI", "cycles per instruction", "sim_cycle / sim_num_insn", /* format */ NULL);
	stat_reg_formula (sdb, "sim_exec_BW", "total instructions (mis-spec + committed) per cycle", "sim_total_insn / sim_cycle", /* format */ NULL);
	stat_reg_counter (sdb, "ldIssueCount", "total ld issue attmept", &ldIssueCount, /* initial value */ 0, /* format */ NULL);
#ifdef PULL_BACK
	char ch2[32];

	for (i = 0; i < numcontexts; i++)
	{
		sprintf (ch2, "pullbackCycleCnt_%d", i);
		stat_reg_counter (sdb, ch2, "", &pullbackCycleCount[i], /* initial value */ 0, /* format */ NULL);
	}
#endif
	stat_reg_counter (sdb, "max_lsq_num", "", &LSQ_NUM_MORE_THAN_TWO, 0, NULL);
	stat_reg_counter (sdb, "total_data_counts", "total num of data packets in all", &TotalDataPacketsInAll, 0, NULL);
	stat_reg_counter (sdb, "total_meta_counts", "total num of meta packets in all", &TotalMetaPacketsInAll, 0, NULL);
	stat_reg_counter (sdb, "total_data_sync_counts", "total num of data sync packets in all", &TotalSyncDataPackets, 0, NULL);
	stat_reg_counter (sdb, "total_meta_sync_counts", "total num of meta sync packets in all", &TotalSyncMetaPackets, 0, NULL);
	stat_reg_counter (sdb, "total_optical_data_counts", "total num of data packets in optical network", &optical_data_packets, 0, NULL);
	stat_reg_counter (sdb, "total_optical_meta_counts", "total num of meta packets in optical network", &optical_meta_packets, 0, NULL);
	stat_reg_counter (sdb, "total_mesh_data_counts", "total num of data packets in mesh network", &mesh_data_packets, 0, NULL);
	stat_reg_counter (sdb, "total_mesh_meta_counts", "total num of meta packets in mesh network", &mesh_meta_packets, 0, NULL);
	stat_reg_counter (sdb, "total_sync_writeup", "total num of event inital created", &totalSyncWriteup, 0, NULL);
	stat_reg_counter (sdb, "total_sync_writemiss", "total num of event inital created", &totalSyncWriteM, 0, NULL);
	stat_reg_counter (sdb, "total_sync_readmiss", "total num of event inital created", &totalSyncReadM, 0, NULL);
	stat_reg_counter (sdb, "total_normal_event_count", "total num of event inital created", &totalNormalEvent, 0, NULL);
	stat_reg_counter (sdb, "total_sync_event_count", "total num of event inital created", &totalSyncEvent, 0, NULL);
	stat_reg_counter (sdb, "total_event_count", "total num of event inital created", &totaleventcountnum, 0, NULL);
	stat_reg_counter (sdb, "total_write_indicate", "", &totalWriteIndicate, 0, NULL);
	stat_reg_counter (sdb, "total_exclusive_modified", "", &total_exclusive_modified, 0, NULL);
	stat_reg_counter (sdb, "total_exclusive_conf", "", &total_exclusive_conf, 0, NULL);
	stat_reg_counter (sdb, "total_exclusive_cross", "", &total_exclusive_cross, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_wb_early", "total num of event created due to L1 prefetch", &write_back_early, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_l1_count", "total num of event created due to L1 prefetch", &total_L1_prefetch, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_l2_count", "total num of event created due to L2 prefetch", &l2_prefetch_num, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_l1_first_count", "total num of event created due to L1 prefetch", &total_L1_first_prefetch, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_l1_sec_count", "total num of event created due to L1 prefetch", &total_L1_sec_prefetch, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_l1_usefull", "total num of usefull L1 prefetch", &L1_prefetch_usefull, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_l1_writeafter", "total num of usefull L1 prefetch", &L1_prefetch_writeafter, 0, NULL);
// @ fanglei

    stat_reg_counter (sdb, "replace_history                                         ", "depth of replace history", &replace_history_depth, REPLACE_HISTORY, NULL);
    stat_reg_counter (sdb, "coherence_history                                       ", "depth of coherence history", &coherence_history_depth, COHERENCE_HISTORY, NULL);
    stat_reg_counter (sdb, "prefetch_replace_history                                ", "depth of prefetch replace history", &prefetch_replace_history_depth, PREFETCH_REPLACE_HISTORY, NULL);
    stat_reg_counter (sdb, "L1_miss_table_size                                      ", "L1_miss_table_size", &L1_miss_table_size, L1_MISS_TABLE_SIZE, NULL);
    stat_reg_counter (sdb, "L1_stream_entries                                       ", "L1_stream_entries", &L1_stream_entries, L1_STREAM_ENTRIES, NULL);
    stat_reg_counter (sdb, "L2_miss_table_size                                      ", "L2_miss_table_size", &L2_miss_table_size, L2_MISS_TABLE_SIZE, NULL);
    stat_reg_counter (sdb, "L2_stream_entries                                       ", "L2_stream_entries", &L2_stream_entries, L2_STREAM_ENTRIES, NULL);

    stat_reg_counter (sdb, "Total_MetaPackets_Num                                   ", "total number of MetaPackets", &Total_MetaPackets_1, 0, NULL);
    stat_reg_counter (sdb, "Total_DataPackets_Num                                   ", "total number of DataPackets", &Total_DataPackets_1, 0, NULL);

    stat_reg_counter (sdb, "total_L2_MEM_L2Prefetch_Req                             ", "total_L2_MEM_L2Prefetch_Req", &total_L2_MEM_L2Prefetch_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L2_MEM_Data_Miss_Req                              ", "total_L2_MEM_Data_Miss_Req", &total_L2_MEM_Data_Miss_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L2_MEM_Inst_Miss_Req                              ", "total_L2_MEM_Inst_Miss_Req", &total_L2_MEM_Inst_Miss_Req, 0, NULL);
    stat_reg_counter (sdb, "total_MEM_L2_Data_Payload                               ", "total_MEM_L2_Data_Payload", &total_MEM_L2_Data_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_MEM_L2_Inst_Payload                               ", "total_MEM_L2_Inst_Payload", &total_MEM_L2_Inst_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L2_MEM_Data_Payload                               ", "total_L2_MEM_Data_Payload", &total_L2_MEM_Data_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Inst_Miss_Req                               ", "total_L1_L2_Inst_Miss_Req", &total_L1_L2_Inst_Miss_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Inst_Payload                                ", "total_L2_L1_Inst_Payload", &total_L2_L1_Inst_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Read_Miss_Req                          ", "total_L1_L2_Data_Read_Miss_Req", &total_L1_L2_Data_Read_Miss_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Read_Miss_Payload                      ", "total_L2_L1_Data_Read_Miss_Payload", &total_L2_L1_Data_Read_Miss_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Read_Invalid                           ", "total_L2_L1_Data_Read_Invalid", &total_L2_L1_Data_Read_Invalid, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Read_Invalid_Ack                       ", "total_L1_L2_Data_Read_Invalid_Ack", &total_L1_L2_Data_Read_Invalid_Ack, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Read_Invalid_Payload                   ", "total_L1_L2_Data_Read_Invalid_Payload", &total_L1_L2_Data_Read_Invalid_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Write_Miss_Req                         ", "total_L1_L2_Data_Write_Miss_Req", &total_L1_L2_Data_Write_Miss_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Write_Miss_Payload                     ", "total_L2_L1_Data_Write_Miss_Payload", &total_L2_L1_Data_Write_Miss_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_L2replaced_Invalid                     ", "total_L2_L1_Data_L2replaced_Invalid", &total_L2_L1_Data_L2replaced_Invalid, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Write_Invalid                          ", "total_L2_L1_Data_Write_Invalid", &total_L2_L1_Data_Write_Invalid, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_L2replaced_Invalid_Ack                 ", "total_L1_L2_Data_L2replaced_Invalid_Ack", &total_L1_L2_Data_L2replaced_Invalid_Ack, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Write_Invalid_Ack                      ", "total_L1_L2_Data_Write_Invalid_Ack", &total_L1_L2_Data_Write_Invalid_Ack, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_L2replaced_Invalid_Payload             ", "total_L1_L2_Data_L2replaced_Invalid_Payload", &total_L1_L2_Data_L2replaced_Invalid_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Write_Invalid_Payload                  ", "total_L1_L2_Data_Write_Invalid_Payload", &total_L1_L2_Data_Write_Invalid_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Write_Update                           ", "total_L1_L2_Data_Write_Update", &total_L1_L2_Data_Write_Update, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Write_Update_Ack                       ", "total_L2_L1_Data_Write_Update_Ack", &total_L2_L1_Data_Write_Update_Ack, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Data_Writeback_Payload                      ", "total_L1_L2_Data_Writeback_Payload", &total_L1_L2_Data_Writeback_Payload, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Verify_Ack                             ", "total_L2_L1_Data_Verify_Ack", &total_L2_L1_Data_Verify_Ack, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Verify_Playload                        ", "total_L2_L1_Data_Verify_Playload", &total_L2_L1_Data_Verify_Playload, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_Data_Prefetch_Playload_PPP                  ", "total_L2_L1_Data_Prefetch_Playload_PPP", &total_L2_L1_Data_Prefetch_Playload_PPP, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Read_Verify_Req                             ", "total_L1_L2_Read_Verify_Req", &total_L1_L2_Read_Verify_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_Write_Verify_Req                            ", "total_L1_L2_Write_Verify_Req", &total_L1_L2_Write_Verify_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_shr_tlb_miss_Req                            ", "total_L1_L2_shr_tlb_miss_Req", &total_L1_L2_shr_tlb_miss_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_shr_tlb_write_back_Req                      ", "total_L1_L2_shr_tlb_write_back_Req", &total_L1_L2_shr_tlb_write_back_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_shr_tlb_sr_recovery_Req                     ", "total_L1_L2_shr_tlb_sr_recovery_Req", &total_L1_L2_shr_tlb_sr_recovery_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_shr_tlb_update_pr_ack_Req                   ", "total_L1_L2_shr_tlb_update_pr_ack_Req", &total_L1_L2_shr_tlb_update_pr_ack_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_shr_tlb_update_pw_ack_Req                   ", "total_L1_L2_shr_tlb_update_pw_ack_Req", &total_L1_L2_shr_tlb_update_pw_ack_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_shr_tlb_recovery_ack_Req                    ", "total_L1_L2_shr_tlb_recovery_ack_Req", &total_L1_L2_shr_tlb_recovery_ack_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_dir_tlb_refill_Req                          ", "total_L1_L2_dir_tlb_refill_Req", &total_L1_L2_dir_tlb_refill_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_dir_tlb_update_Req                          ", "total_L1_L2_dir_tlb_update_Req", &total_L1_L2_dir_tlb_update_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_dir_tlb_recovery_Req                        ", "total_L1_L2_dir_tlb_recovery_Req", &total_L1_L2_dir_tlb_recovery_Req, 0, NULL);
    stat_reg_counter (sdb, "total_L2_L1_dir_shr_read                                ", "total_L2_L1_dir_shr_read", &total_L2_L1_dir_shr_read, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_ack_shr_read                                ", "total_L1_L2_ack_shr_read", &total_L1_L2_ack_shr_read, 0, NULL);
    stat_reg_counter (sdb, "total_L1_L2_inform_back                                 ", "total_L1_L2_inform_back", &total_L1_L2_inform_back, 0, NULL);
    stat_reg_counter (sdb, "total_TSHR_Full                                         ", "total_TSHR_Full", &total_TSHR_Full, 0, NULL);


    stat_reg_counter (sdb, "level1_data_cache_access                                ", "total num of level1 data cache_access", &level1_data_cache_access_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_read                           ", "total num of level1 data cache_access_read", &level1_data_cache_access_read_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write                          ", "total num of level1 data cache_access_write", &level1_data_cache_access_write_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_read_hit                       ", "total num of level1 data cache_access_read_hit", &level1_data_cache_access_read_hit_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_read_miss                      ", "total num of level1 data cache_access_read_miss", &level1_data_cache_access_read_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_read_compulsory_miss           ", "total num of level1 data cache_access_read_compulsory_miss", &level1_data_cache_access_read_compulsory_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_read_capacity_miss             ", "total num of level1 data cache_access_read_capacity_miss", &level1_data_cache_access_read_capacity_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_read_coherence_miss            ", "total num of level1 data cache_access_read_coherence_miss", &level1_data_cache_access_read_coherence_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_read_prefetch_capacity_miss    ", "total num of level1 data cache_access_read_prefetch_capacity_miss", &level1_data_cache_access_read_prefetch_capacity_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_miss                     ", "total num of level1 data cache_access_write_miss", &level1_data_cache_access_write_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_compulsory_miss          ", "total num of level1 data cache_access_write_compulsory_miss", &level1_data_cache_access_write_compulsory_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_capacity_miss            ", "total num of level1 data cache_access_write_capacity_miss", &level1_data_cache_access_write_capacity_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_coherence_miss           ", "total num of level1 data cache_access_write_coherence_miss", &level1_data_cache_access_write_coherence_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_prefetch_capacity_miss   ", "total num of level1 data cache_access_write_prefetch_capacity_miss", &level1_data_cache_access_write_prefetch_capacity_miss_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_local                    ", "total num of level1 data cache_access_write_local", &level1_data_cache_access_write_local_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_local_FPP                ", "total num of level1 data cache_access_write_local_FPP", &level1_data_cache_access_write_local_FPP_num, 0, NULL);
    stat_reg_counter (sdb, "level1_data_cache_access_write_update                   ", "total num of level1 data cache_access_write_update", &level1_data_cache_access_write_update_num, 0, NULL);


    stat_reg_counter (sdb, "read_L1_miss_L2_hit_num                                 ", "read L1 miss L2 hit num ", &read_L1_miss_L2_hit_num, 0, NULL);
    stat_reg_counter (sdb, "read_L1_miss_L2_hit_total_time                          ", "read L1 miss L2 hit total time", &read_L1_miss_L2_hit_total_time, 0, NULL);
    stat_reg_counter (sdb, "read_L2_miss_num                                        ", "read L2 miss num", &read_L2_miss_num, 0, NULL);
    stat_reg_counter (sdb, "read_L2_miss_total_time                                 ", "read L2 miss total time ", &read_L2_miss_total_time, 0, NULL);
    stat_reg_counter (sdb, "write_L1_miss_L2_hit_num                                ", "write L1 miss L2 hit num", &write_L1_miss_L2_hit_num, 0, NULL);
    stat_reg_counter (sdb, "write_L1_miss_L2_hit_total_time                         ", "write L1 miss L2 hit total time", &write_L1_miss_L2_hit_total_time, 0, NULL);
    stat_reg_counter (sdb, "write_L2_miss_num                                       ", "write L2 miss num", &write_L2_miss_num, 0, NULL);
    stat_reg_counter (sdb, "write_L2_miss_total_time                                ", "write L2 miss total time", &write_L2_miss_total_time, 0, NULL);

    // dd cache counter; @ fanglei
    // page line classify @ fanglei
    stat_reg_counter (sdb, "total_page_num                                          ", "total_page_num", &total_page_num, 0, NULL);
    stat_reg_counter (sdb, "pr_page_num                                             ", "pr_page_num", &pr_page_num, 0, NULL);
    stat_reg_counter (sdb, "pw_page_num                                             ", "pw_page_num", &pw_page_num, 0, NULL);
    stat_reg_counter (sdb, "sr_page_num                                             ", "sr_page_num", &sr_page_num, 0, NULL);
    stat_reg_counter (sdb, "sw_page_num                                             ", "sw_page_num", &sw_page_num, 0, NULL);
    stat_reg_counter (sdb, "total_line_num                                          ", "total_line_num", &total_line_num, 0, NULL);
    stat_reg_counter (sdb, "pr_line_num                                             ", "pr_line_num", &pr_line_num, 0, NULL);
    stat_reg_counter (sdb, "pw_line_num                                             ", "pw_line_num", &pw_line_num, 0, NULL);
    stat_reg_counter (sdb, "sr_line_num                                             ", "sr_line_num", &sr_line_num, 0, NULL);
    stat_reg_counter (sdb, "sw_line_num                                             ", "sw_line_num", &sw_line_num, 0, NULL);
    stat_reg_counter (sdb, "SWP_total_line                                          ", "SWP_total_line", &SWP_total_line, 0, NULL);
    stat_reg_counter (sdb, "SWP_sw_line                                             ", "SWP_sw_line", &SWP_sw_line, 0, NULL);
    stat_reg_counter (sdb, "SWP_sr_line                                             ", "SWP_sr_line", &SWP_sr_line, 0, NULL);
    stat_reg_counter (sdb, "SWP_pw_line                                             ", "SWP_pw_line", &SWP_pw_line, 0, NULL);
    stat_reg_counter (sdb, "SWP_pr_line                                             ", "SWP_pr_line", &SWP_pr_line, 0, NULL);

    stat_reg_counter (sdb, "recall_num                                              ", "recall_num", &recall_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_num                                          ", "recall_inv_num", &recall_inv_num, 0, NULL);

    stat_reg_counter (sdb, "read_inv_S_num                                          ", "read_inv_S_num", &read_inv_S_num, 0, NULL);
    stat_reg_counter (sdb, "read_inv_QH_num                                         ", "read_inv_QH_num", &read_inv_QH_num, 0, NULL);
    stat_reg_counter (sdb, "read_inv_e_QM_num                                       ", "read_inv_e_QM_num", &read_inv_e_QM_num, 0, NULL);
    stat_reg_counter (sdb, "read_inv_d_QM_num                                       ", "read_inv_d_QM_num", &read_inv_d_QM_num, 0, NULL);

    stat_reg_counter (sdb, "write_inv_d_S_num                                       ", "write_inv_d_S_num", &write_inv_d_S_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_e_S_num                                       ", "write_inv_e_S_num", &write_inv_e_S_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_s_S_num                                       ", "write_inv_s_S_num", &write_inv_s_S_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_i_S_num                                       ", "write_inv_i_S_num", &write_inv_i_S_num, 0, NULL);

    stat_reg_counter (sdb, "write_inv_d_QM_num                                      ", "write_inv_d_QM_num", &write_inv_d_QM_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_e_QM_num                                      ", "write_inv_e_QM_num", &write_inv_e_QM_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_s_QM_num                                      ", "write_inv_s_QM_num", &write_inv_s_QM_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_i_QM_num                                      ", "write_inv_i_QM_num", &write_inv_i_QM_num, 0, NULL);

    stat_reg_counter (sdb, "write_inv_d_QH_num                                      ", "write_inv_d_QH_num", &write_inv_d_QH_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_e_QH_num                                      ", "write_inv_e_QH_num", &write_inv_e_QH_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_s_QH_num                                      ", "write_inv_s_QH_num", &write_inv_s_QH_num, 0, NULL);
    stat_reg_counter (sdb, "write_inv_i_QH_num                                      ", "write_inv_i_QH_num", &write_inv_i_QH_num, 0, NULL);

    stat_reg_counter (sdb, "recall_inv_d_S_num                                      ", "recall_inv_d_S_num", &recall_inv_d_S_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_e_S_num                                      ", "recall_inv_e_S_num", &recall_inv_e_S_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_s_S_num                                      ", "recall_inv_s_S_num", &recall_inv_s_S_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_i_S_num                                      ", "recall_inv_i_S_num", &recall_inv_i_S_num, 0, NULL);

    stat_reg_counter (sdb, "recall_inv_d_Q_num                                      ", "recall_inv_d_Q_num", &recall_inv_d_Q_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_e_Q_num                                      ", "recall_inv_e_Q_num", &recall_inv_e_Q_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_s_Q_num                                      ", "recall_inv_s_Q_num", &recall_inv_s_Q_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_i_Q_num                                      ", "recall_inv_i_Q_num", &recall_inv_i_Q_num, 0, NULL);

    stat_reg_counter (sdb, "SW_read_request_num                                     ", "SW_read_request_num", &SW_read_request_num, 0, NULL);
    stat_reg_counter (sdb, "SW_write_request_num                                    ", "SW_write_request_num", &SW_write_request_num, 0, NULL);

    stat_reg_counter (sdb, "read_S_hit_num                                          ", "read_S_hit_num", &read_S_hit_num, 0, NULL);
    stat_reg_counter (sdb, "read_Q_hit_num                                          ", "read_Q_hit_num", &read_Q_hit_num, 0, NULL);
    stat_reg_counter (sdb, "read_Q_init_num                                         ", "read_Q_init_num", &read_Q_init_num, 0, NULL);
    stat_reg_counter (sdb, "read_Q_init_SR_num                                      ", "read_Q_init_SR_num", &read_Q_init_SR_num, 0, NULL);
    stat_reg_counter (sdb, "read_Q_init_QR_num                                      ", "read_Q_init_QR_num", &read_Q_init_QR_num, 0, NULL);
                                                                                                                                  
    stat_reg_counter (sdb, "write_S_hit_num                                         ", "write_S_hit_num", &write_S_hit_num, 0, NULL);
    stat_reg_counter (sdb, "write_Q_hit_num                                         ", "write_Q_hit_num", &write_Q_hit_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_ed_num                                     ", "write_S_init_ed_num", &write_S_init_ed_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_ed_SR_num                                  ", "write_S_init_ed_SR_num", &write_S_init_ed_SR_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_ed_QR_num                                  ", "write_S_init_ed_QR_num", &write_S_init_ed_QR_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_s_num                                      ", "write_S_init_s_num", &write_S_init_s_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_s_SR_num                                   ", "write_S_init_s_SR_num", &write_S_init_s_SR_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_s_QR_num                                   ", "write_S_init_s_QR_num", &write_S_init_s_QR_num, 0, NULL);
    stat_reg_counter (sdb, "write_Q_init_num                                        ", "write_Q_init_num", &write_Q_init_num, 0, NULL);
    stat_reg_counter (sdb, "write_Q_init_SR_num                                     ", "write_Q_init_SR_num", &write_Q_init_SR_num, 0, NULL);
    stat_reg_counter (sdb, "write_Q_init_QR_num                                     ", "write_Q_init_QR_num", &write_Q_init_QR_num, 0, NULL);

    stat_reg_counter (sdb, "shr_data_req_initial_num                                ", "shr_data_req_initial_num", &shr_data_req_initial_num, 0, NULL);
    stat_reg_counter (sdb, "shr_data_req_total_num                                  ", "shr_data_req_total_num", &shr_data_req_total_num, 0, NULL);
    stat_reg_counter (sdb, "shr_data_req_fail_num                                   ", "shr_data_req_fail_num", &shr_data_req_fail_num, 0, NULL);

    stat_reg_counter (sdb, "inform_back_num                                         ", "inform_back_num", &inform_back_num, 0, NULL);
    stat_reg_counter (sdb, "write_back_num                                          ", "write_back_num", &write_back_num, 0, NULL);
    stat_reg_counter (sdb, "inform_back_Q                                           ", "inform_back_Q", &inform_back_Q, 0, NULL);
    stat_reg_counter (sdb, "inform_back_S                                           ", "inform_back_S", &inform_back_S, 0, NULL);
    stat_reg_counter (sdb, "write_back_Q                                            ", "write_back_Q", &write_back_Q, 0, NULL);
    stat_reg_counter (sdb, "write_back_S                                            ", "write_back_S", &write_back_S, 0, NULL);
    stat_reg_counter (sdb, "mb_pain_delay_num                                       ", "mb_pain_delay_num", &mb_pain_delay_num, 0, NULL);
    stat_reg_counter (sdb, "mb_pain_portuse_num                                     ", "mb_pain_portuse_num", &mb_pain_portuse_num, 0, NULL);

    stat_reg_counter (sdb, "read_Q_init_QR_s_num                                    ", "read_Q_init_QR_s_num", &read_Q_init_QR_s_num, 0, NULL);
    stat_reg_counter (sdb, "read_Q_init_QR_ed_num                                   ", "read_Q_init_QR_ed_num", &read_Q_init_QR_ed_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_ed_QR_s_num                                ", "write_S_init_ed_QR_s_num", &write_S_init_ed_QR_s_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_ed_QR_ed_num                               ", "write_S_init_ed_QR_ed_num", &write_S_init_ed_QR_ed_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_s_QR_s_num                                 ", "write_S_init_s_QR_s_num", &write_S_init_s_QR_s_num, 0, NULL);
    stat_reg_counter (sdb, "write_S_init_s_QR_ed_num                                ", "write_S_init_s_QR_ed_num", &write_S_init_s_QR_ed_num, 0, NULL);
    stat_reg_counter (sdb, "write_Q_init_QR_s_num                                   ", "write_Q_init_QR_s_num", &write_Q_init_QR_s_num, 0, NULL);
    stat_reg_counter (sdb, "write_Q_init_QR_ed_num                                  ", "write_Q_init_QR_ed_num", &write_Q_init_QR_ed_num, 0, NULL);

    stat_reg_counter (sdb, "recall_inv_d_Q_s_num                                    ", "recall_inv_d_Q_s_num", &recall_inv_d_Q_s_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_d_Q_ed_num                                   ", "recall_inv_d_Q_ed_num", &recall_inv_d_Q_ed_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_e_Q_s_num                                    ", "recall_inv_e_Q_s_num", &recall_inv_e_Q_s_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_e_Q_ed_num                                   ", "recall_inv_e_Q_ed_num", &recall_inv_e_Q_ed_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_s_Q_s_num                                    ", "recall_inv_s_Q_s_num", &recall_inv_s_Q_s_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_s_Q_ed_num                                   ", "recall_inv_s_Q_ed_num", &recall_inv_s_Q_ed_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_i_Q_s_num                                    ", "recall_inv_i_Q_s_num", &recall_inv_i_Q_s_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_i_Q_ed_num                                   ", "recall_inv_i_Q_ed_num", &recall_inv_i_Q_ed_num, 0, NULL);

    stat_reg_counter (sdb, "recall_inv_d_Q_s_ad_num                                 ", "recall_inv_d_Q_s_ad_num", &recall_inv_d_Q_s_ad_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_e_Q_s_ad_num                                 ", "recall_inv_e_Q_s_ad_num", &recall_inv_e_Q_s_ad_num, 0, NULL);
    stat_reg_counter (sdb, "recall_inv_s_Q_ed_ad_num                                ", "recall_inv_s_Q_ed_ad_num", &recall_inv_s_Q_ed_ad_num, 0, NULL);

    stat_reg_counter (sdb, "line_access_num                                         ", "line_access_num", &line_access_num, 0, NULL);
    stat_reg_counter (sdb, "SWP_line_access_num                                     ", "SWP_line_access_num", &SWP_line_access_num, 0, NULL);
    stat_reg_counter (sdb, "SWP_PR_line_access_num                                  ", "SWP_PR_line_access_num", &SWP_PR_line_access_num, 0, NULL);
    stat_reg_counter (sdb, "SWP_PW_line_access_num                                  ", "SWP_PW_line_access_num", &SWP_PW_line_access_num, 0, NULL);
    stat_reg_counter (sdb, "SWP_SR_line_access_num                                  ", "SWP_SR_line_access_num", &SWP_SR_line_access_num, 0, NULL);
    stat_reg_counter (sdb, "SWP_SW_line_access_num                                  ", "SWP_SW_line_access_num", &SWP_SW_line_access_num, 0, NULL);

    // PVC related; @ fanglei
    char PVC_stat_reg_counter[64];
    if(cache_ml2->assoc > MAX_DIRECTORY_ASSOC)
        panic("MAX_DIRECTORY_ASSOC is smaller than associativity of directory cache\n");
    stat_reg_counter (sdb, "PVC_vector_distr_record_num                             ", "PVC_vector_distr_record_num", &PVC_vector_distr_record_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_vector_inc_distr_record_num                         ", "PVC_vector_inc_distr_record_num", &PVC_vector_inc_distr_record_num, 0, NULL);
	for (i = 0; i <= cache_ml2->assoc; i++)
	{
        sprintf (PVC_stat_reg_counter, "PVC_vector_distr_%d                                     ", i);
        stat_reg_counter (sdb, PVC_stat_reg_counter, "", &PVC_vector_distr[i], /* initial value */ 0, /* format */ NULL);		
	}
	for (i = 0; i <= cache_ml2->assoc; i++)
	{
        sprintf (PVC_stat_reg_counter, "PVC_vector_inc_distr_%d                                 ", i);
        stat_reg_counter (sdb, PVC_stat_reg_counter, "", &PVC_vector_inc_distr[i], /* initial value */ 0, /* format */ NULL);		
	}
    stat_reg_counter (sdb, "PVC_exchange_num                                        ", "PVC_exchange_num", &PVC_exchange_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_flip_num                                            ", "PVC_flip_num", &PVC_flip_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_dowm_no_inv_num                                     ", "PVC_dowm_no_inv_num", &PVC_dowm_no_inv_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_dowm_inv_num                                        ", "PVC_dowm_inv_num", &PVC_dowm_inv_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_dowm_inv_shr_num                                    ", "PVC_dowm_inv_shr_num", &PVC_dowm_inv_shr_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_up_no_add_num                                       ", "PVC_up_no_add_num", &PVC_up_no_add_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_up_add_num                                          ", "PVC_up_add_num", &PVC_up_add_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_up_add_shr_num                                      ", "PVC_up_add_shr_num", &PVC_up_add_shr_num, 0, NULL);
	for (i=0;i<=(mesh_size*mesh_size);i++)
	{
        sprintf (PVC_stat_reg_counter, "PVC_exch_vec_shr_distr_%d                               ", i);
        stat_reg_counter (sdb, PVC_stat_reg_counter, "", &PVC_exch_vec_shr_distr[i], /* initial value */ 0, /* format */ NULL);
	}
	for (i=0;i<=(mesh_size*mesh_size);i++)
	{
        sprintf (PVC_stat_reg_counter, "PVC_dowm_inv_shr_distr_%d                               ", i);
        stat_reg_counter (sdb, PVC_stat_reg_counter, "", &PVC_dowm_inv_shr_distr[i], /* initial value */ 0, /* format */ NULL);
	}
	for (i=0;i<=(mesh_size*mesh_size);i++)
	{
        sprintf (PVC_stat_reg_counter, "PVC_up_add_shr_distr_%d                                 ", i);
        stat_reg_counter (sdb, PVC_stat_reg_counter, "", &PVC_up_add_shr_distr[i], /* initial value */ 0, /* format */ NULL);
	}
    stat_reg_counter (sdb, "PVC_inform_all_shr_num                                  ", "PVC_inform_all_shr_num", &PVC_inform_all_shr_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_rfs_all_shr_num                                     ", "PVC_rfs_all_shr_num", &PVC_rfs_all_shr_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_read_all_shr_num                                    ", "PVC_read_all_shr_num", &PVC_read_all_shr_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_write_all_shr_num                                   ", "PVC_write_all_shr_num", &PVC_write_all_shr_num, 0, NULL);
    stat_reg_counter (sdb, "PVC_recall_all_shr_num                                  ", "PVC_recall_all_shr_num", &PVC_recall_all_shr_num, 0, NULL);
    stat_reg_counter (sdb, "pvc_nv_conflict_num                                     ", "pvc_nv_conflict_num", &pvc_nv_conflict_num, 0, NULL);

	stat_reg_counter (sdb, "total_req_event_count", "total num of request event inital created", &totalreqcountnum, 0, NULL);
	stat_reg_counter (sdb, "StoreConditionFailNum", "Total store conditional failed num", &StoreConditionFailNum, 0, NULL);
	stat_reg_counter (sdb, "lock_network_access", "", &lock_network_access, 0, NULL);
	stat_reg_counter (sdb, "totalUpgrades", "total number of upgrades seen", &totalUpgrades, 0, NULL);
	stat_reg_counter (sdb, "total_miss_event_count", "total num of miss event intial created", &totalmisscountnum, 0, NULL);
	stat_reg_counter (sdb, "totalNetUpgrades", "total number of upgrades seen", &totalNetUpgrades, 0, NULL);
	stat_reg_counter (sdb, "totalUpgradesUsable", "total number of upgrades not usable", &totalUpgradesUsable, 0, NULL);
	stat_reg_counter (sdb, "totalUpgradesBeneficial", "total number of upgrades not usable", &totalUpgradesBeneficial, 0, NULL);
	stat_reg_counter (sdb, "totalUpgradesUse2", "total number of upgrades usable without confirmation", &totalSplitNo, 0, NULL);

	stat_reg_counter (sdb, "totalSyncdata", "total number of data packets due to sync", &data_packets_sync, 0, NULL);
	stat_reg_counter (sdb, "totalSyncmeta", "total number of meta packets due to sync", &meta_packets_sync, 0, NULL);

	stat_reg_counter (sdb, "totalNetWrites", "total number of Writes seen", &totalNetWrites, 0, NULL);
	stat_reg_counter (sdb, "totalWritesUsable", "total number of Writes not usable", &totalWritesUsable, 0, NULL);
	stat_reg_counter (sdb, "totalWritesBeneficial", "total number of Writes not usable", &totalWritesBeneficial, 0, NULL);
	stat_reg_counter (sdb, "totalWritesUse2", "total number of Writes usable without confirmation", &totalSplitWM, 0, NULL);
	stat_reg_counter (sdb, "totalNoExclusive", "total number of Writes unusable because of exclusive somewhere else", &exclusive_somewhere, 0, NULL);
	stat_reg_counter (sdb, "totalFrontWrite", "total number of Writes unusavle because of front write", &front_write, 0, NULL);

	stat_reg_counter (sdb, "totalL2WriteReqHits", "total num of write events", &totalL2WriteReqHits, 0, NULL);
	stat_reg_counter (sdb, "totalL2WriteReqNoSharer", "total num of write req with no sharers", &totalL2WriteReqNoSharer, 0, NULL);
	stat_reg_counter (sdb, "totalL2WriteReqDirty", "total  num of write req with just one sharer", &totalL2WriteReqDirty, 0, NULL);
	stat_reg_counter (sdb, "delayL2WriteReqDirty", "Extra delay to complete write req with just one sharer, in case the earlier write would had been write through", &delayL2WriteReqDirty, 0, NULL);
	stat_reg_counter (sdb, "totalL2WriteReqDirtyWT", "total times write req with just one sharer, in case the earlier write would had been write through", &totalL2WriteReqDirtyWT, 0, NULL);
	stat_reg_counter (sdb, "totalL2WriteReqShared", "total num of write req with multiple sharers", &totalL2WriteReqShared, 0, NULL);
	stat_reg_counter (sdb, "delayL2WriteReqShared", "Extra delay to complete write req with multiple sharers, in case earlier reads would had been read through", &delayL2WriteReqShared, 0, NULL);
	stat_reg_counter (sdb, "totalL2WriteReqSharedWT", "total times write req with multiple sharers, in case earlier reads would had been read through", &totalL2WriteReqSharedWT, 0, NULL);

	stat_reg_counter (sdb, "totalL2ReadReqHits", "total num of Read events", &totalL2ReadReqHits, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReadReqNoSharer", "total num of Read req with no sharers", &totalL2ReadReqNoSharer, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReadReqDirty", "total  num of Read req with just one sharer", &totalL2ReadReqDirty, 0, NULL);
	stat_reg_counter (sdb, "delayL2ReadReqDirty", "Extra delay to complete Read req with just one sharer, in case the earlier write would had been write through", &delayL2ReadReqDirty, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReadReqDirtyWT", "total times Read req with just one sharer, in case the earlier write would had been write through", &totalL2ReadReqDirtyWT, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReadReqShared", "total num of read req with multiple sharers", &totalL2ReadReqShared, 0, NULL);

	stat_reg_counter (sdb, "totalL1LinePrefUse", "total L1 line prefetches useful", &totalL1LinePrefUse, 0, NULL);
	stat_reg_counter (sdb, "totalL1LinePrefNoUse", "total L1 line prefetches not useful", &totalL1LinePrefNoUse, 0, NULL);
	stat_reg_counter (sdb, "totalL1LineReadUse", "total L1 line read misses useful", &totalL1LineReadUse, 0, NULL);
	stat_reg_counter (sdb, "totalL1LineReadNoUse", "total L1 line read misses not useful", &totalL1LineReadNoUse, 0, NULL);
	stat_reg_counter (sdb, "totalL1LineWriteUse", "total L1 line write misses useful", &totalL1LineWriteUse, 0, NULL);
	stat_reg_counter (sdb, "totalL1LineWriteNoUse", "total L1 line write misses not useful", &totalL1LineWriteNoUse, 0, NULL);

	stat_reg_counter (sdb, "totalL1LineExlUsed", "total L1 line in exclusive mode useful", &totalL1LineExlUsed, 0, NULL);
	stat_reg_counter (sdb, "totalL1LineExlInv", "total L1 line in exclusive mode invalidated without use", &totalL1LineExlInv, 0, NULL);
	stat_reg_counter (sdb, "totalL1LineExlDrop", "total L1 line in exclusive mode droped without use", &totalL1LineExlDrop, 0, NULL);

	stat_reg_counter (sdb, "totalL2ReqPrimMiss", "Total number of directory requests involve in primary miss", &totalL2ReqPrimMiss, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReqSecMiss", "Total number of directory requests involve in secondary miss", &totalL2ReqSecMiss, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReqHit", "Total number of directory requests involve in clean hit", &totalL2ReqHit, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReqInTrans", "Total number of directory requests find line in transition", &totalL2ReqInTrans, 0, NULL);
	stat_reg_counter (sdb, "totalL2OwnReqInTrans", "Total number of directory ownership requests find line in transition", &totalL2OwnReqInTrans, 0, NULL);
	stat_reg_counter (sdb, "totalL2ReqInInv", "Total number of directory requests have to invalidate", &totalL2ReqInInv, 0, NULL);

	stat_reg_counter (sdb, "delayL2ReqPrimMiss", "delay of Total number of directory requests involve in primary miss", &delayL2ReqPrimMiss, 0, NULL);
	stat_reg_counter (sdb, "delayL2ReqSecMiss", "delay of Total number of directory requests involve in secondary miss", &delayL2ReqSecMiss, 0, NULL);
	stat_reg_counter (sdb, "delayL2ReqHit", "delay of Total number of directory requests involve in clean hit", &delayL2ReqHit, 0, NULL);
	stat_reg_counter (sdb, "delayL2ReqInTrans", "delay of Total number of directory requests find line in transition", &delayL2ReqInTrans, 0, NULL);
	stat_reg_counter (sdb, "delayL2OwnReqInTrans", "delay of Total number of directory ownership requests find line in transition", &delayL2OwnReqInTrans, 0, NULL);
	stat_reg_counter (sdb, "delayL2ReqInInv", "delay of Total number of directory requests have to invalidate", &delayL2ReqInInv, 0, NULL);

	stat_reg_counter (sdb, "totalWriteReq", "Total number of write request to L2", &totalWriteReq, 0, NULL);
	stat_reg_counter (sdb, "totalWriteReqInv", "Total number of write requests with invalidations to L2", &totalWriteReqInv, 0, NULL);
	stat_reg_counter (sdb, "totalWriteDelay", "Time spent in write requests to L2", &totalWriteDelay, 0, NULL);
	stat_reg_counter (sdb, "totalWriteInvDelay", "Time spent in invalidation for write requests to L2", &totalWriteInvDelay, 0, NULL);

	stat_reg_counter (sdb, "totalUpgradeReq", "Total number of write request to L2", &totalUpgradeReq, 0, NULL);
	stat_reg_counter (sdb, "totalUpgradeReqInv", "Total number of write requests with invalidations to L2", &totalUpgradeReqInv, 0, NULL);
	stat_reg_counter (sdb, "totalUpgradeDelay", "Time spent in write requests to L2", &totalUpgradeDelay, 0, NULL);
	stat_reg_counter (sdb, "totalUpgradeInvDelay", "Time spent in invalidation for write requests to L2", &totalUpgradeInvDelay, 0, NULL);
	stat_reg_counter (sdb, "totalUpgradeDependDelay", "Time spent in depend delay for update requests to L2", &UpdateDependCycle, 0, NULL);

	stat_reg_counter (sdb, "countNonAllocReadEarly", "Number of times a read miss request from L1 has been serviced early without allocation", &countNonAllocReadEarly, 0, NULL);
#ifdef TG
	stat_reg_counter (sdb, "total_packets_generation", "total packets generated by traffic generator", &total_packets_generation, 0, NULL);
	stat_reg_counter (sdb, "total_read_packets_generation", "total packets generated by traffic generator", &total_read_packets_generation, 0, NULL);
	stat_reg_counter (sdb, "total_write_packets_generation", "total packets generated by traffic generator", &total_write_packets_generation, 0, NULL);
#endif
	stat_reg_counter (sdb, "WM_Miss", "", &WM_Miss, 0, NULL);
	stat_reg_counter (sdb, "WM_Clean", "", &WM_Clean, 0, NULL);
	stat_reg_counter (sdb, "WM_S", "", &WM_S, 0, NULL);
	stat_reg_counter (sdb, "WM_EM", "", &WM_EM, 0, NULL);
	stat_reg_counter (sdb, "WM_ShardUseConf", "", &write_shared_used_conf, 0, NULL);

	stat_reg_counter (sdb, "Sync_L2_miss", "", &Sync_L2_miss, 0, NULL);
	stat_reg_counter (sdb, "SyncInstCacheAccess", "", &SyncInstCacheAccess, 0, NULL);
	stat_reg_counter (sdb, "TestCacheAccess", "", &TestCacheAccess, 0, NULL);
	stat_reg_counter (sdb, "TestSecCacheAccess", "", &TestSecCacheAccess, 0, NULL);
	stat_reg_counter (sdb, "SetCacheAccess", "", &SetCacheAccess, 0, NULL);
	stat_reg_counter (sdb, "SyncLoadReadMiss", "", &SyncLoadReadMiss, 0, NULL);
	stat_reg_counter (sdb, "SyncLoadLReadMiss", "", &SyncLoadLReadMiss, 0, NULL);
	stat_reg_counter (sdb, "SyncLoadHit", "", &SyncLoadHit, 0, NULL);
	stat_reg_counter (sdb, "SyncLoadLHit", "", &SyncLoadLHit, 0, NULL);
	stat_reg_counter (sdb, "SyncStoreCHit", "", &SyncStoreCHit, 0, NULL);
	stat_reg_counter (sdb, "SyncStoreCWriteMiss", "", &SyncStoreCWriteMiss, 0, NULL);
	stat_reg_counter (sdb, "SyncStoreCWriteUpgrade", "", &SyncStoreCWriteUpgrade, 0, NULL);
	stat_reg_counter (sdb, "SyncStoreHit", "", &SyncStoreHit, 0, NULL);
	stat_reg_counter (sdb, "SyncStoreWriteMiss", "", &SyncStoreWriteMiss, 0, NULL);
	stat_reg_counter (sdb, "SyncStoreWriteUpgrade", "", &SyncStoreWriteUpgrade, 0, NULL);
	stat_reg_counter (sdb, "BarStoreWriteMiss", "", &BarStoreWriteMiss, 0, NULL);
	stat_reg_counter (sdb, "BarStoreWriteUpgrade", "", &BarStoreWriteUpgrade, 0, NULL);
	stat_reg_formula (sdb, "test_hit_ratio", "", "SyncLoadHit / TestCacheAccess", /* format */ NULL);
	stat_reg_formula (sdb, "test_sec_hit_ratio", "", "SyncLoadLHit / TestSecCacheAccess", /* format */ NULL);
	stat_reg_formula (sdb, "set_hit_ratio", "", "SyncStoreCHit / SetCacheAccess", /* format */ NULL);

	stat_reg_counter (sdb, "lock_fifo_full", "total number of lock fifo overflow", &lock_fifo_full, 0, NULL);
	stat_reg_counter (sdb, "lock_fifo_wrong", "total number of lock fifo overflow", &lock_fifo_wrong, 0, NULL);
	stat_reg_counter (sdb, "lock_fifo_writeback", "total number of lock fifo overflow", &lock_fifo_writeback, 0, NULL);
	stat_reg_counter (sdb, "lock_fifo_benefit", "total number of lock fifo benefit", &lock_fifo_benefit, 0, NULL);
	stat_reg_counter (sdb, "lock_cache_hit", "total number of lock fifo benefit", &lock_cache_hit, 0, NULL);
	stat_reg_counter (sdb, "lock_cache_miss", "total number of lock fifo benefit", &lock_cache_miss, 0, NULL);
	stat_reg_counter (sdb, "L1_mshr_full", "total number of L1 mshr overflow", &L1_mshr_full, 0, NULL);
	stat_reg_counter (sdb, "Stall_L1_mshr", "total stall cycles for L1 mshr overflow", &Stall_L1_mshr, 0, NULL);
	stat_reg_counter (sdb, "L2_mshr_full", "total number of L2 mshr overflow", &L2_mshr_full, 0, NULL);
	stat_reg_counter (sdb, "Stall_L2_mshr", "total stall cycles for L2 mshr overflow", &Stall_L2_mshr, 0, NULL);
	stat_reg_counter (sdb, "L2_mshr_full_prefetch", "total number of L2 mshr overflow when prefetch", &L2_mshr_full_prefetch, 0, NULL);
	stat_reg_counter (sdb, "L1_fifo_full", "total number of L1 fifo overflow", &L1_fifo_full, 0, NULL);
	stat_reg_counter (sdb, "Stall_L1_fifo", "total stall cycles for L1 fifo overflow", &Stall_L1_fifo, 0, NULL);
	stat_reg_counter (sdb, "Dir_fifo_full", "total number of dir fifo overflow", &Dir_fifo_full, 0, NULL);
	stat_reg_counter (sdb, "Stall_dir_fifo", "total stall cycles for dir fifo overflow", &Stall_dir_fifo, 0, NULL);
	stat_reg_counter (sdb, "Input_queue_full", "total number of input queue overflow", &Input_queue_full, 0, NULL);
	stat_reg_counter (sdb, "Output_queue_full", "total number of output queue overflow", &Output_queue_full, 0, NULL);
	stat_reg_counter (sdb, "Stall_input_queue", "total stall cycles due to input queue overflow", &Stall_input_queue, 0, NULL);
	stat_reg_counter (sdb, "Stall_output_queue", "total stall cycles due to output queue overflow", &Stall_output_queue, 0, NULL);
	stat_reg_counter (sdb, "total_load_link_shared", "total load link shared", &load_link_shared, 0, NULL);
	stat_reg_counter (sdb, "total_load_link_exclusive", "total load link shared", &load_link_exclusive, 0, NULL);
	stat_reg_counter (sdb, "total_normal_miss_handle_time", "total time of handling the missing", &totalmisstimeforNormal, 0, NULL);
	stat_reg_counter (sdb, "total_sync_miss_handle_time", "total time of handling the missing", &totalmisstimeforSync, 0, NULL);
	stat_reg_counter (sdb, "total_L2_miss_handle_time", "total time of handling the missing", &totalL2misstime, 0, NULL);
	stat_reg_counter (sdb, "total_Wrong_L2_miss_handle_time", "total time of handling the missing", &totalWrongL2misstime, 0, NULL);
	stat_reg_counter (sdb, "total_L2_misses", "total time of handling the missing", &TotalL2misses, 0, NULL);
	stat_reg_counter (sdb, "total_mem_lat", "total time of handling the missing", &total_mem_lat, 0, NULL);
	stat_reg_counter (sdb, "total_mem_access", "total time of handling the missing", &total_mem_access, 0, NULL);
	stat_reg_counter (sdb, "total_miss_handle_time", "total time of handling the missing", &totalmisshandletime, 0, NULL);
	stat_reg_counter (sdb, "total_nack_count", "total num of nack messages sending", &nack_counter, 0, NULL);
	stat_reg_counter (sdb, "total_prefetch_nack_count", "total num of nack messages sending", &prefetch_nacks, 0, NULL);
	stat_reg_counter (sdb, "total_normal_nack_count", "total num of nack messages sending", &normal_nacks, 0, NULL);
	stat_reg_counter (sdb, "total_write_nack_count", "total num of nack messages sending", &write_nacks, 0, NULL);
	stat_reg_counter (sdb, "total_sync_nack_count", "total num of nack messages sending", &sync_nacks, 0, NULL);
	stat_reg_counter (sdb, "total_sc_fail_count", "total num of store conditional fail", &store_conditional_failed, 0, NULL);
	stat_reg_counter (sdb, "total_flip_count", "total num of flipped messages", &flip_counter, 0, NULL);
	stat_reg_counter (sdb, "total_L1_flip_count", "total num of flipped messages", &L1_flip_counter, 0, NULL);
	stat_reg_counter (sdb, "total_data_reply_benefit", "total num of data reply benefit", &e_to_m, 0, NULL);
	stat_reg_formula (sdb, "average_normal_miss_handle_time", "cycles per every miss and invalidation", "total_normal_miss_handle_time / total_normal_event_count", /* format */ NULL);
	stat_reg_formula (sdb, "average_L2_miss_handle_time", "cycles per every miss and invalidation", "total_L2_miss_handle_time / total_normal_event_count", /* format */ NULL);
	stat_reg_formula (sdb, "average_sync_miss_handle_time", "cycles per every miss and invalidation", "total_sync_miss_handle_time / total_sync_event_count", /* format */ NULL);
	stat_reg_formula (sdb, "average_miss_handle_time", "cycles per every miss and invalidation", "total_miss_handle_time / total_event_count", /* format */ NULL);
	stat_reg_counter (sdb, "total_mesh_network_access", "total mesh_network access", &totalMeshHopCount, 0, NULL);
	stat_reg_counter (sdb, "total_mesh_network_delay", "total mesh_network delay", &totalMeshHopDelay, 0, NULL);
	stat_reg_formula (sdb, "average_mesh_network_time", "cycles per mesh_network access", "total_mesh_network_delay / total_mesh_network_access", /* format */ NULL);
	stat_reg_counter (sdb, "total_normal_mesh_network_access", "total mesh_network access", &totalMeshNorHopCount, 0, NULL);
	stat_reg_counter (sdb, "total_normal_mesh_network_delay", "total mesh_network delay", &totalMeshNorHopDelay, 0, NULL);
	stat_reg_formula (sdb, "average_normal_mesh_network_time", "cycles per mesh_network access", "total_noraml_mesh_network_delay / total_noraml_mesh_network_access", /* format */ NULL);
	stat_reg_counter (sdb, "total_sync_mesh_network_access", "total mesh_network access", &totalMeshSyncHopCount, 0, NULL);
	stat_reg_counter (sdb, "total_sync_mesh_network_delay", "total mesh_network delay", &totalMeshSyncHopDelay, 0, NULL);
	stat_reg_formula (sdb, "average_sync_mesh_network_time", "cycles per mesh_network access", "total_sync_mesh_network_delay / total_sync_mesh_network_access", /* format */ NULL);
	stat_reg_counter (sdb, "total_opt_network_access", "total opt_network access", &totalOptHopCount, 0, NULL);
	stat_reg_counter (sdb, "total_opt_network_delay", "total opt_network delay", &totalOptHopDelay, 0, NULL);
	stat_reg_formula (sdb, "average_opt_network_time", "cycles per opt_network access", "total_opt_network_delay / total_opt_network_access", /* format */ NULL);
	stat_reg_counter (sdb, "total_normal_opt_network_access", "total opt_network access", &totalOptNorHopCount, 0, NULL);
	stat_reg_counter (sdb, "total_normal_opt_network_delay", "total opt_network delay", &totalOptNorHopDelay, 0, NULL);
	stat_reg_formula (sdb, "average_normal_opt_network_time", "cycles per opt_network access", "total_normal_opt_network_delay / total_normal_opt_network_access", /* format */ NULL);
	stat_reg_counter (sdb, "total_sync_opt_network_access", "total opt_network access", &totalOptSyncHopCount, 0, NULL);
	stat_reg_counter (sdb, "total_sync_opt_network_delay", "total opt_network delay", &totalOptSyncHopDelay, 0, NULL);
	stat_reg_formula (sdb, "average_sync_opt_network_time", "cycles per opt_network access", "total_sync_opt_network_delay / total_sync_opt_network_access", /* format */ NULL);
	stat_reg_counter (sdb, "total_local_cache_access", "total local dl2 cache access number", &local_cache_access, 0, NULL);
	stat_reg_counter (sdb, "total_remote_cache_access", "total remote dl2 cache access number", &remote_cache_access, 0, NULL);
	stat_reg_counter (sdb, "total_two_hops_involve_wb", "total two hops involves due to write back", &involve_2_hop_wb, 0, NULL);
	stat_reg_counter (sdb, "total_two_hops_involve_touch", "total two hops involves due to touch", &involve_2_hop_touch, 0, NULL);
	stat_reg_counter (sdb, "total_two_hops_involves", "total two hops involves", &involve_2_hops, 0, NULL);
	stat_reg_counter (sdb, "total_four_hops_involves", "total four hops involves", &involve_4_hops, 0, NULL);
	stat_reg_counter (sdb, "total_two_hops_involves_wm", "total two hops involves", &involve_2_hops_wm, 0, NULL);
	stat_reg_counter (sdb, "total_four_hops_involves_wm", "total four hops involves", &involve_4_hops_wm, 0, NULL);
	stat_reg_counter (sdb, "total_two_hops_involve_miss", "total two hops involves", &involve_2_hops_miss, 0, NULL);
	stat_reg_counter (sdb, "total_four_hops_involve_miss", "total four hops involves", &involve_4_hops_miss, 0, NULL);
	stat_reg_counter (sdb, "total_four_hops_involve_upgrade", "total four hops involves", &involve_4_hops_upgrade, 0, NULL);
	stat_reg_counter (sdb, "total_two_hops_involve_upgrade", "total two hops involves", &involve_2_hops_upgrade, 0, NULL);
	stat_reg_counter (sdb, "data_private_read", "total private data for read miss", &data_private_read, 0, NULL);
	stat_reg_counter (sdb, "data_private_write", "total private data for write miss", &data_private_write, 0, NULL);
	stat_reg_counter (sdb, "data_shared_read", "total shared data for read miss", &data_shared_read, 0, NULL);
	stat_reg_counter (sdb, "data_shared_write", "total shared data for write miss", &data_shared_write, 0, NULL);
	stat_reg_counter (sdb, "total_all_close", "total numbers of all closed consumers", &total_all_close, 0, NULL);
	stat_reg_counter (sdb, "total_not_all_close", "total numbers of not all closed consumers", &total_not_all_close, 0, NULL);
	stat_reg_counter (sdb, "total_p_c_events", "total producer and consumers events (should equal to 4 hops)", &total_p_c_events, 0, NULL);
	stat_reg_counter (sdb, "total_consumers", "total numbers of consumers", &total_consumers, 0, NULL);
	stat_reg_counter (sdb, "total_packets_in_neighbor", "total packets are in neighborhood", &total_packets_in_neighbor, 0, NULL);
	stat_reg_counter (sdb, "total_packets_at_corners", "total packets are at corners", &total_packets_at_corners, 0, NULL);
	stat_reg_counter (sdb, "total_all_almostclose", "total numbers of all closed and corner consumers", &total_all_almostclose, 0, NULL);
	stat_reg_counter (sdb, "total_data_consumers", "total data packets of consumers", &total_data_consumers, 0, NULL);
	stat_reg_counter (sdb, "total_data_at_corner", "total data packets supply from at corners", &total_data_at_corner, 0, NULL);
	stat_reg_counter (sdb, "total_data_close", "total data packets are supplied from closeby sources", &total_data_close, 0, NULL);
	stat_reg_counter (sdb, "total_data_far", "total data packets are sumpplied from far sources", &total_data_far, 0, NULL);
	stat_reg_double (sdb, "average_inside_percent", "average inside packets percent(should be divided by total_not_all_close)", &average_inside_percent, 0, NULL);
	stat_reg_double (sdb, "average_outside_percent", "average outside packets percent(should be divided by total_not_all_close)", &average_outside_percent, 0, NULL);
	stat_reg_double (sdb, "average_outside_abs_percent", "average absolutly outside packets percent(far - corner)", &average_outside_abs_percent, 0, NULL);
	stat_reg_double (sdb, "average_corner_percent", "average outside packets percent(corner: should be divided by total_not_all_clos)", &average_corner_percent, 0, NULL);
	stat_reg_counter (sdb, "total_far_packets", "", &total_far_packets, 0, NULL);
	stat_reg_counter (sdb, "total_corner_packets", "", &total_corner_packets, 0, NULL);
	stat_reg_counter (sdb, "total_neighbor_packets", "", &total_neighbor_packets, 0, NULL);
	stat_reg_double (sdb, "pending_invalid_cycles", "how many cycles to invalidate the cache line", &pending_invalid_cycles, 0, NULL);

	//for (i=0;i<261;i++)
	//{
	//	sprintf (ch2, "ReplySpand_%d", i);
	//	stat_reg_counter (sdb, ch2, "", &req_spand[i], 0, /* format */ NULL);
	//}
	stat_reg_counter (sdb, "total_down_r", "", &downgrade_r, 0, NULL);
	stat_reg_counter (sdb, "total_down_w", "", &downgrade_w, 0, NULL);
	for (i=0;i<5;i++)
	{
		sprintf (ch2, "down_spand_%d", i);
		stat_reg_counter (sdb, ch2, "", &spand[i], 0, /* format */ NULL);
	}

	stat_reg_counter (sdb, "total_private_counts", "", &private_count, 0, NULL);
	stat_reg_counter (sdb, "total_migration_counts", "", &migrate_count, 0, NULL);
	stat_reg_counter (sdb, "total_shared_counts", "", &shared_count, 0, NULL);
	stat_reg_counter (sdb, "total_shared_rw_counts", "", &shared_rw_count, 0, NULL);
	stat_reg_counter (sdb, "total_private_dynamic_counts", "", &private_dynamic_accesses, 0, NULL);
	stat_reg_counter (sdb, "total_migrate_dynamic_counts", "", &migrate_dynamic_accesses, 0, NULL);
	stat_reg_counter (sdb, "total_shared_dynamic_counts", "", &shared_dynamic_accesses, 0, NULL);
	stat_reg_counter (sdb, "total_srw_dynamic_counts", "", &sharedrw_dynamic_accesses, 0, NULL);
	stat_reg_counter (sdb, "total_inflight_private", "", &inflight_private, 0, NULL);
	stat_reg_counter (sdb, "total_inflight_migrate", "", &inflight_migrate, 0, NULL);
	stat_reg_counter (sdb, "total_inflight_shared_only", "", &inflight_shared_only, 0, NULL);
	stat_reg_counter (sdb, "total_inflight_shared_rw", "", &inflight_shared_rw, 0, NULL);
	stat_reg_counter (sdb, "total_private_others", "", &private_others, 0, NULL);
	stat_reg_counter (sdb, "total_type_changed", "", &type_changed, 0, NULL);
	stat_reg_counter (sdb, "total_line_accesses", "", &line_accesses, 0, NULL);
	stat_reg_counter (sdb, "total_inflight_accesses", "", &line_inflight_accesses, 0, NULL);
	stat_reg_counter (sdb, "total_p_to_srw", "", &private_to_shared_rw, 0, NULL);
	stat_reg_counter (sdb, "total_srw_to_p", "", &shared_rw_to_private, 0, NULL);
	stat_reg_counter (sdb, "total_srw_constatnt", "", &shared_rw_constant, 0, NULL);

//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_dynamic_acc_private%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_dynamic_acc_private[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_dynamic_acc_migrate%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_dynamic_acc_migrate[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_dynamic_acc_shared%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_dynamic_acc_shared[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_dynamic_acc_srw%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_dynamic_acc_srw[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_acc_srw_pri_r%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_acc_srw_pri_r[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_acc_srw_pri_w%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_acc_srw_pri_w[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_acc_srw_r%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_acc_srw_r[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_acc_srw_w%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_acc_srw_w[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_acc_srw_private%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_acc_srw_private[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_acc_srw_srw_%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_acc_srw_srw[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_access_%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_access[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "word_utilization_%d", i);
//		stat_reg_counter (sdb, ch2, "", &word_utilization[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_access_p_%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_access_p[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "word_utilization_p_%d", i);
//		stat_reg_counter (sdb, ch2, "", &word_utilization_p[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_access_s_%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_access_s[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "word_utilization_s_%d", i);
//		stat_reg_counter (sdb, ch2, "", &word_utilization_s[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_access_m_%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_access_m[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "word_utilization_m_%d", i);
//		stat_reg_counter (sdb, ch2, "", &word_utilization_m[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "Cache_access_srw_%d", i);
//		stat_reg_counter (sdb, ch2, "", &Cache_access_srw[i], 0, /* format */ NULL);
//	}
//	for (i=0;i<101;i++)
//	{
//		sprintf (ch2, "word_utilization_srw_%d", i);
//		stat_reg_counter (sdb, ch2, "", &word_utilization_srw[i], 0, /* format */ NULL);
//	}
//	for (i = 0; i < 16; i++)
//	{
//		char ch2[20];
//		sprintf (ch2, "port_%d", i);
//		stat_reg_counter (sdb, ch2, "", &port_accord[i], /* initial value */ 0, /* format */ NULL);
//	}
//	for (i = 0; i < MAXTHREADS; i++)
//	{
//		char ch2[20];
//		sprintf (ch2, "sharer_distr_%d", i);
//		stat_reg_counter (sdb, ch2, "", &sharer_distr[i], /* initial value */ 0, /* format */ NULL);
//	}
//	for (i = 0; i < 100; i++)
//	{
//		for (j = 0; j < 100; j++)
//		{
//			sprintf (ch2, "data_packets_sending_%d_%d", i, j);
//			stat_reg_counter (sdb, ch2, "", &data_packets_sending[i][j], /* initial value */ 0, /* format */ NULL);
//		}
//	}
//	for (i = 0; i < 100; i++)
//	{
//		for (j = 0; j < 100; j++)
//		{
//			sprintf (ch2, "meta_packets_sending_%d_%d", i, j);
//			stat_reg_counter (sdb, ch2, "", &meta_packets_sending[i][j], /* initial value */ 0, /* format */ NULL);
//		}
//	}

//	for (i = 0; i < 36; i++)
//	{
//		sprintf (ch2, "meta_even_close_%d", i);
//		stat_reg_counter (sdb, ch2, "", &meta_even_req_close[i], /* initial value */ 0, /* format */ NULL);
//	}
//	for (i = 0; i < 36; i++)
//	{
//		sprintf (ch2, "meta_odd_close_%d", i);
//		stat_reg_counter (sdb, ch2, "", &meta_odd_req_close[i], /* initial value */ 0, /* format */ NULL);
//	}
//	for (i = 0; i < numcontexts; i++)
//	{
//		sprintf (ch2, "sharer_bincount_%d", i);
//		stat_reg_counter (sdb, ch2, "", &sharer_num[i], /* initial value */ 0, /* format */ NULL);
//	}
	optical_reg_stats (sdb);
	/* occupancy stats */
	stat_reg_counter (sdb, "IFQ_count", "cumulative IFQ occupancy", &IFQ_count, /* initial value */ 0, /* format */ NULL);
	stat_reg_counter (sdb, "IFQ_fcount", "cumulative IFQ full count", &IFQ_fcount, /* initial value */ 0, /* format */ NULL);
	stat_reg_formula (sdb, "ifq_occupancy", "avg IFQ occupancy (insn's)", "IFQ_count / sim_cycle", /* format */ NULL);
	stat_reg_formula (sdb, "ifq_rate", "avg IFQ dispatch rate (insn/cycle)", "sim_total_insn / sim_cycle", /* format */ NULL);
	stat_reg_formula (sdb, "ifq_latency", "avg IFQ occupant latency (cycle's)", "ifq_occupancy / ifq_rate", /* format */ NULL);
	stat_reg_formula (sdb, "ifq_full", "fraction of time (cycle's) IFQ was full", "IFQ_fcount / sim_cycle", /* format */ NULL);

	stat_reg_counter (sdb, "RUU_count", "cumulative RUU occupancy", &RUU_count, /* initial value */ 0, /* format */ NULL);
	stat_reg_counter (sdb, "RUU_fcount", "cumulative RUU full count", &RUU_fcount, /* initial value */ 0, /* format */ NULL);
	stat_reg_formula (sdb, "ruu_occupancy", "avg RUU occupancy (insn's)", "RUU_count / sim_cycle", /* format */ NULL);
	stat_reg_formula (sdb, "ruu_rate", "avg RUU dispatch rate (insn/cycle)", "sim_total_insn / sim_cycle", /* format */ NULL);
	stat_reg_formula (sdb, "ruu_latency", "avg RUU occupant latency (cycle's)", "ruu_occupancy / ruu_rate", /* format */ NULL);
	stat_reg_formula (sdb, "ruu_full", "fraction of time (cycle's) RUU was full", "RUU_fcount / sim_cycle", /* format */ NULL);

	stat_reg_counter (sdb, "LSQ_count", "cumulative LSQ occupancy", &LSQ_count, /* initial value */ 0, /* format */ NULL);
	stat_reg_counter (sdb, "LSQ_fcount", "cumulative LSQ full count", &LSQ_fcount, /* initial value */ 0, /* format */ NULL);
	stat_reg_formula (sdb, "lsq_occupancy", "avg LSQ occupancy (insn's)", "LSQ_count / sim_cycle", /* format */ NULL);
	stat_reg_formula (sdb, "lsq_rate", "avg LSQ dispatch rate (insn/cycle)", "sim_total_insn / sim_cycle", /* format */ NULL);
	stat_reg_formula (sdb, "lsq_latency", "avg LSQ occupant latency (cycle's)", "lsq_occupancy / lsq_rate", /* format */ NULL);
	stat_reg_formula (sdb, "lsq_full", "fraction of time (cycle's) LSQ was full", "LSQ_fcount / sim_cycle", /* format */ NULL);
	stat_reg_counter (sdb, "dl2ActuallyAccessed", "", &dl2ActuallyAccessed, /* initial value */ 0, /* format */ NULL);

	stat_reg_counter (sdb, "prefetchLoad", "", &m_sqPrefetchCnt, /* initial value */ 0, /* format */ NULL);

	for (i = 0; i < numcontexts; i++)
	{
		char ch[20], fm[30], ch2[20];

		sprintf (ch2, "brRecovery_%d", i);
		stat_reg_counter (sdb, ch2, "total wrong branches", &brRecovery[i], /* initial value */ 0, /* format */ NULL);

		sprintf (ch2, "extraInsn_%d", i);
		stat_reg_counter (sdb, "extraInsn", "total number of wrong path insn executed", &extraInsn[i], /* initial value */ 0, /* format */ NULL);
	}


	for (i = 0; i < THREADS_PER_JOB; i++)
	{
		char ch2[20];

	}

	for (i = 0; i < MAXTHREADS; i++)
	{
		char ch2[20];

		/*RN: 10.19.06 */
		sprintf (ch2, "thread_sim_insn_%d", i);
		stat_reg_counter (sdb, ch2, "per thread sim num insn", &simInsn[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "thread_pure_insn_%d", i);
		stat_reg_counter (sdb, ch2, "per thread pure num insn", &pureInsn[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "thread_running_cycle_%d", i);
		stat_reg_counter (sdb, ch2, "per thread running num cycle", &runningCycle[i], /* initial value */ 0, /* format */ NULL);	
		sprintf (ch2, "StallCount_%d", i);
		stat_reg_counter (sdb, ch2, "total stall count", &sleepCount[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "quienseWakeup_%d", i);
		stat_reg_counter (sdb, ch2, "", &quienseWakeup[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "seqConsReplay_%d", i);
		stat_reg_counter (sdb, ch2, "", &seqConsReplay[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "seqConsInsnReplay_%d", i);
		stat_reg_counter (sdb, ch2, "", &seqConsInsnReplay[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "ldstRejectionCount_%d", i);
		stat_reg_counter (sdb, ch2, "", &ldstRejectionCount[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "TrueReplay_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_sqNumLdReplay[i], /* initial value */ 0, /* format */ NULL);
		//#ifdef SEQUENTIAL_CONSISTENCY
		sprintf (ch2, "InvalidationReplay_%d", i);
		stat_reg_counter (sdb, ch2, "", &invalidation_replay[i], /* initial value */ 0, /* format */ NULL);
		//#endif
#ifdef RELAXED_CONSISTENCY
		sprintf (ch2, "loadLoadReplay_%d", i);
		stat_reg_counter (sdb, ch2, "", &loadLoadReplayCount[i], /* initial value */ 0, /* format */ NULL);
#endif
		sprintf (ch2, "avgRUUsize_%d", i);
		stat_reg_double (sdb, ch2, "", &avgRUUsize[i], 0, /* format */ NULL);
		sprintf (ch2, "avgLQsize_%d", i);
		stat_reg_double (sdb, ch2, "", &avgLQsize[i], 0, /* format */ NULL);
		sprintf (ch2, "avgSQsize_%d", i);
		stat_reg_double (sdb, ch2, "", &avgSQsize[i], 0, /* format */ NULL);
		sprintf (ch2, "CommitStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_commitStall[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "TotalStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_totalStall[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "RUUStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_RUUFull[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "LQStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_sqLQFull[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "SQStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_sqSQFull[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "StoreMissStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_StoreMiss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "LQHitStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_LoadRealHit[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "LQMissStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_LoadRealMiss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "SQHitStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_StoreRealHit[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "SQMissStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_StoreRealMiss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "StoreStallMiss_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_StoreStallMiss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "StoreStallHit_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_StoreStallHit[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "LoadStallMiss_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_LoadStallMiss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "LoadStallHit_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_LoadStallHit[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "StoreUpdateMissStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_StoreUpdateMiss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "LoadMissStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_LoadMiss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "RegStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_regFull[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "IssueStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_issueFull[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "Il1missStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_il1Miss[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "FetchStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_fetchEmpty[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "STLCStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_STLC[i], /* initial value */ 0, /* format */ NULL);
		sprintf (ch2, "TrapStall_%d", i);
		stat_reg_counter (sdb, ch2, "", &m_trap[i], /* initial value */ 0, /* format */ NULL);
	}

#ifdef LOAD_PREDICTOR
	/* register predictor stats */
	if (ldpred)
		bpred_reg_stats (ldpred, sdb, 0);
#endif

	/* register predictor stats */
	for (i = 0; i < numcontexts; i++)
	{
		ld_reg_stats (sdb, i);
		mem_reg_stats (thecontexts[i]->mem, sdb);
	}

	for (i = 0; i < MAXTHREADS; i++)
	{
		if (cache_dl1[i])
			cache_reg_stats (cache_dl1[i], sdb, i);
		if (cache_il1[i])
			cache_reg_stats (cache_il1[i], sdb, i);
		if (itlb[i])
			cache_reg_stats (itlb[i], sdb, i);
		if (dtlb[i])
			cache_reg_stats (dtlb[i], sdb, i);
	}

	if (cache_ml2)
		cache_reg_stats (cache_ml2, sdb, 0);

	if (cache_dl2)
		cache_reg_stats (cache_dl2, sdb, 0);

	/* register power stats */
	power_reg_stats (sdb);

	/* debug variable(s) */
	stat_reg_counter (sdb, "sim_invalid_addrs", "total non-speculative bogus addresses seen (debug var)", &sim_invalid_addrs, /* initial value */ 0, /* format */ NULL);

	for (i = 0; i < pcstat_nelt; i++)
	{
		char buf[512], buf1[512];
		struct stat_stat_t *stat;

		/* track the named statistical variable by text address */
		/* find it... */
		stat = stat_find_stat (sdb, pcstat_vars[i]);
		if (!stat)
			fatal ("cannot locate any statistic named `%s'", pcstat_vars[i]);

		/* stat must be an integral type */
		if (stat->sc != sc_int && stat->sc != sc_uint && stat->sc != sc_counter)
			fatal ("`-pcstat' statistical variable `%s' is not an integral type", stat->name);

		/* register this stat */
		pcstat_stats[i] = stat;
		pcstat_lastvals[i] = STATVAL (stat);

		/* declare the sparce text distribution */
		sprintf (buf, "%s_by_pc", stat->name);
		sprintf (buf1, "%s (by text address)", stat->desc);
		pcstat_sdists[i] = stat_reg_sdist (sdb, buf, buf1,
				/* initial value */ 0,
				/* print format */ (PF_COUNT | PF_PDF),
				/* format */ "0x%lx %lu %.2f",
				/* print fn */ NULL);
	}


#ifdef TOKENB
	if (tokenB)
		tc_reg_stats (sdb);
#endif
	cluster_reg_stats (sdb);
}

/* forward declarations */
void lsq_init (int);
static void rslink_init (int nlinks);
static void rs_cache_init (void);
static void eventq_init (void);
static void readyq_init (void);
void cv_init (int threadid);
void ruu_init (int threadid);
static void tracer_init (int threadid);
void fetch_init (int threadid);

static void dir_eventq_init(void);   //directory event queue version

/* initialize the simulator */
	void
sim_init (void)
{
	int i;

	sim_num_refs = 0;

	if (!mystricmp (pred_type, "bimod"))
	{
		/* bimodal predictor, bpred_create() checks BTB_SIZE */
		if (bimod_nelt != 1)
			fatal ("bad bimod predictor config (<table_size>)");
		if (btb_nelt != 2)
			fatal ("bad btb config (<num_sets> <associativity>)");

		/* bimodal predictor, bpred_create() checks BTB_SIZE */
		for (i = 0; i < numcontexts; i++)
		{
			thecontexts[i]->pred = bpred_create (BPred2bit,
					/* bimod table size */ bimod_config[0],
					/* 2lev l1 size */ 0,
					/* 2lev l2 size */ 0,
					/* meta table size */ 0,
					/* history reg size */ 0,
					/* history xor address */ 0,
					/* btb sets */ btb_config[0],
					/* btb assoc */ btb_config[1],
					/* ret-addr stack size */ ras_size);
		}
	}
	else if (!mystricmp (pred_type, "2lev"))
	{
		/* 2-level adaptive predictor, bpred_create() checks args */
		if (twolev_nelt != 4)
			fatal ("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if (btb_nelt != 2)
			fatal ("bad btb config (<num_sets> <associativity>)");

		for (i = 0; i < numcontexts; i++)
		{
			thecontexts[i]->pred = bpred_create (BPred2Level,
					/* bimod table size */ 0,
					/* 2lev l1 size */ twolev_config[0],
					/* 2lev l2 size */ twolev_config[1],
					/* meta table size */ 0,
					/* history reg size */ twolev_config[2],
					/* history xor address */ twolev_config[3],
					/* btb sets */ btb_config[0],
					/* btb assoc */ btb_config[1],
					/* ret-addr stack size */ ras_size);
		}
	}
	else if (!mystricmp (pred_type, "comb"))
	{
		/* combining predictor, bpred_create() checks args */
		if (twolev_nelt != 4)
			fatal ("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if (bimod_nelt != 1)
			fatal ("bad bimod predictor config (<table_size>)");
		if (comb_nelt != 1)
			fatal ("bad combining predictor config (<meta_table_size>)");
		if (btb_nelt != 2)
			fatal ("bad btb config (<num_sets> <associativity>)");

		for (i = 0; i < numcontexts; i++)
		{
			thecontexts[i]->pred = bpred_create (BPredComb,
					/* bimod table size */ bimod_config[0],
					/* l1 size */ twolev_config[0],
					/* l2 size */ twolev_config[1],
					/* meta table size */ comb_config[0],
					/* history reg size */ twolev_config[2],
					/* history xor address */ twolev_config[3],
					/* btb sets */ btb_config[0],
					/* btb assoc */ btb_config[1],
					/* ret-addr stack size */ ras_size);
		}
	}

#ifdef  PWR_DE_CENTRALIZED_LSQ
#ifndef CENTRALIZED_CACHE_BANK
	if (res_membank < CLUSTERS)
	{
		fprintf (stderr, "Impossible configuration, for PWR_DE_CENTRALIZED_LSQ res_membank must equal CLUSTERS");
		fprintf (stderr, " res_membank %d <  CLUSTERS  %d", res_membank, CLUSTERS);
		exit (1);
	}

	if (res_membank != CLUSTERS)
	{
		fprintf (stderr, "Impossible configuration, for PWR_DE_CENTRALIZED_LSQ res_membank must equal CLUSTERS");
		fprintf (stderr, " res_membank %d !=  CLUSTERS  %d", res_membank, CLUSTERS);
		exit (1);
	}
#endif // CENTRALIZED_CACHE_BANK
#endif

#ifdef PORT_PER_BANK
	CACHEPORTS = RES_CACHEPORTS = RES_CACHE_PORTS = res_membank;
	res_memport = res_mem_port_bank * res_membank;
#endif

#ifdef ISSUE_PER_CLUSTER
	/*  ruu_issue_width = ruu_issue_width/CLUSTERS; */
	/* I'd rather give the number per cluster, not total in the config files. */
#endif

	/* compute static power estimates */
	
	for (i = 0; i < numcontexts; i++)
	{				
		
		

		if (i == 0)
		{
			thecontexts[i]->power.Powerfactor = (Mhz) * Vdd * Vdd;
			thecontexts[i]->power.SensePowerfactor = (Mhz) * (Vdd / 2) * (Vdd / 2);
			thecontexts[i]->power.vdd_reduction = 1;
		}
		else
		{
			thecontexts[i]->power.Powerfactor = (Mhz) * checkerVDD * Vdd * checkerVDD * Vdd;
			thecontexts[i]->power.SensePowerfactor = (Mhz) * checkerVDD * (Vdd / 2) * checkerVDD * (Vdd / 2);
			thecontexts[i]->power.vdd_reduction = checkerVDD;
		}

		calculate_power (&(thecontexts[i]->power));

		if (i)
		{
			thecontexts[i]->power.bpred_power = 0;
			thecontexts[i]->power.dcache2_power = thecontexts[0]->power.dcache2_power;

			thecontexts[i]->power.dcache2_decoder = thecontexts[0]->power.dcache2_decoder;
			thecontexts[i]->power.dcache2_wordline = thecontexts[0]->power.dcache2_wordline;
			thecontexts[i]->power.dcache2_bitline = thecontexts[0]->power.dcache2_bitline;
			thecontexts[i]->power.dcache2_senseamp = thecontexts[0]->power.dcache2_senseamp;
			thecontexts[i]->power.dcache2_tagarray = thecontexts[0]->power.dcache2_tagarray;
			thecontexts[i]->power.dcache2_gc = thecontexts[0]->power.dcache2_gc;
		}
	}
	
	if (hotspot_active)
		hotspot_init ();
}

/****************************************
 * wxh 2013-02-27
 * Read the files into an array to record the counter
 * Just to test
 * *************************************/
 /*************************************************
#define ADJUST_NUM 5
#define FREQ_NUM 4
#define VMAX 1.00
#define VMIN 0.50
 
extern char * sim_simout;
char  my_simout[100];
FILE * CTB_FILE=NULL;
FILE * Stastic_FILE=NULL;



FreqCounter ProcFreq[MAXTHREADS];
FreqCounter RouterFreq[MaxRouter];
FreqCounter L2Freq[MAXTHREADS];


char * freqindex[4]={"1.00","0.67","0.50","0.33"};

int num[FREQ_NUM]={64};
int Count=0;
int numrouters;


counter_t INSN[STATUS_NUM]={0};
double L1_IAccess[STATUS_NUM]={0.0},L1_DAccess[STATUS_NUM]={0.0}, L2_Access[STATUS_NUM]={0.0}, Mem_Access[STATUS_NUM]={0.0};
double FPC[STATUS_NUM]={0.00},IPC[STATUS_NUM]={0.00},HPF[STATUS_NUM]={0.00},CPI[STATUS_NUM]={0.00},CPF[STATUS_NUM]={0.00};
double PowerParameter[STATUS_NUM]={0.00};
double CPU_Power[STATUS_NUM]={0.00},Router_Power[STATUS_NUM]={0.00},Total_Power[STATUS_NUM]={0.00};
double cpu_power,router_power,cpu_power_back=0.0,router_power_back=0.0;

counter_t realsimcycle_backup=0,slip_cycles_backup=0,realSimCycle_backup=0,purenuminsn_backup=0,warm_cycle=0,warm_insn=0;
counter_t L1_Iaccess_back=0,L1_Daccess_back=0, L2_access_back=0, Mem_access_back=0;

unsigned long cycle_mask_table[]={0xffffffffffffffff,0xfffffffffffffffe,0xfffffffefffffffe,0xffffeffffeffffef,
								0xfffefffefffefffe,0xffeffeffeffeffef,0xfeffefeffefeffef,0xfeffefefeffefefe,
								0xfefefefefefefefe,0xfefefefefefefeee,0xfefefeeefefefeee,0xfefeefeeefeefeee,
								0xfeeefeeefeeefeee,0xfeeeefeeeefeeeee,0xfeeeeeeefeeeeeee,0xfeeeeeeeeeeeeeee,
								0xeeeeeeeeeeeeeeee,0xeeeeeeeeeeeeeeea,0xeeeeeeeaeeeeeeea,0xeeeeaeeeeaeeeeea,
								0xeeeaeeeaeeeaeeea,0xeeaeeaeeaeeaeeae,0xeeaeeaeeaeaeaeea,0xeeaeaeeaeaeeaeaa,
								0xeaeaeaeaeaeaeaea,0xeaeaeaeaeaeaeaaa,0xeaeaeaaeaaeaaeaa,0xeaaeaaeaaeaaeaaa,
								0xeaaaeaaaeaaaeaaa,0xeaaaaeaaaaeaaaaa,0xeaaaaaaaeaaaaaaa,0xeaaaaaaaaaaaaaaa,
								0xaaaaaaaaaaaaaaaa,0xaaaaaaaaaaaaaaa4,0xaaaaaaa4aaaaaaa4,0xaaaa4aaaa4aaaa4a,
								0xaaa4aaa4aaa4aaa4,0xaa4aa4aa4aa4aa4a,0x4aa4aa4aa4aa4aa4,0xa4aa4a4aa4a4a4a4,
								0xa4a4a4a4a4a4a4a4,0xa4a4a4a4a4a4a484,0xa4a4a484a4a4a484,0xa4a484a4a484a484,
								0xa484a484a484a484,0x9249249249248420
								};

void FindValue(char* fraction, FreqCounter * PF )
{
	if (!strcmp(fraction, "1.00"))
	{
		PF->Value = 1.00;
		PF->Nominator = 1;
		PF->Denominator = 1;
		PF->Vdd_reduction= 1.20;
		PF->latency_factor = 1.00;
		PF->cycle_mask = 0xffffffffffffffff;
	}

	else if (!strcmp(fraction, "0.67"))
	{
		PF->Value = 0.67;
		PF->Nominator = 2;
		PF->Denominator = 3;
		PF->Vdd_reduction = 0.90;
		PF->latency_factor = 1.50;
		PF->cycle_mask = 0xdb6db6db6db6db6c;
	}
	
	else if (!strcmp(fraction, "0.50"))
	{
		PF->Value = 0.50;
		PF->Nominator = 1;
		PF->Denominator = 2;
		PF->Vdd_reduction = 0.75;
		PF->latency_factor = 2.00;
		PF->cycle_mask = 0xaaaaaaaaaaaaaaaa;
	}

	else if (!strcmp(fraction, "0.33"))
	{
		PF->Value = 0.33;
		PF->Nominator = 1;
		PF->Denominator = 3;
		PF->Vdd_reduction = 0.60;
		PF->latency_factor = 3.00;
		PF->cycle_mask = 0x9249249249249248;
	}
	else
	{
		PF->Value = 1.00;
		PF->Nominator = 1;
		PF->Denominator = 1;
		PF->Vdd_reduction = 1.20;
		PF->latency_factor = 1.00;
		PF->cycle_mask = 0xffffffffffffffff;
		printf("the input frequency is wrong,set to 1.0\n");
	}
}

void findvalue(float freq, FreqCounter * PF)
{
	PF->Value = freq;
	PF->latency_factor = 1/freq;
	PF->Nominator = floor(sizeof(unsigned long)*8*freq);
	PF->Denominator = sizeof(unsigned long)*8;
	PF->Vdd_reduction = VMAX-(1.00-freq)*(VMAX-VMIN)*1.5;
	PF->cycle_mask = cycle_mask_table[PF->Denominator-PF->Nominator];
}


//***************************************************
//add by ctb 2013-03-04
//init the power struct of each thread
//***************************************************
void CPUPowerInit( )
{
	int i;
	double dcache2_power,dcache2_decoder,dcache2_wordline;
	double dcache2_bitline,dcache2_senseamp,dcache2_tagarray,dcache2_gc;
		
	
	dcache2_power = thecontexts[0]->power.dcache2_power;
	dcache2_decoder = thecontexts[0]->power.dcache2_decoder;
	dcache2_wordline = thecontexts[0]->power.dcache2_wordline;
	dcache2_bitline = thecontexts[0]->power.dcache2_bitline;
	dcache2_senseamp = thecontexts[0]->power.dcache2_senseamp;
	dcache2_tagarray = thecontexts[0]->power.dcache2_tagarray;
	dcache2_gc = thecontexts[0]->power.dcache2_gc;
		
	for (i = 0; i < numcontexts; i++)
	{				
		
		
		thecontexts[i]->power.Powerfactor = (Mhz) * Vdd * Vdd * ProcFreq[i].Vdd_reduction * ProcFreq[i].Vdd_reduction ;
		thecontexts[i]->power.SensePowerfactor = (Mhz) * (Vdd / 2) * (Vdd / 2) * ProcFreq[i].Vdd_reduction * ProcFreq[i].Vdd_reduction ;
		thecontexts[i]->power.vdd_reduction = ProcFreq[i].Vdd_reduction;
	
		calculate_power (&(thecontexts[i]->power));

	

	}
}

void GetRouterVDD(int id, float * vdd)
{
	*vdd = RouterFreq[id].Vdd_reduction;
}

void setall_procfreq(int freq)
{
	int i,speed;
	
	speed = freq;
	
	if(speed<0||speed>=FREQ_NUM)
		speed = 0 ;
	
	for (i = 0; i < numcontexts; i++)
	{				
		
		FindValue(freqindex[speed],&ProcFreq[i]);
		cache_il1[i]->hit_latency = cache_il1_lat*ProcFreq[i].latency_factor;
		cache_dl1[i]->hit_latency = cache_dl1_lat*ProcFreq[i].latency_factor;
	}
}

void SetProc(float freq)
{
	int i;
	float speed;
	
	speed = freq;
	
	if(speed<0.30||speed>1.00)
		speed = 1.00 ;
	
	for (i = 0; i < numcontexts; i++)
	{				
		
		findvalue(speed,&ProcFreq[i]);
		cache_il1[i]->hit_latency = cache_il1_lat*ProcFreq[i].latency_factor;
		cache_dl1[i]->hit_latency = cache_dl1_lat*ProcFreq[i].latency_factor;
	}
}

void setall_routerfreq(int freq)
{
	int i,speed;

	speed = freq;
	
	if(speed<0||speed>=FREQ_NUM)
		speed = 0 ;
	 
	for (i = 0; i < numrouters; i++)
	{				

		FindValue(freqindex[speed],&RouterFreq[i]);

	}
}

void SetRouter(float freq)
{
	int i;
	float speed;

	speed = freq;
	
	if(speed<0.30||speed>1.00)
		speed = 1.00 ;
	 
	for (i = 0; i < numrouters; i++)
	{				

		findvalue(speed,&RouterFreq[i]);

	}
}

void setall_l2freq(int freq)
{
	int i,speed;

	speed = freq;
	
	if(speed<0||speed>=FREQ_NUM)
		speed = 0 ;
	
	for (i = 0; i < numcontexts; i++)
	{				
		
		FindValue(freqindex[speed],&L2Freq[i]);

	}
}

void random_procfreq(int i)
{
	int random;

	for(;;)
	{
		random=rand()%FREQ_NUM;
		if(num[random]>0)
			{
				num[random]--;
				break ;
			}
	}
	FindValue(freqindex[random],&ProcFreq[i]);
	cache_il1[i]->hit_latency = cache_il1_lat*ProcFreq[i].latency_factor;
	cache_dl1[i]->hit_latency = cache_dl1_lat*ProcFreq[i].latency_factor;
	
}

void random_routerfreq(int i)
{
	int random;

	for(;;)
	{
		random=rand()%FREQ_NUM;
		if(num[random]>0)
			{
				num[random]--;
				break ;
			}
	}	

	FindValue(freqindex[random],&RouterFreq[i]);
	
}

void random_l2freq(int i)
{
	int random;

	for(;;)
	{
		random=rand()%FREQ_NUM;
		if(num[random]>0)
			{
				num[random]--;
				break ;
			}
	}
	FindValue(freqindex[random],&L2Freq[i]);
	
}

void random_num()
{
	int counter = 0, i, j, k;
	if(random_proc)
		counter += numcontexts;
	if(random_router)
		counter += numcontexts;
	if(random_l2)
		counter += numcontexts;
	j=rand()%FREQ_NUM;
	for(i=1;i<FREQ_NUM;i++)
	{
		k = rand()%counter;
		num[j] = k;
		counter -= k;
		j = (j+1)%FREQ_NUM;
	}
	num[j] = counter;
}

void FreqAdjust(float P2R , double Power, double P_A, double R_A)
{
	//double RealTimePower_CPU,RealTimePower_Router;
	float Power_Estimate;
	float F_P,F_R,V_P,V_R;
	//float FP,FR,VP,VR;
	float High_R,Low_R,High_P,Low_P;
	int P_or_R=0;
	
	if(P2R > 3.00 )
		P2R = 3.00;
	if(P2R < 0.333)
		P2R = 0.333;

	//FP = proc_freq;
	//FR = router_freq;
	//VP = VMAX-(1.00-FP)*(VMAX-VMIN)*1.5;
	//VR = VMAX-(1.00-FR)*(VMAX-VMIN)*1.5;

	High_R = 1.00;
	Low_R = 0.333;
	High_P = 1.00;
	Low_P = 0.333;
	
	//TotalCPUPower(&cpu_power);
	//TotalRouterPower(&router_power);
	//RealTimePower_CPU = (cpu_power-cpu_power_back)/(realSimCycle-realsimcycle_backup);
	//RealTimePower_Router = (router_power-router_power_back)/(realSimCycle-realsimcycle_backup);
	
	while(1)
	{
		if(P_or_R > 0)	
		{
			F_R = (High_R + Low_R)/2;
			F_P = F_R*P2R;
			if(F_P > High_P)
				F_P = High_P;
			if(F_P < Low_P)
				F_P = Low_P;
		}
		else
		{
			F_P = (High_P + Low_P)/2;
			F_R = F_P/P2R;
			if(F_R > High_R)
				F_R = High_R;
			if(F_R < Low_R)
				F_R = Low_R;
		}

		V_P = VMAX-(1.00-F_P)*(VMAX-VMIN)*1.5;
		V_R = VMAX-(1.00-F_R)*(VMAX-VMIN)*1.5;

		Power_Estimate = P_A*F_P*V_P*V_P+R_A*F_R*V_R*V_R;
		
		if(Power_Estimate < Power)
		{
			Low_R = F_R;
			Low_P = F_P;
			if(Low_P > Low_R)
				P_or_R = 1;
			else
				P_or_R = 0;
		}
		else
		{
			High_R = F_R;
			High_P = F_P;
			if(High_P > High_R)
				P_or_R = 0;
			else
				P_or_R = 1;
		}

		if( fabs(Power_Estimate-Power) < 5.00 )
			break;
		if( fabs(High_R-Low_R) < 0.01 && fabs(High_P-Low_P) < 0.01)
			break;

	}

	
	proc_freq = F_P;
	router_freq = F_R;
	proc_v = V_P;
	router_v = V_R;
	SetProc(proc_freq);
	SetRouter(router_freq);
	CPUPowerInit();
	RouterPowerInit();
	
}

void flits_write(FILE *file)
{
	unsigned int i, j;

	if(file != NULL)
	{
		fprintf(file,"/****************************************************************");
		for (i=0; i<100; i++)
		{
			for (j=0; j<100; j++)
			{
				fprintf(file,"%4d  ", flits_sending[i][j]);
			}
			fprintf(file,"\n");
		}
		fprintf(file,"/****************************************************************");

	}
}

void flits_reset()
{
	unsigned int i, j;

	for (i=0; i<100; i++)
	{
		for (j=0; j<100; j++)
		{
			flits_sending[i][j] = 0;
		}
	}
}

//**************end***********************************
//****************************************************
// * ctb 2013-03-05
// * Set whether to go or not
// * *************************************************
#define DVFS
#ifdef DVFS
int proc_dvfs(int tID)
{

//	if(ProcFreq[tID].counter<ProcFreq[tID].Nominator)
	unsigned long mask;

	mask = 1<<(realSimCycle&0x3f);
	if( (ProcFreq[tID].cycle_mask & mask) == mask)
		return 1;
	else 
		return 0;
		
}

void update_proc_counter( )
{
	int i;
	for(i=0;i<numcontexts;i++)
	{
		ProcFreq[i].counter++;
		ProcFreq[i].counter%=ProcFreq[i].Denominator;

	}
}

int router_dvfs(int tID)
{

//	if(RouterFreq[tID].counter<RouterFreq[tID].Nominator)
	unsigned long mask;

	mask = 1<<(realSimCycle&0x3f);
	if( (RouterFreq[tID].cycle_mask & mask) == mask)
		return 1;
	else 
		return 0;
		
}

void update_router_counter( )
{
	int i;


	for(i=0;i<numrouters;i++)
	{
		RouterFreq[i].counter++;
		RouterFreq[i].counter%=RouterFreq[i].Denominator;

	}
}

#else 

int router_dvfs(int id)
{
	return 1;
}
void update_router_counter( )
{
	return ;
}

int proc_dvfs(int id)
{
	return 1;
}
void update_proc_counter( )
{
	return ;
}

#endif

void  update_active()
{
	int i;

	for(i=0;i<numcontexts;i++)
	{
		thecontexts[i]->active_this_cycle=proc_dvfs(i);
	}
}
//------------------------------END-------------------------------------*/

/* default register state accessor, used by DLite */
static char *			/* err str, NULL for no err */
simoo_reg_obj (struct regs_t *regs,	/* registers to access */
		int is_write,	/* access type */
		enum md_reg_type rt,	/* reg bank to probe */
		int reg,	/* register number */
		struct eval_value_t *val);	/* input, output */

/* default memory state accessor, used by DLite */
static char *			/* err str, NULL for no err */
simoo_mem_obj (struct mem_t *mem,	/* memory space to access */
		int is_write,	/* access type */
		md_addr_t addr,	/* address to access */
		char *p,	/* input/output buffer */
		int nbytes);	/* size of access */

/* default machine state accessor, used by DLite */
static char *			/* err str, NULL for no err */
simoo_mstate_obj (FILE * stream,	/* output stream */
		char *cmd,	/* optional command string */
		struct regs_t *regs,	/* registers to access */
		struct mem_t *mem);	/* memory space to access */

/* total RS links allocated at program start */
#define MAX_RS_LINKS                     65536
counter_t rs_link_num = 0;
/* grbriggs: lechen uses this var */
int MAX_BR_NUM;

/* load threads into simulated state */
	void
sim_load_threads (int argc, char **argv,	/* program arguments */
		char **envp)	/* program environment */
{
	int i, j, arg_status = 1;
	context *current;

	numcontexts = 0;
	for (j = 0; (j < argc) && (numcontexts < MAXTHREADS) && arg_status; j++)
	{
		fprintf (stderr, "initializing context %d\n", numcontexts);
		/* read argument file *.bnc for each thread */
		arg_status = read_thread_args (envp, argv[j], &numcontexts);

		current = thecontexts[j];

		current->actualid = 0;

		/* load program text and data, set up environment, memory, and regs */
		ld_load_prog (current->fname, current->argc, current->argv, envp, &(current->regs), current->mem, TRUE, j);
	}

	/* initialize here, so symbols can be loaded */
	if (ptrace_nelt == 2)
	{
		/* generate a pipeline trace */
		ptrace_open ( /* fname */ ptrace_opts[0], /* range */ ptrace_opts[1]);
	}
	else if (ptrace_nelt == 0)
	{
		/* no pipetracing */ ;
	}
	else
		fatal ("bad pipetrace args, use: <fname|stdout|stderr> <range>");


	MAX_BR_NUM = ruu_ifq_size + RUU_size;



	/* finish initialization of the simulation engine */
	fu_pool = res_create_pool ("fu-pool", fu_config, N_ELT (fu_config));
	rslink_init (MAX_RS_LINKS);
	rs_cache_init();
	activecontexts = numcontexts;
	for (j = 0; j < numcontexts; j++)
		tracer_init (j);
	for (j = 0; j < numcontexts; j++)
		fetch_init (j);
	for (j = 0; j < numcontexts; j++)
		cv_init (j);
	eventq_init ();
	readyq_init ();
#ifdef LOCK_HARD
	lock_eventq_init();
#endif
	dir_eventq_init();
	for(j = 0; j < 100; j++)
	{
		dir_fifo_head[j] = 0;
		dir_fifo_tail[j] = 0;
		dir_fifo_num[j] = 0;
		dir_fifo_portuse[j] = 0;
		l1_fifo_head[j] = 0;
		l1_fifo_tail[j] = 0;
		l1_fifo_num[j] = 0;
		l1_fifo_portuse[j] = 0;
		last_L1_mshr_full[j] = 0;
		last_L1_fifo_full[j] = 0;
		last_Dir_fifo_full[j] = 0;
		last_Input_queue_full[j] = 0;
	}
	last_L2_mshr_full = 0;
	same_L1_L2_fifo_full = 0;
	L1_mshr_full = 0;
	L2_mshr_full = 0;
	L2_mshr_full_prefetch = 0;
	L1_fifo_full = 0;
	Dir_fifo_full = 0;
	Input_queue_full = 0;
	Output_queue_full = 0;

	Stall_L1_mshr = 0;
	Stall_L2_mshr = 0;
	Stall_L1_fifo = 0;
	Stall_dir_fifo = 0;
	Stall_input_queue = 0;
	Stall_output_queue = 0;

	for (j = 0; j < numcontexts; j++)
	{
		ruu_init (j);
		lsq_init (j);
	}

	/* initialize the priority list and key metric */
	for (j = 0; j < numcontexts; j++)
	{
		priority[j] = j;
		key[j] = 0;
#ifdef INTERVAL_BASED
		interval_priority[j] = j;
		interval_key[j] = 0;
#endif
	}

#ifdef   CLUSTER_PRIORITY
	for (j = 0; j < CLUSTERS; j++)
		priority_cl[j] = j;
#endif

#ifdef  PRIORITY_STEER
	for (j = 0; j < CLUSTERS; j++)
		priority_cl[j] = j;
#endif

#ifdef   STEER_DISPATCH_STAT
	for (j = 0; j < numcontexts; j++)
		ready_steer_thrd_max[j] = ready_steer_thrd_min[j] = ready_steer_thrd_value[j] = MAXDEV / 2;
#endif

#if defined(DLITE_SUPPORT)
	/* initialize the DLite debugger */
	dlite_init (simoo_reg_obj, simoo_mem_obj, simoo_mstate_obj);
#endif /* defined(DLITE_SUPPORT) */
}

/* dump simulator-specific auxiliary simulator statistics */
	void
sim_aux_stats (FILE * stream)	/* output stream */
{
	/* nada */
}

/* un-initialize the simulator */
	void
sim_uninit (void)
{
	if (ptrace_nelt > 0)
		ptrace_close ();
	/*    if (hotspot_active)
	      hotspot_uninit ();*/
}


/*
 * processor core definitions and declarations
 */

#ifndef  STEER_DISPATCH_STAT
/* non-zero if all register operands are ready, update with MAX_IDEPS */
#define OPERANDS_READY(RS)                                              \
	((RS)->idep_ready[0] && (RS)->idep_ready[1] && (RS)->idep_ready[2])
#endif //STEER_DISPATCH_STAT

/* non-zero if one register operands is ready, update with MAX_IDEPS */
#define ONE_OPERANDS_READY(RS)                                              \
	((RS)->idep_ready[0] || (RS)->idep_ready[1])


/* allocate and initialize register update unit (RUU) */
	void
ruu_init (int threadid)
{
	int i = 0;
	context *current;

	current = thecontexts[threadid];
	current->RUU = calloc (RUU_size, sizeof (struct RUU_station));
	if (!current->RUU)
		fatal ("out of virtual memory");
	/* set index in eaach RUU entry to its order in the RUU queue */
	for (i = 0; i < RUU_size; i++)
	{
		current->RUU[i].index = i;
		current->RUU[i].threadid = threadid;
	}
	current->RUU_num = 0;
	current->RUU_head = current->RUU_tail = 0;
	current->RUU_count_thrd = 0;
	current->RUU_fcount_thrd = 0;
	RUU_count = 0;
	RUU_fcount = 0;

	if (threadid)
	{
		current->cycle_mask = 0xffffffff;	//0xffffffff means full speed & 0xaaaaaaaa will be half speed
		current->latency_factor = 1;
		current->main_thread = 1;
		cache_dl1[threadid]->hit_latency *= current->latency_factor;
	}

	for (i = 0; i < CLUSTERS; i++)
	{
		iregfile_cl_count[i] = 0;
		iextra_cl_count[i] = 0;
		fregfile_cl_count[i] = 0;
		fextra_cl_count[i] = 0;
		iissueq_cl_count[i] = 0;
		fissueq_cl_count[i] = 0;
#ifdef RAW
		rawq_cl_count[i] = 0;
#endif
	}
}

/* dump the contents of the RUU */
	static void
ruu_dumpent (struct RUU_station *rs,	/* ptr to RUU station */
		int index,		/* entry index */
		FILE * stream,	/* output stream */
		int header)	/* print header? */
{
	if (!stream)
		stream = stderr;

	if (header)
		fprintf (stream, "idx: %2d: opcode: %s, inst: `", index, MD_OP_NAME (rs->op));
	else
		fprintf (stream, "       opcode: %s, inst: `", MD_OP_NAME (rs->op));
	md_print_insn (rs->IR, rs->PC, stream);
	fprintf (stream, "'\n");
	myfprintf (stream, "         PC: 0x%08p, NPC: 0x%08p (pred_PC: 0x%08p)\n", rs->PC, rs->next_PC, rs->pred_PC);
	fprintf (stream, "         in_LSQ: %s, ea_comp: %s, recover_inst: %s\n", rs->in_LSQ ? "t" : "f", rs->ea_comp ? "t" : "f", rs->recover_inst ? "t" : "f");
	myfprintf (stream, "         spec_mode: %s, addr: 0x%08p, tag: 0x%08x\n", rs->spec_mode ? "t" : "f", rs->addr, rs->tag);
	fprintf (stream, "         seq: 0x%08x, ptrace_seq: 0x%08x\n", rs->seq, rs->ptrace_seq);
	fprintf (stream, "         queued: %s, issued: %s, completed: %s\n", rs->queued ? "t" : "f", rs->issued ? "t" : "f", rs->completed ? "t" : "f");
	fprintf (stream, "         operands ready: %s\n", OPERANDS_READY (rs) ? "t" : "f");
}

/* dump the contents of the RUU */
	static void
ruu_dump (FILE * stream, int threadid)	/* output stream */
{
	int num, head;
	struct RUU_station *rs;

	context *current;

	current = thecontexts[threadid];

	if (!stream)
		stream = stderr;

	fprintf (stream, "** Thread %d **\n", threadid);
	fprintf (stream, "** RUU state **\n");
	fprintf (stream, "RUU_head: %d, RUU_tail: %d\n", current->RUU_head, current->RUU_tail);
	fprintf (stream, "RUU_num: %d\n", current->RUU_num);

	num = current->RUU_num;
	head = current->RUU_head;
	while (num)
	{
		rs = &(current->RUU[head]);
		ruu_dumpent (rs, rs - current->RUU, stream, /* header */ TRUE);
		head = (head + 1) % RUU_size;
		num--;
	}
}


	void
lsq_init (int threadid)
{
	int i = 0;
	context *current;

	current = thecontexts[threadid];

	current->LSQ = calloc (LSQ_size, sizeof (struct RUU_station));
	if (!current->LSQ)
		fatal ("out of virtual memory");
	for (i = 0; i < LSQ_size; i++)
	{
		current->LSQ[i].index = i;
		current->LSQ[i].threadid = threadid;
	}
	current->LSQ_num = 0;
	current->LSQ_head = current->LSQ_tail = 0;
	current->LSQ_count = 0;
	current->LSQ_fcount = 0;
	current->m_shLQNum = 0;
	current->m_shSQNum = 0;
	m_totalStall[threadid] = 0;
	m_sqLQFull[threadid] = 0;
	m_sqSQFull[threadid] = 0;
	m_RUUFull[threadid] = 0;
	m_commitStall[threadid] = 0;

}

/* allocate and initialize the load/store queue (LSQ) */


#ifdef COUNT_LSQ
int non_issue_LSQ_num;
#endif

/*
 * input dependencies for stores in the LSQ:
 *   idep #0 - operand input (value that is store'd)
 *   idep #1 - effective address input (address of store operation)
 */
#define STORE_OP_INDEX                  0
#define STORE_ADDR_INDEX                1

#define STORE_OP_READY(RS)              ((RS)->idep_ready[STORE_OP_INDEX])
#define STORE_ADDR_READY(RS)            ((RS)->idep_ready[STORE_ADDR_INDEX])

/*
 * RS_LINK defs and decls
 */

/* a reservation station link: this structure links elements of a RUU
   reservation station list; used for ready instruction queue, event queue, and
   output dependency lists; each RS_LINK node contains a pointer to the RUU
   entry it references along with an instance tag, the RS_LINK is only valid if
   the instruction instance tag matches the instruction RUU entry instance tag;
   this strategy allows entries in the RUU can be squashed and reused without
   updating the lists that point to it, which significantly improves the
   performance of (all to frequent) squash events */

/* RS link free list, grab RS_LINKs from here, when needed */
struct RS_link *rslink_free_list;
struct RS_link *rslink_list[MAX_RS_LINKS];
/* NULL value for an RS link */

struct RS_list *rs_cache_list[MAXTHREADS][MSHR_SIZE];
// struct RS_list *rs_cache_pref_list[MAXTHREADS][MSHR_SIZE];/*hq*/


static struct RS_link RSLINK_NULL = RSLINK_NULL_DATA;
counter_t rs_link_id = 0;

/* create and initialize an RS link */
#define RSLINK_INIT(RSL, RS)						\
	((RSL).next = NULL, (RSL).rs = (RS), (RSL).tag = (RS)->tag)

/* non-zero if RS link is NULL */
#define RSLINK_IS_NULL(LINK)            ((LINK)->rs == NULL)

/* non-zero if RS link is to a valid (non-squashed) entry */
#define RSLINK_VALID(LINK)              ((LINK)->tag == (LINK)->rs->tag)

/* extra RUU reservation station pointer */
#define RSLINK_RS(LINK)                 ((LINK)->rs)

/* get a new RS link record */
#define RSLINK_NEW(DST, RS)						\
{ struct RS_link *n_link;						\
	if (!rslink_free_list)						\
	panic("out of rs links");						\
	rs_link_num++;							\
	n_link = rslink_free_list;						\
	rslink_free_list = rslink_free_list->next;				\
	n_link->next = NULL;						\
	n_link->rs = (RS); n_link->tag = n_link->rs->tag;			\
	n_link->last_use = sim_cycle;					\
	(DST) = n_link;							\
}

/* free an RS link record */
#define RSLINK_FREE(LINK)						\
{  struct RS_link *f_link = (LINK);					\
	f_link->rs = NULL; f_link->tag = 0;				\
	f_link->next = rslink_free_list;					\
	rslink_free_list = f_link;						\
	rs_link_num--;							\
}

/* FIXME: could this be faster!!! */
/* free an RS link list */
#define RSLINK_FREE_LIST(LINK)						\
{  struct RS_link *fl_link, *fl_link_next;				\
	for (fl_link=(LINK); fl_link; fl_link=fl_link_next)		\
	{								\
		fl_link_next = fl_link->next;					\
		RSLINK_FREE(fl_link);						\
	}								\
}

/* initialize the free RS_LINK pool */
	static void
rslink_init (int nlinks)	/* total number of RS_LINK available */
{
	int i;
	struct RS_link *link;

	rslink_free_list = NULL;
	for (i = 0; i < nlinks; i++)
	{
		link = calloc (1, sizeof (struct RS_link));
		if (!link)
			fatal ("out of virtual memory");
		rslink_list[i] = link;
		link->next = rslink_free_list;
		rslink_free_list = link;
	}
}

struct RS_link * findMax_rslink()
{
	int i, max_id;
	struct RS_link *link;
	counter_t max = 0;

	for (i = 0; i < MAX_RS_LINKS; i++)
	{
		if(rslink_list[i]->last_use > max)
		{
			max = rslink_list[i]->last_use;
			max_id = i;
		}
	}
	return rslink_list[max_id];
}

struct RS_link * findMin_rslink()
{
	int i, min_id;
	struct RS_link *link;
	counter_t min = WAIT_TIME;

	for (i = 0; i < MAX_RS_LINKS; i++)
	{
		if(rslink_list[i]->last_use < min)
		{
			min = rslink_list[i]->last_use;
			min_id = i;
		}
	}
	return rslink_list[min_id];
}

	static void
rs_cache_init()
{
	int i, n;
	for (n = 0; n < MAXTHREADS; n++)
	{
		for(i=0; i<MSHR_SIZE; i++)
			rs_cache_list[n][i] = NULL;
	}
}
/*011305mwr*/
/*check which threads have operations ongoing in diff func units*/


	static void
check_fu_busy (void)
{
	int i;

	for (i = 0; i < numcontexts; i++)
	{
		fu_busy[i] = 0;
	}

	/* walk all resource units, decrement busy counts by one */
	for (i = 0; i < fu_pool->num_resources; i++)
	{
		if (fu_pool->resources[i].busy)
			fu_busy[fu_pool->resources[i].usr_id]++;
	}
}




/* service all functional unit release events, this function is called
   once per cycle, and it used to step the BUSY timers attached to each
   functional unit in the function unit resource pool, as long as a functional
   unit's BUSY count is > 0, it cannot be issued an operation */
	static void
ruu_release_fu (void)
{
	int i;

	/* walk all resource units, decrement busy counts by one */
	for (i = 0; i < fu_pool->num_resources; i++)
	{
		/* resource is released when BUSY hits zero */
		if (fu_pool->resources[i].busy > 0)
		{
			fu_pool->resources[i].busy--;

#ifdef  PWR_ALU
#ifdef ALU_MERGE
			if (fu_pool->resources[i].x[0].class == FU_IALU_INDEX)
			{
				ialu1_access++;
				thecontexts[fu_pool->resources[i].usr_id]->ialu1_access++;
			}
			else if (fu_pool->resources[i].x[0].class == FU_FPALU_INDEX)
			{
				falu1_access++;
				thecontexts[fu_pool->resources[i].usr_id]->falu1_access++;
			}
#else //ALU_MERGE
			if (fu_pool->resources[i].x[0].class == FU_IALU_INDEX)
			{
				ialu1_access++;
				thecontexts[fu_pool->resources[i].usr_id]->ialu1_access++;
			}
			else if (fu_pool->resources[i].x[0].class == FU_IMULT_INDEX)
			{
				ialu2_access++;
				thecontexts[fu_pool->resources[i].usr_id]->ialu2_access++;
			}

			else if (fu_pool->resources[i].x[0].class == FU_FPALU_INDEX)
			{
				falu1_access++;
				thecontexts[fu_pool->resources[i].usr_id]->falu1_access++;
			}

			else if (fu_pool->resources[i].x[0].class == FU_FPMULT_INDEX)
			{
				falu2_access++;
				thecontexts[fu_pool->resources[i].usr_id]->falu2_access++;
			}

#endif //ALU_MERGE
#endif //PWR_ALU

		}
	}
}

#ifdef CONF_RES_RESEND
struct QUEUE_EVENT *reply_queue[MAXTHREADS], *send_queue[MAXTHREADS];
#endif
struct DIRECTORY_EVENT *dir_event_queue;   // event queue directory version, global directory event queue
struct DIRECTORY_EVENT *event_list[L2_MSHR_SIZE];   //event list appending to a mshr entry

	static void                                       // initialize directory event queue
dir_eventq_init(void)
{
	int n = 0;
#ifdef CONF_RES_RESEND
	for (n = 0; n<MAXTHREADS; n++)
	{
		reply_queue[n] = (struct QUEUE_EVENT *) calloc (1, sizeof (struct QUEUE_EVENT));
		send_queue[n] = (struct QUEUE_EVENT *) calloc (1, sizeof (struct QUEUE_EVENT));
		initQueue(reply_queue[n]);
		initQueue(send_queue[n]);
	}
#endif
	dir_event_queue = NULL;
	for (n = 0; n<L2_MSHR_SIZE; n++)
		event_list[n] = NULL;
}
#ifdef LOCK_HARD
void
lock_eventq_init(void)
{
	int n = 0, j = 0;
	for (n = 0; n<MAXTHREADS; n++)
	{
		for(j=0;j<MAXLOCK;j++)
		{
			LOCK_REG[n][j] = (struct LOCK_ENTRY *) calloc (1, sizeof (struct LOCK_ENTRY));
			LOCK_REG[n][j]->lock_owner = 0;
			LOCK_REG[n][j]->waiting_num = 0;
			LOCK_REG[n][j]->waiting_head = 0;
			LOCK_REG[n][j]->waiting_tail = 0;
		}
		for(j=0;j<MAXBARRIER;j++)
		{
			BAR_REG[n][j] = (struct BAR_ENTRY *) calloc (1, sizeof (struct BAR_ENTRY));
			BAR_REG[n][j]->barrier_num = 0;
		}
	}
}
#endif

#ifdef CONF_RES_RESEND
	static void
initQueue(struct QUEUE_EVENT *queue)
{
	int i;
	if(!queue) return;
	queue->free_Entries = QUEUE_SIZE;
	for(i=0; i<QUEUE_SIZE; i++)
	{
		queue->Queue_entry[i].isvalid = 0;
		queue->Queue_entry[i].event = NULL;
	}
}
#endif

/* this function is checking outgoing buffer in network for first */
int buffer_full_check(struct DIRECTORY_EVENT *event)
{
	int vc = 0, options = -1, buffer_full = 0;
	if((event->des1*mesh_size+event->des2 != event->src1*mesh_size+event->src2) && (event->input_buffer_full)
        && (event->operation == MISS_WRITE || event->operation == MISS_READ || event->operation == WRITE_UPDATE || event->operation == MISS_IL1 ))
	{
		if (!mystricmp (network_type, "FSOI") || (!mystricmp (network_type, "HYBRID")))
		{
			if(opticalBufferSpace(event->src1, event->src2, event->operation))
				buffer_full = 1;
		}
		else if (!mystricmp (network_type, "MESH"))
		{
			options = OrderCheck(event->src1, event->src2, event->des1, event->des2, event->addr&~(cache_dl1[0]->bsize-1));
            vc = simEsynetBufferSpace(event->src1, event->src2, options);
			if(vc == -1)
				buffer_full = 1;

		}
		else if ((!mystricmp (network_type, "COMB")))
		{
			buffer_full = CombNetworkBufferSpace(event->src1, event->src2, event->des1, event->des2, event->addr&~(cache_dl1[0]->bsize-1), event->operation, &vc);
		}
		if(buffer_full)
		{
			/* buffer is full */
			if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
			{
				Input_queue_full ++;
				last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
			}
			return 1;
		}
		else
		{
			event->input_buffer_full = 0;
			event->startCycle = sim_cycle;
			scheduleThroughNetwork(event, sim_cycle, meta_packet_size, vc);
			if(last_Input_queue_full[event->des1*mesh_size+event->des2])
			{
				Stall_input_queue += sim_cycle - last_Input_queue_full[event->des1*mesh_size+event->des2];
				last_Input_queue_full[event->des1*mesh_size+event->des2] = 0;
			}
			return 2;
		}
	}
	else
		return 0;
}
static void dir_eventq_nextevent(void)                  // return next directory event occured, call a function working on event
{
	struct DIRECTORY_EVENT *event, *ev, *next, *prev;
	int i, insert_flag = 0;
	event = dir_event_queue;
	ev = NULL;
	while (event!=NULL)
	{
		next = event->next;

		if (event->when <= sim_cycle)
		{ /* event is ready */
			insert_flag = buffer_full_check(event); /* check the outgoing buffer for the first class event (First class event is the MISS events and WriteUpgrade events) */

			if(insert_flag == 0)
			{ /* event is the first class event */
				if(event == dir_event_queue)
				{
					dir_event_queue = event->next;
					event->next = NULL;
					if(!(dir_fifo_enqueue(event, 1)))
					{
						event->next = dir_event_queue;
						dir_event_queue = event;
						ev = event;
						event = event->next;
						same_L1_L2_fifo_full ++;
					}
					else
						event = dir_event_queue;
				}
				else
				{
					ev->next = event->next;
					event->next = NULL;
					if(!(dir_fifo_enqueue(event, 1)))
					{
						event->next = dir_event_queue;
						dir_event_queue = event;
						ev = event;
						event = event->next;
						same_L1_L2_fifo_full ++;
					}
					else
						event = ev->next;
				}
			}
			else if(insert_flag == 1)
			{
				ev = event;
				event = event->next;
			}
			else if(insert_flag == 2)
			{
				if(event == dir_event_queue)
				{
					dir_event_queue = event->next;
					event->next = NULL;
					dir_eventq_insert(event);
					event = dir_event_queue;
				}
				else
				{
					ev->next = event->next;
					event->next = NULL;
					dir_eventq_insert(event);
					event = ev->next;
				}
			}

		}
		else
		{
			ev = event;
			event = event->next;
		}

	}
}


/*
 * the execution unit event queue implementation follows, the event queue
 * indicates which instruction will complete next, the writeback handler
 * drains this queue
 */

/* pending event queue, sorted from soonest to latest event (in time), NOTE:
   RS_LINK nodes are used for the event queue list so that it need not be
   updated during squash events */
struct RS_link *event_queue;
/* initialize the event queue structures */
	static void
eventq_init (void)
{
	event_queue = NULL;
}


/* dump the contents of the event queue */

	static void
eventq_dump (FILE * stream, int threadid)	/* output stream */
{
	struct RS_link *ev;

	context *current;

	current = thecontexts[threadid];

	if (!stream)
		stream = stderr;

	fprintf (stream, "** event queue state **\n");

	for (ev = event_queue; ev != NULL; ev = ev->next)
	{
		/* is event still valid? */
		if (RSLINK_VALID (ev))
		{
			struct RUU_station *rs = RSLINK_RS (ev);

			fprintf (stream, "idx: %2d: @ %.0f\n", rs->index, (double) ev->x.when);
			ruu_dumpent (rs, rs->index, stream, /* !header */ FALSE);
		}
	}
}


/* insert an event for RS into the event queue, event queue is sorted from
   earliest to latest event, event and associated side-effects will be
   apparent at the start of cycle WHEN */
static void
eventq_queue_event (struct RUU_station *rs, tick_t when, int lat)
{
	struct RS_link *prev, *ev, *new_ev;

	if (rs->completed)
		panic ("event completed");

	if (when <= sim_cycle)
		panic ("event occurred in the past");

	/* get a free event record */
	RSLINK_NEW (new_ev, rs);
	new_ev->x.when = when;

	if(rs == NULL)
		printf("RS problem %lld\n", sim_cycle);
	/* locate insertion point */
	for (prev = NULL, ev = event_queue; ev && ev->x.when < when; prev = ev, ev = ev->next);

	if (prev)
	{
		/* insert middle or end */
		new_ev->next = prev->next;
		prev->next = new_ev;
	}
	else
	{
		/* insert at beginning */
		new_ev->next = event_queue;
		event_queue = new_ev;
	}
}

/* return the next event that has already occurred, returns NULL when no
   remaining events or all remaining events are in the future */

/*102104mwr: return the first free for thread active in this cycle*/

	static struct RUU_station *
eventq_next_event (void)
{
	struct RS_link *ev;
	struct RS_link *temp_ev, *pred_ev = NULL;

	temp_ev = event_queue;

	while (temp_ev && !thecontexts[temp_ev->rs->threadid]->active_this_cycle)
	{
		pred_ev = temp_ev;
		temp_ev = temp_ev->next;
	}

	if (temp_ev && temp_ev->x.when <= sim_cycle)
	{
		/* unlink and return first event on priority list */
		ev = temp_ev;
		if (temp_ev == event_queue)
		{
			event_queue = event_queue->next;
		}
		else
		{
			pred_ev->next = temp_ev->next;
		}

		/* event still valid? */
		if (RSLINK_VALID (ev))
		{
			struct RUU_station *rs = RSLINK_RS (ev);

			/* reclaim event record */
			RSLINK_FREE (ev);

			/* event is valid, return resv station */
			return rs;
		}
		else
		{
			/* reclaim event record */
			RSLINK_FREE (ev);

			/* receiving inst was squashed, return next event */
			return eventq_next_event ();
		}
	}
	else
	{
		/* no event or no event is ready */
		return NULL;
	}
}

changeInEventQueue (struct RUU_station * rs, int threadid)
{
	/* This instruction can provide results on the bus next cycle. So we can issue and
	 ** execute dependent instructions next cycle */
	struct RS_link *ev = event_queue;
	struct RS_link *prev = NULL;

	while (ev != NULL)
	{
		if (ev->rs == rs)
			break;
		prev = ev;
		ev = ev->next;
	}

	if (ev == NULL)
		return;
	ev->x.when = sim_cycle;

	/* Remove this event and insert it at the bottom */
	if (prev == NULL)           /* Beginning, leave it here only */
		event_queue = ev->next;
	else
		prev->next = ev->next;

	RSLINK_FREE (ev);
}


/*
 * the ready instruction queue implementation follows, the ready instruction
 * queue indicates which instruction have all of there *register* dependencies
 * satisfied, instruction will issue when 1) all memory dependencies for
 * the instruction have been satisfied (see lsq_refresh() for details on how
 * this is accomplished) and 2) resources are available; ready queue is fully
 * constructed each cycle before any operation is issued from it -- this
 * ensures that instruction issue priorities are properly observed; NOTE:
 * RS_LINK nodes are used for the event queue list so that it need not be
 * updated during squash events
 */

/* the ready instruction queue */
static struct RS_link *ready_queue;

/* initialize the event queue structures */
	static void
readyq_init (void)
{
	ready_queue = NULL;
}

/* dump the contents of the ready queue */

	static void
readyq_dump (FILE * stream, int threadid)	/* output stream */
{
	struct RS_link *link;

	context *current;

	current = thecontexts[threadid];

	if (!stream)
		stream = stderr;

	fprintf (stream, "** ready queue state **\n");

	for (link = ready_queue; link != NULL; link = link->next)
	{
		/* is entry still valid? */
		if (RSLINK_VALID (link))
		{
			struct RUU_station *rs = RSLINK_RS (link);

			ruu_dumpent (rs, rs->index, stream, /* header */ TRUE);
		}
	}
}

/* insert ready node into the ready list using ready instruction scheduling
   policy; currently the following scheduling policy is enforced:

   memory and long latency operands, and branch instructions first

   then

   all other instructions, oldest instructions first

   this policy works well because branches pass through the machine quicker
   which works to reduce branch misprediction latencies, and very long latency
   instructions (such loads and multiplies) get priority since they are very
   likely on the program's critical path */
	void
readyq_enqueue (struct RUU_station *rs)	/* RS to enqueue */
{
	struct RS_link *prev, *node, *new_node;

	/* node is now queued */
	if (rs->queued)
		panic("node is already queued");

	rs->queued = TRUE;

#ifdef REMOVE_COMPUTE_QUEUE
#ifdef PWR_WAKE
	if (rs->in_LSQ)
	{
		lsqready++;
		rs->counted_lsqready = 1;
	}
	else if ((MD_OP_FUCLASS (rs->op) > 3) && (MD_OP_FUCLASS (rs->op) < 10))
	{
		fready++;
		rs->counted_fready = 1;
	}
	else
	{
		iready++;
		rs->counted_iready = 1;
	}
#endif
#endif

	/* get a free ready list node */
	RSLINK_NEW (new_node, rs);
	new_node->x.seq = rs->seq;

	/* otherwise insert in program order (earliest seq first) */
	for (prev = NULL, node = ready_queue; node && node->x.seq < rs->seq; prev = node, node = node->next);

	if (prev)
	{
		/* insert middle or end */
		new_node->next = prev->next;
		prev->next = new_node;
	}
	else
	{
		/* insert at beginning */
		new_node->next = ready_queue;
		ready_queue = new_node;
	}
}


#if 0
	static void
readyq_enqueue (struct RUU_station *rs)	/* RS to enqueue */
{
	struct RS_link *prev, *node, *new_node;

	/* node is now queued */
	if (rs->queued)
		panic ("node is already queued");
	rs->queued = TRUE;

#ifdef PWR_WAKE
#ifdef READY_ROB_STAT
	if (!rs->in_LSQ)
		ready_thrd[rs->threadid]++;
#endif //READY_ROB_STAT
	if (rs->in_LSQ)
	{
		lsqready++;
		lsqready_cl[rs->cluster]++;
		rs->counted_lsqready = 1;
	}
	else if ((MD_OP_FUCLASS (rs->op) > 3) && (MD_OP_FUCLASS (rs->op) < 10))
	{
		fready++;
		fready_cl[rs->cluster]++;
		rs->counted_fready = 1;
	}
	else
	{
		iready++;
		iready_cl[rs->cluster]++;
		rs->counted_iready = 1;
	}
#endif

	/* get a free ready list node */
	RSLINK_NEW (new_node, rs);
	new_node->x.seq = rs->seq;

	/* locate insertion point */
	if (rs->in_LSQ || MD_OP_FLAGS (rs->op) & (F_LONGLAT | F_CTRL))
	{
		/* insert loads/stores and long latency ops at the head of the queue */
		prev = NULL;
		node = ready_queue;
	}
	else
	{
		/* otherwise insert in program order (earliest seq first) */
		for (prev = NULL, node = ready_queue; node && node->x.seq < rs->seq; prev = node, node = node->next);
	}

	if (prev)
	{
		/* insert middle or end */
		new_node->next = prev->next;
		prev->next = new_node;
	}
	else
	{
		/* insert at beginning */
		new_node->next = ready_queue;
		ready_queue = new_node;
	}
}
#endif


/*
 * the create vector maps a logical register to a creator in the RUU (and
 * specific output operand) or the architected register file (if RS_link
 * is NULL)
 */

/* a NULL create vector entry */
struct CV_link CVLINK_NULL = { NULL, 0 };

/* get a new create vector link */
#define CVLINK_INIT(CV, RS,ONUM)	((CV).rs = (RS), (CV).odep_num = (ONUM))

/* size of the create vector (one entry per architected register) */
#define CV_BMAP_SZ              (BITMAP_SIZE(MD_TOTAL_REGS))

/* the create vector, NOTE: speculative copy on write storage provided
   for fast recovery during wrong path execute (see tracer_recover() for
   details on this process */

/* read a create vector entry */
#define CREATE_VECTOR(N)        (BITMAP_SET_P(current->use_spec_cv, CV_BMAP_SZ, (N))\
		? current->spec_create_vector[N]                \
		: current->create_vector[N])

/* read a create vector timestamp entry */
#define CREATE_VECTOR_RT(N)     (BITMAP_SET_P(current->use_spec_cv, CV_BMAP_SZ, (N))\
		? current->spec_create_vector_rt[N]             \
		: current->create_vector_rt[N])

/* set a create vector entry */

#define SET_CREATE_VECTOR(N, L) ( current->spec_mode             \
		? (BITMAP_SET(current->use_spec_cv, CV_BMAP_SZ, (N)),\
			current->spec_create_vector[N] = (L)) \
		: (current->create_vector[N] = (L)))



/*
 * configure the instruction decode engine
 */

#define DNA			(0)

#if defined(TARGET_PISA)

/* general register dependence decoders */
#define DGPR(N)			(N)
#define DGPR_D(N)		((N) &~1)

/* floating point register dependence decoders */
#define DFPR_L(N)		(((N)+32)&~1)
#define DFPR_F(N)		(((N)+32)&~1)
#define DFPR_D(N)		(((N)+32)&~1)

/* miscellaneous register dependence decoders */
#define DHI			(0+32+32)
#define DLO			(1+32+32)
#define DFCC			(2+32+32)
#define DTMP			(3+32+32)

#elif defined(TARGET_ALPHA)

/* general register dependence decoders, $r31 maps to DNA (0) */
#define DGPR(N)			(31 - (N))	/* was: (((N) == 31) ? DNA : (N)) */

/* floating point register dependence decoders */
#define DFPR(N)			(((N) == 31) ? DNA : ((N)+32))

/* miscellaneous register dependence decoders */
#define DFPCR			(0+32+32)
#define DUNIQ			(1+32+32)
#define DTMP			(2+32+32)

#else
#error No ISA target defined...
#endif

/* **************************************************************************   */
/*
 * configure the execution engine
 */
/* next program counter */
#define SET_NPC(EXPR)           (current->regs.regs_NPC = (EXPR))

/* target program counter */
#undef  SET_TPC
#define SET_TPC(EXPR)		(target_PC = (EXPR))

/* current program counter */
#define CPC                     (current->regs.regs_PC)
#define SET_CPC(EXPR)           (current->regs.regs_PC = (EXPR))

/* general purpose register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#ifdef TEST_BUG

#define GPR(N)                  (BITMAP_SET_P(use_spec_R, R_BMAP_SZ, (N))	\
		? current->spec_regs_R[N]             		\
		: current->regs.regs_R[N])


#define SET_GPR(N,EXPR)         (current->spec_mode 				\
		? ((current->spec_regs_R[N] = (EXPR)),		\
			BITMAP_SET(use_spec_R, R_BMAP_SZ, (N)),	\
			current->spec_regs_R[N])			\
		: (current->regs.regs_R[N] = (EXPR)))


#if defined(TARGET_PISA)

/* floating point register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPR_L(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))	\
		? current->spec_regs_F.l[(N)]                  \
		: current->regs.regs_F.l[(N)])


#define SET_FPR_L(N,EXPR)       (current->spec_mode 				\
		? ((current->spec_regs_F.l[(N)] = (EXPR)),	\
			BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),	\
			current->spec_regs_F.l[(N)])		\
		: (current->regs.regs_F.l[(N)] = (EXPR)))


#define FPR_F(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))	\
		? current->spec_regs_F.f[(N)]     		\
		: current->regs.regs_F.f[(N)])


#define SET_FPR_F(N,EXPR)       (current->spec_mode  				\
		? ((current->spec_regs_F.f[(N)] = (EXPR)),	\
			BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),	\
			current->spec_regs_F.f[(N)])		\
		: (current->regs.regs_F.f[(N)] = (EXPR)))


#define FPR_D(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))	\
		? current->spec_regs_F.d[(N) >> 1]      	\
		: current->regs.regs_F.d[(N) >> 1])


#define SET_FPR_D(N,EXPR)       (current->spec_mode  				\
		? ((current->spec_regs_F.d[(N) >> 1] = (EXPR)),\
			BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),	\
			current->spec_regs_F.d[(N) >> 1])		\
		: (current->regs.regs_F.d[(N) >> 1] = (EXPR)))


/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define HI			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, /*hi*/0)	\
		? current->spec_regs_C.hi			\
		: current->regs.regs_C.hi)


#define SET_HI(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.hi = (EXPR)),		\
			BITMAP_SET(use_spec_C, C_BMAP_SZ,/*hi*/0),	\
			current->spec_regs_C.hi)			\
		: (current->regs.regs_C.hi = (EXPR)))


#define LO			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, /*lo*/1)	\
		? current->spec_regs_C.lo			\
		: current->regs.regs_C.lo)


#define SET_LO(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.lo = (EXPR)),		\
			BITMAP_SET(use_spec_C, C_BMAP_SZ,/*lo*/1),	\
			current->spec_regs_C.lo)			\
		: (current->regs.regs_C.lo = (EXPR)))


#define FCC			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fcc*/2)	\
		? current->spec_regs_C.fcc			\
		: current->regs.regs_C.fcc)


#define SET_FCC(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.fcc = (EXPR)),	\
			BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fcc*/2),	\
			current->spec_regs_C.fcc)			\
		: (current->regs.regs_C.fcc = (EXPR)))


#elif defined(TARGET_ALPHA)

/* floating point register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPR_Q(N)		(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N))	\
		? current->spec_regs_F.q[(N)]                  \
		: current->regs.regs_F.q[(N)])


#define SET_FPR_Q(N,EXPR)	(current->spec_mode 				\
		? ((current->spec_regs_F.q[(N)] = (EXPR)),	\
			BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)),	\
			current->spec_regs_F.q[(N)])		\
		: (current->regs.regs_F.q[(N)] = (EXPR)))


#define FPR(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N))	\
		? current->spec_regs_F.d[(N)]			\
		: current->regs.regs_F.d[(N)])


#define SET_FPR(N,EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_F.d[(N)] = (EXPR)),	\
			BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)),	\
			current->spec_regs_F.d[(N)])		\
		: (current->regs.regs_F.d[(N)] = (EXPR)))


/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPCR			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fpcr*/0)	\
		? current->spec_regs_C.fpcr			\
		: current->regs.regs_C.fpcr)


#define SET_FPCR(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.fpcr = (EXPR)),	\
			BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fpcr*/0),	\
			current->spec_regs_C.fpcr)			\
		: (current->regs.regs_C.fpcr = (EXPR)))


#define UNIQ			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*uniq*/1)	\
		? current->spec_regs_C.uniq			\
		: current->regs.regs_C.uniq)


#define SET_UNIQ(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.uniq = (EXPR)),	\
			BITMAP_SET(use_spec_C,C_BMAP_SZ,/*uniq*/1),	\
			current->spec_regs_C.uniq)			\
		: (current->regs.regs_C.uniq = (EXPR)))


#define FCC			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fcc*/2)	\
		? current->spec_regs_C.fcc			\
		: current->regs.regs_C.fcc)


#define SET_FCC(EXPR)		(current->spec_mode 			\
		? ((current->spec_regs_C.fcc = (EXPR)),	\
			BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fcc*/1),	\
			current->spec_regs_C.fcc)			\
		: (current->regs.regs_C.fcc = (EXPR)))


#else
#error No ISA target defined...
#endif

#else
#define GPR(N)                  (BITMAP_SET_P(current->use_spec_R, R_BMAP_SZ, (N))	\
		? current->spec_regs_R[N]                       	\
		: current->regs.regs_R[N])


#define SET_GPR(N,EXPR)         (current->spec_mode 		\
		? ((current->spec_regs_R[N] = (EXPR)),			\
			BITMAP_SET(current->use_spec_R, R_BMAP_SZ, (N)),	\
			current->spec_regs_R[N])				\
		: (current->regs.regs_R[N] = (EXPR)))


#if defined(TARGET_PISA)

/* floating point register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPR_L(N)                (BITMAP_SET_P(current->use_spec_F, F_BMAP_SZ, ((N)&~1))\
		? current->spec_regs_F.l[(N)]                   \
		: current->regs.regs_F.l[(N)])


#define SET_FPR_L(N,EXPR)       (current->spec_mode 				\
		? ((current->spec_regs_F.l[(N)] = (EXPR)),	\
			BITMAP_SET(current->use_spec_F,F_BMAP_SZ,((N)&~1)),\
			current->spec_regs_F.l[(N)])			\
		: (current->regs.regs_F.l[(N)] = (EXPR)))


#define FPR_F(N)                (BITMAP_SET_P(current->use_spec_F, F_BMAP_SZ, ((N)&~1))\
		? current->spec_regs_F.f[(N)]                   \
		: current->regs.regs_F.f[(N)])


#define SET_FPR_F(N,EXPR)       (current->spec_mode 				\
		? ((current->spec_regs_F.f[(N)] = (EXPR)),	\
			BITMAP_SET(current->use_spec_F,F_BMAP_SZ,((N)&~1)),\
			current->spec_regs_F.f[(N)])			\
		: (current->regs.regs_F.f[(N)] = (EXPR)))


#define FPR_D(N)                (BITMAP_SET_P(current->use_spec_F, F_BMAP_SZ, ((N)&~1))\
		? current->spec_regs_F.d[(N) >> 1]              \
		: current->regs.regs_F.d[(N) >> 1])


#define SET_FPR_D(N,EXPR)       (current->spec_mode 			\
		? ((current->spec_regs_F.d[(N) >> 1] = (EXPR)),	\
			BITMAP_SET(current->use_spec_F,F_BMAP_SZ,((N)&~1)),\
			current->spec_regs_F.d[(N) >> 1])		\
		: (current->regs.regs_F.d[(N) >> 1] = (EXPR)))


/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define HI			(BITMAP_SET_P(current->use_spec_C, C_BMAP_SZ, /*hi*/0)\
		? current->spec_regs_C.hi			\
		: current->regs.regs_C.hi)


#define SET_HI(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.hi = (EXPR)),		\
			BITMAP_SET(current->use_spec_C, C_BMAP_SZ,/*hi*/0),\
			current->spec_regs_C.hi)			\
		: (current->regs.regs_C.hi = (EXPR)))


#define LO			(BITMAP_SET_P(current->use_spec_C, C_BMAP_SZ, /*lo*/1)\
		? current->spec_regs_C.lo			\
		: current->regs.regs_C.lo)


#define SET_LO(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.lo = (EXPR)),		\
			BITMAP_SET(current->use_spec_C, C_BMAP_SZ,/*lo*/1),\
			current->spec_regs_C.lo)			\
		: (current->regs.regs_C.lo = (EXPR)))


#define FCC			(BITMAP_SET_P(current->use_spec_C, C_BMAP_SZ,/*fcc*/2)\
		? current->spec_regs_C.fcc			\
		: current->regs.regs_C.fcc)


#define SET_FCC(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.fcc = (EXPR)),		\
			BITMAP_SET(current->use_spec_C,C_BMAP_SZ,/*fcc*/2),\
			current->spec_regs_C.fcc)			\
		: (current->regs.regs_C.fcc = (EXPR)))


#elif defined(TARGET_ALPHA)

/* floating point register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPR_Q(N)		(BITMAP_SET_P(current->use_spec_F, F_BMAP_SZ, (N))\
		? current->spec_regs_F.q[(N)]                   \
		: current->regs.regs_F.q[(N)])


#define SET_FPR_Q(N,EXPR)	(current->spec_mode 				\
		? ((current->spec_regs_F.q[(N)] = (EXPR)),	\
			BITMAP_SET(current->use_spec_F,F_BMAP_SZ, (N)),\
			current->spec_regs_F.q[(N)])			\
		: (current->regs.regs_F.q[(N)] = (EXPR)))


#define FPR(N)			(BITMAP_SET_P(current->use_spec_F, F_BMAP_SZ, (N))\
		? current->spec_regs_F.d[(N)]			\
		: current->regs.regs_F.d[(N)])


#define SET_FPR(N,EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_F.d[(N)] = (EXPR)),	\
			BITMAP_SET(current->use_spec_F,F_BMAP_SZ, (N)),\
			current->spec_regs_F.d[(N)])			\
		: (current->regs.regs_F.d[(N)] = (EXPR)))


/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPCR			(BITMAP_SET_P(current->use_spec_C, C_BMAP_SZ,/*fpcr*/0)\
		? current->spec_regs_C.fpcr			\
		: current->regs.regs_C.fpcr)


#define SET_FPCR(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.fpcr = (EXPR)),	\
			BITMAP_SET(current->use_spec_C,C_BMAP_SZ,/*fpcr*/0),\
			current->spec_regs_C.fpcr)			\
		: (current->regs.regs_C.fpcr = (EXPR)))


#define UNIQ			(BITMAP_SET_P(current->use_spec_C, C_BMAP_SZ,/*uniq*/1)\
		? current->spec_regs_C.uniq			\
		: current->regs.regs_C.uniq)


#define SET_UNIQ(EXPR)		(current->spec_mode 				\
		? ((current->spec_regs_C.uniq = (EXPR)),	\
			BITMAP_SET(current->use_spec_C,C_BMAP_SZ,/*uniq*/1),\
			current->spec_regs_C.uniq)			\
		: (current->regs.regs_C.uniq = (EXPR)))


#define FCC			(BITMAP_SET_P(current->use_spec_C, C_BMAP_SZ,/*fcc*/2)\
		? current->spec_regs_C.fcc			\
		: current->regs.regs_C.fcc)


#define SET_FCC(EXPR)		(current->spec_mode 			\
		? ((current->spec_regs_C.fcc = (EXPR)),		\
			BITMAP_SET(current->use_spec_C,C_BMAP_SZ,/*fcc*/1),\
			current->spec_regs_C.fcc)			\
		: (current->regs.regs_C.fcc = (EXPR)))


#else
#error No ISA target defined...
#endif

#endif


/* precise architected memory state accessor macros, NOTE: speculative copy on
   write storage provided for fast recovery during wrong path execute (see
   tracer_recover() for details on this process */
#define __READ_SPECMEM(SRC, SRC_V, FAULT)				\
	(addr = (SRC),							\
	 (current->spec_mode 								\
	  ? ((FAULT) = spec_mem_access(current->mem, Read, addr, &SRC_V, sizeof(SRC_V)))\
	  : ((FAULT) = WB_access1(current->mem, Read, addr, &SRC_V, sizeof(SRC_V), current->id, inst))),	\
	 SRC_V)


#define READ_BYTE(SRC, FAULT)                                           \
	__READ_SPECMEM((SRC), temp_byte, (FAULT))
#define READ_HALF(SRC, FAULT)                                           \
	MD_SWAPH(__READ_SPECMEM((SRC), temp_half, (FAULT)))
#define READ_WORD(SRC, FAULT)                                           \
	MD_SWAPW(__READ_SPECMEM((SRC), temp_word, (FAULT)))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)                                          \
	MD_SWAPQ(__READ_SPECMEM((SRC), temp_qword, (FAULT)))
#endif /* HOST_HAS_QWORD */

#define __WRITE_SPECMEM(SRC, DST, DST_V, FAULT)				\
	(DST_V = (SRC), addr = (DST),						\
	 (current->spec_mode  								\
	  ? ((FAULT) = spec_mem_access(current->mem, Write, addr, &DST_V, sizeof(DST_V)))\
	  : ((FAULT) = WB_access1(current->mem, Write, addr, &DST_V, sizeof(DST_V), current->id, inst))))

#define WRITE_BYTE(SRC, DST, FAULT)                                     \
	__WRITE_SPECMEM((SRC), (DST), temp_byte, (FAULT))
#define WRITE_HALF(SRC, DST, FAULT)                                     \
	__WRITE_SPECMEM(MD_SWAPH(SRC), (DST), temp_half, (FAULT))
#define WRITE_WORD(SRC, DST, FAULT)                                     \
	__WRITE_SPECMEM(MD_SWAPW(SRC), (DST), temp_word, (FAULT))

#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)                                    \
	__WRITE_SPECMEM(MD_SWAPQ(SRC), (DST), temp_qword, (FAULT))
#endif /* HOST_HAS_QWORD */

/* system call handler macro */
#define SYSCALL(INST)							\
	(/* only execute system calls in non-speculative mode */		\
	 (current->spec_mode  ? panic("speculative syscall") : (void) 0),		\
	 sys_syscall(&(current->regs), mem_access, current->mem, INST, TRUE))


#define QUIESCE(INST)							\
	(									\
										(current->spec_mode  ? panic("speculative quiescent instruction") : (void) 0),	\
										call_quiesce(current->id))


/* initialize the create vector */

	void
cv_init (int threadid)
{
	int i;
	context *current;

	current = thecontexts[threadid];

	/* initially all registers are valid in the architected register file,
	   i.e., the create vector entry is CVLINK_NULL */
	for (i = 0; i < MD_TOTAL_REGS; i++)
	{
		current->create_vector[i] = CVLINK_NULL;
		current->create_vector_rt[i] = 0;
		current->spec_create_vector[i] = CVLINK_NULL;
		current->spec_create_vector_rt[i] = 0;
	}

	/* all create vector entries are non-speculative */
	BITMAP_CLEAR_MAP (current->use_spec_cv, CV_BMAP_SZ);

}

/* dump the contents of the create vector */

	static void
cv_dump (FILE * stream, int threadid)	/* output stream */
{
	int i;
	struct CV_link ent;

	context *current;

	current = thecontexts[threadid];

	if (!stream)
		stream = stderr;

	fprintf (stream, "** create vector state **\n");

	for (i = 0; i < MD_TOTAL_REGS; i++)
	{
		ent = CREATE_VECTOR (i);
		if (!ent.rs)
			fprintf (stream, "[cv%02d]: from architected reg file\n", i);
		else
			fprintf (stream, "[cv%02d]: from %s, idx: %d\n", i, (ent.rs->in_LSQ ? "LSQ" : "RUU"), ent.rs->index);
	}
}

static void ruu_recover (int branch_index, int threadid, int mode);

/* forward declarations */
static void tracer_recover (int threadid);
void recFreeRegList (int threadid);


void RS_block_list(struct RUU_station *rs, tick_t when, int entry, int threadid)
{
	if(rs->spec_mode)
		return;
	if(entry < 1 || entry > MSHR_SIZE)
		panic("L1 MSHR: wrong mshr entry");

	entry = entry - 1;

	struct RS_list *rs_list;
	rs_list = calloc( 1, sizeof(struct RS_list));
	rs_list->rs = rs;
	rs_list->next = NULL;
	rs_list->spec_mode = rs->spec_mode;
	rs_list->when = when;
	rs_list->next = rs_cache_list[threadid][entry];
	rs_cache_list[threadid][entry] = rs_list;
}

void RS_block_next(struct RS_list *cur)
{
	if(!cur->rs) return;

	if((cur->spec_mode)&& !cur->rs->spec_mode )
		return;
	cur->rs->cache_ready = 0;
	if(((MD_OP_FLAGS(cur->rs->op) & F_LOAD) && (cur->rs->issued || cur->rs->queued || cur->rs->completed)) || (!(MD_OP_FLAGS(cur->rs->op) & F_LOAD) && !(cur->rs->issued && !cur->rs->queued)))
		panic("L1 MSHR: issue, queue, or commit flag is set!!!");

	if((MD_OP_FLAGS(cur->rs->op) & F_LOAD))
	{
		//cur->rs->issued = 1;
		//eventq_queue_event (cur->rs, sim_cycle + 1, 0);
		readyq_enqueue (cur->rs);

		//cur->rs->issue_time = sim_cycle;
		//cur->rs->finish_time = sim_cycle+1;

		/* entered execute stage, indicate in pipe trace */
		//ptrace_newstage (cur->rs->ptrace_seq, PST_EXECUTE, PEV_CACHEMISS);
	}
	else
		cur->rs->writewait = 2;
}


/*
 *  RUU_COMMIT() - instruction retirement pipeline stage
 */
static double commit_idle = 0;
int commit_flag[MAXTHREADS] = {0};
/* this function commits the results of the oldest completed entries from the
   RUU and LSQ to the architected reg file, stores in the LSQ will commit
   their store data to the data cache at this point as well */
	static void
ruu_commit (void)
{

	int i, j, threadid, lat, events, committed;
	int mm;
	context *current;
	int temp_LSQ_index, temp_LSQ_num, LSQ_index;
	struct RUU_station *temp_LSQ_rs;
	int store_op = 0, pcbSearch;
	struct RS_link *olink, *olink_next;
	int readShareDir = 0;

#if defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
	int countedThisCycle = 0;
#endif
	md_addr_t tempAddr;
	enum md_opcode tempOp;

	thread_commit = 0;
	int isSTLC;


	/*   loop over all threads looking for completed instructions to commit  */
	for (j = 0; j <= numcontexts; j++, thread_commit = (thread_commit + 1) % numcontexts)
	{

		if (j > 0)
		{
			commit_done[j - 1] = committed;
			if(!committed)
				m_commitStall[j-1]++;
		}
		if (j == numcontexts)
			break;

		threadid = thread_commit;
		current = thecontexts[threadid];

		/* stall the commit because of uncertain prefetch --hq*/
//		if (current->pref_mode || current->waitForPrefetchInfo)
//		    break;

		temp_LSQ_num = current->LSQ_num;
		LSQ_index = current->LSQ_head;
		committed = 0;
		int haha = 0;


		/* all values must be retired to the architected reg file in program order */
		while (current->RUU_num > 0 && committed < ruu_commit_width /*add by ctb&& current->active_this_cycle*/)
		{
#if defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
			if (busSlotCount < 11)
			{
				if (!countedThisCycle)
					stallNoBusSlot++;
				countedThisCycle = 1;
				break;
			}
#endif
			struct RUU_station *rs = &(current->RUU[current->RUU_head]);


			tempAddr = 0;
			store_op = 0;
			tempOp = 0;
			isSTLC = 0;

			if(rs->writewait == 1)
			{
				break;
			}
			if (!rs->completed)
			{
				break;
			}

			if (current->masterid == 0 && rs->PC < 0x120000000)
				panic ("Illegal instruction trying to commit\n");


			/* Stall on memory fence untill all the stores are completed. */
			if((MD_OP_FLAGS(rs->op) & F_FENCE) && (m_L1WBufCnt[threadid]))
				break;

			/* default commit events */
			events = 0;

			/* load/stores must retire load/store queue entry as well */
			if (current->RUU[current->RUU_head].ea_comp)
			{

				while (current->LSQ[LSQ_index].threadid != current->RUU[current->RUU_head].threadid)
				{
					LSQ_index = (LSQ_index + 1) % LSQ_size;
					temp_LSQ_num--;
					if (!temp_LSQ_num)
						panic ("RUU and LSQ out of sync");
				}
				if (current->LSQ_num <= 0 || !current->LSQ[LSQ_index].in_LSQ)
					panic ("RUU out of sync with LSQ");
				/* load/store operation must be complete */
				if (!current->LSQ[LSQ_index].completed)
				{
					break;
				}

#ifdef SEQUENTIAL_CONSISTENCY
				if (invalidationStall[threadid])
				{
					break;
				}
				if (commitStall[threadid])
				{
					break;
				}

				if (((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE)) && (pending_invalidation[threadid]))
					break;

#endif
				if ((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE))
				{
					if (current->LSQ[LSQ_index].op == STL_C)
						isSTLC = 1;

					if (m_L1WBufCnt[threadid] >= SMD_L1_WBUF_SIZE)	//No space left in WB. WB commit stall.
						break;
					if(isSTLC && m_L1WBufCnt[threadid])
						break;
					else
					{/* Commit the store into write buffer.*/
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].PC = current->LSQ[LSQ_index].PC;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].IR = current->LSQ[LSQ_index].IR;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].op = current->LSQ[LSQ_index].op;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].lsqId = LSQ_index;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].robId = rs->index;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].writeValue = current->LSQ[LSQ_index].val_ra;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].addr = current->LSQ[LSQ_index].addr;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].sim_num_insn = current->LSQ[LSQ_index].sim_num_insn;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].writeStarted = 0;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].finishTime = 0;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].isMiss = 0;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].entryTime = sim_cycle;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].STL_C_fail = 0;
						m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].already_check = 0;
						if(/*!isSTLC ||*/ isSTLC && !isCacheHit(cache_dl1[threadid], current->LSQ[LSQ_index].addr, threadid))
						{/* Complete Store must fail for a missed STL_C instruction */
							completeStore(threadid, current->LSQ[LSQ_index].addr, GetMemAccWidth(current->LSQ[LSQ_index].op), isSTLC);
							m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].commitDone = 1;
							if(isSTLC && storeCondFail == 0)
								panic("Store conditional should fail on a L1 miss");
						}
						else
							m_L1WBuf[threadid][m_L1WBuf_tail[threadid]].commitDone = 0;

						m_L1WBufCnt[threadid]++;
						m_L1WBuf_tail[threadid] = (m_L1WBuf_tail[threadid]+1)%SMD_L1_WBUF_SIZE;
					}

				    }
//				} /*i added --hq */
                else		//load
				{
					readShareDir = current->LSQ[LSQ_index].ReadShareDir;

#if defined(SMD_USE_WRITE_BUF) && defined(SEQUENTIAL_CONSISTENCY)
					/* Load can-not be committed until all the stores from the write buffer are done. */
					if (m_L1WBufCnt[threadid])
						break;	//There is committed store in WB waiting to writeback
#endif
				}
				if(rs->writewait != 1)
				{
					freelist_insert (current->LSQ[LSQ_index].oldpreg, threadid);

					current->LSQ[LSQ_index].tag++;

					/* indicate to pipeline trace that this instruction retired */
					ptrace_newstage (current->LSQ[LSQ_index].ptrace_seq, PST_COMMIT, events);
					ptrace_endinst (current->LSQ[LSQ_index].ptrace_seq);

					if (LSQ_index != current->LSQ_head)
					{
						temp_LSQ_rs = &current->LSQ[LSQ_index];
						temp_LSQ_index = LSQ_index;
						while (temp_LSQ_index != current->LSQ_head)
						{
							current->LSQ[temp_LSQ_index] = current->LSQ[(int) ((temp_LSQ_index + (LSQ_size - 1)) % LSQ_size)];
							current->LSQ[temp_LSQ_index].index = temp_LSQ_index;
							temp_LSQ_index = (temp_LSQ_index + (LSQ_size - 1)) % LSQ_size;
						}

						current->LSQ[temp_LSQ_index] = *temp_LSQ_rs;
						current->LSQ[temp_LSQ_index].index = temp_LSQ_index;
					}
					if ((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD))
					{
						if (!current->LSQ[LSQ_index].isPrefetch)
							current->m_shLQNum--;
					}
					else if ((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE))
					{
						if (!current->LSQ[LSQ_index].isPrefetch)
							current->m_shSQNum--;
					}
					if(ruu_inorder_issue)
					{
						for(i=0;i<MAX_ODEPS;i++)
						{
							if (current->LSQ[LSQ_index].onames[i] != NA)
							{
								for (olink = current->LSQ[LSQ_index].odep_list[i]; olink; olink = olink_next)
								{
									olink_next = olink->next;
									RSLINK_FREE (olink);
								}
								current->LSQ[LSQ_index].odep_list[i] = NULL;
							}
						}
					}

					current->LSQ_head = (current->LSQ_head + 1) % LSQ_size;
					LSQ_index = (LSQ_index + 1) % LSQ_size;
					current->LSQ_num--;
					temp_LSQ_num--;
				}

			} //if (current->RUU[current->RUU_head].ea_comp)

			if(rs->writewait != 1)
			{
				if (current->pred && bpred_spec_update == spec_CT && (MD_OP_FLAGS (rs->op) & F_CTRL) && !rs->duplicate	//092904mwr: only for original code
				   )
				{
					bpred_access++;	/*mwr: was already here */
					if (!(rs->duplicate))
					{
						current->bpred_access++;
					}

					bpred_update (current->pred,
							/* branch address */ rs->PC,
							/* actual target address */ rs->next_PC,
							/* taken? */ rs->next_PC != (rs->PC +
								sizeof (md_inst_t)),
							/* pred taken? */ rs->pred_PC != (rs->PC +
								sizeof (md_inst_t)),
							/* correct pred? */ rs->pred_PC == rs->next_PC,
							/* opcode */ rs->op,
							/* dir predictor update pointer */ &rs->dir_update,
							/* thread id */ rs->threadid,
							/* thread id for BTB */ (bpred_btb_use_masterid ? thecontexts[rs->threadid]->masterid : rs->threadid));
				}


				freelist_insert (current->RUU[current->RUU_head].oldpreg, threadid);

				current->RUU[current->RUU_head].tag++;

				ptrace_newstage (current->RUU[current->RUU_head].ptrace_seq, PST_COMMIT, events);
				ptrace_endinst (current->RUU[current->RUU_head].ptrace_seq);

				total_slip_cycles += (sim_cycle - rs->disp_time);
				max_slip_cycles = MAX ((sim_cycle - rs->disp_time), max_slip_cycles);

				if (rs->issue_time)
				{
					total_issue_slip_cycles += (sim_cycle - rs->issue_time);
					max_issue_slip_cycles = MAX ((sim_cycle - rs->issue_time), max_issue_slip_cycles);
				}

				if(ruu_inorder_issue)
				{
					for(i=0;i<MAX_ODEPS;i++)
					{
						if (rs->onames[i] != NA)
						{
							for (olink = rs->odep_list[i]; olink; olink = olink_next)
							{
								olink_next = olink->next;
								RSLINK_FREE (olink);
							}
							rs->odep_list[i] = NULL;
						}
					}
				}

				current->RUU_head = (current->RUU_head + 1) % RUU_size;
				current->RUU_num--;
				commit_flag[threadid] = 0;

				if (rs->duplicate)
				{
					COMPARE_access[threadid]++;
					compare_access++;
				}

				if(!ruu_inorder_issue)
				{
					rob2_access++;
					current->rob2_access++;
				}
				committed++;

				commit_idle = 0;
				for (i = 0; i < MAX_ODEPS; i++)
				{
					if (rs->odep_list[i])
						panic ("retired instruction has odeps\n");
				}
			}
		}			// while RUU_num >0 .........
		stat_add_sample (commit_width_stat, committed);
	 }				// for(threadid=0......
}


/*
 *  RUU_RECOVER() - squash mispredicted microarchitecture state
 */

/* recover processor microarchitecture state back to point of the
   mis-predicted branch at RUU[BRANCH_INDEX] */


	static void
ruu_recover (int branch_index, int threadid, int mode)	/* index of mis-pred branch */
{

	int mm;
	context *current = thecontexts[threadid];
	int i, RUU_index, LSQ_index, temp_RUU_num;

	struct RUU_station *temp_LSQ_rs;
	int temp_LSQ_index, temp_LSQ_num;

	/*lechen */
	int shift_right = 0;	/*how many bits history will need to shift right */


	brRecovery[threadid]++;

	disp_delay = 0;

	/* go to first element to squash */
	RUU_index = current->RUU_head;
	temp_RUU_num = current->RUU_num;
	LSQ_index = current->LSQ_head;
	temp_LSQ_num = current->LSQ_num;

	/* the RUU should not drain since the mispredicted branch will remain */
	if (!current->RUU_num)
		panic ("empty RUU");

	/* search the LSQ to find the matched instruction with the one after
	   mispredicted branch   */
	while (RUU_index != branch_index)
	{
		/* the RUU should not drain since the mispredicted branch will remain */
		if (!temp_RUU_num)
			panic ("empty RUU");

		/* load/stores must retire load/store queue entry as well */
		if (current->RUU[RUU_index].ea_comp)
		{
			while (current->LSQ[LSQ_index].threadid != current->RUU[RUU_index].threadid)
			{
				/* go to next earlier LSQ slot */
				LSQ_index = (LSQ_index + 1) % LSQ_size;
				temp_LSQ_num--;
				if (!temp_LSQ_num)
				{
					fprintf (stderr, "Thread %d, cycle %lu\n", threadid, (unsigned long) sim_num_insn);
					panic ("RUU and LSQ out of sync");
				}
			}
			LSQ_index = (LSQ_index + 1) % LSQ_size;
			temp_LSQ_num--;
		}
		RUU_index = (RUU_index + 1) % RUU_size;
		temp_RUU_num--;
	}

	if (mode == 0)
	{
		/* start squash from the instruction next to the mispredicted branch */
		RUU_index = (RUU_index + 1) % RUU_size;
		temp_RUU_num--;
	}

	/* traverse to earlier insts until the tail is encountered */
	//while (RUU_index != current->RUU_tail)
	while (temp_RUU_num != 0)
	{
		extraInsn[threadid]++;
		/* the RUU should not drain since the mispredicted branch will remain */
		if (!temp_RUU_num)
			panic ("empty RUU");
		/* should meet up with the tail first */
		if (mode == 0 && RUU_index == current->RUU_head)
			panic ("RUU head and tail broken");

		if (current->RUU[RUU_index].counted_iissueq)
		{
			current->iissueq_thrd--;
			iissueq--;
			iissueq_cl[current->id]--;
			current->RUU[RUU_index].counted_iissueq = 0;
		}
		if (current->RUU[RUU_index].counted_fissueq)
		{
			current->fissueq_thrd--;
			fissueq--;
			fissueq_cl[current->id]--;
			current->RUU[RUU_index].counted_fissueq = 0;
		}

		if (current->RUU[RUU_index].ea_comp)
		{
			if (!temp_LSQ_num)
				panic ("RUU and LSQ out of sync");
			while (current->LSQ[LSQ_index].threadid != current->RUU[RUU_index].threadid)
			{
				LSQ_index = (LSQ_index + 1) % LSQ_size;
				temp_LSQ_num--;
				if (!temp_LSQ_num)
					panic ("RUU and LSQ out of sync");
			}

			if (current->LSQ[LSQ_index].counted_iissueq)
			{
				current->iissueq_thrd--;
				iissueq--;
				iissueq_cl[current->id]--;
				current->LSQ[LSQ_index].counted_iissueq = 0;
			}
			if (current->LSQ[LSQ_index].counted_fissueq)
			{
				current->fissueq_thrd--;
				fissueq--;
				fissueq_cl[current->id]--;
				current->LSQ[LSQ_index].counted_fissueq = 0;
			}

			for (i = 0; i < MAX_ODEPS; i++)
			{
				RSLINK_FREE_LIST (current->LSQ[LSQ_index].odep_list[i]);
				/* blow away the consuming op list */
				current->LSQ[LSQ_index].odep_list[i] = NULL;
			}

			freelist_insert (current->LSQ[LSQ_index].opreg, threadid);
			changeInEventQueue(&current->LSQ[LSQ_index], threadid);

			/* squash this LSQ entry */
			current->LSQ[LSQ_index].tag++;

			/* indicate in pipetrace that this instruction was squashed */
			ptrace_endinst (current->LSQ[LSQ_index].ptrace_seq);

			/* go to previous older LSQ slot and move the head too toward tail one slot */
			if ((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD))
			{
				if (!current->LSQ[LSQ_index].isPrefetch)
					current->m_shLQNum--;
			}
			else if ((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE))
			{
				if (!current->LSQ[LSQ_index].isPrefetch)
					current->m_shSQNum--;
			}

			current->LSQ_tail = (current->LSQ_tail + LSQ_size - 1) % LSQ_size;
			LSQ_index = (LSQ_index + 1) % LSQ_size;
			current->LSQ_num--;
			temp_LSQ_num--;
		}

		/* recover any resources used by this RUU operation */
		for (i = 0; i < MAX_ODEPS; i++)
		{
			RSLINK_FREE_LIST (current->RUU[RUU_index].odep_list[i]);
			current->RUU[RUU_index].odep_list[i] = NULL;
		}

		freelist_insert (current->RUU[RUU_index].opreg, threadid);
		changeInEventQueue(&current->RUU[RUU_index], threadid);
		current->RUU[RUU_index].tag++;
		ptrace_endinst (current->RUU[RUU_index].ptrace_seq);

		RUU_index = (RUU_index + 1) % RUU_size;
		current->RUU_num--;
		temp_RUU_num--;
	}

	if(!ruu_inorder_issue)
	{
		rob2_access++;
		current->rob2_access++;
	}

	instseqnum = current->RUU[branch_index].instnum;
	lastcluster = current->RUU[branch_index].cluster;
	num_sent = 1;

	if (mode == 0)
	{
		current->RUU_tail = (branch_index + 1) % RUU_size;
		copy_from (threadid);
	}
	else
	{
		if (current->RUU_num != 0)
			panic ("ROB must be completely empty");
		current->RUU_tail = current->RUU_head;
	}

	/* revert create vector back to last precise create vector state, NOTE:
	   this is accomplished by resetting all the copied-on-write bits in the
	   USE_SPEC_CV bit vector */
	BITMAP_CLEAR_MAP (current->use_spec_cv, CV_BMAP_SZ);

	/* FIXME: could reset functional units at squash time */

	/* checkup for the consistancy of the index of LSQ */
	for (temp_LSQ_index = 0; temp_LSQ_index < LSQ_size; temp_LSQ_index++)
		if (temp_LSQ_index != current->LSQ[temp_LSQ_index].index)
			panic ("LSQ index messed up");
}


/*
 *  RUU_WRITEBACK() - instruction result writeback pipeline stage
 */

/* writeback completed operation results from the functional units to RUU,
   at this point, the output dependency chains of completing instructions
   are also walked to determine if any dependent instruction now has all
   of its register operands, if so the (nearly) ready instruction is inserted
   into the ready instruction queue */

	static void
ruu_writeback (void)
{
	int i;
	int sum1, sum2, sum3;
	struct RUU_station *rs;
	int delay;

	context *current;

	/*011305mwr */

	int temp;

	for (temp = 0; temp < numcontexts; temp++)
	{
		wb_done[temp] = 0;
	}

	/* service all completed events */
	while ((rs = eventq_next_event ()))
	{
		

		current = thecontexts[rs->threadid];

		if (!OPERANDS_READY (rs) || rs->queued || !rs->issued || rs->completed)
			panic ("inst completed and !ready, !issued, or completed");

		rs->completed = TRUE;

		wb_done[rs->threadid]++;


		if (!(MD_OP_FLAGS (rs->op) & F_CTRL))
		{
			if ((rs->out1 > 0) && (rs->out1 < 32))
			{
				iresultbus_access++;
				current->iresultbus_access++;
				if(!ruu_inorder_issue)
				{
					rob1_access++;
					current->rob1_access++;
					iwakeup_access++;
					current->iwakeup_access++;
				}
				iregfile_access++;
				current->iregfile_access++;
			}
			if ((rs->out1 > 31) && (rs->out1 < 63))
			{

				fresultbus_access++;
				current->fresultbus_access++;
				if(!ruu_inorder_issue)
				{
					rob1_access++;
					current->rob1_access++;
					fwakeup_access++;
					current->fwakeup_access++;
				}
				fregfile_access++;
				current->fregfile_access++;
			}
		}

		/* does this operation reveal a mis-predicted branch? */
		if (rs->recover_inst)
		{
			if (rs->in_LSQ)
				panic ("mis-predicted load or store?!?!?");

			ruu_recover (rs - current->RUU, rs->threadid, 0);
			tracer_recover (rs->threadid);
			bpred_recover (current->pred, rs->PC, rs->stack_recover_idx, rs->threadid);
			int threadid = rs->threadid;
			int i = 0;
			for(i=0;i<MSHR_SIZE;i++)
			{
				struct RS_list *prev, *cur;
				for(prev=NULL, cur = rs_cache_list[threadid][i]; cur; cur=cur->next)
				{
					if(cur->rs && cur->rs->spec_mode )
					{
						if(prev == NULL)
						{
							rs_cache_list[threadid][i] = cur->next;
							prev = NULL;
						}
						else
						{
							prev->next = cur->next;
							cur = prev;
							prev = cur;
						}
					}
					else
						prev = cur;

				}
			}
			if(current->ruu_fetch_issue_delay < WAIT_TIME/2)
				current->ruu_fetch_issue_delay = ruu_branch_penalty;	// * current->latency_factor;
		}
		if (current->pred && bpred_spec_update == spec_WB && !rs->in_LSQ && (MD_OP_FLAGS (rs->op) & F_CTRL) && !rs->duplicate	//092904mwr: do this only for original inst
		   )
		{
			bpred_access++;	/*mwr: was already here */
			if (!(rs->duplicate))
			{
				current->bpred_access++;
			}
			bpred_update (current->pred,
					/* branch address */ rs->PC,
					/* actual target address */ rs->next_PC,
					/* taken? */ rs->next_PC != (rs->PC +
						sizeof (md_inst_t)),
					/* pred taken? */ rs->pred_PC != (rs->PC +
						sizeof (md_inst_t)),
					/* correct pred? */ rs->pred_PC == rs->next_PC,
					/* opcode */ rs->op,
					/* dir predictor update pointer */ &rs->dir_update,
					/* thread id */ rs->threadid,
					/* thread id for BTB */ (bpred_btb_use_masterid ? thecontexts[rs->threadid]->masterid : rs->threadid));
		}

		/* entered writeback stage, indicate in pipe trace */
		ptrace_newstage (rs->ptrace_seq, PST_WRITEBACK, rs->recover_inst ? PEV_MPDETECT : 0);

		sum1 = issue_rate;
		sum2 = 0;
		sum3 = 0;

		for (i = 0; i < MAX_ODEPS; i++)
		{
			if (rs->onames[i] != NA)
			{
				struct CV_link link;
				struct RS_link *olink, *olink_next;

				if (rs->spec_mode)
				{
					link = current->spec_create_vector[rs->onames[i]];
					if (link.rs && (link.rs == rs && link.odep_num == i))
					{
						current->spec_create_vector[rs->onames[i]] = CVLINK_NULL;
						current->spec_create_vector_rt[rs->onames[i]] = sim_cycle;
					}
				}
				else
				{
					link = current->create_vector[rs->onames[i]];
					if (link.rs && (link.rs == rs && link.odep_num == i))
					{
						current->create_vector[rs->onames[i]] = CVLINK_NULL;
						current->create_vector_rt[rs->onames[i]] = sim_cycle;
					}
				}
				for (olink = rs->odep_list[i]; olink; olink = olink_next)
				{
					if (RSLINK_VALID (olink))
					{
						if (olink->rs->idep_ready[olink->x.opnum])
							panic ("output dependence already satisfied");
						olink->rs->idep_ready[olink->x.opnum] = TRUE;

						olink->rs->when_ready[olink->x.opnum] = sim_cycle;
						olink->rs->when_idep_ready[olink->x.opnum] = sim_cycle;

						sum2++;
						if (OPERANDS_READY (olink->rs))
						{
							if (!olink->rs->in_LSQ || ((MD_OP_FLAGS (olink->rs->op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE)))
								readyq_enqueue (olink->rs);
							sum3++;
						}
					}
					olink_next = olink->next;
					if(!ruu_inorder_issue)
						RSLINK_FREE (olink);
				}
				if(!ruu_inorder_issue)
					rs->odep_list[i] = NULL;
			}			/* if not NA output */
		}			/* for all outputs */
	}				/* for all writeback events */
}

/*
 *  LSQ_REFRESH() - memory access dependence checker/scheduler
 */

/* this function locates ready instructions whose memory dependencies have
   been satisfied, this is accomplished by walking the LSQ for loads, looking
   for blocking memory dependency condition (e.g., earlier store with an
   unknown address) */
#define MAX_STD_UNKNOWNS  LSQSIZE

/*RN: 10.11.06*/
#ifdef   LSQ_LOAD_FAST_ISSUE
	static void
lsq_refresh (void)
{
	int i, j, index, idx, num_sta_unknown_thread = 0;

	context *current;

#ifdef	LSQ_FIX_PARTIAL_BUG
	char std_width[LSQSIZE];
#endif

	for (idx = 0; idx < numcontexts; idx++)
	{
		/* scan entire queue for ready loads: scan from oldest instruction (head) until we reach the tail or an unresolved store, after which no
		   other instruction will become ready */
		current = thecontexts[idx];

		for (i = 0, index = current->LSQ_head; (i < current->LSQ_num) && (num_sta_unknown_thread < numcontexts); i++, index = (index + 1) % LSQ_size)
		{
			if (((MD_OP_FLAGS (current->LSQ[index].op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD)) && /* queued? */ !current->LSQ[index].queued
					&& /* waiting? */ !current->LSQ[index].issued
					&& /* completed? */ !current->LSQ[index].completed
					&& !current->LSQ[index].cache_ready &&
					/* regs ready? */ OPERANDS_READY (&current->LSQ[index]))
			{
				readyq_enqueue (&current->LSQ[index]);
			}
		}
	}


}
#else
	static void
lsq_refresh (void)
{
	int i, idx, j, index, n_std_unknowns;
	md_addr_t std_unknowns[MAX_STD_UNKNOWNS];
	context *current;

	/* scan entire queue for ready loads: scan from oldest instruction
	   (head) until we reach the tail or an unresolved store, after which no
	   other instruction will become ready */
	for (idx = 0; idx < numcontexts; idx++)
	{
		/* scan entire queue for ready loads: scan from oldest instruction
		   (head) until we reach the tail or an unresolved store, after which no
		   other instruction will become ready */
		current = thecontexts[idx];
		n_std_unknowns = 0;
		for (i = 0; i < LSQ_size; i++)
			std_unknowns[i] = 0;

		for (i = 0, index = current->LSQ_head, n_std_unknowns = 0; i < current->LSQ_num; i++, index = (index + 1) % LSQ_size)
		{
			if ((MD_OP_FLAGS (current->LSQ[index].op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE))
			{
				if (!STORE_ADDR_READY (&current->LSQ[index]))
				{
					/* FIXME: a later STD + STD known could hide the STA unknown */
					/* STA unknown, blocks all later loads, stop search */
					break;
				}
				else if (!OPERANDS_READY (&current->LSQ[index]))
				{
					/* STA known, but STD unknown: may block a later store, record
					   this address for later referral, we use an array here because
					   for most simulations the number of entries to search will be
					   very small.  If perfect disambiguation is on and the address is
					   not ready, treat this in the same manner as a STD unknown. */
					if (n_std_unknowns == LSQ_size)
						fatal ("STD unknown array overflow, increase MAX_STD_UNKNOWNS");
					std_unknowns[n_std_unknowns++] = current->LSQ[index].addr;
				}
				else		/* STORE_ADDR_READY() && OPERANDS_READY() */
				{
					/* a later STD known hides an earlier STD unknown */
					for (j = 0; j < n_std_unknowns; j++)
					{
						if (std_unknowns[j] == /* STA/STD known */ current->LSQ[index].addr)
							std_unknowns[j] = /* bogus addr */ 0;
					}
				}
			}

			if (		/* load? */
					((MD_OP_FLAGS (current->LSQ[index].op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD)) && /* queued? */ !current->LSQ[index].queued
					&& /* waiting? */ !current->LSQ[index].issued
					&& /* completed? */ !current->LSQ[index].completed
					&& /* regs ready? */ OPERANDS_READY (&current->LSQ[index]))
			{
				/* no STA unknown conflict (because we got to this check), check for
				   a STD unknown conflict */
				for (j = 0; j < n_std_unknowns; j++)
				{
					/* found a relevant STD unknown? */
					if (std_unknowns[j] == current->LSQ[index].addr)
						break;
				}
				if (j == n_std_unknowns)
				{
					/* no STA or STD unknown conflicts, put load on ready queue */
					readyq_enqueue (&current->LSQ[index]);
				}
			}
		}
	}
}
#endif

/*
 *  RUU_ISSUE() - issue instructions to functional units
 */
	static void
ruu_issue (void)
{
	int k, i, load_lat, tlb_lat, n_issued;
	struct RS_link *node, *next_node;
	struct res_template *fu;
	int inter = -1;
	int this_cycle_issued[CLUSTERS];
	int mm;
	int delay = 0;
	bool_t bLSQHit = FALSE;
	bool_t bPartialHit = FALSE;
	int nW1;

#ifdef  MSHR
	int use_mshr = 0;
#endif

	int parentID;

	context *current;		/* to access the parameters  for the thread that RUU entry belongs to */
	int threadid;


	int issueLimit;

	int temp;

	/*RN: 10.11.06*/
#ifdef  PULL_BACK
	int pullbackDelay[MAXTHREADS];

	for (k = 0; k < numcontexts; k++)
	{
		pullbackDoneThisCycle[k] = 0;
		dl1MissThisCycle[k] = dl1HitThisCycle[k] = 0;
		pullbackDelay[k] = 0;

		if (pullbackWaitCycle[k])
		{
			pullbackWaitCycle[k]--;
		}

		if (pullbackBubble[k] && !pullbackWaitCycle[k])
		{
			pullbackCycleCount[k]++;
			pullbackBubble[k]--;
			pullbackDelay[k] = 1;
		}
	}
#endif


	for (temp = 0; temp < numcontexts; temp++)
	{
		issue_done[temp] = 0;
		this_cycle_issued[temp] = 0;
		loadRejectCount[temp] = 0;
		replayIndex[temp] = 0;
	}

	node = ready_queue;
	ready_queue = NULL;

	/*RN: 10.11.06 */

	issueLimit = (ruu_issue_width /*+ res_memport */ ) * numcontexts;
	int inorder_stall[MAXTHREADS] = {0};
	int inorder_stall_ruunum[MAXTHREADS] = {0};

	for (n_issued = 0; node && n_issued < issueLimit; node = next_node)
	{
#ifdef  MSHR
		use_mshr = 0;
#endif

		next_node = node->next;


		/* still valid? */
		if (RSLINK_VALID (node))
		{
			struct RUU_station *rs = RSLINK_RS (node);

			current = thecontexts[rs->threadid];
			threadid = rs->threadid;
			if (!OPERANDS_READY (rs) || !rs->queued || rs->issued || rs->completed)
				panic ("issued inst !ready, issued, or completed");

			rs->queued = FALSE;

			/*************************************
			//ctb 2013-03-07
			//*************************************
			if(!current->active_this_cycle)
			{
				readyq_enqueue (rs);
				RSLINK_FREE (node);
				continue;
			}
			//*************end*********************/

			if(ruu_inorder_issue)
			{
				int rs_index = rs->index; // get the issue id
				if(i = rs->in_LSQ)
				{ //for load/store get the proper issue id
					if(rs->prod[1] == NULL)
						panic("There is no address producer of load/store\n");
					rs_index = rs->prod[1]->index;
				}
				if(inorder_stall[threadid] && rs_index > inorder_stall_ruunum[threadid])
				{
					readyq_enqueue (rs);
					RSLINK_FREE (node);
					continue;
				}
				if ((MD_OP_FUCLASS (rs->op) > 3) && (MD_OP_FUCLASS (rs->op) < 10))
				{ /* floating point unit*/
					for(i = thecontexts[threadid]->RUU_head; i < rs_index; i = (i+1)%RUU_size)
					{
						if ((MD_OP_FUCLASS (thecontexts[threadid]->RUU[i].op) > 3) && (MD_OP_FUCLASS (thecontexts[threadid]->RUU[i].op) < 10))
						{
							if(!thecontexts[threadid]->RUU[i].issued)  //smaller id isn't issued yet
								break;
							if(thecontexts[threadid]->RUU[i].ea_comp && !current->RUU[i].odep_list[0]->rs->issued)
								break;
						}
					}
				}
				else
				{ /* integer unit */
					for(i = thecontexts[threadid]->RUU_head; i < rs_index; i = (i+1)%RUU_size)
					{
						if (!((MD_OP_FUCLASS (thecontexts[threadid]->RUU[i].op) > 3) && (MD_OP_FUCLASS (thecontexts[threadid]->RUU[i].op) < 10)))
						{
							if(!thecontexts[threadid]->RUU[i].issued)  //smaller id isn't issued yet
								break;
							if(thecontexts[threadid]->RUU[i].ea_comp && !current->RUU[i].odep_list[0]->rs->issued)
								break;
						}
					}
				}
				if(i != rs_index) //break in the middle in walking the RUU, so the inst can't issue
				{
					readyq_enqueue (rs);
					RSLINK_FREE (node);
					inorder_stall[threadid] = 1;
					inorder_stall_ruunum[threadid] = rs_index;
					continue;
				}
				if(rs->in_LSQ && thecontexts[threadid]->LSQ_num)
				{
					if(thecontexts[threadid]->LSQ[(thecontexts[threadid]->LSQ_tail+LSQ_size-1)%LSQ_size].miss_flag)
					{
						readyq_enqueue (rs);
						RSLINK_FREE (node);
						continue;
					}
				}

			}
			inorder_stall[threadid] = 0;

#ifdef TG
			if (rs->in_LSQ)
				traffic_generate(sim_cycle, rs->addr, rs);

#endif

#ifdef  PULL_BACK
			if (pullbackDelay[rs->threadid])
			{
				readyq_enqueue (rs);
				RSLINK_FREE (node);
				continue;
			}
#endif

			if (issue_done[threadid] == (ruu_issue_width /*+ res_memport */ ))	//limit reached for this thread
			{
				readyq_enqueue (rs);
				RSLINK_FREE (node);
				continue;
			}

			if (loadRejectCount[threadid])
			{
				readyq_enqueue (rs);
				RSLINK_FREE (node);
				continue;
			}
			if(rs->cache_ready ==1)
			{
				readyq_enqueue (rs);
				RSLINK_FREE (node);
				continue;
			}

			//if issue time greater than sim cycle (due to ld rejection....) don't consider for issue
			if (rs->issue_time > sim_cycle)
			{
				readyq_enqueue (rs);
				RSLINK_FREE (node);
				continue;
			}

			if (((rs->when_ready[0] <= sim_cycle) && (rs->when_ready[1] <= sim_cycle) && (rs->when_ready[2] <= sim_cycle) && (rs->when_inq < sim_cycle) && !rs->in_qwait))
			{

#ifdef  PWR_WAKEUP_SELECT
				if (!(MD_OP_FLAGS (rs->op) & F_CTRL))
				{
					if ((rs->out1 > 0) && (rs->out1 < 32))
					{
						iselection_access++;
						current->iselection_access++;
					}
					if ((rs->out1 > 31) && (rs->out1 < 63))
					{
						fselection_access++;
						current->fselection_access++;
					}
				}
#endif //PWR_WAKEUP_SELECT
				int ll_sc = 0;

				if (rs->op == STL_C || rs->op == LDL_L)
				{
					//            fprintf(stderr,"issue: %lld  %s %d %d %llx\n",sim_cycle,MD_OP_NAME(rs->op),threadid,rs->index,rs->addr);
					ll_sc++;

				}

				if (rs->in_LSQ && ((MD_OP_FLAGS (rs->op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE)))
				{

					/*RN: 10.11.06 */
					// Pre-fetch the store address
					md_addr_t addr;

						addr = rs->addr;

					int valid_addr = md_valid_addr (addr, current->masterid);
					int i;

					if (!valid_addr)
						sim_invalid_addrs++;

					//do a store line prefetch by doing a cache read
#ifdef  MSHR
					use_mshr = 1;
#endif

					fu = res_get (fu_pool, MD_OP_FUCLASS (rs->op), threadid);	//changed to threadid

					if (!fu)
					{
						readyq_enqueue (rs);
						RSLINK_FREE (node);
						continue;
					}

					/* reserve the functional unit */
					if (fu->master->busy)
						panic ("functional unit already in use");

					/* First access the DTLB. In case of a miss, delay until miss dtlb miss is over. */
					if (valid_addr && dtlb[threadid])
					{
						tlb_lat = 0;
                        int tlb_mshr_match = -1;
                        tlb_mshr_match = tlb_mshr_check(addr, threadid, rs);
                        // during recovery, we delay the issue to simulate the overhead @ fanglei
                        int flushing;
                        flushing = tlb_lock_check(threadid);
                        if ( flushing )
                        {
                            rs->issue_time = sim_cycle + 1;
							readyq_enqueue (rs);
							RSLINK_FREE (node);
							continue;
                        }
                        if (tlb_mshr_match != -1)
                            tlb_lat = WAIT_TIME;
                        else
                        {
						    // tlb_lat = cache_access (dtlb[threadid], Read, (addr & ~3), NULL, 4, sim_cycle, NULL, NULL, NULL, threadid, NULL);
                            tlb_lat = cache_access (dtlb[threadid], Write, (addr & ~3), NULL, 4, sim_cycle, NULL, NULL, NULL, threadid, NULL);
                            // write to SR (tlb_lat == (dtlb[threadid]->hit_latency+1)) @ fanglei
                            if ( tlb_lat == WAIT_TIME )
                            {
                                int evicting = -1;
                                evicting = tlb_cu_check(threadid, addr, SHR_TLB_WRITE_BACK);
                                tlb_mshr_allocate(addr, threadid, rs, evicting);
                            }
						}
                        if(tlb_lat > dtlb[threadid]->hit_latency)	/* TLB miss */
						{/* This instruction is still not ready */
                            rs->issue_time = sim_cycle + tlb_lat;
							readyq_enqueue (rs);
							RSLINK_FREE (node);
							continue;
						}
					}

#ifdef	DCACHE_MSHR
						if (isMSHRFull(cache_dl1[threadid]->mshr, 0, threadid))
						{
							readyq_enqueue (rs);
							RSLINK_FREE (node);
							continue;
						}
#endif
					/* schedule functional unit release event */
					fu->master->busy = fu->issuelat;
					rs->issued = TRUE;
					rs->issue_time = sim_cycle;
					rs->completed = TRUE;
					rs->finish_time = sim_cycle;
					/* Pre-fetch the store address */
					/* no! go to the data cache if addr is valid */
					if (valid_addr && (rs->op != STL_C) && prefetch_flag)
					{
						int lat = 0;
						rsCacheAccess = rs;
						int matchnum = 0;
						dcache_access++;	//mwr: already there
						current->dcache_access++;
						matchnum = MSHR_block_check(cache_dl1[threadid]->mshr, addr, cache_dl1[threadid]->set_shift);
						if(!matchnum)
						{
							if (cache_dl1[threadid])
                            {
                                share_pattern(threadid, addr, Write);
								lat = cache_access (cache_dl1[threadid], Write, (addr & ~3), NULL, 4, sim_cycle, NULL, NULL, rs, rs->threadid, NULL);
                            }

#ifdef	DCACHE_MSHR
							if(lat > cache_dl1[threadid]->hit_latency)
								MSHRLookup(cache_dl1[threadid]->mshr, addr, lat, 0, rs);
#endif
						}
						else //useless? already set matchnum=0
							cache_dl1[threadid]->in_mshr ++;
						if(lat>cache_dl1[threadid]->hit_latency)
							rs->miss_flag = 1;
						else
							rs->miss_flag = 0;
					}

					rs->issued = TRUE;
					rs->completed = TRUE;
					rs->issue_time = sim_cycle;
					rs->finish_time = sim_cycle;

					if (rs->counted_iissueq)
					{
						current->iissueq_thrd--;
						iissueq--;
						iissueq_cl[rs->threadid]--;
						rs->counted_iissueq = 0;
					}
					if (rs->counted_fissueq)
					{
						current->fissueq_thrd--;
						fissueq--;
						fissueq_cl[rs->threadid]--;
						rs->counted_fissueq = 0;
					}


					if (rs->onames[0] || rs->onames[1])
						panic ("store creates result");

					if (rs->recover_inst)
						panic ("mis-predicted store");

					ptrace_newstage (rs->ptrace_seq, PST_WRITEBACK, 0);
					n_issued++;

					/*011305mwr */
					issue_done[rs->threadid]++;


					current->iqram_access++;

					if (rs->issue_time && rs->issue_time != sim_cycle)
						warn ("already issue timed on another cycle");
					else
						rs->issue_time = sim_cycle;

					for (k = 0; k < MAX_IDEPS; k++)
					{
						if (rs->when_idep_ready[k] < sim_cycle)
						{
							if ((rs->idep_name[k] > 0) && (rs->idep_name[k] < 32))
							{
								iregfile_access++;
								current->iregfile_access++;
								iregfile_access_cl[rs->cluster]++;
								if (k)
									iregfile_total_pop_count_cycle += pop_count (rs->val_rb);
								else
									iregfile_total_pop_count_cycle += pop_count (rs->val_ra);
								iregfile_num_pop_count_cycle++;
							}
							if ((rs->idep_name[k] > 31) && (rs->idep_name[k] < 63))
							{
								fregfile_access++;
								current->fregfile_access++;
								fregfile_access_cl[rs->cluster]++;
								if (k)
									iregfile_total_pop_count_cycle += pop_count (rs->val_rb);
								else
									iregfile_total_pop_count_cycle += pop_count (rs->val_ra);
								iregfile_num_pop_count_cycle++;
							}
						}
					}

					this_cycle_issued[rs->cluster]++;
					current->lsq_access++;
					lsq_store_data_access++;
					current->lsq_preg_access++;

					/*RN:10.11.06 */
					/*lq_asoc_wakeup_access++;
					  m_sqLQCheckCnt++; */
					//rs->finish_time = sim_cycle + LSQ_LATENCY;
					i = rs->index;
					bLSQHit = FALSE;
					nW1 = GetMemAccWidth (rs->op);

					for (;;)
					{
						bLSQHit = FALSE;
						if (rs->isPrefetch)
							break;

						i = (i + (LSQ_size + 1)) % LSQ_size;

						if (i == current->LSQ_tail)
							break;

#ifdef	LSQ_FIX_PARTIAL_BUG
						if ((MD_OP_FLAGS (current->LSQ[i].op) & F_LOAD)
								//Effect address ready?
								&& (OPERANDS_READY (&current->LSQ[i])) && (current->LSQ[i].threadid == rs->threadid) && current->LSQ[i].issued
								//current store is newer than the forwarded one before.
								//If there's no forwarded store before, sqStoreForwardT==-1,
								//will satisfy this condition automatically.
								/*&& current->LSQ[i].sqStoreForwardT < rs->disp_time */
								&& !current->LSQ[i].isPrefetch)
						{
							int nW2 = GetMemAccWidth (current->LSQ[i].op);
							int nW = (nW1 >= nW2) ? nW1 : nW2;

							md_addr_t qwAddr1 = addr & ~(nW - 1);
							md_addr_t qwAddr2 = current->LSQ[i].addr & ~(nW - 1);

							if (qwAddr1 == qwAddr2)
							{
								bLSQHit = TRUE;
								break;
								/*if( nW1<nW2 )
								  {    //Store width is less than load width.
								  m_sqLSQPartialHitCnt++;
								  } */
							}
						}
#else
						/* FIXME: not dealing with partials! */
						/* ALso check if the matched LOAD is from the same thread of the LOAD */
						if ((MD_OP_FLAGS (current->LSQ[i].op) & F_LOAD)
								//Effect address ready?
								&& (OPERANDS_READY (&current->LSQ[i])) && (current->LSQ[i].addr == addr) && (current->LSQ[i].threadid == rs->threadid) && current->LSQ[i].issued
								//current store is newer than the forwarded one before.
								//If there's no forwarded store before, sqStoreForwardT==-1,
								//will satisfy this condition automatically.
								&& current->LSQ[i].sqStoreForwardT < rs->disp_time && !current->LSQ[i].isPrefetch)
						{
							bLSQHit = TRUE;
							break;
						}
#endif //#ifdef        LSQ_FIX_PARTIAL_BUG


					}		//End of for(;;)

					if (bLSQHit)
					{
						if (!loadRejectCount[threadid])
						{
							m_sqNumLdReplay[threadid]++;
							loadRejectReplay[threadid] = i;
							replayIndex[threadid] = current->LSQ[i].robEntry;	/*RN: 12.04.06 */
							loadRejectCount[threadid]++;

						}

						//Mark the newer forward time.
						current->LSQ[i].sqStoreForwardT = rs->disp_time;

					}		//End of LSQ hit.
				}
				else
				{
					/* issue the instruction to a functional unit */
					if (MD_OP_FUCLASS (rs->op) != NA)
					{
						/* Determine the cache interleave in case of a memop-Ronz */
						if (rs->in_LSQ && ((MD_OP_FLAGS (rs->op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD)))
						{
							inter = (rs->addr >> cache_dl1[rs->threadid]->set_shift) & (res_membank - 1);
						}
						else if (!DEDICATED_ADDER || (MD_OP_FUCLASS (rs->op) != IntALU))
						{
							/* Not a memory op and not an intalu operation or you do not have separate adders for the lsq */
							inter = MAGICN + rs->cluster;
						}
						else
						{
							if (rs->ea_comp)
							{
								inter = RES_CACHEPORTS + 1;
							}
							else
							{
								inter = RES_CACHEPORTS + 2;
							}
						}

						fu = res_get (fu_pool, MD_OP_FUCLASS (rs->op), threadid);	//changed to threadid

						if (fu)
						{
							rs->issued = TRUE;
							if (rs->counted_iissueq)
							{
								current->iissueq_thrd--;
								iissueq--;
								iissueq_cl[rs->threadid]--;
								rs->counted_iissueq = 0;
							}
							if (rs->counted_fissueq)
							{
								current->fissueq_thrd--;
								fissueq--;
								fissueq_cl[rs->threadid]--;
								rs->counted_fissueq = 0;
							}

							if (fu->master->busy)
								panic ("functional unit already in use");

							fu->master->usr_id = threadid;	//mwr: keep threadid info too
							fu->master->duplicate = rs->duplicate;	//mwr: dup inst or not
							fu->master->busy = fu->issuelat;	//  * current->latency_factor; //mwr: increase the fu busy time if not load/st

							/* schedule a result writeback event */
							if (rs->in_LSQ && ((MD_OP_FLAGS (rs->op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD)))
							{
								int events = 0;
								load_lat = 0;
								md_addr_t addr;
									addr = rs->addr;

								if (dtlb[threadid] && md_valid_addr (addr, current->masterid))
								{
									tlb_lat = 0;
                                    int tlb_mshr_match = -1;
                                    tlb_mshr_match = tlb_mshr_check(addr, threadid, rs);
                                    int flushing;
                                    flushing = tlb_lock_check(threadid);
                                    if ( flushing )
                                    {
                                        rs->issue_time = sim_cycle + 1;
										fu->master->busy = 0;
										rs->issued = FALSE;
                                    	readyq_enqueue (rs);
                                    	RSLINK_FREE (node);
                                    	continue;
                                    }
                                    if (tlb_mshr_match != -1)
                                        tlb_lat = WAIT_TIME;
                                    else
                                    {
									    tlb_lat = cache_access(dtlb[threadid], Read, (addr & ~3), NULL, 4, sim_cycle, NULL, NULL, NULL, rs->threadid, NULL);  // add a parameter here
                                        if ( tlb_lat == WAIT_TIME )
                                        {
                                            int evicting = -1;
                                            evicting = tlb_cu_check(threadid, addr, SHR_TLB_WRITE_BACK);
                                            tlb_mshr_allocate(addr, threadid, rs, evicting);
                                        }
                                    }
									if (tlb_lat > dtlb[threadid]->hit_latency)
									{
										rs->issue_time = sim_cycle + tlb_lat;
										fu->master->busy = 0;
										rs->issued = FALSE;
										readyq_enqueue (rs);
										RSLINK_FREE (node);
										continue;
									}
								}
								int matchnum;
								matchnum = MSHR_block_check(cache_dl1[threadid]->mshr, addr, cache_dl1[threadid]->set_shift);
								if(matchnum && !rs->isPrefetch)
								{
									cache_dl1[threadid]->in_mshr ++;
									rs->cache_ready = 1;
									fu->master->busy = 0;
									rs->issued = FALSE;
									RS_block_list(rs, sim_cycle, matchnum, threadid);
									//ld_miss_pending[threadid] = 1;   /*In inorder processor, the instruction can not be issued until previous load is served*/
									RSLINK_FREE (node);
									continue;
								}

#ifdef	DCACHE_MSHR
									if (isMSHRFull(cache_dl1[threadid]->mshr, 0, threadid))
									{
										fu->master->busy = 0;
										rs->issued = FALSE;
										readyq_enqueue (rs);
										RSLINK_FREE (node);
										continue;
									}
#endif

								current->lsq_access++;
								current->lsq_wakeup_access++;

								/*RN: 10.12.06 */
								ldIssueCount++;
#ifdef RELAXED_CONSISTENCY
								i = rs->index;
								nW1 = GetMemAccWidth (rs->op);
								for (;;)
								{
									i = (i + (LSQ_size + 1)) % LSQ_size;

									if (i == current->LSQ_tail)
										break;

									if ((MD_OP_FLAGS (current->LSQ[i].op) & F_LOAD)
											&& (OPERANDS_READY (&current->LSQ[i])) && (current->LSQ[i].threadid == rs->threadid) && current->LSQ[i].issued
											&& !current->LSQ[i].isPrefetch)
									{
										int nW2 = GetMemAccWidth (current->LSQ[i].op);
										int nW = (nW1 >= nW2) ? nW1 : nW2;

										md_addr_t qwAddr1 = addr & ~(nW - 1);
										md_addr_t qwAddr2 = current->LSQ[i].addr & ~(nW - 1);

										if ((qwAddr1 == qwAddr2) && current->LSQ[i].invalidationReceived)
										{
											if (!loadRejectCount[threadid])
											{
												m_sqNumLdReplay[threadid]++;
												loadRejectReplay[threadid] = i;
												replayIndex[threadid] = current->LSQ[i].robEntry;
												loadRejectCount[threadid]++;
												invalidation_replay[threadid]++;
												break;
											}
										}
									}
								}	//End of for(;;)

#endif

								int ldstReplay = 1;

								if (!rs->isPrefetch)
									ldstReplay = checkLSQforSt (rs->index, current);	//if 1 then no reject else reject

								if (!ldstReplay)
								{

#ifdef REMOVE_COMPUTE_QUEUE
									if (rs->counted_iissueq)
									{
										current->iissueq_thrd++;
										iissueq++;
										rs->counted_iissueq = 1;
									}
									if (rs->counted_fissueq)
									{
										current->fissueq_thrd++;
										fissueq++;
										rs->counted_fissueq = 1;
									}
#endif
									rs->issued = FALSE;
									rs->issue_time = sim_cycle + rejectRetryCycle;
									readyq_enqueue (rs);
									RSLINK_FREE (node);
									ldstRejectionCount[threadid]++;
									continue;
								}

								//trying to find out how many loads are issued out of order
								bLSQHit = FALSE;
								bPartialHit = FALSE;

								i = rs->index;
								nW1 = GetMemAccWidth (rs->op);


								int lsq_lat = 0;
								if (i != current->LSQ_head && !rs->isPrefetch)
								{
									//bool_t bConflict = FALSE;
									for (;;)
									{
										//bool_t bAddrReady;
										/* go to next earlier LSQ entry */
										i = (i + (LSQ_size - 1)) % LSQ_size;


										if ((MD_OP_FLAGS (current->LSQ[i].op) & F_STORE)
#ifdef LSQ_LOAD_FAST_ISSUE
												//Effect address ready?
												&& (STORE_ADDR_READY (&current->LSQ[i]))
#endif
												&& (current->LSQ[i].threadid == rs->threadid) && !current->LSQ[i].isPrefetch)
										{
											int nW2 = GetMemAccWidth (current->LSQ[i].op);
											int nW = (nW1 >= nW2) ? nW1 : nW2;
											md_addr_t qwAddr1 = addr & ~(nW - 1);
											md_addr_t qwAddr2 = current->LSQ[i].addr & ~(nW - 1);

											if (qwAddr1 == qwAddr2)
											{
												bLSQHit = TRUE;
												if (nW1 > nW2)
												{
													//Load width is greater than store width
													//m_sqLSQPartialHitCnt++;
													bPartialHit = TRUE;
													break;
												}
											}
										}

										if (bLSQHit)
										{
											//Effect address ready?
											/* hit in the LSQ */

											lsq_lat = LSQ_LATENCY;	//data being forwarded from the LSQ
											rs->sqStoreForwardT = current->LSQ[i].disp_time;
											break;
										}
										/* scan finished? */
										if (i == current->LSQ_head)
											break;
									}
								}


								int valid_addr = md_valid_addr (addr, current->masterid);

								if (!current->spec_mode&& !valid_addr)
									sim_invalid_addrs++;

								if (cache_dl1[threadid] && valid_addr && !(matchnum))
								{

									dcache_access++;
									current->dcache_access++;
									rsCacheAccess = rs;
                                    share_pattern(threadid, addr, Read);
									load_lat = cache_access(cache_dl1[threadid], Read, (addr & ~3), NULL, 4, sim_cycle, NULL, NULL, /*ruu pointer*/rs, rs->threadid, NULL);

#ifdef	DCACHE_MSHR

									if(load_lat >= WAIT_TIME && rs->sqStoreForwardT == -1)
										MSHRLookup(cache_dl1[threadid]->mshr, addr, load_lat, 0, rs);
									else if(load_lat >= WAIT_TIME && rs->sqStoreForwardT != -1)
										MSHRLookup(cache_dl1[threadid]->mshr, addr, load_lat, 0, NULL);
#endif
									if (load_lat > cache_dl1[threadid]->hit_latency)
										events |= PEV_CACHEMISS;
									if(load_lat>cache_dl1[threadid]->hit_latency)
										rs->miss_flag = 1;
									else
										rs->miss_flag = 0;
								}
								else
								{
									load_lat = fu->oplat;
								}

#ifdef  PULL_BACK
								if (load_lat > cache_dl1[threadid]->hit_latency && !rs->isPrefetch)	//L1 miss
								{
									dl1MissThisCycle[rs->threadid] = 1;
									if (rs->op != LDT && rs->op != LDS)
									{	//not floating ld
										if (!pullbackDoneThisCycle[rs->threadid])
										{
											if (pullbackWaitCycle[rs->threadid])
												pullbackBubble[rs->threadid]++;
											else
												pullbackBubble[rs->threadid] = 2;
											pullbackDoneThisCycle[rs->threadid] = 1;
										}
										if (!pullbackWaitCycle[rs->threadid])
										{	// only if pullbackWaitCycle = 0, we need to wait the next cycle
											pullbackWaitCycle[rs->threadid] = 2;
										}
									}
									//ld_miss_pending[threadid] = 1;  /*In inorder processor, the instruction can not be issued until previous load is served*/
								}
								else if (!rs->isPrefetch)
								{	//L1 hit
									dl1HitThisCycle[rs->threadid] = 1;
									if (rs->op == LDT || rs->op == LDS)
									{
										load_lat = cache_dl1[threadid]->hit_latency + 1;
									}
								}
#endif


								if (PERFECT)
								{
									load_lat = PERFECTLAT;
								}

								if (lsq_lat)
								{
									load_lat = lsq_lat;	//hit in lsq, so using forwarded data
								}
								if (rs->isPrefetch)
								{
									load_lat = cache_dl1[threadid]->hit_latency;
									if (!rs->spec_mode)
										m_sqPrefetchCnt++;
								}

								/* use computed cache access latency */
								if (bPartialHit)
									//Charge 5 cycle overhead for partial hit!
									eventq_queue_event (rs, sim_cycle + load_lat + 5, load_lat+5);
								else
									/* use computed cache access latency */
									eventq_queue_event(rs, sim_cycle + load_lat, load_lat);

								if (!rs->ea_comp)
								{
									rs->finish_time = (sim_cycle + load_lat);
									rs->issue_time = sim_cycle;
								}

								/* entered execute stage, indicate in pipe trace */
								ptrace_newstage (rs->ptrace_seq, PST_EXECUTE, ((rs->ea_comp ? PEV_AGEN : 0) | events));
							}
							else	/* !load && !store */
							{
								/* Wattch -- ALU access Wattch-FIXME (different op types) also spread out power of multi-cycle ops */
#ifdef PWR_ACCESS
								if ((MD_OP_FUCLASS (rs->op) > 3) && (MD_OP_FUCLASS (rs->op) < 10))
									fqram_access_cl[rs->cluster]++;
								else
									iqram_access_cl[rs->cluster]++;
#endif

								if (MD_OP_FUCLASS (rs->op) == IntALU)
								{
									ialu1_access++;
									current->ialu1_access++;

									if (rs->ea_comp)
									{
										add_alu++;
									}
									else
									{
										nonadd_alu++;
									}
								}
								else if ((MD_OP_FUCLASS (rs->op) == IntMULT) || (MD_OP_FUCLASS (rs->op) == IntDIV))
								{
									ialu2_access++;
									current->ialu2_access++;
								}
								else if ((MD_OP_FUCLASS (rs->op) == FloatMULT) || (MD_OP_FUCLASS (rs->op) == FloatDIV))
								{
									falu2_access++;
									current->falu2_access++;
								}
								else
								{
									falu1_access++;
									current->falu1_access++;
								}
								eventq_queue_event (rs, sim_cycle + fu->oplat, 0);
								rs->finish_time = (sim_cycle + fu->oplat);
								rs->issue_time = sim_cycle;
								ptrace_newstage (rs->ptrace_seq, PST_EXECUTE, rs->ea_comp ? PEV_AGEN : 0);
							}

							/* one more inst issued */
							n_issued++;
							issue_done[rs->threadid]++;
							/*mwr */
							if ((MD_OP_FUCLASS (rs->op) > 3) && (MD_OP_FUCLASS (rs->op) < 10))
							{
								fqram_access++;
								current->fqram_access++;
							}
							else
							{
								iqram_access++;
								current->iqram_access++;
							}

							this_cycle_issued[rs->cluster]++;

							for (k = 0; k < MAX_IDEPS; k++)
							{
								if (rs->when_idep_ready[k] < sim_cycle)
								{
									if ((rs->idep_name[k] > 0) && (rs->idep_name[k] < 32))
									{
										iregfile_access++;
										current->iregfile_access++;
										iregfile_access_cl[rs->cluster]++;
										if (k)
											iregfile_total_pop_count_cycle += pop_count (rs->val_rb);
										else
											iregfile_total_pop_count_cycle += pop_count (rs->val_ra);
										iregfile_num_pop_count_cycle++;
									}
									if ((rs->idep_name[k] > 31) && (rs->idep_name[k] < 63))
									{
										fregfile_access++;
										current->iregfile_access++;
										fregfile_access_cl[rs->cluster]++;
										if (k)
											iregfile_total_pop_count_cycle += pop_count (rs->val_rb);
										else
											iregfile_total_pop_count_cycle += pop_count (rs->val_ra);
										iregfile_num_pop_count_cycle++;
									}
								}
							}
						}	// if(fu)
						else	/* no functional unit */
						{
							readyq_enqueue (rs);
							if (!rs->spec_mode)
								iss_stall++;
							if (inter < RES_CACHEPORTS)
							{
								if (!rs->spec_mode)
									bank_conf++;
							}
						}
					}
					else	/* does not require a functional unit! */
					{
						rs->issued = TRUE;
						if (rs->counted_iissueq)
						{
							current->iissueq_thrd--;
							iissueq--;
							iissueq_cl[rs->threadid]--;
							rs->counted_iissueq = 0;
						}
						if (rs->counted_fissueq)
						{
							current->fissueq_thrd--;
							fissueq--;
							fissueq_cl[rs->threadid]--;
							rs->counted_fissueq = 0;
						}
						eventq_queue_event (rs, sim_cycle + 1, 0);
						if (!rs->ea_comp && rs->out1)
						{
							rs->finish_time = (sim_cycle + 1);
							rs->issue_time = sim_cycle;
						}

						/* entered execute stage, indicate in pipe trace */
						ptrace_newstage (rs->ptrace_seq, PST_EXECUTE, rs->ea_comp ? PEV_AGEN : 0);
						n_issued++;
						issue_done[rs->threadid]++;
						if ((MD_OP_FUCLASS (rs->op) > 3) && (MD_OP_FUCLASS (rs->op) < 10))
						{
							fqram_access++;
							current->fqram_access++;
						}
						else
						{
							iqram_access++;
							current->iqram_access++;
						}
						if ((rs->instnum - current->RUU[current->RUU_head].instnum) > DISTILP)
						{
							distilp++;
							if (!rs->spec_mode)
								distilp_valid++;
							rs->distissue = 1;
						}
						this_cycle_issued[rs->cluster]++;

						for (k = 0; k < MAX_IDEPS; k++)
						{
							if (rs->when_idep_ready[k] < sim_cycle)
							{
								if ((rs->idep_name[k] > 0) && (rs->idep_name[k] < 32))
								{
									iregfile_access++;
									current->iregfile_access++;
									iregfile_access_cl[rs->cluster]++;
									if (k)
										iregfile_total_pop_count_cycle += pop_count (rs->val_rb);
									else
										iregfile_total_pop_count_cycle += pop_count (rs->val_ra);
									iregfile_num_pop_count_cycle++;
								}
								if ((rs->idep_name[k] > 31) && (rs->idep_name[k] < 63))
								{
									fregfile_access++;
									current->fregfile_access++;

									fregfile_access_cl[rs->cluster]++;
									if (k)
										iregfile_total_pop_count_cycle += pop_count (rs->val_rb);
									else
										iregfile_total_pop_count_cycle += pop_count (rs->val_ra);
									iregfile_num_pop_count_cycle++;
								}
							}
						}
					}
				}		/* !store */
			}
			else
			{
				readyq_enqueue (rs);
			}
		}
		RSLINK_FREE (node);
	}

	for (; node; node = next_node)
	{
		next_node = node->next;

		if (RSLINK_VALID (node))
		{
			struct RUU_station *rs = RSLINK_RS (node);

			rs->queued = FALSE;
			readyq_enqueue (rs);
		}
		RSLINK_FREE (node);
	}

	/*RN: 10.11.06 */

	for (temp = 0; temp < numcontexts; temp++)
	{
		if (loadRejectCount[temp] & (collect_stats == 1))
		{
			/*RN: 12.04.06 */
			int k = replayIndex[temp];

			//k = ((k + 1) % RUU_size);
			if (!thecontexts[temp]->RUU[k].spec_mode)
			{
				fixSpecState (k, temp);
				thecontexts[temp]->regs = thecontexts[temp]->RUU[k].backupReg;
				seqConsistancyReplay (k, temp);
				loadRejectStat[temp]++;
			}
		}
	}

	num_issued = n_issued;
}


/*
 * routines for generating on-the-fly instruction traces with support
 * for control and data misspeculation modeling
 */

#define R_BMAP_SZ       (BITMAP_SIZE(MD_NUM_IREGS))
#define F_BMAP_SZ       (BITMAP_SIZE(MD_NUM_FREGS))
#define C_BMAP_SZ       (BITMAP_SIZE(MD_NUM_CREGS))

/* dump speculative register state */
	static void
rspec_dump (FILE * stream, int threadid)	/* output stream */
{
	int i;

	context *current;

	current = thecontexts[threadid];

	if (!stream)
		stream = stderr;

	fprintf (stream, "Thread %d, ** speculative register contents **\n", threadid);

	fprintf (stream, "spec_mode: %s\n", current->spec_mode ? "t" : "f");

	/* dump speculative integer regs */
	for (i = 0; i < MD_NUM_IREGS; i++)
	{
		if (BITMAP_SET_P (current->use_spec_R, R_BMAP_SZ, i))
		{
			md_print_ireg (current->spec_regs_R, i, stream);
			fprintf (stream, "\n");
		}
	}

	/* dump speculative FP regs */
	for (i = 0; i < MD_NUM_FREGS; i++)
	{
		if (BITMAP_SET_P (current->use_spec_F, F_BMAP_SZ, i))
		{
			md_print_fpreg (current->spec_regs_F, i, stream);
			fprintf (stream, "\n");
		}
	}

	/* dump speculative CTRL regs */
	for (i = 0; i < MD_NUM_CREGS; i++)
	{
#ifdef TEST_BUG
		if (BITMAP_SET_P (use_spec_C, C_BMAP_SZ, i))
#else
			if (BITMAP_SET_P (current->use_spec_C, C_BMAP_SZ, i))
#endif
			{
				md_print_creg (current->spec_regs_C, i, stream);
				fprintf (stream, "\n");
			}
	}
}

// static struct fetch_rec *fetch_data[1024];   /* IFETCH -> DISPATCH inst queue */
// int fetch_num;                       /* num entries in IF -> DIS queue */
// static int fetch_tail, fetch_head;   /* head and tail pointers of queue */


/* recover instruction trace generator state to precise state state immediately
   before the first mis-predicted branch; this is accomplished by resetting
   all register value copied-on-write bitmasks are reset, and the speculative
   memory hash table is cleared */
	static void
tracer_recover (int threadid)
{
	int i;
	struct spec_mem_ent *ent, *ent_next;

	context *current;

	current = thecontexts[threadid];


	/* better be in mis-speculative trace generation mode */
	if (!current->spec_mode)
		panic ("Thread %d, cannot recover unless in speculative mode", threadid);
	current->spec_mode = FALSE;


	if (current->waitForBranchResolve == 1)
		current->waitForBranchResolve = 0;


	/* reset copied-on-write register bitmasks back to non-speculative state */
#ifdef TEST_BUG
	BITMAP_CLEAR_MAP (use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP (use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP (use_spec_C, C_BMAP_SZ);
#else
	BITMAP_CLEAR_MAP (current->use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP (current->use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP (current->use_spec_C, C_BMAP_SZ);
#endif

	/* reset memory state back to non-speculative state */
	/* FIXME: could version stamps be used here?!?!? */
	for (i = 0; i < STORE_HASH_SIZE; i++)
	{
		/* release all hash table buckets */
		for (ent = current->store_htable[i]; ent; ent = ent_next)
		{
			ent_next = ent->next;
			ent->next = current->bucket_free_list;
			current->bucket_free_list = ent;
		}
		current->store_htable[i] = NULL;
	}

	/* if pipetracing, indicate squash of instructions in the inst fetch queue */
	if (ptrace_active)
	{
		while (current->fetch_num != 0)
		{
			ptrace_endinst (current->fetch_data[current->fetch_head]->ptrace_seq);
			current->fetch_head = (current->fetch_head + 1) & (ruu_ifq_size - 1);
			current->fetch_num--;
		}
	}
	/* reset IFETCH state */
	current->fetch_num = 0;
	current->fetch_tail = current->fetch_head = 0;
	current->fetch_pred_PC = current->fetch_regs_PC = current->recover_PC;
}

/* initialize the speculative instruction state generator state */
	static void
tracer_init (int threadid)
{
	int i;

	context *current;

	current = thecontexts[threadid];

	current->spec_mode = FALSE;
	if (current->waitForBranchResolve == 1)
		current->waitForBranchResolve = 0;


#ifdef TEST_BUG
	BITMAP_CLEAR_MAP (use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP (use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP (use_spec_C, C_BMAP_SZ);
#else
	BITMAP_CLEAR_MAP (current->use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP (current->use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP (current->use_spec_C, C_BMAP_SZ);
#endif
	for (i = 0; i < STORE_HASH_SIZE; i++)
		current->store_htable[i] = NULL;
}




/* speculative memory hash table address hash function */
#define HASH_ADDR(ADDR)	((((ADDR) >> 24)^((ADDR) >> 16)^((ADDR) >> 8)^(ADDR)) & (STORE_HASH_SIZE-1))


/* this functional provides a layer of mis-speculated state over the
   non-speculative memory state, when in mis-speculation trace generation mode,
   the simulator will call this function to access memory, instead of the
   non-speculative memory access interfaces defined in memory.h; when storage
   is written, an entry is allocated in the speculative memory hash table,
   future reads and writes while in mis-speculative trace generation mode will
   access this buffer instead of non-speculative memory state; when the trace
   generator transitions back to non-speculative trace generation mode,
   tracer_recover() clears this table, returns any access fault */
	static enum md_fault_type
spec_mem_access (struct mem_t *mem,	/* memory space to access */
		enum mem_cmd cmd,	/* Read or Write access cmd */
		md_addr_t addr,	/* virtual address of access */
		void *p,	/* input/output buffer */
		int nbytes)	/* number of bytes to access */
{
	int i, index;
	struct spec_mem_ent *ent, *prev;

	context *current;
	int threadid = mem->threadid;

	current = thecontexts[threadid];

	/* FIXME: partially overlapping writes are not combined... */
	/* FIXME: partially overlapping reads are not handled correctly... */

	/* check alignments, even speculative this test should always pass */
	if ((nbytes & (nbytes - 1)) != 0 || (addr & (nbytes - 1)) != 0)
	{
		/* no can do, return zero result */
		for (i = 0; i < nbytes; i++)
			((char *) p)[i] = 0;

		return md_fault_none;
	}
	/* check permissions */
	if (!((addr >= current->ld_text_base && addr < (current->ld_text_base + current->ld_text_size) && cmd == Read) || md_valid_addr (addr, current->masterid)))
	{
		/* no can do, return zero result */
		for (i = 0; i < nbytes; i++)
			((char *) p)[i] = 0;

		return md_fault_none;
	}

	/* has this memory state been copied on mis-speculative write? */
	index = HASH_ADDR (addr);
	for (prev = NULL, ent = current->store_htable[index]; ent; prev = ent, ent = ent->next)
	{
		if (ent->addr == addr)
		{
			/* reorder chains to speed access into hash table */
			if (prev != NULL)
			{
				/* not at head of list, relink the hash table entry at front */
				prev->next = ent->next;
				ent->next = current->store_htable[index];
				current->store_htable[index] = ent;
			}
			break;
		}
	}

	/* no, if it is a write, allocate a hash table entry to hold the data */
	if (!ent && cmd == Write)
	{
		/* try to get an entry from the free list, if available */
		if (!current->bucket_free_list)
		{
			/* otherwise, call calloc() to get the needed storage */
			current->bucket_free_list = calloc (1, sizeof (struct spec_mem_ent));
			if (!current->bucket_free_list)
				fatal ("thread %d , out of virtual memory", threadid);
		}
		ent = current->bucket_free_list;
		current->bucket_free_list = current->bucket_free_list->next;

		if (!bugcompat_mode)
		{
			/* insert into hash table */
			ent->next = current->store_htable[index];
			current->store_htable[index] = ent;
			ent->addr = addr;
			ent->data[0] = 0;
			ent->data[1] = 0;
		}
	}

	/* handle the read or write to speculative or non-speculative storage */
	switch (nbytes)
	{
		case 1:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((byte_t *) p) = *((byte_t *) (&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((byte_t *) p) = MEM_READ_BYTE (mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((byte_t *) (&ent->data[0])) = *((byte_t *) p);
			}
			break;
		case 2:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((half_t *) p) = *((half_t *) (&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((half_t *) p) = MEM_READ_HALF (mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((half_t *) & ent->data[0]) = *((half_t *) p);
			}
			break;
		case 4:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((word_t *) p) = *((word_t *) & ent->data[0]);
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((word_t *) p) = MEM_READ_WORD (mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((word_t *) & ent->data[0]) = *((word_t *) p);
			}
			break;
		case 8:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((word_t *) p) = *((word_t *) & ent->data[0]);
					*(((word_t *) p) + 1) = *((word_t *) & ent->data[1]);
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((word_t *) p) = MEM_READ_WORD (mem, addr);
					*(((word_t *) p) + 1) = MEM_READ_WORD (mem, addr + sizeof (word_t));
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((word_t *) & ent->data[0]) = *((word_t *) p);
				*((word_t *) & ent->data[1]) = *(((word_t *) p) + 1);
			}
			break;
		default:
			panic ("access size not supported in mis-speculative mode");
	}

	return md_fault_none;
}

/* dump speculative memory state */
	static void
mspec_dump (FILE * stream, int threadid)	/* output stream */
{
	int i, j;
	struct spec_mem_ent *ent;

	context *current;

	current = thecontexts[threadid];

	if (!stream)
		stream = stderr;

	fprintf (stream, "Thread %d,  ** speculative memory contents **\n", threadid);

	fprintf (stream, "spec_mode: %s\n", current->spec_mode ? "t" : "f");

	for (j = 0; j < numcontexts; j++)
	{
		fprintf (stream, "thread %d : \n", j);
		for (i = 0; i < STORE_HASH_SIZE; i++)
		{
			/* dump contents of all hash table buckets */
			for (ent = thecontexts[j]->store_htable[i]; ent; ent = ent->next)
			{
				myfprintf (stream, "[0x%08p]: %12.0f/0x%08x:%08x\n", ent->addr, (double) (*((double *) ent->data)), *((unsigned int *) &ent->data[0]), *(((unsigned int *) &ent->data[0]) + 1));
			}
		}
	}
}

/* default memory state accessor, used by DLite */
	static char *			/* err str, NULL for no err */
simoo_mem_obj (struct mem_t *mem,	/* memory space to access */
		int is_write,	/* access type */
		md_addr_t addr,	/* address to access */
		char *p,		/* input/output buffer */
		int nbytes)	/* size of access */
{
	enum mem_cmd cmd;

	int threadid = mem->threadid;
	context *current;

	current = thecontexts[threadid];

	if (!is_write)
		cmd = Read;
	else
		cmd = Write;

#if 0
	char *errstr;

	errstr = mem_valid (cmd, addr, nbytes, /* declare */ FALSE);
	if (errstr)
		return errstr;
#endif

	/* else, no error, access memory */
	if (current->spec_mode)
		spec_mem_access (mem, cmd, addr, p, nbytes);
	else
		mem_access (mem, cmd, addr, p, nbytes, threadid);

	/* no error */
	return NULL;
}


/* R. Garg: Flushing of WBbuffer is ideal in the sense that entries in the
 * buffer just disappear and data is written to the memory */
	void
flushWriteBuffer (int threadid)
{
	int index;
	struct wb_mem_ent *ent, *prev;
	int nbytes;
	byte_t *p, *q;
	md_addr_t addr;
	int ii;

	context *current = thecontexts[threadid];

	for (ent = current->WBtableTail; ent != NULL; ent = ent->prev)
	{
		p = ent->data;
		nbytes = ent->nbytes;
		addr = ent->addr;

#if !WATER_NSQUARED
		if (!ent->writeAlreadyDone)
		{
			while (nbytes-- > 0)
			{
				MEM_WRITE_BYTE (current->mem, addr, *((byte_t *) p));
				p += sizeof (byte_t);
				addr += sizeof (byte_t);
			}
		}
#endif
		ent->writeAlreadyDone = 1;
	}
	return;
}


	void
completeStore (int threadid, md_addr_t addr, int width, int isFail)
{

	byte_t *p, *q;
	context *current;
	struct wb_mem_ent *ent, *prev;


	int nbytes;
	enum md_opcode op;
	md_addr_t inst;
	int do_write = 1;
	int jj;
	int accessed;

	current = thecontexts[threadid];
	ent = current->WBtableTail;

	if ((width & (width - 1)) != 0 || (addr & (width - 1)) != 0)
		return;

	p = ent->data;
	nbytes = ent->nbytes;
	if (addr != ent->addr)
		printf("ent->addr %lld\n", ent->addr);
	if (addr != ent->addr)
		panic ("write buffer miss-alignment\n");

	MD_SET_OPCODE (op, ent->instr);
	inst = ent->instr;

	p = ent->data;
	nbytes = ent->nbytes;


	if (op != 35)
	{
#if !WATER_NSQUARED
		if (!ent->writeAlreadyDone)
		{
			while (nbytes-- > 0)
			{
				MEM_WRITE_BYTE (current->mem, addr, *((byte_t *) p));
				p += sizeof (byte_t);
				addr += sizeof (byte_t);
			}
		}
#endif
	}
	else
	{
#if WATER_NSQUARED
		current->waitForSTLC = 0;
#else
		if (!isFail && (common_regs_s[current->masterid][current->actualid].regs_lock != 0) && (common_regs_s[current->masterid][current->actualid].address == GPR (RB) + SEXT (OFS)))
		{
			do_write = 1;

			for (jj = 0; jj < THREADS_PER_JOB; jj++)
			{
				if (common_regs_s[current->masterid][jj].address == GPR (RB) + SEXT (OFS))
				{
					common_regs_s[current->masterid][jj].regs_lock = 0;
					common_regs_s[current->masterid][jj].address = 0;
				}
			}
			if (!ent->writeAlreadyDone)
			{

				while (nbytes-- > 0)
				{
					MEM_WRITE_BYTE (current->mem, addr, *((byte_t *) p));
					p += sizeof (byte_t);
					addr += sizeof (byte_t);
				}
			}
			storeCondFail = 0;
		}
		else
		{
			current->regs.regs_R[((inst >> 21) & 0x1f)] = 0;
			do_write = 0;
			storeCondFail = 1;
		}
		current->waitForSTLC = 0;
#endif
	}
	accessed = ent->accessed;
	current->numOfWBEntry--;
	if ((ent->prev == NULL) && (ent->next == NULL))
	{
		current->WBtableTail = NULL;
		current->WBtableHead = NULL;
		ent->next = current->WBbucket_free_list;
		current->WBbucket_free_list = ent;
	}
	else if (ent->next == NULL)
	{
		current->WBtableTail = ent->prev;
		ent->prev->next = NULL;
		ent->next = current->WBbucket_free_list;
		current->WBbucket_free_list = ent;

	}
	else if (ent->prev == NULL)
	{
		current->WBtableHead = ent->next;
		ent->next->prev = NULL;
		ent->next = current->WBbucket_free_list;
		current->WBbucket_free_list = ent;
	}
	else
	{
		ent->prev->next = ent->next;
		ent->next->prev = ent->prev;
		ent->next = current->WBbucket_free_list;
		current->WBbucket_free_list = ent;
	}
}



	void
commitWrite (md_addr_t addr, int threadID, counter_t completeCycle, int needBusAccess, int accessed)
{
	int index;
	struct wb_mem_ent *ent;
	context *current;
	enum md_opcode op;
	int lat = 0;
	int port_lat = 0, now_lat = 0, port_now = 0;


	current = thecontexts[threadID];
	if (!COHERENT_CACHE)
	{
		printf ("This is not TRUE for non-coherent cache\n");
		exit (0);
	}
	index = 0;
	for (ent = current->WBtableHead; ent; ent = ent->next)
	{
		if (ent->addr == addr && !ent->completeCycle)
		{
			MD_SET_OPCODE (op, ent->instr);
			if (op == 35)
			{
				ent->completeCycle = sim_cycle + lat;
				ent->needBusAccess = 1;
				ent->accessed = accessed;
			}
			else
			{
				ent->completeCycle = completeCycle;
				ent->needBusAccess = needBusAccess;
				ent->accessed = accessed;
			}
			//return;
		}
	}
}



#if PROCESS_MODEL
	int
checkForSharedAddr (unsigned long long addr)
{
	struct sharedAddressList_s *tmpPtr;

	int indx = (int) (((int) addr & 1016) >> 3);

	indx = 0;
	tmpPtr = sharedAddressList[indx];
	if ((addr >= LastSharedAddress) && (addr < (LastSharedAddress + LastSharedSize)))
	{
		return 1;
	}
	while (tmpPtr != NULL)
	{
		if ((addr >= tmpPtr->address) && (addr < (tmpPtr->address + tmpPtr->size)))
		{
			LastSharedAddress = tmpPtr->address;
			LastSharedSize = tmpPtr->size;
			return 1;
		}
		tmpPtr = tmpPtr->next;
	}
	return 0;
}
#endif

	enum md_fault_type
WB_access1 (struct mem_t *mem, enum mem_cmd cmd, md_addr_t addr, void *p, int nbytes, int threadID, md_addr_t instr)
{
	enum md_opcode op;
	enum md_fault_type return_type;

	TempInstr = instr;
	MD_SET_OPCODE (op, TempInstr);

	if(!thecontexts[threadID]->spec_mode
		&& (addr <= thecontexts[0]->ld_stack_base) && (addr > thecontexts[0]->ld_stack_base-numcontexts*STACKSIZE) && !(addr <= thecontexts[threadID]->ld_stack_base && addr > thecontexts[threadID]->ld_stack_base-STACKSIZE))
//		panic("Stack boundaries breached: May be the STACKSIZE parameter defined in context.h file is small for the application");
//        printf("STACKSIZE BUG")
        ;

#if defined(DO_COMPLETE_FASTFWD) || defined(PARALLEL_EMUL)
#if PROCESS_MODEL
	if (checkForSharedAddr ((unsigned long long) addr))
	{
		return_type = mem_access_drct (thecontexts[0]->mem, cmd, addr, p, nbytes, threadID);
		return return_type;
	}
#endif
	return_type = mem_access_drct (mem, cmd, addr, p, nbytes, threadID);
	return return_type;
#endif


	if (COHERENT_CACHE && allForked)
	{
		if (op == 35)
		{
			thecontexts[threadID]->waitForSTLC = 1;
		}
		return_type = mem_access (mem, cmd, addr, p, nbytes, threadID);
#if WATER_NSQUARED
		return_type = mem_access_drct (mem, cmd, addr, p, nbytes, threadID);
#endif

		if (access_mem && access_mem_id == threadID)
		{
			flushWriteBuffer (threadID);
		}
		return return_type;
	}
	else
	{
		return_type = mem_access_drct (mem, cmd, addr, p, nbytes, threadID);
		return return_type;
	}
}



	static INLINE void
ruu_link_idep (struct RUU_station *rs,	/* rs station to link */
		int idep_num,	/* input dependence number */
		int idep_name)	/* input register name */
{
	struct CV_link head;
	struct RS_link *link;

	int threadid = rs->threadid;
	context *current;

	current = thecontexts[threadid];

	rs->prod[idep_num] = NULL;
	rs->idep_name[idep_num] = idep_name;
	/* any dependence? */
	if (idep_name == NA)
	{
		/* no input dependence for this input slot, mark operand as ready */
		rs->idep_ready[idep_num] = TRUE;
		rs->when_ready[idep_num] = 0;
		rs->when_idep_ready[idep_num] = WAIT_TIME;
		return;
	}

	head = CREATE_VECTOR (idep_name);

	/* any creator? */
	if (!head.rs || head.rs->completed)
	{
		rs->idep_ready[idep_num] = TRUE;
		rs->when_ready[idep_num] = sim_cycle - 1;
		rs->when_idep_ready[idep_num] = sim_cycle - 1;
		return;
	}
	rs->idep_ready[idep_num] = FALSE;
	rs->prod[idep_num] = head.rs;

	RSLINK_NEW (link, rs);
	link->x.opnum = idep_num;
	link->next = head.rs->odep_list[head.odep_num];
	head.rs->odep_list[head.odep_num] = link;
}

	int
needsWBflush (context * current, enum md_opcode op, md_addr_t addr)
{
	int nbytes = GetMemAccWidth (op);

	if (op == LDQ_U)
		addr = addr & ~7;

	struct wb_mem_ent *ent;

	for (ent = current->WBtableHead; ent; ent = ent->next)
	{
		if (nbytes == 1)
		{
			if (!(ent->nbytes == nbytes))
			{
				if ((ent->addr <= addr) && ((ent->addr + ent->nbytes) > addr))
				{
					return 1;
				}
			}
		}
		else if (nbytes == 2)
		{
			if (ent->nbytes != nbytes)
			{
				if (ent->nbytes < nbytes)
				{
					if ((ent->addr >= addr) && (ent->addr < (addr + nbytes)))
					{
						return 1;
					}
				}
				else
				{
					if ((addr >= ent->addr) && (addr < (ent->addr + ent->nbytes)))
					{
						return 1;
					}
				}
			}
		}
		else if (nbytes == 4)
		{
			if (nbytes != ent->nbytes)
			{
				if (ent->nbytes < nbytes)
				{
					if ((ent->addr >= addr) && (ent->addr < (addr + nbytes)))
					{
						return 1;
					}
				}
			}
		}
		else if (nbytes == 8)
		{
			if (nbytes != ent->nbytes)
			{
				if ((ent->nbytes == 1) || (ent->nbytes == 2) || (ent->nbytes == 4))
				{
					if ((ent->addr >= addr) && (ent->addr < (addr + nbytes)))
					{
						return 1;
					}
				}
			}
		}
	}
	return 0;
}



/* make RS the creator of architected register ODEP_NAME */
	static INLINE void
ruu_install_odep (struct RUU_station *rs,	/* creating RUU station */
		int odep_num,	/* output operand number */
		int odep_name)	/* output register name */
{
	struct CV_link cv;

	context *current = thecontexts[rs->threadid];

	/* any dependence? */
	if (odep_name == NA)
	{
		/* no value created */
		rs->onames[odep_num] = NA;
		return;
	}
	/* else, create a RS_NULL terminated output chain in create vector */
	/* record output name, used to update create vector at completion */
	rs->onames[odep_num] = odep_name;
	/* initialize output chain to empty list */
	rs->odep_list[odep_num] = NULL;
	/* indicate this operation is latest creator of ODEP_NAME */
	/* get a new create vector link */
	CVLINK_INIT (cv, rs, odep_num);
	SET_CREATE_VECTOR (odep_name, cv);
}



	static INLINE void
ruu_install_odep_skip (struct RUU_station *rs,	/* creating RUU station */
		int odep_num,	/* output operand number */
		int odep_name)	/* output register name */
{

	/* any dependence? */
	if (odep_name == NA)
	{
		/* no value created */
		rs->onames[odep_num] = NA;
		return;
	}
	/* else, create a RS_NULL terminated output chain in create vector */
	/* record output name, used to update create vector at completion */
	rs->onames[odep_num] = odep_name;
	/* initialize output chain to empty list */
	rs->odep_list[odep_num] = NULL;
	/* do not indicate this operation is latest creator of ODEP_NAME */
}



/* default register state accessor, used by DLite */
	static char *			/* err str, NULL for no err */
simoo_reg_obj (struct regs_t *xregs,	/* registers to access */
		int is_write,	/* access type */
		enum md_reg_type rt,	/* reg bank to probe */
		int reg,		/* register number */
		struct eval_value_t *val)	/* input, output */
{
	/*  FIXME : set threadid to 0 right now until invastigate on dlite option */
	int threadid = 0;
	context *current;

	current = thecontexts[threadid];

	switch (rt)
	{
		case rt_gpr:
			if (reg < 0 || reg >= MD_NUM_IREGS)
				return "register number out of range";

			if (!is_write)
			{
				val->type = et_uint;
				val->value.as_uint = GPR (reg);
			}
			else
				SET_GPR (reg, eval_as_uint (*val));
			break;

		case rt_lpr:
			if (reg < 0 || reg >= MD_NUM_FREGS)
				return "register number out of range";

			/* FIXME: this is not portable... */
			abort ();
			break;

		case rt_fpr:
			/* FIXME: this is not portable... */
			abort ();
			break;

		case rt_dpr:
			/* FIXME: this is not portable... */
			abort ();
			break;

			/* FIXME: this is not portable... */

		case rt_PC:
			if (!is_write)
			{
				val->type = et_addr;
				val->value.as_addr = current->regs.regs_PC;
			}
			else
				current->regs.regs_PC = eval_as_addr (*val);
			break;

		case rt_NPC:
			if (!is_write)
			{
				val->type = et_addr;
				val->value.as_addr = current->regs.regs_NPC;
			}
			else
				current->regs.regs_NPC = eval_as_addr (*val);
			break;

		default:
			panic ("bogus register bank");
	}

	/* no error */
	return NULL;
}

/*lechen, need to do refetch*/
static void ruu_fetch (void);

int stallCause[MAXTHREADS] = {0};
counter_t mb_count = 0;
/*
 *  RUU_DISPATCH() - decode instructions and allocate RUU and LSQ resources
 */
/* dispatch instructions from the IFETCH -> DISPATCH queue: instructions are
   first decoded, then they allocated RUU (and LSQ for load/stores) resources
   and input and output dependence chains are updated accordingly */

int isLockInst(md_addr_t PC)
{
	if(PC <= LockInitPC && PC >= LockInitPC - 68)
		return 1;
	else
		return 0;
}
int isBarRelInst(md_addr_t PC)
{
	if(PC <= last_last_pc + 112 && PC >= last_last_pc - 844)
		return 1;
	else
		return 0;
}

	int
ruu_dispatch (int threadid)
{
	int i;
	int tempcl;
	long temppc = 0;
	int mm, k;
	int force_bank;
	int slot;

	int n_dispatched;		/* total insts dispatched */
	md_inst_t inst;		/* actual instruction bits */
	enum md_opcode op;		/* decoded opcode enum */
	int out1, out2, in1, in2, in3;	/* output/input register names */
	md_addr_t target_PC = 0;	/* actual next/target PC address */
	md_addr_t addr = 0;		/* effective address, if load/store */
	struct RUU_station *rs;	/* RUU station being allocated */
	struct RUU_station *lsq;	/* LSQ station for ld/st's */

	/*092904mwr: extra var for duplication */
	struct RUU_station *rs_dup;	/* RUU station being allocated */
	struct bpred_update_t *dir_update_ptr;	/* branch predictor dir update ptr */
	int stack_recover_idx;	/* bpred retstack recovery index */
	unsigned int pseq;		/* pipetrace sequence number */
	int is_write = FALSE;	/* store? */
	int made_check;		/* used to ensure DLite entry */
	int br_taken, br_pred_taken;	/* if br, taken?  predicted taken? */
	byte_t temp_byte;		/* temp variable for spec mem access */
	half_t temp_half;		/* " ditto " */
	word_t temp_word;		/* " ditto " */

#if defined(HOST_HAS_QWORD) && defined(TARGET_ALPHA)
	qword_t temp_qword = 0;	/* " ditto " */
#endif /* HOST_HAS_QWORD && TARGET_ALPHA */
	qword_t regVal;
	half_t ushLdFlag;

	struct regs_t tempReg;

	/*RN: 12.04.06 */
	struct regs_t tempRegPrev;

	enum md_fault_type fault;

	int soft_error, temp_fetch_num;

	/* Wattch:  Added for pop count generation (AFs) */
	qword_t val_ra = 0, val_rb = 0, val_rc = 0, val_ra_result = 0;
	void copy_to (int);
	int bank_lookup (md_addr_t, int, int);

	context *current;
	long fetch_time = 0;

	int second_run = TRUE, no_dsp_cls;

	int in_LSQ = 0;
	int lsq_full = 0;

	int parentID;

	int resource_need[4];

	resource_need[0] = 1;
	resource_need[1] = 1;
	resource_need[2] = 3;
	resource_need[3] = 3;

	current = thecontexts[threadid];

	n_dispatched = 0;		/*110704mwr: moved inside the for loop to make sure that no thread dispatches more than what it should */
	temp_fetch_num = current->fetch_num;

	current->fetch_redirected = FALSE;
	stallCause[threadid] = 0;
	int lsq_miss_stall = 0;

	if (temp_fetch_num != 0)
	{
		made_check = FALSE;
		tempcl = actual_clusters;

		while (			/* instruction decode B/W left? */
				n_dispatched < (ruu_decode_width * fetch_speed)	/*110704mwr: no thread dispatches more than its share */
				&& current->LSQ_num < LSQ_size && current->m_shLQNum < m_shLQSize
				&& (current->m_shSQNum + m_L1WBufCnt[threadid]) < m_shSQSize	//SQ size is equal to SQ_size + WB_size
				&& (current->RUU_num < RUU_size)
				&& temp_fetch_num != 0
	            && (ruu_include_spec || !current->spec_mode)
	          )
		{

			inst = current->fetch_data[current->fetch_head]->IR;
			MD_SET_OPCODE (op, inst);

			if (MD_OP_FLAGS (op) & F_MEM)
				in_LSQ = 1;
			else
				in_LSQ = 0;

			if ((MD_OP_FUCLASS (op) > 3) && (MD_OP_FUCLASS (op) < 10))
			{
				resource_need[0] = 0;
				resource_need[1] = 1;
				resource_need[2] = 0;
				resource_need[3] = 3;
			}
			else
			{
				resource_need[0] = 1;
				resource_need[1] = 0;
				resource_need[2] = 3;
				resource_need[3] = 1;
			}

			if (!((iissueq_cl[threadid] <= (IIQ_size - resource_need[0])) && (fissueq_cl[threadid] <= (FIQ_size - resource_need[1])) && (iregfile_cl[threadid] <= ((IPREG_size) - resource_need[2])) && (fregfile_cl[threadid] <= ((FPREG_size) - resource_need[3]))))
			{
				if(!((iissueq_cl[threadid] <= (IIQ_size - resource_need[0])) && (fissueq_cl[threadid] <= (FIQ_size - resource_need[1]))))
					stallCause[threadid] = StallIssueFull;
				if(!((iregfile_cl[threadid] <= ((IPREG_size) - resource_need[2])) && (fregfile_cl[threadid] <= ((FPREG_size) - resource_need[3]))))
					stallCause[threadid] = StallRegFull;
				break;
			}

			if ((thecontexts[threadid]->waitForBranchResolve == 1) || (thecontexts[threadid]->waitForSTLC))
			{
				stallCause[threadid] = StallSTLC;
				break;
			}

			//if (ruu_inorder_issue && (current->last_op.rs && RSLINK_VALID (&current->last_op) && !OPERANDS_READY (current->last_op.rs)))
			//{
			//	break;
			//}

#if defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
			if (busSlotCount < 11)
			{
				stallNoBusSlot++;
				break;
			}
#endif

			if (collectStatStop[current->id])
			{
				break;
			}

			int j, bool = 0, tmp, ll_sc = 0;
			context *tmpCxt;
			md_addr_t mask;

			mask = ~cache_dl1[current->id]->blk_mask;


			inst = current->fetch_data[current->fetch_head]->IR;
			dir_update_ptr = &(current->fetch_data[current->fetch_head]->dir_update);
			stack_recover_idx = current->fetch_data[current->fetch_head]->stack_recover_idx;
			pseq = current->fetch_data[current->fetch_head]->ptrace_seq;
			fetch_time = current->fetch_data[current->fetch_head]->fetch_time;

			current->fetch_data[current->fetch_head]->is_dup = TRUE;
			if(m_update_miss_flag[threadid])/*this flag means there is an instruction depending on write update*/
			{
					m_update_miss_flag[threadid] = 0; /*this flag means there is an instruction depending on write update*/
					if(m_update_miss_depend_start[threadid])
						UpdateDependCycle += (sim_cycle - m_update_miss_depend_start[threadid]);
					m_update_miss_depend_start[threadid] = 0;
			}


			if (op == STL_C)
			{

				ll_sc++;

			}

			if (op == LDL_L)
			{
				ll_sc++;
			}
			if(MD_OP_FLAGS(op) & F_FENCE)
				mb_count++;

			/* drain RUU for TRAPs and system calls */
			if (MD_OP_FLAGS (op) & F_TRAP)
			{
//				#define OSF_SYS_getthreadid 500
//				#define OSF_SYS_STATS 530
//				#define OSF_SYS_BARRIER_STATS 531
//				#define OSF_SYS_LOCK_STATS 532
//				#define OSF_SYS_BARRIER_INSTR 533

                #define OSF_SYS_getthreadid 162
				#define OSF_SYS_STATS 174
				#define OSF_SYS_BARRIER_STATS 175
				#define OSF_SYS_LOCK_STATS 176
				#define OSF_SYS_BARRIER_INSTR 177
				qword_t syscode = current->regs.regs_R[MD_REG_V0];
				if(current->spec_mode
					|| !(syscode == OSF_SYS_getthreadid || syscode == OSF_SYS_STATS || /*syscode == OSF_SYS_BARRIER_STATS ||*/ syscode == OSF_SYS_BARRIER_INSTR/* || syscode == OSF_SYS_LOCK_STATS*/))
				{
					if (current->RUU_num != 0)
					{
						stallCause[threadid] = StallTrap;
						break;
					}
				}

				if (current->spec_mode)
					panic ("drained and speculative");
			}

			current->regs.regs_PC = current->fetch_data[current->fetch_head]->regs_PC;
			current->pred_PC = current->fetch_data[current->fetch_head]->pred_PC;
			current->regs.regs_NPC = current->regs.regs_PC + sizeof (md_inst_t);

			/* maintain $r0 semantics (in spec and non-spec space) */
			current->regs.regs_R[MD_REG_ZERO] = 0;
			current->spec_regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
			current->regs.regs_F.d[MD_REG_ZERO] = 0.0;
			current->spec_regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

				/* default effective address (none) and access */
				addr = 0;
				is_write = FALSE;

				/* Wattch: Get values of source operands */
#if defined(TARGET_PISA)
				val_ra = GPR (RS);
				val_rb = GPR (RT);
#elif defined(TARGET_ALPHA)
#endif

				/* set default fault - none */
				fault = md_fault_none;
				soft_error = FALSE;

				/*RN: 12.04.06 */
				tempRegPrev = current->regs;


				ushLdFlag = 0;

					switch (op)
					{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,CLASS,O1,O2,I1,I2,I3)						\
						case OP:							\
														/* compute output/input dependencies to out1-2 and in1-3 */	\
						in1 = I1; in2 = I2; in3 = I3;					\
						out1 = O1; out2 = O2; 						\
						if(in1 < 32) val_ra = current->regs.regs_R[31 - in1];		\
						else	val_ra = current->regs.regs_F.q[in1 - 32];		\
						if(in2 < 32) val_rb = current->regs.regs_R[31 - in2];		\
						else	val_rb = current->regs.regs_F.q[in2 - 32];		\
						if(out1 < 32) regVal = current->regs.regs_R[31 - out1];		\
						else regVal = current->regs.regs_F.q[out1 - 32];		\
						/* execute the instruction */					\
						if(second_run) SYMCAT(OP,_IMPL);				\
						break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)									\
						case OP:							\
														/* could speculatively decode a bogus inst, convert to NOP */	\
						op = MD_NOP_OP;							\
						/* compute output/input dependencies to out1-2 and in1-3 */	\
						out1 = NA; out2 = NA;						\
						in1 = NA; in2 = NA; in3 = NA;					\
						/* no EXPR */							\
						break;
#define CONNECT(OP)		/* nada... */
						/* the following macro wraps the instruction fault declaration macro
						   with a test to see if the trace generator is in non-speculative
						   mode, if so the instruction fault is declared, otherwise, the
						   error is shunted because instruction faults need to be masked on
						   the mis-speculated instruction paths */

#define DECLARE_FAULT(FAULT)										\
						{								\
							if (!current->spec_mode  )					\
							fault = (FAULT);						\
							/* else, spec fault, ignore it, always terminate exec... */	\
							break;								\
						}

#define THREADID (current->id)
#include "machine.def"
						default:
							/* can speculatively decode a bogus inst, convert to a NOP */
							op = MD_NOP_OP;
							/* compute output/input dependencies to out1-2 and in1-3 */
							out1 = NA;
							out2 = NA;
							in1 = NA;
							in2 = NA;
							in3 = NA;
							/* no EXPR */
					}
				if (!current->spec_mode)
				{
					if(!(collect_lock_stats[threadid] | collect_barrier_stats[threadid]))
					{
						pure_num_insn++;
						current->pure_num_insn++;
					}
					total_num_insn++;
					sim_num_insn++;
					current->sim_num_insn++;
				}

				if (fault != md_fault_none)
				{
					fprintf (stderr, "sim_cycle %ld\n sim_insn  %ld, thrdid  %d, pc %lx, Next_pc %lx\n", (unsigned long) sim_cycle, (unsigned long) current->sim_num_insn, current->id, (unsigned long) current->regs.regs_PC,
							(unsigned long) current->regs.regs_PC);
					fprintf (stderr, "fetch pc  %lx,fetch  Next_pc  %lx\n", (unsigned long) current->fetch_regs_PC, (unsigned long) current->fetch_pred_PC);
					fatal ("Thread %d, non-speculative fault (%d) detected @ %lx", threadid, fault, (unsigned long) current->regs.regs_PC);
				}


#if defined(TARGET_PISA)
				val_rc = GPR (RD);
				val_ra_result = GPR (RS);
#elif defined(TARGET_ALPHA)
				if (out1 < 32)
					val_rc = current->regs.regs_R[31 - out1];
				else
					val_rc = current->regs.regs_F.q[out1 - 32];
				if ((MD_OP_FLAGS (op) & (F_LOAD)) && isFPMemAcc (op))
					val_ra_result = FPR_Q (RA);
				else
					val_ra_result = GPR (RA);
#endif

				tempReg = current->regs;

				/* update memory access stats */
				if ((MD_OP_FLAGS (op) & F_MEM))
				{
					sim_total_refs++;
					if (!current->spec_mode)
						sim_num_refs++;

					if (MD_OP_FLAGS (op) & F_STORE)
						is_write = TRUE;
					else
					{
						sim_total_loads++;
						if (!current->spec_mode)
							sim_num_loads++;
					}
				}

				br_taken = (current->regs.regs_NPC != (current->regs.regs_PC + sizeof (md_inst_t)));
				br_pred_taken = (current->pred_PC != (current->regs.regs_PC + sizeof (md_inst_t)));

				/* is this a NOP */
				if (op != MD_NOP_OP)
				{
					/* for load/stores:
					   idep #0     - store operand (value that is store'ed)
					   idep #1, #2 - eff addr computation inputs (addr of access)

					   resulting RUU/LSQ operation pair:
					   RUU (effective address computation operation):
					   idep #0, #1 - eff addr computation inputs (addr of access)
					   LSQ (memory access operation):
					   idep #0     - operand input (value that is store'd)
					   idep #1     - eff addr computation result (from RUU op)

					   effective address computation is transfered via the reserved
					   name DTMP
					 */

					/* fill in RUU reservation station */

					/*092904mwr: extra RS for dup */

					DUPLICATION_access[threadid]++;	//mwr: duplication access
					duplication_access++;	//mwr: duplication access

					rs = &current->RUU[current->RUU_tail];

#ifdef HOTLEAKAGE
					rs->slip = sim_cycle - 1;
#endif //HOTLEAKAGE
					if(!ruu_inorder_issue)
					{
						rename_access++;
						current->rename_access++;
					}

					rs->threadid = threadid;
					rs->IR = inst;
					rs->op = op;
					rs->instnum = ++instseqnum;
					rs->isSyncInst = (rs->op == LDL_L || rs->op == STL_C);
					rs->isSyncInstBar = (collect_barrier_stats[threadid]);
					rs->isSyncInstBarRel = (collect_barrier_release && isBarRelInst(current->regs.regs_PC));
					//if(op == LDL_L)
					//	fprintf(fp_trace, "load link threadid %d\n", threadid);
					//else if(op == STL_C)
					//	fprintf(fp_trace, "store conditional threadid %d\n", threadid);
					if(current->regs.regs_PC >= 100000 && threadid == 15)
					{
						last_last_pc = last_pc;
						last_pc = current->regs.regs_PC;
					}

					rs->PC = current->regs.regs_PC;
					rs->next_PC = current->regs.regs_NPC;
					rs->pred_PC = current->pred_PC;
					rs->in_LSQ = FALSE;
					rs->ea_comp = FALSE;
					rs->abank = -1;
					rs->pbank = -1;
					rs->recover_inst = FALSE;
					rs->dir_update = *dir_update_ptr;
					rs->stack_recover_idx = stack_recover_idx;
					rs->spec_mode = current->spec_mode;
					rs->fetch_time = fetch_time;
					rs->disp_time = sim_cycle;
					rs->finish_time = 0;
					rs->cache_ready = 0;
					rs->in_qwait = 0;
					rs->issue_time = 0;
					rs->addr = 0;
					rs->distissue = 0;
					rs->lsqEntry = 0;
					rs->miss_flag = 0;
					/* rs->tag is already set */
					rs->seq = ++(current->inst_seq);
					rs->queued = rs->issued = rs->completed = FALSE;
					rs->ptrace_seq = pseq;
					rs->counted_iissueq = 0;
					rs->counted_fissueq = 0;
					rs->invalidationReceived = 0;
					rs->duplicate = (1 - second_run);	//if second run =1 then 0 else 1

					rs->prevRegVal = regVal;
					/*RN: 12.04.06 */
					rs->backupReg = tempRegPrev;
					rs->writewait = 0;
					rs->ReadShareDir = 0;
					rs->writemiss = 0;
					rs->STL_C_fail = 0;
					if (!second_run)
						rs->spec_mode = 0;
					rs->faulting_instr = FALSE;

#ifdef PWR_WAKE
					rs->counted_iready = 0;
					rs->counted_fready = 0;
					rs->counted_lsqready = 0;
#endif

					rs->freed = 0;
					rs->val_ra = val_ra;
					rs->val_rb = val_rb;
					rs->val_rc = val_rc;
					rs->val_ra_result = val_ra_result;
					rs->when_ready[0] = WAIT_TIME;
					rs->when_ready[1] = WAIT_TIME;
					rs->when_ready[2] = WAIT_TIME;
					rs->when_idep_ready[0] = WAIT_TIME;
					rs->when_idep_ready[1] = WAIT_TIME;
					rs->when_idep_ready[2] = WAIT_TIME;
					rs->idep_name[0] = 0;
					rs->idep_name[1] = 0;
					rs->idep_name[2] = 0;
					rs->isL1Miss = 0;

						rs->sim_num_insn = current->sim_num_insn;
					rs->outReg = out1;

					rs->prevReg = tempReg;

					if ((MD_OP_FLAGS (op) & F_MEM) && !second_run)	//100104mwr
						rs->op = MD_AGEN_OP;

					/* split ld/st's into two operations: eff addr comp + mem access */
					if ((MD_OP_FLAGS (op) & F_MEM) && second_run)
					{
						lsq = &current->LSQ[current->LSQ_tail];
#ifdef HOTLEAKAGE
						lsq->slip = sim_cycle - 1;
#endif //HOTLEAKAGE
						rs->op = MD_AGEN_OP;
						rs->ea_comp = TRUE;
						/*RN: 10.11.06 */
						////////Enhanced LSQ////////
						//Invalidate the dispatch time of forwarded store.
						//Set the flag
						lsq->sqStoreForwardT = -1;
						/*
						   lsq->ushSpecLdDelay = 0;
						 */
						lsq->ushFlag = ushLdFlag;


						lsq->threadid = threadid;
						lsq->IR = inst;
						lsq->op = op;
						lsq->instnum = instseqnum;
						lsq->isSyncInst = (lsq->op == LDL_L || lsq->op == STL_C);
						lsq->isSyncInstBar = (collect_barrier_stats[threadid]);
						lsq->isSyncInstBarRel = (collect_barrier_release && isBarRelInst(current->regs.regs_PC));
						lsq->STL_C_fail = 0;

						lsq->PC = current->regs.regs_PC;
						lsq->next_PC = current->regs.regs_NPC;
						lsq->pred_PC = current->pred_PC;
						lsq->in_LSQ = TRUE;
						lsq->ea_comp = FALSE;
						lsq->in_qwait = 0;
						lsq->distissue = 0;
						lsq->freed = 0;
						lsq->abank = -1;
						lsq->pbank = -1;

						lsq->recover_inst = FALSE;
						lsq->dir_update.pdir1 = lsq->dir_update.pdir2 = NULL;
						lsq->dir_update.pmeta = NULL;
						lsq->stack_recover_idx = 0;
						lsq->spec_mode = current->spec_mode;
						lsq->fetch_time = fetch_time;
						lsq->disp_time = sim_cycle;
						lsq->finish_time = 0;
						lsq->cache_ready = 0;
						lsq->issue_time = 0;
						lsq->counted_iissueq = 0;
						lsq->counted_fissueq = 0;
						lsq->miss_flag = 0;

						lsq->invalidationReceived = 0;
#ifdef PWR_WAKE
						lsq->counted_iready = 0;
						lsq->counted_fready = 0;
						lsq->counted_lsqready = 0;
#endif
						for (mm = 0; mm < CLUSTERS; mm++)
						{
							lsq->st_reach[mm] = 0;
						}

						lsq->backupReg = tempRegPrev;
						lsq->addr = addr;
						rs->addr = addr;

						rs->abank = lsq->abank;
						if (!BANKPRED)
						{
							rs->pbank = rs->abank;
							lsq->pbank = lsq->abank;
						}
						rs->abank = rs->abank % CLUSTERS;
						lsq->abank = rs->abank;
						rs->pbank = rs->pbank % CLUSTERS;
						lsq->pbank = rs->pbank;
						if (!lsq->spec_mode
							)
						{
							if (lsq->abank == lsq->pbank)
								ac_bank_pred_corr++;
							else
								ac_bank_pred_wrong++;
						}
						lsq->seq = ++(current->inst_seq);
						lsq->ptrace_seq = ptrace_seq++;
						lsq->val_ra = val_ra;
						lsq->val_rb = val_rb;
						lsq->val_rc = val_rc;
						lsq->val_ra_result = val_ra_result;
						ptrace_newuop (lsq->ptrace_seq, "internal ld/st", lsq->PC, 0);
						ptrace_newstage (lsq->ptrace_seq, PST_DISPATCH, 0);
						lsq->queued = lsq->issued = lsq->completed = FALSE;

						lsq->when_ready[0] = WAIT_TIME;
						lsq->when_ready[1] = WAIT_TIME;
						lsq->when_ready[2] = WAIT_TIME;
						lsq->when_idep_ready[0] = WAIT_TIME;
						lsq->when_idep_ready[1] = WAIT_TIME;
						lsq->when_idep_ready[2] = WAIT_TIME;
						lsq->idep_name[0] = 0;
						lsq->idep_name[1] = 0;
						lsq->idep_name[2] = 0;


							lsq->sim_num_insn = current->sim_num_insn;
						lsq->outReg = out1;

						/*RN: 12.04.06 */
						lsq->robEntry = rs->index;
						rs->lsqEntry = lsq->index;
						lsq->isPrefetch = 0;
						lsq->isL1Miss = 0;

						rs->ipreg[0] = 0;
						rs->ipreg[1] = current->ltopmap[in2];
						rs->ipreg[2] = current->ltopmap[in3];

						rs->cluster = lsq->cluster = threadid;
						current->iissueq_thrd++;
						iissueq++;
						iissueq_cl[rs->threadid]++;

#ifdef  PWR_CLUSTER
						iqram_access_cl[rs->cluster]++;
#endif

						lsq->counted_iissueq = 1;

						lsq->pbank = lsq->cluster;
						rs->pbank = rs->cluster;

						rs->when_inq = sim_cycle;
						lsq->when_inq = rs->when_inq;

						lsq->ipreg[0] = current->ltopmap[in1];
						lsq->ipreg[1] = 0;
						lsq->ipreg[2] = 0;

						lsq->oldpreg = current->ltopmap[out1];
						lsq->opreg = get_free_reg (out1, threadid);

						//                                      lsq->opreg = current->ltopmap[out1][lsq->cluster];


						ruu_link_idep (rs, /* idep_ready[] index */ 0, NA);
						ruu_link_idep (rs, /* idep_ready[] index */ 1, in2);
						ruu_link_idep (rs, /* idep_ready[] index */ 2, in3);

						ruu_install_odep (rs, /* odep_list[] index */ 0, DTMP);
						ruu_install_odep (rs, /* odep_list[] index */ 1, NA);

						rs->out1 = DTMP;
						rs->opreg = 0;
						rs->oldpreg = 0;

						ruu_link_idep (lsq, STORE_OP_INDEX, in1);
						ruu_link_idep (lsq, STORE_ADDR_INDEX, DTMP);
						ruu_link_idep (lsq, 2, NA);

						ruu_install_odep (lsq, 0, out1);
						ruu_install_odep (lsq, 1, out2);
						lsq->out1 = out1;

						n_dispatched++;

						current->RUU_tail = (current->RUU_tail + 1) % RUU_size;
						current->RUU_num++;
						current->LSQ_tail = (current->LSQ_tail + 1) % LSQ_size;
						current->LSQ_num++;

						if ((MD_OP_FLAGS (op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD))
						{
							{
								if (out1 != 0 && out1 != 63)	/* Prefetch Instruction */
									current->m_shLQNum++;
								else
									lsq->isPrefetch = 1;
							}
						}
						else if ((MD_OP_FLAGS (op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE))
						{
								current->m_shSQNum++;
						}

						lsq_access++;
						current->lsq_access++;
						lsq_preg_access++;
						current->lsq_preg_access++;


						/*mwr */
						if(!ruu_inorder_issue)
						{
							rob2_access++;
							current->rob2_access++;
						}

						if (OPERANDS_READY (rs))
						{
							n_ready++;
							readyq_enqueue (rs);
						}
						RSLINK_INIT (current->last_op, lsq);

						if (((MD_OP_FLAGS (op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE)) && OPERANDS_READY (lsq))
						{
							lsq_store_data_access++;
							readyq_enqueue (lsq);
						}

						if (op == STL_C || op == LDL_L)
						{

							//      fprintf(stderr,"dispatch: %lld  %s %d %d %llx\n",sim_cycle,MD_OP_NAME(op),threadid,lsq->index,addr);
						}

					}
					else	/* !(MD_OP_FLAGS(op) & F_MEM)        --------- not load/store */
					{
						rs->ipreg[0] = current->ltopmap[in1];
						rs->ipreg[1] = current->ltopmap[in2];
						rs->ipreg[2] = current->ltopmap[in3];

						rs->oldpreg = current->ltopmap[out1];
						rs->opreg = get_free_reg (out1, threadid);
						rs->out1 = out1;

						rs->cluster = rs->threadid;

						if ((MD_OP_FUCLASS (op) > 3) && (MD_OP_FUCLASS (op) < 10))
						{
							current->fissueq_thrd++;
							fissueq++;
							fissueq_cl[rs->threadid]++;
#ifdef  PWR_CLUSTER
							fqram_access_cl[rs->cluster]++;
#endif
							rs->counted_fissueq = 1;
							fqram_access++;
							current->fqram_access++;
						}
						else
						{
							current->iissueq_thrd++;
							iissueq++;
							iissueq_cl[rs->threadid]++;
#ifdef  PWR_CLUSTER
							iqram_access_cl[rs->cluster]++;
#endif
							rs->counted_iissueq = 1;
							iqram_access++;
							current->iqram_access++;
						}
						/*					rs->opreg = current->ltopmap[out1];*/
						rs->when_inq = sim_cycle;

						if (!second_run)
						{
							ruu_link_idep (rs, /* idep_ready[] index */ 0, in1);
							ruu_link_idep (rs, /* idep_ready[] index */ 1, in2);
							ruu_link_idep (rs, /* idep_ready[] index */ 2, in3);
							if (out1)
							{
								ruu_install_odep_skip (rs, /* odep_list[] index */ 0, out1);
								ruu_install_odep_skip (rs, /* odep_list[] index */ 1, out2);
								rs->out1 = out1;
							}
							else
							{
								ruu_install_odep_skip (rs, /* odep_list[] index */ 0, DTMP);
								ruu_install_odep_skip (rs, /* odep_list[] index */ 1, NA);
								rs->out1 = DTMP;
							}
						}
						else
						{
							ruu_link_idep (rs, /* idep_ready[] index */ 0, in1);
							ruu_link_idep (rs, /* idep_ready[] index */ 1, in2);
							ruu_link_idep (rs, /* idep_ready[] index */ 2, in3);

							ruu_install_odep (rs, /* odep_list[] index */ 0, out1);
							ruu_install_odep (rs, /* odep_list[] index */ 1, out2);
							rs->out1 = out1;
						}

						/* install operation in the RUU */
						n_dispatched++;

						current->RUU_tail = (current->RUU_tail + 1) % RUU_size;
						current->RUU_num++;
						if(!ruu_inorder_issue)
						{
							rob2_access++;
							current->rob2_access++;
						}

						/* issue op if all its reg operands are ready (no mem input) */
						if (OPERANDS_READY (rs))
						{
							n_ready++;
							readyq_enqueue (rs);
							current->last_op = RSLINK_NULL;
						}
						else
						{
							RSLINK_INIT (current->last_op, rs);
						}
					}
				}
				else
				{
					/* this is a NOP, no need to update RUU/LSQ state */
					rs = NULL;
				}

				/* one more instruction executed, speculative or otherwise */
				sim_total_insn++;
				if (MD_OP_FLAGS (op) & F_CTRL)
					sim_total_branches++;
				if (!current->spec_mode && second_run)	//100104mwr
				{
					/* if this is a branching instruction update BTB, i.e., only
					   non-speculative state is committed into the BTB */
					if (MD_OP_FLAGS (op) & F_CTRL)
					{
						sim_num_branches++;
						/*mwr: this bpred_access++ was not here before */
						if (!(rs->duplicate))
						{
							current->bpred_access++;
							bpred_access++;
						}

						if (current->pred && bpred_spec_update == spec_ID)	//102704mwr: update only for main thread
						{
							bpred_update (current->pred,
									/* branch address */ current->regs.regs_PC,
									/* actual target address */ current->regs.regs_NPC,
									/* taken? */ current->regs.regs_NPC != (current->regs.regs_PC +
										sizeof (md_inst_t)),
									/* pred taken? */ current->pred_PC != (current->regs.regs_PC +
										sizeof (md_inst_t)),
									/* correct pred? */ current->pred_PC == current->regs.regs_NPC,
									/* opcode */ op,
									/* predictor update ptr */ &rs->dir_update,
									rs->threadid, (bpred_btb_use_masterid ? thecontexts[rs->threadid]->masterid : rs->threadid));
						}
					}
					if (current->pred_PC != current->regs.regs_NPC && !current->fetch_redirected)
					{
						current->spec_mode = TRUE;
						rs->recover_inst = TRUE;
						current->recover_PC = current->regs.regs_NPC;
						copy_to (threadid);
					}
				}

			/* entered decode/allocate stage, indicate in pipe trace */
			ptrace_newstage (pseq, PST_DISPATCH, (current->pred_PC != current->regs.regs_NPC) ? PEV_MPOCCURED : 0);
			if (op == MD_NOP_OP)
			{
				ptrace_endinst (pseq);
			}
			/* update any stats tracked by PC */
			for (i = 0; i < pcstat_nelt; i++)
			{
				counter_t newval;
				int delta;

				/* check if any tracked stats changed */
				newval = STATVAL (pcstat_stats[i]);
				delta = newval - pcstat_lastvals[i];
				if (delta != 0)
				{
					stat_add_samples (pcstat_sdists[i], current->regs.regs_PC, delta);
					pcstat_lastvals[i] = newval;
				}
			}

			current->fetch_head = (current->fetch_head + 1) & (ruu_ifq_size - 1);
			current->fetch_num--;
			temp_fetch_num--;

			/* need to enter DLite at least once per cycle */
			if (!made_check)
			{
				if (dlite_check_break (0, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_cycle))
					dlite_main (current->regs.regs_PC, /* no next PC */ 0, sim_cycle, &(current->regs), current->mem);
			}
		}			// while
	}				//if temp_fetch_num
	else
		stallCause[threadid] = StallFetchEmpty;
	if (!n_dispatched)
	{
		m_totalStall[threadid]++;
		if(stallCause[threadid])
		{
			switch (stallCause[threadid])
			{
				case StallRegFull :
					m_regFull[threadid]++;
					break;
				case StallIssueFull	:
					m_issueFull[threadid]++;
					break;
				case StallFetchEmpty :
					if(thecontexts[threadid]->ruu_fetch_issue_delay)
						m_il1Miss[threadid]++;
					else
						m_fetchEmpty[threadid]++;
					break;
				case StallSTLC	:
					m_STLC[threadid]++;
					break;
				case StallTrap	:
					m_trap[threadid]++;
					break;
				default	: panic("No stall cause found");
			}
		}
		else if (current->m_shLQNum == m_shLQSize)
		{
			stallCause[threadid] = StallLQFull;
			m_sqLQFull[threadid]++;
			if(current->LSQ[current->LSQ_head].miss_flag)
				m_LoadRealMiss[threadid]++;
			else
				m_LoadRealHit[threadid]++;
		}
		else if (m_shSQSize == (current->m_shSQNum+m_L1WBufCnt[threadid]))
		{
			stallCause[threadid] = StallSQFull;
			m_sqSQFull[threadid]++;
			if(current->LSQ[current->LSQ_head].miss_flag)
				m_StoreRealMiss[threadid]++;
			else
				m_StoreRealHit[threadid]++;
		}
		else if(current->RUU_num == RUU_size)
		{
			stallCause[threadid] = StallRUUFull;
			m_RUUFull[threadid]++;
		}

		if((stallCause[threadid] == StallRUUFull || stallCause[threadid] == StallLQFull || stallCause[threadid] == StallSQFull || stallCause[threadid] == StallRegFull || stallCause[threadid] == StallIssueFull || stallCause[threadid] == StallSTLC || stallCause[threadid] == StallTrap))
		{
			if(current->LSQ_num)
			{
				if(MD_OP_FLAGS(current->LSQ[current->LSQ_head].op) & F_LOAD)
				{
					if(current->LSQ[current->LSQ_head].miss_flag)
						m_LoadStallMiss[threadid]++;
					else
						m_LoadStallHit[threadid]++;
					m_LoadMiss[threadid]++;
				}
				else
				{
					m_StoreMiss[threadid]++;
					if(current->LSQ[current->LSQ_head].miss_flag)
						m_StoreStallMiss[threadid]++;
					else
						m_StoreStallHit[threadid]++;
					if(m_update_miss[threadid])
						m_StoreUpdateMiss[threadid]++;
					if(m_update_miss[threadid] && !m_update_miss_flag[threadid])
					{ /* m_update_miss[threadid] means the first entry in the store queue is a write update */
						m_update_miss_flag[threadid] = 1; /*this flag means there is an instruction depending on write update*/
						m_update_miss_depend_start[threadid] = sim_cycle;
					}
				}
			}
			else if(MD_OP_FLAGS(current->RUU[current->RUU_head].op) & F_FENCE)
			{
				m_StoreMiss[threadid]++;
				if(current->LSQ[current->LSQ_head].miss_flag)
					m_StoreStallMiss[threadid]++;
				else
					m_StoreStallHit[threadid]++;
				if(m_update_miss[threadid])
					m_StoreUpdateMiss[threadid]++;
				if(m_update_miss[threadid] && !m_update_miss_flag[threadid])
				{ /* m_update_miss[threadid] means the first entry in the store queue is a write update */
					m_update_miss_flag[threadid] = 1; /*this flag means there is an instruction depending on write update*/
					m_update_miss_depend_start[threadid] = sim_cycle;
				}
			}
		}
	}
	return n_dispatched;
}


/*
 *  RUU_FETCH() - instruction fetch pipeline stage(s)
 */
/* initialize the instruction fetch pipeline stage */
	void
fetch_init (int threadid)
{
	/* allocate the IFETCH -> DISPATCH instruction queue */
	int i = 0;
	context *current;

	current = thecontexts[threadid];

	for (i = 0; i < ruu_ifq_size; i++)
	{
		current->fetch_data[i] = (struct fetch_rec *) calloc (1, sizeof (struct fetch_rec));
		if (!current->fetch_data[i])
			fatal ("out of virtual memory");
		current->fetch_data[i]->index = i;
		current->fetch_data[i]->threadid = threadid;
	}
	current->fetch_num = 0;
	current->fetch_tail = current->fetch_head = 0;
	IFQ_count = 0;
	IFQ_fcount = 0;

}

static int last_inst_missed = FALSE;
static int last_inst_tmissed = FALSE;

/* fetch up as many instruction as one branch prediction and one cache line
   acess will support without overflowing the IFETCH -> DISPATCH QUEUE */

	static void
ruu_fetch (void)
{
	int i = 0, j = 0, k = 0, lat, tlb_lat, done = FALSE;
	md_inst_t inst;
	int stack_recover_idx;
	int branch_cnt, ray_mult;
	md_addr_t fetch_set = 0, inst_set;
	int index;
	enum md_opcode op;		/* decoded opcode enum */

	int threadid, tID = 0;
	context *current = 0;
	int threadsfetched = 0;
	int temp;

	if (disable_ray)
		ray_mult = 1;
	else
		ray_mult = 2;


	while (tID < numcontexts)
	{
		


		current = thecontexts[tID];
		tID++;

		threadid = current->id;

		if (current->wait_for_fetch == 2)
			current->wait_for_fetch = 1;
		fetch_done[threadid] = 0;

		for (j = 0, done = FALSE, branch_cnt = 0;
				(current->fetch_num < ruu_ifq_size-1)
				&& !done && !current->ruu_fetch_issue_delay && current->running && j < (ruu_decode_width * fetch_speed) /*&& current->active_this_cycle && ((ray_mult * (j + current->fetch_num) + current->RUU_num) < (RUU_size - 1))*/; i++, j++)
		{
			if (!j)
			{			//access cache once
				icache_access++;
				current->icache_access++;
			}

			/* fetch an instruction at the next predicted fetch address */
			current->fetch_regs_PC = current->fetch_pred_PC;

			/* is this a bogus text address? (can happen on mis-spec path) */
			if (current->ld_text_base <= current->fetch_regs_PC && current->fetch_regs_PC < (current->ld_text_base + current->ld_text_size) &&/*hq:I suggest remove "!"*/ !(current->fetch_regs_PC & (sizeof (md_inst_t) - 1)))
			{
				/* read instruction from memory */
				{
					MD_FETCH_INST (inst, current->mem, current->fetch_regs_PC);
				}
				MD_SET_OPCODE (op, inst);

				lat = cache_il1[threadid]->hit_latency;

				/*fetch one block per cycle */
				if (!j)
					fetch_set = CACHE_SET (cache_il1[threadid], current->fetch_regs_PC);
				inst_set = CACHE_SET (cache_il1[threadid], current->fetch_regs_PC);
				if (fetch_set != inst_set)
					break;

#ifdef  FIX2_BUG
				if (!current->wait_for_fetch)
				{
#endif
					if (cache_il1[threadid])
					{
						/* access the I-cache */

						rsCacheAccess = NULL;
						if(ISCOMPRESS(sizeof(md_inst_t)) == 32)
							printf("debug 1\n");
						lat = cache_access (cache_il1[threadid], Read, IACOMPRESS(current->fetch_regs_PC),
								NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
								NULL, NULL, /*ruu writewait*/NULL,threadid, NULL);
						if (lat > (cache_il1[threadid]->hit_latency))
							last_inst_missed = TRUE;
					}

					if (itlb[threadid])
					{
						if(ISCOMPRESS(sizeof(md_inst_t)) == 32)
							printf("debug 2\n");
						tlb_lat = cache_access(itlb[threadid], Read, IACOMPRESS(current->fetch_regs_PC),
								NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
								NULL, NULL, /*ruu writewait*/NULL, threadid, NULL); /* Note: Assumes FE is at cluster zero. This isn't used anyways though. */

						if (tlb_lat > itlb[threadid]->hit_latency)
							last_inst_tmissed = TRUE;

						lat = max2 (tlb_lat, lat);
					}

					if (lat != (cache_il1[threadid]->hit_latency))
					{
						current->ruu_fetch_issue_delay += (lat - 1);
#ifdef  FIX2_BUG
						current->wait_for_fetch = 2;
#endif
						break;
					}
					/* else, I-cache/I-TLB hit */

#ifdef  FIX2_BUG
				}
#endif
			}			// if PC valid
			else
			{
				inst = MD_NOP_INST;
			}

			if (current->pred)
			{
				branchInst *branchInfo = NULL;
				enum md_opcode op;

				/* pre-decode instruction, used for bpred stats recording */
				MD_SET_OPCODE (op, inst);

				/* get the next predicted fetch address; only use branch predictor
				   result for branches (assumes pre-decode bits); NOTE: returned
				   value may be 1 if bpred can only predict a direction */
				if (MD_OP_FLAGS (op) & F_CTRL)
				{
					md_addr_t btarget;

					if ((MD_OP_FLAGS (op) & (F_CTRL | F_UNCOND | F_INDIRJMP)) == (F_CTRL | F_UNCOND | F_INDIRJMP))
						btarget = current->fetch_regs_PC + 4 + 4 * (inst & 0x00003fff); /* JMP Hint */
					else
						btarget = current->fetch_regs_PC + 4 + 4 * SEXT21 (inst & 0x001fffff);  /* Branch Taken Address */

					current->fetch_pred_PC = bpred_lookup (current->pred,
							/* branch address */ current->fetch_regs_PC,
							/* target address */ 0,
							/* opcode */ op,
							/* call? */ MD_IS_CALL (op),
							/* return? */ MD_IS_RETURN (op),
							/* updt */ &(current->fetch_data[current->fetch_tail]->dir_update),
							/* RSB index */ &stack_recover_idx,
							/* thread id */ current->id,
							/* thread id for BTB */ (bpred_btb_use_masterid ? current->masterid : current->id));
				}
				else
					current->fetch_pred_PC = 0;

				/* valid address returned from branch predictor? */
				if (!current->fetch_pred_PC)
				{
					current->fetch_pred_PC = current->fetch_regs_PC + sizeof (md_inst_t);
				}
				else if (current->fetch_pred_PC != current->fetch_regs_PC + sizeof (md_inst_t))
				{

					branch_cnt++;	//should this be just for lead threads!!!!!!!!!!
					if (branch_cnt >= fetch_speed)
						done = TRUE;
				}
			}
			else
			{
				current->fetch_pred_PC = current->fetch_regs_PC + sizeof (md_inst_t);
			}

			current->fetch_total_insn++;
			current->fetch_data[current->fetch_tail]->threadid = threadid;
			current->fetch_data[current->fetch_tail]->IR = inst;
			current->fetch_data[current->fetch_tail]->regs_PC = current->fetch_regs_PC;
			current->fetch_data[current->fetch_tail]->pred_PC = current->fetch_pred_PC;
			current->fetch_data[current->fetch_tail]->stack_recover_idx = stack_recover_idx;
			current->fetch_data[current->fetch_tail]->ptrace_seq = ptrace_seq++;
			current->fetch_data[current->fetch_tail]->fetch_time = (long) sim_cycle;

			ptrace_newinst (current->fetch_data[current->fetch_tail]->ptrace_seq, inst, current->fetch_data[current->fetch_tail]->regs_PC, 0);
			ptrace_newstage (current->fetch_data[current->fetch_tail]->ptrace_seq, PST_IFETCH, ((last_inst_missed ? PEV_CACHEMISS : 0) | (last_inst_tmissed ? PEV_TLBMISS : 0)));

			current->fetch_tail = (current->fetch_tail + 1) & (ruu_ifq_size - 1);
			current->fetch_num++;
			fetch_done[threadid]++;
		}			// for inst/thread limit

#ifdef  FIX2_BUG
		/* A complete cycle after icache miss have to pass before access icach */
		if (current->wait_for_fetch == 1)
		{
			if (!j && current->ruu_fetch_issue_delay)
				current->wait_for_fetch = 2;
			else
				current->wait_for_fetch = 0;
		}
#endif
	}				// while thread limit not reached
}



/* default machine state accessor, used by DLite */
	static char *			/* err str, NULL for no err */
simoo_mstate_obj (FILE * stream,	/* output stream */
		char *cmd,	/* optional command string */
		struct regs_t *regs,	/* registers to access */
		struct mem_t *mem)	/* memory space to access */
{

	int threadid = mem->threadid;
	context *current;

	current = thecontexts[threadid];

	if (!cmd || !strcmp (cmd, "help"))
		fprintf (stream,
				"mstate commands:\n"
				"\n"
				"    mstate help   - show all machine-specific commands (this list)\n"
				"    mstate stats  - dump all statistical variables\n"
				"    mstate res    - dump current functional unit resource states\n"
				"    mstate ruu    - dump contents of the register update unit\n"
				"    mstate lsq    - dump contents of the load/store queue\n"
				"    mstate eventq - dump contents of event queue\n"
				"    mstate readyq - dump contents of ready instruction queue\n"
				"    mstate cv     - dump contents of the register create vector\n"
				"    mstate rspec  - dump contents of speculative regs\n" "    mstate mspec  - dump contents of speculative memory\n" "    mstate fetch  - dump contents of fetch stage registers and fetch queue\n" "\n");
	else if (!strcmp (cmd, "stats"))
	{
		/* just dump intermediate stats */
		sim_print_stats (stream);
	}
	else if (!strcmp (cmd, "res"))
	{
		/* dump resource state */
		res_dump (fu_pool, stream);
	}
	else if (!strcmp (cmd, "ruu"))
	{
		/* dump RUU contents */
		ruu_dump (stream, threadid);
	}
	// /*  else if (!strcmp(cmd, "lsq"))
	//     {
	//       /* dump LSQ contents */
	//       lsq_dump(stream);
	//     }*/
	else if (!strcmp (cmd, "eventq"))
	{
		/* dump event queue contents */
		eventq_dump (stream, threadid);
	}
	else if (!strcmp (cmd, "readyq"))
	{
		/* dump event queue contents */
		readyq_dump (stream, threadid);
	}
	else if (!strcmp (cmd, "cv"))
	{
		/* dump event queue contents */
		cv_dump (stream, threadid);
	}
	else if (!strcmp (cmd, "rspec"))
	{
		/* dump event queue contents */
		rspec_dump (stream, threadid);
	}
	else if (!strcmp (cmd, "mspec"))
	{
		/* dump event queue contents */
		mspec_dump (stream, threadid);
	}
	// /*  else if (!strcmp(cmd, "fetch"))
	//     {
	//       /* dump event queue contents */
	//       fetch_dump(stream,threadid);
	//     }*/
	else
		return "unknown mstate command";

	/* no error */
	return NULL;
}

#define TAKE_STATS_PHASE	1
#define DRAIN_PIPELINE_PHASE	2
#define RECONFIGURE_PHASE	3
#define NORMAL_PHASE		4
#define PRIVTOSHARED		5
#define SHAREDTOPRIV		6
#define EXTRA_PHASE		7
int changeConfig = 0;
int currentPhase = NORMAL_PHASE;


/*102204mwr: forward decleration*/

void regenerate_fetch_queue ();
void copy_fetch_queue ();

void regenerate_f_queue ();
void copy_f_queue ();
void branch_address_copy (int masterid, long long int chunkExecuted);


/* start simulation, program loaded, processor precise state initialized */
/* execution run times */
static time_t print_time, now_time;
void ruu_release_writebuffer ();
void ruu_write_from_Writebuffer ();
#ifdef	COLLECT_STAT_STARTUP
void fstfwd_fineAdjust ();
void fstfwd_collectStats();
#endif
	void
sim_main (void)
{
	int unPrintLimit = 1;
	//***************************

	double TotalRouterEnergy = 0.0, TotalRouterEnergy_back = 0.0;
	double TotalCPUEnergy = 0.0, TotalCPUEnergy_back = 0.0;
	double CPUPower, RouterPower;
	double CPUActive, RouterActive;
	//***********END*************
	int i, j, k, l;
	int tempcl, maxguy, maxval, ref_mis, ref_imis, ref_br, ref_refs, ref_cyc, mm, nn;
	int got_ref_point = 0;
	int minguy, minval;
	float cyc_count = 0;
	int num_picks[NUMPHASES + 1];
	unsigned long sim_elapsed_sec;
	int dispatched = 0;

	int threadid = 0, thread_stop = 0;
	context *current;
	int killall = 0;
	int fetch_blocked = 1;	/* to check if all threads have fetch delay */
	int cnt;

	FILE *fp, *fp1;
	int brNum, ldNum;

#ifdef DO_COMPLETE_FASTFWD
	int out1, out2, in1, in2, in3;	/* output/input register names */
	int x, loopiter;
#endif


	signal (SIGFPE, SIG_IGN);

	/* cache statitics intialization */
	nack_counter = 0;
	flip_counter = 0;
	store_conditional_failed = 0;
	L1_flip_counter = 0;
	e_to_m = 0;
	int f;
	for(f=0;f<16;f++)
		mem_port_lat[f] = 0;
	totaleventcountnum = 0;
	totalWriteIndicate = 0;
	total_exclusive_modified = 0;
	total_exclusive_conf = 0;
	total_exclusive_cross = 0;
	totalSyncEvent = 0;
	totalNormalEvent = 0;
	totalSyncWriteup = 0;
	totalSyncWriteM = 0;
	totalSyncReadM = 0;
	totalmisstimeforSync = 0;
	totalmisstimeforNormal = 0;
	total_L1_prefetch = 0;
	write_back_early = 0;
	total_L1_first_prefetch = 0;
	total_L1_sec_prefetch = 0;
	totalreqcountnum = 0;
	totalmisscountnum = 0;
	involve_4_hops = 0;
	involve_4_hops_upgrade = 0;
	involve_4_hops_miss = 0;
	involve_2_hops = 0;
	involve_2_hop_touch = 0;
	involve_2_hop_wb = 0;
	involve_4_hops_wm = 0;
	totalSplitWM = 0;
	totalSplitNo = 0;
	involve_2_hops_wm = 0;
	involve_2_hops_upgrade = 0;
	involve_2_hops_miss = 0;
	data_shared_read = 0;
	data_private_read = 0;
	data_shared_write = 0;
	data_private_write = 0;
	total_all_close = 0;
	total_all_almostclose = 0;
	total_not_all_close = 0;
	total_p_c_events = 0;
	total_consumers = 0;
	total_data_consumers = 0;
	total_data_far = 0;
	total_data_at_corner = 0;
	total_data_close = 0;
	total_packets_in_neighbor = 0;
	total_packets_at_corners = 0;
	average_outside_percent = 0;
	average_outside_abs_percent = 0;
	average_inside_percent = 0;
	average_corner_percent = 0;

	for(i = 0; i< numcontexts; i++)
		sharer_num[i] = 0;

#if PROCESS_MODEL
	for (i = 0; i < 128; i++)
	{
		sharedAddressList[i] = NULL;
	}
	LastSharedAddress = 0;
	LastSharedSize = 0;
#endif

	initClusterInfo ();

#if 	defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
	busInit ();
#endif

	char fnm[256];
	char nm[256];

	strcpy (nm, thecontexts[0]->fname);

	for (l = 0; l < MAXTHREADS; l++)
	{
		numThreads[l] = mta_maxthreads;
	}


	/*100404mwr: this has to be fixed in a proper manner later */
	n_limit_thrd[0] = n_limit_thrd[1] = n_limit_thrd[2] = n_limit_thrd[3] = 1;

	/*102104mwr: initialize the cycle stall parameters */

	fprintf (stderr, "Issueq size   - int %d fp %d\n", IIQ_size, FIQ_size);
#ifdef REG_RENAME_CONFIGUARE
	fprintf (stderr, "rename Pregfile size - int %d fp %d\n", IPREG_size, FPREG_size);
#else //REG_RENAME_CONFIGUARE
	fprintf (stderr, "Pregfile size - int %d fp %d\n", IPREG_size, FPREG_size);
#endif //REG_RENAME_CONFIGUARE
	fprintf (stderr, "CLUSTERS %d\n", CLUSTERS);
	fprintf (stderr, "ROWCOUNT %d\n", ROWCOUNT);
	fprintf (stderr, "CENTRAL_CACHE %d\n", CENTRAL_CACHE);
	fprintf (stderr, "CENTRALCACHELAT %d\n", CENTRALCACHELAT);
	fprintf (stderr, "CACHELOC %d\n", CACHELOC);
	fprintf (stderr, "PRED_BR_CHANGES %d\n", PRED_BR_CHANGES);
	fprintf (stderr, "PRED_JR_CHANGES %d\n", PRED_JR_CHANGES);
	fprintf (stderr, "DYNAMIC_CHANGE %d\n", DYNAMIC_CHANGE);
	fprintf (stderr, "METRIC_CHANGE %d\n", METRIC_CHANGE);
	fprintf (stderr, "LINEAR_PHASE %d\n", LINEAR_PHASE);
	fprintf (stderr, "SMALLEST_CONFIG %d\n", SMALLEST_CONFIG);
	fprintf (stderr, "GRID %d\n", GRID);
	fprintf (stderr, "CACHEPORTS %d\n", CACHEPORTS);
	fprintf (stderr, "RES_CACHEPORTS %d\n", RES_CACHEPORTS);
	fprintf (stderr, "READY_COMM %d\n", READY_COMM);
	fprintf (stderr, "HOLY_REGS %d\n", HOLY_REGS);
	fprintf (stderr, "NUM_H_REGS %d\n", NUM_H_REGS);
	fprintf (stderr, "COPY_THRESH %d\n", COPY_THRESH);
	fprintf (stderr, "FIRST_FIT %d\n", FIRST_FIT);
	fprintf (stderr, "FIRST_FIT_ENFORCE %d\n", FIRST_FIT_ENFORCE);
	fprintf (stderr, "FIRST_VAL %d\n", FIRST_VAL);
	fprintf (stderr, "STEER_ENFORCE %d\n", STEER_ENFORCE);
	fprintf (stderr, "MOD_N %d\n", MOD_N);
	fprintf (stderr, "MOD_N_N %d\n", MOD_N_N);
	fprintf (stderr, "FIRST_MOD %d\n", FIRST_MOD);
	fprintf (stderr, "GROUPSIZE %d\n", GROUPSIZE);
	fprintf (stderr, "MOD_N_T %d\n", MOD_N_T);
	fprintf (stderr, "ANYWILLDO %d\n", ANYWILLDO);
	fprintf (stderr, "NEIGHBOR_SEEK %d\n", NEIGHBOR_SEEK);
	fprintf (stderr, "PICKMIDDLE %d\n", PICKMIDDLE);
	fprintf (stderr, "MIDTHRESH %d\n", MIDTHRESH);
	fprintf (stderr, "HOPTIME %d\n", HOPTIME);
	fprintf (stderr, "COMM_LATENCY %d\n", COMM_LATENCY);
#ifndef CONFIG_BASED_ICS
	fprintf (stderr, "FE_LAT %d\n", FE_LAT);
#endif
	fprintf (stderr, "CACHE_REFILL_LAT %d\n", CACHE_REFILL_LAT);
	fprintf (stderr, "MPRED_COMM_LAT %d\n", MPRED_COMM_LAT);
	fprintf (stderr, "LD_PROXIMITY %d\n", LD_PROXIMITY);
	fprintf (stderr, "LDPROX_FACTOR %d\n", LDPROX_FACTOR);
	fprintf (stderr, "STEER_2NONREADY %d\n", STEER_2NONREADY);
	fprintf (stderr, "STEER_CRITICAL %d\n", STEER_CRITICAL);
	fprintf (stderr, "ILOAD_MIN %d\n", ILOAD_MIN);
	fprintf (stderr, "IFACTOR %d\n", IFACTOR);
	fprintf (stderr, "RLOAD_MIN %d\n", RLOAD_MIN);
	fprintf (stderr, "RFACTOR %d\n", RFACTOR);
	fprintf (stderr, "DEP_FACTOR %d\n", DEP_FACTOR);
	fprintf (stderr, "LD_FACTOR %d\n", LD_FACTOR);
	fprintf (stderr, "IQBUF %d\n", IQBUF);
	fprintf (stderr, "REGFREE %d\n", REGFREE);
	fprintf (stderr, "FREE_CURR_REGS %d\n", FREE_CURR_REGS);
	fprintf (stderr, "FREE_INTERVAL %d\n", FREE_INTERVAL);
	fprintf (stderr, "DISPDELAY %d\n", DISPDELAY);
	fprintf (stderr, "RING_BW %d\n", RING_BW);
	fprintf (stderr, "TIMEWINDOW %d\n", TIMEWINDOW);
	fprintf (stderr, "ST_ADDR_COMM %d\n", ST_ADDR_COMM);
	fprintf (stderr, "ST_RING_BW %d\n", ST_RING_BW);
	fprintf (stderr, "RING_IC %d\n", RING_IC);
	fprintf (stderr, "IGNORE_COMM_LATENCY %d\n", IGNORE_COMM_LATENCY);
	fprintf (stderr, "TOL_COUNT %d\n", TOL_COUNT);
	fprintf (stderr, "STOREDELAY %d\n", STOREDELAY);
	fprintf (stderr, "BANKPRED %d\n", BANKPRED);
	fprintf (stderr, "BANKMISPREDLAT %d\n", BANKMISPREDLAT);
	fprintf (stderr, "BANKP_SPEC %d\n", BANKP_SPEC);
	fprintf (stderr, "LEV1SIZE %d\n", LEV1SIZE);
	fprintf (stderr, "LEV2SIZE %d\n", LEV2SIZE);
	fprintf (stderr, "XORSIZE %d\n", XORSIZE);
	fprintf (stderr, "WORD_INTERLEAVED %d\n", WORD_INTERLEAVED);
	fprintf (stderr, "WORDSIZE %d\n", WORDSIZE);
	fprintf (stderr, "SORT %d\n", SORT);
	fprintf (stderr, "NUMPHASES %d\n", NUMPHASES);
	fprintf (stderr, "MIN_INSN_INT %d\n", MIN_INSN_INT);
	fprintf (stderr, "STOP_EXPL_THRESH %d\n", STOP_EXPL_THRESH);
	fprintf (stderr, "ILPTAGCHECK %d\n", ILPTAGCHECK);
	fprintf (stderr, "DISTILP %d\n", DISTILP);
	fprintf (stderr, "DISTILPTHRESH %d\n", DISTILPTHRESH);
	fprintf (stderr, "SAMPLETHRESH %d\n", SAMPLETHRESH);
	fprintf (stderr, "BR_INT %d\n", BR_INT);
	fprintf (stderr, "ITABLESIZE %d\n", ITABLESIZE);
	fprintf (stderr, "OWINDOWSIZE %d\n", OWINDOWSIZE);
	fprintf (stderr, "ILPCLEARINT %d\n", ILPCLEARINT);
	fprintf (stderr, "STATPRINT %d\n", STATPRINT);
	fprintf (stderr, "CYCNOISE %d\n", CYCNOISE);
	fprintf (stderr, "NUMCYCTHRESH %d\n", NUMCYCTHRESH);
	fprintf (stderr, "STATINT %d\n", STATINT);
	fprintf (stderr, "FREE_REG_THRESH %d\n", FREE_REG_THRESH);
	fprintf (stderr, "TABLE2SIZE %d\n", TABLE2SIZE);
	fprintf (stderr, "TABLESIZE %d\n", TABLESIZE);

#ifdef	COLLECT_STAT_STARTUP

	/* set up program entry state */
	{
		thrdPerJobCnt = 1;
		current = thecontexts[threadid];
		current->regs.regs_PC = current->ld_prog_entry;
		current->regs.regs_NPC = current->regs.regs_PC + sizeof (md_inst_t);

		/* check for DLite debugger entry condition */
		if (dlite_check_break (current->regs.regs_PC, /* no access */ 0, /* addr */ 0, 0, 0))
			dlite_main (current->regs.regs_PC, current->regs.regs_PC + sizeof (md_inst_t), sim_cycle, &(current->regs), current->mem);

		/* fast forward simulator loop, performs functional simulation for
		   FASTFWD_COUNT insts, then turns on performance (timing) simulation */
		if (!current->fastfwd_done)
		{
			counter_t icount;
			md_inst_t inst;	/* actual instruction bits */
			enum md_opcode op;	/* decoded opcode enum */
			md_addr_t target_PC;	/* actual next/target PC address */
			md_addr_t addr;	/* effective address, if load/store */
			int is_write;	/* store? */
			byte_t temp_byte;	/* temp variable for spec mem access */
			half_t temp_half;	/* " ditto " */
			word_t temp_word;	/* " ditto " */
            int fl_flag_0;      // alternative to branch condition(GDB) @ fanglei
            fl_flag_0 = 0;      // alternative to branch condition(GDB) @ fanglei



#if defined(HOST_HAS_QWORD) && defined(TARGET_ALPHA)
			qword_t temp_qword = 0;	/* " ditto " */
#endif /* HOST_HAS_QWORD && TARGET_ALPHA */

			enum md_fault_type fault;

			fprintf (stderr, "Thread %d, sim: ** fast forwarding %ld insts **\n", threadid, current->fastfwd_count);

			while (!current->fastfwd_done)
			{
				/* maintain $r0 semantics */
				current->regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
				current->regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

				/* get the next instruction to execute */
				MD_FETCH_INST (inst, current->mem, current->regs.regs_PC);
				cache_warmup (cache_il1[threadid], Read, IACOMPRESS(current->fetch_regs_PC), ISCOMPRESS(sizeof(md_inst_t)), threadid);

				/* set default reference address */
				addr = 0;
				is_write = FALSE;

				/* set default fault - none */
				fault = md_fault_none;

				/* decode the instruction */
				MD_SET_OPCODE (op, inst);

				/* execute the instruction */
				switch (op)
				{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
					case OP:							\
													SYMCAT(OP,_IMPL);						\
					break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
					case OP:							\
													panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#undef DECLARE_FAULT
#define DECLARE_FAULT(FAULT)						\
					{ fault = (FAULT); break; }
#include "machine.def"
					default:
						panic ("attempted to execute a bogus opcode");
				}

				if (fault != md_fault_none)
					fatal ("fault (%d) detected @ 0x%08p", fault, current->regs.regs_PC);

				/* update memory access stats */
				if (MD_OP_FLAGS (op) & F_MEM)
				{
					if (MD_OP_FLAGS (op) & F_STORE)
					{
						is_write = TRUE;
						/* Warmup the L1 and L2 cache */
#ifndef PARALLEL_EMUL
						cache_warmup(cache_dl1[threadid], Write, addr & ~7, 8, threadid);
#endif
					}
#ifndef PARALLEL_EMUL
					else
						cache_warmup(cache_dl1[threadid], Read, addr & ~7, 8, threadid);
#endif
				}

				/* check for DLite debugger entry condition */
				if (dlite_check_break (current->regs.regs_NPC, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_num_insn))
					dlite_main (current->regs.regs_PC, current->regs.regs_NPC, sim_num_insn, &(current->regs), current->mem);


				/* go to the next instruction */
//                fprintf(outFile,"%X\n",current->regs.regs_PC);        // recode PC @ fanglei
                if( fl_count == 19090609 )               // alternative to branch condition(GDB) @ fanglei
                    fl_flag_0 = 1;                                      // alternative to branch condition(GDB) @ fanglei
                fl_count  = fl_count +1;                                // number of executed instructions @ fanglei
				current->regs.regs_PC = current->regs.regs_NPC;
				current->regs.regs_NPC += sizeof (md_inst_t);
			}
		}
	}

	for(i = 0; i < numcontexts; i++)
	{
		thecontexts[i]->jobThdId = i;
		thecontexts[i]->helpThdId = -1;
		thecontexts[i]->oracleId = -1;
	}

	printf ("ff Stage 0 done\n");
	fflush (stdout);
	//fstfwd_fineAdjust ();
	fstfwd_collectStats();
#else
	/* set up program entry state */
	for (threadid = 0; threadid < numcontexts; threadid++)
	{
		current = thecontexts[threadid];
		current->regs.regs_PC = current->ld_prog_entry;
		current->regs.regs_NPC = current->regs.regs_PC + sizeof (md_inst_t);

		/* check for DLite debugger entry condition */
		if (dlite_check_break (current->regs.regs_PC, /* no access */ 0, /* addr */ 0, 0, 0))
			dlite_main (current->regs.regs_PC, current->regs.regs_PC + sizeof (md_inst_t), sim_cycle, &(current->regs), current->mem);

		/* fast forward simulator loop, performs functional simulation for
		   FASTFWD_COUNT insts, then turns on performance (timing) simulation */
		if (current->fastfwd_count > 0)
		{
			counter_t icount;
			md_inst_t inst;	/* actual instruction bits */
			enum md_opcode op;	/* decoded opcode enum */
			md_addr_t target_PC;	/* actual next/target PC address */
			md_addr_t addr;	/* effective address, if load/store */
			int is_write;	/* store? */
			byte_t temp_byte;	/* temp variable for spec mem access */
			half_t temp_half;	/* " ditto " */
			word_t temp_word;	/* " ditto " */

#if defined(HOST_HAS_QWORD) && defined(TARGET_ALPHA)
			qword_t temp_qword = 0;	/* " ditto " */
#endif /* HOST_HAS_QWORD && TARGET_ALPHA */

			enum md_fault_type fault;

			fprintf (stderr, "Thread %d, sim: ** fast forwarding %ld insts **\n", threadid, current->fastfwd_count);

			for (icount = 0; icount < current->fastfwd_count; icount++)
			{
				/* maintain $r0 semantics */
				current->regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
				current->regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

				/* get the next instruction to execute */
				MD_FETCH_INST (inst, current->mem, current->regs.regs_PC);

				/* set default reference address */
				addr = 0;
				is_write = FALSE;

				/* set default fault - none */
				fault = md_fault_none;

				/* decode the instruction */
				MD_SET_OPCODE (op, inst);

				/* execute the instruction */
				switch (op)
				{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
					case OP:							\
													SYMCAT(OP,_IMPL);						\
					break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
					case OP:							\
													panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#undef DECLARE_FAULT
#define DECLARE_FAULT(FAULT)						\
					{ fault = (FAULT); break; }
#include "machine.def"
					default:
						panic ("attempted to execute a bogus opcode");
				}

				if (fault != md_fault_none)
					fatal ("fault (%d) detected @ 0x%08p", fault, current->regs.regs_PC);

				/* update memory access stats */
				if (MD_OP_FLAGS (op) & F_MEM)
				{
					if (MD_OP_FLAGS (op) & F_STORE)
					{
						is_write = TRUE;
						cache_warmup(cache_dl1[threadid], Write, addr & ~7, 8, threadid);
					}
					else
						cache_warmup(cache_dl1[threadid], Read, addr & ~7, 8, threadid);
				}

				/* check for DLite debugger entry condition */
				if (dlite_check_break (current->regs.regs_NPC, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_num_insn))
					dlite_main (current->regs.regs_PC, current->regs.regs_NPC, sim_num_insn, &(current->regs), current->mem);

				/* go to the next instruction */
				current->regs.regs_PC = current->regs.regs_NPC;
				current->regs.regs_NPC += sizeof (md_inst_t);
			}
		}
	}
#endif


	printf ("ff stage 1 done\n");
	fflush (stdout);

	fprintf (stderr, "sim: ** starting performance simulation **\n");
#ifdef LOAD_PREDICTOR
	fprintf (stderr, " LOAD_PREDICTOR\n");
#ifdef COUNT_MISS_ONLY
	fprintf (stderr, "COUNT_MISS_ONLY \n");
#endif
#ifdef PREDICT_LOAD_COMM
	fprintf (stderr, "PREDICT_LOAD_COMM \n");
#endif
#ifdef GATE_L1_MISS
	fprintf (stderr, "GATE_L1_MISS \n");
#endif
#ifdef GATE_L2_MISS
	fprintf (stderr, "GATE_L2_MISS\n");
#endif
#ifdef PDG
	fprintf (stderr, "PDG \n");
#endif
#endif
#ifdef ACTIVE_N_WAY
	fprintf (stderr, " ACTIVE_N_WAY\n");
#ifdef FIXED_N_WAY
	fprintf (stderr, "  FIXED_N_WAY\n");
#endif
#endif
#ifdef  SMT_TEST
	fprintf (stderr, "SMT_TEST\n");
#endif
#ifdef  THRD_WAY_CACHE
	fprintf (stderr, " THRD_WAY_CACHE\n");
#endif
#ifdef  N_WAY_CACHE
	fprintf (stderr, " N_WAY_CACHE: default only,  %d\n", n_way_cache);
#ifdef SET_BASED
	fprintf (stderr, "SET_BASED\n");
#endif
#endif
#ifdef PARTITIONED_LSQ
	fprintf (stderr, "PARTITIONED_LSQ\n");
#else
	fprintf (stderr, "(PARTITIONED_LSQ not defined)\n");
#endif
#ifdef CENTRALIZED_LSQ
	fprintf (stderr, "CENTRALIZED_LSQ\n");
#endif
#ifdef DE_CENTRALIZED_LSQ
	fprintf (stderr, "DE_CENTRALIZED_LSQ\n");
#ifdef DE_CENTRALIZED_POWER_BUG
	fprintf (stderr, " DE_CENTRALIZED_POWER_BUG\n");
#endif
#ifdef  SPLIT_LSQ
	fprintf (stderr, "SPLIT_LSQ\n");
#endif
#ifdef DISTRIBUTE_STORE
	fprintf (stderr, "DISTRIBUTE_STORE\n");
#endif
#ifdef  PWR_DE_CENTRALIZED_LSQ
	fprintf (stderr, " PWR_DE_CENTRALIZED_LSQ\n");
#endif
#endif
#ifdef ALLOW_ANY_CLUSTER
	fprintf (stderr, "ALLOW_ANY_CLUSTER\n");
#endif
#ifdef   CLUSTER_PRIORITY
	fprintf (stderr, "CLUSTER_PRIORITY\n");
#endif
#ifdef DISTRIBUTED_STEER
	fprintf (stderr, "DISTRIBUTED_STEER\n");
#endif
#ifdef CENTRALIZED_STEER
	fprintf (stderr, "CENTRALIZED_STEER\n");
#endif
#ifdef MINIMUM_STEER
	fprintf (stderr, "MINIMUM_STEER\n");
#endif
#ifdef INIT_CENTRALIZED_STEER
	fprintf (stderr, "INIT_CENTRALIZED_STEER\n");
#endif
#ifdef CMP_STEER
	fprintf (stderr, "CMP_STEER\n");
#endif
#ifdef INIT_CMP_STEER
	fprintf (stderr, "INIT_CMP_STEER\n");
#endif
#ifdef N_WAY_STEER
	fprintf (stderr, "N_WAY_STEER ,N_WAY:default %d \n", n_way);
#ifdef PRIVATE_STEER
	fprintf (stderr, " PRIVATE_STEER\n");
#endif
#ifdef N_WAY_ADAPT
	fprintf (stderr, " N_WAY_ADAPT,ADAPT_RATE %d \n", ADAPT_RATE);
#ifdef STABLE_IPC_ADAPT
	fprintf (stderr, "STABLE_IPC_ADAPT\n");
#endif
#ifdef PERIODIC_INCREASE_ADAPT
	fprintf (stderr, "PERIODIC_INCREASE_ADAPT\n");
#endif
#ifdef DOUBLE_ADAPT
	fprintf (stderr, "DOUBLE_ADAPT\n");
#endif
#ifdef OVERALL_LIMIT
	fprintf (stderr, "OVERALL_LIMIT\n");
#endif
#ifdef  PRIVATE_STEER
	fprintf (stderr, "PRIVATE_STEER\n");
#endif
#endif
#ifdef THRD_PRIO_ADAPT
	fprintf (stderr, " THRD_PRIO_ADAPT\n");
#ifdef ADAPT_LIMIT
	fprintf (stderr, " ADAPT_LIMIT\n");
#endif
#endif
#ifdef PDG_ADAPT
	fprintf (stderr, " PDG_ADAPT\n");
#endif
#ifdef PDG_STOP_ADAPT
	fprintf (stderr, " PDG_STOP_ADAPT\n");
#endif
#ifdef PDG_THRD_ADAPT
	fprintf (stderr, " PDG_THRD_ADAPT\n");
#ifdef PDG_THRD_AGGRESSIVE
	fprintf (stderr, " PDG_THRD_AGGRESSIVE\n");
#endif
#ifdef PDG_THRD_SOFT
	fprintf (stderr, " PDG_THRD_SOFT\n");
#endif
#endif
#ifdef PDG_THRD_PRIO_ADAPT
	fprintf (stderr, "PDG_THRD_PRIO_ADAPT \n");
#ifdef COMM_OPER_LIMIT
	fprintf (stderr, "COMM_OPER_LIMIT COMM_OPER_RATIO %2.2f\n", COMM_OPER_RATIO);
#endif
#ifdef ADAPT_NO_PRIO
	fprintf (stderr, " ADAPT_NO_PRIO\n");
#endif
#ifdef ADAPT_NO_COMM_OPER
	fprintf (stderr, " ADAPT_NO_COMM_OPER\n");
#endif
#ifdef AVG_LIMIT
	fprintf (stderr, " AVG_LIMIT\n");
#endif
#ifdef PDG_THRD_AGGRESSIVE
	fprintf (stderr, " PDG_THRD_AGGRESSIVE\n");
#endif
#ifdef PDG_THRD_SOFT
	fprintf (stderr, " PDG_THRD_SOFT\n");
#endif
#endif
#ifdef INIT_N_WAY_CENTRAL
	fprintf (stderr, " INIT_N_WAY_CENTRAL\n");
#endif
#ifdef N_WAY_BALANCE
	fprintf (stderr, "N_WAY_BALANCE \n");
#endif
#ifdef THRD_PROFILE
	fprintf (stderr, " THRD_PROFILE\n");
#endif
#ifdef N_CACHE_FIXED
	fprintf (stderr, "N_CACHE_FIXED \n");
#endif
#ifdef PRIVATE_CACHE
	fprintf (stderr, " PRIVATE_CACHE\n");
#endif
#endif
#ifdef STEER_MEM_LSQ
	fprintf (stderr, "STEER_MEM_LSQ\n");
#endif
#ifdef STEER_MEM_NEIGHBOR
	fprintf (stderr, "STEER_MEM_NEIGHBOR\n");
#endif
#ifdef STEER_OPRANDS
	fprintf (stderr, "STEER_OPRANDS\n");
#endif
#ifdef   STEER_FU
	fprintf (stderr, "STEER_FU\n");
#endif
#ifdef SELECTIVE_DISPATCH
	fprintf (stderr, "SELECTIVE_DISPATCH\n");
#endif
#ifdef SELECTIVE_FETCH
	fprintf (stderr, "SELECTIVE_FETCH\n");
#endif
#ifdef SELECTIVE_DISPATCH_GATE
	fprintf (stderr, "SELECTIVE_DISPATCH_GATE\n");
#ifdef UNREADY_PRIVATE
	fprintf (stderr, " UNREADY_PRIVATE \n");
#endif
#ifdef UNREADY_COUNT
	fprintf (stderr, " UNREADY_COUNT  %d\n", UNREADY_COUNT_LIMIT);
#endif
#ifdef UNREADY_COUNT_ONLY
	fprintf (stderr, " UNREADY_COUNT_ONLY \n");
#endif
#ifdef UNREADY_CHAIN_PRIVATE
	fprintf (stderr, " UNREADY_CHAIN_PRIVATE  %d\n", UNREADY_CHAIN_LIMIT);
#endif
#endif
#ifdef  RAW
	fprintf (stderr, "RAW ,default RAWSIZE %d\n", RAWSIZE);
#endif
#ifdef  RABMAP
	fprintf (stderr, "RABMAP \n");
#endif
#ifdef  RABCOMM
	fprintf (stderr, "RABCOMM \n");
#endif
#ifdef  ALU_MERGE
	fprintf (stderr, "ALU_MERGE \n");
#endif
#ifdef INST_TRANS_RATE
	fprintf (stderr, "INST_TRANS_RATE\n");
#ifdef TRACE_REG
	fprintf (stderr, " TRACE_REG\n");
#endif
#endif
#ifdef IGNORE_ALL_COMM_LATENCY
	fprintf (stderr, "IGNORE_ALL_COMM_LATENCY\n");
#endif
#ifdef IGNORE_MEM_COMM_LATENCY
	fprintf (stderr, "IGNORE_MEM_COMM_LATENCY\n");
#endif
#ifdef COUNT_COMMIT
	fprintf (stderr, "COUNT_COMMIT\n");
#endif
#ifdef CYCLE_JUMP
	fprintf (stderr, "CYCLE_JUMP \n");
#endif
#ifdef FETCH_ONE_BLOCK
	fprintf (stderr, "FETCH_ONE_BLOCK \n");
#endif
#ifdef  MSHR
	fprintf (stderr, "MSHR , MSHRSIZE %d\n", MSHRSIZE);
#endif
#ifdef GENERIC_FU
	fprintf (stderr, "GENERIC_FU\n");
#endif
#ifdef NO_EXTRA_REG
	fprintf (stderr, "NO_EXTRA_REG\n");
#endif
#ifdef NO_CACHE_BANK
	fprintf (stderr, "NO_CACHE_BANK\n");
#endif
#ifdef NO_QFILL_CONTENTION
	fprintf (stderr, "NO_QFILL_CONTENTION\n");
#endif
#ifdef NO_BUS_CONTENTION
	fprintf (stderr, " NO_BUS_CONTENTION \n");
#endif
#ifdef EVENT_CACHE_UPDATE
	fprintf (stderr, "EVENT_CACHE_UPDATE\n");
#ifdef APPROXIMATE_LAT
	fprintf (stderr, " APPROXIMATE_LAT \n");
#endif
#ifdef  ACURATE_LAT
	fprintf (stderr, " ACURATE_LAT \n");
#endif
#ifdef  MOST_ACURATE_LAT
	fprintf (stderr, " MOST_ACURATE_LAT \n");
#endif
#endif
#ifdef PORT_PER_BANK
	fprintf (stderr, "PORT_PER_BANK\n");
#endif
#ifdef   LINE_INTERLEAVED
	fprintf (stderr, "LINE_INTERLEAVED\n");
#endif
#ifdef  PWR_CLUSTER
	fprintf (stderr, "PWR_CLUSTER,ISSUE_PER_CLUSTER \n");
#endif
#ifdef  CHECKS
	fprintf (stderr, "CHECKS\n");
#endif
#ifdef  PWR__SMT_RR
	fprintf (stderr, "PWR__SMT_RR\n");
#endif
	fprintf (stderr, "REMOVE_ILP = %d\n", (unsigned int) REMOVE_ILP);
	fprintf (stderr, "HOPTIME = %d\n", (unsigned int) HOPTIME);
#ifdef CENTRALIZED_CACHE_BANK
	fprintf (stderr, "CENTRALIZED_CACHE_BANK\n");
#endif
#ifdef IGNORE_MEM_COMM_LATENCY
	fprintf (stderr, "IGNORE_MEM_COMM_LATENCY\n");
#endif
	fprintf (stderr, "DO_DYNAMIC_ADAPTATION = %d\n", (unsigned int) DO_DYNAMIC_ADAPTATION);
	fprintf (stderr, "N_WAY = %d\n", n_way);
	fprintf (stderr, "n_way_cache = %d\n", n_way_cache);
#ifdef OPTICAL_FE_LINK
	fprintf (stderr, "OPTICAL_FE_LINK\n");
#endif
#ifdef MONOLETHIC_MACHINE
	fprintf (stderr, "MONOLETHIC_MACHINE\n");
#endif

#ifdef TOKENB
	fprintf (stderr, "TOKENB\n");
#endif

	{                           /* Open a trace file for debug purposes. */
		extern char *sim_str_dump;
		char *sim_dump = sim_str_dump;
		char strFileName[256];
		if (!sim_dump)
		{
			fprintf (stderr, "No name for dump file\n");
			exit (1);
		}

		strcpy (strFileName, sim_dump);
		strcat (strFileName, ".trc");
		fp_trace = fopen (strFileName, "w");
		if(!fp_trace)	panic("Could not open trace file");
	}

	allocate_free_event_list();

	/* set up timing simulation entry state */
	for (threadid = 0; threadid < numcontexts; threadid++)
	{
		current = thecontexts[threadid];
		current->fetch_pred_PC = current->regs.regs_PC;
		current->regs.regs_NPC = current->regs.regs_PC;

		current->fetch_regs_PC = current->regs.regs_PC - sizeof (md_inst_t);
		current->regs.regs_PC = current->regs.regs_PC - sizeof (md_inst_t);
#ifdef PDG
		current->active = 1;
#endif
	}

	/* Initialize structures before going into detailed simulation - Ronz */
	bin_init ();
	cache_init ();
    page_table_init ();
    tlb_mshr_init ();
    tlb_cu_init ();
#ifdef  STREAM_PREFETCHER
	init_sp ();
#endif
#ifdef  L1_STREAM_PREFETCHER
	l1init_sp ();
#endif

	for (i = 0; i < MD_TOTAL_REGS; i++)
	{
		disp_time[i] = 0;
	}

	for (i = 0; i < numcontexts; i++)
	{
		reg_init (i);
		bpred_init (i);
	}

#ifdef REG_RENAME_CONFIGUARE
	fprintf (stderr, "Total Pregfile size - int %d fp %d\n", IPREG_size, FPREG_size);
#endif //REG_RENAME_CONFIGUARE

	for (i = 0; i < CLUSTERS; i++)
	{
		num_storeq[i] = 0;
		max_storeq[i] = 0;
	}

	for (mm = 0; mm <= NUMPHASES; mm++)
	{
		num_picks[mm] = 0;
	}

	if (CENTRAL_CACHE)
	{
		actual_cacheports = CACHEPORTS;
		actual_clusters = SMALLEST_CONFIG;
	}
	else
	{
		cache_change_config (SMALLEST_CONFIG);
		actual_clusters = SMALLEST_CONFIG;
		actual_cacheports = (CACHEPORTS * actual_clusters) / CLUSTERS;
	}

	rthresh = insn_interval / 100;
	brthresh = insn_interval / 100;
	msthresh = insn_interval / 1000;
	imsthresh = insn_interval / 1000;
	cycthresh = insn_interval / 100;
	rnoise = insn_interval / 20;
	brnoise = insn_interval / 50;
	msnoise = insn_interval / 100;
	imsnoise = insn_interval / 200;

#ifdef	COLLECT_STAT_STARTUP
	actualProcess = 1;
#else
	actualProcess = numcontexts;
#endif
	executeLimits = max_cycle;	


	/* main simulator loop, NOTE: the pipe stages are traverse in reverse order
	   to eliminate this/next state synchronization and relaxation problems */

	for (threadid = 0; threadid < numcontexts; threadid++)
	{
		current = thecontexts[threadid];
		current->active_this_cycle = 1;
	}


	//write buffer Initialization
	for (threadid = 0; threadid < MAXTHREADS; threadid++)
	{
		for (i = 0; i < SMD_L1_WBUF_SIZE; i++)
		{
			m_L1WBuf[threadid][i].PC = 0;
			m_L1WBuf[threadid][i].lsqId = 0;
			m_L1WBuf[threadid][i].robId = 0;
			m_L1WBuf[threadid][i].robExId = 0;
			m_L1WBuf[threadid][i].addr = 0;
			m_L1WBuf[threadid][i].op = 0;
			m_L1WBuf[threadid][i].lat = 0;
			m_L1WBuf[threadid][i].sim_num_insn = 0;
			m_L1WBuf[threadid][i].commitDone = 0;
			m_L1WBuf[threadid][i].writeStarted = 0;
			m_L1WBuf[threadid][i].finishTime = 0;
			m_L1WBuf[threadid][i].entryTime = 0;
			m_L1WBuf[threadid][i].writeValue = 0;
			m_L1WBuf[threadid][i].isMiss = 0;
			m_L1WBuf[threadid][i].STL_C_fail = 0;
			m_L1WBuf[threadid][i].already_check = 0;
		}
		m_L1WBufCnt[threadid] = 0;
		m_L1WBuf_head[threadid] = 0;
		m_L1WBuf_tail[threadid] = 0;
	}

	if (!mystricmp (network_type, "FSOI") || !mystricmp (network_type, "HYBRID"))
		
		maindirectsim(network_receiver, network_conflict);
	
	else if (!mystricmp (network_type, "MESH"))
		
		simEsynetInit(mesh_size+2);
	
	else if ((!mystricmp (network_type, "COMB")))
	
	{
		
		maindirectsim(network_receiver, network_conflict);
		
		simEsynetInit(mesh_size+2);
	
	}
	
	/************************
	//ctb 2013-03-07
	//³õÊ¼»¯Êä³öÎÄ¼þÃû
	//************************
	strcpy(my_simout,sim_simout);
	strcat(my_simout,".dat");
	printf("CTB TEST:%s\n",my_simout);
	
	//init the freq of CPU ROUTER L2cache
	//setall_procfreq(proc_freq);
	//setall_routerfreq(router_freq);
	//setall_l2freq(l2_freq);
	proc_v = VMAX-(1.00-proc_freq)*(VMAX-VMIN)*1.5;
	router_v = VMAX-(1.00-router_freq)*(VMAX-VMIN)*1.5;
	SetProc(proc_freq);
	SetRouter(router_freq);
	CPUPowerInit();
	RouterPowerInit();
	flits_reset();

	Stastic_FILE=fopen("flits.txt","w");
	*/
	//srand((int)time(NULL));
	//if(random_proc || random_router || random_l2)
		//random_num();
	//printf("the random frequency setting times: %d %d %d %d ",num[0],num[1],num[2],num[3]);
	
	

	/*
	if(random_proc)
	{
		for(i=0;i<numcontexts;i++)
		{
		
			random_procfreq(i);

		}
	}
	if(random_router)
	{
		for(i=0;i<numcontexts;i++)
		{
			int k=(i/mesh_size+1)*(mesh_size+2)+i%mesh_size;
		
			random_routerfreq(k);

		}
	}
	if(random_l2)
	{
		for(i=0;i<numcontexts;i++)
		{
		
			random_l2freq(i);

		}
	}

	
	
	int tt,start_id,set_id,m,n;

	for(i=0;i<mesh_size/row;i++)
	{
		for(j=0;j<mesh_size/col;j++)
		{
			tt=rand()%FREQ_NUM;
			start_id=i*row*mesh_size+j*col;
			for(m=0;m<row;m++)
			{
				for(n=0;n<col;n++)
				{
					set_id=start_id+m*mesh_size+n;
					int k=(set_id/mesh_size+1)*(mesh_size+2)+set_id%mesh_size;
					FindValue(freqindex[tt],&ProcFreq[set_id]);
					FindValue(freqindex[tt],&RouterFreq[k]);
					FindValue(freqindex[tt],&L2Freq[set_id]);
					
				}
			}
		}
	}
	

	char SetFreq[6];
	FILE * FreqSet;	

	FreqSet=fopen("FreqSet.txt","r+");
	if(FreqSet!=NULL)
	{
		if(set_proc)
		{
			for(i=0;i<numcontexts;i++)
			{
				fscanf(FreqSet,"%s ", SetFreq);
				FindValue(SetFreq,&ProcFreq[i]);

			}
		}
		if(set_router)
		{
			for(i=0;i<numcontexts;i++)
			{
				fscanf(FreqSet,"%s ", SetFreq);
				int k=(i/mesh_size+1)*(mesh_size+2)+i%mesh_size;
				FindValue(SetFreq,&RouterFreq[k]);

			}
		}
		if(set_l2)
		{
			for(i=0;i<numcontexts;i++)
			{
				fscanf(FreqSet,"%s ", SetFreq);
				FindValue(SetFreq,&L2Freq[i]);

			}
		}
		fclose(FreqSet);
	}
	*/

	
	

	
	for (;;)
	{
		
		//update_active();

#ifdef	DCACHE_MSHR
		for(i = 0; i < numcontexts; i++)
        {
			freeMSHR(cache_dl1[i]->mshr); /* set the used entries free for futural use */
		    tlb_flush_check(i);
        }
        freeL2MSHR(cache_dl2->mshr);
#endif

#ifdef POWER_ACCESS_STAT
		lsq_preg_access_count += lsq_preg_access;
		lsq_wakeup_access_count += lsq_wakeup_access;
#endif


		/* check if pipetracing is still active */
		/* FIXME : make it for thread 0 until study ptrace  */
		ptrace_check_active (thecontexts[0]->regs.regs_PC, sim_num_insn, sim_cycle);

		ptrace_newcycle (sim_cycle);

		for (i = 0; i<numcontexts; i++)
		{
			//if(commit_flag[i] > 2000)
			//	panic("Commit Stall for thread %d", i);
			commit_flag[i] ++; /* stall cycles when commit */
		}
		/* commit entries from RUU/LSQ to architected register file */
		ruu_commit ();


		ruu_release_writebuffer ();
		ruu_write_from_Writebuffer ();


		/* service function unit release events */
		ruu_release_fu ();

		ruu_writeback ();


		if (!bugcompat_mode)
		{
			lsq_refresh ();
			ruu_issue ();
		}

		int ggg;

		for (ggg = 0; ggg < numcontexts; ggg++)
		{
			

			dispatch_done[ggg] = 0;
			//if (thecontexts[ggg]->active_this_cycle)
			{
				dispatch_done[ggg] = dispatched = ruu_dispatch (ggg);
			}
		}

		if (!allForked)
		{
#ifdef	COLLECT_STAT_STARTUP
			for (ggg = 0; ggg < numcontexts; ggg++)
			{
				collectStatStop[ggg] = 0;
				//cache_flush (cache_dl1[ggg], sim_cycle);
			}
			collectStatBarrier = 0;
			allForked = 1;
#else
			initParallelSimulation ();
			collect_stats = 1;
#endif
		}


		if (flushImpStats)
		{
			flushImpStats = 0;
			collect_stats = 1;
			sim_num_insn = 0;
			realSimCycle = 0;

			for (i = 0; i < MAXTHREADS; i++)
			{
				m_sqLQFull[i] = 0;
				m_sqSQFull[i] = 0;
				m_sqNumLdReplay[i] = 0;
				ldstRejectionCount[i] = 0;
				seqConsReplay[i] = 0;
				seqConsInsnReplay[i] = 0;

				//#ifdef SEQUENTIAL_CONSISTENCY
				invalidation_replay[i] = 0;
				//#endif
				totalRUUsize[i] = totalLQsize[i] = totalSQsize[i] = 0;

#ifdef RELAXED_CONSISTENCY
				loadLoadReplayCount[i] = 0;
#endif
			}
			m_sqPrefetchCnt = 0;
		}

		if (timeToReturn)
		{
			fclose (fp_trace);

			for (i = 0; i < numcontexts; i++)
			{
				pureInsn[i] = thecontexts[i]->pure_num_insn;
				simInsn[i] = thecontexts[i]->sim_num_insn;
			}
			fprintf(stderr,"[INFO 0]: sim_num_insn=%llu", pure_num_insn);
			return;
		}

		for (i = 0; i < numcontexts; i++)
		{

			totalRUUsize[i] += thecontexts[i]->RUU_num;
			avgRUUsize[i] = (1.0 * totalRUUsize[i]) / realSimCycle;

			totalLQsize[i] += thecontexts[i]->m_shLQNum;
			avgLQsize[i] = (1.0 * totalLQsize[i]) / realSimCycle;


			totalSQsize[i] += thecontexts[i]->m_shSQNum + m_L1WBufCnt[i];

			avgSQsize[i] = (1.0 * totalSQsize[i]) / realSimCycle;
		}



#ifdef  CHECK_FETCH_QUEUE
		check_fetch_queue ();
#endif

		if (bugcompat_mode)
		{
			/* try to locate memory operations that are ready to execute */
			/* ==> inserts operations into ready queue --> mem deps resolved */
			lsq_refresh ();

			/* issue operations ready to execute from a previous cycle */
			/* <== drains ready queue <-- ready operations commence execution */
			ruu_issue ();
		}

		ruu_fetch ();

		dir_fifo_dequeue();
		dir_eventq_nextevent();
#ifdef LOCK_HARD
		lock_eventq_nextevent();
#endif
		if (!mystricmp (network_type, "FSOI") || !mystricmp (network_type, "HYBRID"))
			directRunSim(sim_cycle);
		else if (!mystricmp (network_type, "MESH"))
            simEsynetRunSim(sim_cycle);
		else if ((!mystricmp (network_type, "COMB")))
		{
			directRunSim(sim_cycle);
            simEsynetRunSim(sim_cycle);
		}

		int f, j;
		for(f=0; f<16; f++)
		{
			if(mem_port_lat[f] != 0)
				mem_port_lat[f] --;
		}
		if(!mystricmp(network_type, "HYBRID"))
		{
			for(f = 0; f< 100; f++)
				for(j = 0; j< 100; j++)
					if(link_ser_lat[f][j] > 0)
						link_ser_lat[f][j] --;
		}

		for (threadid = 0; threadid < numcontexts; threadid++)
		{
			//if(thecontexts[threadid]->active_this_cycle)
				update_power_stats(thecontexts[threadid]->power, threadid, thecontexts[threadid]);

			current = thecontexts[threadid];
			if (current->ruu_fetch_issue_delay)
				current->ruu_fetch_issue_delay--;
		}
		clear_access_stats();

		//      for(threadid=0;threadid < numcontexts; threadid++)
		//          compute_queue(threadid); /* Figure out queue and register file sizes before dispatch. - Ronz */


#ifdef  CHECKS
		check_reg ();
#endif
		check_fu_busy ();

		//******************************
		//ctb 2013-03-07
		//******************************
		//update_proc_counter();
		//update_router_counter();
		

		for(i = 0; i < numcontexts; i++)
		{
			if(thecontexts[i]->running==1 && thecontexts[i]->freeze==0)
				runningCycle[i]++;
		}
        //************end***************

		if (hotspot_active && sim_cycle >= last_hotspot_cycle + hotspot_cycle_sampling_intvl)
		{
			update_hotspot_stats (sim_cycle - last_hotspot_cycle);
			last_hotspot_cycle = sim_cycle;
		}


		int tt;

		if (!spec_benchmarks && max_insts && !max_barrier)
		{
			for (threadid = 0; threadid < numcontexts; threadid++)
			{
				current = thecontexts[threadid];
					if (((unsigned long) (current->sim_num_insn - WARMUP) >= (unsigned long) max_insts) && current->running)
					{
						fprintf (stderr, "Thread %d has exceeded max_insts; killing all threads.\n", threadid);
						killall = 1;
					}
			}
		}

		for (threadid = 0, thread_stop = 0; threadid < numcontexts; threadid++)
		{
			int tmp = 0, tid;

			current = thecontexts[threadid];

				if (((killall || tmp || (spec_benchmarks && max_insts && !max_barrier && ((unsigned long) (current->sim_num_insn - WARMUP) >= (unsigned long) max_insts))) && current->running))
				{
					current->finish_cycle = (unsigned long) (sim_cycle - cyclecheck);
					current->running = 0;
					stopped_num_threads++;

					activecontexts = numcontexts - stopped_num_threads;

					thread_stop = 1;
					fprintf (stderr, "********Thread %d finish at %lu**********\n", threadid, (unsigned long) current->finish_cycle);
					fprintf (stderr, "IPC_patial%d,per thread %4.3f,    ", stopped_num_threads, (double) (sim_num_insn - WARMUP) / (double) (sim_cycle - cyclecheck));

					if (hotspot_active && sim_hotspotout_fd != stderr)
					{
						fprintf (sim_hotspotout_fd, "********Thread %d finish at %lu**********\n", threadid, (unsigned long) current->finish_cycle);
					}

					for (threadid = 0; threadid < numcontexts; threadid++)
					{
						current = thecontexts[threadid];
						if (current->finish_cycle)
							fprintf (stderr, "%4.3f, ", (double) (current->sim_num_insn - WARMUP) / (double) (current->finish_cycle - cyclecheck));
						else
							fprintf (stderr, "%4.3f, ", (double) (current->sim_num_insn - WARMUP) / (double) (sim_cycle - cyclecheck));
					}
					fprintf (stderr, "\n");
					fprintf (stderr, "num_insn_patial%d,per thread %lu,    ", stopped_num_threads, (unsigned long) (sim_num_insn - WARMUP));

					for (threadid = 0; threadid < numcontexts; threadid++)
					{
						current = thecontexts[threadid];
						if (current->finish_cycle)
							fprintf (stderr, "%lu, ", (unsigned long) (current->sim_num_insn - WARMUP));
						else
							fprintf (stderr, "%lu, ", (unsigned long) (current->sim_num_insn - WARMUP));
					}
					fprintf (stderr, "\n");
					if (stopped_num_threads == numcontexts)
					{
						fprintf (stderr, "********All finish  simulation done**********\n");
						fprintf (stderr, "finish_cycle %lu, per thread ,  ", (unsigned long) sim_cycle);
						for (threadid = 0; threadid < numcontexts; threadid++)
						{
							current = thecontexts[threadid];
							fprintf (stderr, "%lu, ", (unsigned long) (current->finish_cycle - WARMUP));
						}
						fprintf (stderr, "\n");
					}
				}
		}
		now_time = time ((time_t *) NULL);

		if (!sim_cycle)
			print_time = now_time;
		sim_elapsed_sec = 0;

		if ((unsigned long) sim_num_insn && ((stopped_num_threads == numcontexts) || thread_stop))
		{
			if (stopped_num_threads == numcontexts)
			{
				for (i = 0; i < numcontexts; i++)
				{
					pureInsn[i] = thecontexts[i]->pure_num_insn;
					simInsn[i] = thecontexts[i]->sim_num_insn;
				}
				sim_cycle -= warmupEndCycle;
				fprintf(stderr,"[INFO 1]: sim_num_insn=%llu", pure_num_insn);
				return;
			}
		}

#define PERIOD 100000
#define INTERVAL 10000
#define NUM_CONFIG 3
#if DO_DYNAMIC_ADAPTATION
		/* Do Dynamic allocation only in parallel section */
		if (mta_maxthreads == 1)
		{
			if (((sim_num_insn % PERIOD) >= 0) && ((sim_num_insn % PERIOD) < NUM_CONFIG * INTERVAL))
				periodic_adaptation ();
		}
#endif

#define PRINT_STATISTICS 0
#if PRINT_STATISTICS
		changeCacheConfig ();
#endif

#ifdef INTERVAL_BASED
		if (!((unsigned long) sim_cycle % ADAPT_INTERVAL) && ((unsigned long) sim_cycle))
		{
#endif //  INTERVAL_BASED
#ifdef  EXAMIN_N_WAY_ADAPT
			update_examin_n_limit ();
#endif
#ifdef  N_WAY_ADAPT
			update_n_limit ();
#endif
#ifdef THRD_PRIO_ADAPT
			priority_n_way ();
#endif
#ifdef PDG_THRD_ADAPT
			pdg_thrd_n_way ();
#endif
#ifdef PDG_THRD_PRIO_ADAPT
			pdg_priority_n_way ();
#endif
#ifdef PDG_ADAPT
			pdg_n_way ();
#endif
#ifdef N_WAY_CACHE_ADAPT
			n_way_cache_adapt ();
#endif //N_WAY_CACHE_ADAPT
#ifdef INTERVAL_BASED
		}
#endif //  INTERVAL_BASED

#if 	defined(BUS_INTERCONNECT) || defined(CROSSBAR_INTERCONNECT)
		busFree ();
#endif
		sim_cycle++;

		reset_ports();



#ifdef SEQUENTIAL_CONSISTENCY
		for (i = 0; i < numcontexts; i++)
		{
			if (invalidationStall[i])
				invalidationStall[i]--;
			if (commitStall[i])
				commitStall[i]--;
		}
#endif
		realSimCycle++;
		int no_ideal = 1;
		for (i = 0; i < numcontexts; i++)
		{
			int lat = 1000;
			if (!allForked)
			{
				if ((thecontexts[i]->freeze == 1) && (thecontexts[i]->running == 0) && ((thecontexts[i]->sleptAt + 1000) < sim_cycle) && (!spec_benchmarks))
				{
					thecontexts[i]->freeze = 0;
					thecontexts[i]->running = 1;
				}
			}
			else
			{
				if ((thecontexts[i]->freeze == 1) && (thecontexts[i]->running == 0) && (thecontexts[i]->sleptAt + lat + (random () % 20) < sim_cycle) && (!spec_benchmarks) && no_ideal)
				{
					thecontexts[i]->freeze = 0;
					thecontexts[i]->running = 1;
					if (collect_stats)
						quienseWakeup[i]++;
				}
			}
		}

		if (((!max_barrier && (pure_num_insn > max_insts)) || (max_barrier && TotalBarriers >= max_barrier) ) && collect_stats)
		{
			for (i = 0; i < numcontexts; i++)
			{
				pureInsn[i] = thecontexts[i]->pure_num_insn;
				simInsn[i] = thecontexts[i]->sim_num_insn;
			}
			printf("[INFO 2]: sim_num_insn=%llu, sim_cycle=%llu\n", pure_num_insn, sim_cycle);
			printf("RR%llu|%llu\n", pure_num_insn, sim_cycle);
			fprintf(stderr,"[INFO 2]: sim_num_insn=%llu, sim_cycle=%llu\n", pure_num_insn, sim_cycle);
			for (i = 0; i < numcontexts; i++)
			{
				struct cache_t *cp = cache_dl1[i];
				int j = 0, set = 0, m = 0;
				struct cache_blk_t *blk;
				for(j=0;j<cp->nsets;j++)
				{
					for (blk = cp->sets[j].way_head; blk; blk = blk->way_next)
					{
						for(m=0;m<8;m++)
							if(blk->WordUseFlag[m])
								blk->WordCount ++;
						stats_do(blk->addr, blk->ReadCount, blk->WriteCount, blk->WordCount, blk->Type, i);
					}
				}
			}
			stats_collect();
			//fprintf(stderr,"[INFO 2]: sim_num_insn=%llu, barriers=%d", pure_num_insn, TotalBarriers);
			/**********************************
			// ctb 2013-03-08
			//**********************************
			RouterLinkDump(Stastic_FILE);
			fclose(Stastic_FILE);
			TotalCPUPower(&cpu_power);
			TotalRouterPower(&router_power);
			TotalRouterEnergy += (router_power - router_power_back);
			TotalCPUEnergy += (cpu_power - cpu_power_back);
			
			PacketDelay( CPF+Count );
			INSN[Count] = pure_num_insn - purenuminsn_backup;
			CPI[Count] = (1.0*(total_slip_cycles-slip_cycles_backup))/(pure_num_insn-purenuminsn_backup);
			IPC[Count] = (1.0*(pure_num_insn-purenuminsn_backup))/(realSimCycle-realsimcycle_backup);
			FPC[Count] = (1.0*(data_packet_size*DataPackets_1+MetaPackets_1))/(realSimCycle-realsimcycle_backup);
			HPF[Count] = (1.0*Hops_1)/(data_packet_size*DataPackets_1+MetaPackets_1);
			CPU_Power[Count] = (TotalCPUEnergy-TotalCPUEnergy_back)/(realSimCycle-realsimcycle_backup);
			Router_Power[Count] = (TotalRouterEnergy-TotalRouterEnergy_back)/(realSimCycle-realsimcycle_backup);
			Total_Power[Count] = CPU_Power[Count] + Router_Power[Count];
			
			L2_Access[Count] = (1.0*( cache_dl2->hits + cache_dl2->misses + cache_dl2->in_mshr - L2_access_back))/(pure_num_insn-purenuminsn_backup);
			Mem_Access[Count] = (1.0*(cache_dl2->misses - Mem_access_back))/(pure_num_insn-purenuminsn_backup);


			CTB_FILE = fopen(my_simout,"a+");
			if(CTB_FILE!=NULL)
			{

				fprintf(CTB_FILE,"\nIPC: %07.3f", (1.0*(pure_num_insn-warm_insn))/(sim_cycle-warm_cycle));
				fprintf(CTB_FILE,"\nC_P: %07.3f", TotalCPUEnergy/(sim_cycle-warm_cycle));
				fprintf(CTB_FILE,"\nR_P: %07.3f", TotalRouterEnergy/(sim_cycle-warm_cycle));

				fprintf(CTB_FILE,"\nIPC: ");
				for(i=0;i<STATUS_NUM;i++)
				{
					fprintf(CTB_FILE,"%07.3f  ",IPC[i]);
				}
				fprintf(CTB_FILE,"\nCPI: ");
				for(i=0;i<STATUS_NUM;i++)
				{
					fprintf(CTB_FILE,"%07.4f  ",CPI[i]);
				}
				fprintf(CTB_FILE,"\nFPC: ");
				for(i=0;i<STATUS_NUM;i++)
				{
					fprintf(CTB_FILE,"%7.5f  ",FPC[i]);
				}
				fprintf(CTB_FILE,"\nCPF: ");
				for(i=0;i<STATUS_NUM;i++)
				{
					fprintf(CTB_FILE,"%7.5f  ",CPF[i]);
				}
				fprintf(CTB_FILE,"\nP_C: ");
				for(i=0;i<STATUS_NUM;i++)
				{
					fprintf(CTB_FILE,"%07.3f  ",CPU_Power[i]);
				}
				fprintf(CTB_FILE,"\nP_R: ");
				for(i=0;i<STATUS_NUM;i++)
				{
					fprintf(CTB_FILE,"%07.3f  ",Router_Power[i]);
				}
				fprintf(CTB_FILE,"\nP_T: ");
				for(i=0;i<STATUS_NUM;i++)
				{
					fprintf(CTB_FILE,"%07.3f  ",Total_Power[i]);
				}
				fprintf(CTB_FILE,"\n");
				fclose(CTB_FILE);

			}
			if(IsTrain)
			{
				Stastic_FILE=fopen("train.txt","w");
				if(Stastic_FILE != NULL)
				{
					//fprintf(Stastic_FILE,"%d: ",STATUS_NUM);
					for(i=0;i<STATUS_NUM;i++)
					{
						fprintf(Stastic_FILE,"%lld: %7.3f %7.3f %7.4f %7.4f %8.6f %8.6f\n",INSN[i],IPC[i],CPI[i],FPC[i],CPF[i],L2_Access[i],Mem_Access[i]);
					}
					
					fclose(Stastic_FILE);
				}
			}

			//**************end*****************/
			return;
		}
		if (unPrintLimit < STATUS_NUM && (pure_num_insn >= (unPrintLimit * (max_insts / STATUS_NUM))))
		{
			if (!mystricmp (network_type, "FSOI") || !mystricmp (network_type, "HYBRID"))
			{
				printf ("[INFO]: sim_cycle= %lld\tsim_num_insn=%lld\tProgress=%d\%\tdata_packets_generate=%d\t", sim_cycle, pure_num_insn, (int)((unPrintLimit * 100) / STATUS_NUM), global_data_packets_generation);
				fprintf (stderr, "[INFO]: sim_cycle= %lld\tsim_num_insn=%lld\tProgress=%d\%\tdata_packets_generate=%d\t", sim_cycle, pure_num_insn, (int)((unPrintLimit * 100) / STATUS_NUM), global_data_packets_generation);
			}
			else if (!mystricmp (network_type, "MESH"))
			{
                printf ("[INFO]: sim_cycle= %lld\t", sim_cycle);
                printf ("sim_num_insn=%lld\t", pure_num_insn);
                printf ("Progress=%d%\t", (unPrintLimit * 100) / STATUS_NUM);
                printf ("data_packets_generate=%d\t",DataPackets_1);
                printf ("meta_packets=%d\t", MetaPackets_1);
				fprintf (stderr, "[INFO]: sim_cycle= %lld\t", sim_cycle);
                fprintf (stderr, "sim_num_insn=%lld\t", pure_num_insn);
                fprintf (stderr, "Progress=%d%\t", (unPrintLimit * 100) / STATUS_NUM);
                fprintf (stderr, "data_packets_generate=%d\t", DataPackets_1);
                fprintf (stderr, "meta_packets=%d\t", MetaPackets_1);
			}
			else if ((!mystricmp (network_type, "COMB")))
			{
				printf ("[INFO]: sim_cycle= %lld\tsim_num_insn=%lld\tProgress=%d%\t", sim_cycle, pure_num_insn, (unPrintLimit * 100) / STATUS_NUM);
				fprintf (stderr, "[INFO]: sim_cycle= %lld\tsim_num_insn=%lld\tProgress=%d%\t", sim_cycle, pure_num_insn, (unPrintLimit * 100) / STATUS_NUM);
			}

          	printf("barriers=%lld\tlocks=%lld\n", TotalBarriers, TotalLocks);
			fprintf(stderr, "barriers=%lld\tlocks=%lld\n", TotalBarriers, TotalLocks);
            fflush (stderr);
			fflush (stdout);
			/*****************************************************************
			//abb by ctb 2013-03-04
			//count performance value of each sample time

			
			//printf("RealTime_insn=%lld\tRealTime_simcycle=%lld\tRealTime_IPC=%f\n", pure_num_insn-purenuminsn_backup, realSimCycle-realsimcycle_backup, (1.0*(pure_num_insn-purenuminsn_backup))/(realSimCycle-realsimcycle_backup));
			//fprintf(stderr, "RealTime_insn=%lld\tRealTime_simcycle=%lld\tRealTime_IPC=%f\n", pure_num_insn-purenuminsn_backup, realSimCycle-realsimcycle_backup, (1.0*(pure_num_insn-purenuminsn_backup))/(realSimCycle-realsimcycle_backup));
			RouterLinkDump(Stastic_FILE);
			RouterLinkReset();
			TotalCPUPower(&cpu_power);
			TotalRouterPower(&router_power);
			CPUPower = (cpu_power-cpu_power_back)/(realSimCycle-realSimCycle_backup);
			RouterPower = (router_power-router_power_back)/(realSimCycle-realSimCycle_backup);
			TotalRouterEnergy += (router_power - router_power_back);
			TotalCPUEnergy += (cpu_power - cpu_power_back);
			CPUActive = CPUPower/(proc_freq*proc_v*proc_v);
			RouterActive = RouterPower/(router_freq*router_v*router_v);
			
			PacketDelay( CPF+Count );
			INSN[Count] = pure_num_insn - purenuminsn_backup;
			CPI[Count] = (1.0*(total_slip_cycles-slip_cycles_backup))/(pure_num_insn-purenuminsn_backup);
			IPC[Count] = (1.0*(pure_num_insn-purenuminsn_backup))/(realSimCycle-realsimcycle_backup);
			HPF[Count] = (1.0*Hops_1)/(data_packet_size*DataPackets_1+MetaPackets_1);
			FPC[Count] = (1.0*(data_packet_size*DataPackets_1+MetaPackets_1))/(realSimCycle-realsimcycle_backup);
			CPU_Power[Count] = (TotalCPUEnergy-TotalCPUEnergy_back)/(realSimCycle-realsimcycle_backup);
			Router_Power[Count] = (TotalRouterEnergy-TotalRouterEnergy_back)/(realSimCycle-realsimcycle_backup);
			Total_Power[Count] = CPU_Power[Count] + Router_Power[Count];
			
			L2_Access[Count] = (1.0*( cache_dl2->hits + cache_dl2->misses + cache_dl2->in_mshr - L2_access_back))/(pure_num_insn-purenuminsn_backup);
			Mem_Access[Count] = (1.0*(cache_dl2->misses- Mem_access_back))/(pure_num_insn-purenuminsn_backup);


			L2_access_back = cache_dl2->hits + cache_dl2->misses + cache_dl2->in_mshr;
			Mem_access_back = cache_dl2->misses;
			
			
			//printf("RealTime_CPUPower=%0.4f\tRealTime_RouterPower=%0.4f\tRealTime_TotalPower=%.4f\n", CPU_Power[Count],Router_Power[Count],Total_Power[Count]);
			//fprintf(stderr, "RealTime_CPUPower=%0.4f\tRealTime_RouterPower=%0.4f\tRealTime_TotalPower=%.4f\n", CPU_Power[Count],Router_Power[Count],Total_Power[Count]);
			
			Count++;

			purenuminsn_backup = pure_num_insn;
			slip_cycles_backup = total_slip_cycles;
			realsimcycle_backup = realSimCycle;
			realSimCycle_backup = realSimCycle;
			cpu_power_back = cpu_power;
			router_power_back = router_power;
			TotalRouterEnergy_back = TotalRouterEnergy;
			TotalCPUEnergy_back = TotalCPUEnergy;
			
			//***********************end***************************************/
			fflush (stderr);
			fflush (stdout);
			unPrintLimit++;
			global_data_packets_generation = 0;
			Total_MetaPackets_1 += MetaPackets_1;
            Total_DataPackets_1 += DataPackets_1;
			Total_Hops_1 += Hops_1;
			Hops_1 = 0;
            DataPackets_1 = 0;
			MetaPackets_1 = 0;
		}
		
	
	}
}


int
get_free_reg (int lregnum, int threadid)
{
	int tempval;
	struct listelt *temp;

	context *current;

	current = thecontexts[threadid];

	if ((lregnum < 1) || (lregnum > 62))
		return 0;
	if (lregnum < 32)
	{
		if (!ifreelist_head[threadid])
		{
			printf ("Num of IREG=%d\n", iregfile);
			fprintf (stderr, "Num of IREG=%d\n", iregfile);
			panic ("Out of registers?!");
		}

		temp = ifreelist_head[threadid];
		tempval = temp->preg;
		ifreelist_head[threadid] = ifreelist_head[threadid]->next;
		if (!ifreelist_head[threadid])
			ifreelist_tail[threadid] = NULL;
		if (ifreelist_head[threadid])
			ifreelist_head[threadid]->prev = NULL;
		free (temp);
		current->ltopmap[lregnum] = tempval;
		current->ptolmap[tempval].lreg = lregnum;
		current->ptolmap[tempval].threadid = threadid;
#ifdef  REMOVE_REG_COUNT
		iregfile++;
#endif
		iregfile_cl[threadid]++;
		return tempval;
	}
	else
	{
		if (!ffreelist_head[threadid])
		{
			printf ("Num of FREG=%d\n", fregfile);
			fprintf (stderr, "Num of FREG=%d\n", fregfile);
			panic ("Out of registers?!");
		}

		temp = ffreelist_head[threadid];
		tempval = temp->preg;
		ffreelist_head[threadid] = ffreelist_head[threadid]->next;
		if (!ffreelist_head[threadid])
			ffreelist_tail[threadid] = NULL;
		if (ffreelist_head[threadid])
			ffreelist_head[threadid]->prev = NULL;
		free (temp);
		current->ltopmap[lregnum] = tempval;
		current->ptolmap[tempval].lreg = lregnum;
		current->ptolmap[tempval].threadid = threadid;
#ifdef  REMOVE_REG_COUNT
		fregfile++;
#endif
		fregfile_cl[threadid]++;
		return tempval;
	}
}


void
freelist_insert (int pregnum, int threadid)
{
	struct listelt *elt;
	struct listelt *p;
	struct listelt *q;

	context *current = thecontexts[threadid];


	if (threadid == 0)
	{
		p = ifreelist_head[threadid];
		while (p)
		{
			q = p;
			p = p->next;
		}
	}


	if (!pregnum)
		return;
	if (pregnum <= IPREG_size)
	{
#ifdef  REMOVE_REG_COUNT
		iregfile--;
#endif
		elt = (struct listelt *) malloc (sizeof (struct listelt));
		elt->preg = pregnum;
		if (ifreelist_tail[threadid])
		{
			p = ifreelist_head[threadid];
			q = NULL;
			while (p && (p->preg < pregnum))
			{
				q = p;
				p = p->next;
			}
			if (!p || !SORT)
			{
				ifreelist_tail[threadid]->next = elt;
				elt->prev = ifreelist_tail[threadid];
				elt->next = NULL;
				ifreelist_tail[threadid] = elt;
			}
			else if (q)
			{
				q->next = elt;
				elt->prev = q;
				elt->next = p;
				p->prev = elt;
			}
			else
			{
				elt->next = ifreelist_head[threadid];
				ifreelist_head[threadid]->prev = elt;
				elt->prev = NULL;
				ifreelist_head[threadid] = elt;
			}
		}
		else
		{
			elt->next = NULL;
			elt->prev = NULL;
			ifreelist_tail[threadid] = elt;
			ifreelist_head[threadid] = elt;
		}
		current->ptolmap[pregnum].lreg = 0;
		current->ptolmap[pregnum].threadid = -1;
		iregfile_cl[threadid]--;
	}
	else
	{
#ifdef  REMOVE_REG_COUNT
		fregfile--;
#endif
		elt = (struct listelt *) malloc (sizeof (struct listelt));
		elt->preg = pregnum;
		if (ffreelist_tail[threadid])
		{
			p = ffreelist_head[threadid];
			q = NULL;
			while (p && (p->preg < pregnum))
			{
				q = p;
				p = p->next;
			}
			if (!p || !SORT)
			{
				ffreelist_tail[threadid]->next = elt;
				elt->prev = ffreelist_tail[threadid];
				elt->next = NULL;
				ffreelist_tail[threadid] = elt;
			}
			else if (q)
			{
				q->next = elt;
				elt->prev = q;
				elt->next = p;
				p->prev = elt;
			}
			else
			{
				elt->next = ffreelist_head[threadid];
				ffreelist_head[threadid]->prev = elt;
				elt->prev = NULL;
				ffreelist_head[threadid] = elt;
			}
		}
		else
		{
			elt->next = NULL;
			elt->prev = NULL;
			ffreelist_tail[threadid] = elt;
			ffreelist_head[threadid] = elt;
		}
		fregfile_cl[threadid]--;
		current->ptolmap[pregnum].lreg = 0;
		current->ptolmap[pregnum].threadid = -1;
	}
}

void
recFreeRegList (int threadid)
{
	struct listelt *p;
	struct listelt *temp;
	int count = 0;
	int i;

	p = ifreelist_head[threadid];
	if (p)
	{
		count++;
		while ((p != ifreelist_tail[threadid]) && p)
		{
			count++;
			temp = p;
			p = p->next;
			free (temp);
		}
	}

	p = ffreelist_head[threadid];
	if (p)
	{
		count++;
		while ((p != ffreelist_tail[threadid]) && p)
		{
			count++;
			temp = p;
			p = p->next;
			free (temp);
		}
	}

	reg_init (threadid);
}

int
numFreeReg (int threadid)
{
	struct listelt *p;
	int count = 0;

	p = ifreelist_head[threadid];
	if (p)
	{
		count++;
		while ((p != ifreelist_tail[threadid]) && p)
		{
			count++;
			p = p->next;
		}
	}

	p = ffreelist_head[threadid];
	if (p)
	{
		count++;
		while ((p != ffreelist_tail[threadid]) && p)
		{
			count++;
			p = p->next;
		}
	}
	return count;
}

/* check point (saving) Map tables when experience redirect fetch with wrong path execution */
void
copy_to (int threadid)
{
	int i;

	context *current;

	current = thecontexts[threadid];

	for (i = 0; i < MD_TOTAL_REGS; i++)
	{
		current->ltopmap_chk[i] = current->ltopmap[i];
	}
	for (i = 0; i < p_total_regs; i++)
	{
		if (current->ptolmap[i].threadid == threadid)
		{
			current->ptolmap_chk[i].lreg = current->ptolmap[i].lreg;
			current->ptolmap_chk[i].threadid = current->ptolmap[i].threadid;
		}
	}
}

/* retrive the check pointed Map tables */
void
copy_from (int threadid)
{
	int i;

	context *current;

	current = thecontexts[threadid];

	for (i = 0; i < MD_TOTAL_REGS; i++)
	{
		current->ltopmap[i] = current->ltopmap_chk[i];
	}
}


void
bpred_init (int i)
{

	if (ruu_branch_penalty < 1)
		fatal ("mis-prediction penalty must be at least 1 cycle");

	if (fetch_speed != 1)
		fatal ("front-end speed must be 1 for SMT version of simulator");

	if (!mystricmp (pred_type, "bimod"))
	{
		if (bimod_nelt != 1)
			fatal ("bad bimod predictor config (<table_size>)");
		if (btb_nelt != 2)
			fatal ("bad btb config (<num_sets> <associativity>)");
		thecontexts[i]->pred = bpred_create (BPred2bit,
				/* bimod table size */ bimod_config[0],
				/* 2lev l1 size */ 0,
				/* 2lev l2 size */ 0,
				/* meta table size */ 0,
				/* history reg size */ 0,
				/* history xor address */ 0,
				/* btb sets */ btb_config[0],
				/* btb assoc */ btb_config[1],
				/* ret-addr stack size */ ras_size);
	}
	else if (!mystricmp (pred_type, "2lev"))
	{
		/* 2-level adaptive predictor, bpred_create() checks args */
		if (twolev_nelt != 4)
			fatal ("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if (btb_nelt != 2)
			fatal ("bad btb config (<num_sets> <associativity>)");

		thecontexts[i]->pred = bpred_create (BPred2Level,
				/* bimod table size */ 0,
				/* 2lev l1 size */ twolev_config[0],
				/* 2lev l2 size */ twolev_config[1],
				/* meta table size */ 0,
				/* history reg size */ twolev_config[2],
				/* history xor address */ twolev_config[3],
				/* btb sets */ btb_config[0],
				/* btb assoc */ btb_config[1],
				/* ret-addr stack size */ ras_size);
	}
	else if (!mystricmp (pred_type, "comb"))
	{
		/* combining predictor, bpred_create() checks args */
		if (twolev_nelt != 4)
			fatal ("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if (bimod_nelt != 1)
			fatal ("bad bimod predictor config (<table_size>)");
		if (comb_nelt != 1)
			fatal ("bad combining predictor config (<meta_table_size>)");
		if (btb_nelt != 2)
			fatal ("bad btb config (<num_sets> <associativity>)");

		thecontexts[i]->pred = bpred_create (BPredComb,
				/* bimod table size */ bimod_config[0],
				/* l1 size */ twolev_config[0],
				/* l2 size */ twolev_config[1],
				/* meta table size */ comb_config[0],
				/* history reg size */ twolev_config[2],
				/* history xor address */ twolev_config[3],
				/* btb sets */ btb_config[0],
				/* btb assoc */ btb_config[1],
				/* ret-addr stack size */ ras_size);
	}

	if (!bpred_spec_opt)
		bpred_spec_update = spec_CT;
	else if (!mystricmp (bpred_spec_opt, "ID"))
		bpred_spec_update = spec_ID;
	else if (!mystricmp (bpred_spec_opt, "WB"))
		bpred_spec_update = spec_WB;
	else
		fatal ("bad speculative update stage specifier, use {ID|WB}");

	bpred_reg_stats (thecontexts[i]->pred, sim_sdb, i);
}


void
reg_init (int threadid)
{
	int i;

	context *current;

	current = thecontexts[threadid];

	ifreelist_head[threadid] = NULL;
	ifreelist_tail[threadid] = NULL;
	ffreelist_head[threadid] = NULL;
	ffreelist_tail[threadid] = NULL;

#ifdef  REMOVE_REG_COUNT
	/* assume all physical registers are used from the begining then
	   insert them one by one into the free list */
	iregfile = IPREG_size;
	fregfile = FPREG_size;
#endif
	for (i = 0; i < p_total_regs; i++)
	{
		freelist_insert (i, threadid);
		current->ptolmap[i].lreg = 0;
		current->ptolmap[i].threadid = -1;
	}
	iregfile_cl[threadid] = 0;
	fregfile_cl[threadid] = 0;
#ifndef   NO_REG_INIT

	current = thecontexts[threadid];
	for (i = 0; i < 63; i++)
	{
		current->ltopmap[i] = get_free_reg (i, threadid);
	}
	for (i = 63; i < MD_TOTAL_REGS; i++)
		current->ltopmap[i] = 0;
#endif


}

void
reg_count ()
{

	iregfile = 0;
	fregfile = 0;
	int i, j;
	context *current;

	for (j = 0; j < numcontexts; j++)
	{
		current = thecontexts[j];
		for (i = 1; i < p_total_regs; i++)
		{
			if ((current->ptolmap[i].lreg > 0) && (current->ptolmap[i].lreg < 32) && current->ptolmap[i].threadid != -1)
			{
				iregfile++;
			}
			if ((current->ptolmap[i].lreg > 31) && (current->ptolmap[i].lreg < 63) && current->ptolmap[i].threadid != -1)
			{
				fregfile++;
			}
		}
	}
	if (iregfile < 31)
		panic ("phys iregfile less than 31");
	if (fregfile < 31)
		panic ("phys fregfile less than 31");
}


void
initParallelSimulation ()
{
	int cnt, cx, j;
	long long int x, y, z, max;

	for (cnt = 0; cnt < numcontexts; cnt++)
		thecontexts[cnt]->start_cycle = sim_cycle;

	for (cx = 0; cx < numcontexts; cx++)
	{
		thecontexts[cx]->fetch_num = 0;
		thecontexts[cx]->fetch_tail = thecontexts[cx]->fetch_head = 0;
		thecontexts[cx]->fetch_pred_PC = thecontexts[cx]->fetch_regs_PC = thecontexts[cx]->regs.regs_NPC;
		collectStatStop[cx] = 0;
		thecontexts[cx]->sim_num_insn = 0;
	}
	collectStatBarrier = 0;
	allForked = 1;
}

/*RN: 10.11.06*/
/////////////////////////////Ruke Huang///////////////////////
/*
   int	GetMemAccWidth(enum md_opcode op)
   {
   switch(op)
   {
   case LDBU:
   case STB:
   return 1;
   case LDWU:
   case STW:
   return 2;
   case LDL:
   case LDL_L:
   case STL:
   case STL_C:
   case LDS:
   case LDF:
   case STS:
   case STF:
   return 4;
   case LDQ:
   case LDQ_U:
   case LDQ_L:
   case STQ:
   case STQ_U:
   case STQ_C:
   case LDT:
   case LDG:
   case STT:
   case STG:
   return 8;
   default:
   printf("Fatal: GetMemAccWidth cannot deal with op : %d!\n",
   op);
   fatal("GetMemAccWidth cannot deal with op : %d!\n",
   op);
   }
   return 0;
   }
 */

int
checkLSQforSt (int i, context * current)
{

	struct RUU_station *rs = &current->LSQ[i];
	int nW1 = GetMemAccWidth (rs->op);

	if (i != current->LSQ_head)
	{
		for (;;)
		{
			i = (i + (LSQ_size - 1)) % LSQ_size;

			if ((MD_OP_FLAGS (current->LSQ[i].op) & F_STORE) && (STORE_ADDR_READY (&current->LSQ[i])) && (current->LSQ[i].threadid == rs->threadid))
			{
				int nW2 = GetMemAccWidth (current->LSQ[i].op);
				int nW = (nW1 >= nW2) ? nW1 : nW2;
				md_addr_t qwAddr1 = rs->addr & ~(nW - 1);
				md_addr_t qwAddr2 = current->LSQ[i].addr & ~(nW - 1);

				if (qwAddr1 == qwAddr2)
				{
					if (current->LSQ[i].issued)	//we r fine
						return 1;
					else
						return 0;	//we need to reject
				}
			}
			if (i == current->LSQ_head)
				break;
		}

	}
	return 1;			//wr r fine
}


void
seqConsistancyReplay (int replay_index, int threadid)
{
	context *current = thecontexts[threadid];
	int i, RUU_index, LSQ_index, temp_RUU_num, j, storeCount = m_L1WBufCnt[threadid];
	struct wb_mem_ent *link;

	struct RUU_station *temp_LSQ_rs;
	int temp_LSQ_index, temp_LSQ_num;
	int shift_right = 0;	/*how many bits history will need to shift right */


	seqConsReplay[current->actualid]++;

	RUU_index = current->RUU_head;
	temp_RUU_num = current->RUU_num;
	LSQ_index = current->LSQ_head;
	temp_LSQ_num = current->LSQ_num;

	if (!current->RUU_num)
		panic ("empty RUU");

	while (RUU_index != replay_index)
	{
		/* the RUU should not drain since the mispredicted branch will remain */
		if (!temp_RUU_num)
			panic ("empty RUU");

		/* load/stores must retire load/store queue entry as well */
		if (current->RUU[RUU_index].ea_comp)
		{
			if (MD_OP_FLAGS (current->LSQ[LSQ_index].op) & F_STORE)
			{
				int width = GetMemAccWidth(current->LSQ[LSQ_index].op);

				if ((width & (width - 1)) == 0 && (current->LSQ[LSQ_index].addr & (width - 1)) == 0 && !current->LSQ[LSQ_index].isPrefetch)
					storeCount++;
			}
			LSQ_index = (LSQ_index + 1) % LSQ_size;
			temp_LSQ_num--;
		}
		RUU_index = (RUU_index + 1) % RUU_size;
		temp_RUU_num--;
	}

	while (RUU_index != current->RUU_tail)
	{
		/* should meet up with the tail first */
		if (current->RUU[RUU_index].counted_iissueq)
		{
			current->iissueq_thrd--;
			iissueq--;
			iissueq_cl[current->id]--;
			current->RUU[RUU_index].counted_iissueq = 0;
		}
		if (current->RUU[RUU_index].counted_fissueq)
		{
			current->fissueq_thrd--;
			fissueq--;
			fissueq_cl[current->id]--;
			current->RUU[RUU_index].counted_fissueq = 0;
		}

		current->sim_num_insn--;
		sim_num_insn--;
		seqConsInsnReplay[current->actualid]++;

		if (current->RUU[RUU_index].ea_comp)
		{
			if (!temp_LSQ_num)
				panic ("RUU and LSQ out of sync");

			if (current->LSQ[LSQ_index].counted_iissueq)
			{
				current->iissueq_thrd--;
				iissueq--;
				iissueq_cl[current->id]--;
				current->LSQ[LSQ_index].counted_iissueq = 0;
			}
			if (current->LSQ[LSQ_index].counted_fissueq)
			{
				current->fissueq_thrd--;
				fissueq--;
				fissueq_cl[current->id]--;
				current->LSQ[LSQ_index].counted_fissueq = 0;
			}

			for (i = 0; i < MAX_ODEPS; i++)
			{
				RSLINK_FREE_LIST (current->LSQ[LSQ_index].odep_list[i]);
				current->LSQ[LSQ_index].odep_list[i] = NULL;
			}

			if (current->LSQ[LSQ_index].op == STL_C || current->LSQ[LSQ_index].op == STQ_C)
			{
				current->waitForSTLC = 0;
			}
			int matchnum = MSHR_block_check(cache_dl1[threadid]->mshr, current->LSQ[LSQ_index].addr, cache_dl1[threadid]->set_shift);
			if(matchnum)
			{
				matchnum = matchnum-1;
				struct RUU_station *rs_rob = &current->RUU[RUU_index];
				struct RUU_station *rs_lsq = &current->LSQ[LSQ_index];

				if(cache_dl1[threadid]->mshr->mshrEntry[matchnum].rs == rs_rob || cache_dl1[threadid]->mshr->mshrEntry[matchnum].rs == rs_lsq)
					cache_dl1[threadid]->mshr->mshrEntry[matchnum].event->rs = NULL;

				struct RS_list *cur = rs_cache_list[threadid][matchnum];

				while(cur)
				{
					if(cur->rs == rs_rob || cur->rs == rs_lsq)
						cur->rs = NULL;
					cur = cur->next;
				}
			}
			freelist_insert (current->LSQ[LSQ_index].opreg, threadid);
			changeInEventQueue(&current->LSQ[LSQ_index], threadid);
			current->LSQ[LSQ_index].tag++;
			ptrace_endinst (current->LSQ[LSQ_index].ptrace_seq);
			if ((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_LOAD)) == (F_MEM | F_LOAD))
			{
				if (!current->LSQ[LSQ_index].isPrefetch)
					current->m_shLQNum--;
			}
			else if ((MD_OP_FLAGS (current->LSQ[LSQ_index].op) & (F_MEM | F_STORE)) == (F_MEM | F_STORE))
			{
				if (!current->LSQ[LSQ_index].isPrefetch)
					current->m_shSQNum--;
			}

			current->LSQ_tail = (current->LSQ_tail + LSQ_size - 1) % LSQ_size;
			LSQ_index = (LSQ_index + 1) % LSQ_size;
			current->LSQ_num--;
			temp_LSQ_num--;
		}


		/* recover any resources used by this RUU operation */
		for (i = 0; i < MAX_ODEPS; i++)
		{
			RSLINK_FREE_LIST (current->RUU[RUU_index].odep_list[i]);
			current->RUU[RUU_index].odep_list[i] = NULL;
		}
		freelist_insert (current->RUU[RUU_index].opreg, threadid);
		changeInEventQueue(&current->RUU[RUU_index], threadid);
		current->RUU[RUU_index].tag++;
		ptrace_endinst (current->RUU[RUU_index].ptrace_seq);
		RUU_index = (RUU_index + 1) % RUU_size;
		current->RUU_num--;
	}
	current->RUU_tail = replay_index;


	link = current->WBtableTail;

	if (link == NULL || storeCount == 0)
	{
		current->WBtableHead = current->WBtableTail = NULL;
		if (link != NULL)
		{
			while (link != NULL)
			{
				current->numOfWBEntry--;
				link->next = current->WBbucket_free_list;
				current->WBbucket_free_list = link;
				link = link->prev;
				current->WBbucket_free_list->prev = NULL;
			}
		}
	}
	else
	{
		current->WBtableHead = current->WBtableTail;
		storeCount--;
		while (storeCount)
		{
			current->WBtableHead = current->WBtableHead->prev;
			storeCount--;
		}
		link = current->WBtableHead->prev;
		current->WBtableHead->prev = NULL;

		while (link != NULL)
		{
			current->numOfWBEntry--;
			link->next = current->WBbucket_free_list;
			current->WBbucket_free_list = link;
			link = link->prev;
			current->WBbucket_free_list->prev = NULL;
		}
	}

	for (i = 0; i < MD_TOTAL_REGS; i++)
	{
		current->create_vector[i] = CVLINK_NULL;
		current->create_vector_rt[i] = sim_cycle;
	}

	if(current->ruu_fetch_issue_delay < WAIT_TIME/2)
		current->ruu_fetch_issue_delay = ruu_branch_penalty;


	enum md_opcode op;

	j = current->fetch_num;
	for (i = current->fetch_head; j > 0;)
	{
		MD_SET_OPCODE (op, current->fetch_data[i]->IR);
		i = (i + 1) & (ruu_ifq_size - 1);
		j--;
	}
	current->fetch_num = 0;
	current->fetch_tail = current->fetch_head = 0;
	current->fetch_pred_PC = current->fetch_regs_PC = current->regs.regs_NPC = current->regs.regs_PC;	//a special case..............
	//needReplay[threadid] = 0;  //do I need this??????


}


void
fixSpecState (int replay_index, int threadid)
{
	//context *current= current;
	context *current = thecontexts[threadid];
	struct RUU_station *rs;
	int i, RUU_index;

	RUU_index = current->RUU_head;

	while (RUU_index != replay_index)
	{
		RUU_index = (RUU_index + 1) % RUU_size;
	}

	while (RUU_index != current->RUU_tail)
	{
		if (current->RUU[RUU_index].recover_inst && !current->RUU[RUU_index].completed)
		{
			rs = &current->RUU[RUU_index];
			ruu_recover (rs - current->RUU, rs->threadid, 0);
			tracer_recover (rs->threadid);
			bpred_recover (current->pred, rs->PC, rs->stack_recover_idx, rs->threadid);
			return;
		}
		RUU_index = (RUU_index + 1) % RUU_size;
	}
}


/* TSO (Total store order) is being followed */
void
ruu_release_writebuffer ()
{
	int threadid, i, cnt, j;
	context * current;

	for (threadid = 0; threadid < numcontexts; threadid++)
	{
		//add by ctb
		current = thecontexts[threadid];
		if(!current->active_this_cycle)
			continue ;
		//****************************
		j = m_L1WBufCnt[threadid];
		cnt = 0;
		i = m_L1WBuf_head[threadid];

		if ((m_L1WBufCnt[threadid] == 0 || m_L1WBufCnt[threadid] == SMD_L1_WBUF_SIZE) && (m_L1WBuf_tail[threadid] != m_L1WBuf_head[threadid]))
			panic ("Head and tail pointer messed up\n");

		while (cnt != j)
		{
			if (sim_cycle >= m_L1WBuf[threadid][i].finishTime && m_L1WBuf[threadid][i].writeStarted)
			{
				if(m_L1WBuf[threadid][i].isMiss && MSHR_block_check(cache_dl1[threadid]->mshr, m_L1WBuf[threadid][i].addr, cache_dl1[threadid]->set_shift))
					break;
				if(!m_L1WBuf[threadid][i].commitDone)
				//	completeStore(threadid, m_L1WBuf[threadid][i].addr, GetMemAccWidth(m_L1WBuf[threadid][i].op), 0);
				{
					completeStore(threadid, m_L1WBuf[threadid][i].addr, GetMemAccWidth(m_L1WBuf[threadid][i].op), m_L1WBuf[threadid][i].STL_C_fail);
					m_L1WBuf[threadid][i].STL_C_fail = 0;
				}

				m_L1WBufCnt[threadid]--;
				m_L1WBuf_head[threadid] = (m_L1WBuf_head[threadid] + 1) % SMD_L1_WBUF_SIZE;
				i = (i + 1) % SMD_L1_WBUF_SIZE;
				cnt++;
			}
			else
				break;
		}

		if ((m_L1WBufCnt[threadid] == 0 || m_L1WBufCnt[threadid] == SMD_L1_WBUF_SIZE) && (m_L1WBuf_tail[threadid] != m_L1WBuf_head[threadid]))
			panic ("Head and tail pointer messed up\n");
	}
}

void
ruu_write_from_Writebuffer ()
{
	int threadid, i, cnt;
	struct res_template *fu;
	int lat = 0;
	context *current;

	for (threadid = 0; threadid < numcontexts; threadid++)
	{
		current = thecontexts[threadid];

		//add by ctb
		if(!current->active_this_cycle)
			continue ;
		//****************************
		if ((m_L1WBufCnt[threadid] == 0 || m_L1WBufCnt[threadid] == SMD_L1_WBUF_SIZE) && (m_L1WBuf_tail[threadid] != m_L1WBuf_head[threadid]))
			panic ("Head and tail pointer messed up\n");

		for(cnt = 0, i = m_L1WBuf_head[threadid]; cnt < m_L1WBufCnt[threadid]; cnt++, i = (i + 1) % SMD_L1_WBUF_SIZE)
		{
			lat = 0;
				if (!m_L1WBuf[threadid][i].writeStarted)
				{
#ifdef	DCACHE_MSHR
					int matchnum;
					matchnum = MSHR_block_check(cache_dl1[threadid]->mshr, m_L1WBuf[threadid][i].addr, cache_dl1[threadid]->set_shift);
					if(matchnum)
					{
						if(m_L1WBuf[threadid][i].already_check)
						{
							cache_dl1[threadid]->in_mshr ++;
							m_L1WBuf[threadid][i].already_check = 1;
						}
						continue;
					}
					else
					m_L1WBuf[threadid][i].already_check = 0;

#endif
					fu = res_get (fu_pool, MD_OP_FUCLASS (m_L1WBuf[threadid][i].op), threadid);

					if (fu
#ifdef	DCACHE_MSHR
						&& !isMSHRFull(cache_dl1[threadid]->mshr, 0, threadid)
#endif
				   )
					{
						if (fu->master->busy)
							panic ("functional unit already in use");
						/* scheddule functional unit release event */
						fu->master->busy = fu->issuelat;
						fu->master->usr_id = threadid;	//mwr: keep threadid info too
						fu->master->duplicate = 0;	/*mwr: duplicate = 0 as this a ld/st operation */

						if (cache_dl1[threadid])
						{
							dcache_access++;	//mwr: already there
							current->dcache_access++;

							invalidateOtherCache = 0;

							if (COHERENT_CACHE && allForked)
							{
								if(m_L1WBuf[threadid][i].op == STL_C && m_L1WBuf[threadid][i].commitDone)
								{/* Complete Store must fail for a missed STL_C instruction */
									lat = cache_dl1[threadid]->hit_latency;
								}
								else
								{
									if(md_valid_addr (m_L1WBuf[threadid][i].addr, current->masterid))
                                    {
//                                        share_pattern(threadid, m_L1WBuf[threadid][i].addr, Write);
										lat = cache_access(cache_dl1[threadid], Write, (m_L1WBuf[threadid][i].addr&~3), NULL, 4, sim_cycle, NULL, NULL,/* RUU pointer */ NULL, threadid, &m_L1WBuf[threadid][i]);
                                    }
								}
							}
#ifdef	DCACHE_MSHR
							if(lat > cache_dl1[threadid]->hit_latency)
							{
								MSHRLookup(cache_dl1[threadid]->mshr, m_L1WBuf[threadid][i].addr, lat, 0, NULL);
								m_L1WBuf[threadid][i].isMiss = 1;
							}
							m_L1WBuf[threadid][i].finishTime = sim_cycle ;
#endif
						}
						m_L1WBuf[threadid][i].writeStarted = 1;
					}
					else
						break;
				}
		}
	}
}




#ifdef	COLLECT_STAT_STARTUP

void 
fstfwd_collectStats()
{
	int barrierCount = 0;
	int threadid;
	/* wait till collect_stats becomes one */
	printf("Fast-forwarding till the collect stats system call\n");
	for (threadid = 0; threadid < numcontexts; threadid++)
	{

		if (thecontexts[threadid]->jobThdId != 0)
		{
			thecontexts[threadid]->regs.regs_PC = thecontexts[threadid]->regs.regs_NPC;
			thecontexts[threadid]->regs.regs_NPC += sizeof (md_inst_t);
		}

		if (thecontexts[threadid]->barrierReached == 1)
			panic ("Fast forward error\n");

		if (thecontexts[threadid]->startReached == 1)
			panic ("Fast forward error\n");
	}

	while(1)
	{
		/* Fast forward all threads until barrier is reached */
		for(threadid = 0; threadid < numcontexts; threadid++)
		{
			context *current = thecontexts[threadid];

			/* check for DLite debugger entry condition */
			if (dlite_check_break (current->regs.regs_PC, /* no access */ 0, /* addr */ 0, 0, 0))
				dlite_main (current->regs.regs_PC, current->regs.regs_PC + sizeof (md_inst_t), sim_cycle, &(current->regs), current->mem);

			md_inst_t inst;	/* actual instruction bits */
			enum md_opcode op;	/* decoded opcode enum */
			md_addr_t target_PC;	/* actual next/target PC address */
			md_addr_t addr;	/* effective address, if load/store */
			int is_write;	/* store? */
			byte_t temp_byte;	/* temp variable for spec mem access */
			half_t temp_half;	/* " ditto " */
			word_t temp_word;	/* " ditto " */
			if(current->running == 0 && barrier_waiting[threadid])
				continue;

#if defined(HOST_HAS_QWORD) && defined(TARGET_ALPHA)
			qword_t temp_qword = 0;	/* " ditto " */
#endif /* HOST_HAS_QWORD && TARGET_ALPHA */

			enum md_fault_type fault;

			/* till the barrier is reached for this thread */
#ifndef PARALLEL_EMUL
			while (1)
#endif
			{
#ifdef PARALLEL_EMUL
				sim_num_insn++;
#endif
				/* maintain $r0 semantics */
				current->regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
				current->regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

				/* get the next instruction to execute */
				MD_FETCH_INST (inst, current->mem, current->regs.regs_PC);
				cache_warmup (cache_il1[threadid], Read, IACOMPRESS(current->fetch_regs_PC), ISCOMPRESS(sizeof(md_inst_t)), threadid);
				if(current->running == 0 && barrier_waiting[threadid])
					break;
				/* set default reference address */
				addr = 0;
				is_write = FALSE;

				/* set default fault - none */
				fault = md_fault_none;

				/* decode the instruction */
				MD_SET_OPCODE (op, inst);

				/* execute the instruction */
				switch (op)
				{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
					case OP:							\
													SYMCAT(OP,_IMPL);						\
					break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
					case OP:							\
													panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#undef DECLARE_FAULT
#define DECLARE_FAULT(FAULT)						\
					{ fault = (FAULT); break; }
#include "machine.def"
					default:
						panic ("attempted to execute a bogus opcode");
				}

				if (fault != md_fault_none)
					fatal ("fault (%d) detected @ 0x%08p", fault, current->regs.regs_PC);

				/* update memory access stats */
				if (MD_OP_FLAGS (op) & F_MEM)
				{
					if (MD_OP_FLAGS (op) & F_STORE)
					{
						is_write = TRUE;
#ifndef PARALLEL_EMUL
						cache_warmup(cache_dl1[threadid], Write, addr & ~7, 8, threadid);
#endif
					}
#ifndef PARALLEL_EMUL
					else
						cache_warmup(cache_dl1[threadid], Read, addr & ~7, 8, threadid);
#endif
				}

				/* check for DLite debugger entry condition */
				if (dlite_check_break (current->regs.regs_NPC, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_num_insn))
					dlite_main (current->regs.regs_PC, current->regs.regs_NPC, sim_num_insn, &(current->regs), current->mem);

				/* go to the next instruction */
				current->regs.regs_PC = current->regs.regs_NPC;
				current->regs.regs_NPC += sizeof (md_inst_t);

#ifndef PARALLEL_EMUL
				if ((current->barrierReached == 1 || current->barrierReached == 2))
				{
					if(current->barrierReached == 2)
					{
						barrierCount++;
						printf("Crossed the barrier number: %d with threadid: %d\n", barrierCount, threadid);
					}
					current->barrierReached = 0;
					break;
				}

				if(collect_stats)
				{
					printf("Finished fast-forwarding at barrier number: %d with threadid: %d\n", barrierCount, threadid);
					return;
				}
#endif
			}
		}
	}
}


void
fstfwd_fineAdjust ()
{
	int threadid;
	int count = 0;
	int totalItr = 2;
	int prId = 0;
	md_addr_t tempPC[3] = { 0, 0, 0 };

	for (threadid = 0; threadid < numcontexts; threadid++)
	{

		if (thecontexts[threadid]->jobThdId != 0)
		{
			thecontexts[threadid]->regs.regs_PC = thecontexts[threadid]->regs.regs_NPC;
			thecontexts[threadid]->regs.regs_NPC += sizeof (md_inst_t);
		}

		if (thecontexts[threadid]->barrierReached == 1)
			panic ("Fast forward error\n");

		if (thecontexts[threadid]->startReached == 1)
			panic ("Fast forward error\n");
	}
	if(ilink_run)
	{
		collect_stats = 1;
		return 0;
	}
	//collect_stats = 1;
	//return 0;
	{
		count = 0;

		/* set up program entry state */
		for (threadid = 0;threadid < numcontexts; threadid = (threadid + numcontexts + 1) % numcontexts)
		{
			context *current = thecontexts[threadid];

			if (current->masterid != prId)
				continue;

			/* check for DLite debugger entry condition */
			if (dlite_check_break (current->regs.regs_PC, /* no access */ 0, /* addr */ 0, 0, 0))
				dlite_main (current->regs.regs_PC, current->regs.regs_PC + sizeof (md_inst_t), sim_cycle, &(current->regs), current->mem);

			counter_t icount = 0;
			md_inst_t inst;	/* actual instruction bits */
			enum md_opcode op;	/* decoded opcode enum */
			md_addr_t target_PC;	/* actual next/target PC address */
			md_addr_t addr;	/* effective address, if load/store */
			int is_write;	/* store? */
			byte_t temp_byte;	/* temp variable for spec mem access */
			half_t temp_half;	/* " ditto " */
			word_t temp_word;	/* " ditto " */

#if defined(HOST_HAS_QWORD) && defined(TARGET_ALPHA)
			qword_t temp_qword = 0;	/* " ditto " */
#endif /* HOST_HAS_QWORD && TARGET_ALPHA */

			enum md_fault_type fault;

			while (1)
			{
				icount++;
				/* maintain $r0 semantics */
				current->regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
				current->regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

				/* get the next instruction to execute */
				MD_FETCH_INST (inst, current->mem, current->regs.regs_PC);
				cache_warmup (cache_il1[threadid], Read, IACOMPRESS(current->fetch_regs_PC), ISCOMPRESS(sizeof(md_inst_t)), threadid);

				/* set default reference address */
				addr = 0;
				is_write = FALSE;

				/* set default fault - none */
				fault = md_fault_none;

				/* decode the instruction */
				MD_SET_OPCODE (op, inst);

				/* execute the instruction */
				switch (op)
				{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
					case OP:							\
													SYMCAT(OP,_IMPL);						\
					break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
					case OP:							\
													panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#undef DECLARE_FAULT
#define DECLARE_FAULT(FAULT)						\
					{ fault = (FAULT); break; }
#include "machine.def"
					default:
						panic ("attempted to execute a bogus opcode");
				}

				if (fault != md_fault_none)
					fatal ("fault (%d) detected @ 0x%08p", fault, current->regs.regs_PC);

				/* update memory access stats */
				if (MD_OP_FLAGS (op) & F_MEM)
				{
					if (MD_OP_FLAGS (op) & F_STORE)
					{
						is_write = TRUE;
						cache_warmup(cache_dl1[threadid], Write, addr & ~7, 8, threadid);
					}
					else
						cache_warmup(cache_dl1[threadid], Read, addr & ~7, 8, threadid);
				}

				/* check for DLite debugger entry condition */
				if (dlite_check_break (current->regs.regs_NPC, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_num_insn))
					dlite_main (current->regs.regs_PC, current->regs.regs_NPC, sim_num_insn, &(current->regs), current->mem);

				/* go to the next instruction */
				current->regs.regs_PC = current->regs.regs_NPC;
				current->regs.regs_NPC += sizeof (md_inst_t);

				if (count == 0 && (current->barrierReached == 1 || current->barrierReached == 2))
					break;

				if (count == 0 && current->startReached)
				{
					current->startReached = 0;

					if(current->jobThdId != 0)
						panic ("Can not be reached by other threads");

					count++;
					continue;
				}

				if(count == 5)
					break;

				if(count > 0 && count < 6)
					count++;

				if (count == 6 && current->regs.regs_PC == tempPC[current->masterid])
					break;
			}

			if (count == 0 && (current->barrierReached == 1 || current->barrierReached == 2))
			{
				if(current->barrierReached == 2 && current->jobThdId != (THREADS_PER_JOB-1))
					panic("End of barrier reached with wrong id");

				current->barrierReached = 0;
				continue;
			}

			if (count == 5)
			{
				count++;
				tempPC[current->masterid] = current->regs.regs_PC;
				continue;
			}

			if (count == 6 && current->regs.regs_PC == tempPC[current->masterid])
			{
				if (current->jobThdId == (THREADS_PER_JOB-1))
					break;
				else
					continue;
			}
		}

		printf("Job %d synchronized\n", prId);
		fflush(stdout);
	}
}

#endif

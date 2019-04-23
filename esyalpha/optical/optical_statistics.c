
#include "point-point.h"
/* *************************** OPTICAL_STATISTICS.C ****************************/
void optical_reg_stats (struct opt_odb_t *sdb)
{
    int i = 0;
    stat_reg_counter (sdb, "meta_conflict_pro", "total meta conflict event", &meta_conflict_pro, 0, NULL);
    stat_reg_counter (sdb, "data_conflict_pro", "total data conflict event", &data_conflict_pro, 0, NULL);
    stat_reg_counter (sdb, "large_retry", "retry using even probability", &large_retry, 0, NULL);
    stat_reg_counter (sdb, "small_retry", "retry using uneven probability", &small_retry, 0, NULL);

    stat_reg_counter (sdb, "extra_input_buffer_overflow", "total number of extra overflow in inputbuffer", &input_extra_overflow, 0, NULL);

    stat_reg_counter (sdb, "total_req_msg", "total request messages in network", &meta_packets, 0, NULL);
    stat_reg_counter (sdb, "total_data_msg", "total data messages in network", &data_packets, 0, NULL);
    stat_reg_counter (sdb, "total_req_msg_sending", "total request messages sending in network", &meta_packet_transfer, 0, NULL);
    stat_reg_counter (sdb, "total_data_msg_sending", "total data messages sending in network", &data_packet_transfer, 0, NULL);

    stat_reg_counter (sdb, "total_conf_ack_msg", "total acknowledgement in confirmation bits messages in network", &conf_ack_packets, 0, NULL);
    stat_reg_counter (sdb, "total_conf_ack_read_msg", "total acknowledgement in confirmation bits messages in network", &conf_ack_packets_read, 0, NULL);
    stat_reg_counter (sdb, "total_conf_upgrades_msg", "total upgrades messages in confirmation bits including final ack and ack_dir_upgrades", &conf_upgrades, 0, NULL);
    stat_reg_counter (sdb, "total_conf_readupgrades_msg", "total read upgrades messages in confirmation bits including final ack and ack_dir_upgrades", &conf_readupgrades, 0, NULL);
    stat_reg_counter (sdb, "total_conf_sync_lock", "total sync locks messages in confirmation bits", &conf_sync_lock, 0, NULL);
    stat_reg_counter (sdb, "total_acknowledge_msg", "total acknowledgement not in confirmation bits messages in network", &acknowledge_packets, 0, NULL);

    /* wb split statistics */
    stat_reg_counter (sdb, "total_wb_conflict_event", "total wb conflicts event in network", &wb_split_conf_event, 0, NULL);
    stat_reg_counter (sdb, "total_wb_conf", "total wb conflicts event in network", &WbSplitConf, 0, NULL);
    stat_reg_counter (sdb, "total_wb_failconf", "total wb conflicts event in network", &WbSplitFailConf, 0, NULL);
    stat_reg_counter (sdb, "total_wb_msg", "total write back messages", &total_wb_msg, 0, NULL);
    stat_reg_counter (sdb, "total_wbhead_msg", "total write back head messages", &total_wbhead_msg, 0, NULL);
    stat_reg_counter (sdb, "total_wb_head_success", "total wb header messages", &total_wb_head_success, 0, NULL);
    stat_reg_counter (sdb, "total_wb_head_fail", "total wb header conflict", &total_wb_head_fail, 0, NULL);
    stat_reg_counter (sdb, "total_wbreq_confirmation", "total wb invitation using confirmation", &total_wbreq_confirmation, 0, NULL);
    stat_reg_counter (sdb, "total_wbreq_convention", "total wb invitation using conventional packets", &total_wbreq_convention, 0, NULL);
    stat_reg_counter (sdb, "total_wb_req_convention_success", "total wb invitation using conventional packets successfully", &total_wb_req_convention_success, 0, NULL);
    stat_reg_counter (sdb, "total_wb_req_convention_fail_event", "total wb invitation conflict event", &total_wb_req_convention_fail_event, 0, NULL);
    stat_reg_counter (sdb, "total_wb_req_convention_fail_time", "total wb invitation conflict time", &total_wb_req_convention_fail_time, 0, NULL);
    stat_reg_counter (sdb, "bit_overflow", "wb split using confirmation bits overflow", &bit_overflow, 0, NULL);
    stat_reg_counter (sdb, "bit_used", "wb split using confirmation bits overflow", &bit_used, 0, NULL);
    stat_reg_counter (sdb, "wb_close", "wb split invitation is close to other meta packet", &wb_close, 0, NULL);

    /* mem split statistics */
    stat_reg_counter (sdb, "total_mem_conflict_event", "total mem conflicts event in network", &mem_split_conf_event, 0, NULL);
    stat_reg_counter (sdb, "total_mem_conf", "total mem conflicts event in network", &MemSplitConf, 0, NULL);
    stat_reg_counter (sdb, "total_mem_failconf", "total mem conflicts event in network", &MemSplitFailConf, 0, NULL);
    stat_reg_counter (sdb, "total_mem_msg", "total memory messages", &total_mem_msg, 0, NULL);
    stat_reg_counter (sdb, "total_memhead_msg", "total memory head messages", &total_memhead_msg, 0, NULL);
    stat_reg_counter (sdb, "total_mem_head_success", "total mem header messages", &total_mem_head_success, 0, NULL);
    stat_reg_counter (sdb, "total_mem_head_fail", "total mem header conflict", &total_mem_head_fail, 0, NULL);
    stat_reg_counter (sdb, "total_memreq_confirmation", "total mem invitation using confirmation", &total_memreq_confirmation, 0, NULL);
    stat_reg_counter (sdb, "total_memreq_convention", "total mem invitation using conventional packets", &total_memreq_convention, 0, NULL);
    stat_reg_counter (sdb, "total_mem_req_convention_success", "total mem invitation using conventional packets successfully", &total_mem_req_convention_success, 0, NULL);
    stat_reg_counter (sdb, "total_mem_req_convention_fail_event", "total mem invitation conflict event", &total_mem_req_convention_fail_event, 0, NULL);
    stat_reg_counter (sdb, "total_mem_req_convention_fail_time", "total mem invitation conflict time", &total_mem_req_convention_fail_time, 0, NULL);

    /* power statistics  */
    stat_reg_counter (sdb, "laser_power_switch_off", "total numbers of laser power is switch off", &laser_power_switchoff_counter, 0, NULL);
    stat_reg_counter (sdb, "laser_power_on_cycle", "total cycles of laser power on", &total_cycle_laser_power_on, 0, NULL);
    stat_reg_counter (sdb, "laser_power_switch_on", "total numbers of laser power is switch on", &laser_power_switchon_counter, 0, NULL);
    stat_reg_counter (sdb, "laser_conf_power_switch_off", "total numbers of laser power is switch off", &conf_laser_power_switchoff_counter, 0, NULL);
    stat_reg_counter (sdb, "laser_conf_power_on_cycle", "total cycles of laser power on", &total_cycle_conf_laser_power_on, 0, NULL);
    stat_reg_counter (sdb, "laser_conf_power_switch_on", "total numbers of laser power is switch on", &conf_laser_power_switchon_counter, 0, NULL);
    stat_reg_counter (sdb, "laser_direction_switch", "total times of laser steering is required", &laser_direction_switch_counter, 0, NULL);
    stat_reg_counter (sdb, "flits_transmit", "total numbers of flits transmittion", &flits_transmit_counter, 0, NULL);
    stat_reg_counter (sdb, "flits_receiving", "total numbers of flits are receiving", &flits_receiving_counter, 0, NULL);

#ifdef CONF_PREDICTOR
    stat_reg_counter (sdb, "wb_retry_conf", "", &wb_retry_conf, 0, NULL);
    stat_reg_counter (sdb, "MaxReqPending", "", &MaxReqPending, 0, NULL);
    stat_reg_counter (sdb, "predict_correct", "", &predict_correct, 0, NULL);
    stat_reg_counter (sdb, "predict_fail", "", &predict_fail, 0, NULL);
    stat_reg_counter (sdb, "hint_received_wrong", "", &hint_received_wrong, 0, NULL);
    stat_reg_counter (sdb, "hint_receive", "", &hint_received, 0, NULL);
    stat_reg_counter (sdb, "packet_conf_hint_conf", "", &packet_conf_hint_conf, 0, NULL);
    stat_reg_counter (sdb, "packet_conf_hint_suc", "", &packet_conf_hint_suc, 0, NULL);
    stat_reg_counter (sdb, "MultiPendingError", "", &MultiPendingError, 0, NULL);
    stat_reg_counter (sdb, "WbPendingError", "", &WbPendingError, 0, NULL);
    stat_reg_counter (sdb, "ReqPendingError", "", &ReqPendingError, 0, NULL);
    stat_reg_counter (sdb, "ReqPendingError2", "", &ReqPendingError2, 0, NULL);
    stat_reg_formula (sdb, "PredErrorRate", "", "predict_correct / (predict_fail + predict_correct)", /* format */ NULL);
    stat_reg_formula (sdb, "WbErrorRate", "", "WbPendingError / predict_fail", /* format */ NULL);
    stat_reg_formula (sdb, "MultiPendingErrorRate", "", "MultiPendingError / predict_fail", /* format */ NULL);
    stat_reg_counter (sdb, "tencase", "", &tencase, 0, NULL);
#endif

    /* network delay statistics */
#ifdef CONF_RES_ACK
    stat_reg_counter (sdb, "data_conflict_packet_num", "total numbers of data packet in confliction", &data_conflict_packet_num, 0, NULL);
    stat_reg_counter (sdb, "meta_conflict_packet_num", "total numbers of meta packet in confliction", &meta_conflict_packet_num, 0, NULL);
    stat_reg_counter (sdb, "mem_data_conflict", "total numbers of mem refill data packet in confliction", &mem_data_conflict, 0, NULL);
    stat_reg_counter (sdb, "mem_req_conflict", "total numbers of mem refill req packet in confliction", &mem_req_conflict, 0, NULL);
    stat_reg_counter (sdb, "data_retry_packet_num", "total times of data packet retry", &data_retry_packet_num, 0, NULL);
    stat_reg_counter (sdb, "meta_retry_packet_num", "total times of meta packet retry", &meta_retry_packet_num, 0, NULL);
    stat_reg_counter (sdb, "mem_data_retry", "total numbers of mem refill data packet in confliction", &mem_data_retry, 0, NULL);
    stat_reg_counter (sdb, "mem_req_retry", "total numbers of mem refill req packet in confliction", &mem_req_retry, 0, NULL);
    stat_reg_counter (sdb, "pending_packet_num", "total numbers of pending packet", &pending_packet_num, 0, NULL);
    stat_reg_counter (sdb, "conflict_delay", "total delay of confliction", &confliction_delay, 0, NULL);
    stat_reg_counter (sdb, "data_confliction_delay", "total delay of data confliction", &data_confliction_delay, 0, NULL);
    stat_reg_counter (sdb, "meta_confliction_delay", "total delay of meta confliction", &meta_confliction_delay, 0, NULL);
    stat_reg_counter (sdb, "pending_delay", "total delay of pending packet", &pending_delay, 0, NULL);
#endif
    /* conflict reason */
    stat_reg_counter (sdb, "MemPckConfEvent", "Conflict event due to memory packets", &MemPckConfEvent, 0, NULL);
    stat_reg_counter (sdb, "MemSecPckConfEvent", "Conflict event due to memory packets", &MemSecPckConfEvent, 0, NULL);
    stat_reg_counter (sdb, "WriteBackConfEvent", "Conflict event due to Write back packets", &WriteBackConfEvent, 0, NULL);
    stat_reg_counter (sdb, "RetransmissionConfEvent", "Conflict event due to Retransmission packets", &RetransmissionConfEvent, 0, NULL);
    stat_reg_counter (sdb, "ReplyConfEvent", "Conflict event due to reply packets", &ReplyConfEvent, 0, NULL);

    stat_reg_counter (sdb, "retry conflict", "conflict with retry packets", &retry_conflict, 0, NULL);
    stat_reg_counter (sdb, "L2_miss_conflict", "conflict due to L2 miss", &L2_miss_conflict, 0, NULL);
    stat_reg_counter (sdb, "all_L2miss_conflict", "conflict due to L2 miss (all the packets involved in a conflict", &all_L2_miss_conflict, 0, NULL);
    stat_reg_counter (sdb, "L2_dirty_conflict", "conflict due to L2 dirty", &L2_dirty_conflict, 0, NULL);
    stat_reg_counter (sdb, "queue_delay_conflict", "conflict due to queue delay (overall)", &queue_delay_conflict, 0, NULL);
    stat_reg_counter (sdb, "L1_L2_conflict", "conflict due to L1 and L2 (L1 and L2 data reply conflict)", &L1_L2_conflict, 0, NULL);
    stat_reg_counter (sdb, "l2_fifo_conflict", "conflict due to L2 fifo conflict", &l2_fifo_conflict, 0, NULL);
    stat_reg_counter (sdb, "request_close_conflict", "conflict due to request close to each other", &request_close_conflict, 0, NULL);
    stat_reg_counter (sdb, "wrong_conflict", "wrong conflict (should be request closed)", &wrong_conflict, 0, NULL);
    stat_reg_counter (sdb, "queue_conflict", "conflict due to queue delay", &queue_conflict, 0, NULL);
    stat_reg_counter (sdb, "steering_queue_conflict", "conflict due to steering delay", &steering_queue_conflict, 0, NULL);
    stat_reg_counter (sdb, "fifo_queue_steering_conflict", "conflict due to all the factors", &fifo_queue_steering_conflict, 0, NULL);


    stat_reg_counter (sdb, "laser_switch_on_delay", "total delay of laser warmup", &laser_switch_on_delay, 0, NULL);
    stat_reg_counter (sdb, "network_delay_in_fiber", "total network delay in fiber", &network_delay_in_fiber, 0, NULL);
    stat_reg_counter (sdb, "steering_delay", "total delay of laser steering", &steering_delay, 0, NULL);
    stat_reg_counter (sdb, "net_work_queue_delay", "total delay due to the queue effect", &network_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "output_queue_delay", "total delay due to the queue effect", &output_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "no_queue_delay", "total number of data packets without queue delay", &no_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "data_network_queue_delay", "total data delay due to the queue effect", &data_network_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "meta_network_queue_delay", "total meta delay due to the queue effect", &meta_network_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "timeslot_wait_delay", "total delay due to data channel time slot", &timeslot_wait_time, 0, NULL);
    stat_reg_counter (sdb, "meta_wait_delay", "total delay due to meta channel time slot", &meta_timeslot_wait_time, 0, NULL);
    stat_reg_counter (sdb, "wbreq_wait_delay", "total delay due to wb req time slot", &wbreq_timeslot_wait_time, 0, NULL);

    stat_reg_counter (sdb, "reply_data_packets", "total data packets which are reply", &reply_data_packets, 0, NULL);
    stat_reg_counter (sdb, "original_replydata_packets", "total data packets which are reply", &original_reply_data_packets, 0, NULL);
    stat_reg_counter (sdb, "write_back_packets", "total data packets which are write back", &write_back_packets, 0, NULL);
    stat_reg_counter (sdb, "no_write_back", "total data packets which are write back", &no_write_back, 0, NULL);
    stat_reg_counter (sdb, "all_write_back", "total data packets which are write back", &all_write_back, 0, NULL);
    stat_reg_counter (sdb, "partial_write_back", "total data packets which are write back", &partial_write_back, 0, NULL);
    stat_reg_counter (sdb, "no_solution_pwb", "total data packets which are write back", &no_solution_pwb, 0, NULL);
    stat_reg_counter (sdb, "original_writeback_packets", "total data packets which are write back", &original_write_back_packets, 0, NULL);
    stat_reg_counter (sdb, "data_retry_queue", "total retried data packets queued", &retry_data_queue, 0, NULL);
    stat_reg_counter (sdb, "meta_retry_queue", "total retried meta packets queued", &retry_meta_queue, 0, NULL);
    stat_reg_counter (sdb, "max_conflict_involves", "maximum packets involves in one conflict", &max_conf, 0, NULL);
    stat_reg_counter (sdb, "max_packet_retry", "maximum number packets retry", &max_packet_retry, 0, NULL);
    stat_reg_counter (sdb, "max_packets_inbuffer", "maximum number packets in the input buffer", &max_packets_inbuffer, 0, NULL);
    stat_reg_counter (sdb, "max_flits_inbuffer", "maximum number flits in the input buffer", &max_flits_inbuffer, 0, NULL);
    stat_reg_counter (sdb, "meta_delay_packets", "number of meta packets delay", &meta_delay_packets, 0, NULL);
    stat_reg_counter (sdb, "wbreq_delay_packets", "number of meta packets delay", &wbreq_delay_packets, 0, NULL);
    stat_reg_counter (sdb, "meta_l1_delay_packets", "number of meta packets delay", &meta_l1_delay_packets, 0, NULL);
    stat_reg_counter (sdb, "meta_packets_forward", "number of meta packets forward", &meta_packets_forward, 0, NULL);
    stat_reg_counter (sdb, "data_packets_forward", "number of data packets forward", &data_packets_forward, 0, NULL);
    stat_reg_counter (sdb, "RTCC", "data_conflict due to Request Time Close to each other Counter", &RTCC, 0, NULL);
    stat_reg_counter (sdb, "RTC_C_miss", "data_conflict due to Request Time Close to each other Counter", &RTCC_miss, 0, NULL);
    stat_reg_counter (sdb, "RTC_C_half", "data_conflict due to Request Time Close to half Counter", &RTCC_half, 0, NULL);
    stat_reg_counter (sdb, "RTC_C_fail", "data_conflict due to Request Time Close failed counter", &RTCC_fail, 0, NULL);
    stat_reg_counter (sdb, "RTC_counter_reduced", "data_conflict due to Request Time Close to each other Counter", &RTCC_reduced, 0, NULL);
    stat_reg_counter (sdb, "meta_reduced_packets", "Request Time Close to each other Counter", &meta_reduced_packets, 0, NULL);
    stat_reg_counter (sdb, "meta_close_packets", "Request Time Close to each other Counter", &meta_close_packets, 0, NULL);
    stat_reg_counter (sdb, "data_packets_delay", "data packets are actually delayed to avoid conflict", &data_packets_delay, 0, NULL);
    stat_reg_counter (sdb, "data_packets_no_delay", "data packets are not delayed to avoid conflict", &data_packets_no_delay, 0, NULL);
    stat_reg_counter (sdb, "data_delay", "data packets delay time", &data_delay, 0, NULL);
    stat_reg_counter (sdb, "data_packet_generate_max", "data packets generation max per 100 cycles", &data_packet_generate_max, 0, NULL);


    /* conflicts statistics */
    stat_reg_counter (sdb, "data_sync_conf_event", "", &sync_conf_event, 0, NULL);
    stat_reg_counter (sdb, "data_sync_conf_packets", "", &sync_conf_packets, 0, NULL);
    stat_reg_counter (sdb, "meta_sync_conf_event,", "", &meta_sync_conf_event, 0, NULL);
    stat_reg_counter (sdb, "meta_sync_conf_packets", "", &meta_sync_conf_packets, 0, NULL);
    stat_reg_counter (sdb, "meta_ldl_conf_event,", "", &meta_ldl_conf_event, 0, NULL);
    stat_reg_counter (sdb, "meta_ldl_conf_packets", "", &meta_ldl_conf_packets, 0, NULL);

    stat_reg_counter (sdb, "max_queue_delay", "maximum queue delay", &max_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "more_than_Two_conflict", "", &more_than_two_conflict, 0, NULL);
    stat_reg_counter (sdb, "meta_more_than_Two_conflict", "", &meta_more_than_two_conflict, 0, NULL);
    stat_reg_counter (sdb, "meta_ldl_more_than_Two_conflict", "", &meta_ldl_more_than_two_conflict, 0, NULL);
    int j = 0, m = 0;
    /*   for(i=0; i<(mesh_size+2)*(mesh_size+2)*2; i++)
       {
    	for(j=0; j<(mesh_size+2)*(mesh_size+2); j++)
    	{
    		for(m=0; m<(mesh_size+2)*(mesh_size+2); m++)
    		{
    			char ch2[32];
    			sprintf(ch2, "data_queue_%d_%d_%d", i, j, m);
    			stat_reg_counter(sdb, ch2, "", &conflict_table[i][j][m], 0, NULL);
    		}
    	}
    }
       for(i=0; i<(mesh_size+2)*(mesh_size+2)*2; i++)
    	{
    		for(j=0; j<(mesh_size+2)*(mesh_size+2); j++)
    		{
    			for(m=0; m<(mesh_size+2)*(mesh_size+2); m++)
    			{
    				char ch3[32];
    				sprintf(ch3, "meta_queue_%d_%d_%d", i, j, m);
    				stat_reg_counter(sdb, ch3, "", &meta_conflict_table[i][j][m], 0, NULL);
    				char ch4[32];
    				sprintf(ch4, "meta_ldl_queue_%d_%d_%d", i, j, m);
    				stat_reg_counter(sdb, ch4, "", &meta_ldl_conflict_table[i][j][m], 0, NULL);
    			}
    		}
    	}*/
//
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "NetQueueDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &network_queue_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "OutQueueDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &output_queue_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "DataQueueDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &data_network_queue_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "MetaQueueDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &meta_network_queue_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "PendDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &pending_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "SteerDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &steering_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "DataColDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &data_confliction_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "MetaColDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &meta_confliction_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "LaserSDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &laser_switch_on_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "TSDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &timeslot_wait_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<PROCESSOR; i++)
    {
        char ch2[32];
        sprintf(ch2, "MetaTSlDelay_%d", i);
        stat_reg_counter(sdb, ch2, "", &meta_timeslot_wait_Delay[i], 0, /* format */NULL);
    }
    for(i=0; i<24; i++)
    {
        char ch2[32];
        sprintf(ch2, "queue_%d", i);
        stat_reg_counter(sdb, ch2, "queue delay distribution", &queue_distr[i], 0, /* format */NULL);
    }
    for(i=0; i<20; i++)
    {
        char ch2[32];
        sprintf(ch2, "distance_%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bincount for distance of sending two packets", &dist_bin[i], 0, /* format */NULL);
    }
    for(i=0; i<7; i++)
    {
        char ch2[32];
        sprintf(ch2, "meta_%d_way_conf", i);
        stat_reg_counter(sdb, ch2, "", &meta_way_conf[i], 0, /* format */NULL);
    }
    for(i=0; i<7; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_%d_way_conf", i);
        stat_reg_counter(sdb, ch2, "", &data_way_conf[i], 0, /* format */NULL);
    }
    for(i=0; i<7; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_conf_inv%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bincount for packets involved in a conflict", &data_conf_inv_bincount[i], 0, /* format */NULL);
    }
    for(i=0; i<3; i++)
    {
        char ch2[32];
        sprintf(ch2, "wb_conf_inv%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bincount for wb packets involves in a conflict", &write_back_inv_bincount[i], 0, /* format */NULL);
    }
    for(i=0; i<4; i++)
    {
        char ch2[32];
        sprintf(ch2, "request_%d.conflict", i);
        stat_reg_counter(sdb, ch2, "request conflict on destination per event", &req_conf[i], /* intital value */0, /* format */NULL);
    }
    for(i=0; i<4; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_%d.conflict", i);
        stat_reg_counter(sdb, ch2, "data conflict on destination per event", &data_conf[i], /* intital value */0, /* format */NULL);
    }
}

/* *****************************************************************************/

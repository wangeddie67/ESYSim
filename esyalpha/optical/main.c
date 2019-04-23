/* when the sim_main started, call maindirectsim function to initialize the message list and flit list */
#include "point-point.h"
void maindirectsim(int map_algr, int net_algr)
{
    int i = 0, n=0, m=0, l =0;
    meta_conf_event = 0;
    meta_conflict_pro = 0;
    data_conf_event = 0;
    data_conflict_pro = 0;
    no_queue_delay = 0;
    large_retry = 0;
    small_retry = 0;
    more_than_two_conflict = 0;

    wb_close = 0;
    acrossUsed = 0;
    for(m=0; m<24; m++)
        queue_distr[m] = 0;

#ifdef CONF_PREDICTOR
    for(m=0; m<MAXRECEIVERS; m++)
    {
        for(n=0; n<PROCESSOR; n++)
        {
            conf_packet_list[m][n] = -1;
#ifdef EMC
            meta_conf_packet_list[m][n] = -1;
#endif
        }
        conf_packet_num[m] = 0;
#ifdef EMC
        meta_conf_packet_num[m] = 0;
#endif
        conf_PID[m] = 0;
        ConfWait[m] = 0;
    }
    predict_correct = 0;
    predict_fail = 0;
    predictCorrect_two = 0;
    predictCorrect_more = 0;
    predictFail_two = 0;
    predictFail_more = 0;
    predict_table_correct = 0;
    predict_req_correct = 0;
    hint_received_wrong = 0;
    hint_received = 0;
    packet_conf_hint_conf = 0;
    packet_conf_hint_suc = 0;
    ReqPendingError = 0;
    ReqPendingError2 = 0;
    WbPendingError = 0;
    MultiPendingError = 0;
    MaxReqPending = 0;
    TotalReqPendingNum = 0;
    wb_retry_conf = 0;
    tencase = 0;

    for(m=0; m<PROCESSOR; m++)
    {
        for(n=0; n<PROCESSOR; n++)
            for(i=0; i<PROCESSOR; i++)
                conflict_table[m][n][i] = 0;
        for(n=0; n<REQ_QUEUE_SIZE; n++)
        {
            ReqPendingQueue[m][n].valid = 0;
            ReqPendingQueue[m][n].addr = 0;
            ReqPendingQueue[m][n].des = -1;
            ReqPendingQueue[m][n].op_code = 0;
            ReqPendingQueue[m][n].time = 0;
        }
        ReqPendingQueue_num[m] = 0;
        last_packet_cycle[m][0] = 0;
        last_packet_cycle[m][1] = 0;
    }
    for(m=0; m<20; m++)
        dist_bin[m] = 0;
#endif

    allocate_free_flit_list();

    /* fot statistics counter intialization */
    flit_tranfer_time = flit_size/(laser_cycle * meta_transmitter_size);
    data_flit_tranfer_time = flit_size/(laser_cycle * data_transmitter_size);
    time_slot_data = data_packet_size/flit_tranfer_time;
    transmitter_busy = 0;

    bit_overflow = 0;
    bit_used = 0;
    channel_occupancy = 0;
    meta_packets = 0;
    data_packets = 0;
    data_packet_transfer = 0;
    meta_packet_transfer = 0;
    conf_ack_packets = 0;
    conf_ack_packets_read = 0;
    conf_upgrades = 0;
    conf_readupgrades = 0;

    conf_sync_lock = 0;
    acknowledge_packets = 0;
    input_extra_overflow = 0;
    wb_split_conf_event = 0;
    WbSplitFailConf = 0;
    WbSplitConf = 0;
    total_wb_msg = 0;
    total_wbhead_msg = 0;
    total_wb_head_success = 0;
    total_wb_head_fail = 0;
    total_wbreq_confirmation = 0;
    total_wbreq_convention = 0;
    total_wb_req_convention_success = 0;
    total_wb_req_convention_fail_event = 0;
    total_wb_req_convention_fail_time = 0;

    mem_split_conf_event = 0;
    MemSplitFailConf = 0;
    MemSplitConf = 0;
    total_mem_msg = 0;
    total_memhead_msg = 0;
    total_mem_head_success = 0;
    total_mem_head_fail = 0;
    total_memreq_confirmation = 0;
    total_memreq_convention = 0;
    total_mem_req_convention_success = 0;
    total_mem_req_convention_fail_event = 0;
    total_mem_req_convention_fail_time = 0;


    /* conflict reason */
    MemPckConfEvent = 0;
    MemSecPckConfEvent = 0;
    WriteBackConfEvent = 0;
    RetransmissionConfEvent = 0;
    ReplyConfEvent = 0;

    retry_conflict = 0;
    L2_miss_conflict = 0;
    L2_dirty_conflict = 0;
    queue_delay_conflict = 0;
    L1_L2_conflict = 0;
    all_L2_miss_conflict = 0;
    l2_fifo_conflict = 0;
    fifo_queue_steering_conflict = 0;
    steering_queue_conflict = 0;
    queue_conflict = 0;
    wrong_conflict = 0;
    request_close_conflict = 0;
    confirmation_bit = 0;
    confliction_delay = 0;

    for(m=0; m<PROCESSOR; m++)
    {
        network_queue_Delay[m] = 0;
        output_queue_Delay[m] = 0;
        data_network_queue_Delay[m] = 0;
        meta_network_queue_Delay[m] = 0;
        pending_Delay[m] = 0;
        steering_Delay[m] = 0;
        data_confliction_Delay[m] = 0;
        meta_confliction_Delay[m] = 0;
        laser_switch_on_Delay[m] = 0;
        timeslot_wait_Delay[m] = 0;
        meta_timeslot_wait_Delay[m] = 0;
    }

    network_queue_delay = 0;
    output_queue_delay = 0;
    data_network_queue_delay = 0;
    meta_network_queue_delay = 0;
    pending_delay = 0;
    steering_delay = 0;
    data_confliction_delay = 0;
    meta_confliction_delay = 0;
    laser_switch_on_delay = 0;
    timeslot_wait_time = 0;
    meta_timeslot_wait_time = 0;

    network_delay_in_fiber = 0;
    data_retry_packet_num = 0;
    meta_retry_packet_num = 0;
    pending_packet_num = 0;
    data_conflict_packet_num = 0;
    meta_conflict_packet_num = 0;
    mem_data_conflict = 0;
    mem_req_conflict = 0;
    mem_data_retry = 0;
    mem_req_retry = 0;
    wbreq_timeslot_wait_time = 0;
    write_back_packets = 0;
    no_write_back = 0;
    all_write_back = 0;
    partial_write_back = 0;
    no_solution_pwb = 0;
    reply_data_packets = 0;
    original_reply_data_packets = 0;
    original_write_back_packets = 0;
    retry_data_queue = 0;
    retry_meta_queue = 0;
    max_conf = 0;
    max_packets_inbuffer = 0;
    max_flits_inbuffer = 0;
    max_packet_retry = 0;
    meta_delay_packets = 0;
    wbreq_delay_packets = 0;
    meta_l1_delay_packets = 0;
    meta_packets_forward = 0;
    data_packets_forward = 0;
    RTCC = 0;
    RTCC_half = 0;
    RTCC_fail = 0;
    RTCC_miss = 0;
    RTCC_reduced = 0;
    meta_close_packets = 0;
    meta_reduced_packets = 0;
    data_packets_delay = 0;
    data_packets_no_delay = 0;
    data_delay = 0;
    data_packet_generate_max = 0;
    data_packet_generate = 0;
    total_cycle_laser_power_on = 0;
    total_cycle_conf_laser_power_on = 0;
    laser_direction_switch_counter = 0;
    flits_transmit_counter = 0;
    flits_receiving_counter = 0;
    conf_laser_power_switchoff_counter = 0;
    conf_laser_power_switchon_counter = 0;

    for(n=0; n<6; n++)
        data_conf_inv_bincount[n] = 0;
    for(n=0; n<3; n++)
        write_back_inv_bincount[n] = 0;

    max_queue_delay = 0;
    for(n=0; n<PROCESSOR; n++)
    {
        last_Output_queue_full[n] = 0;
        confirmation_bit_inf[n] = 0;
        delay_counter[0][n] = 0;
        delay_counter[1][n] = 0;
        for(l=0; l<PROCESSOR; l++)
        {
            delay_count[0][n][l] = 0;
            delay_count[1][n][l] = 0;
        }
        RTCC_flag[n] = 0;
        delay_counter_backup[n] = 0;
        last_meta_packet_fake[n] = 0;
        last_packet_time[n] = 0;
        last_packet_des[n] = 0;
        last_packet_des_backup[n] = 0;
        last_close_time[n] = 0;
        meta_wait_time[n] = 0;
        backup_transfer_waittime[n] = 0;
        last_delay_for_data[n] = 0;
        transmitter_backup_schedule[n] = 0;
        last_meta_packet_des_fake[n] = 0;
        first_packet_time[n] = 0;
        packets_sending[n] = 0;
        input_buffer_num[n][0] = 0;
        input_buffer_num[n][1] = 0;
        laser_power[0][n] = LASER_OFF;
        laser_power[1][n] = LASER_OFF;
        laser_power_on_cycle[0][n] = 0;
        laser_power_on_cycle[1][n] = 0;
        laser_power_start_cycle[0][n] = 0;
        laser_power_start_cycle[1][n] = 0;
        conf_laser_power[0][n] = LASER_OFF;
        conf_laser_power[1][n] = LASER_OFF;
        conf_laser_power_on_cycle[0][n] = 0;
        conf_laser_power_on_cycle[1][n] = 0;
        conf_laser_power_start_cycle[0][n] = 0;
        conf_laser_power_start_cycle[1][n] = 0;
        last_data_time[n] = 0;
        last_time[n] = 0;

        req_conf[n] = 0;
        data_conf[n] = 0;
        req_cycle_conflicts[n] = 0;
        data_cycle_conflicts[n] = 0;
        channel_busy_meta[n] = 0;
        channel_busy_data[n] = 0;

    }

    /* for the input buffer and output buffer*/
    for(n=0; n<PROCESSOR; n++)
    {
        meta_receiver[n] = (Flit_list *)malloc(256*sizeof(Flit_list));
        data_receiver[n] = (Flit_list *)malloc(256*sizeof(Flit_list));
        data_way_conf[n] = 0;
        meta_way_conf[n] = 0;

        flit_list_init(transmitter_meta[n]);
        flit_list_init(transmitter_data[n]);
        flit_list_init(transmitter_backup[n]);

        for(i=0; i<meta_receivers; i++)
            flit_list_init(meta_receiver[n][i]);

        for(i=0; i<data_receivers; i++)
        {
            flit_list_init(data_receiver[n][i]);
            receiver_occ_time[n][i] = 0;
            data_conf_involves[n][i] = 0;
            bin_count_start[n][i] = 0;
            bin_count_end[n][i] = 0;
            write_back_involves[n][i] = 0;
            last_meta_packet[i][n] = 0;
            last_meta_packet_des[i][n] = 0;
        }
        for(i=0; i<PROCESSOR; i++)
            RT_time[n][i] = 0;
    }
    /* Message list intialization */
    msg_list = (Msg_list *)malloc(sizeof(Msg_list));
    Msg_list_init(msg_list);

    /* algorisms read from configuration file */
    Map_algr = map_algr;
    network_algr = net_algr;
}

void Msg_list_init(Msg_list *msg_list)
{
    msg_list->head = NULL;
    msg_list->tail = NULL;
}
void flit_list_init(Flit_list flit_alist)
{
    flit_alist.head = NULL;
    flit_alist.tail = NULL;
    flit_alist.flits_in_list = 0;
}



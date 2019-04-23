#include "point-point.h"
/* ************************** SIM_MESS.C ***************************************/
/* handling EVG_MSG, adding packet into sending flit list, adding receive message into the message list */
int laser_steering(int src, int des, counter_t sim_cycle);
int timeslot(counter_t schedule_time, int type);
int pseudo_schedule(int st1, int st0);

void queue_statistics(int queue_delay)
{
    if(queue_delay<20)
        queue_distr[queue_delay] ++;
    else if(queue_delay >=20 && queue_delay < 25)
        queue_distr[20] ++;
    else if(queue_delay >=25 && queue_delay < 30)
        queue_distr[21] ++;
    else if(queue_delay >=30 && queue_delay < 35)
        queue_distr[22] ++;
    else if(queue_delay >=35)
        queue_distr[23] ++;
}

counter_t time_schedule(Packet packet, counter_t current_time, int busy_flag, int *tran_type, int *meta_delay, counter_t *transfer_time_flag, int *queue_time, int *steering_time, int *ts_delay)
{
    long size = packet.packet_size;
    int p_type = packet.packet_type;
    long src = packet.src;
    long des = packet.des;
    int delay, packet_transfer_time, schedule_time, setup_time, laser_steering_delay;
    int rec_id = packet.des % data_receivers;
    *transfer_time_flag = 0;

    if(p_type == META)
        packet_transfer_time = size*flit_tranfer_time;
    else
        packet_transfer_time = size*data_flit_tranfer_time;

    /* laser setup time and steering time */
    setup_time = setup_delay(src, des,current_time, packet.channel);
    laser_steering_delay = laser_steering(src, des,current_time);
    steering_delay += laser_steering_delay;
    steering_Delay[src] += laser_steering_delay;

    if(packet.retry == 1 && setup_time)
    {
        laser_power[packet.channel][src] = LASER_ON;
        setup_time = 0;
    }
    /* conflict reasons */
    packet.steering_time = laser_steering_delay;
    *steering_time = setup_time;
#ifdef IDEALCASE
    long src1, src2, des1, des2;
    src1 = src/(mesh_size+2);
    src2 = src%(mesh_size+2);
    des1 = des/(mesh_size+2);
    des2 = des%(mesh_size+2);
    int hops = abs(src1-des1)+abs(src2-des2);
    int turning = 1;
    if(src1 == des1 || src2 == des2)
        turning = 0;
#ifdef CORONA
    int up_src = 0, up_des = 0, left_src = 0, left_des = 0, zone_src = 0, zone_des = 0;
    if(src1 <= 4)
        up_src = 1;
    else
        up_src = 0;
    if(src2 <= 4)
        left_src = 1;
    else
        left_src = 0;

    if(des1 <= 4)
        up_des = 1;
    else
        up_des = 0;
    if(des2 <= 4)
        left_des = 1;
    else
        left_des = 0;

    if(up_src && left_src)
        zone_src = 1;
    else if(up_src)
        zone_src = 2;
    else if(left_src)
        zone_src = 3;
    else
        zone_src = 4;

    if(up_des && left_des)
        zone_des = 1;
    else if(up_des)
        zone_des = 2;
    else if(left_des)
        zone_des = 3;
    else
        zone_des = 4;
    delay = abs(zone_des - zone_src);
    if(!delay)
        delay = 1;
    else
    {
        int des_des = (des1-1)/2 * 4 + des2/2;
        delay += arbi_delay[des_des];
        if(delay > 2000)
            printf("delay %d\n", delay);
        arbi_delay[des_des] += 8+1; //one cycle to acquire and release the bandwidth. 8 cycles are arbitration delay plus the data transfer delay
    }
#else
    delay = packet_transfer_time + hops*2;
#endif
#else
    delay = setup_time + laser_steering_delay + packet_transfer_time;	// tranfer time and laser setup time
#endif

    /* statistics collection */

    if(!busy_flag)
    {
        schedule_time = current_time + delay;   /* there is no packet in queue, so no queue delay */
        if(p_type == DATA)
            no_queue_delay ++;
    }
    else
    {
#ifdef IDEALCASEn
        packet.queue_time = delay_count[packet.channel][src][des] - current_time;
        *queue_time = delay_count[packet.channel][src][des] - current_time;
        schedule_time = delay_count[packet.channel][src][des] + delay;   /* schedule time should add the queue delay */
        if(p_type == DATA && (packet.operation != WRITE_BACK && packet.operation != MEM_WRITE_BACK))
            queue_statistics(delay_count[packet.channel][src][des] - current_time);
#else
        packet.queue_time = delay_counter[packet.channel][src] - current_time;
        *queue_time = delay_counter[packet.channel][src] - current_time;
        schedule_time = delay_counter[packet.channel][src] + delay;   /* schedule time should add the queue delay */
        if(p_type == DATA && (packet.operation != WRITE_BACK && packet.operation != MEM_WRITE_BACK))
            queue_statistics(delay_counter[packet.channel][src] - current_time);
#endif

        /* statistics collection */
        if(packet.retry != 1)
        {
            if(p_type == DATA)
            {
                data_network_queue_delay += delay_counter[packet.channel][src] - current_time;  /* data queue delay */
                data_network_queue_Delay[packet.src] += delay_counter[packet.channel][src] - current_time;  /* data queue delay */
                if(delay_counter[packet.channel][src] - current_time > max_queue_delay)
                    max_queue_delay = delay_counter[packet.channel][src] - current_time;
            }
            else
            {
                meta_network_queue_delay += delay_counter[packet.channel][src] - current_time;  /* meta queue delay */
                meta_network_queue_Delay[packet.src] += delay_counter[packet.channel][src] - current_time;  /* meta queue delay */
            }
        }
        else if(packet.retry == 1)
        {
            if(p_type == DATA)
                retry_data_queue ++;
            else
                retry_meta_queue ++;
        }
    }

    /* statistics collection */
    if((setup_time < laser_warmup_time) && (first_packet_time[src] != 0))
        last_packet_time[src] = schedule_time - packet_transfer_time + 1;
    else
    {
        first_packet_time[src] = schedule_time - packet_transfer_time + 1;
        last_packet_time[src] = first_packet_time[src];
    }
#ifdef IDEALCASE
    return schedule_time;
#endif
    if(p_type == DATA)
    {
        if(data_chan_timeslot == 1)
        {
            int TsDelay = timeslot(schedule_time, p_type); /* Time slot function */
            packet.ts_delay = TsDelay;
            *ts_delay = TsDelay;
            schedule_time = schedule_time + TsDelay;
        }
    }
    else
    {
        if(meta_chan_timeslot == 1)
        {
            int TsDelay = timeslot(schedule_time, p_type); /* Time slot function */
            packet.ts_delay = TsDelay;
            *ts_delay = TsDelay;
            schedule_time = schedule_time + TsDelay;
        }

        if(packet.operation == MISS_IL1 || packet.operation == MISS_READ || packet.operation == MISS_WRITE || packet.operation == WRITE_BACK_REPLY)
        {
            /* collected statistics for pre-schedule mechanisms */
            *transfer_time_flag = schedule_time;
            if(packet.des != last_meta_packet_des_fake[src])
            {
                if(schedule_time - last_meta_packet_fake[src] < data_packet_size*data_flit_tranfer_time)
                {
                    meta_reduced_packets ++;
                    meta_close_packets ++;
                    if(RTCC_flag[src] == 0)
                        meta_close_packets ++;
                    RTCC_flag[src] = 1;
                    *transfer_time_flag = schedule_time;
                    last_close_time[src] = schedule_time - last_meta_packet_fake[src];
                }
                else
                {
                    RTCC_flag[src] = 0;
                    last_delay_for_data[src] = 0;
                }
            }
            else
            {
                if((schedule_time - last_meta_packet_fake[src] + last_close_time[src])< data_packet_size*data_flit_tranfer_time)
                {
                    meta_close_packets ++;
                    *transfer_time_flag = schedule_time;
                }
                else
                {
                    RTCC_flag[src] = 0;
                    last_delay_for_data[src] = 0;
                    last_close_time[src] = 0;
                }
            }
            last_meta_packet_fake[src] = schedule_time;
            last_meta_packet_des_fake[src] = des;
        }

#ifdef PSEUDO_SCHEDULE
        if(packet.operation == MISS_IL1 || packet.operation == MISS_READ || packet.operation == MISS_WRITE || packet.operation == MEM_READ_REQ || packet.operation == WRITE_BACK_REPLY || packet.operation == MEM_PACK_REPLY)
        {
            /* see whether these request packets are close to each other */
            if(packet.des != last_meta_packet_des[rec_id][src] && current_time != 0)
            {
                /* they should go to different destinations */
                if(schedule_time - last_meta_packet[rec_id][src] < data_packet_size*data_flit_tranfer_time)
                {
                    //meta_wait_time[src] = pseudo_schedule((schedule_time-1)%(data_packet_size*data_flit_tranfer_time), (last_meta_packet[rec_id][src]-1)%(data_packet_size*data_flit_tranfer_time));
                    meta_wait_time[src] = data_packet_size*data_flit_tranfer_time - (schedule_time - last_meta_packet[rec_id][src]) + 1;
                    meta_timeslot_wait_time += meta_wait_time[src];
                    meta_timeslot_wait_Delay[src] += meta_wait_time[src];
                    schedule_time = schedule_time + meta_wait_time[src];
                    meta_delay_packets ++;
                    packet.transmitter_time = schedule_time;
                    *transfer_time_flag = schedule_time;
                    packet.transfer_time = schedule_time;
                    if(meta_chan_timeslot == 1 && meta_wait_time[src])
                    {
                        int TsDelay = timeslot(schedule_time, p_type); /* Time slot function */
                        packet.ts_delay = TsDelay;
                        *ts_delay = TsDelay;
                        schedule_time = schedule_time + TsDelay;
                    }
                }
            }
            last_meta_packet[rec_id][src] = schedule_time;
            last_meta_packet_des[rec_id][src] = des;
        }
#endif

    }
    if(setup_time == laser_warmup_time && laser_power[packet.channel][src] == LASER_OFF)
    {
        laser_power_start_cycle[packet.channel][src] = schedule_time - packet_transfer_time;
        laser_power[packet.channel][src] = LASER_ON;
        laser_power_switchon_counter ++;
    }
    return schedule_time;
}

int pseudo_schedule(int st1, int st0)
{
    int delay = 0;
    switch (st0)
    {
    case 0:
        if(st1 == 2)
            delay = 4;
        else if(st1 == 4)
            delay = 2;
        else
            panic("out of data spand or meta packets are not sloted\n");
        break;
    case 1:
        if(st1 == 3)
            delay = 4;
        else if(st1 == 0)
            delay = 2;
        else
            panic("out of data spand or meta packets are not sloted\n");
        break;
    case 2:
        if(st1 == 4)
            delay = 2;
        else if(st1 == 1)
            delay = 0;
        else
            panic("out of data spand or meta packets are not sloted\n");
        break;
    case 3:
        if(st1 == 0)
            delay = 2;
        else if(st1 == 2)
            delay = 0;
        else
            panic("out of data spand or meta packets are not sloted\n");
        break;
    case 4:
        if(st1 == 1)
            delay = 4;
        else if(st1 == 3)
            delay = 2;
        else
            panic("out of data spand or meta packets are not sloted\n");
        break;
    default:
        break;
    }
    return delay;
}

int timeslot(counter_t schedule_time, int type)
{
    int packet_transfer_time;
    if(type == DATA)
        packet_transfer_time = (data_packet_size+laser_setup_time)*data_flit_tranfer_time;
    else
        packet_transfer_time = (meta_packet_size+laser_setup_time)*flit_tranfer_time;

    counter_t start_cycle = schedule_time - packet_transfer_time + 1;

    counter_t timeflag = start_cycle;

    if(start_cycle % (packet_transfer_time) != 0)
        start_cycle = (start_cycle/packet_transfer_time)*packet_transfer_time + packet_transfer_time;

    return (start_cycle - timeflag);
}
int is_transmitter_backup_empty(int src)
{
    int flag = 0;
    if(transmitter_backup[src].tail == NULL)
        flag = 1; //empty
    return flag;
}
int PID_form(int PID)
{
    /* return PID + PID_bar */
#ifdef PRE_ID
    return (1<<PID);
#else
    return (PID<<PID_shift)|(~PID&PID_bar_mask);
#endif
}

void packet_spend(int src, int a, counter_t sim_cycle)
{
    int dis = sim_cycle - last_packet_cycle[src][a];
    switch(dis)
    {
    case 0:
        dist_bin[19] ++;
        break;
    case 1:
        dist_bin[0] ++;
        break;
    case 2:
        dist_bin[1] ++;
        break;
    case 3:
        dist_bin[2] ++;
        break;
    case 4:
        dist_bin[3] ++;
        break;
    case 5:
        dist_bin[4] ++;
        break;
    case 6:
        dist_bin[5] ++;
        break;
    case 7:
        dist_bin[6] ++;
        break;
    case 8:
        dist_bin[7] ++;
        break;
    case 9:
        dist_bin[8] ++;
        break;
    case 10:
        dist_bin[9] ++;
        break;
    case 11:
        dist_bin[10] ++;
        break;
    case 12:
        dist_bin[11] ++;
        break;
    case 13:
        dist_bin[12] ++;
        break;
    case 14:
        dist_bin[13] ++;
        break;
    case 15:
        dist_bin[14] ++;
        break;
    case 16:
        dist_bin[15] ++;
        break;
    case 17:
        dist_bin[16] ++;
        break;
    case 18:
        dist_bin[17] ++;
        break;
    default:
        dist_bin[18] ++;
    }
    last_packet_cycle[src][a] = sim_cycle;
}

void receive_msg(Packet packet, counter_t sim_cycle)
{
    /* first receive the packet from the processor, adding packet into its sending flit list */
    long size = packet.packet_size;
    int p_type = packet.packet_type;

    long src = packet.src;
    long des = packet.des;
    long i;
    int busy_flag = 0, tran_type = 0;
    int match = 0, meta_delay = 0;
    counter_t current_time = packet.start_time;
    counter_t transfer_time_flag;
    counter_t packet_transfer_time, schedule_time;
    int queue_time = 0, ts_delay = 0, steering_time = 0;
    int rec_id = packet.des % data_receivers;
    if(p_type == META)
        packet_transfer_time = size*flit_tranfer_time;
    else
        packet_transfer_time = size*data_flit_tranfer_time;

    if(current_time < sim_cycle)
        current_time = sim_cycle;

    busy_flag = channel_busy_check(src, des, packet.channel, sim_cycle);
    packet_spend(src, packet.channel, sim_cycle);

#ifdef WB_SPLIT_NET
    if(packet.operation == WRITE_BACK_REPLY)
    {
        if(packet.retry_num == 1)
            total_wb_req_convention_fail_event ++;
        total_wb_req_convention_fail_time ++;
    }
    else if(packet.operation == WRITE_BACK)
    {
        if(packet.retry_num == 1)
            wb_split_conf_event ++;
    }
#ifdef MEM_SPLIT_NET
    if(packet.operation == MEM_PACK_REPLY)
    {
        if(packet.retry_num == 1)
            total_mem_req_convention_fail_event ++;
        total_mem_req_convention_fail_time ++;
    }
    else if(packet.operation == WAIT_MEM_READ_N)
    {
        if(packet.retry_num == 1)
            mem_split_conf_event ++;
    }
#endif
#endif
    if(packet.retry == 0)
        match = is_appending_addr(packet);
    else
        match = 0;

    /* firt check if this packet has the same addr with the appending packet, if does, not transfer this packet until appending packet is clear*/
    if(!match)
    {
        if(packet.retry == 1)
        {
            if(delay_counter[packet.channel][src] >= sim_cycle)
                transmitter_busy ++;
        }
        schedule_time = time_schedule(packet, current_time, busy_flag, &tran_type, &meta_delay, &transfer_time_flag, &queue_time, &steering_time, &ts_delay);

        /* then adding receive message (DATA_MSG or META_MSG) to message list */
        if(p_type == DATA)
        {
            /* if it is a data packet */
            /* all data packets are sloted in 5 cycles */
            data_packet_transfer ++;

            while(packet.flitnum != 0)
            {
                /* schedule the data packet tranporting time */
                int flitNum = packet.flitnum;
                flitNum--;
                packet.flitnum = flitNum;
                if(packet.schedule == 0 && packet.retry_num == 0)
                {
                    packet.meta_delay = meta_delay;
                    packet.steering_time = steering_time;
                    packet.queue_time = queue_time;
                    packet.ts_delay = ts_delay;
                }
                packet.transfer_time = schedule_time;
                add_message(schedule_time - flitNum*data_flit_tranfer_time, DATA_MSG, packet);
            }
        }
        else
        {
            /* schedule the meta packet tranporting time */
            meta_packet_transfer ++;
            while(packet.flitnum != 0)
            {
                /* schedule the data packet tranporting time */
                int flitNum = packet.flitnum;
                flitNum--;
                packet.flitnum = flitNum;
                packet.transfer_time = schedule_time;
                if(packet.schedule == 0 && packet.retry_num == 0)
                {
                    packet.meta_delay = meta_delay;
                    packet.transmitter_time = transfer_time_flag;
                    packet.steering_time = steering_time;
                    packet.queue_time = queue_time;
                    packet.ts_delay = ts_delay;
                }
                add_message(schedule_time - flitNum*flit_tranfer_time, META_MSG, packet);
            }
        }

        packet.flitnum = size;
#ifdef CONF_PREDICTOR
        if(p_type == DATA)
            input_buffer_add(packet, schedule_time - (size - 1) + 2 + laser_setup_time + confirmation_time);  /* FIXME 2 cycle to wait the hint */
        else
#ifdef EMC
            input_buffer_add(packet, schedule_time + laser_setup_time + confirmation_time - 1); //EMC
#else
            input_buffer_add(packet, schedule_time + laser_setup_time + confirmation_time); //not EMC
#endif

#else
        input_buffer_add(packet, schedule_time + laser_setup_time + confirmation_time);
#endif

        /* recording the infront packet destination for the laser_setup_time*/
        if(tran_type == 0)
        {
#ifdef IDEALCASEn
            delay_count[packet.channel][src][des] = schedule_time;
#else
            delay_counter[packet.channel][src] = schedule_time;
            last_packet_des[src] = des;
            laser_power_on_cycle[packet.channel][src] = delay_counter[packet.channel][src];
#endif
        }

        /* Insert flit into transmitter buffer */
    }
    else
    {
        input_buffer_add(packet, WAIT_TIME);
    }

}
#ifdef WB_SPLIT_NET
/* this function is used for replying the write back packets responding to the write back requester */
void receive_wb_reply_msg(Packet packet, counter_t sim_cycle)
{
    int i=0, src, des;
    if(packet.conf_bit != -1)
    {
        des = packet.src;
        src = packet.des;
    }
    else
    {
        src = packet.src;
        des = packet.des;
    }
    for(i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.msgno == packet.msgno)
        {
            Packet new_packet = input_buffer[src].InputEntry[i].packet;
            new_packet.conf_bit = packet.conf_bit;
            if(new_packet.operation == WRITE_BACK || new_packet.operation == WAIT_MEM_READ_N)
            {
                retry_packet(new_packet.msgno, sim_cycle, src);
                return 0;
            }
        }
    }
    panic("can not found write back packet in the input buffer!\n");
}

/* received the wb header successfully, schedule a wb request msg either through confirmation laser or meta packets */
void receive_wb_req_msg(Packet packet, counter_t sim_cycle)
{
    int p_type = packet.packet_type;
    long src = packet.src;
    long des = packet.des;
    int busy_flag = 0;
    counter_t current_time = packet.start_time;
    counter_t transfer_time_flag;
    counter_t schedule_time;
    int queue_time = 0, ts_delay = 0, steering_time = 0, meta_delay = 0, tran_type = 0;
    int rec_id = packet.des % data_receivers;

    if(current_time < sim_cycle)
        current_time = sim_cycle;

    busy_flag = channel_busy_check(src, des, packet.channel, sim_cycle);

    schedule_time = time_schedule(packet, current_time, busy_flag, &tran_type, &meta_delay, &transfer_time_flag, &queue_time, &steering_time, &ts_delay);

    packet.meta_delay = meta_delay;
    packet.transfer_time = schedule_time;
    packet.steering_time = steering_time;
    packet.transmitter_time = transfer_time_flag;
    packet.queue_time = queue_time;
    packet.ts_delay = ts_delay;
    if(packet.conf_bit != -1)
    {
        /* allocate one bit position in confirmation laser successfully, and sending the request through confirmation laser */
        if(packet.operation == WRITE_BACK_REPLY)
            total_wbreq_confirmation ++;
        else if(packet.operation == MEM_PACK_REPLY)
            total_memreq_confirmation ++;
        add_message(schedule_time + laser_setup_time + confirmation_time, ACK_MSG, packet);
        if(conf_laser_power[packet.channel][src] == LASER_OFF)
            conf_laser_power_start_cycle[packet.channel][src] = sim_cycle + laser_setup_time;
        conf_laser_power_on_cycle[packet.channel][src] = sim_cycle + laser_setup_time + confirmation_time;
        if(tran_type == 0)
        {
            delay_counter[packet.channel][src] = schedule_time;
            last_packet_des[src] = des;
            laser_power_on_cycle[packet.channel][src] = delay_counter[packet.channel][src];
        }
    }
    else
    {
        /* Sending the request through the normal meta packets */
        if(packet.operation == WRITE_BACK_REPLY)
            total_wbreq_convention ++;
        else
            total_memreq_convention ++;
        int flitNum = packet.packet_size;
        while(flitNum != 0)
        {
            /* schedule the data packet tranporting time */
            flitNum--;
            packet.flitnum = flitNum;
            add_message(schedule_time - flitNum*flit_tranfer_time, META_MSG, packet);
        }
        //add_message(schedule_time, META_MSG, packet);
        /* recording the infront packet destination for the laser_setup_time*/
        if(tran_type == 0)
        {
            delay_counter[packet.channel][src] = schedule_time;
            last_packet_des[src] = des;
            laser_power_on_cycle[packet.channel][src] = delay_counter[packet.channel][src];
        }

        /* Insert flit into transmitter buffer */
        packet.flitnum = meta_packet_size;
        input_buffer_add(packet, schedule_time + laser_setup_time + confirmation_time);
    }

}
void receive_wb_msg(Packet packet, counter_t sim_cycle)
{
    /* first receive the packet from the processor, adding packet into its sending flit list */
    long size = packet.packet_size;
    int p_type = packet.packet_type;
    long src = packet.src;
    long des = packet.des;
    long i;
    int busy_flag = 0, tran_type = 0;
    int match = 0, meta_delay = 0;
    counter_t current_time = packet.start_time;
    counter_t transfer_time_flag;
    counter_t packet_transfer_time, schedule_time;
    int queue_time = 0, ts_delay = 0, steering_time = 0;
    packet_transfer_time = size*data_flit_tranfer_time;

    if(current_time < sim_cycle)
        current_time = sim_cycle;

    busy_flag = channel_busy_check(src, des, 0, sim_cycle); /* FIXME: the 0 here means the head packets use the meta transmitter */

    if(packet.retry == 0)
        match = is_appending_addr(packet);
    else
        match = 0;

    /* firt check if this packet has the same addr with the appending packet, if does, not transfer this packet until appending packet is clear*/
    if(!match)
    {
        /* sending the wb header */
        if(packet.operation == WRITE_BACK)
            total_wbhead_msg ++;
        else
            total_memhead_msg ++;
        Packet head_packet;
        head_packet.des = des;
        head_packet.src = src;
        if(packet.operation == WAIT_MEM_READ_N)
            head_packet.operation = MEM_PACK_HEAD;
        else
            head_packet.operation = WRITE_BACK_HEAD;
        head_packet.packet_type = META;
        head_packet.packet_size = meta_packet_size;
        head_packet.flitnum = 0;
        head_packet.msgno = packet.msgno;
        head_packet.start_time = packet.start_time;
        meta_packets ++;
        meta_packet_transfer ++;
        head_packet.addr = packet.addr;
        head_packet.retry = 0;
        head_packet.retry_num = 0;
        head_packet.transfer_time = 0;
        head_packet.transmitter_time = 0;
        head_packet.meta_delay = 0;
        head_packet.prefetch = packet.prefetch;
        head_packet.arrival_time = packet.arrival_time;
        head_packet.queue_time = 0;
        head_packet.steering_time = 0;
        head_packet.ts_delay = 0;
        head_packet.dirty_flag = packet.dirty_flag;
        head_packet.data_reply = packet.data_reply;
        head_packet.data_delay = packet.data_delay;
        head_packet.miss_flag = packet.miss_flag;

        for(i=0; i<PENDING_SIZE; i++)
            head_packet.pending[i] = 0;

        for (i=0; i<4; i++)
            head_packet.con_flag[i] = 0;
        head_packet.meta_id = receiver_map(src, des, meta_receivers, packet.addr);
        head_packet.data_id = 0;
        head_packet.flitnum = meta_packet_size - 1;
        int random = rand()%10;
        head_packet.channel = 0;

        /* allocate one bit position in the confirmation laser */
        int j=0, bit_location = -1;
        for (j = 0; j < CONF_WIDTH; j++)
        {
            if((confirmation_bit_inf[src] & (1<<j)) != (1<<j))
            {
                bit_location = j;
                break;
            }
        }
        if(bit_location != -1)
        {
            head_packet.conf_bit = bit_location;
            packet.conf_bit = bit_location;
            confirmation_bit_inf[src] |= 1<<bit_location;
            bit_used ++;
        }
        else
        {
            head_packet.conf_bit = -1;
            packet.conf_bit = -1;
            bit_overflow ++;
        }

        schedule_time = time_schedule(head_packet, current_time, busy_flag, &tran_type, &meta_delay, &transfer_time_flag, &queue_time, &steering_time, &ts_delay);

        if(packet.retry == 1)
            packet.flitnum = size;
        input_buffer_add(packet, schedule_time + laser_setup_time + confirmation_time);

        int flitNum = meta_packet_size;
        while(flitNum != 0)
        {
            /* schedule the data packet tranporting time */
            flitNum--;
            head_packet.flitnum = flitNum;
            head_packet.transfer_time = schedule_time;
            head_packet.meta_delay = meta_delay;
            head_packet.transmitter_time = transfer_time_flag;
            add_message(schedule_time - flitNum*flit_tranfer_time, META_MSG, head_packet);
        }


        /* recording the infront packet destination for the laser_setup_time*/
        if(tran_type == 0)
        {
            delay_counter[META][src] = schedule_time;
            last_packet_des[src] = des;
            laser_power_on_cycle[META][src] = delay_counter[META][src];
        }
    }
    else
    {
        input_buffer_add(packet, WAIT_TIME);
    }

}
#endif

/* Laser warm up and set up delay */
int setup_delay(int src, int des, counter_t sim_cycle, int p_type)
{
    int delay = 0;
    if(laser_power[p_type][src] != LASER_ON)
    {
        delay += laser_warmup_time;
    }

    packets_sending[src] ++;
    return delay;

}
int laser_steering(int src, int des, counter_t sim_cycle)
{
    int delay = 0;
    if(last_packet_des[src] == des)
        delay += 0;
    else
    {
        /* direction switch --- laser setup time*/
        laser_direction_switch_counter ++;
        //delay += laser_setup_time;
    }

    return delay;

}

/* channel busy check */
int channel_busy_check(int src, int des, int type, counter_t sim_cycle)
{
    /* check if there is a data packet under transporting at this channel */

#ifdef IDEALCASEn
    if(delay_count[type][src][des] < sim_cycle)
        return 0;
    else
        return 1;
#else
    if(delay_counter[type][src] < sim_cycle)
        return 0;
    else
        return 1;
#endif
}

/* handing DATA_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_data(Packet pck, counter_t sim_cycle)
{
    Flit_list *transmitter;

    if(pck.channel == 0)
        transmitter = transmitter_meta;
    else
        transmitter = transmitter_data;
    if(pck.flitnum == pck.packet_size - 1)
    {
        Packet packet = pck;
        int src = packet.src;
        int size = packet.packet_size;
        int i = 0;

        for(i=0; i<size; i++)
        {
            Flit *flit;
            if(currentFlits >= maxFlits)
                panic("Out of total flits\n");
            flit = allocate_flit();
            currentFlits++;
            if(i == (size-1))
            {
                /* packet tail */
                flit->flit_type = TAIL;
                flit->next = NULL;
                flit->Num = packet.msgno;
                flit->PID = PID_form(src);
                add_flit(flit, &transmitter[src]);
            }
            else if(i == 0)
            {
                /* packet header */
                flit->flit_type = HEADER;
                flit->next = NULL;
                flit->Num = packet.msgno;
                flit->PID = PID_form(src);
                /* before adding any flit to flit list at sender, check if there is packet under transporting */
                add_flit(flit, &transmitter[src]);
            }
            else
            {
                /* packet body */
                flit->next = NULL;
                flit->flit_type = BODY;
                flit->Num = packet.msgno;
                flit->PID = PID_form(src);
                add_flit(flit, &transmitter[src]);
            }
        }
    }
#ifdef IDEALCASE
    if(pck.flitnum == 0)
        sending_flit(pck, sim_cycle);
#else
    /*check if it is to the same cache line with packet in the wait_queue */
    sending_flit(pck, sim_cycle);
#endif
}

/* handing META_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_meta(Packet pck, counter_t sim_cycle)
{
    Flit_list *transmitter;

    if(pck.channel == 0)
        transmitter = transmitter_meta;
    else
        transmitter = transmitter_data;
    if(pck.flitnum == pck.packet_size - 1)
    {
        Packet packet = pck;
        int src = packet.src;
        int size = packet.packet_size;
        int i = 0;

        for(i=0; i<size; i++)
        {
            Flit *flit;
            if(currentFlits >= maxFlits)
                panic("Out of total flits\n");
            flit = allocate_flit();
            currentFlits++;
            if(i == (size-1))
            {
                /* packet tail */
                flit->flit_type = TAIL;
                flit->next = NULL;
                flit->Num = packet.msgno;
                flit->PID = PID_form(src);
                add_flit(flit, &transmitter[src]);
            }
            else if(i == 0)
            {
                /* packet header */
                flit->flit_type = HEADER;
                flit->next = NULL;
                flit->Num = packet.msgno;
                flit->PID = PID_form(src);
                /* before adding any flit to flit list at sender, check if there is packet under transporting */
                add_flit(flit, &transmitter[src]);
            }
            else
            {
                /* packet body */
                flit->next = NULL;
                flit->flit_type = BODY;
                flit->Num = packet.msgno;
                flit->PID = PID_form(src);
                add_flit(flit, &transmitter[src]);
            }
        }
    }
#ifdef IDEALCASE
    if(pck.flitnum == 0)
        sending_flit(pck, sim_cycle);
#else
    sending_flit(pck, sim_cycle);
#endif
}

/* handling RECIVEING FLIT MSG */
void receive_flit_msg(Packet pck, counter_t sim_cycle)
{
    receive_flit(pck, sim_cycle);
}

//#ifdef CONF_RES_ACK
/* Handling confirmation message */
void receive_ack_msg(Packet pck, counter_t sim_cycle)
{
    int i=0;
    input_buffer_free(pck, sim_cycle);
    /* receive the confirmation bit and take it as the acknowledgement to invalidation */
    if(!( 0
#ifdef SERIALIZATION_ACK
            || ((pck.operation == FINAL_INV_ACK && pck.conf_bit == 1) || pck.operation == ACK_DIR_WRITEUPDATE)
#endif
#ifdef SYNC_LOCK_OPT
            || ((pck.operation == ACK_DIR_READ_EXCLUSIVE || pck.operation == ACK_DIR_READ_SHARED || pck.operation == INV_MSG_UPDATE) && pck.SyncLock)
#endif
#ifdef PREFETCHER_OPT
            || (pck.operation == MISS_READ && pck.prefetch == 4)
#endif
#ifdef LOCK_HARD
            || pck.operation == ACQ_ACK || pck.operation == WAIT_ACK
#endif
#ifdef WB_SPLIT_NET
            || pck.operation == WRITE_BACK_REPLY || pck.operation == WRITE_BACK_HEAD
            || pck.operation == MEM_PACK_REPLY || pck.operation == MEM_PACK_HEAD
#endif
#ifdef FETCH_DEADLOCK
            || pck.escape_channel
#endif
#ifdef EXCLUSIVE_OPT
            || (pck.operation == ACK_MSG_READUPDATE && !pck.exclusive_after_wb)
#endif
        ))
        input_buffer_num[pck.src][pck.channel] = input_buffer_num[pck.src][pck.channel] - pck.packet_size;


    if(0
#ifdef PREFETCHER_OPT
            || (pck.operation == MISS_READ && pck.prefetch == 4)
#else
#ifdef LOCK_HARD
            || (pck.operation == ACQ_ACK || pck.operation == WAIT_ACK)
#endif
#endif
#ifdef SYNC_LOCK_OPT
            || ((pck.operation == ACK_DIR_READ_EXCLUSIVE || pck.operation == ACK_DIR_READ_SHARED || pck.operation == INV_MSG_UPDATE) && pck.SyncLock)
#endif
#ifdef EXCLUSIVE_OPT
            || (pck.operation == ACK_MSG_READUPDATE && !pck.exclusive_after_wb)
#endif
      )
    {
        for(i=0; i<pck.packet_size; i++)
        {
            currentFlits++;
            Flit *new_flit;
            if(currentFlits >= maxFlits)
                panic("Out of total flits\n");
            new_flit = allocate_flit();
            if(i == (pck.packet_size-1))
            {
                /* packet tail */
                new_flit->flit_type = TAIL;
                new_flit->next = NULL;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.src);
                add_flit(new_flit, &out_buffer[pck.des]);
            }
            else if(i == 0)
            {
                /* packet header */
                new_flit->flit_type = HEADER;
                new_flit->next = NULL;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.src);
                /* before adding any flit to flit list at sender, check if there is packet under transporting */
                add_flit(new_flit, &out_buffer[pck.des]);
            }
            else
            {
                /* packet body */
                new_flit->next = NULL;
                new_flit->flit_type = BODY;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.src);
                add_flit(new_flit, &out_buffer[pck.des]);
            }
        }

        add_packet(pck, pck.des);
    }
#ifdef SERIALIZATION_ACK
    if((pck.operation == FINAL_INV_ACK && pck.conf_bit == 1) || pck.operation == ACK_DIR_WRITEUPDATE)
    {
        for(i=0; i<pck.packet_size; i++)
        {
            currentFlits++;
            Flit *new_flit;
            if(currentFlits >= maxFlits)
                panic("Out of total flits\n");
            new_flit = allocate_flit();
            if(i == (pck.packet_size-1))
            {
                /* packet tail */
                new_flit->flit_type = TAIL;
                new_flit->next = NULL;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.src);
                add_flit(new_flit, &out_buffer[pck.des]);
            }
            else if(i == 0)
            {
                /* packet header */
                new_flit->flit_type = HEADER;
                new_flit->next = NULL;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.src);
                /* before adding any flit to flit list at sender, check if there is packet under transporting */
                add_flit(new_flit, &out_buffer[pck.des]);
            }
            else
            {
                /* packet body */
                new_flit->next = NULL;
                new_flit->flit_type = BODY;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.src);
                add_flit(new_flit, &out_buffer[pck.des]);
            }
        }

        add_packet(pck, pck.des);
    }
#endif
#ifdef INV_ACK_CON
#ifdef COHERENCE_MODIFIED
    if((pck.operation == INV_MSG_WRITE) && (pck.data_reply == 0))
#else
    if((pck.operation == INV_MSG_READ || pck.operation == INV_MSG_WRITE) && (pck.data_reply == 0))
#endif
    {
        /* successfully received the invalidation */
        pck.msgno --;
        for(i=0; i<pck.packet_size; i++)
        {
            currentFlits++;
            Flit *new_flit;
            if(currentFlits >= maxFlits)
                panic("Out of total flits\n");
            new_flit = allocate_flit();
            if(i == (pck.packet_size-1))
            {
                /* packet tail */
                new_flit->flit_type = TAIL;
                new_flit->next = NULL;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.des);
                add_flit(new_flit, &out_buffer[pck.src]);
            }
            else if(i == 0)
            {
                /* packet header */
                new_flit->flit_type = HEADER;
                new_flit->next = NULL;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.des);
                /* before adding any flit to flit list at sender, check if there is packet under transporting */
                add_flit(new_flit, &out_buffer[pck.src]);
            }
            else
            {
                /* packet body */
                new_flit->next = NULL;
                new_flit->flit_type = BODY;
                new_flit->Num = pck.msgno;
                new_flit->PID = PID_form(pck.des);
                add_flit(new_flit, &out_buffer[pck.src]);
            }
        }

        int temp;
        temp = pck.des;
        pck.des = pck.src;
        pck.src = temp;
        if(pck.operation == INV_MSG_WRITE)
            pck.operation = ACK_MSG_WRITEUPDATE;
        else if(pck.operation == INV_MSG_READ)
            pck.operation = ACK_MSG_READUPDATE;
        add_packet(pck, pck.des);
    }
#endif
#ifdef WB_SPLIT_NET
    if(pck.operation == WRITE_BACK_REPLY && pck.retry_num == 0 && pck.conf_bit == -1)
        total_wb_req_convention_success ++;

    if(pck.operation == WRITE_BACK_REPLY && pck.conf_bit != -1)
    {
        pck.operation = WRITE_BACK;
        add_message(sim_cycle, WB_REQ_MSG, pck);
        confirmation_bit_inf[pck.des] &= ~(1<<pck.conf_bit);
    }
#ifdef MEM_SPLIT_NET
    if(pck.operation == MEM_PACK_REPLY && pck.retry_num == 0 && pck.conf_bit == -1)
        total_mem_req_convention_success ++;
    if(pck.operation == MEM_PACK_REPLY && pck.conf_bit != -1)
    {
        pck.operation = WAIT_MEM_READ_N;
        add_message(sim_cycle, WB_REQ_MSG, pck);
        confirmation_bit_inf[pck.des] &= ~(1<<pck.conf_bit);
    }
#endif
#endif
}
//#endif

#ifdef WB_SPLIT_NET
void input_buffer_time_stamp_adjust(int src, int msgno, int delay)
{
    int i;
    for (i = 0; i < input_buffer_size; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.msgno == msgno)
        {
            input_buffer[src].InputEntry[i].time_stamp += delay;
            input_buffer[src].InputEntry[i].packet.queue_time += data_packet_size*data_flit_tranfer_time;
            break;
        }
    }
}
int wbschedule(Packet pck, counter_t sim_cycle)
{
    Packet packet;
    Msg *current_msg;
    current_msg = msg_list->head;
    int delay = data_packet_size*data_flit_tranfer_time;
    int flag = 0;

    while(current_msg != NULL)
    {
        if (current_msg->msg_type != DATA_MSG || current_msg->packet.packet_type == META)
        {
            /* check the message is the ready to operate or not */
            current_msg = current_msg->next;
            continue;
        }
        packet = current_msg->packet;
        if(current_msg->when >= sim_cycle && packet.src == pck.src && packet.msgno != pck.msgno)
        {
            flag = 1;
            current_msg->when = current_msg->when + delay;
            current_msg->packet.transfer_time += delay;
            current_msg->packet.queue_time += delay;
            input_buffer_time_stamp_adjust(packet.src, packet.msgno, delay);
        }
        current_msg = current_msg->next;
    }
    return flag;
}

#endif

/* *****************************************************************************/

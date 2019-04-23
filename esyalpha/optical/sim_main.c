/* SIM_MAIN FUNCTION */

#include "point-point.h"

/* ******************************** SIM_MAIN.C *********************************/
void laser_switch(counter_t sim_cycle);
void conf_laser_switch(counter_t sim_cycle);

#ifdef CONF_PREDICTOR
void ReqPendingQueueInsert(int src, int des, md_addr_t addr, counter_t start_time, int op_type);
void ReqPendingQueueRemove(int src, int des, md_addr_t addr, counter_t sim_cycle);
#endif

/* first receive the packet from the processor, add EVG_MSG into message list */
void directMsgInsert(long s1, long s2, long d1, long d2, counter_t sim_cycle, long size, long long int msgNo, md_addr_t addr, int operation, int delay, int L2miss_flag, int prefetch, int dirty_flag, counter_t arrival_time, int data_reply, int conf_bit, int escape_channel, int even_flag, int isSyncAccess, int SyncLock
#ifdef EXCLUSIVE_OPT
                     , int exclusive_after_wb
#endif
                    )
{
    /* First time receiver the data from the processor, insert the message to central message list*/
    long src, des;
    Packet packet;
    int i;

    src = s1*(mesh_size+2) + s2;
    des = d1*(mesh_size+2) + d2;

    packet.start_time = sim_cycle;
    packet.src = src;
    packet.des = des;
    packet.msgno = msgNo;
    packet.packet_size = size;
    packet.addr = addr;
    packet.retry = 0;
    packet.retry_num = 0;
    packet.operation = operation;
    packet.isSyncAccess = isSyncAccess;
    packet.ldl = even_flag;
    if(msgNo == 1)
        printf("processor number is %d\n", PROCESSOR);

    packet.transfer_time = 0;
    packet.transmitter_time = 0;
    packet.meta_delay = 0;
    packet.prefetch = prefetch;
    packet.arrival_time = arrival_time;
    packet.queue_time = 0;
    packet.steering_time = 0;
    packet.ts_delay = 0;
    packet.data_delay = delay;
    packet.dirty_flag = dirty_flag;
    packet.data_reply = data_reply;
    packet.miss_flag = L2miss_flag;
    packet.escape_channel = escape_channel;

    packet.conf_bit = conf_bit;

    packet.conf_hint = 0;
    packet.early_conf = 0;
    packet.schedule = 0;
    packet.SyncLock = SyncLock;
#ifdef EXCLUSIVE_OPT
    packet.exclusive_after_wb = exclusive_after_wb;
#endif

    for(i=0; i<PENDING_SIZE; i++)
        packet.pending[i] = 0;

    for (i=0; i<4; i++)
        packet.con_flag[i] = 0;

    if (size <= meta_packet_size)
    {
        /* meta packet */
        packet.packet_type = META;
        /* how many cycles is this packet going to take to finish tranfering */
        packet.meta_id = receiver_map(src, des, meta_receivers, addr);
        packet.data_id = 0;
        packet.flitnum = meta_packet_size;
        packet.channel = 0;
        meta_packets ++;
    }
    else
    {
        /* data packet */
        packet.packet_type = DATA;
        /* how many cycles is this packet going to take to finish tranfering */
        packet.data_id = receiver_map(src, des, data_receivers, addr);
        packet.meta_id = 0;
        packet.flitnum = data_packet_size;
        data_packets ++;
        packet.channel = 1;
    }

#ifdef LOCK_HARD
    if((operation == ACQ_ACK || operation == WAIT_ACK))
    {
        add_message(sim_cycle+laser_setup_time, ACK_MSG, packet);
        return 0;
    }
#endif
#ifdef EXCLUSIVE_OPT
    if(operation == ACK_MSG_READUPDATE && !exclusive_after_wb)
    {
        add_message(sim_cycle+laser_setup_time, ACK_MSG, packet);
        return 0;
    }
#endif
#ifdef SYNC_LOCK_OPT
    if((operation == ACK_DIR_READ_SHARED || operation == ACK_DIR_READ_EXCLUSIVE || operation == INV_MSG_UPDATE) && (SyncLock))
    {
        if(operation == INV_MSG_UPDATE)
        {
            int serialize_time = laser_setup_time;
            serialize_time = laser_setup_time + conf_bit*(laser_setup_time+1);
            add_message(sim_cycle+serialize_time, ACK_MSG, packet);
        }
        else
            add_message(sim_cycle+laser_setup_time, ACK_MSG, packet);
        return 0;
    }
#endif
#ifdef PREFETCHER_OPT
    if((operation == MISS_READ) && prefetch == 4)
    {
        add_message(sim_cycle+5+laser_setup_time, ACK_MSG, packet);
        return 0;
    }
#endif
#ifdef SERIALIZATION_ACK
    if((operation == FINAL_INV_ACK && conf_bit == 1) || operation == ACK_DIR_WRITEUPDATE)
    {
        add_message(sim_cycle+laser_setup_time, ACK_MSG, packet);
        return 0;
    }
#endif

#ifdef WB_SPLIT_NET
#ifdef MEM_SPLIT_NET
    if(operation == WRITE_BACK || operation == WAIT_MEM_READ_N)
#else
    if(operation == WRITE_BACK)
#endif
        add_message(sim_cycle, WB_MSG, packet);
    else
        add_message(sim_cycle, EVG_MSG, packet);
#else
    if(operation == WRITE_BACK)
        total_wb_msg ++;
    add_message(sim_cycle, EVG_MSG, packet);
#endif
#ifndef MEM_SPLIT_NET
    if(operation == WAIT_MEM_READ_N)
        total_mem_msg ++;
#endif
#ifdef FETCH_DEADLOCK
    if(!escape_channel)
#endif
        input_buffer_num[src][packet.channel] += size;
    if(packet.packet_type == DATA && laser_power[packet.packet_type][src] == LASER_OFF)
    {
        laser_power_start_cycle[packet.packet_type][src] = sim_cycle;
        laser_power_on_cycle[packet.packet_type][src] = sim_cycle;
        laser_power[packet.packet_type][src] = LASER_ON;
        laser_power_switchon_counter ++;
    }
}


/* call this function at every cycle, check it message list to operate message */
void directRunSim(counter_t sim_cycle)
{
    counter_t cur_time;
    Packet packet;
    Msg *current_msg;
    Msg *pre_msg;
    current_msg = msg_list->head;
    int occupancy_flag = 0;
    int occupancy_data_bincount = 0;
    int occupancy_meta_bincount = 0;
    int i = 0;

    int req_cycle_conflicts_flag[4] = {0};
    int data_cycle_conflicts_flag[4] = {0};
#ifdef CORONA
    for(i=0; i<PROCESSOR; i++)
        if(arbi_delay[i])
            arbi_delay[i] --;
#endif

#ifdef CONF_PREDICTOR
    ConfPredictor(sim_cycle);
    ConfPacketListClean(sim_cycle);
#endif

#ifdef CONF_RES_ACK
    /* check the input buffer at each cycle to reinsert the conflict packet */
    for(i=0; i<PROCESSOR; i++)
    {
        if(input_buffer[i].flits_in_buffer != 0)
            input_buffer_lookup(i, sim_cycle);
    }
#endif
    /* conflict bincount collection function */
    while(current_msg != NULL)
    {
        Msg *msg;
        msg = current_msg;
        cur_time = msg->when;

        if (cur_time > sim_cycle)
        {
            /* check the message is the ready to operate or not */
            current_msg = current_msg->next;
            continue;
        }
        packet = msg->packet;
        switch(msg->msg_type)
        {
#ifdef WB_SPLIT_NET
        case WB_MSG:
        {
            /* receive the write back packet from the processor */
            if(packet.operation == WRITE_BACK)
                total_wb_msg ++;
            else
                total_mem_msg ++;
            receive_wb_msg(packet, sim_cycle);
            break;
        }
        case WB_REQ_MSG:
        {
            /* receive the write back request */
            if(packet.operation == WRITE_BACK || packet.operation == WAIT_MEM_READ_N)
            {
                receive_wb_reply_msg(packet, sim_cycle);
#ifdef WB_PENDING
#ifdef CONF_PREDICTOR
                ReqPendingQueueRemove(packet.src, packet.des, packet.addr, sim_cycle);
#endif
#endif
            }
            else
            {
                receive_wb_req_msg(packet, sim_cycle);
#ifdef WB_PENDING
#ifdef CONF_PREDICTOR
                ReqPendingQueueInsert(packet.src, packet.des, packet.addr, sim_cycle, packet.operation);
#endif
#endif
            }
            break;
        }
#endif
        case EVG_MSG:
        {
            /* receive the packet from the processor */
            receive_msg(packet, sim_cycle);
            break;
        }
        case META_MSG:
        {
            /* receive the meta packet at the receiver */
            receive_msg_meta(packet, sim_cycle);
            break;
        }
        case DATA_MSG:
        {
            /* receive the data packet at the receiver */
            if(packet.flitnum == 4 && packet.retry_num == 0)
            {
                data_packet_generate ++;
                global_data_packets_generation ++;
            }
            receive_msg_data(packet, sim_cycle);
            break;
        }
        case WIRE_MSG:
        {
            receive_flit_msg(packet, sim_cycle);
            break;
        }
        case ACK_MSG:
        {
            receive_ack_msg(packet, sim_cycle);
            break;
        }
#ifdef CONF_PREDICTOR
        case CONF_HINT:
        {
            /*FIXME */
            conf_hint(packet, sim_cycle);
            break;
        }
        case EARLY_ACK_MSG:
        {
            receive_early_ack_msg(packet, sim_cycle);
            break;
        }
#endif
#ifdef EMC
        case EMC_ACK_MSG:
            receive_emc_ack_msg(packet, sim_cycle);
            break;
#endif

        }
        current_msg = current_msg->next;
        /* after finish this message, free it from the message list */
        free_message(msg);
    }

    /* laser switches power statistics */

    laser_switch(sim_cycle);
    conf_laser_switch(sim_cycle);
#ifdef EMC
    MetaEarlyConf(sim_cycle);
    MetaConfPacketListClean(sim_cycle);
#endif

#ifdef CONF_RES_ACK
    /* check input buffer and if there are some entry are not receiving confirmation bit then retry the packet */
    conlict_bincount_collect(sim_cycle);
    if(meta_conf_event)
    {
        meta_conflict_pro ++;
        switch(meta_conf_event)
        {
        case 1:
            req_conf[0] ++;
            break;
        case 2:
            req_conf[1] ++;
            break;
        case 3:
            req_conf[2] ++;
            break;
        default:
            req_conf[3] ++;
            break;
        }
        meta_conf_event = 0;
    }
#endif

    if(sim_cycle%100 == 0)
    {
        if(data_packet_generate > data_packet_generate_max)
            data_packet_generate_max = data_packet_generate;
        data_packet_generate = 0;
    }

    /* output flits from output buffer each cycle */
    int j, m;
    for(i = 0; i<PROCESSOR*output_ports; i++)
    {
        m = packet_queue_num[i];
        for(j=0; j<m; j++)
        {
            int out_buffer_dec = 1;
            packet = packet_queue[i%PROCESSOR][packet_queue_head[i%PROCESSOR]];
#if 0
            if(j>0 && !((packet.operation == ACK_MSG_WRITEUPDATE || packet.operation == ACK_MSG_READUPDATE) && packet.data_reply == 0
#ifdef SERIALIZATION_ACK
                        || (packet.operation == FINAL_INV_ACK && packet.conf_bit == 1) || packet.operation == ACK_DIR_WRITEUPDATE
#endif
                       )
#ifdef FETCH_DEADLOCK
                    && (packet.escape_channel == 0)
#endif
              )
                break;
#endif
#ifndef IDEALCASE
            if(0
#ifdef INV_ACK_CON
#ifdef COHERENCE_MODIFIED
                    || ((packet.operation == ACK_MSG_WRITEUPDATE) && packet.data_reply == 0)
#else
                    || ((packet.operation == ACK_MSG_WRITEUPDATE || packet.operation == ACK_MSG_READUPDATE) && packet.data_reply == 0)
#endif
#endif
                    || packet.escape_channel
#ifdef SERIALIZATION_ACK
                    || (packet.operation == FINAL_INV_ACK && packet.conf_bit == 1) || packet.operation == ACK_DIR_WRITEUPDATE
#endif
#ifdef SYNC_LOCK_OPT
                    || ((packet.operation == ACK_DIR_READ_EXCLUSIVE || packet.operation == ACK_DIR_READ_SHARED || packet.operation == INV_MSG_UPDATE) && packet.SyncLock)
#endif
#ifdef PREFETCHER_OPT
                    || (packet.operation == MISS_READ && packet.prefetch == 4)
#endif
#ifdef LOCK_HARD
                    || (packet.operation == ACQ_ACK || packet.operation == WAIT_ACK)
#endif
#ifdef EXCLUSIVE_OPT
                    || (packet.operation == ACK_MSG_READUPDATE && !packet.exclusive_after_wb)
#endif
              )
                out_buffer_dec = 0;
#endif
            if(packet.msgno != (out_buffer[i%PROCESSOR].head)->Num)
                panic("NETWORK: output flits buffer unmatch with output packet!");

            counter_t request_time = 0;
            int op_type = packet.operation;

            if(finishMsg(packet.src, packet.des, packet.start_time, packet.msgno, packet.packet_type, packet.transmitter_time, packet.meta_delay, request_time, packet.operation, out_buffer_dec))
            {
#ifdef SPLIT_MEM_NEW
                if(op_type == WAIT_MEM_READ && last_meta_packet[packet.data_id][packet.des] < sim_cycle)
                    last_meta_packet[packet.data_id][packet.des] = sim_cycle;
                last_meta_packet_des[packet.data_id][packet.des] = packet.src;
#endif
#ifdef INV_ACK_CON
                if(op_type == ACK_MSG_WRITEUPDATE)
                    conf_ack_packets ++;
                if((packet.operation == ACK_MSG_READUPDATE) && packet.data_reply == 0)
                    conf_ack_packets_read ++;
#endif
#ifdef SERIALIZATION_ACK
                if((op_type == FINAL_INV_ACK && packet.conf_bit == 1) || packet.operation == ACK_DIR_WRITEUPDATE)
                    conf_upgrades ++;
#endif
#ifdef SYNC_LOCK_OPT
                if((op_type == ACK_DIR_READ_EXCLUSIVE || op_type == ACK_DIR_READ_SHARED || op_type == INV_MSG_UPDATE) && packet.SyncLock)
                    conf_sync_lock ++;
#endif
#ifdef EXCLUSIVE_OPT
                if(op_type == ACK_MSG_READUPDATE && !packet.exclusive_after_wb)
                    conf_readupgrades ++;
#endif

#ifdef CONF_PREDICTOR   	/* add the finish request to request pending list for conflict prediction */
                if(op_type == MISS_IL1 || op_type == MISS_READ || op_type == MISS_WRITE || op_type == MEM_READ_REQ
#ifdef ACK_PENDING
#ifdef COHERENCE_MODIFIED
                        || ((op_type == INV_MSG_WRITE) && (packet.data_reply == 1))
#else
                        || ((op_type == INV_MSG_WRITE || op_type == INV_MSG_READ) && (packet.data_reply == 1))
#endif
#endif
#ifdef PREFETCH_OPTI
                        || op_type == L1_PREFETCH || op_type == READ_COHERENCE_UPDATE || op_type == WRITE_COHERENCE_UPDATE
#endif
                  )
                    ReqPendingQueueInsert(packet.src, packet.des, packet.addr, packet.start_time, op_type);
                else if(op_type == ACK_DIR_WRITE || op_type == ACK_DIR_READ_EXCLUSIVE || op_type == ACK_DIR_READ_SHARED || op_type == ACK_DIR_IL1
                        || op_type == ACK_MSG_READ || op_type == ACK_MSG_WRITE || op_type == WAIT_MEM_READ_N
#ifdef PREFETCH_OPTI
                        || op_type == READ_META_REPLY_SHARED || op_type == READ_META_REPLY_EXCLUSIVE || op_type == READ_DATA_REPLY_SHARED || op_type == READ_DATA_REPLY_EXCLUSIVE
                        || op_type == WRITE_META_REPLY || op_type == WRITE_DATA_REPLY
#endif
                       )
                    ReqPendingQueueRemove(packet.src, packet.des, packet.addr, sim_cycle);
#endif

                /* delay statistic collection */
                laser_switch_on_delay += packet.steering_time;
                network_queue_delay += packet.queue_time; /* queue delay */
                timeslot_wait_time += packet.ts_delay;
                meta_timeslot_wait_time += packet.meta_delay;
                laser_switch_on_Delay[packet.src] += packet.steering_time;
                network_queue_Delay[packet.src] += packet.queue_time; /* queue delay */
                timeslot_wait_Delay[packet.src] += packet.ts_delay;
                meta_timeslot_wait_Delay[packet.src] += packet.meta_delay;
                counter_t conflict_time = 0;
                counter_t trans_time = 0;
                counter_t pending_time = 0;
                counter_t data_time = 0;
                counter_t meta_time = 0;
                if(packet.packet_type == DATA)
                {
                    data_time=(sim_cycle-packet.start_time)-(data_packet_size*data_flit_tranfer_time)-(packet.steering_time+packet.queue_time+packet.ts_delay+packet.meta_delay);
                    if(packet.retry == 1)
                    {
                        data_confliction_delay += data_time;
                        data_confliction_Delay[packet.src] += data_time;
                        confliction_delay += data_time;
                        conflict_time = data_time;
                    }
                    network_delay_in_fiber += data_packet_size*data_flit_tranfer_time;
                    trans_time = data_packet_size*data_flit_tranfer_time;
                    if(packet.retry == 2)
                    {
                        pending_delay += data_time;
                        pending_Delay[packet.src] += data_time;
                        pending_time = data_time;
                        if(packet.src == 40)
                            printf("hehehehe\n");
                    }
                }
                else
                {
                    meta_time=(sim_cycle-packet.start_time)-(meta_packet_size*flit_tranfer_time)-(packet.steering_time+packet.queue_time+packet.ts_delay+packet.meta_delay);
                    if(packet.retry == 1)
                    {
                        meta_confliction_delay += meta_time;
                        meta_confliction_Delay[packet.src] += meta_time;
                        confliction_delay += meta_time;
                        conflict_time = meta_time;
                    }
                    network_delay_in_fiber += meta_packet_size*flit_tranfer_time;
                    trans_time = meta_packet_size*flit_tranfer_time;
                    if(packet.retry == 2 && packet.operation != ACK_MSG_WRITEUPDATE)
                    {
                        pending_delay += meta_time;
                        pending_Delay[packet.src] += meta_time;
                        pending_time = meta_time;
                        if(packet.src == 40)
                            printf("hehehe\n");
                    }
                }
                if(packet.operation != ACK_MSG_WRITEUPDATE)
                {
                    output_queue_delay += (sim_cycle - packet.start_time - trans_time) - (packet.steering_time + packet.queue_time + packet.ts_delay + conflict_time + packet.meta_delay + pending_time);
                    output_queue_Delay[packet.src] += (sim_cycle - packet.start_time - trans_time) - (packet.steering_time + packet.queue_time + packet.ts_delay + conflict_time + packet.meta_delay + pending_time);
                }
            }
            else
                break;
        }
    }
    return;
}
int opticalBufferSpace(long des1, long des2, int opt)
{
    if((input_buffer_num[des1*mesh_size+des2][0]+input_buffer_num[des1*mesh_size+des2][1]) > input_buffer_size - data_packet_size + 1)
        return 1;
    else
        return 0;
#if 0
    int channel_full = 0;
    if(input_buffer_num[des1*mesh_size+des2][0] > input_buffer_size - meta_packet_size + 1)
        channel_full = 1;
    if(input_buffer_num[des1*mesh_size+des2][1] > input_buffer_size - data_packet_size + 1)
        channel_full = 1;
    if(channel_full)
        return 1;
    else
        return 0;
#endif
}

void free_out_buffer_flit(int iteration, long des)
{
    int i=0;
    Flit *flit;
    for(i = 0; i<iteration; i++)
    {
        output_buf_access++;

        flit = out_buffer[des].head;
        free_flit(flit, &out_buffer[des]);
    }
}

/* when it receives its packet successfully, call the finishMsg function to wake up the event */
int finishMsg(long src, long des, counter_t start_time, counter_t esynetmsgno, 
int packet_type, counter_t transfer_time, int meta_delay, counter_t req_time, 
int operation, int out_buffer_dec)
{
    long src1, src2, des1, des2;
    int iteration, complete_flag = 0;
    src1 = src/(mesh_size+2);
    src2 = src%(mesh_size+2);
    des1 = des/(mesh_size+2);
    des2 = des%(mesh_size+2);

#ifdef LOCK_HARD
    if(operation >= 100)
    {
        if(LockMsgComplete(src1, src2, des1, des2, esynetmsgno))
            complete_flag = 1;
    }
    else
#endif
    {
        if(MsgComplete(src1, src2, des1, des2, start_time, esynetmsgno, 
transfer_time, meta_delay, req_time))
            complete_flag = 1;
    }
    if(complete_flag)
    {
        if(packet_type == DATA)
            iteration = data_packet_size;
        else
            iteration = meta_packet_size;

        free_out_buffer_flit(iteration, des);
        free_packet(des);
        if(out_buffer_dec)
            output_buffer_flits[des] = output_buffer_flits[des] - iteration;
        return 1;
    }
    return 0;
}

#ifdef CONF_PREDICTOR
void ReqPendingQueueInsert(int src, int des, md_addr_t addr, counter_t start_time, int op_type)
{
    int i = 0, a = 0;
    for(i=0; i<REQ_QUEUE_SIZE; i++)
    {
        if(ReqPendingQueue[src][i].valid == 0)
        {
            ReqPendingQueue[src][i].des = des;
            ReqPendingQueue[src][i].addr = addr;
            ReqPendingQueue[src][i].valid = 1;
            ReqPendingQueue[src][i].time = start_time;
            ReqPendingQueue[src][i].op_code = op_type;
            ReqPendingQueue_num[src] ++;
            if(ReqPendingQueue_num[src] > MaxReqPending)
                MaxReqPending = ReqPendingQueue_num[src];
            return 0;
        }
    }
}

void ReqPendingQueueClean(int des, int i)
{
    ReqPendingQueue[des][i].valid = 0;
    ReqPendingQueue[des][i].des = -1;
    ReqPendingQueue[des][i].addr = 0;
    ReqPendingQueue[des][i].time = 0;
    ReqPendingQueue[des][i].op_code = 0;
    ReqPendingQueue_num[des] --;
    return;
}

void ReqPendingQueueRemove(int src, int des, md_addr_t addr, counter_t sim_cycle)
{
    int i=0;
    for(i=0; i<REQ_QUEUE_SIZE; i++)
    {
#ifdef ACK_PENDING
        if(ReqPendingQueue[des][i].op_code==INV_MSG_READ || ReqPendingQueue[des][i].op_code==INV_MSG_WRITE)
        {
            if(ReqPendingQueue[des][i].valid && sim_cycle-ReqPendingQueue[des][i].time > 100)
                ReqPendingQueueClean(des, i);
        }
#endif
#ifdef WB_PENDING
#ifdef WB_SPLIT_NET
        if(ReqPendingQueue[des][i].op_code==WRITE_BACK_REPLY || ReqPendingQueue[des][i].op_code==MEM_PACK_REPLY)
        {
            if(ReqPendingQueue[des][i].valid && sim_cycle-ReqPendingQueue[des][i].time > 100)
                ReqPendingQueueClean(des, i);
        }
#endif
#endif
        if(ReqPendingQueue[des][i].valid && ReqPendingQueue[des][i].des == src && ReqPendingQueue[des][i].addr == addr)
        {
            ReqPendingQueueClean(des, i);
            return 0;
        }
    }
}
#endif


void laser_switch(counter_t sim_cycle)
{
    int i = 0, j = 0;
    /* Laser switch on/off */
    for(i = 0; i<PROCESSOR; i++)
    {
        for(j=0; j<2; j++)
        {
            if((laser_power[j][i] == LASER_OFF) && (laser_power_start_cycle[j][i] == sim_cycle) && sim_cycle != 0)
            {
                laser_power[j][i] = LASER_ON;
                laser_power_switchon_counter ++;
            }
            if((laser_power[j][i] == LASER_ON) && ((sim_cycle - laser_power_on_cycle[j][i]) > laser_switchoff_timeout))
            {
                /* turn off the laser (VCSEL) */
                laser_power[j][i] = LASER_OFF;
                laser_power_switchoff_counter ++;

                /* data packets sending during laser switch on and off */
                last_packet_time[i] = 0;
                first_packet_time[i] = 0;
            }
            if(laser_power[j][i] == LASER_ON)
                total_cycle_laser_power_on ++;
        }
    }
}
void conf_laser_switch(counter_t sim_cycle)
{
    int i = 0, j = 0;
    /* Laser switch on/off */
    for(i = 0; i<PROCESSOR; i++)
    {
        for(j=0; j<2; j++)
        {
            if((conf_laser_power[j][i] == LASER_OFF) && (conf_laser_power_start_cycle[j][i] == sim_cycle) && sim_cycle != 0)
            {
                conf_laser_power[j][i] = LASER_ON;
                conf_laser_power_switchon_counter ++;
            }
            if((conf_laser_power[j][i] == LASER_ON) && ((sim_cycle - conf_laser_power_on_cycle[j][i]) > laser_switchoff_timeout))
            {
                /* turn off the laser (VCSEL) */
                conf_laser_power[j][i] = LASER_OFF;
                conf_laser_power_switchoff_counter ++;
            }
            if(conf_laser_power[j][i] == LASER_ON)
                total_cycle_conf_laser_power_on ++;
        }
    }
}
/* *****************************************************************************/

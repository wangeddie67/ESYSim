
#include "point-point.h"
counter_t input_buf_access;
/* *************************** INPUT_BUFFER.C **********************************/
/* insert packets into input buffer */
void input_buffer_add(Packet packet, counter_t time_stamp)
{
    int i;
    long src = packet.src;

    input_buf_access += packet.packet_size;
    //thecontexts[src]->input_buf_access += packet.packet_size;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        if(packet.retry != 0)
        {
            if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.msgno == packet.msgno)
            {
                input_buffer[src].InputEntry[i].packet.retry = packet.retry;
                input_buffer[src].InputEntry[i].time_stamp = time_stamp;
                return;
            }
        }
        else
        {
            if(!input_buffer[src].InputEntry[i].isValid)
            {
                input_buffer[src].InputEntry[i].isValid = 1;
                input_buffer[src].InputEntry[i].packet = packet;
                input_buffer[src].InputEntry[i].time_stamp = time_stamp;

                input_buffer[src].flits_in_buffer += packet.packet_size;
                input_buffer[src].packets_in_buffer ++;
                if(input_buffer[src].flits_in_buffer >= input_buffer_size)
                    input_extra_overflow ++;
                if(max_packets_inbuffer < input_buffer[src].packets_in_buffer)
                    max_packets_inbuffer = input_buffer[src].packets_in_buffer;
                if(max_flits_inbuffer < input_buffer[src].flits_in_buffer)
                    max_flits_inbuffer = input_buffer[src].flits_in_buffer;
                return;
            }
        }
    }
    panic("NETWORK: There is no free entry to insert in input buffer");
}

/* free packet free input buffer */
void input_buffer_free(Packet packet, counter_t sim_cycle)
{
    int i, n, pendingnum, m, Msgno = 0;
    long src = packet.src;

    for (i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid
                && (input_buffer[src].InputEntry[i].packet.msgno == packet.msgno))
        {
            if(input_buffer[src].InputEntry[i].time_stamp < sim_cycle)
                panic("NETWORK: INPUT_BUFFER_FREE: packets should be retry first");
#ifdef WB_SPLIT_NET
            if(packet.operation == WRITE_BACK_HEAD)
            {
                /* receive the confirmation of the header */
                total_wb_head_success ++;
                input_buffer[src].InputEntry[i].time_stamp = WAIT_TIME-1;
                return 0;
            }
#ifdef MEM_SPLIT_NET
            if(packet.operation == MEM_PACK_HEAD)
            {
                /* receive the confirmation of the header */
                total_mem_head_success ++;
                input_buffer[src].InputEntry[i].time_stamp = WAIT_TIME-1;
                return 0;
            }
#endif
#endif
            input_buf_access += packet.packet_size;
            //thecontexts[src]->input_buf_access += packet.packet_size;

            Packet packet = input_buffer[src].InputEntry[i].packet;
#ifdef CONF_PREDICTOR
            if(packet.conf_hint == 1 && packet.retry_num == 1)
                packet_conf_hint_suc ++;
#endif
            input_buffer[src].InputEntry[i].isValid = 0;
            input_buffer[src].flits_in_buffer -= packet.packet_size;
            input_buffer[src].packets_in_buffer --;

            for(n=0; n<PENDING_SIZE; n++)
            {
                pendingnum = input_buffer[src].InputEntry[i].packet.pending[n];
                if(pendingnum)
                {
                    input_buffer[src].InputEntry[i].packet.pending[n] = 0;
                    if(n == 0)
                    {
                        /* retry the first pending packet */
                        retry_packet(pendingnum, sim_cycle, src);
                        Msgno = pendingnum;
                    }
                    else
                    {
                        /* put the rest packet pending to the first packet */
                        if(Msgno == 0)
                            panic("NETWORK: first pending packet msgno should be none zero!");
                        for(m=0; m<BUFFER_SIZE; m++)
                        {
                            if(input_buffer[src].InputEntry[m].isValid && input_buffer[src].InputEntry[m].packet.msgno == Msgno)
                            {
                                if(n == 1 && input_buffer[src].InputEntry[m].packet.pending[n-1] != 0)
                                    panic("NETWORK: pending parent is not the real father!");
                                input_buffer[src].InputEntry[m].packet.pending[n-1] = pendingnum;
                                break;
                            }
                        }
                    }
                }
                else
                    break;
            }
            break;
        }
    }

}

int is_appending_addr(Packet packet)
{
    long src = packet.src;
    int i, match = 0, n;
    for (i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid)
        {
#ifdef ORDER_NODE
            if((input_buffer[src].InputEntry[i].time_stamp != WAIT_TIME) && (input_buffer[src].InputEntry[i].packet.des == packet.des)
                    && (input_buffer[src].InputEntry[i].packet.msgno != packet.msgno))
            {
                if(match != 0)  panic("NETWORK: INPUT_BUFFER: a miss address belongs to more than two INPUT Entries");
                match = 1;
                for(n=0; n<PENDING_SIZE; n++)
                {
                    if(input_buffer[src].InputEntry[i].packet.pending[n] == 0)
                    {
                        pending_packet_num ++;
                        input_buffer[src].InputEntry[i].packet.pending[n] = packet.msgno;
                        break;
                    }
                    else if(n == PENDING_SIZE - 1)
                        panic("NETWORK: pending buffer is full!");
                }
            }
#endif
#ifdef ORDER_ADDR
            if(((input_buffer[src].InputEntry[i].packet.addr >> cache_dl1_set_shift) == (packet.addr >> cache_dl1_set_shift))
                    && (input_buffer[src].InputEntry[i].time_stamp != WAIT_TIME) && (input_buffer[src].InputEntry[i].packet.des == packet.des)
                    && (input_buffer[src].InputEntry[i].packet.msgno != packet.msgno))
            {
                if(match != 0)  panic("NETWORK: INPUT_BUFFER: a miss address belongs to more than two INPUT Entries");
                if(src == 40)
                    printf("hehe\n");
                match = 1;
                for(n=0; n<PENDING_SIZE; n++)
                {
                    if(input_buffer[src].InputEntry[i].packet.pending[n] == 0)
                    {
                        pending_packet_num ++;
                        input_buffer[src].InputEntry[i].packet.pending[n] = packet.msgno;
                        break;
                    }
                    else if(n == PENDING_SIZE - 1)
                        panic("NETWORK: pending buffer is full!");
                }
            }
#endif
        }
    }
    return match;
}

int retry_pending_check(Packet packet)
{
    /* check if there are some previous packets are conflicting over # times, then this packet will be pending until the previous on are resolved*/
    int i;
    int src = packet.src;
    for(i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.des == packet.des
                && input_buffer[src].InputEntry[i].packet.msgno < packet.msgno && input_buffer[src].InputEntry[i].packet.retry_num > 2000)
            return 1;
    }
    return 0;
}


void conflict_bin_count(int src, int des, int data_id, counter_t sim_cycle, int wb_flag, int msgno, Packet packet)
{
    int i=0;

    if((sim_cycle - bin_count_end[des][data_id]) >= data_packet_size*data_flit_tranfer_time)
    {
        data_conf_involves[des][data_id] = 0;
        write_back_involves[des][data_id] = 0;
        bin_count_start[des][data_id] = sim_cycle;
        bin_count_end[des][data_id] = sim_cycle;
        data_conf_involves[des][data_id] ++;
        packet_conflict[des][data_id].packet[0] = packet;
        packet_conflict[des][data_id].packet_num = 1;

        if(wb_flag == 1)
            write_back_involves[des][data_id] ++;
    }
    else
    {
        data_conf_involves[des][data_id] ++;
        bin_count_end[des][data_id] = sim_cycle;
        if(wb_flag == 1)
            write_back_involves[des][data_id] ++;

        if(packet_conflict[des][data_id].packet_num == 0)
            panic("Network: conflict packet can not be none! L215 in inputbuffer.c");

        packet_conflict[des][data_id].packet[packet_conflict[des][data_id].packet_num] = packet;
        packet_conflict[des][data_id].packet_num ++;
    }
}

void conlict_bincount_collect(counter_t sim_cycle)
{
    int i, j, m;
    int wrb_flag = 0;
    for(i=0; i<PROCESSOR; i++)
    {
        for(j=0; j<data_receivers; j++)
        {
            if((sim_cycle - bin_count_end[i][j]) >= data_packet_size*data_flit_tranfer_time)
            {
                if(data_conf_involves[i][j])
                {
                    switch (data_conf_involves[i][j])
                    {
                    case 0:
                        panic("NETWORK: no/one data packet can not conflict with no one!\n");
                    case 1:
                        data_conf_inv_bincount[6] ++;
                        break;
                    case 2:
                        data_conf_inv_bincount[0] ++;
                        break;
                    case 3:
                        data_conf_inv_bincount[1] ++;
                        break;
                    case 4:
                        data_conf_inv_bincount[2] ++;
                        break;
                    case 5:
                        data_conf_inv_bincount[3] ++;
                        break;
                    case 6:
                    case 7:
                    case 8:
                        data_conf_inv_bincount[4] ++;
                        break;
                    default:
                        data_conf_inv_bincount[5] ++;
                        break;
                    }
                    if(data_conf_involves[i][j] > max_conf)
                        max_conf = data_conf_involves[i][j];

                    if(write_back_involves[i][j] == 1)
                        write_back_inv_bincount[0] ++;
                    else if(write_back_involves[i][j] == 2)
                        write_back_inv_bincount[1] ++;
                    else if(write_back_involves[i][j] > 2)
                        write_back_inv_bincount[2] ++;

                    if(write_back_involves[i][j] == data_conf_involves[i][j])
                        all_write_back ++;
                    else if(write_back_involves[i][j] == 0)
                        no_write_back ++;
                    else if(write_back_involves[i][j] < data_conf_involves[i][j])
                    {
                        partial_write_back ++;
                        if((data_conf_involves[i][j] - write_back_involves[i][j]) == 1)
                            no_solution_pwb ++;
                    }
                    int mempck_flag = 0, mempck1_flag;
                    for(m=0; m<data_conf_involves[i][j]; m++)
                    {
                        if(packet_conflict[i][j].packet[m].operation == WAIT_MEM_READ)
                        {
                            mempck_flag = 1;
                            break;
                        }
                        if(packet_conflict[i][j].packet[m].operation == WAIT_MEM_READ_N)
                        {
                            mempck1_flag = 1;
                            break;
                        }
                    }
                    if(mempck_flag)
                        MemPckConfEvent ++;
                    if(mempck1_flag)
                        MemSecPckConfEvent ++;
                    else
                    {

                        for(m=0; m<data_conf_involves[i][j]; m++)
                        {
                            if(packet_conflict[i][j].packet[m].operation == 6)
                            {
                                wrb_flag = 1;
                                break;
                            }
                        }
                        if(wrb_flag)
                            WriteBackConfEvent ++;
                        /* conflict reason statistics */
                        int retry_flag = 0, l1l2_flag = 0;
                        if(!wrb_flag)
                        {
                            for(m=0; m<data_conf_involves[i][j]; m++)
                            {
                                if(packet_conflict[i][j].packet[m].retry == 1)
                                {
                                    retry_conflict ++;
                                    retry_flag = 1;
                                    break;
                                }
                            }
                            if(retry_flag)
                                RetransmissionConfEvent ++;
                            else
                                ReplyConfEvent ++;
                            if(!retry_flag)
                            {
                                int home_flag = 0;
                                for(m=0; m<data_conf_involves[i][j]; m++)
                                {
                                    if(packet_conflict[i][j].packet[m].miss_flag)
                                    {
                                        L2_miss_conflict ++;
                                        if(data_conf_involves[i][j] == 2 && m == 0)
                                            if(packet_conflict[i][j].packet[m+1].miss_flag)
                                                all_L2_miss_conflict ++;
                                        home_flag = 1;
                                        break;
                                    }
                                    else if(packet_conflict[i][j].packet[m].dirty_flag)
                                    {
                                        L2_dirty_conflict ++;
                                        home_flag = 1;
                                        break;
                                    }
                                }

                                if(!home_flag)
                                {

                                    for(m=0; m<data_conf_involves[i][j]; m++)
                                    {
                                        if(packet_conflict[i][j].packet[m].operation == 10 || packet_conflict[i][j].packet[m].operation == 12)
                                        {
                                            L1_L2_conflict ++;
                                            l1l2_flag = 1;
                                            break;
                                        }
                                    }

                                    if(!l1l2_flag)
                                    {
                                        int close_flag = 0;
                                        queue_delay_conflict ++;
                                        for(m=0; m<data_conf_involves[i][j]; m++)
                                        {
                                            if(packet_type_check(packet_conflict[i][j].packet[m], sim_cycle))
                                                close_flag = 1;
                                        }
                                        if(!close_flag)
                                        {
                                            if(data_conf_involves[i][j] == 2)
                                            {
                                                Packet pck1, pck2;
                                                pck1 = packet_conflict[i][j].packet[0];
                                                pck2 = packet_conflict[i][j].packet[1];
                                                if(pck1.arrival_time == pck2.arrival_time)
                                                {
                                                    if((pck1.transfer_time - pck1.ts_delay) > (pck1.transfer_time - data_packet_size*data_flit_tranfer_time)
                                                            && ((pck2.transfer_time - pck2.ts_delay) > (pck2.transfer_time - data_packet_size*data_flit_tranfer_time)))
                                                    {
                                                        if(pck1.steering_time == pck2.steering_time)
                                                        {
                                                            if(abs(pck1.queue_time - pck2.queue_time)>=data_packet_size*data_flit_tranfer_time)
                                                                queue_conflict ++;
                                                            else if(abs(pck1.queue_time - pck2.queue_time + pck1.ts_delay - pck2.ts_delay) >= data_packet_size*data_flit_tranfer_time)
                                                                wrong_conflict ++;
                                                        }
                                                        else
                                                        {
                                                            if(abs(pck1.queue_time - pck2.queue_time)>=data_packet_size*data_flit_tranfer_time)
                                                                queue_conflict ++;
                                                            else if(abs(pck1.steering_time+pck1.queue_time - pck2.steering_time - pck2.queue_time) >= data_packet_size*data_flit_tranfer_time)
                                                                steering_queue_conflict ++;
                                                        }
                                                    }
                                                }
                                                else if(abs(pck1.arrival_time - pck2.arrival_time) >= data_packet_size*data_flit_tranfer_time)
                                                {
                                                    l2_fifo_conflict ++;
                                                }
                                                else
                                                {
                                                    fifo_queue_steering_conflict ++;
                                                }
                                            }
                                        }
                                        else
                                            request_close_conflict ++;
                                    }
                                }
                            }
                        }
                    }
                }
                data_conf_involves[i][j] = 0;
                write_back_involves[i][j] = 0;
                packet_conflict[i][j].packet_num = 0;
            }
        }
    }
}


counter_t retry_algorithms(Packet packet, counter_t sim_cycle);

void input_buffer_lookup(long src, counter_t sim_cycle)
{
    int i = 0;

    for (i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].time_stamp < sim_cycle)
        {
            Packet packet = input_buffer[src].InputEntry[i].packet;
            int src = packet.src;
            int des = packet.des;
            int data_id = packet.data_id;
            counter_t schedule_t;
            /* checking if there are some previous packets are conflicting over # times, then this packet will be pending until the previous one are resolved*/
#ifdef WB_SPLIT_NET
            if(packet.operation == WRITE_BACK && packet.retry == 0)
            {
                /* did not receiver the wb header, then resend the wb packets */
                total_wb_head_fail ++;
                retry_packet(packet.msgno, sim_cycle, src);
                confirmation_bit_inf[packet.src] &= ~(1<<packet.conf_bit);
                return 0;
            }
#ifdef MEM_SPLIT_NET
            if(packet.operation == WAIT_MEM_READ_N && packet.retry == 0)
            {
                /* did not receiver the wb header, then resend the wb packets */
                total_mem_head_fail ++;
                retry_packet(packet.msgno, sim_cycle, src);
                confirmation_bit_inf[packet.src] &= ~(1<<packet.conf_bit);
                return 0;
            }
#endif
#endif

            if(retry_pending_check(packet))
                input_buffer[src].InputEntry[i].time_stamp = sim_cycle + 30; //we can use some other algorisms here
            else
            {
#ifdef PREORITY_PACKETS
                if(packet.retry_num > 3 && packet.prefetch == 2)
                    input_buffer[src].InputEntry[i].time_stamp = sim_cycle + 30; //we can use some other algorisms here
#endif
                /* bin count for data conflict involves */
                if(packet.packet_type == DATA)
                {
                    int wb_flag = 0;
                    if(packet.operation == 6)
                    {
                        write_back_packets ++;
                        if(packet.retry_num == 0)
                            original_write_back_packets ++;
                        wb_flag = 1;
                    }
                    conflict_bin_count(src, des, data_id, sim_cycle, wb_flag, packet.msgno, packet);
                }
                /* bin count end */

                /* statistics collected */
                if(packet.retry != 1)
                {
                    if(packet.packet_type == DATA)
                    {
                        data_conflict_packet_num ++;
                    }
                    else
                    {
                        meta_conflict_packet_num ++;
                    }
                }
                if(packet.packet_type == DATA)
                    data_retry_packet_num ++;
                else
                {
                    meta_retry_packet_num ++;
                }
                if(
#ifdef MEM_MOD
#if (MESH_SIZE == 4)
                    (src == 0 || src == 2 || src == 20 || src == 22)  &&
#endif
#if (MESH_SIZE == 8)
                    (src == 0 || src == 4 || src == 72 || src == 76)  &&
#endif
#endif

                    packet.packet_type == DATA)
                {
                    if(packet.retry != 1)
                        mem_data_conflict ++;
                    mem_data_retry ++;
                }
                if(
#if (MESH_SIZE == 4)
                    (des == 0 || des == 2 || des == 20 || des == 22) &&
#endif
#if (MESH_SIZE == 8)
                    (des == 0 || des == 4 || des == 72 || des == 76) &&
#endif
                    packet.packet_type == META)
                {
                    if(packet.retry != 1)
                        mem_req_conflict ++;
                    mem_req_retry ++;
                }

                input_buffer[src].InputEntry[i].packet.retry = 1;
                input_buffer[src].InputEntry[i].packet.retry_num ++;
                packet.retry_num ++;

                /* retry time out algorisms */
#ifdef CONF_PREDICTOR
                if((packet.packet_type == DATA) && packet.retry_num < 5 && !packet.early_conf)
                {
                    if(packet.conf_hint == 1)
                        schedule_t = sim_cycle + (data_packet_size*data_flit_tranfer_time - 4) + 1;  /* FIXME: 1 is proper? */
#ifdef SEND_IMM
                    else if(packet.conf_hint == 0)
                        schedule_t = sim_cycle + 2*data_packet_size*data_flit_tranfer_time - 4 + 1;
#ifdef PRE_ID
                    else if(packet.conf_hint == 2)
                        schedule_t = sim_cycle + 3*data_packet_size*data_flit_tranfer_time - 4 + 1;
                    else if(packet.conf_hint == 3)
                        schedule_t = sim_cycle + 4*data_packet_size*data_flit_tranfer_time - 4 + 1;
#endif
#endif
                    else
                        schedule_t = retry_algorithms(packet, sim_cycle + (data_packet_size*data_flit_tranfer_time - 3) + 1);
                }
                else if(packet.packet_type == DATA)
                    schedule_t = retry_algorithms(packet, sim_cycle + (data_packet_size*data_flit_tranfer_time - 3) + 1);
                else if(packet.packet_type == META)
                    schedule_t = retry_algorithms(packet, sim_cycle);
                if(packet.conf_hint == 1 && packet.retry_num == 2)
                    packet_conf_hint_conf ++;
#else
                schedule_t = retry_algorithms(packet, sim_cycle);
#endif
                retry_packet(packet.msgno, schedule_t, src);
            }
        }
    }
}
#ifdef EMC
void receive_emc_ack_msg(Packet packet, counter_t sim_cycle)
{
    int i = 0;

    for (i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[packet.src].InputEntry[i].isValid && input_buffer[packet.src].InputEntry[i].packet.msgno == packet.msgno)
        {
            Packet packet1 = input_buffer[packet.src].InputEntry[i].packet;
            if(packet1.src == packet.src && packet1.des == packet.des)
            {
                input_buffer[packet.src].InputEntry[i].time_stamp = sim_cycle + 1;
                return;
            }
        }
    }
}
#endif
#ifdef CONF_PREDICTOR
void input_buffer_check(Packet packet, counter_t sim_cycle)
{
    int i = 0;

    for (i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[packet.src].InputEntry[i].isValid && input_buffer[packet.src].InputEntry[i].packet.msgno == packet.msgno)
        {
            Packet packet1 = input_buffer[packet.src].InputEntry[i].packet;
            if(packet1.src == packet.src && packet1.des == packet.des)
            {
                input_buffer[packet.src].InputEntry[i].time_stamp = sim_cycle + data_packet_size*data_flit_tranfer_time - 2;
                input_buffer[packet.src].InputEntry[i].packet.early_conf = 1;
                return;
            }
        }
    }
}
void conf_hint(Packet packet, counter_t sim_cycle)
{
    int i = 0, flag = 0;

    for (i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[packet.des].InputEntry[i].isValid && input_buffer[packet.des].InputEntry[i].time_stamp == sim_cycle)
        {
            Packet packet1 = input_buffer[packet.des].InputEntry[i].packet;
            if(packet1.des == packet.src && packet1.src == packet.des)
            {
                if(packet.conf_hint == 0)
                    input_buffer[packet.des].InputEntry[i].packet.conf_hint = 1;
#ifdef PRE_ID
                else if(packet.conf_hint == 2)
                    input_buffer[packet.des].InputEntry[i].packet.conf_hint = 2;
                else if(packet.conf_hint == 3)
                    input_buffer[packet.des].InputEntry[i].packet.conf_hint = 3;
#endif
                hint_received ++;
                flag = 1;
            }
            else if(packet1.des != packet.src)
                hint_received_wrong ++;
            if(flag == 1)
                return;
        }
    }
}
void receive_early_ack_msg(Packet pck, counter_t sim_cycle)
{
    input_buffer_check(pck, sim_cycle);
}
#endif
int probability(int slot_num)
{
    int randnum;
    float p0=0.4566, p1=0.3196, p2=0.2237;
    //float p0=0.7194, p1=0.2158, p2=0.0647;
    randnum=rand()%10000;
    float prob = (float)randnum/(float)10000;
    if(slot_num!=3)
    {
        large_retry ++;
        return rand()%(slot_num);
    }
    else
    {
        small_retry ++;
        if(prob<=p0)
            return 0;
        else if(prob<=(p0+p1))
            return 1;
        else
            return 2;
    }
}

counter_t retry_algorithms(Packet packet, counter_t sim_cycle)
{
    int schedule_t, t1, transfer_time, slot_num, mytime;
    if(data_chan_timeslot == 1)
    {
        /* time slot for data channel */
        if(packet.packet_type == DATA)
        {
            /* time slot for retry data packets */
            schedule_t = sim_cycle + laser_setup_time;
            transfer_time = packet.packet_size*data_flit_tranfer_time;
            t1 = (schedule_t/transfer_time)*(transfer_time);
            if(t1<schedule_t)
                schedule_t = t1 + packet.packet_size*data_flit_tranfer_time;

            if(retry_algr == 1)
            {
                /* paper - TBEB */
                mytime = packet_retry_algr(packet);
            }
            else
            {
                /* our exponential backoff algorithms: 0-none backoff; 1-backoff; 2-hybrid with backoff and none backoff ; 3-our TBEB*/
                if((exp_backoff == 0) || (exp_backoff == 2 && packet.retry_num <= 5))
                    slot_num = first_slot_num;
                else if((exp_backoff == 1) || (exp_backoff == 2 && packet.retry_num > 5) || (exp_backoff == 3))
                {
                    if(exp_backoff == 1)
                        slot_num = exponential_2(packet.retry_num-1);
                    else if(exp_backoff == 3)
                    {
                        if(packet.retry_num < 10)
                            slot_num = exponential_2(packet.retry_num-1);
                        else
                            slot_num = exponential_2(10);
                    }
                    else
                        slot_num = exponential_2(packet.retry_num-3);
                }
                //mytime = rand()%(slot_num);
                //mytime = probability(slot_num);
                mytime = slot_num;
            }
            schedule_t = schedule_t - laser_setup_time + mytime*(transfer_time);
        }
        else
        {
            /* time slot for retry meta packets */
            schedule_t = sim_cycle + laser_setup_time;
            transfer_time = packet.packet_size*flit_tranfer_time;
            t1 = (schedule_t/transfer_time)*(transfer_time);
            if(t1<schedule_t)
                schedule_t = t1 + packet.packet_size*flit_tranfer_time;

            if(retry_algr == 1)
            {
                /* paper - TBEB */
                mytime = packet_retry_algr(packet);
            }
            else
            {
                /* our exponential backoff algorithms: 0-none backoff; 1-backoff; 2-hybrid with backoff and none backoff ; 3-our TBEB*/
                if((exp_backoff == 0) || (exp_backoff == 2 && packet.retry_num <= 5))
                    slot_num = first_slot_num;
                else if((exp_backoff == 1) || (exp_backoff == 2 && packet.retry_num > 5) || (exp_backoff == 3))
                {
                    if(exp_backoff == 1)
                        slot_num = exponential_2(packet.retry_num-1);
                    else if(exp_backoff == 3)
                    {
                        if(packet.retry_num < 10)
                            slot_num = exponential_2(packet.retry_num-1);
                        else
                            slot_num = exponential_2(10);
                    }
                    else
                        slot_num = exponential_2(packet.retry_num-3);
                }
                //mytime = rand()%(slot_num);
                mytime = slot_num;
            }
            schedule_t = schedule_t - laser_setup_time + mytime*(transfer_time);
        }
    }
    else
    {
        if(packet.packet_type == DATA)
        {
            if(exp_backoff == 0)
                slot_num = 70;
            else if(exp_backoff == 1)
                slot_num = 10*exponential_2(packet.retry_num-1);

            schedule_t = sim_cycle + rand()%(slot_num);
        }
        else

            schedule_t = sim_cycle + rand()%(PROCESSOR);
    }
    return schedule_t;
}

int packet_type_check(Packet packet, counter_t sim_cycle)
{
    struct DIRECTORY_EVENT *event;
    event = dir_event_queue;
    int des = packet.des;
    int flag = 0;
    counter_t packet_real_start_time;


    if (event!=NULL)
    {
        while (event)
        {
            if(event->src1*(mesh_size+2)+event->src2 == packet.src && 
event->des1*(mesh_size+2)+event->des2 == packet.des && event->startCycle == 
packet.start_time && packet.msgno == event->esynetMsgNo)
            {
                if(event->operation == ACK_MSG_READ || event->operation == ACK_MSG_WRITE || event->operation == ACK_DIR_IL1
                        || event->operation == ACK_DIR_READ_SHARED || event->operation == ACK_DIR_READ_EXCLUSIVE || event->operation == ACK_DIR_WRITE
#ifdef MEM_MOD
                        || event->operation == MEM_READ_REPLY || event->operation == WAIT_MEM_READ || event->operation == WAIT_MEM_READ_N
#endif
                  )
                {
                    reply_data_packets ++;
                    if(packet.retry_num == 0)
                        original_reply_data_packets ++;

                    /* For pre-schedule mechanism, collect some statistics */
                    int i = 0;

                    if(event->operation == ACK_DIR_IL1 || event->operation == ACK_DIR_READ_SHARED ||
                            event->operation == ACK_DIR_READ_EXCLUSIVE || event->operation == ACK_DIR_WRITE || event->operation == WRITE_BACK)
                    {
                        if(sim_cycle != last_data_time[des])
                        {
                            /* No equal means the new start of a new conflict */
                            for(i=0; i<PROCESSOR; i++)
                                RT_time[des][i] = 0;
                        }
                        for(i=0; i<PROCESSOR; i++)
                        {
                            if(RT_time[des][i] && event->transfer_time)
                            {
                                if(abs(event->transfer_time - RT_time[des][i]) < data_packet_size*data_flit_tranfer_time)
                                {
                                    if(event->L2miss_flag == 1)
                                        RTCC_miss ++;
                                    RTCC ++;
                                    RTCC_reduced ++;
                                    if(sim_cycle != last_time[des])
                                        RTCC ++;
                                    last_time[des] = sim_cycle;
                                    flag = 1;
                                    break;
                                }
                                else
                                    RTCC_fail ++;
                            }
                            else if(RT_time[des][i] && !event->transfer_time)
                            {
                                RTCC_half ++;
                            }
                        }
                        for(i=0; i<PROCESSOR; i++)
                        {
                            if(!RT_time[des][i])
                            {
                                RT_time[des][i] = event->transfer_time;
                                break;
                            }
                        }
                        last_data_time[des] = sim_cycle;  /* last_data_time record the start time of the one conflict */
                    }
                }
                //else
                //	panic("NETWORK: data packet should not be other case here!");
                break;
            }
            event = event->next;
        }
    }
    return flag;
}
void retry_packet(int msgno, counter_t sim_cycle, long src)
{
    int i = 0;
    for (i=0; i<BUFFER_SIZE; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.msgno == msgno)
        {
            Packet packet = input_buffer[src].InputEntry[i].packet;

            if(packet.retry == 0)
            {
                packet.retry = 2;
                if(packet.operation == WRITE_BACK)
                    packet.retry = 3;
            }
            else
                packet.retry = 1;
            add_message(sim_cycle, EVG_MSG, packet);
            input_buffer[src].InputEntry[i].time_stamp += WAIT_TIME;
            return;
        }
    }
    panic("NETWORK: can not find the retry packet entry !");
}
/* *****************************************************************************/

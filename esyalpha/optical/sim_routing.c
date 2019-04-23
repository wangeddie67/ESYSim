#include "point-point.h"
/* ***************************** SIM_ROUTING.C *********************************/
/* sending the flits */
void sending_flit(Packet packet, counter_t sim_cycle)
{
    long src, des;
    int meta_id, data_id;
    src = packet.src;
    des = packet.des;
    int p_type = packet.packet_type;

    meta_id = packet.meta_id;
    data_id = packet.data_id;
    Flit_list *receiver, *transmitter;

    /* this is fixed receiver for channel model */
    if(p_type == META)
        receiver = meta_receiver[des];
    else
        receiver = data_receiver[des];

    if(packet.channel == 0)
        transmitter = transmitter_meta;
    else
        transmitter = transmitter_data;


    Flit *flit;

    int i, iteration = 0;
    int conflict_flag = 0;

#ifdef CONF_RES_ACK
    /* Conflict at receiver detection check */
    flit = transmitter[src].head;
    if((p_type == DATA && packet.flitnum == data_packet_size - 1))
    {
        if(receiver[data_id].flits_in_list != 0)
        {
            conflict_flag = 1;
            conflict_type_change(&receiver[data_id], flit->PID);
        }
    }
    else if(p_type == DATA && packet.flitnum != data_packet_size - 1)
    {
        if((receiver[data_id].tail)->flit_type == CONF_TYPE)
            conflict_flag = 1;
    }
    else if(p_type == META && packet.flitnum == meta_packet_size - 1)
    {
        if(receiver[meta_id].flits_in_list != 0)
        {
            conflict_flag = 1;
            conflict_type_change(&receiver[meta_id], flit->PID);
        }
    }
    else if(p_type == META && packet.flitnum != meta_packet_size - 1)
    {
        if((receiver[meta_id].tail)->flit_type == CONF_TYPE)
            conflict_flag = 1;
    }

    flits_transmit_counter ++;
    /* sending a flit at a cycle */
    if(flit == NULL)
        panic("Network: No meta packet at the sender ");

    if (flit->Num != packet.msgno)
        panic("Network: packet %d conflits with packet %d\n", packet.msgno, flit->Num);


#ifdef CONF_PREDICTOR
    if(p_type == META)
    {
        if(receiver[meta_id].tail != NULL)
            flit->PID = receiver[meta_id].tail->PID;
    }
    else
    {
        if(receiver[data_id].tail != NULL)
            flit->PID = receiver[data_id].tail->PID;
    }

    if(flit->flit_type == HEADER && p_type == DATA)
    {
        /* Receiver the header and detect its a conflict packet header */
        ConfPacketListInsert(packet, flit->PID);
    }
#endif
#ifdef EMC
    if(p_type == META && flit->flit_type == HEADER)
        MetaConfPacketListInsert(packet, flit->PID);
#endif
    Flit *new_flit;
    if(currentFlits >= maxFlits)
        panic("Out of total flits\n");
    new_flit = allocate_flit();
    currentFlits++;
    new_flit->next = NULL;
    new_flit->Num = flit->Num;
    new_flit->PID = flit->PID;
    if(conflict_flag == 1)
    {
        new_flit->flit_type = CONF_TYPE;
        if(p_type == META)
            meta_conf_event ++;
    }
    else
        new_flit->flit_type = flit->flit_type;

    /* transfer the flits into receiver flit list througn point to point network */
    if(p_type == META)
        add_flit(new_flit, &receiver[meta_id]);
    else
        add_flit(new_flit, &receiver[data_id]);

    free_flit(flit, &transmitter[src]);

#endif

#ifdef IDEALCASE
    if(p_type == META)
        iteration = meta_packet_size;
    else
        iteration = data_packet_size;

    for (i=0; i<iteration; i++)
    {
        flit = transmitter[src].head;

        if(flit == NULL)
            panic("Network: No meta packet at the sender ");

        if (flit->Num != packet.msgno)
            panic("Network: packet %d conflits with packet %d\n", packet.msgno, flit->Num);

        Flit *new_flit;
        if(currentFlits >= maxFlits)
            panic("Out of total flits\n");
        new_flit = allocate_flit();
        currentFlits++;
        new_flit->flit_type = flit->flit_type;
        new_flit->next = NULL;
        new_flit->Num = flit->Num;

        /* transfer the flits into receiver flit list througn point to point network */
        if(p_type == META)
            add_flit(new_flit, &meta_receiver[des][meta_id]);
        else
            add_flit(new_flit, &data_receiver[des][data_id]);

        free_flit(flit, &transmitter[src]);

        flits_transmit_counter ++;
    }

    input_buffer_free(packet, sim_cycle);
    if(!( 0
#ifdef FETCH_DEADLOCK
            || packet.escape_channel
#endif
        ))
        input_buffer_num[packet.src][packet.channel] = input_buffer_num[packet.src][packet.channel] - packet.packet_size;
#endif
    /* scheduling network to receive the packet */
    if(packet.flitnum == 0)
        add_message(sim_cycle , WIRE_MSG, packet);
}

/* receiving the flits */
void receive_flit(Packet packet, counter_t sim_cycle)
{
    long src, des;
    counter_t start_time;

    src = packet.src;
    des = packet.des;
    start_time = packet.start_time;
    int p_type = packet.packet_type;

    int meta_id = packet.meta_id;
    int data_id = packet.data_id;

    Flit *flit;
    int i;
    int iteration, is_buffer_full = 0;
    Flit_list *receiver;
    if(p_type == META)
        receiver = meta_receiver[des];
    else
        receiver = data_receiver[des];

#ifdef CONF_RES_ACK
    if(p_type == DATA)
        iteration = data_packet_size;
    else
        iteration = meta_packet_size;

    for(i=0; i<iteration; i++)
    {
        if(p_type == DATA)
            flit = receiver[data_id].head;
        else
            flit = receiver[meta_id].head;

        if(flit == NULL)
            panic("Network: No data packet at receiver");

        if(i == 0 && (output_buffer_flits[des] > (output_buffer_size - data_packet_size*data_flit_tranfer_time))
#ifdef FETCH_DEADLOCK
                && packet.escape_channel == 0
#endif
          )
        {
            if(!last_Output_queue_full[des])
            {
                Output_queue_full ++;
                last_Output_queue_full[des] = sim_cycle;
            }
            is_buffer_full = 1;
        }
        else if(i==0)
        {
            if(last_Output_queue_full[des])
            {
                Stall_output_queue += sim_cycle - last_Output_queue_full[des];
                last_Output_queue_full[des] = 0;
            }
        }

        if(!is_buffer_full && flit->flit_type != CONF_TYPE)
        {
            /* if the output buffer is not full, add flits into the output buffer*/
#ifdef WB_SPLIT_NET
            if(!(packet.operation == WRITE_BACK_HEAD || packet.operation == WRITE_BACK_REPLY || packet.operation == MEM_PACK_HEAD || packet.operation == MEM_PACK_REPLY))
#endif
            {
                Flit *new_flit;
                if(currentFlits >= maxFlits)
                    panic("Out of total flits\n");
                new_flit = allocate_flit();
                currentFlits++;
                new_flit->flit_type = flit->flit_type;
                new_flit->next = NULL;
                new_flit->Num = flit->Num;

                add_flit(new_flit, &out_buffer[des]);
                if(packet.escape_channel == 0)
                    output_buffer_flits[des] ++;
                output_buf_access++;
            }

            if((flit->flit_type == TAIL))
            {
                /* confirmation delay here */
                add_message(sim_cycle + laser_setup_time + confirmation_time, ACK_MSG, packet);

                if(conf_laser_power[packet.channel][src] == LASER_OFF)
                    conf_laser_power_start_cycle[packet.channel][src] = sim_cycle + laser_setup_time;
                conf_laser_power_on_cycle[packet.channel][src] = sim_cycle + laser_setup_time + confirmation_time;
#ifdef INV_ACK_CON
#ifdef COHERENCE_MODIFIED
                if((packet.operation == INV_MSG_WRITE) && (packet.data_reply == 0))
#else
                if((packet.operation == INV_MSG_READ || packet.operation == INV_MSG_WRITE) && (packet.data_reply == 0))
#endif
                {
                    if(!pending_invalidation[packet.des])
                        pending_invalid_start_cycles[packet.des] = sim_cycle;
                    pending_invalidation[packet.des] ++;
                }
#endif

#ifdef WB_SPLIT_NET
                if(packet.operation == WRITE_BACK_HEAD)
                {
                    int temp;
                    temp = packet.des;
                    packet.des = packet.src;
                    packet.src = temp;
                    packet.operation = WRITE_BACK_REPLY;
                    add_message(sim_cycle, WB_REQ_MSG, packet);
                }
                else if(packet.operation == WRITE_BACK_REPLY)
                {
                    int temp;
                    temp = packet.des;
                    packet.des = packet.src;
                    packet.src = temp;
                    packet.operation = WRITE_BACK;
                    add_message(sim_cycle, WB_REQ_MSG, packet);

                }
#ifdef MEM_SPLIT_NET
                else if(packet.operation == MEM_PACK_HEAD)
                {
                    int temp;
                    temp = packet.des;
                    packet.des = packet.src;
                    packet.src = temp;
                    packet.operation = MEM_PACK_REPLY;
                    add_message(sim_cycle, WB_REQ_MSG, packet);
                }
                else if(packet.operation == MEM_PACK_REPLY)
                {
                    int temp;
                    temp = packet.des;
                    packet.des = packet.src;
                    packet.src = temp;
                    packet.operation = WAIT_MEM_READ_N;
                    add_message(sim_cycle, WB_REQ_MSG, packet);

                }
                else
                    add_packet(packet, des);
#else

                else
                    add_packet(packet, des);
#endif
#else
                add_packet(packet, des);
#endif
            }
        }
        if(p_type == DATA)
        {
            free_flit(flit, &receiver[data_id]);
            if(receiver[data_id].flits_in_list == 0)
                flits_receiving_counter += data_packet_size;
        }
        else
        {
            free_flit(flit, &receiver[meta_id]);
            if(receiver[meta_id].flits_in_list == 0)
                flits_receiving_counter ++;
        }
    }
#endif

#ifdef IDEALCASE
    if(p_type == DATA)
        iteration = data_packet_size;
    else
        iteration = meta_packet_size;

    for(i=0; i<iteration; i++)
    {
        if(p_type == DATA)
            flit = data_receiver[des][data_id].head;
        else
            flit = meta_receiver[des][meta_id].head;

        if(flit == NULL)
            panic("Network: No data packet at receiver");

        if (flit->Num != packet.msgno)
            panic("Network: packet %d conflits with packet %d\n", packet.msgno, flit->Num);

        /* if the output buffer is not full, add flits into the output buffer*/
        Flit *new_flit;
        if(currentFlits >= maxFlits)
            panic("Out of total flits\n");
        new_flit = allocate_flit();
        currentFlits++;
        new_flit->flit_type = flit->flit_type;
        new_flit->next = NULL;
        new_flit->Num = flit->Num;
        add_flit(new_flit, &out_buffer[des]);
        output_buffer_flits[des] ++;

        if((flit->flit_type == TAIL))
            add_packet(packet,des);


        if(p_type == DATA)
        {
            free_flit(flit, &data_receiver[des][data_id]);
            if(data_receiver[des][data_id].flits_in_list == 0)
                flits_receiving_counter += data_packet_size;
        }
        else
        {
            free_flit(flit, &meta_receiver[des][meta_id]);
            if(meta_receiver[des][meta_id].flits_in_list == 0)
                flits_receiving_counter ++;
        }
    }

#endif
}

#ifdef EMC
void MetaConfPacketListInsert(Packet packet, int PID)
{
    /* store all the conflict packet id for checking hint work or not */
    int i=0;
    int id = packet.des * meta_receivers + packet.meta_id;
    meta_conf_packet_num[id] ++;

    for(i=0; i<PROCESSOR; i++)
    {
        if(meta_conf_packet_list[id][i] == -1)
        {
            meta_conf_packet_list[id][i] = packet.src;
            meta_conf_packet[id][i] = packet;
            break;
        }
    }
}
void MetaConfPacketListClean(counter_t sim_cycle)
{
    /* clean the conflict packets record every start cycle */
    int i=0, j=0;
    for(j=0; j<MAXRECEIVERS; j++)
    {
        if(meta_conf_packet_num[j])
        {
            for(i=0; i<PROCESSOR; i++)
            {
                meta_conf_packet_list[j][i] = -1;
            }
        }
        meta_conf_packet_num[j] = 0;
    }
}
void MetaEarlyConf(counter_t sim_cycle)
{
    int i=0, j=0;
    int threadid, threadid_sec, threadid_third; /* Conflict packet will be send  a hit at 4_1 confirmation */
    for(j=0; j<MAXRECEIVERS; j++)
    {
        if(meta_conf_packet_num[j] == 1)
        {
            for(i=0; i<PROCESSOR; i++)
            {
                if(meta_conf_packet_list[j][i] != -1)
                {
                    add_message(sim_cycle + laser_setup_time + confirmation_time, EMC_ACK_MSG, meta_conf_packet[j][i]);
                    break;
                }
            }
        }
        int meta_sync_conf_flag = 0;
        int meta_ldl_conf_flag = 0;
        meta_way_conf[meta_conf_packet_num[j]] ++;
        if(meta_conf_packet_num[j] >= 2)
        {
            for(i=0; i<meta_conf_packet_num[j]; i++)
            {
                if(meta_conf_packet[j][i].isSyncAccess)
                {
                    meta_sync_conf_packets ++;
                    meta_sync_conf_flag = 1;
                }
                if(meta_conf_packet[j][i].ldl)
                {
                    meta_ldl_conf_packets ++;
                    meta_ldl_conf_flag = 1;
                }
            }
            if(meta_sync_conf_flag)
                meta_sync_conf_event ++;
            if(meta_ldl_conf_flag)
                meta_ldl_conf_event ++;
        }
        if(meta_conf_packet_num[j] == 2 && meta_sync_conf_flag)
            meta_conflict_table[j][meta_conf_packet_list[j][0]][meta_conf_packet_list[j][1]] ++;
        else if(meta_conf_packet_num[j] > 2 && meta_sync_conf_flag)
            meta_more_than_two_conflict ++;
        if(meta_conf_packet_num[j] == 2 && meta_ldl_conf_flag)
            meta_ldl_conflict_table[j][meta_conf_packet_list[j][0]][meta_conf_packet_list[j][1]] ++;
        else if(meta_conf_packet_num[j] > 2 && meta_ldl_conf_flag)
            meta_ldl_more_than_two_conflict ++;
    }
}
#endif
#ifdef CONF_PREDICTOR
void ConfPacketListInsert(Packet packet, int PID)
{
    /* store all the conflict packet id for checking hint work or not */
    int i=0;
    int id = packet.des * data_receivers + packet.data_id;
    conf_packet_num[id] ++;
    conf_PID[id] = PID;

    for(i=0; i<PROCESSOR; i++)
    {
        if(conf_packet_list[id][i] == -1)
        {
            conf_packet_list[id][i] = packet.src;
            conf_packet[id][i] = packet;
            break;
        }
    }
}

void ConfPacketListClean(counter_t sim_cycle)
{
    /* clean the conflict packets record every start cycle */
    int i=0, j=0;
    for(j=0; j<MAXRECEIVERS; j++)
    {
        if(conf_packet_num[j])
        {
            for(i=0; i<PROCESSOR; i++)
                conf_packet_list[j][i] = -1;
        }
        conf_packet_num[j] = 0;
        conf_PID[j] = -1;
    }
}
int WbCheck(int j);

#ifdef PRE_ID
int PreIDCheck(int PID, int confnum)
{
    int i=0, flag = 0;
    for(i=0; i<PROCESSOR; i++)
    {
        if((1<<i) & PID)
            flag ++;
        if(flag == confnum - 1)
            return i;
    }
}
#endif

void WbConfCheck(int j)
{
    int i = 0, m, src, pendingreqnum = 0;
    counter_t time1, time2;
    for(i=0; i<conf_packet_num[j]; i++)
    {
#ifdef MEM_SPLIT_NET
        if(conf_packet[j][i].operation == WAIT_MEM_READ_N && conf_packet[j][i].conf_bit != -1)
            MemSplitConf ++;
        else if(conf_packet[j][i].operation == WAIT_MEM_READ_N)
            MemSplitFailConf ++;
#endif

        if(conf_packet[j][i].operation == WRITE_BACK && conf_packet[j][i].conf_bit != -1)
            WbSplitConf ++;
        else if(conf_packet[j][i].operation == WRITE_BACK)
            WbSplitFailConf ++;
        if(conf_packet[j][i].operation == WRITE_BACK)
        {
            src = conf_packet[j][i].des;
            for(m=0; m<REQ_QUEUE_SIZE; m++)
            {
                if(ReqPendingQueue[src][m].valid)
                    pendingreqnum ++;
                if(conf_packet[j][0].src == ReqPendingQueue[src][m].des && ReqPendingQueue[src][m].valid && ReqPendingQueue[src][m].addr == conf_packet[j][0].addr)
                    time1 = ReqPendingQueue[src][m].time;
                if(conf_packet[j][1].src == ReqPendingQueue[src][m].des && ReqPendingQueue[src][m].valid && ReqPendingQueue[src][m].addr == conf_packet[j][1].addr)
                    time2 = ReqPendingQueue[src][m].time;
                if(pendingreqnum >= ReqPendingQueue_num[src])
                    break;
            }
            if(abs(time1-time2) < 5)
                wb_close ++;
        }
    }
    return 0;
}
void ConfPredictor(counter_t sim_cycle)
{
    int i=0, j=0;
    int threadid, threadid_sec, threadid_third; /* Conflict packet will be send  a hit at 4_1 confirmation */
    for(j=0; j<MAXRECEIVERS; j++)
    {
        if((((conf_PID[j]>>PID_shift)-(confpred[j].LastPred>>PID_shift))==0) && ConfWait[j] == sim_cycle - 1 && conf_PID[j] != -1)
            confpred[j].counter = 1;
        else
            confpred[j].counter = 0;
        data_way_conf[conf_packet_num[j]] ++;

        if(conf_packet_num[j] == 1)
        {
            for(i=0; i<PROCESSOR; i++)
            {
                if(conf_packet_list[j][i] != -1)
                {
                    add_message(sim_cycle + laser_setup_time + confirmation_time, EARLY_ACK_MSG, conf_packet[j][i]);
                }
            }
        }
        if(conf_packet_num[j] > 1)
        {
            if(conf_PID[j] <= 0)
                panic("Conflict hint error: PID and PID_bar can not be zero!\n");
            data_conflict_pro ++;
            WbConfCheck(j);
            Packet packet;
            packet.src = j/data_receivers;
            packet.conf_hint = 0;
            int m = 0, conf_list = 0;

            int sync_conf_flag = 0;
            if(conf_packet_num[j] >= 2)
            {
                for(i=0; i<conf_packet_num[j]; i++)
                {
                    if(conf_packet[j][i].isSyncAccess)
                    {
                        sync_conf_packets ++;
                        sync_conf_flag = 1;
                    }
                }
                if(sync_conf_flag)
                    sync_conf_event ++;
            }
            if(conf_packet_num[j] == 2 && sync_conf_flag)
            {
                conflict_table[j][conf_packet_list[j][0]][conf_packet_list[j][1]] ++;
            }
            else if(sync_conf_flag)
                more_than_two_conflict ++;
            /* Predict the conflict packet winner to send conflict hit */
#ifdef PRE_ID
            threadid = PreIDCheck(conf_PID[j], 2);
            if(conf_packet_num[j] >= 3)
            {
                threadid_sec = PreIDCheck(conf_PID[j], 3);
                if(conf_packet_num[j] >= 4)
                    threadid_third = PreIDCheck(conf_PID[j], 4);
            }
#else
            threadid = PidTableCheck(conf_PID[j], packet.src, j, sim_cycle);
#endif

            /* sending the conflict hint through confirmation bit */
            packet.des = threadid;
            add_message(sim_cycle + 1 + confirmation_time, CONF_HINT, packet); /* FIXME: 1 cycle is the handling time */
#ifdef PRE_ID
            if(conf_packet_num[j] >= 3)
            {
                packet.des = threadid_sec;
                packet.conf_hint = 2;
                add_message(sim_cycle + 1 + confirmation_time, CONF_HINT, packet); /* FIXME: 1 cycle is the handling time */
                if(conf_packet_num[j] >= 4)
                {
                    packet.des = threadid_third;
                    packet.conf_hint = 3;
                    add_message(sim_cycle + 1 + confirmation_time, CONF_HINT, packet); /* FIXME: 1 cycle is the handling time */
                }
            }
#endif


            int correct_flag = 0;
            for(i=0; i<PROCESSOR; i++)
            {
                if(conf_packet_list[j][i] == threadid)
                {
                    predict_correct ++;
                    correct_flag = 1;
                    if(conf_packet_num[j] == 2)
                        predictCorrect_two ++;   /* conflict involves two packets and the predictor predict correctly */
                    else
                        predictCorrect_more ++;  /* conflict involves more packest and the predictor predict correctly */
                    break;
                }
            }
            if(!correct_flag)
            {
                predict_fail ++;

                if(WbCheck(j))
                    WbPendingError ++;
                else if(ReqPendingQueue_num[packet.src] > 2)
                    MultiPendingError ++;
                else
                    ReqPendingError ++;

                if(conf_packet_num[j] == 2)
                    predictFail_two ++;   /* conflict involves two packets and the predictor predict failed */
                else
                    predictFail_more ++;  /* conflict involves more packest and the predictor predict failed */
            }
        }
    }
}
int WbCheck(int j)
{
    int wb_ture = 0, i = 0;
    for(i=0; i<PROCESSOR; i++)
    {
        if(conf_packet_list[j][i] != -1 && conf_packet[j][i].operation == WRITE_BACK)
        {
            wb_ture = 1;
            break;
        }
    }
    return wb_ture;
}
int PredictTable(int pid, int pid_bar, int seed, int m) /* return conflict pairs */
{
    int id, i, temp = 0, temp2 = 0, temp1 = 0, temp3 = 0, id_bar, max_Num = 0, a=0, b=0, c=0;
    temp = pid & pid_bar;
    if(seed/4 == 3 || seed/4 == 6)
        a = 1;
    else if(seed/4 == 5)
        a = 2;
    else if(seed/4 == 7)
    {
        a = 1;
        b = 2;
    }


    if(seed >= 4 && seed < 8)
        max_Num = 4;
    else if(seed >= 8 && seed < 16)
        max_Num = 5;
    else if(seed >= 16 && seed < 32)
        max_Num = 6;

    if(seed == 0) /* seed zero means to return the 0 and 1 pairs in the x position in ID */
    {
        id_bar = (~temp) & pid;
        return (pid<<PID_shift)|(id_bar&PID_bar_mask);
    }
    else if(seed < 4)
    {
        int count = 0;
        for(i=0; i<PID_SIZE; i++)
        {
            temp2 = ((1<<i) & temp);
            if(temp2)
            {
                count ++;
                if(count == seed)
                    break;
            }
        }
        id_bar = ((~(temp - temp2)) & PID_bar_mask) & pid;
        id = ((~(1<<i)) & PID_bar_mask) & pid;
        return (id<<PID_shift)|(id_bar&PID_bar_mask);
    }
    else
    {

        int count = 0;
        for(i=0; i<PID_SIZE; i++)
        {
            temp3 = ((1<<i) & temp);
            if(temp3)
            {
                count ++;
                if(count == max_Num-c || count == max_Num-a || count == max_Num-b)
                {
                    temp1 = temp1 | temp3;
                }
            }
        }
        count = 0;
        if((seed%4) == 0)
        {
            pid = pid & (~temp1);
            id_bar = ((~temp) & pid) | temp1;
            return (pid<<PID_shift)|(id_bar&PID_bar_mask);
        }
        else
        {
            for(i=0; i<PID_SIZE; i++)
            {
                temp2 = ((1<<i) & temp);
                if(temp2)
                {
                    count ++;
                    if(count == seed%4)
                        break;
                }
            }
            temp2 = temp2 | temp1;

            id_bar = ((~(temp - temp2)) & PID_bar_mask) & pid;
            id = ((~(temp2)) & PID_bar_mask) & pid;
            return (id<<PID_shift)|(id_bar&PID_bar_mask);
        }
    }
}

int ReqPendingCheck(int src, int threadid, counter_t sim_cycle)
{
    int i=0, flag = 0, pendingreqnum = 0, conf_id1, conf_id2;
    counter_t startTime = 0;
    conf_id1 = threadid >> PID_shift;
    conf_id2 = threadid & PID_bar_mask;
    for(i=0; i<REQ_QUEUE_SIZE; i++)
    {
        if(ReqPendingQueue[src][i].valid)
            pendingreqnum ++;
        if(conf_id1 == ReqPendingQueue[src][i].des && ReqPendingQueue[src][i].valid)
        {
            if(ReqPendingQueue_num[src] > 2)
            {
                startTime = ReqPendingQueue[src][i].time;
                if((((sim_cycle - startTime) >= 4 && (sim_cycle - startTime) < 170) || (sim_cycle - startTime) >= 240)
                        ||(ReqPendingQueue[src][i].op_code == WRITE_BACK_REPLY)
                  )
                {
                    flag = 1;
                    break;
                }
            }
            else
            {
                flag = 1;
                break;
            }
        }
        if(pendingreqnum >= ReqPendingQueue_num[src])
            break;
    }
#ifdef FULL_CHECK
    if(flag == 1)
    {
        pendingreqnum = 0;
        flag = 0;
        for(i=0; i<REQ_QUEUE_SIZE; i++)
        {
            if(ReqPendingQueue[src][i].valid)
                pendingreqnum ++;
            if(conf_id2 == ReqPendingQueue[src][i].des && ReqPendingQueue[src][i].valid)
            {
                if(ReqPendingQueue_num[src] > 2)
                {
                    startTime = ReqPendingQueue[src][i].time;
                    if((((sim_cycle - startTime) >= 4 && (sim_cycle - startTime) < 170) || (sim_cycle - startTime) >= 240)
                            ||(ReqPendingQueue[src][i].op_code == WRITE_BACK_REPLY)
                      )
                    {
                        flag = 1;
                        break;
                    }
                }
                else
                {
                    flag = 1;
                    break;
                }
            }
            if(pendingreqnum >= ReqPendingQueue_num[src])
                break;
        }
    }
#endif
    return flag;
}

#ifdef CONF_PRED_TABLE
int ConfPredTable(int RecNum, int j)  /* using a table to keep a record of last prediction and the result of the prediction(is last prediction right or wrong) to help current prediction */
{
    int id = -1, i = 0, HighPriority = 0, Priority = 0;
    if(RecNum == 1)
        id = ConfRecord[RecNum-1];
    else
    {
        id = ConfRecord[0];
        for(i=0; i<RecNum; i++)
        {
            if(confpred[j].LastPred == ConfRecord[i] && confpred[j].counter == 1)
                Priority = 4; /* highest priority */
            else if(confpred[j].LastPred == ConfRecord[i] && !confpred[j].counter)
                Priority = 0; /* Lowest priority */
#ifdef TWO_RECORD
            else if(confpred[j].LLastPred == ConfRecord[i] && confpred[j].Lcounter == 1)
                Priority = 3;
            else if(confpred[j].LLastPred == ConfRecord[i] && !confpred[j].Lcounter)
                Priority = 1;
#endif
            else
                Priority = 2;
            if(HighPriority < Priority)
            {
                id = ConfRecord[i];
                HighPriority = Priority;
            }
            ConfRecord[i] = 0;
        }
    }
    return id;
}
#endif
int Sharebits(pid, pid_bar)
{
    int i = 0, temp, temp2, count = 0;
    temp = pid & pid_bar;
    for(i=0; i<PID_SIZE; i++)
    {
        temp2 = ((1<<i) & temp);
        if(temp2)
            count ++;
    }
    return count;
}
int exp_2(int n)
{
    int i = 0;
    double result = 1.0;
    for(i=0; i<n; i++)
        result = result*2;
    return (int)result;
}

int PidTableCheck(int PID, int src, int j, counter_t sim_cycle)  /* predict the conflict threadid */
{
    int pid, pid_bar, i=0, m=0, iter = 0, threadid, id = -1, tempid = -1, flag = 0, pendingreqnum = 0, RecNum = 0, sharebit = 0, iteration = 0;
    pid = PID >> PID_shift;
    pid_bar = PID & PID_bar_mask;
    counter_t startTime = 0;
    sharebit = Sharebits(pid, pid_bar);
    iteration = exp_2(sharebit-1);
    for(i=0; i<iteration; i++)
    {
        int tmp_id;
        tempid = PredictTable(pid, pid_bar, i, sharebit);  /* FIXME: Predict table can calculated */ /* Predict the conflict threaid using the ID and ID_bar */
        tmp_id = tempid >> PID_shift;
        if(ReqPendingCheck(src, tempid, sim_cycle)
#ifdef MEM_MOD
#if (MESH_SIZE == 4)
                && (tmp_id != 1) && (tmp_id != 3) && (tmp_id != 21) && (tmp_id != 23)
#endif
#if (MESH_SIZE == 8)
                && (tmp_id != 1) && (tmp_id != 2) && (tmp_id != 3) && (tmp_id != 5) && (tmp_id != 6) && (tmp_id != 7)&& (tmp_id != 73)&& (tmp_id != 74)&& (tmp_id != 75)&& (tmp_id != 77)&& (tmp_id != 78)&& (tmp_id != 79)
#endif
#endif
          )  /* check whether the predict threadid is on the out-going pending list */

        {
#ifdef MEM_MOD
#if (MESH_SIZE == 4)
            if(((src/MESH_SIZE)<(MESH_SIZE/2) && (src%MESH_SIZE)<(MESH_SIZE/2) && tmp_id != 2 && tmp_id != 20 && tmp_id != 22) ||
                    ((src/MESH_SIZE)<(MESH_SIZE/2) && (src%MESH_SIZE)>=(MESH_SIZE/2) && tmp_id != 0 && tmp_id != 20 && tmp_id != 22) ||
                    ((src/MESH_SIZE)>=(MESH_SIZE/2) && (src%MESH_SIZE)<(MESH_SIZE/2) && tmp_id != 0 && tmp_id != 2 && tmp_id != 22) ||
                    ((src/MESH_SIZE)>=(MESH_SIZE/2) && (src%MESH_SIZE)>=(MESH_SIZE/2) && tmp_id != 0 && tmp_id != 2 && tmp_id != 20))
#endif
#if (MESH_SIZE == 8)
                if(((src/MESH_SIZE)<(MESH_SIZE/2) && (src%MESH_SIZE)<(MESH_SIZE/2) && (tmp_id != 4 && tmp_id != 72 && tmp_id != 76)) ||
                        ((src/MESH_SIZE)<(MESH_SIZE/2) && (src%MESH_SIZE)>=(MESH_SIZE/2) && (tmp_id != 0 && tmp_id != 72 && tmp_id != 76)) ||
                        ((src/MESH_SIZE)>=(MESH_SIZE/2) && (src%MESH_SIZE)<(MESH_SIZE/2) && (tmp_id != 0 && tmp_id != 4 && tmp_id != 76)) ||
                        ((src/MESH_SIZE)>=(MESH_SIZE/2) && (src%MESH_SIZE)>=(MESH_SIZE/2) && (tmp_id != 0 && tmp_id != 4 && tmp_id != 72)))
#endif
#endif
                {
                    id = tempid;
                    flag = 1;
                    ConfRecord[RecNum] = id;
                    RecNum ++;
                }
        }
    }

    if(flag)
    {
        MultiCase[RecNum] ++;
#ifdef CONF_PRED_TABLE
        if(RecNum > 1)
            id = ConfPredTable(RecNum, j);
#else
        ranum = rand()%RecNum;
        id = ConfRecord[ranum];
#endif
        threadid = id >> PID_shift;
    }

    int list_flag = 0;
    if(!flag) /* check again, if not then choose an out-going pending request as a predict threadid */
    {
        for(i=0; i<REQ_QUEUE_SIZE; i++)
        {
            if((ReqPendingQueue[src][i].valid))
            {
                pendingreqnum ++;
                if(ReqPendingQueue_num[src] > 2)
                {
                    startTime = ReqPendingQueue[src][i].time;
                    if((((sim_cycle - startTime) >= 4 && (sim_cycle - startTime) < 170) || (sim_cycle - startTime) >= 240)
                            ||(ReqPendingQueue[src][i].op_code == WRITE_BACK_REPLY)
                      )
                    {
                        threadid = ReqPendingQueue[src][i].des;
                        int Pid_Pred = PID_form(threadid);
                        if((Pid_Pred & PID) == Pid_Pred)
                        {
                            list_flag = 1;
                            break;
                        }
                        else
                            continue;
                    }
                }
                else
                {
                    threadid = ReqPendingQueue[src][i].des;
                    int Pid_Pred = PID_form(threadid);
                    if((Pid_Pred & PID) == Pid_Pred)
                    {
                        list_flag = 1;
                        break;
                    }
                    else
                        continue;
                }
            }
            if(pendingreqnum >= ReqPendingQueue_num[src])
                break;

        }
    }
    if(!list_flag && !flag)
    {
        id = (PredictTable(pid, pid_bar, 0, 0));
        threadid = id>>PID_shift;
    }
    int hit = 0;
    for(i=0; i<PROCESSOR; i++)
    {
        if(conf_packet_list[j][i] == threadid)
        {
            hit = 1;
#ifdef CONF_PRED_TABLE
            confpred[j].Lcounter = confpred[j].counter;
            confpred[j].LLastPred = confpred[j].LastPred;
            confpred[j].counter = 2;
            confpred[j].LastPred = id;
            ConfWait[j] = sim_cycle;
#endif
            if(flag)
                predict_table_correct ++;
            else
                predict_req_correct ++;
            break;
        }
    }
    if(!hit)
    {
        /* predict fail */
        if(flag == 0 && list_flag == 1)
            ReqPendingError2 ++;
        int m = 0, n=0;
        for(m=0; m<PROCESSOR; m++)
        {
            if(conf_packet_list[j][m] != -1 && conf_packet[j][m].operation == WRITE_BACK)
            {
                for(i=0; i<REQ_QUEUE_SIZE; i++)
                {
                    if(ReqPendingQueue[src][i].valid)
                        pendingreqnum ++;
                    if(conf_packet_list[j][m] == ReqPendingQueue[src][i].des && ReqPendingQueue[src][i].valid && conf_packet[j][m].addr == ReqPendingQueue[src][i].addr)
                    {
                        startTime = ReqPendingQueue[src][i].time;
                        if(conf_packet[j][m].retry_num >= 2)
                            wb_retry_conf ++;
                    }
                    if(pendingreqnum >= ReqPendingQueue_num[src])
                        break;
                }
                break;
            }
        }
#ifdef CONF_PRED_TABLE
        confpred[j].Lcounter = confpred[j].counter;
        confpred[j].LLastPred = confpred[j].LastPred;
        confpred[j].counter = 2;
        confpred[j].LastPred = id;
        ConfWait[j] = sim_cycle;
#if 0
        if(id == -1)
        {
            confpred[j].LastPred = threadid;
            confpred[j].counter = 2;
        }
#endif
#endif

    }
    return threadid;
}
#endif
/* *****************************************************************************/

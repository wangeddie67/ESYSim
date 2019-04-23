/* 
 	NETWORK CONFIGURATION FILE
	In this c file, the network access is defined. 

	demenstration for network: network id distribution
	MC is the memory controller location. Empth[] stead for a unuse node, we can also put MC there. Those nodes are configurable. 
	Left numbers in the [] stead for threadid or core ID. Right numbers in the [] stread for the ID for network mapping. 

	 *************************************************************
	column
line-->  [  ]==[MC]==[  ]==[MC]			[00]==[01]==[02]==[03]
	  ||    ||    ||    ||		 	 ||    ||    ||    || 
	 [00]==[01]==[02]==[03]			[10]==[11]==[12]==[13]
	  ||    ||    ||    ||			 ||    ||    ||    || 
	 [04]==[05]==[06]==[07]			[20]==[21]==[22]==[23]
	  ||    ||    ||    ||	   -------->	 ||    ||    ||    || 
	 [08]==[09]==[10]==[11]			[30]==[31]==[32]==[33]
	  ||    ||    ||    ||		  	 ||    ||    ||    ||   
	 [12]==[13]==[14]==[15]		 	[40]==[41]==[42]==[43]
	  ||    ||    ||    ||		  	 ||    ||    ||    || 
	 [  ]==[MC]==[  ]==[MC]		  	[50]==[51]==[52]==[53]

	 <-----Mesh size ----->			  Network ID mapping


	 *************************************************************
	Combinning Network 
		    |
	 [  ]==[MC] | [  ]==[MC]			  |	
	  ||    ||  |  ||    ||		      Chip0 ID(00)| Chip1 ID(01) 
	 [00]==[01] | [02]==[03]		FSOI/MESH | FSOI/MESH   <---Network configuration within a chip	
	  ||    ||  |  ||    ||				  |	
	 [04]==[05] | [06]==[07]		 	  |	
	------------+------------  ----------> -------MESH/FSOI-------  <---Network configuration between chip
	 [08]==[09] | [10]==[11]			  |	
	  ||    ||  |  ||    ||		      Chip2 Id(10)| Chip3 ID(11) 
	 [12]==[13] | [14]==[15]		FSOI/MESH | FSOI/MESH 
	  ||    ||  |  ||    ||		 		  | 
	 [  ]==[MC] | [  ]==[MC]		 	  | 
		    |

	 *************************************************************
 */
#include "headers.h"

#include "smt.h"
#ifdef SMT_SS
#include "context.h"
#endif

// fanglei
counter_t total_L2_MEM_L2Prefetch_Req;
counter_t total_L2_MEM_Data_Miss_Req;
counter_t total_L2_MEM_Inst_Miss_Req;
counter_t total_MEM_L2_Data_Payload;
counter_t total_MEM_L2_Inst_Payload;
counter_t total_L2_MEM_Data_Payload;
counter_t total_L1_L2_Inst_Miss_Req;
counter_t total_L2_L1_Inst_Payload;
counter_t total_L1_L2_Data_Read_Miss_Req;
counter_t total_L2_L1_Data_Read_Miss_Payload;
counter_t total_L2_L1_Data_Read_Invalid;
counter_t total_L1_L2_Data_Read_Invalid_Ack;
counter_t total_L1_L2_Data_Read_Invalid_Payload;
counter_t total_L1_L2_Data_Write_Miss_Req;
counter_t total_L2_L1_Data_Write_Miss_Payload;
counter_t total_L2_L1_Data_L2replaced_Invalid;
counter_t total_L2_L1_Data_Write_Invalid;
counter_t total_L1_L2_Data_L2replaced_Invalid_Ack;
counter_t total_L1_L2_Data_Write_Invalid_Ack;
counter_t total_L1_L2_Data_L2replaced_Invalid_Payload;
counter_t total_L1_L2_Data_Write_Invalid_Payload;
counter_t total_L1_L2_Data_Write_Update;
counter_t total_L2_L1_Data_Write_Update_Ack;
counter_t total_L1_L2_Data_Writeback_Payload;
counter_t total_L2_L1_Data_Verify_Ack;
counter_t total_L2_L1_Data_Verify_Playload;
counter_t total_L2_L1_Data_Prefetch_Playload_PPP;
counter_t total_L1_L2_Read_Verify_Req;
counter_t total_L1_L2_Write_Verify_Req;
counter_t total_L1_L2_shr_tlb_miss_Req;
counter_t total_L1_L2_shr_tlb_write_back_Req;
counter_t total_L1_L2_shr_tlb_sr_recovery_Req;
counter_t total_L1_L2_shr_tlb_update_pr_ack_Req;
counter_t total_L1_L2_shr_tlb_update_pw_ack_Req;
counter_t total_L1_L2_shr_tlb_recovery_ack_Req;
counter_t total_L1_L2_dir_tlb_refill_Req;
counter_t total_L1_L2_dir_tlb_update_Req;
counter_t total_L1_L2_dir_tlb_recovery_Req;
counter_t total_L2_L1_dir_shr_read;
counter_t total_L1_L2_ack_shr_read;
counter_t total_L1_L2_inform_back;
counter_t total_TSHR_Full;


extern counter_t esynetMsgNo;
extern counter_t Input_queue_full;
extern counter_t last_Input_queue_full[MAXTHREADS+MESH_SIZE*2];
extern struct cache_t *cache_dl1[], *cache_ml2, *cache_dl2;
extern counter_t MetaPackets_1;
extern counter_t DataPackets_1;
extern counter_t Hops_1;
extern counter_t TotalMetaPacketsInAll, TotalDataPacketsInAll, TotalSyncMetaPackets, TotalSyncDataPackets;
extern counter_t optical_data_packets, optical_meta_packets, mesh_data_packets, mesh_meta_packets;
extern counter_t totalOptHopCount, totalOptHopDelay, totalOptNorHopCount, totalOptNorHopDelay, totalOptSyncHopCount, totalOptSyncHopDelay;
extern counter_t totalMeshHopCount, totalMeshHopDelay, totalEventCount, totalEventProcessTime;
extern counter_t totalMeshNorHopCount, totalMeshNorHopDelay, totalMeshSyncHopCount, totalMeshSyncHopDelay;
extern counter_t totalUpgrades;
extern counter_t totalNetUpgrades;
extern counter_t totalUpgradesUsable;
extern counter_t totalUpgradesBeneficial;
extern counter_t data_packets_sending[300][300];
extern counter_t meta_packets_sending[300][300];
extern counter_t flits_sending[100][100];
counter_t  meta_packet_dir[300][2];
extern counter_t meta_even_req_close[300];
extern counter_t meta_odd_req_close[300];
extern counter_t req_spand[261];

extern counter_t link_ser_lat[300][300]; /* FIXME: if threadnum is more than 64, large array has to be clarified instread 100x100*/

extern counter_t totalNetWrites;
extern counter_t totalWritesUsable;
extern counter_t totalWritesBeneficial;
extern counter_t exclusive_somewhere;
extern counter_t front_write;
extern lock_network_access;
void IDChangeForIntraNetwork(int src1, int src2, int des1, int des2, int *new_src1, int *new_src2, int *new_des1, int *new_des2);

extern short m_shSQSize;		//Total number of entries in Store Queue.
extern int collect_stats;


void network_reg_options(struct opt_odb_t *odb)
{ /* configuration input */
	opt_reg_string (odb, "-network", "network type {FSOI|MESH|COMB|HYBRID}", &network_type, /* default */ "FSOI",
			/* print */ TRUE, /* format */ NULL);

	/* if network type is COMB, then the network configuration should be defined both in inter-chip and intra-chip */
	opt_reg_string (odb, "-network:intercfg", "inter chip network config {FSOI|MESH}", &inter_cfg, "FSOI", /* print */ TRUE, NULL);
	opt_reg_string (odb, "-network:intracfg", "intra chip network config {FSOI|MESH}", &intra_cfg, "MESH", /* print */ TRUE, NULL);

	/* if total number of chips larger than 1, it means the the multi chip simulation */ 
	opt_reg_int (odb, "-network:chipnum", "number of chips in whole system", &total_chip_num, /* default */ 4,
			/* print */ TRUE, /* format */ NULL);	
	optical_options(odb);
    simEsynetOptions(odb);
}

void reply_statistics(counter_t req_time)
{
	if(sim_cycle - req_time < 100)
		req_spand[sim_cycle-req_time] ++;
	else if(sim_cycle - req_time >=100 && sim_cycle - req_time < 120)
		req_spand[100]++;
	else if(sim_cycle - req_time >=120 && sim_cycle - req_time < 140)
		req_spand[101]++;
	else if(sim_cycle - req_time >=140 && sim_cycle - req_time < 160)
		req_spand[102]++;
	else if(sim_cycle - req_time >=160 && sim_cycle - req_time < 180)
		req_spand[103]++;
	else if(sim_cycle - req_time >=180 && sim_cycle - req_time < 200)
		req_spand[104]++;
	else if(sim_cycle - req_time >= 200 && sim_cycle - req_time < 350)
		req_spand[sim_cycle-req_time-95] ++;
	else if(sim_cycle - req_time >= 350 && sim_cycle - req_time < 400)
		req_spand[255] ++;
	else if(sim_cycle - req_time >= 400 && sim_cycle - req_time < 450)
		req_spand[256] ++;
	else if(sim_cycle - req_time >= 450 && sim_cycle - req_time < 550)
		req_spand[257] ++;
	else if(sim_cycle - req_time >= 550 && sim_cycle - req_time < 650)
		req_spand[258] ++;
	else if(sim_cycle - req_time >= 650 && sim_cycle - req_time < 850)
		req_spand[259] ++;
	else if(sim_cycle - req_time >= 850)
		req_spand[260] ++;
}
void network_check_options()
{
	if(!mystricmp (network_type, "COMB"))
	{
		if(total_chip_num == 0)
			fatal("Single chip can not use hybrid network system!\n");
		if(total_chip_num > MAXTHREADS)
			fatal("number of chip exceed the number of threads!\n");
		if(MAXTHREADS == 16)
		{
			if(!(total_chip_num == 2 || total_chip_num == 4 || total_chip_num == 8))
				fatal("number of chip exceed the number of threads!\n");
		}
		if(MAXTHREADS == 32)
		{
			/* FIXME: hard code for 16 and 64, which are rectangular, and no mesh size for 32 */
			fatal("number of chip exceed the number of threads!\n");
		}
		if(MAXTHREADS == 64)
		{
			if(!(total_chip_num == 2 || total_chip_num == 4 || total_chip_num == 8 || total_chip_num == 16 || total_chip_num == 32))
				fatal("number of chip exceed the number of threads!\n");
		}
	}
	Line_chip_num = 2;
	Column_chip_num = total_chip_num/Line_chip_num;
	return;
}


int ChipIdCheck(int a1, int a2)
{
	if(total_chip_num == 0)
		fatal("Single chip can not use hybrid network system!\n");

	int line_size = mesh_size;
	int column_size = mesh_size + 2; /* Array size plus two lines for memory controller */
		
	if(total_chip_num != 4)
		panic("only hard code for chip number is 4\n");	

	if(a1 < column_size/Column_chip_num)
	{
		if(a2 < line_size/Line_chip_num)
			return 0;	/* belongs to the chip 0 */	
		else
			return 1	/* belongs to the chip 1 */;
	}
	else
	{
		if(a2 < line_size/Line_chip_num)
			return 2;	/* belongs to the chip 2 */	
		else
			return 3	/* belongs to the chip 3 */;
	}
}

 /* why I change the ID for inter network access? because if you want to perform inter network access and intra network access both MESH or FSOI, 
     basically you have to use a secondary level network forum to perfrom the communication between inter chip. This ID change serves as the secondary
     level network forum, and use the two more extra lines to do this communications */
void IDChangeForIntraNetwork(int src1, int src2, int des1, int des2, int *new_src1, int *new_src2, int *new_des1, int *new_des2)
{
	int ChipID_x = ChipIdCheck(src1, src2);
	int ChipID_y = ChipIdCheck(des1, des2);
	int x_start = 0, y_start = mesh_size;
	*new_src1 = x_start + ChipID_x/2;
	*new_src2 = y_start + ChipID_x%2;
	*new_des1 = x_start + ChipID_y/2;
	*new_des2 = y_start + ChipID_y%2;
	return;
}
/* check the communication belongs to which network configuration */
int ConfigCheck(int src1, int src2, int des1, int des2)
{
	int src_chip = ChipIdCheck(src1, src2);
	int des_chip = ChipIdCheck(des1, des2);	
	if(src_chip == des_chip)
	{
		/* belongs to the same chip */
		if(!mystricmp(intra_cfg, "FSOI"))
			return FSOI_NET_INTRA;  /* FSOI network configuration within chip */  
		else 	
			return MESH_NET_INTRA;   /* MESH network configuration within chip */
	}
	else
	{
		/* belongs to different chips */
		if(!mystricmp(inter_cfg, "FSOI"))
			return FSOI_NET_INTER;  /* FSOI network configuration between chips */  
		else 	
			return MESH_NET_INTER;  /* MESH network configuration between chips */ 
	}
}
int CombNetworkBufferSpace(int src1, int src2, int des1, int des2, md_addr_t addr, int operation, int *vc)
{
	int options = -1, buffer_full = 0;
	int new_src1, new_src2, new_des1, new_des2;
	IDChangeForIntraNetwork(src1, src2, des1, des2, &new_src1, &new_src2, &new_des1, &new_des2);
	int flag = ConfigCheck(src1, src2, des1, des2);

	if(flag == 0 || flag == 2)
	{ /* belongs to FSOI network */
		if(flag == 2)
		{ /* Intra chip */
			if(opticalBufferSpace(new_src1, new_src2, operation))
				buffer_full = 1;
		}
		else
		{ /* Inter chip */
			if(opticalBufferSpace(src1, src2, operation))
				buffer_full = 1;
		}
	}
	else
	{ /* belongs to MESH network */
		if(flag == 3)
		{ /* Intra chip */
			options = OrderCheck(new_src1, new_src2, new_des1, new_des2, addr);
            *vc = simEsynetBufferSpace(new_src1, new_src2, options);
			if(*vc == -1)
				buffer_full = 1;
		}
		else
		{ /* Inter chip */
			options = OrderCheck(src1, src2, des1, des2, addr);
            *vc = simEsynetBufferSpace(src1, src2, options);
			if(*vc == -1)
				buffer_full = 1;
		}
	}
	return buffer_full;
}
#ifdef LOCK_HARD
void schedule_network(struct LOCK_EVENT *event)
{
//	int flag = local_access_check(event->src1, event->src2, event->des1, event->des2);
    int flag = local_access_check(event);
	int type = 0;
	if(event->opt == ACQ_REMOTE_ACC || event->opt == REL_REMOTE_ACC)
		type = meta_packet_size;	
    esynetMsgNo++;
	if(!flag && collect_stats)
	{
		event->when = sim_cycle + 1;
		lock_network_access ++;
		/* insert into network if it's not local access */
		if (!mystricmp (network_type, "FSOI"))
            directMsgInsert(event->src1, event->src2, event->des1, event->des2, 
sim_cycle, meta_packet_size, esynetMsgNo, 0, event->opt, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1,0);
		else if(!mystricmp (network_type, "MESH"))
            simEsynetMessageInsert(event->src1, event->src2, event->des1, 
event->des2, sim_cycle, meta_packet_size, esynetMsgNo, 0, event->opt, 0);
		event->when = sim_cycle + WAIT_TIME;
		
	}
	else
		event->when = sim_cycle;
    event->esynetMsgNo = esynetMsgNo;
}
int LockMsgComplete(long w, long x, long y, long z, long long int msgNo)
{
	struct LOCK_EVENT *event, *prev;
	event = lock_event_queue;
	prev = NULL;
	if (event!=NULL)
	{
		while (event)
		{
            if(event->src1 == w && event->src2 == x && event->des1 == y && 
event->des2 == z && msgNo == event->esynetMsgNo)
			{
				event->when = sim_cycle;
				return 1;
			}
			prev = event;
			event = event->next;
		}
	}
	panic("Error finding event for the returned message from network");
}
#endif

/* Schedule an event through network */
void total_transfer_type(struct DIRECTORY_EVENT *event, int type)
{
        switch (event->operation)
        {
            // L2 <-> MEM message; @ fanglei
            case L2_PREFETCH:       
            {
                total_L2_MEM_L2Prefetch_Req++;
                break;
            }
            case MEM_READ_REQ:      
            {
                if(event->cmd == Read)
                    total_L2_MEM_Data_Miss_Req++;
                else
                    total_L2_MEM_Inst_Miss_Req++;
                break;
            }
            case WAIT_MEM_READ: 
            case WAIT_MEM_READ_N:   
            {
                if(event->cmd == Read)
                    total_MEM_L2_Data_Payload++;
                else
                    total_MEM_L2_Inst_Payload++;
                break;
            }
            case MEM_WRITE_BACK:    
            {
                total_L2_MEM_Data_Payload++;
                break;
                }
            // L1 <-> L2 message; @ fanglei
            case MISS_IL1:
            {
                total_L1_L2_Inst_Miss_Req++;
                break;
            }
            case ACK_DIR_IL1:       
            {
                total_L2_L1_Inst_Payload++;
                break;
            }
            case MISS_READ:         
            {
                total_L1_L2_Data_Read_Miss_Req++;
                break;
            }
            case ACK_DIR_READ_SHARED:
            case ACK_DIR_READ_EXCLUSIVE:
            {
                total_L2_L1_Data_Read_Miss_Payload++;
                break;
            }
            case INV_MSG_READ:
            {
                total_L2_L1_Data_Read_Invalid++;
                break;
            }
            case ACK_MSG_READ:
            {
                total_L1_L2_Data_Read_Invalid_Ack++;
                break;
            }
            case ACK_MSG_READUPDATE:
            {
                total_L1_L2_Data_Read_Invalid_Payload++;
                break;
            }
            case MISS_WRITE:
            {
                total_L1_L2_Data_Write_Miss_Req++;
                break;
            }
            case ACK_DIR_WRITE:
            {
                total_L2_L1_Data_Write_Miss_Payload++;
                break;
            }
            case INV_MSG_WRITE:
            {
                if(event->parent->sharer_num)
                    total_L2_L1_Data_L2replaced_Invalid++;
                else
                    total_L2_L1_Data_Write_Invalid++;
                break;
            }
            case ACK_MSG_WRITE:
            {
                if(event->parent->sharer_num)
                    total_L1_L2_Data_L2replaced_Invalid_Ack++;
                else
                    total_L1_L2_Data_Write_Invalid_Ack++;
                break;
            }
            case ACK_MSG_WRITEUPDATE:
            {
                if( event->parent->sharer_num )
                    total_L1_L2_Data_L2replaced_Invalid_Payload++;
                else
                    total_L1_L2_Data_Write_Invalid_Payload++;
                break;
            }
            case WRITE_UPDATE:
            {
                total_L1_L2_Data_Write_Update++;
                break;
            }
            case ACK_DIR_WRITEUPDATE:
            {
                total_L2_L1_Data_Write_Update_Ack++;
                break;
            }
            case WRITE_BACK:
            {
                total_L1_L2_Data_Writeback_Payload++;
                break;
            }
            case NACK:
            {
                total_TSHR_Full++;
                break;
            }
            case SHR_TLB_MISS:
            {
                total_L1_L2_shr_tlb_miss_Req++;
                break;
            }
            case SHR_TLB_WRITE_BACK:
            {
                total_L1_L2_shr_tlb_write_back_Req++;
                break;
            }
            case SHR_TLB_SR_RECOVERY:
            {
                total_L1_L2_shr_tlb_sr_recovery_Req++;
                break;
            }
            case SHR_TLB_UPDATE_PR_ACK:
            {
                total_L1_L2_shr_tlb_update_pr_ack_Req++;
                break;
            }
            case SHR_TLB_UPDATE_PW_ACK:
            {
                total_L1_L2_shr_tlb_update_pw_ack_Req++;
                break;
            }
            case SHR_TLB_RECOVERY_ACK:
            {
                total_L1_L2_shr_tlb_recovery_ack_Req++;
                break;
            }
            case DIR_TLB_REFILL:
            {
                total_L1_L2_dir_tlb_refill_Req++;
                break;
            }
            case DIR_TLB_UPDATE:
            {
                total_L1_L2_dir_tlb_update_Req++;
                break;
            }
            case DIR_TLB_RECOVERY:
            {
                total_L1_L2_dir_tlb_recovery_Req++;
                break;
            }
            case DIR_TLB_REFILL_WAIT:
                break;
            case DIR_TLB_UPDATE_WAIT:
                break;
            case DIR_TLB_RECOVERY_WAIT:
                break;

            case DIR_SHR_READ:
            {
                total_L2_L1_dir_shr_read++;
                break;
            }
            case ACK_SHR_READ:
            {
                total_L1_L2_ack_shr_read++;
                break;
            }
            case INFORM_BACK:
            {
                total_L1_L2_inform_back++;
                break;
            }
            default:
            {
                fflush(stdout);
                printf("%d\n", event->operation);
                fflush(stdout);
                panic("the operation is missed, fix it!");
            }
        }
}


void scheduleThroughNetwork(struct DIRECTORY_EVENT *event, long start, unsigned int type, int vc)
{
	if(event->src1 < 0 || event->src1 > 16 || event->src2 < 0 || event->src2 > 16 || event->des1 < 0 || event->des1 > 16 || event->des2 < 0 || event->des2 > 16)
		panic("Network: source or destination addresses are wrong.");

//	int flag = local_access_check(event->src1, event->src2, event->des1, event->des2);
    int flag = local_access_check(event);
	int even_flag = 0;
    esynetMsgNo++;
	
	/* for load link optimization, it's a HACK, should be removed */
	if(event->rs)
		if(event->rs->op == LDL_L)
			even_flag = 1;

	/* statistics collecting for sync instructions */
	/*
	if(event->isSyncAccess)
	{
		if(type == data_packet_size)
			data_packets_sending[event->src1*(mesh_size+2)+event->src2][event->des1*(mesh_size+2)+event->des2] ++;
		else
		{
			meta_packets_sending[event->src1*(mesh_size+2)+event->src2][event->des1*(mesh_size+2)+event->des2] ++;
			if((event->des1*(mesh_size+2)+event->des2) % 2== 0)
			{// sending out the request to even nodes 
				if(sim_cycle - meta_packet_dir[event->src1*(mesh_size+2)+event->src2][0] <10)
					meta_even_req_close[event->src1*(mesh_size+2)+event->src2] ++;
				meta_packet_dir[event->src1*(mesh_size+2)+event->src2][0] = sim_cycle;
			}
			else
			{
				if(sim_cycle - meta_packet_dir[event->src1*(mesh_size+2)+event->src2][1] <10)
					meta_odd_req_close[event->src1*(mesh_size+2)+event->src2] ++;
				meta_packet_dir[event->src1*(mesh_size+2)+event->src2][1] = sim_cycle;
			}
		}
	}
	*/
	if (!mystricmp (network_type, "FSOI"))
	{ /* optical network schedule */
		if(!flag)
		{ /* for non-local access, go the network */
			if(event->operation == MISS_IL1 || event->operation == MISS_READ || event->operation == MISS_WRITE || event->operation == WRITE_UPDATE )
			{ /* before go to the network, first check the outgoing buffer in network is full or not for those first class requests */
				if(opticalBufferSpace(event->src1, event->src2, event->operation))
				{
					event->input_buffer_full = 1;
					event->when = start+RETRY_TIME; /*retry again next cycle */		
					if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
					{
						Input_queue_full ++;
						last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
					}
					return 0;
				}	
			}
			/* insert into network if it's not local access */
            directMsgInsert(event->src1, event->src2, event->des1, event->des2, 
start, type, esynetMsgNo, event->addr, event->operation, event->delay, 
event->L2miss_flag, event->prefetch_next, event->dirty_flag, 
event->arrival_time, event->data_reply, event->conf_bit, 0, even_flag, 
event->isSyncAccess, event->store_cond);
			event->when = start + WAIT_TIME; /* after comming back from the network, will wake up this event at the directory fifo queue or l1 fifo queue */
		}
		else
			event->when = start; /* for local access */
		event->esynetMsgNo = esynetMsgNo;
	}
	else if(!mystricmp (network_type, "MESH"))
	{ /* mesh network schedule */
        total_transfer_type(event, type);
		if(!flag)
		{ /*non-local accesses, go the network */
			int options = -1;
			options = OrderCheck(event->src1, event->src2, event->des1, event->des2, event->addr&~(cache_dl1[0]->bsize-1));
            vc = simEsynetBufferSpace(event->src1, event->src2, options);
			if(event->operation == MISS_IL1 || event->operation == MISS_READ || event->operation == MISS_WRITE || event->operation == WRITE_UPDATE )
			{ /* for this first class request, check the outgoing buffer in network */
				if(vc == -1)
				{ /* vc is -1 means the outgoing buffer is full */
					event->input_buffer_full = 1;
					event->when = start+RETRY_TIME;	/* retry again */	
					if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
					{
						Input_queue_full ++;
						last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
					}
					return 0;
				}	
			}

			if(vc == -1)
			{ /* vc is -1 means the outgoing buffer is full, overflow in outgoing buffer  */ 
				if(options == -1)
					vc = 0; // FIXME: hard core here, 1 should be the number of virtual channel 
				else
					vc = options;
			}
			if(type == data_packet_size)
			{
				DataPackets_1 ++;
				flits_sending[event->src1*(mesh_size+2)+event->src2][event->des1*(mesh_size+2)+event->des2] += data_packet_size;
				Hops_1 += data_packet_size*(abs(event->src1-event->des1)+abs(event->src2-event->des2));
			}
			else
			{
				MetaPackets_1 ++;
				flits_sending[event->src1*(mesh_size+2)+event->src2][event->des1*(mesh_size+2)+event->des2] += 1;
				Hops_1 += abs(event->src1-event->des1)+abs(event->src2-event->des2);
			}

	#ifndef MULTI_VC
			vc = 0;
	#endif
            simEsynetMessageInsert(event->src1, event->src2, event->des1, 
event->des2, start, type, esynetMsgNo, event->addr>>cache_dl2->set_shift, 
event->operation, vc);

	#ifdef MULTI_VC
			/*Insert the request into table to record which vc it used */
            OrderBufInsert(event->src1, event->src2, event->des1, event->des2, 
event->addr&~(cache_dl1[0]->bsize-1), vc, esynetMsgNo);
	#endif
			event->when = start + WAIT_TIME;
		}
		else
			event->when = start;
        event->esynetMsgNo = esynetMsgNo;
	}
	else if (!mystricmp (network_type, "HYBRID"))
	{ /* On-chip hybrid network insert */
		if((abs(event->des1 - event->src1) + abs(event->des2 - event->src2)) > 1)
		{ /* larger distance using optical network */
			/* optical network schedule */
			if(!flag)
			{
				if(event->operation == MISS_IL1 || event->operation == MISS_READ || event->operation == MISS_WRITE || event->operation == WRITE_UPDATE )
				{
					if(opticalBufferSpace(event->src1, event->src2, event->operation))
					{
						event->input_buffer_full = 1;
						event->when = start+RETRY_TIME;		
						if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
						{
							Input_queue_full ++;
							last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
						}
						return 0;
					}	
				}
				/* insert into network if it's not local access */
                directMsgInsert(event->src1, event->src2, event->des1, 
event->des2, start, type, esynetMsgNo, event->addr, event->operation, 
event->delay, event->L2miss_flag, event->prefetch_next, event->dirty_flag, 
event->arrival_time, event->data_reply, event->conf_bit, 0, even_flag, 
event->isSyncAccess, event->store_cond);

				if(type == data_packet_size)
					optical_data_packets ++;
				else
					optical_meta_packets ++;
				event->when = start + WAIT_TIME;
			}
			else
				event->when = start;
            event->esynetMsgNo = esynetMsgNo;
		}
		else
		{
			/* electrical link network schedule */
			if(!flag)
			{
				if(type == data_packet_size)
					link_ser_lat[event->src1*mesh_size+event->src2][event->des1*mesh_size+event->des2] += type; /* serilaize latency */
				else 
					link_ser_lat[event->src1*mesh_size+event->src2][event->des1*mesh_size+event->des2] += 1; /* serilaize latency */

				event->when = start + link_ser_lat[event->src1*mesh_size+event->src2][event->des1*mesh_size+event->des2];
				if(type == data_packet_size)
					mesh_data_packets ++;
				else
					mesh_meta_packets ++;
				
			}
			else
				event->when = start;
            event->esynetMsgNo = esynetMsgNo;
		}
	}
	else
	{ /* Multi-chip hybrid network insert */
		if(!flag)
		{
			int Chip_identify = ConfigCheck(event->src1, event->src2, event->des1, event->des2);
			if(Chip_identify == FSOI_NET_INTER || Chip_identify == FSOI_NET_INTRA)
			{
				/* Belongs to the FSOI network*/
				if(Chip_identify == FSOI_NET_INTER)
				{
					/* Chip_identify == FSOI_NET_INTER means the inter chip network configuration is FSOI, use secondary level network forum to transfer the data */
					int new_src1, new_src2, new_des1, new_des2;
					IDChangeForIntraNetwork(event->src1, event->src2, event->des1, event->des2, &new_src1, &new_src2, &new_des1, &new_des2);
					if(event->operation == MISS_IL1 || event->operation == MISS_READ || event->operation == MISS_WRITE || event->operation == WRITE_UPDATE )
					{
						if(opticalBufferSpace(new_src1, new_src2, event->operation))
						{
							event->input_buffer_full = 1;
							event->when = start+1;		
							if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
							{
								Input_queue_full ++;
								last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
							}
							return 0;
						}	
					}
					/* insert into network if it's not local access */
                    directMsgInsert(new_src1, new_src2, new_des1, new_des2, 
start, type, esynetMsgNo, event->addr, event->operation, event->delay, 
event->L2miss_flag, event->prefetch_next, event->dirty_flag, 
event->arrival_time, event->data_reply, event->conf_bit, 0, even_flag, 
event->isSyncAccess, event->store_cond);
					event->when = start + WAIT_TIME;
					event->new_src1 = new_src1;
					event->new_src2 = new_src2;
					event->new_des1 = new_des1;
					event->new_des2 = new_des2;
					
				}
				else  
				{
					/* Chip_identify == FSOI_NET_INTRA means the intra chip network configruation is FSOI */
					if(event->operation == MISS_IL1 || event->operation == MISS_READ || event->operation == MISS_WRITE || event->operation == WRITE_UPDATE )
					{
						if(opticalBufferSpace(event->src1, event->src2, event->operation))
						{
							event->input_buffer_full = 1;
							event->when = start+1;		
							if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
							{
								Input_queue_full ++;
								last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
							}
							return 0;
						}	
					}
					/* insert into network if it's not local access */
                    directMsgInsert(event->src1, event->src2, event->des1, 
event->des2, start, type, esynetMsgNo, event->addr, event->operation, 
event->delay, event->L2miss_flag, event->prefetch_next, event->dirty_flag, 
event->arrival_time, event->data_reply, event->conf_bit, 0, even_flag, 
event->isSyncAccess, event->store_cond);
					event->when = start + WAIT_TIME;
				}
			}
			else 
			{
				/* Belongs to the MESH network*/
				int options = -1;
				if(Chip_identify == MESH_NET_INTER)
				{
					/* Chip_identify == MESH_NET_INTER means the inter chip network configuration is MESH */
					int new_src1, new_src2, new_des1, new_des2;
					IDChangeForIntraNetwork(event->src1, event->src2, event->des1, event->des2, &new_src1, &new_src2, &new_des1, &new_des2);

					options = OrderCheck(event->src1, event->src2, event->des1, event->des2, event->addr&~(cache_dl1[0]->bsize-1));
                    vc = simEsynetBufferSpace(event->src1, event->src2, 
options);

					if(event->operation == MISS_IL1 || event->operation == MISS_READ || event->operation == MISS_WRITE || event->operation == WRITE_UPDATE )
					{
						if(vc == -1)
						{
							event->input_buffer_full = 1;
							event->when = start+1;		
							if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
							{
								Input_queue_full ++;
								last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
							}
							return 0;
						}	
					}

					if(vc == -1)
					{
						if(options == -1)
							vc = 0; // FIXME: hard core here, 1 should be the number of virtual channel 
						else
							vc = options;
					}
					if(type == data_packet_size)
						DataPackets_1 ++;
					else
						MetaPackets_1 ++;
#ifndef MULTI_VC
					vc = 0;
#endif
                    simEsynetMessageInsert(new_src1, new_src2, new_des1, 
new_des2, 
start, type, esynetMsgNo, event->addr>>cache_dl2->set_shift, event->operation, 
vc);
#ifdef MULTI_VC
                    OrderBufInsert(new_src1, new_src2, new_des1, new_des2, 
event->addr&~(cache_dl1[0]->bsize-1), vc, esynetMsgNo);
#endif
					event->when = start + WAIT_TIME;
					event->new_src1 = new_src1;
					event->new_src2 = new_src2;
					event->new_des1 = new_des1;
					event->new_des2 = new_des2;
					
				}
				else
				{
					/* Chip_identify == MESH_NET_INTRA means the intra chip network configuration is MESH */
					options = OrderCheck(event->src1, event->src2, event->des1, event->des2, event->addr&~(cache_dl1[0]->bsize-1));
                    vc = simEsynetBufferSpace(event->src1, event->src2, 
options);
					if(event->operation == MISS_IL1 || event->operation == MISS_READ || event->operation == MISS_WRITE || event->operation == WRITE_UPDATE )
					{
						options = OrderCheck(event->src1, event->src2, event->des1, event->des2, event->addr&~(cache_dl1[0]->bsize-1));
                        vc = simEsynetBufferSpace(event->src1, event->src2, 
options);
						if(vc == -1)
						{
							event->input_buffer_full = 1;
							event->when = start+1;		
							if(!last_Input_queue_full[event->des1*mesh_size+event->des2])
							{
								Input_queue_full ++;
								last_Input_queue_full[event->des1*mesh_size+event->des2] = sim_cycle;
							}
							return 0;
						}	
					}

					if(vc == -1)
					{
						if(options == -1)
							vc = 0; // FIXME: hard core here, 1 should be the number of virtual channel 
						else
							vc = options;
					}
					if(type == data_packet_size)
						DataPackets_1 ++;
					else
						MetaPackets_1 ++;
#ifndef MULTI_VC
					vc = 0;
#endif
                    simEsynetMessageInsert(event->src1, event->src2, 
event->des1, 
event->des2, start, type, esynetMsgNo, event->addr>>cache_dl2->set_shift, 
event->operation, vc);
#ifdef MULTI_VC
                    OrderBufInsert(event->src1, event->src2, event->des1, 
event->des2, event->addr&~(cache_dl1[0]->bsize-1), vc, esynetMsgNo);
#endif
					event->when = start + WAIT_TIME;
				}
			}

		}
		else
			event->when = start;
        event->esynetMsgNo = esynetMsgNo;
	}

	/*collected all the statistics for total number of packets*/
	if(type == data_packet_size)
	{
		TotalDataPacketsInAll ++;
		data_packets_sending[event->src1*(mesh_size+2)+event->src2][event->des1*(mesh_size+2)+event->des2] ++;
		if(event->isSyncAccess)
			TotalSyncDataPackets ++;
	}
	else
	{
		TotalMetaPacketsInAll ++;
		meta_packets_sending[event->src1*(mesh_size+2)+event->src2][event->des1*(mesh_size+2)+event->des2] ++;
		if(event->isSyncAccess)
			TotalSyncMetaPackets ++;
	}
}



int MsgComplete(long w, long x, long y, long z, counter_t stTime, long long int msgNo, counter_t transfer_time, int delay, counter_t req_time)
{
    /* optical network event complete */
	struct DIRECTORY_EVENT *event, *prev;
	event = dir_event_queue;
	prev = NULL;
	if (event!=NULL)
	{
		while (event)
		{
			int equal_flag = 0;
			if(!mystricmp(network_type, "COMB"))
			{
				int Chip_identify = ConfigCheck(event->src1, event->src2, event->des1, event->des2);
				if(Chip_identify == FSOI_NET_INTER)
				{
                    if(event->new_src1 == w && event->new_src2 == x && 
event->new_des1 == y && event->new_des2 == z && event->startCycle == stTime && 
!event->processingDone && msgNo == event->esynetMsgNo)
						equal_flag = 1;
				}
				else
				{
                    if(event->src1 == w && event->src2 == x && event->des1 == y 
&& event->des2 == z && event->startCycle == stTime && !event->processingDone && 
msgNo == event->esynetMsgNo)
						equal_flag = 1;
				}
			}
			else
			{
                if(event->src1 == w && event->src2 == x && event->des1 == y && 
event->des2 == z && event->startCycle == stTime && !event->processingDone && 
msgNo == event->esynetMsgNo)
					equal_flag = 1;
			}
		
			if(equal_flag)
			{
				struct DIRECTORY_EVENT *temp = event->next;
				event->next = NULL;
				int return_value = dir_fifo_enqueue(event, 0);
				if(return_value == 0)
				{
					event->next = temp;
					return 0;
				}
				else
				{
					/* statistics */
					totalOptHopCount++;  //total optical network accesses 
					if(event->operation == ACK_DIR_WRITEUPDATE)
						totalOptHopDelay += 1; //toal optical network delay for acknowledgement is 1 for INV_ACK_CON optimization 
					else
						totalOptHopDelay += (sim_cycle - event->startCycle);  //total optical network delay 
					if(event->store_cond)
					{ /* network access and delay count for sync messages */
						totalOptSyncHopCount++;
						if(event->operation == ACK_DIR_WRITEUPDATE)
							totalOptSyncHopDelay += 1;
						else
							totalOptSyncHopDelay += (sim_cycle - event->startCycle);
					}
					else
					{ /* network access and delay count for normal messages */
						totalOptNorHopCount++;
						if(event->operation == ACK_DIR_WRITEUPDATE)
							totalOptNorHopDelay += 1;
						else
							totalOptNorHopDelay += (sim_cycle - event->startCycle);
					}
					//if(event->operation == ACK_DIR_WRITE || event->operation == ACK_DIR_READ_SHARED || event->operation == ACK_DIR_READ_EXCLUSIVE || event->operation == ACK_DIR_IL1 || event->operation == ACK_MSG_WRITE || event->operation == ACK_MSG_READ || event->operation == WAIT_MEM_READ || event->operation == WAIT_MEM_READ_N)
					//	reply_statistics(event->req_time);
					event->processingDone = 1;
					event->transfer_time = transfer_time;
					event->delay = delay;
					event->req_time = sim_cycle;
					event->arrival_time = sim_cycle;

					if(prev == NULL)
						dir_event_queue = temp;
					else
						prev->next = temp;
					/* coherence optimization for optical network, try to send early notification for write upgrade and write miss to indicate the write permission through confirmation laser */
					if(!mystricmp(network_type, "FSOI") || (!mystricmp(network_type, "HYBRID")))
					{

					}
					return 1;
				}
			}
			prev = event;
			event = event->next;
		}
	}
	panic("Error finding event for the returned message from network");
}
int simEsynetMsgComplete(long w, long x, long y, long z, counter_t stTime, 
long long int msgNo)
{
	/* mesh network event complete */
	struct DIRECTORY_EVENT *event, *prev;
	event = dir_event_queue;
	prev = NULL;
	if (event!=NULL)
	{
		while (event)
		{
			int equal_flag = 0;
//            if ( event->startCycle == stTime && !event->processingDone && 
//msgNo == event->esynetMsgNo )
//            {
//            printf( "\t%d-%d\t%d-%d\t%d-%d\t%d-%d\t%d-%d\t%d-%d\n", 
//event->src1, w, event->src2, x, event->des1, y, event->des2, z, 
//event->startCycle, stTime, msgNo, event->esynetMsgNo );
//            }
            if(!mystricmp(network_type, "COMB"))
			{
                int Chip_identify = ConfigCheck(event->src1, event->src2, 
event->des1, event->des2);
				if(Chip_identify == MESH_NET_INTER)
				{
                    if(event->new_src1 == w && event->new_src2 == x && 
event->new_des1 == y && event->new_des2 == z && event->startCycle == stTime && 
!event->processingDone && msgNo == event->esynetMsgNo)
						equal_flag = 1;
				}
				else
				{
                    if(event->src1 == w && event->src2 == x && event->des1 == y 
&& event->des2 == z && event->startCycle == stTime && !event->processingDone && 
msgNo == event->esynetMsgNo)
						equal_flag = 1;
				}
			}
			else
			{
                if(event->src1 == w && event->src2 == x && event->des1 == y && 
event->des2 == z && event->startCycle == stTime && !event->processingDone && 
msgNo == event->esynetMsgNo)
					equal_flag = 1;
			}
		
			if(equal_flag)
			{
				struct DIRECTORY_EVENT *temp = event->next;
				if(dir_fifo_enqueue(event, 0) == 0)
					return 0;
				else
				{
#ifdef MULTI_VC
					OrderBufRemove(w, x, y, z, event->addr&~(cache_dl1[0]->bsize-1), msgNo);
#endif
					totalMeshHopCount++;
					totalMeshHopDelay += (sim_cycle - event->startCycle);
					if(event->store_cond)
					{
						totalMeshSyncHopCount++;
						totalMeshSyncHopDelay += (sim_cycle - event->startCycle);
					}
					else
					{
						totalMeshNorHopCount++;
						totalMeshNorHopDelay += (sim_cycle - event->startCycle);
					}
					event->processingDone = 1;
					if(prev == NULL)
						dir_event_queue = temp;
					else
						prev->next = temp;
					if(event->operation == WRITE_UPDATE)
						totalNetUpgrades++;
					return 1;
				}
			}
			prev = event;
			event = event->next;
		}
	}

#ifdef LOCK_HARD
	if(!LockMsgComplete(w, x, y, z, msgNo))
		panic("Error finding event for the returned message from network");
#endif

}

#define CACHE_HASH(cp, key)						\
	(((key >> 24) ^ (key >> 16) ^ (key >> 8) ^ key) & ((cp)->hsize-1))





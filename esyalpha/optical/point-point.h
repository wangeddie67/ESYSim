/******************************************************************/
/* This function is used for optical point to point communication */
/* March 04 2008           by Jing  */
/******************************************************************/


/******************************************************************/
/* Data Stucture */
/******************************************************************/
#ifndef _POINT_POINT_H_

#define _POINT_POINT_H_

#include "../host.h"
#include "../smt.h"
#include "../machine.h"
#include "../options.h"
#include "../cache.h"
#include "../syscall.h"
#include "../context.h"
#include <stdlib.h>
#include <time.h>
extern int mesh_size;
#define MEM_MOD
/* ********* *********/
#define MESHNUM 		(MESH_SIZE+2)  		//mesh dimentional e.g. 4 is 4*4 mesh
#ifdef MEM_MOD
#define PROCESSOR 		(MESHNUM*MESHNUM)
#else
#define PROCESSOR 		MAXTHREADS	//Processors
#endif

//#define SYNC_LOCK_OPT
//#define PREFETCHER_OPT

/* ********* conflict resolution *********/
//#define IDEALCASE

/* ********* baseline optical interconnect definition **********/
#ifdef BASELINE_OPTICAL_NETWORK
#ifndef IDEALCASE
#define CONF_RES_ACK
//	#define EMC		//can not be enable if the meta packet is transfered in one cycle
//	#define CONF_PREDICTOR		//confirmation laser used as a conflict hint --- optimization
#endif

#define ORDER_ADDR			//Ordering by address
#define PREORITY_PACKETS		//prefetch packets have lower preority
#endif

/* ********* optimized optical interconnect definition ***********/
#ifdef OPTIMIZED_OPTICAL_NETWORK
#define CONF_RES_ACK
#define EMC
#define CONF_PREDICTOR		//confirmation laser used as a conflict hint --- optimization
#define PSEUDO_SCHEDULE  		//probability scheduling --- optimization
/* options:
	//#define PSEUDO_SCHEDULE_REORDER //reorder probability scheduling
	//#define PSEUDO_SCHEDULE_DATA	//Intelligent delay (scheduling)
	//#define WB_REQ_DELAY
	//#define L1_L2_CONF//L1 and L2 reply packets conflict resolution
*/

#define ORDER_ADDR			//Ordering by address
//#define ORDER_NODE
/* options :
	//#define ORDER_NODE	//Ordering by node
	//#define ORDER_ADDR	//Ordering by address
*/
#define PREORITY_PACKETS		//prefetch packets have lower preority
#endif

//#define UNLIMIT_PORTS  //unlimited ports for output buffer

//#define PER_SCHEDULE   //perfect scheduling for data packets
//#define CORONA
#ifdef CORONA
arbi_delay[PROCESSOR];
#endif


#ifdef CONF_PREDICTOR
//#define PRE_ID
//#define SEND_IMM
#define FULL_CHECK
#define WB_PENDING
#define ACK_PENDING
#define CONF_PRED_TABLE
int ConfRecord[64];
#endif

counter_t large_retry;
counter_t small_retry;
counter_t acrossUsed;
#define CONF_WIDTH		4
int confirmation_bit_inf[PROCESSOR];	/* confimration bits can contain useful informations */
counter_t bit_overflow;
counter_t bit_used;
counter_t transmitter_busy;
counter_t wb_close;
extern int meta_total_num;
extern int meta_collision_total;
extern int data_total_num;
extern int data_collision_total;


/* network configuration parameters */

/* flit unit */
#define HEADER 			1		//flit unit header
#define TAIL 			2		//flit unit tail
#define BODY 			3		//flit unit body
#define CONF_TYPE		4

/* packet type */
#define META 			0		//Meta packet type
#define DATA 			1		//data packet type

#define FIXED			0
#define RANDOM			1

/* message type */
#define EVG_MSG 		0		//adding packet into sender
#define META_MSG 		1		//receiving meta packet
#define DATA_MSG 		2		//receiving data packet
#define WIRE_MSG		3
#define ACK_MSG			4
#define MSG_BACKUP		5
#define WB_MSG			6
#define WB_REQ_MSG		7
#ifdef CONF_PREDICTOR
#define CONF_HINT		8
#define EARLY_ACK_MSG		9
#endif
#ifdef EMC
#define EMC_ACK_MSG		10
#endif
/* laser on or off */
#define LASER_ON		1
#define LASER_OFF		0

#define BUFFER_SIZE		256  		//Input buffer packet entries
#define WAIT_TIME		800000000
#define PENDING_SIZE		64		//how many address are going to the same cache line

#define maxFlits 300000
int currentFlits;

int input_extra_overflow;
extern int cache_dl1_set_shift;

extern FILE *fp_trace;
extern struct DIRECTORY_EVENT *dir_event_queue;  /* Head pointer point to the directory event queue */
extern counter_t Output_queue_full;
counter_t last_Output_queue_full[PROCESSOR];
counter_t more_than_two_conflict;
counter_t meta_more_than_two_conflict;
counter_t meta_sync_conf_event;
counter_t meta_sync_conf_packets;
counter_t meta_ldl_more_than_two_conflict;
counter_t meta_ldl_conf_event;
counter_t meta_ldl_conf_packets;
counter_t sync_conf_packets;
counter_t sync_conf_event;
extern counter_t Stall_output_queue;
#define R_R_STATISTIC
#ifdef R_R_STATISTIC
counter_t reply_time_distribution[250];
counter_t reply_L1_time_distribution[50];
#endif
counter_t queue_distr[24];

int meta_conf_event;
int data_conf_event;
counter_t data_conflict_pro;
counter_t meta_conflict_pro;

counter_t input_buf_access;
counter_t output_buf_access;
counter_t output_buffer_flits[PROCESSOR];
/* network delay */
int laser_warmup_time;

int laser_setup_time;

int laser_switchoff_timeout;

int flit_tranfer_time;
int data_flit_tranfer_time;

int laser_cycle;

int meta_transmitter_size;
int data_transmitter_size;

int flit_size;
#ifndef ESYNET
int data_packet_size;

int meta_packet_size;
#endif

int confirmation_bit;

int input_buffer_size;

int output_buffer_size;
int output_ports;

int packet_queue_size;

int meta_receivers;

int data_receivers;

int transmitters;

int confirmation_trans;

int confirmation_receiver;

int time_slot_data;

int data_chan_timeslot;
int meta_chan_timeslot;

int exp_backoff;

float first_slot_num;

int retry_algr;

int TBEB_algr;

int confirmation_time;

int conf_ack_packets;
counter_t conf_ack_packets_read;
int conf_upgrades;
int conf_readupgrades;
int conf_sync_lock;
int acknowledge_packets;
/* *********************************data structure ***********************/
/* flit unit */
typedef struct Flit
{
    int flit_type;
    struct Flit *next;
    int Num;
    int PID;
} Flit;

/* flit list */
typedef struct Flit_list
{
    Flit *head;
    Flit *tail;
    int flits_in_list;
} Flit_list;

struct Flit_list free_flit_list;
void allocate_free_flit_list();
Flit * allocate_flit();

/* packet unit */
typedef struct Packet
{
#ifdef EXCLUSIVE_OPT
    int exclusive_after_wb;
#endif
    counter_t start_time;
    counter_t transfer_time;
    counter_t transmitter_time;
    md_addr_t addr;
    int packet_type;
    unsigned long packet_size;
    long src;
    long des;
    int flitnum;
    int msgno;
    int meta_id;
    int data_id;
    int retry;
    int pending[PENDING_SIZE];
    int con_flag[4];
    int retry_num;
    int operation;
    int channel;
    int meta_delay;
    int data_delay;
    int prefetch;
    /* data packet conflict reason statistics*/
    counter_t arrival_time; /* L2 fifo delay */
    int queue_time; /* queue delay */
    int steering_time; /* steering delay */
    int miss_flag; /* L2 miss flag */
    int dirty_flag; /* L2 dirty flag */
    int ts_delay;
    int data_reply;
    int conf_bit;
    int conf_hint;
    int early_conf;
    int schedule;
    int escape_channel;
    int isSyncAccess;
    int ldl;
    int SyncLock;
} Packet;

/* message struct */
typedef struct Msg
{
    int msg_type;
    counter_t when;
    struct Msg *next;
    struct Msg *pre;
    struct Packet packet;
} Msg;

/* message list */
typedef struct Msg_list
{
    Msg *head;
    Msg *tail;
} Msg_list;

/* two alogism parameters from configuration file */
int Map_algr;
int network_algr;

struct Input_entry_t
{
    int isValid;
    Packet packet;
    counter_t time_stamp;
};

struct Input_buffer
{
    int flits_in_buffer;
    int packets_in_buffer;
    struct Input_entry_t InputEntry[BUFFER_SIZE];
};

/* statistics counters */
counter_t last_packet_cycle[PROCESSOR][2];
counter_t dist_bin[20];
counter_t channel_occupancy;
counter_t meta_packets;
counter_t data_packets;
counter_t meta_packet_transfer;
counter_t data_packet_transfer;
counter_t channel_busy_data[PROCESSOR];
counter_t channel_busy_meta[PROCESSOR];

counter_t req_conf[PROCESSOR];
counter_t data_conf[PROCESSOR];
counter_t data_conf_involves[PROCESSOR][PROCESSOR];
counter_t write_back_involves[PROCESSOR][PROCESSOR];
counter_t write_back_inv_bincount[3];
counter_t data_conf_inv_bincount[7];
counter_t data_way_conf[PROCESSOR];
counter_t meta_way_conf[PROCESSOR];
counter_t bin_count_end[PROCESSOR][PROCESSOR];
counter_t bin_count_start[PROCESSOR][PROCESSOR];
counter_t req_cycle_conflicts[PROCESSOR];
counter_t data_cycle_conflicts[PROCESSOR];
counter_t last_cycle[PROCESSOR][4];
Msg *conf_list[PROCESSOR][4];
counter_t no_write_back;
counter_t all_write_back;
counter_t partial_write_back;
counter_t no_solution_pwb;

/* wb split statistics */
counter_t total_wb_msg;
counter_t wb_split_conf_event;
counter_t WbSplitConf;
counter_t WbSplitFailConf;
counter_t total_wbhead_msg;
counter_t total_wb_head_success;
counter_t total_wb_head_fail;
counter_t total_wbreq_confirmation;
counter_t total_wbreq_convention;
counter_t total_wb_req_convention_success;
counter_t total_wb_req_convention_fail_event;
counter_t total_wb_req_convention_fail_time;
/* mem split statistics */
counter_t total_mem_msg;
counter_t mem_split_conf_event;
counter_t MemSplitConf;
counter_t MemSplitFailConf;
counter_t total_memhead_msg;
counter_t total_mem_head_success;
counter_t total_mem_head_fail;
counter_t total_memreq_confirmation;
counter_t total_memreq_convention;
counter_t total_mem_req_convention_success;
counter_t total_mem_req_convention_fail_event;
counter_t total_mem_req_convention_fail_time;


int max_conf;
int max_packet_retry;
int max_packets_inbuffer;
int max_flits_inbuffer;
/* conflict reason */
counter_t MemPckConfEvent;
counter_t MemSecPckConfEvent;
counter_t WriteBackConfEvent;
counter_t RetransmissionConfEvent;
counter_t ReplyConfEvent;

counter_t retry_conflict;
counter_t L1_L2_conflict;
counter_t all_L2_miss_conflict;
counter_t L2_miss_conflict;
counter_t L2_dirty_conflict;
counter_t queue_delay_conflict;
counter_t l2_fifo_conflict;
counter_t fifo_queue_steering_conflict;
counter_t steering_queue_conflict;
counter_t queue_conflict;
counter_t wrong_conflict;
counter_t request_close_conflict;
counter_t max_queue_delay;
//Packet *packet_conflict[PROCESSOR][PROCESSOR][PROCESSOR];
struct packet_conflict_list
{
    Packet packet[PROCESSOR];
    int packet_num;
} packet_conflict[PROCESSOR][5];

counter_t last_data_time[PROCESSOR];
counter_t last_time[PROCESSOR];
counter_t RT_time[PROCESSOR][PROCESSOR]; /* Request transfer time */
counter_t RTCC; /* Request time clost to each other counter */
counter_t RTCC_fail; /* Request time clost to each other counter */
counter_t RTCC_half; /* Request time clost to each other counter */
counter_t RTCC_miss; /* Request time clost to each other counter */
counter_t RTCC_reduced; /* Request time clost to each other counter */
counter_t RTCC_flag[PROCESSOR];
counter_t meta_close_packets;
counter_t meta_reduced_packets;
counter_t data_packet_generate;
extern counter_t global_data_packets_generation;
counter_t data_packet_generate_max;

/* network delay statistics counters */
counter_t network_queue_delay;
counter_t output_queue_delay;
counter_t data_network_queue_delay;
counter_t meta_network_queue_delay;
counter_t pending_delay;
counter_t steering_delay;
counter_t data_confliction_delay;
counter_t meta_confliction_delay;
counter_t laser_switch_on_delay;
counter_t timeslot_wait_time;
counter_t meta_timeslot_wait_time;

counter_t network_queue_Delay[PROCESSOR];
counter_t output_queue_Delay[PROCESSOR];
counter_t data_network_queue_Delay[PROCESSOR];
counter_t meta_network_queue_Delay[PROCESSOR];
counter_t pending_Delay[PROCESSOR];
counter_t steering_Delay[PROCESSOR];
counter_t data_confliction_Delay[PROCESSOR];
counter_t meta_confliction_Delay[PROCESSOR];
counter_t laser_switch_on_Delay[PROCESSOR];
counter_t timeslot_wait_Delay[PROCESSOR];
counter_t meta_timeslot_wait_Delay[PROCESSOR];

counter_t no_queue_delay;
counter_t network_delay_in_fiber;
counter_t confliction_delay;
counter_t data_conflict_packet_num;
counter_t meta_conflict_packet_num;
counter_t mem_data_conflict;
counter_t mem_req_conflict;
counter_t mem_data_retry;
counter_t mem_req_retry;
counter_t reply_data_packets;
counter_t original_reply_data_packets;
counter_t write_back_packets;
counter_t original_write_back_packets;
counter_t data_retry_packet_num;
counter_t meta_retry_packet_num;
counter_t pending_packet_num;
counter_t wbreq_timeslot_wait_time;
counter_t packets_sending[PROCESSOR];
counter_t packets_sending_bin_count[5];
counter_t last_packet_time[PROCESSOR];
counter_t last_close_time[PROCESSOR];
counter_t last_meta_packet[3][PROCESSOR];
counter_t next_meta_packet[3][PROCESSOR];
counter_t last_meta_packet_fake[PROCESSOR];
counter_t meta_wait_time[PROCESSOR];
counter_t backup_transfer_waittime[PROCESSOR];
counter_t last_delay_for_data[PROCESSOR];
counter_t transmitter_backup_schedule[PROCESSOR];
counter_t first_packet_time[PROCESSOR];
counter_t retry_data_queue;
counter_t retry_meta_queue;
counter_t receiver_occ_time[PROCESSOR][PROCESSOR];
counter_t meta_delay_packets;
counter_t wbreq_delay_packets;
counter_t meta_l1_delay_packets;
counter_t meta_packets_forward;
counter_t data_packets_forward;

/* power statistics */
counter_t total_cycle_laser_power_on;
counter_t laser_power_switchon_counter;
counter_t laser_power_switchoff_counter;
counter_t laser_direction_switch_counter;
counter_t flits_transmit_counter;
counter_t flits_receiving_counter;
counter_t data_packets_delay;
counter_t data_packets_no_delay;
counter_t data_delay;
counter_t total_cycle_conf_laser_power_on;
counter_t conf_laser_power_switchon_counter;
counter_t conf_laser_power_switchoff_counter;

/* central message list */
Msg_list *msg_list;

/* input buffer */
Flit_list  transmitter_meta[PROCESSOR];
Flit_list  transmitter_data[PROCESSOR];
Flit_list  transmitter_backup[PROCESSOR];
struct Input_buffer input_buffer[PROCESSOR];
int last_packet_des[PROCESSOR];
int last_meta_packet_des[3][PROCESSOR];
int last_meta_packet_des_fake[PROCESSOR];
int last_packet_des_backup[PROCESSOR];
counter_t delay_counter[2][PROCESSOR];
counter_t delay_count[2][PROCESSOR][PROCESSOR];
counter_t delay_counter_backup[PROCESSOR];

/* output buffer */
Flit_list  out_buffer[PROCESSOR];
Packet packet_queue[PROCESSOR][256];
int packet_queue_tail[PROCESSOR];
int packet_queue_head[PROCESSOR];
int packet_queue_num[PROCESSOR];
int input_buffer_num[PROCESSOR][2];

#ifdef CONF_PREDICTOR

#define MAXRECEIVERS	PROCESSOR*3
#define REQ_QUEUE_SIZE  256
struct ReqPending
{
    int valid;
    int des;
    md_addr_t addr;
    counter_t time;
    int op_code;
};

counter_t conflict_table[MAXRECEIVERS][PROCESSOR][PROCESSOR];
counter_t meta_conflict_table[MAXRECEIVERS][PROCESSOR][PROCESSOR];
counter_t meta_ldl_conflict_table[MAXRECEIVERS][PROCESSOR][PROCESSOR];


#ifdef CONF_PRED_TABLE
struct ConfPredTable
{
    int LastPred;
    int LLastPred;
    int counter;
    int Lcounter;
};

struct ConfPredTable confpred[MAXRECEIVERS];
#endif
counter_t tencase;
counter_t MultiCase[PROCESSOR];

struct ReqPending ReqPendingQueue[PROCESSOR][REQ_QUEUE_SIZE];
int ReqPendingQueue_num[PROCESSOR];
int MaxReqPending;

int conf_packet_list[MAXRECEIVERS][PROCESSOR];
Packet conf_packet[MAXRECEIVERS][PROCESSOR];
int conf_packet_num[MAXRECEIVERS];
int conf_PID[MAXRECEIVERS];
int ConfWait[MAXRECEIVERS];
counter_t wb_retry_conf;
counter_t predict_correct;
counter_t predict_fail;
counter_t predictCorrect_two;
counter_t predictCorrect_more;
counter_t predictFail_two;
counter_t predictFail_more;
counter_t predict_table_correct;
counter_t predict_req_correct;
counter_t hint_received;
counter_t hint_received_wrong;
counter_t packet_conf_hint_conf;
counter_t packet_conf_hint_suc;
counter_t MultiPendingError;
counter_t WbPendingError;
counter_t ReqPendingError;
counter_t ReqPendingError2;
counter_t TotalReqPendingNum;
counter_t ReqPendingBin[12];
void ConfPacketListInsert(Packet packet, int PID);
void ConfPacketListClean(counter_t sim_cycle);
void ConfPredictor(counter_t sim_cycle);
int PredictTable(int pid, int pid_bar, int seed, int m);
int PidTableCheck(int PID, int src, int j, counter_t sim_cycle);
void input_buffer_check(Packet packet, counter_t sim_cycle);
void conf_hint(Packet packet, counter_t sim_cycle);
#endif
#ifdef EMC
int meta_conf_packet_list[MAXRECEIVERS][PROCESSOR];
Packet meta_conf_packet[MAXRECEIVERS][PROCESSOR];
int meta_conf_packet_num[MAXRECEIVERS];
void MetaConfPacketListInsert(Packet packet, int PID);
void MetaConfPacketListClean(counter_t sim_cycle);
void MetaEarlyConf(counter_t sim_cycle);
#endif

#if (MAXTHREADS == 1)
#define PID_SIZE 0
#define PID_MASK 2
#endif
#if (MAXTHREADS == 2)
#define PID_SIZE 1
#define PID_MASK 4
#endif
#if (MAXTHREADS == 4)
#define PID_SIZE 3
#define PID_MASK 8
#endif
#if (MAXTHREADS == 8)
#define PID_SIZE 4
#define PID_MASK 16
#endif
#if (MAXTHREADS == 16)
#define PID_SIZE 5
#define PID_MASK 32
#endif
#if (MAXTHREADS == 32)
#define PID_SIZE 6
#define PID_MASK 64
#endif
#if (MAXTHREADS == 64)
#define PID_SIZE 7
#define PID_MASK 128
#endif
#if (MAXTHREADS == 80)
#define PID_SIZE 8
#define PID_MASK 256
#endif
#if (MAXTHREADS == 128)
#define PID_SIZE 8
#define PID_MASK 256
#endif
#if (MAXTHREADS == 256)
#define PID_SIZE 9
#define PID_MASK 512
#endif
#define PID_shift	PID_SIZE
#define PID_bar_mask	(PID_MASK - 1)
int PID_form(int PID);

/* data and meta receivers */
Flit_list  *data_receiver[PROCESSOR], *meta_receiver[PROCESSOR];

int laser_power[2][PROCESSOR];
counter_t laser_power_on_cycle[2][PROCESSOR];
counter_t laser_power_start_cycle[2][PROCESSOR];
int conf_laser_power[2][PROCESSOR];
counter_t conf_laser_power_on_cycle[2][PROCESSOR];
counter_t conf_laser_power_start_cycle[2][PROCESSOR];
/***********************************function **************************/
/* network configuration inputfile */
void optical_options(struct opt_odb_t *odb);

void optical_reg_stats (struct opt_odb_t *sdb);
int packet_retry_algr(Packet packet);

/* flit list handling functions */
int add_flit(Flit *flit, Flit_list *flit_list);
int free_flit(Flit *flit, Flit_list *flit_list);


/* adding message to central message list */
void add_message(counter_t start_time, int msg_type, Packet packet);
/* free message from central message list */
void free_message(Msg *oldmsg);

void add_packet(Packet packet, long des);
void free_packet(long des);

/* when the sim_main started, call maindirectsim function to initialize the message list and flit list */
void maindirectsim(int map_algr, int net_algr);
void Msg_list_init(Msg_list *msg_list);
void flit_list_init(Flit_list flit_alist);

/* check if there is packet conflits at receiver */
int conflict_check(Packet *packet, counter_t when);
void conflict_type_change(Flit_list *flit_list, int flit_id);
int cycle_conflict_check(Packet packet, counter_t when, int receivers);

void conlict_bincount_collect(counter_t sim_cycle);
/* insert packets into input buffer */
void input_buffer_add(Packet packet, counter_t time_stamp);
/* free packet free input buffer */
void input_buffer_free(Packet packet, counter_t sim_cycle);
int is_appending_addr(Packet packet);
void input_buffer_lookup(long src, counter_t sim_cycle);
void retry_packet(int msgno, counter_t sim_cycle, long src);

/* handling EVG_MSG, adding packet into sending flit list, adding receive message into the message list */
void receive_msg(Packet packet, counter_t sim_cycle);
/* Laser warm up and set up delay */
int setup_delay(int src, int des, counter_t sim_cycle, int p_type);
/* channel busy check */
int channel_busy_check(int src, int des, int type, counter_t sim_cycle);
//int channel_busy_check(int src, int type);
/* handing DATA_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_data(Packet pck, counter_t sim_cycle);
/* handing META_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_meta(Packet pck, counter_t sim_cycle);
/* handling RECIVEING FLIT MSG */
void receive_flit_msg(Packet pck, counter_t sim_cycle);
#ifdef CONF_RES_ACK
/* Handling acknowledge message */
void receive_ack_msg(Packet pck, counter_t sim_cycle);
#endif

/* first receive the packet from the processor, add EVG_MSG into message list */
void directMsgInsert(long s1, long s2, long d1, long d2, counter_t sim_cycle, long size, long long int msgNo, md_addr_t addr, int operation, int delay, int L2miss_flag, int prefetch, int dirty_flag, counter_t arrival_time, int data_reply, int conf_bit, int escape_channel, int even_flag, int isSyncAccess, int SyncLock
#ifdef EXCLUSIVE_OPT
                     ,int exclusive_after_wb
#endif
                    );
/* call this function at every cycle, check it message list to operate message */
void directRunSim(counter_t sim_cycle);
int opticalBufferSpace(long des1, long des2, int opt);
/* when it receives its packet successfully, call the finishMsg function to wake up the event */
int finishMsg(long src, long des, counter_t start_time, counter_t esynetmsgno, 
int packet_type, counter_t transfer_time, int meta_delay, counter_t req_time, 
int operation, int out_buffer_dec);
#ifdef OPTICAL_INTERCONNECT
void free_out_buffer_flit(int iteration, long des);
#endif

/* sending the flits */
void sending_flit(Packet packet, counter_t sim_cycle);
/* receiving the flits */
void receive_flit(Packet packet, counter_t sim_cycle);

/* map the receiver for packet */
int receiver_map(long src, long des, int receiver, md_addr_t addr);

#endif

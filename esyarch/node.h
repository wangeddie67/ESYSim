/*
 * node.h
 *
 *  Created on: 2016年2月28日
 *      Author: root
 */

#ifndef NODE_H_
#define NODE_H_
/*
 * header file
 * */
#include <stdio.h>

#include <errno.h>
#include <math.h>
#include <assert.h>
#include <signal.h>
#include <limits.h>

/*eio.cc*/
#ifdef _MSC_VER
#include <io.h>
#else /* !_MSC_VER */
#include <unistd.h>
#endif

#include "host.h"//ind
#include "bitmap.h"//ind
#include "eventq.h"//ind

#include "misc.h"//ind
#include "resource.h"//ind
#include "define.h"//ind
#include "node_cfg.h"
//#include "eio.h"
//#include "version.h"//不能包含，因为version.h中有ifdef target_pisa
//#include "regs.h"
//#include "memory.h"
//#include "cache.h"
////#include "bpred.h"
#include "Statistics.h"
#include "power_mc.h"//功耗统计/计算类
#include "Simulation.h"
#include "esynetRecord.h"
/*misc.cc - misc.h和machine.h无关，但是misc.cc和machine.h有关,新建的misc_c.h用于存放misc.cc中的一些值，node.h
 * 由于是最顶层的类，而misc.cc中有大部分和machine.h无关，但是与机器有关，所以放在了node.h中（比如随机种子产生器）*/

#include "esy_soccfg.h"


/*
 * class define
 * */

class TNode 
{
public:
	TNode(int exist_t,CoreStruct* cs);
	TNode(int exist_t, FpgaStruct* fs);
	//id,masterid,actualid should be place here
	int exist;
	int running,actualid,masterid;
	int tileid;
	time_t sim_start_time;
	time_t sim_end_time;
	int sim_exit_now;// = FALSE;
	int sim_dump_stats;// = FALSE;
	int init_quit;//0
	int frequence;
	int frequence_holder;
	double cycle_time;
	static int frequenceholder_lcm;
	core_tp ntype;
	char* name;
	char *sim_in;//NULL
	char *sim_out;//NULL
	/*
	 * running:是否运行，用于在程序运行过程中更改
	 * exist:是否存在，即此节点是否真实存在，如果不存在，那么只是一个路由器的形式展现
	 * exist的优先级高于running
	 * masterid:是MPI通信系统ID
	 * id:是线程id，放在了core类下
	 * */
	unsigned long start_cycle,finish_cycle,fastfwd_count;
	//exit状态记录
	static short int Exitstates[MAXTHREADS];
	time_t node_endtime;//仿真结束时间
	int sim_node_time;//仿真时间
	Statistics *statcpnt;//指向Statistics类
	tick_t sim_cycle;
	Power_mc *thispower;//指向功耗类
	//esynet 文件记录
	esynetRecord* esynetrcd;
	esynetRecord* esynetrcd_temp;
	//输出文件记录
	FILE *progout;
	FILE *statisticsout;
	FILE *inststreamout;
	char* powerout_fb;
	char* powerxml_4mcpat;
	FILE *powerout;
	FILE *powerxml;

	void regstatdb();
	void idle(int cycle);
	short int Getexitstates();//获取系统退出状态
	static void Setexitarray();
	static CoreStruct* New_corecfg(char * cfg[]);
    //根据main中读入文件产生的数组实例化一个结构体，实例化过程中进行检查,并输出配置
    static CoreStruct* New_corecfg(const EsySoCCfgTile & cfg);
	static FpgaStruct* New_fpgacfg(char * cfg[]);
    static FpgaStruct* New_fpgacfg(const EsySoCCfgTile & cfg);
	virtual sim_rt_t run();

//	virtual int Getmythreadid();//ID_TABLE相关

	virtual ~TNode();
};
/*TNode end*/
/*
	 * Define of SimpleScalar
	 * */

	#define STORE_HASH_SIZE		32
	#define MAX_IDEPS               3
	#define MAX_ODEPS               2



#include "sim-oodef.h"//contain undependent-ISA data and define.usually from sim-outorder.cc
class TCore : public TNode
{

public:
	/*
	 * typedef of SimpleScalar
	 * */
	typedef unsigned int INST_TAG_TYPE;
	typedef unsigned int INST_SEQ_TYPE;
	/*range.h*/
	enum range_ptype_t 
	{
	  pt_addr = 0,			/* address position */
	  pt_inst,			/* instruction count position */
	  pt_cycle,			/* cycle count position */
	  pt_NUM
	};
	struct range_pos_t 
	{
	  enum range_ptype_t ptype;	/* type of position */
	  counter_t pos;		/* position */
	};
	struct range_range_t 
	{
	  struct range_pos_t start;
	  struct range_pos_t end;
	};
	/*end of range.h*/


	/*cache.h*/
	enum cache_policy 
	{
	  LRU,		/* replace least recently used block (perfect LRU) */
	  Random,	/* replace a random block */
	  FIFO		/* replace the oldest block in the set */
	};
	//成员变量
	int threadid;
	/*eio.cc*/
	counter_t eio_trans_icnt;// = -1;
	/*ptrace.h depends on individual core*/
	FILE *ptrace_outfd;// = NULL;
	int ptrace_active;// = FALSE;
	/*ptrace.cc depends on individual core*/
	struct range_range_t ptrace_range;
	int ptrace_oneshot;// = FALSE;
	/*sim.h中涉及到的main.cc的变量*/
	int sim_swap_bytes;
	int sim_swap_words;
	char *sim_eio_fname;// = NULL;
	char *sim_chkpt_fname;// = NULL;
	FILE *sim_eio_fd;// = NULL;
	char *sim_progin;// = NULL
	char *sim_simout;// = NULL;
	char *sim_progout;// = NULL;
	char *sim_progfd;// = NULL;
//	static int exec_index;// = -1;
	int bugcompat_mode;// = 0/* operate in backward-compatible bugs mode (for testing only) */
/* context below */
	unsigned long ruu_fetch_issue_delay;// = 0
	/* speculation mode, non-zero when mis-speculating, i.e., executing
	   instructions down the wrong path, thus state recovery will eventually have
	   to occur that resets processor register and memory state back to the last
	   precise state */
	int spec_mode = FALSE;
	/* perfect prediction enabled */
	int pred_perfect = FALSE;
	int pref_mode;// = 0
	unsigned int inst_seq;// = 0 /* change when dispatch, allocate two num for ld/st? --hq*/
	unsigned int ptrace_seq;// = 0 /* pipetrace instruction sequence counter */
	enum { spec_ID, spec_WB, spec_CT } bpred_spec_update;
	int thread_id;
	int last_inst_missed;// = FALSE;
	int last_inst_tmissed;// = FALSE;
	static const int pcstat_nelt = 0;
//	static char *pcstat_vars[MAX_PCSTAT_VARS];
//程序中未有变量
//	unsigned long numInstrBarrier;
//	unsigned long numCycleBarrier;
//	unsigned long totalBarrierInstr;
//	unsigned long totalBarrierCycle;
//	unsigned long numInstrLock;
//	unsigned long numCycleLock;
//	unsigned long totalLockInstr;
//	unsigned long totalLockCycle;
//仿真器参数
	/*with out processor architecture*/
	char* testpath;//BNC path
	/*processor architecture*/
	//杂项
	unsigned long long max_insts;
	//	char *ptrace_opts[2]; ptrace在代码中配置是否需要，而不是配置文件中
	int fetch_speed;//1
	char *bpred_spec_opt;//NULL/ID/WB
	char *pred_type;//perfect/taken/nottaken/bimod(default)
	int bimod_config[1];// = {2048};/* bimod tbl size */
	int twolev_config[4];// = { /* l1size */1, /* l2size */1024, /* hist */8, /* xor */FALSE };2-level predictor config
	int comb_config[1];// = {1024};/* meta_table_size */
	int ras_size;// = 8;//return address stack size (0 for no return stack)
	int btb_config[2];// = { /* nsets */512, /* assoc */4 };//BTB config (<num_sets> <associativity>)
	//mem & mc
	int mem_lat[2];// = { /* lat to first chunk */18, /* lat between remaining chunks */2 };
	char *itlb_opt;// = "itlb:16:4096:4:l";
	char *dtlb_opt;// = "dtlb:32:4096:4:l";
	int tlb_miss_lat;// = 30;
	int mem_bus_width;// = 4;
//	char *bpred_spec_opt; bpred_spec_opt在代码中配置是否需要，而不是配置文件中
	//ruu
	int ruu_ifq_size;// = 4
	int ruu_branch_penalty;// =3
	int ruu_decode_width;// = 4;
	int ruu_issue_width;// = 4;
	int ruu_inorder_issue;// = 1;
	int ruu_include_spec;// = 1;
	int ruu_commit_width;// = 4;
	int RUU_size;// = 16;
	int LSQ_size;// = 8;
	//cache
	char *cache_dl1_opt;//"dl1:128:32:4:l"
	unsigned int cache_dl1_lat;// = 1;
	char *cache_dl2_opt;// = "ul2:1024:64:4:l";
	int cache_dl2_lat;// = 6;
	char *cache_il1_opt;// = "il1:512:32:1:l";
	int cache_il1_lat;// = 1;
	char *cache_il2_opt;// = "dl2";
	int cache_il2_lat;// = 6;
	int flush_on_syscalls;// = 0;//flush caches on system calls
	int compress_icache_addrs;// = 0;
	//执行单元/单元
	int res_ialu;
	int res_imult;
	int res_fpalu;
	int res_fpmult;
	int res_memport;
//以上是处理器参数

//流水线踪迹
	int ptrace_nelt;// = 0;
	char *ptrace_opts[2];

//loader.h
	int ld_target_big_endian;

/*functions*/
	TCore(int exist,CoreStruct* cs,int thread_id);
	void Creat_core(CoreStruct* cs);
	void sim_aux_config(FILE *stream);/* output stream */
	/*main.cc*/
//	void sim_print_stats(FILE *fd);
//	int Getmythreadid();//virtual
	virtual sim_rt_t run();
	virtual ~TCore();
};

#endif /* NODE_H_ */

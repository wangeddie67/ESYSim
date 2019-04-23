/*
 * pisa.cc
 *
 *  Created on: 2016年2月29日
 *      Author: root
 */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "host.h"
#include "misc.h"
#include "endian.h"//ind
#include "node.h"
#include "mips.h"
#include "mipsdef.h"
#include "MPITable.h"
#include "Noc.h"
#include "fpgac.h"
#include "power_mc.h"
//#include "network.h"
#include "libexo/libexo.h"
using namespace std;
extern MPITable *mt;
extern char** envp;
extern Fpgac* sim_asic[];
extern Power_mc *allpower;
extern jmp_buf sim_exit_buf,node_exit_buf;
extern unsigned long long simulation_cycle;
extern Noc glb_network;
extern long long int visual_hop;
extern int msg_send_num;
extern int msg_recv_num;

//初始化statis const成员变量
const TMIPS::RS_link TMIPS::RSLINK_NULL = {NULL,NULL,0};
const TMIPS::CV_link TMIPS::CVLINK_NULL = {NULL,0};
const TMIPS::md_reg_names_t TMIPS::md_reg_names[122]=
{
  /* name */	/* file */	/* reg */
  /* integer register file */
  { "$r0",	TMIPS::rt_gpr,		0 },
  { "$zero",	TMIPS::rt_gpr,		0 },
  { "$r1",	TMIPS::rt_gpr,		1 },
  { "$r2",	TMIPS::rt_gpr,		2 },
  { "$r3",	TMIPS::rt_gpr,		3 },
  { "$r4",	TMIPS::rt_gpr,		4 },
  { "$r5",	TMIPS::rt_gpr,		5 },
  { "$r6",	TMIPS::rt_gpr,		6 },
  { "$r7",	TMIPS::rt_gpr,		7 },
  { "$r8",	TMIPS::rt_gpr,		8 },
  { "$r9",	TMIPS::rt_gpr,		9 },
  { "$r10",	TMIPS::rt_gpr,		10 },
  { "$r11",	TMIPS::rt_gpr,		11 },
  { "$r12",	TMIPS::rt_gpr,		12 },
  { "$r13",	TMIPS::rt_gpr,		13 },
  { "$r14",	TMIPS::rt_gpr,		14 },
  { "$r15",	TMIPS::rt_gpr,		15 },
  { "$r16",	TMIPS::rt_gpr,		16 },
  { "$r17",	TMIPS::rt_gpr,		17 },
  { "$r18",	TMIPS::rt_gpr,		18 },
  { "$r19",	TMIPS::rt_gpr,		19 },
  { "$r20",	TMIPS::rt_gpr,		20 },
  { "$r21",	TMIPS::rt_gpr,		21 },
  { "$r22",	TMIPS::rt_gpr,		22 },
  { "$r23",	TMIPS::rt_gpr,		23 },
  { "$r24",	TMIPS::rt_gpr,		24 },
  { "$r25",	TMIPS::rt_gpr,		25 },
  { "$r26",	TMIPS::rt_gpr,		26 },
  { "$r27",	TMIPS::rt_gpr,		27 },
  { "$r28",	TMIPS::rt_gpr,		28 },
  { "$gp",	TMIPS::rt_gpr,		28 },
  { "$r29",	TMIPS::rt_gpr,		29 },
  { "$sp",	TMIPS::rt_gpr,		29 },
  { "$r30",	TMIPS::rt_gpr,		30 },
  { "$fp",	TMIPS::rt_gpr,		30 },
  { "$r31",	TMIPS::rt_gpr,		31 },

  /* floating point register file - single precision */
  { "$f0",	TMIPS::rt_fpr,		0 },
  { "$f1",	TMIPS::rt_fpr,		1 },
  { "$f2",	TMIPS::rt_fpr,		2 },
  { "$f3",	TMIPS::rt_fpr,		3 },
  { "$f4",	TMIPS::rt_fpr,		4 },
  { "$f5",	TMIPS::rt_fpr,		5 },
  { "$f6",	TMIPS::rt_fpr,		6 },
  { "$f7",	TMIPS::rt_fpr,		7 },
  { "$f8",	TMIPS::rt_fpr,		8 },
  { "$f9",	TMIPS::rt_fpr,		9 },
  { "$f10",	TMIPS::rt_fpr,		10 },
  { "$f11",	TMIPS::rt_fpr,		11 },
  { "$f12",	TMIPS::rt_fpr,		12 },
  { "$f13",	TMIPS::rt_fpr,		13 },
  { "$f14",	TMIPS::rt_fpr,		14 },
  { "$f15",	TMIPS::rt_fpr,		15 },
  { "$f16",	TMIPS::rt_fpr,		16 },
  { "$f17",	TMIPS::rt_fpr,		17 },
  { "$f18",	TMIPS::rt_fpr,		18 },
  { "$f19",	TMIPS::rt_fpr,		19 },
  { "$f20",	TMIPS::rt_fpr,		20 },
  { "$f21",	TMIPS::rt_fpr,		21 },
  { "$f22",	TMIPS::rt_fpr,		22 },
  { "$f23",	TMIPS::rt_fpr,		23 },
  { "$f24",	TMIPS::rt_fpr,		24 },
  { "$f25",	TMIPS::rt_fpr,		25 },
  { "$f26",	TMIPS::rt_fpr,		26 },
  { "$f27",	TMIPS::rt_fpr,		27 },
  { "$f28",	TMIPS::rt_fpr,		28 },
  { "$f29",	TMIPS::rt_fpr,		29 },
  { "$f30",	TMIPS::rt_fpr,		30 },
  { "$f31",	TMIPS::rt_fpr,		31 },

  /* floating point register file - double precision */
  { "$d0",	TMIPS::rt_dpr,		0 },
  { "$d1",	TMIPS::rt_dpr,		1 },
  { "$d2",	TMIPS::rt_dpr,		2 },
  { "$d3",	TMIPS::rt_dpr,		3 },
  { "$d4",	TMIPS::rt_dpr,		4 },
  { "$d5",	TMIPS::rt_dpr,		5 },
  { "$d6",	TMIPS::rt_dpr,		6 },
  { "$d7",	TMIPS::rt_dpr,		7 },
  { "$d8",	TMIPS::rt_dpr,		8 },
  { "$d9",	TMIPS::rt_dpr,		9 },
  { "$d10",	TMIPS::rt_dpr,		10 },
  { "$d11",	TMIPS::rt_dpr,		11 },
  { "$d12",	TMIPS::rt_dpr,		12 },
  { "$d13",	TMIPS::rt_dpr,		13 },
  { "$d14",	TMIPS::rt_dpr,		14 },
  { "$d15",	TMIPS::rt_dpr,		15 },

  /* floating point register file - integer precision */
  { "$l0",	TMIPS::rt_lpr,		0 },
  { "$l1",	TMIPS::rt_lpr,		1 },
  { "$l2",	TMIPS::rt_lpr,		2 },
  { "$l3",	TMIPS::rt_lpr,		3 },
  { "$l4",	TMIPS::rt_lpr,		4 },
  { "$l5",	TMIPS::rt_lpr,		5 },
  { "$l6",	TMIPS::rt_lpr,		6 },
  { "$l7",	TMIPS::rt_lpr,		7 },
  { "$l8",	TMIPS::rt_lpr,		8 },
  { "$l9",	TMIPS::rt_lpr,		9 },
  { "$l10",	TMIPS::rt_lpr,		10 },
  { "$l11",	TMIPS::rt_lpr,		11 },
  { "$l12",	TMIPS::rt_lpr,		12 },
  { "$l13",	TMIPS::rt_lpr,		13 },
  { "$l14",	TMIPS::rt_lpr,		14 },
  { "$l15",	TMIPS::rt_lpr,		15 },
  { "$l16",	TMIPS::rt_lpr,		16 },
  { "$l17",	TMIPS::rt_lpr,		17 },
  { "$l18",	TMIPS::rt_lpr,		18 },
  { "$l19",	TMIPS::rt_lpr,		19 },
  { "$l20",	TMIPS::rt_lpr,		20 },
  { "$l21",	TMIPS::rt_lpr,		21 },
  { "$l22",	TMIPS::rt_lpr,		22 },
  { "$l23",	TMIPS::rt_lpr,		23 },
  { "$l24",	TMIPS::rt_lpr,		24 },
  { "$l25",	TMIPS::rt_lpr,		25 },
  { "$l26",	TMIPS::rt_lpr,		26 },
  { "$l27",	TMIPS::rt_lpr,		27 },
  { "$l28",	TMIPS::rt_lpr,		28 },
  { "$l29",	TMIPS::rt_lpr,		29 },
  { "$l30",	TMIPS::rt_lpr,		30 },
  { "$l31",	TMIPS::rt_lpr,		31 },

  /* miscellaneous registers */
  { "$hi",	TMIPS::rt_ctrl,	0 },
  { "$lo",	TMIPS::rt_ctrl,	1 },
  { "$fcc",	TMIPS::rt_ctrl,	2 },

  /* program counters */
  { "$pc",	TMIPS::rt_PC,		0 },
  { "$npc",	TMIPS::rt_NPC,		0 },

  /* sentinel */
  { NULL,	TMIPS::rt_gpr,		0 }
};
const TMIPS::ss_flag_t TMIPS::ss_flag_table[SS_FLAG_NUM] = {
  /* target flag */	/* host flag */
#ifdef _MSC_VER
  { SS_O_RDONLY,	_O_RDONLY },
  { SS_O_WRONLY,	_O_WRONLY },
  { SS_O_RDWR,		_O_RDWR },
  { SS_O_APPEND,	_O_APPEND },
  { SS_O_CREAT,		_O_CREAT },
  { SS_O_TRUNC,		_O_TRUNC },
  { SS_O_EXCL,		_O_EXCL },
#ifdef _O_NONBLOCK
  { SS_O_NONBLOCK,	_O_NONBLOCK },
#endif
#ifdef _O_NOCTTY
  { SS_O_NOCTTY,	_O_NOCTTY },
#endif
#ifdef _O_SYNC
  { SS_O_SYNC,		_O_SYNC },
#endif
#else /* !_MSC_VER */
  { SS_O_RDONLY,	O_RDONLY },
  { SS_O_WRONLY,	O_WRONLY },
  { SS_O_RDWR,		O_RDWR },
  { SS_O_APPEND,	O_APPEND },
  { SS_O_CREAT,		O_CREAT },
  { SS_O_TRUNC,		O_TRUNC },
  { SS_O_EXCL,		O_EXCL },
  { SS_O_NONBLOCK,	O_NONBLOCK },
  { SS_O_NOCTTY,	O_NOCTTY },
#ifdef O_SYNC
  { SS_O_SYNC,		O_SYNC },
#endif
#endif /* _MSC_VER */
};


TMIPS::TMIPS(CoreStruct* cs,int thread_id)
:TCore(1,cs,thread_id),sim_num_insn(0), sim_num_refs(0), sim_num_branches(0), sim_total_insn(0), sim_total_refs(0),
 sim_total_loads(0), sim_total_branches(0), IFQ_count(0), IFQ_fcount(0), RUU_count(0), RUU_fcount(0),LSQ_count(0),
 LSQ_fcount(0),sim_invalid_addrs(0), sim_slip(0),
 //loader.cc
 ld_text_base(0),ld_text_size(0),ld_data_base(0),ld_data_size(0),ld_brk_point(0),ld_stack_base(MD_STACK_BASE),ld_stack_size(0),
 ld_stack_min((md_addr_t)-1),ld_prog_entry(0),ld_environ_base(0)

/*FIXME:Try brace-enclosed initializer list to initial failed*/
{
    // initlization
    for (int i = 0; i <= MD_MAX_MASK; i ++)
    {
         md_mask2op[i] = OP_NA;
    }
    
	int i;
	//esynetrecord
	esynetrcd = new esynetRecord(actualid);
	esynetrcd_temp = new esynetRecord(actualid);
//初始化上下文结构
	runcontext = (struct context*)malloc(sizeof(struct context));
	if(!runcontext)
    {
		fatal("runcontext initial error");
    }
	runcontext->last_op.next = NULL;
	runcontext->last_op.rs = NULL;
	runcontext->last_op.tag = 0;
	runcontext->bucket_free_list = NULL;
	sim_init();
//初始化资源池
	fu_name.IALU = "integer-ALU";
	fu_name.IMPU = "integer-MULT/DIV";
	fu_name.MEMPORT = "memory-port";
	fu_name.FPALU = "FP-addr";
	fu_name.FPMPU = "FP-MULT/DIV";
	//fu_config[0] config
	for(i=0;i<FU_RES_NUM;i++)
    {
		fu_config[i]%=NULL;
	}
	fu_config[0].name = fu_name.IALU;
	fu_config[0].quantity = cs->res_ialu;
	fu_config[0].busy = 0;
	fu_config[0].x[0].classt = IntALU;
	fu_config[0].x[0].oplat = 1;
	fu_config[0].x[0].issuelat = 1;
	//fu_config[1] config
	fu_config[1].name = fu_name.IMPU;
	fu_config[1].quantity = cs->res_imult;
	fu_config[1].busy = 0;
	fu_config[1].x[0].classt = IntMULT;
	fu_config[1].x[0].oplat = 3;
	fu_config[1].x[0].issuelat = 1;
	fu_config[1].x[1].classt = IntDIV;
	fu_config[1].x[1].oplat = 20;
	fu_config[1].x[1].issuelat = 19;
	//fu_config[2] config
	fu_config[2].name = fu_name.MEMPORT;
	fu_config[2].quantity = cs->res_memport;
	fu_config[2].busy = 0;
	fu_config[2].x[0].classt = RdPort;
	fu_config[2].x[0].oplat = 1;
	fu_config[2].x[0].issuelat = 1;
	fu_config[2].x[1].classt = WrPort;
	fu_config[2].x[1].oplat = 1;
	fu_config[2].x[1].issuelat = 1;
	//fu_config[3] config
	fu_config[3].name = fu_name.FPALU;
	fu_config[3].quantity = cs->res_fpalu;
	fu_config[3].busy = 0;
	fu_config[3].x[0].classt = FloatADD;
	fu_config[3].x[0].oplat = 2;
	fu_config[3].x[0].issuelat = 1;
	fu_config[3].x[1].classt = FloatCMP;
	fu_config[3].x[1].oplat = 2;
	fu_config[3].x[1].issuelat = 1;
	fu_config[3].x[1].classt = FloatCVT;
	fu_config[3].x[1].oplat = 2;
	fu_config[3].x[1].issuelat = 1;
	//fu_config[4] config
	fu_config[4].name = fu_name.FPMPU;
	fu_config[4].quantity = cs->res_fpmult;
	fu_config[4].busy = 0;
	fu_config[4].x[0].classt = FloatMULT;
	fu_config[4].x[0].oplat = 4;
	fu_config[4].x[0].issuelat = 1;
	fu_config[4].x[1].classt = FloatDIV;
	fu_config[4].x[1].oplat = 12;
	fu_config[4].x[1].issuelat = 12;
	fu_config[4].x[1].classt = FloatSQRT;
	fu_config[4].x[1].oplat = 24;
	fu_config[4].x[1].issuelat = 24;
//配置输出文件
	extern char* progoutpre;
	char s[4] = {NULL},s2[50] = {NULL};
	char *p = strcpy(s2,progoutpre);
	sprintf(s,"%d",actualid);
	char *prot = strcat(s2,s);
	progout = fopen(prot,"wt+");

//配置数据输出
	extern char* statisticspre;
	char si[4] = {NULL},si2[50] = {NULL};
	char *pi = strcpy(si2,statisticspre);
	sprintf(si,"%d",actualid);
	char *stat = strcat(si2,si);
	statisticsout = fopen(stat,"wt+");

//配置功耗输出
	extern char* poweroutpre;
	char sp[4] = {NULL},sp2[50] = {NULL};
	char *pip = strcpy(sp2,poweroutpre);
	sprintf(sp,"%d",actualid);
	char *statp = strcat(sp2,sp);
	strcpy(powerout_fb,statp);
	powerout = fopen(powerout_fb,"wt+");

//配置功耗输入
	extern char* powerxmlpre, *powerxmlpre_4mcpat;
	char sp_x[4] = {NULL}, sp2_x[50] = {NULL};
	char sp2_x4[50] = {NULL}, *statpx;
	strcpy(sp2_x,powerxmlpre);
	strcpy(sp2_x4,powerxmlpre_4mcpat);
	sprintf(sp_x,"%d",actualid);
	char *statp_x = strcat(sp2_x,sp_x);
	statpx = strcat(sp2_x4,sp_x);
	strcpy(powerxml_4mcpat,statpx);
	powerxml = fopen(statp_x,"wt+");


//配置指令流文件，用于与C代码行数对应
	extern char* inststreampre;
	char sii[4] = {NULL},sii2[50] = {NULL};
	char *pii = strcpy(sii2,inststreampre);
	sprintf(sii,"%d",actualid);
	char *instt = strcat(sii2,sii);
	inststreamout = fopen(instt,"wt+");

	bncfile = NULL;
	check_simoptions();//检查成员变量赋值
	md_init_decoder();
//初始化统计数据
	reg_stat_init(*statcpnt);
	mt->Id_table_assign(threadid,masterid,actualid);
	sim_load_prog(cs);
	init_context();
}

inline void TMIPS::exitrun(){
	//call when exit condition
	running = 0;
	Exitstates[actualid] = 1;
}
void TMIPS::reg_stat_init(Statistics & ss){
	int i;
	ss.stat_reg_counter("sim_num_insn",
			   "total number of instructions committed",
			   &sim_num_insn, sim_num_insn, NULL);
	ss.stat_reg_counter("sim_num_refs",
			   "total number of loads and stores committed",
			   &sim_num_refs, 0, NULL);
	ss.stat_reg_counter("sim_num_loads",
			   "total number of loads committed",
			   &sim_num_loads, 0, NULL);
	ss.stat_reg_formula("sim_num_stores",
			   "total number of stores committed",
			   "sim_num_refs - sim_num_loads", NULL);
	ss.stat_reg_counter("sim_num_branches",
			   "total number of branches committed",
			   &sim_num_branches, /* initial value */0, /* format */NULL);
	ss.stat_reg_int("sim_node_time",
		       "total simulation time in seconds",
		       &sim_node_time, 0, NULL);
	ss.stat_reg_formula("sim_inst_rate",
			   "simulation speed (in insts/sec)",
			   "sim_num_insn / sim_node_time", NULL);
	ss.stat_reg_counter("sim_total_insn",
			   "total number of instructions executed",
			   &sim_total_insn, 0, NULL);
	ss.stat_reg_counter("sim_total_refs",
			   "total number of loads and stores executed",
			   &sim_total_refs, 0, NULL);
	ss.stat_reg_counter("sim_total_loads",
			   "total number of loads executed",
			   &sim_total_loads, 0, NULL);
	ss.stat_reg_formula("sim_total_stores",
			   "total number of stores executed",
			   "sim_total_refs - sim_total_loads", NULL);
	ss.stat_reg_counter("sim_total_branches",
			   "total number of branches executed",
			   &sim_total_branches, /* initial value */0, /* format */NULL);
	  /* register performance stats */
	ss.stat_reg_counter("sim_cycle",
			   "total simulation time in cycles",
			   &sim_cycle, /* initial value */0, /* format */NULL);
	ss.stat_reg_formula("sim_IPC",
			   "instructions per cycle",
			   "sim_num_insn / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("sim_CPI",
			   "cycles per instruction",
			   "sim_cycle / sim_num_insn", /* format */NULL);
	ss.stat_reg_formula("sim_exec_BW",
			   "total instructions (mis-spec + committed) per cycle",
			   "sim_total_insn / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("sim_IPB",
			   "instruction per branch",
			   "sim_num_insn / sim_num_branches", /* format */NULL);
	  /* occupancy stats */
	ss.stat_reg_counter("IFQ_count", "cumulative IFQ occupancy",
	                   &IFQ_count, /* initial value */0, /* format */NULL);
	ss.stat_reg_counter("IFQ_fcount", "cumulative IFQ full count",
	                   &IFQ_fcount, /* initial value */0, /* format */NULL);
	ss.stat_reg_formula("ifq_occupancy", "avg IFQ occupancy (insn's)",
	                   "IFQ_count / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("ifq_rate", "avg IFQ dispatch rate (insn/cycle)",
	                   "sim_total_insn / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("ifq_latency", "avg IFQ occupant latency (cycle's)",
	                   "ifq_occupancy / ifq_rate", /* format */NULL);
	ss.stat_reg_formula("ifq_full", "fraction of time (cycle's) IFQ was full",
	                   "IFQ_fcount / sim_cycle", /* format */NULL);
	ss.stat_reg_counter("RUU_count", "cumulative RUU occupancy",
	                   &RUU_count, /* initial value */0, /* format */NULL);
	ss.stat_reg_counter("RUU_fcount", "cumulative RUU full count",
	                   &RUU_fcount, /* initial value */0, /* format */NULL);
	ss.stat_reg_formula("ruu_occupancy", "avg RUU occupancy (insn's)",
	                   "RUU_count / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("ruu_rate", "avg RUU dispatch rate (insn/cycle)",
	                   "sim_total_insn / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("ruu_latency", "avg RUU occupant latency (cycle's)",
	                   "ruu_occupancy / ruu_rate", /* format */NULL);
	ss.stat_reg_formula("ruu_full", "fraction of time (cycle's) RUU was full",
	                   "RUU_fcount / sim_cycle", /* format */NULL);
	ss.stat_reg_counter("LSQ_count", "cumulative LSQ occupancy",
	                   &LSQ_count, /* initial value */0, /* format */NULL);
	ss.stat_reg_counter("LSQ_fcount", "cumulative LSQ full count",
	                   &LSQ_fcount, /* initial value */0, /* format */NULL);
	ss.stat_reg_formula("lsq_occupancy", "avg LSQ occupancy (LSQ_count/cycle)",
	                   "LSQ_count / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("lsq_rate", "avg LSQ dispatch rate (insn/cycle)",
	                   "sim_total_insn / sim_cycle", /* format */NULL);
	ss.stat_reg_formula("lsq_latency", "avg LSQ occupant latency (cycle's)",
	                   "lsq_occupancy / lsq_rate", /* format */NULL);
	ss.stat_reg_formula("lsq_full", "fraction of time (cycle's) LSQ was full",
	                   "LSQ_fcount / sim_cycle", /* format */NULL);
	ss.stat_reg_counter("sim_slip",
	                   "total number of slip cycles",
	                   &sim_slip, 0, NULL);
	  /* register baseline stats */
	ss.stat_reg_formula("avg_sim_slip",
	                   "the average slip between issue and retirement",
	                   "sim_slip / sim_num_insn", NULL);
	  /* debug variable(s) */
	ss.stat_reg_counter("sim_invalid_addrs",
			   "total non-speculative bogus addresses seen (debug var)",
	                   &sim_invalid_addrs, /* initial value */0, /* format */NULL);

//	  if(runcontext->pred)
//		  bpred_reg_stats(runcontext->pred,ss);

	  /* register predictor stats
	  if (pred)
	    bpred_reg_stats(pred, sdb);

	  //register cache stats
	  if (cache_il1
	      && (cache_il1 != cache_dl1 && cache_il1 != cache_dl2))
	    cache_reg_stats(cache_il1, sdb);
	  if (cache_il2
	      && (cache_il2 != cache_dl1 && cache_il2 != cache_dl2))
	    cache_reg_stats(cache_il2, sdb);
	  if (cache_dl1)
	    cache_reg_stats(cache_dl1, sdb);
	  if (cache_dl2)
	    cache_reg_stats(cache_dl2, sdb);
	  if (itlb)
	    cache_reg_stats(itlb, sdb);
	  if (dtlb)
	    cache_reg_stats(dtlb, sdb);
	  mem_reg_stats(mem, sdb);
	  */

}

//context初始化
void TMIPS::init_context(){
	char name[128], c;
	int nsets, bsize, assoc;
	test_dl1_miss_counter = 0;
	test_dl2_miss_counter = 0;
	runcontext->ld_text_base = ld_text_base;
	/* program text (code) size in bytes */
	runcontext->ld_text_size = ld_text_size;
	/* program initialized data segment base */
	runcontext->ld_data_base = ld_data_base;
	/* top of the data segment */
	runcontext->ld_brk_point = ld_brk_point;
	/* program initialized ".data" and uninitialized ".bss" size in bytes */
	runcontext->ld_data_size = ld_data_size;
	/* program stack segment base (highest address in stack) */
	runcontext->ld_stack_base = ld_stack_base;
	/* program initial stack size */
	runcontext->ld_stack_size = ld_stack_size;
	/* lowest address accessed on the stack */
	runcontext->ld_stack_min = ld_stack_min;
	/* program file name */
	runcontext->ld_prog_fname = ld_prog_fname;
	/* program entry point (initial PC) */
	runcontext->ld_prog_entry = ld_prog_entry;
	/* program environment base address address */
	runcontext->ld_environ_base = ld_environ_base;
	/* target executable endian-ness, non-zero if big endian */
	runcontext->ld_target_big_endian = ld_target_big_endian;
	fetch_init();
	ruu_init();
	lsq_init();
	tracer_init();
	cv_init();
	runcontext->fu_pool = res_create_pool("fu-pool", fu_config,N_ELT(fu_config));
	if (!mystricmp(pred_type, "perfect")) {
		/* perfect predictor */
		runcontext->pred = NULL;
		pref_mode = TRUE;
	} else if (!mystricmp(pred_type, "taken")) {
		/* static predictor, not taken */
		runcontext->pred = bpred_create(BPredTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	} else if (!mystricmp(pred_type, "nottaken")) {
		/* static predictor, taken */
		runcontext->pred = bpred_create(BPredNotTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	} else if (!mystricmp(pred_type, "bimod")) {
		/* bimodal predictor, bpred_create() checks BTB_SIZE */
		runcontext->pred = bpred_create(BPred2bit,
		/* bimod table size */bimod_config[0],
		/* 2lev l1 size */0,
		/* 2lev l2 size */0,
		/* meta table size */0,
		/* history reg size */0,
		/* history xor address */0,
		/* btb sets */btb_config[0],
		/* btb assoc */btb_config[1],
		/* ret-addr stack size */ras_size);
	} else if (!mystricmp(pred_type, "2lev")) {
		runcontext->pred = bpred_create(BPred2Level,
		/* bimod table size */0,
		/* 2lev l1 size */twolev_config[0],
		/* 2lev l2 size */twolev_config[1],
		/* meta table size */0,
		/* history reg size */twolev_config[2],
		/* history xor address */twolev_config[3],
		/* btb sets */btb_config[0],
		/* btb assoc */btb_config[1],
		/* ret-addr stack size */ras_size);
	} else if (!mystricmp(pred_type, "comb")) {
		/* combining predictor, bpred_create() checks args */
		runcontext->pred = bpred_create(BPredComb,
		/* bimod table size */bimod_config[0],
		/* l1 size */twolev_config[0],
		/* l2 size */twolev_config[1],
		/* meta table size */comb_config[0],
		/* history reg size */twolev_config[2],
		/* history xor address */twolev_config[3],
		/* btb sets */btb_config[0],
		/* btb assoc */btb_config[1],
		/* ret-addr stack size */ras_size);
	} else
		fatal("cannot parse predictor type `%s'", pred_type);
	/* use a level 1 D-cache? */
	if (cache_dl1_opt[0] == '\0') {
		runcontext->cache_dl1 = NULL;
	/* the level 2 D-cache cannot be defined */
	if (cache_dl2_opt[0] == '\0')
		fatal("the l1 data cache must defined if the l2 cache is defined");
		runcontext->cache_dl2 = NULL;
	} else /* dl1 is defined */
	{
		if (sscanf(cache_dl1_opt, "%[^:]:%d:%d:%d:%c", name, &nsets,&bsize, &assoc, &c) != 5)
			fatal("bad l1 D-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");

		runcontext->cache_dl1 = cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */0, assoc, cache_char2policy(c),
				(& TMIPS::dl1_access_fn), /* hit lat */cache_dl1_lat);

		/* is the level 2 D-cache defined? */
		if (cache_dl2_opt[0] == '\0')
			runcontext->cache_dl2 = NULL;
		else {
			if (sscanf(cache_dl2_opt, "%[^:]:%d:%d:%d:%c", name, &nsets,&bsize, &assoc, &c) != 5)
				fatal("bad l2 D-cache parms: ""<name>:<nsets>:<bsize>:<assoc>:<repl>");
			runcontext->cache_dl2 = cache_create(name, nsets, bsize, /* balloc */FALSE,
					/* usize */0, assoc, cache_char2policy(c),
					(& TMIPS::dl2_access_fn), /* hit lat */cache_dl2_lat);
		}
	}
	/* use a level 1 I-cache? */
	if (cache_il1_opt[0] == '\0') {
		runcontext->cache_il1 = NULL;
		/* the level 2 I-cache cannot be defined */
		if (cache_il2_opt[0] == '\0')
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		runcontext->cache_il2 = NULL;
	} else if (!mystricmp(cache_il1_opt, "dl1")) {
		if (!runcontext->cache_dl1)
			fatal("I-cache l1 cannot access D-cache l1 as it's undefined");
		runcontext->cache_il1 = runcontext->cache_dl1;
		/* the level 2 I-cache cannot be defined */
		if (cache_il2_opt[0] == '\0')
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		runcontext->cache_il2 = NULL;
	} else if (!mystricmp(cache_il1_opt, "dl2")) {
		if (!runcontext->cache_dl2)
			fatal("I-cache l1 cannot access D-cache l2 as it's undefined");
		runcontext->cache_il1 = runcontext->cache_dl2;
		/* the level 2 I-cache cannot be defined */
		if (cache_il2_opt[0] == '\0')
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		runcontext->cache_il2 = NULL;
	} else /* il1 is defined */
	{
		if (sscanf(cache_il1_opt, "%[^:]:%d:%d:%d:%c", name, &nsets,
				&bsize, &assoc, &c) != 5)
			fatal("bad l1 I-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");
		runcontext->cache_il1 = cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */0, assoc, cache_char2policy(c),
				(& TMIPS::il1_access_fn), /* hit lat */cache_il1_lat);

		/* is the level 2 D-cache defined? */
		if (cache_il2_opt[0] == '\0')
			runcontext->cache_il2 = NULL;
		else if (!mystricmp(cache_il2_opt, "dl2")) {
			if (!runcontext->cache_dl2)
				fatal(
						"I-cache l2 cannot access D-cache l2 as it's undefined");
			runcontext->cache_il2 = runcontext->cache_dl2;
		} else {
			if (sscanf(cache_il2_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
				fatal("bad l2 I-cache parms: "
						"<name>:<nsets>:<bsize>:<assoc>:<repl>");
			runcontext->cache_il2 = cache_create(name, nsets, bsize, /* balloc */FALSE,
					/* usize */0, assoc, cache_char2policy(c),
					(& TMIPS::il2_access_fn), /* hit lat */cache_il2_lat);
		}
	}
	/* use an I-TLB? */
	if (itlb_opt[0] == '\0')
		runcontext->itlb = NULL;
	else {
		if (sscanf(itlb_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
			fatal(
					"bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
		runcontext->itlb = cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */sizeof(md_addr_t), assoc,
				cache_char2policy(c), (& TMIPS::itlb_access_fn),
				/* hit latency */1);
	}
	/* use a D-TLB? */
	if (dtlb_opt[0] == '\0')
		runcontext->dtlb = NULL;
	else {
		if (sscanf(dtlb_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize,
				&assoc, &c) != 5)
			fatal("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
		runcontext->dtlb = cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */sizeof(md_addr_t), assoc,
				cache_char2policy(c), (& TMIPS::dtlb_access_fn),
				/* hit latency */1);
	}
	//initial regs
	signal(SIGFPE,SIG_IGN);
	runcontext->regs->regs_PC = runcontext->ld_prog_entry;
	runcontext->regs->regs_NPC = runcontext->regs->regs_PC + sizeof(md_inst_t);
	runcontext->fetch_regs_PC = runcontext->regs->regs_PC - sizeof(md_inst_t);//sizeof(md_inst_t)=8
	runcontext->fetch_pred_PC = runcontext->regs->regs_PC;
	runcontext->regs->regs_PC = runcontext->regs->regs_PC - sizeof(md_inst_t);
//	main_popnet();
}



//检查成员变量
void TMIPS::check_simoptions(){
	if (ruu_ifq_size < 1 || (ruu_ifq_size & (ruu_ifq_size - 1)) != 0)
	    fatal("inst fetch queue size must be positive > 0 and a power of two");
  if (ruu_branch_penalty < 1)
	fatal("mis-prediction penalty must be at least 1 cycle");
  if (fetch_speed < 1)
	fatal("front-end speed must be positive and non-zero");
  if (ruu_decode_width < 1 || (ruu_decode_width & (ruu_decode_width-1)) != 0)
	fatal("issue width must be positive non-zero and a power of two");
  if (ruu_issue_width < 1 || (ruu_issue_width & (ruu_issue_width-1)) != 0)
	fatal("issue width must be positive non-zero and a power of two");
  if (ruu_commit_width < 1)
	fatal("commit width must be positive non-zero");
  if (RUU_size < 2 || (RUU_size & (RUU_size-1)) != 0)
	fatal("RUU size must be a positive number > 1 and a power of two");
  if (LSQ_size < 2 || (LSQ_size & (LSQ_size-1)) != 0)
	fatal("LSQ size must be a positive number > 1 and a power of two");
  if (cache_dl1_lat < 1)
	fatal("l1 data cache latency must be greater than zero");
  if (cache_dl2_lat < 1)
	fatal("l2 data cache latency must be greater than zero");
  if (cache_il1_lat < 1)
	fatal("l1 instruction cache latency must be greater than zero");
  if (cache_il2_lat < 1)
	fatal("l2 instruction cache latency must be greater than zero");
  if (mem_lat[0] < 1 || mem_lat[1] < 1)
	fatal("all memory access latencies must be greater than zero");
  if (mem_bus_width < 1 || (mem_bus_width & (mem_bus_width-1)) != 0)
	fatal("memory bus width must be positive non-zero and a power of two");
  if (tlb_miss_lat < 1)
	fatal("TLB miss latency must be greater than zero");
  if (res_ialu < 1)
	fatal("number of integer ALU's must be greater than zero");
  if (res_ialu > MAX_INSTS_PER_CLASS)
	fatal("number of integer ALU's must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_IALU_INDEX].quantity = res_ialu;
  if (res_imult < 1)
	fatal("number of integer multiplier/dividers must be greater than zero");
  if (res_imult > MAX_INSTS_PER_CLASS)
	fatal("number of integer mult/div's must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_IMULT_INDEX].quantity = res_imult;
  if (res_memport < 1)
	fatal("number of memory system ports must be greater than zero");
  if (res_memport > MAX_INSTS_PER_CLASS)
	fatal("number of memory system ports must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_MEMPORT_INDEX].quantity = res_memport;
  if (res_fpalu < 1)
	fatal("number of floating point ALU's must be greater than zero");
  if (res_fpalu > MAX_INSTS_PER_CLASS)
	fatal("number of floating point ALU's must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_FPALU_INDEX].quantity = res_fpalu;
  if (res_fpmult < 1)
	fatal("number of floating point multiplier/dividers must be > zero");
  if (res_fpmult > MAX_INSTS_PER_CLASS)
	fatal("number of FP mult/div's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_FPMULT_INDEX].quantity = res_fpmult;
}

void TMIPS::sim_init(){
	/* initialize here, so symbols can be loaded */
	  if (ptrace_nelt == 2)
	    {
	      /* generate a pipeline trace */
	      ptrace_open(/* fname */ptrace_opts[0], /* range */ptrace_opts[1]);
	    }
	  else if (ptrace_nelt == 0)
	    {
	      /* no pipetracing */;
	    }
	  else
	    fatal("bad pipetrace args, use: <fname|stdout|stderr> <range>");
	  rslink_init();
	  eventq_init();
	  readyq_init();
	  /* initialize the DLite debugger */
//	  dlite_init(simoo_reg_obj, simoo_mem_obj, simoo_mstate_obj);
}

int TMIPS::read_thread_args(FILE* bncfile){
	int retcode,bncarg_index = 0,j=0;
	char fstring[100] = {NULL};
	if(!bncfile){
		fprintf(stderr,"BNC file read error");
		return 1;
	}
	fscanf(bncfile,"%s",fstring);
	if(strcmp(fstring,"#")){
		fprintf(stderr,"BNC file must start with '#'");
		return 1;
	}
	runcontext->argv = (char **) malloc(30*sizeof(char *));
	if(runcontext->argv == NULL)
	{
		fprintf(stderr,"allocation fail in read_thread_args\n");
		exit(1);
	}
	for(j=0;j<30;j++)
	{
		runcontext->argv[j] = NULL;
		runcontext->argv[j] = (char *) malloc(240*sizeof(char));
		if(runcontext->argv[j] == NULL)
		{
			fprintf(stderr,"allocation fail in read_thread_args\n");
			exit(1);
		}
	}
	while(1){
		retcode = fscanf(bncfile,"%s",runcontext->argv[bncarg_index]);
		if(retcode == EOF || runcontext->argv[bncarg_index][0] == '#'){
			//读入完毕，开始处理
			runcontext->argc = bncarg_index;
			break;
		}
		bncarg_index++;
	}
	return 0;
}
void TMIPS::sim_load_prog(CoreStruct* cs){
	bncfile = fopen(cs->testpath,"r");
	int r = read_thread_args(bncfile);
	fclose(bncfile);
	if(r)
		fatal("bncfile open filled,node %d",cs->nid);
	//初始化regs
	regs_create();
	regs_init();
	//初始化mem
	mem_create("mem");
	mem_init();
	ld_load_prog (runcontext->argv[0], runcontext->argc, runcontext->argv, envp, runcontext->regs, runcontext->mem, TRUE);

}



sim_rt_t TMIPS::run(){
//	printf("it's node %d",actualid);

	if(exist == 0 ||running == 0)
		return NOWORKING;
	else
		return run_cycle();
}

sim_rt_t TMIPS::run_cycle(){
	{
//		fprintf(Simulation::visualFile, "sim_cycle:%lld ", sim_cycle);
		esynetrcd->actualid = actualid;
		esynetrcd->tileid = tileid;
//		printf("tileid = %d\n",tileid);
		esynetrcd->sim_cycle = sim_cycle;
		/* RUU/LSQ sanity checks */
		if (runcontext->RUU_num < runcontext->LSQ_num)
		panic("RUU_num < LSQ_num");
		if (((runcontext->RUU_head + runcontext->RUU_num) % RUU_size) != runcontext->RUU_tail)
		panic("RUU_head/RUU_tail wedged");
		if (((runcontext->LSQ_head + runcontext->LSQ_num) % LSQ_size) != runcontext->LSQ_tail)
		panic("LSQ_head/LSQ_tail wedged");
//		if(actualid == 5){
//			test_dl1_miss_counter = runcontext->cache_dl1->misses;
//			test_dl2_miss_counter = runcontext->cache_dl2->misses;
//		}
		ruu_commit();
		ruu_release_fu();
		ruu_writeback();
		if (!bugcompat_mode)
		{
		  /* try to locate memory operations that are ready to execute */
		  /* ==> inserts operations into ready queue --> mem deps resolved */
		  lsq_refresh();
		  /* issue operations ready to execute from a previous cycle */
		  /* <== drains ready queue <-- ready operations commence execution */
		  ruu_issue();
		}
		ruu_dispatch();
		if (bugcompat_mode)
		{
		  /* try to locate memory operations that are ready to execute */
		  /* ==> inserts operations into ready queue --> mem deps resolved */
		  lsq_refresh();

		  /* issue operations ready to execute from a previous cycle */
		  /* <== drains ready queue <-- ready operations commence execution */
		  ruu_issue();
		}
		if (!ruu_fetch_issue_delay)
			ruu_fetch();
		else{
//			fputs("no_access_il1:0 il1_miss:0 no_access_il2:0 il2_miss:0 ",Simulation::visualFile);//不访问Icache
			ruu_fetch_issue_delay--;
			thispower->gpp_data->core_para->ifu_block_num+=1;
			allpower->gpp_data->core_para->ifu_block_num+=1;
		}
		IFQ_count += runcontext->fetch_num;
		IFQ_fcount += ((runcontext->fetch_num == ruu_ifq_size) ? 1 : 0);
		RUU_count += runcontext->RUU_num;
		RUU_fcount += ((runcontext->RUU_num == RUU_size) ? 1 : 0);
		LSQ_count += runcontext->LSQ_num;
		LSQ_fcount += ((runcontext->LSQ_num == LSQ_size) ? 1 : 0);
		//功耗统计计算
		//TODO
		//功耗统计计算结束
		//调用MCPAT
		//TODO
		//调用MCPAT结束
		//printf("PISA -- simulation_cycle = %ld & sim_cycle=%ld & numthreads=%d\n",simulation_cycle,sim_cycle,actualid);
		sim_cycle++;
		if (max_insts && sim_num_insn >= max_insts){
			return MAX_INST;
		}
		thispower->gpp_data->core_para->total_cycle_num+=1;
		allpower->gpp_data->core_para->total_cycle_num+=1;
	}
//	if(actualid == 5){
//	fprintf(Simulation::testfile, "sim_cycle = %lld, dl1_miss = %lld, dl2_miss = %lld\n"
//			,sim_cycle, runcontext->cache_dl1->misses,runcontext->cache_dl2->misses);
//		if( runcontext->cache_dl1->misses - test_dl1_miss_counter > 1){
//			printf("sim_cycle = %d",sim_cycle);
//			printf("runcontext->cache_dl1->misses > 1\n");
//		}
//		if( runcontext->cache_dl2->misses - test_dl2_miss_counter > 1){
//			printf("sim_cycle = %d",sim_cycle);
//			printf("runcontext->cache_dl2->misses > 1\n");
//		}
//	}
	return NO_Q;
}


/*-----------------------------------------------------------------------------------------------*/


/*symbol.cc*/

#ifdef PRINT_SYMS
/* convert BFD symbols flags to a printable string */
static char *			/* symbol flags string */
flags2str(unsigned int flags)	/* bfd symbol flags */
{
  static char buf[256];
  char *p;

  if (!flags)
    return "";

  p = buf;
  *p = '\0';

  if (flags & BSF_LOCAL)
    {
      *p++ = 'L';
      *p++ = '|';
    }
  if (flags & BSF_GLOBAL)
    {
      *p++ = 'G';
      *p++ = '|';
    }
  if (flags & BSF_DEBUGGING)
    {
      *p++ = 'D';
      *p++ = '|';
    }
  if (flags & BSF_FUNCTION)
    {
      *p++ = 'F';
      *p++ = '|';
    }
  if (flags & BSF_KEEP)
    {
      *p++ = 'K';
      *p++ = '|';
    }
  if (flags & BSF_KEEP_G)
    {
      *p++ = 'k'; *p++ = '|';
    }
  if (flags & BSF_WEAK)
    {
      *p++ = 'W';
      *p++ = '|';
    }
  if (flags & BSF_SECTION_SYM)
    {
      *p++ = 'S'; *p++ = '|';
    }
  if (flags & BSF_OLD_COMMON)
    {
      *p++ = 'O';
      *p++ = '|';
    }
  if (flags & BSF_NOT_AT_END)
    {
      *p++ = 'N';
      *p++ = '|';
    }
  if (flags & BSF_CONSTRUCTOR)
    {
      *p++ = 'C';
      *p++ = '|';
    }
  if (flags & BSF_WARNING)
    {
      *p++ = 'w';
      *p++ = '|';
    }
  if (flags & BSF_INDIRECT)
    {
      *p++ = 'I';
      *p++ = '|';
    }
  if (flags & BSF_FILE)
    {
      *p++ = 'f';
      *p++ = '|';
    }

  if (p == buf)
    panic("no flags detected");

  *--p = '\0';
  return buf;
}
#endif /* PRINT_SYMS */
int TMIPS::acmp(const struct sym_sym_t **sym1, const struct sym_sym_t **sym2)
{
  return (int)((*sym1)->addr - (*sym2)->addr);
}
int TMIPS::ncmp(struct TMIPS::sym_sym_t **sym1, struct TMIPS::sym_sym_t **sym2)
{
  return strcmp((*sym1)->name, (*sym2)->name);
}
void TMIPS::sym_loadsyms(char *fname,	/* file name containing symbols */
	     int load_locals)	/* load local symbols */
{
  int i, debug_cnt;
#ifdef BFD_LOADER
  bfd *abfd;
  asymbol **syms;
  int storage, i, nsyms, debug_cnt;
#else /* !BFD_LOADER */
  int len;
  FILE *fobj;
  struct ecoff_filehdr fhdr;
  struct ecoff_aouthdr ahdr;
  struct ecoff_symhdr_t symhdr;
  char *strtab = NULL;
  ecoff_EXTR *extr;
#endif /* BFD_LOADER */

  if (syms_loaded)
    {
      /* symbols are already loaded */
      /* FIXME: can't handle symbols from multiple files */
      return;
    }

#ifdef BFD_LOADER

  /* load the program into memory, try both endians */
  if (!(abfd = bfd_openr(fname, "ss-coff-big")))
    if (!(abfd = bfd_openr(fname, "ss-coff-little")))
      fatal("cannot open executable `%s'", fname);

  /* this call is mainly for its side effect of reading in the sections.
     we follow the traditional behavior of `strings' in that we don't
     complain if we don't recognize a file to be an object file.  */
  if (!bfd_check_format(abfd, bfd_object))
    {
      bfd_close(abfd);
      fatal("cannot open executable `%s'", fname);
    }

  /* sanity check, endian should be the same as loader.c encountered */
  if (abfd->xvec->byteorder_big_p != (unsigned)ld_target_big_endian)
    panic("binary endian changed");

  if ((bfd_get_file_flags(abfd) & (HAS_SYMS|HAS_LOCALS)))
    {
      /* file has locals, read them in */
      storage = bfd_get_symtab_upper_bound(abfd);
      if (storage <= 0)
	fatal("HAS_SYMS is set, but `%s' still lacks symbols", fname);

      syms = (asymbol **)calloc(storage, 1);
      if (!syms)
	fatal("out of virtual memory");

      nsyms = bfd_canonicalize_symtab (abfd, syms);
      if (nsyms <= 0)
	fatal("HAS_SYMS is set, but `%s' still lacks symbols", fname);

      /*
       * convert symbols to local format
       */

      /* first count symbols */
      sym_ndatasyms = 0; sym_ntextsyms = 0;
      for (i=0; i < nsyms; i++)
	{
	  asymbol *sym = syms[i];

	  /* decode symbol type */
	  if (/* from the data section */
	      (!strcmp(sym->section->name, ".rdata")
	       || !strcmp(sym->section->name, ".data")
	       || !strcmp(sym->section->name, ".sdata")
	       || !strcmp(sym->section->name, ".bss")
	       || !strcmp(sym->section->name, ".sbss"))
	      /* from a scope we are interested in */
	      && RELEVANT_SCOPE(sym))
	    {
	      /* data segment symbol */
	      sym_ndatasyms++;
#ifdef PRINT_SYMS
	      fprintf(stderr,
		      "+sym: %s  sect: %s  flags: %s  value: 0x%08lx\n",
		      sym->name, sym->section->name, flags2str(sym->flags),
		      sym->value + sym->section->vma);
#endif /* PRINT_SYMS */
	    }
	  else if (/* from the text section */
		   !strcmp(sym->section->name, ".text")
		   /* from a scope we are interested in */
		   && RELEVANT_SCOPE(sym))
	    {
	      /* text segment symbol */
	      sym_ntextsyms++;
#ifdef PRINT_SYMS
	      fprintf(stderr,
		      "+sym: %s  sect: %s  flags: %s  value: 0x%08lx\n",
		      sym->name, sym->section->name, flags2str(sym->flags),
		      sym->value + sym->section->vma);
#endif /* PRINT_SYMS */
	    }
	  else
	    {
	      /* non-segment sections */
#ifdef PRINT_SYMS
	      fprintf(stderr,
		      "-sym: %s  sect: %s  flags: %s  value: 0x%08lx\n",
		      sym->name, sym->section->name, flags2str(sym->flags),
		      sym->value + sym->section->vma);
#endif /* PRINT_SYMS */
	    }
	}
      sym_nsyms = sym_ntextsyms + sym_ndatasyms;
      if (sym_nsyms <= 0)
	fatal("`%s' has no text or data symbols", fname);

      /* allocate symbol space */
      sym_db = (struct sym_sym_t *)calloc(sym_nsyms, sizeof(struct sym_sym_t));
      if (!sym_db)
	fatal("out of virtual memory");

      /* convert symbols to internal format */
      for (debug_cnt=0, i=0; i < nsyms; i++)
	{
	  asymbol *sym = syms[i];

	  /* decode symbol type */
	  if (/* from the data section */
	      (!strcmp(sym->section->name, ".rdata")
	       || !strcmp(sym->section->name, ".data")
	       || !strcmp(sym->section->name, ".sdata")
	       || !strcmp(sym->section->name, ".bss")
	       || !strcmp(sym->section->name, ".sbss"))
	      /* from a scope we are interested in */
	      && RELEVANT_SCOPE(sym))
	    {
	      /* data segment symbol, insert into symbol database */
	      sym_db[debug_cnt].name = mystrdup((char *)sym->name);
	      sym_db[debug_cnt].seg = ss_data;
	      sym_db[debug_cnt].initialized =
		(!strcmp(sym->section->name, ".rdata")
		 || !strcmp(sym->section->name, ".data")
		 || !strcmp(sym->section->name, ".sdata"));
	      sym_db[debug_cnt].pub = (sym->flags & BSF_GLOBAL);
	      sym_db[debug_cnt].local = (sym->name[0] == '$');
	      sym_db[debug_cnt].addr = sym->value + sym->section->vma;

	      debug_cnt++;
	    }
	  else if (/* from the text section */
		   !strcmp(sym->section->name, ".text")
		   /* from a scope we are interested in */
		   && RELEVANT_SCOPE(sym))
	    {
	      /* text segment symbol, insert into symbol database */
	      sym_db[debug_cnt].name = mystrdup((char *)sym->name);
	      sym_db[debug_cnt].seg = ss_text;
	      sym_db[debug_cnt].initialized = /* seems reasonable */TRUE;
	      sym_db[debug_cnt].pub = (sym->flags & BSF_GLOBAL);
	      sym_db[debug_cnt].local = (sym->name[0] == '$');
	      sym_db[debug_cnt].addr = sym->value + sym->section->vma;

	      debug_cnt++;
	    }
	  else
	    {
	      /* non-segment sections */
	    }
	}
      /* sanity check */
      if (debug_cnt != sym_nsyms)
	panic("could not locate all counted symbols");

      /* release bfd symbol storage */
      free(syms);
    }

  /* done with file, close if */
  if (!bfd_close(abfd))
    fatal("could not close executable `%s'", fname);

#else /* !BFD_LOADER */

  /* load the program into memory, try both endians */
#if defined(__CYGWIN32__) || defined(_MSC_VER)
  fobj = fopen(fname, "rb");
#else
  fobj = fopen(fname, "r");
#endif
  if (!fobj)
    fatal("cannot open executable `%s'", fname);

  if (fread(&fhdr, sizeof(struct ecoff_filehdr), 1, fobj) < 1)
    fatal("cannot read header from executable `%s'", fname);

  /* record endian of target */
  if (fhdr.f_magic != ECOFF_EB_MAGIC && fhdr.f_magic != ECOFF_EL_MAGIC)
    fatal("bad magic number in executable `%s'", fname);

  if (fread(&ahdr, sizeof(struct ecoff_aouthdr), 1, fobj) < 1)
    fatal("cannot read AOUT header from executable `%s'", fname);

  /* seek to the beginning of the symbolic header */
  fseek(fobj, fhdr.f_symptr, 0);

  if (fread(&symhdr, sizeof(struct ecoff_symhdr_t), 1, fobj) < 1)
    fatal("could not read symbolic header from executable `%s'", fname);

  if (symhdr.magic != ECOFF_magicSym)
    fatal("bad magic number (0x%x) in symbolic header", symhdr.magic);

  /* allocate space for the string table */
  len = symhdr.issMax + symhdr.issExtMax;
  strtab = (char *)calloc(len, sizeof(char));
  if (!strtab)
    fatal("out of virtual memory");

  /* read all the symbol names into memory */
  fseek(fobj, symhdr.cbSsOffset, 0);
  if (fread(strtab, len, 1, fobj) < 0)
    fatal("error while reading symbol table names");

  /* allocate symbol space */
  len = symhdr.isymMax + symhdr.iextMax;
  if (len <= 0)
    fatal("`%s' has no text or data symbols", fname);
  sym_db = (struct sym_sym_t *)calloc(len, sizeof(struct sym_sym_t));
  if (!sym_db)
    fatal("out of virtual memory");

  /* allocate space for the external symbol entries */
  extr =
    (ecoff_EXTR *)calloc(symhdr.iextMax, sizeof(ecoff_EXTR));
  if (!extr)
    fatal("out of virtual memory");

  fseek(fobj, symhdr.cbExtOffset, 0);
  if (fread(extr, sizeof(ecoff_EXTR), symhdr.iextMax, fobj) < 0)
    fatal("error reading external symbol entries");

  sym_nsyms = 0; sym_ndatasyms = 0; sym_ntextsyms = 0;

  /* convert symbols to internal format */
  for (i=0; i < symhdr.iextMax; i++)
    {
      int str_offset;

      str_offset = symhdr.issMax + extr[i].asym.iss;

#if 0
      printf("ext %2d: ifd = %2d, iss = %3d, value = %8x, st = %3x, "
	     "sc = %3x, index = %3x\n",
	     i, extr[i].ifd,
	     extr[i].asym.iss, extr[i].asym.value,
	     extr[i].asym.st, extr[i].asym.sc,
	     extr[i].asym.index);
      printf("       %08x %2d %2d %s\n",
	     extr[i].asym.value,
	     extr[i].asym.st,
	     extr[i].asym.sc,
	     &strtab[str_offset]);
#endif

      switch (extr[i].asym.st)
	{
	case ECOFF_stGlobal:
	case ECOFF_stStatic:
	  /* from data segment */
	  sym_db[sym_nsyms].name = mystrdup(&strtab[str_offset]);
	  sym_db[sym_nsyms].seg = ss_data;
	  sym_db[sym_nsyms].initialized = /* FIXME: ??? */TRUE;
	  sym_db[sym_nsyms].pub = /* FIXME: ??? */TRUE;
	  sym_db[sym_nsyms].local = /* FIXME: ??? */FALSE;
	  sym_db[sym_nsyms].addr = extr[i].asym.value;
	  sym_nsyms++;
	  sym_ndatasyms++;
	  break;

	case ECOFF_stProc:
	case ECOFF_stStaticProc:
	case ECOFF_stLabel:
	  /* from text segment */
	  sym_db[sym_nsyms].name = mystrdup(&strtab[str_offset]);
	  sym_db[sym_nsyms].seg = ss_text;
	  sym_db[sym_nsyms].initialized = /* FIXME: ??? */TRUE;
	  sym_db[sym_nsyms].pub = /* FIXME: ??? */TRUE;
	  sym_db[sym_nsyms].local = /* FIXME: ??? */FALSE;
	  sym_db[sym_nsyms].addr = extr[i].asym.value;
	  sym_nsyms++;
	  sym_ntextsyms++;
	  break;

	default:
	  /* FIXME: ignored... */;
	}
    }
  free(extr);

  /* done with the executable, close it */
  if (fclose(fobj))
    fatal("could not close executable `%s'", fname);

#endif /* BFD_LOADER */

  /*
   * generate various sortings
   */

  /* all symbols sorted by address and name */
  sym_syms =
    (struct sym_sym_t **)calloc(sym_nsyms, sizeof(struct sym_sym_t *));
  if (!sym_syms)
    fatal("out of virtual memory");

  sym_syms_by_name =
    (struct sym_sym_t **)calloc(sym_nsyms, sizeof(struct sym_sym_t *));
  if (!sym_syms_by_name)
    fatal("out of virtual memory");

  for (debug_cnt=0, i=0; i<sym_nsyms; i++)
    {
      sym_syms[debug_cnt] = &sym_db[i];
      sym_syms_by_name[debug_cnt] = &sym_db[i];
      debug_cnt++;
    }
  /* sanity check */
  if (debug_cnt != sym_nsyms)
    panic("could not locate all symbols");

  /* sort by address */
  qsort(sym_syms, sym_nsyms, sizeof(struct sym_sym_t *), (int (*)(const void *, const void *))(& TMIPS::acmp));

  /* sort by name */
  qsort(sym_syms_by_name, sym_nsyms, sizeof(struct sym_sym_t *), (int (*)(const void *, const void *))(& TMIPS::ncmp));

  /* text segment sorted by address and name */
  sym_textsyms =
    (struct sym_sym_t **)calloc(sym_ntextsyms, sizeof(struct sym_sym_t *));
  if (!sym_textsyms)
    fatal("out of virtual memory");

  sym_textsyms_by_name =
    (struct sym_sym_t **)calloc(sym_ntextsyms, sizeof(struct sym_sym_t *));
  if (!sym_textsyms_by_name)
    fatal("out of virtual memory");

  for (debug_cnt=0, i=0; i<sym_nsyms; i++)
    {
      if (sym_db[i].seg == ss_text)
	{
	  sym_textsyms[debug_cnt] = &sym_db[i];
	  sym_textsyms_by_name[debug_cnt] = &sym_db[i];
	  debug_cnt++;
	}
    }
  /* sanity check */
  if (debug_cnt != sym_ntextsyms)
    panic("could not locate all text symbols");

  /* sort by address */
  qsort(sym_textsyms, sym_ntextsyms, sizeof(struct sym_sym_t *), (int (*)(const void *, const void *))(& TMIPS::acmp));

  /* sort by name */
  qsort(sym_textsyms_by_name, sym_ntextsyms,
	sizeof(struct sym_sym_t *), (int (*)(const void *, const void *))(& TMIPS::ncmp));

  /* data segment sorted by address and name */
  sym_datasyms =
    (struct sym_sym_t **)calloc(sym_ndatasyms, sizeof(struct sym_sym_t *));
  if (!sym_datasyms)
    fatal("out of virtual memory");

  sym_datasyms_by_name =
    (struct sym_sym_t **)calloc(sym_ndatasyms, sizeof(struct sym_sym_t *));
  if (!sym_datasyms_by_name)
    fatal("out of virtual memory");

  for (debug_cnt=0, i=0; i<sym_nsyms; i++)
    {
      if (sym_db[i].seg == ss_data)
	{
	  sym_datasyms[debug_cnt] = &sym_db[i];
	  sym_datasyms_by_name[debug_cnt] = &sym_db[i];
	  debug_cnt++;
	}
    }
  /* sanity check */
  if (debug_cnt != sym_ndatasyms)
    panic("could not locate all data symbols");

  /* sort by address */
  qsort(sym_datasyms, sym_ndatasyms, sizeof(struct sym_sym_t *), (int (*)(const void *, const void *))(& TMIPS::acmp));

  /* sort by name */
  qsort(sym_datasyms_by_name, sym_ndatasyms,
	sizeof(struct sym_sym_t *), (int (*)(const void *, const void *))(& TMIPS::ncmp));

  /* compute symbol sizes */
  for (i=0; i<sym_ntextsyms; i++)
    {
      sym_textsyms[i]->size =
	(i != (sym_ntextsyms - 1)
	 ? (sym_textsyms[i+1]->addr - sym_textsyms[i]->addr)
	 : ((ld_text_base + ld_text_size) - sym_textsyms[i]->addr));
    }
  for (i=0; i<sym_ndatasyms; i++)
    {
      sym_datasyms[i]->size =
	(i != (sym_ndatasyms - 1)
	 ? (sym_datasyms[i+1]->addr - sym_datasyms[i]->addr)
	 : ((ld_data_base + ld_data_size) - sym_datasyms[i]->addr));
    }

  /* symbols are now available for use */
  syms_loaded = TRUE;
}
void TMIPS::sym_dumpsym(struct sym_sym_t *sym,	/* symbol to display */
	    FILE *fd)			/* output stream */
{
  fprintf(fd,
    "sym `%s': %s seg, init-%s, pub-%s, local-%s, addr=0x%08x, size=%d\n",
	  sym->name,
	  sym->seg == ss_data ? "data" : "text",
	  sym->initialized ? "y" : "n",
	  sym->pub ? "y" : "n",
	  sym->local ? "y" : "n",
	  sym->addr,
	  sym->size);
}
void TMIPS::sym_dumpsyms(FILE *fd)			/* output stream */
{
  int i;

  for (i=0; i < sym_nsyms; i++)
    sym_dumpsym(sym_syms[i], fd);
}
void TMIPS::sym_dumpstate(FILE *fd)			/* output stream */
{
  int i;

  if (fd == NULL)
    fd = stderr;

  fprintf(fd, "** All symbols sorted by address:\n");
  for (i=0; i < sym_nsyms; i++)
    sym_dumpsym(sym_syms[i], fd);

  fprintf(fd, "\n** All symbols sorted by name:\n");
  for (i=0; i < sym_nsyms; i++)
    sym_dumpsym(sym_syms_by_name[i], fd);

  fprintf(fd, "** Text symbols sorted by address:\n");
  for (i=0; i < sym_ntextsyms; i++)
    sym_dumpsym(sym_textsyms[i], fd);

  fprintf(fd, "\n** Text symbols sorted by name:\n");
  for (i=0; i < sym_ntextsyms; i++)
    sym_dumpsym(sym_textsyms_by_name[i], fd);

  fprintf(fd, "** Data symbols sorted by address:\n");
  for (i=0; i < sym_ndatasyms; i++)
    sym_dumpsym(sym_datasyms[i], fd);

  fprintf(fd, "\n** Data symbols sorted by name:\n");
  for (i=0; i < sym_ndatasyms; i++)
    sym_dumpsym(sym_datasyms_by_name[i], fd);
}
struct TMIPS::sym_sym_t *			/* symbol found, or NULL */
TMIPS::sym_bind_addr(md_addr_t addr,		/* address of symbol to locate */
	      int *pindex,		/* ptr to index result var */
	      int exact,		/* require exact address match? */
	      enum sym_db_t db)		/* symbol database to search */
{
  int nsyms, low, high, pos;
  struct sym_sym_t **syms;

  switch (db)
    {
    case sdb_any:
      syms = sym_syms;
      nsyms = sym_nsyms;
      break;
    case sdb_text:
      syms = sym_textsyms;
      nsyms = sym_ntextsyms;
      break;
    case sdb_data:
      syms = sym_datasyms;
      nsyms = sym_ndatasyms;
      break;
    default:
      panic("bogus symbol database");
    }

  /* any symbols to search? */
  if (!nsyms)
    {
      if (pindex)
	*pindex = -1;
      return NULL;
    }

  /* binary search symbol database (sorted by address) */
  low = 0;
  high = nsyms-1;
  pos = (low + high) >> 1;
  while (!(/* exact match */
	   (exact && syms[pos]->addr == addr)
	   /* in bounds match */
	   || (!exact
	       && syms[pos]->addr <= addr
	       && addr < (syms[pos]->addr + MAX(1, syms[pos]->size)))))
    {
      if (addr < syms[pos]->addr)
	high = pos - 1;
      else
	low = pos + 1;
      if (high >= low)
	pos = (low + high) >> 1;
      else
	{
	  if (pindex)
	    *pindex = -1;
	  return NULL;
	}
    }

  /* bound! */
  if (pindex)
    *pindex = pos;
  return syms[pos];
}
struct TMIPS::sym_sym_t * TMIPS::				/* symbol found, or NULL */
sym_bind_name(char *name,			/* symbol name to locate */
	      int *pindex,			/* ptr to index result var */
	      enum sym_db_t db)			/* symbol database to search */
{
  int nsyms, low, high, pos, cmp;
  struct sym_sym_t **syms;

  switch (db)
    {
    case sdb_any:
      syms = sym_syms_by_name;
      nsyms = sym_nsyms;
      break;
    case sdb_text:
      syms = sym_textsyms_by_name;
      nsyms = sym_ntextsyms;
      break;
    case sdb_data:
      syms = sym_datasyms_by_name;
      nsyms = sym_ndatasyms;
      break;
    default:
      panic("bogus symbol database");
    }

  /* any symbols to search? */
  if (!nsyms)
    {
      if (pindex)
	*pindex = -1;
      return NULL;
    }

  /* binary search symbol database (sorted by name) */
  low = 0;
  high = nsyms-1;
  pos = (low + high) >> 1;
  while (!(/* exact string match */!(cmp = strcmp(syms[pos]->name, name))))
    {
      if (cmp > 0)
	high = pos - 1;
      else
	low = pos + 1;
      if (high >= low)
	pos = (low + high) >> 1;
      else
	{
	  if (pindex)
	    *pindex = -1;
	  return NULL;
	}
    }

  /* bound! */
  if (pindex)
    *pindex = pos;
  return syms[pos];
}
/*end of symbol.cc*/
/*eio.cc*/
//extern struct exo_term_t;
//extern struct exo_term_t *exo_new(enum exo_class_t ec, ...);
//extern enum exo_class_t{};
FILE * TMIPS::eio_create(char *fname)
{
  FILE *fd;
  struct exo_term_t *exo;
  int target_big_endian;
  target_big_endian = (endian_host_byte_order() == endian_big);

  fd = gzopen(fname, "w");
  if (!fd)
    fatal("unable to create EIO file `%s'", fname);

  /* emit EIO file header */
  fprintf(fd, "%s\n", EIO_FILE_HEADER);
  fprintf(fd, "/* file_format: %d, file_version: %d, big_endian: %d */\n",
	  MD_EIO_FILE_FORMAT, EIO_FILE_VERSION, ld_target_big_endian);
  exo = exo_new(ec_list,
		exo_new(ec_integer, (exo_integer_t)MD_EIO_FILE_FORMAT),
		exo_new(ec_integer, (exo_integer_t)EIO_FILE_VERSION),
		exo_new(ec_integer, (exo_integer_t)target_big_endian),
		NULL);
  exo_print(exo, fd);
  exo_delete(exo);
//  fprintf(fd, "\n\n");

  return fd;
}
//undef of libexo.h
//#undef as_integer	as_integer
//#undef as_address	variant.as_address
//#undef as_float	variant.as_float
//#undef as_char		variant.as_char
//#undef as_string	variant.as_string
//#undef as_list		variant.as_list
//#undef as_array	variant.as_array
//#undef as_token	variant.as_token
//#undef as_blob		variant.as_blob

FILE * TMIPS::eio_open(char *fname)
{
  FILE *fd;
  struct exo_term_t *exo;
  int file_format, file_version, big_endian, target_big_endian;

  target_big_endian = (endian_host_byte_order() == endian_big);

  fd = gzopen(fname, "r");
  if (!fd)
    fatal("unable to open EIO file `%s'", fname);

  /* read and check EIO file header */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_integer
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || !exo->as_list.head->next->next
      || exo->as_list.head->next->next->ec != ec_integer
      || exo->as_list.head->next->next->next != NULL)
    fatal("could not read EIO file header");

  file_format = exo->as_list.head->as_integer.val;
  file_version = exo->as_list.head->next->as_integer.val;
  big_endian = exo->as_list.head->next->next->as_integer.val;
  exo_delete(exo);

  if (file_format != MD_EIO_FILE_FORMAT)
    fatal("EIO file `%s' has incompatible format", fname);

  if (file_version != EIO_FILE_VERSION)
    fatal("EIO file `%s' has incompatible version", fname);

  if (!!big_endian != !!target_big_endian)
    {
      warn("endian of `%s' does not match host", fname);
      warn("running with experimental cross-endian execution support");
      warn("****************************************");
      warn("**>> please check results carefully <<**");
      warn("****************************************");
    }

  return fd;
}

/* returns non-zero if file FNAME has a valid EIO header */
int TMIPS::eio_valid(char *fname)
{
  FILE *fd;
  char buf[512];

  /* open possible EIO file */
  fd = gzopen(fname, "r");
  if (!fd)
    return FALSE;

  /* read and check EIO file header */
  fgets(buf, 512, fd);

  /* check the header */
  if (strcmp(buf, EIO_FILE_HEADER))
    return FALSE;

  /* all done, close up file */
  gzclose(fd);

  /* else, has a valid header, go with it... */
  return TRUE;
}

void
TMIPS::eio_close(FILE *fd)
{
  gzclose(fd);
}

/* check point current architected state to stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t TMIPS::eio_write_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd)			/* stream to write to */
{
  int i;
  struct exo_term_t *exo;
  struct mem_pte_t *pte;

  myfprintf(fd, "/* ** start checkpoint @ %n... */\n\n", eio_trans_icnt);

  myfprintf(fd, "/* EIO file pointer: %n... */\n", eio_trans_icnt);
  exo = exo_new(ec_integer, (exo_integer_t)eio_trans_icnt);
  exo_print(exo, fd);
//  fprintf(fd, "\n\n");
  exo_delete(exo);

  /* dump misc regs: icnt, PC, NPC, etc... */
  fprintf(fd, "/* misc regs icnt, PC, NPC, etc... */\n");
  exo = MD_MISC_REGS_TO_EXO(regs);
  exo_print(exo, fd);
//  fprintf(fd, "\n\n");
  exo_delete(exo);

  /* dump integer registers */
  fprintf(fd, "/* integer regs */\n");
  exo = exo_new(ec_list, NULL);
  for (i=0; i < MD_NUM_IREGS; i++)
    exo->as_list.head = exo_chain(exo->as_list.head, MD_IREG_TO_EXO(regs, i));
  exo_print(exo, fd);
 // fprintf(fd, "\n\n");
  exo_delete(exo);

  /* dump FP registers */
  fprintf(fd, "/* FP regs (integer format) */\n");
  exo = exo_new(ec_list, NULL);
  for (i=0; i < MD_NUM_FREGS; i++)
    exo->as_list.head = exo_chain(exo->as_list.head, MD_FREG_TO_EXO(regs, i));
  exo_print(exo, fd);
//  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* writing `%d' memory pages... */\n", (int)mem->page_count);
  exo = exo_new(ec_list,
		exo_new(ec_integer, (exo_integer_t)mem->page_count),
		exo_new(ec_address, (exo_integer_t)ld_brk_point),
		exo_new(ec_address, (exo_integer_t)ld_stack_min),
		NULL);
  exo_print(exo, fd);
 // fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* text segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)ld_text_base),
		exo_new(ec_integer, (exo_integer_t)ld_text_size),
		NULL);
  exo_print(exo, fd);
//  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* data segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)ld_data_base),
		exo_new(ec_integer, (exo_integer_t)ld_data_size),
		NULL);
  exo_print(exo, fd);
//  fprintf(fd, "\n\n");
  exo_delete(exo);

  fprintf(fd, "/* stack segment specifiers (base & size) */\n");
  exo = exo_new(ec_list,
		exo_new(ec_address, (exo_integer_t)ld_stack_base),
		exo_new(ec_integer, (exo_integer_t)ld_stack_size),
		NULL);
  exo_print(exo, fd);
//  fprintf(fd, "\n\n");
  exo_delete(exo);

  /* visit all active memory pages, and dump them to the checkpoint file */
  MEM_FORALL(mem, i, pte)
    {
      /* dump this page... */
      exo = exo_new(ec_list,
		    exo_new(ec_address, (exo_integer_t)MEM_PTE_ADDR(pte, i)),
		    exo_new(ec_blob, MD_PAGE_SIZE, pte->page),
		    NULL);
      exo_print(exo, fd);
//      fprintf(fd, "\n\n");
      exo_delete(exo);
    }

  myfprintf(fd, "/* ** end checkpoint @ %n... */\n\n", eio_trans_icnt);

  return eio_trans_icnt;
}

/* read check point of architected state from stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t TMIPS::eio_read_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd)			/* stream to read */
{
  int i, page_count;
  counter_t trans_icnt;
  struct exo_term_t *exo, *elt;

  /* read the EIO file pointer */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_integer)
    fatal("could not read EIO file pointer");
  trans_icnt = exo->as_integer.val;
  exo_delete(exo);

  /* read misc regs: icnt, PC, NPC, HI, LO, FCC */
  exo = exo_read(fd);
  MD_EXO_TO_MISC_REGS(exo, sim_num_insn, regs);
  exo_delete(exo);

  /* read integer registers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list)
    fatal("could not read EIO integer regs");
  elt = exo->as_list.head;
  for (i=0; i < MD_NUM_IREGS; i++)
    {
      if (!elt)
	fatal("could not read EIO integer regs (too few)");

      if (elt->ec != ec_address)
	fatal("could not read EIO integer regs (bad value)");

      MD_EXO_TO_IREG(elt, regs, i);
      elt = elt->next;
    }
  if (elt != NULL)
    fatal("could not read EIO integer regs (too many)");
  exo_delete(exo);

  /* read FP registers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list)
    fatal("could not read EIO FP regs");
  elt = exo->as_list.head;
  for (i=0; i < MD_NUM_FREGS; i++)
    {
      if (!elt)
	fatal("could not read EIO FP regs (too few)");

      if (elt->ec != ec_address)
	fatal("could not read EIO FP regs (bad value)");

      MD_EXO_TO_FREG(elt, regs, i);
      elt = elt->next;
    }
  if (elt != NULL)
    fatal("could not read EIO FP regs (too many)");
  exo_delete(exo);

  /* read the number of page defs, and memory config */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_integer
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_address
      || !exo->as_list.head->next->next
      || exo->as_list.head->next->next->ec != ec_address
      || exo->as_list.head->next->next->next != NULL)
    fatal("could not read EIO memory page count");
  page_count = exo->as_list.head->as_integer.val;
  ld_brk_point = (md_addr_t)exo->as_list.head->next->as_address.val;
  ld_stack_min = (md_addr_t)exo->as_list.head->next->next->as_address.val;
  exo_delete(exo);

  /* read text segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO text segment specifiers");
  ld_text_base = (md_addr_t)exo->as_list.head->as_address.val;
  ld_text_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);

  /* read data segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO data segment specifiers");
  ld_data_base = (md_addr_t)exo->as_list.head->as_address.val;
  ld_data_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);

  /* read stack segment specifiers */
  exo = exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    fatal("count not read EIO stack segment specifiers");
  ld_stack_base = (md_addr_t)exo->as_list.head->as_address.val;
  ld_stack_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  exo_delete(exo);

  for (i=0; i < page_count; i++)
    {
      int j;
      md_addr_t page_addr;
      struct exo_term_t *blob;

      /* read the page */
      exo = exo_read(fd);
      if (!exo
	  || exo->ec != ec_list
	  || !exo->as_list.head
	  || exo->as_list.head->ec != ec_address
	  || !exo->as_list.head->next
	  || exo->as_list.head->next->ec != ec_blob
	  || exo->as_list.head->next->next != NULL)
	fatal("could not read EIO memory page");
      page_addr = (md_addr_t)exo->as_list.head->as_address.val;
      blob = exo->as_list.head->next;

      /* write data to simulator memory */
      for (j=0; j < blob->as_blob.size; j++)
	{
	  byte_t val;

	  val = blob->as_blob.data[j];
	  /* unchecked access... */
	  MEM_WRITE_BYTE(mem, page_addr, val);
	  page_addr++;
	}
      exo_delete(exo);
    }

  return trans_icnt;
}
void
TMIPS::eio_read_trace(FILE *eio_fd,			/* EIO stream file desc */
	       counter_t icnt,			/* instruction count */
	       struct regs_t *regs,		/* registers to update */
	       mem_access_fn mem_fn,		/* generic memory accessor */
	       struct mem_t *mem,		/* memory to update */
	       md_inst_t inst)			/* system call inst */
{
  int i;
  struct exo_term_t *exo, *exo_icnt, *exo_pc;
  struct exo_term_t *exo_inregs, *exo_inmem, *exo_outregs, *exo_outmem;
  struct exo_term_t *brkrec, *regrec, *memrec;

  /* exit() system calls get executed for real... */
  if (MD_EXIT_SYSCALL(regs))
    {
      sys_syscall(regs, mem_fn, mem, inst, FALSE);
      panic("returned from exit() system call");
    }

  /* else, read the external I/O (EIO) transaction */
  exo = exo_read(eio_fd);

  /* one more transaction processed */
  eio_trans_icnt = icnt;

  /* pull apart the EIO transaction (EXO format) */
  if (!exo
      || exo->ec != ec_list
      || !(exo_icnt = exo->as_list.head)
      || exo_icnt->ec != ec_integer
      || !(exo_pc = exo_icnt->next)
      || exo_pc->ec != ec_address
      || !(exo_inregs = exo_pc->next)
      || exo_inregs->ec != ec_list
      || !(exo_inmem = exo_inregs->next)
      || exo_inmem->ec != ec_list
      || !(exo_outregs = exo_inmem->next)
      || exo_outregs->ec != ec_list
      || !(exo_outmem = exo_outregs->next)
      || exo_outmem->ec != ec_list
      || exo_outmem->next != NULL)
    fatal("cannot read EIO transaction");

  /*
   * check the system call inputs
   */

  /* check ICNT input */
  if (icnt != (counter_t)exo_icnt->as_integer.val)
    fatal("EIO trace inconsistency: ICNT mismatch");

  /* check PC input */
  if (regs->regs_PC != (md_addr_t)exo_pc->as_integer.val)
    fatal("EIO trace inconsistency: PC mismatch");

  /* check integer register inputs */
  for (i=MD_FIRST_IN_REG, regrec=exo_inregs->as_list.head;
       i <= MD_LAST_IN_REG; i++, regrec=regrec->next)
    {
      if (!regrec || regrec->ec != ec_address)
	fatal("EIO trace inconsistency: missing input reg");

      if (MD_EXO_CMP_IREG(regrec, regs, i))
	fatal("EIO trace inconsistency: R[%d] input mismatch", i);
#ifdef VERBOSE
      fprintf(stderr, "** R[%d] checks out...\n", i);
#endif /* VERBOSE */
    }
  if (regrec != NULL)
    fatal("EIO trace inconsistency: too many input regs");

  /* check memory inputs */
  for (memrec=exo_inmem->as_list.head; memrec != NULL; memrec=memrec->next)
    {
      md_addr_t loc;
      struct exo_term_t *addr, *blob;

      /* check the mem transaction format */
      if (!memrec
	  || memrec->ec != ec_list
	  || !(addr = memrec->as_list.head)
	  || addr->ec != ec_address
	  || !(blob = addr->next)
	  || blob->ec != ec_blob
	  || blob->next != NULL)
	fatal("EIO trace inconsistency: bad memory transaction");

      for (loc=addr->as_integer.val, i=0; i < blob->as_blob.size; loc++,i++)
	{
	  unsigned char val;

	  /* was: val = MEM_READ_BYTE(loc); */
	  (this->*mem_fn)(mem, Read, loc, &val, sizeof(unsigned char));

	  if (val != blob->as_blob.data[i])
	    fatal("EIO trace inconsistency: addr 0x%08p input mismatch", loc);

#ifdef VERBOSE
	  myfprintf(stderr, "** 0x%08p checks out...\n", loc);
#endif /* VERBOSE */
	}

      /* simulate view'able I/O */
      if (MD_OUTPUT_SYSCALL(regs))
	{
	  if (sim_progfd)
	    {
	      /* redirect program output to file */
	      fwrite(blob->as_blob.data, 1, blob->as_blob.size, fopen(sim_progfd,"w+"));
	    }
	  else
	    {
	      /* write the output to stdout/stderr */
	      write(MD_STREAM_FILENO(regs),
		    blob->as_blob.data, blob->as_blob.size);
	    }
	}
    }

  /*
   * write system call outputs
   */

  /* adjust breakpoint */
  brkrec = exo_outregs->as_list.head;
  if (!brkrec || brkrec->ec != ec_address)
    fatal("EIO trace inconsistency: missing memory breakpoint");
  ld_brk_point = (md_addr_t)brkrec->as_integer.val;

  /* write integer register outputs */
  for (i=MD_FIRST_OUT_REG, regrec=exo_outregs->as_list.head->next;
       i <= MD_LAST_OUT_REG; i++, regrec=regrec->next)
    {
      if (!regrec || regrec->ec != ec_address)
	fatal("EIO trace inconsistency: missing output reg");

      MD_EXO_TO_IREG(regrec, regs, i);

#ifdef VERBOSE
      fprintf(stderr, "** R[%d] written...\n", i);
#endif /* VERBOSE */
    }
  if (regrec != NULL)
    fatal("EIO trace inconsistency: too many output regs");

  /* write memory outputs */
  for (memrec=exo_outmem->as_list.head; memrec != NULL; memrec=memrec->next)
    {
      md_addr_t loc;
      struct exo_term_t *addr, *blob;

      /* check the mem transaction format */
      if (!memrec
	  || memrec->ec != ec_list
	  || !(addr = memrec->as_list.head)
	  || addr->ec != ec_address
	  || !(blob = addr->next)
	  || blob->ec != ec_blob
	  || blob->next != NULL)
	fatal("EIO trace icnonsistency: bad memory transaction");

      for (loc=addr->as_integer.val, i=0; i < blob->as_blob.size; loc++,i++)
	{
	  /* was: MEM_WRITE_BYTE(loc, blob->as_blob.data[i]); */
	  (this->*mem_fn)(mem, Write,
		    loc, &blob->as_blob.data[i], sizeof(unsigned char));

#ifdef VERBOSE
	  fprintf(stderr, "** 0x%08p written...\n", loc);
#endif /* VERBOSE */
	}
    }

  /* release the EIO EXO node */
  exo_delete(exo);
}

void
TMIPS::eio_fast_forward(FILE *eio_fd, counter_t icnt)
{
  struct exo_term_t *exo, *exo_icnt;

  do
    {
      /* read the next external I/O (EIO) transaction */
      exo = exo_read(eio_fd);

      if (!exo)
	fatal("could not fast forward to EIO checkpoint");

      /* one more transaction processed */
      eio_trans_icnt = icnt;

      /* pull apart the EIO transaction (EXO format) */
      if (!exo
	  || exo->ec != ec_list
	  || !(exo_icnt = exo->as_list.head)
	  || exo_icnt->ec != ec_integer)
	fatal("cannot read EIO transaction (during fast forward)");
    }
  while ((counter_t)exo_icnt->as_integer.val != icnt);

  /* found it! */
}


/*end of eio.cc*/
/*cache.cc*/
struct TMIPS::cache_t *	TMIPS::cache_create(char *name,		/* name of the cache */
	     int nsets,			/* total number of sets in cache */
	     int bsize,			/* block (line) size of cache */
	     int balloc,		/* allocate data space for blocks? */
	     int usize,			/* size of user data to alloc w/blks */
	     int assoc,			/* associativity of cache */
	     enum cache_policy policy,	/* replacement policy w/in sets */
	     /* block access function, see description w/in struct cache def */
	     unsigned int (TMIPS::*blk_access_fn)(enum mem_cmd cmd,
					   md_addr_t baddr, int bsize,
					   struct cache_blk_t *blk,
					   tick_t now),
	     unsigned int hit_latency)	/* latency in cycles for a hit */
{
  struct cache_t *cp;
  struct cache_blk_t *blk;
  int i, j, bindex;

  /* check all cache parameters */
  if (nsets <= 0)
    fatal("cache size (in sets) `%d' must be non-zero", nsets);
  if ((nsets & (nsets-1)) != 0)
    fatal("cache size (in sets) `%d' is not a power of two", nsets);
  /* blocks must be at least one datum large, i.e., 8 bytes for SS */
  if (bsize < 8)
    fatal("cache block size (in bytes) `%d' must be 8 or greater", bsize);
  if ((bsize & (bsize-1)) != 0)
    fatal("cache block size (in bytes) `%d' must be a power of two", bsize);
  if (usize < 0)
    fatal("user data size (in bytes) `%d' must be a positive value", usize);
  if (assoc <= 0)
    fatal("cache associativity `%d' must be non-zero and positive", assoc);
  if ((assoc & (assoc-1)) != 0)
    fatal("cache associativity `%d' must be a power of two", assoc);
  if (!blk_access_fn)
    fatal("must specify miss/replacement functions");

  /* allocate the cache structure */
  cp = (struct cache_t *)
    calloc(1, sizeof(struct cache_t) + (nsets-1)*sizeof(struct cache_set_t));
  if (!cp)
    fatal("out of virtual memory");

  /* initialize user parameters */
  cp->name = mystrdup(name);
  cp->nsets = nsets;
  cp->bsize = bsize;
  cp->balloc = balloc;
  cp->usize = usize;
  cp->assoc = assoc;
  cp->policy = policy;
  cp->hit_latency = hit_latency;

  /* miss/replacement functions */
  cp->blk_access_fn = blk_access_fn;

  /* compute derived parameters */
  cp->hsize = CACHE_HIGHLY_ASSOC(cp) ? (assoc >> 2) : 0;
  cp->blk_mask = bsize-1;
  cp->set_shift = log_base2(bsize);
  cp->set_mask = nsets-1;
  cp->tag_shift = cp->set_shift + log_base2(nsets);
  cp->tag_mask = (1 << (32 - cp->tag_shift))-1;
  cp->tagset_mask = ~cp->blk_mask;
  cp->bus_free = 0;

  /* print derived parameters during debug */
  debug("%s: cp->hsize     = %d", cp->name, cp->hsize);
  debug("%s: cp->blk_mask  = 0x%08x", cp->name, cp->blk_mask);
  debug("%s: cp->set_shift = %d", cp->name, cp->set_shift);
  debug("%s: cp->set_mask  = 0x%08x", cp->name, cp->set_mask);
  debug("%s: cp->tag_shift = %d", cp->name, cp->tag_shift);
  debug("%s: cp->tag_mask  = 0x%08x", cp->name, cp->tag_mask);

  /* initialize cache stats */
  cp->hits = 0;
  cp->misses = 0;
  cp->replacements = 0;
  cp->writebacks = 0;
  cp->invalidations = 0;

  /* blow away the last block accessed */
  cp->last_tagset = 0;
  cp->last_blk = NULL;

  /* allocate data blocks */
  cp->data = (byte_t *)calloc(nsets * assoc,
			      sizeof(struct cache_blk_t) +
			      (cp->balloc ? (bsize*sizeof(byte_t)) : 0));
  if (!cp->data)
    fatal("out of virtual memory");

  /* slice up the data blocks */
  for (bindex=0,i=0; i<nsets; i++)
    {
      cp->sets[i].way_head = NULL;
      cp->sets[i].way_tail = NULL;
      /* get a hash table, if needed */
      if (cp->hsize)
	{
	  cp->sets[i].hash =
	    (struct cache_blk_t **)calloc(cp->hsize,
					  sizeof(struct cache_blk_t *));
	  if (!cp->sets[i].hash)
	    fatal("out of virtual memory");
	}
      /* NOTE: all the blocks in a set *must* be allocated contiguously,
	 otherwise, block accesses through SET->BLKS will fail (used
	 during random replacement selection) */
      cp->sets[i].blks = CACHE_BINDEX(cp, cp->data, bindex);

      /* link the data blocks into ordered way chain and hash table bucket
         chains, if hash table exists */
      for (j=0; j<assoc; j++)
	{
	  /* locate next cache block */
	  blk = CACHE_BINDEX(cp, cp->data, bindex);
	  bindex++;

	  /* invalidate new cache block */
	  blk->status = 0;
	  blk->tag = 0;
	  blk->ready = 0;
	  blk->user_data = (usize != 0
			    ? (byte_t *)calloc(usize, sizeof(byte_t)) : NULL);

	  /* insert cache block into set hash table */
	  if (cp->hsize)
	    link_htab_ent(cp, &cp->sets[i], blk);

	  /* insert into head of way list, order is arbitrary at this point */
	  blk->way_next = cp->sets[i].way_head;
	  blk->way_prev = NULL;
	  if (cp->sets[i].way_head)
	    cp->sets[i].way_head->way_prev = blk;
	  cp->sets[i].way_head = blk;
	  if (!cp->sets[i].way_tail)
	    cp->sets[i].way_tail = blk;
	}
    }
  return cp;
}
enum TCore::cache_policy TMIPS::cache_char2policy(char c)		/* replacement policy as a char */
{
  switch (c) {
  case 'l': return LRU;
  case 'r': return Random;
  case 'f': return FIFO;
  default: fatal("bogus replacement policy, `%c'", c);
  }
}
/* print cache configuration */
void TMIPS::cache_config(struct cache_t *cp,	/* cache instance */
	     FILE *stream)		/* output stream */
{
  fprintf(stream,
	  "cache: %s: %d sets, %d byte blocks, %d bytes user data/block\n",
	  cp->name, cp->nsets, cp->bsize, cp->usize);
  fprintf(stream,
	  "cache: %s: %d-way, `%s' replacement policy, write-back\n",
	  cp->name, cp->assoc,
	  cp->policy == LRU ? "LRU"
	  : cp->policy == Random ? "Random"
	  : cp->policy == FIFO ? "FIFO"
	  : (abort(), ""));
}



/* print cache stats */
void TMIPS::cache_stats(struct cache_t *cp,		/* cache instance */
	    FILE *stream)		/* output stream */
{
  double sum = (double)(cp->hits + cp->misses);

  fprintf(stream,
	  "cache: %s: %.0f hits %.0f misses %.0f repls %.0f invalidations\n",
	  cp->name, (double)cp->hits, (double)cp->misses,
	  (double)cp->replacements, (double)cp->invalidations);
  fprintf(stream,
	  "cache: %s: miss rate=%f  repl rate=%f  invalidation rate=%f\n",
	  cp->name,
	  (double)cp->misses/sum, (double)(double)cp->replacements/sum,
	  (double)cp->invalidations/sum);
}
unsigned int TMIPS::cache_access(struct cache_t *cp,	/* cache to access */
	     enum mem_cmd cmd,		/* access type, Read or Write */
	     md_addr_t addr,		/* address of access */
	     void *vp,			/* ptr to buffer for input/output */
	     int nbytes,		/* number of bytes to access */
	     tick_t now,		/* time of access */
	     byte_t **udata,		/* for return of user data ptr */
	     md_addr_t *repl_addr)	/* for address of replaced block */
{
  byte_t *p = (byte_t *)vp;
  md_addr_t tag = CACHE_TAG(cp, addr);
  md_addr_t set = CACHE_SET(cp, addr);
  md_addr_t bofs = CACHE_BLK(cp, addr);
  struct cache_blk_t *blk, *repl;
  int lat = 0;

  /* default replacement address */
  if (repl_addr)
    *repl_addr = 0;

  /* check alignments */
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    fatal("cache: access error: bad size or alignment, addr 0x%08x", addr);

  /* access must fit in cache block */
  /* FIXME:
     ((addr + (nbytes - 1)) > ((addr & ~cp->blk_mask) + (cp->bsize - 1))) */
  if ((addr + nbytes) > ((addr & ~cp->blk_mask) + cp->bsize))
    fatal("cache: access error: access spans block, addr 0x%08x", addr);

  /* permissions are checked on cache misses */

  /* check for a fast hit: access to same block */
  if (CACHE_TAGSET(cp, addr) == cp->last_tagset)
    {
      /* hit in the same block */
      blk = cp->last_blk;
      goto cache_fast_hit;
    }

  if (cp->hsize)
    {
      /* higly-associativity cache, access through the per-set hash tables */
      int hindex = CACHE_HASH(cp, tag);

      for (blk=cp->sets[set].hash[hindex];
	   blk;
	   blk=blk->hash_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }
  else
    {
      /* low-associativity cache, linear search the way list */
      for (blk=cp->sets[set].way_head;
	   blk;
	   blk=blk->way_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }

  /* cache block not found */

  /* **MISS** */
  cp->misses++;
  if(!strcmp(cp->name,"itlb")){
  	  thispower->gpp_data->tlb_para->total_itlb_miss_num +=1;
  	  allpower->gpp_data->tlb_para->total_itlb_miss_num +=1;
    }
  else if(!strcmp(cp->name,"dtlb")){
    	  thispower->gpp_data->tlb_para->total_dtlb_miss_num +=1;
    	  allpower->gpp_data->tlb_para->total_dtlb_miss_num +=1;
    }
  /* select the appropriate block to replace, and re-link this entry to
     the appropriate place in the way list */
  switch (cp->policy) {
  case LRU:
  case FIFO:
    repl = cp->sets[set].way_tail;
    update_way_list(&cp->sets[set], repl, Head);
    break;
  case Random:
    {
      int bindex = myrand() & (cp->assoc - 1);
      repl = CACHE_BINDEX(cp, cp->sets[set].blks, bindex);
    }
    break;
  default:
    panic("bogus replacement policy");
  }

  /* remove this block from the hash bucket chain, if hash exists */
  if (cp->hsize)
    unlink_htab_ent(cp, &cp->sets[set], repl);

  /* blow away the last block to hit */
  cp->last_tagset = 0;
  cp->last_blk = NULL;

  /* write back replaced block data */
  if (repl->status & CACHE_BLK_VALID)
    {
      cp->replacements++;

      if (repl_addr)
	*repl_addr = CACHE_MK_BADDR(cp, repl->tag, set);

      /* don't replace the block until outstanding misses are satisfied */
      lat += BOUND_POS(repl->ready - now);

      /* stall until the bus to next level of memory is available */
      lat += BOUND_POS(cp->bus_free - (now + lat));

      /* track bus resource usage */
      cp->bus_free = MAX(cp->bus_free, (now + lat)) + 1;

      if (repl->status & CACHE_BLK_DIRTY)
	{
	  /* write back the cache block */
	  cp->writebacks++;
	  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Write,
				   CACHE_MK_BADDR(cp, repl->tag, set),
				   cp->bsize, repl, now+lat);
	}
    }

  /* update block tags */
  repl->tag = tag;
  repl->status = CACHE_BLK_VALID;	/* dirty bit set on update */

  /* read data block */
  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Read, CACHE_BADDR(cp, addr), cp->bsize,
			   repl, now+lat);

  /* copy data out of cache block */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, repl, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    repl->status |= CACHE_BLK_DIRTY;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = repl->user_data;

  /* update block status */
  repl->ready = now+lat;

  /* link this entry back into the hash table */
  if (cp->hsize)
    link_htab_ent(cp, &cp->sets[set], repl);

  /* return latency of the operation */
  return lat;


 cache_hit: /* slow hit handler */

  /* **HIT** */

  cp->hits++;
  if(!strcmp(cp->name,"itlb")){
	  thispower->gpp_data->tlb_para->total_itlb_access_num +=1;
	  allpower->gpp_data->tlb_para->total_itlb_access_num +=1;
   }
  else if(!strcmp(cp->name,"dtlb")){
     	  thispower->gpp_data->tlb_para->total_dtlb_access_num +=1;
     	  allpower->gpp_data->tlb_para->total_dtlb_access_num +=1;
    }
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* if LRU replacement and this is not the first element of list, reorder */
  if (blk->way_prev && cp->policy == LRU)
    {
      /* move this block to head of the way (MRU) list */
      update_way_list(&cp->sets[set], blk, Head);
    }

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));

 cache_fast_hit: /* fast hit handler */

  /* **FAST HIT** */
  cp->hits++;
  if(!strcmp(cp->name,"itlb")){
  	  thispower->gpp_data->tlb_para->total_itlb_access_num +=1;
  	  allpower->gpp_data->tlb_para->total_itlb_access_num +=1;
    }
  else if(!strcmp(cp->name,"dtlb")){
     	  thispower->gpp_data->tlb_para->total_dtlb_access_num +=1;
     	  allpower->gpp_data->tlb_para->total_dtlb_access_num +=1;
     }
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* this block hit last, no change in the way list */

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));
}

unsigned int TMIPS::cache_access_il1(struct cache_t *cp,	/* cache to access */
	     enum mem_cmd cmd,		/* access type, Read or Write */
	     md_addr_t addr,		/* address of access */
	     void *vp,			/* ptr to buffer for input/output */
	     int nbytes,		/* number of bytes to access */
	     tick_t now,		/* time of access */
	     byte_t **udata,		/* for return of user data ptr */
	     md_addr_t *repl_addr)	/* for address of replaced block */
{
  byte_t *p = (byte_t *)vp;
  md_addr_t tag = CACHE_TAG(cp, addr);
  md_addr_t set = CACHE_SET(cp, addr);
  md_addr_t bofs = CACHE_BLK(cp, addr);
  struct cache_blk_t *blk, *repl;
  int lat = 0;

  /* default replacement address */
  if (repl_addr)
    *repl_addr = 0;

  /* check alignments */
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    fatal("cache: access error: bad size or alignment, addr 0x%08x", addr);

  /* access must fit in cache block */
  /* FIXME:
     ((addr + (nbytes - 1)) > ((addr & ~cp->blk_mask) + (cp->bsize - 1))) */
  if ((addr + nbytes) > ((addr & ~cp->blk_mask) + cp->bsize))
    fatal("cache: access error: access spans block, addr 0x%08x", addr);

  /* permissions are checked on cache misses */

  /* check for a fast hit: access to same block */
  if (CACHE_TAGSET(cp, addr) == cp->last_tagset)
    {
      /* hit in the same block */
      blk = cp->last_blk;
      goto cache_fast_hit;
    }

  if (cp->hsize)
    {
      /* higly-associativity cache, access through the per-set hash tables */
      int hindex = CACHE_HASH(cp, tag);

      for (blk=cp->sets[set].hash[hindex];
	   blk;
	   blk=blk->hash_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }
  else
    {
      /* low-associativity cache, linear search the way list */
      for (blk=cp->sets[set].way_head;
	   blk;
	   blk=blk->way_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }

  /* cache block not found */

  /* **MISS** */
//  fputs("il1_cache_miss:0 ",Simulation::visualFile);

  esynetrcd->l1icache_miss += 1;
  esynetrcd->valid = 1;
  cp->misses++;
  thispower->gpp_data->cache_para->total_icache_read_miss_num +=1;
  allpower->gpp_data->cache_para->total_icache_read_miss_num +=1;
//  printf("l1_i_cache miss num: %lld",cp->misses);
  /* select the appropriate block to replace, and re-link this entry to
     the appropriate place in the way list */
  switch (cp->policy) {
  case LRU:
  case FIFO:
    repl = cp->sets[set].way_tail;
    update_way_list(&cp->sets[set], repl, Head);
    break;
  case Random:
    {
      int bindex = myrand() & (cp->assoc - 1);
      repl = CACHE_BINDEX(cp, cp->sets[set].blks, bindex);
    }
    break;
  default:
    panic("bogus replacement policy");
  }

  /* remove this block from the hash bucket chain, if hash exists */
  if (cp->hsize)
    unlink_htab_ent(cp, &cp->sets[set], repl);

  /* blow away the last block to hit */
  cp->last_tagset = 0;
  cp->last_blk = NULL;

  /* write back replaced block data */
  if (repl->status & CACHE_BLK_VALID)
    {
      cp->replacements++;

      if (repl_addr)
	*repl_addr = CACHE_MK_BADDR(cp, repl->tag, set);

      /* don't replace the block until outstanding misses are satisfied */
      lat += BOUND_POS(repl->ready - now);

      /* stall until the bus to next level of memory is available */
      lat += BOUND_POS(cp->bus_free - (now + lat));

      /* track bus resource usage */
      cp->bus_free = MAX(cp->bus_free, (now + lat)) + 1;

      if (repl->status & CACHE_BLK_DIRTY)
	{
	  /* write back the cache block */
	  cp->writebacks++;
	  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Write,
				   CACHE_MK_BADDR(cp, repl->tag, set),
				   cp->bsize, repl, now+lat);
	}
    }

  /* update block tags */
  repl->tag = tag;
  repl->status = CACHE_BLK_VALID;	/* dirty bit set on update */

  /* read data block */
  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Read, CACHE_BADDR(cp, addr), cp->bsize,
			   repl, now+lat);

  /* copy data out of cache block */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, repl, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    repl->status |= CACHE_BLK_DIRTY;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = repl->user_data;

  /* update block status */
  repl->ready = now+lat;

  /* link this entry back into the hash table */
  if (cp->hsize)
    link_htab_ent(cp, &cp->sets[set], repl);

  /* return latency of the operation */
  return lat;


 cache_hit: /* slow hit handler */
  /* **HIT** */
// fputs("il1_cache_hit:1 ",Simulation::visualFile);
 	 esynetrcd->l1icache_hit += 1;
 	 esynetrcd->valid = 1;
 	 cp->hits++;
 	 thispower->gpp_data->cache_para->total_icache_read_access_num +=1;
 	 allpower->gpp_data->cache_para->total_icache_read_access_num +=1;
//  printf("l1_i_cache miss num: %lld",cp->misses);

  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* if LRU replacement and this is not the first element of list, reorder */
  if (blk->way_prev && cp->policy == LRU)
    {
      /* move this block to head of the way (MRU) list */
      update_way_list(&cp->sets[set], blk, Head);
    }

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));

 cache_fast_hit: /* fast hit handler */

  /* **FAST HIT** */
// fputs("il1_cache_hit:1 ",Simulation::visualFile);
 	 esynetrcd->l1icache_hit += 1;
 	 esynetrcd->valid = 1;
 	 cp->hits++;
 	 thispower->gpp_data->cache_para->total_icache_read_access_num +=1;
 	 allpower->gpp_data->cache_para->total_icache_read_access_num +=1;
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* this block hit last, no change in the way list */

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));
}

unsigned int TMIPS::cache_access_il2(struct cache_t *cp,	/* cache to access */
	     enum mem_cmd cmd,		/* access type, Read or Write */
	     md_addr_t addr,		/* address of access */
	     void *vp,			/* ptr to buffer for input/output */
	     int nbytes,		/* number of bytes to access */
	     tick_t now,		/* time of access */
	     byte_t **udata,		/* for return of user data ptr */
	     md_addr_t *repl_addr)	/* for address of replaced block */
{
  byte_t *p = (byte_t *)vp;
  md_addr_t tag = CACHE_TAG(cp, addr);
  md_addr_t set = CACHE_SET(cp, addr);
  md_addr_t bofs = CACHE_BLK(cp, addr);
  struct cache_blk_t *blk, *repl;
  int lat = 0;

  /* default replacement address */
  if (repl_addr)
    *repl_addr = 0;

  thispower->gpp_data->cache_para->l2cache_cycle_duty +=1;
  /* check alignments */
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    fatal("cache: access error: bad size or alignment, addr 0x%08x", addr);

  /* access must fit in cache block */
  /* FIXME:
     ((addr + (nbytes - 1)) > ((addr & ~cp->blk_mask) + (cp->bsize - 1))) */
  if ((addr + nbytes) > ((addr & ~cp->blk_mask) + cp->bsize))
    fatal("cache: access error: access spans block, addr 0x%08x", addr);

  /* permissions are checked on cache misses */

  /* check for a fast hit: access to same block */
  if (CACHE_TAGSET(cp, addr) == cp->last_tagset)
    {
      /* hit in the same block */
      blk = cp->last_blk;
      goto cache_fast_hit;
    }

  if (cp->hsize)
    {
      /* higly-associativity cache, access through the per-set hash tables */
      int hindex = CACHE_HASH(cp, tag);

      for (blk=cp->sets[set].hash[hindex];
	   blk;
	   blk=blk->hash_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }
  else
    {
      /* low-associativity cache, linear search the way list */
      for (blk=cp->sets[set].way_head;
	   blk;
	   blk=blk->way_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }

  /* cache block not found */

  /* **MISS** */
  	  cp->misses++;
  	  esynetrcd->l2icache_miss += 1;
  	  esynetrcd->valid = 1;
  	  thispower->gpp_data->cache_para->total_icache2_read_miss_num +=1;
  	  allpower->gpp_data->cache_para->total_icache2_read_miss_num +=1;
  /* select the appropriate block to replace, and re-link this entry to
     the appropriate place in the way list */
  switch (cp->policy) {
  case LRU:
  case FIFO:
    repl = cp->sets[set].way_tail;
    update_way_list(&cp->sets[set], repl, Head);
    break;
  case Random:
    {
      int bindex = myrand() & (cp->assoc - 1);
      repl = CACHE_BINDEX(cp, cp->sets[set].blks, bindex);
    }
    break;
  default:
    panic("bogus replacement policy");
  }

  /* remove this block from the hash bucket chain, if hash exists */
  if (cp->hsize)
    unlink_htab_ent(cp, &cp->sets[set], repl);

  /* blow away the last block to hit */
  cp->last_tagset = 0;
  cp->last_blk = NULL;

  /* write back replaced block data */
  if (repl->status & CACHE_BLK_VALID)
    {
      cp->replacements++;

      if (repl_addr)
	*repl_addr = CACHE_MK_BADDR(cp, repl->tag, set);

      /* don't replace the block until outstanding misses are satisfied */
      lat += BOUND_POS(repl->ready - now);

      /* stall until the bus to next level of memory is available */
      lat += BOUND_POS(cp->bus_free - (now + lat));

      /* track bus resource usage */
      cp->bus_free = MAX(cp->bus_free, (now + lat)) + 1;

      if (repl->status & CACHE_BLK_DIRTY)
	{
	  /* write back the cache block */
	  cp->writebacks++;
	  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Write,
				   CACHE_MK_BADDR(cp, repl->tag, set),
				   cp->bsize, repl, now+lat);
	}
    }

  /* update block tags */
  repl->tag = tag;
  repl->status = CACHE_BLK_VALID;	/* dirty bit set on update */

  /* read data block */
  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Read, CACHE_BADDR(cp, addr), cp->bsize,
			   repl, now+lat);

  /* copy data out of cache block */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, repl, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    repl->status |= CACHE_BLK_DIRTY;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = repl->user_data;

  /* update block status */
  repl->ready = now+lat;

  /* link this entry back into the hash table */
  if (cp->hsize)
    link_htab_ent(cp, &cp->sets[set], repl);

  /* return latency of the operation */
  return lat;


 cache_hit: /* slow hit handler */
  /* **HIT** */
 	 esynetrcd->l2icache_hit += 1;
 	 esynetrcd->valid = 1;
 	 cp->hits++;
 	 thispower->gpp_data->cache_para->total_icache2_read_access_num +=1;
 	 allpower->gpp_data->cache_para->total_icache2_read_access_num +=1;
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* if LRU replacement and this is not the first element of list, reorder */
  if (blk->way_prev && cp->policy == LRU)
    {
      /* move this block to head of the way (MRU) list */
      update_way_list(&cp->sets[set], blk, Head);
    }

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));

 cache_fast_hit: /* fast hit handler */

  /* **FAST HIT** */
// fputs("il2_cache_hit:1 ",Simulation::visualFile);
 	  esynetrcd->l2icache_hit += 1;
 	  esynetrcd->valid = 1;
  	  cp->hits++;
  	  thispower->gpp_data->cache_para->total_icache2_read_access_num +=1;
  	  allpower->gpp_data->cache_para->total_icache2_read_access_num +=1;
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* this block hit last, no change in the way list */

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));
}

unsigned int TMIPS::cache_access_dl1(struct cache_t *cp,	/* cache to access */
	     enum mem_cmd cmd,		/* access type, Read or Write */
	     md_addr_t addr,		/* address of access */
	     void *vp,			/* ptr to buffer for input/output */
	     int nbytes,		/* number of bytes to access */
	     tick_t now,		/* time of access */
	     byte_t **udata,		/* for return of user data ptr */
	     md_addr_t *repl_addr)	/* for address of replaced block */
{

  byte_t *p = (byte_t *)vp;
  md_addr_t tag = CACHE_TAG(cp, addr);
  md_addr_t set = CACHE_SET(cp, addr);
  md_addr_t bofs = CACHE_BLK(cp, addr);
  struct cache_blk_t *blk, *repl;
  int lat = 0;

  /* default replacement address */
  if (repl_addr)
    *repl_addr = 0;

  /* check alignments */
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    fatal("cache: access error: bad size or alignment, addr 0x%08x", addr);

  /* access must fit in cache block */
  /* FIXME:
     ((addr + (nbytes - 1)) > ((addr & ~cp->blk_mask) + (cp->bsize - 1))) */
  if ((addr + nbytes) > ((addr & ~cp->blk_mask) + cp->bsize))
    fatal("cache: access error: access spans block, addr 0x%08x", addr);

  esynetrcd->l1dcache_access += 1;
  esynetrcd->valid = 1;
  /* permissions are checked on cache misses */

  /* check for a fast hit: access to same block */
  if (CACHE_TAGSET(cp, addr) == cp->last_tagset)
    {
      /* hit in the same block */
      blk = cp->last_blk;
      goto cache_fast_hit;
    }

  if (cp->hsize)
    {
      /* higly-associativity cache, access through the per-set hash tables */
      int hindex = CACHE_HASH(cp, tag);

      for (blk=cp->sets[set].hash[hindex];
	   blk;
	   blk=blk->hash_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }
  else
    {
      /* low-associativity cache, linear search the way list */
      for (blk=cp->sets[set].way_head;
	   blk;
	   blk=blk->way_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }

  /* cache block not found */

  /* **MISS** */
  	  cp->misses++;
  	  esynetrcd->l1dcache_miss += 1;
  	  esynetrcd->valid = 1;
  	  if(cmd == Read){
  		  thispower->gpp_data->cache_para->total_dcache_read_miss_num +=1;
  		  allpower->gpp_data->cache_para->total_dcache_read_miss_num +=1;
  	  }
  	  else if(cmd==Write){
  		  thispower->gpp_data->cache_para->total_dcache_write_miss_num +=1;
  		  allpower->gpp_data->cache_para->total_dcache_write_miss_num +=1;
  	  }

  /* select the appropriate block to replace, and re-link this entry to
     the appropriate place in the way list */
  switch (cp->policy) {
  case LRU:
  case FIFO:
    repl = cp->sets[set].way_tail;
    update_way_list(&cp->sets[set], repl, Head);
    break;
  case Random:
    {
      int bindex = myrand() & (cp->assoc - 1);
      repl = CACHE_BINDEX(cp, cp->sets[set].blks, bindex);
    }
    break;
  default:
    panic("bogus replacement policy");
  }

  /* remove this block from the hash bucket chain, if hash exists */
  if (cp->hsize)
    unlink_htab_ent(cp, &cp->sets[set], repl);

  /* blow away the last block to hit */
  cp->last_tagset = 0;
  cp->last_blk = NULL;

  /* write back replaced block data */
  if (repl->status & CACHE_BLK_VALID)
    {
      cp->replacements++;

      if (repl_addr)
	*repl_addr = CACHE_MK_BADDR(cp, repl->tag, set);

      /* don't replace the block until outstanding misses are satisfied */
      lat += BOUND_POS(repl->ready - now);

      /* stall until the bus to next level of memory is available */
      lat += BOUND_POS(cp->bus_free - (now + lat));

      /* track bus resource usage */
      cp->bus_free = MAX(cp->bus_free, (now + lat)) + 1;

      if (repl->status & CACHE_BLK_DIRTY)
	{
	  /* write back the cache block */
	  cp->writebacks++;
	  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Write,
				   CACHE_MK_BADDR(cp, repl->tag, set),
				   cp->bsize, repl, now+lat);
	}
    }

  /* update block tags */
  repl->tag = tag;
  repl->status = CACHE_BLK_VALID;	/* dirty bit set on update */

  /* read data block */
  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Read, CACHE_BADDR(cp, addr), cp->bsize,
			   repl, now+lat);

  /* copy data out of cache block */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, repl, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    repl->status |= CACHE_BLK_DIRTY;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = repl->user_data;

  /* update block status */
  repl->ready = now+lat;

  /* link this entry back into the hash table */
  if (cp->hsize)
    link_htab_ent(cp, &cp->sets[set], repl);

  /* return latency of the operation */
  return lat;


 cache_hit: /* slow hit handler */

  /* **HIT** */
 	 esynetrcd->l1dcache_hit += 1;
 	 esynetrcd->valid = 1;
  	 cp->hits++;
	 if(cmd == Read){
		 thispower->gpp_data->cache_para->total_dcache_read_access_num +=1;
		 allpower->gpp_data->cache_para->total_dcache_read_access_num +=1;
	 }
	 else if(cmd==Write){
		 thispower->gpp_data->cache_para->total_dcache_write_access_num +=1;
		 allpower->gpp_data->cache_para->total_dcache_write_access_num +=1;
	 }
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* if LRU replacement and this is not the first element of list, reorder */
  if (blk->way_prev && cp->policy == LRU)
    {
      /* move this block to head of the way (MRU) list */
      update_way_list(&cp->sets[set], blk, Head);
    }

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));

 cache_fast_hit: /* fast hit handler */

  /* **FAST HIT** */
// fputs("dl1_cache_hit:1 ",Simulation::visualFile);
 	 esynetrcd->l1dcache_hit += 1;
 	 esynetrcd->valid = 1;
  	 cp->hits++;
	 if(cmd == Read){
		 thispower->gpp_data->cache_para->total_dcache_read_access_num +=1;
		 allpower->gpp_data->cache_para->total_dcache_read_access_num +=1;
	 }
	 else if(cmd==Write){
		 thispower->gpp_data->cache_para->total_dcache_write_access_num +=1;
		 allpower->gpp_data->cache_para->total_dcache_write_access_num +=1;
	 }
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* this block hit last, no change in the way list */

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));
}

unsigned int TMIPS::cache_access_dl2(struct cache_t *cp,	/* cache to access */
	     enum mem_cmd cmd,		/* access type, Read or Write */
	     md_addr_t addr,		/* address of access */
	     void *vp,			/* ptr to buffer for input/output */
	     int nbytes,		/* number of bytes to access */
	     tick_t now,		/* time of access */
	     byte_t **udata,		/* for return of user data ptr */
	     md_addr_t *repl_addr)	/* for address of replaced block */
{
//	if(!strcmp(cp->name,"il2")){
//			return cache_access_il2(cp, cmd, addr, vp, nbytes, now, udata, repl_addr);
//		}
  byte_t *p = (byte_t *)vp;
  md_addr_t tag = CACHE_TAG(cp, addr);
  md_addr_t set = CACHE_SET(cp, addr);
  md_addr_t bofs = CACHE_BLK(cp, addr);
  struct cache_blk_t *blk, *repl;
  int lat = 0;

  /* default replacement address */
  if (repl_addr)
    *repl_addr = 0;
  thispower->gpp_data->cache_para->l2cache_cycle_duty +=1;
  /* check alignments */
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    fatal("cache: access error: bad size or alignment, addr 0x%08x", addr);

  /* access must fit in cache block */
  /* FIXME:
     ((addr + (nbytes - 1)) > ((addr & ~cp->blk_mask) + (cp->bsize - 1))) */
  if ((addr + nbytes) > ((addr & ~cp->blk_mask) + cp->bsize))
    fatal("cache: access error: access spans block, addr 0x%08x", addr);

  /* permissions are checked on cache misses */

  /* check for a fast hit: access to same block */
  if (CACHE_TAGSET(cp, addr) == cp->last_tagset)
    {
      /* hit in the same block */
      blk = cp->last_blk;
      goto cache_fast_hit;
    }

  if (cp->hsize)
    {
      /* higly-associativity cache, access through the per-set hash tables */
      int hindex = CACHE_HASH(cp, tag);

      for (blk=cp->sets[set].hash[hindex];
	   blk;
	   blk=blk->hash_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }
  else
    {
      /* low-associativity cache, linear search the way list */
      for (blk=cp->sets[set].way_head;
	   blk;
	   blk=blk->way_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    goto cache_hit;
	}
    }

  /* cache block not found */

  /* **MISS** */
  cp->misses++;
//  fputs("dl2_cache_miss:0 ",Simulation::visualFile);
  esynetrcd->l2dcache_miss += 1;
  esynetrcd->valid = 1;
  if(cmd == Write){
	  thispower->gpp_data->cache_para->total_dcache2_write_miss_num +=1;
	  allpower->gpp_data->cache_para->total_dcache2_write_miss_num +=1;
  }
  else if(cmd == Read){
	  thispower->gpp_data->cache_para->total_dcache2_read_miss_num +=1;
	  allpower->gpp_data->cache_para->total_dcache2_read_miss_num +=1;
  }
  /* select the appropriate block to replace, and re-link this entry to
     the appropriate place in the way list */
  switch (cp->policy) {
  case LRU:
  case FIFO:
    repl = cp->sets[set].way_tail;
    update_way_list(&cp->sets[set], repl, Head);
    break;
  case Random:
    {
      int bindex = myrand() & (cp->assoc - 1);
      repl = CACHE_BINDEX(cp, cp->sets[set].blks, bindex);
    }
    break;
  default:
    panic("bogus replacement policy");
  }

  /* remove this block from the hash bucket chain, if hash exists */
  if (cp->hsize)
    unlink_htab_ent(cp, &cp->sets[set], repl);

  /* blow away the last block to hit */
  cp->last_tagset = 0;
  cp->last_blk = NULL;

  /* write back replaced block data */
  if (repl->status & CACHE_BLK_VALID)
    {
      cp->replacements++;

      if (repl_addr)
	*repl_addr = CACHE_MK_BADDR(cp, repl->tag, set);

      /* don't replace the block until outstanding misses are satisfied */
      lat += BOUND_POS(repl->ready - now);

      /* stall until the bus to next level of memory is available */
      lat += BOUND_POS(cp->bus_free - (now + lat));

      /* track bus resource usage */
      cp->bus_free = MAX(cp->bus_free, (now + lat)) + 1;

      if (repl->status & CACHE_BLK_DIRTY)
	{
	  /* write back the cache block */
	  cp->writebacks++;
	  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Write,
				   CACHE_MK_BADDR(cp, repl->tag, set),
				   cp->bsize, repl, now+lat);
	}
    }

  /* update block tags */
  repl->tag = tag;
  repl->status = CACHE_BLK_VALID;	/* dirty bit set on update */

  /* read data block */
  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Read, CACHE_BADDR(cp, addr), cp->bsize,
			   repl, now+lat);

  /* copy data out of cache block */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, repl, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    repl->status |= CACHE_BLK_DIRTY;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = repl->user_data;

  /* update block status */
  repl->ready = now+lat;

  /* link this entry back into the hash table */
  if (cp->hsize)
    link_htab_ent(cp, &cp->sets[set], repl);

  /* return latency of the operation */
  return lat;


 cache_hit: /* slow hit handler */
  /* **HIT** */
 esynetrcd->l2dcache_hit += 1;
 esynetrcd->valid = 1;
  cp->hits++;
  if(cmd == Write){
	  thispower->gpp_data->cache_para->total_dcache2_write_access_num +=1;
	  allpower->gpp_data->cache_para->total_dcache2_write_access_num +=1;
  }
  else if(cmd == Read){
	  thispower->gpp_data->cache_para->total_dcache2_read_access_num +=1;
	  allpower->gpp_data->cache_para->total_dcache2_read_access_num +=1;
  }
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* if LRU replacement and this is not the first element of list, reorder */
  if (blk->way_prev && cp->policy == LRU)
    {
      /* move this block to head of the way (MRU) list */
      update_way_list(&cp->sets[set], blk, Head);
    }

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));

 cache_fast_hit: /* fast hit handler */

  /* **FAST HIT** */
// fputs("dl2_cache_miss:1 ",Simulation::visualFile);
 esynetrcd->l2dcache_miss += 1;
 esynetrcd->valid = 1;
  cp->hits++;
	if(cmd == Write){
	  thispower->gpp_data->cache_para->total_dcache2_write_access_num +=1;
	  allpower->gpp_data->cache_para->total_dcache2_write_access_num +=1;
	}
	else if(cmd == Read){
	  thispower->gpp_data->cache_para->total_dcache2_read_access_num +=1;
	  allpower->gpp_data->cache_para->total_dcache2_read_access_num +=1;
	}
  /* copy data out of cache block, if block exists */
  if (cp->balloc)
    {
      CACHE_BCOPY(cmd, blk, bofs, p, nbytes);
    }

  /* update dirty status */
  if (cmd == Write)
    blk->status |= CACHE_BLK_DIRTY;

  /* this block hit last, no change in the way list */

  /* tag is unchanged, so hash links (if they exist) are still valid */

  /* get user block data, if requested and it exists */
  if (udata)
    *udata = blk->user_data;

  /* record the last block to hit */
  cp->last_tagset = CACHE_TAGSET(cp, addr);
  cp->last_blk = blk;

  /* return first cycle data is available to access */
  return (int) MAX(cp->hit_latency, (blk->ready - now));
}

int TMIPS::cache_probe(struct cache_t *cp,		/* cache instance to probe */
	    md_addr_t addr)		/* address of block to probe */
{
  md_addr_t tag = CACHE_TAG(cp, addr);
  md_addr_t set = CACHE_SET(cp, addr);
  struct cache_blk_t *blk;

  /* permissions are checked on cache misses */

  if (cp->hsize)
  {
    /* higly-associativity cache, access through the per-set hash tables */
    int hindex = CACHE_HASH(cp, tag);

    for (blk=cp->sets[set].hash[hindex];
	 blk;
	 blk=blk->hash_next)
    {
      if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	  return TRUE;
    }
  }
  else
  {
    /* low-associativity cache, linear search the way list */
    for (blk=cp->sets[set].way_head;
	 blk;
	 blk=blk->way_next)
    {
      if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	  return TRUE;
    }
  }

  /* cache block not found */
  return FALSE;
}

/* flush the entire cache, returns latency of the operation */
unsigned int TMIPS::cache_flush(struct cache_t *cp,		/* cache instance to flush */
	    tick_t now)			/* time of cache flush */
{
  int i, lat = cp->hit_latency; /* min latency to probe cache */
  struct cache_blk_t *blk;

  /* blow away the last block to hit */
  cp->last_tagset = 0;
  cp->last_blk = NULL;

  /* no way list updates required because all blocks are being invalidated */
  for (i=0; i<cp->nsets; i++)
    {
      for (blk=cp->sets[i].way_head; blk; blk=blk->way_next)
	{
	  if (blk->status & CACHE_BLK_VALID)
	    {
	      cp->invalidations++;
	      blk->status &= ~CACHE_BLK_VALID;

	      if (blk->status & CACHE_BLK_DIRTY)
		{
		  /* write back the invalidated block */
          	  cp->writebacks++;
		  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Write,
					   CACHE_MK_BADDR(cp, blk->tag, i),
					   cp->bsize, blk, now+lat);
		}
	    }
	}
    }

  /* return latency of the flush operation */
  return lat;
}

unsigned int TMIPS::cache_flush_addr(struct cache_t *cp,	/* cache instance to flush */
		 md_addr_t addr,	/* address of block to flush */
		 tick_t now)		/* time of cache flush */
{
  md_addr_t tag = CACHE_TAG(cp, addr);
  md_addr_t set = CACHE_SET(cp, addr);
  struct cache_blk_t *blk;
  int lat = cp->hit_latency; /* min latency to probe cache */

  if (cp->hsize)
    {
      /* higly-associativity cache, access through the per-set hash tables */
      int hindex = CACHE_HASH(cp, tag);

      for (blk=cp->sets[set].hash[hindex];
	   blk;
	   blk=blk->hash_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    break;
	}
    }
  else
    {
      /* low-associativity cache, linear search the way list */
      for (blk=cp->sets[set].way_head;
	   blk;
	   blk=blk->way_next)
	{
	  if (blk->tag == tag && (blk->status & CACHE_BLK_VALID))
	    break;
	}
    }

  if (blk)
    {
      cp->invalidations++;
      blk->status &= ~CACHE_BLK_VALID;

      /* blow away the last block to hit */
      cp->last_tagset = 0;
      cp->last_blk = NULL;

      if (blk->status & CACHE_BLK_DIRTY)
	{
	  /* write back the invalidated block */
          cp->writebacks++;
	  lat += (this->*cp->TMIPS::cache_t::blk_access_fn)(Write,
				   CACHE_MK_BADDR(cp, blk->tag, set),
				   cp->bsize, blk, now+lat);
	}
      /* move this block to tail of the way (LRU) list */
      update_way_list(&cp->sets[set], blk, Tail);
    }

  /* return latency of the operation */
  return lat;
}
void TMIPS::unlink_htab_ent(struct cache_t *cp,		/* cache to update */
		struct cache_set_t *set,	/* set containing bkt chain */
		struct cache_blk_t *blk)	/* block to unlink */
{
  struct cache_blk_t *prev, *ent;
  int index = CACHE_HASH(cp, blk->tag);

  /* locate the block in the hash table bucket chain */
  for (prev=NULL,ent=set->hash[index];
       ent;
       prev=ent,ent=ent->hash_next)
    {
      if (ent == blk)
	break;
    }
  assert(ent);

  /* unlink the block from the hash table bucket chain */
  if (!prev)
    {
      /* head of hash bucket list */
      set->hash[index] = ent->hash_next;
    }
  else
    {
      /* middle or end of hash bucket list */
      prev->hash_next = ent->hash_next;
    }
  ent->hash_next = NULL;
}

/* insert BLK onto the head of the hash table bucket chain in SET */
void TMIPS::link_htab_ent(struct cache_t *cp,		/* cache to update */
	      struct cache_set_t *set,		/* set containing bkt chain */
	      struct cache_blk_t *blk)		/* block to insert */
{
  int index = CACHE_HASH(cp, blk->tag);

  /* insert block onto the head of the bucket chain */
  blk->hash_next = set->hash[index];
  set->hash[index] = blk;
}

/* where to insert a block onto the ordered way chain */


/* insert BLK into the order way chain in SET at location WHERE */
void TMIPS::update_way_list(struct cache_set_t *set,	/* set contained way chain */
		struct cache_blk_t *blk,	/* block to insert */
		enum list_loc_t where)		/* insert location */
{
  /* unlink entry from the way list */
  if (!blk->way_prev && !blk->way_next)
    {
      /* only one entry in list (direct-mapped), no action */
      assert(set->way_head == blk && set->way_tail == blk);
      /* Head/Tail order already */
      return;
    }
  /* else, more than one element in the list */
  else if (!blk->way_prev)
    {
      assert(set->way_head == blk && set->way_tail != blk);
      if (where == Head)
	{
	  /* already there */
	  return;
	}
      /* else, move to tail */
      set->way_head = blk->way_next;
      blk->way_next->way_prev = NULL;
    }
  else if (!blk->way_next)
    {
      /* end of list (and not front of list) */
      assert(set->way_head != blk && set->way_tail == blk);
      if (where == Tail)
	{
	  /* already there */
	  return;
	}
      set->way_tail = blk->way_prev;
      blk->way_prev->way_next = NULL;
    }
  else
    {
      /* middle of list (and not front or end of list) */
      assert(set->way_head != blk && set->way_tail != blk);
      blk->way_prev->way_next = blk->way_next;
      blk->way_next->way_prev = blk->way_prev;
    }

  /* link BLK back into the list */
  if (where == Head)
    {
      /* link to the head of the way list */
      blk->way_next = set->way_head;
      blk->way_prev = NULL;
      set->way_head->way_prev = blk;
      set->way_head = blk;
    }
  else if (where == Tail)
    {
      /* link to the tail of the way list */
      blk->way_prev = set->way_tail;
      blk->way_next = NULL;
      set->way_tail->way_next = blk;
      set->way_tail = blk;
    }
  else
    panic("bogus WHERE designator");
}




/*end of cache.cc*/
/*bpred.cc*/
struct TMIPS::bpred_t *	TMIPS::bpred_create(enum bpred_class classt,	/* type of predictor to create */
	     unsigned int bimod_size,	/* bimod table size */
	     unsigned int l1size,	/* 2lev l1 table size */
	     unsigned int l2size,	/* 2lev l2 table size */
	     unsigned int meta_size,	/* meta table size */
	     unsigned int shift_width,	/* history register width */
	     unsigned int xort,  	/* history xor address flag */
	     unsigned int btb_sets,	/* number of sets in BTB */
	     unsigned int btb_assoc,	/* BTB associativity */
	     unsigned int retstack_size) /* num entries in ret-addr stack */
{
  struct bpred_t *pred;

  if (!(pred = (struct bpred_t *)calloc(1, sizeof(struct bpred_t))))
    fatal("out of virtual memory");

  pred->classt = classt;

  switch (classt) {
  case BPredComb:
    /* bimodal component */
    pred->dirpred.bimod =
      bpred_dir_create(BPred2bit, bimod_size, 0, 0, 0);

    /* 2-level component */
    pred->dirpred.twolev =
      bpred_dir_create(BPred2Level, l1size, l2size, shift_width, xort);

    /* metapredictor component */
    pred->dirpred.meta =
      bpred_dir_create(BPred2bit, meta_size, 0, 0, 0);

    break;

  case BPred2Level:
    pred->dirpred.twolev =
      bpred_dir_create(classt, l1size, l2size, shift_width, xort);

    break;

  case BPred2bit:
    pred->dirpred.bimod =
      bpred_dir_create(classt, bimod_size, 0, 0, 0);break;

  case BPredTaken:
  case BPredNotTaken:
    /* no other state */
    break;

  default:
    panic("bogus predictor class");
  }

  /* allocate ret-addr stack */
  switch (classt) {
  case BPredComb:
  case BPred2Level:
  case BPred2bit:
    {
      int i;

      /* allocate BTB */
      if (!btb_sets || (btb_sets & (btb_sets-1)) != 0)
	fatal("number of BTB sets must be non-zero and a power of two");
      if (!btb_assoc || (btb_assoc & (btb_assoc-1)) != 0)
	fatal("BTB associativity must be non-zero and a power of two");

      if (!(pred->btb.btb_data = (struct bpred_btb_ent_t *)calloc(btb_sets * btb_assoc,
					sizeof(struct bpred_btb_ent_t))))
	fatal("cannot allocate BTB");

      pred->btb.sets = btb_sets;
      pred->btb.assoc = btb_assoc;

      if (pred->btb.assoc > 1)
	for (i=0; i < (pred->btb.assoc*pred->btb.sets); i++)
	  {
	    if (i % pred->btb.assoc != pred->btb.assoc - 1)
	      pred->btb.btb_data[i].next = &pred->btb.btb_data[i+1];
	    else
	      pred->btb.btb_data[i].next = NULL;

	    if (i % pred->btb.assoc != pred->btb.assoc - 1)
	      pred->btb.btb_data[i+1].prev = &pred->btb.btb_data[i];
	  }

      /* allocate retstack */
      if ((retstack_size & (retstack_size-1)) != 0)
	fatal("Return-address-stack size must be zero or a power of two");

      pred->retstack.size = retstack_size;
      if (retstack_size)
	if (!(pred->retstack.stack = (struct bpred_btb_ent_t *)calloc(retstack_size,
					    sizeof(struct bpred_btb_ent_t))))
	  fatal("cannot allocate return-address-stack");
      pred->retstack.tos = retstack_size - 1;

      break;
    }

  case BPredTaken:
  case BPredNotTaken:
    /* no other state */
    break;

  default:
    panic("bogus predictor class");
  }

  return pred;
}

/* create a branch direction predictor */
struct TMIPS::bpred_dir_t *	TMIPS::bpred_dir_create (
  enum bpred_class classt,	/* type of predictor to create */
  unsigned int l1size,	 	/* level-1 table size */
  unsigned int l2size,	 	/* level-2 table size (if relevant) */
  unsigned int shift_width,	/* history register width */
  unsigned int xort)	    	/* history xor address flag */
{
  struct bpred_dir_t *pred_dir;
  unsigned int cnt;
  int flipflop;

  if (!(pred_dir = (struct bpred_dir_t *)calloc(1, sizeof(struct bpred_dir_t))))
    fatal("out of virtual memory");

  pred_dir->classt = classt;

  cnt = -1;
  switch (classt) {
  case BPred2Level:
    {
      if (!l1size || (l1size & (l1size-1)) != 0)
	fatal("level-1 size, `%d', must be non-zero and a power of two",
	      l1size);
      pred_dir->config.two.l1size = l1size;

      if (!l2size || (l2size & (l2size-1)) != 0)
	fatal("level-2 size, `%d', must be non-zero and a power of two",
	      l2size);
      pred_dir->config.two.l2size = l2size;

      if (!shift_width || shift_width > 30)
	fatal("shift register width, `%d', must be non-zero and positive",
	      shift_width);
      pred_dir->config.two.shift_width = shift_width;

      pred_dir->config.two.xort = xort;
      pred_dir->config.two.shiftregs = (int *)calloc(l1size, sizeof(int));
      if (!pred_dir->config.two.shiftregs)
	fatal("cannot allocate shift register table");

      pred_dir->config.two.l2table = (unsigned char *)calloc(l2size, sizeof(unsigned char));
      if (!pred_dir->config.two.l2table)
	fatal("cannot allocate second level table");

      /* initialize counters to weakly this-or-that */
      flipflop = 1;
      for (cnt = 0; cnt < l2size; cnt++)
	{
	  pred_dir->config.two.l2table[cnt] = flipflop;
	  flipflop = 3 - flipflop;
	}

      break;
    }

  case BPred2bit:
    if (!l1size || (l1size & (l1size-1)) != 0)
      fatal("2bit table size, `%d', must be non-zero and a power of two",
	    l1size);
    pred_dir->config.bimod.size = l1size;
    if (!(pred_dir->config.bimod.table =
	  (unsigned char *)calloc(l1size, sizeof(unsigned char))))
      fatal("cannot allocate 2bit storage");
    /* initialize counters to weakly this-or-that */
    flipflop = 1;
    for (cnt = 0; cnt < l1size; cnt++)
      {
	pred_dir->config.bimod.table[cnt] = flipflop;
	flipflop = 3 - flipflop;
      }

    break;

  case BPredTaken:
  case BPredNotTaken:
    /* no other state */
    break;

  default:
    panic("bogus branch direction predictor class");
  }

  return pred_dir;
}
void TMIPS::bpred_dir_config(
  struct bpred_dir_t *pred_dir,	/* branch direction predictor instance */
  char name[],			/* predictor name */
  FILE *stream)			/* output stream */
{
  switch (pred_dir->classt) {
  case BPred2Level:
    fprintf(stream,
      "pred_dir: %s: 2-lvl: %d l1-sz, %d bits/ent, %s xort, %d l2-sz, direct-mapped\n",
      name, pred_dir->config.two.l1size, pred_dir->config.two.shift_width,
      pred_dir->config.two.xort ? "" : "no", pred_dir->config.two.l2size);
    break;

  case BPred2bit:
    fprintf(stream, "pred_dir: %s: 2-bit: %d entries, direct-mapped\n",
      name, pred_dir->config.bimod.size);
    break;

  case BPredTaken:
    fprintf(stream, "pred_dir: %s: predict taken\n", name);
    break;

  case BPredNotTaken:
    fprintf(stream, "pred_dir: %s: predict not taken\n", name);
    break;

  default:
    panic("bogus branch direction predictor class");
  }
}

/* print branch predictor configuration */
void TMIPS::bpred_config(struct bpred_t *pred,	/* branch predictor instance */
	     FILE *stream)		/* output stream */
{
  switch (pred->classt) {
  case BPredComb:
    bpred_dir_config (pred->dirpred.bimod, "bimod", stream);
    bpred_dir_config (pred->dirpred.twolev, "2lev", stream);
    bpred_dir_config (pred->dirpred.meta, "meta", stream);
    fprintf(stream, "btb: %d sets x %d associativity",
	    pred->btb.sets, pred->btb.assoc);
    fprintf(stream, "ret_stack: %d entries", pred->retstack.size);
    break;

  case BPred2Level:
    bpred_dir_config (pred->dirpred.twolev, "2lev", stream);
    fprintf(stream, "btb: %d sets x %d associativity",
	    pred->btb.sets, pred->btb.assoc);
    fprintf(stream, "ret_stack: %d entries", pred->retstack.size);
    break;

  case BPred2bit:
    bpred_dir_config (pred->dirpred.bimod, "bimod", stream);
    fprintf(stream, "btb: %d sets x %d associativity",
	    pred->btb.sets, pred->btb.assoc);
    fprintf(stream, "ret_stack: %d entries", pred->retstack.size);
    break;

  case BPredTaken:
    bpred_dir_config (pred->dirpred.bimod, "taken", stream);
    break;
  case BPredNotTaken:
    bpred_dir_config (pred->dirpred.bimod, "nottaken", stream);
    break;

  default:
    panic("bogus branch predictor class");
  }
}

/* print predictor stats */
void TMIPS::bpred_stats(struct bpred_t *pred,	/* branch predictor instance */
	    FILE *stream)		/* output stream */
{
  fprintf(stream, "pred: addr-prediction rate = %f\n",
	  (double)pred->addr_hits/(double)(pred->addr_hits+pred->misses));
  fprintf(stream, "pred: dir-prediction rate = %f\n",
	  (double)pred->dir_hits/(double)(pred->dir_hits+pred->misses));
}

/* register branch predictor stats */
void TMIPS::bpred_reg_stats(struct bpred_t *pred,	/* branch predictor instance */
		Statistics & ss)	/* stats database */
{
  char buf[512], buf1[512], *name;

  /* get a name for this predictor */
  switch (pred->classt)
    {
    case BPredComb:
      name = "bpred_comb";
      break;
    case BPred2Level:
      name = "bpred_2lev";
      break;
    case BPred2bit:
      name = "bpred_bimod";
      break;
    case BPredTaken:
      name = "bpred_taken";
      break;
    case BPredNotTaken:
      name = "bpred_nottaken";
      break;
    default:
      panic("bogus branch predictor class");
    }

  sprintf(buf, "%s.lookups", name);
  ss.stat_reg_counter(buf, "total number of bpred lookups",
		   &pred->lookups, 0, NULL);
  sprintf(buf, "%s.updates", name);
  sprintf(buf1, "%s.dir_hits + %s.misses", name, name);
  ss.stat_reg_formula(buf, "total number of updates", buf1, "%12.0f");
  sprintf(buf, "%s.addr_hits", name);
  ss.stat_reg_counter(buf, "total number of address-predicted hits",
		   &pred->addr_hits, 0, NULL);
  sprintf(buf, "%s.dir_hits", name);
  ss.stat_reg_counter(buf,
		   "total number of direction-predicted hits "
		   "(includes addr-hits)",
		   &pred->dir_hits, 0, NULL);
  if (pred->classt == BPredComb)
    {
      sprintf(buf, "%s.used_bimod", name);
      ss.stat_reg_counter(buf,
		       "total number of bimodal predictions used",
		       &pred->used_bimod, 0, NULL);
      sprintf(buf, "%s.used_2lev", name);
      ss.stat_reg_counter(buf,
		       "total number of 2-level predictions used",
		       &pred->used_2lev, 0, NULL);
    }
  sprintf(buf, "%s.misses", name);
  ss.stat_reg_counter(buf, "total number of misses", &pred->misses, 0, NULL);
  sprintf(buf, "%s.jr_hits", name);
  ss.stat_reg_counter(buf,
		   "total number of address-predicted hits for JR's",
		   &pred->jr_hits, 0, NULL);
  sprintf(buf, "%s.jr_seen", name);
  ss.stat_reg_counter(buf,
		   "total number of JR's seen",
		   &pred->jr_seen, 0, NULL);
  sprintf(buf, "%s.jr_non_ras_hits.PP", name);
  ss.stat_reg_counter(buf,
		   "total number of address-predicted hits for non-RAS JR's",
		   &pred->jr_non_ras_hits, 0, NULL);
  sprintf(buf, "%s.jr_non_ras_seen.PP", name);
  ss.stat_reg_counter(buf,
		   "total number of non-RAS JR's seen",
		   &pred->jr_non_ras_seen, 0, NULL);
  sprintf(buf, "%s.bpred_addr_rate", name);
  sprintf(buf1, "%s.addr_hits / %s.updates", name, name);
  ss.stat_reg_formula(buf,
		   "branch address-prediction rate (i.e., addr-hits/updates)",
		   buf1, "%9.4f");
  sprintf(buf, "%s.bpred_dir_rate", name);
  sprintf(buf1, "%s.dir_hits / %s.updates", name, name);
  ss.stat_reg_formula(buf,
		  "branch direction-prediction rate (i.e., all-hits/updates)",
		  buf1, "%9.4f");
  sprintf(buf, "%s.bpred_jr_rate", name);
  sprintf(buf1, "%s.jr_hits / %s.jr_seen", name, name);
  ss.stat_reg_formula(buf,
		  "JR address-prediction rate (i.e., JR addr-hits/JRs seen)",
		  buf1, "%9.4f");
  sprintf(buf, "%s.bpred_jr_non_ras_rate.PP", name);
  sprintf(buf1, "%s.jr_non_ras_hits.PP / %s.jr_non_ras_seen.PP", name, name);
  ss.stat_reg_formula(buf,
		   "non-RAS JR addr-pred rate (ie, non-RAS JR hits/JRs seen)",
		   buf1, "%9.4f");
  sprintf(buf, "%s.retstack_pushes", name);
  ss.stat_reg_counter(buf,
		   "total number of address pushed onto ret-addr stack",
		   &pred->retstack_pushes, 0, NULL);
  sprintf(buf, "%s.retstack_pops", name);
  ss.stat_reg_counter(buf,
		   "total number of address popped off of ret-addr stack",
		   &pred->retstack_pops, 0, NULL);
  sprintf(buf, "%s.used_ras.PP", name);
  ss.stat_reg_counter(buf,
		   "total number of RAS predictions used",
		   &pred->used_ras, 0, NULL);
  sprintf(buf, "%s.ras_hits.PP", name);
  ss.stat_reg_counter(buf,
		   "total number of RAS hits",
		   &pred->ras_hits, 0, NULL);
  sprintf(buf, "%s.ras_rate.PP", name);
  sprintf(buf1, "%s.ras_hits.PP / %s.used_ras.PP", name, name);
  ss.stat_reg_formula(buf,
		   "RAS prediction rate (i.e., RAS hits/used RAS)",
		   buf1, "%9.4f");
}
void TMIPS::bpred_after_priming(struct bpred_t *bpred)
{
  if (bpred == NULL)
    return;

  bpred->lookups = 0;
  bpred->addr_hits = 0;
  bpred->dir_hits = 0;
  bpred->used_ras = 0;
  bpred->used_bimod = 0;
  bpred->used_2lev = 0;
  bpred->jr_hits = 0;
  bpred->jr_seen = 0;
  bpred->misses = 0;
  bpred->retstack_pops = 0;
  bpred->retstack_pushes = 0;
  bpred->ras_hits = 0;
}
#define BIMOD_HASH(PRED, ADDR)						\
  ((((ADDR) >> 19) ^ ((ADDR) >> MD_BR_SHIFT)) & ((PRED)->config.bimod.size-1))
char * TMIPS::bpred_dir_lookup(struct bpred_dir_t *pred_dir,	/* branch dir predictor inst */
		 md_addr_t baddr)		/* branch address */
{
  unsigned char *p = NULL;

  /* Except for jumps, get a pointer to direction-prediction bits */
  switch (pred_dir->classt) {
    case BPred2Level:
      {
	int l1index, l2index;

        /* traverse 2-level tables */
        l1index = (baddr >> MD_BR_SHIFT) & (pred_dir->config.two.l1size - 1);
        l2index = pred_dir->config.two.shiftregs[l1index];
        if (pred_dir->config.two.xort)
	  {
#if 1
	    /* this L2 index computation is more "compatible" to McFarling's
	       verison of it, i.e., if the PC xor address component is only
	       part of the index, take the lower order address bits for the
	       other part of the index, rather than the higher order ones */
	    l2index = (((l2index ^ (baddr >> MD_BR_SHIFT))
			& ((1 << pred_dir->config.two.shift_width) - 1))
		       | ((baddr >> MD_BR_SHIFT)
			  << pred_dir->config.two.shift_width));
#else
	    l2index = l2index ^ (baddr >> MD_BR_SHIFT);
#endif
	  }
	else
	  {
	    l2index =
	      l2index
		| ((baddr >> MD_BR_SHIFT) << pred_dir->config.two.shift_width);
	  }
        l2index = l2index & (pred_dir->config.two.l2size - 1);

        /* get a pointer to prediction state information */
        p = &pred_dir->config.two.l2table[l2index];
      }
      break;
    case BPred2bit:
      p = &pred_dir->config.bimod.table[BIMOD_HASH(pred_dir, baddr)];
      break;
    case BPredTaken:
    case BPredNotTaken:
      break;
    default:
      panic("bogus branch direction predictor class");
    }

  return (char *)p;
}

TMIPS::md_addr_t TMIPS::bpred_lookup(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* branch target if taken */
	     enum md_opcode op,		/* opcode of instruction */
	     int is_call,		/* non-zero if inst is fn call */
	     int is_return,		/* non-zero if inst is fn return */
	     struct bpred_update_t *dir_update_ptr, /* pred state pointer */
	     int *stack_recover_idx)	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
{
  struct bpred_btb_ent_t *pbtb = NULL;
  int index, i;

  if (!dir_update_ptr)
    panic("no bpred update record");

  /* if this is not a branch, return not-taken */
  if (!(MD_OP_FLAGS(op) & F_CTRL))
    return 0;

  pred->lookups++;

  dir_update_ptr->dir.ras = FALSE;
  dir_update_ptr->pdir1 = NULL;
  dir_update_ptr->pdir2 = NULL;
  dir_update_ptr->pmeta = NULL;
  /* Except for jumps, get a pointer to direction-prediction bits */
  switch (pred->classt) {
    case BPredComb:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  char *bimod, *twolev, *meta;
	  bimod = bpred_dir_lookup (pred->dirpred.bimod, baddr);
	  twolev = bpred_dir_lookup (pred->dirpred.twolev, baddr);
	  meta = bpred_dir_lookup (pred->dirpred.meta, baddr);
	  dir_update_ptr->pmeta = meta;
	  dir_update_ptr->dir.meta  = (*meta >= 2);
	  dir_update_ptr->dir.bimod = (*bimod >= 2);
	  dir_update_ptr->dir.twolev  = (*twolev >= 2);
	  if (*meta >= 2)
	    {
	      dir_update_ptr->pdir1 = twolev;
	      dir_update_ptr->pdir2 = bimod;
	    }
	  else
	    {
	      dir_update_ptr->pdir1 = bimod;
	      dir_update_ptr->pdir2 = twolev;
	    }
	}
      break;
    case BPred2Level:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  dir_update_ptr->pdir1 =
	    bpred_dir_lookup (pred->dirpred.twolev, baddr);
	}
      break;
    case BPred2bit:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  dir_update_ptr->pdir1 =
	    bpred_dir_lookup (pred->dirpred.bimod, baddr);
	}
      break;
    case BPredTaken:
      return btarget;
    case BPredNotTaken:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  return baddr + sizeof(md_inst_t);
	}
      else
	{
	  return btarget;
	}
    default:
      panic("bogus predictor class");
  }

  /*
   * We have a stateful predictor, and have gotten a pointer into the
   * direction predictor (except for jumps, for which the ptr is null)
   */

  /* record pre-pop TOS; if this branch is executed speculatively
   * and is squashed, we'll restore the TOS and hope the data
   * wasn't corrupted in the meantime. */
  if (pred->retstack.size)
    *stack_recover_idx = pred->retstack.tos;
  else
    *stack_recover_idx = 0;

  /* if this is a return, pop return-address stack */
  if (is_return && pred->retstack.size)
    {
      md_addr_t target = pred->retstack.stack[pred->retstack.tos].target;
      pred->retstack.tos = (pred->retstack.tos + pred->retstack.size - 1)
	                   % pred->retstack.size;
      pred->retstack_pops++;
      dir_update_ptr->dir.ras = TRUE; /* using RAS here */
      return target;
    }

#ifndef RAS_BUG_COMPATIBLE
  /* if function call, push return-address onto return-address stack */
  if (is_call && pred->retstack.size)
    {
      pred->retstack.tos = (pred->retstack.tos + 1)% pred->retstack.size;
      pred->retstack.stack[pred->retstack.tos].target =
	baddr + sizeof(md_inst_t);
      pred->retstack_pushes++;
    }
#endif /* !RAS_BUG_COMPATIBLE */

  /* not a return. Get a pointer into the BTB */
  index = (baddr >> MD_BR_SHIFT) & (pred->btb.sets - 1);

  if (pred->btb.assoc > 1)
    {
      index *= pred->btb.assoc;

      /* Now we know the set; look for a PC match */
      for (i = index; i < (index+pred->btb.assoc) ; i++)
	if (pred->btb.btb_data[i].addr == baddr)
	  {
	    /* match */
	    pbtb = &pred->btb.btb_data[i];
	    break;
	  }
    }
  else
    {
      pbtb = &pred->btb.btb_data[index];
      if (pbtb->addr != baddr)
	pbtb = NULL;
    }

  /*
   * We now also have a pointer into the BTB for a hit, or NULL otherwise
   */

  /* if this is a jump, ignore predicted direction; we know it's taken. */
  if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) == (F_CTRL|F_UNCOND))
    {
      return (pbtb ? pbtb->target : 1);
    }

  /* otherwise we have a conditional branch */
  if (pbtb == NULL)
    {
      /* BTB miss -- just return a predicted direction */
      return ((*(dir_update_ptr->pdir1) >= 2)
	      ? /* taken */ 1
	      : /* not taken */ 0);
    }
  else
    {
      /* BTB hit, so return target if it's a predicted-taken branch */
      return ((*(dir_update_ptr->pdir1) >= 2)
	      ? /* taken */ pbtb->target
	      : /* not taken */ 0);
    }
}
void TMIPS::bpred_recover(struct bpred_t *pred,	/* branch predictor instance */
	      md_addr_t baddr,		/* branch address */
	      int stack_recover_idx)	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
{
  if (pred == NULL)
    return;

  pred->retstack.tos = stack_recover_idx;
}

/* update the branch predictor, only useful for stateful predictors; updates
   entry for instruction type OP at address BADDR.  BTB only gets updated
   for branches which are taken.  Inst was determined to jump to
   address BTARGET and was taken if TAKEN is non-zero.  Predictor
   statistics are updated with result of prediction, indicated by CORRECT and
   PRED_TAKEN, predictor state to be updated is indicated by *DIR_UPDATE_PTR
   (may be NULL for jumps, which shouldn't modify state bits).  Note if
   bpred_update is done speculatively, branch-prediction may get polluted. */
void TMIPS::bpred_update(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* resolved branch target */
	     int taken,			/* non-zero if branch was taken */
	     int pred_taken,		/* non-zero if branch was pred taken */
	     int correct,		/* was earlier addr prediction ok? */
	     enum md_opcode op,		/* opcode of instruction */
	     struct bpred_update_t *dir_update_ptr)/* pred state pointer */
{
  struct bpred_btb_ent_t *pbtb = NULL;
  struct bpred_btb_ent_t *lruhead = NULL, *lruitem = NULL;
  int index, i;

  /* don't change bpred state for non-branch instructions or if this
   * is a stateless predictor*/
  if (!(MD_OP_FLAGS(op) & F_CTRL))
    return;

  /* Have a branch here */

  if (correct)
    pred->addr_hits++;

  if (!!pred_taken == !!taken)
    pred->dir_hits++;
  else
    pred->misses++;

  if (dir_update_ptr->dir.ras)
    {
      pred->used_ras++;
      if (correct)
	pred->ras_hits++;
    }
  else if ((MD_OP_FLAGS(op) & (F_CTRL|F_COND)) == (F_CTRL|F_COND))
    {
      if (dir_update_ptr->dir.meta)
	pred->used_2lev++;
      else
	pred->used_bimod++;
    }

  /* keep stats about JR's; also, but don't change any bpred state for JR's
   * which are returns unless there's no retstack */
  if (MD_IS_INDIR(op))
    {
      pred->jr_seen++;
      if (correct)
	pred->jr_hits++;

      if (!dir_update_ptr->dir.ras)
	{
	  pred->jr_non_ras_seen++;
	  if (correct)
	    pred->jr_non_ras_hits++;
	}
      else
	{
	  /* return that used the ret-addr stack; no further work to do */
	  return;
	}
    }

  /* Can exit now if this is a stateless predictor */
  if (pred->classt == BPredNotTaken || pred->classt == BPredTaken)
    return;

  /*
   * Now we know the branch didn't use the ret-addr stack, and that this
   * is a stateful predictor
   */

#ifdef RAS_BUG_COMPATIBLE
  /* if function call, push return-address onto return-address stack */
  if (MD_IS_CALL(op) && pred->retstack.size)
    {
      pred->retstack.tos = (pred->retstack.tos + 1)% pred->retstack.size;
      pred->retstack.stack[pred->retstack.tos].target =
	baddr + sizeof(md_inst_t);
      pred->retstack_pushes++;
    }
#endif /* RAS_BUG_COMPATIBLE */

  /* update L1 table if appropriate */
  /* L1 table is updated unconditionally for combining predictor too */
  if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND) &&
      (pred->classt == BPred2Level || pred->classt == BPredComb))
    {
      int l1index, shift_reg;

      /* also update appropriate L1 history register */
      l1index =
	(baddr >> MD_BR_SHIFT) & (pred->dirpred.twolev->config.two.l1size - 1);
      shift_reg =
	(pred->dirpred.twolev->config.two.shiftregs[l1index] << 1) | (!!taken);
      pred->dirpred.twolev->config.two.shiftregs[l1index] =
	shift_reg & ((1 << pred->dirpred.twolev->config.two.shift_width) - 1);
    }

  /* find BTB entry if it's a taken branch (don't allocate for non-taken) */
  if (taken)
    {
      index = (baddr >> MD_BR_SHIFT) & (pred->btb.sets - 1);

      if (pred->btb.assoc > 1)
	{
	  index *= pred->btb.assoc;

	  /* Now we know the set; look for a PC match; also identify
	   * MRU and LRU items */
	  for (i = index; i < (index+pred->btb.assoc) ; i++)
	    {
	      if (pred->btb.btb_data[i].addr == baddr)
		{
		  /* match */
		  assert(!pbtb);
		  pbtb = &pred->btb.btb_data[i];
		}

	      dassert(pred->btb.btb_data[i].prev
		      != pred->btb.btb_data[i].next);
	      if (pred->btb.btb_data[i].prev == NULL)
		{
		  /* this is the head of the lru list, ie current MRU item */
		  dassert(lruhead == NULL);
		  lruhead = &pred->btb.btb_data[i];
		}
	      if (pred->btb.btb_data[i].next == NULL)
		{
		  /* this is the tail of the lru list, ie the LRU item */
		  dassert(lruitem == NULL);
		  lruitem = &pred->btb.btb_data[i];
		}
	    }
	  dassert(lruhead && lruitem);

	  if (!pbtb)
	    /* missed in BTB; choose the LRU item in this set as the victim */
	    pbtb = lruitem;
	  /* else hit, and pbtb points to matching BTB entry */

	  /* Update LRU state: selected item, whether selected because it
	   * matched or because it was LRU and selected as a victim, becomes
	   * MRU */
	  if (pbtb != lruhead)
	    {
	      /* this splices out the matched entry... */
	      if (pbtb->prev)
		pbtb->prev->next = pbtb->next;
	      if (pbtb->next)
		pbtb->next->prev = pbtb->prev;
	      /* ...and this puts the matched entry at the head of the list */
	      pbtb->next = lruhead;
	      pbtb->prev = NULL;
	      lruhead->prev = pbtb;
	      dassert(pbtb->prev || pbtb->next);
	      dassert(pbtb->prev != pbtb->next);
	    }
	  /* else pbtb is already MRU item; do nothing */
	}
      else
	pbtb = &pred->btb.btb_data[index];
    }

  /*
   * Now 'p' is a possibly null pointer into the direction prediction table,
   * and 'pbtb' is a possibly null pointer into the BTB (either to a
   * matched-on entry or a victim which was LRU in its set)
   */

  /* update state (but not for jumps) */
  if (dir_update_ptr->pdir1)
    {
      if (taken)
	{
	  if (*dir_update_ptr->pdir1 < 3)
	    ++*dir_update_ptr->pdir1;
	}
      else
	{ /* not taken */
	  if (*dir_update_ptr->pdir1 > 0)
	    --*dir_update_ptr->pdir1;
	}
    }

  /* combining predictor also updates second predictor and meta predictor */
  /* second direction predictor */
  if (dir_update_ptr->pdir2)
    {
      if (taken)
	{
	  if (*dir_update_ptr->pdir2 < 3)
	    ++*dir_update_ptr->pdir2;
	}
      else
	{ /* not taken */
	  if (*dir_update_ptr->pdir2 > 0)
	    --*dir_update_ptr->pdir2;
	}
    }

  /* meta predictor */
  if (dir_update_ptr->pmeta)
    {
      if (dir_update_ptr->dir.bimod != dir_update_ptr->dir.twolev)
	{
	  /* we only update meta predictor if directions were different */
	  if (dir_update_ptr->dir.twolev == (unsigned int)taken)
	    {
	      /* 2-level predictor was correct */
	      if (*dir_update_ptr->pmeta < 3)
		++*dir_update_ptr->pmeta;
	    }
	  else
	    {
	      /* bimodal predictor was correct */
	      if (*dir_update_ptr->pmeta > 0)
		--*dir_update_ptr->pmeta;
	    }
	}
    }

  /* update BTB (but only for taken branches) */
  if (pbtb)
    {
      /* update current information */
      dassert(taken);

      if (pbtb->addr == baddr)
	{
	  if (!correct)
	    pbtb->target = btarget;
	}
      else
	{
	  /* enter a new branch in the table */
	  pbtb->addr = baddr;
	  pbtb->op = op;
	  pbtb->target = btarget;
	}
    }
}













/*misc.cc*/

/*machine.cc*/
char *
TMIPS::md_reg_name(enum md_reg_type rt, int reg)
{
  int i;

  for (i=0; md_reg_names[i].str != NULL; i++)
    {
      if (md_reg_names[i].file == rt && md_reg_names[i].reg == reg)
	return md_reg_names[i].str;
    }
  /* no found... */
  return NULL;
}

void TMIPS::md_print_ireg(md_gpr_t regs, int reg, FILE *stream)
{
  fprintf(stream, "%4s: %12d/0x%08x",
	  md_reg_name(rt_gpr, reg), regs[reg], regs[reg]);
}

void TMIPS::md_print_iregs(md_gpr_t regs, FILE *stream)
{
  int i;

  for (i=0; i < MD_NUM_IREGS; i += 2)
    {
      md_print_ireg(regs, i, stream);
      fprintf(stream, "  ");
      md_print_ireg(regs, i+1, stream);
      fprintf(stream, "\n");
    }
}
void TMIPS::md_print_fpreg(md_fpr_t regs, int reg, FILE *stream)
{
  fprintf(stream, "%4s: %12d/0x%08x/%f",
	  md_reg_name(rt_fpr, reg), regs.l[reg], regs.l[reg], regs.f[reg]);
  if (/* even? */!(reg & 1))
    {
      fprintf(stream, " (%4s as double: %f)",
	      md_reg_name(rt_dpr, reg/2), regs.d[reg/2]);
    }
}
void TMIPS::md_print_fpregs(md_fpr_t regs, FILE *stream)
{
  int i;

  /* floating point registers */
  for (i=0; i < MD_NUM_FREGS; i += 2)
    {
      md_print_fpreg(regs, i, stream);
      fprintf(stream, "\n");

      md_print_fpreg(regs, i+1, stream);
      fprintf(stream, "\n");
    }
}

void TMIPS::md_print_creg(md_ctrl_t regs, int reg, FILE *stream)
{
  /* index is only used to iterate over these registers, hence no enums... */
  switch (reg)
    {
    case 0:
      fprintf(stream, "HI: 0x%08x", regs.hi);
      break;

    case 1:
      fprintf(stream, "LO: 0x%08x", regs.lo);
      break;

    case 2:
      fprintf(stream, "FCC: 0x%08x", regs.fcc);
      break;

    default:
      panic("bogus control register index");
    }
}

void TMIPS::md_print_cregs(md_ctrl_t regs, FILE *stream)
{
  md_print_creg(regs, 0, stream);
  fprintf(stream, "  ");
  md_print_creg(regs, 1, stream);
  fprintf(stream, "  ");
  md_print_creg(regs, 2, stream);
  fprintf(stream, "\n");
}
word_t TMIPS::md_crc_regs(struct regs_t *regs)
{
  int i;
  word_t crc_accum = 0;

  for (i=0; i < MD_NUM_IREGS; i++)
    crc_accum = crc(crc_accum, regs->regs_R[i]);

  for (i=0; i < MD_NUM_FREGS; i++)
    crc_accum = crc(crc_accum, regs->regs_F.l[i]);

  crc_accum = crc(crc_accum, regs->regs_C.hi);
  crc_accum = crc(crc_accum, regs->regs_C.lo);
  crc_accum = crc(crc_accum, regs->regs_C.fcc);
  crc_accum = crc(crc_accum, regs->regs_PC);
  crc_accum = crc(crc_accum, regs->regs_NPC);

  return crc_accum;
}

/* xor checksum registers */
word_t TMIPS::md_xor_regs(struct regs_t *regs)
{
  int i;
  word_t checksum = 0;

  for (i=0; i < MD_NUM_IREGS; i++)
    checksum ^= regs->regs_R[i];

  for (i=0; i < MD_NUM_FREGS; i++)
    checksum ^= regs->regs_F.l[i];

  checksum ^= regs->regs_C.hi;
  checksum ^= regs->regs_C.lo;
  checksum ^= regs->regs_C.fcc;
  checksum ^= regs->regs_PC;
  checksum ^= regs->regs_NPC;

  return checksum;
}
void TMIPS::md_init_decoder(void)
{
  /* FIXME: CONNECT defined? */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
  if (md_mask2op[(MSK)]) fatal("doubly defined mask value");		\
  if ((MSK) >= MD_MAX_MASK) fatal("mask value is too large");		\
  md_mask2op[(MSK)]=(OP);

#include "pisa.def"
}

/* disassemble a SimpleScalar instruction */
void TMIPS::md_print_insn(md_inst_t inst,		/* instruction to disassemble */
	      md_addr_t pc,		/* addr of inst, used for PC-rels */
	      FILE *stream)		/* output stream */
{
  enum md_opcode op;

  /* use stderr as default output stream */
  if (!stream)
    stream = stderr;

  /* decode the instruction, assumes predecoded text segment */
  MD_SET_OPCODE(op, inst);

  /* disassemble the instruction */
  if (op == OP_NA || op >= OP_MAX)
    {
      /* bogus instruction */
      fprintf(stream, "<invalid inst: 0x%08x:%08x>", inst.a, inst.b);
    }
  else
    {
      char *s;

      fprintf(stream, "%-10s", MD_OP_NAME(op));

      s = MD_OP_FORMAT(op);
      while (*s) {
	switch (*s) {
	case 'd':
	  fprintf(stream, "r%d", RD);
	  break;
	case 's':
	  fprintf(stream, "r%d", RS);
	  break;
	case 't':
	  fprintf(stream, "r%d", RT);
	  break;
	case 'b':
	  fprintf(stream, "r%d", BS);
	  break;
	case 'D':
	  fprintf(stream, "f%d", FD);
	  break;
	case 'S':
	  fprintf(stream, "f%d", FS);
	  break;
	case 'T':
	  fprintf(stream, "f%d", FT);
	  break;
	case 'j':
	  fprintf(stream, "0x%x", (pc + 8 + (OFS << 2)));
	  break;
	case 'o':
	case 'i':
	  fprintf(stream, "%d", IMM);
	  break;
	case 'H':
	  fprintf(stream, "%d", SHAMT);
	  break;
	case 'u':
	  fprintf(stream, "%u", UIMM);
	  break;
	case 'U':
	  fprintf(stream, "0x%x", UIMM);
	  break;
	case 'J':
	  fprintf(stream, "0x%x", ((pc & 036000000000) | (TARG << 2)));
	  break;
	case 'B':
	  fprintf(stream, "0x%x", BCODE);
	  break;
#if 0 /* FIXME: obsolete... */
	case ')':
	  /* handle pre- or post-inc/dec */
	  if (SS_COMP_OP == SS_COMP_NOP)
	    fprintf(stream, ")");
	  else if (SS_COMP_OP == SS_COMP_POST_INC)
	    fprintf(stream, ")+");
	  else if (SS_COMP_OP == SS_COMP_POST_DEC)
	    fprintf(stream, ")-");
	  else if (SS_COMP_OP == SS_COMP_PRE_INC)
	    fprintf(stream, ")^+");
	  else if (SS_COMP_OP == SS_COMP_PRE_DEC)
	    fprintf(stream, ")^-");
	  else if (SS_COMP_OP == SS_COMP_POST_DBL_INC)
	    fprintf(stream, ")++");
	  else if (SS_COMP_OP == SS_COMP_POST_DBL_DEC)
	    fprintf(stream, ")--");
	  else if (SS_COMP_OP == SS_COMP_PRE_DBL_INC)
	    fprintf(stream, ")^++");
	  else if (SS_COMP_OP == SS_COMP_PRE_DBL_DEC)
	    fprintf(stream, ")^--");
	  else
	    panic("bogus SS_COMP_OP");
	  break;
#endif
	default:
	  /* anything unrecognized, e.g., '.' is just passed through */
	  fputc(*s, stream);
	}
	s++;
      }
    }
  fputc('\n', stream);
}
/*regs.cc*/
void TMIPS::regs_create(void)
{
  runcontext->regs  = (struct regs_t *)calloc(1, sizeof(struct regs_t));
  if (!runcontext->regs)
    fatal("out of virtual memory");
}

void TMIPS::regs_init(void)		/* register file to initialize */
{
  /* FIXME: assuming all entries should be zero... */
  memset(runcontext->regs, 0, sizeof(*runcontext->regs));
  /* regs->regs_R[MD_SP_INDEX] and regs->regs_PC initialized by loader... */
}

/*regs.cc end*/
/*range.cc*/
char * TMIPS::range_parse_pos(char *pstr,			/* execution position string */
		struct TCore::range_pos_t *pos)	/* position return buffer */
{
  char *s, *endp;
  struct sym_sym_t *sym;

  /* determine position type */
  if (pstr[0] == '@')
    {
      /* address position */
      pos->ptype = pt_addr;
      s = pstr + 1;
    }
  else if (pstr[0] == '#')
    {
      /* cycle count position */
      pos->ptype = pt_cycle;
      s = pstr + 1;
    }
  else
    {
      /* inst count position */
      pos->ptype = pt_inst;
      s = pstr;
    }

  /* get position value */
  errno = 0;
  pos->pos = (counter_t)strtoul(s, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      return NULL;
    }

  /* else, not an integer, attempt double conversion */
  errno = 0;
  pos->pos = (counter_t)strtod(s, &endp);
  if (!errno && !*endp)
    {
      /* good conversion */
      /* FIXME: ignoring decimal point!! */
      return NULL;
    }

  /* else, attempt symbol lookup */
  sym_loadsyms(ld_prog_fname, /* !locals */FALSE);
  sym = sym_bind_name(s, NULL, sdb_any);
  if (sym != NULL)
    {
      pos->pos = (counter_t)sym->addr;
      return NULL;
    }

  /* else, no binding made */
  return "cannot bind execution position to a value";
}
void TMIPS::range_print_pos(struct range_pos_t *pos,	/* execution position */
		FILE *stream)			/* output stream */
{
  switch (pos->ptype)
    {
    case pt_addr:
      myfprintf(stream, "@0x%08p", (md_addr_t)pos->pos);
      break;
    case pt_inst:
      fprintf(stream, "%.0f", (double)pos->pos);
      break;
    case pt_cycle:
      fprintf(stream, "#%.0f", (double)pos->pos);
      break;
    default:
      panic("bogus execution position type");
    }
}
char * TMIPS::range_parse_range(char *rstr,			/* execution range string */
		  struct range_range_t *range)	/* range return buffer */
{
  char *pos1, *pos2, *p, buf[512], *errstr;

  /* make a copy of the execution range */
  strcpy(buf, rstr);
  pos1 = buf;

  /* find mid-point */
  p = buf;
  while (*p != ':' && *p != '\0')
    {
      p++;
    }
  if (*p != ':')
    return "badly formed execution range";
  *p = '\0';

  /* this is where the second position will start */
  pos2 = p + 1;

  /* parse start position */
  if (*pos1 && *pos1 != ':')
    {
      errstr = range_parse_pos(pos1, &range->start);
      if (errstr)
	return errstr;
    }
  else
    {
      /* default start range */
      range->start.ptype = pt_inst;
      range->start.pos = 0;
    }

  /* parse end position */
  if (*pos2)
    {
      if (*pos2 == '+')
	{
	  int delta;
	  char *endp;

	  /* get delta value */
	  errno = 0;
	  delta = strtol(pos2 + 1, &endp, /* parse base */0);
	  if (!errno && !*endp)
	    {
	      /* good conversion */
	      range->end.ptype = range->start.ptype;
	      range->end.pos = range->start.pos + delta;
	    }
	  else
	    {
	      /* bad conversion */
	      return "badly formed execution range delta";
	    }
	}
      else
	{
	  errstr = range_parse_pos(pos2, &range->end);
	  if (errstr)
	    return errstr;
	}
    }
  else
    {
      /* default end range */
      range->end.ptype = range->start.ptype;
#ifdef HOST_HAS_QWORD
      range->end.pos = ULL(0x7fffffffffffffff);
#else /* !__GNUC__ */
      range->end.pos = 281474976645120.0;
#endif /* __GNUC__ */
    }

  /* no error */
  return NULL;
}
void TMIPS::range_print_range(struct range_range_t *range,	/* execution range */
		  FILE *stream)			/* output stream */
{
  range_print_pos(&range->start, stream);
  fprintf(stream, ":");
  range_print_pos(&range->end, stream);
}
int TMIPS::range_cmp_pos(struct range_pos_t *pos,		/* execution position */
	      counter_t val)			/* position value */
{
  if (val < pos->pos)
    return /* before */-1;
  else if (val == pos->pos)
    return /* equal */0;
  else /* if (pos->pos < val) */
    return /* after */1;
}
int TMIPS::range_cmp_range(struct range_range_t *range,	/* execution range */
		counter_t val)			/* position value */
{
  if (range->start.ptype != range->end.ptype)
    panic("invalid range");

  if (val < range->start.pos)
    return /* before */-1;
  else if (range->start.pos <= val && val <= range->end.pos)
    return /* inside */0;
  else /* if (range->end.pos < val) */
    return /* after */1;
}
int TMIPS::range_cmp_range1(struct TCore::range_range_t *range,	/* execution range */
		 md_addr_t addr,		/* address value */
		 counter_t icount,		/* instruction count */
		 counter_t cycle)		/* cycle count */
{
  if (range->start.ptype != range->end.ptype)
    panic("invalid range");

  switch (range->start.ptype)
    {
    case pt_addr:
      if (addr < (md_addr_t)range->start.pos)
	return /* before */-1;
      else if ((md_addr_t)range->start.pos <= addr && addr <= (md_addr_t)range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < addr) */
	return /* after */1;
      break;
    case pt_inst:
      if (icount < range->start.pos)
	return /* before */-1;
      else if (range->start.pos <= icount && icount <= range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < icount) */
	return /* after */1;
      break;
    case pt_cycle:
      if (cycle < range->start.pos)
	return /* before */-1;
      else if (range->start.pos <= cycle && cycle <= range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < cycle) */
	return /* after */1;
      break;
    default:
      panic("bogus range type");
    }
}

/*range.cc end*/
/*some of ptrace.cc/h*/
void TMIPS::ptrace_open(char *fname,		/* output filename */
	    char *range)		/* trace range */
{
  char *errstr;

  /* parse the output range */
  if (!range)
    {
      /* no range */
      errstr = range_parse_range(":", &ptrace_range);
      if (errstr)
	panic("cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = TRUE;
    }
  else
    {
      errstr = range_parse_range(range, &ptrace_range);
      if (errstr)
	fatal("cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = FALSE;
    }

  if (ptrace_range.start.ptype != ptrace_range.end.ptype)
    fatal("range endpoints are not of the same type");

  /* open output trace file */
  if (!fname || !strcmp(fname, "-") || !strcmp(fname, "stderr"))
    ptrace_outfd = stderr;
  else if (!strcmp(fname, "stdout"))
    ptrace_outfd = stdout;
  else
    {
      ptrace_outfd = fopen(fname, "w");
      if (!ptrace_outfd)
	fatal("cannot open pipetrace output file `%s'", fname);
    }
}

/* close pipeline trace */
void TMIPS::ptrace_close(void)
{
  if (ptrace_outfd != NULL && ptrace_outfd != stderr && ptrace_outfd != stdout)
    fclose(ptrace_outfd);
}
void TMIPS::__ptrace_endinst(unsigned int iseq)	/* instruction sequence number */
{
  fprintf(ptrace_outfd, "- %u\n", iseq);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new cycle */
void TMIPS::__ptrace_newcycle(tick_t cycle)		/* new cycle */
{
  fprintf(ptrace_outfd, "@ %.0f\n", (double)cycle);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* indicate instruction transition to a new pipeline stage */
void TMIPS::__ptrace_newstage(unsigned int iseq,	/* instruction sequence number */
		  char *pstage,		/* pipeline stage entered */
		  unsigned int pevents)/* pipeline events while in stage */
{
  fprintf(ptrace_outfd, "* %u %s 0x%08x\n", iseq, pstage, pevents);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}
void TMIPS::__ptrace_newinst(unsigned int iseq,	/* instruction sequence number */
		 md_inst_t inst,	/* new instruction */
		 md_addr_t pc,		/* program counter of instruction */
		 md_addr_t addr)	/* address referenced, if load/store */
{
  myfprintf(ptrace_outfd, "+ %u 0x%08p 0x%08p ", iseq, pc, addr);
  md_print_insn(inst, addr, ptrace_outfd);
  fprintf(ptrace_outfd, "\n");

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new uop */
void TMIPS::__ptrace_newuop(unsigned int iseq,	/* instruction sequence number */
		char *uop_desc,		/* new uop description */
		md_addr_t pc,		/* program counter of instruction */
		md_addr_t addr)		/* address referenced, if load/store */
{
  myfprintf(ptrace_outfd,
	    "+ %u 0x%08p 0x%08p [%s]\n", iseq, pc, addr, uop_desc);
  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/*some of ptrace.cc/h end*/





/*sim-outorder.cc*/
unsigned int TMIPS::			/* total latency of access */
mem_access_latency(int blk_sz)		/* block size accessed */
{
  int chunks = (blk_sz + (mem_bus_width - 1)) / mem_bus_width;
//  fputs("access_mem:1 ",Simulation::visualFile);
  esynetrcd->mem_access += 1;
  esynetrcd->valid = 1;
  assert(chunks > 0);

  return (/* first chunk latency */mem_lat[0] +
	  (/* remainder chunk latency */mem_lat[1] * (chunks - 1)));
}
unsigned int TMIPS::		/* latency of block access */
dl1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
	      md_addr_t baddr,		/* block address to access */
	      int bsize,		/* size of block to access */
	      struct cache_blk_t *blk,	/* ptr to block in upper level */
	      tick_t now)		/* time of access */
{
  unsigned int lat;

  if (runcontext->cache_dl2)
    {
	  esynetrcd->l2dcache_access += 1;
	  esynetrcd->valid = 1;
      /* access next level of data cache hierarchy */
      lat = cache_access_dl2(runcontext->cache_dl2, cmd, baddr, NULL, bsize,
			 /* now */now, /* pudata */NULL, /* repl addr */NULL);
      if (cmd == Read)
	return lat;
      else
	{
	  /* FIXME: unlimited write buffers */
	  return 0;
	}
    }
  else
    {
      /* access main memory */
      if (cmd == Read)
	return mem_access_latency(bsize);
      else
	{
	  /* FIXME: unlimited write buffers */
	  return 0;
	}
    }
}
unsigned int TMIPS::dl2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
	      md_addr_t baddr,		/* block address to access */
	      int bsize,		/* size of block to access */
	      struct cache_blk_t *blk,	/* ptr to block in upper level */
	      tick_t now)		/* time of access */
{
  /* this is a miss to the lowest level, so access main memory */
  if (cmd == Read)
    return mem_access_latency(bsize);
  else
    {
      /* FIXME: unlimited write buffers */
      return 0;
    }
}
unsigned int TMIPS::il1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
	      md_addr_t baddr,		/* block address to access */
	      int bsize,		/* size of block to access */
	      struct cache_blk_t *blk,	/* ptr to block in upper level */
	      tick_t now)		/* time of access */
{
  unsigned int lat;

if (runcontext->cache_il2)
    {
	esynetrcd->l2icache_access += 1;
	esynetrcd->valid = 1;
      /* access next level of inst cache hierarchy */
      lat = cache_access_il2(runcontext->cache_il2, cmd, baddr, NULL, bsize,
			 /* now */now, /* pudata */NULL, /* repl addr */NULL);
      if (cmd == Read)
	return lat;
      else
	panic("writes to instruction memory not supported");
    }
  else
    {
      /* access main memory */
      if (cmd == Read)
	return mem_access_latency(bsize);
      else
	panic("writes to instruction memory not supported");
    }
}
unsigned int TMIPS::il2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
	      md_addr_t baddr,		/* block address to access */
	      int bsize,		/* size of block to access */
	      struct cache_blk_t *blk,	/* ptr to block in upper level */
	      tick_t now)		/* time of access */
{
  /* this is a miss to the lowest level, so access main memory */
  if (cmd == Read)
    return mem_access_latency(bsize);
  else
    panic("writes to instruction memory not supported");
}

unsigned int TMIPS::itlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
	       md_addr_t baddr,		/* block address to access */
	       int bsize,		/* size of block to access */
	       struct cache_blk_t *blk,	/* ptr to block in upper level */
	       tick_t now)		/* time of access */
{
  md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;

  /* no real memory access, however, should have user data space attached */
  assert(phy_page_ptr);

  /* fake translation, for now... */
  *phy_page_ptr = 0;

  /* return tlb miss latency */
  return tlb_miss_lat;
}
unsigned int TMIPS::dtlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
	       md_addr_t baddr,	/* block address to access */
	       int bsize,		/* size of block to access */
	       struct cache_blk_t *blk,	/* ptr to block in upper level */
	       tick_t now)		/* time of access */
{
  md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;

  /* no real memory access, however, should have user data space attached */
  assert(phy_page_ptr);

  /* fake translation, for now... */
  *phy_page_ptr = 0;

  /* return tlb miss latency */
  return tlb_miss_lat;
}

/* un-initialize the simulator */
void TMIPS::sim_uninit(void)
{

  if (ptrace_nelt > 0)
    ptrace_close();

}
void TMIPS::ruu_init(void)
{
  int i;

  runcontext->RUU = (struct RUU_station *)calloc(RUU_size, sizeof(struct RUU_station));
  if (!runcontext->RUU)
    fatal("out of virtual memory");

  runcontext->RUU_num = 0;
  runcontext->RUU_head = runcontext->RUU_tail = 0;
  runcontext->RUU_count = 0;
  runcontext->RUU_fcount = 0;
//  for(i = 0; i < RUU_size; i++)
//	  runcontext->RUU[i].threadid = ;
}
void TMIPS::ruu_dump(FILE *stream)				/* output stream */
{
  int num, head;
  struct RUU_station *rs;
  if (!stream)
    stream = stderr;

  fprintf(stream, "** RUU state **\n");
  fprintf(stream, "RUU_head: %d, RUU_tail: %d\n", runcontext->RUU_head, runcontext->RUU_tail);
  fprintf(stream, "RUU_num: %d\n", runcontext->RUU_num);

  num = runcontext->RUU_num;
  head = runcontext->RUU_head;
  while (num)
    {
      rs = & runcontext->RUU[head];
      ruu_dumpent(rs, rs - runcontext->RUU, stream, /* header */TRUE);
      head = (head + 1) % RUU_size;
      num--;
    }
}
void TMIPS::ruu_dumpent(struct RUU_station *rs,		/* ptr to RUU station */
	    int index,				/* entry index */
	    FILE *stream,			/* output stream */
	    int header)				/* print header? */
{
  if (!stream)
    stream = stderr;

  if (header)
    fprintf(stream, "idx: %2d: opcode: %s, inst: `",
	    index, MD_OP_NAME(rs->op));
  else
    fprintf(stream, "       opcode: %s, inst: `",
	    MD_OP_NAME(rs->op));
  md_print_insn(rs->IR, rs->PC, stream);
  fprintf(stream, "'\n");
  myfprintf(stream, "         PC: 0x%08p, NPC: 0x%08p (pred_PC: 0x%08p)\n",
	    rs->PC, rs->next_PC, rs->pred_PC);
  fprintf(stream, "         in_LSQ: %s, ea_comp: %s, recover_inst: %s\n",
	  rs->in_LSQ ? "t" : "f",
	  rs->ea_comp ? "t" : "f",
	  rs->recover_inst ? "t" : "f");
  myfprintf(stream, "         spec_mode: %s, addr: 0x%08p, tag: 0x%08x\n",
	    rs->spec_mode ? "t" : "f", rs->addr, rs->tag);
  fprintf(stream, "         seq: 0x%08x, ptrace_seq: 0x%08x\n",
	  rs->seq, rs->ptrace_seq);
  fprintf(stream, "         queued: %s, issued: %s, completed: %s\n",
	  rs->queued ? "t" : "f",
	  rs->issued ? "t" : "f",
	  rs->completed ? "t" : "f");
  fprintf(stream, "         operands ready: %s\n",
	  OPERANDS_READY(rs) ? "t" : "f");
}
void TMIPS::lsq_init(void)
{
  int i;

  runcontext->LSQ = (struct RUU_station *)calloc(LSQ_size, sizeof(struct RUU_station));
  if (!runcontext->LSQ)
    fatal("out of virtual memory");

  runcontext->LSQ_num = 0;
  runcontext->LSQ_head = runcontext->LSQ_tail = 0;
  runcontext->LSQ_count = 0;
  runcontext->LSQ_fcount = 0;
}
void TMIPS::lsq_dump(FILE *stream)				/* output stream */
{
  int num, head;
  struct RUU_station *rs;

  if (!stream)
    stream = stderr;

  fprintf(stream, "** LSQ state **\n");
  fprintf(stream, "LSQ_head: %d, LSQ_tail: %d\n", runcontext->LSQ_head, runcontext->LSQ_tail);
  fprintf(stream, "LSQ_num: %d\n", runcontext->LSQ_num);

  num = runcontext->LSQ_num;
  head = runcontext->LSQ_head;
  while (num)
    {
      rs = & runcontext->LSQ[head];
      ruu_dumpent(rs, rs - runcontext->LSQ, stream, /* header */TRUE);
      head = (head + 1) % LSQ_size;
      num--;
    }
}
void TMIPS::rslink_init(void)			/* total number of RS_LINK available */
{
  int i;
  struct RS_link *link;
  rslink_free_list = NULL;
  for (i=0; i<MAX_RS_LINKS; i++)
    {
      link = (struct RS_link *)calloc(1, sizeof(struct RS_link));
      if (!link)
	fatal("out of virtual memory");
      link->next = rslink_free_list;
      rslink_free_list = link;
    }
}
void TMIPS::ruu_release_fu(void)
{
  int i;

  /* walk all resource units, decrement busy counts by one */
  for (i=0; i<runcontext->fu_pool->num_resources; i++)
    {
      /* resource is released when BUSY hits zero */
      if (runcontext->fu_pool->resources[i].busy > 0)
    	  runcontext->fu_pool->resources[i].busy--;
    }
}
void TMIPS::eventq_init(void)
{
  int i;
  for(i = 0; i < MAXTHREADS; i++)
	event_queue = NULL;
}
void TMIPS::readyq_init(void)
{
  int i;
  for(i = 0; i < MAXTHREADS; i++)
	ready_queue = NULL;
}
void TMIPS::eventq_dump(FILE *stream)			/* output stream */
{
  struct RS_link *ev;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** event queue state **\n");
  for (ev = event_queue; ev != NULL; ev = ev->next)
    {
      /* is event still valid? */
      if (RSLINK_VALID(ev))
      {
	  struct RUU_station *rs = RSLINK_RS(ev);

	  fprintf(stream, "idx: %2d: @ %.0f\n",
		  (int)(rs - (rs->in_LSQ ? runcontext->LSQ : runcontext->RUU)), (double)ev->x.when);
	  ruu_dumpent(rs, rs - (rs->in_LSQ ? runcontext->LSQ : runcontext->RUU),
		      stream, /* !header */FALSE);
      }
    }

}

void TMIPS::eventq_queue_event(struct RUU_station *rs, tick_t when)
{
  struct RS_link *prev, *ev, *new_ev;

  if (rs->completed)
    panic("event completed");

  if (when <= sim_cycle)
    panic("event occurred in the past");

  /* get a free event record */
  RSLINK_NEW(new_ev, rs);
  new_ev->x.when = when;

  /* locate insertion point */
  for (prev=NULL, ev=event_queue;
       ev && ev->x.when < when;
       prev=ev, ev=ev->next);

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
struct TMIPS::RUU_station * TMIPS::eventq_next_event(void)
{
  struct RS_link *ev;

  if (event_queue && event_queue->x.when <= sim_cycle)
    {
      /* unlink and return first event on priority list */
      ev = event_queue;
      event_queue = event_queue->next;

      /* event still valid? */
      if (RSLINK_VALID(ev))
	{
	  struct RUU_station *rs = RSLINK_RS(ev);

	  /* reclaim event record */
	  RSLINK_FREE(ev);

	  /* event is valid, return resv station */
	  return rs;
	}
      else
	{
	  /* reclaim event record */
	  RSLINK_FREE(ev);

	  /* receiving inst was squashed, return next event */
	  return eventq_next_event();
	}
    }
  else
    {
      /* no event or no event is ready */
      return NULL;
    }
}
void TMIPS::readyq_dump(FILE *stream)			/* output stream */
{
  struct RS_link *link;

  if (!stream)
    stream = stderr;

  fprintf(stream, "** ready queue state **\n");

  for (link = ready_queue; link != NULL; link = link->next)
    {
      /* is entry still valid? */
      if (RSLINK_VALID(link))
	{
	  struct RUU_station *rs = RSLINK_RS(link);

	  ruu_dumpent(rs, rs - (rs->in_LSQ ? runcontext->LSQ : runcontext->RUU),
		      stream, /* header */TRUE);
	}
    }
}
void TMIPS::readyq_enqueue(struct RUU_station *rs)		/* RS to enqueue */
{
  struct RS_link *prev, *node, *new_node;

  /* node is now queued */
  if (rs->queued)
    panic("node is already queued");
  rs->queued = TRUE;

  /* get a free ready list node */
  RSLINK_NEW(new_node, rs);
  new_node->x.seq = rs->seq;

  /* locate insertion point */
  if (rs->in_LSQ || MD_OP_FLAGS(rs->op) & (F_LONGLAT|F_CTRL))
    {
      /* insert loads/stores and long latency ops at the head of the queue */
      prev = NULL;
      node = ready_queue;
    }
  else
    {
      /* otherwise insert in program order (earliest seq first) */
      for (prev=NULL, node=ready_queue;
	   node && node->x.seq < rs->seq;
	   prev=node, node=node->next);
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
void TMIPS::cv_init()
{
  int i;

  /* initially all registers are valid in the architected register file,
     i.e., the create vector entry is CVLINK_NULL */
  for (i=0; i < MD_TOTAL_REGS; i++)
    {
      runcontext->create_vector[i] = CVLINK_NULL;
      runcontext->create_vector_rt[i] = 0;
      runcontext->spec_create_vector[i] = CVLINK_NULL;
      runcontext->spec_create_vector_rt[i] = 0;
    }

  /* all create vector entries are non-speculative */
  BITMAP_CLEAR_MAP(runcontext->use_spec_cv, CV_BMAP_SZ);
}
void TMIPS::cv_dump(FILE *stream)				/* output stream */
{
  int i;
  struct CV_link ent;

  if (!stream)
    stream = stderr;

  fprintf(stream, "** create vector state **\n");

  for (i=0; i < MD_TOTAL_REGS; i++)
    {
      ent = CREATE_VECTOR(i);
      if (!ent.rs)
	fprintf(stream, "[cv%02d]: from architected reg file\n", i);
      else
	fprintf(stream, "[cv%02d]: from %s, idx: %d\n",
		i, (ent.rs->in_LSQ ? "LSQ" : "RUU"),
		(int)(ent.rs - (ent.rs->in_LSQ ? runcontext->LSQ : runcontext->RUU)));
    }
}
void TMIPS::ruu_commit(void)
{
  int i, lat, events, committed = 0;
  static counter_t sim_ret_insn = 0;

  /* all values must be retired to the architected reg file in program order */
  while (runcontext->RUU_num > 0 && committed < ruu_commit_width)//4
    {	/*线程中操作的lsq，但是ruu是赋值到rs的*/
      struct RUU_station *rs = &(runcontext->RUU[runcontext->RUU_head]);

      if (!rs->completed)
	{
	  /* at least RUU entry must be complete */
	  break;
	}

      /* default commit events */
      events = 0;

      /* load/stores must retire load/store queue entry as well */
      if (runcontext->RUU[runcontext->RUU_head].ea_comp)
	{
	  /* load/store, retire head of LSQ as well */
	  if (runcontext->LSQ_num <= 0 || !runcontext->LSQ[runcontext->LSQ_head].in_LSQ)
	    panic("RUU out of sync with LSQ");

	  /* load/store operation must be complete */
	  if (!runcontext->LSQ[runcontext->LSQ_head].completed)
	    {
	      /* load/store operation is not yet complete */
	      break;
	    }

	  if ((MD_OP_FLAGS(runcontext->LSQ[runcontext->LSQ_head].op) & (F_MEM|F_STORE))
	      == (F_MEM|F_STORE))
	    {
	      struct res_template *fu;


	      /* stores must retire their store value to the cache at commit,
		 try to get a store port (functional unit allocation) */
	      fu = res_get(runcontext->fu_pool, MD_OP_FUCLASS(runcontext->LSQ[runcontext->LSQ_head].op));
	      if (fu)
		{
		  /* reserve the functional unit */
		  if (fu->master->busy)
		    panic("functional unit already in use");

		  /* schedule functional unit release event */
		  fu->master->busy = fu->issuelat;

		  /* go to the data cache */
		  if (runcontext->cache_dl1)
		    {
		      /* commit store value to D-cache */
//			  fputs("1(sendto_dl1_cache) ",Simulation::visualFile);

		      lat =
			cache_access_dl1(runcontext->cache_dl1, Write, (runcontext->LSQ[runcontext->LSQ_head].addr&~3),
				     NULL, 4, sim_cycle, NULL, NULL);
		      if (lat > cache_dl1_lat)
			events |= PEV_CACHEMISS;
		    }

		  /* all loads and stores must to access D-TLB */
		  if (runcontext->dtlb)
		    {
		      /* access the D-TLB */
		      lat =
			cache_access(runcontext->dtlb, Read, (runcontext->LSQ[runcontext->LSQ_head].addr & ~3),
				     NULL, 4, sim_cycle, NULL, NULL);
		      if (lat > 1)
			events |= PEV_TLBMISS;
		    }
		}
	      else
		{
		  /* no store ports left, cannot continue to commit insts */
		  break;
		}
	    }

	  /* invalidate load/store operation instance */
	  runcontext->LSQ[runcontext->LSQ_head].tag++;
          sim_slip += (sim_cycle - runcontext->LSQ[runcontext->LSQ_head].slip);

	  /* indicate to pipeline trace that this instruction retired */
	  ptrace_newstage(runcontext->LSQ[runcontext->LSQ_head].ptrace_seq, PST_COMMIT, events);
	  ptrace_endinst(runcontext->LSQ[runcontext->LSQ_head].ptrace_seq);

	  /* commit head of LSQ as well */
	  runcontext->LSQ_head = (runcontext->LSQ_head + 1) % LSQ_size;
	  runcontext->LSQ_num--;
	}

      if (runcontext->pred
	  && bpred_spec_update == spec_CT
	  && (MD_OP_FLAGS(rs->op) & F_CTRL))
	{
	  bpred_update(runcontext->pred,
		       /* branch address */rs->PC,
		       /* actual target address */rs->next_PC,
                       /* taken? */rs->next_PC != (rs->PC +
                                                   sizeof(md_inst_t)),
                       /* pred taken? */rs->pred_PC != (rs->PC +
                                                        sizeof(md_inst_t)),
                       /* correct pred? */rs->pred_PC == rs->next_PC,
                       /* opcode */rs->op,
                       /* dir predictor update pointer */&rs->dir_update);
	  thispower->gpp_data->btb_para->total_btb_write++;
	  allpower->gpp_data->btb_para->total_btb_write++;
	}

      /* invalidate RUU operation instance */
      runcontext->RUU[runcontext->RUU_head].tag++;
      sim_slip += (sim_cycle - runcontext->RUU[runcontext->RUU_head].slip);
      /* print retirement trace if in verbose mode */
      if (!verbose)
	{
	  sim_ret_insn++;
	  //myfprintf(stderr, "%10n @ 0x%08p: ", sim_ret_insn, runcontext->RUU[runcontext->RUU_head].PC);
 	  //md_print_insn(runcontext->RUU[runcontext->RUU_head].IR, runcontext->RUU[runcontext->RUU_head].PC, stderr);
	  if (MD_OP_FLAGS(runcontext->RUU[runcontext->RUU_head].op) & F_MEM)
	    myfprintf(stderr, "  mem: 0x%08p", runcontext->RUU[runcontext->RUU_head].addr);
//	  fprintf(stderr, "\n");
	  /* fflush(stderr); */
	}

      /* indicate to pipeline trace that this instruction retired */
      ptrace_newstage(runcontext->RUU[runcontext->RUU_head].ptrace_seq, PST_COMMIT, events);
      ptrace_endinst(runcontext->RUU[runcontext->RUU_head].ptrace_seq);

      /* commit head entry of RUU */
      runcontext->RUU_head = (runcontext->RUU_head + 1) % RUU_size;
      runcontext->RUU_num--;

      /* one more instruction committed to architected state */
      committed++;

      for (i=0; i<MAX_ODEPS; i++)
	{
	  if (rs->odep_list[i])
	    panic ("retired instruction has odeps\n");
        }
    }
}
void TMIPS::ruu_recover(int branch_index)			/* index of mis-pred branch */
{
  int i, RUU_index = runcontext->RUU_tail, LSQ_index = runcontext->LSQ_tail;
  int RUU_prev_tail = runcontext->RUU_tail, LSQ_prev_tail = runcontext->LSQ_tail;

  /* recover from the tail of the RUU towards the head until the branch index
     is reached, this direction ensures that the LSQ can be synchronized with
     the RUU */

  /* go to first element to squash */
  RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
  LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;
  thispower->gpp_data->core_para->total_branch_miss_num+=1;
  allpower->gpp_data->core_para->total_branch_miss_num+=1;
  /* traverse to older insts until the mispredicted branch is encountered */
  while (RUU_index != branch_index)
    {
      /* the RUU should not drain since the mispredicted branch will remain */
      if (!runcontext->RUU_num)
	panic("empty RUU");

      /* should meet up with the tail first */
      if (RUU_index == runcontext->RUU_head)
	panic("RUU head and tail broken");

      /* is this operation an effective addr calc for a load or store? */
      if (runcontext->RUU[RUU_index].ea_comp)
	{
	  /* should be at least one load or store in the LSQ */
	  if (!runcontext->LSQ_num)
	    panic("RUU and LSQ out of sync");

	  /* recover any resources consumed by the load or store operation */
	  for (i=0; i<MAX_ODEPS; i++)
	    {
	      RSLINK_FREE_LIST(runcontext->LSQ[LSQ_index].odep_list[i]);
	      /* blow away the consuming op list */
	      runcontext->LSQ[LSQ_index].odep_list[i] = NULL;
	    }

	  /* squash this LSQ entry */
	  runcontext->LSQ[LSQ_index].tag++;

	  /* indicate in pipetrace that this instruction was squashed */
	  ptrace_endinst(runcontext->LSQ[LSQ_index].ptrace_seq);

	  /* go to next earlier LSQ slot */
	  LSQ_prev_tail = LSQ_index;
	  LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;
	  runcontext->LSQ_num--;
	}

      /* recover any resources used by this RUU operation */
      for (i=0; i<MAX_ODEPS; i++)
	{
	  RSLINK_FREE_LIST(runcontext->RUU[RUU_index].odep_list[i]);
	  /* blow away the consuming op list */
	  runcontext->RUU[RUU_index].odep_list[i] = NULL;
	}

      /* squash this RUU entry */
      runcontext->RUU[RUU_index].tag++;

      /* indicate in pipetrace that this instruction was squashed */
      ptrace_endinst(runcontext->RUU[RUU_index].ptrace_seq);

      /* go to next earlier slot in the RUU */
      RUU_prev_tail = RUU_index;
      RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
      runcontext->RUU_num--;
    }

  /* reset head/tail pointers to point to the mis-predicted branch */
  runcontext->RUU_tail = RUU_prev_tail;
  runcontext->LSQ_tail = LSQ_prev_tail;

  /* revert create vector back to last precise create vector state, NOTE:
     this is accomplished by resetting all the copied-on-write bits in the
     USE_SPEC_CV bit vector */
  BITMAP_CLEAR_MAP(runcontext->use_spec_cv, CV_BMAP_SZ);

  /* FIXME: could reset functional units at squash time */
}
void TMIPS::ruu_writeback(void)
{
  int i;
  struct RUU_station *rs;

  /* service all completed events */
  while ((rs = eventq_next_event()))
    {
      /* RS has completed execution and (possibly) produced a result */
      if (!OPERANDS_READY(rs) || rs->queued || !rs->issued || rs->completed)
	panic("inst completed and !ready, !issued, or completed");//issue one by one

      /* operation has completed */
      rs->completed = TRUE;

      /* does this operation reveal a mis-predicted branch? */
      if (rs->recover_inst)//mis-predicted 恢复状态
	{
	  if (rs->in_LSQ)
	    panic("mis-predicted load or store?!?!?");

	  /* recover processor state and reinit fetch to correct path */
	  ruu_recover(rs - runcontext->RUU);//恢复RUU状态
	  tracer_recover();//恢复指令流状态
	  bpred_recover(runcontext->pred, rs->PC, rs->stack_recover_idx);//恢复return address的堆栈
	  /* stall fetch until I-fetch and I-decode recover */
	  ruu_fetch_issue_delay = ruu_branch_penalty;

	  /* continue writeback of the branch/control instruction */
	}

      /* if we speculatively update branch-predictor, do it here */
      if (runcontext->pred
	  && bpred_spec_update == spec_WB
	  && !rs->in_LSQ
	  && (MD_OP_FLAGS(rs->op) & F_CTRL))
	{
	  bpred_update(runcontext->pred,
		       /* branch address */rs->PC,
		       /* actual target address */rs->next_PC,
		       /* taken? */rs->next_PC != (rs->PC +
						   sizeof(md_inst_t)),
		       /* pred taken? */rs->pred_PC != (rs->PC +
							sizeof(md_inst_t)),
		       /* correct pred? */rs->pred_PC == rs->next_PC,
		       /* opcode */rs->op,
		       /* dir predictor update pointer */&rs->dir_update);
	  thispower->gpp_data->btb_para->total_btb_write++;
	  allpower->gpp_data->btb_para->total_btb_write++;
	}

      /* entered writeback stage, indicate in pipe trace */
      ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK,
		      rs->recover_inst ? PEV_MPDETECT : 0);

      /* broadcast results to consuming operations, this is more efficiently
         accomplished by walking the output dependency chains of the
	 completed instruction */
      for (i=0; i<MAX_ODEPS; i++)
	{
	  if (rs->onames[i] != NA)
	    {//返回结构内有寄存器（寄存器内有结果）
	      struct CV_link link;
	      struct RS_link *olink, *olink_next;

	      if (rs->spec_mode)
		{
		  /* update the speculative create vector, future operations
		     get value from later creator or architected reg file */
		  link = runcontext->spec_create_vector[rs->onames[i]];
		  if (/* !NULL */link.rs
		      && /* refs RS */(link.rs == rs && link.odep_num == i))
		    {
		      /* the result can now be read from a physical register,
			 indicate this as so */
			  runcontext->spec_create_vector[rs->onames[i]] = CVLINK_NULL;
			  runcontext->spec_create_vector_rt[rs->onames[i]] = sim_cycle;
		    }
		  /* else, creator invalidated or there is another creator */
		}
	      else
		{
		  /* update the non-speculative create vector, future
		     operations get value from later creator or architected
		     reg file */
		  link = runcontext->create_vector[rs->onames[i]];//create_vector:指向寄存器的创建
		  if (/* !NULL */link.rs
		      && /* refs RS */(link.rs == rs && link.odep_num == i))
		    {
			  /* the result can now be read from a physical register,
			 indicate this as so */
			  runcontext->create_vector[rs->onames[i]] = CVLINK_NULL;
			  runcontext->create_vector_rt[rs->onames[i]] = sim_cycle;
		    }
		  /* else, creator invalidated or there is another creator */
		}

	      /* walk output list, queue up ready operations */
	      for (olink=rs->odep_list[i]; olink; olink=olink_next)//olink:RS_LINK
		{//将与该条指令输出值相关联的其它指令的输入数据值为有效（该条指令已经完成），然后对它们排序放入ready_queue
		  if (RSLINK_VALID(olink))
		    {
		      if (olink->rs->idep_ready[olink->x.opnum])
			panic("output dependence already satisfied");//WAW

		      /* input is now ready */
		      olink->rs->idep_ready[olink->x.opnum] = TRUE;

		      /* are all the register operands of target ready? */
		      if (OPERANDS_READY(olink->rs))
			{
			  /* yes! enqueue instruction as ready, NOTE: stores
			     complete at dispatch, so no need to enqueue
			     them */
			  if (!olink->rs->in_LSQ
			      || ((MD_OP_FLAGS(olink->rs->op)&(F_MEM|F_STORE))
				  == (F_MEM|F_STORE)))
			    readyq_enqueue(olink->rs);
			  /* else, ld op, issued when no mem conflict */
			}
		    }

		  /* grab link to next element prior to free */
		  olink_next = olink->next;

		  /* free dependence link element */
		  RSLINK_FREE(olink);
		}
	      /* blow away the consuming op list */
	      rs->odep_list[i] = NULL;

	    } /* if not NA output */

	} /* for all outputs */

   } /* for all writeback events */

}
void TMIPS::lsq_refresh(void)
{
  int i, j, index, n_std_unknowns;
  md_addr_t std_unknowns[MAX_STD_UNKNOWNS];

  /* scan entire queue for ready loads: scan from oldest instruction
     (head) until we reach the tail or an unresolved store, after which no
     other instruction will become ready */
  for (i=0, index=runcontext->LSQ_head, n_std_unknowns=0;
       i < runcontext->LSQ_num;
       i++, index=(index + 1) % LSQ_size)
    {
      /* terminate search for ready loads after first unresolved store,
	 as no later load could be resolved in its presence */
      if (/* store? */
	  (MD_OP_FLAGS(runcontext->LSQ[index].op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
	{
	  if (!STORE_ADDR_READY(&runcontext->LSQ[index]))
	    {
	      /* FIXME: a later STD + STD known could hide the STA unknown */
	      /* sta(store address) unknown, blocks all later loads, stop search */
	      break;
	    }
	  else if (!OPERANDS_READY(&runcontext->LSQ[index]))
	    {
	      /* sta known, but std(store data) unknown, may block a later store, record
		 this address for later referral, we use an array here because
		 for most simulations the number of entries to search will be
		 very small */
	      if (n_std_unknowns == MAX_STD_UNKNOWNS)
		fatal("STD unknown array overflow, increase MAX_STD_UNKNOWNS");
	      std_unknowns[n_std_unknowns++] = runcontext->LSQ[index].addr;
	    }
	  else /* STORE_ADDR_READY() && OPERANDS_READY() */
	    {
	      /* a later STD known hides an earlier STD unknown */
	      for (j=0; j<n_std_unknowns; j++)
		{
		  if (std_unknowns[j] == /* STA/STD known */runcontext->LSQ[index].addr)
		    std_unknowns[j] = /* bogus addr */0;
		}
	    }
	}

      if (/* load? */
	  ((MD_OP_FLAGS(runcontext->LSQ[index].op) & (F_MEM|F_LOAD)) == (F_MEM|F_LOAD))
	  && /* queued? */!runcontext->LSQ[index].queued
	  && /* waiting? */!runcontext->LSQ[index].issued
	  && /* completed? */!runcontext->LSQ[index].completed
	  && /* regs ready? */OPERANDS_READY(&runcontext->LSQ[index]))
	{
	  /* no STA unknown conflict (because we got to this check), check for
	     a STD unknown conflict */
	  for (j=0; j<n_std_unknowns; j++)
	    {
	      /* found a relevant STD unknown? */
	      if (std_unknowns[j] == runcontext->LSQ[index].addr)
		break;
	    }
	  if (j == n_std_unknowns)
	    {
	      /* no STA or STD unknown conflicts, put load on ready queue */
	      readyq_enqueue(&runcontext->LSQ[index]);
	    }
	}
    }
}
void TMIPS::ruu_issue(void)
{
  int i, load_lat, tlb_lat, n_issued;
  struct RS_link *node, *next_node;
  struct res_template *fu;

  /* FIXME: could be a little more efficient when scanning the ready queue */

  /* copy and then blow away the ready list, NOTE: the ready list is
     always totally reclaimed each cycle, and instructions that are not
     issue are explicitly reinserted into the ready instruction queue,
     this management strategy ensures that the ready instruction queue
     is always properly sorted */
  node = ready_queue;
  ready_queue = NULL;

  /* visit all ready instructions (i.e., insts whose register input
     dependencies have been satisfied, stop issue when no more instructions
     are available or issue bandwidth is exhausted */
  for (n_issued=0;
       node && n_issued < ruu_issue_width;//4
       node = next_node)
    {
      next_node = node->next;

      /* still valid? */
      if (RSLINK_VALID(node))
	{
	  struct RUU_station *rs = RSLINK_RS(node);//一个节点就是一个发送对象

	  /* issue operation, both reg and mem deps have been satisfied */
	  if (!OPERANDS_READY(rs) || !rs->queued
	      || rs->issued || rs->completed)
	    panic("issued inst !ready, issued, or completed");

	  /* node is now un-queued */
	  rs->queued = FALSE;

	  if (rs->in_LSQ
	      && ((MD_OP_FLAGS(rs->op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE)))
	    {
	      /* stores complete in effectively zero time, result is
		 written into the load/store queue, the actual store into
		 the memory system occurs when the instruction is retired
		 (see ruu_commit()) */
	      rs->issued = TRUE;
	      rs->completed = TRUE;
	      if (rs->onames[0] || rs->onames[1])
		panic("store creates result");

	      if (rs->recover_inst)
		panic("mis-predicted store");

	      /* entered execute stage, indicate in pipe trace */
	      ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK, 0);

	      /* one more inst issued */
	      n_issued++;
	    }
	  else
	    {
	      /* issue the instruction to a functional unit */
	      if (MD_OP_FUCLASS(rs->op) != NA)
		{
		  fu = res_get(runcontext->fu_pool, MD_OP_FUCLASS(rs->op));//fu = function unit
		  if (fu)
		    {
		      /* got one! issue inst to functional unit */
		      rs->issued = TRUE;
		      /* reserve the functional unit */
		      if (fu->master->busy)
			panic("functional unit already in use");

		      /* schedule functional unit release event */
		      fu->master->busy = fu->issuelat;

		      /* schedule a result writeback event */
		      if (rs->in_LSQ
			  && ((MD_OP_FLAGS(rs->op) & (F_MEM|F_LOAD))
			      == (F_MEM|F_LOAD)))
			{
			  int events = 0;

			  /* for loads, determine cache access latency:
			     first scan LSQ to see if a store forward is
			     possible, if not, access the data cache */
			  load_lat = 0;
			  i = (rs - runcontext->LSQ);
			  if (i != runcontext->LSQ_head)
			    {
			      for (;;)
				{
				  /* go to next earlier LSQ entry */
				  i = (i + (LSQ_size-1)) % LSQ_size;

				  /* FIXME: not dealing with partials! */
				  if ((MD_OP_FLAGS(runcontext->LSQ[i].op) & F_STORE)
				      && (runcontext->LSQ[i].addr == rs->addr))
				    {
				      /* hit in the LSQ */
				      load_lat = 1;
				      break;
				    }

				  /* scan finished? */
				  if (i == runcontext->LSQ_head)
				    break;
				}
			    }

			  /* was the value store forwared from the LSQ? */
			  if (!load_lat)
			    {
			      int valid_addr = MD_VALID_ADDR(rs->addr);

			      if (!spec_mode && !valid_addr)
				sim_invalid_addrs++;

			      /* no! go to the data cache if addr is valid */
			      if (runcontext->cache_dl1 && valid_addr)
				{
				  /* access the cache if non-faulting */
				  load_lat =
				    cache_access_dl1(runcontext->cache_dl1, Read,
						 (rs->addr & ~3), NULL, 4,
						 sim_cycle, NULL, NULL);
				  if (load_lat > cache_dl1_lat)
				    events |= PEV_CACHEMISS;
				}
			      else
				{
				  /* no caches defined, just use op latency */
				  load_lat = fu->oplat;
				}
			    }

			  /* all loads and stores must to access D-TLB */
			  if (runcontext->dtlb && MD_VALID_ADDR(rs->addr))
			    {
			      /* access the D-DLB, NOTE: this code will
				 initiate speculative TLB misses */
			      tlb_lat =
				cache_access(runcontext->dtlb, Read, (rs->addr & ~3),
					     NULL, 4, sim_cycle, NULL, NULL);
			      if (tlb_lat > 1)
				events |= PEV_TLBMISS;

			      /* D-cache/D-TLB accesses occur in parallel */
			      load_lat = MAX(tlb_lat, load_lat);
			    }

			  /* use computed cache access latency */
			  eventq_queue_event(rs, sim_cycle + load_lat);

			  /* entered execute stage, indicate in pipe trace */
			  ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
					  ((rs->ea_comp ? PEV_AGEN : 0)
					   | events));
			}
		      else /* !load && !store */
			{
			  /* use deterministic functional unit latency */
			  eventq_queue_event(rs, sim_cycle + fu->oplat);

			  /* entered execute stage, indicate in pipe trace */
			  ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
					  rs->ea_comp ? PEV_AGEN : 0);
			}

		      /* one more inst issued */
		      n_issued++;
		    }
		  else /* no functional unit */
		    {
		      /* insufficient functional unit resources, put operation
			 back onto the ready list, we'll try to issue it
			 again next cycle */
		      readyq_enqueue(rs);
		    }
		}
	      else /* does not require a functional unit! */
		{
		  /* FIXME: need better solution for these */
		  /* the instruction does not need a functional unit */
		  rs->issued = TRUE;

		  /* schedule a result event */
		  eventq_queue_event(rs, sim_cycle + 1);

		  /* entered execute stage, indicate in pipe trace */
		  ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
				  rs->ea_comp ? PEV_AGEN : 0);

		  /* one more inst issued */
		  n_issued++;
		}
	    } /* !store */

	}
      /* else, RUU entry was squashed */

      /* reclaim ready list entry, NOTE: this is done whether or not the
         instruction issued, since the instruction was once again reinserted
         into the ready queue if it did not issue, this ensures that the ready
         queue is always properly sorted */
      RSLINK_FREE(node);
    }

  /* put any instruction not issued back into the ready queue, go through
     normal channels to ensure instruction stay ordered correctly */
  for (; node; node = next_node)
    {
      next_node = node->next;

      /* still valid? */
      if (RSLINK_VALID(node))
        {
	  struct RUU_station *rs = RSLINK_RS(node);

          /* node is now un-queued */
          rs->queued = FALSE;

	  /* not issued, put operation back onto the ready list, we'll try to
	     issue it again next cycle */
          readyq_enqueue(rs);
        }
      /* else, RUU entry was squashed */

      /* reclaim ready list entry, NOTE: this is done whether or not the
         instruction issued, since the instruction was once again reinserted
         into the ready queue if it did not issue, this ensures that the ready
         queue is always properly sorted */
      RSLINK_FREE(node);
    }
}
void TMIPS::rspec_dump(FILE *stream)			/* output stream */
{
  int i;

  if (!stream)
    stream = stderr;

  fprintf(stream, "** speculative register contents **\n");

  fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");

  /* dump speculative integer regs */
  for (i=0; i < MD_NUM_IREGS; i++)
    {
      if (BITMAP_SET_P(runcontext->use_spec_R, R_BMAP_SZ, i))
	{
	  md_print_ireg(runcontext->spec_regs_R, i, stream);
	  fprintf(stream, "\n");
	}
    }

  /* dump speculative FP regs */
  for (i=0; i < MD_NUM_FREGS; i++)
    {
      if (BITMAP_SET_P(runcontext->use_spec_F, F_BMAP_SZ, i))
	{
	  md_print_fpreg(runcontext->spec_regs_F, i, stream);
	  fprintf(stream, "\n");
	}
    }

  /* dump speculative CTRL regs */
  for (i=0; i < MD_NUM_CREGS; i++)
    {
      if (BITMAP_SET_P(runcontext->use_spec_C, C_BMAP_SZ, i))
	{
	  md_print_creg(runcontext->spec_regs_C, i, stream);
	  fprintf(stream, "\n");
	}
    }
}
void TMIPS::tracer_recover(void)
{
  int i;
  struct spec_mem_ent *ent, *ent_next;

  /* better be in mis-speculative trace generation mode */
  if (!spec_mode)
    panic("cannot recover unless in speculative mode");

  /* reset to non-speculative trace generation mode */
  spec_mode = FALSE;

  /* reset copied-on-write register bitmasks back to non-speculative state */
  BITMAP_CLEAR_MAP(runcontext->use_spec_R, R_BMAP_SZ);
  BITMAP_CLEAR_MAP(runcontext->use_spec_F, F_BMAP_SZ);
  BITMAP_CLEAR_MAP(runcontext->use_spec_C, C_BMAP_SZ);

  /* reset memory state back to non-speculative state */
  /* FIXME: could version stamps be used here?!?!? */
  for (i=0; i<STORE_HASH_SIZE; i++)
    {
      /* release all hash table buckets */
      for (ent=runcontext->store_htable[i]; ent; ent=ent_next)
	{
	  ent_next = ent->next;
	  ent->next = runcontext->bucket_free_list;
	  runcontext->bucket_free_list = ent;
	}
      runcontext->store_htable[i] = NULL;
    }

  /* if pipetracing, indicate squash of instructions in the inst fetch queue */
  if (ptrace_active)
    {
      while (runcontext->fetch_num != 0)
	{
	  /* squash the next instruction from the IFETCH -> DISPATCH queue */
	  ptrace_endinst(runcontext->fetch_data[runcontext->fetch_head].ptrace_seq);

	  /* consume instruction from IFETCH -> DISPATCH queue */
	  runcontext->fetch_head = (runcontext->fetch_head+1) & (ruu_ifq_size - 1);
	  runcontext->fetch_num--;
	}
    }

  /* reset IFETCH state */
  runcontext->fetch_num = 0;
  runcontext->fetch_tail = runcontext->fetch_head = 0;
  runcontext->fetch_pred_PC = runcontext->fetch_regs_PC = runcontext->recover_PC;
}
void TMIPS::tracer_init(void)
{
  int i;

  /* initially in non-speculative mode */
  spec_mode = FALSE;

  /* register state is from non-speculative state buffers */
  BITMAP_CLEAR_MAP(runcontext->use_spec_R, R_BMAP_SZ);
  BITMAP_CLEAR_MAP(runcontext->use_spec_F, F_BMAP_SZ);
  BITMAP_CLEAR_MAP(runcontext->use_spec_C, C_BMAP_SZ);

  /* memory state is from non-speculative memory pages */
  for (i=0; i<STORE_HASH_SIZE; i++)
	  runcontext->store_htable[i] = NULL;
}
enum TMIPS::md_fault_type
TMIPS::spec_mem_access(struct mem_t *mem,		/* memory space to access */
		enum mem_cmd cmd,		/* Read or Write access cmd */
		md_addr_t addr,			/* virtual address of access */
		void *p,			/* input/output buffer */
		int nbytes)			/* number of bytes to access */
{
  int i, index;
  struct spec_mem_ent *ent, *prev;

  /* FIXME: partially overlapping writes are not combined... */
  /* FIXME: partially overlapping reads are not handled correctly... */

  /* check alignments, even speculative this test should always pass */
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    {
      /* no can do, return zero result */
      for (i=0; i < nbytes; i++)
	((char *)p)[i] = 0;

      return md_fault_none;
    }

  /* check permissions */
  if (!((addr >= ld_text_base && addr < (ld_text_base+ld_text_size)
	 && cmd == Read)
	|| MD_VALID_ADDR(addr)))
    {
      /* no can do, return zero result */
      for (i=0; i < nbytes; i++)
	((char *)p)[i] = 0;

      return md_fault_none;
    }

  /* has this memory state been copied on mis-speculative write? */
  index = HASH_ADDR(addr);
  for (prev=NULL,ent=runcontext->store_htable[index]; ent; prev=ent,ent=ent->next)
    {
      if (ent->addr == addr)
	{
	  /* reorder chains to speed access into hash table */
	  if (prev != NULL)
	    {
	      /* not at head of list, relink the hash table entry at front */
	      prev->next = ent->next;
              ent->next = runcontext->store_htable[index];
              runcontext->store_htable[index] = ent;
	    }
	  break;
	}
    }

  /* no, if it is a write, allocate a hash table entry to hold the data */
  if (!ent && cmd == Write)
    {
      /* try to get an entry from the free list, if available */
      if (!runcontext->bucket_free_list)
	{
	  /* otherwise, call calloc() to get the needed storage */
    	  runcontext->bucket_free_list = (struct spec_mem_ent *)calloc(1, sizeof(struct spec_mem_ent));
	  if (!runcontext->bucket_free_list)
	    fatal("out of virtual memory");
	}
      ent = runcontext->bucket_free_list;
      runcontext->bucket_free_list = runcontext->bucket_free_list->next;

      if (!bugcompat_mode)
	{
	  /* insert into hash table */
	  ent->next = runcontext->store_htable[index];
	  runcontext->store_htable[index] = ent;
	  ent->addr = addr;
	  ent->data[0] = 0; ent->data[1] = 0;
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
	      *((byte_t *)p) = *((byte_t *)(&ent->data[0]));
	    }
	  else
	    {
	      /* read from non-speculative memory state, don't allocate
	         memory pages with speculative loads */
	      *((byte_t *)p) = MEM_READ_BYTE(mem, addr);
	    }
	}
      else
	{
	  /* always write into mis-speculated state buffer */
	  *((byte_t *)(&ent->data[0])) = *((byte_t *)p);
	}
      break;
    case 2:
      if (cmd == Read)
	{
	  if (ent)
	    {
	      /* read from mis-speculated state buffer */
	      *((half_t *)p) = *((half_t *)(&ent->data[0]));
	    }
	  else
	    {
	      /* read from non-speculative memory state, don't allocate
	         memory pages with speculative loads */
	      *((half_t *)p) = MEM_READ_HALF(mem, addr);
	    }
	}
      else
	{
	  /* always write into mis-speculated state buffer */
	  *((half_t *)&ent->data[0]) = *((half_t *)p);
	}
      break;
    case 4:
      if (cmd == Read)
	{
	  if (ent)
	    {
	      /* read from mis-speculated state buffer */
	      *((word_t *)p) = *((word_t *)&ent->data[0]);
	    }
	  else
	    {
	      /* read from non-speculative memory state, don't allocate
	         memory pages with speculative loads */
	      *((word_t *)p) = MEM_READ_WORD(mem, addr);
	    }
	}

      else
	{
	  /* always write into mis-speculated state buffer */
	  *((word_t *)&ent->data[0]) = *((word_t *)p);
	}
      break;
    case 8:
      if (cmd == Read)
	{
	  if (ent)
	    {
	      /* read from mis-speculated state buffer */
	      *((word_t *)p) = *((word_t *)&ent->data[0]);
	      *(((word_t *)p)+1) = *((word_t *)&ent->data[1]);
	    }
	  else
	    {
	      /* read from non-speculative memory state, don't allocate
	         memory pages with speculative loads */
	      *((word_t *)p) = MEM_READ_WORD(mem, addr);
	      *(((word_t *)p)+1) =
		MEM_READ_WORD(mem, addr + sizeof(word_t));
	    }
	}
      else
	{
	  /* always write into mis-speculated state buffer */
	  *((word_t *)&ent->data[0]) = *((word_t *)p);
	  *((word_t *)&ent->data[1]) = *(((word_t *)p)+1);
	}
      break;
    default:
      panic("access size not supported in mis-speculative mode");
    }

  return md_fault_none;
}
void TMIPS::mspec_dump(FILE *stream)			/* output stream */
{
  int i;
  struct spec_mem_ent *ent;

  if (!stream)
    stream = stderr;

  fprintf(stream, "** speculative memory contents **\n");

  fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");

  for (i=0; i<STORE_HASH_SIZE; i++)
    {
      /* dump contents of all hash table buckets */
      for (ent=runcontext->store_htable[i]; ent; ent=ent->next)
	{
	  myfprintf(stream, "[0x%08p]: %12.0f/0x%08x:%08x\n",
		    ent->addr, (double)(*((double *)ent->data)),
		    *((unsigned int *)&ent->data[0]),
		    *(((unsigned int *)&ent->data[0]) + 1));
	}
    }
}
//char * TMIPS::simoo_mem_obj(
//			TMIPS &t,
//			struct mem_t *mem,		/* memory space to access */
//	      int is_write,			/* access type */
//	      md_addr_t addr,			/* address to access */
//	      char *p,				/* input/output buffer */
//	      int nbytes)			/* size of access */
//{
//  enum mem_cmd cmd;
//
//  if (!is_write)
//    cmd = Read;
//  else
//    cmd = Write;
//
//#if 0
//  char *errstr;
//
//  errstr = mem_valid(cmd, addr, nbytes, /* declare */FALSE);
//  if (errstr)
//    return errstr;
//#endif
//
//  /* else, no error, access memory */
//  if (spec_mode)
//    spec_mem_access(mem, cmd, addr, p, nbytes);
//  else
//    mem_access(mem, cmd, addr, p, nbytes);
//
//  /* no error */
//  return NULL;
//}
void TMIPS::ruu_link_idep(struct RUU_station *rs,		/* rs station to link */
	      int idep_num,			/* input dependence number */
	      int idep_name)			/* input register name */
{
  struct CV_link head;
  struct RS_link *link;

  /* any dependence? */
  if (idep_name == NA)
    {
      /* no input dependence for this input slot, mark operand as ready */
      rs->idep_ready[idep_num] = TRUE;
      return;
    }

  /* locate creator of operand */
  head = CREATE_VECTOR(idep_name);

  /* any creator? */
  if (!head.rs)
    {
      /* no active creator, use value available in architected reg file,
         indicate the operand is ready for use */
      rs->idep_ready[idep_num] = TRUE;
      return;
    }
  /* else, creator operation will make this value sometime in the future */

  /* indicate value will be created sometime in the future, i.e., operand
     is not yet ready for use */
  rs->idep_ready[idep_num] = FALSE;

  /* link onto creator's output list of dependant operand */
  RSLINK_NEW(link, rs); link->x.opnum = idep_num;
  link->next = head.rs->odep_list[head.odep_num];
  head.rs->odep_list[head.odep_num] = link;
}
void TMIPS::ruu_install_odep(struct RUU_station *rs,	/* creating RUU station */
		 int odep_num,			/* output operand number */
		 int odep_name)			/* output register name */
{
  struct CV_link cv;

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
  CVLINK_INIT(cv, rs, odep_num);
  SET_CREATE_VECTOR(odep_name, cv);
}
//char * TMIPS::simoo_reg_obj(
//			TMIPS &t,
//			struct regs_t *xregs,		/* registers to access */
//	      int is_write,			/* access type */
//	      enum md_reg_type rt,		/* reg bank to probe */
//	      int reg,				/* register number */
//	      struct eval_value_t *val)		/* input, output */
//{
//  switch (rt)
//    {
//    case rt_gpr:
//      if (reg < 0 || reg >= MD_NUM_IREGS)
//	return "register number out of range";
//
//      if (!is_write)
//	{
//	  val->type = et_uint;
//	  val->value.as_uint = GPR(reg);
//	}
//      else
//	SET_GPR(reg, eval_as_uint(*val));
//      break;
//
//    case rt_lpr:
//      if (reg < 0 || reg >= MD_NUM_FREGS)
//	return "register number out of range";
//
//      /* FIXME: this is not portable... */
//      abort();
//#if 0
//      if (!is_write)
//	{
//	  val->type = et_uint;
//	  val->value.as_uint = FPR_L(reg);
//	}
//      else
//	SET_FPR_L(reg, eval_as_uint(*val));
//#endif
//      break;
//
//    case rt_fpr:
//      /* FIXME: this is not portable... */
//      abort();
//#if 0
//      if (!is_write)
//	val->value.as_float = FPR_F(reg);
//      else
//	SET_FPR_F(reg, val->value.as_float);
//#endif
//      break;
//
//    case rt_dpr:
//      /* FIXME: this is not portable... */
//      abort();
//#if 0
//      /* 1/2 as many regs in this mode */
//      if (reg < 0 || reg >= MD_NUM_REGS/2)
//	return "register number out of range";
//
//      if (at == at_read)
//	val->as_double = FPR_D(reg * 2);
//      else
//	SET_FPR_D(reg * 2, val->as_double);
//#endif
//      break;
//
//      /* FIXME: this is not portable... */
//#if 0
//      abort();
//    case rt_hi:
//      if (at == at_read)
//	val->as_word = HI;
//      else
//	SET_HI(val->as_word);
//      break;
//    case rt_lo:
//      if (at == at_read)
//	val->as_word = LO;
//      else
//	SET_LO(val->as_word);
//      break;
//    case rt_FCC:
//      if (at == at_read)
//	val->as_condition = FCC;
//      else
//	SET_FCC(val->as_condition);
//      break;
//#endif
//
//    case rt_PC:
//      if (!is_write)
//	{
//	  val->type = et_addr;
//	  val->value.as_addr = regs->regs_PC;
//	}
//      else
//	regs->regs_PC = eval_as_addr(*val);
//      break;
//
//    case rt_NPC:
//      if (!is_write)
//	{
//	  val->type = et_addr;
//	  val->value.as_addr = regs->regs_NPC;
//	}
//      else
//	regs->regs_NPC = eval_as_addr(*val);
//      break;
//
//    default:
//      panic("bogus register bank");
//    }
//
//  /* no error */
//  return NULL;
//}
void TMIPS::ruu_dispatch(void)
{
  int i;
  int n_dispatched;			/* total insts dispatched */
  md_inst_t inst;			/* actual instruction bits */
  enum md_opcode op;			/* decoded opcode enum */
  int out1, out2, in1, in2, in3;	/* output/input register names */
  char *restype[] = {"intalu","memport","intmult","fpalu","fpmult"};
  md_addr_t target_PC;			/* actual next/target PC address */
  md_addr_t addr;			/* effective address, if load/store */
  struct RUU_station *rs;		/* RUU station being allocated */
  struct RUU_station *lsq;		/* LSQ station for ld/st's */
  struct bpred_update_t *dir_update_ptr;/* branch predictor dir update ptr */
  int stack_recover_idx;		/* bpred retstack recovery index */
  unsigned int pseq;			/* pipetrace sequence number */
  int is_write;				/* store? */
  int made_check;			/* used to ensure DLite entry */
  int br_taken, br_pred_taken;		/* if br, taken?  predicted taken? */
  int fetch_redirected = FALSE;
  byte_t temp_byte = 0;			/* temp variable for spec mem access */
  half_t temp_half = 0;			/* " ditto " */
  word_t temp_word = 0;			/* " ditto " */
#ifdef HOST_HAS_QWORD
  qword_t temp_qword = 0;		/* " ditto " */
#endif /* HOST_HAS_QWORD */
  enum md_fault_type fault;

  made_check = FALSE;
  n_dispatched = 0;
  while (/* instruction decode B/W left? */
	 n_dispatched < (ruu_decode_width * fetch_speed)
	 /* RUU and LSQ not full? */
	 && runcontext->RUU_num < RUU_size && runcontext->LSQ_num < LSQ_size//ruu_size=16 lsq_size=8 chenghao
	 /* insts still available from fetch unit? */
	 && runcontext->fetch_num != 0
	 /* on an acceptable trace path */
	 && (ruu_include_spec || !spec_mode))
    {
      /* if issuing in-order, block until last op issues if inorder issue */
      if (ruu_inorder_issue
	  && (runcontext->last_op.rs && RSLINK_VALID(&runcontext->last_op)
	      && !OPERANDS_READY(runcontext->last_op.rs)))
	{
	  /* stall until last operation is ready to issue */
	  break;
	}

      /* get the next instruction from the IFETCH -> DISPATCH queue */
      inst = runcontext->fetch_data[runcontext->fetch_head].IR;
      runcontext->regs->regs_PC = runcontext->fetch_data[runcontext->fetch_head].regs_PC;
      runcontext->pred_PC = runcontext->fetch_data[runcontext->fetch_head].pred_PC;
      dir_update_ptr = &(runcontext->fetch_data[runcontext->fetch_head].dir_update);
      stack_recover_idx = runcontext->fetch_data[runcontext->fetch_head].stack_recover_idx;
      pseq = runcontext->fetch_data[runcontext->fetch_head].ptrace_seq;

      /* decode the inst */
      MD_SET_OPCODE(op, inst);

      /* compute default next PC */
      runcontext->regs->regs_NPC = runcontext->regs->regs_PC + sizeof(md_inst_t);

      /* drain RUU for TRAPs and system calls */
      if (MD_OP_FLAGS(op) & F_TRAP)
	{
	  if (runcontext->RUU_num != 0)
	    break;

	  /* else, syscall is only instruction in the machine, at this
	     point we should not be in (mis-)speculative mode */
	  if (spec_mode)
	    panic("drained and speculative");
	}

      /* maintain $r0 semantics (in spec and non-spec space) */
      runcontext->regs->regs_R[MD_REG_ZERO] = 0;
      runcontext->spec_regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
      regs->regs_F.d[MD_REG_ZERO] = 0.0; spec_regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

      if (!spec_mode)
	{
	  /* one more non-speculative instruction executed */
	  sim_num_insn++;
	}

      /* default effective address (none) and access */
      addr = 0; is_write = FALSE;

      /* set default fault - none */
      fault = md_fault_none;

      /* more decoding and execution */
      switch (op)
	{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,CLASS,O1,O2,I1,I2,I3)		\
	case OP:											\
	  /* compute output/input dependencies to out1-2 and in1-3 */		\
	  out1 = O1; out2 = O2;						\
	  in1 = I1; in2 = I2; in3 = I3;			\
	  /*get res type --lch*/					\
      switch(RES){							\
      case IntALU:		thispower->gpp_data->core_para->total_ialu_num+=1;	\
      	  	  	  	  	  	thispower->gpp_data->core_para->total_res_call_num+=1;	\
      	  	  	  	  	  	allpower->gpp_data->core_para->total_ialu_num+=1; 	\
      	  	  	  	  	  	allpower->gpp_data->core_para->total_res_call_num+=1;	\
      	  	  	  	  	  	esynetrcd->add_integer++; break;			\
      case RdPort:		thispower->gpp_data->mem_para->total_memport_rd+=1;	\
      	  	  	  	  	  	allpower->gpp_data->mem_para->total_memport_rd+=1;		break;	\
      case WrPort:		thispower->gpp_data->mem_para->total_memport_wt+=1;	\
      	  	  	  	  	  	allpower->gpp_data->mem_para->total_memport_wt+=1;		break;	\
      case IntMULT:		thispower->gpp_data->core_para->total_imult_num+=1;	\
      	  	  	  	  	  	thispower->gpp_data->core_para->total_res_call_num+=1;	\
      	  	  	  	  	  	allpower->gpp_data->core_para->total_imult_num+=1;	\
      	  	  	  	  	  	allpower->gpp_data->core_para->total_res_call_num+=1;	\
      	  	  	  	  	  	esynetrcd->multi_integer+=1;break;		\
      case IntDIV:		thispower->gpp_data->core_para->total_idiv_num+=1;	\
     							thispower->gpp_data->core_para->total_res_call_num+=1; \
     							allpower->gpp_data->core_para->total_idiv_num+=1;	\
     							allpower->gpp_data->core_para->total_res_call_num+=1;break;		\
      case FloatADD:		thispower->gpp_data->core_para->total_fpalu_num+=1;	\
      	  	  	  	  	  	thispower->gpp_data->core_para->total_res_call_num+=1;	\
      	  	  	  	  	  	allpower->gpp_data->core_para->total_fpalu_num+=1;	\
      	  	  	  	  	  	allpower->gpp_data->core_para->total_res_call_num+=1;	\
      	  	  	  	  	  	esynetrcd->add_float+=1;		break;	\
      case FloatCMP:		break;	\
      case FloatCVT:		break;	\
      case FloatMULT:	thispower->gpp_data->core_para->total_fpmult_num+=1;	\
  	  	  						thispower->gpp_data->core_para->total_res_call_num+=1;	\
  	  	  						allpower->gpp_data->core_para->total_fpmult_num+=1;	\
  	  	  						allpower->gpp_data->core_para->total_res_call_num+=1;	\
  	  	  						esynetrcd->multi_float+=1;	break;	\
      case FloatDIV:		thispower->gpp_data->core_para->total_fpdiv_num+=1;	\
								thispower->gpp_data->core_para->total_res_call_num+=1;	\
								allpower->gpp_data->core_para->total_fpdiv_num+=1;	\
								allpower->gpp_data->core_para->total_res_call_num+=1;	\
								esynetrcd->multi_float+=1;	break;	\
      case FloatSQRT:	break;	\
      }\
	  /* execute the instruction */					\
	  SYMCAT(OP,_IMPL);						\
	  break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
	case OP:							\
	  /* could speculatively decode a bogus inst, convert to NOP */	\
	  op = MD_NOP_OP;						\
	  /* compute output/input dependencies to out1-2 and in1-3 */	\
	  out1 = NA; out2 = NA;						\
	  in1 = NA; in2 = NA; in3 = NA;					\
	  /* no EXPR */							\
	  break;
#define CONNECT(OP)	/* nada... */
	  /* the following macro wraps the instruction fault declaration macro
	     with a test to see if the trace generator is in non-speculative
	     mode, if so the instruction fault is declared, otherwise, the
	     error is shunted because instruction faults need to be masked on
	     the mis-speculated instruction paths */
#define DECLARE_FAULT(FAULT)						\
	  {								\
	    if (!spec_mode)						\
	      fault = (FAULT);						\
	    /* else, spec fault, ignore it, always terminate exec... */	\
	    break;							\
	  }
#include "pisa.def"
	default:
	  /* can speculatively decode a bogus inst, convert to a NOP */
	  op = MD_NOP_OP;
	  /* compute output/input dependencies to out1-2 and in1-3 */	\
	  out1 = NA; out2 = NA;
	  in1 = NA; in2 = NA; in3 = NA;
	  /* no EXPR */
	}
      /* operation sets next PC */

      /* print retirement trace if in verbose mode */
      if (!spec_mode && !verbose)
        {
//IS - LCH
//    	  myfprintf(stderr, "PISA -- node %d & sim_cycle = %d ++ %10n [xor: 0x%08x] {%d} @ 0x%08p: \n",
//                    actualid,sim_cycle,sim_num_insn, md_xor_regs(runcontext->regs),
//                   inst_seq+1, runcontext->regs->regs_PC);
//          fprintf(stderr,"ISA: PIS actualid = %d simulation cycle = %d,sim_cycle = %d \t\t\t [xor:0x%08x] {%d} \t\t\t @ 0x%08p\n",
//        		  actualid,simulation_cycle,sim_cycle,md_xor_regs(runcontext->regs),inst_seq+1,runcontext->regs->regs_PC);
//          md_print_insn(inst, runcontext->regs->regs_PC, stderr);
//          fprintf(stderr,"\n");
    	  //op记录于statisticsout
//    	  enum md_opcode op_esy;
//    	  MD_SET_OPCODE(op_esy, inst);
    	  md_print_insn(inst, runcontext->regs->regs_PC, progout);
    	  esynetrcd->inst = inst.a;
//          fprintf(progout," cycle = %d",sim_cycle);
//          fprintf(stderr, "\n");
//          fprintf(progout, "\n");
          /* fflush(stderr); */

        }

      if (fault != md_fault_none)
	fatal("non-speculative fault (%d) detected @ 0x%08p",
	      fault, runcontext->regs->regs_PC);

      /* update memory access stats */
      if (MD_OP_FLAGS(op) & F_MEM)
	{
	  sim_total_refs++;//内存引用执行就加
	  if (!spec_mode)
	    sim_num_refs++;//内存引用提交就加

	  if (MD_OP_FLAGS(op) & F_STORE){
		  is_write = TRUE;
		  thispower->gpp_data->mem_para->total_store_num++;
		  allpower->gpp_data->mem_para->total_store_num++;
		}
	  else
	    {
		  thispower->gpp_data->mem_para->total_load_num +=1;
		  allpower->gpp_data->mem_para->total_load_num +=1;
	     sim_total_loads++;
	     if (!spec_mode)
	    	 sim_num_loads++;
	    }
	}

      br_taken = (runcontext->regs->regs_NPC != (runcontext->regs->regs_PC + sizeof(md_inst_t)));
      br_pred_taken = (runcontext->pred_PC != (runcontext->regs->regs_PC + sizeof(md_inst_t)));

      if ((runcontext->pred_PC != runcontext->regs->regs_NPC && pref_mode)
	  || ((MD_OP_FLAGS(op) & (F_CTRL|F_DIRJMP)) == (F_CTRL|F_DIRJMP)
	      && target_PC != runcontext->pred_PC && br_pred_taken))
	{
	  /* Either 1) we're simulating perfect prediction and are in a
             mis-predict state and need to patch up, or 2) We're not simulating
             perfect prediction, we've predicted the branch taken, but our
             predicted target doesn't match the computed target (i.e.,
             mis-fetch).  Just update the PC values and do a fetch squash.
             This is just like calling fetch_squash() except we pre-anticipate
             the updates to the fetch values at the end of this function.  If
             case #2, also charge a mispredict penalty for redirecting fetch */
    	  runcontext->fetch_pred_PC = runcontext->fetch_regs_PC = runcontext->regs->regs_NPC;
	  /* was: if (pref_mode) */
	  if (pref_mode)
		  runcontext->pred_PC = runcontext->regs->regs_NPC;

	  runcontext->fetch_head = (ruu_ifq_size-1);
	  runcontext->fetch_num = 1;
	  runcontext->fetch_tail = 0;

	  if (!pref_mode)
	    ruu_fetch_issue_delay = ruu_branch_penalty;

	  fetch_redirected = TRUE;
	}

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
	  rs = &(runcontext->RUU[runcontext->RUU_tail]);
          rs->slip = sim_cycle - 1;
//	  rs->threadid = id;
	  rs->IR = inst;
	  rs->op = op;
	  rs->PC = runcontext->regs->regs_PC;
	  rs->next_PC = runcontext->regs->regs_NPC; rs->pred_PC = runcontext->pred_PC;
	  rs->in_LSQ = FALSE;
	  rs->ea_comp = FALSE;
	  rs->recover_inst = FALSE;
          rs->dir_update = *dir_update_ptr;
	  rs->stack_recover_idx = stack_recover_idx;
	  rs->spec_mode = spec_mode;
	  rs->addr = 0;
	  /* rs->tag is already set */
	  rs->seq = ++inst_seq;
	  rs->queued = rs->issued = rs->completed = FALSE;
	  rs->ptrace_seq = pseq;

	  /* split ld/st's into two operations: eff addr comp + mem access */
	  if (MD_OP_FLAGS(op) & F_MEM)
	    {
	      /* convert RUU operation from ld/st to an add (eff addr comp) */
	      rs->op = MD_AGEN_OP;
	      rs->ea_comp = TRUE;

	      /* fill in LSQ reservation station */
	      lsq = &(runcontext->LSQ[runcontext->LSQ_tail]);
              lsq->slip = sim_cycle - 1;
//	      lsq->threadid = id;
		  lsq->IR = inst;
	      lsq->op = op;
	      lsq->PC = runcontext->regs->regs_PC;
	      lsq->next_PC = runcontext->regs->regs_NPC; lsq->pred_PC = runcontext->pred_PC;
	      lsq->in_LSQ = TRUE;
	      lsq->ea_comp = FALSE;
	      lsq->recover_inst = FALSE;
	      lsq->dir_update.pdir1 = lsq->dir_update.pdir2 = NULL;
	      lsq->dir_update.pmeta = NULL;
	      lsq->stack_recover_idx = 0;
	      lsq->spec_mode = spec_mode;
	      lsq->addr = addr;
	      /* lsq->tag is already set */
	      lsq->seq = ++inst_seq;
	      lsq->queued = lsq->issued = lsq->completed = FALSE;
	      lsq->ptrace_seq = ptrace_seq++;

	      /* pipetrace this uop */
	      ptrace_newuop(lsq->ptrace_seq, "internal ld/st", lsq->PC, 0);
	      ptrace_newstage(lsq->ptrace_seq, PST_DISPATCH, 0);

	      /* link eff addr computation onto operand's output chains */
	      ruu_link_idep(rs, /* idep_ready[] index */0, NA);
	      ruu_link_idep(rs, /* idep_ready[] index */1, in2);
	      ruu_link_idep(rs, /* idep_ready[] index */2, in3);

	      /* install output after inputs to prevent self reference */
	      ruu_install_odep(rs, /* odep_list[] index */0, DTMP);
	      ruu_install_odep(rs, /* odep_list[] index */1, NA);

	      /* link memory access onto output chain of eff addr operation */
	      ruu_link_idep(lsq,
			    /* idep_ready[] index */STORE_OP_INDEX/* 0 */,
			    in1);
	      ruu_link_idep(lsq,
			    /* idep_ready[] index */STORE_ADDR_INDEX/* 1 */,
			    DTMP);
	      ruu_link_idep(lsq, /* idep_ready[] index */2, NA);

	      /* install output after inputs to prevent self reference */
	      ruu_install_odep(lsq, /* odep_list[] index */0, out1);
	      ruu_install_odep(lsq, /* odep_list[] index */1, out2);

	      /* install operation in the RUU and LSQ */
	      n_dispatched++;
	      runcontext->RUU_tail = (runcontext->RUU_tail + 1) % RUU_size;
	      runcontext->RUU_num++;
	      runcontext->LSQ_tail = (runcontext->LSQ_tail + 1) % LSQ_size;
	      runcontext->LSQ_num++;

	      if (OPERANDS_READY(rs))
		{
		  /* eff addr computation ready, queue it on ready list */
		  readyq_enqueue(rs);
		}
	      /* issue may continue when the load/store is issued */
	      RSLINK_INIT(runcontext->last_op, lsq);

	      /* issue stores only, loads are issued by lsq_refresh() */
	      if (((MD_OP_FLAGS(op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
		  && OPERANDS_READY(lsq))
		{
		  /* panic("store immediately ready"); */
		  /* put operation on ready list, ruu_issue() issue it later */
		  readyq_enqueue(lsq);
		}
	    }
	  else /* !(MD_OP_FLAGS(op) & F_MEM) */
	    {
	      /* link onto producing operation */
	      ruu_link_idep(rs, /* idep_ready[] index */0, in1);
	      ruu_link_idep(rs, /* idep_ready[] index */1, in2);
	      ruu_link_idep(rs, /* idep_ready[] index */2, in3);

	      /* install output after inputs to prevent self reference */
	      ruu_install_odep(rs, /* odep_list[] index */0, out1);
	      ruu_install_odep(rs, /* odep_list[] index */1, out2);

	      /* install operation in the RUU */
	      n_dispatched++;
	      runcontext->RUU_tail = (runcontext->RUU_tail + 1) % RUU_size;
	      runcontext->RUU_num++;

	      /* issue op if all its reg operands are ready (no mem input) */
	      if (OPERANDS_READY(rs))
		{
		  /* put operation on ready list, ruu_issue() issue it later */
		  readyq_enqueue(rs);
		  /* issue may continue */
		  runcontext->last_op = RSLINK_NULL;
		}
	      else
		{
		  /* could not issue this inst, stall issue until we can */
		  RSLINK_INIT(runcontext->last_op, rs);
		}
	    }
	}
      else
	{
	  /* this is a NOP, no need to update RUU/LSQ state */
    //printf("this is a nop\t,sim_cycle = %d\n",sim_cycle);
	  rs = NULL;
	}

      /* one more instruction executed, speculative or otherwise */
      sim_total_insn++;
      thispower->gpp_data->core_para->total_inst_num++;
      allpower->gpp_data->core_para->total_inst_num++;
      if (MD_OP_FLAGS(op) & F_CTRL){
		sim_total_branches++;
		thispower->gpp_data->core_para->total_branch_num++;
		allpower->gpp_data->core_para->total_branch_num++;
		}
      if (!spec_mode)
	{
#if 0 /* moved above for EIO trace file support */
	  /* one more non-speculative instruction executed */
	  sim_num_insn++;
#endif

	  /* if this is a branching instruction update BTB, i.e., only
	     non-speculative state is committed into the BTB */
	  if (MD_OP_FLAGS(op) & F_CTRL)
	    {
	      sim_num_branches++;
	      thispower->gpp_data->btb_para->total_btb_read++;
	      allpower->gpp_data->btb_para->total_btb_read++;
	      if (runcontext->pred && bpred_spec_update == spec_ID)
		{
		  bpred_update(runcontext->pred,
			       /* branch address */runcontext->regs->regs_PC,
			       /* actual target address */runcontext->regs->regs_NPC,
			       /* taken? */runcontext->regs->regs_NPC != (runcontext->regs->regs_PC +
						       sizeof(md_inst_t)),
			       /* pred taken? */runcontext->pred_PC != (runcontext->regs->regs_PC +
							sizeof(md_inst_t)),
			       /* correct pred? */runcontext->pred_PC == runcontext->regs->regs_NPC,
			       /* opcode */op,
			       /* predictor update ptr */&rs->dir_update);
		  thispower->gpp_data->btb_para->total_btb_write++;
		  allpower->gpp_data->btb_para->total_btb_write++;
		}
	    }

	  /* is the trace generator trasitioning into mis-speculation mode? */
	  if (runcontext->pred_PC != runcontext->regs->regs_NPC && !fetch_redirected)
	    {
	      /* entering mis-speculation mode, indicate this and save PC */
	      spec_mode = TRUE;
	      rs->recover_inst = TRUE;
	      runcontext->recover_PC = runcontext->regs->regs_NPC;
	    }
	}

      /* entered decode/allocate stage, indicate in pipe trace */
      ptrace_newstage(pseq, PST_DISPATCH,
		      (runcontext->pred_PC != runcontext->regs->regs_NPC) ? PEV_MPOCCURED : 0);
      if (op == MD_NOP_OP)
	{
	  /* end of the line */
	  ptrace_endinst(pseq);
	}

      /* update any stats tracked by PC */
      for (i=0; i<pcstat_nelt; i++)
	{
	  counter_t newval;
	  int delta;

	  /* check if any tracked stats changed */
	  newval = STATVAL(pcstat_stats[i]);
	  delta = newval - pcstat_lastvals[i];
	  if (delta != 0)
	    {
		  statcpnt->stat_add_samples(pcstat_sdists[i], runcontext->regs->regs_PC, delta);
	      pcstat_lastvals[i] = newval;
	    }
	}

      /* consume instruction from IFETCH -> DISPATCH queue */
      runcontext->fetch_head = (runcontext->fetch_head+1) & (ruu_ifq_size - 1);
      runcontext->fetch_num--;

      /* check for DLite debugger entry condition */
      made_check = TRUE;
//      if (dlite_check_break(pred_PC,
//			    is_write ? ACCESS_WRITE : ACCESS_READ,
//			    addr, sim_num_insn, sim_cycle))
//	dlite_main(regs->regs_PC, pred_PC, sim_cycle, regs, mem);
    }

  /* need to enter DLite at least once per cycle */
  if (!made_check)
    {
//      if (dlite_check_break(/* no next PC */0,
//			    is_write ? ACCESS_WRITE : ACCESS_READ,
//			    addr, sim_num_insn, sim_cycle))
//	dlite_main(regs->regs_PC, /* no next PC */0, sim_cycle, regs, mem);
    }
}
void TMIPS::fetch_init(void)
{
  /* allocate the IFETCH -> DISPATCH instruction queue */
  runcontext->fetch_data =
    (struct fetch_rec *)calloc(ruu_ifq_size, sizeof(struct fetch_rec));//ruu_ifq_size=4 chenghao
  if (!runcontext->fetch_data)
    fatal("out of virtual memory");

  runcontext->fetch_num = 0;
  runcontext->fetch_tail = runcontext->fetch_head = 0;
  IFQ_count = 0;
  IFQ_fcount = 0;
}
void TMIPS::fetch_dump(FILE *stream)			/* output stream */
{
  int num, head;

  if (!stream)
    stream = stderr;

  fprintf(stream, "** fetch stage state **\n");

  fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");
  myfprintf(stream, "pred_PC: 0x%08p, recover_PC: 0x%08p\n",
		  runcontext->pred_PC, runcontext->recover_PC);
  myfprintf(stream, "fetch_regs_PC: 0x%08p, fetch_pred_PC: 0x%08p\n",
		  runcontext->fetch_regs_PC, runcontext->fetch_pred_PC);
  fprintf(stream, "\n");

  fprintf(stream, "** fetch queue contents **\n");
  fprintf(stream, "fetch_num: %d\n", runcontext->fetch_num);
  fprintf(stream, "fetch_head: %d, fetch_tail: %d\n",
		  runcontext->fetch_head, runcontext->fetch_tail);

  num = runcontext->fetch_num;
  head = runcontext->fetch_head;
  while (num)
    {
      fprintf(stream, "idx: %2d: inst: `", head);
      md_print_insn(runcontext->fetch_data[head].IR, runcontext->fetch_data[head].regs_PC, stream);
      fprintf(stream, "'\n");
      myfprintf(stream, "         regs_PC: 0x%08p, pred_PC: 0x%08p\n",
    		  runcontext->fetch_data[head].regs_PC, runcontext->fetch_data[head].pred_PC);
      head = (head + 1) & (ruu_ifq_size - 1);
      num--;
    }
}
void TMIPS::ruu_fetch(void)
{
  int i, lat, tlb_lat, done = FALSE;
  md_inst_t inst;
  inst.a =inst.b = 0;
  int stack_recover_idx;
  int branch_cnt;

  for (i=0, branch_cnt=0;//每次4个
       /* fetch up to as many instruction as the DISPATCH stage can decode */
       i < (ruu_decode_width * fetch_speed)//4*1 chenghao
       /* fetch until IFETCH -> DISPATCH queue fills */
       && runcontext->fetch_num < ruu_ifq_size
       /* and no IFETCH blocking condition encountered */
       && !done;
       i++)
    {
      /* fetch an instruction at the next predicted fetch address */
	  runcontext->fetch_regs_PC = runcontext->fetch_pred_PC;

      /* is this a bogus text address? (can happen on mis-spec path) */
      if (ld_text_base <= runcontext->fetch_regs_PC
	  && runcontext->fetch_regs_PC < (runcontext->ld_text_base+runcontext->ld_text_size)
	  && !(runcontext->fetch_regs_PC & (sizeof(md_inst_t)-1)))
	{//进入
	  /* read instruction from memory */
	  MD_FETCH_INST(inst, runcontext->mem, runcontext->fetch_regs_PC);

	  /* address is within program text, read instruction from memory */
	  lat = cache_il1_lat;
	  if (runcontext->cache_il1)
	    {
	      /* access the I-cache */
//		  fputs("il1_cache_access:1 ",Simulation::visualFile);
		  esynetrcd->l1icache_access += 1;
		  esynetrcd->valid = 1;
		  lat =
		cache_access_il1(runcontext->cache_il1, Read, IACOMPRESS(runcontext->fetch_regs_PC),
			     NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
			     NULL, NULL);//lat=32
	      if (lat > cache_il1_lat)
		last_inst_missed = TRUE;
	    }

	  if (runcontext->itlb)
	    {
	      /* access the I-TLB, NOTE: this code will initiate
		 speculative TLB misses */
	      tlb_lat =
		cache_access(runcontext->itlb, Read, IACOMPRESS(runcontext->fetch_regs_PC),
			     NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
			     NULL, NULL);
	      if (tlb_lat > 1)
		last_inst_tmissed = TRUE;

	      /* I-cache/I-TLB accesses occur in parallel */
	      lat = MAX(tlb_lat, lat);
	    }

	  /* I-cache/I-TLB miss? assumes I-cache hit >= I-TLB hit */
	  if (lat != cache_il1_lat)
	    {
	      /* I-cache miss, block fetch until it is resolved */
	      ruu_fetch_issue_delay += lat - 1;
	      break;
	    }
	  /* else, I-cache/I-TLB hit */
	}
      else
	{
	  /* fetch PC is bogus, send a NOP down the pipeline */
	  inst = MD_NOP_INST;
	}

      /* have a valid inst, here */

      /* possibly use the BTB target *///BTB:分支目标缓冲
	  /*当分支预测器选定一条分支时，
	  分支目标缓冲（branch target buffer，BTB）就负责预测目标地址。*/
      if (runcontext->pred)
	{
	  enum md_opcode op;

	  /* pre-decode instruction, used for bpred stats recording */
	  MD_SET_OPCODE(op, inst);

	  /* get the next predicted fetch address; only use branch predictor
	     result for branches (assumes pre-decode bits); NOTE: returned
	     value may be 1 if bpred can only predict a direction */
	  if (MD_OP_FLAGS(op) & F_CTRL){
		  runcontext->fetch_pred_PC =
	      bpred_lookup(runcontext->pred,
			   /* branch address */runcontext->fetch_regs_PC,
			   /* target address *//* FIXME: not computed */0,
			   /* opcode */op,
			   /* call? */MD_IS_CALL(op),
			   /* return? */MD_IS_RETURN(op),
			   /* updt */&(runcontext->fetch_data[runcontext->fetch_tail].dir_update),
			   /* RSB index */&(stack_recover_idx));
		 thispower->gpp_data->btb_para->total_btb_read++;
		 allpower->gpp_data->btb_para->total_btb_read++;
			}
	  else//32cycle 进入
		  runcontext->fetch_pred_PC = 0;

	  /* valid address returned from branch predictor? */
	  if (!runcontext->fetch_pred_PC)
	    {
	      /* no predicted taken target, attempt not taken target */
		  runcontext->fetch_pred_PC = runcontext->fetch_regs_PC + sizeof(md_inst_t);
	    }
	  else
	    {
	      /* go with target, NOTE: discontinuous fetch, so terminate */
	      branch_cnt++;
	      if (branch_cnt >= fetch_speed)
		done = TRUE;
	    }
	}
      else
	{
	  /* no predictor, just default to predict not taken, and
	     continue fetching instructions linearly */
    	  runcontext->fetch_pred_PC = runcontext->fetch_regs_PC + sizeof(md_inst_t);
	}

      /* commit this instruction to the IFETCH -> DISPATCH queue */
      runcontext->fetch_data[runcontext->fetch_tail].IR = inst;
      runcontext->fetch_data[runcontext->fetch_tail].regs_PC = runcontext->fetch_regs_PC;
      runcontext->fetch_data[runcontext->fetch_tail].pred_PC = runcontext->fetch_pred_PC;
      runcontext->fetch_data[runcontext->fetch_tail].stack_recover_idx = stack_recover_idx;
      runcontext->fetch_data[runcontext->fetch_tail].ptrace_seq = ptrace_seq++;//轨迹序列

      /* for pipe trace */
      ptrace_newinst(runcontext->fetch_data[runcontext->fetch_tail].ptrace_seq,
		     inst, runcontext->fetch_data[runcontext->fetch_tail].regs_PC,
		     0);
      ptrace_newstage(runcontext->fetch_data[runcontext->fetch_tail].ptrace_seq,
		      PST_IFETCH,
		      ((last_inst_missed ? PEV_CACHEMISS : 0)
		       | (last_inst_tmissed ? PEV_TLBMISS : 0)));
      last_inst_missed = FALSE;
      last_inst_tmissed = FALSE;

      /* adjust instruction fetch queue */
      runcontext->fetch_tail = (runcontext->fetch_tail + 1) & (ruu_ifq_size - 1);
      runcontext->fetch_num++;
    }
}
//char * TMIPS::simoo_mstate_obj(
//			FILE *stream,			/* output stream */
//			char *cmd,			/* optional command string */
//			struct regs_t *regs,		/* registers to access */
//			struct mem_t *mem)		/* memory space to access */
//{
//  if (!cmd || !strcmp(cmd, "help"))
//    fprintf(stream,
//"mstate commands:\n"
//"\n"
//"    mstate help   - show all machine-specific commands (this list)\n"
//"    mstate stats  - dump all statistical variables\n"
//"    mstate res    - dump current functional unit resource states\n"
//"    mstate ruu    - dump contents of the register update unit\n"
//"    mstate lsq    - dump contents of the load/store queue\n"
//"    mstate eventq - dump contents of event queue\n"
//"    mstate readyq - dump contents of ready instruction queue\n"
//"    mstate cv     - dump contents of the register create vector\n"
//"    mstate rspec  - dump contents of speculative regs\n"
//"    mstate mspec  - dump contents of speculative memory\n"
//"    mstate fetch  - dump contents of fetch stage registers and fetch queue\n"
//"\n"
//	    );
//  else if (!strcmp(cmd, "stats"))
//    {
//      /* just dump intermediate stats */
////      sim_print_stats(stream);
//    }
//  else if (!strcmp(cmd, "res"))
//    {
//      /* dump resource state */
//      res_dump(runcontext->fu_pool, stream);
//    }
//  else if (!strcmp(cmd, "ruu"))
//    {
//      /* dump RUU contents */
//      ruu_dump(stream);
//    }
//  else if (!strcmp(cmd, "lsq"))
//    {
//      /* dump LSQ contents */
//      lsq_dump(stream);
//    }
//  else if (!strcmp(cmd, "eventq"))
//    {
//      /* dump event queue contents */
//      eventq_dump(stream);
//    }
//  else if (!strcmp(cmd, "readyq"))
//    {
//      /* dump event queue contents */
//      readyq_dump(stream);
//    }
//  else if (!strcmp(cmd, "cv"))
//    {
//      /* dump event queue contents */
//      cv_dump(stream);
//    }
//  else if (!strcmp(cmd, "rspec"))
//    {
//      /* dump event queue contents */
//      rspec_dump(stream);
//    }
//  else if (!strcmp(cmd, "mspec"))
//    {
//      /* dump event queue contents */
//      mspec_dump(stream);
//    }
//  else if (!strcmp(cmd, "fetch"))
//    {
//      /* dump event queue contents */
//      fetch_dump(stream);
//    }
//  else
//    return "unknown mstate command";
//
//  /* no error */
//  return NULL;
//}

/*sim-outorder.cc end*/

/*memory.cc*/
void TMIPS::mem_create(const char *name)			/* name of the memory space */
{
  runcontext->mem = (struct mem_t *)calloc(1, sizeof(struct mem_t));
  if (!runcontext->mem)
    fatal("out of virtual memory");
  runcontext->mem->name = mystrdup(name);
}

/* translate address ADDR in memory space MEM, returns pointer to host page */
byte_t * TMIPS::mem_translate(struct mem_t *mem,	/* memory space to access */
	      qword_t addr)		/* virtual address to translate */
{
  struct mem_pte_t *pte, *prev;

  /* got here via a first level miss in the page tables */
  mem->ptab_misses++; mem->ptab_accesses++;

  /* locate accessed PTE */
  for (prev=NULL, pte=mem->ptab[MEM_PTAB_SET(addr)];
       pte != NULL;
       prev=pte, pte=pte->next)
    {
      if (pte->tag == MEM_PTAB_TAG(addr))
	{
	  /* move this PTE to head of the bucket list */
	  if (prev)
	    {
	      prev->next = pte->next;
	      pte->next = mem->ptab[MEM_PTAB_SET(addr)];
	      mem->ptab[MEM_PTAB_SET(addr)] = pte;
	    }
	  return pte->page;
	}
    }

  /* no translation found, return NULL */
  return NULL;
}

/* allocate a memory page */
void TMIPS::mem_newpage(struct mem_t *mem,		/* memory space to allocate in */
	    qword_t addr)		/* virtual address to allocate */
{
  byte_t *page;
  struct mem_pte_t *pte;

  /* see misc.c for details on the getcore() function */
  page = (byte_t *)getcore(MD_PAGE_SIZE);
  if (!page)
    fatal("out of virtual memory");

  /* generate a new PTE */
  pte = (struct mem_pte_t *)calloc(1, sizeof(struct mem_pte_t));
  if (!pte)
    fatal("out of virtual memory");
  pte->tag = MEM_PTAB_TAG(addr);
  pte->page = page;

  /* insert PTE into inverted hash table */
  pte->next = mem->ptab[MEM_PTAB_SET(addr)];
  mem->ptab[MEM_PTAB_SET(addr)] = pte;

  /* one more page allocated */
  mem->page_count++;
}

/* generic memory access function, it's safe because alignments and permissions
   are checked, handles any natural transfer sizes; note, faults out if nbytes
   is not a power-of-two or larger then MD_PAGE_SIZE */
enum TMIPS::md_fault_type TMIPS::mem_access(struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   qword_t addr,		/* target address to access */
	   void *vp,			/* host memory address to access */
	   int nbytes)			/* number of bytes to access */
{
  byte_t *p = (byte_t *)vp;

  /* check alignments */
  if (/* check size */(nbytes & (nbytes-1)) != 0
      || /* check max size */nbytes > MD_PAGE_SIZE)
    return md_fault_access;

  if (/* check natural alignment */(addr & (nbytes-1)) != 0)
    return md_fault_alignment;

  /* perform the copy */
  {
    if (cmd == Read)
      {
	while (nbytes-- > 0)
	  {
	    *((byte_t *)p) = MEM_READ_BYTE(mem, addr);
	    p += sizeof(byte_t);
	    addr += sizeof(byte_t);
	  }
      }
    else
      {
	while (nbytes-- > 0)
	  {
	    MEM_WRITE_BYTE(mem, addr, *((byte_t *)p));
/*	    ((!( ((mem)->ptab[((((qword_t)(addr)) >> 12) & ((32*1024) - 1))]
						  && (mem)->ptab[((((qword_t)(addr)) >> 12) & ((32*1024) - 1))]->tag
						  == (((qword_t)(addr)) >> (12 + 15))) ?
								  ( (mem)->ptab_accesses++, (mem)->ptab[((((qword_t)(addr)) >> 12) & ((32*1024) - 1))]->page) :
								  ( mem_translate((mem), ((qword_t)(addr))))) ? ( mem_newpage(mem, (qword_t)(addr))) :
										  ( (void)0)), *((byte_t *)(( ((mem)->ptab[((((qword_t)(addr)) >> 12) & ((32*1024) - 1))]
																				   && (mem)->ptab[((((qword_t)(addr)) >> 12) & ((32*1024) - 1))]->tag
																				   == (((qword_t)(addr)) >> (12 + 15))) ?
																						   ( (mem)->ptab_accesses++, (mem)->ptab[((((qword_t)(addr)) >> 12) & ((32*1024) - 1))]->page) :
																						   	   ( mem_translate((mem), ((qword_t)(addr))))) + ((addr) & (4096 - 1)))) = (*((byte_t *)p)));
*/
	    p += sizeof(byte_t);
	    addr += sizeof(byte_t);
	  }
      }
  }

#if 0
  switch (nbytes)
    {
    case 1:
      if (cmd == Read)
	*((byte_t *)p) = MEM_READ_BYTE(mem, addr);
      else
	MEM_WRITE_BYTE(mem, addr, *((byte_t *)p));
      break;

    case 2:
      if (cmd == Read)
	*((half_t *)p) = MEM_READ_HALF(mem, addr);
      else
	MEM_WRITE_HALF(mem, addr, *((half_t *)p));
      break;

    case 4:
      if (cmd == Read)
	*((word_t *)p) = MEM_READ_WORD(mem, addr);
      else
	MEM_WRITE_WORD(mem, addr, *((word_t *)p));
      break;

#ifdef HOST_HAS_QWORD
    case 8:
      if (cmd == Read)
	*((qword_t *)p) = MEM_READ_QWORD(mem, addr);
      else
	MEM_WRITE_QWORD(mem, addr, *((qword_t *)p));
      break;
#endif /* HOST_HAS_QWORD */

    default:
      break;
    }
#endif

  /* no fault... */
  return md_fault_none;
}

///* register memory system-specific statistics */
//void TMIPS::mem_reg_stats(struct mem_t *mem,	/* memory space to declare */
//	      struct stat_sdb_t *sdb)	/* stats data base */
//{
//  char buf[512], buf1[512];
//
//  sprintf(buf, "%s.page_count", mem->name);
//  stat_reg_counter(sdb, buf, "total number of pages allocated",
//		   &mem->page_count, mem->page_count, NULL);
//
//  sprintf(buf, "%s.page_mem", mem->name);
//  sprintf(buf1, "%s.page_count * %d / 1024", mem->name, MD_PAGE_SIZE);
//  stat_reg_formula(sdb, buf, "total size of memory pages allocated",
//		   buf1, "%11.0fk");
//
//  sprintf(buf, "%s.ptab_misses", mem->name);
//  stat_reg_counter(sdb, buf, "total first level page table misses",
//		   &mem->ptab_misses, mem->ptab_misses, NULL);
//
//  sprintf(buf, "%s.ptab_accesses", mem->name);
//  stat_reg_counter(sdb, buf, "total page table accesses",
//		   &mem->ptab_accesses, mem->ptab_accesses, NULL);
//
//  sprintf(buf, "%s.ptab_miss_rate", mem->name);
//  sprintf(buf1, "%s.ptab_misses / %s.ptab_accesses", mem->name, mem->name);
//  stat_reg_formula(sdb, buf, "first level page table miss rate", buf1, NULL);
//}

/* initialize memory system, call before loader.c */
void TMIPS::mem_init(void)	/* memory space to initialize */
{
  int i;

  /* initialize the first level page table to all empty */
  for (i=0; i < MEM_PTAB_SIZE; i++)
    runcontext->mem->ptab[i] = NULL;
  runcontext->mem->page_count = 0;
  runcontext->mem->ptab_misses = 0;
  runcontext->mem->ptab_accesses = 0;
}

/* dump a block of memory, returns any faults encountered */
enum TMIPS::md_fault_type TMIPS::mem_dump(struct mem_t *mem,		/* memory space to display */
	 qword_t addr,		/* target address to dump */
	 int len,			/* number bytes to dump */
	 FILE *stream)			/* output stream */
{
  int data;
  enum md_fault_type fault;

  if (!stream)
    stream = stderr;

  addr &= ~sizeof(word_t);
  len = (len + (sizeof(word_t) - 1)) & ~sizeof(word_t);
  while (len-- > 0)
    {
      fault = mem_access(mem, Read, addr, &data, sizeof(word_t));
      if (fault != md_fault_none)
	return fault;

      myfprintf(stream, "0x%08p: %08x\n", addr, data);
      addr += sizeof(word_t);
    }

  /* no faults... */
  return md_fault_none;
}

/* copy a '\0' terminated string to/from simulated memory space, returns
   the number of bytes copied, returns any fault encountered */
enum TMIPS::md_fault_type TMIPS::mem_strcpy(mem_access_fn mem_fn,	/* user-specified memory accessor */
	   struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   qword_t addr,		/* target address to access */
	   char *s)
{
  int n = 0;
  char c;
  enum md_fault_type fault;

  switch (cmd)
    {
    case Read:
      /* copy until string terminator ('\0') is encountered */
      do {
	fault = (this->*mem_fn)(mem, Read, addr++, &c, 1);
	if (fault != md_fault_none)
	  return fault;
	*s++ = c;
	n++;
      } while (c);
      break;

    case Write:
      /* copy until string terminator ('\0') is encountered */
      do {
	c = *s++;
	fault = (this->*mem_fn)(mem, Write, addr++, &c, 1);
	if (fault != md_fault_none)
	  return fault;
	n++;
      } while (c);
      break;

    default:
      return md_fault_internal;
  }

  /* no faults... */
  return md_fault_none;
}

/* copy NBYTES to/from simulated memory space, returns any faults */
enum TMIPS::md_fault_type TMIPS::mem_bcopy(mem_access_fn mem_fn,		/* user-specified memory accessor */
	  struct mem_t *mem,		/* memory space to access */
	  enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	  qword_t addr,		/* target address to access */
	  void *vp,			/* host memory address to access */
	  int nbytes)
{
  byte_t *p = (byte_t *)vp;
  enum md_fault_type fault;

  /* copy NBYTES bytes to/from simulator memory */
  while (nbytes-- > 0)
    {
      fault = (this->*mem_fn)(mem, cmd, addr++, p++, 1);
      if (fault != md_fault_none)
	return fault;
    }

  /* no faults... */
  return md_fault_none;
}

/* copy NBYTES to/from simulated memory space, NBYTES must be a multiple
   of 4 bytes, this function is faster than mem_bcopy(), returns any
   faults encountered */
enum TMIPS::md_fault_type TMIPS::mem_bcopy4(mem_access_fn mem_fn,	/* user-specified memory accessor */
	   struct mem_t *mem,		/* memory space to access */
	   enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	   qword_t addr,		/* target address to access */
	   void *vp,			/* host memory address to access */
	   int nbytes)
{
  byte_t *p = (byte_t *)vp;
  int words = nbytes >> 2;		/* note: nbytes % 2 == 0 is assumed */
  enum md_fault_type fault;

  while (words-- > 0)
    {
      fault = (this->*mem_fn)(mem, cmd, addr, p, sizeof(word_t));
      if (fault != md_fault_none)
	return fault;

      addr += sizeof(word_t);
      p += sizeof(word_t);
    }

  /* no faults... */
  return md_fault_none;
}

/* zero out NBYTES of simulated memory, returns any faults encountered */
enum TMIPS::md_fault_type TMIPS::mem_bzero(mem_access_fn mem_fn,		/* user-specified memory accessor */
	  struct mem_t *mem,		/* memory space to access */
	  qword_t addr,		/* target address to access */
	  int nbytes)
{
  byte_t c = 0;
  enum md_fault_type fault;

  /* zero out NBYTES of simulator memory */
  while (nbytes-- > 0)
    {
      fault = (this->*mem_fn)(mem, Write, addr++, &c, 1);
      if (fault != md_fault_none)
	return fault;
    }

  /* no faults... */
  return md_fault_none;
}
/*end of memory.cc*/
/*loader.cc*/

void TMIPS::ld_reg_stats()	/* stats data base */
{
	statcpnt->stat_reg_addr("ld_text_base_t",
		"program text (code) segment base",
		&ld_text_base, ld_text_base, "  0x%08p");
	statcpnt->stat_reg_uint("ld_text_size_t",
		"program text (code) size in bytes",
		&ld_text_size, ld_text_size, NULL);
	statcpnt->stat_reg_addr("ld_data_base_t",
		"program initialized data segment base",
		&ld_data_base, ld_data_base, "  0x%08p");
	statcpnt->stat_reg_uint("ld_data_size_t",
		"program init'ed `.data' and uninit'ed `.bss' size in bytes",
		&ld_data_size, ld_data_size, NULL);
	statcpnt->stat_reg_addr("ld_stack_base_t",
		"program stack segment base (highest address in stack)",
		&ld_stack_base, ld_stack_base, "  0x%08p");
	statcpnt->stat_reg_uint("ld_stack_size_t",
		"program initial stack size",
		&ld_stack_size, ld_stack_size, NULL);
#if 0 /* FIXME: broken... */
  stat_reg_addr(sdb, "ld_stack_min",
		"program stack segment lowest address",
		&ld_stack_min, ld_stack_min, "  0x%08p");
#endif
  statcpnt->stat_reg_addr("ld_prog_entry_t",
		"program entry point (initial PC)",
		&ld_prog_entry, ld_prog_entry, "  0x%08p");
  statcpnt->stat_reg_addr("ld_environ_base_t",
		"program environment base address address",
		&ld_environ_base, ld_environ_base, "  0x%08p");
  statcpnt->stat_reg_int("ld_target_big_endian_t",
	       "target executable endian-ness, non-zero if big endian",
	       &ld_target_big_endian, ld_target_big_endian, NULL);
}
void TMIPS::ld_load_prog(char *fname,		/* program to load */
	     int argc, char **argv,	/* simulated program cmd line args */
	     char **envp,		/* simulated program environment */
	     struct regs_t *regs,	/* registers to initialize for load */
	     struct mem_t *mem,		/* memory space to load prog into */
	     int zero_bss_segs)		/* zero uninit data segment? */
{
  int i;
  word_t temp;
  md_addr_t sp, data_break = 0, null_ptr = 0, argv_addr, envp_addr;

  if (eio_valid(fname))
    {
      if (argc != 1)
	{
	  fprintf(stderr, "error: EIO file has arguments\n");
	  exit(1);
	}

      fprintf(stderr, "sim: loading EIO file: %s\n", fname);

      sim_eio_fname = mystrdup(fname);

      /* open the EIO file stream */
      sim_eio_fd = eio_open(fname);

      /* load initial state checkpoint */
      if (eio_read_chkpt(regs, mem, sim_eio_fd) != -1)
	fatal("bad initial checkpoint in EIO file");

      /* load checkpoint? */
      if (sim_chkpt_fname != NULL)
	{
	  counter_t restore_icnt;

	  FILE *chkpt_fd;

	  fprintf(stderr, "sim: loading checkpoint file: %s\n",
		  sim_chkpt_fname);

	  if (!eio_valid(sim_chkpt_fname))
	    fatal("file `%s' does not appear to be a checkpoint file",
		  sim_chkpt_fname);

	  /* open the checkpoint file */
	  chkpt_fd = eio_open(sim_chkpt_fname);

	  /* load the state image */
	  restore_icnt = eio_read_chkpt(regs, mem, chkpt_fd);

	  /* fast forward the baseline EIO trace to checkpoint location */
	  myfprintf(stderr, "sim: fast forwarding to instruction %n\n",
		    restore_icnt);
	  eio_fast_forward(sim_eio_fd, restore_icnt);
	}

      /* computed state... */
      ld_environ_base = regs->regs_R[MD_REG_SP];
      ld_prog_entry = regs->regs_PC;

      /* fini... */
      return;
    }
#ifdef MD_CROSS_ENDIAN
  else
    {
      fatal("SimpleScalar/PISA only supports binary execution on\n"
	    "       same-endian hosts, use EIO files on cross-endian hosts");
    }
#endif /* MD_CROSS_ENDIAN */


  if (sim_chkpt_fname != NULL)
    fatal("checkpoints only supported while EIO tracing");

#ifdef BFD_LOADER

  {
    bfd *abfd;
    asection *sect;

    /* set up a local stack pointer, this is where the argv and envp
       data is written into program memory */
    ld_stack_base = MD_STACK_BASE;
    sp = ROUND_DOWN(MD_STACK_BASE - MD_MAX_ENVIRON, sizeof(dfloat_t));
    ld_stack_size = ld_stack_base - sp;

    /* initial stack pointer value */
    ld_environ_base = sp;

    /* load the program into memory, try both endians */
    if (!(abfd = bfd_openr(argv[0], "ss-coff-big")))
      if (!(abfd = bfd_openr(argv[0], "ss-coff-little")))
	fatal("cannot open executable `%s'", argv[0]);

    /* this call is mainly for its side effect of reading in the sections.
       we follow the traditional behavior of `strings' in that we don't
       complain if we don't recognize a file to be an object file.  */
    if (!bfd_check_format(abfd, bfd_object))
      {
	bfd_close(abfd);
	fatal("cannot open executable `%s'", argv[0]);
      }

    /* record profile file name */
    ld_prog_fname = argv[0];

    /* record endian of target */
    ld_target_big_endian = abfd->xvec->byteorder_big_p;

    debug("processing %d sections in `%s'...",
	  bfd_count_sections(abfd), argv[0]);

    /* read all sections in file */
    for (sect=abfd->sections; sect; sect=sect->next)
      {
	char *p;

	debug("processing section `%s', %d bytes @ 0x%08x...",
	      bfd_section_name(abfd, sect), bfd_section_size(abfd, sect),
	      bfd_section_vma(abfd, sect));

	/* read the section data, if allocated and loadable and non-NULL */
	if ((bfd_get_section_flags(abfd, sect) & SEC_ALLOC)
	    && (bfd_get_section_flags(abfd, sect) & SEC_LOAD)
	    && bfd_section_vma(abfd, sect)
	    && bfd_section_size(abfd, sect))
	  {
	    /* allocate a section buffer */
	    p = calloc(bfd_section_size(abfd, sect), sizeof(char));
	    if (!p)
	      fatal("cannot allocate %d bytes for section `%s'",
		    bfd_section_size(abfd, sect),
		    bfd_section_name(abfd, sect));

	    if (!bfd_get_section_contents(abfd, sect, p, (file_ptr)0,
					  bfd_section_size(abfd, sect)))
	      fatal("could not read entire `%s' section from executable",
		    bfd_section_name(abfd, sect));

	    /* copy program section it into simulator target memory */
	    mem_bcopy(mem_access, mem, Write, bfd_section_vma(abfd, sect),
		      p, bfd_section_size(abfd, sect));

	    /* release the section buffer */
	    free(p);
	  }
	/* zero out the section if it is loadable but not allocated in exec */
	else if (zero_bss_segs
		 && (bfd_get_section_flags(abfd, sect) & SEC_LOAD)
		 && bfd_section_vma(abfd, sect)
		 && bfd_section_size(abfd, sect))
	  {
	    /* zero out the section region */
	    mem_bzero(mem_access, mem,
		      bfd_section_vma(abfd, sect),
		      bfd_section_size(abfd, sect));
	  }
	else
	  {
	    /* else do nothing with this section, it's probably debug data */
	    debug("ignoring section `%s' during load...",
		  bfd_section_name(abfd, sect));
	  }

	/* expected text section */
	if (!strcmp(bfd_section_name(abfd, sect), ".text"))
	  {
	    /* .text section processing */
	    ld_text_size =
	      ((bfd_section_vma(abfd, sect) + bfd_section_size(abfd, sect))
	       - MD_TEXT_BASE)
		+ /* for speculative fetches/decodes */TEXT_TAIL_PADDING;

	    /* create tail padding and copy into simulator target memory */
	    mem_bzero(mem_access, mem,
		      bfd_section_vma(abfd, sect)
		      + bfd_section_size(abfd, sect),
		      TEXT_TAIL_PADDING);
	  }
	/* expected data sections */
	else if (!strcmp(bfd_section_name(abfd, sect), ".rdata")
		 || !strcmp(bfd_section_name(abfd, sect), ".data")
		 || !strcmp(bfd_section_name(abfd, sect), ".sdata")
		 || !strcmp(bfd_section_name(abfd, sect), ".bss")
		 || !strcmp(bfd_section_name(abfd, sect), ".sbss"))
	  {
	    /* data section processing */
	    if (bfd_section_vma(abfd, sect) + bfd_section_size(abfd, sect) >
		data_break)
	      data_break = (bfd_section_vma(abfd, sect) +
			    bfd_section_size(abfd, sect));
	  }
	else
	  {
	    /* what is this section??? */
	    fatal("encountered unknown section `%s', %d bytes @ 0x%08x",
		  bfd_section_name(abfd, sect), bfd_section_size(abfd, sect),
		  bfd_section_vma(abfd, sect));
	  }
      }

    /* compute data segment size from data break point */
    ld_text_base = MD_TEXT_BASE;
    ld_data_base = MD_DATA_BASE;
    ld_prog_entry = bfd_get_start_address(abfd);
    ld_data_size = data_break - ld_data_base;

    /* done with the executable, close it */
    if (!bfd_close(abfd))
      fatal("could not close executable `%s'", argv[0]);
  }

#else /* !BFD_LOADER, i.e., standalone loader */

  {
    FILE *fobj;
    long floc;
    struct ecoff_filehdr fhdr;
    struct ecoff_aouthdr ahdr;
    struct ecoff_scnhdr shdr;

    /* set up a local stack pointer, this is where the argv and envp
       data is written into program memory */
    ld_stack_base = MD_STACK_BASE;
    sp = ROUND_DOWN(MD_STACK_BASE - MD_MAX_ENVIRON, sizeof(dfloat_t));
    ld_stack_size = ld_stack_base - sp;

    /* initial stack pointer value */
    ld_environ_base = sp;

    /* record profile file name */
    ld_prog_fname = argv[0];

    /* load the program into memory, try both endians */
#if defined(__CYGWIN32__) || defined(_MSC_VER)
    fobj = fopen(argv[0], "rb");
#else
    fobj = fopen(argv[0], "r");
#endif
    if (!fobj)
      fatal("cannot open executable `%s'", argv[0]);

    if (fread(&fhdr, sizeof(struct ecoff_filehdr), 1, fobj) < 1)
      fatal("cannot read header from executable `%s'", argv[0]);

    /* record endian of target */
    if (fhdr.f_magic == ECOFF_EB_MAGIC)
      ld_target_big_endian = TRUE;
    else if (fhdr.f_magic == ECOFF_EL_MAGIC)//√
      ld_target_big_endian = FALSE;
    else if (fhdr.f_magic == ECOFF_EB_OTHER || fhdr.f_magic == ECOFF_EL_OTHER)
      fatal("PISA binary `%s' has wrong endian format", argv[0]);
    else if (fhdr.f_magic == ECOFF_ALPHAMAGIC)
      fatal("PISA simulator cannot run Alpha binary `%s'", argv[0]);
    else
      fatal("bad magic number in executable `%s' (not an executable?)",
	    argv[0]);

    if (fread(&ahdr, sizeof(struct ecoff_aouthdr), 1, fobj) < 1)
      fatal("cannot read AOUT header from executable `%s'", argv[0]);

    data_break = MD_DATA_BASE + ahdr.dsize + ahdr.bsize;

#if 0
    Data_start = ahdr.data_start;
    Data_size = ahdr.dsize;
    Bss_size = ahdr.bsize;
    Bss_start = ahdr.bss_start;
    Gp_value = ahdr.gp_value;
    Text_entry = ahdr.entry;
#endif

    /* seek to the beginning of the first section header, the file header comes
       first, followed by the optional header (this is the aouthdr), the size
       of the aouthdr is given in Fdhr.f_opthdr */
    fseek(fobj, sizeof(struct ecoff_filehdr) + fhdr.f_opthdr, 0);

//    debug("processing %d sections in `%s'...", fhdr.f_nscns, argv[0]);
    printf("processing %d sections in `%s'...\n", fhdr.f_nscns, argv[0]);
    /* loop through the section headers */
    floc = ftell(fobj);
    for (i = 0; i < fhdr.f_nscns; i++)
      {
	char *p;

	if (fseek(fobj, floc, 0) == -1)
	  fatal("could not reset location in executable");
	if (fread(&shdr, sizeof(struct ecoff_scnhdr), 1, fobj) < 1)
	  fatal("could not read section %d from executable", i);
	floc = ftell(fobj);

	switch (shdr.s_flags)
	  {
	  case ECOFF_STYP_TEXT:
	    ld_text_size = ((shdr.s_vaddr + shdr.s_size) - MD_TEXT_BASE)
	      + TEXT_TAIL_PADDING;

	    p = (char *)calloc(shdr.s_size, sizeof(char));
	    if (!p)
	      fatal("out of virtual memory");

	    if (fseek(fobj, shdr.s_scnptr, 0) == -1)
	      fatal("could not read `.text' from executable", i);
	    if (fread(p, shdr.s_size, 1, fobj) < 1)
	      fatal("could not read text section from executable");

	    /*p读入文件，用于track指令对应C的行*/
	    int ipat; char* pat; int* ircd; sword_t fv_addr;
	    pat = p; fv_addr = shdr.s_vaddr;
	    for(ipat=0;ipat<shdr.s_size;ipat+=8,fv_addr+=8){
	    	ircd = (int*)(&(pat[ipat]));
//	    	printf("%x %d\n",*ircd,fv_addr);
	    	fprintf(inststreamout,"%02x %d\n",*ircd,fv_addr);
	    }
	    fflush(inststreamout);
	    fclose(inststreamout);


	    /* copy program section into simulator target memory */
	    mem_bcopy(&TMIPS::mem_access, mem, Write, shdr.s_vaddr, p, shdr.s_size);

	    /* create tail padding and copy into simulator target memory */
	    mem_bzero(&TMIPS::mem_access, mem,
		      shdr.s_vaddr + shdr.s_size, TEXT_TAIL_PADDING);

	    /* release the section buffer */
	    free(p);

#if 0
	    Text_seek = shdr.s_scnptr;
	    Text_start = shdr.s_vaddr;
	    Text_size = shdr.s_size / 4;
	    /* there is a null routine after the supposed end of text */
	    Text_size += 10;
	    Text_end = Text_start + Text_size * 4;
	    /* create_text_reloc(shdr.s_relptr, shdr.s_nreloc); */
#endif
	    break;

	  case ECOFF_STYP_RDATA:
	    /* The .rdata section is sometimes placed before the text
	     * section instead of being contiguous with the .data section.
	     */
#if 0
	    Rdata_start = shdr.s_vaddr;
	    Rdata_size = shdr.s_size;
	    Rdata_seek = shdr.s_scnptr;
#endif
	    /* fall through */
	  case ECOFF_STYP_DATA:
#if 0
	    Data_seek = shdr.s_scnptr;
#endif
	    /* fall through */
	  case ECOFF_STYP_SDATA:
#if 0
	    Sdata_seek = shdr.s_scnptr;
#endif

	    p = (char *)calloc(shdr.s_size, sizeof(char));
	    if (!p)
	      fatal("out of virtual memory");

	    if (fseek(fobj, shdr.s_scnptr, 0) == -1)
	      fatal("could not read `.text' from executable", i);
	    if (fread(p, shdr.s_size, 1, fobj) < 1)
	      fatal("could not read text section from executable");

	    /* copy program section it into simulator target memory */
	    mem_bcopy(&TMIPS::mem_access, mem, Write, shdr.s_vaddr, p, shdr.s_size);

	    /* release the section buffer */
	    free(p);

	    break;

	  case ECOFF_STYP_BSS:
	    break;

	  case ECOFF_STYP_SBSS:
	    break;
	  }
      }

    /* compute data segment size from data break point */
    ld_text_base = MD_TEXT_BASE;
    ld_data_base = MD_DATA_BASE;
    ld_prog_entry = ahdr.entry;
    ld_data_size = data_break - ld_data_base;

    /* done with the executable, close it */
    if (fclose(fobj))
      fatal("could not close executable `%s'", argv[0]);
  }
#endif /* BFD_LOADER */

  /* perform sanity checks on segment ranges */
  if (!ld_text_base || !ld_text_size)
    fatal("executable is missing a `.text' section");
  if (!ld_data_base || !ld_data_size)
    fatal("executable is missing a `.data' section");
  if (!ld_prog_entry)
    fatal("program entry point not specified");

  /* determine byte/words swapping required to execute on this host */
  sim_swap_bytes = (endian_host_byte_order() != endian_target_byte_order(this));
  if (sim_swap_bytes)
    {
#if 0 /* FIXME: disabled until further notice... */
      /* cross-endian is never reliable, why this is so is beyond the scope
	 of this comment, e-mail me for details... */
      fprintf(stderr, "sim: *WARNING*: swapping bytes to match host...\n");
      fprintf(stderr, "sim: *WARNING*: swapping may break your program!\n");
#else
      fatal("binary endian does not match host endian");
#endif
    }
  sim_swap_words = (endian_host_word_order() != endian_target_word_order(this));
  if (sim_swap_words)
    {
#if 0 /* FIXME: disabled until further notice... */
      /* cross-endian is never reliable, why this is so is beyond the scope
	 of this comment, e-mail me for details... */
      fprintf(stderr, "sim: *WARNING*: swapping words to match host...\n");
      fprintf(stderr, "sim: *WARNING*: swapping may break your program!\n");
#else
      fatal("binary endian does not match host endian");
#endif
    }

  /* write [argc] to stack */
  temp = MD_SWAPW(argc);
  mem_access(mem, Write, sp, &temp, sizeof(word_t));
  sp += sizeof(word_t);

  /* skip past argv array and NULL */
  argv_addr = sp;
  sp = sp + (argc + 1) * sizeof(md_addr_t);

  /* save space for envp array and NULL */
  envp_addr = sp;
  for (i=0; envp[i]; i++)
    sp += sizeof(md_addr_t);
  sp += sizeof(md_addr_t);

  /* fill in the argv pointer array and data */
  for (i=0; i<argc; i++)
    {
      /* write the argv pointer array entry */
      temp = MD_SWAPW(sp);
      mem_access(mem, Write, argv_addr + i*sizeof(md_addr_t),
		 &temp, sizeof(md_addr_t));
      /* and the data */
      mem_strcpy(&TMIPS::mem_access, mem, Write, sp, argv[i]);
      sp += strlen(argv[i]) + 1;
    }
  /* terminate argv array with a NULL */
  mem_access(mem, Write, argv_addr + i*sizeof(md_addr_t),
	     &null_ptr, sizeof(md_addr_t));

  /* write envp pointer array and data to stack */
  for (i = 0; envp[i]; i++)
    {
      /* write the envp pointer array entry */
      temp = MD_SWAPW(sp);
      mem_access(mem, Write, envp_addr + i*sizeof(md_addr_t),
		 &temp, sizeof(md_addr_t));
      /* and the data */
      mem_strcpy(&TMIPS::mem_access, mem, Write, sp, envp[i]);
      sp += strlen(envp[i]) + 1;
    }
  /* terminate the envp array with a NULL */
  mem_access(mem, Write, envp_addr + i*sizeof(md_addr_t),
	     &null_ptr, sizeof(md_addr_t));

  /* did we tromp off the stop of the stack? */
  if (sp > ld_stack_base)
    {
      /* we did, indicate to the user that MD_MAX_ENVIRON must be increased,
	 alternatively, you can use a smaller environment, or fewer
	 command line arguments */
      fatal("environment overflow, increase MD_MAX_ENVIRON in ss.h");
    }

  /* initialize the bottom of heap to top of data segment */
  ld_brk_point = ROUND_UP(ld_data_base + ld_data_size, MD_PAGE_SIZE);

  /* set initial minimum stack pointer value to initial stack value */
  ld_stack_min = regs->regs_R[MD_REG_SP];

  /* set up initial register state */
  regs->regs_R[MD_REG_SP] = ld_environ_base;
  regs->regs_PC = ld_prog_entry;

  debug("ld_text_base_t: 0x%08x  ld_text_size: 0x%08x",
	ld_text_base, ld_text_size);
  debug("ld_data_base_t: 0x%08x  ld_data_size: 0x%08x",
	ld_data_base, ld_data_size);
  debug("ld_stack_base_t: 0x%08x  ld_stack_size: 0x%08x",
	ld_stack_base, ld_stack_size);
  debug("ld_prog_entry_t: 0x%08x", ld_prog_entry);

  /* finally, predecode the text segment... */
  {
    md_addr_t addr;
    md_inst_t inst;
    enum md_fault_type fault;

    if (OP_MAX > 255)
      fatal("cannot perform fast decoding, too many opcodes");

    debug("sim: decoding text segment...");
    for (addr=ld_text_base;
	 addr < (ld_text_base+ld_text_size);
	 addr += sizeof(md_inst_t))
      {
	fault = mem_access(mem, Read, addr, &inst, sizeof(inst));
	if (fault != md_fault_none)
	  fatal("could not read instruction memory");
	inst.a = (inst.a & ~0xff) | (word_t)MD_OP_ENUM(MD_OPFIELD(inst));
	fault = mem_access(mem, Write, addr, &inst, sizeof(inst));
	if (fault != md_fault_none)
	  fatal("could not write instruction memory");
      }
  }
}

/*end of loader.cc*/
/*syscall.cc*/
void TMIPS::sys_syscall(struct regs_t *regs,	/* registers to access */
	    mem_access_fn mem_fn,	/* generic memory accessor */
	    struct mem_t *mem,		/* memory space to access */
	    md_inst_t inst,		/* system call inst */
	    int traceable)		/* traceable system call? */
{
  word_t syscode = regs->regs_R[2];

  /* first, check if an EIO trace is being consumed... */
  /*eio:Error In Operation*/
  if (traceable && sim_eio_fd != NULL)
    {
      eio_read_trace(sim_eio_fd, sim_num_insn, regs, mem_fn, mem, inst);

      /* fini... */
      return;
    }
#ifdef MD_CROSS_ENDIAN
  else if (syscode == SS_SYS_exit)
    {
      /* exit jumps to the target set in main() */
      longjmp(sim_exit_buf, /* exitcode + fudge */regs->regs_R[4]+1);
    }
  else
    fatal("cannot execute PISA system call on cross-endian host");

#else /* !MD_CROSS_ENDIAN */

  /* no, OK execute the live system call... */
  switch (syscode)
    {
    case SS_SYS_exit:
    	exitrun();
    	if(Getexitstates()){
 //   		esynetRecord er(actualid);
 //   		er.valid = 1;
 //   		er.sim_cycle = sim_cycle;
 //   		er.hop = visual_hop;
 //   		er.esynetInitData();
//    		er.esynetWriteIn(Simulation::visualFile,0);
    		/* exit jumps to the target set in main() */
    		printf("node %d exited and ", actualid);
    		longjmp(sim_exit_buf, /* exitcode + fudge */regs->regs_R[4]+1);
    	}
    	else{
  //  		esynetRecord er(actualid);
  //  		er.valid = 1;
  //  		er.sim_cycle = sim_cycle;
  //  		er.hop = visual_hop;
  //  		er.esynetInitData();
   // 		er.esynetWriteIn(Simulation::visualFile,0);
    		longjmp(node_exit_buf, /* exitcode + fudge */regs->regs_R[4]+1);
    	}
      break;

#if 0
    case SS_SYS_fork:
      /* FIXME: this is broken... */
      regs->regs_R[2] = fork();
      if (regs->regs_R[2] != -1)
	{
	  regs->regs_R[7] = 0;
	  /* parent process */
	  if (regs->regs_R[2] != 0)
	  regs->regs_R[3] = 0;
	}
      else
	fatal("SYS_fork failed");
      break;
#endif

#if 0
    case SS_SYS_vfork:
      /* FIXME: this is broken... */
      int r31_parent = regs->regs_R[31];
      /* pid */regs->regs_R[2] = vfork();
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	fatal("vfork() in SYS_vfork failed");
      if (regs->regs_R[2] != 0)
	{
	  regs->regs_R[3] = 0;
	  regs->regs_R[7] = 0;
	  regs->regs_R[31] = r31_parent;
	}
      break;
#endif

    case SS_SYS_read:
      {
	char *buf;

	/* allocate same-sized input buffer in host memory */
	if (!(buf = (char *)calloc(/*nbytes*/regs->regs_R[6], sizeof(char))))
	  fatal("out of memory in SYS_read");

	/* read data from file */
	/*nread*/regs->regs_R[2] =
	  read(/*fd*/regs->regs_R[4], buf, /*nbytes*/regs->regs_R[6]);

	/* check for error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* copy results back into host memory */
	mem_bcopy(mem_fn, mem,
		  Write, /*buf*/regs->regs_R[5],
		  buf, /*nread*/regs->regs_R[2]);

	/* done with input buffer */
	free(buf);
      }
      break;

    case SS_SYS_write:
      {
	char *buf;

	/* allocate same-sized output buffer in host memory */
	if (!(buf = (char *)calloc(/*nbytes*/regs->regs_R[6], sizeof(char))))
	  fatal("out of memory in SYS_write");

	/* copy inputs into host memory */
	mem_bcopy(mem_fn, mem,
		  Read, /*buf*/regs->regs_R[5],
		  buf, /*nbytes*/regs->regs_R[6]);

	/* write data to file */
	if (sim_progfd && MD_OUTPUT_SYSCALL(regs))
	  {
	    /* redirect program output to file */

	    /*nwritten*/regs->regs_R[2] =
	      fwrite(buf, 1, /*nbytes*/regs->regs_R[6], fopen(sim_progfd,"w+"));
	  }
	else
	  {
	    /* perform program output request */

	    /*nwritten*/regs->regs_R[2] =
	      write(/*fd*/regs->regs_R[4],
		    buf, /*nbytes*/regs->regs_R[6]);
	  }

	/* check for an error condition */
	if (regs->regs_R[2] == regs->regs_R[6])
	  /*result*/regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* done with output buffer */
	free(buf);
      }
      break;

    case SS_SYS_open:
      {
	char buf[MAXBUFSIZE];
	unsigned int i;
	int ss_flags = regs->regs_R[5], local_flags = 0;

	/* translate open(2) flags */
	for (i=0; i<SS_NFLAGS; i++)
	  {
	    if (ss_flags & ss_flag_table[i].ss_flag)
	      {
		ss_flags &= ~ss_flag_table[i].ss_flag;
		local_flags |= ss_flag_table[i].local_flag;
	      }
	  }
	/* any target flags left? */
	if (ss_flags != 0)
	  fatal("syscall: open: cannot decode flags: 0x%08x", ss_flags);

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[4], buf);

	/* open the file */
	/*fd*/regs->regs_R[2] =
	  open(buf, local_flags, /*mode*/regs->regs_R[6]);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;

    case SS_SYS_close:
      /* don't close stdin, stdout, or stderr as this messes up sim logs */
      if (/*fd*/regs->regs_R[4] == 0
	  || /*fd*/regs->regs_R[4] == 1
	  || /*fd*/regs->regs_R[4] == 2)
	{
	  regs->regs_R[7] = 0;
	  break;
	}

      /* close the file */
      regs->regs_R[2] = close(/*fd*/regs->regs_R[4]);

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
      break;

    case SS_SYS_creat:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[4], buf);

	/* create the file */
	/*fd*/regs->regs_R[2] = creat(buf, /*mode*/regs->regs_R[5]);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;

    case SS_SYS_unlink:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[4], buf);

	/* delete the file */
	/*result*/regs->regs_R[2] = unlink(buf);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;

    case SS_SYS_chdir:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[4], buf);

	/* change the working directory */
	/*result*/regs->regs_R[2] = chdir(buf);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;

    case SS_SYS_chmod:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[4], buf);

	/* chmod the file */
	/*result*/regs->regs_R[2] = chmod(buf, /*mode*/regs->regs_R[5]);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;
	case SS_SYS_mpisend:{//mpi send
		void *buf;
		char* buf_chara;
		int s_actual_id=-1,d_actual_id=-1;
		if(!(buf = (void *)calloc(regs->regs_R[6],sizeof(char))))
			panic("apply space from os error");
		if(!(buf_chara = (char *)calloc(MAX_MPICHARA,sizeof(char))))
			panic("apply space from os error");
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[5], buf, /*nbytes*/regs->regs_R[6]);
		md_addr_t buf_chara_addr;
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[7], buf_chara, /*nbytes*/MAX_MPICHARA);
//		mem_bcopy(mem_fn, mem, Read, buf_chara_addr, buf_chara, /*nbytes*/MAX_MPICHARA);
		s_actual_id = actualid;
		d_actual_id = regs->regs_R[4];
		if(s_actual_id<0)
			panic("unvaliable s_actual_id");
		if(d_actual_id<0)
			panic("unvaliable d_actual_id");
		/* read the 5th parameter (timeout) from the stack */
		md_addr_t para_5_addr;
		mem_bcopy(mem_fn, mem,  Read, regs->regs_R[29]+16, &para_5_addr, sizeof(word_t));
//		mem_bcopy(mem_fn, mem,  Read, para_5_addr, buf_para5, MAX_MPICHARA);
		MPIMsg * mpimsg = new MPIMsg(s_actual_id, d_actual_id, regs->regs_R[6], buf_chara, buf, 0, nomsg);
		ss_msg_r* thismsg = glb_network.InsertMsg(mpimsg, sim_cycle);
		esynetrcd->msg_send += 1;
		msg_send_num++;
		printf("node %d send msg %d, num = %d\n",actualid, esynetrcd->msg_send, msg_send_num);
		esynetrcd->valid = 1;
		md_fault_type s = mem_bcopy(mem_fn, mem, Write, para_5_addr, (void*)thismsg, /*nbytes*/sizeof(ss_msg_r));
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
	    /* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
//		free(thismsg);
	}
	break;
	case SS_SYS_mpirecv:{//mpi receive
		char *chara_buf;
		int s_actual_id=-1;
		int d_actual_id=-1;
//		s_actual_id = mt->Id_table_get(masterid,regs->regs_R[4]);
		s_actual_id = regs->regs_R[4];
		chara_buf = (char*)calloc(MAX_MPICHARA,sizeof(char));
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[5], chara_buf, /*nbytes*/MAX_MPICHARA);
		MPIMsg* msg =  glb_network.MsgReceiveFromClient(chara_buf,s_actual_id,actualid);
		msg->over_cycle = sim_cycle;
		ss_msg_r thismsg = {msg->msgsize,msg->simcycle,msg->simcycle_noc,
						msg->msgid,msg->over_cycle,msg->over_cycle_noc};
//		fputs("msg_recv:1 ", Simulation::visualFile);
		esynetrcd->msg_recv += 1;
		esynetrcd->valid = 1;
		msg_recv_num++;
		printf("node %d recv msg %d, num = %d\n",actualid, esynetrcd->msg_recv, msg_recv_num);
		mem_bcopy(mem_fn, mem, Write, regs->regs_R[6], msg->msgpnt, /*nbytes*/msg->msgsize);
		mem_bcopy(mem_fn, mem, Write, regs->regs_R[7], &thismsg, /*nbytes*/sizeof(ss_msg_r));
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
		  regs->regs_R[7] = 0;
		else
		  {
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		  }
		free(chara_buf);
	}
	break;
	case SS_SYS_rtactid:
		regs->regs_R[2] = mt->Id_table_get(masterid,threadid);
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
		  regs->regs_R[7] = 0;
		else
		  {
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		  }
	break;
	case SS_SYS_rtthdid:
		regs->regs_R[2] = threadid;
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
		  regs->regs_R[7] = 0;
		else
		  {
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		  }
	break;
	case SS_SYS_mpimsgstat:{
		char* chara_buf = (char*)calloc(MAX_MPICHARA,sizeof(char));
		int s_actual_id,d_actual_id;
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[6], chara_buf, /*nbytes*/MAX_MPICHARA);
		s_actual_id = regs->regs_R[4];
		d_actual_id = regs->regs_R[5];
		Msgstatus s = glb_network.LoopUpMsgStatus(chara_buf,s_actual_id,d_actual_id);
//		fputs("access_NI_to_ask_msg_if_arrive_or_not:1 ", Simulation::visualFile);
		esynetrcd->msg_probe += 1;
		esynetrcd->valid = 1;
		regs->regs_R[2] = s;
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
		  regs->regs_R[7] = 0;
		else
		  {
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		  }
		free(chara_buf);
	}
	break;
	case SS_SYS_mpisend_f:{
		void *buf;
		char* buf_chara;
		int s_actual_id=-1,d_actual_id=-1;
		if(!(buf = (void *)calloc(regs->regs_R[6],sizeof(char))))
			panic("apply space from os error");
		if(!(buf_chara = (char *)calloc(MAX_MPICHARA,sizeof(char))))
			panic("apply space from os error");
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[5], buf, /*nbytes*/regs->regs_R[6]);
		md_addr_t buf_chara_addr;
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[7], buf_chara, /*nbytes*/MAX_MPICHARA);
		s_actual_id = actualid;
		d_actual_id = regs->regs_R[4];
		if(s_actual_id<0)
			panic("unvaliable s_actual_id");
		if(d_actual_id<0)
			panic("unvaliable d_actual_id");
		/* read the 5th parameter (timeout) from the stack */
		md_addr_t para_5_addr;
		mem_bcopy(mem_fn, mem,  Read, regs->regs_R[29]+16, &para_5_addr, sizeof(word_t));
		MPIMsg * mpimsg = new MPIMsg(s_actual_id, d_actual_id, regs->regs_R[6], buf_chara, buf, 1, fpga_msgcreat);
		ss_msg_r* thismsg = glb_network.InsertMsg(mpimsg, sim_cycle);
		//遍历fpgac_node，找出目的节点信息
		int i;
		for(i=0;i<MAXTHREADS;i++){
			if(sim_asic[i] && sim_asic[i]->actualid == d_actual_id){
				if(sim_asic[i]->running == 1){
					switch(sim_asic[i]->t){
						case(asic_fft_128):{//放入队列
							struct fft_status* task = (struct fft_status*)malloc(sizeof(struct fft_status));
							task->source = (fft_data_t*)buf;
							task->target = (fft_data_t*)malloc(regs->regs_R[6]);
							task->src = actualid;
							task->msgchara = buf_chara;
							sim_asic[i]->task_list.insert(pair<char*, void*>(buf_chara, (void*)task));
						}break;
						default:{printf("no a asic unit\n");}
					}
				}
			}
		}
		esynetrcd->msg_send += 1;
		esynetrcd->valid = 1;
		md_fault_type s = mem_bcopy(mem_fn, mem, Write, para_5_addr, (void*)thismsg, /*nbytes*/sizeof(ss_msg_r));
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
		/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
//		free(thismsg);
	}
	break;
	case SS_SYS_mpirecv_f:{
		char *chara_buf;
		int s_actual_id=-1;
		int d_actual_id=-1;
		s_actual_id = regs->regs_R[4];
		chara_buf = (char*)calloc(MAX_MPICHARA,sizeof(char));
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[5], chara_buf, /*nbytes*/MAX_MPICHARA);
		MPIMsg* msg =  glb_network.MsgReceiveFromClient(chara_buf,s_actual_id,actualid);
		msg->over_cycle = sim_cycle;
		ss_msg_r thismsg = {msg->msgsize,msg->simcycle,msg->simcycle_noc,
						msg->msgid,msg->over_cycle,msg->over_cycle_noc};
//		fputs("msg_recv:1 ", Simulation::visualFile);
		esynetrcd->msg_recv += 1;
		esynetrcd->valid = 1;
		mem_bcopy(mem_fn, mem, Write, regs->regs_R[6], msg->msgpnt, /*nbytes*/msg->msgsize);
		mem_bcopy(mem_fn, mem, Write, regs->regs_R[7], &thismsg, /*nbytes*/sizeof(ss_msg_r));
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
		  regs->regs_R[7] = 0;
		else
		  {
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		  }
		free(chara_buf);
	}
	break;

	case SS_SYS_mpimsgstat_f:{
		char* chara_buf = (char*)calloc(MAX_MPICHARA,sizeof(char));
		int s_actual_id,d_actual_id;
		mem_bcopy(mem_fn, mem, Read, regs->regs_R[6], chara_buf, /*nbytes*/MAX_MPICHARA);
		s_actual_id = regs->regs_R[4];
		d_actual_id = regs->regs_R[5];
		Msgstatus s = glb_network.LoopUpMsgStatus(chara_buf, d_actual_id, s_actual_id);
		esynetrcd->msg_probe += 1;
		esynetrcd->valid = 1;
		regs->regs_R[2] = s;
		/* check for an error condition */
		if (regs->regs_R[2] != -1)
		  regs->regs_R[7] = 0;
		else
		  {
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		  }
		free(chara_buf);
	}
	break;

    case SS_SYS_chown:
#ifdef _MSC_VER
      warn("syscall chown() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#else /* !_MSC_VER */
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[4], buf);

	/* chown the file */
	/*result*/regs->regs_R[2] = chown(buf, /*owner*/regs->regs_R[5],
				    /*group*/regs->regs_R[6]);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
#endif /* _MSC_VER */
      break;

    case SS_SYS_brk:
      {
	md_addr_t addr;

	/* round the new heap pointer to the its page boundary */
	addr = ROUND_UP(/*base*/regs->regs_R[4], MD_PAGE_SIZE);

	/* check whether heap area has merged with stack area */
	if (addr >= ld_brk_point && addr < (md_addr_t)regs->regs_R[29])
	  {
	    regs->regs_R[2] = 0;
	    regs->regs_R[7] = 0;
	    ld_brk_point = addr;
	  }
	else
	  {
	    /* out of address space, indicate error */
	    regs->regs_R[2] = ENOMEM;
	    regs->regs_R[7] = 1;
	  }
      }
      break;

    case SS_SYS_lseek:
      /* seek into file */
      regs->regs_R[2] =
	lseek(/*fd*/regs->regs_R[4],
	      /*off*/regs->regs_R[5], /*dir*/regs->regs_R[6]);

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
      break;

    case SS_SYS_getpid:
      /* get the simulator process id */
      /*result*/regs->regs_R[2] = getpid();

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
      break;

    case SS_SYS_getuid:
#ifdef _MSC_VER
      warn("syscall getuid() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#else /* !_MSC_VER */
      /* get current user id */
      /*first result*/regs->regs_R[2] = getuid();

      /* get effective user id */
      /*second result*/regs->regs_R[3] = geteuid();

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
#endif /* _MSC_VER */
      break;

    case SS_SYS_access:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fName*/regs->regs_R[4], buf);

	/* check access on the file */
	/*result*/regs->regs_R[2] = access(buf, /*mode*/regs->regs_R[5]);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;

    case SS_SYS_stat:
    case SS_SYS_lstat:
      {
	char buf[MAXBUFSIZE];
	struct ss_statbuf ss_sbuf;
#ifdef _MSC_VER
	struct _stat sbuf;
#else /* !_MSC_VER */
	struct stat sbuf;
#endif /* _MSC_VER */

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fName*/regs->regs_R[4], buf);

	/* stat() the file */
	if (syscode == SS_SYS_stat)
	  /*result*/regs->regs_R[2] = stat(buf, &sbuf);
	else /* syscode == SS_SYS_lstat */
	  {
#ifdef _MSC_VER
	    warn("syscall lstat() not yet implemented for MSC...");
	    regs->regs_R[7] = 0;
	    break;
#else /* !_MSC_VER */
	    /*result*/regs->regs_R[2] = lstat(buf, &sbuf);
#endif /* _MSC_VER */
	  }

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* translate from host stat structure to target format */
	ss_sbuf.ss_st_dev = MD_SWAPH(sbuf.st_dev);
	ss_sbuf.ss_pad = 0;
	ss_sbuf.ss_st_ino = MD_SWAPW(sbuf.st_ino);
	ss_sbuf.ss_st_mode = MD_SWAPH(sbuf.st_mode);
	ss_sbuf.ss_st_nlink = MD_SWAPH(sbuf.st_nlink);
	ss_sbuf.ss_st_uid = MD_SWAPH(sbuf.st_uid);
	ss_sbuf.ss_st_gid = MD_SWAPH(sbuf.st_gid);
	ss_sbuf.ss_st_rdev = MD_SWAPH(sbuf.st_rdev);
	ss_sbuf.ss_pad1 = 0;
	ss_sbuf.ss_st_size = MD_SWAPW(sbuf.st_size);
	ss_sbuf.ss_st_atime = MD_SWAPW(sbuf.st_atime);
	ss_sbuf.ss_st_spare1 = 0;
	ss_sbuf.ss_st_mtime = MD_SWAPW(sbuf.st_mtime);
	ss_sbuf.ss_st_spare2 = 0;
	ss_sbuf.ss_st_ctime = MD_SWAPW(sbuf.st_ctime);
	ss_sbuf.ss_st_spare3 = 0;
#ifndef _MSC_VER
	ss_sbuf.ss_st_blksize = MD_SWAPW(sbuf.st_blksize);
	ss_sbuf.ss_st_blocks = MD_SWAPW(sbuf.st_blocks);
#endif /* !_MSC_VER */
	ss_sbuf.ss_st_gennum = 0;
	ss_sbuf.ss_st_spare4 = 0;

	/* copy stat() results to simulator memory */
	mem_bcopy(mem_fn, mem, Write, /*sbuf*/regs->regs_R[5],
		  &ss_sbuf, sizeof(struct ss_statbuf));
      }
      break;

    case SS_SYS_dup:
      /* dup() the file descriptor */
      /*fd*/regs->regs_R[2] = dup(/*fd*/regs->regs_R[4]);

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
      break;

#ifndef _MSC_VER
    case SS_SYS_pipe:
      {
	int fd[2];

	/* copy pipe descriptors to host memory */;
	mem_bcopy(mem_fn, mem, Read, /*fd's*/regs->regs_R[4], fd, sizeof(fd));

	/* create a pipe */
	/*result*/regs->regs_R[7] = pipe(fd);

	/* copy descriptor results to result registers */
	/*pipe1*/regs->regs_R[2] = fd[0];
	/*pipe 2*/regs->regs_R[3] = fd[1];

	/* check for an error condition */
	if (regs->regs_R[7] == -1)
	  {
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;
#endif

    case SS_SYS_getgid:
#ifdef _MSC_VER
      warn("syscall getgid() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#else /* !_MSC_VER */
      /* get current group id */
      /*first result*/regs->regs_R[2] = getgid();

      /* get current effective group id */
      /*second result*/regs->regs_R[3] = getegid();

	/* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
#endif /* _MSC_VER */
      break;

    case SS_SYS_ioctl:
      {
	char buf[NUM_IOCTL_BYTES];
	int local_req = 0;

	/* convert target ioctl() request to host ioctl() request values */
	switch (/*req*/regs->regs_R[5]) {
#ifdef TIOCGETP
	case SS_IOCTL_TIOCGETP:
	  local_req = TIOCGETP;
	  break;
#endif
#ifdef TIOCSETP
	case SS_IOCTL_TIOCSETP:
	  local_req = TIOCSETP;
	  break;
#endif
#ifdef TIOCGETP
	case SS_IOCTL_TCGETP:
	  local_req = TIOCGETP;
	  break;
#endif
#ifdef TCGETA
	case SS_IOCTL_TCGETA:
	  local_req = TCGETA;
	  break;
#endif
#ifdef TIOCGLTC
	case SS_IOCTL_TIOCGLTC:
	  local_req = TIOCGLTC;
	  break;
#endif
#ifdef TIOCSLTC
	case SS_IOCTL_TIOCSLTC:
	  local_req = TIOCSLTC;
	  break;
#endif
#ifdef TIOCGWINSZ
	case SS_IOCTL_TIOCGWINSZ:
	  local_req = TIOCGWINSZ;
	  break;
#endif
#ifdef TCSETAW
	case SS_IOCTL_TCSETAW:
	  local_req = TCSETAW;
	  break;
#endif
#ifdef TIOCGETC
	case SS_IOCTL_TIOCGETC:
	  local_req = TIOCGETC;
	  break;
#endif
#ifdef TIOCSETC
	case SS_IOCTL_TIOCSETC:
	  local_req = TIOCSETC;
	  break;
#endif
#ifdef TIOCLBIC
	case SS_IOCTL_TIOCLBIC:
	  local_req = TIOCLBIC;
	  break;
#endif
#ifdef TIOCLBIS
	case SS_IOCTL_TIOCLBIS:
	  local_req = TIOCLBIS;
	  break;
#endif
#ifdef TIOCLGET
	case SS_IOCTL_TIOCLGET:
	  local_req = TIOCLGET;
	  break;
#endif
#ifdef TIOCLSET
	case SS_IOCTL_TIOCLSET:
	  local_req = TIOCLSET;
	  break;
#endif
	}

#if !defined(TIOCGETP) && (defined(linux) || defined(__CYGWIN32__))
        if (!local_req && /*req*/regs->regs_R[5] == SS_IOCTL_TIOCGETP)
          {
            struct termios lbuf;
            struct ss_sgttyb buf;

            /* result */regs->regs_R[2] =
                          tcgetattr(/* fd */(int)regs->regs_R[4], &lbuf);

            /* translate results */
            buf.sg_ispeed = lbuf.c_ispeed;
            buf.sg_ospeed = lbuf.c_ospeed;
            buf.sg_erase = lbuf.c_cc[VERASE];
            buf.sg_kill = lbuf.c_cc[VKILL];
            buf.sg_flags = 0;   /* FIXME: this is wrong... */

            mem_bcopy(mem_fn, mem, Write,
                      /* buf */regs->regs_R[6], &buf,
                      sizeof(struct ss_sgttyb));

            if (regs->regs_R[2] != -1)
              regs->regs_R[7] = 0;
            else /* probably not a typewriter, return details */
              {
                regs->regs_R[2] = errno;
                regs->regs_R[7] = 1;
              }
          }
        else
#endif

	if (!local_req)
	  {
	    /* FIXME: could not translate the ioctl() request, just warn user
	       and ignore the request */
	    warn("syscall: ioctl: ioctl code not supported d=%d, req=%d",
		 regs->regs_R[4], regs->regs_R[5]);
	    regs->regs_R[2] = 0;
	    regs->regs_R[7] = 0;
	  }
	else
	  {
#ifdef _MSC_VER
	    warn("syscall getgid() not yet implemented for MSC...");
	    regs->regs_R[7] = 0;
	    break;
#else /* !_MSC_VER */

#if 0 /* FIXME: needed? */
#ifdef TIOCGETP
	    if (local_req == TIOCGETP && sim_progfd)
	      {
		/* program I/O has been redirected to file, make
		   termios() calls fail... */

		/* got an error, return details */
		regs->regs_R[2] = ENOTTY;
		regs->regs_R[7] = 1;
		break;
	      }
#endif
#endif
	    /* ioctl() code was successfully translated to a host code */

	    /* if arg ptr exists, copy NUM_IOCTL_BYTES bytes to host mem */
	    if (/*argp*/regs->regs_R[6] != 0)
	      mem_bcopy(mem_fn, mem,
			Read, /*argp*/regs->regs_R[6], buf, NUM_IOCTL_BYTES);

	    /* perform the ioctl() call */
	    /*result*/regs->regs_R[2] =
	      ioctl(/*fd*/regs->regs_R[4], local_req, buf);

	    /* if arg ptr exists, copy NUM_IOCTL_BYTES bytes from host mem */
	    if (/*argp*/regs->regs_R[6] != 0)
	      mem_bcopy(mem_fn, mem, Write, regs->regs_R[6],
			buf, NUM_IOCTL_BYTES);

	    /* check for an error condition */
	    if (regs->regs_R[2] != -1)
	      regs->regs_R[7] = 0;
	    else
	      {
		/* got an error, return details */
		regs->regs_R[2] = errno;
		regs->regs_R[7] = 1;
	      }
#endif /* _MSC_VER */
	  }
      }
      break;

    case SS_SYS_fstat:
      {
	struct ss_statbuf ss_sbuf;
#ifdef _MSC_VER
	struct _stat sbuf;
#else /* !_MSC_VER */
	struct stat sbuf;
#endif /* _MSC_VER */

	/* fstat() the file */
	/*result*/regs->regs_R[2] = fstat(/*fd*/regs->regs_R[4], &sbuf);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* translate the stat structure to host format */
	ss_sbuf.ss_st_dev = MD_SWAPH(sbuf.st_dev);
	ss_sbuf.ss_pad = 0;
	ss_sbuf.ss_st_ino = MD_SWAPW(sbuf.st_ino);
	ss_sbuf.ss_st_mode = MD_SWAPH(sbuf.st_mode);
	ss_sbuf.ss_st_nlink = MD_SWAPH(sbuf.st_nlink);
	ss_sbuf.ss_st_uid = MD_SWAPH(sbuf.st_uid);
	ss_sbuf.ss_st_gid = MD_SWAPH(sbuf.st_gid);
	ss_sbuf.ss_st_rdev = MD_SWAPH(sbuf.st_rdev);
	ss_sbuf.ss_pad1 = 0;
	ss_sbuf.ss_st_size = MD_SWAPW(sbuf.st_size);
	ss_sbuf.ss_st_atime = MD_SWAPW(sbuf.st_atime);
        ss_sbuf.ss_st_spare1 = 0;
	ss_sbuf.ss_st_mtime = MD_SWAPW(sbuf.st_mtime);
        ss_sbuf.ss_st_spare2 = 0;
	ss_sbuf.ss_st_ctime = MD_SWAPW(sbuf.st_ctime);
        ss_sbuf.ss_st_spare3 = 0;
#ifndef _MSC_VER
	ss_sbuf.ss_st_blksize = MD_SWAPW(sbuf.st_blksize);
	ss_sbuf.ss_st_blocks = MD_SWAPW(sbuf.st_blocks);
#endif /* !_MSC_VER */
        ss_sbuf.ss_st_gennum = 0;
        ss_sbuf.ss_st_spare4 = 0;

	/* copy fstat() results to simulator memory */
	mem_bcopy(mem_fn, mem, Write, /*sbuf*/regs->regs_R[5],
		  &ss_sbuf, sizeof(struct ss_statbuf));
      }
      break;

    case SS_SYS_getpagesize:
      /* get target pagesize */
      regs->regs_R[2] = /* was: getpagesize() */MD_PAGE_SIZE;

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
      break;

    case SS_SYS_setitimer:
      /* FIXME: the sigvec system call is ignored */
      regs->regs_R[2] = regs->regs_R[7] = 0;
      warn("syscall: setitimer ignored");
      break;

    case SS_SYS_getdtablesize:
#if defined(_AIX)
      /* get descriptor table size */
      regs->regs_R[2] = getdtablesize();

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
#elif defined(__CYGWIN32__) || defined(ultrix) || defined(_MSC_VER)
      {
	/* no comparable system call found, try some reasonable defaults */
	warn("syscall: called getdtablesize()\n");
	regs->regs_R[2] = 16;
	regs->regs_R[7] = 0;
      }
#else
      {
	struct rlimit rl;

	/* get descriptor table size in rlimit structure */
	if (getrlimit(RLIMIT_NOFILE, &rl) != -1)
	  {
	    regs->regs_R[2] = rl.rlim_cur;
	    regs->regs_R[7] = 0;
	  }
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
#endif
      break;

    case SS_SYS_dup2:
      /* dup2() the file descriptor */
      regs->regs_R[2] =
	dup2(/* fd1 */regs->regs_R[4], /* fd2 */regs->regs_R[5]);

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
      break;

    case SS_SYS_fcntl:
#ifdef _MSC_VER
      warn("syscall fcntl() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#else /* !_MSC_VER */
      /* get fcntl() information on the file */
      regs->regs_R[2] =
	fcntl(/*fd*/regs->regs_R[4], /*cmd*/regs->regs_R[5],
	      /*arg*/regs->regs_R[6]);

      /* check for an error condition */
      if (regs->regs_R[2] != -1)
	regs->regs_R[7] = 0;
      else
	{
	  /* got an error, return details */
	  regs->regs_R[2] = errno;
	  regs->regs_R[7] = 1;
	}
#endif /* _MSC_VER */
      break;

    case SS_SYS_select:
#ifdef _MSC_VER
      warn("syscall select() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#else /* !_MSC_VER */
      {
	fd_set readfd, writefd, exceptfd;
	fd_set *readfdp, *writefdp, *exceptfdp;
	struct timeval timeout, *timeoutp;
	word_t param5;

	/* FIXME: swap words? */

	/* read the 5th parameter (timeout) from the stack */
	mem_bcopy(mem_fn, mem,
		  Read, regs->regs_R[29]+16, &param5, sizeof(word_t));

	/* copy read file descriptor set into host memory */
	if (/*readfd*/regs->regs_R[5] != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read, /*readfd*/regs->regs_R[5],
		      &readfd, sizeof(fd_set));
	    readfdp = &readfd;
	  }
	else
	  readfdp = NULL;

	/* copy write file descriptor set into host memory */
	if (/*writefd*/regs->regs_R[6] != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read, /*writefd*/regs->regs_R[6],
		      &writefd, sizeof(fd_set));
	    writefdp = &writefd;
	  }
	else
	  writefdp = NULL;

	/* copy exception file descriptor set into host memory */
	if (/*exceptfd*/regs->regs_R[7] != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read, /*exceptfd*/regs->regs_R[7],
		      &exceptfd, sizeof(fd_set));
	    exceptfdp = &exceptfd;
	  }
	else
	  exceptfdp = NULL;

	/* copy timeout value into host memory */
	if (/*timeout*/param5 != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read, /*timeout*/param5,
		      &timeout, sizeof(struct timeval));
	    timeoutp = &timeout;
	  }
	else
	  timeoutp = NULL;

#if defined(hpux) || defined(__hpux)
	/* select() on the specified file descriptors */
	/*result*/regs->regs_R[2] =
	  select(/*nfd*/regs->regs_R[4],
		 (int *)readfdp, (int *)writefdp, (int *)exceptfdp, timeoutp);
#else
	/* select() on the specified file descriptors */
	/*result*/regs->regs_R[2] =
	  select(/*nfd*/regs->regs_R[4],
		 readfdp, writefdp, exceptfdp, timeoutp);
#endif

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, return details */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* copy read file descriptor set to target memory */
	if (/*readfd*/regs->regs_R[5] != 0)
	  mem_bcopy(mem_fn, mem, Write, /*readfd*/regs->regs_R[5],
		    &readfd, sizeof(fd_set));

	/* copy write file descriptor set to target memory */
	if (/*writefd*/regs->regs_R[6] != 0)
	  mem_bcopy(mem_fn, mem, Write, /*writefd*/regs->regs_R[6],
		    &writefd, sizeof(fd_set));

	/* copy exception file descriptor set to target memory */
	if (/*exceptfd*/regs->regs_R[7] != 0)
	  mem_bcopy(mem_fn, mem, Write, /*exceptfd*/regs->regs_R[7],
		    &exceptfd, sizeof(fd_set));

	/* copy timeout value result to target memory */
	if (/* timeout */param5 != 0)
	  mem_bcopy(mem_fn, mem, Write, /*timeout*/param5,
		    &timeout, sizeof(struct timeval));
      }
#endif
      break;

    case SS_SYS_sigvec:
      /* FIXME: the sigvec system call is ignored */
      regs->regs_R[2] = regs->regs_R[7] = 0;
      warn("syscall: sigvec ignored");
      break;

    case SS_SYS_sigblock:
      /* FIXME: the sigblock system call is ignored */
      regs->regs_R[2] = regs->regs_R[7] = 0;
      warn("syscall: sigblock ignored");
      break;

    case SS_SYS_sigsetmask:
      /* FIXME: the sigsetmask system call is ignored */
      regs->regs_R[2] = regs->regs_R[7] = 0;
      warn("syscall: sigsetmask ignored");
      break;

#if 0
    case SS_SYS_sigstack:
      /* FIXME: this is broken... */
      /* do not make the system call; instead, modify (the stack
	 portion of) the simulator's main memory, ignore the 1st
	 argument (regs->regs_R[4]), as it relates to signal handling */
      if (regs->regs_R[5] != 0)
	{
	  (*maf)(Read, regs->regs_R[29]+28, (unsigned char *)&temp, 4);
	  (*maf)(Write, regs->regs_R[5], (unsigned char *)&temp, 4);
	}
      regs->regs_R[2] = regs->regs_R[7] = 0;
      break;
#endif

    case SS_SYS_gettimeofday:
#ifdef _MSC_VER
      warn("syscall gettimeofday() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#else /* _MSC_VER */
      {
	struct ss_timeval ss_tv;
	struct timeval tv, *tvp;
	struct ss_timezone ss_tz;
	struct timezone tz, *tzp;

	if (/*timeval*/regs->regs_R[4] != 0)
	  {
	    /* copy timeval into host memory */
	    mem_bcopy(mem_fn, mem, Read, /*timeval*/regs->regs_R[4],
		      &ss_tv, sizeof(struct ss_timeval));

	    /* convert target timeval structure to host format */
	    tv.tv_sec = MD_SWAPW(ss_tv.ss_tv_sec);
	    tv.tv_usec = MD_SWAPW(ss_tv.ss_tv_usec);
	    tvp = &tv;
	  }
	else
	  tvp = NULL;

	if (/*timezone*/regs->regs_R[5] != 0)
	  {
	    /* copy timezone into host memory */
	    mem_bcopy(mem_fn, mem, Read, /*timezone*/regs->regs_R[5],
		      &ss_tz, sizeof(struct ss_timezone));

	    /* convert target timezone structure to host format */
	    tz.tz_minuteswest = MD_SWAPW(ss_tz.ss_tz_minuteswest);
	    tz.tz_dsttime = MD_SWAPW(ss_tz.ss_tz_dsttime);
	    tzp = &tz;
	  }
	else
	  tzp = NULL;

	/* get time of day */
	/*result*/regs->regs_R[2] = gettimeofday(tvp, tzp);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, indicate result */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	if (/*timeval*/regs->regs_R[4] != 0)
	  {
	    /* convert host timeval structure to target format */
	    ss_tv.ss_tv_sec = MD_SWAPW(tv.tv_sec);
	    ss_tv.ss_tv_usec = MD_SWAPW(tv.tv_usec);

	    /* copy timeval to target memory */
	    mem_bcopy(mem_fn, mem, Write, /*timeval*/regs->regs_R[4],
		      &ss_tv, sizeof(struct ss_timeval));
	  }

	if (/*timezone*/regs->regs_R[5] != 0)
	  {
	    /* convert host timezone structure to target format */
	    ss_tz.ss_tz_minuteswest = MD_SWAPW(tz.tz_minuteswest);
	    ss_tz.ss_tz_dsttime = MD_SWAPW(tz.tz_dsttime);

	    /* copy timezone to target memory */
	    mem_bcopy(mem_fn, mem, Write, /*timezone*/regs->regs_R[5],
		      &ss_tz, sizeof(struct ss_timezone));
	  }
      }
#endif /* !_MSC_VER */
      break;

    case SS_SYS_getrusage:
#if defined(__svr4__) || defined(__USLC__) || defined(hpux) || defined(__hpux) || defined(_AIX)
      {
	struct tms tms_buf;
	struct ss_rusage rusage;

	/* get user and system times */
	if (times(&tms_buf) != -1)
	  {
	    /* no error */
	    regs->regs_R[2] = 0;
	    regs->regs_R[7] = 0;
	  }
	else
	  {
	    /* got an error, indicate result */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* initialize target rusage result structure */
#if defined(__svr4__)
	memset(&rusage, '\0', sizeof(struct ss_rusage));
#else /* !defined(__svr4__) */
	bzero(&rusage, sizeof(struct ss_rusage));
#endif

	/* convert from host rusage structure to target format */
	rusage.ss_ru_utime.ss_tv_sec = tms_buf.tms_utime/CLK_TCK;
	rusage.ss_ru_utime.ss_tv_sec = MD_SWAPW(rusage.ss_ru_utime.ss_tv_sec);
	rusage.ss_ru_utime.ss_tv_usec = 0;
	rusage.ss_ru_stime.ss_tv_sec = tms_buf.tms_stime/CLK_TCK;
	rusage.ss_ru_stime.ss_tv_sec = MD_SWAPW(rusage.ss_ru_stime.ss_tv_sec);
	rusage.ss_ru_stime.ss_tv_usec = 0;

	/* copy rusage results into target memory */
	mem_bcopy(mem_fn, mem, Write, /*rusage*/regs->regs_R[5],
		  &rusage, sizeof(struct ss_rusage));
      }
#elif defined(__unix__) || defined(unix)
      {
	struct rusage local_rusage;
	struct ss_rusage rusage;

	/* get rusage information */
	/*result*/regs->regs_R[2] =
	  getrusage(/*who*/regs->regs_R[4], &local_rusage);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, indicate result */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* convert from host rusage structure to target format */
	rusage.ss_ru_utime.ss_tv_sec = local_rusage.ru_utime.tv_sec;
	rusage.ss_ru_utime.ss_tv_usec = local_rusage.ru_utime.tv_usec;
	rusage.ss_ru_utime.ss_tv_sec = MD_SWAPW(local_rusage.ru_utime.tv_sec);
	rusage.ss_ru_utime.ss_tv_usec =
	  MD_SWAPW(local_rusage.ru_utime.tv_usec);
	rusage.ss_ru_stime.ss_tv_sec = local_rusage.ru_stime.tv_sec;
	rusage.ss_ru_stime.ss_tv_usec = local_rusage.ru_stime.tv_usec;
	rusage.ss_ru_stime.ss_tv_sec =
	  MD_SWAPW(local_rusage.ru_stime.tv_sec);
	rusage.ss_ru_stime.ss_tv_usec =
	  MD_SWAPW(local_rusage.ru_stime.tv_usec);
	rusage.ss_ru_maxrss = MD_SWAPW(local_rusage.ru_maxrss);
	rusage.ss_ru_ixrss = MD_SWAPW(local_rusage.ru_ixrss);
	rusage.ss_ru_idrss = MD_SWAPW(local_rusage.ru_idrss);
	rusage.ss_ru_isrss = MD_SWAPW(local_rusage.ru_isrss);
	rusage.ss_ru_minflt = MD_SWAPW(local_rusage.ru_minflt);
	rusage.ss_ru_majflt = MD_SWAPW(local_rusage.ru_majflt);
	rusage.ss_ru_nswap = MD_SWAPW(local_rusage.ru_nswap);
	rusage.ss_ru_inblock = MD_SWAPW(local_rusage.ru_inblock);
	rusage.ss_ru_oublock = MD_SWAPW(local_rusage.ru_oublock);
	rusage.ss_ru_msgsnd = MD_SWAPW(local_rusage.ru_msgsnd);
	rusage.ss_ru_msgrcv = MD_SWAPW(local_rusage.ru_msgrcv);
	rusage.ss_ru_nsignals = MD_SWAPW(local_rusage.ru_nsignals);
	rusage.ss_ru_nvcsw = MD_SWAPW(local_rusage.ru_nvcsw);
	rusage.ss_ru_nivcsw = MD_SWAPW(local_rusage.ru_nivcsw);

	/* copy rusage results into target memory */
	mem_bcopy(mem_fn, mem, Write, /*rusage*/regs->regs_R[5],
		  &rusage, sizeof(struct ss_rusage));
      }
#elif defined(__CYGWIN32__) || defined(_MSC_VER)
	    warn("syscall: called getrusage()\n");
            regs->regs_R[7] = 0;
#else
#error No getrusage() implementation!
#endif
      break;

    case SS_SYS_writev:
#ifdef _MSC_VER
      warn("syscall writev() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#else /* !_MSC_VER */
      {
	int i;
	char *buf;
	struct iovec *iov;

	/* allocate host side I/O vectors */
	iov =
	  (struct iovec *)malloc(/*iovcnt*/regs->regs_R[6]
				 * sizeof(struct iovec));
	if (!iov)
	  fatal("out of virtual memory in SYS_writev");

	/* copy target side pointer data into host side vector */
	mem_bcopy(mem_fn, mem, Read, /*iov*/regs->regs_R[5],
		  iov, /*iovcnt*/regs->regs_R[6] * sizeof(struct iovec));

	/* copy target side I/O vector buffers to host memory */
	for (i=0; i < /*iovcnt*/regs->regs_R[6]; i++)
	  {
	    iov[i].iov_base = (char *)MD_SWAPW((unsigned long)iov[i].iov_base);
	    iov[i].iov_len = MD_SWAPW(iov[i].iov_len);
	    if (iov[i].iov_base != NULL)
	      {
		buf = (char *)calloc(iov[i].iov_len, sizeof(char));
		if (!buf)
		  fatal("out of virtual memory in SYS_writev");
		mem_bcopy(mem_fn, mem, Read, (qword_t)(iov[i].iov_base),
			  buf, iov[i].iov_len);
		iov[i].iov_base = buf;
	      }
	  }

	/* perform the vector'ed write */
	/*result*/regs->regs_R[2] =
	  writev(/*fd*/regs->regs_R[4], iov, /*iovcnt*/regs->regs_R[6]);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, indicate results */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* free all the allocated memory */
	for (i=0; i < /*iovcnt*/regs->regs_R[6]; i++)
	  {
	    if (iov[i].iov_base)
	      {
		free(iov[i].iov_base);
		iov[i].iov_base = NULL;
	      }
	  }
	free(iov);
      }
#endif /* !_MSC_VER */
      break;

    case SS_SYS_utimes:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[4], buf);

	if (/*timeval*/regs->regs_R[5] == 0)
	  {
#if defined(hpux) || defined(__hpux) || defined(linux)
	    /* no utimes() in hpux, use utime() instead */
	    /*result*/regs->regs_R[2] = utime(buf, NULL);
#elif defined(_MSC_VER)
	    /* no utimes() in MSC, use utime() instead */
	    /*result*/regs->regs_R[2] = utime(buf, NULL);
#elif defined(__svr4__) || defined(__USLC__) || defined(unix) || defined(_AIX) || defined(__alpha)
	    /*result*/regs->regs_R[2] = utimes(buf, NULL);
#elif defined(__CYGWIN32__)
	    warn("syscall: called utimes()\n");
#else
#error No utimes() implementation!
#endif
	  }
	else
	  {
	    struct ss_timeval ss_tval[2];
#ifndef _MSC_VER
	    struct timeval tval[2];
#endif /* !_MSC_VER */

	    /* copy timeval structure to host memory */
	    mem_bcopy(mem_fn, mem, Read, /*timeout*/regs->regs_R[5],
		      ss_tval, 2*sizeof(struct ss_timeval));

#ifndef _MSC_VER
	    /* convert timeval structure to host format */
	    tval[0].tv_sec = MD_SWAPW(ss_tval[0].ss_tv_sec);
	    tval[0].tv_usec = MD_SWAPW(ss_tval[0].ss_tv_usec);
	    tval[1].tv_sec = MD_SWAPW(ss_tval[1].ss_tv_sec);
	    tval[1].tv_usec = MD_SWAPW(ss_tval[1].ss_tv_usec);
#endif /* !_MSC_VER */

#if defined(hpux) || defined(__hpux) || defined(__svr4__)
	    /* no utimes() in hpux, use utime() instead */
	    {
	      struct utimbuf ubuf;

	      ubuf.actime = tval[0].tv_sec;
	      ubuf.modtime = tval[1].tv_sec;

	      /* result */regs->regs_R[2] = utime(buf, &ubuf);
	    }
#elif defined(_MSC_VER)
	    /* no utimes() in MSC, use utime() instead */
	    {
	      struct _utimbuf ubuf;

	      ubuf.actime = ss_tval[0].ss_tv_sec;
	      ubuf.modtime = ss_tval[1].ss_tv_sec;

	      /* result */regs->regs_R[2] = utime(buf, &ubuf);
	    }
#elif defined(__USLC__) || defined(unix) || defined(_AIX) || defined(__alpha)
	    /* result */regs->regs_R[2] = utimes(buf, tval);
#elif defined(__CYGWIN32__)
	    warn("syscall: called utimes()\n");
#else
#error No utimes() implementation!
#endif
	  }

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, indicate results */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }
      }
      break;

    case SS_SYS_getrlimit:
    case SS_SYS_setrlimit:
#ifdef _MSC_VER
      warn("syscall get/setrlimit() not yet implemented for MSC...");
      regs->regs_R[7] = 0;
#elif defined(__CYGWIN32__)
      warn("syscall: called get/setrlimit()\n");
      regs->regs_R[7] = 0;
#else
      {
	/* FIXME: check this..., was: struct rlimit ss_rl; */
	struct ss_rlimit ss_rl;
	struct rlimit rl;

	/* copy rlimit structure to host memory */
	mem_bcopy(mem_fn, mem, Read, /*rlimit*/regs->regs_R[5],
		  &ss_rl, sizeof(struct ss_rlimit));

	/* convert rlimit structure to host format */
	rl.rlim_cur = MD_SWAPW(ss_rl.ss_rlim_cur);
	rl.rlim_max = MD_SWAPW(ss_rl.ss_rlim_max);

	/* get rlimit information */
	if (syscode == SS_SYS_getrlimit)
	  /*result*/regs->regs_R[2] = getrlimit(regs->regs_R[4], &rl);
	else /* syscode == SS_SYS_setrlimit */
	  /*result*/regs->regs_R[2] = setrlimit(regs->regs_R[4], &rl);

	/* check for an error condition */
	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    /* got an error, indicate results */
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* convert rlimit structure to target format */
	ss_rl.ss_rlim_cur = MD_SWAPW(rl.rlim_cur);
	ss_rl.ss_rlim_max = MD_SWAPW(rl.rlim_max);

	/* copy rlimit structure to target memory */
	mem_bcopy(mem_fn, mem, Write, /*rlimit*/regs->regs_R[5],
		  &ss_rl, sizeof(struct ss_rlimit));
      }
#endif
      break;

#if 0
    case SS_SYS_getdirentries:
      /* FIXME: this is currently broken due to incompatabilities in
	 disk directory formats */
      {
	unsigned int i;
	char *buf;
	int base;

	buf = (char *)calloc(/* nbytes */regs->regs_R[6] + 1, sizeof(char));
	if (!buf)
	  fatal("out of memory in SYS_getdirentries");

	/* copy in */
	for (i=0; i</* nbytes */regs->regs_R[6]; i++)
	  (*maf)(Read, /* buf */regs->regs_R[5]+i,
		 (unsigned char *)&buf[i], 1);
	(*maf)(Read, /* basep */regs->regs_R[7], (unsigned char *)&base, 4);

	/*cc*/regs->regs_R[2] =
	  getdirentries(/*fd*/regs->regs_R[4], buf,
			/*nbytes*/regs->regs_R[6], &base);

	if (regs->regs_R[2] != -1)
	  regs->regs_R[7] = 0;
	else
	  {
	    regs->regs_R[2] = errno;
	    regs->regs_R[7] = 1;
	  }

	/* copy out */
	for (i=0; i</* nbytes */regs->regs_R[6]; i++)
	  (*maf)(Write, /* buf */regs->regs_R[5]+i,
		 (unsigned char *)&buf[i], 1);
	(*maf)(Write, /* basep */regs->regs_R[7], (unsigned char *)&base, 4);

	free(buf);
      }
      break;

#endif

    default:
      panic("invalid/unimplemented system call encountered, code %d", syscode);
    }
#endif /* MD_CROSS_ENDIAN */
}

/*end of syscall.cc*/

/*
 * undef of TMIPS
 * */

#include "mipsudef.h"





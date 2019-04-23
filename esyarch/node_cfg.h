/*
 * node_cfg.h
 *
 *  Created on: 2016年4月23日
 *      Author: root
 */

#ifndef NODE_CFG_H_
#define NODE_CFG_H_
#include "host.h"
#include "misc.h"
#include "define.h"

//resource.h  执行单元/单元定义
#define MAX_RES_CLASSES		16
#define MAX_INSTS_PER_CLASS	8

extern char *nd_type[];
extern char *gpp_is[];

struct res_name
{
public:
	const char* IALU;// = "integer-ALU";
	const char* IMPU;// = "integer-MULT/DIV";
	const char* MEMPORT;// = "memory-port";
	const char* FPALU;// = "FP-addr";
	const char* FPMPU;// = "FP-MULT/DIV";

};
enum core_tp
{
	gppcore,
	fpgaasic
};

//用于规定输入文件输入次序的结构体，也用于实力化对象
typedef struct
{
/*with out processor architecture*/
	int nid;//nodeid
	int mastid;//masterid
	int isrun;
	char* node_t;
	int tileid;
	int frequence;
	char* isa;
	char* testpath;//BNC path
/*processor architecture*/
	//杂项
	unsigned long long max_insts;
//	char *ptrace_opts[2]; ptrace在代码中配置是否需要，而不是配置文件中
	int fetch_speed;//1
	char *bpred_spec_opt;//NULL/ID/WB
	char *pred_type;//perfect/taken/nottaken/bimod(default)
	int bimod_config[1];// = {2048};/* bimod tbl size */
	int twolev_config[4];// = { /* l1size */1, /* l2size */1024, /* hist */8, /* xor */FALSE };
	//2-level predictor config (<l1size> <l2size> <hist_size> <xor>)
	int comb_config[1];// = {1024};/* meta_table_size */
	int ras_size;// = 8;//return address stack size (0 for no return stack)
	int btb_config[2];// = { /* nsets */512, /* assoc */4 };//BTB config (<num_sets> <associativity>)
	//mem & mc
	int mem_lat[2];// = { /* lat to first chunk */18, /* lat between remaining chunks */2 };
	char *itlb_opt;// = "itlb:16:4096:4:l";
	char *dtlb_opt;// = "dtlb:32:4096:4:l";
	int tlb_miss_lat;// = 30;
	int mem_bus_width;// = 4;
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
	int cache_dl1_lat;// = 1;
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
//	char *pcstat_vars[MAX_PCSTAT_VARS];
//	int pcstat_nelt = 0;
}CoreStruct;

typedef struct
{
	int nid;
	int masterid;
	int isrun;
	int tileid;
	int frequence;
	char* name;
	asic_t t;
	int cycle;
}FpgaStruct;




#endif /* NODE_CFG_H_ */

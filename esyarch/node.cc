/*
 * node.cc
 *
 *  Created on: 2016年2月28日
 *      Author: root
 */
#include <stdarg.h>
#include <time.h>
#include "node.h"
extern char *asic_type[];
extern time_t sim_starttime;
short int TNode::Exitstates[MAXTHREADS] = {0};
int TNode::frequenceholder_lcm = 1;
//gpp配置
TNode::TNode(int exist_t,CoreStruct* cs):exist(exist_t),sim_cycle(0),frequence_holder(1),node_endtime(0),sim_node_time(0),
		ntype(gppcore),name(NULL){
	this->exist = exist_t;
	this->actualid = cs->nid;
	this->masterid = cs->mastid;
	this->running = cs->isrun;
	this->tileid = cs->tileid;
	this->frequence = cs->frequence;
	this->cycle_time = 1.0/cs->frequence;
	if(this->running){
		Exitstates[actualid] = 0;
	}
	//功耗输出文件
	powerout_fb = (char*)malloc(50*sizeof(char));
	memset(powerout_fb,'\0',50);
	//mcpat输入的xml文件，路径相对于mcpat
	powerxml_4mcpat = (char*)malloc(50*sizeof(char));
	memset(powerxml_4mcpat,'\0',50);
	sim_start_time = time(NULL);
	sim_end_time = 0;
	sim_exit_now = FALSE;
	sim_dump_stats = FALSE;
	sim_in = sim_out = NULL;
	start_cycle = finish_cycle = fastfwd_count =0;
	regstatdb();//注册数据库
	thispower = new Power_mc("gpp_nd",1);//注册功耗参数统计/功耗类型定义类
}
TNode::TNode(int exist_t, FpgaStruct* fs):sim_cycle(0),node_endtime(0),sim_node_time(0),frequence_holder(1),
		ntype(fpgaasic),name(fs->name){
	exist = exist_t;
	actualid = fs->nid;
	masterid = fs->masterid;
	running = fs->isrun;
	tileid = fs->tileid;
	frequence = fs->frequence;
	sim_start_time = time(NULL);
	sim_exit_now = FALSE;
	sim_dump_stats = FALSE;
	sim_in = sim_out = NULL;
	start_cycle = 0;
	finish_cycle = 0;
	fastfwd_count = 0;
}


sim_rt_t TNode::run(){
	/*nada*/
}

//读取参数，进行对core节点结构体的赋值工作
CoreStruct* TNode::New_corecfg(char * cfg[]){
	CoreStruct* cs = (CoreStruct*)malloc(sizeof(CoreStruct));
	int temp = 0;
	//debug const pointer
	char bimod_t[] = "bimod";
#define STARTCHAR	0
#define NXC (cfg[STARTCHAR+(temp++)])
#define XC (cfg[STARTCHAR+(temp-1)])
	cs->nid = atoi(NXC);
	cs->mastid = atoi(NXC);
	cs->isrun = atoi(NXC);
	cs->node_t = NXC;
	cs->tileid = atoi(NXC);
	cs->frequence = atoi(NXC)?atoi(XC):1;
	cs->isa = NXC;
	cs->testpath = NXC;
	cs->max_insts = (unsigned long long)atoi(NXC)?(unsigned long long)atoi(XC):0;
	cs->fetch_speed = atoi(NXC)?atoi(XC):1;
	cs->bpred_spec_opt = NXC;
	cs->pred_type = NXC?XC:bimod_t;
	cs->bimod_config[0] = atoi(NXC);
	cs->twolev_config[0] = atoi(NXC);
	cs->twolev_config[1] = atoi(NXC);
	cs->twolev_config[2] = atoi(NXC);
	cs->twolev_config[3] = atoi(NXC);
	cs->comb_config[0] = atoi(NXC);
	cs->ras_size = atoi(NXC);
	cs->btb_config[0] = atoi(NXC);
	cs->btb_config[1] = atoi(NXC);
	cs->mem_lat[0] = atoi(NXC);
	cs->mem_lat[1] = atoi(NXC);
	cs->itlb_opt = NXC;
	cs->dtlb_opt = NXC;
	cs->tlb_miss_lat = atoi(NXC);
	cs->mem_bus_width = atoi(NXC);
	cs->ruu_ifq_size = atoi(NXC);
	cs->ruu_branch_penalty = atoi(NXC);
	cs->ruu_decode_width = atoi(NXC);
	cs->ruu_issue_width = atoi(NXC);
	cs->ruu_inorder_issue = atoi(NXC);
	cs->ruu_include_spec = atoi(NXC);
	cs->ruu_commit_width = atoi(NXC);
	cs->RUU_size = atoi(NXC);
	cs->LSQ_size = atoi(NXC);
	cs->cache_dl1_opt = NXC;
	cs->cache_dl1_lat = atoi(NXC);
	cs->cache_dl2_opt = NXC;
	cs->cache_dl2_lat = atoi(NXC);
	cs->cache_il1_opt = NXC;
	cs->cache_il1_lat = atoi(NXC);
	cs->cache_il2_opt = NXC;
	cs->cache_il2_lat = atoi(NXC);
	cs->flush_on_syscalls = atoi(NXC);
	cs->compress_icache_addrs = atoi(NXC);
	cs->res_ialu = atoi(NXC);
	cs->res_imult = atoi(NXC);
	cs->res_fpalu = atoi(NXC);
	cs->res_fpmult = atoi(NXC);
	cs->res_memport = atoi(NXC);
#undef STARTCHAR
#undef NXC
	return cs;
}


//读取参数，进行对core节点结构体的赋值工作
CoreStruct* TNode::New_corecfg(const EsySoCCfgTile & cfg)
{
    CoreStruct* cs = (CoreStruct*)malloc(sizeof(CoreStruct));
    int temp = 0;
    //debug const pointer
    char bimod_t[] = "bimod";

    cs->nid = cfg.niId();
    cs->mastid = cfg.mpiId();
    cs->isrun = cfg.tileEnable()?1:0;
    cs->node_t = nd_type[0];
    cs->tileid = cfg.tileId();
    cs->frequence = cfg.tileFreq()?cfg.tileFreq():1;
    cs->isa = gpp_is[(int)(cfg.coreIsaType())];
    cs->testpath = new char[100];
    strcpy(cs->testpath, cfg.coreAppDir().c_str());
    cs->max_insts = (unsigned long long)cfg.coreMaxInst()?
        (unsigned long long)cfg.coreMaxInst():0;
    cs->fetch_speed = cfg.coreFetchSpeed()?cfg.coreFetchSpeed():1;
    cs->bpred_spec_opt = new char[100];
    strcpy(cs->bpred_spec_opt, cfg.predUpdateType().c_str());
    cs->pred_type = new char[100];
    if (cfg.predPredictType().size() > 0)
    {
        strcpy(cs->pred_type, cfg.predPredictType().c_str());
    }
    else
    {
        strcpy(cs->pred_type, bimod_t);
    }
    cs->bimod_config[0] = cfg.predBimodSize();
    cs->twolev_config[0] = cfg.predL1Size();
    cs->twolev_config[1] = cfg.predL2Size();
    cs->twolev_config[2] = cfg.predL2HistSize();
    cs->twolev_config[3] = cfg.predL2XorEnable()?1:0;
    cs->comb_config[0] = cfg.metaSize();
    cs->ras_size = cfg.returnStackSize();
    cs->btb_config[0] = cfg.btbSet();
    cs->btb_config[1] = cfg.btbCombination();
    cs->mem_lat[0] = cfg.firstChunkDelay();
    cs->mem_lat[1] = cfg.neiberChunkDelay();
    cs->itlb_opt = new char[100];
    strcpy(cs->itlb_opt, cfg.tlbItlbCfg().c_str());
    cs->dtlb_opt = new char[100];
    strcpy(cs->dtlb_opt, cfg.tlbDtlbCfg().c_str());
    cs->tlb_miss_lat = cfg.tlbMissDelay();
    cs->mem_bus_width = cfg.memoryBusWidth();
    cs->ruu_ifq_size = cfg.ruuIfqSize();
    cs->ruu_branch_penalty = cfg.ruuBranchPenalt();
    cs->ruu_decode_width = cfg.ruuDecoderWidth();
    cs->ruu_issue_width = cfg.ruuIfqSize();
    cs->ruu_inorder_issue = cfg.ruuInorderIssu();
    cs->ruu_include_spec = cfg.ruuIncludeSpec();
    cs->ruu_commit_width = cfg.ruuCommitWidth();
    cs->RUU_size = cfg.ruuSize();
    cs->LSQ_size = cfg.lsqSize();
    cs->cache_dl1_opt = new char[100];
    strcpy(cs->cache_dl1_opt, cfg.cacheDl1Cfg().c_str());
    cs->cache_dl1_lat = cfg.cacheDl1Delay();
    cs->cache_dl2_opt = new char[100];
    strcpy(cs->cache_dl2_opt, cfg.cacheDl2Cfg().c_str());
    cs->cache_dl2_lat = cfg.cacheDl2Delay();
    cs->cache_il1_opt = new char[100];
    strcpy(cs->cache_il1_opt, cfg.cacheIl1Cfg().c_str());
    cs->cache_il1_lat = cfg.cacheIl1Delay();
    cs->cache_il2_opt = new char[100];
    strcpy(cs->cache_il2_opt, cfg.cacheIl2Cfg().c_str());
    cs->cache_il2_lat = cfg.cacheIl2Delay();
    cs->flush_on_syscalls = 0;
    cs->compress_icache_addrs = cfg.cacheICompressEnable();
    cs->res_ialu = cfg.integarAdderUnit();
    cs->res_imult = cfg.integarMultiUnit();
    cs->res_fpalu = cfg.floatAddrUnit();
    cs->res_fpmult = cfg.floatMultiUnit();
    cs->res_memport = cfg.memoryPort();

    return cs;
}

FpgaStruct* TNode::New_fpgacfg(char * cfg[])
{
	FpgaStruct* fs = (FpgaStruct*)malloc(sizeof(FpgaStruct));
	fs->nid = atoi(cfg[0]);
	fs->masterid = atoi(cfg[1]);
	fs->isrun = atoi(cfg[2]);
	fs->tileid = atoi(cfg[4]);
	fs->frequence = atoi(cfg[5]);
	if(!strcmp(cfg[6], "asic_fft_128")){
				fs->t = asic_fft_128;
	}
	fs->name = cfg[7];
	fs->cycle = atoi(cfg[8]);
	return fs;
}

FpgaStruct* TNode::New_fpgacfg(const EsySoCCfgTile & cfg)
{
    FpgaStruct* fs = (FpgaStruct*)malloc(sizeof(FpgaStruct));
    fs->nid = cfg.niId();
    fs->masterid = cfg.mpiId();
    fs->isrun = cfg.tileEnable()?1:0;
    fs->tileid = cfg.tileId();
    fs->frequence = cfg.tileFreq();
    if(!strcmp(asic_type[(asic_t)cfg.asicType()], "asic_fft_128"))
    {
                fs->t = asic_fft_128;
    }
    fs->name = new char[100];
    strcpy(fs->name, cfg.asicName().c_str());
    fs->cycle = cfg.asicDelay();
    return fs;
}


extern long long int sim_step;
extern int now_run;
extern TNode* sim_node[];
void TNode::idle(int cycle){
	sim_cycle+=cycle;
}

void TNode::regstatdb(){
	statcpnt = new Statistics();
	statcpnt->statdb = statcpnt->stat_new();
}

short int TNode::Getexitstates(){
	short int i,exit = 1;
	for(i = 0; i< MAXTHREADS; i++){
		exit = exit & Exitstates[i];
	}
	time(&node_endtime);
	if(sizeof(int) == 4)
		sim_node_time = (node_endtime - sim_starttime>0?node_endtime - sim_starttime:1) & 0xffffffff;
	else if(sizeof(int) == 2)
		sim_node_time = (node_endtime - sim_starttime>0?node_endtime - sim_starttime:1) & 0xffff;
	return exit;
}
void TNode::Setexitarray(){
	int i;
	for(i = 0; i< MAXTHREADS; i++){
		Exitstates[i] = 1;
	}
}

TNode::~TNode() {
}




/*
 * core.cc
 *
 *  Created on: 2016年2月29日
 *      Author: root
 */
#include <iostream>
#include <string.h>
#include "MPITable.h"
#include "node.h"
using namespace std;

extern MPITable *mt;
/*
 * sim-outorder.c define
 * */
#define MAX_RS_LINKS                    4096

TCore::TCore(int exist,CoreStruct* cs,int thread_id)
:TNode(exist,cs),eio_trans_icnt(-1),ptrace_outfd(NULL),ptrace_oneshot(FALSE),
 sim_swap_bytes(0),sim_swap_words(0),sim_eio_fname(NULL),sim_chkpt_fname(NULL),sim_eio_fd(NULL),
 sim_simout(NULL),sim_progin(NULL),sim_progout(NULL),sim_progfd(NULL),bugcompat_mode(0),ruu_fetch_issue_delay(0),
 spec_mode(FALSE),pred_perfect(FALSE),pref_mode(0),inst_seq(0),ptrace_seq(0),threadid(thread_id)
 {
	ptrace_active = 0;
	last_inst_missed = last_inst_tmissed = 0;
//	mt->Id_table_assign(thread_id,masterid,actualid);
	Creat_core(cs);
	statcpnt = new Statistics();
}
void TCore::Creat_core(CoreStruct* cs){
#define CSAUG(A)   A=cs->A
	thread_id = cs->nid;
	testpath = cs->testpath;
	max_insts = cs->max_insts;
	fetch_speed = cs->fetch_speed;
	pred_type = cs->pred_type;
	bpred_spec_opt = cs->bpred_spec_opt;
	if(!cs->bpred_spec_opt || !strcmp(cs->bpred_spec_opt," "))
		bpred_spec_update = spec_CT;
	else if(!strcmp(cs->bpred_spec_opt,"ID"))
		bpred_spec_update = spec_ID;
	else if(!strcmp(cs->bpred_spec_opt,"WB"))
		bpred_spec_update = spec_WB;
	else
		fatal("bad speculative update stage specifier, use {ID|WB}");
	bimod_config[0] = cs->bimod_config[0];
	bimod_config[1] = cs->bimod_config[1];
	twolev_config[0] = cs->twolev_config[0];
	twolev_config[1] = cs->twolev_config[1];
	twolev_config[2] = cs->twolev_config[2];
	twolev_config[3] = cs->twolev_config[3];
	comb_config[0] = cs->comb_config[0];
	comb_config[1] = cs->comb_config[1];
	ras_size = cs->ras_size;
	btb_config[0] = cs->btb_config[0];
	btb_config[1] = cs->btb_config[1];
	mem_lat[0] = cs->mem_lat[0];
	mem_lat[1] = cs->mem_lat[1];
	itlb_opt = cs->itlb_opt;
	dtlb_opt = cs->dtlb_opt;
	tlb_miss_lat = cs->tlb_miss_lat;
	mem_bus_width = cs->mem_bus_width;
	//ruu
	ruu_ifq_size = cs->ruu_ifq_size;
	ruu_branch_penalty = cs->ruu_branch_penalty;
	ruu_decode_width = cs->ruu_decode_width;
	ruu_issue_width = cs->ruu_issue_width;
    ruu_inorder_issue = cs->ruu_inorder_issue;
	ruu_include_spec = cs->ruu_include_spec;
	ruu_commit_width = cs->ruu_commit_width;
	RUU_size = cs->RUU_size;
	LSQ_size = cs->LSQ_size;
	//cache
	cache_dl1_opt = cs->cache_dl1_opt;
	cache_dl1_lat = cs->cache_dl1_lat;
	cache_dl2_opt = cs->cache_dl2_opt;
	cache_dl2_lat = cs->cache_dl2_lat;
	cache_il1_opt = cs->cache_il1_opt;
	cache_il1_lat = cs->cache_il1_lat;
	cache_il2_opt = cs->cache_il2_opt;
	cache_il2_lat = cs->cache_il2_lat;
	flush_on_syscalls = cs->flush_on_syscalls;
	compress_icache_addrs = cs->compress_icache_addrs;
	//执行单元
	res_ialu = cs->res_ialu;
	res_imult = cs->res_imult;
	res_fpalu = cs->res_fpalu;
	res_fpmult = cs->res_fpmult;
	res_memport = cs->res_memport;
	//资源池在子类
	//流水线踪迹
	ptrace_nelt = 0;
	ptrace_opts[0] = NULL;
	ptrace_opts[1] = NULL;
#undef CSAUG(A)
}

sim_rt_t TCore::run(){
	/*nada*/
}


void TCore::sim_aux_config(FILE *stream)            /* output stream */
{
  /* nada */
}




//int TCore::Getmythreadid(){
//	return threadid;
//}
TCore::~TCore(){
	cout<<"Tcore deconstructor has been called"<<endl;
}






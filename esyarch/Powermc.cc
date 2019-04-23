/*
 * Powermc.cpp
 *
 *  Created on: 2016年5月8日
 *      Author: root
 */
#include "host.h"
#include "misc.h"
#include "define.h"
#include "node_cfg.h"
#include "node.h"
#include "power_mc.h"

Power_mc::Power_mc(const char* nodetype,int has_temp = 1) {
	// TODO Auto-generated constructor stub
	// 初始化，成员变量赋值
	if(!strcmp(nodetype,nd_type[0]))
    {//core类
		gpp_data = mc_gpppara_create();
		mcpat_init(0);
		if(has_temp){
			temp_data = mc_gpppara_create();
			mcpat_init(1);
		}
		corepower = gpppowertype_create();
	}//core类实例完毕
	else
    {

	}
}
Power_mc::Power_mc()
{
	corepower = gpppowertype_create();
	gpp_data = temp_data = NULL;
}
//创建gpp节点内的统计参数载体
struct core_para *
Power_mc::mc_core_create(void)
{
	struct core_para *corep;
	corep = (struct core_para *)calloc(1,sizeof(struct core_para));
	if (!corep)
	  fatal("out of virtual memory");
	return corep;
}
struct mem_para *
Power_mc::mc_mem_create(void)
{
	struct mem_para *memp;
	memp = (struct mem_para *)calloc(1,sizeof(struct mem_para));
	if (!memp)
	  fatal("out of virtual memory");
	return memp;
}
struct cache_para *
Power_mc::mc_cache_create(void)
{
	struct cache_para *cachep;
	cachep = (struct cache_para *)calloc(1,sizeof(struct cache_para));
	if (!cachep)
	  fatal("out of virtual memory");
	return cachep;
}
struct tlb_para *
Power_mc::mc_tlb_create(void)
{
	struct tlb_para *tlbp;
	tlbp = (struct tlb_para *)calloc(1,sizeof(struct tlb_para));
	if (!tlbp)
	  fatal("out of virtual memory");
	return tlbp;
}
struct btb_para *
Power_mc::mc_btb_create(void)
{
	struct btb_para *btbp;
	btbp = (struct btb_para *)calloc(1,sizeof(struct btb_para));
	if (!btbp)
	  fatal("out of virtual memory");
	return btbp;
}
struct noc_para *
Power_mc::mc_noc_create(void)
{
	struct noc_para *nocp;
	nocp = (struct noc_para *)calloc(1,sizeof(struct noc_para));
	if (!nocp)
	  fatal("out of virtual memory");
	return nocp;
}
struct regs_para *
Power_mc::mc_regs_create(void)
{
	struct regs_para *regsp;
	regsp = (struct regs_para *)calloc(1,sizeof(struct regs_para));
	if (!regsp)
	  fatal("out of virtual memory");
	return regsp;
}
//创建gpp类的统计参数载体
gpp_node_para *
Power_mc::mc_gpppara_create()
{
	gpp_node_para* g = (gpp_node_para*)malloc(sizeof(gpp_node_para));
	if(!g)
		fatal("out of virtual memory");
	g->btb_para = mc_btb_create();
	g->cache_para = mc_cache_create();
	g->core_para = mc_core_create();
	g->mem_para = mc_mem_create();
	g->noc_para = mc_noc_create();
	g->regs_para = mc_regs_create();
	g->tlb_para = mc_tlb_create();
	return g;
}
//创建gpp内部功耗数值载体
struct power_processor* Power_mc::power_processor_create(void)
{
	struct power_processor* ptype;
	ptype = (struct power_processor*)calloc(1,sizeof(struct power_processor));
	return ptype;
}
void power_core_create(struct powertype* po)
{
	int corenum = 1;
	int corenum_t;
	for(corenum_t = 0;corenum_t<corenum;corenum_t++)
    {
        po->p_core[corenum_t] = 
            (struct power_core*)calloc(1,sizeof(struct power_core));
	}
}
void power_noc_create(struct powertype* po)
{
	int nocnum=1;
	int nocnum_t;
	for(nocnum_t = 0;nocnum_t<nocnum;nocnum_t++)
    {
		po->p_noc[nocnum_t] = 
            (struct power_noc*)calloc(1,sizeof(struct power_noc));
	}
}
//创建gpp功耗数值载体
struct powertype*
Power_mc::gpppowertype_create(void)
{
	struct powertype* ptype;
	ptype = (struct powertype*)calloc(1,sizeof(struct powertype));
	ptype->p_processor = power_processor_create();
	power_core_create(ptype);
	power_noc_create(ptype);
	return ptype;
}

//初始化功耗统计量
void Power_mc::mcpat_init(int type = 0)
{//type > 0: gpp_data+temp_data
	//core
	gpp_data->core_para->total_cycle_num = 0.0;
	gpp_data->core_para->total_inst_num = 0;
	gpp_data->core_para->total_res_call_num = 0.0;
	gpp_data->core_para->ifu_block_num = 0;
	gpp_data->core_para->homo = 0;
	gpp_data->core_para->total_branch_num = 0.0;
	gpp_data->core_para->total_branch_access_num = 0.0;
	gpp_data->core_para->total_branch_miss_num = 0.0;
	gpp_data->core_para->total_ialu_num = 0.0;
	gpp_data->core_para->total_imult_num = 0.0;
	gpp_data->core_para->total_idiv_num = 0.0;
	gpp_data->core_para->total_fpalu_num = 0.0;
	gpp_data->core_para->total_fpmult_num = 0.0;
	gpp_data->core_para->total_fpdiv_num = 0.0;
	//cache
	gpp_data->cache_para->total_icache_read_access_num = 0;
	gpp_data->cache_para->total_icache_read_miss_num = 0;
	gpp_data->cache_para->total_dcache_read_access_num = 0;
	gpp_data->cache_para->total_dcache_read_miss_num = 0;
	gpp_data->cache_para->total_dcache_write_access_num = 0;
	gpp_data->cache_para->total_dcache_write_miss_num = 0;
	gpp_data->cache_para->total_icache2_read_access_num = 0;
	gpp_data->cache_para->total_icache2_read_miss_num = 0;
	gpp_data->cache_para->total_dcache2_read_access_num = 0;
	gpp_data->cache_para->total_dcache2_read_miss_num = 0;
	gpp_data->cache_para->total_dcache2_write_access_num = 0;
	gpp_data->cache_para->total_dcache2_write_miss_num = 0;
	gpp_data->cache_para->l2cache_cycle_duty = 0;
	//noc
	//mem
	gpp_data->mem_para->total_memport_rd = 0.0;
	gpp_data->mem_para->total_memport_wt = 0.0;
	gpp_data->mem_para->total_load_num = 0.0;
	gpp_data->mem_para->total_store_num = 0.0;
	//regs
	gpp_data->regs_para->total_iregs_write_num = 0.0;
	gpp_data->regs_para->total_iregs_read_num = 0.0;
	gpp_data->regs_para->total_fregs_write_num = 0.0;
	gpp_data->regs_para->total_fregs_read_num = 0.0;
	gpp_data->regs_para->total_cregs_write_num = 0.0;
	gpp_data->regs_para->total_cregs_read_num = 0.0;
	//btb
	gpp_data->btb_para->total_btb_write = 0.0;
	gpp_data->btb_para->total_btb_read = 0.0;
	//tlb
	gpp_data->tlb_para->total_itlb_access_num = 0.0;
	gpp_data->tlb_para->total_itlb_miss_num = 0.0;
	gpp_data->tlb_para->total_dtlb_access_num = 0.0;
	gpp_data->tlb_para->total_dtlb_miss_num = 0.0;
	/*tmp*/
	if(type)
    {
		temp_data->core_para->total_cycle_num = 0.0;
		temp_data->core_para->total_inst_num = 0;
		temp_data->core_para->total_res_call_num = 0.0;
		temp_data->core_para->ifu_block_num = 0.0;
		//core
		temp_data->core_para->homo = 0;
		temp_data->core_para->total_branch_num = 0.0;
		temp_data->core_para->total_branch_access_num = 0.0;
		temp_data->core_para->total_branch_miss_num = 0.0;
		temp_data->core_para->total_ialu_num = 0.0;
		temp_data->core_para->total_imult_num = 0.0;
		temp_data->core_para->total_idiv_num = 0.0;
		temp_data->core_para->total_fpalu_num = 0.0;
		temp_data->core_para->total_fpmult_num = 0.0;
		temp_data->core_para->total_fpdiv_num = 0.0;
		//cache
		temp_data->cache_para->total_icache_read_access_num = 0;
		temp_data->cache_para->total_icache_read_miss_num = 0;
		temp_data->cache_para->total_dcache_read_access_num = 0;
		temp_data->cache_para->total_dcache_read_miss_num = 0;
		temp_data->cache_para->total_dcache_write_access_num = 0;
		temp_data->cache_para->total_dcache_write_miss_num = 0;
		temp_data->cache_para->total_icache2_read_access_num = 0;
		temp_data->cache_para->total_icache2_read_miss_num = 0;
		temp_data->cache_para->total_dcache2_read_access_num = 0;
		temp_data->cache_para->total_dcache2_read_miss_num = 0;
		temp_data->cache_para->total_dcache2_write_access_num = 0;
		temp_data->cache_para->total_dcache2_write_miss_num = 0;
		temp_data->cache_para->l2cache_cycle_duty = 0;
		//noc
		//mem
		temp_data->mem_para->total_memport_rd = 0.0;
		temp_data->mem_para->total_memport_wt = 0.0;
		temp_data->mem_para->total_load_num = 0.0;
		temp_data->mem_para->total_store_num = 0.0;
		//regs
		temp_data->regs_para->total_iregs_write_num = 0.0;
		temp_data->regs_para->total_iregs_read_num = 0.0;
		temp_data->regs_para->total_fregs_write_num = 0.0;
		temp_data->regs_para->total_fregs_read_num = 0.0;
		temp_data->regs_para->total_cregs_write_num = 0.0;
		temp_data->regs_para->total_cregs_read_num = 0.0;
		//btb
		temp_data->btb_para->total_btb_write = 0.0;
		temp_data->btb_para->total_btb_read = 0.0;
		//tlb
		temp_data->tlb_para->total_itlb_access_num = 0.0;
		temp_data->tlb_para->total_itlb_miss_num = 0.0;
		temp_data->tlb_para->total_dtlb_access_num = 0.0;
		temp_data->tlb_para->total_dtlb_miss_num = 0.0;
	}
}

//gpp_node_para* Power_mc::total_calt(TCore &fstcore){
//	gpp_node_para *t;
//	return t;
//}
void
Power_mc::add_coreinfo(FILE *p, int numthreads)
{
	int core_id = 0;
	for(core_id=0;core_id<numthreads;core_id++){
	fprintf(p,"\t\t<component id=\"system.core%d\" name=\"core\">\n",core_id);
	/* Core property */
	fprintf(p,"\t\t\t<param name=\"clock_rate\" value=\"2000\"/>\n");
	fprintf(p,"\t\t\t<param name=\"device_type\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t<param name=\"vdd\" value=\"0\"/>\n");/* 0 means using ITRS default vdd */
	/* for cores with unknown timing, set to 0 to force off the opt flag */
	fprintf(p,"\t\t\t<param name=\"opt_local\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t<param name=\"instruction_length\" value=\"32\"/>\n");
	fprintf(p,"\t\t\t<param name=\"opcode_width\" value=\"16\"/>\n");
	fprintf(p,"\t\t\t<param name=\"x86\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t<param name=\"micro_opcode_width\" value=\"8\"/>\n");
	fprintf(p,"\t\t\t<param name=\"machine_type\" value=\"0\"/>\n");/* inorder/OoO; 1 inorder; 0 OOO*/
	fprintf(p,"\t\t\t<param name=\"number_hardware_threads\" value=\"%d\"/>\n",1);
	/* number_instruction_fetch_ports(icache ports) is always 1 in single-thread processor,
			it only may be more than one in SMT processors. BTB ports always equals to fetch ports since
			branch information in consecutive branch instructions in the same fetch group can be read out from BTB once.*/
	fprintf(p,"\t\t\t<param name=\"fetch_width\" value=\"4\"/>\n");/* fetch_width determines the size of cachelines of L1 cache block */
	fprintf(p,"\t\t\t<param name=\"number_instruction_fetch_ports\" value=\"1\"/>\n");
	fprintf(p,"\t\t\t<param name=\"decode_width\" value=\"4\"/>\n");/* decode_width determines the number of ports of the renaming table (both RAM and CAM) scheme */
	fprintf(p,"\t\t\t<param name=\"issue_width\" value=\"4\"/>\n");/* issue_width determines the number of ports of Issue window and other
			logic as in the complexity effective processors paper; issue_width==dispatch_width */
	fprintf(p,"\t\t\t<param name=\"peak_issue_width\" value=\"2\"/>\n");//no
	fprintf(p,"\t\t\t<param name=\"commit_width\" value=\"2\"/>\n");/* commit_width determines the number of ports of register files */
	fprintf(p,"\t\t\t<param name=\"fp_issue_width\" value=\"2\"/>\n");
	fprintf(p,"\t\t\t<param name=\"prediction_width\" value=\"1\"/>\n");
	/* number of branch instructions can be predicted simultaneously*/
	/* Current version of McPAT does not distinguish int and floating point pipelines
			Theses parameters are reserved for future use.*/
	fprintf(p,"\t\t\t<param name=\"pipelines_per_core\" value=\"1,1\"/>\n");/*integer_pipeline and floating_pipelines, if the floating_pipelines is 0, then the pipeline is shared*/
	fprintf(p,"\t\t\t<param name=\"pipeline_depth\" value=\"5,5\"/>\n");/* pipeline depth of int and fp, if pipeline is shared, the second number is the average cycles of fp ops */
	/* issue and exe unit*/
	fprintf(p,"\t\t\t<param name=\"ALU_per_core\" value=\"4\"/>\n");/* contains an adder, a shifter, and a logical unit */
	fprintf(p,"\t\t\t<param name=\"MUL_per_core\" value=\"1\"/>\n");/* For MUL and Div */
	fprintf(p,"\t\t\t<param name=\"FPU_per_core\" value=\"4\"/>\n");/* buffer between IF and ID stage */
	fprintf(p,"\t\t\t<param name=\"instruction_buffer_size\" value=\"32\"/>\n");/* buffer between ID and sche/exe stage */
	fprintf(p,"\t\t\t<param name=\"decoded_stream_buffer_size\" value=\"32\"/>\n");
	fprintf(p,"\t\t\t<param name=\"instruction_window_scheme\" value=\"0\"/>\n");/* 0 PHYREG based, 1 RSBASED*/
	/* McPAT support 2 types of OoO cores, RS based and physical reg based*/
	fprintf(p,"\t\t\t<param name=\"instruction_window_size\" value=\"32\"/>\n");
	fprintf(p,"\t\t\t<param name=\"fp_instruction_window_size\" value=\"32\"/>\n");
	/* Numbers need to be confirmed */
	/* the instruction issue Q as in Alpha 21264; The RS as in Intel P6 */
	fprintf(p,"\t\t\t<param name=\"ROB_size\" value=\"0\"/>\n");/* each in-flight instruction has an entry in ROB */
	/* registers */
	fprintf(p,"\t\t\t<param name=\"archi_Regs_IRF_size\" value=\"32\"/>\n");
	fprintf(p,"\t\t\t<param name=\"archi_Regs_FRF_size\" value=\"32\"/>\n");
	/*  if OoO processor, phy_reg number is needed for renaming logic, renaming logic is for both integer and floating point insts.  */
	fprintf(p,"\t\t\t<param name=\"phy_Regs_IRF_size\" value=\"64\"/>\n");
	fprintf(p,"\t\t\t<param name=\"phy_Regs_FRF_size\" value=\"64\"/>\n");
	/* rename logic */
	fprintf(p,"\t\t\t<param name=\"rename_scheme\" value=\"0\"/>\n");
	/* can be RAM based(0) or CAM based(1) rename scheme
			RAM-based scheme will have free list, status table;
			CAM-based scheme have the valid bit in the data field of the CAM
			both RAM and CAM need RAM-based checkpoint table, checkpoint_depth=# of in_flight instructions;
			Detailed RAT Implementation see TR */
	fprintf(p,"\t\t\t<param name=\"register_windows_size\" value=\"0\"/>\n");
	/* how many windows in the windowed register file, sun processors;
			no register windowing is used when this number is 0 */
			/* In OoO cores, loads and stores can be issued whether inorder(Pentium Pro) or (OoO)out-of-order(Alpha),
			They will always try to execute out-of-order though. */
	fprintf(p,"\t\t\t<param name=\"LSU_order\" value=\"inorder\"/>\n");
	fprintf(p,"\t\t\t<param name=\"store_buffer_size\" value=\"4\"/>\n");
	/* By default, in-order cores do not have load buffers */
	fprintf(p,"\t\t\t<param name=\"load_buffer_size\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t<param name=\"memory_ports\" value=\"2\"/>\n");//ok
	/* max_allowed_in_flight_memo_instructions determines the # of ports of load and store buffer
			as well as the ports of Dcache which is connected to LSU */
	/* dual-pumped Dcache can be used to save the extra read/write ports */
	fprintf(p,"\t\t\t<param name=\"RAS_size\" value=\"32\"/>\n");
	/* general stats, defines simulation periods;require total, idle, and busy cycles for sanity check  */
	/* please note: if target architecture is X86, then all the instructions refer to (fused) micro-ops */
	fprintf(p,"\t\t\t<stat name=\"total_instructions\" value=\"%d\"/>\n",gpp_data->core_para->total_inst_num);
	/*fprintf(p,"\t\t\t<stat name=\"int_instructions\" value=\"200000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"fp_instructions\" value=\"100000\"/>\n");*/
	fprintf(p,"\t\t\t<stat name=\"branch_instructions\" value=\"%.0f\"/>\n",gpp_data->core_para->total_branch_miss_num+gpp_data->core_para->total_branch_access_num);
	fprintf(p,"\t\t\t<stat name=\"branch_mispredictions\" value=\"%.0f\"/>\n",gpp_data->core_para->total_branch_miss_num);
	fprintf(p,"\t\t\t<stat name=\"load_instructions\" value=\"%.0f\"/>\n",gpp_data->mem_para->total_load_num);
	fprintf(p,"\t\t\t<stat name=\"store_instructions\" value=\"%.0f\"/>\n",gpp_data->mem_para->total_store_num);
	//cycle relative
	fprintf(p,"\t\t\t<stat name=\"total_cycles\" value=\"%.0f\"/>\n",gpp_data->core_para->total_cycle_num);
	fprintf(p,"\t\t\t<stat name=\"busy_cycles\" value=\"%.0f\"/>\n",gpp_data->core_para->total_cycle_num*0.76);//added by lch -- note:wrong
	fprintf(p,"\t\t\t<stat name=\"idle_cycles\" value=\"%.0f\"/>\n",gpp_data->core_para->total_cycle_num*0.24);//added by lch -- note:wrong
	/*  RF accesses */
	fprintf(p,"\t\t\t<stat name=\"int_regfile_reads\" value=\"%.0f\"/>\n",gpp_data->regs_para->total_iregs_read_num);
	fprintf(p,"\t\t\t<stat name=\"float_regfile_reads\" value=\"%.0f\"/>\n",gpp_data->regs_para->total_fregs_read_num);
	fprintf(p,"\t\t\t<stat name=\"int_regfile_writes\" value=\"%.0f\"/>\n",gpp_data->regs_para->total_iregs_write_num);
	fprintf(p,"\t\t\t<stat name=\"float_regfile_writes\" value=\"%.0f\"/>\n",gpp_data->regs_para->total_fregs_write_num);
	/* Alu stats by default, the processor has one FPU that includes the divider and
			 multiplier. The fpu accesses should include accesses to multiplier and divider  */
	fprintf(p,"\t\t\t<stat name=\"ialu_accesses\" value=\"%.0f\"/>\n",gpp_data->core_para->total_ialu_num);
	fprintf(p,"\t\t\t<stat name=\"mul_accesses\" value=\"%.0f\"/>\n",gpp_data->core_para->total_imult_num+gpp_data->core_para->total_fpmult_num);
	fprintf(p,"\t\t\t<stat name=\"fpu_accesses\" value=\"%.0f\"/>\n",gpp_data->core_para->total_fpalu_num);
	fprintf(p,"\t\t\t<stat name=\"cdb_alu_accesses\" value=\"%.0f\"/>\n",gpp_data->core_para->total_ialu_num);
	fprintf(p,"\t\t\t<stat name=\"cdb_mul_accesses\" value=\"%.0f\"/>\n",gpp_data->core_para->total_imult_num+gpp_data->core_para->total_fpmult_num);
	fprintf(p,"\t\t\t<stat name=\"cdb_fpu_accesses\" value=\"%.0f\"/>\n",gpp_data->core_para->total_fpalu_num);
	/*
	fprintf(p,"\t\t\t<stat name=\"committed_instructions\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"committed_int_instructions\" value=\"200000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"committed_fp_instructions\" value=\"100000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"pipeline_duty_cycle\" value=\"1\"/>\n");			\A1\CC
	fprintf(p,"\t\t\t<stat name=\"total_cycles\" value=\"100000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"idle_cycles\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"busy_cycles\"  value=\"100000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"ROB_reads\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"committed_instructions\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"ROB_writes\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"rename_reads\" value=\"800000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"rename_writes\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"fp_rename_reads\" value=\"200000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"fp_rename_writes\" value=\"100000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"inst_window_reads\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"inst_window_writes\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"inst_window_wakeup_accesses\" value=\"800000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"fp_inst_window_reads\" value=\"200000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"fp_inst_window_writes\" value=\"200000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"fp_inst_window_wakeup_accesses\" value=\"400000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"int_regfile_reads\" value=\"600000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"float_regfile_reads\" value=\"100000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"int_regfile_writes\" value=\"300000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"float_regfile_writes\" value=\"50000\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"function_calls\" value=\"5\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"context_switches\" value=\"260343\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"ialu_accesses\" value=\"300000\"/>\n");			\A1\CC
	fprintf(p,"\t\t\t<stat name=\"fpu_accesses\" value=\"100000\"/>\n");			\A1\CC
	fprintf(p,"\t\t\t<stat name=\"mul_accesses\" value=\"200000\"/>\n");			\A1\CC
	fprintf(p,"\t\t\t<stat name=\"cdb_alu_accesses\" value=\"300000\"/>\n");        \A1\CC
	fprintf(p,"\t\t\t<stat name=\"cdb_mul_accesses\" value=\"200000\"/>\n");		\A1\CC
	fprintf(p,"\t\t\t<stat name=\"cdb_fpu_accesses\" value=\"100000\"/>\n");		\A1\CC
	fprintf(p,"\t\t\t<stat name=\"IFU_duty_cycle\" value=\"1\"/>\n");				\A1\CC
	fprintf(p,"\t\t\t<stat name=\"LSU_duty_cycle\" value=\"0.5\"/>\n");				\A1\CC
	fprintf(p,"\t\t\t<stat name=\"MemManU_I_duty_cycle\" value=\"1\"/>\n");			\A1\CC
	fprintf(p,"\t\t\t<stat name=\"MemManU_D_duty_cycle\" value=\"0.5\"/>\n");		\A1\CC
	fprintf(p,"\t\t\t<stat name=\"ALU_duty_cycle\" value=\"1\"/>\n");				\A1\CC
	fprintf(p,"\t\t\t<stat name=\"MUL_duty_cycle\" value=\"0.3\"/>\n");				\A1\CC
	fprintf(p,"\t\t\t<stat name=\"FPU_duty_cycle\" value=\"0.3\"/>\n");				\A1\CC
	fprintf(p,"\t\t\t<stat name=\"ALU_cdb_duty_cycle\" value=\"1\"/>\n");			\A1\CC
	fprintf(p,"\t\t\t<stat name=\"MUL_cdb_duty_cycle\" value=\"0.3\"/>\n");			\A1\CC
	fprintf(p,"\t\t\t<stat name=\"FPU_cdb_duty_cycle\" value=\"0.3\"/>\n");			\A1\CC
	*/
	/*fprintf(p,"\t\t\t<stat name=\"pipeline_duty_cycle\" value=\"1\"/>\n");//<=1, runtime_ipc/peak_ipc; averaged for all cores if homogeneous
	fprintf(p,"\t\t\t<stat name=\"IFU_duty_cycle\" value=\"0.9\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"LSU_duty_cycle\" value=\"0.71\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"BR_duty_cycle\" value=\"0.72\"/>\n");//branch		//newone
	fprintf(p,"\t\t\t<stat name=\"MemManU_I_duty_cycle\" value=\"0.9\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"MemManU_D_duty_cycle\" value=\"0.72\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"ALU_duty_cycle\" value=\"0.76\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"MUL_duty_cycle\" value=\"0.82\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"FPU_duty_cycle\" value=\"0.0\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"ALU_cdb_duty_cycle\" value=\"0.76\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"MUL_cdb_duty_cycle\" value=\"0.82\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"FPU_cdb_duty_cycle\" value=\"0.0\"/>\n");
	*/
	/*duty set as follow:--lch*/
	fprintf(p,"\t\t\t<stat name=\"pipeline_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?1.0-gpp_data->core_para->ifu_block_num/gpp_data->core_para->total_cycle_num:0.00);//<=1, runtime_ipc/peak_ipc; averaged for all cores if homogeneous
	fprintf(p,"\t\t\t<stat name=\"IFU_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?1.0-gpp_data->core_para->ifu_block_num/gpp_data->core_para->total_cycle_num:0.00);
	fprintf(p,"\t\t\t<stat name=\"LSU_duty_cycle\" value=\"0.71\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"BR_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?gpp_data->btb_para->total_btb_read/gpp_data->core_para->total_cycle_num:0.00);//branch		//newone
	fprintf(p,"\t\t\t<stat name=\"MemManU_I_duty_cycle\" value=\"0.9\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"MemManU_D_duty_cycle\" value=\"0.72\"/>\n");
	fprintf(p,"\t\t\t<stat name=\"ALU_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?(gpp_data->core_para->total_ialu_num+gpp_data->core_para->total_fpalu_num)/gpp_data->core_para->total_cycle_num:0.00);
	fprintf(p,"\t\t\t<stat name=\"MUL_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?(gpp_data->core_para->total_imult_num+gpp_data->core_para->total_imult_num)/gpp_data->core_para->total_cycle_num:0.00);
	fprintf(p,"\t\t\t<stat name=\"FPU_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?(gpp_data->core_para->total_fpalu_num+gpp_data->core_para->total_fpalu_num)/gpp_data->core_para->total_cycle_num:0.00);
	fprintf(p,"\t\t\t<stat name=\"ALU_cdb_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?(gpp_data->core_para->total_ialu_num+gpp_data->core_para->total_fpalu_num)/gpp_data->core_para->total_cycle_num:0.00);
	fprintf(p,"\t\t\t<stat name=\"MUL_cdb_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?(gpp_data->core_para->total_imult_num+gpp_data->core_para->total_imult_num)/gpp_data->core_para->total_cycle_num:0.00);
	fprintf(p,"\t\t\t<stat name=\"FPU_cdb_duty_cycle\" value=\"%.2f\"/>\n",gpp_data->core_para->total_cycle_num?(gpp_data->core_para->total_fpalu_num+gpp_data->core_para->total_fpalu_num)/gpp_data->core_para->total_cycle_num:0.00);

	fprintf(p,"\t\t\t<param name=\"number_of_BPT\" value=\"2\"/>\n");
	fprintf(p,"\t\t\t<component id=\"system.core%d.predictor\" name=\"PBT\">\n",core_id);
	/* branch predictor; tournament predictor see Alpha implementation */
	fprintf(p,"\t\t\t\t<param name=\"local_predictor_size\" value=\"10,3\"/>\n");
	fprintf(p,"\t\t\t\t<param name=\"local_predictor_entries\" value=\"1024\"/>\n");
	fprintf(p,"\t\t\t\t<param name=\"global_predictor_entries\" value=\"4096\"/>\n");
	fprintf(p,"\t\t\t\t<param name=\"global_predictor_bits\" value=\"2\"/>\n");
	fprintf(p,"\t\t\t\t<param name=\"chooser_predictor_entries\" value=\"4096\"/>\n");
	fprintf(p,"\t\t\t\t<param name=\"chooser_predictor_bits\" value=\"2\"/>\n");
	/* These parameters can be combined like below in next version
				<param name="load_predictor" value="10,3,1024"/>
				<param name="global_predictor" value="4096,2"/>
				<param name="predictor_chooser" value="4096,2"/>*/
	fprintf(p,"\t\t\t</component>\n");
	fprintf(p,"\t\t\t<component id=\"system.core%d.itlb\" name=\"itlb\">\n",core_id);
	fprintf(p,"\t\t\t\t<param name=\"number_entries\" value=\"64\"/>\n");
	fprintf(p,"\t\t\t\t<stat name=\"total_accesses\" value=\"%.0f\"/>\n",gpp_data->tlb_para->total_itlb_access_num);
	fprintf(p,"\t\t\t\t<stat name=\"total_misses\" value=\"%.0f\"/>\n",gpp_data->tlb_para->total_itlb_miss_num);
	fprintf(p,"\t\t\t\t<stat name=\"conflicts\" value=\"0\"/>\n");
	/* there is no write requests to itlb although writes happen to itlb after miss,
				which is actually a replacement */
	fprintf(p,"\t\t\t</component>\n");
	fprintf(p,"\t\t\t<component id=\"system.core%d.icache\" name=\"icache\">\n",core_id);
	/* there is no write requests to itlb although writes happen to it after miss,
				which is actually a replacement */
	fprintf(p,"\t\t\t\t<param name=\"icache_config\" value=\"16384,32,1,1,2,2,32,0\"/>\n");//512:32:1
	/* the parameters are capacity,block_width, associativity, bank, throughput w.r.t. core clock, latency w.r.t. core clock,output_width, cache policy,  */
	/* cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate */
	fprintf(p,"\t\t\t\t<param name=\"buffer_sizes\" value=\"4, 4, 4,0\"/>\n");/* cache controller buffer sizes: miss_buffer_size(MSHR),fill_buffer_size,prefetch_buffer_size,wb_buffer_size*/
	fprintf(p,"\t\t\t\t<stat name=\"read_accesses\" value=\"%d\"/>\n",gpp_data->cache_para->total_icache_read_access_num);
	fprintf(p,"\t\t\t\t<stat name=\"read_misses\" value=\"%d\"/>\n",gpp_data->cache_para->total_icache_read_miss_num);
	fprintf(p,"\t\t\t\t<stat name=\"conflicts\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t</component>\n");
	fprintf(p,"\t\t\t<component id=\"system.core%d.dtlb\" name=\"dtlb\">\n",core_id);
	fprintf(p,"\t\t\t\t<param name=\"number_entries\" value=\"64\"/>\n");/*dual threads*/
	fprintf(p,"\t\t\t\t<stat name=\"total_accesses\" value=\"%.0f\"/>\n",gpp_data->tlb_para->total_dtlb_access_num);
	fprintf(p,"\t\t\t\t<stat name=\"total_misses\" value=\"%.0f\"/>\n",gpp_data->tlb_para->total_dtlb_miss_num);
	fprintf(p,"\t\t\t\t<stat name=\"conflicts\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t</component>\n");
	fprintf(p,"\t\t\t<component id=\"system.core%d.dcache\" name=\"dcache\">\n",core_id);
	fprintf(p,"\t\t\t\t<param name=\"dcache_config\" value=\"32768,32,4,1,2,2,32,1 \"/>\n");
	fprintf(p,"\t\t\t\t<param name=\"buffer_sizes\" value=\"4, 4, 4, 4\"/>\n");/* cache controller buffer sizes: miss_buffer_size(MSHR),fill_buffer_size,prefetch_buffer_size,wb_buffer_size*/
	fprintf(p,"\t\t\t\t<stat name=\"read_accesses\" value=\"%d\"/>\n",gpp_data->cache_para->total_dcache_read_access_num);
	fprintf(p,"\t\t\t\t<stat name=\"write_accesses\" value=\"%d\"/>\n",gpp_data->cache_para->total_dcache_write_access_num);
	fprintf(p,"\t\t\t\t<stat name=\"read_misses\" value=\"%d\"/>\n",gpp_data->cache_para->total_dcache_read_miss_num);
	fprintf(p,"\t\t\t\t<stat name=\"write_misses\" value=\"%d\"/>\n",gpp_data->cache_para->total_dcache_write_miss_num);
	fprintf(p,"\t\t\t\t<stat name=\"conflicts\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t</component>\n");
	fprintf(p,"\t\t\t<param name=\"number_of_BTB\" value=\"2\"/>\n");//ok
	fprintf(p,"\t\t\t<component id=\"system.core%d.BTB\" name=\"BTB\">\n",core_id);
	fprintf(p,"\t\t\t\t<param name=\"BTB_config\" value=\"5120,4,2,1, 1,3\"/>\n");/*should be 4096 + 1024 */
	fprintf(p,"\t\t\t\t<stat name=\"read_accesses\" value=\"%d\"/>\n",gpp_data->btb_para->total_btb_read); /*See IFU code for guideline */
	fprintf(p,"\t\t\t\t<stat name=\"write_accesses\" value=\"%d\"/>\n",gpp_data->btb_para->total_btb_write);
	fprintf(p,"\t\t\t</component>\n");
	fprintf(p,"\t\t</component>\n");
}

}
void
Power_mc::add_L1Dinfo(FILE *p,int exist, int numthreads)
{
	if(exist)
    {
		int core_id = 0;
		for (core_id = 0; core_id < numthreads; core_id++) 
        {
			fprintf(p,
					"\t\t<component id=\"system.L1Directory%d\" name=\"L1Directory\">\n",
					core_id);
			fprintf(p, "\t\t\t<param name=\"Directory_type\" value=\"0\"/>\n");
			fprintf(p,
					"\t\t\t<param name=\"Dir_config\" value=\"4096,2,0,1,100,100, 8\"/>\n");/* the parameters are capacity,block_width, associativity,bank, throughput w.r.t. core clock, latency w.r.t. core clock,*/
			fprintf(p,
					"\t\t\t<param name=\"buffer_sizes\" value=\"8, 8, 8, 8\"/>\n");/* all the buffer related are optional */
			fprintf(p, "\t\t\t<param name=\"vdd\" value=\"0\"/>\n");/* 0 means using ITRS default vdd */ //newone
			fprintf(p, "\t\t\t<param name=\"clockrate\" value=\"2000\"/>\n");
			fprintf(p, "\t\t\t<param name=\"ports\" value=\"1,1,1\"/>\n");/* number of r, w, and rw search ports */
			fprintf(p, "\t\t\t<param name=\"device_type\" value=\"0\"/>\n");
			/* altough there are multiple access types,
			 Performance simulator needs to cast them into reads or writes
			 e.g. the invalidates can be considered as writes */
			fprintf(p,
					"\t\t\t<stat name=\"read_accesses\" value=\"0\"/>\n");
			fprintf(p,
					"\t\t\t<stat name=\"write_accesses\" value=\"0\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"read_misses\" value=\"0\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"write_misses\" value=\"0\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"conflicts\" value=\"0\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"duty_cycle\" value=\"0.0\"/>\n"); //newone
			fprintf(p, "\t\t</component>\n");
		}
	}
}

void
Power_mc::add_L2Dinfo(FILE *p,int exist, int numthreads)
{
	if(exist)
    {
		int core_id;
		for(core_id=0;core_id<numthreads;core_id++)
        {
		fprintf(p,"\t\t<component id=\"system.L2Directory%d\" name=\"L2Directory%d\">\n",core_id);
		fprintf(p,"\t\t\t<param name=\"Directory_type\" value=\"1\"/>\n");
		/*0 cam based shadowed tag. 1 directory cache */
		fprintf(p,"\t\t\t<param name=\"Dir_config\" value=\"1048576,16,16,1,2, 100\"/>\n");
		/* the parameters are capacity,block_width, associativity,bank, throughput w.r.t. core clock, latency w.r.t. core clock,*/
		fprintf(p,"\t\t\t<param name=\"buffer_sizes\" value=\"8, 8, 8, 8\"/>\n");
		/* all the buffer related are optional */
		fprintf(p,"\t\t\t<param name=\"clockrate\" value=\"2000\"/>\n");
		fprintf(p,"\t\t\t<param name=\"vdd\" value=\"0\"/>\n");/* 0 means using ITRS default vdd */     //newone
		fprintf(p,"\t\t\t<param name=\"ports\" value=\"1,1,1\"/>\n");
		/* number of r, w, and rw search ports */
		fprintf(p,"\t\t\t<param name=\"device_type\" value=\"0\"/>\n");
		/* altough there are multiple access types,
		 Performance simulator needs to cast them into reads or writes
		 e.g. the invalidates can be considered as writes */
		fprintf(p, "\t\t\t<stat name=\"read_accesses\" value=\"58824\"/>\n");
		fprintf(p, "\t\t\t<stat name=\"write_accesses\" value=\"27276\"/>\n");
		fprintf(p, "\t\t\t<stat name=\"read_misses\" value=\"1632\"/>\n");
		fprintf(p, "\t\t\t<stat name=\"write_misses\" value=\"183\"/>\n");
		fprintf(p, "\t\t\t<stat name=\"conflicts\" value=\"100\"/>\n");
		fprintf(p, "\t\t\t<stat name=\"duty_cycle\" value=\"0.1\"/>\n"); //newone
		fprintf(p, "\t\t</component>\n");
		}
	}
}

void
Power_mc::add_L2info(FILE *p,int exist, int numthreads)
{
	if(exist)
    {
		int core_id;
		for (core_id = 0; core_id < numthreads; core_id++) 
        {
			fprintf(p, "\t\t<component id=\"system.L2%d\" name=\"L2\">\n",
					core_id);
			/* all the buffer related are optional */
			fprintf(p,
					"\t\t\t<param name=\"L2_config\" value=\"1048576,32, 8, 8, 8, 23, 32, 1\"/>\n");
			/* the parameters are capacity,block_width, associativity, bank, throughput w.r.t. core clock, latency w.r.t. core clock,output_width, cache policy */
			fprintf(p,
					"\t\t\t<param name=\"buffer_sizes\" value=\"16, 16, 16, 16\"/>\n");
			/* cache controller buffer sizes: miss_buffer_size(MSHR),fill_buffer_size,prefetch_buffer_size,wb_buffer_size*/
			fprintf(p, "\t\t\t<param name=\"clockrate\" value=\"2000\"/>\n");
			fprintf(p, "\t\t\t<param name=\"vdd\" value=\"0\"/>\n");/* 0 means using ITRS default vdd */ //newone
			fprintf(p, "\t\t\t<param name=\"ports\" value=\"1,1,1\"/>\n");/* number of r, w, and rw ports */
			fprintf(p, "\t\t\t<param name=\"device_type\" value=\"0\"/>\n");
			fprintf(p,
					"\t\t\t<stat name=\"read_accesses\" value=\"%d\"/>\n",gpp_data->cache_para->total_icache2_read_access_num+
					gpp_data->cache_para->total_dcache2_read_access_num);
			fprintf(p,
					"\t\t\t<stat name=\"write_accesses\" value=\"%d\"/>\n",gpp_data->cache_para->total_dcache2_write_access_num);
			fprintf(p, "\t\t\t<stat name=\"read_misses\" value=\"%d\"/>\n",gpp_data->cache_para->total_icache2_read_miss_num+
					gpp_data->cache_para->total_dcache2_read_miss_num);
			fprintf(p, "\t\t\t<stat name=\"write_misses\" value=\"%d\"/>\n",gpp_data->cache_para->total_dcache2_write_miss_num);
			fprintf(p, "\t\t\t<stat name=\"conflicts\" value=\"0\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"duty_cycle\" value=\"%.2f\"/>\n",gpp_data->cache_para->l2cache_cycle_duty /
					gpp_data->core_para->total_cycle_num);
			fprintf(p, "\t\t</component>\n");
		}
	}
}

void
Power_mc::add_L3info(FILE *p,int exist, int numthreads)
{
	if(exist)
    {
		int core_id;
		for(core_id=0;core_id<numthreads;core_id++)
        {
			fprintf(p, "\t\t<component id=\"system.L3%d\" name=\"L3\">\n",core_id);
			fprintf(p,
					"\t\t\t<param name=\"L3_config\" value=\"16777216,64,16, 16, 16, 100,1\"/>\n");
			/* the parameters are capacity,block_width, associativity,bank, throughput w.r.t. core clock, latency w.r.t. core clock,*/
			fprintf(p, "\t\t\t<param name=\"clockrate\" value=\"850\"/>\n");
			fprintf(p, "\t\t\t<param name=\"vdd\" value=\"0\"/>\n");/* 0 means using ITRS default vdd */ //newone
			fprintf(p, "\t\t\t<param name=\"ports\" value=\"1,1,1\"/>\n");/* number of r, w, and rw ports */
			fprintf(p, "\t\t\t<param name=\"device_type\" value=\"0\"/>\n");
			fprintf(p,
					"\t\t\t<param name=\"buffer_sizes\" value=\"16, 16, 16, 16\"/>\n");
			/* cache controller buffer sizes: miss_buffer_size(MSHR),fill_buffer_size,prefetch_buffer_size,wb_buffer_size*/
			fprintf(p,
					"\t\t\t<stat name=\"read_accesses\" value=\"11824\"/>\n");
			fprintf(p,
					"\t\t\t<stat name=\"write_accesses\" value=\"11276\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"read_misses\" value=\"1632\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"write_misses\" value=\"183\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"conflicts\" value=\"0\"/>\n");
			fprintf(p, "\t\t\t<stat name=\"duty_cycle\" value=\"1.0\"/>\n");
			fprintf(p, "\t\t</component>\n");
		}
	}
}


void
Power_mc::write_xml_t(FILE* p, int numthreads)
{
	fprintf(p,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	fprintf(p,"<component id=\"root\" name=\"root\">\n");
	fprintf(p,"\t<component id=\"system\" name=\"system\">\n");
	fprintf(p,"\t\t<param name=\"number_of_cores\" value=\"%d\"/>\n",numthreads);
	fprintf(p,"\t\t<param name=\"number_of_L1Directories\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"number_of_L2Directories\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"number_of_L2s\" value=\"0\"/>\n");/* This number means how many L2 clusters in each cluster there can be multiple banks/ports */
	fprintf(p,"\t\t<param name=\"Private_L2\" value=\"1\"/>\n");/*1 Private, 0 shared/coherent */
	fprintf(p,"\t\t<param name=\"Private_L3\" value=\"0\"/>\n");/*1 Private, 0 shared/coherent */
	fprintf(p,"\t\t<param name=\"number_of_L3s\" value=\"0\"/>\n");/* This number means how many L3 clusters */
	fprintf(p,"\t\t<param name=\"number_of_NoCs\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"homogeneous_cores\" value=\"1\"/>\n");/*1 means homo */
	fprintf(p,"\t\t<param name=\"homogeneous_L2s\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"homogeneous_L1Directories\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"homogeneous_L2Directories\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"homogeneous_L3s\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"homogeneous_ccs\" value=\"1\"/>\n");/*cache coherence hardware */
	fprintf(p,"\t\t<param name=\"homogeneous_NoCs\" value=\"0\"/>\n");
	fprintf(p,"\t\t<param name=\"core_tech_node\" value=\"45\"/>\n");/* nm */
	fprintf(p,"\t\t<param name=\"target_core_clockrate\" value=\"2000\"/>\n");/*MHz */
	fprintf(p,"\t\t<param name=\"temperature\" value=\"360\"/>\n");/* Kelvin */
	fprintf(p,"\t\t<param name=\"number_cache_levels\" value=\"2\"/>\n");
	fprintf(p,"\t\t<param name=\"interconnect_projection_type\" value=\"1\"/>\n");/*0: aggressive wire technology; 1: conservative wire technology */
	fprintf(p,"\t\t<param name=\"device_type\" value=\"0\"/>\n");/*0: HP(High Performance Type); 1: LSTP(Low standby power) 2: LOP (Low Operating Power)  */
	fprintf(p,"\t\t<param name=\"longer_channel_device\" value=\"1\"/>\n");/* 0 no use; 1 use when possible */
	fprintf(p,"\t\t<param name=\"Embedded\" value=\"1\"/>\n");/* Embedded processor like ARM or general purpose processors?  */
	fprintf(p,"\t\t<param name=\"machine_bits\" value=\"64\"/>\n");
	fprintf(p,"\t\t<param name=\"virtual_address_width\" value=\"32\"/>\n");
	fprintf(p,"\t\t<param name=\"physical_address_width\" value=\"32\"/>\n");
	fprintf(p,"\t\t<param name=\"virtual_memory_page_size\" value=\"%d\"/>\n",4096);
	/* address width determines the tag_width in Cache, LSQ and buffers in cache controller
			default value is machine_bits, if not set */

	/*This page size(B) is complete different from the page size in Main memo section. this page size is the size of
			virtual memory from OS/Archi perspective; the page size in Main memo section is the actual physical line in a DRAM bank  */

	/*fprintf(p,"\t\t<stat name=\"idle_cycles\" value=\"0\"/>\n");//no
	fprintf(p,"\t\t<stat name=\"busy_cycles\"  value=\"100000\"/>\n");//no*/
	/* *********************** cores ******************* */
	/*
	 * core_i's info added here
	 * */
	add_coreinfo(p, numthreads);
	add_L1Dinfo(p, 0, numthreads);
	add_L2Dinfo(p, 0, numthreads);
	add_L2info(p,1, numthreads);
	add_L3info(p,0, numthreads);
/************************************************************************/
	if(numthreads>1)
    {
	fprintf(p,"\t\t<component id=\"system.NoC0\" name=\"noc0\">\n");
	fprintf(p,"\t\t\t<param name=\"clockrate\" value=\"2000\"/>\n");
	fprintf(p,"\t\t\t<param name=\"vdd\" value=\"0\"/>\n");/* 0 means using ITRS default vdd */     //newone
	fprintf(p,"\t\t\t<param name=\"type\" value=\"1\"/>\n");/*0:bus, 1:NoC , for bus no matter how many nodes sharing the bus at each time only one node can send req */
	fprintf(p,"\t\t\t<param name=\"horizontal_nodes\" value=\"4\"/>\n");
	fprintf(p,"\t\t\t<param name=\"vertical_nodes\" value=\"4\"/>\n");
	fprintf(p,"\t\t\t<param name=\"has_global_link\" value=\"1\"/>\n");/* 1 has global link, 0 does not have global link */
	fprintf(p,"\t\t\t<param name=\"link_throughput\" value=\"1\"/>\n");/*w.r.t clock */
	fprintf(p,"\t\t\t<param name=\"link_latency\" value=\"1\"/>\n");/*w.r.t clock */
	/* througput >= latency */
	/* Router architecture */
	fprintf(p,"\t\t\t<param name=\"input_ports\" value=\"5\"/>\n");
	fprintf(p,"\t\t\t<param name=\"output_ports\" value=\"5\"/>\n");/* For bus the I/O ports should be 1 */
	fprintf(p,"\t\t\t<param name=\"virtual_channel_per_port\" value=\"2\"/>\n");            //newone
	fprintf(p,"\t\t\t<param name=\"input_buffer_entries_per_vc\" value=\"32\"/>\n");		//newone
	fprintf(p,"\t\t\t<param name=\"flit_bits\" value=\"64\"/>\n");
	fprintf(p,"\t\t\t<param name=\"chip_coverage\" value=\"1\"/>\n");/* When multiple NOC present, one NOC will cover part of the whole chip.chip_coverage <=1 */
	fprintf(p,"\t\t\t<param name=\"link_routing_over_percentage\" value=\"0.5\"/>\n");
	/* Links can route over other components or occupy whole area.
				by default, 50% of the NoC global links routes over other
				components */
	fprintf(p,"\t\t\t<stat name=\"total_accesses\" value=\"100000\"/>\n");/* This is the number of total accesses within the whole network not for each router */
	fprintf(p,"\t\t\t<stat name=\"duty_cycle\" value=\"0.2\"/>\n");
	fprintf(p,"\t\t</component>\n");
	}
/************************************************************************/

//	fprintf(p,"\t\t<component id=\"system.mem\" name=\"mem\">\n");
	/* Main memory property */
//	fprintf(p,"\t\t\t<param name=\"mem_tech_node\" value=\"32\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"device_clock\" value=\"200\"/>\n");/*MHz, this is clock rate of the actual memory device, not the FSB */
//	fprintf(p,"\t\t\t<param name=\"peak_transfer_rate\" value=\"6400\"/>\n");/*MB/S*/
//	fprintf(p,"\t\t\t<param name=\"internal_prefetch_of_DRAM_chip\" value=\"4\"/>\n");
	/* 2 for DDR, 4 for DDR2, 8 for DDR3...*/
	/* the device clock, peak_transfer_rate, and the internal prefetch decide the DIMM property */
	/* above numbers can be easily found from Wikipedia */
//	fprintf(p,"\t\t\t<param name=\"capacity_per_channel\" value=\"4096\"/>\n"); /* MB */
	/* capacity_per_Dram_chip=capacity_per_channel/number_of_dimms/number_ranks/Dram_chips_per_rank
			Current McPAT assumes single DIMMs are used.*/
//	fprintf(p,"\t\t\t<param name=\"number_ranks\" value=\"2\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"num_banks_of_DRAM_chip\" value=\"8\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"Block_width_of_DRAM_chip\" value=\"64\"/>\n"); /* B */
//	fprintf(p,"\t\t\t<param name=\"output_width_of_DRAM_chip\" value=\"8\"/>\n");
	/*number of Dram_chips_per_rank=" 72/output_width_of_DRAM_chip*/
	/*number of Dram_chips_per_rank=" 72/output_width_of_DRAM_chip*/
//	fprintf(p,"\t\t\t<param name=\"page_size_of_DRAM_chip\" value=\"8\"/>\n"); /* 8 or 16 */
//	fprintf(p,"\t\t\t<param name=\"burstlength_of_DRAM_chip\" value=\"8\"/>\n");
//	fprintf(p,"\t\t\t<stat name=\"memory_accesses\" value=\"1052\"/>\n");
//	fprintf(p,"\t\t\t<stat name=\"memory_reads\" value=\"1052\"/>\n");
//	fprintf(p,"\t\t\t<stat name=\"memory_writes\" value=\"1052\"/>\n");
//	fprintf(p,"\t\t</component>\n");

	fprintf(p,"\t\t<component id=\"system.mc\" name=\"mc\">\n");
	/* Memeory controllers are for DDR(2,3...) DIMMs */
	/* current version of McPAT uses published values for base parameters of memory controller
			improvments on MC will be added in later versions. */
	fprintf(p,"\t\t\t<param name=\"type\" value=\"0\"/>\n"); /* 1: low power; 0 high performance */
	fprintf(p,"\t\t\t<param name=\"mc_clock\" value=\"500\"/>\n");/*DIMM IO bus clock rate MHz DDR2-400 for Niagara 1*/
	fprintf(p,"\t\t\t<param name=\"peak_transfer_rate\" value=\"6400\"/>\n");/*MB/S*/
	fprintf(p,"\t\t\t<param name=\"block_size\" value=\"64\"/>\n");/*B*/
	fprintf(p,"\t\t\t<param name=\"number_mcs\" value=\"0\"/>\n");
	/* current McPAT only supports homogeneous memory controllers */
	fprintf(p,"\t\t\t<param name=\"memory_channels_per_mc\" value=\"1\"/>\n");
	fprintf(p,"\t\t\t<param name=\"number_ranks\" value=\"0\"/>\n");
	fprintf(p,"\t\t\t<param name=\"withPHY\" value=\"0\"/>\n");
	/* # of ranks of each channel*/
	fprintf(p,"\t\t\t<param name=\"req_window_size_per_channel\" value=\"32\"/>\n");
	fprintf(p,"\t\t\t<param name=\"IO_buffer_size_per_channel\" value=\"32\"/>\n");
	fprintf(p,"\t\t\t<param name=\"databus_width\" value=\"128\"/>\n");
	fprintf(p,"\t\t\t<param name=\"addressbus_width\" value=\"51\"/>\n");
	/* McPAT will add the control bus width to the addressbus width automatically */
	fprintf(p,"\t\t\t<stat name=\"memory_accesses\" value=\"%.0f\"/>\n",gpp_data->mem_para->total_load_num+gpp_data->mem_para->total_store_num);
	fprintf(p,"\t\t\t<stat name=\"memory_reads\" value=\"%.0f\"/>\n",gpp_data->mem_para->total_load_num);
	fprintf(p,"\t\t\t<stat name=\"memory_writes\" value=\"%.0f\"/>\n",gpp_data->mem_para->total_store_num);
	/* McPAT does not track individual mc, instead, it takes the total accesses and calculate
			the average power per MC or per channel. This is sufficent for most application.
			Further trackdown can be easily added in later versions. */
	fprintf(p,"\t\t</component>\n");
/************************************************************************/
//	fprintf(p,"\t\t<component id=\"system.niu\" name=\"niu\">\n");
//	/* On chip 10Gb Ethernet NIC, including XAUI Phy and MAC controller  */
//	/* For a minimum IP packet size of 84B at 10Gb/s, a new packet arrives every 67.2ns.
//				 the low bound of clock rate of a 10Gb MAC is 150Mhz */
//	fprintf(p,"\t\t\t<param name=\"type\" value=\"0\"/>\n"); /* 1: low power; 0 high performance */
//	fprintf(p,"\t\t\t<param name=\"clockrate\" value=\"350\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"number_units\" value=\"0\"/>\n"); /* unlike PCIe and memory controllers, each Ethernet controller only have one port */
//	fprintf(p,"\t\t\t<stat name=\"duty_cycle\" value=\"1.0\"/>\n"); /* achievable max load <= 1.0 */
//	fprintf(p,"\t\t\t<stat name=\"total_load_perc\" value=\"0.7\"/>\n"); /* ratio of total achived load to total achivable bandwidth  */
//	/* McPAT does not track individual nic, instead, it takes the total accesses and calculate
//			the average power per nic or per channel. This is sufficent for most application. */
//	fprintf(p,"\t\t</component>\n");
/************************************************************************/
//
//	fprintf(p,"\t\t<component id=\"system.pcie\" name=\"pcie\">\n");
//	/* On chip PCIe controller, including Phy*/
//	/* For a minimum PCIe packet size of 84B at 8Gb/s per lane (PCIe 3.0), a new packet arrives every 84ns.
//				 the low bound of clock rate of a PCIe per lane logic is 120Mhz */
//	fprintf(p,"\t\t\t<param name=\"type\" value=\"0\"/>\n"); /* 1: low power; 0 high performance */
//	fprintf(p,"\t\t\t<param name=\"withPHY\" value=\"1\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"clockrate\" value=\"350\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"number_units\" value=\"0\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"num_channels\" value=\"8\"/>\n"); /* 2 ,4 ,8 ,16 ,32 */
//	fprintf(p,"\t\t\t<stat name=\"duty_cycle\" value=\"1.0\"/>\n"); /* achievable max load <= 1.0 */
//	fprintf(p,"\t\t\t<stat name=\"total_load_perc\" value=\"0.7\"/>\n"); /* Percentage of total achived load to total achivable bandwidth  */
//	/* McPAT does not track individual pcie controllers, instead, it takes the total accesses and calculate
//			the average power per pcie controller or per channel. This is sufficent for most application. */
//
//fprintf(p,"\t\t</component>\n");
/************************************************************************/
//	fprintf(p,"\t\t<component id=\"system.flashc\" name=\"flashc\">\n");
//	fprintf(p,"\t\t\t<param name=\"number_flashcs\" value=\"0\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"type\" value=\"1\"/>\n"); /* 1: low power; 0 high performance */
//	fprintf(p,"\t\t\t<param name=\"withPHY\" value=\"1\"/>\n");
//	fprintf(p,"\t\t\t<param name=\"peak_transfer_rate\" value=\"200\"/>\n");/*Per controller sustainable reak rate MB/S */
//	fprintf(p,"\t\t\t<stat name=\"duty_cycle\" value=\"1.0\"/>\n"); /* achievable max load <= 1.0 */
//	fprintf(p,"\t\t\t<stat name=\"total_load_perc\" value=\"0.7\"/>\n"); /* Percentage of total achived load to total achivable bandwidth  */
	/* McPAT does not track individual flash controller, instead, it takes the total accesses and calculate
			the average power per fc or per channel. This is sufficent for most application */
//	fprintf(p,"\t\t</component>\n");
/************************************************************************/
	fprintf(p,"\t</component>\n");
	fprintf(p,"</component>\n");
}
void
Power_mc::write_xml(FILE* f, int num_core)
{
	write_xml_t(f, num_core);
	fclose(f);
}

Power_mc::~Power_mc() 
{
	// TODO Auto-generated destructor stub
	delete this;
}


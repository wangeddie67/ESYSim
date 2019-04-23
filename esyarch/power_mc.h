/*
 * power_mc.h
 *
 *  Created on: 2016年5月8日
 *      Author: root
 */

#ifndef POWER_MC_H_
#define POWER_MC_H_
/*for count*/
struct core_para
{
	int homo;
	int total_inst_num;
	float total_cycle_num;
	float total_res_call_num;
	int ifu_block_num;
	float total_branch_num;
	float total_branch_access_num;
	float total_branch_miss_num;
	float total_ialu_num;
	float total_imult_num;
	float total_idiv_num;
	float total_fpalu_num;
	float total_fpmult_num;
	float total_fpdiv_num;
};
struct noc_para
{


};
struct mem_para
{
	float total_memport_rd;
	float total_memport_wt;
	float total_load_num;
	float total_store_num;
};
struct cache_para
{
	int total_icache_read_access_num;
	int total_icache_read_miss_num;
	int total_dcache_read_access_num;
	int total_dcache_read_miss_num;
	int total_dcache_write_access_num;
	int total_dcache_write_miss_num;
	int total_dcache2_read_access_num;
	int total_dcache2_read_miss_num;
	int total_dcache2_write_access_num;
	int total_dcache2_write_miss_num;
	int total_icache2_read_access_num;
	int total_icache2_read_miss_num;
	int l2cache_cycle_duty;
};
struct tlb_para
{
	float total_itlb_access_num;
	float total_itlb_miss_num;
	float total_dtlb_access_num;
	float total_dtlb_miss_num;

};
struct regs_para
{
	float total_iregs_write_num;
	float total_iregs_read_num;
	float total_fregs_write_num;
	float total_fregs_read_num;
	float total_cregs_write_num;
	float total_cregs_read_num;
};
struct btb_para
{
	float total_btb_write;
	float total_btb_read;

};
//用于统计处理器节点的所有数据
typedef struct gpp_node_para
{
	struct core_para* core_para;
	struct noc_para* noc_para;
	struct mem_para* mem_para;
	struct cache_para* cache_para;
	struct tlb_para* tlb_para;
	struct regs_para* regs_para;
	struct btb_para* btb_para;
}gpp_node_para;
/*for power*/
struct power_processor
{
	double peak_power;
	double total_leakage;
	double peak_dynamic;
	double sub_leakage;
	double gate_leakage;
	double rt_dynamic;
	double total_cores_peak_dynamic;
	double total_cores_sub_leakage;
	double total_cores_gate_leakage;
	double total_cores_rt_dynamic;
	double total_noc_peak_dynamic;
	double total_noc_sub_leakage;
	double total_noc_gate_leakage;
	double total_noc_rt_dynamic;
};
struct power_core
{
	double peak_dynamic;
	double sub_leakage;
	double gate_leakage;
	double rt_dynamic;
	//component -- ifu
	double ifu_peak_dynamic;
	double ifu_sub_leakage;
	double ifu_gate_leakage;
	double ifu_rt_dynamic;
	//component -- rn unit
	double rnu_peak_dynamic;
	double rnu_sub_leakage;
	double rnu_gate_leakage;
	double rnu_rt_dynamic;
	//component -- lsu
	double lsu_peak_dynamic;
	double lsu_sub_leakage;
	double lsu_gate_leakage;
	double lsu_rt_dynamic;
	//component -- mmu
	double mmu_peak_dynamic;
	double mmu_sub_leakage;
	double mmu_gate_leakage;
	double mmu_rt_dynamic;
	//component -- eu
	double eu_peak_dynamic;
	double eu_sub_leakage;
	double eu_gate_leakage;
	double eu_rt_dynamic;
	//l2
	double l2_peak_dynamic;
	double l2_sub_leakage;
	double l2_gate_leakage;
	double l2_rt_dynamic;
	//l3
	double l3_peak_dynamic;
	double l3_sub_leakage;
	double l3_gate_leakage;
	double l3_rt_dynamic;
};
struct power_noc
{
	double peak_dynamic;
	double sub_leakage;
	double gate_leakage;
	double rt_dynamic;
	double router_peak_dynamic;
	double router_sub_leakage;
	double router_gate_leakage;
	double router_rt_dynamic;
	double linkbus_peak_dynamic;
	double linkbus_sub_leakage;
	double linkbus_gate_leakage;
	double linkbus_rt_dynamic;
};
typedef struct powertype
{
		struct power_processor *p_processor;
		struct power_core *p_core[3];//contain l2&&l3
		struct power_noc *p_noc[1];
}CorePower;

class Power_mc 
{
public:
	gpp_node_para* gpp_data,*temp_data;
	CorePower *corepower;
	struct core_para *mc_core_create(void);
	struct mem_para *mc_mem_create(void);
	struct cache_para *mc_cache_create(void);
	struct tlb_para *mc_tlb_create(void);
	struct btb_para *mc_btb_create(void);
	struct noc_para *mc_noc_create(void);
	struct regs_para *mc_regs_create(void);
	void mcpat_init(int type);
	void add_coreinfo(FILE *p, int numthreads);
	void add_L1Dinfo(FILE *p,int exist, int numthreads);
	void add_L2Dinfo(FILE *p,int exist, int numthreads);
	void add_L2info(FILE *p,int exist, int numthreads);
	void add_L3info(FILE *p,int exist, int numthreads);
	void write_xml(FILE* f, int num_core);
	gpp_node_para *mc_gpppara_create();//统计参数创建
	struct powertype*gpppowertype_create(void);//功耗载体创建
	struct power_processor* power_processor_create(void);
	Power_mc();
	Power_mc(const char* nodetype,int has_temp);
//	gpp_node_para* total_calt(TCore & fstcore/*first core*/);//每隔一定周期计算总的参数统计
	virtual ~Power_mc();
private:
	void write_xml_t(FILE* xmlfile, int numthreads);
};
#endif /* POWER_MC_H_ */


/*****************************************************************************
 *                                McPAT
 *                      SOFTWARE LICENSE AGREEMENT
 *            Copyright 2012 Hewlett-Packard Development Company, L.P.
 *                          All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.”
 *
 ***************************************************************************/
#include "io.h"
#include <iostream>
#include "xmlParser.h"
#include "XML_Parse.h"
#include "processor.h"
#include "globalvar.h"
#include "version.h"
#include "core.h"
#include "../power_mc.h"
using namespace std;

ParseXML *Parse1 = NULL;

Processor *Proc1 = NULL;
//Core *Core1 = NULL;
//SharedCache *L2 = NULL;
int  plevel               = 2;


void power_core_create_m(Power_mc & po,int corenum=1){
	int corenum_t;
	for(corenum_t = 0;corenum_t<corenum;corenum_t++){
	po.corepower->p_core[corenum_t] = (struct power_core*)calloc(1,sizeof(struct power_core));
	}
	//return ptype;
}
void power_noc_create_m(Power_mc & po,int nocnum=1){
	int nocnum_t;
	for(nocnum_t = 0;nocnum_t<nocnum;nocnum_t++){
		po.corepower->p_noc[nocnum_t] = (struct power_noc*)calloc(1,sizeof(struct power_noc));
		}
}

struct powertype* GetPower(struct powertype * pt,int numcore = 1,int numnoc = 1){
	Proc1 = new Processor(Parse1);
	Proc1->displayEnergy(2, plevel);
	pt->p_processor->peak_power	 	=		Proc1->GetProcessorPeakPower();
	pt->p_processor->total_leakage 	=		Proc1->GetProcessorLeakagePower();
	pt->p_processor->peak_dynamic		=		Proc1->GetProcessorPeakDynamicPower();
	pt->p_processor->sub_leakage		=		Proc1->GetProcessorSubLeakagePower();
	pt->p_processor->gate_leakage		=		Proc1->GetProcessorGateLeakagePower();
	pt->p_processor->rt_dynamic		=		Proc1->GetProcessorRuntimeDynamicPower();
	//total core
	pt->p_processor->total_cores_peak_dynamic = Proc1->GetTotalCorePeakDynamicPower();
	pt->p_processor->total_cores_sub_leakage = Proc1->GetTotalCoreSubLeakagePower();
	pt->p_processor->total_cores_gate_leakage = Proc1->GetTotalCoreGateLeakagePower();
	pt->p_processor->total_cores_rt_dynamic = Proc1->GetTotalCoreRuntimeDynamicPower();
	//total noc
	pt->p_processor->total_noc_peak_dynamic = Proc1->GetTotalNocPeakDynamicPower();
	pt->p_processor->total_noc_sub_leakage = Proc1->GetTotalNocSubLeakagePower();
	pt->p_processor->total_noc_gate_leakage = Proc1->GetTotalNocGateLeakagePower();
	pt->p_processor->total_noc_rt_dynamic = Proc1->GetTotalNocRuntimeDynamicPower();
	//noc
	int ith = 0;
	while(numnoc){
	pt->p_noc[ith]->peak_dynamic = Proc1->nocs[ith]->GetNocPeakDynamicPower();
	pt->p_noc[ith]->sub_leakage = Proc1->nocs[ith]->GetNocSubLeakagePower();
	pt->p_noc[ith]->gate_leakage = Proc1->nocs[ith]->GetNocGateLeakagePower();
	pt->p_noc[ith]->rt_dynamic = Proc1->nocs[ith]->GetNocRuntimeDynamicPower();
	//router
	pt->p_noc[ith]->router_peak_dynamic = Proc1->nocs[ith]->GetRouterPeakDynamicPower();
	pt->p_noc[ith]->router_sub_leakage = Proc1->nocs[ith]->GetRouterSubLeakagePower();
	pt->p_noc[ith]->router_gate_leakage = Proc1->nocs[ith]->GetRouterGateLeakagePower();
	pt->p_noc[ith]->router_rt_dynamic = Proc1->nocs[ith]->GetRouterRuntimeDynamicPower();
	//link bus
	pt->p_noc[ith]->linkbus_peak_dynamic = Proc1->nocs[ith]->GetLinkbusPeakDynamicPower();
	pt->p_noc[ith]->linkbus_sub_leakage = Proc1->nocs[ith]->GetLinkbusSubLeakagePower();
	pt->p_noc[ith]->linkbus_gate_leakage = Proc1->nocs[ith]->GetLinkbusGateLeakagePower();
	pt->p_noc[ith]->linkbus_rt_dynamic = Proc1->nocs[ith]->GetLinkbusRuntimeDynamicPower();
	ith++,numnoc--;
	}
	//individual core
	ith = 0;
	while(numcore){
		pt->p_core[ith]->peak_dynamic = Proc1->cores[ith]->GetCorePeakDynamicPower();
		pt->p_core[ith]->sub_leakage = Proc1->cores[ith]->GetCoreSubLeakagePower();
		pt->p_core[ith]->rt_dynamic = Proc1->cores[ith]->GetCoreRuntimeDynamicPower();
		pt->p_core[ith]->gate_leakage = Proc1->cores[ith]->GetCoreGateLeakagePower();
		//component
		pt->p_core[ith]->ifu_gate_leakage = Proc1->cores[ith]->GetIfuGateLeakagePower();
		pt->p_core[ith]->ifu_peak_dynamic = Proc1->cores[ith]->GetIfuPeakDynamicPower();
		pt->p_core[ith]->ifu_sub_leakage = Proc1->cores[ith]->GetIfuSubLeakagePower();
		pt->p_core[ith]->ifu_rt_dynamic = Proc1->cores[ith]->GetIfuRuntimeDynamicPower();
		pt->p_core[ith]->rnu_gate_leakage = Proc1->cores[ith]->GetRnuGateLeakagePower();
		pt->p_core[ith]->rnu_peak_dynamic = Proc1->cores[ith]->GetRnuPeakDynamicPower();
		pt->p_core[ith]->rnu_sub_leakage = Proc1->cores[ith]->GetRnuSubLeakagePower();
		pt->p_core[ith]->rnu_rt_dynamic = Proc1->cores[ith]->GetRnuRuntimeDynamicPower();
		pt->p_core[ith]->lsu_gate_leakage = Proc1->cores[ith]->GetLsuGateLeakagePower();
		pt->p_core[ith]->lsu_peak_dynamic = Proc1->cores[ith]->GetLsuPeakDynamicPower();
		pt->p_core[ith]->lsu_sub_leakage = Proc1->cores[ith]->GetLsuSubLeakagePower();
		pt->p_core[ith]->lsu_rt_dynamic = Proc1->cores[ith]->GetLsuRuntimeDynamicPower();
		pt->p_core[ith]->mmu_gate_leakage = Proc1->cores[ith]->GetMmuGateLeakagePower();
		pt->p_core[ith]->mmu_peak_dynamic = Proc1->cores[ith]->GetMmuPeakDynamicPower();
		pt->p_core[ith]->mmu_sub_leakage = Proc1->cores[ith]->GetMmuSubLeakagePower();
		pt->p_core[ith]->mmu_rt_dynamic = Proc1->cores[ith]->GetMmuRuntimeDynamicPower();
		pt->p_core[ith]->eu_gate_leakage = Proc1->cores[ith]->GetEuGateLeakagePower();
		pt->p_core[ith]->eu_peak_dynamic = Proc1->cores[ith]->GetEuPeakDynamicPower();
		pt->p_core[ith]->eu_sub_leakage = Proc1->cores[ith]->GetEuSubLeakagePower();
		pt->p_core[ith]->eu_rt_dynamic = Proc1->cores[ith]->GetEuRuntimeDynamicPower();
		//private l2cache
		if(Parse1->sys.Private_L2){
			pt->p_core[ith]->l2_gate_leakage = Proc1->cores[ith]->l2cache->GetL2GateLeakagePower();
			pt->p_core[ith]->l2_peak_dynamic = Proc1->cores[ith]->l2cache->GetL2PeakDynamicPower();
			pt->p_core[ith]->l2_sub_leakage = Proc1->cores[ith]->l2cache->GetL2SubLeakagePower();
			pt->p_core[ith]->l2_rt_dynamic = Proc1->cores[ith]->l2cache->GetL2RuntimeDynamicPower();
		}
		if(Parse1->sys.Private_L3){
			pt->p_core[ith]->l3_gate_leakage = Proc1->cores[ith]->l3cache->GetL3GateLeakagePower();
			pt->p_core[ith]->l3_peak_dynamic = Proc1->cores[ith]->l3cache->GetL3PeakDynamicPower();
			pt->p_core[ith]->l3_sub_leakage = Proc1->cores[ith]->l3cache->GetL3SubLeakagePower();
			pt->p_core[ith]->l3_rt_dynamic = Proc1->cores[ith]->l3cache->GetL3RuntimeDynamicPower();
		}
		numcore--,ith++;
	}
	return pt;
}

int ExitMc(){
	delete Parse1;
	return 0;
}

struct powertype* MainMc(char *fin){
	char * fb   = NULL;
	opt_for_clk	= false;
	fb = fin;
	cout<<"McPAT (version "<< VER_MAJOR <<"."<< VER_MINOR
		<< " of " << VER_UPDATE << ") is computing the target processor...\n "<<endl;
	//parse XML-based interface
	Parse1 = new ParseXML();
	Parse1->parse(fb);
	Power_mc power;
	power.corepower->p_processor = power.power_processor_create();
	power_core_create_m(power,Parse1->sys.number_of_cores);
	power_noc_create_m(power,Parse1->sys.number_of_NoCs);
	struct powertype* return_power;
	return_power = (struct powertype*)calloc(1,sizeof(struct powertype));
	return_power = GetPower(power.corepower,Parse1->sys.number_of_cores,Parse1->sys.number_of_NoCs);
	delete Parse1;
	return return_power;
}






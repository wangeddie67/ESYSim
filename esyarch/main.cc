/*
 * mainb.cc
 *
 *  Created on: 2016年4月6日
 *      Author: root
 */
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <map>
#endif
#ifdef BFD_LOADER
#include <bfd.h>
#endif /* BFD_LOADER */

/*my work is ...*/
#include "host.h"
#include "bitmap.h"
#include "eventq.h"
#include "endian.h"
#include "misc.h"
#include "resource.h"
#include "define.h"
#include "tools.h"
#include "MPITable.h"
#include "Noc.h"
#include "Simulation.h"
#include "node.h"
#include "mips.h"
#include "alpha.h"
#include "fpgac.h"
#include "esynetRecord.h"
#include "esy_soccfg.h"
#include "esy_argument.h"
#include "esy_iodatafile.h"
#include "esy_interdata.h"
#include "esy_linktool.h"

#define MAXTAUGNUM 70
#define MAXAUGLENG 50
//extern void popnet_options();
//extern void popnet_sim(long long int sim_cycle);
//extern void main_popnet();
using namespace std;
time_t sim_starttime;
char *nd_type[] = {"gpp_nd","mec_nd","mem_nd","dsp_nd","fpga_nd"};
char *gpp_is[]= {"MIPS","ALPHA",'\0'};//
char *asic_type[] = {"asic_fft_128"};

TNode* sim_node[MAXTHREADS] = {NULL};//all node
TCore* sim_core[MAXTHREADS] = {NULL};//all core node
Fpgac* sim_asic[MAXTHREADS] = {NULL};//all asic node
const char* fhold = "ini.in",*delim = "|";
jmp_buf sim_exit_buf,node_exit_buf;
#ifndef _MSC_VER
/* simulator scheduling priority */
	int nice_priority;
#endif
int help_me;//0
int rand_seed;//1
//int gpp_num;
int node_num;
int now_run;// current running node's threadid
int msg_send_num = 0;
int msg_recv_num = 0;

MPITable *mt;//ID_TABLE record 
Power_mc *allpower;//mcpat record of all node
Noc glb_network;
map<const char *,toolsConfigFunc,Toolstruct> toolscfgmap;
unsigned long long simulation_cycle;//simulation cycle
long long int sim_cycle;//used by popnet
extern int simEsynetInit(const EsyNetworkCfg & cfg);
extern int simEsynetRunSim(long long int);
extern void simEsynetReport(FILE *fd);
//extern struct powertype* MainMc(char *fin);
void node_exit_fc(TNode* tn);
const char* progoutpre = "./records/progout/node_";
const char* inststreampre = "./records/inststream/node_";
const char* statisticspre = "./records/statistics/node_";
const char* powerxmlpre = "./records/power/power_xml/node_";
const char* powerxmlpre_4mcpat = "./records/power/power_xml/node_";
//related direction to mcpact
const char* poweroutpre = "./records/power/node_";
long long int visual_hop = 0;// for visualarch record file

extern char * gp_esynet_command;

string a_soc_cfg_file = "../example/yanshi";
bool a_soc_record_enable = false;
string a_soc_record_file_name = "../example/socrecord";
long a_soc_record_buf_size = 10000;
bool a_soc_record_text_enable = false;
bool a_soc_record_cout_enable = false;
EsyDataFileOStream<EsyDataItemSoCRecord> *event_out;   

//longjmp exit handler
void exit_now()
{
    cout << "msg_send_num = " << msg_send_num << endl;
    cout << "msg_recv_num = " << msg_recv_num << endl;
    //print statistics
    for(int i = 0; i < MAXTHREADS; i ++)
    {
        if(sim_node[i] != NULL && sim_node[i]->ntype == gppcore)
        {
            cout << endl;
            cout << "sim_node = " << i
                 << " (actualid = " << sim_node[i]->actualid
                 << ") 's statistics:" << endl;
            cout << endl;
            sim_node[i]->statcpnt->stat_print_stats(stdout);
            // run mcpact
            sim_node[i]->thispower->write_xml(sim_node[i]->powerxml, 1);
    //			MainMc(sim_node[i]->powerxml_4mcpat);
        }
    }
    for(int i = 0; i < MAXTHREADS; i ++ )
    {
        if(sim_node[i] != NULL && sim_node[i]->ntype == gppcore)
        {
            cout << "node " << sim_node[i]->actualid
                 << " exit and now time is : "
                 << sim_node[i]->cycle_time * sim_node[i]->sim_cycle << " us." 
                 << endl;
        }
    }
    simEsynetReport(stdout);
    
    if (a_soc_record_enable)
    {
        event_out->flushBuffer();
        event_out->setTotal(visual_hop);
        event_out->writeTotal();
        delete event_out;
    }
    
    exit(1);
}

// clear configuration
void gubbagereg(char* cfg[MAXTHREADS][MAXTAUGNUM])
{
    for(int i = 0; i < MAXTHREADS; i ++)
    {
        for(int j = 0; j < MAXTAUGNUM; j ++)
        {
            free(cfg[i][j]);
        }
    }
}

// create items based on parameters
/*void new_node(char* cfg[MAXTHREADS][MAXTAUGNUM])
{
    int threadholder = 0;
    int asicholder = 0;

    mt = new MPITable();
    allpower = new Power_mc(nd_type[0], 0);

    for(int i = 0; i < MAXTHREADS; i ++)
    {
        if(*(cfg[i]) != '\0')
        {
            if(!strcmp(cfg[i][3], nd_type[0]))
            {//gpp
                CoreStruct* cs = TNode::New_corecfg(cfg[i]);
                cout << "node " << i << "is being structured...,"
                     << " it's a gpp node, and the threadid is "
                     << threadholder << "." << endl;
                if(!strcmp(cfg[i][6], gpp_is[0]))//mips
                {
                    sim_node[i] = sim_core[threadholder] = 
                        new TMIPS(cs, threadholder);// configure architect
                }
                else if(!strcmp(cfg[i][6],gpp_is[1]))//alpha
                {
                    sim_node[i] = sim_core[threadholder] = 
                        new TALPHA(cs, threadholder);
                }
                threadholder++;
            }
            else if(!strcmp(cfg[i][3], nd_type[4]))
            {
                FpgaStruct* fs = TNode::New_fpgacfg(cfg[i]);
                sim_node[i] = sim_asic[asicholder] = 
                    new Fpgac(fs, threadholder);
            }
            else
            {

            }
            node_num++;
        }
    }
    Simulation::lcm_n(sim_node, node_num); //set frequency
}*/

void new_node(EsySoCCfg & esysoccfg){
    int threadholder = 0;
    int asicholder=0;
    int index;
    mt = new MPITable();
    allpower = new Power_mc(nd_type[0],0);
    //	esysoccfg.updateSoC();
    
    for(int i = 0; i < esysoccfg.tileCount(); i ++ )
    {
    //		if(esysoccfg.tile(i).tileEnable()){
        if(esysoccfg.tile(i).tileType() == EsySoCCfgTile::TILE_GPP)
        {//GPP TILE
            index = esysoccfg.tile(i).niId();
            cout << "node " << index << "is being structured..., "
                 << "it's a gpp node, and the threadid is " <<  threadholder 
                 << "." << endl;
            EsySoCCfgTile::ISAType isatype = esysoccfg.tile(i).coreIsaType();
            if(isatype == EsySoCCfgTile::ISA_MIPS)
            {
                CoreStruct* cs = TNode::New_corecfg(esysoccfg.tile(i));
                sim_node[index] = sim_core[threadholder] = 
                    new TMIPS(cs,threadholder);
            }
    		else if(isatype == EsySoCCfgTile::ISA_ALPHA)
            {
				CoreStruct* cs = TNode::New_corecfg(esysoccfg.tile(i));
				sim_node[index] = sim_core[threadholder] = 
                    new TALPHA(cs,threadholder);
			}
            threadholder++;
        }
        else if(esysoccfg.tile(i).tileType() == EsySoCCfgTile::TILE_ASIC)
        {
            index = esysoccfg.tile(i).niId();
            FpgaStruct* fs = TNode::New_fpgacfg(esysoccfg.tile(i));
            sim_node[index] = sim_asic[asicholder] = 
                new Fpgac(fs, asicholder);
            asicholder++;
        }
        node_num++;
    }

    Noc::frequence = 500;
    Simulation::lcm_n(sim_node,node_num);//设置频率相关
}


// read parameters
void new_simulation(const char* soccfg_xml)
{
    if(!soccfg_xml)
    {
        cerr << "Error: must specify the EsySoc config file" << endl;
        exit(1);
    }
    EsySoCCfg esysoccfg;
    EsyXmlError t_err = esysoccfg.readXml(soccfg_xml);
    if(t_err.hasError())
    {
        cerr << "Error: cannot read file "<< soccfg_xml << endl;
        exit(1);
    }
    new_node(esysoccfg);
    simEsynetInit(esysoccfg.network());
}

void NocConfig(const char* noccfg){
//	cout<<noccfg<<"abv"<<endl;
}

void FFTConfig(const char* noccfg){

}

int Mapcmp(const char* noccfg){}


long long int sim_step;
char** envp;

int main(int argc, char** argv, char** envp_)
{
    string a_noc_config_str;
    
    EsyArgumentList arg_list;
    arg_list.setHead("ESYArch Manycore System-on-Chip Simulator");
    arg_list.setVersion("2.0");
    arg_list.insertOpenFile("-soc_cfg_file", 
        "file path of soc configuration file", &a_soc_cfg_file,
        "", 0, SOCCFG_EXTENSION);
    arg_list.insertBool("-soc_record_enable", 
        "enable soc record file", &a_soc_record_enable);
    arg_list.insertNewFileIF("-soc_record_file_name", 
        "file path of soc record file", &a_soc_record_file_name,
        "-soc_record_enable", 1, SOCRECORD_EXTENSION);
    arg_list.insertLong("-soc_record_buf_size",
        "size of soc record file", &a_soc_record_buf_size,
        "-soc_record_enable", 1);
    arg_list.insertBool("-soc_record_text_enable",
        "enable text format soc record file", &a_soc_record_text_enable,
        "-soc_record_enable", 1);
    arg_list.insertBool("-soc_record_cout_enable",
        "enable soc record to cout", &a_soc_record_cout_enable);
    arg_list.insertString("-noc_configure_str",
        "noc configuration string", &a_noc_config_str);

    if ( arg_list.analyse( argc, argv ) != EsyArgumentList::RESULT_OK )
    {
        return 0;
    }

    gp_esynet_command = new char[1024];
    strcpy(gp_esynet_command, a_noc_config_str.c_str());

    if (a_soc_record_enable)
    {
        event_out = new EsyDataFileOStream<EsyDataItemSoCRecord>(
            a_soc_record_buf_size, a_soc_record_file_name, SOCRECORD_EXTENSION, 
            true, a_soc_record_text_enable);
    }
    
    long long int esynet_record_hop = 0;
    int sim_exit;
    int node_exit;

    if((sim_exit = setjmp(sim_exit_buf))!=0)
    {
        cout << "exit from main" << endl;
        exit_now();
    }
    envp = envp_;

    if(access("./progout",0)==-1)
    {
        mkdir("./progout",S_IRWXU);
    }

    toolscfgmap.insert(pair<const char *,toolsConfigFunc> ("NoC",NocConfig));
    TNode::Setexitarray();

    new_simulation(a_soc_cfg_file.c_str());

    int i = 0;
    int freqh_lcm = TNode::frequenceholder_lcm;
    int noc_freqh = Noc::frequence_holder;
    int hop_recorded = 0, valid_sim_step = 0;

    simulation_cycle = 0;
    sim_rt_t top_sim = NO_Q;
    cout << "sim: ** starting performance simulation **" << endl;
    fopen("./progout/sim_ins_trace" , "wt");
    //	setbuf(Simulation::visualFile,Simulation::esynetBuffer);
    time(&sim_starttime);
    cout << "LCM of freq is : " << freqh_lcm << endl;

    if((node_exit = setjmp(node_exit_buf))!=0)
    {
        node_exit_fc(sim_node[i]);
    }
    
    for(;;)
    {
        for(sim_step = 1; sim_step <= freqh_lcm; )
        {
            visual_hop++;
            bool hasItem = false;
            for(i = 0; i < MAXTHREADS; i ++)
            {
                if(sim_node[i] != NULL && sim_node[i] -> running && 
                    sim_step % sim_node[i]->frequence_holder == 0)
                {
                    if(sim_node[i]->ntype == gppcore)
                    {
                        sim_node[i]->esynetrcd->esynetInitData();
                        sim_node[i]->esynetrcd->hop = visual_hop;
                        sim_node[i]->run();
                        if(sim_node[i]->esynetrcd->valid)
                        {
//  							if(sim_node[i]->esynetrcd->msg_send!=0)	
//                            {
//                                cout << "msg send " << 
//                                    sim_node[i]->esynetrcd->msg_send << endl; 
//                            }
                            if(sim_node[i]->esynetrcd->msg_recv!=0)
                            {
                                cout << "msg recv " << 
                                    sim_node[i]->esynetrcd->msg_recv << endl; 
                            }
                            *(sim_node[i]->esynetrcd_temp) = 
                                *(sim_node[i]->esynetrcd);
//                            cout << "tile id1 = " << sim_node[i]->tileid 
//                                 << endl;
//                            sim_node[i]->esynetrcd->esynetWriteIn(
//                                Simulation::visualFile,0);
                            if (a_soc_record_enable)
                            {
                                EsyDataItemSoCRecord * t_record_item = 
                                    sim_node[i]->esynetrcd->toSoCRecord();
                                event_out->addNextItem((*t_record_item));
                                delete t_record_item;
                            }
                            if (a_soc_record_cout_enable)
                            {
                                EsyDataItemSoCRecord * t_record_item = 
                                    sim_node[i]->esynetrcd->toSoCRecord();
                                string itemstr = t_record_item->writeItem();
                                LINK_RECORD_PRINT( itemstr )
                                delete t_record_item;
                                hasItem = true;
                            }
                            sim_node[i]->esynetrcd->esynetWriteIn(
                                sim_node[i]->statisticsout,1);
                        }
                        else
                        {
                            sim_node[i]->esynetrcd_temp->valid = 0;
                            sim_node[i]->esynetrcd_temp->hop = visual_hop;
                            sim_node[i]->esynetrcd_temp->msg_send = 0;
                            sim_node[i]->esynetrcd_temp->msg_recv = 0;
                            sim_node[i]->esynetrcd_temp->msg_probe = 0;
                            sim_node[i]->esynetrcd_temp->sim_cycle = 
                                sim_node[i]->sim_cycle - 1;
//                            cout << "tile id2 = " << sim_node[i]->tileid 
//                                 << endl;
                            if (a_soc_record_enable)
                            {
                                EsyDataItemSoCRecord * t_record_item = 
                                    sim_node[i]->esynetrcd_temp->toSoCRecord();
                                event_out->addNextItem((*t_record_item));
                                delete t_record_item;
                            }
                            if (a_soc_record_cout_enable)
                            {
                                EsyDataItemSoCRecord * t_record_item = 
                                    sim_node[i]->esynetrcd_temp->toSoCRecord();
                                string itemstr = t_record_item->writeItem();
                                LINK_RECORD_PRINT( itemstr )
                                delete t_record_item;
                                hasItem = true;
                            }
//                            sim_node[i]->esynetrcd_temp->esynetWriteIn(
//                                Simulation::visualFile,0);
                            sim_node[i]->esynetrcd_temp->esynetWriteIn(
                                sim_node[i]->statisticsout,0);
                        }
                    }
                    else if(sim_node[i]->ntype == fpgaasic)
                    {
                        sim_node[i]->run();
                    }
                }
            }
            if(sim_step % noc_freqh == 0){
                sim_cycle = Noc::sim_cycle;
                simEsynetRunSim(sim_cycle);
                Noc::sim_cycle += 1;
            }
            sim_step = sim_step%freqh_lcm+1;
            if(sim_step==1)
            {
                simulation_cycle++;
                if (a_soc_record_cout_enable) 
                {
                    LINK_SYNCHORNOIZE('n')
                }
            }
        }
    }
    return 0;
}



void node_exit_fc(TNode* tn)
{
    cout << "node " << tn->actualid << "exit and now time is : "
        << tn->cycle_time * tn->sim_cycle << " us" << endl;
    //tn->statcpnt->stat_print_stats(stdout);
}




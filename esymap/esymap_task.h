// sim_task.h

#ifndef __SIM_TASK_H__
#define __SIM_TASK_H__

#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <sys/time.h>
#include "esymap_task_machine.h"
#include "esymap_config.h"
#include "esynet_sim_base.h"
#include "esynet_statistics.h"
#include "math.h"
#include <algorithm>

using namespace std;

enum APP_STATE
{
    APP_WAIT = 0,
    APP_RUNNING,
    APP_OVER,
};

/*
this class store all information of an application,
and the task ordering algs are implemented in this class.
*/
class application
{
  public:
    unsigned long app_id;
    APP_STATE app_states;
    // computing of every task
    vector<unsigned long> tasks;
    // communication between tasks, adjacency
    vector<vector<unsigned long> > tasks_communication;
    string mapping_algs;
    int period;
    int execution_count;
    vector<int> deadline;

    vector<vector<int> > task_order;
    vector<vector<int> > connect_1;
    vector<vector<int> > connect_2;
    vector<vector<int> > connect_3;

    // ff
    void getTaskOrder_ff();
    // cona
    int firstTask;
    void findFirstTask();
    void getTaskOrder_cona();
    // wena
    void getTaskOrder_wena();
    // cam
    void getTaskOrder_cam();

  public:
    application(EsymapConfig *argument_cfg, unsigned long id, int period_, int execution_count_,
                vector<int> deadline_, vector<unsigned long> tasks_,
                vector<vector<unsigned long> > tasks_communication_, string mapping_algs_);
    application() {};
};

/*
PeState
*/
class PeState
{
    friend std::ostream &operator<<(std::ostream &os, const PeState &s);

  public:
    int pe_id;
    bool busy;
    int app_id;
    int task_id;
    int neighborsFreeNum;
    std::vector<int> neighborsFreePE;
    std::map<int, std::vector<double> > e;
    std::map<int, std::vector<double> > w;
    std::map<int, std::vector<double> > n;
    std::map<int, std::vector<double> > s;

    //  petable[PE_ID].e[ID].push_back(ID);
    //  petable[PE_ID].e[ID].push_back(rate);
    //  petable[PE_ID].e[ID].push_back(comm);

  public:
    PeState(int id, bool b);
    PeState() {};
};

class SimTask : public EsynetSimBaseUnit
{
  public:
    SimTask(EsyNetworkCfg *network_cfg_, EsymapConfig *argument_cfg_,
            vector<EsynetTaskMachine> &m_pe_list_);

  private:
    EsymapConfig *argument_cfg;
    vector<EsynetTaskMachine> &m_pe_list; // obtain the outer name m_pe_list
    bool sim_over;

  private:
    long random_seed;
    long app_size_min;
    long app_size_max;
    string tgff_file_name;
    long period_min;
    long period_max;
    long period_exec_times_min;
    long period_exec_times_max;
    long total_app_num;
    bool tools_matlab_enable;
    bool tools_graph_enable;

    bool use_DyXY;

  private:
    bool mapping_end;
    // store all applications
    std::map<unsigned int, application> application_map;
    long m_pe_num;
    long m_array_size;
    // exeution time if all PE is the same
    vector<int> task_time;

    //<app_id, vector<task_id>>: APP running on PE of an NoC
    // vector<task_id>: the PE id for task_id
    std::map<unsigned int, std::vector<unsigned int> > running_app_state;
    // information of one PEs
    std::map<unsigned int, PeState> petable;
    // the finish task counts of an application
    std::map<unsigned int, unsigned int> fini_task_count;
    std::map<unsigned int, std::vector<unsigned int> > unfini_task_list;
    // = application_map.begin()
    std::map<unsigned int, application>::iterator next_exec;
    std::map<unsigned int, application>::iterator next_sche;

    // avaliable PE initialize = PE number
    unsigned int pecount;
    unsigned int application_rate, application_rate_count;
    unsigned int wait_mapping_count;

    double pe_rate;
    double occupy_rate;

    string mapping_algs;

    int waiting_sche_task_count;

    vector<vector<int> > app_list;

    double casqa_alpha;
    string factor_type;

  public:
    void init(); // called by sim_foundation constructor function (at its **end**).
    void runMapping(double sim_cycle);
    bool simulation_over();
    void doSchedule(application &app);
    void lookupAppStates();
    void releaseApp(unsigned int app_id);

    // first node
    int first_node(int app_size);
    unsigned int getFirstNode_cona();
    int SHiC(int app_size);
    int getFirstNode_nn();
    int getFirstNode_cam();

    // FF
    bool task_mapping_ff(application &app);
    bool task_mapping_random(application &app);
    bool task_mapping_nn(application &app);

    // task_mapping_casqa
    bool task_mapping_casqa(application &app);
    int get_ICE(const application &app, int task_id, int node);

    // BN
    bool task_mapping_bn(application &app);
    double get_interference_factor_bn(int app_id, int task_id, int node, bool flag);
    double get_interference_factor_xy_bn(std::vector<std::vector<int> > flow, bool flag);

    // for CoNA
    bool task_mapping_cona(application &app);
    bool task_mapping_new(application &app);
    void updateNeighborsFreePE();

    // for WeNA
    bool task_mapping_wena(application &app);
    std::vector<int> sf_calculation(int v);
    std::vector<int> get_next_dir(int n_cur_x, int n_cur_y, int dir_x_c, int dir_y_c, int n);

    // for congestion metric
    void update_link_info(bool add, unsigned int app_id, std::vector<unsigned int> map_result);
    void update_link_info_new_node_XY(bool add, double rate, int app_id, int task_id);
    void update_link_info_XY(bool add, double rate, int ID, unsigned int src, unsigned int des,
                             double comm);
    void update_link_info_DyXY(bool add, double rate, int ID, unsigned int src, unsigned int des,
                               double comm);
    int get_flow_id(int a, int b, int c);
    //   void update_link_info_new_node_XY_rm(bool add, double rate, int app_id, int task_id);

    int get_interference_factor(int app_id, int task_id, int node, bool flag);
    double get_interference_factor_DyXY(int app_id, int task_id, int node, bool flag);
    int get_interference_factor_xy(std::vector<std::vector<int> > flow, bool flag);

    // visualization and data output
    void PrintMatlab();
    void PrintMetric();
    // TGFF
    void read_tgff();
    void applicationGraph(application &app);
    // other
    // statistic task_schedule & ts() { return *t_s; }

  private:
  private:
    vector<double> MRD;
    void cal_MRD(int app_id);

  public:
    void print_in_main();
};

#endif

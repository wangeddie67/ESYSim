#ifndef ESYMAP_CONFIG_H
#define ESYMAP_CONFIG_H

#include "esynet_config.h"

class EsymapConfig : public EsynetConfig
{
  private:
    string mapping_algs;
    long app_size_min;
    long app_size_max;
    long pe_rate;             // the average PE send rate
    double system_usage_rate; // occupancy rate
    long app_inject_rate;
    string tgff_file_name;
    long packet_size_min;
    long packet_size_max;
    string result_file_name;
    long period_min;
    long period_max;
    long period_exec_times_min;
    long period_exec_times_max;
    long total_app_num;
    string task_order_type;
    bool tools_matlab_enable;
    bool tools_graph_enable;
    bool tools_inject_enable;

  public:
    EsymapConfig(int argc, char *const argv[]);
    void insertMapVariable();
    string get_mapping_algs()
    {
        return mapping_algs;
    }
    string get_task_order_type()
    {
        return task_order_type;
    }
    string get_tgff_file_name()
    {
        return tgff_file_name;
    }
    string get_result_file_name()
    {
        return result_file_name;
    }
    long get_app_size_min()
    {
        return app_size_min;
    }
    long get_app_size_max()
    {
        return app_size_max;
    }
    long get_pe_rate()
    {
        return pe_rate;
    }
    long get_app_inject_rate()
    {
        return app_inject_rate;
    }
    double get_system_usage_rate()
    {
        return system_usage_rate;
    }
    long get_packet_size_min()
    {
        return packet_size_min;
    }
    long get_packet_size_max()
    {
        return packet_size_max;
    }
    long get_period_min()
    {
        return period_min;
    }
    long get_period_max()
    {
        return period_max;
    }
    long get_period_exec_times_min()
    {
        return period_exec_times_min;
    }
    long get_period_exec_times_max()
    {
        return period_exec_times_max;
    }

    long get_total_app_num()
    {
        return total_app_num;
    }

    bool get_tools_matlab_enable()
    {
        return tools_matlab_enable;
    }
    bool get_tools_graph_enable()
    {
        return tools_graph_enable;
    }
    bool get_tools_inject_enable()
    {
        return tools_inject_enable;
    }
};

#endif

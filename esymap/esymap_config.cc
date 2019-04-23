#include "esymap_config.h"
#include <iostream>

EsymapConfig::EsymapConfig(int argc, char *const argv[])
    : EsynetConfig(CONFIG_INTERFACE), mapping_algs("cona"), pe_rate(5), app_size_min(4),
      app_size_max(20), period_min(2000), period_max(4000), period_exec_times_min(2),
      period_exec_times_max(10), system_usage_rate(1), app_inject_rate(1000),
      tgff_file_name("./TGFF/simple.tgff"), packet_size_min(5), packet_size_max(5),
      result_file_name("./result.txt"), total_app_num(500), 
task_order_type("cona"), tools_matlab_enable(false),
      tools_graph_enable(false), tools_inject_enable(false)
{
    insertMapVariable();
    if (analyse(argc, argv) != RESULT_OK)
    {
        exit(0);
    }
    preDefineCheck();
}
void EsymapConfig::insertMapVariable()
{
    bool i = false;
    insertBool("\n-------\nESYMAP Configure:\n-------\n", "", &i);
    insertLong("-app_size_min", "the minimum application size, default: 4", &app_size_min);
    insertLong("-app_size_max", "the maximum application size, default: 20", &app_size_max);
    insertLong("-packet_size_min", "the minimum packet size, default: 5 (flits)", &packet_size_min);
    insertLong("-packet_size_max", "the maximum packet size, default: 5 (flits)", &packet_size_max);
    insertLong("-period_min", "the minimum period, default: 2000 cycles", &period_min);
    insertLong("-period_max", "the maximum period, default: 4000 cycles", &period_max);
    insertLong("-period_exec_times_min", "the minimum execution times, default: 2",
               &period_exec_times_min);
    insertLong("-period_exec_times_max", "the maximum execution times, default: 10",
               &period_exec_times_max);
    insertLong("-total_app_num", "total application number to be executed, default: 500",
               &total_app_num);
    insertString(
        "-mapping_algs",
        "mapping algs, default: cona \n\t\tOptions: FF, NN, CoNA, WeNA, CASqA, CAM (lowercase)",
        &mapping_algs);
    insertOpenFile("-tgff_file_name", "TGFF file name, default: \
./TGFF/simple.tgff", &tgff_file_name);
    insertNewFile("-result_file_name", "result log file name, default: \"\", \
No result file.",
                 &result_file_name);
    insertString("-task_order_type", "task_order_type, default: cona", &task_order_type);
    insertLong("-pe_rate", "the average PE send rate, default: 5 (full rate if 5 flits/packet)",
               &pe_rate);
    insertLong("-app_inject_rate", "application injection rate, default: 1000", &app_inject_rate);
    insertDouble("-system_usage_rate", "the system occupancy rate, default: 1. Range from 0 to 1",
                 &system_usage_rate);
    insertBool("-tools_matlab_enable", "generate matlab script, default: false",
               &tools_matlab_enable);
    insertBool("-tools_graph_enable", "generate graph using graphviz, default: false",
               &tools_graph_enable);
    insertBool("-tools_inject_enable", "inject packets instead of application, default: false",
               &tools_inject_enable);
}

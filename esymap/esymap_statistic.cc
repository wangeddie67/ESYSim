#include "esymap_statistic.h"
#include <iostream>
#include <fstream>

using namespace std;

esymap_statistic::esymap_statistic(EsymapConfig *argu)
    : argu_list(argu)
{
    packet_send_num = 0;
    packet_recv_num = 0;
    total_latency = 0;
    avg_latency = 0;
    total_distance = 0;
    avg_distance = 0;
}

void esymap_statistic::print()
{
    avg_latency = total_latency / packet_recv_num;
    avg_distance = (double)total_distance / packet_recv_num;
    cout << "****** ESYMap Statistic*********" << endl;
    cout << "Packet Receive: " << packet_recv_num << endl;
    cout << "Average Latency: " << avg_latency << endl;
    cout << "Average Distance: " << avg_distance << endl;
}

void esymap_statistic::print(std::string s)
{
    if (s.size() == 0)
    {
        return;
    }
    avg_latency = total_latency / packet_recv_num;
    avg_distance = (double)total_distance / packet_recv_num;

    ofstream out(s.c_str(), std::iostream::app);
    out << argu_list->get_mapping_algs() << "\t"
        << argu_list->get_system_usage_rate() << "\t"
        << argu_list->get_tgff_file_name() << "\t"
        << avg_latency << "\t"
        << avg_distance << endl;
}

void esymap_statistic::print_apl_in(std::string s)
{
    avg_latency = total_latency / packet_recv_num;
    avg_distance = (double)total_distance / packet_recv_num;

    ofstream out(s.c_str(), std::iostream::app);

    out << avg_latency << "\t" << avg_distance << endl;
}

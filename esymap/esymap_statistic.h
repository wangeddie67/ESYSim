#ifndef ESYMAP_STATISTIC
#define ESYMAP_STATISTIC
#include <string>
#include "esymap_config.h"
#include <iostream>

class esymap_statistic
{
  public:
    esymap_statistic(EsymapConfig *argu);
    ~esymap_statistic()
    {
    }

  private:
    EsymapConfig *argu_list;

  private:
    long packet_send_num;
    long packet_recv_num;
    double total_latency;
    double avg_latency;

  private:
    long total_distance;
    double avg_distance;

  public:
    void inc_total_latency(double l)
    {
        total_latency += l;
    }
    void inc_total_distance(long d)
    {
        total_distance += d;
    }
    void inc_packet_recv_num(int d)
    {
        packet_recv_num += d;
    }
    void inc_packet_send_num(int d)
    {
        packet_send_num += d;
    }
    long get_total_latency()
    {
        return total_latency;
    }
    long get_avg_latency()
    {
        return total_latency / packet_recv_num;
    }
    long get_avg_distance()
    {
        return total_distance / packet_recv_num;
    }
    void print();
    void print(std::string s);
    void print_apl_in(std::string s);
};

#endif

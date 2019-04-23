#ifndef ESYNET_TASK_MACHINE_H
#define ESYNET_TASK_MACHINE_H

#include "esy_interdata.h"
#include "esy_networkcfg.h"
#include "esynet_sim_base.h"
#include <map>
#include <vector>
#include <sstream>
#include "esymap_config.h"
#include "esymap_statistic.h"

using namespace std;

enum PE_STATE
{
    PE_wait = 1,
    PE_receive,
    PE_computing,
    PE_send,
    PE_finish,
    PE_release,
};

class EsynetTaskMachine : public EsynetSimBaseUnit
{
  public:
    EsynetTaskMachine(EsyNetworkCfg *network_cfg, long id, EsymapConfig *argument_cfg, esymap_statistic *);
    // from TGFF
  public:
    vector<double> pe_attrib;
    vector<int> task_time; // exeution time
    void set_pe_attrib(vector<double> pe_attrib_, vector<int> task_time_);

    // For application
  public:
    PE_STATE stat;

  private:
    esymap_statistic *mp_esymap_statistic;
    int network_size;
    int m_id;
    int period;
    int period_count;
    int wait_exec_count;
    int execution_count;
    int recv_finish_count;

  private:
    unsigned long app_id;
    unsigned long task_id;
    unsigned int m_compute_count;
    unsigned int m_send_packet_count;
    unsigned int m_recv_packet_count;
    unsigned int m_compute;
    unsigned int m_send_packet;
    unsigned int m_recv_packet;
    long m_packet_size;

    map<long, long> packet_from_sche;
    map<long, long> packet_to_sche;
    std::vector<long> sendOrder;
    // packet inject rate
    unsigned int rate, rate_count;

    // receive packet from PE (ID, packet)
    map<long, long> packet_from;
    // store addtional packet, and no desti packet
    map<long, long> packet_from_temp;
    // send packet to PE (ID, packet)
    map<long, long> packet_to;

    std::vector<unsigned int> mapping_result;

  public:
    // Schedule - Invoked by task_schedule
    void Schedule(unsigned long app_id_, unsigned long task_id_, int period_, int execution_count_, unsigned int rate_,
                  unsigned int recv_num, unsigned int send_num, unsigned int compute_time, map<long, long> packet_from_,
                  map<long, long> packet_to_, std::vector<long> sendOrder_,
                  std::vector<unsigned int> running_app_state);

    // receivePacket
    void task_receivePacket(const EsynetFlit &p);
    // compute
    void task_compute();
    void task_sendPacket();
    void task_runBeforeRouter();
    // for state change
    void task_runAfterRouter(double cycle);
    void releasePE();

    // other function and data
    void handlePacket(const EsynetFlit &p);
    bool inject_flag;
    int packet_num;
    int rate_no_app, rate_count_no_app;
    int packet_size;
    int des_pe;
    int start_time;
    int end_time;

    void read_inject();

    //============for latency statistic
  public:
    std::vector<double> latency_basic;
    std::map<int, double> total_latency;
    std::map<int, int> total_packets;
    int get_flow_id(int a, int b, int c);
};

#endif
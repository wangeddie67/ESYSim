#include "esymap_task_machine.h"

//=========================for application====================

EsynetTaskMachine::EsynetTaskMachine(EsyNetworkCfg *network_cfg, long id,
                                     EsymapConfig *argument_cfg, esymap_statistic *s)
    : m_id(id), m_packet_size(argument_cfg->packetSize()), m_send_packet_count(0),
      m_recv_packet_count(0), m_compute_count(0), app_id(0), stat(PE_release), inject_flag(0),
      mp_esymap_statistic(s)
{
    network_size = 8;
    packet_num = -1;
    rate_no_app = -1;
    rate_count_no_app = -1;
    packet_size = -1;
    des_pe = -1;
    start_time = -1;
    end_time = -1;
    read_inject();
    m_packet_size;
    pe_attrib.clear();
    task_time.clear();
    latency_basic.push_back(0);
    for (int i = 1; i < 7 * 2; i++)
    {
        latency_basic.push_back(m_packet_size - 1 + 6 + (i - 1) * 2);
    }
    inject_flag = argument_cfg->get_tools_inject_enable();
}

void EsynetTaskMachine::set_pe_attrib(vector<double> pe_attrib_, vector<int> task_time_)
{
    pe_attrib = pe_attrib_;
    task_time = task_time_;
}

/*************************************************************************************************************/
void EsynetTaskMachine::Schedule(unsigned long app_id_, unsigned long task_id_, int period_,
                                 int execution_count_, unsigned int rate_, unsigned int recv_num,
                                 unsigned int send_num, unsigned int compute_time,
                                 map<long, long> packet_from_, map<long, long> packet_to_,
                                 std::vector<long> sendOrder_,
                                 std::vector<unsigned int> running_app_state)
{
    app_id = app_id_;
    task_id = task_id_;
    rate = rate_; // rate_;  // 1 flit/cycle
    m_recv_packet = recv_num;
    m_send_packet = send_num;
    if (0)
    {
        m_compute = compute_time;
    }
    else
    {
        m_compute = task_time[compute_time];
    }
    packet_from = packet_from_;
    packet_from_sche = packet_from;
    packet_to = packet_to_;
    packet_to_sche = packet_to;
    sendOrder = sendOrder_;
    mapping_result = running_app_state;

    rate_count = 0;
    stat = PE_wait;
    period = period_;
    wait_exec_count = 0;
    period_count = 0;

    execution_count = execution_count_;
    recv_finish_count = 0;

    total_latency.clear();
    total_packets.clear();

    addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_release, PE_wait, app_id,
                             task_id, -1, EsynetFlit()));
}

/*************************************************************************************************************/
// receivePacket
void EsynetTaskMachine::task_receivePacket(const EsynetFlit &p)
{
    // cout << "recv packets \n";
    handlePacket(p);

    long des_t = p.desAddr();
    long sor_t = p.sorAddr();
    if (packet_from.count(sor_t) == 0)
    {
        packet_from_temp[sor_t] = packet_from_temp[sor_t] + 1;
        if (!inject_flag)
            cout << "A wrong addr packet received! \n"; // << endl;
    }
    else if (packet_from[sor_t] > 0)
    {
        if (stat == PE_wait)
        {
            stat = PE_receive;
            addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_wait, PE_receive,
                                     app_id, task_id, -1, EsynetFlit()));
        }

        // packet_from[sor_t] = packet_from[sor_t] - 1;
        m_recv_packet_count++;
        if (m_recv_packet_count >= m_recv_packet)
        {
            recv_finish_count++;
            m_recv_packet_count = 0;
        }
    }
    else
    {
        packet_from_temp[sor_t] = packet_from_temp[sor_t] + 1;
    }
}

/*************************************************************************************************************/
// compute
void EsynetTaskMachine::task_compute()
{
    m_compute_count++;
}

/*************************************************************************************************************/
//***sendPacket
void EsynetTaskMachine::task_sendPacket()
{
    for (int i = 0; i < sendOrder.size(); ++i)
    {
        if (packet_to[sendOrder[i]] <= 0)
        {
            continue;
        }
        long m_src, m_dst;
        m_src = m_id;
        m_dst = sendOrder[i];
        // send one packet every time, this packet is divided into 5 flits, /mess_queue.cc 42/
        // cout << m_id << " is sending a packets!: " << m_dst << "    time is " << m_current_time
        // << endl;
        addEvent(EsynetMessEvent::generateEvgMessage(m_current_time, m_id, m_dst, m_packet_size, -1,
                                                     m_current_time));
        m_send_packet_count++;
        packet_to[sendOrder[i]]--;
        break;
    }
}

void EsynetTaskMachine::releasePE()
{
    m_send_packet_count = 0;
    m_recv_packet_count = 0;
    m_compute_count = 0;
    // app_id = 0;
    stat = PE_release;
    // cout << "released app_id " << app_id << "  task id  " << task_id << "  m_id  " << m_id <<
    // endl;
    addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_finish, PE_release, app_id,
                             task_id, -1, EsynetFlit()));
    map<long, long>::iterator it = packet_from.begin();
    for (; it != packet_from.end(); it++)
    {
        int hop = abs(it->first / 8 - m_id / 8) + abs(it->first % 8 - m_id % 8);
        int congestion_flag = 0;

        int sor_addr = it->first;
        int sor_task;
        for (int i = 0; i < mapping_result.size(); i++)
        {
            if (sor_addr == mapping_result[i])
            {
                sor_task = i;
                break;
            }
        }
        int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(sor_task),
                             static_cast<int>(task_id));

        double l_avg = total_latency[ID] / total_packets[ID];
        if (l_avg > latency_basic[hop] + 10)
        {
            congestion_flag = 1;
        }
        //        ofstream interfence_flag("./interfence_flag.txt", std::ofstream::app);
        //
        //        interfence_flag << app_id << "\t" << task_id << "\t" << hop << "\t" << ID << "\t"
        // << l_avg << "\t"
        //                        << congestion_flag << "\t" << endl;

        ofstream flow_statistic("./flow_statistic.txt", std::ofstream::app);

        flow_statistic << ID << "\t" << l_avg << "\t" << congestion_flag << "\t" << hop << "\t"
                       << l_avg - latency_basic[hop] << "\t" << total_latency[ID] << "\t"
                       << total_packets[ID] << "\t" << endl;

        if (congestion_flag)
        {
            cout << "congestion_flag" << endl;
        }
    }
}

/*************************************************************************************************************/
void EsynetTaskMachine::task_runAfterRouter(double cycle)
{
    // m_current_time = cycle;
    setTime(cycle);

    if (inject_flag)
    {
        task_runBeforeRouter();
        // cout<< m_id<< "  inject no packet"<< endl;
        return;
    }

    if (stat != PE_release && m_recv_packet == 0)
    {
        period_count++;
        if (period > 0)
        {
            if (period_count % period == 1 && wait_exec_count < execution_count)
            {
                wait_exec_count++;
            }
        }
        else
        {
            wait_exec_count = 1;
        }
    }

    if (stat == PE_release)
    {
        // do nothing
        return;
    }
    else
    {
        switch (stat)
        {

        case PE_wait:
        {
            // m_recv_packet == packet_from.empty(), /for token task/
            if (m_recv_packet == 0 && wait_exec_count > 0)
            {
                stat = PE_computing;
                execution_count--;
                wait_exec_count--;
                addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_wait, PE_computing,
                                         app_id, task_id, -1, EsynetFlit()));
            }

            break;
        }

        case PE_receive:
        {
            if (recv_finish_count > 0)
            {
                stat = PE_computing;
                execution_count--; // the PE with start task will not enter this case
                addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_receive,
                                         PE_computing, app_id, task_id, -1, EsynetFlit()));
                recv_finish_count--;
            }

            break;
        }

        case PE_computing:
        {
            task_compute();
            if (m_compute_count >= m_compute)
            { // for tail task
                if (m_send_packet == 0)
                {
                    stat = PE_finish;
                    addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_computing,
                                             PE_finish, app_id, task_id, -1, EsynetFlit()));
                    m_compute_count = 0;
                }
                else
                {
                    stat = PE_send;
                    addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_computing,
                                             PE_send, app_id, task_id, -1, EsynetFlit()));
                }
            }

            break;
        }

        case PE_send:
        {
            if (rate_count == 0)
            {
                task_sendPacket();
                rate_count = rate;
                rate_count--;
            }
            else
            {
                rate_count--;
            }
            if (m_send_packet_count >= m_send_packet)
            {
                stat = PE_finish;
                addEvent(EsynetMessEvent(m_current_time, ET_PE_CHANGE, m_id, PE_send, PE_finish,
                                         app_id, task_id, -1, EsynetFlit()));

                m_send_packet_count = 0;
            }
            break;
        }

        case PE_finish:
        {
            if (execution_count > 0)
            {
                if (recv_finish_count > 0)
                {
                    stat = PE_receive;
                }
                else
                {
                    stat = PE_wait;
                }
                m_send_packet_count = 0;
                m_compute_count = 0;
                packet_to = packet_to_sche;
                // packet_from = packet_from_sche;
            }
            else
            {
                stat = PE_release;
            }
            break;
        }

        case PE_release:
            break;
        default:
            ;
        }
    }
}

/*************************************************************************************************************/
// Use this function to inject packet without application
void EsynetTaskMachine::task_runBeforeRouter()
{
    if (!inject_flag)
    {
        // cout<< m_id<< "  inject no packet"<< endl;
        return;
    }

    long cycle_now = m_current_time;
    if (packet_num > 0 && rate_count_no_app == 0)
    {
        packet_num = packet_num - 1;
        addEvent(EsynetMessEvent::generateEvgMessage(m_current_time, m_id, des_pe, packet_size, -1,
                                                     m_current_time));
        // mess_queue::wm_pointer().add_message( mess_event(
        //     mess_queue::wm_pointer().current_time(), EVG_, m_id, des_pe, packet_size, -1, 0) );

        cout << cycle_now << " inject:  " << m_id << "  -->  " << des_pe << " : " << packet_size
             << "  " << packet_num << endl;
    }
    rate_count_no_app++;
    if (rate_count_no_app == rate_no_app)
    {
        rate_count_no_app = 0;
    }
}

void EsynetTaskMachine::handlePacket(const EsynetFlit &p)
{
    double cycle_now = m_current_time;
    /*statistic info */
    mp_esymap_statistic->inc_packet_recv_num(1);
    mp_esymap_statistic->inc_total_latency(cycle_now - p.e2eStartTime()); // p.startTime());
    int src_pe = p.sorAddr();
    int des_pe = p.desAddr();
    int hop = abs(src_pe / 8 - des_pe / 8) + abs(src_pe % 8 - des_pe % 8);
    mp_esymap_statistic->inc_total_distance(hop);
    /* end statistic*/

    int sor_addr = p.sorAddr();
    int sor_task;
    for (int i = 0; i < mapping_result.size(); i++)
    {
        if (sor_addr == mapping_result[i])
        {
            sor_task = i;
            break;
        }
    }
    int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(sor_task),
                         static_cast<int>(task_id));
    app_id * 100 + sor_task;

    if (total_latency.find(ID) == total_latency.end())
    {
        total_latency[ID] = 0;
    }
    total_latency[ID] += cycle_now - p.startTime();

    if (total_packets.find(ID) == total_packets.end())
    {
        total_packets[ID] = 0;
    }
    total_packets[ID] += 1;

    if (!inject_flag)
    {
        // cout<< m_id<< "  inject no packet"<< endl;
        return;
    }

    ofstream flitout("flit1000.txt", std::ofstream::app);

    flitout << p.flitId() << "\t" << p.sorAddr() << "\t" << p.desAddr() << "\t" << p.startTime()
            << "\t" << cycle_now << "\t" << ID << "\t" << cycle_now - p.startTime() << endl;
}

void EsynetTaskMachine::read_inject()
{
    ifstream inFile("./Tools/inject.txt", ios::in);

    string line;
    int a;
    char flag;
    while (getline(inFile, line))
    {
        istringstream s_line(line);
        istringstream s_line_value(line);

        s_line >> flag;
        if (flag == '#')
            continue;
        s_line_value >> a;
        if (a != m_id)
            continue;

        s_line_value >> a;
        des_pe = a;

        s_line_value >> a;
        rate_no_app = a;
        rate_count_no_app = 0;

        s_line_value >> a;
        packet_size = a;

        s_line_value >> a;
        packet_num = a;

        s_line_value >> a;
        start_time = a;

        s_line_value >> a;
        end_time = a;
    }
}

int EsynetTaskMachine::get_flow_id(int a, int b, int c)
{
    int ID = 10000 * a + 100 * b + c;
    return ID;
}

#include "esymap_task.h"
#include "esynet_mess_queue.h"
#include "esynet_foundation.h"
#include "esynet_ni_unit.h"

#include <cstring>
#include <fstream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

application::application(EsymapConfig *argument_cfg, unsigned long id, int period_,
                         int execution_count_, vector<int> deadline_, vector<unsigned long> tasks_,
                         vector<vector<unsigned long> > tasks_communication_, string mapping_algs_)
    : app_id(id), period(period_), deadline(deadline_), app_states(APP_WAIT), tasks(tasks_),
      tasks_communication(tasks_communication_), mapping_algs(mapping_algs_),
      execution_count(execution_count_)
{
    string task_order_algs = argument_cfg->get_task_order_type();
    findFirstTask();
    if (task_order_algs == "ff")
    {
        getTaskOrder_ff();
    }

    if (task_order_algs == "cona")
    {
        getTaskOrder_cona();
    }

    if (task_order_algs == "wena")
    {
        getTaskOrder_wena();
        // getTaskOrder_cona();
    }

    if (task_order_algs == "cam")
    {
        getTaskOrder_wena();
        getTaskOrder_cam();
        // getTaskOrder_cona();
    }
}

PeState::PeState(int id, bool b) : pe_id(id), busy(b)
{
}

SimTask::SimTask(EsyNetworkCfg *network_cfg, EsymapConfig *argument_cfg_,
                 vector<EsynetTaskMachine> &m_pe_list_)
    : m_pe_list(m_pe_list_), random_seed(argument_cfg->randomSeed()), argument_cfg(argument_cfg_),
      sim_over(false), use_DyXY(false)
{
    //==================for application mapping===================
    m_pe_num = network_cfg->routerCount();
    m_array_size = 8; // ary[0];
    pe_rate = argument_cfg->get_pe_rate();
    application_rate = argument_cfg->get_app_inject_rate();
    occupy_rate = argument_cfg->get_system_usage_rate();
    app_size_min = argument_cfg->get_app_size_min();
    app_size_max = argument_cfg->get_app_size_max();
    mapping_algs = argument_cfg->get_mapping_algs();
    tgff_file_name = argument_cfg->get_tgff_file_name();
    application_rate_count = application_rate;
    wait_mapping_count = 0;
    pecount = m_pe_num - 1;
    waiting_sche_task_count = 0;
    casqa_alpha = 1;
    mapping_end = false;
    period_min = argument_cfg->get_period_min();
    period_max = argument_cfg->get_period_max();
    period_exec_times_min = argument_cfg->get_period_exec_times_min();
    period_exec_times_max = argument_cfg->get_period_exec_times_max();
    total_app_num = argument_cfg->get_total_app_num();
    tools_matlab_enable = argument_cfg->get_tools_matlab_enable();
    tools_graph_enable = argument_cfg->get_tools_graph_enable();
}

void SimTask::init()
{
    for (int i = 0; i < m_pe_num; ++i)
    {
        PeState p(i, 0);
        petable[i] = p;
    }
    petable[0].busy = 1;
    petable[0].app_id = 1000;
    read_tgff();
    cout << "applications count " << application_map.size() << endl;
    bool dotPng = tools_graph_enable; // use dot(graphviz) to generate task graph (in png format)
    if (dotPng)
    {
        for (int i = 0; i < application_map.size(); i++)
        {
            applicationGraph(application_map[i]);
        }
    }
    next_exec = application_map.begin();
    next_sche = application_map.begin();
    app_list.resize(5000);
    for (int i = 0; i < app_list.size(); i++)
    {
        app_list[i].resize(3);
    }
}

void SimTask::updateNeighborsFreePE()
{
    int idleCount = 0;
    for (int i = 1; i < m_pe_num; i++)
    {
        petable[i].neighborsFreePE.clear();
        idleCount = 0;
        if (i % 8 != 0)
        {
            if (!petable[i - 1].busy)
            {
                idleCount++;
                petable[i].neighborsFreePE.push_back(i - 1);
            }
        }
        if (i % 8 != 7)
        {
            if (!petable[i + 1].busy)
            {
                idleCount++;
                petable[i].neighborsFreePE.push_back(i + 1);
            }
        }
        if (i / 8 != 7)
        {
            if (!petable[i + 8].busy)
            {
                idleCount++;
                petable[i].neighborsFreePE.push_back(i + 8);
            }
        }
        if (i / 8 != 0)
        {
            if (!petable[i - 8].busy)
            {
                idleCount++;
                petable[i].neighborsFreePE.push_back(i - 8);
            }
        }
        petable[i].neighborsFreeNum = idleCount;
    }
}

void SimTask::doSchedule(application &app)
{
    unsigned int m_compute = 0;
    unsigned int m_send_packet = 0;
    unsigned int m_recv_packet = 0;
    map<long, long> packet_from;
    map<long, long> packet_to;
    std::vector<long> sendOrder;
    int period = app.period;
    int execution_count = app.execution_count;
    int task_num = app.tasks.size();
    int i; // i is Schedule task id
    i = app.task_order[task_num - waiting_sche_task_count][0];

    m_compute = app.tasks[i];
    for (int j = 0; j < task_num; ++j)
    {
        if (app.tasks_communication[i][j] == 0)
        {
            continue;
        }
        else
        {
            packet_to[running_app_state[app.app_id][j]] = app.tasks_communication[i][j];
            m_send_packet = m_send_packet + app.tasks_communication[i][j];
            sendOrder.push_back(running_app_state[app.app_id][j]);
        }
    }
    for (int j = 0; j < task_num; ++j)
    {
        if (app.tasks_communication[j][i] == 0)
        {
            continue;
        }
        else
        {
            packet_from[running_app_state[app.app_id][j]] = app.tasks_communication[j][i];
            m_recv_packet = m_recv_packet + app.tasks_communication[j][i];
        }
    }
    petable[running_app_state[app.app_id][i]].busy = 1;
    petable[running_app_state[app.app_id][i]].app_id = app.app_id;
    petable[running_app_state[app.app_id][i]].task_id = i;

    m_pe_list[running_app_state[app.app_id][i]]
        .Schedule(app.app_id, i, period, execution_count, pe_rate, m_recv_packet, m_send_packet,
                  m_compute, packet_from, packet_to, sendOrder, running_app_state[app.app_id]);
    m_compute = 0;
    m_send_packet = 0;
    m_recv_packet = 0;
    packet_to.clear();
    packet_from.clear();
    sendOrder.clear();
    pecount--;

    waiting_sche_task_count = waiting_sche_task_count - 1;
    if (waiting_sche_task_count == 0)
    {
        // ofstream app_exec("./app_execution.txt", std::ofstream::app);
        // app_exec << app.app_id <<"\t"<<<< endl;
        app_list[app.app_id][0] = app.app_id;
        app_list[app.app_id][1] = m_current_time;
        fini_task_count[app.app_id] = 0;
        unfini_task_list[app.app_id] = running_app_state[app.app_id];
        next_exec++;
        cout << "app period info: \napp id: " << app.app_id << "\nperiod: " << period
             << "\nexecution times: " << execution_count << endl;
    }
}

void SimTask::lookupAppStates()
{
    if (running_app_state.size() == 0)
    {
        // cout<<"size == 0 -----------------"<<endl;
        return;
    }
    else
    {
        std::vector<unsigned int> finish_app_list;
        std::map<unsigned int, std::vector<unsigned int> >::iterator it = unfini_task_list.begin();
        for (; it != unfini_task_list.end(); it++)
        {
            std::vector<unsigned int> task_pe_list(it->second);
            int m = running_app_state[it->first].size();
            for (int i = 0; i < task_pe_list.size(); ++i)
            {
                if (m_pe_list[task_pe_list[i]].stat == PE_release)
                {
                    fini_task_count[it->first]++;
                    std::vector<unsigned int>::iterator it1;
                    it1 = find(it->second.begin(), it->second.end(), task_pe_list[i]);
                    if (it1 != it->second.end())
                    {
                        // cout<<"erase "<<task_pe_list[i]<<"   id  "<<it->first<<"  shenyu "
                        //<<m - fini_task_count[it->first]<<endl;
                        it->second.erase(it1);
                    }
                }
            }
            if (fini_task_count[it->first] == m)
            {
                finish_app_list.push_back(it->first);
            }
        }
        if (finish_app_list.size() != 0)
        {
            for (int i = 0; i < finish_app_list.size(); ++i)
            {
                releaseApp(finish_app_list[i]);
            }
        }
    }
}

void SimTask::runMapping(double sim_cycle)
{
    m_current_time = sim_cycle;
    if (0)
    {
        update_link_info_DyXY(1, 1, 100, 41, 18, 1);
        // update_link_info_DyXY(1,1,110,9,27,1);
        //      cout<<"petable: ";
        //             for(int i = 0; i < petable.size(); i++)
        //             {
        //              cout<<i<<"\t";
        //              cout<<petable[i].n.size()<<"\t"<<
        //                      petable[i].s.size()<<"\t"<<
        //                      petable[i].w.size()<<"\t"<<
        //                      petable[i].e.size()<<endl;
        //             }

        update_link_info_DyXY(0, 1, 100, 41, 18, 1);

        update_link_info_DyXY(1, 1, 100, 41, 18, 1);
        update_link_info_DyXY(0, 1, 100, 41, 18, 1);
    }

    if (0)
    {
        update_link_info_XY(1, 1, 100, 43, 22, 100);
        update_link_info_XY(1, 1, 101, 38, 14, 100);
        std::vector<std::vector<int> > flow;
        std::vector<int> temp;
        temp.push_back(54);
        temp.push_back(38);
        temp.push_back(100);
        flow.push_back(temp);
        int f = get_interference_factor_xy(flow, 0);
        cout << "interfence is:\n";
        cout << f << endl;
        temp[0] = 54;
        temp[1] = 55;
        temp[2] = 10;
        flow[0] = temp;
        f = get_interference_factor_xy(flow, 0);
        cout << "interfence is:\n";
        cout << f << endl;
        exit(1);
    }
    // vector<int> test = sf_calculation(7);
    lookupAppStates();

    if (mapping_end && waiting_sche_task_count <= 0)
    {
        return;
    }

    bool task_mapping_ok;

    if (waiting_sche_task_count > 0)
    {
        doSchedule(next_exec->second);
    }

    // if (next_exec->first == 1)
    //{
    //    return;
    //}

    bool flag_randn = 0;

    if (next_sche != application_map.end())
    {
        if (application_rate_count == application_rate)
        {
            wait_mapping_count++;
            addEvent(EsynetMessEvent(m_current_time, ET_APP_REQUEST, next_sche->second.app_id,
                                     pecount, next_sche->second.tasks.size(), wait_mapping_count,
                                     -1, -1, EsynetFlit()));
            application_rate_count = 0;
            next_sche++;
            if (flag_randn)
            {
                // srand(time(NULL) + application_rate);
                srand((application_rate));
                application_rate = 10000; // rand() % (601) + 200; // 200 - 600
                cout << "application_rate   " << application_rate << endl;
            }
        }
        else
        {
            application_rate_count++;
        }
    }
    else if (wait_mapping_count <= 0)
    {
        // cout << "no more applications" << endl;
    }

    // double occupy_rate = 1;
    int pe_max = m_pe_num * occupy_rate;

    if (wait_mapping_count <= 0)
    {
        // cout<<"no more application!"<<endl;
        return;
    }
    else if (waiting_sche_task_count == 0 && next_exec != application_map.end())
    {
        if (m_pe_num - pecount + (next_exec->second).tasks.size() > pe_max)
        {
            // cout << "return due to occupy rate  " << pecount << "  " <<
            // (next_exec->second).tasks.size() << endl;
            return;
        }

        if (mapping_algs == "ff") // || mapping_algs == "random" || mapping_algs == "nn")
        {
            task_mapping_ok = task_mapping_ff(next_exec->second);
        }

        if (mapping_algs == "random") //|| mapping_algs == "nn")
        {
            task_mapping_ok = task_mapping_random(next_exec->second);
        }

        if (mapping_algs == "nn") //|| mapping_algs == "nn")
        {
            task_mapping_ok = task_mapping_nn(next_exec->second);
        }

        if (mapping_algs == "cona") //|| mapping_algs == "nn")
        {
            task_mapping_ok = task_mapping_cona(next_exec->second);
        }

        if (mapping_algs == "wena" || mapping_algs == "wena_cona_order")
        {
            task_mapping_ok = task_mapping_wena(next_exec->second);
        }

        if (mapping_algs == "cam") //|| mapping_algs == "nn")
        {
            task_mapping_ok = task_mapping_new(next_exec->second);
        }

        if (mapping_algs == "casqa") //|| mapping_algs == "nn")
        {
            task_mapping_ok = task_mapping_casqa(next_exec->second);
        }

        if (mapping_algs == "bn") //|| mapping_algs == "nn")
        {
            task_mapping_ok = task_mapping_bn(next_exec->second);
        }

        if (task_mapping_ok)
        {
            addEvent(EsynetMessEvent(m_current_time, ET_APP_ENTER, next_exec->second.app_id, -1, -1,
                                     -1, -1, -1, EsynetFlit()));

            waiting_sche_task_count = (next_exec->second).tasks.size();

            // update_link_info(1, next_exec->first, running_app_state[next_exec->first]);
            // PrintMetric();
            PrintMatlab();
            wait_mapping_count--;
            if (next_exec->first == total_app_num - 1)
                mapping_end = true;
        }
        else
        {
            return;
        }
    }
}

void SimTask::releaseApp(unsigned int app_id)
{
    // ofstream interfence_flag("./interfence_flag.txt", std::ofstream::app);
    // interfence_flag << endl;
    app_list[app_id][2] = m_current_time;
    update_link_info(0, app_id, running_app_state[app_id]);

    // std::map<unsigned int, std::vector<unsigned int> >::iterator it = running_app_state.begin();
    // for (; it != running_app_state.end(); ++it)
    //{
    //     print_interference_factor(it->first, 1);
    //}

    application_map[app_id].app_states = APP_OVER;
    for (int i = 0; i < running_app_state[app_id].size(); ++i)
    {
        petable[running_app_state[app_id][i]].busy = 0;
        pecount++;
        m_pe_list[running_app_state[app_id][i]].releasePE();
    }
    running_app_state.erase(app_id);
    unfini_task_list.erase(app_id);
    addEvent(
        EsynetMessEvent(m_current_time, ET_APP_EXIT, app_id, -1, -1, -1, -1, -1, EsynetFlit()));

    // PrintMetric();
    PrintMatlab();
    cout << "Finished, " << m_current_time << ": " << mapping_algs << " occupy_rate " << occupy_rate
         << " app_id " << app_id << endl;

    if (app_id == total_app_num - 1)
    {
        sim_over = true;
    }
}

void SimTask::PrintMatlab()
{
    if (tools_matlab_enable == false)
    {
        return;
        // if use this function.
        // debug: app_id == 1000 for pe_id == 0
        // so application_map[petable[i+j*8].app_id] will create a new item application_map[1000] =
        // ???
    }
    ofstream outfile("./Tools/matlab_mapping.m", std::ofstream::app);
    outfile << endl;
    outfile << "%matlab script" << endl;
    outfile << "figure" << endl;
    outfile << "title('current cycle is " << m_current_time << "')" << endl;
    // outfile<<"rectangle('position',[0,7,1,1],'facecolor',[0,0,0])"<<endl;
    outfile << "hold on" << endl;

    outfile << "set(gca,'xaxislocation','top','yaxislocation','left','ydir','reverse')" << endl;

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            outfile << endl;
            if (i + j * 8 == 0)
            {
                continue;
            }
            if (!petable[i + j * 8].busy)
            {
                outfile << "rectangle('position',[" << i << "," << j << ",1,1],'facecolor',[1,1,1])"
                        << endl;
            }
            else
            {
                outfile << "rectangle('position',[" << i << "," << j << ",1,1],'facecolor',["
                        << (petable[i + j * 8].app_id + 2) % 5 / 5.0 +
                               (1 - ((petable[i + j * 8].app_id + 2) % 5 / 5.0)) / 1.5 << ","
                        << (petable[i + j * 8].app_id + 2) % 8 / 8.0 +
                               (1 - ((petable[i + j * 8].app_id + 2) % 8 / 8.0)) / 1.5 << ","
                        << (petable[i + j * 8].app_id + 2) % 3 / 3.0 +
                               (1 - ((petable[i + j * 8].app_id + 2) % 3 / 3.0)) / 1.5 << "])"
                        << endl;
                if (application_map[petable[i + j * 8].app_id].firstTask ==
                    petable[i + j * 8].task_id)
                {
                    outfile << "text(" << i << ".4," << j << ".4,"
                            << "'*" << petable[i + j * 8].task_id << "*','fontsize',13"
                            << ")" << endl;
                    outfile << "text(" << i << ".15," << j << ".85,"
                            << "'" << petable[i + j * 8].app_id << "'"
                            << ")" << endl;
                }
                else
                {
                    outfile << "text(" << i << ".4," << j << ".4,"
                            << "'" << petable[i + j * 8].task_id << "'"
                            << ")" << endl;
                }
            }
        }
    }

    if (1)
    {
        return;
    }
    /*
        double maxv = 1;
        for (int i = 0; i < 64; ++i)
        {
            if (petable[i].w > maxv)
            {
                maxv = petable[i].w;
            }
            if (petable[i].e > maxv)
            {
                maxv = petable[i].e;
            }
            if (petable[i].n > maxv)
            {
                maxv = petable[i].n;
            }
            if (petable[i].s > maxv)
            {
                maxv = petable[i].s;
            }
        }
        // cout<<"maxv : "<<maxv<<endl;

        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                outfile << "rectangle('position',[" << i << ".8," << j
                        << ",0.2,0.2],'facecolor',[1,1,1]"
                        << ")" << endl; // router

                if (j == 0)
                {
                    if (i == 0)
                    {
                        if (petable[i + j * 8].s != 0)
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].e != 0)
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    }
                    else if (i == 7)
                    {
                        if (petable[i + j * 8].s != 0)
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].w != 0)
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].w) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    }
                    else
                    {
                        if (petable[i + j * 8].s != 0)
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].w != 0)
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].w) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].e != 0)
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    }
                }
                else if (i == 0)
                {
                    if (j == 7)
                    {
                        if (petable[i + j * 8].n != 0)
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].n) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].e != 0)
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    }
                    else
                    {
                        if (petable[i + j * 8].s != 0)
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".8," << j
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].n != 0)
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].n) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].e != 0)
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    }
                }
                else if (j == 7)
                {
                    if (i == 7)
                    {
                        if (petable[i + j * 8].n != 0)
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].n) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].w != 0)
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].w) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    }
                    else
                    {
                        if (petable[i + j * 8].n != 0)
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].n) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << ".9," << j - 1
                                    << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].w != 0)
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].w) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i << "," << j
                                    << ".1,0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                        if (petable[i + j * 8].e != 0)
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*"
                                    << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                        else
                            outfile << "rectangle('position',[" << i + 1 << "," << j
                                    << ",0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    }
                }
                else if (i == 7)
                {
                    if (petable[i + j * 8].s != 0)
                        outfile << "rectangle('position',[" << i << ".8," << j
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                    else
                        outfile << "rectangle('position',[" << i << ".8," << j
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    if (petable[i + j * 8].n != 0)
                        outfile << "rectangle('position',[" << i << ".9," << j - 1
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                << (maxv - petable[i + j * 8].n) / maxv << ")" << endl;
                    else
                        outfile << "rectangle('position',[" << i << ".9," << j - 1
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    if (petable[i + j * 8].w != 0)
                        outfile << "rectangle('position',[" << i << "," << j
                                << ".1,0.8,0.1],'facecolor',[1,1,1].*"
                                << (maxv - petable[i + j * 8].w) / maxv << ")" << endl;
                    else
                        outfile << "rectangle('position',[" << i << "," << j
                                << ".1,0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                }
                else
                {
                    if (petable[i + j * 8].s != 0)
                        outfile << "rectangle('position',[" << i << ".8," << j
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                    else
                        outfile << "rectangle('position',[" << i << ".8," << j
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    if (petable[i + j * 8].n != 0)
                        outfile << "rectangle('position',[" << i << ".9," << j - 1
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*"
                                << (maxv - petable[i + j * 8].n) / maxv << ")" << endl;
                    else
                        outfile << "rectangle('position',[" << i << ".9," << j - 1
                                << ".2,0.1,0.8],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    if (petable[i + j * 8].w != 0)
                        outfile << "rectangle('position',[" << i << "," << j
                                << ".1,0.8,0.1],'facecolor',[1,1,1].*"
                                << (maxv - petable[i + j * 8].w) / maxv << ")" << endl;
                    else
                        outfile << "rectangle('position',[" << i << "," << j
                                << ".1,0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                    if (petable[i + j * 8].e != 0)
                        outfile << "rectangle('position',[" << i + 1 << "," << j
                                << ",0.8,0.1],'facecolor',[1,1,1].*"
                                << (maxv - petable[i + j * 8].s) / maxv << ")" << endl;
                    else
                        outfile << "rectangle('position',[" << i + 1 << "," << j
                                << ",0.8,0.1],'facecolor',[1,1,1].*" << 1 << ")" << endl;
                }
            }
        }
    */
}

void SimTask::applicationGraph(application &app)
{
    int n = app.app_id;
    ostringstream str;
    str << "./Tools/graph/" << n << ".dot";
    // str.c_str();
    ofstream outfile(str.str().c_str());
    outfile << "digraph{" << endl;
    // cout<<app.taskOrder.size()<<endl;
    for (int i = 0; i < app.tasks.size(); i++)
    {
        // a [label="Foo"];
        outfile << i << " [label=\"" << i << "-" << app.tasks[i] << "\"];" << endl;
    }
    for (int i = 0; i < app.tasks.size(); i++)
    {
        for (int j = 0; j < app.tasks.size(); j++)
        {
            if (app.tasks_communication[i][j] > 0)
                // LR_5 -> LR_7 [ label = "S(b)" ];
                outfile << i << "->" << j << " [label=\"" << app.tasks_communication[i][j] << "\"];"
                        << endl;
        }
    }
    outfile << "tt"
            << " [label=\"";
    for (int i = 0; i < app.task_order.size(); i++)
    {
        for (int j = 0; j < app.task_order[i].size(); j++)
        {
            outfile << app.task_order[i][j] << "-";
        }
        outfile << "   ";
    }
    outfile << "\"];" << endl;
    outfile << "}" << endl;
    outfile.close();

    ostringstream str1;
    str1 << "dot -Tpng ./Tools/graph/" << n << ".dot -o ./Tools/graph/" << n << ".png";
    // str1.c_str();
    cout << str1.str() << endl;
    system(str1.str().c_str());
}

void SimTask::read_tgff()
{
    int comm_multi = 1;
    int max_tasks_size;
    int min_tasks_size;

    max_tasks_size = app_size_max;
    min_tasks_size = app_size_min;

    ifstream inFile(tgff_file_name.c_str(), ios::in);
    if (!inFile)
    {
        cerr << "Open TGFF file Fail" << endl;
        abort();
    }

    string line;
    char flag;
    // state1 use to control table tpye: 0: communication table 1: PE 2: task graph
    // state2 is used in table
    int state1 = 0; // communication table == 0, task_tabel == 1, task graph == 2;
    int state2 = 0; //
    int app_num = 0;
    int period = 0;
    vector<unsigned long> tasks;
    vector<vector<unsigned long> > tasks_communication;
    vector<int> deadline;
    map<int, int> commun_num;
    map<int, int> task_cycle;

    std::vector<double> pe_attrib;
    vector<int> task_time_;

    bool just_a_pe_table = true;
    std::vector<double> pe_attrib_same;
    vector<int> task_time_same;

    std::vector<int> tasks_size;
    tasks_size.resize(50, 0);

    bool flag_end = 0;
    int app_num_max = 2000;

    while (getline(inFile, line))
    {
        if (flag_end)
        {
            break;
        }
        istringstream s_line(line);
        istringstream s_line_value(line);
        s_line >> flag;
        int type = 0, value = 0;
        int pe_id;
        if (flag == '@')
        {
            s_line >> flag;
            if (flag == 'C')
            {
                state1 = 1;
                state2 = 0;
                continue;
            }
            if (flag == 'P')
            {
                state1 = 2;
                state2 = 0;
                s_line >> flag;
                s_line >> pe_id;
                continue;
            }
            if (flag == 'T')
            {
                state1 = 3;
                state2 = 0;
                continue;
            }
        }

        if (state1 == 1)
        {
            if (flag == '0')
            {
                state2 = 1; // jump to the begin of data
            }
            if (flag == '}')
            {
                state2 = 0;
                flag = 'j'; // any value
                continue;
            }
            if (state2 == 0)
            {
                continue;
            }
            else
            {
                s_line_value >> type;
                s_line_value >> value;
                commun_num[type] = value * comm_multi;
                // cout << "type is " << type << "  packet number is " << value << endl;
            }
        }

        if (state1 == 2)
        {
            if (flag == '#')
            {
                switch (state2)
                {
                case 0:
                    state2 = 1;
                    flag = 'j'; // any value
                    continue;
                    break;
                case 1:
                    state2 = 2;
                    flag = 'j'; // any value
                    getline(inFile, line);
                    continue;
                    break;
                default:
                    break;
                }
            }

            if (flag == '}')
            {

                if (pe_id == 0)
                {
                    pe_attrib_same = pe_attrib;
                    task_time_same = task_time_;
                }

                if (pe_id > 0)
                {
                    just_a_pe_table = false;
                }

                state2 = 0;
                m_pe_list[pe_id].set_pe_attrib(pe_attrib, task_time_);
                if (1)
                {
                    cout << "pe_id: " << pe_id << endl << "pe_attrib" << endl;
                    for (int i = 0; i < pe_attrib.size(); i++)
                    {
                        cout << pe_attrib[i] << " ";
                    }
                    cout << endl << "task_time_" << endl;
                    for (int i = 0; i < task_time_.size(); i++)
                    {
                        cout << task_time_[i] << " ";
                    }
                    cout << endl;
                }
                pe_attrib.clear();
                task_time_.clear();
                flag = 'j'; // any value
                continue;
            }

            if (state2 == 0)
            {
                continue;
            }

            if (state2 == 1)
            {
                double temp;
                while (s_line_value >> temp)
                {
                    pe_attrib.push_back(temp);
                }
            }

            if (state2 == 2)
            {
                s_line_value >> type;
                int temp;
                s_line_value >> temp;
                s_line_value >> value;
                // task_cycle[type] = value;
                task_time_.push_back(value);
                // cout << "type is " << type << "  exec_time is " << value << endl;
            }
        }

        if (state1 == 3)
        {
            if (state2 == 0)
            {
                if (flag == 'P')
                {
                    char temp_char;
                    int next = 0;
                    while (s_line_value >> temp_char)
                    {
                        if (temp_char == 'D')
                        {
                            next = 1;
                            // continue;
                        }
                        if (next == 1)
                        {
                            s_line_value >> period;
                            break;
                        }
                    }
                }
                state2 = 1;
            }

            // cout << "new line : " << line << "  |  state2 is:  " << state2 << endl;
            if (flag == 'T')
            {
                state2 = 2; // read Task
            }

            if (flag == 'A')
            {
                char temp_char;
                s_line >> temp_char;
                if (state2 != 3 && temp_char == 'R') // first time: map resize
                {
                    int task_num = tasks.size();
                    tasks_communication.resize(task_num);
                    for (int i = 0; i < task_num; i++)
                        tasks_communication[i].resize(task_num);

                    state2 = 3; // read ARC
                }
            }

            if (flag == 'H')
            {
                state2 = 4;
            }

            if (flag == '}')
            {
                // if (tasks.size() == 1)
                //{
                //    assert("size == 1, exit");
                //}
                if (min_tasks_size <= tasks.size() && tasks.size() <= max_tasks_size)
                {

                    tasks_size[tasks.size()] = tasks_size[tasks.size()] + 1;

                    if (tasks.size() == 1)
                    {
                        tasks_communication.resize(1);
                        tasks_communication[0].push_back(0);
                    }

                    srand(app_num);

                    int exec_time_1 = 1;
                    if (period != 0)
                    {
                        period = rand() % (period_max - period_min + 1) + period_min;
                        exec_time_1 = rand() % (period_exec_times_max - period_exec_times_min + 1) +
                                      period_exec_times_min;
                    }
                    application_map[app_num] =
                        application(argument_cfg, app_num, period, exec_time_1, deadline, tasks,
                                    tasks_communication, mapping_algs);
                    if (0)
                    {
                        cout << "app: " << app_num << endl << "tasks" << endl;
                        for (int i = 0; i < tasks.size(); i++)
                        {
                            cout << tasks[i] << " ";
                        }
                        cout << endl << "tasks_communication" << endl;
                        for (int i = 0; i < tasks_communication.size(); i++)
                        {
                            for (int j = 0; j < tasks_communication[i].size(); j++)
                            {
                                if (tasks_communication[i][j] != 0)
                                {
                                    cout << i << " -> " << j << "  " << tasks_communication[i][j]
                                         << endl;
                                }
                            }
                        }
                        cout << "period:" << endl;
                        cout << period;
                        cout << endl << "deadline:" << endl;
                        for (int i = 0; i < deadline.size(); i++)
                        {
                            if (deadline[i] != 0)
                            {
                                cout << i << " - " << deadline[i] << "    ";
                            }
                        }
                        cout << endl;
                    }
                    app_num++;
                    if (app_num >= app_num_max)
                    {
                        flag_end = 1;
                    }
                }
                // cout << "push back a app, app id is " << app_num << endl;
                state2 = 5;
                period = 0;
                tasks.clear();
                tasks_communication.clear();
                flag = 'j'; // any value
                continue;
            }

            if (state2 == 2)
            {
                char temp_char;
                int next = 0;
                while (s_line_value >> temp_char)
                {
                    if (temp_char == 'E')
                    {
                        next = 1;
                        // continue;
                    }
                    if (next == 1)
                    {
                        int value;
                        s_line_value >> value;
                        // cout << "value" << value << endl;
                        tasks.push_back(value);
                        // cout << "app_num " << app_num << " value cycle " << task_cycle[value] <<
                        // endl;
                        break;
                    }
                }
            }

            if (state2 == 3)
            {
                char temp_char;
                // cout << "enter /...." << endl;
                int next = 0;
                int i, j;
                while (s_line_value >> temp_char)
                {
                    if (temp_char == 't')
                    {
                        int tt;
                        s_line_value >> tt;
                        s_line_value >> temp_char;
                        if (next == 0)
                        {
                            s_line_value >> i;
                            next = 1;
                        }
                        else
                        {
                            s_line_value >> j;
                        }
                        continue;
                    }
                    if (temp_char == 'E')
                    {
                        int value;
                        s_line_value >> value;
                        tasks_communication[i][j] = commun_num[value];
                        // cout << "app_num " << app_num << " i " << i << " j " << j << " value
                        // commm " <<
                        // commun_num[value] <<
                        // endl;
                        break;
                    }
                }
            }

            if (state2 == 4)
            {
                deadline.resize(tasks.size(), 0);
                char temp_char;
                int next = 0;
                int task_id;
                while (s_line_value >> temp_char)
                {
                    if (temp_char == 't')
                    {
                        int tt;
                        s_line_value >> tt;
                        s_line_value >> temp_char;
                        s_line_value >> task_id;
                        continue;
                    }
                    if (temp_char == 'T')
                    {
                        int value;
                        s_line_value >> value;
                        deadline[task_id] = value;
                        break;
                    }
                }
            }
        }
    }

    if (just_a_pe_table)
    {
        for (int i = 0; i < m_pe_num; i++)
        {
            m_pe_list[i].set_pe_attrib(pe_attrib_same, task_time_same);
        }
        task_time = task_time_;
    }

    cout << "task size statatic:\n";
    for (int i = 0; i < tasks_size.size(); i++)
    {
        if (tasks_size[i])
        {
            cout << tasks_size[i] << "\t- [" << i << "]"
                 << "\n";
        }
    }
    cout << endl;
}

int SimTask::get_flow_id(int a, int b, int c)
{
    int ID = 10000 * a + 100 * b + c;
    // modify by 2016/05/11 for a same source, no congestion.
    // int ID = 10000 * a + 100 * b;
    return ID;
}

void SimTask::cal_MRD(int app_id)
{
    vector<unsigned int> task_mapped = running_app_state[app_id];
    int num = 0;
    double hop_sum = 0;

    for (int i = 0; i < task_mapped.size(); i++)
    {
        int src = task_mapped[i];
        for (int j = i + 1; j < task_mapped.size(); j++)
        {
            int des = task_mapped[j];
            int hop = abs(src / 8 - des / 8) + abs(src % 8 - des % 8);
            hop_sum += hop;
            num++;
        }
    }
    double mrd_value = hop_sum / num;
    MRD.push_back(mrd_value);
}

void SimTask::print_in_main()
{
    ofstream MRD_file("./MRD.txt", std::ofstream::app);
    for (int i = 0; i < MRD.size(); i++)
    {
        MRD_file << MRD[i] << endl;
    }
}

bool SimTask::simulation_over()
{
    return sim_over;
}

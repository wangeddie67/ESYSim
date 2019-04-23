#include "esymap_task.h"

#include "math.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

bool SimTask::task_mapping_new(application &app)
{
    updateNeighborsFreePE();
    int task_num = app.tasks.size();
    running_app_state[app.app_id].resize(task_num);
    petable[0].busy = 1;

    if (pecount < app.tasks.size())
    {
        return 0;
    }
    int firstTask = app.firstTask;
    int firstNode = first_node(task_num);
    if (petable[firstNode].neighborsFreeNum < 2)
    {
        return 0;
    }
    vector<vector<int> > task_order = app.task_order;
    petable[firstNode].busy = 1;
    petable[firstNode].app_id = app.app_id;
    petable[firstNode].task_id = firstTask;
    running_app_state[app.app_id][firstTask] = firstNode;
    update_link_info_new_node_XY(true, 1, app.app_id, firstTask);
    // node_statistic[0] += 1;
    //  cout << "task " << firstTask << "\tof application\t" << app.app_id << " -> "
    //       << "PE\t" << firstNode << endl;
    int size_x = 8;
    int size_y = 8;

    for (int task_i = 1; task_i < app.tasks.size(); ++task_i)
    {
        int hop = 0;
        int minHop = 1000;
        int current_taskid = task_order[task_i][0];
        int current_predecessor = task_order[task_i][1];
        int current_predecessor_PE = running_app_state[app.app_id][current_predecessor];
        std::vector<int> nextPeList;

        for (int i = 1; i < m_pe_num; ++i)
        {
            if (!petable[i].busy)
            {
                hop = abs(current_predecessor_PE / size_x - i / size_x) +
                      abs(current_predecessor_PE % size_x - i % size_x);
                if (hop < minHop)
                {
                    minHop = hop;
                }
            }
        }

        for (int i = 1; i < m_pe_num; ++i)
        {
            if (!petable[i].busy)
            {
                hop = abs(current_predecessor_PE / size_x - i / size_x) +
                      abs(current_predecessor_PE % size_x - i % size_x);
                if (hop <= minHop /*+MSR_Config*/ && hop >= minHop)
                {
                    nextPeList.push_back(i);
                }
            }
        }

        double DyXY_to_int_interval = 0.5;

        double interference_factor_min = 10000000;
        double interference_factor = 0;
        int index = 0;

        int nextNode;

        int hop_from_first = 100;

        bool use_DyXY = 0;

        std::vector<int> min_inde_list;
        cout << "next " << nextPeList.size() << " ---------------" << endl;
        for (int i = 0; i < nextPeList.size(); i++)
        {
            if (use_DyXY)
            {
                running_app_state[app.app_id][current_taskid] = nextPeList[i];
                update_link_info_new_node_XY(true, 1, app.app_id, current_taskid);
                interference_factor =
                    get_interference_factor_DyXY(app.app_id, current_taskid, nextPeList[i], 0);
                update_link_info_new_node_XY(false, 1, app.app_id, current_taskid);
                running_app_state[app.app_id][current_taskid] = 0;

                //  cout<<"next "<<interference_factor<<"\t";

                interference_factor = (int)(interference_factor / DyXY_to_int_interval);

                cout << "next " << interference_factor << endl;
            }
            else
                interference_factor =
                    get_interference_factor(app.app_id, current_taskid, nextPeList[i], 0);

            if (interference_factor < interference_factor_min)
            {
                interference_factor_min = interference_factor;
                // index = i;
                nextNode = nextPeList[i];
                min_inde_list.clear();
                min_inde_list.push_back(nextPeList[i]);
            }
            else if (interference_factor == interference_factor_min)
            {
                min_inde_list.push_back(nextPeList[i]);
            }
        }
        cout << "next " << nextPeList.size() << " ---------------\nnext" << endl;

        std::vector<int> nextPeList_more;
        int nextNode_new = 0;

        double BFV_max = -100;
        double interference_factor_search = interference_factor_min;
        double alpha = 2;

        int hop_log = minHop;

        int MSR = interference_factor_min / alpha;
        int MSR_Config = -1;

        if (use_DyXY)
            MSR = MSR * DyXY_to_int_interval;
        if (MSR_Config >= 0)
            MSR = MSR_Config;

        cout << "next interference_factor_min " << interference_factor_min << "  MSR " << MSR
             << endl;
        if (MSR > 0)
        {
            nextPeList_more.clear();
            for (int i = 1; i < m_pe_num; ++i)
            {
                if (!petable[i].busy)
                {
                    hop = abs(current_predecessor_PE / size_x - i / size_x) +
                          abs(current_predecessor_PE % size_x - i % size_x);
                    if (hop <= minHop + MSR && hop > minHop)
                    {
                        nextPeList_more.push_back(i);
                    }
                }
            }

            for (int i = 0; i < nextPeList_more.size(); i++)
            {
                double interference_factor_temp = 0;
                if (use_DyXY)
                {
                    running_app_state[app.app_id][current_taskid] = nextPeList_more[i];
                    update_link_info_new_node_XY(true, 1, app.app_id, current_taskid);
                    interference_factor_temp = get_interference_factor_DyXY(
                        app.app_id, current_taskid, nextPeList_more[i], 0);
                    update_link_info_new_node_XY(false, 1, app.app_id, current_taskid);
                    running_app_state[app.app_id][current_taskid] = 0;

                    interference_factor_temp = (int)(interference_factor / DyXY_to_int_interval);
                }
                else
                    interference_factor_temp =
                        get_interference_factor(app.app_id, current_taskid, nextPeList_more[i], 0);

                int hop_temp = abs(current_predecessor_PE / size_x - nextPeList_more[i] / size_x) +
                               abs(current_predecessor_PE % size_x - nextPeList_more[i] % size_x);
                double BFV = interference_factor_min - interference_factor_temp -
                             alpha * (hop_temp - minHop) + 1.0 / (hop_temp - minHop + 1);
                // cout<<"hop_temp "<<hop_temp<<" minHop "<<minHop<<" BFV "<<BFV<<endl;

                if (BFV < 0)
                    continue;

                cout << "in MSR" << endl;
                if (BFV > BFV_max)
                {
                    nextNode = nextPeList_more[i];
                    BFV_max = BFV;
                    min_inde_list.clear();
                    min_inde_list.push_back(nextPeList_more[i]);
                    interference_factor_search = interference_factor_temp;
                }
                else if (BFV == BFV_max)
                {
                    min_inde_list.push_back(nextPeList_more[i]);
                }
            }
        }
        interference_factor_min = interference_factor_search;

        if (min_inde_list.size() == 1)
        {
        }
        else
        {
            //----------------------
            bool flag_ii = false;
            if (app.connect_1[task_i].size() + app.connect_2[task_i].size() != 0)
            {
                flag_ii = true;
                int hop_sum_min = 1000000;
                for (int i = 0; i < min_inde_list.size(); i++)
                {
                    int pe1 = min_inde_list[i];
                    int hop_sum = 0;
                    for (int j = 0; j < app.connect_1[task_i].size(); j++)
                    {
                        int connected_PE = running_app_state[app.app_id][app.connect_1[task_i][j]];
                        int hop_t = abs(pe1 / size_x - connected_PE / size_x) +
                                    abs(pe1 % size_x - connected_PE % size_x);
                        hop_sum += hop_t;
                        //*(app.tasks_communication[current_taskid][app.connect_1[task_i][j]]+
                        //        app.tasks_communication[app.connect_1[task_i][j]][current_taskid]);
                    }
                    for (int j = 0; j < app.connect_2[task_i].size(); j++)
                    {
                        int connected_PE = running_app_state[app.app_id][app.connect_2[task_i][j]];
                        int hop_t = max(abs(pe1 / size_x - connected_PE / size_x),
                                        abs(pe1 % size_x - connected_PE % size_x));
                        hop_sum += hop_t;
                    }
                    if (hop_sum < hop_sum_min)
                    {
                        hop_sum_min = hop_sum;
                        nextNode = pe1;
                    }
                }
            }
            else if (0) // app.connect_3[task_i].size() != 0)
            {
                int hop_sum_max = -100;
                for (int i = 0; i < min_inde_list.size(); i++)
                {
                    int pe1 = min_inde_list[i];
                    int hop_sum = 0;
                    for (int j = 0; j < app.connect_3[task_i].size(); j++)
                    {
                        int connected_PE = running_app_state[app.app_id][app.connect_3[task_i][j]];
                        int hop_t = abs(pe1 / size_x - connected_PE / size_x) +
                                    abs(pe1 % size_x - connected_PE % size_x);
                        hop_sum += hop_t;
                        //*(app.tasks_communication[current_taskid][app.connect_1[task_i][j]]+
                        //        app.tasks_communication[app.connect_1[task_i][j]][current_taskid]);
                    }
                    if (hop_sum > hop_sum_max)
                    {
                        hop_sum_max = hop_sum;
                        nextNode = pe1;
                    }
                }
            }

            if (flag_ii)
                cout << "cam_1766--\n";
        }

        petable[nextNode].busy = 1;
        petable[nextNode].app_id = app.app_id;
        petable[nextNode].task_id = current_taskid;
        running_app_state[app.app_id][current_taskid] = nextNode;
        update_link_info_new_node_XY(true, 1, app.app_id, current_taskid);
        // cout << "task " << current_taskid << "\tof application\t" << app.app_id << " -> "
        //      << "PE\t" << nextNode << endl;
    }

    return 1;
}

int SimTask::get_interference_factor(int app_id, int task_id, int node, bool flag)
{
    int node_x = node % 8;
    int node_y = node / 8;
    // get the flow list bellow
    std::vector<unsigned int> task_mapped = running_app_state[app_id];
    std::vector<std::vector<int> > flow;
    for (int i = 0; i < task_mapped.size(); i++)
    {
        if (task_mapped[i] == 0)
        {
            continue;
        }
        if (application_map[app_id].tasks_communication[i][task_id])
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(i),
                                 static_cast<int>(task_id));
            std::vector<int> temp;
            temp.push_back(task_mapped[i]);
            temp.push_back(node);
            temp.push_back(application_map[app_id].tasks_communication[i][task_id]);
            temp.push_back(ID);
            flow.push_back(temp);
        }
        if (application_map[app_id].tasks_communication[task_id][i])
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(task_id),
                                 static_cast<int>(i));
            std::vector<int> temp;
            temp.push_back(node);
            temp.push_back(task_mapped[i]);
            temp.push_back(application_map[app_id].tasks_communication[task_id][i]);
            temp.push_back(ID);
            flow.push_back(temp);
        }
    }
    // cal factor
    int factor = get_interference_factor_xy(flow, flag);
    // int factor = get_interference_factor_xy_icr(flow, flag, app_id);
    return factor;
}

int SimTask::get_interference_factor_xy(std::vector<std::vector<int> > flow, bool flag)
{
    int factor = 0;
    int factor_max = 0;
    for (int i = 0; i < flow.size(); i++)
    {
        int src_x = flow[i][0] % 8;
        int src_y = flow[i][0] / 8;
        int des_x = flow[i][1] % 8;
        int des_y = flow[i][1] / 8;
        int comm = 0; // flow[i][2];
        int ID = 0;   // flow[i][3];

        std::vector<int> inter_flow;

        if (src_x < des_x)
        {
            for (int k = src_x; k < des_x; k++)
            {
                // cout<<"1 "<<k<<endl;
                int PE_ID = src_y * 8 + k;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].e.begin();
                for (; it != petable[PE_ID].e.end(); it++)
                {
                    int flow_id = (it->second)[0];

                    int flow_src = flow_id / 100;
                    if (flow_src == flow[i][0])
                        continue;

                    if (std::find(inter_flow.begin(), inter_flow.end(), flow_id) ==
                        inter_flow.end())
                    {
                        inter_flow.push_back(flow_id);
                        // factor = factor + 1000 * 1 + comm;
                        factor += 1;
                    }
                }
            }
        }
        else if (src_x > des_x)
        {
            for (int k = src_x; k > des_x; k--)
            {
                // cout<<"src_x "<<src_x<<"  des_x "<<des_x<<endl;
                // cout<<"2 "<<k<<endl;
                int PE_ID = src_y * 8 + k;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].w.begin();
                for (; it != petable[PE_ID].w.end(); it++)
                {
                    int flow_id = (it->second)[0];

                    int flow_src = flow_id / 100;
                    if (flow_src == flow[i][0])
                        continue;
                    // flow_id = flow_id / 100;
                    if (std::find(inter_flow.begin(), inter_flow.end(), flow_id) ==
                        inter_flow.end())
                    {
                        inter_flow.push_back(flow_id);
                        //                        factor = factor + 1000 * 1 + comm;
                        factor += 1;
                    }
                }
            }
        }
        else
        {
        }
        if (src_y < des_y)
        {
            for (int k = src_y; k < des_y; k++)
            {
                // cout<<"3 "<<k<<endl;
                int PE_ID = k * 8 + des_x;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].s.begin();
                for (; it != petable[PE_ID].s.end(); it++)
                {
                    int flow_id = (it->second)[0];
                    int flow_src = flow_id / 100;
                    if (flow_src == flow[i][0])
                        continue;
                    if (std::find(inter_flow.begin(), inter_flow.end(), flow_id) ==
                        inter_flow.end())
                    {
                        inter_flow.push_back(flow_id);
                        //                        factor = factor + 1000 * 1 + comm;
                        factor += 1;
                    }
                }
            }
        }
        else if (src_y > des_y)
        {
            for (int k = src_y; k > des_y; k--)
            {
                // cout<<"4 "<<k<<endl;
                int PE_ID = k * 8 + des_x;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].n.begin();
                for (; it != petable[PE_ID].n.end(); it++)
                {
                    int flow_id = (it->second)[0];
                    int flow_src = flow_id / 100;
                    if (flow_src == flow[i][0])
                        continue;
                    if (std::find(inter_flow.begin(), inter_flow.end(), flow_id) ==
                        inter_flow.end())
                    {
                        inter_flow.push_back(flow_id);
                        //                        factor = factor + 1000 * 1 + comm;
                        factor += 1;
                    }
                }
            }
        }
        else
        {
        }
        inter_flow.clear();

        // factor_max += factor;
        if (factor_max < factor)
        {
            factor_max = factor;
            factor = 0;
        }
        // if (flag)
        //{
        //   flow_factor_log[ID].push_back(factor);
        //}
    }
    return factor_max; /// flow.size();
}

double SimTask::get_interference_factor_DyXY(int app_id, int task_id, int node, bool flag)
{
    application task_graph = application_map[app_id];
    vector<unsigned long> tasks = task_graph.tasks;
    vector<vector<unsigned long> > tasks_communication = task_graph.tasks_communication;
    int n = tasks.size();
    std::vector<unsigned int> map_result = running_app_state[app_id];

    std::vector<std::vector<double> > flow;
    for (int i = 0; i < n; i++)
    {
        if (map_result[i] == 0)
            continue;

        double comm1 = tasks_communication[i][task_id];
        if (comm1 != 0)
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(i),
                                 static_cast<int>(task_id));
            int src = map_result[i];
            int des = node;
            std::vector<double> temp;
            temp.push_back(src);
            temp.push_back(des);
            temp.push_back(comm1);
            temp.push_back(ID);
            flow.push_back(temp);
        }

        double comm2 = tasks_communication[task_id][i];
        if (comm2 != 0)
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(task_id),
                                 static_cast<int>(i));
            int des = map_result[i];
            int src = node;
            std::vector<double> temp;
            temp.push_back(src);
            temp.push_back(des);
            temp.push_back(comm2);
            temp.push_back(ID);
            flow.push_back(temp);
        }
    }

    double factor = 0;
    double factor_max = 0;
    for (int i = 0; i < flow.size(); i++)
    {
        int src_x = (int)flow[i][0] % 8;
        int src_y = (int)flow[i][0] / 8;
        int des_x = (int)flow[i][1] % 8;
        int des_y = (int)flow[i][1] / 8;
        double comm = flow[i][2];
        int ID = flow[i][3];

        factor = 0;

        double f_i_sum = 0;

        for (int pe = 0; pe < 64; pe++)
        {
            int pe_x = pe % 8;
            int pe_y = pe / 8;

            if (pe_x < src_x && pe_x < des_x)
                continue;

            if (pe_x > src_x && pe_x > des_x)
                continue;

            if (pe_y < src_y && pe_y < des_y)
                continue;

            if (pe_y > src_y && pe_y > des_y)
                continue;

            if (pe == (int)flow[i][1])
                continue;

            int hops_to_src = abs(src_x - pe_x) + abs(src_y - pe_y);

            if (pe_x < des_x)
            {
                double f_i = petable[pe].e[ID][1];
                std::map<int, std::vector<double> >::iterator it = petable[pe].e.begin();
                double load = 0;
                int load_num = 0;
                for (; it != petable[pe].e.end(); it++)
                {
                    //                  int flow_id = (it->second)[0];
                    //                  if(flow_id == ID)
                    //                      continue;

                    //                  int flow_src = flow_id / 100;
                    //                  if(flow_src == flow[i][0])
                    //                      continue;
                    // cout<<"load-"<<(it->second)[1]<<endl;
                    if ((it->second)[1] < 0.1)
                        continue;
                    load += (it->second)[1];
                    load_num++;
                }
                if (load > 1)
                {
                    factor += (load) / load_num * f_i;
                    f_i_sum += f_i;
                }
            }

            if (pe_x > des_x)
            {

                double f_i = petable[pe].w[ID][1];
                std::map<int, std::vector<double> >::iterator it = petable[pe].w.begin();

                double load = 0;
                int load_num = 0;
                for (; it != petable[pe].w.end(); it++)
                {
                    if ((it->second)[1] < 0.1)
                        continue;
                    load += (it->second)[1];
                    load_num++;
                }
                if (load > 1)
                {
                    factor += (load) / load_num * f_i;
                    f_i_sum += f_i;
                }
            }

            if (pe_y < des_y)
            {
                double f_i = petable[pe].s[ID][1];
                std::map<int, std::vector<double> >::iterator it = petable[pe].s.begin();

                double load = 0;
                int load_num = 0;
                for (; it != petable[pe].s.end(); it++)
                {
                    if ((it->second)[1] < 0.1)
                        continue;
                    load += (it->second)[1];
                    load_num++;
                }
                if (load > 1)
                {
                    factor += (load) / load_num * f_i;
                    f_i_sum += f_i;
                }
            }

            if (pe_y > des_y)
            {
                double f_i = petable[pe].n[ID][1];
                std::map<int, std::vector<double> >::iterator it = petable[pe].n.begin();

                double load = 0;
                int load_num = 0;
                for (; it != petable[pe].n.end(); it++)
                {
                    if ((it->second)[1] < 0.1)
                        continue;
                    load += (it->second)[1];
                    load_num++;
                }
                if (load > 1)
                {
                    factor += (load) / load_num * f_i;
                    f_i_sum += f_i;
                }
            }
        }

        if (f_i_sum > 1)
        {
            //  factor = factor / f_i_sum;
        }

        if (factor_max < factor)
        {
            factor_max = factor;
        }
        cout << "factor-" << factor << endl;
    }
    return factor_max;
}

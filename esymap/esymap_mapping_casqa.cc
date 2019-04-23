#include "esymap_task.h"


bool SimTask::task_mapping_casqa(application &app)
{
    if (1)
    {
        cout << "------------------mapping info " << endl;
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                int peid = j + i * 8;
                if (petable[peid].busy)
                {
                    // if (petable[peid].app_id == app.app_id)
                    {
                        cout << petable[peid].app_id << "\t"; // = firstTask;
                    }
                }
                else
                {
                    cout << "-" << peid << "-"
                         << "\t";
                }
            }
            cout << endl;
        }
    }

    updateNeighborsFreePE();
    int task_num = app.tasks.size();
    running_app_state[app.app_id].clear();
    running_app_state[app.app_id].resize(task_num, 0);
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
    cout << "mapping result for App " << app.app_id << " :" << endl;
    // mapping
    petable[firstNode].busy = 1;
    petable[firstNode].app_id = app.app_id;
    petable[firstNode].task_id = firstTask;
    running_app_state[app.app_id][firstTask] = firstNode;
    update_link_info_new_node_XY(true, 1, app.app_id, firstTask);
    cout << "task " << firstTask << "\tof application\t" << app.app_id << " -> "
         << "PE\t" << firstNode << endl;
    updateNeighborsFreePE();
    int size_x = 8;
    int size_y = 8;

    std::vector<int> MAP;
    std::vector<int> MET;
    std::vector<int> UNM;

    MAP.push_back(firstTask);
    for (int i = 0; i < task_order.size(); ++i)
    {
        if (task_order[i][1] == firstTask)
        {
            MET.push_back(task_order[i][0]);
        }
    }

    // push back UNM task

    double alpha = casqa_alpha;
    int R_max = sqrt(task_num) / 2;
    double tau = task_num * alpha;
    cout << "alpha  " << alpha << "  R_max " << R_max << "  tau  " << tau << endl;
    for (int r = 1; r <= R_max; ++r)
    {
        // task mapping here =====================

        for (int MD = 1; MD <= 4 * r; ++MD)
        {
            for (int i_in_MET = 0; i_in_MET < MET.size();)
            {
                int current_taskid = MET[0];
                int current_predecessor = -1; // = task_order[current_taskid][1];
                for (int i_1 = 0; i_1 < task_order.size(); ++i_1)
                {
                    if (task_order[i_1][0] == current_taskid)
                    {
                        current_predecessor = task_order[i_1][1];
                    }
                }

                // cout<<"current_predecessor  "<<current_predecessor<<endl;
                int current_predecessor_PE = running_app_state[app.app_id][current_predecessor];

                // 1. ---get the avaiable nodes in current radius with MD hop distance from tc parent
                std::vector<int> PEs_in_current_radius;
                std::vector<int> nextPeList;
                for (int i = 1; i < m_pe_num; ++i)
                {
                    if (!petable[i].busy)
                    {
                        int rr_temp = max(abs(firstNode / size_x - i / size_x), abs(firstNode % size_x - i % size_x));
                        if (rr_temp == r)
                        {
                            PEs_in_current_radius.push_back(i);
                        }
                    }
                }

                for (int i = 0; i < PEs_in_current_radius.size(); ++i)
                {
                    int jj = PEs_in_current_radius[i];
                    if (!petable[jj].busy)
                    {
                        int hop = abs(current_predecessor_PE / size_x - jj / size_x) +
                                  abs(current_predecessor_PE % size_x - jj % size_x);
                        if (hop == MD)
                        {
                            nextPeList.push_back(jj);
                        }
                    }
                }

                if (0)
                {
                    cout << "------------------------debug info------------" << endl;
                    cout << "r " << r << " MD " << MD << endl;
                    cout << "nextPeList.size() " << nextPeList.size() << " :  ";
                    for (int i = 0; i < nextPeList.size(); ++i)
                    {
                        cout << nextPeList[i] << "\t";
                    }
                    cout << endl;
                }
                // 1. ---end

                cout << "nextPeList,size() " << nextPeList.size() << "  ----  "
                     << " r " << r << " MD " << MD << " tau " << tau << " R_max " << R_max << "  MAP.size()  "
                     << MAP.size() << " task_num " << task_num << endl;

                // 2. ---choose the smaller ICE value node
                int nextNode = 0;
                if (!nextPeList.empty())
                {
                    int ICE = 100000;
                    for (int i = 0; i < nextPeList.size(); i++)
                    {
                        int ICE_temp = get_ICE(app, current_taskid, nextPeList[i]);
                        if (ICE_temp < ICE)
                        {
                            ICE = ICE_temp;
                            nextNode = nextPeList[i];
                        }
                    }

                    // ----mapping---
                    petable[nextNode].busy = 1;
                    petable[nextNode].app_id = app.app_id;
                    petable[nextNode].task_id = current_taskid;
                    running_app_state[app.app_id][current_taskid] = nextNode;
                    update_link_info_new_node_XY(true, 1, app.app_id, current_taskid);
                    cout << "task " << current_taskid << "\tof application\t" << app.app_id << " -> "
                         << "PE\t" << nextNode << endl;
                    updateNeighborsFreePE();
                    // ----mapping end---
                    MD = 1; // reset MD
                    MAP.push_back(current_taskid);
                    MET.erase(find(MET.begin(), MET.end(), current_taskid));
                    bool new_MET = false;
                    for (int i = 0; i < task_order.size(); ++i)
                    {
                        int task_id_temp = task_order[i][0];
                        if (running_app_state[app.app_id][task_id_temp] > 0)
                        {
                            continue;
                        }

                        if (find(MET.begin(), MET.end(), task_id_temp) != MET.end())
                        {
                            continue;
                        }

                        if (app.tasks_communication[task_id_temp][current_taskid])
                        {
                            MET.push_back(task_id_temp);
                            new_MET = true;
                        }

                        if (app.tasks_communication[current_taskid][task_id_temp])
                        {
                            MET.push_back(task_id_temp);
                            new_MET = true;
                        }
                    }
                    if (new_MET)
                    {
                        MD = 1;
                    }
                }
                else
                {
                    break;
                }
                // 2. ---end
            }
        }

        // task mapping end ===========================
        if (MAP.size() == task_num)
        {
            return 1;
        }
        else if (r == R_max && task_num - MAP.size() < tau)
        {
            R_max = R_max + 1;
            tau = tau * alpha;
            cout << "tau change " << tau << " r " << r << endl;
        }
    }

    cout << "fail "
         << " tau " << tau << " R_max " << R_max << "  MAP.size()  " << MAP.size() << " task_num " << task_num << endl;
    // restore
    for (int i = 0; i < running_app_state[app.app_id].size(); ++i)
    {
        if (running_app_state[app.app_id][i] != 0)
        {
            petable[running_app_state[app.app_id][i]].busy = 0;
        }
    }
    update_link_info(0, app.app_id, running_app_state[app.app_id]);
    return 0;
}

int SimTask::get_ICE(const application &app, int task_id, int node)
{
    std::vector<unsigned int> mapped_result = running_app_state[app.app_id];
    int app_id = app.app_id;
    // get the flow of task in node
    std::vector<std::vector<int> > flow;
    for (int i = 0; i < mapped_result.size(); i++)
    {
        if (mapped_result[i] == 0)
        {
            continue;
        }
        if (application_map[app_id].tasks_communication[i][task_id])
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(i), static_cast<int>(task_id));
            std::vector<int> temp;
            temp.push_back(mapped_result[i]);
            temp.push_back(node);
            temp.push_back(application_map[app_id].tasks_communication[i][task_id]);
            temp.push_back(ID);
            flow.push_back(temp);
        }
        if (application_map[app_id].tasks_communication[task_id][i])
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(task_id), static_cast<int>(i));
            std::vector<int> temp;
            temp.push_back(node);
            temp.push_back(mapped_result[i]);
            temp.push_back(application_map[app_id].tasks_communication[task_id][i]);
            temp.push_back(ID);
            flow.push_back(temp);
        }
    }

    // get_ICE
    // do not count in the flow of the mapping task_id
    int ICE = 0;
    for (int i = 0; i < flow.size(); ++i)
    {
        int src_x = flow[i][0] % 8;
        int src_y = flow[i][0] / 8;
        int des_x = flow[i][1] % 8;
        int des_y = flow[i][1] / 8;
        int comm = flow[i][2];
        int ID = flow[i][3];

        if (src_x < des_x)
        {
            for (int k = src_x; k < des_x; k++)
            {
                int PE_ID = src_y * 8 + k;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].e.begin();
                for (; it != petable[PE_ID].e.end(); it++)
                {
                    if (it->first / 10000 == app_id)
                        ICE += (it->second)[2];
                }
            }
        }
        else if (src_x > des_x)
        {
            for (int k = src_x; k > des_x; k--)
            {
                int PE_ID = src_y * 8 + k;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].w.begin();
                for (; it != petable[PE_ID].w.end(); it++)
                {
                    if (it->first / 10000 == app_id)
                        ICE += (it->second)[2];
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
                int PE_ID = k * 8 + des_x;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].s.begin();
                for (; it != petable[PE_ID].s.end(); it++)
                {
                    if (it->first / 10000 == app_id)
                        ICE += (it->second)[2];
                }
            }
        }
        else if (src_y > des_y)
        {
            for (int k = src_y; k > des_y; k--)
            {
                int PE_ID = k * 8 + des_x;
                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].n.begin();
                for (; it != petable[PE_ID].n.end(); it++)
                {
                    if (it->first / 10000 == app_id)
                        ICE += (it->second)[2];
                }
            }
        }
        else
        {
        }
    }

    // debug
    if (0)
    {
        cout << "------------------mapping info " << endl;
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                int peid = j + i * 8;
                if (petable[peid].busy)
                {
                    if (petable[peid].app_id == app.app_id)
                    {
                        cout << petable[peid].task_id << "\t"; // = firstTask;
                    }
                    else
                    {
                        cout << "*"
                             << "\t";
                    }
                }
                else
                {
                    if (peid == node)
                    {
                        cout << "#"
                             << "\t";
                    }
                    else
                        cout << "-"
                             << "\t";
                }
            }
            cout << endl;
        }
        cout << "task_id  " << task_id << " node " << node << " ----- ICE " << ICE << endl;
    }
    return ICE;
}

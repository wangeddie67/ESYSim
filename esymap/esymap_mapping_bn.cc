#include "esymap_task.h"

bool SimTask::task_mapping_bn(application &app)
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
    //  cout << "mapping result for App " << app.app_id << " :" << endl;
    // mapping
    petable[firstNode].busy = 1;
    petable[firstNode].app_id = app.app_id;
    petable[firstNode].task_id = firstTask;
    running_app_state[app.app_id][firstTask] = firstNode;
    update_link_info_new_node_XY(true, 1, app.app_id, firstTask);
    // node_statistic[0] += 1;
    //  cout << "task " << firstTask << "\tof application\t" << app.app_id << " -> "
    //      << "PE\t" << firstNode << endl;
    // updateNeighborsFreePE();
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
                if (hop == minHop)
                {
                    nextPeList.push_back(i);
                }
            }
        }

        int interference_factor_min = 10000000;
        int interference_factor = 0;
        int index = 0;

        int nextNode;
        /// std::vector<int> inter_factor;

        for (int i = 0; i < nextPeList.size(); i++)
        {
            interference_factor =
                get_interference_factor_bn(app.app_id, current_taskid, nextPeList[i], 0);
            /// inter_factor.push_back(interference_factor);
            if (interference_factor < interference_factor_min)
            {
                interference_factor_min = interference_factor;
                index = i;
            }
        }
        nextNode = nextPeList[index];

        // node_statistic[0] += 1;
        petable[nextNode].busy = 1;
        petable[nextNode].app_id = app.app_id;
        petable[nextNode].task_id = current_taskid;
        running_app_state[app.app_id][current_taskid] = nextNode;
        update_link_info_new_node_XY(true, 1, app.app_id, current_taskid);
        //   cout << "task " << current_taskid << "\tof application\t" << app.app_id << " -> "
        //        << "PE\t" << nextNode << endl;
        // updateNeighborsFreePE();
        // nextPeList_more.clear();
        // nextPeList.clear();
        //   cout << "search_pes\t" << nextPeList.size() << endl;
    }

    return 1;
}

double SimTask::get_interference_factor_bn(int app_id, int task_id, int node, bool flag)
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
    int factor = get_interference_factor_xy_bn(flow, flag);
    return factor;
}

double SimTask::get_interference_factor_xy_bn(std::vector<std::vector<int> > flow, bool flag)
{
    int flow_rate_sum = 0;

    int weight_max = 0;
    int factor = 0;
    for (int i = 0; i < flow.size(); i++)
    {
        int src_x = flow[i][0] % 8;
        int src_y = flow[i][0] / 8;
        int des_x = flow[i][1] % 8;
        int des_y = flow[i][1] / 8;
        int comm = flow[i][2];
        int ID = 0; // flow[i][3];

        std::vector<int> inter_flow;

        if (src_x < des_x)
        {
            for (int k = src_x; k < des_x; k++)
            {
                // cout<<"1 "<<k<<endl;
                int PE_ID = src_y * 8 + k;
                int flow_rate_temp = 0;
                int comm_temp = 0;

                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].e.begin();
                for (; it != petable[PE_ID].e.end(); it++)
                {
                    int flow_id = (it->second)[0];
                    int rate1 = (it->second)[1];
                    int comm1 = (it->second)[2];

                    // flow_id = flow_id / 100;
                    flow_rate_temp += rate1;
                    comm_temp += comm1;
                }

                flow_rate_sum += flow_rate_temp;
            }
        }
        else if (src_x > des_x)
        {
            for (int k = src_x; k > des_x; k--)
            {
                // cout<<"src_x "<<src_x<<"  des_x "<<des_x<<endl;
                // cout<<"2 "<<k<<endl;
                int PE_ID = src_y * 8 + k;
                int flow_rate_temp = 0;
                int comm_temp = 0;

                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].w.begin();
                for (; it != petable[PE_ID].w.end(); it++)
                {
                    int flow_id = (it->second)[0];
                    int rate1 = (it->second)[1];
                    int comm1 = (it->second)[2];

                    // flow_id = flow_id / 100;
                    flow_rate_temp += rate1;
                    comm_temp += comm1;
                }

                flow_rate_sum += flow_rate_temp;
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
                int flow_rate_temp = 0;
                int comm_temp = 0;

                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].s.begin();
                for (; it != petable[PE_ID].s.end(); it++)
                {
                    int flow_id = (it->second)[0];
                    int rate1 = (it->second)[1];
                    int comm1 = (it->second)[2];

                    // flow_id = flow_id / 100;
                    flow_rate_temp += rate1;
                    comm_temp += comm1;
                }

                flow_rate_sum += flow_rate_temp;
            }
        }
        else if (src_y > des_y)
        {
            for (int k = src_y; k > des_y; k--)
            {
                // cout<<"4 "<<k<<endl;
                int PE_ID = k * 8 + des_x;
                int flow_rate_temp = 0;
                int comm_temp = 0;

                std::map<int, std::vector<double> >::iterator it = petable[PE_ID].n.begin();
                for (; it != petable[PE_ID].n.end(); it++)
                {
                    int flow_id = (it->second)[0];
                    int rate1 = (it->second)[1];
                    int comm1 = (it->second)[2];

                    // flow_id = flow_id / 100;
                    flow_rate_temp += rate1;
                    comm_temp += comm1;
                }

                flow_rate_sum += flow_rate_temp;
            }
        }
        else
        {
        }

        if (weight_max == 0)
        {
            weight_max += comm;
        }

        // if (weight_max > factor)
        // {
        //     factor = weight_max + comm;
        // }

        // if (flag)
        //{
        //   flow_factor_log[ID].push_back(factor);
        //}
    }

    return flow_rate_sum;
}

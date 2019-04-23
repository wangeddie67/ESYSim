#include "esymap_task.h"

// ff method
// input:  application
// output: running_app_state[app.app_id]
// note: do not change pecount, pecount will be changed in doShedule
bool SimTask::task_mapping_ff(application &app)
{
    petable[0].busy = 1;
    int i = 0;
    int task_num = app.tasks.size();
    running_app_state[app.app_id].resize(task_num);
    if (pecount < task_num)
    {
        return 0;
    }
    else
    {
        cout << "mapping result for App " << app.app_id << " :" << endl;
        std::map<unsigned int, PeState>::iterator it = petable.begin();
        while (it != petable.end())
        {
            if ((it->second).busy == 1)
            {
                it++;
                continue;
            }
            if (i < task_num)
            {
                running_app_state[app.app_id][i] = it->first;
                update_link_info_new_node_XY(true, 1, app.app_id, i);
                petable[it->first].busy = 1;
                cout << "task " << i << "\tof application\t" << app.app_id << " -> "
                     << "PE\t" << it->first << endl;
                i++;
                it++;
            }
            else
            {
                break;
            }
        }
    }

    return 1;
}

bool SimTask::task_mapping_nn(application &app)
{
    updateNeighborsFreePE();
    int task_num = app.tasks.size();
    running_app_state[app.app_id].resize(task_num);
    petable[0].busy = 1;

    if (pecount < app.tasks.size())
    {
        return 0;
    }

    vector<vector<int> > task_order = app.task_order;
    int firstTask = task_order[0][0];
    int firstNode = first_node(task_num); // getFirstNode_nn();
    // cout << "mapping result for App " << app.app_id << " :" << endl;
    // mapping
    petable[firstNode].busy = 1;
    petable[firstNode].app_id = app.app_id;
    petable[firstNode].task_id = firstTask;
    running_app_state[app.app_id][firstTask] = firstNode;
    update_link_info_new_node_XY(true, 1, app.app_id, firstTask);
    cout << "task " << firstTask << "\tof application\t" << app.app_id << " -> "
         << "PE\t" << firstNode << endl;
    //  updateNeighborsFreePE();
    int size_x = 8;
    int size_y = 8;
    for (int i = 1; i < app.tasks.size(); ++i)
    {
        int hop = 0;
        int minHop = 1000;
        int current_taskid = task_order[i][0];
        int current_predecessor = task_order[i][1];
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
        int nextNode = nextPeList[0];

        petable[nextNode].busy = 1;
        petable[nextNode].app_id = app.app_id;
        petable[nextNode].task_id = current_taskid;
        running_app_state[app.app_id][current_taskid] = nextNode;
        cout << "task " << current_taskid << "\tof application\t" << app.app_id << " -> "
             << "PE\t" << nextNode << endl;
        //  updateNeighborsFreePE();
        //   nextPeList.clear();

        update_link_info_new_node_XY(true, 1, app.app_id, current_taskid);
    }

    // print_interference_factor(app.app_id, 1);
    return 1;
}

bool SimTask::task_mapping_random(application &app)
{
    petable[0].busy = 1;
    int i = 0;
    int task_num = app.tasks.size();
    running_app_state[app.app_id].resize(task_num);

    if (pecount < task_num)
    {
        return 0;
    }
    else
    {
        cout << "mapping result for App " << app.app_id << " :" << endl;

        while (1)
        {
            if (i < task_num)
            {
                srand(time(NULL) + i);
                int k = rand() % (m_pe_num);
                while (petable[k].busy)
                {
                    k = (k + 1) % (m_pe_num);
                    // cout << k << "---petable[k].busy--" << petable[k].busy << endl;
                }
                running_app_state[app.app_id][i] = k;
                update_link_info_new_node_XY(true, 1, app.app_id, i);
                petable[k].busy = 1;
                cout << "task " << i << "\tof application\t" << app.app_id << " -> "
                     << "PE\t" << k << endl;
                i++;
            }
            else
            {
                break;
            }
        }
    }

    return 1;
}

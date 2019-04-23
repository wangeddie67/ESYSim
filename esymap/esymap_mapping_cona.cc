#include "esymap_task.h"

bool SimTask::task_mapping_wena(application &app)
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

    if (0)
    {
        for (int i = 0; i < task_order.size(); i++)
        {
            cout << endl;
            for (int j = 0; j < task_order[i].size(); j++)
            {
                cout << task_order[i][j] << "  ";
            }
        }
    }

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
    std::vector<bool> mapped;
    mapped.resize(task_num, false);
    mapped[firstTask] = true;
    for (int i = 1; i < task_num; i++)
    {
        int current_taskid = task_order[i][0];
        int current_task_father_id = task_order[i][1];
        int current_task_ep = task_order[i][2];
        int current_task_father_PE = running_app_state[app.app_id][current_task_father_id];
        std::vector<int> OPT;
        if (!petable[current_task_father_PE].neighborsFreePE.empty())
        {
            OPT = petable[current_task_father_PE].neighborsFreePE;
        }
        else
        {
            int hop = 0;
            int minHop = 1000;
            int square = 0;
            int minS = 1000;
            // minimun hops
            for (int i = 1; i < m_pe_num; ++i)
            {
                if (!petable[i].busy)
                {
                    hop = abs(current_task_father_PE / size_x - i / size_x) +
                          abs(current_task_father_PE % size_x - i % size_x);
                    if (hop < minHop)
                    {
                        minHop = hop;
                        OPT.clear();
                        OPT.push_back(i);
                        minS = abs(current_task_father_PE / size_x - i / size_x) *
                                   abs(current_task_father_PE / size_x - i / size_x) +
                               abs(current_task_father_PE % size_x - i % size_x) *
                                   abs(current_task_father_PE % size_x - i % size_x);
                    }
                    else if (hop == minHop)
                    {
                        square = abs(current_task_father_PE / size_x - i / size_x) *
                                     abs(current_task_father_PE / size_x - i / size_x) +
                                 abs(current_task_father_PE % size_x - i % size_x) *
                                     abs(current_task_father_PE % size_x - i % size_x);
                        if (square < minS)
                        {
                            minS = square;
                            OPT.clear();
                            OPT.push_back(i);
                            minS = square;
                        }
                        if (square == minS)
                        {
                            OPT.push_back(i);
                        }
                    }
                }
            }
        }

        // get MT(tc)
        std::vector<int> MT_task;
        std::vector<int> MT_PE;
        for (int i = 0; i < task_num; i++)
        {
            if ((app.tasks_communication[i][current_taskid] != 0 ||
                 app.tasks_communication[current_taskid][i] != 0) &&
                mapped[i])
            {
                MT_task.push_back(i);
                MT_PE.push_back(running_app_state[app.app_id][i]);
            }
        }
        int current_task_PE;
        if (MT_task.size() > 1)
        {
            int sum_min = 10000;
            int sum = 0;
            int index = 0;
            int minS = 0;
            for (int i = 0; i < OPT.size(); i++)
            {
                sum = 0;
                for (int j = 0; j < MT_PE.size(); j++)
                {
                    sum += (abs(MT_PE[j] / size_x - OPT[i] / size_x) +
                            abs(MT_PE[j] % size_x - OPT[i] % size_x)) *
                           (app.tasks_communication[MT_task[j]][current_taskid] +
                            app.tasks_communication[current_taskid][MT_task[j]]);
                }
                if (sum < sum_min)
                {
                    sum_min = sum;
                    index = OPT[i];
                    minS = abs(firstNode / size_x - OPT[i] / size_x) *
                               abs(firstNode / size_x - OPT[i] / size_x) +
                           abs(firstNode % size_x - OPT[i] % size_x) *
                               abs(firstNode % size_x - OPT[i] % size_x);
                }
                else if (sum == sum_min)
                {
                    int minS_temp = abs(firstNode / size_x - OPT[i] / size_x) *
                                        abs(firstNode / size_x - OPT[i] / size_x) +
                                    abs(firstNode % size_x - OPT[i] % size_x) *
                                        abs(firstNode % size_x - OPT[i] % size_x);
                    if (minS_temp < minS)
                    {
                        sum_min = sum;
                        index = OPT[i];
                        minS = minS_temp;
                    }
                }
            }
            current_task_PE = index;
        }
        else
        {
            int min = 1000;
            int vaule = 0;
            int index = 0;
            for (int i = 0; i < OPT.size(); i++)
            {
                vaule = abs(current_task_ep - petable[OPT[i]].neighborsFreeNum);
                if (vaule < min)
                {
                    min = vaule;
                    index = OPT[i];
                }
            }
            current_task_PE = index;
        }
        mapped[current_taskid] = true;
        petable[current_task_PE].busy = 1;
        petable[current_task_PE].app_id = app.app_id;
        petable[current_task_PE].task_id = current_taskid;
        running_app_state[app.app_id][current_taskid] = current_task_PE;
        update_link_info_new_node_XY(true, 1, app.app_id, current_taskid);
        cout << "task " << current_taskid << "\tof application\t" << app.app_id << " -> "
             << "PE\t" << current_task_PE << endl;
        updateNeighborsFreePE();
    }

    return 1;
}

bool SimTask::task_mapping_cona(application &app)
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
    // cout << "mapping result for App " << app.app_id << " :" << endl;
    // mapping
    petable[firstNode].busy = 1;
    petable[firstNode].app_id = app.app_id;
    petable[firstNode].task_id = firstTask;
    running_app_state[app.app_id][firstTask] = firstNode;
    cout << "task " << firstTask << "\tof application\t" << app.app_id << " -> "
         << "PE\t" << firstNode << endl;
    // updateNeighborsFreePE();
    int size_x = 8;
    int size_y = 8;
    for (int i = 1; i < app.tasks.size(); ++i)
    {
        int current_taskid = task_order[i][0];
        int current_predecessor = task_order[i][1];
        // cout<<"current_predecessor  "<<current_predecessor<<endl;
        int current_predecessor_PE = running_app_state[app.app_id][current_predecessor];
        std::vector<int> nextPeList;

        int hop = 0;
        int minHop = 1000;
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

        int squareMin = 1000;
        int square_temp = 0;
        int index = 0;
        for (int i = 0; i < nextPeList.size(); ++i)
        {
            square_temp = max(abs((nextPeList[i] / size_x - firstNode / size_x)),
                              abs((nextPeList[i] % size_x - firstNode % size_x)));
            if (square_temp < squareMin)
            {
                squareMin = square_temp;
                index = i;
            }
        }
        int nextNode = nextPeList[index];

        petable[nextNode].busy = 1;
        petable[nextNode].app_id = app.app_id;
        petable[nextNode].task_id = current_taskid;
        running_app_state[app.app_id][current_taskid] = nextNode;
        cout << "task " << current_taskid << "\tof application\t" << app.app_id << " -> "
             << "PE\t" << nextNode << endl;
        updateNeighborsFreePE();
        //  cout << "search_pes\t" << nextPeList.size() << endl;
    }

    cal_MRD(app.app_id);

    // print_interference_factor(app.app_id, 1);
    return 1;
}

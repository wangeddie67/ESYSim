#include "esymap_task.h"


void application::findFirstTask()
{
    if (tasks.size() == 1)
    {
        firstTask = 0;
        return;
    }
    int taskID = 1000;
    int Degree = 0;
    int Comm = 0;
    int maxDegree = 0;
    int maxComm = 0;
    for (int i = 0; i < tasks.size(); i++)
    {
        Degree = 0;
        Comm = 0;
        for (int j = 0; j < tasks.size(); j++)
        {
            if (tasks_communication[i][j] > 0)
            {
                Degree++;
                Comm = Comm + tasks_communication[i][j];
            }
            if (tasks_communication[j][i] > 0)
            {
                Degree++;
                Comm = Comm + tasks_communication[j][i];
            }
        }
        if (Degree > maxDegree)
        {
            maxDegree = Degree;
            maxComm = Comm;
            taskID = i;
        }
        else if (Degree == maxDegree)
        {
            if (Comm > maxComm)
            {
                maxComm = Comm;
                taskID = i;
            }
        }
    }
    firstTask = taskID;
}

void application::getTaskOrder_ff()
{
    unsigned int taskNum = tasks.size();
    vector<vector<int> > taskOrder_;
    std::vector<int> taskOrder_item;
    taskOrder_item.resize(2, 0);
    taskOrder_item[0] = 0;
    taskOrder_item[1] = 0;
    taskOrder_.push_back(taskOrder_item);
    if (taskNum == 1)
    {
        task_order = taskOrder_;
        return;
    }
    for (int i = 1; i < tasks.size(); i++)
    {
        taskOrder_item[0] = i;
        taskOrder_item[1] = i - 1;
        taskOrder_.push_back(taskOrder_item);
    }
    task_order = taskOrder_;
}

void application::getTaskOrder_cona()
{
    unsigned int taskNum = tasks.size();
    unsigned int currentTask = firstTask;

    vector<vector<int> > taskOrder_;
    std::vector<int> taskOrder_item;
    taskOrder_item.resize(2, 0);
    taskOrder_item[0] = firstTask;
    taskOrder_item[1] = -1;
    taskOrder_.push_back(taskOrder_item);

    if (taskNum == 1)
    {
        task_order = taskOrder_;
        return;
    }

    int offset = 0;
    int last_loop_size = 1;
    std::map<int, vector<int> > taskOrder_temp; // task_id ,task_id_father
    std::vector<bool> enter_TQ;
    enter_TQ.resize(taskNum, false);
    enter_TQ[firstTask] = true;
    while (1)
    {
        taskOrder_temp.clear();
        for (int k = 0; k < last_loop_size; k++)
        {
            int current_father = taskOrder_[offset][0];
            for (int i = 0; i < taskNum; ++i)
            {
                if (tasks_communication[i][current_father] != 0)
                {
                    if (enter_TQ[i])
                    {
                    }
                    else
                    {
                        taskOrder_temp[i].push_back(i);
                        taskOrder_temp[i].push_back(current_father);
                    }
                }
                if (tasks_communication[current_father][i] != 0)
                {
                    if (enter_TQ[i])
                    {
                    }
                    else
                    {
                        taskOrder_temp[i].push_back(i);
                        taskOrder_temp[i].push_back(current_father);
                    }
                }
            }
            offset = offset + 1;
        }

        last_loop_size = taskOrder_temp.size();

        std::map<int, vector<int> >::iterator it = taskOrder_temp.begin();
        for (; it != taskOrder_temp.end(); it++)
        {
            taskOrder_item[0] = (it->second)[0];
            taskOrder_item[1] = (it->second)[1];
            taskOrder_.push_back(taskOrder_item);
            enter_TQ[taskOrder_item[0]] = true;
        }

        if (taskOrder_.size() >= tasks.size())
        {
            break;
        }
    }
    task_order = taskOrder_;

    // debug
    if (0)
    {
        cout << "task_order_cona:" << endl;
        cout << "size : " << task_order.size() << endl;
        for (int i = 0; i < task_order.size(); i++)
        {
            for (int j = 0; j < task_order[i].size(); j++)
            {
                cout << task_order[i][j] << "\t";
            }
            cout << endl;
        }
        cout << "Enter to continue the program: ";
        cin.ignore().get();
    }
}

void application::getTaskOrder_wena()
{
    unsigned int taskNum = tasks.size();
    unsigned int currentTask = firstTask;

    vector<vector<int> > taskOrder_;
    std::vector<int> taskOrder_item;
    taskOrder_item.resize(3, 0);
    taskOrder_item[0] = firstTask;
    taskOrder_item[1] = -1;
    taskOrder_.push_back(taskOrder_item);

    if (taskNum == 1)
    {
        task_order = taskOrder_;
        return;
    }

    int offset = 0;
    int last_loop_size = 1;
    std::map<int, vector<int> > taskOrder_temp; // task_id ,task_id_father, volume
    std::vector<bool> enter_TQ;
    enter_TQ.resize(taskNum, false);
    enter_TQ[firstTask] = true;
    while (1)
    {
        // waiting queue
        std::vector<bool> enter_WQ;
        enter_WQ.resize(taskNum, false);
        taskOrder_temp.clear();
        for (int k = 0; k < last_loop_size; k++)
        {
            int current_father = taskOrder_[offset][0];
            for (int i = 0; i < taskNum; ++i)
            {
                if (tasks_communication[i][current_father] != 0)
                {
                    if (enter_TQ[i])
                    {
                        continue;
                    }
                    if (enter_WQ[i])
                    {
                        if (tasks_communication[i][current_father] > taskOrder_temp[i][2])
                        {
                            taskOrder_temp[i][0] = i;
                            taskOrder_temp[i][1] = current_father;
                            taskOrder_temp[i][2] = tasks_communication[i][current_father];
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        taskOrder_temp[i].push_back(i);
                        taskOrder_temp[i].push_back(current_father);
                        taskOrder_temp[i].push_back(tasks_communication[i][current_father]);
                        enter_WQ[i] = true;
                    }
                }

                if (tasks_communication[current_father][i] != 0)
                {
                    if (enter_TQ[i])
                    {
                        continue;
                    }
                    if (enter_WQ[i])
                    {
                        if (tasks_communication[current_father][i] > taskOrder_temp[i][2])
                        {
                            taskOrder_temp[i][0] = i;
                            taskOrder_temp[i][1] = current_father;
                            taskOrder_temp[i][2] = tasks_communication[current_father][i];
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        taskOrder_temp[i].push_back(i);
                        taskOrder_temp[i].push_back(current_father);
                        taskOrder_temp[i].push_back(tasks_communication[current_father][i]);
                        enter_WQ[i] = true;
                    }
                }
            }
            offset = offset + 1;
        }

        last_loop_size = taskOrder_temp.size();

        // rearrange
        int nn = taskOrder_temp.size();
        for (int i = 0; i < nn; i++)
        {
            int nn_in = taskOrder_temp.size();
            int max = ((taskOrder_temp.begin())->second)[2];
            int max_index = (taskOrder_temp.begin())->first;
            std::map<int, vector<int> >::iterator it = taskOrder_temp.begin();
            for (; it != taskOrder_temp.end(); it++)
            {
                int in_temp = (it->second)[2];
                int in_temp_index = (it->second)[0];
                if (in_temp > max)
                {
                    max = in_temp;
                    max_index = in_temp_index;
                }
            }
            taskOrder_item[0] = taskOrder_temp[max_index][0];
            taskOrder_item[1] = taskOrder_temp[max_index][1];
            taskOrder_item[2] = 0;
            taskOrder_.push_back(taskOrder_item);
            enter_TQ[max_index] = true;
            taskOrder_temp.erase(max_index);
        }

        if (taskOrder_.size() >= tasks.size())
        {
            break;
        }
    }

    // compute father number
    for (int i = 0; i < taskOrder_.size(); i++)
    {
        for (int j = 0; j < taskOrder_.size(); j++)
        {
            if (taskOrder_[j][1] == taskOrder_[i][0])
            {
                taskOrder_[i][2] = taskOrder_[i][2] + 1;
            }
        }
    }

    task_order = taskOrder_;

    // debug
    if (0)
    {
        cout << "task_order_wena:" << endl;
        cout << "size : " << task_order.size() << endl;
        for (int i = 0; i < task_order.size(); i++)
        {
            for (int j = 0; j < task_order[i].size(); j++)
            {
                cout << task_order[i][j] << "\t";
            }
            cout << endl;
        }
        cout << "Enter to continue the program: ";
        cin.ignore().get();
    }
}

void application::getTaskOrder_cam()
{
    vector<int> unmapped_tasks; //the unmapped tasks connected to target task;
    vector<int> mapped;
    connect_1.resize(tasks.size());
    connect_2.resize(tasks.size());
    connect_3.resize(tasks.size());
    mapped.resize(tasks.size(), 0);
    mapped[task_order[0][0]] = 1;

    for (int i = 1; i < task_order.size(); i++)
    {
        unmapped_tasks.clear();
        int target_task = task_order[i][0];
        mapped[target_task] = 1;

        for (int j = 0; j < tasks.size(); j++)
        {
            if (j == task_order[i][1])
                continue;

            if (j == target_task)
                continue;

            if (tasks_communication[j][target_task])
            {
                if (mapped[j])
                    connect_1[i].push_back(j);
                else
                    unmapped_tasks.push_back(j);
            }
            if (tasks_communication[target_task][j])
            {
                if (mapped[j])
                    connect_1[i].push_back(j);
                else
                    unmapped_tasks.push_back(j);
            }
        }

        for (int k = 0; k < unmapped_tasks.size(); k++)
        {
            int tt = unmapped_tasks[k];
            for (int m = 0; m < tasks.size(); m++)
            {
                if (mapped[m] == 0)
                    continue;

                if (m == task_order[i][1])
                    continue;

                if (m == target_task)
                    continue;

                if (tasks_communication[m][tt])
                {
                    connect_2[i].push_back(m);
                }
                if (tasks_communication[tt][m])
                {
                    connect_2[i].push_back(m);
                }
            }
        }
    }

    for (int i = 0; i < task_order.size(); i++)
    {
        if (connect_1.size() + connect_2.size() > 0)
            continue;

        for (int j = 0; j < i; j++)
        {
            int mapped_task = task_order[j][0];
            for (int k = i + 1; k < task_order.size(); k++)
            {
                bool f = false;
                if (find(connect_1[k].begin(), connect_1[k].end(), mapped_task) != connect_1[k].end())
                    f = true;
                if (find(connect_2[k].begin(), connect_2[k].end(), mapped_task) != connect_2[k].end())
                    f = true;
                if (f)
                    connect_3[i].push_back(mapped_task);
            }
        }
    }
}

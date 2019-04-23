#include "esymap_task.h"


void SimTask::update_link_info(bool add, unsigned int app_id, std::vector<unsigned int> map_result)
{
    // bool useDyXY = 0;
    application task_graph = application_map[app_id];
    vector<unsigned long> tasks = task_graph.tasks;
    vector<vector<unsigned long> > tasks_communication = task_graph.tasks_communication;
    int n = tasks.size();
    // cout << "size " << n << endl;
    for (int i = 0; i < n; ++i)
    {
        // cout<<"in for i "<<i<<endl;
        for (int j = 0; j < n; ++j)
        {
            // cout<<"in for for j "<<j<<endl;
            double comm = tasks_communication[i][j];
            if (comm != 0)
            {
                int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(i), static_cast<int>(j));
                // app_id * 100 + i;
                // cout<<"bu dengyu 0 = "<<comm<<endl;
                unsigned int src = map_result[i];
                unsigned int des = map_result[j];
                if (use_DyXY)
                    update_link_info_DyXY(false, 1, ID, src, des, comm);
                else
                    update_link_info_XY(false, 1, ID, src, des, comm);

                // update_link_info_XY(add, src, des, comm);
                // update_link_info_DyXY(add, src, des, comm);
            }
        }
    }
}

void SimTask::update_link_info_new_node_XY(bool add, double rate, int app_id, int task_id)
{
    std::vector<unsigned int> task_mapped = running_app_state[app_id];
    for (int i = 0; i < task_mapped.size(); i++)
    {
        if (task_mapped[i] == 0)
        {
            continue;
        }
        if (application_map[app_id].tasks_communication[i][task_id])
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(i), static_cast<int>(task_id));
            // app_id * 100 + i;
            if (use_DyXY)
            {
                update_link_info_DyXY(add, rate, ID, task_mapped[i], task_mapped[task_id],
                                      application_map[app_id].tasks_communication[i][task_id]);
                //cout<<"hhh flag "<<add<<"\t"<<ID<<"\t"<<i<<"\t"<<task_id<<"\t"<<" appid"<<app_id<<endl;
            }
            else
            {
                update_link_info_XY(add, rate, ID, task_mapped[i], task_mapped[task_id],
                                    application_map[app_id].tasks_communication[i][task_id]);
            }
        }
        if (application_map[app_id].tasks_communication[task_id][i])
        {
            int ID = get_flow_id(static_cast<int>(app_id), static_cast<int>(task_id), static_cast<int>(i));
            // app_id * 100 + task_id;
            if (use_DyXY)
            {
                update_link_info_DyXY(add, rate, ID, task_mapped[task_id], task_mapped[i],
                                      application_map[app_id].tasks_communication[task_id][i]);
                //cout<<"hhh flag "<<add<<"\t"<<ID<<"\t"<<i<<"\t"<<task_id<<"\t"<<" appid"<<app_id<<endl;
            }
            else
            {
                update_link_info_XY(add, rate, ID, task_mapped[task_id], task_mapped[i],
                                    application_map[app_id].tasks_communication[task_id][i]);
            }
        }
    }
}


void SimTask::update_link_info_XY(bool add, double rate, int ID, unsigned int src, unsigned int des, double comm)
{
    int src_x = src % 8;
    int src_y = src / 8;
    int des_x = des % 8;
    int des_y = des / 8;

    bool b = 0;

    if (src_x < des_x)
    {
        for (int k = src_x; k < des_x; k++)
        {
            // cout<<"1 "<<k<<endl;
            int PE_ID = src_y * 8 + k;
            if (add)
            {
                if (petable[PE_ID].e.find(ID) == petable[PE_ID].e.end())
                {
                    petable[PE_ID].e[ID].push_back(ID);
                    petable[PE_ID].e[ID].push_back(rate);
                    petable[PE_ID].e[ID].push_back(comm);
                }
                if (b)
                {
                    cout << "PE_ID (e): " << PE_ID << " ID: " << ID << endl;
                    cout << petable[PE_ID].e[ID][0] << "\t";
                    cout << petable[PE_ID].e[ID][1] << "\t";
                    cout << petable[PE_ID].e[ID][2] << "\t" << endl;
                }
            }
            else
            {
                petable[PE_ID].e.erase(ID);
                cout << PE_ID << " e erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
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
            if (add)
            {
                if (petable[PE_ID].w.find(ID) == petable[PE_ID].w.end())
                {
                    petable[PE_ID].w[ID].push_back(ID);
                    petable[PE_ID].w[ID].push_back(rate);
                    petable[PE_ID].w[ID].push_back(comm);
                }
                if (b)
                {
                    cout << "PE_ID (w): " << PE_ID << " ID: " << ID << endl;
                    cout << petable[PE_ID].w[ID][0] << "\t";
                    cout << petable[PE_ID].w[ID][1] << "\t";
                    cout << petable[PE_ID].w[ID][2] << "\t" << endl;
                }
            }
            else
            {
                petable[PE_ID].w.erase(ID);
                cout << PE_ID << " w erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
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
            if (add)
            {
                if (petable[PE_ID].s.find(ID) == petable[PE_ID].s.end())
                {
                    petable[PE_ID].s[ID].push_back(ID);
                    petable[PE_ID].s[ID].push_back(rate);
                    petable[PE_ID].s[ID].push_back(comm);
                }
                if (b)
                {
                    cout << "PE_ID (s): " << PE_ID << " ID: " << ID << endl;
                    cout << petable[PE_ID].s[ID][0] << "\t";
                    cout << petable[PE_ID].s[ID][1] << "\t";
                    cout << petable[PE_ID].s[ID][2] << "\t" << endl;
                }
            }
            else
            {
                petable[PE_ID].s.erase(ID);
                cout << PE_ID << " s erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
            }
        }
    }
    else if (src_y > des_y)
    {
        for (int k = src_y; k > des_y; k--)
        {
            // cout<<"4 "<<k<<endl;
            int PE_ID = k * 8 + des_x;
            if (add)
            {
                if (petable[PE_ID].n.find(ID) == petable[PE_ID].n.end())
                {
                    petable[PE_ID].n[ID].push_back(ID);
                    petable[PE_ID].n[ID].push_back(rate);
                    petable[PE_ID].n[ID].push_back(comm);
                }
                if (b)
                {
                    cout << "PE_ID (n): " << PE_ID << " ID: " << ID << endl;
                    cout << petable[PE_ID].n[ID][0] << "\t";
                    cout << petable[PE_ID].n[ID][1] << "\t";
                    cout << petable[PE_ID].n[ID][2] << "\t" << endl;
                }
            }
            else
            {
                petable[PE_ID].n.erase(ID);
                cout << PE_ID << " n erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
            }
        }
    }
    else
    {
    }
}

//recursive
void SimTask::update_link_info_DyXY(bool add, double rate, int ID, unsigned int src, unsigned int des, double comm)
{
    //  if(!add)
    //      cout<<"~";
    //  cout<<"src: "<<src<<", des: "<<des<<" - "<<rate<<endl;

    int src_x = src % 8;
    int src_y = src / 8;
    int des_x = des % 8;
    int des_y = des / 8;

    if (src == des)
    {
        return;
    }

    bool pp = 1;

    if (src_x < des_x)
    {
        if (src_y < des_y)
        {
            unsigned int id1 = src_x + 1 + src_y * 8;
            if (add)
            {
                if (petable[src].e.find(ID) == petable[src].e.end())
                {
                    petable[src].e[ID].push_back(ID);
                    petable[src].e[ID].push_back(rate / 2.0);
                    petable[src].e[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].e[ID][1] += rate / 2.0;
                    petable[src].e[ID][2] += comm / 2.0;
                    if (petable[src].e[ID][1] > 1)
                        cout << "rate " << petable[src].e[ID][1] << "        " << rate / 2.0 << endl;
                }
            }
            else
            {
                if (petable[src].e.find(ID) != petable[src].e.end())
                {
                    petable[src].e.erase(ID);
                    if (pp)
                        cout << src << " e erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id1, des, comm / 2.0);

            unsigned int id2 = src_x + (src_y + 1) * 8;
            if (add)
            {
                if (petable[src].s.find(ID) == petable[src].s.end())
                {
                    petable[src].s[ID].push_back(ID);
                    petable[src].s[ID].push_back(rate / 2.0);
                    petable[src].s[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].s[ID][1] += rate / 2.0;
                    petable[src].s[ID][2] += comm / 2.0;
                }
            }
            else
            {
                if (petable[src].s.find(ID) != petable[src].s.end())
                {
                    petable[src].s.erase(ID);
                    if (pp)
                        cout << src << " s erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id2, des, comm / 2.0);
        }
        else if (src_y > des_y)
        {
            unsigned int id1 = src_x + 1 + src_y * 8;
            if (add)
            {
                if (petable[src].e.find(ID) == petable[src].e.end())
                {
                    petable[src].e[ID].push_back(ID);
                    petable[src].e[ID].push_back(rate / 2.0);
                    petable[src].e[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].e[ID][1] += rate / 2.0;
                    petable[src].e[ID][2] += comm / 2.0;
                }
            }
            else
            {
                if (petable[src].e.find(ID) != petable[src].e.end())
                {
                    petable[src].e.erase(ID);
                    if (pp)
                        cout << src << " e erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id1, des, comm / 2.0);

            unsigned int id2 = src_x + (src_y - 1) * 8;
            if (add)
            {
                if (petable[src].n.find(ID) == petable[src].n.end())
                {
                    petable[src].n[ID].push_back(ID);
                    petable[src].n[ID].push_back(rate / 2.0);
                    petable[src].n[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].n[ID][1] += rate / 2.0;
                    petable[src].n[ID][2] += comm / 2.0;
                }
            }
            else
            {
                if (petable[src].n.find(ID) != petable[src].n.end())
                {
                    petable[src].n.erase(ID);
                    if (pp)
                        cout << src << " n erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id2, des, comm / 2.0);
        }
        else
        {
            unsigned int id1 = src_x + 1 + src_y * 8;
            if (add)
            {
                if (petable[src].e.find(ID) == petable[src].e.end())
                {
                    petable[src].e[ID].push_back(ID);
                    petable[src].e[ID].push_back(rate);
                    petable[src].e[ID].push_back(comm);
                }
                else
                {
                    petable[src].e[ID][1] += rate;
                    petable[src].e[ID][2] += comm;
                }
            }
            else
            {
                if (petable[src].e.find(ID) != petable[src].e.end())
                {
                    petable[src].e.erase(ID);
                    if (pp)
                        cout << src << " e erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate, ID, id1, des, comm);
        }
    }
    else if (src_x > des_x)
    {
        if (src_y < des_y)
        {
            unsigned int id1 = src_x - 1 + src_y * 8;
            if (add)
            {
                if (petable[src].w.find(ID) == petable[src].w.end())
                {
                    petable[src].w[ID].push_back(ID);
                    petable[src].w[ID].push_back(rate / 2.0);
                    petable[src].w[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].w[ID][1] += rate / 2.0;
                    petable[src].w[ID][2] += comm / 2.0;
                }
            }
            else
            {
                if (petable[src].w.find(ID) != petable[src].w.end())
                {
                    petable[src].w.erase(ID);
                    if (pp)
                        cout << src << " w erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id1, des, comm / 2.0);

            unsigned int id2 = src_x + (src_y + 1) * 8;
            if (add)
            {
                if (petable[src].s.find(ID) == petable[src].s.end())
                {
                    petable[src].s[ID].push_back(ID);
                    petable[src].s[ID].push_back(rate / 2.0);
                    petable[src].s[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].s[ID][1] += rate / 2.0;
                    petable[src].s[ID][2] += comm / 2.0;
                }
            }
            else
            {
                if (petable[src].s.find(ID) != petable[src].s.end())
                {
                    petable[src].s.erase(ID);
                    if (pp)
                        cout << src << " s erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id2, des, comm / 2.0);
        }
        else if (src_y > des_y)
        {
            unsigned int id1 = src_x - 1 + src_y * 8;
            if (add)
            {
                if (petable[src].w.find(ID) == petable[src].w.end())
                {
                    petable[src].w[ID].push_back(ID);
                    petable[src].w[ID].push_back(rate / 2.0);
                    petable[src].w[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].w[ID][1] += rate / 2.0;
                    petable[src].w[ID][2] += comm / 2.0;
                }
            }
            else
            {
                if (petable[src].w.find(ID) != petable[src].w.end())
                {
                    petable[src].w.erase(ID);
                    if (pp)
                        cout << src << " w erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id1, des, comm / 2.0);

            unsigned int id2 = src_x + (src_y - 1) * 8;
            if (add)
            {
                if (petable[src].n.find(ID) == petable[src].n.end())
                {
                    petable[src].n[ID].push_back(ID);
                    petable[src].n[ID].push_back(rate / 2.0);
                    petable[src].n[ID].push_back(comm / 2.0);
                }
                else
                {
                    petable[src].n[ID][1] += rate / 2.0;
                    petable[src].n[ID][2] += comm / 2.0;
                }
            }
            else
            {
                if (petable[src].n.find(ID) != petable[src].n.end())
                {
                    petable[src].n.erase(ID);
                    if (pp)
                        cout << src << " n erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate / 2.0, ID, id2, des, comm / 2.0);
        }
        else
        {
            unsigned int id1 = src_x - 1 + src_y * 8;
            if (add)
            {
                if (petable[src].w.find(ID) == petable[src].w.end())
                {
                    petable[src].w[ID].push_back(ID);
                    petable[src].w[ID].push_back(rate);
                    petable[src].w[ID].push_back(comm);
                }
                else
                {
                    petable[src].w[ID][1] += rate;
                    petable[src].w[ID][2] += comm;
                }
            }
            else
            {
                if (petable[src].w.find(ID) != petable[src].w.end())
                {
                    petable[src].w.erase(ID);
                    if (pp)
                        cout << src << " w erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate, ID, id1, des, comm);
        }
    }
    else
    {
        if (src_y < des_y)
        {
            unsigned int id1 = src_x + (src_y + 1) * 8;
            if (add)
            {
                if (petable[src].s.find(ID) == petable[src].s.end())
                {
                    petable[src].s[ID].push_back(ID);
                    petable[src].s[ID].push_back(rate);
                    petable[src].s[ID].push_back(comm);
                }
                else
                {
                    petable[src].s[ID][1] += rate;
                    petable[src].s[ID][2] += comm;
                }
            }
            else
            {
                if (petable[src].s.find(ID) != petable[src].s.end())
                {
                    petable[src].s.erase(ID);
                    if (pp)
                        cout << src << " s erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate, ID, id1, des, comm);
        }
        else if (src_y > des_y)
        {
            unsigned int id1 = src_x + (src_y - 1) * 8;
            if (add)
            {
                if (petable[src].n.find(ID) == petable[src].n.end())
                {
                    petable[src].n[ID].push_back(ID);
                    petable[src].n[ID].push_back(rate);
                    petable[src].n[ID].push_back(comm);
                }
                else
                {
                    petable[src].n[ID][1] += rate;
                    petable[src].n[ID][2] += comm;
                }
            }
            else
            {
                if (petable[src].n.find(ID) != petable[src].n.end())
                {
                    petable[src].n.erase(ID);
                    if (pp)
                        cout << src << " n erase " << ID << "  ( appid " << ID / 10000 << " )" << endl;
                }
            }
            update_link_info_DyXY(add, rate, ID, id1, des, comm);
        }
    }
}

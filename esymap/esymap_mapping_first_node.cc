#include "esymap_task.h"

int SimTask::first_node(int app_size)
{
    updateNeighborsFreePE();
    int first_node_int;
    switch (0)
    {
    case 0:
        first_node_int = getFirstNode_cona();
        break;
    case 1:
        first_node_int = SHiC(app_size);
        break;
    default:
        break;
    }

    if (mapping_algs == "cam")
    { //first_node_int = getFirstNode_cam();
    }

    return first_node_int;
}

int SimTask::getFirstNode_nn()
{
    petable[0].busy = 1;
    for (int i = 1; i < m_pe_num; i++)
    {
        if (!petable[i].busy)
        {
            return i;
        }
    }
}

// for CoNA algorithm
unsigned int SimTask::getFirstNode_cona()
{
    petable[0].busy = 1;
    // update PE nerghbors number
    std::vector<unsigned int> idlePE;
    std::vector<unsigned int> maxNerghborPE;
    int maxC = 0;
    int idleCount = 0;
    for (int i = 1; i < m_pe_num; i++)
    {
        if (petable[i].busy)
        {
            continue;
        }
        else
        {
            idleCount = petable[i].neighborsFreeNum;

            idlePE.push_back(i);
            if (idleCount > maxC)
            {
                maxC = idleCount;
                maxNerghborPE.clear();
                maxNerghborPE.push_back(i);
            }
            else if (idleCount == maxC)
            {
                maxNerghborPE.push_back(i);
            }
        }
        idleCount = 0;
    }

    unsigned int firstNode = 0;
    double minDistance = 1000;
    double distance = 0;
    for (int i = 0; i < maxNerghborPE.size(); ++i)
    {
        unsigned int pe_id = maxNerghborPE[i];
        distance = sqrt((double)(pe_id / 8 + 1) * (pe_id / 8 + 1) + (pe_id % 8 + 1) * (pe_id % 8 + 1));
        if (distance < minDistance)
        {
            minDistance = distance;
            firstNode = pe_id;
        }
    }
    return firstNode;
}

int SimTask::getFirstNode_cam()
{
    petable[0].busy = 1;
    // update PE nerghbors number
    std::vector<unsigned int> idlePE;
    std::vector<unsigned int> maxNerghborPE;
    int maxC = 0;
    int idleCount = 0;
    for (int i = 1; i < m_pe_num; i++)
    {
        if (petable[i].busy)
        {
            continue;
        }
        else
        {
            idleCount = petable[i].neighborsFreeNum;

            idlePE.push_back(i);
            if (idleCount > maxC)
            {
                maxC = idleCount;
                maxNerghborPE.clear();
                maxNerghborPE.push_back(i);
            }
            else if (idleCount == maxC)
            {
                maxNerghborPE.push_back(i);
            }
        }
        idleCount = 0;
    }

    unsigned int firstNode = 0;
    double minV = 10000000;
    double V = 0;

    for (int i = 0; i < maxNerghborPE.size(); i++)
    {
        int node = maxNerghborPE[i];
        int node_x = node % 8;
        int node_y = node / 8;
        V = 0;
        for (int j = 0; j < petable[maxNerghborPE[i]].neighborsFreePE.size(); j++)
        {
            int node_n = petable[maxNerghborPE[i]].neighborsFreePE[j];
            int node_n_x = node_n % 8;
            int node_n_y = node_n / 8;

            if (node_n_x + 1 == node_x)
            {
                std::map<int, std::vector<double> >::iterator it1 = petable[node_n].e.begin();
                for (; it1 != petable[node_n].e.end(); it1++)
                {
                    V += (it1->second)[1];
                }

                std::map<int, std::vector<double> >::iterator it2 = petable[node].w.begin();
                for (; it2 != petable[node].w.end(); it2++)
                {
                    V += (it2->second)[1];
                }
            }

            if (node_n_x - 1 == node_x)
            {
                std::map<int, std::vector<double> >::iterator it3 = petable[node_n].w.begin();
                for (; it3 != petable[node_n].w.end(); it3++)
                {
                    V += (it3->second)[1];
                }

                std::map<int, std::vector<double> >::iterator it4 = petable[node].e.begin();
                for (; it4 != petable[node].e.end(); it4++)
                {
                    V += (it4->second)[1];
                }
            }

            if (node_n_y + 1 == node_y)
            {
                std::map<int, std::vector<double> >::iterator it5 = petable[node_n].s.begin();
                for (; it5 != petable[node_n].s.end(); it5++)
                {
                    V += (it5->second)[1];
                }

                std::map<int, std::vector<double> >::iterator it6 = petable[node].n.begin();
                for (; it6 != petable[node].n.end(); it6++)
                {
                    V += (it6->second)[1];
                }
            }

            if (node_n_y - 1 == node_y)
            {
                std::map<int, std::vector<double> >::iterator it7 = petable[node_n].n.begin();
                for (; it7 != petable[node_n].n.end(); it7++)
                {
                    V += (it7->second)[1];
                }

                std::map<int, std::vector<double> >::iterator it8 = petable[node].s.begin();
                for (; it8 != petable[node].s.end(); it8++)
                {
                    V += (it8->second)[1];
                }
            }
        }
        if (V < minV)
        {
            minV = V;
            firstNode = node;
        }
    }
    return firstNode;
}



int SimTask::SHiC(int app_size)
{
    int size_x = 8;
    int size_y = 8;
    int n_fn;
    int outloop = 0;
    int app_num = running_app_state.size() - 1;
    while (outloop <= floor(2 + sqrt(app_num)))
    {
        int n_cur;
        srand(time(NULL) + outloop * 100);
        n_cur = rand() % (m_pe_num);
        while (1)
        {
            if (!petable[n_cur].busy)
            {
                break;
            }
            else
            {
                n_cur = (n_cur + 1) % (m_pe_num - 1);
            }
        }
        int n_cur_x = n_cur % size_x;
        int n_cur_y = n_cur / size_x;
        // cout << "in while(in) a rand n_cur" << endl;
        std::vector<int> sf_n_cur = sf_calculation(n_cur);
        int iter = 0;
        bool yam_angle = false;
        while (iter < size_x / 2 && sf_n_cur[0] != app_size)
        {
            n_cur_x = n_cur % size_x;
            n_cur_y = n_cur / size_x;
            sf_n_cur = sf_calculation(n_cur);
            int dir_x = 0;
            int dir_y = 0;
            if (sf_n_cur[1] == 0 && sf_n_cur[2] == 0)
            {
                srand(time(NULL));
                dir_x = rand() % 2 - 1;
                srand(time(NULL) + 100);
                dir_y = rand() % 2 - 1;
                std::vector<int> v_temp;
                v_temp = get_next_dir(n_cur_x, n_cur_y, dir_x, dir_y, 0);
                if (v_temp[0] == 0)
                {
                    assert("no dir avilable");
                }
                dir_x = v_temp[1];
                dir_y = v_temp[2];
                // cout << "get a randn dir dur to 0,0 " << endl;
            }
            else
            {
                if (sf_n_cur[0] < app_size)
                {
                    dir_x = sf_n_cur[1];
                    dir_y = sf_n_cur[2];
                    if (!(n_cur_x + dir_x >= 0 && n_cur_x + dir_x < size_x && n_cur_y + dir_y >= 0 &&
                          n_cur_y + dir_y < size_y))
                    {
                        cout << "error 1 " << endl;
                    }
                    // cout << "dir is dir " << endl;
                }
                else
                {
                    dir_x = 0 - sf_n_cur[1];
                    dir_y = 0 - sf_n_cur[2];
                    std::vector<int> v_temp;
                    v_temp = get_next_dir(n_cur_x, n_cur_y, dir_x, dir_y, 0);
                    if (v_temp[0] == 0)
                    {
                        assert("no dir avilable");
                    }
                    dir_x = v_temp[1];
                    dir_y = v_temp[2];
                    // cout << "dir is - dir " << endl;
                }
                if (yam_angle)
                {
                    srand(time(NULL));
                    int rand1 = rand() % 3 + 3;
                    // int rand1 = rand() % 5 + 2;
                    std::vector<int> v_temp;
                    v_temp = get_next_dir(n_cur_x, n_cur_y, dir_x, dir_y, rand1);
                    if (v_temp[0] == 0)
                    {
                        assert("no dir avilable");
                    }
                    dir_x = v_temp[1];
                    dir_y = v_temp[2];
                }
            }

            int n_next_x = n_cur_x + dir_x;
            int n_next_y = n_cur_y + dir_y;
            int n_next = n_next_x + n_next_y * size_x;
            bool prefer = false;
            // cout << "in while(in) get a new n_next " << endl;
            // cout << dir_x << " " << dir_y << "  yam_angle is " << yam_angle << endl;
            std::vector<int> sf_n_next = sf_calculation(n_next);
            if ((sf_n_cur[0] < app_size && sf_n_next[0] > sf_n_cur[0]) ||
                (sf_n_cur[0] >= app_size && sf_n_next[0] < sf_n_cur[0]))
            {
                prefer = true;
            }
            if (!petable[n_next].busy && prefer)
            {
                n_cur = n_next;
                yam_angle = false;
                // cout << "choose the new node" << endl;
            }
            else
            {
                yam_angle = true;
                // cout << "not choose the new node" << endl;
            }
            iter++;
        }

        if (outloop == 0)
        {
            n_fn = n_cur;
        }
        // cout << "in while(out) n_fn :  (outloop is)" << outloop << endl;
        std::vector<int> sf_fn = sf_calculation(n_fn);
        bool prefer_1 = false;
        // cout << "in while(out) n_cur : " << endl;
        std::vector<int> sf_out_n_next = sf_calculation(n_cur);
        if ((sf_fn[0] < app_size && sf_out_n_next[0] > sf_fn[0]) ||
            (sf_fn[0] >= app_size && sf_out_n_next[0] < sf_fn[0]))
        {
            prefer_1 = true;
        }
        if (prefer_1)
        {
            n_fn = n_cur;
        }
        else if (sf_out_n_next[0] == sf_fn[0])
        {
            int n_fn_x = n_fn % size_x;
            int n_fn_y = n_fn / size_x;
            if (n_cur_x * n_cur_x + n_cur_y * n_cur_y < n_fn_x * n_fn_x + n_fn_y * n_fn_y)
            {
                n_fn = n_cur;
            }
        }
        outloop++;
    }
    // debug
    if (0)
    {
        for (int i = 0; i < m_pe_num; i++)
        {
            if (!petable[i].busy)
            {
                sf_calculation(i);
            }
        }
        cout << "Enter to continue the program: ";
        cin.ignore().get();
    }

    return n_fn;
}

std::vector<int> SimTask::get_next_dir(int n_cur_x, int n_cur_y, int dir_x_c, int dir_y_c, int n)
{
    int size_x = 8;
    int size_y = 8;
    int i;
    int axel[8][2] = { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } };
    for (i = 0; i < 8; i++)
    {
        if ((axel[i][0] == dir_x_c) && (axel[i][1] == dir_y_c))
            break;
    }
    int ok = 0;
    int dir_x = axel[(i + n) % 8][0];
    int dir_y = axel[(i + n) % 8][1];
    bool flag = false;
    for (int i = 0; i < 8; i++)
    {
        if (n_cur_x + dir_x >= 0 && n_cur_x + dir_x < size_x && n_cur_y + dir_y >= 0 && n_cur_y + dir_y < size_y)
        {
            flag = true;
            break;
        }
        else
        {
            n = n + 1;
            dir_x = axel[(i + n) % 8][0];
            dir_y = axel[(i + n) % 8][1];
        }
    }
    if (flag)
    {
        ok = 1;
    }
    std::vector<int> v;
    v.push_back(ok);
    v.push_back(dir_x);
    v.push_back(dir_y);
    return v;
}

std::vector<int> SimTask::sf_calculation(int n_cur)
{
    int x_size = 8;
    int y_size = 8;
    int n_cur_x = n_cur % x_size;
    int n_cur_y = n_cur / x_size;
    // 1.  rectangle
    vector<vector<int> > in_rect;
    in_rect.resize(x_size);
    for (int i = 0; i < x_size; i++)
    {
        in_rect[i].resize(y_size, -1);
    }
    in_rect[0][0] = 1000;
    std::map<unsigned int, std::vector<unsigned int> >::iterator it = running_app_state.begin();
    if (it != running_app_state.end())
    {
        for (; it != running_app_state.end(); it++)
        {
            int x_min = 1000;
            int y_min = 1000;
            int x_max = 0;
            int y_max = 0;
            for (int i = 0; i < (it->second).size(); i++)
            {
                int x = (it->second)[i] % x_size;
                int y = (it->second)[i] / x_size;
                if (x < x_min)
                {
                    x_min = x;
                    if (y < y_min)
                    {
                        y_min = y;
                    }
                }
                if (x > x_max)
                {
                    x_max = x;
                    if (y > y_max)
                    {
                        y_max = y;
                    }
                }
            }

            for (int i = x_min; i <= x_max; i++)
            {
                for (int j = y_min; j <= y_max; j++)
                {
                    in_rect[i][j] = it->first;
                }
            }
        }
    }

    vector<vector<int> > square;
    int square_count = 0;
    square.resize(x_size);
    for (int i = 0; i < x_size; i++)
    {
        square[i].resize(y_size, 0);
    }
    std::vector<int> dir_count; // 0: up 1:down 2:left 3:right
    dir_count.resize(4, 0);
    // 2. large square
    // 3. add node
    // 4. open_dir
    int square_size = 3;
    while (1)
    {
        dir_count[0] = 0;
        dir_count[1] = 0;
        dir_count[2] = 0;
        dir_count[3] = 0;
        bool flag = false;
        int n_cur_x_min;
        int n_cur_x_max;
        int n_cur_y_min;
        int n_cur_y_max;
        if (n_cur_x - (square_size - 1) / 2 < 0)
        {
            n_cur_x_min = 0;
            flag = true;
        }
        else
        {
            n_cur_x_min = n_cur_x - (square_size - 1) / 2;
        }

        if (n_cur_y - (square_size - 1) / 2 < 0)
        {
            n_cur_y_min = 0;
            flag = true;
        }
        else
        {
            n_cur_y_min = n_cur_y - (square_size - 1) / 2;
        }

        if (n_cur_x + (square_size - 1) / 2 >= x_size)
        {
            n_cur_x_max = x_size - 1;
            flag = true;
        }
        else
        {
            n_cur_x_max = n_cur_x + (square_size - 1) / 2;
        }

        if (n_cur_y + (square_size - 1) / 2 >= y_size)
        {
            n_cur_y_max = y_size - 1;
            flag = true;
        }
        else
        {
            n_cur_y_max = n_cur_y + (square_size - 1) / 2;
        }

        if (in_rect[n_cur_x][n_cur_y] >= 0)
        {
            flag = true;
        }

        for (int i = n_cur_x_min; i <= n_cur_x_max; i++)
        {
            for (int j = n_cur_y_min; j <= n_cur_y_max; j++)
            {
                if (in_rect[n_cur_x][n_cur_y] >= 0)
                {
                    if (square[i][j] == 0)
                    {
                        square[i][j] = 1;
                        square_count++;
                        // direction count
                        if (i == n_cur_x - (square_size - 1) / 2 &&
                            (j != n_cur_y - (square_size - 1) / 2 && j != n_cur_y + (square_size - 1) / 2))
                        {
                            dir_count[2]++;
                        }
                        if (i == n_cur_x + (square_size - 1) / 2 &&
                            (j != n_cur_y - (square_size - 1) / 2 && j != n_cur_y + (square_size - 1) / 2))
                        {
                            dir_count[3]++;
                        }
                        if (j == n_cur_y - (square_size - 1) / 2 &&
                            (i != n_cur_x - (square_size - 1) / 2 && i != n_cur_x + (square_size - 1) / 2))
                        {
                            dir_count[0]++;
                        }
                        if (j == n_cur_y + (square_size - 1) / 2 &&
                            (i != n_cur_x - (square_size - 1) / 2 && i != n_cur_x + (square_size - 1) / 2))
                        {
                            dir_count[1]++;
                        }
                    }
                }
                else
                {
                    if (in_rect[i][j] >= 0)
                    {
                        flag = true;
                    }
                    else if (square[i][j] == 0)
                    {
                        square[i][j] = 1;
                        square_count++;
                        // direction count
                        if (i == n_cur_x - (square_size - 1) / 2 &&
                            (j != n_cur_y - (square_size - 1) / 2 && j != n_cur_y + (square_size - 1) / 2))
                        {
                            dir_count[2]++;
                        }
                        if (i == n_cur_x + (square_size - 1) / 2 &&
                            (j != n_cur_y - (square_size - 1) / 2 && j != n_cur_y + (square_size - 1) / 2))
                        {
                            dir_count[3]++;
                        }
                        if (j == n_cur_y - (square_size - 1) / 2 &&
                            (i != n_cur_x - (square_size - 1) / 2 && i != n_cur_x + (square_size - 1) / 2))
                        {
                            dir_count[0]++;
                        }
                        if (j == n_cur_y + (square_size - 1) / 2 &&
                            (i != n_cur_x - (square_size - 1) / 2 && i != n_cur_x + (square_size - 1) / 2))
                        {
                            dir_count[1]++;
                        }
                    }
                }
            }
        }
        if (flag)
        {
            break;
        }
        square_size = square_size + 2;
    }
    square_size = square_size - 2;
    // 5. direction jugde
    vector<int> dir_vector;
    dir_vector.push_back(0);
    dir_vector.push_back(0);
    int max = 0;
    for (int i = 0; i < 3; i++)
    {
        if (dir_count[i] > max)
        {
            max = dir_count[i];
        }
    }
    if (max != 0)
    {
        if (dir_count[0] == max)
        {
            dir_vector[1] = dir_vector[1] - 1;
        }
        if (dir_count[1] == max)
        {
            dir_vector[1] = dir_vector[1] + 1;
        }
        if (dir_count[2] == max)
        {
            dir_vector[0] = dir_vector[0] - 1;
        }
        if (dir_count[3] == max)
        {
            dir_vector[0] = dir_vector[0] + 1;
        }
    }
    // cout << "dir_vector : " << dir_vector[0] << " " << dir_vector[1] << endl;
    vector<int> return_v;
    return_v.push_back(square_count);
    return_v.push_back(dir_vector[0]);
    return_v.push_back(dir_vector[1]);

    // debug
    if (0)
    {
        cout << "-----------SF for (" << n_cur_x << " , " << n_cur_y << ")------------\n";
        cout << "SF value: " << return_v[0] << "\n";
        for (int i = 0; i < square.size(); i++)
        {
            for (int j = 0; j < square[i].size(); j++)
            {
                cout << square[j][i] << " ";
            }
            cout << endl;
        }
        cout << "max " << max << "  |  " << dir_count[0] << " , " << dir_count[1] << " , " << dir_count[2] << " , "
             << dir_count[3] << "    |   "
             << "direction : (" << return_v[1] << "," << return_v[2] << ")" << endl;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (petable[i * 8 + j].busy)
                {
                    cout << "*"
                         << " ";
                }
                else
                {
                    cout << "-"
                         << " ";
                }
            }
            cout << endl;
        }
        cout << "in_rect: " << endl;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (in_rect[j][i] >= 0)
                {
                    cout << "*"
                         << " ";
                }
                else
                {
                    cout << "-"
                         << " ";
                }
            }
            cout << endl;
        }
        cout << "Enter to continue the program: ";
        cin.ignore().get();
    }
    return return_v;
}

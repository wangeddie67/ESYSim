/*
 * Simulation.cpp
 *
 *  Created on: 2016年8月16日
 *      Author: root
 */

#include "Simulation.h"
//仿真序列实现原理：lcm_n lcm
/*
 * step1:求N个节点频率的最小公倍数LCM1
 * step2:用LCM1除以N个节点的频率得到各个节点自己的特征值
 * step3:求特征值的最小公倍数LCM2
 * step4:从0到LCM2递加1，得到的值除以各自各自的特征值，如果整除，则仿真1个周期
 * */

//int Simulation::simlink = NULL;

char* Simulation::esynetBuffer = (char*)calloc(ESYNETBUFFERSIZE,BUFSIZ);
FILE* Simulation::visualFile = fopen("./records/esynet7.recordt","w+");
FILE* Simulation::testfile = fopen("./records/esynet4","w+");

Simulation::Simulation()
{
	// TODO Auto-generated constructor stub
}

void Simulation::lcm_n(TNode** tn, int num)
{
    int i;
    int lcm1 = 0;
    int lcm2 = 0;
    int noc_f = Noc::frequence;
    for(i = 0; i < MAXTHREADS; i ++)
    {
        if(tn[i])
        {
            lcm1 = lcm1 ? lcm1 : tn[i]->frequence;
            lcm1 = lcm(lcm1, tn[i]->frequence);
        }
    }
//	for(i=1;i<num;i++){
//		lcm1 = lcm(lcm1,(*(tn+i))->frequence);
//	}
    lcm1 = lcm(lcm1, noc_f);
    for(i = 0; i < MAXTHREADS; i ++)
    {
        if(tn[i])
        {
            tn[i]->frequence_holder = lcm1 / (tn[i]->frequence);
        }
    }
    Noc::frequence_holder = lcm1 / noc_f;
    for(i = 0; i < MAXTHREADS; i ++)
    {
        if(tn[i])
        {
            lcm2 = lcm2 ? lcm2 : tn[i]->frequence_holder;
            lcm2 = lcm(lcm2, tn[i]->frequence_holder);
        }
    }
    lcm2 = lcm(lcm2, Noc::frequence_holder);
    TNode::frequenceholder_lcm = lcm2;
}

int Simulation::lcm(int l, int m)
{
    int lt = l;
    int mt = m;
    int remainder = -1;
    while(remainder != 0)
    {
        remainder = l % m;
        l = m;
        m = remainder;
    }
    return lt * mt / l;
}

Simulation::~Simulation() 
{
    // TODO Auto-generated destructor stub
}


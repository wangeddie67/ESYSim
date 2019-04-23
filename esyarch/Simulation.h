/*
 * Simulation.h
 *
 *  Created on: 2016年8月16日
 *      Author: root
 */

#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <stdio.h>
#include <vector>
#include "node.h"
#include "Noc.h"
class TNode;
class Simulation 
{
public:
    Simulation();
    static void lcm_n(TNode**, int num);
    static FILE* visualFile;
    static FILE* testfile;
    static char* esynetBuffer;
//	static const char* node_statistics_dir = "./record/";
//	vector<FILE*> node_statistics;
    virtual ~Simulation();

//	static int *simlink;

private:
	static int lcm(int l, int m);
};


#endif /* SIMULATION_H_ */

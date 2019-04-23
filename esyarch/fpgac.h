/*
 * fpgac.h
 *
 *  Created on: 2017年4月25日
 *      Author: root
 */

#ifndef ESYSIM_OPENSOURCE_ESYARCH_FPGAC_H_
#define ESYSIM_OPENSOURCE_ESYARCH_FPGAC_H_

#include <map>
#include <vector>
#include "define.h"
#include "node.h"
using namespace std;


class MPIMsg;
class Noc;

//fft数据结构
typedef struct fft_data
{
	int real;
	int imag;

}fft_data_t;
struct fft_status
{
	int src;
	char* msgchara;
	fft_data_t* source;
	fft_data_t* target;
};

class Fpgac:public TNode
{
public:
	//串口信息
//	static const char *dev;
//	int fd;
	//公共数据结构
	const int cycle;
	int block;
	tick_t sim_cycle;
	asic_t t;
	fft_status* now_run;
	map<char*, void*> task_list;
	Fpgac(FpgaStruct* fs, int threadid);
	virtual sim_rt_t run();
	void data_send(void* data, MPIMsg* msg);
	//具体不同种类asic的send函数
	void send_asic_fft_128(fft_status* data);
};


#endif /* ESYSIM_OPENSOURCE_ESYARCH_FPGAC_H_ */

/*
 * define.h
 *
 *  Created on: 2016年3月5日
 *      Author: root
 */

#ifndef DEFINE_H_
#define DEFINE_H_
/*global define(which is contain the programmer(outside of node))*/
#define NA		0
#define DNA			(0)
#define MAXTHREADS  64	//最大节点数
#define MAXTAUGNUM 50	//最大参数个数
#define MAXAUGLENG 50	//单个参数最大字符数

//MCPAT
#define MCPAT_CALL_FREQ		3000
//仿真运行时问题
enum sim_rt_t
{
	NO_Q = 0,//no question
	NOWORKING,
	MAX_INST
};

//tools配置函数类型
typedef void (*toolsConfigFunc)(const char* configstr);

//esynet输出缓冲区大小
#define ESYNETBUFFERSIZE 5


/*
 * LCH:
 * NOTE:For the same struct, the app and ss may have different size, which could lead to a mistake.
 * It may be that the sizeof(long) is 8 Bytes in ss, but 4 Bytes in app.That means sizeof(long) equal to sizeof(int) in ss.
 * So, avoid the using of long.(replaced by long long)
 * */
/*Application Interface*/
//MPI
#define MAX_MPICHARA 20
#define FLIT_SIZE (64) /*bit*/
#define PACK_SIZE (5) /*number of flit*/
#define MSG_SIZE (FLIT_SIZE*PACK_SIZE/8) /*Bytes*/
typedef struct
{
	int msgsize;
	long long int cycle;
	long long int noccycle;
	long long int msg_id;
	long long int over_cycle;
	long long int over_cycle_noc;
}ss_msg_r;
/*
 * MPI消息状态
 * */
enum Msgstatus
{
    nomsg = 0,
    msgnoarrive,
    msgarrived,
    fpga_msgcreat,
    fpga_noarrive,//还没到达fpga
    fpga_waitcomput,//到达fpga，等待计算
    fpga_comput,//fpga正在计算
    fpga_returning,//fpga正在返回，包在网络
    fpge_over//结果从网络到达，结束可取
};

enum asic_t{
	asic_fft_128
};



#endif /* DEFINE_H_ */

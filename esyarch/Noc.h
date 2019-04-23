/*
 * Noc.h
 *
 *  Created on: 2016年8月17日
 *      Author: root
 */

#ifndef NOC_H_
#define NOC_H_
#include <vector>
#include "define.h"
#include "node.h"

using namespace std;
class Noc;
class MPIMsg
{
public:
	short int over;
	int src;
	int des;
	int msgsize;//Byte
	long long msgid;
	long long simcycle;//核的cycle
	long long simcycle_noc;//noc 的cycle
	long long over_cycle;
	long long over_cycle_noc;
	char* character;//用户空间表征这个信息
	void* msgpnt;//msg的指针
	int type;//0:core-core; 1:core-fpga
	enum Msgstatus fpga_status;
	static long long int msgnum;
	MPIMsg(int _src,int _des,int _msgsize,char* _character,void* _msgpnt, int type = 0, enum Msgstatus _fpga_status = nomsg);
	int LookupMsgByCharacter(int _src,int _des,const char* _character);
	int LookupMsgByMsgid(int _src,int _des,long long _msgid);
	void SetInsertCycle(long long _simcycle);
	short int IsOver();
	void Over();
	void fpga_over();
	virtual ~MPIMsg();
	friend class Noc;
};

class Noc
{
public:
	Noc();
	ss_msg_r* InsertMsg(MPIMsg * msg,long long _simcycle);//网络注入一条消息
	long long int LastMsgId() const {return msgid_noc;}//随着发送消息而递增的id
	MPIMsg* MsgReceiveFromClient(const char* _character,int _src,int _des);//用户空间查询消息是否到达
	int ReceiveMsgFromNet(int _src,int _dst,long long _settime,long long _msgNo);
	Msgstatus LoopUpMsgStatus(const char* _character,int _src,int _des);
	virtual ~Noc();
	static long long int sim_cycle;
	static int frequence;
	static int frequence_holder;
	char* netconfig;
	vector<MPIMsg*> MsgQueue;
private:
	long long msgid_noc;
//	MPIMsg* CheckMsgFromCharacter(char* _character);
};


#endif /* NOC_H_ */

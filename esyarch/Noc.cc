/*
 * Noc.cpp
 *
 *  Created on: 2016年8月17日
 *      Author: root
 */
#include <typeinfo>
#include <string.h>
#include <assert.h>
#include "Noc.h"
#include "fpgac.h"
long long int Noc::sim_cycle = 0;
int Noc::frequence = 1;
int Noc::frequence_holder = 1;
long long int MPIMsg::msgnum = 0;
extern void simEsynetMessageInsert(long s, long d, long long int
sim_cycle, long size, counter_t msgNo, int vc);
/*
 * class MPIMsg
 * */
MPIMsg::MPIMsg(int _src,int _des,int _msgsize,char* _character,void* _msgpnt, int _type, enum Msgstatus _fpga_status)
:over(0),src(_src),des(_des),msgid(msgnum),msgsize(_msgsize),character(_character),msgpnt(_msgpnt),type(_type),
 fpga_status(_fpga_status), simcycle(0),over_cycle(0),over_cycle_noc(0){
	assert((msgsize>=0 && character!=NULL));
	msgnum++;
}

int
MPIMsg::LookupMsgByCharacter(int _src,int _des,const char* _character){
	if(src==_src && des==_des && !strcmp(_character,character)){return 1;}
	return 0;
}

int
MPIMsg::LookupMsgByMsgid(int _src,int _des,long long _msgid){
	if(src==_src && des==_des && msgid==_msgid){return 1;}
	return 0;
}

short int
MPIMsg::IsOver(){
	if(type == 1)
		return (short int)fpga_status;
	return over;
}

void
MPIMsg::Over(){
	over = msgarrived;
}

void
MPIMsg::fpga_over(){
	if(fpga_status == fpga_noarrive){fpga_status = fpga_waitcomput;}
	if(fpga_status == fpga_returning){fpga_status = fpge_over;}
}

void
MPIMsg::SetInsertCycle(long long _simcycle){
	simcycle = _simcycle;
}
MPIMsg::~MPIMsg(){
	delete character;
	delete msgpnt;
}
/*
 * class Noc
 * */
Noc::Noc():msgid_noc(0),netconfig((char*)calloc(300,sizeof(char))){
	// TODO Auto-generated constructor stub
}

ss_msg_r*
Noc::InsertMsg(MPIMsg * msg,long long int _simcycle){
	msg->SetInsertCycle(_simcycle);
	msg->simcycle_noc = sim_cycle;
	MsgQueue.push_back(msg);
	msgid_noc++;
	ss_msg_r* rm = (ss_msg_r*)malloc(sizeof(ss_msg_r));
	rm->cycle = _simcycle;
	rm->noccycle = sim_cycle;//消息发出时noc的cycle
	rm->msgsize = msg->msgsize;
	rm->msg_id = msg->msgid;
	simEsynetMessageInsert(msg->src, msg->des, sim_cycle, (msg->msgsize)/4+2, msg->msgid, 0);
	if(msg->type == 1){
		if(msg->fpga_status == fpga_msgcreat)
			msg->fpga_status = fpga_noarrive;
	}
	return rm;
}

Msgstatus
Noc::LoopUpMsgStatus(const char* _character,int _src,int _des){
	vector<MPIMsg*>::iterator itor;
	for(itor=MsgQueue.begin();itor!=MsgQueue.end();itor++){
		if((*itor)->LookupMsgByCharacter(_src,_des,_character)){
				return (enum Msgstatus)(*itor)->IsOver();
			}
		}
	return nomsg;
}

MPIMsg*
Noc::MsgReceiveFromClient(const char* _character,int _src,int _des){
	vector<MPIMsg*>::iterator itor;
	for(itor=MsgQueue.begin();itor!=MsgQueue.end();itor++){
		if((*itor)->LookupMsgByCharacter(_src,_des,_character)){
			return (*itor);
		}
	}
	return NULL;
}

//MPIMsg*
//Noc::CheckMsgFromCharacter(char* _character){
//	vector<MPIMsg *>::iterator p;
//	for(p=MsgQueue.begin();p!=MsgQueue.end();p++){
//		if(!strcmp((*(*p)).character,_character)){
//			return p;
//		}
//	}
//	return NULL;
//}

int
Noc::ReceiveMsgFromNet(int _src, int _dst, long long _settime, long long _msgNo){
	vector<MPIMsg *>::iterator itor;
	for(itor=MsgQueue.begin();itor!=MsgQueue.end();itor++){
		if((*itor)->LookupMsgByMsgid(_src,_dst,_msgNo)){
			if((*itor)->type == 1){//fpga
				(*itor)->fpga_over();
			}
			(*itor)->Over();
			(*itor)->over_cycle_noc = _settime;
			return 1;
		}
	}
	return nomsg;
}
Noc::~Noc() {
	// TODO Auto-generated destructor stub
}


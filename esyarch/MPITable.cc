/*
 * MPITable.cc
 *
 *  Created on: 2016年5月9日
 *      Author: root
 */
#include "host.h"
#include "misc.h"
#include "node.h"
#include "MPITable.h"

MPITable::MPITable(){
	Id_table_init();
}
void MPITable::Id_table_init(){
	int t1,t2;
	for(t1=0;t1<MAXTHREADS;t1++){
        ID_TABLE[t1][0] = 0;
		for(t2=1;t2<ID_TYPE;t2++){
			ID_TABLE[t1][t2]=-1;
		}
	}
}
int MPITable::Id_table_assign(int id,int master_id,int actual_id){
	if(actual_id>=MAXTHREADS){panic("enter actual_id error\n");}
	int t1;
	//check up
	for(t1=0;t1<MAXTHREADS;t1++){
	if(ID_TABLE[t1][0]==1){
		if(ID_TABLE[t1][3]==actual_id){
			panic("the thread %d is being using\n",actual_id);
			//do other thing
			}
		if(ID_TABLE[t1][2]==master_id && ID_TABLE[t1][1]==id){
			panic("master %d has the same id %d\n",master_id,id);
			}
		}
	}
	ID_TABLE[actual_id][0]=1;
	ID_TABLE[actual_id][1]=id;
	ID_TABLE[actual_id][2]=master_id;
	ID_TABLE[actual_id][3]=actual_id;
	return actual_id;
}
//根据master_id和id获取actual_id
int MPITable::Id_table_get(int master_id,int id){
	int num=0;
	int actual_id=-1;
	int t1;
	for(t1=0;t1<MAXTHREADS;t1++){
		if(ID_TABLE[t1][0]==1 && ID_TABLE[t1][1]==id && ID_TABLE[t1][2]==master_id){
			num++;
			actual_id=t1;
		}
	}
	if(num==1)
		return actual_id;
	else if(num==0){
		panic("have no this thread where master_id=%d & id=%d\n",master_id,id);
		return 1;
	}
	else{
		panic("some error must happen\n");
	}
}
MPITable::~MPITable(){

}
//int Id_table_getmy(TNode & t){
//	return t.Getmythreadid();
//}

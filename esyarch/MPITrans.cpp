/*
 * MPITrans.cpp
 *
 *  Created on: 2016年5月26日
 *      Author: root
 */
#include <iostream>
#include "MPITrans.h"
using namespace std;
namespace std {

MPITrans::MPITrans() {
	// TODO Auto-generated constructor stub


}
//void MPITrans::msgqueue_insert(struct msg_queue *queue, struct msg_type *msg)
//{
//	if(queue->head == NULL)
//	{
//		queue->head = msg;
//		queue->tail = msg;
//		queue->msg_size ++;
//	}
//	else
//	{
//		queue->tail->next = msg;
//		msg->prev = queue->tail;
//		queue->tail = msg;
//		queue->msg_size ++;
//	}
//}
MPITrans::~MPITrans() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */

/*
 * MPITrans.h
 *
 *  Created on: 2016年5月26日
 *      Author: root
 */

#ifndef MPITRANS_H_
#define MPITRANS_H_
struct msg_type_t
{
	char *msg_info;
	short int status;
	int len;
	int src;
	int des;
	long msg_num;
	struct msg_type *next;
	struct msg_type *prev;
};
struct msg_queue_t
{
	struct msg_type *head;
	struct msg_type *tail;
	int msg_size;
};

class MPITrans 
{
public:
	msg_queue_t msg_queue;
	MPITrans();
	void MPITrans::msgqueue_insert(struct msg_queue_t *queue, struct msg_type_t *msg);
	virtual ~MPITrans();
};

#endif /* MPITRANS_H_ */

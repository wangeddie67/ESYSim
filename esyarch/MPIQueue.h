#ifndef MPIQUEUE_H_
#define MPIQUEUE_H_

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
class MPIQueue
{


};

#endif

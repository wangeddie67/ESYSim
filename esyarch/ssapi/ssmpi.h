/*
 * ssmpi.h
 *	this file defines the MPI functions API standards of SimpleScalar
 *  Created on: 2016年7月10日
 *      Author: Chenghao Liang
 *
 * NOTE:
 *	SS:SimpeScalar
 * define.h: define the common definitions shared by application and processor(SS),it must be included
 */

#ifndef SSAPI_SSMPI_H_
#define SSAPI_SSMPI_H_
#include "../define.h"


/*
 * sendmsg()
 * 	:send message
 * 		:arg1:target node (defined by thread_id, which tagged by application)
 * 		:arg2:pointer of msg (nomatter what types of the msg is)
 * 		:arg3:msg size (standard by size_t, the value of sizeof() function)
 * 		:arg4:msg character, it must be unique
 * 		:return:none
 * */
 ss_sendmsg_f(int target_node, void* msg_p, size_t msg_size, const char* character, ss_msg_r* msgstatus);

/*
 * sendmsg()
 * 	:send message
 * 		:arg1:target node (defined by actual id)
 * 		:arg2:pointer of msg (nomatter what types of the msg is)
 * 		:arg3:msg size (standard by size_t, the value of sizeof() function)
 * 		:arg4:msg character, it must be unique
 * 		:return:none
 * */
 ss_sendmsg(int target_nodeid, void* msg_p, size_t msg_size, const char* character, ss_msg_r* msgstatus);
 /*
  * recvmsg()
  * 	:receive message
  * 		:arg1:thread id (src)
  * 		:arg2:msg character,it must be unique
  * 		:arg3:msg pointer for receiving
  *		:return:none
  * */
 void* ss_recvmsg(int src_nodeid, const char* character, void* msgprt, ss_msg_r* msgstatus);

 /*
  * recvmsg()
  * 	:receive message
  * 		:arg1:thread id (src)
  * 		:arg2:msg character,it must be unique
  * 		:arg3:msg pointer for receiving
  *		:return:none
  * */
 void* ss_recvmsg_f(int target_node, const char* character, void* msgprt, ss_msg_r* msgstatus);

 /*
  *ismsgarrive()
  *	:look for the MPI msg queue, return the msg status.
  *		:arg1:thread id (src)
  *		:arg2:thread id (des)
  *		:arg3:msg character, it must be unique
  *		:return:the msg status define by Msgstatus.
  * */
 enum Msgstatus ss_msgstatus(int src_nodeid, int des_nodeid, const char* character);

 /*
   *ismsgarrive()
   *	:look for the MPI msg queue, return the msg status.
   *		:arg1:thread id (src)
   *		:arg2:thread id (des)
   *		:arg3:msg character, it must be unique
   *		:return:the msg status define by Msgstatus.
   * */
 enum Msgstatus ss_msgstatus_f(int src_threadid, int des_nodeid, const char* character);

 /*
  * getmythreadid()
  * 	:return threadid of this application
  * */
 int ss_getmythreadid(void);
 /*
  * getmyactualid()
  * 	:return actual_id of this application,where ini.in mapped.
  * */
 int ss_getmyactualid(void);



#endif /* SSAPI_SSMPI_H_ */

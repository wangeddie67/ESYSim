/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ssapi/ssmpi.h"
void main(){
//	const char* recdfile = "./testmpi/recdfile.out";
//	FILE* appf = fopen(recdfile,"a+");

//-------------------------

	struct struct_t{
		int a;
		char* ps;
	};
	int i,j;
	int id = ss_getmythreadid();
	printf("my id is %d\n",id);
	if(id == 0){
		int int_test = 199;
		ss_msg_r* tmsg_int, *tmsg_struct;
		struct struct_t p;
		tmsg_int = malloc(sizeof(ss_msg_r));
		tmsg_struct = malloc(sizeof(ss_msg_r));
		p.a = 10;
		p.ps = "context package";
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara1",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara2",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara3",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara4",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara5",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara6",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara7",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara8",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara9",tmsg_struct);
		ss_sendmsg(1, &p, sizeof(struct struct_t),"01chara10",tmsg_struct);

		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara1",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara2",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara3",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara4",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara5",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara6",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara7",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara8",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara9",tmsg_struct);
		ss_sendmsg(2, &p, sizeof(struct struct_t),"02chara10",tmsg_struct);

		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara1",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara2",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara3",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara4",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara5",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara6",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara7",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara8",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara9",tmsg_struct);
		ss_sendmsg(3, &p, sizeof(struct struct_t),"03chara10",tmsg_struct);
//		ss_sendmsg(1, &p, sizeof(struct struct_t),"chara1",tmsg_struct);
//		ss_sendmsg(1, &int_test, sizeof(int),"chara2",tmsg_int);
//		printf("send a struct in noc_cycle %lld and core_cycle %lld \n", tmsg_struct->noccycle, tmsg_struct->cycle);
//		printf("send a int in noc_cycle %lld and core_cycle %lld \n", tmsg_int->noccycle, tmsg_int->cycle);

	}
	if(id == 2){
		struct struct_t* p2 = malloc(sizeof(struct struct_t));
		int i2;
		ss_msg_r *tmsg_int_recv, *tmsg_struct_recv;
		tmsg_int_recv = malloc(sizeof(int));
		tmsg_struct_recv = malloc(sizeof(ss_msg_r));
		while(ss_msgstatus(0,2,"02chara1") == nomsg || ss_msgstatus(0,2,"02chara1") == msgnoarrive){}
		ss_recvmsg(0, "02chara1", p2, tmsg_struct_recv);

		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara2") == nomsg || ss_msgstatus(0,2,"02chara2") == msgnoarrive){}
		ss_recvmsg(0, "02chara2", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara3") == nomsg || ss_msgstatus(0,2,"02chara3") == msgnoarrive){}
		ss_recvmsg(0, "02chara3", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara4") == nomsg || ss_msgstatus(0,2,"02chara4") == msgnoarrive){}
		ss_recvmsg(0, "02chara4", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara5") == nomsg || ss_msgstatus(0,2,"02chara5") == msgnoarrive){}
		ss_recvmsg(0, "02chara5", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara6") == nomsg || ss_msgstatus(0,2,"02chara6") == msgnoarrive){}
		ss_recvmsg(0, "02chara6", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara7") == nomsg || ss_msgstatus(0,2,"02chara7") == msgnoarrive){}
		ss_recvmsg(0, "02chara7", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara8") == nomsg || ss_msgstatus(0,2,"02chara8") == msgnoarrive){}
		ss_recvmsg(0, "02chara8", p2, tmsg_struct_recv);
				printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara9") == nomsg || ss_msgstatus(0,2,"02chara9") == msgnoarrive){}
		ss_recvmsg(0, "02chara9", p2, tmsg_struct_recv);
				printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
												p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,2,"02chara10") == nomsg || ss_msgstatus(0,2,"02chara10") == msgnoarrive){}
		ss_recvmsg(0, "02chara10", p2, tmsg_struct_recv);
						printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
														p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		//int
		while(ss_msgstatus(3,2,"32int1") == nomsg || ss_msgstatus(3,2,"32int1") == msgnoarrive){}
				ss_recvmsg(3, "32int1", &i2, tmsg_int_recv);
				printf("msg11 received, it's a int\n", i2);
		while(ss_msgstatus(3,2,"32int2") == nomsg || ss_msgstatus(3,2,"32int2") == msgnoarrive){}
				ss_recvmsg(3, "32int2", &i2, tmsg_int_recv);printf("msg12 received, it's a int\n", i2);
		while(ss_msgstatus(3,2,"32int3") == nomsg || ss_msgstatus(3,2,"32int3") == msgnoarrive){}
				ss_recvmsg(3, "32int1", &i2, tmsg_int_recv);printf("msg13 received, it's a int\n", i2);
		while(ss_msgstatus(3,2,"32int4") == nomsg || ss_msgstatus(3,2,"32int4") == msgnoarrive){}
				ss_recvmsg(3, "32int1", &i2, tmsg_int_recv);printf("msg14 received, it's a int\n", i2);
		while(ss_msgstatus(3,2,"32int5") == nomsg || ss_msgstatus(3,2,"32int5") == msgnoarrive){}
				ss_recvmsg(3, "32int5", &i2, tmsg_int_recv);printf("msg15 received, it's a int\n", i2);
		while(ss_msgstatus(3,2,"32int6") == nomsg || ss_msgstatus(3,2,"32int6") == msgnoarrive){}
				ss_recvmsg(3, "32int6", &i2, tmsg_int_recv);printf("msg16 received, it's a int\n", i2);
		while(ss_msgstatus(3,2,"32int7") == nomsg || ss_msgstatus(3,2,"32int7") == msgnoarrive){}
				ss_recvmsg(3, "32int7", &i2, tmsg_int_recv);printf("msg17 received, it's a int\n", i2);
		while(ss_msgstatus(3,2,"32int8") == nomsg || ss_msgstatus(3,2,"32int8") == msgnoarrive){}
				ss_recvmsg(3, "32int8", &i2, tmsg_int_recv);printf("msg18 received, it's a int\n", i2);
	}
	if(id==1){
		struct struct_t* p2 = malloc(sizeof(struct struct_t));
		ss_msg_r *tmsg_struct_recv;
		tmsg_struct_recv = malloc(sizeof(ss_msg_r));
		while(ss_msgstatus(0,1,"01chara1") == nomsg || ss_msgstatus(0,1,"01chara1") == msgnoarrive){}
		ss_recvmsg(0, "01chara1", p2, tmsg_struct_recv);
		printf("msg1 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara2") == nomsg || ss_msgstatus(0,1,"01chara2") == msgnoarrive){}
		ss_recvmsg(0, "01chara2", p2, tmsg_struct_recv);
		printf("msg2 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara3") == nomsg || ss_msgstatus(0,1,"01chara3") == msgnoarrive){}
		ss_recvmsg(0, "01chara3", p2, tmsg_struct_recv);
		printf("msg3 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara4") == nomsg || ss_msgstatus(0,1,"01chara4") == msgnoarrive){}
		ss_recvmsg(0, "01chara4", p2, tmsg_struct_recv);
		printf("msg4 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara5") == nomsg || ss_msgstatus(0,1,"01chara5") == msgnoarrive){}
		ss_recvmsg(0, "01chara5", p2, tmsg_struct_recv);
		printf("msg5 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara6") == nomsg || ss_msgstatus(0,1,"01chara6") == msgnoarrive){}
		ss_recvmsg(0, "01chara6", p2, tmsg_struct_recv);
		printf("msg6 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara7") == nomsg || ss_msgstatus(0,1,"01chara7") == msgnoarrive){}
		ss_recvmsg(0, "01chara7", p2, tmsg_struct_recv);
		printf("msg7 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara8") == nomsg || ss_msgstatus(0,1,"01chara8") == msgnoarrive){}
		ss_recvmsg(0, "01chara8", p2, tmsg_struct_recv);
				printf("msg8 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
												p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara9") == nomsg || ss_msgstatus(0,1,"01chara9") == msgnoarrive){}
		ss_recvmsg(0, "01chara9", p2, tmsg_struct_recv);
				printf("msg9 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
												p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(0,1,"01chara10") == nomsg || ss_msgstatus(0,1,"01chara10") == msgnoarrive){}
		ss_recvmsg(0, "01chara10", p2, tmsg_struct_recv);
						printf("msg10 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
														p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);

	}
	if(id==3){
		int int_test = 199;
		ss_msg_r* tmsg_int;
		ss_sendmsg(2, &int_test, sizeof(int),"32int1",tmsg_int);
		ss_sendmsg(2, &int_test, sizeof(int),"32int2",tmsg_int);
		ss_sendmsg(2, &int_test, sizeof(int),"32int3",tmsg_int);
		ss_sendmsg(2, &int_test, sizeof(int),"32int4",tmsg_int);
		ss_sendmsg(2, &int_test, sizeof(int),"32int5",tmsg_int);
		ss_sendmsg(2, &int_test, sizeof(int),"32int6",tmsg_int);
		ss_sendmsg(2, &int_test, sizeof(int),"32int7",tmsg_int);
		ss_sendmsg(2, &int_test, sizeof(int),"32int8",tmsg_int);

	}

//--------------------------


}
*/


//		printf("sizeof short int  = %d\n",sizeof(short int));
//		printf("sizeof int  = %d\n",sizeof(int));
//		printf("sizeof long  = %d\n",sizeof(long int));
//		printf("sizeof long long  = %d\n",sizeof(long long int));
//		printf("sizeof tmsg = %d",sizeof(ss_msg_r));
//		printf("my actualid = %d, and initial msg size = %d\n",id,sizeof(msg));
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ssapi/ssmpi.h"
void main(){
//	const char* recdfile = "./testmpi/recdfile.out";
//	FILE* appf = fopen(recdfile,"a+");

//-------------------------

	struct struct_t{
		int a;
		char* ps;
	};
	int i,j;
	int id = ss_getmyactualid();
	printf("my id is %d\n",id);
	if(id == 1){
		int int_test = 199;
		ss_msg_r* tmsg_int, *tmsg_struct;
		struct struct_t p;
		tmsg_int = malloc(sizeof(ss_msg_r));
		tmsg_struct = malloc(sizeof(ss_msg_r));
		p.a = 10;
		p.ps = "context package";
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara1",tmsg_struct);printf("0 1\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara2",tmsg_struct);printf("0 2\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara3",tmsg_struct);printf("0 3\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara4",tmsg_struct);printf("0 4\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara5",tmsg_struct);printf("0 5\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara6",tmsg_struct);printf("0 6\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara7",tmsg_struct);printf("0 7\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara8",tmsg_struct);printf("0 8\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara9",tmsg_struct);printf("0 9\n");
		ss_sendmsg(5, &p, sizeof(struct struct_t),"01chara10",tmsg_struct);printf("0 10\n");

		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara1",tmsg_struct);printf("0 11\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara2",tmsg_struct);printf("0 12\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara3",tmsg_struct);printf("0 13\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara4",tmsg_struct);printf("0 14\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara5",tmsg_struct);printf("0 15\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara6",tmsg_struct);printf("0 16\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara7",tmsg_struct);printf("0 17\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara8",tmsg_struct);printf("0 18\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara9",tmsg_struct);printf("0 19\n");
		ss_sendmsg(6, &p, sizeof(struct struct_t),"02chara10",tmsg_struct);printf("0 20\n");

		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara1",tmsg_struct);printf("0 21\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara2",tmsg_struct);printf("0 22\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara3",tmsg_struct);printf("0 23\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara4",tmsg_struct);printf("0 24\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara5",tmsg_struct);printf("0 25\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara6",tmsg_struct);printf("0 26\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara7",tmsg_struct);printf("0 27\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara8",tmsg_struct);printf("0 28\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara9",tmsg_struct);printf("0 29\n");
		ss_sendmsg(11, &p, sizeof(struct struct_t),"03chara10",tmsg_struct);printf("0 30\n");
//		ss_sendmsg(1, &p, sizeof(struct struct_t),"chara1",tmsg_struct);
//		ss_sendmsg(1, &int_test, sizeof(int),"chara2",tmsg_int);
//		printf("send a struct in noc_cycle %lld and core_cycle %lld \n", tmsg_struct->noccycle, tmsg_struct->cycle);
//		printf("send a int in noc_cycle %lld and core_cycle %lld \n", tmsg_int->noccycle, tmsg_int->cycle);

	}
	if(id == 6){
		struct struct_t* p2 = malloc(sizeof(struct struct_t));
		int i2;
		ss_msg_r *tmsg_int_recv, *tmsg_struct_recv;
		tmsg_int_recv = malloc(sizeof(int));
		tmsg_struct_recv = malloc(sizeof(ss_msg_r));
		while(ss_msgstatus(1,6,"02chara1") == nomsg || ss_msgstatus(1,6,"02chara1") == msgnoarrive){printf("2 1\n");}
		ss_recvmsg(1, "02chara1", p2, tmsg_struct_recv);

		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara2") == nomsg || ss_msgstatus(1,6,"02chara2") == msgnoarrive){printf("2 2\n");}
		ss_recvmsg(1, "02chara2", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara3") == nomsg || ss_msgstatus(1,6,"02chara3") == msgnoarrive){}
		ss_recvmsg(1, "02chara3", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara4") == nomsg || ss_msgstatus(1,6,"02chara4") == msgnoarrive){}
		ss_recvmsg(1, "02chara4", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara5") == nomsg || ss_msgstatus(1,6,"02chara5") == msgnoarrive){}
		ss_recvmsg(1, "02chara5", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara6") == nomsg || ss_msgstatus(1,6,"02chara6") == msgnoarrive){}
		ss_recvmsg(1, "02chara6", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara7") == nomsg || ss_msgstatus(1,6,"02chara7") == msgnoarrive){}
		ss_recvmsg(1, "02chara7", p2, tmsg_struct_recv);
		printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara8") == nomsg || ss_msgstatus(1,6,"02chara8") == msgnoarrive){}
		ss_recvmsg(1, "02chara8", p2, tmsg_struct_recv);
				printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara9") == nomsg || ss_msgstatus(1,6,"02chara9") == msgnoarrive){}
		ss_recvmsg(1, "02chara9", p2, tmsg_struct_recv);
				printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
												p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,6,"02chara10") == nomsg || ss_msgstatus(1,6,"02chara10") == msgnoarrive){}
		ss_recvmsg(1, "02chara10", p2, tmsg_struct_recv);
						printf("msg arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
														p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		//int
		while(ss_msgstatus(14,6,"32int1") == nomsg || ss_msgstatus(14,6,"32int1") == msgnoarrive){}
				ss_recvmsg(14, "32int1", &i2, tmsg_int_recv);
				printf("msg11 received, it's a int\n", i2);
		while(ss_msgstatus(14,6,"32int2") == nomsg || ss_msgstatus(14,6,"32int2") == msgnoarrive){}
				ss_recvmsg(14, "32int2", &i2, tmsg_int_recv);printf("msg12 received, it's a int\n", i2);
		while(ss_msgstatus(14,6,"32int3") == nomsg || ss_msgstatus(14,6,"32int3") == msgnoarrive){}
				ss_recvmsg(14, "32int1", &i2, tmsg_int_recv);printf("msg13 received, it's a int\n", i2);
		while(ss_msgstatus(14,6,"32int4") == nomsg || ss_msgstatus(14,6,"32int4") == msgnoarrive){}
				ss_recvmsg(14, "32int1", &i2, tmsg_int_recv);printf("msg14 received, it's a int\n", i2);
		while(ss_msgstatus(14,6,"32int5") == nomsg || ss_msgstatus(14,6,"32int5") == msgnoarrive){}
				ss_recvmsg(14, "32int5", &i2, tmsg_int_recv);printf("msg15 received, it's a int\n", i2);
		while(ss_msgstatus(14,6,"32int6") == nomsg || ss_msgstatus(14,6,"32int6") == msgnoarrive){}
				ss_recvmsg(14, "32int6", &i2, tmsg_int_recv);printf("msg16 received, it's a int\n", i2);
		while(ss_msgstatus(14,6,"32int7") == nomsg || ss_msgstatus(14,6,"32int7") == msgnoarrive){}
				ss_recvmsg(14, "32int7", &i2, tmsg_int_recv);printf("msg17 received, it's a int\n", i2);
		while(ss_msgstatus(14,6,"32int8") == nomsg || ss_msgstatus(14,6,"32int8") == msgnoarrive){}
				ss_recvmsg(14, "32int8", &i2, tmsg_int_recv);printf("msg18 received, it's a int\n", i2);
	}
	if(id==5){
		struct struct_t* p2 = malloc(sizeof(struct struct_t));
		ss_msg_r *tmsg_struct_recv;
		tmsg_struct_recv = malloc(sizeof(ss_msg_r));
		while(ss_msgstatus(1,5,"01chara1") == nomsg || ss_msgstatus(1,5,"01chara1") == msgnoarrive){printf("1 1\n");}
		ss_recvmsg(1, "01chara1", p2, tmsg_struct_recv);
		printf("msg1 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara2") == nomsg || ss_msgstatus(1,5,"01chara2") == msgnoarrive){printf("1 2\n");}
		ss_recvmsg(1, "01chara2", p2, tmsg_struct_recv);
		printf("msg2 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara3") == nomsg || ss_msgstatus(1,5,"01chara3") == msgnoarrive){}
		ss_recvmsg(1, "01chara3", p2, tmsg_struct_recv);
		printf("msg3 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara4") == nomsg || ss_msgstatus(1,5,"01chara4") == msgnoarrive){}
		ss_recvmsg(1, "01chara4", p2, tmsg_struct_recv);
		printf("msg4 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara5") == nomsg || ss_msgstatus(1,5,"01chara5") == msgnoarrive){}
		ss_recvmsg(1, "01chara5", p2, tmsg_struct_recv);
		printf("msg5 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara6") == nomsg || ss_msgstatus(1,5,"01chara6") == msgnoarrive){}
		ss_recvmsg(1, "01chara6", p2, tmsg_struct_recv);
		printf("msg6 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara7") == nomsg || ss_msgstatus(1,5,"01chara7") == msgnoarrive){}
		ss_recvmsg(1, "01chara7", p2, tmsg_struct_recv);
		printf("msg7 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
										p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara8") == nomsg || ss_msgstatus(1,5,"01chara8") == msgnoarrive){}
		ss_recvmsg(1, "01chara8", p2, tmsg_struct_recv);
				printf("msg8 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
												p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara9") == nomsg || ss_msgstatus(1,5,"01chara9") == msgnoarrive){}
		ss_recvmsg(1, "01chara9", p2, tmsg_struct_recv);
				printf("msg9 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
												p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);
		while(ss_msgstatus(1,5,"01chara10") == nomsg || ss_msgstatus(1,5,"01chara10") == msgnoarrive){}
		ss_recvmsg(1, "01chara10", p2, tmsg_struct_recv);
						printf("msg10 arrived, it's a struct, struct->a = %d and struct->ps = %s ,over_cycle_noc is %lld and over_core_cycle is %lld\n",
														p2->a, p2->ps, tmsg_struct_recv->over_cycle_noc, tmsg_struct_recv->over_cycle);

	}
	if(id==14){
		int int_test = 199;
		ss_msg_r* tmsg_int;
		ss_sendmsg(6, &int_test, sizeof(int),"32int1",tmsg_int);printf("3 1\n");
		ss_sendmsg(6, &int_test, sizeof(int),"32int2",tmsg_int);printf("3 2\n");
		ss_sendmsg(6, &int_test, sizeof(int),"32int3",tmsg_int);printf("3 3\n");
		ss_sendmsg(6, &int_test, sizeof(int),"32int4",tmsg_int);printf("3 4\n");
		ss_sendmsg(6, &int_test, sizeof(int),"32int5",tmsg_int);printf("3 5\n");
		ss_sendmsg(6, &int_test, sizeof(int),"32int6",tmsg_int);printf("3 6\n");
		ss_sendmsg(6, &int_test, sizeof(int),"32int7",tmsg_int);printf("3 7\n");
		ss_sendmsg(6, &int_test, sizeof(int),"32int8",tmsg_int);printf("3 8\n");

	}

//--------------------------


}



//		printf("sizeof short int  = %d\n",sizeof(short int));
//		printf("sizeof int  = %d\n",sizeof(int));
//		printf("sizeof long  = %d\n",sizeof(long int));
//		printf("sizeof long long  = %d\n",sizeof(long long int));
//		printf("sizeof tmsg = %d",sizeof(ss_msg_r));
//		printf("my actualid = %d, and initial msg size = %d\n",id,sizeof(msg));

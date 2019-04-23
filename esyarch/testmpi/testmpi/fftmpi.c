#include <stdio.h>

#include "../ssapi/ssmpi.h"
typedef struct data{
	int real;
	int imag;
}data_t;
void main(){
	int node_id = ss_getmyactualid();
	printf("node_id is %d\n",node_id);
	if(node_id == 7){
		int i, base;
		data_t d[128];
		data_t *t;
		ss_msg_r *tmsg_fft;
		ss_msg_r *tmsg_fft_recv;
		base = 5;
		t = malloc(128*sizeof(data_t));
		tmsg_fft = malloc(sizeof(ss_msg_r));
		tmsg_fft_recv = malloc(sizeof(ss_msg_r));
//		printf("base is %d\n",base);
		for(i=0;i<128;i++){
			d[i].real = base;
			d[i].imag = base+2;
//			printf("app: data %d 's real is %d, imag is %d\n", i, d[i].real, d[i].imag);
			base += 2;
		}
		//传送
		ss_sendmsg_f(2, d, 128*sizeof(data_t),"fft_id2",tmsg_fft);
		//接收
		while(ss_msgstatus_f(7,2,"fft_id2") != fpge_over){
			//printf("app in node 1 waiting for fft result\n");
		}
		ss_recvmsg_f(2, "fft_id2", t, tmsg_fft_recv);
//		printf("app:fft received\n");
//		for(i=0;i<128;i++){
//			printf("data from fft_asic: num.%d -- real: %d & imag: %d \n",i, t[i].real, t[i].imag);
//		}
	}
	if(node_id == 12){
		int i, base;
		data_t d[128];
		data_t *t;
		ss_msg_r *tmsg_fft;
		ss_msg_r *tmsg_fft_recv;
		base = 5;
		t = malloc(128*sizeof(data_t));
		tmsg_fft = malloc(sizeof(ss_msg_r));
		tmsg_fft_recv = malloc(sizeof(ss_msg_r));
//		printf("base is %d\n",base);
		for(i=0;i<128;i++){
			d[i].real = base;
			d[i].imag = base+2;
//			printf("app: data %d 's real is %d, imag is %d\n", i, d[i].real, d[i].imag);
			base += 2;
		}
		//传送
		ss_sendmsg_f(2, d, 128*sizeof(data_t),"fft_id2",tmsg_fft);
		//接收
		while(ss_msgstatus_f(12,2,"fft_id2") != fpge_over){
			//printf("app in node 5 waiting for fft result\n");
			}
		ss_recvmsg_f(2, "fft_id2", t, tmsg_fft_recv);
//		printf("app:node 2 fft received\n");
//		for(i=0;i<128;i++){
//			printf("data from fft_asic: num.%d -- real: %d & imag: %d \n",i, t[i].real, t[i].imag);
//		}
	}


}

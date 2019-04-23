/*
 * fpgac.cc
 *
 *  Created on: 2017年4月25日
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <vector>
#include "fpgac.h"
extern Noc glb_network;
//extern pthread_t p_fpga;

const static char* dev = "/dev/ttyUSB0";
int fd = open(dev,O_RDWR|O_NOCTTY);

Fpgac::Fpgac(FpgaStruct* fs, int threadid):TNode(1,fs),cycle(fs->cycle),t(fs->t),block(0),now_run(NULL),
		sim_cycle(0)
{
	ntype = fpgaasic;
}

void setT(struct termios *t, unsigned short speed)
{
		bzero(t,sizeof(struct termios));
		t->c_cflag=speed|CS8|CREAD|CLOCAL;
//		t->c_cflag &= ~PARENB;
		t->c_cflag &= ~CSTOPB;
		t->c_cflag &= ~PARODD;
		t->c_cflag &= ~CRTSCTS;
		t->c_iflag=IGNBRK|IGNPAR|IGNCR;
		t->c_iflag &= ~(IXON|IXOFF|IXANY|INLCR|ICRNL|IGNCR|INPCK|PARMRK);
		t->c_oflag &= ~(ONLCR|OCRNL);
		t->c_lflag &= ~(ICANON);;
		t->c_cc[VTIME]=0;
		t->c_cc[VMIN]=5;
}

void Fpgac::data_send(void* data, MPIMsg* msg){
	msg->fpga_status = fpga_comput;
	if(t == asic_fft_128){//128点fft的send
		send_asic_fft_128((fft_status*)data);
	}
}

void
Fpgac::send_asic_fft_128(fft_status* data){
	printf("data has been sent to fpga.\n");
		int i, core;
		fd_set fdr;
		int data_r, data_i;
		unsigned char buf[5];
		unsigned char opcode_base = 0x00;
		memcpy(&opcode_base,&(data->src),sizeof(char));
		unsigned char opcode_r = 0x80|opcode_base, opcode_i = 0x00|opcode_base;
		struct termios old, new1;
		setT(&new1,B115200);
		tcgetattr(fd,&old);
	   tcflush(fd,TCIFLUSH);
	   tcsetattr(fd,TCSANOW,&new1);
		core = data->src;
	//	if(core>1) opcode=opcode<<(core-1);
		for(i=0;i<128;i++){
			data_r = (data->source)[i].real;
			memcpy(buf,&data_r,sizeof(int));
			memcpy(buf+4,&opcode_r,1);
			write(fd,buf,5*sizeof(char));
	//		usleep(1000);
	//		printf("data in: r %d\n", data_r);
			data_i = (data->source)[i].imag;
			memcpy(buf,&data_i,sizeof(int));
			memcpy(buf+4,&opcode_i,1);
			write(fd,buf,5*sizeof(char));
	//		usleep(1000);
	//		printf("data in: i %d\n", data_i);
	//		printf("data %d 's real is: %d and",i, data_r);
	//		printf("imag is: %d\n", data_i);
		}
		//读
		int j;
		for(j=0;j<256;j++)
		{
			FD_ZERO(&fdr);
			FD_SET(fd,&fdr);
			struct timeval timeout;
			timeout.tv_sec=0;
			timeout.tv_usec=1;
			if(select(fd+1,&fdr,NULL,NULL,NULL))
			{
//				printf("select\n");
				if(FD_ISSET(fd,&fdr))
				{
					int tt1;
					unsigned char bufx[5];
					read(fd,&bufx,5*sizeof(char));
					memcpy(&tt1,&bufx,sizeof(float));
					char opcodex=bufx[4];
					char sig;
					sig = opcodex & 0x80;
					if(sig==0x80 && j<128){
						(data->target)[j].real = tt1;
//						printf("output: data %d 's real is %d\n", j, tt1);
					}
					else if(sig==0x00){
						(data->target)[j-128].imag = tt1;
//						printf("output: data %d 's imag is %d\n", j, tt1);
					}
				}
			}
		}
		now_run = data;
		block = cycle;

//		for(i=0;i<128;i++){
//			printf("output_end: data %d 's real is %d\n", i, (data->target)[i].real);
//			printf("output_end: data %d 's imag is %d\n", i, (data->target)[i].imag);
//		}
}

sim_rt_t
Fpgac::run(){
	sim_cycle++;
	//周期处理
	if(block){
//		printf("block:%d\n",block);
		block-=1;
		return NO_Q;
	}
	else{
		if(now_run != NULL){//send back
			printf("data received from fpga.\n");
			MPIMsg * mpimsg = new MPIMsg(actualid, now_run->src, 256*sizeof(int), now_run->msgchara, now_run->target, 1, fpga_returning);
			glb_network.InsertMsg(mpimsg, sim_cycle);
			now_run = NULL;
			return NO_Q;
		}
		vector<MPIMsg*>::iterator itor;
		for(itor=glb_network.MsgQueue.begin();itor != glb_network.MsgQueue.end();itor++){
			if((*itor)->type == 1 && (*itor)->des == actualid && (*itor)->fpga_status == fpga_waitcomput){//数据从网络到达
				//队列中取东西
				map<char *, void*>::iterator task_item;
				task_item = task_list.find((*itor)->character);
				if(task_item != task_list.end()){
					assert(((struct fft_status*)(task_item->second))->src == (*itor)->src);
					//发送到fpga计算
					data_send(task_item->second, (*itor));
					return NO_Q;
//					(*itor)->fpga_status = fpga_returning;
				}

			}
		}
	}
}

void fpga_read(){
	//数据从fpga内read回target
	int i;
	fd_set fdr;
	int counter=0;
	while(1)
	{
		FD_ZERO(&fdr);
		FD_SET(fd,&fdr);
		struct timeval timeout;
		timeout.tv_sec=0;
		timeout.tv_usec=1;
		if(select(fd+1,&fdr,NULL,NULL,&timeout))
		{
			if(FD_ISSET(fd,&fdr))
			{
				if(counter==256) {counter=0;}
				float tt1;
				unsigned char buf[5];
				read(fd,&buf,5*sizeof(char));
				memcpy(&tt1,&buf,sizeof(float));
				char opcode=buf[4];
				int core;char sig;
				unsigned char corechek=opcode&0x3f;
				switch(corechek)
				{
						case 0x01:core=1;break;
						case 0x02:core=2;break;
						case 0x04:core=3;break;
						case 0x08:core=4;break;
						case 0x10:core=5;break;
						case 0x20:core=6;break;
						default :core=0;
				}

			}
		}
	}
	//数据打包发送至
}




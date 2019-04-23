#ifndef __SIM_OUT_ORDER_H__
#define __SIM_OUT_ORDER_H__

#define	  SMD_USE_WRITE_BUF



#define GHB_size 512
#define DELTA_INDEX_TABLE_Size 8
#define TAG_INDEX_TABLE_Size        128
#define  NET_ALGR

#ifdef	EDA
void calculateWrongValue(struct RUU_station *rs);
struct	psn_struct_t
{
    int dirty;
    qword_t data;
};

#endif


#define	SMD_L1_WBUF_SIZE	8	//size of L1 write buffer, # of entries.
typedef struct  m_L1WBufEntry
{
    md_addr_t PC;
    md_inst_t IR;
    int lsqId;
    int robId;
    int robExId;
    md_addr_t addr;
    enum md_opcode op;		// decoded instruction opcode
    counter_t lat;
    counter_t sim_num_insn;
    int commitDone, writeStarted;
	int isMiss;
    counter_t finishTime, entryTime;
    quad_t writeValue;	
	int STL_C_fail;
	int already_check;

#ifdef DIRECTORY
    struct RUU_station *rs;
#endif

#ifdef	EDA
    struct psn_struct_t poison[2];
#endif
}m_L1WBufEntry;

m_L1WBufEntry m_L1WBuf[MAXTHREADS][SMD_L1_WBUF_SIZE]; //8 equals thread id..


	//int m_L1WBufCnt[MAXTHREADS];  //L1 write buffer count.
	int m_L1WBuf_head[MAXTHREADS];    //L1 write buffer head.
	int m_L1WBuf_tail[MAXTHREADS];    //L1 write buffer tail.
	
	int MarkWriteCommit(int threadid, struct RUU_station *rs);

	int m_L1WBufCnt[MAXTHREADS];  //L1 write buffer count.

#endif

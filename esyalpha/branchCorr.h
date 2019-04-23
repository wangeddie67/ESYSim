#ifndef __BRANCHCORR_H__
#define __BRANCHCORR_H__

#include "machine.h"
#include "host.h"
#include "misc.h"

#define brCorrSize 1024
#define realMaxSize 512
#define IND_JMP_SIZE    32
#define HIGHDRAGCOUNT 100
#define	B_SIZE_UPDT_ITRV	512

typedef struct
{
    int id;
    md_inst_t inst;
    enum md_opcode op;
    md_addr_t PC;
    md_addr_t NPC;
    counter_t time;
    counter_t sim_num_insn;
    int instType;
} branchInst;

typedef struct
{
    branchInst bI[brCorrSize];
    int boqSize;
    int corrNum; 	/* Number between Tail and current head */
    int totalNum;   	/* Number between Tail and head */
    int indBrNum;
    int corrHead;
    int corrCurrentHead;
    int corrTail;

    int bIntrvCount;
    counter_t lastUpdtCycle;
} branchCorr;


void 	initBranchCorr(branchCorr *pQ);
void 	resetQueue(branchCorr *pQ);
int 	pushBranchPred(branchCorr *pQ, branchInst **branch, enum md_opcode op);
int 	popBranchPred(branchCorr *pQ, branchInst **branch);
void 	commitPrediction(branchCorr *pQ, int index);
int 	queueSize(branchCorr *pQ);
void 	recoverPredQueue(branchCorr *pQ, int boqIndex);
#endif

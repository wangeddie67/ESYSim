#include "branchCorr.h"
#include "context.h"

#ifdef	EDA
extern int mem_lat[2];
extern int cache_dl2_lat;
extern int cache_dl1_lat;
extern counter_t sim_cycle;

void initBranchCorr(branchCorr *pQ)
{
    int j;
    pQ->boqSize = realMaxSize;
    pQ->corrHead = 0;
    pQ->corrCurrentHead = 0;
    pQ->corrTail = 0;
    pQ->corrNum = 0;
    pQ->totalNum = 0;
    pQ->indBrNum = 0;
    pQ->lastUpdtCycle = 0;

    for(j = 0; j < brCorrSize; j++) 
    {
        pQ->bI[j].id = j;
    }
}

void resetQueue(branchCorr *pQ)
{
    pQ->corrHead = 0;
    pQ->corrCurrentHead = 0;
    pQ->corrTail = 0;
    pQ->corrNum = 0;
    pQ->totalNum = 0;
    pQ->indBrNum = 0;
}

int pushBranchPred(branchCorr *pQ, branchInst **branch, enum md_opcode op)
{
    if(pQ->totalNum >= pQ->boqSize) 
    { 
        panic("Branch Prediction Queue is actually full. There would be loss of branches now. Need to increase the size of queue or extra toleration in queue.\n");
	return 0;
    }
    
    /* Queue is neither full, nor empty */
    *branch = &pQ->bI[pQ->corrTail];
    pQ->corrNum++;
    pQ->totalNum++;
    if((op == JMP) || (op == JSR) || (op == RETN) || (op == JSR_COROUTINE))
        pQ->indBrNum++;
	
    pQ->corrTail = (pQ->corrTail+1)%brCorrSize;
    return 1;
}

int popBranchPred(branchCorr *pQ, branchInst **branch)
{
    if(pQ->corrNum == 0) 
    { /* Queue is empty */
        *branch = NULL;
        return 0;
    }

    pQ->corrNum--;
    *branch = &pQ->bI[pQ->corrCurrentHead];
    pQ->corrCurrentHead = (pQ->corrCurrentHead+1)%brCorrSize;
    return 1;
}

void commitPrediction(branchCorr *pQ, int index)
{
    if(index != pQ->corrHead)
    {
        if(pQ->bI[index].instType == TYPE_INST_BARRIER || pQ->bI[index].instType == TYPE_LOCK_ACQUIRE)
            return;
        else
            panic("Prediction Indexing messed up");
    }
    else
    {
        enum md_opcode op = pQ->bI[index].op;
        if((op == JMP) || (op == JSR) || (op == RETN) || (op == JSR_COROUTINE))
            pQ->indBrNum--;
        pQ->corrHead = (pQ->corrHead+1)%brCorrSize;
        pQ->totalNum--;
    }
}

void dynConfBOQSize(branchCorr *pQ)
{
    if(pQ->bIntrvCount < B_SIZE_UPDT_ITRV)
        pQ->bIntrvCount++;
    else
    {
        int queueSize = (1*B_SIZE_UPDT_ITRV*(mem_lat[0]+cache_dl2_lat+cache_dl1_lat))/(sim_cycle-pQ->lastUpdtCycle);
        pQ->lastUpdtCycle = sim_cycle;
        pQ->bIntrvCount = 0;
        if(queueSize > realMaxSize)
            pQ->boqSize = realMaxSize;
        else if(queueSize > 10)
            pQ->boqSize = queueSize;
        else
            pQ->boqSize = 10;
    }
}

int queueSize(branchCorr *pQ)
{
    if(pQ->indBrNum >= IND_JMP_SIZE) return -1;
    if(pQ->totalNum >= pQ->boqSize) return -1;
    return pQ->totalNum;
}

void recoverPredQueue(branchCorr *pQ, int boqIndex)
{
    /* We have to adjust total number of branches available */
    boqIndex = (boqIndex+1)%brCorrSize;

    while (pQ->corrCurrentHead != boqIndex)
    {
        pQ->corrNum++;
        pQ->corrCurrentHead = (pQ->corrCurrentHead + brCorrSize - 1)%brCorrSize;
    }
}
#endif

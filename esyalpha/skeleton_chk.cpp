#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include "host.h"
#include "machine.h"
#include "misc.h"



using namespace std;

extern "C" void StartSkeletonRead (int type);
extern "C" int GetExecStatus (md_addr_t pc, md_addr_t *bPt);

extern "C" void PushMissPredLoad (md_addr_t pc, int latency);
extern "C" void CacheProfWrite (void);

extern "C" void insert_br_dir (md_addr_t pc, int dir, int missPred);
extern "C" void BranchProfWrite (void);

void AddStoreRef (md_addr_t PC, md_addr_t loadPC, counter_t commDist);
extern "C" void StoreProfWrite (void);

extern "C" void instExecutedFreq (md_addr_t pc);
extern "C" void FrequencyWrite (void);

#define CONFIG_NINS     (1024*768)

struct inst_stat
{
    md_addr_t PC;
    int freqCount;
    int depCount;
    struct inst_stat *next;
};

struct inst_stat *lastAccessedTargets;

/* To save arcs from store to communicating loads for store profile */
class statsEntry
{
public:
    md_addr_t pc;
    counter_t count;
    statsEntry *next;

    /* Constructor */
    statsEntry ()
    {
        pc = 0;
        count = 0;
        next = NULL;
    }

    statsEntry (md_addr_t pc_new)
    {
        pc = pc_new;
        count = 1;
        next = NULL;
    }

    /* Deconstructor */
    ~statsEntry ()
    {
    };

    void insertStat (md_addr_t pc_new)
    {
        if (pc == pc_new)
        {
            count++;
            return;
        }

        if (next == NULL)
        {
            next = new statsEntry (pc_new);
            return;
        }

        next->insertStat (pc_new);
        return;
    }

    void writeCommLoad (FILE * fp)
    {
        fprintf (fp, "PC: 0x%llx\tCount: %lld\t", pc, count);
        if (next != NULL)
            next->writeCommLoad (fp);
    }
};

class SkeletonChk
{
private:
    md_addr_t PC[CONFIG_NINS];	/* PC address for verification */
    int InsToExec[CONFIG_NINS];	/* Is 1, if instruction is to be executed */
    md_addr_t BoundaryPt[CONFIG_NINS];	/* Let us know the boundaries to be omitted */

    counter_t statsTable[CONFIG_NINS][14];	/* 14 entry stats table for instruction characterization  */

    /* Frequency profile */
    counter_t refCount[CONFIG_NINS];	/* Number of times this instruction executed: Frequency count */

    /* Store profile */
    counter_t storeRefCount[CONFIG_NINS];	/* Number of times this store is referenced by some load */
    counter_t store2LdDis[CONFIG_NINS];	/* Cummulative store to load distance */
    /*  Store communication arcs */
    statsEntry *stLdArcs[CONFIG_NINS];	/* Keep store to load arcs with count */

    /* Branch profile */
    counter_t notTakenBr[CONFIG_NINS];	/* Count of not taken branch */
    counter_t takenBr[CONFIG_NINS];	/* Count of taken branch */
    counter_t missPred[CONFIG_NINS];	/* Total Miss-predictions for this branch */

    /* Cache miss profile */
    counter_t missPredictCount[CONFIG_NINS];	/* Miss-prediction count */
    counter_t missPredictLat[CONFIG_NINS];	/* Cummulative miss-prediction latency */

    /* ITE profile or we also use it to keep loop target PC information */
    struct inst_stat *instStat[CONFIG_NINS];	/* Keeps the status of ITE instructions */

    int uIns;			/* Total number of static instructions */
    md_addr_t Start;			/* Address starting point for array indexing */

public:
    /* Constructor */
    SkeletonChk ()
    {
        uIns = 0;
        for (int i = 0; i < CONFIG_NINS; i++)
        {
            int ii;
            InsToExec[i] = 0;
            BoundaryPt[i] = 0;
            for (ii = 0; ii < 14; ii++)
                statsTable[i][ii] = 0;
            refCount[i] = 0;
            storeRefCount[i] = 0;
            store2LdDis[i] = 0;
            stLdArcs[i] = NULL;
            notTakenBr[i] = 0;
            takenBr[i] = 0;
            missPred[i] = 0;
            missPredictCount[i] = 0;
            missPredictLat[i] = 0;
            instStat[i] = NULL;
        }
    }

    /* Deconstructor */
    ~SkeletonChk ()
    {
    };

    /**********************************************************************/
    /* Frequency profile */
    void ExecFreq (md_addr_t pc)
    {
        int Index = (pc - Start) / 4;

        if (uIns > Index && PC[Index] == pc)
            refCount[Index]++;
    }

    /* Frequency Profile */
    void FrequencyWrite (FILE * fp)
    {
        extern md_addr_t max_insts;
        fwrite (&uIns, sizeof (int), 1, fp);
        fwrite (refCount, sizeof (counter_t), uIns, fp);
    }

    /**********************************************************************/
    /* Store profile */
    void AddStoreRef (md_addr_t pc, md_addr_t loadPC, counter_t commDist)
    {
        int Index = (int) (pc - Start) / 4;

        if (uIns > Index && PC[Index] == pc)
        {
            storeRefCount[Index]++;
            store2LdDis[Index] += commDist;
            if (stLdArcs[Index] == NULL)
                stLdArcs[Index] = new statsEntry (loadPC);
            else
                stLdArcs[Index]->insertStat (loadPC);
        }
    }

    void StoreProfWrite (FILE * fp)
    {
        for (int i = 0; i < uIns; i++)
        {
            if (storeRefCount[i] == 0)
                store2LdDis[i] = 2000000000;
            else
            {
                store2LdDis[i] = store2LdDis[i] / storeRefCount[i];
                fprintf (fp, "PC: 0x%llx\tStoreLdDist: %lld\tInstCount: %lld\tRefCount: %lld\t", PC[i], store2LdDis[i], refCount[i], storeRefCount[i]);
                if (stLdArcs[i] != NULL)
                    stLdArcs[i]->writeCommLoad (fp);
                fprintf (fp, "\n");
            }
        }
    }

    /**********************************************************************/
    /* Branch profile */
    void insertBrDir (md_addr_t pc, int dir, int isMissPred)
    {
        int Index = (int) (pc - Start) / 4;

        if (uIns > Index && PC[Index] == pc)
        {
            if (dir == 0)	/* Not taken branch */
                notTakenBr[Index]++;
            else		/* Taken branch */
                takenBr[Index]++;
            missPred[Index] += isMissPred;
        }
    }

    void BranchProfWrite (FILE * fp)
    {
        for (int i = 0; i < uIns; i++)
        {
            if (notTakenBr[i] != 0 || takenBr[i] != 0)
                fprintf (fp, "PC: 0x%llx\tNotTaken: %lld\tTaken Branch: %lld Total Missprediction %lld\n", PC[i], notTakenBr[i], takenBr[i], missPred[i]);
        }
    }

    /**********************************************************************/
    /* Cache miss profile */
    void PushMissPredLoad (md_addr_t pc, int latency)
    {
        int Index = (int) (pc - Start) / 4;

        if (uIns > Index && PC[Index] == pc)
        {
            missPredictCount[Index]++;
            missPredictLat[Index] += latency;
        }
    }

    void CacheProfWrite (FILE * fp)
    {
        for (int i = 0; i < uIns; i++)
        {
            if (missPredictCount[i] != 0)
                fprintf (fp, "PC: 0x%llx\tTotalMissLat: %lld\tMiss Count: %lld\n", PC[i], missPredictLat[i], missPredictCount[i]);
        }
    }

    /**********************************************************************/
    /* Read the skeleton strem constructed by ALTO from the file */
    void ReadSkeleton (FILE * fp, int type)
    {
        char line[20000];
        char *word;
        char *next;
        char *token;

        while (fgets (line, 20000, fp))
        {
            word = line;
            token = strtok_r (word, " ", &next);
            word = next;
            PC[uIns] = (md_addr_t) strtoll (token, &next, 0);

            token = strtok_r (word, "\t", &next);
            word = next;
            InsToExec[uIns] = (int) strtol (token, &next, 0);

            if (type == 1)
            {
                for (int i = 0; i < InsToExec[uIns]; i++)
                {
                    token = strtok_r (word, "\t", &next);
                    word = next;
                    md_addr_t targetPC = (md_addr_t) strtoll (token, &next, 0);

                    if (targetPC == 0)
                        fprintf (stderr, "Target PC can not be loaded zero\n");

                    if (instStat[uIns] == NULL)
                    {
                        md_addr_t tempMask = 1;

                        tempMask = (tempMask << 32);
                        instStat[uIns] = new struct inst_stat;

                        instStat[uIns]->PC = 0;
                        instStat[uIns]->PC = targetPC;
                        instStat[uIns]->PC = instStat[uIns]->PC | tempMask;
                        instStat[uIns]->next = NULL;
                    }
                    else
                    {
                        md_addr_t tempMask = 1;

                        tempMask = (tempMask << 32);
                        struct inst_stat *temp = new struct inst_stat;

                        temp->PC = 0;
                        temp->PC = targetPC;
                        temp->PC |= tempMask;
                        temp->next = NULL;
                        struct inst_stat *traverse = instStat[uIns];

                        while (traverse->next != NULL)
                            traverse = traverse->next;
                        traverse->next = temp;
                    }
                }
            }

            if (uIns == 0)
            {
                Start = PC[uIns];
            }
            else if (PC[uIns] != (md_addr_t) (Start + 4 * uIns))
            {
                fprintf (stderr, "Skeleton Read Contunity Error\n");
                exit (0);
            }
            uIns++;
        }
    }

    /* Return skeleton status of the instruction */
    int ExecStatus (md_addr_t pc, md_addr_t *bPt)
    {
        int Index = (pc - Start) / 4;

        if (uIns > Index && Index >= 0)
        {
            if (PC[Index] != pc)
            {
                //fprintf(stderr,"Not Found\n");
                return 0;
            }
            lastAccessedTargets = instStat[Index];
            if(bPt)
                *bPt = BoundaryPt[Index];
            return InsToExec[Index];
        }
        //fprintf(stderr,"Not Found\n");
        return 0;
    }
};

SkeletonChk skeleton;

void
StartSkeletonRead (int type)
{
    FILE *fp;
    extern char *sim_str_dump;
    char *sim_dump = sim_str_dump;
    char strFileName[256];

    if (!sim_dump)
    {
        fprintf (stderr, "No name for dump file\n");
        exit (1);
    }

    strcpy (strFileName, sim_dump);
    if (type == 0)
        strcat (strFileName, ".dis");
    else
        strcat (strFileName, ".bbl");	/* With basic block loop information */

    if ((fp = fopen (strFileName, "r")) == NULL)
    {
        fprintf (stderr, "Error opening file\n");
        exit (0);
    }
    skeleton.ReadSkeleton (fp, type);
    fclose (fp);
}

int
GetExecStatus (md_addr_t pc, md_addr_t *bPt)
{
    return skeleton.ExecStatus (pc, bPt);
}

/* Frequency Profile */
void
instExecutedFreq (md_addr_t pc)
{
    skeleton.ExecFreq (pc);
}

void
FrequencyWrite (void)
{
    FILE *fp;
    extern char *sim_str_dump;
    char *sim_dump = sim_str_dump;
    char strFileName[256];

    if (!sim_dump)
    {
        fprintf (stderr, "No name for dump file\n");
        exit (1);
    }

    strcpy (strFileName, sim_dump);
    strcat (strFileName, ".fpf");

    if ((fp = fopen (strFileName, "w")) == NULL)
    {
        fprintf (stderr, "Error opening file\n");
        exit (0);
    }
    skeleton.FrequencyWrite (fp);
    fclose (fp);
}

/* Store profile */
void
AddStoreRef (md_addr_t PC, md_addr_t loadPC, counter_t commDist)
{
    skeleton.AddStoreRef (PC, loadPC, commDist);
}

void
StoreProfWrite (void)
{
    FILE *fp;
    extern char *sim_str_dump;
    char *sim_dump = sim_str_dump;
    char strFileName[256];

    if (!sim_dump)
    {
        fprintf (stderr, "No name for dump file\n");
        exit (1);
    }

    strcpy (strFileName, sim_dump);
    strcat (strFileName, ".spf");

    if ((fp = fopen (strFileName, "w")) == NULL)
    {
        fprintf (stderr, "Error opening file\n");
        exit (0);
    }
    skeleton.StoreProfWrite (fp);
    fclose (fp);
}

/* Branch profile */
void
insert_br_dir (md_addr_t pc, int dir, int missPred)
{
    skeleton.insertBrDir (pc, dir, missPred);
}

void
BranchProfWrite (void)
{
    FILE *fp;
    extern char *sim_str_dump;
    char *sim_dump = sim_str_dump;
    char strFileName[256];

    if (!sim_dump)
    {
        fprintf (stderr, "No name for dump file\n");
        exit (1);
    }

    strcpy (strFileName, sim_dump);
    strcat (strFileName, ".bpf");

    if ((fp = fopen (strFileName, "w")) == NULL)
    {
        fprintf (stderr, "Error opening file\n");
        exit (0);
    }
    skeleton.BranchProfWrite (fp);
    fclose (fp);
}

/* Cache profile */
void
PushMissPredLoad (md_addr_t pc, int latency)
{
    skeleton.PushMissPredLoad (pc, latency);
}

void
CacheProfWrite (void)
{
    FILE *fp;
    extern char *sim_str_dump;
    char *sim_dump = sim_str_dump;
    char strFileName[256];

    if (!sim_dump)
    {
        fprintf (stderr, "No name for dump file\n");
        exit (1);
    }

    strcpy (strFileName, sim_dump);
    strcat (strFileName, ".cpf");

    if ((fp = fopen (strFileName, "w")) == NULL)
    {
        fprintf (stderr, "Error opening file\n");
        exit (0);
    }
    skeleton.CacheProfWrite (fp);
    fclose (fp);
}


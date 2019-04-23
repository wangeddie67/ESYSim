#ifndef MTA_H
#define MTA_H
#define R_BMAP_SZ       (BITMAP_SIZE(MD_NUM_IREGS))
#define F_BMAP_SZ       (BITMAP_SIZE(MD_NUM_FREGS))
#define C_BMAP_SZ       (BITMAP_SIZE(MD_NUM_CREGS))


#define JUSTFASTFWD 0

//#define DO_COMPLETE_FASTFWD

extern void StartSkeletonRead (int type);
extern int GetExecStatus (md_addr_t pc, md_addr_t *bPt);

#ifdef	DO_COMPLETE_FASTFWD
    #define CACHE_PROFILE
    #define BRANCH_PROFILE
    #define STORE_PROFILE
    #define INDIRECTJUMP_PROFILE
    #define FREQ_PROFILE

extern void instExecutedFreq (md_addr_t pc);
extern void FrequencyWrite (void);

extern void PushMissPredLoad (md_addr_t pc, int latency);
extern void CacheProfWrite (void);

extern void insert_br_dir (md_addr_t pc, int dir, int missPred);
extern void BranchProfWrite (void);

extern void pushStore (md_addr_t pc, md_addr_t addr, counter_t sim_cycle, int width);
extern void verifyLoad (md_addr_t pc, md_addr_t addr, counter_t sim_cycle, int width);
extern void StoreProfWrite (void);

extern void pushJump (md_addr_t pc, md_addr_t next_pc);
extern void printJumpStats ();

#endif


#define REMOVE_ILP 1
#define DO_DYNAMIC_ADAPTATION 0


//#define PROCESS_MODEL 1

//#define TESTCACHE 0
#define MTA

struct quiesceStruct {
    md_addr_t address;
    int threadid;
};
#endif



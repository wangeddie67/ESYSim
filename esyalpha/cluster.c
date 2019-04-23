#include "headers.h"

/* cycle counter */
extern counter_t sim_cycle;

/* register update unit (RUU) size */
extern int RUU_size;

/* load/store queue (LSQ) size */
extern int LSQ_size;

/* functional unit resource pool */
extern struct res_pool *fu_pool;

/* l1 data cache hit latency (in cycles) */
extern int cache_dl1_lat;

/* level 1 instruction cache, entry level instruction cache */
extern struct cache_t *cache_il1;

/* level 2 instruction cache */
extern struct cache_t *cache_il2;

/* level 1 data cache, entry level data cache */
extern struct cache_t *cache_dl1;

/* level 2 data cache */
extern struct cache_t *cache_dl2;

/* total number of memory system ports available  */
extern int res_memport;

extern double min_addr;
extern long max_addr;

extern double stddev;
extern int ban0;
extern int ban1;
extern int ban2;
extern int ban3;
extern long bin_acc_tot;
extern long bin_mis_tot;
extern long bin_acc[(MAXADDR - MINADDR) / BINSIZE];
extern long bin_lds[(MAXADDR - MINADDR) / BINSIZE];
extern long bin_tol[(MAXADDR - MINADDR) / BINSIZE];
extern long bin_mis[(MAXADDR - MINADDR) / BINSIZE];

extern long addr0, addr1, addr2;

extern long insn16;
extern long insn4;

extern double non_crit;
extern double crit;
extern double tab2_corr;
extern double tab2_incorr;
extern double tab2_miss;
extern double tab_corr;
extern double tab_incorr;
extern double tab_miss;
extern long table_tags[MAXTHREADS][TABLESIZE];
extern int table_num[MAXTHREADS][TABLESIZE];
extern long table2_tags[MAXTHREADS][TABLE2SIZE];
extern int table2_num[MAXTHREADS][TABLE2SIZE];


extern int original_bsize;

extern int iextra;
extern int fextra;
extern int ivalid;
extern int fvalid;

extern double tot_ireg;
extern double tot_freg;
extern double tot_iext;
extern double tot_fext;
extern double tot_inondup;
extern double tot_fnondup;

extern double comm_delay;
extern double num_single;
extern double iss_stall;
extern double fe_stall;
extern double n_ready;
extern double num_semi_f;
extern double num_qwstall;
extern double in_this;
extern double num_freed;
extern double tot_del;
extern double tot_del_bw;
extern double num_bw_of;
extern double rrobin;

extern long int_distilpthresh;
extern int distilptable[ITABLESIZE];
extern struct tag_id_t distilptabletagid[ITABLESIZE];
extern long distilpcounter[ITABLESIZE];
extern long distilpsamples[ITABLESIZE];
extern int distilpcount;
extern long int_distilp;


#ifdef  MSHR
extern int mshr_size;
extern int mshr_full;
extern double mshr_count;
extern double mshr_size_dist[];
extern double mshr_full_count;
#endif


/*092004:mwr*/

#ifdef  FETCH_QUEUE_CLUSTER
extern int fetch_num_cl[CLUSTERS];
#endif //FETCH_QUEUE_CLUSTER

#ifdef DISPATCH_CLUSTER
extern int dispatch_cl[CLUSTERS];
#endif //DISPATCH_CLUSTER






/*  stat. for the issue queues and regester files  Ali-SMT*/
extern double IISSUEQ_count;
extern double IISSUEQ_fcount;
extern double FISSUEQ_count;
extern double FISSUEQ_fcount;
extern double IPREG_count;
extern double Iextra_count;
extern double IPREG_fcount;
extern double FPREG_count;
extern double Fextra_count;
extern double FPREG_fcount;

extern double iregfile_cl_count[];
extern double iextra_cl_count[];
extern double fregfile_cl_count[];
extern double fextra_cl_count[];
extern double iissueq_cl_count[];
extern double fissueq_cl_count[];

#ifdef  SPLIT_LSQ
extern double lsq_cl_count[];
#endif

extern double no_disp_cluster;
extern double disp_dist[];

#ifdef RAW
extern double rawq_count;
extern double rawq_fcount;
extern double rawq_cl_count[];
extern double rawq_dist[];
#endif

extern struct lreg_id_t ptolmap[];
extern struct lreg_id_t consumers[];
extern struct lreg_id_t ptolmap_chk[];
extern int p_total_regs;

#ifdef RABMAP
extern int inst_use_extra[];
extern int ltopmap_extra[MAXTHREADS][MD_TOTAL_REGS][CLUSTERS];
extern int ltopmap_preg[MAXTHREADS][MD_TOTAL_REGS][CLUSTERS];
#endif

#ifdef RAW
extern int rawq;
extern int rawq_cl[];
extern int rawqready;
extern int rawqready_cl[];	// Number of ready entries in the RAW queues
extern int rawq_full;
extern double rawq_stall;
#endif
extern int iissueq;		/* Sizes of the issueq */
extern int fissueq;

#ifdef PWR_WAKE
extern int iready;		// Number of ready entries in the issue queues
extern int fready;
extern int lsqready;
extern int iunready;		// Number of ready entries in the issue queues
extern int funready;
extern int lsqunready;
extern double IREADY_count;
extern double FREADY_count;
extern double LSQREADY_count;
extern int iready_cl[];		// Number of ready entries in the issue queues
extern int fready_cl[];
extern int lsqready_cl[];
extern int iunready_cl[];	// Number of ready entries in the issue queues
extern int funready_cl[];
#endif

#ifdef  SPLIT_LSQ
extern int lsq_cl[];
extern int lsqunready_cl[];
#endif

extern int iregfile;		/* Sizes of the physical register files */
extern int fregfile;
extern int iregfile_cl[];
extern int iextra_cl[];
extern int fregfile_cl[];
extern int fextra_cl[];
extern int iissueq_cl[];
extern int fissueq_cl[];
extern int num_storeq[];
extern int max_storeq[];
extern int disp_delay;

#ifdef  PWR_CLUSTER
extern counter_t iqram_access_cl[];
extern counter_t fqram_access_cl[];
extern counter_t iwakeup_access_cl[];
extern counter_t fwakeup_access_cl[];
extern counter_t iselection_access_cl[];
extern counter_t fselection_access_cl[];
extern counter_t iregfile_access_cl[];
extern counter_t fregfile_access_cl[];
extern counter_t ialu1_access_cl[];
extern counter_t ialu2_access_cl[];
extern counter_t falu1_access_cl[];
extern counter_t falu2_access_cl[];
extern counter_t iresultbus_access_cl[];
extern counter_t fresultbus_access_cl[];
#else
extern counter_t iqram_access;
extern counter_t fqram_access;
extern counter_t iwakeup_access;
extern counter_t fwakeup_access;
extern counter_t iselection_access;
extern counter_t fselection_access;
extern counter_t iregfile_access;
extern counter_t fregfile_access;
extern counter_t ialu1_access;
extern counter_t ialu2_access;
extern counter_t falu1_access;
extern counter_t falu2_access;
extern counter_t iresultbus_access;
extern counter_t fresultbus_access;
#endif

#ifdef  RAW
extern counter_t rawqram_access_cl[];
extern counter_t rawqwakeup_access_cl[];
#endif
#ifndef NEW_INTERCONNECT
#ifdef  COMM_BUFF
extern counter_t comm1qram_access_cl[];
extern counter_t comm2qram_access_cl[];
#endif
#ifdef  COMM_LINK_PWR
extern counter_t comm1link_access_cl[];
extern counter_t comm2link_access_cl[];
#endif
#endif

#ifdef CACHE_MISS_STAT
extern int spec_rdb_miss;
extern int spec_wrb_miss;
extern int count_error;
#endif


#ifdef IFREELIST_FREELIST
struct listelt *ifreelist_freelist;

#define IFREELIST_FREE(temp) \
      temp->next = ifreelist_freelist; \
      ifreelist_freelist = temp;
#define IFREELIST_GET(DST) \
  { struct listelt *ifree_l;						\
    if (!ifreelist_freelist)						\
    { \
	int w; \
	ifreelist_freelist = malloc(10*sizeof(struct listelt)); \
	for( w = 0; w<9; w++) \
	    ifreelist_freelist[w].next = &ifreelist_freelist[w+1]; \
	ifreelist_freelist[9].next = NULL;	\
    } \
    ifree_l = ifreelist_freelist; \
    ifreelist_freelist = ifreelist_freelist->next; \
    ifree_l->next = NULL; /* mostly about debugging, but */ \
    (DST) = ifree_l; 	\
  }

#endif

extern int shiftreg1[MAXTHREADS][LEV1SIZE];
extern int specshiftreg1[MAXTHREADS][LEV1SIZE];
extern int shiftreg2[MAXTHREADS][LEV1SIZE];
extern int specshiftreg2[MAXTHREADS][LEV1SIZE];
extern int shiftreg3[MAXTHREADS][LEV1SIZE];
extern int specshiftreg3[MAXTHREADS][LEV1SIZE];
extern int shiftreg4[MAXTHREADS][LEV1SIZE];
extern int specshiftreg4[MAXTHREADS][LEV1SIZE];
extern int shiftreg5[MAXTHREADS][LEV1SIZE];
extern int specshiftreg5[MAXTHREADS][LEV1SIZE];
extern int lev2pred1[];
extern int lev2pred2[];
extern int lev2pred3[];
extern int lev2pred4[];
extern int lev2pred5[];


extern struct RUU_station *LSQ[];	/* load/store queue */
extern int LSQ_head, LSQ_tail;	/* LSQ head and tail pointers */
extern int LSQ_num;		/* num entries currently in LSQ */

#ifdef  COUNT_LSQ
extern int non_issue_LSQ_num;
#endif

extern int IIQ_size;
extern int FIQ_size;
extern int IPREG_size;
extern int FPREG_size;

extern int ring1[CLUSTERS][TIMEWINDOW];
extern int ring2[CLUSTERS][TIMEWINDOW];
extern int ring3[CLUSTERS][TIMEWINDOW];
extern int ring4[CLUSTERS][TIMEWINDOW];


extern int actual_clusters;

#ifdef PORT_PER_BANK
extern int CACHEPORTS;
extern int RES_CACHEPORTS;
extern int RES_CACHE_PORTS;
extern int actual_cacheports;
#else
extern int actual_cacheports;
#endif

#ifdef RAW
extern int rawq_size;
#endif
#ifdef  COMM_BUFF
extern int timewindow;
#endif

/*  metric list for measuring the priority, updated with icount_thrd for ICOUNT scheme */
extern int key[];

#ifdef INTERVAL_BASED
extern double interval_key[MAXTHREADS];
#endif

//extern int fetch_num;                 /* num entries in IF -> DIS queue */

extern double flushes;
extern double num_flushes;
extern double free_cluster;


#ifdef N_WAY_STEER
extern int steer_cluster_thrd[MAXTHREADS][CLUSTERS];
extern int n_limit_thrd[];
extern int n_start_thrd[];
#endif // N_WAY_STEER

#ifdef  THRD_WAY_CACHE
extern int n_cache_start_thrd[];
extern int n_cache_limit_thrd[];
#endif

#ifdef IC_STAT
double fetch_num_thrd_count[MAXTHREADS];
double iissueq_thrd_count[MAXTHREADS];
double fissueq_thrd_count[MAXTHREADS];
double lsq_thrd_count[MAXTHREADS];
double non_issue_lsq_thrd_count[MAXTHREADS];
double icount_thrd_count[MAXTHREADS];
#endif



/*092004:mwr*/

#ifdef  PARTITIONED_LSQ
extern int lsq_part_bank;
extern int lsq_part_cl;
extern int lsq_part_thrd;
#endif







extern int activecontexts;
extern int res_membank;

/* Function prototype   */
void calculate_priority ();
int get_cluster (int, int, int, struct RUU_station *, int);
int simple_cache_flush (struct cache_t *);
unsigned int			/* latency of block access */
  dl1_access_fn (enum mem_cmd,	/* access cmd, Read or Write */
		 md_addr_t,	/* block address to access */
		 int,		/* size of block to access */
		 struct cache_blk_t *,	/* ptr to block in upper level */
		 tick_t,	/* time of access */
		 int
#if defined (L2_COMM_LAT) || defined (TOKENB)
		 , int cluster	/* the cluster who owns this cache */
#endif
    );


int
cluster_check (md_addr_t addr)	// Ali : not working
{
    if ((long) addr < addr0)
    {
	return 0;
    }
    else if ((long) addr < addr1)
    {
	return 1;
    }
    else if ((long) addr < addr2)
    {
	return 2;
    }
    else
    {
	return 3;
    }
}

/* Various functions to simulate the caches in greater detail - Ronz */

struct fillq
{				/* This queue keeps track of when cache fills return,
				   so that cache port contention can be modeled */
    long when;
    md_addr_t addr;
#ifdef   THRD_WAY_CACHE
    int threadid;
#endif
    struct fillq *next;
    struct fillq *prev;
};

struct fillq *tailfill;
struct fillq *topfill;
int fillqsize;

void
fillq_init ()
{
    tailfill = NULL;

    topfill = NULL;
    fillqsize = 0;
}

#ifdef   THRD_WAY_CACHE
void
insert_fillq (long when, md_addr_t addr, int threadid)
#else
void
insert_fillq (long when, md_addr_t addr)	/* When a cache discovers a miss,
						   it calls this function to designate when the
						   miss is going to return */
#endif
{
    /* The fillq is sorted by time, for quick retrieval. Hence, traverse
       the list till the correct insertion point is found. */

    struct fillq *p, *q;
    int found;

    fillqsize++;
    p = (struct fillq *) malloc (sizeof (struct fillq));
    p->when = when;
    p->addr = addr;
#ifdef   THRD_WAY_CACHE
    p->threadid = threadid;
#endif
    if (!topfill)
    {
	topfill = p;
	p->next = NULL;
	p->prev = NULL;
	tailfill = p;		// First Entry: top=head=p & next=prev=Null
	return;
    }

    q = tailfill;		// Start searching from the tail (most recent)
    found = 0;

    while (!found)
    {
	if (q->when < when)	// Search for Entry
	{			// The entry is more recent than current-> will put it below it (at its next)
	    found = 1;
	    continue;
	}
	if (!q->prev)		// Reached topfill (topfill->prev = NULL)
	{
	    found = 1;		// Actually not found ! will check for pointer after exit
	    continue;
	}
	q = q->prev;
    }

    if (q->when < when)		// Actually found.
    {
	if (q->next)		// Not at the tail -- Insert in the middle
	{
	    q->next->prev = p;
	    p->next = q->next;
	    p->prev = q;
	    q->next = p;
	}
	else			// Make it the new tail.
	{
	    tailfill = p;
	    q->next = p;
	    p->prev = q;
	    p->next = NULL;
	}
    }				//found
    else			// Not found, already reached the top: Make it new top
    {
	q->prev = p;
	p->next = q;
	p->prev = NULL;
	topfill = p;
    }

    return;
}

void
bin_init ()
{
    int i;

    for (i = 0; i < (MAXADDR - MINADDR) / BINSIZE; i++)
    {
	bin_acc[i] = 0;
	bin_lds[i] = 0;
	bin_tol[i] = 0;
	bin_mis[i] = 0;
    }
    bin_acc_tot = 0;
    bin_mis_tot = 0;
}

void
cache_init ()			/* Initializes the writeback buffer sizes and
				   the bus delays for each cache structure */
{
    cache_dl1->wrbufsize = DL1WRB;
    cache_dl1->pipedelay = DL1PIPE;
    cache_il1->wrbufsize = IL1WRB;
    cache_il1->pipedelay = IL1PIPE;
    cache_dl2->wrbufsize = DL2WRB;
    cache_dl2->pipedelay = DL2PIPE;
    fillq_init ();
#ifdef EVENT_CACHE_UPDATE
    mshr_init ();
#endif
}

#ifdef INST_TRANS_RATE
#ifndef NEW_INTERCONNECT
double *link_width_dist;
#endif
#endif
#ifndef NEW_INTERCONNECT
int ring1[CLUSTERS][TIMEWINDOW];
int ring2[CLUSTERS][TIMEWINDOW];
int ring3[CLUSTERS][TIMEWINDOW];
int ring4[CLUSTERS][TIMEWINDOW];

#ifdef TRACE_REG
int *reg1[CLUSTERS][TIMEWINDOW];
int *reg2[CLUSTERS][TIMEWINDOW];
#else
int reg1[CLUSTERS][TIMEWINDOW];
int reg2[CLUSTERS][TIMEWINDOW];
#endif
int reg3[CLUSTERS][TIMEWINDOW];
int reg4[CLUSTERS][TIMEWINDOW];

void
cluster_init (void)
{
    int i, j;

#ifdef INST_TRANS_RATE
#ifndef NEW_INTERCONNECT
    link_width_dist = malloc (sizeof (double) * link_width);
#endif
#endif
#ifdef TRACE_REG
    for (i = 0; i < CLUSTERS; i++)
    {
	for (j = 0; j < TIMEWINDOW; j++)
	{
	    reg1[i][j] = malloc (link_width * sizeof (int));
	    reg2[i][j] = malloc (link_width * sizeof (int));
	}
    }
#endif

}

void
cluster_uninit (void)
{
    int i, j;

#ifdef INST_TRANS_RATE
#ifndef NEW_INTERCONNECT
    free (link_width_dist);
#endif
#endif
#ifdef TRACE_REG
    for (i = 0; i < CLUSTERS; i++)
    {
	for (j = 0; j < TIMEWINDOW; j++)
	{
	    free (reg1[i][j]);
	    free (reg2[i][j]);
	}
    }
#endif
}
#endif /*New Interconnect */
#ifdef EVENT_CACHE_UPDATE

void
mshr_init ()
{
    cache_dl1->mshr_queue = NULL;
    cache_il1->mshr_queue = NULL;
    cache_dl2->mshr_queue = NULL;
}

int
insert_mshr (struct cache_t *cp, int rd_wt, long when, md_addr_t addr, int pipedelay)	/* When a cache discovers a miss,
											   it calls this function to designate when the
											   miss is going to return */
{
    /* The fillq is sorted by time, for quick retrieval. Hence, traverse
       the list till the correct insertion point is found. */

    struct mshrq *p, *q;
    int found;
    int order = 0;
    int order_cont = 0;
    int first_cont_when = when;

    p = (struct mshrq *) malloc (sizeof (struct mshrq));
    p->when = when;
    p->rd_wt = rd_wt;
    p->addr = addr;
    if (!cp->mshr_queue)
    {
	cp->mshr_queue = p;
	p->next = NULL;
	p->prev = NULL;
	return order;
    }
    q = cp->mshr_queue;
    found = 0;
    while (!found)
    {
	if (q->when > when)
	{
	    found = 1;
	    continue;
	}
	if (!q->next)
	{
	    found = 1;
	    order++;
	    if (q->when + pipedelay >= when)
	    {
		if (!order_cont)
		    first_cont_when = q->when;
		order_cont++;
	    }
	    continue;
	}
	if (q->when + pipedelay >= when)
	{
	    if (!order_cont)
		first_cont_when = q->when;
	    order_cont++;
	}
	q = q->next;
	order++;
    }
    if (q->when > when)
    {
	if (q->prev)
	{
	    q->prev->next = p;
	    p->prev = q->prev;
	    p->next = q;
	    q->prev = p;
	}
	else
	{
	    cp->mshr_queue = p;
	    q->prev = p;
	    p->next = q;
	    p->prev = NULL;
	}
    }
    else
    {
	q->next = p;
	p->prev = q;
	p->next = NULL;
    }
#ifdef  APPROXIMATE_LAT
    return order;
#endif
#ifdef  ACURATE_LAT
    return order_cont;
#endif
#ifdef  MOST_ACURATE_LAT
    return order_cont * pipedelay - (when - first_cont_when);
#endif
}

#endif

#ifdef EVENT_CACHE_UPDATE

void
cache_update ()			/* Model contention for the L1D. If there was a
				   cache fill in this cycle, mark the ports as busy.
				   Also, drain out the read and writeback buffers
				   for L1D, L1I, and L2. */
{
    struct mshrq *mp, *mq;

    struct fillq *p, *q;
    int i;

#ifdef   THRD_WAY_CACHE
    int bank_per_thread, thrd_bank_start, thrd_word_bank, threadid;
#endif
    /*Ali: 01/28/2003  this part of code we leave it as in original cache contention version */
    /* Mark the write ports as busy if there was a cache-line fill this cycle */
    p = topfill;
    while (p && (p->when < sim_cycle))
    {
	q = p;
	p = p->next;
	if (p)
	    p->prev = NULL;
	fillqsize--;
	topfill = p;
#ifndef  NO_QFILL_CONTENTION
#ifdef   LINE_INTERLEAVED
#ifdef   THRD_WAY_CACHE
	threadid = q->threadid;
	bank_per_thread = res_membank / activecontexts + (res_membank % activecontexts ? 1 : 0);
	thrd_bank_start = bank_per_thread * threadid;
	thrd_word_bank = (CACHE_SET (cache_dl1, q->addr) % n_cache_limit_thrd[threadid] + n_cache_start_thrd[threadid]) % res_membank;
	i = thrd_word_bank;
#else //  THRD_WAY_CACHE
	i = (q->addr >> cache_dl1->set_shift) & (res_membank - 1);
#endif //  THRD_WAY_CACHE
	fu_pool->table[10][i]->master->busy += fu_pool->table[10][i]->issuelat;
#else //  LINE_INTERLEAVED
	/* Mark the write ports busy for a while */
	if (WORD_INTERLEAVED)
	{
	    for (i = 0; i < res_memport; i++)
		fu_pool->table[10][i]->master->busy += fu_pool->table[10][i]->issuelat;
	}
	else
	{
	    i = cluster_check (q->addr);	// Ali : not working
	    fu_pool->table[10][i]->master->busy += fu_pool->table[10][i]->issuelat;
	}
#endif //  LINE_INTERLEAVED
#endif //  NO_QFILL_CONTENTION
	free (q);
    }
/*end  this part of code we leave it as in original cache contention version */

    mp = cache_dl1->mshr_queue;
    while (mp && (mp->when < sim_cycle) && ((sim_cycle - cache_dl1->lastuse) >= DL1PIPE))
    {
	cache_dl1->lastuse = sim_cycle;
	if (mp->rd_wt == 1)
	    cache_dl1->rdb--;
	if (mp->rd_wt == 2)
	    cache_dl1->wrb--;
	mq = mp;
	mp = mp->next;
	if (mp)
	    mp->prev = NULL;
	cache_dl1->mshr_queue = mp;
	free (mq);
    }

    mp = cache_il1->mshr_queue;
    while (mp && (mp->when < sim_cycle) && ((sim_cycle - cache_il1->lastuse) >= IL1PIPE))
    {
	cache_il1->lastuse = sim_cycle;
	if (mp->rd_wt == 1)
	    cache_il1->rdb--;
	if (mp->rd_wt == 2)
	    cache_il1->wrb--;
	mq = mp;
	mp = mp->next;
	if (mp)
	    mp->prev = NULL;
	cache_il1->mshr_queue = mp;
	free (mq);
    }

    mp = cache_dl2->mshr_queue;
    while (mp && (mp->when < sim_cycle) && ((sim_cycle - cache_dl2->lastuse) >= DL2PIPE))
    {
	cache_dl2->lastuse = sim_cycle;
	if (mp->rd_wt == 1)
	    cache_dl2->rdb--;
	if (mp->rd_wt == 2)
	    cache_dl2->wrb--;
	mq = mp;
	mp = mp->next;
	if (mp)
	    mp->prev = NULL;
	cache_dl2->mshr_queue = mp;
	free (mq);
    }

#ifdef  MSHR
    mshr_size = cache_dl1->rdb + cache_dl2->rdb;
    if (mshr_size >= MSHRSIZE)
	mshr_full = 1;
    else
	mshr_full = 0;
    mshr_count += mshr_size;
    if (mshr_full)
    {
#ifdef  MSHR_DIST
	mshr_size_dist[MSHRSIZE - 1]++;
#endif
	mshr_full_count++;
    }
#ifdef  MSHR_DIST
    else
	mshr_size_dist[mshr_size]++;
#endif
#endif

//  fprintf(stderr,"C %d L1 r%d w%d L2 r%d w%d\n",sim_cycle, cache_dl1->rdb, cache_dl1->wrb, cache_dl2->rdb, cache_dl2->wrb);
}

#else

void
cache_update ()			/* Model contention for the L1D. If there was a
				   cache fill in this cycle, mark the ports as busy.
				   Also, drain out the read and writeback buffers
				   for L1D, L1I, and L2. */
{
    struct fillq *p, *q;
    int i;
printf("I am coming here!\n");
#ifdef   THRD_WAY_CACHE
    int bank_per_thread, thrd_bank_start, thrd_word_bank, threadid;
#endif
    /* Mark the write ports as busy if there was a cache-line fill this cycle */
    p = topfill;
    while (p && (p->when < sim_cycle))
    {
	q = p;
	p = p->next;
	if (p)
	    p->prev = NULL;
	fillqsize--;
	topfill = p;
#ifndef  NO_QFILL_CONTENTION
#ifdef   LINE_INTERLEAVED
#ifdef   THRD_WAY_CACHE
	threadid = q->threadid;
	bank_per_thread = res_membank / activecontexts + (res_membank % activecontexts ? 1 : 0);
	thrd_bank_start = bank_per_thread * threadid;
	thrd_word_bank = (CACHE_SET (cache_dl1, q->addr) % n_cache_limit_thrd[threadid] + n_cache_start_thrd[threadid]) % res_membank;
	i = thrd_word_bank;
#else //  THRD_WAY_CACHE
	i = (q->addr >> cache_dl1->set_shift) & (res_membank - 1);
#endif //  THRD_WAY_CACHE
	fu_pool->table[10][i]->master->busy += fu_pool->table[10][i]->issuelat;
#else //  LINE_INTERLEAVED
	/* Mark the write ports busy for a while */
	if (WORD_INTERLEAVED)
	{
	    for (i = 0; i < res_memport; i++)
		fu_pool->table[10][i]->master->busy += fu_pool->table[10][i]->issuelat;
	}
	else
	{
	    i = cluster_check (q->addr);	// Ali : not working
	    fu_pool->table[10][i]->master->busy += fu_pool->table[10][i]->issuelat;
	}
#endif //  LINE_INTERLEAVED
#endif //  NO_QFILL_CONTENTION
	free (q);
    }

    /* Drain out the read and write buffers for L1D, L1I and L2 */
    /* lastuse specifies the last time the bus was accessed. If that bus
       access is now over, initiate the next one. */
    if ((sim_cycle - cache_dl1->lastuse) >= DL1PIPE)
    {
	if ((cache_dl1->wrb - cache_dl1->wrbufsize) > DL1WRB)
	{
	    /* Drain out the write buffer only if it is full. Else, wait for
	       a period of lull before draining it out. */
	    cache_dl1->wrb--;
	    cache_dl1->lastuse = sim_cycle;
	}
	else if (cache_dl1->rdb)
	{
	    /* There is at least one read entry. Drain it out. */
	    cache_dl1->rdb--;
	    cache_dl1->lastuse = sim_cycle;
	}
	else if (cache_dl1->wrb)
	{
	    /* There are no reads...this is a period of lull - drain out a write */
	    cache_dl1->wrb--;
	    cache_dl1->lastuse = sim_cycle;
	}
    }

    /* Do the same for the IL1 */
    if ((sim_cycle - cache_il1->lastuse) >= IL1PIPE)
    {
	if ((cache_il1->wrb - cache_il1->wrbufsize) > IL1WRB)
	{
	    /* Need to drain out write buffer */
	    cache_il1->wrb--;
	    cache_il1->lastuse = sim_cycle;
	}
	else if (cache_il1->rdb)
	{
	    /* There is at least one read entry. Drain it out. */
	    cache_il1->rdb--;
	    cache_il1->lastuse = sim_cycle;
	}
	else if (cache_il1->wrb)
	{
	    /* There is at least one write entry. Drain it out. */
	    cache_il1->wrb--;
	    cache_il1->lastuse = sim_cycle;
	}
    }

    /* Repeat for L2 */
    if ((sim_cycle - cache_dl2->lastuse) >= DL2PIPE)
    {
	if ((cache_dl2->wrb - cache_dl2->wrbufsize) > DL2WRB)
	{
	    /* Need to drain out write buffer */
	    cache_dl2->wrb--;
	    cache_dl2->lastuse = sim_cycle;
#ifdef CACHE_MISS_STAT
	    if (thecontexts[0]->non_spec_wrb_miss_count > 0)
		thecontexts[0]->non_spec_wrb_miss_count--;
	    else if (thecontexts[0]->spec_wrb_miss_count > 0)
		thecontexts[0]->spec_wrb_miss_count--;
#endif
	}
	else if (cache_dl2->rdb)
	{
	    /* There is at least one read entry. Drain it out. */
	    cache_dl2->rdb--;
	    cache_dl2->lastuse = sim_cycle;
#ifdef CACHE_MISS_STAT
	    if (thecontexts[0]->non_spec_rdb_miss_count > 0)
		thecontexts[0]->non_spec_rdb_miss_count--;
	    else if (thecontexts[0]->spec_rdb_miss_count > 0)
		thecontexts[0]->spec_rdb_miss_count--;

	    if (thecontexts[0]->inst_miss_count > 0)
		thecontexts[0]->inst_miss_count--;
	    else if (thecontexts[0]->load_miss_count > 0)
		thecontexts[0]->load_miss_count--;
	    else if (thecontexts[0]->store_miss_count > 0)
		thecontexts[0]->store_miss_count--;
#endif
	}
	else if (cache_dl2->wrb)
	{
	    /* There is at least one write entry. Drain it out. */
	    cache_dl2->wrb--;
	    cache_dl2->lastuse = sim_cycle;
#ifdef CACHE_MISS_STAT
	    if (thecontexts[0]->non_spec_wrb_miss_count > 0)
		thecontexts[0]->non_spec_wrb_miss_count--;
	    else if (thecontexts[0]->spec_wrb_miss_count > 0)
		thecontexts[0]->spec_wrb_miss_count--;
#endif
	}
    }
#ifdef CACHE_MISS_STAT
    if (!count_error)
    {
	count_error = 1;
	if ((thecontexts[0]->non_spec_rdb_miss_count + thecontexts[0]->spec_rdb_miss_count) != cache_dl2->rdb)
	    fprintf (stderr, "Error cache_dl2rdb mismatch spec-nonspec on cycle %lu\n", (unsigned long) sim_cycle);
	if ((thecontexts[0]->inst_miss_count + thecontexts[0]->load_miss_count + thecontexts[0]->store_miss_count) != cache_dl2->rdb)
	    fprintf (stderr, "Error cache_dl2rdb mismatch inst/ld/st on cycle %lu\n", (unsigned long) sim_cycle);
	if ((thecontexts[0]->non_spec_wrb_miss_count + thecontexts[0]->spec_wrb_miss_count) != cache_dl2->wrb)
	    fprintf (stderr, "Error cache_dl2wrb mismatch on cycle %lu\n", (unsigned long) sim_cycle);
    }
#endif

#ifdef  MSHR
    mshr_size = cache_dl1->rdb + cache_dl2->rdb;
    if (mshr_size >= MSHRSIZE)
	mshr_full = 1;
    else
	mshr_full = 0;
    mshr_count += mshr_size;
    if (mshr_full)
    {
#ifdef  MSHR_DIST
	mshr_size_dist[MSHRSIZE - 1]++;
#endif
	mshr_full_count++;
    }
#ifdef  MSHR_DIST
    else
	mshr_size_dist[mshr_size]++;
#endif
#endif
}

#endif

void
compute_queue (int threadid)	/* Compute the sizes of the queue and register
				   files. - Ronz  - Ali */
{
}				// compute_queue


int
bank_lookup (md_addr_t pc, int spec, int threadid)
{
    int index, xorbits, lev2index1, lev2index2, lev2index3, lev2index4, lev2index5, bnk;

    if (spec)
    {
	index = (pc >> 2) % LEV1SIZE;
	xorbits = (pc >> 2) - ((pc >> (2 + XORSIZE)) << XORSIZE);

	lev2index1 = (specshiftreg1[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index2 = (specshiftreg2[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index3 = (specshiftreg3[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index4 = (specshiftreg4[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index5 = (specshiftreg5[threadid][index] ^ xorbits) % LEV2SIZE;
	bnk = 0;
	if ((CACHEPORTS > 1) && (lev2pred1[lev2index1] >= 2))
	    bnk++;
	if ((CACHEPORTS > 2) && (lev2pred2[lev2index2] >= 2))
	    bnk += 2;
	if ((CACHEPORTS > 4) && (lev2pred3[lev2index3] >= 2))
	    bnk += 4;
	if ((CACHEPORTS > 8) && (lev2pred4[lev2index4] >= 2))
	    bnk += 8;
	if ((CACHEPORTS > 16) && (lev2pred5[lev2index5] >= 2))
	    bnk += 16;

	specshiftreg1[threadid][index] = (specshiftreg1[threadid][index] << 1) % LEV2SIZE;
	specshiftreg2[threadid][index] = (specshiftreg2[threadid][index] << 1) % LEV2SIZE;
	specshiftreg3[threadid][index] = (specshiftreg3[threadid][index] << 1) % LEV2SIZE;
	specshiftreg4[threadid][index] = (specshiftreg4[threadid][index] << 1) % LEV2SIZE;
	specshiftreg5[threadid][index] = (specshiftreg5[threadid][index] << 1) % LEV2SIZE;
	if (bnk % 2)
	{
	    specshiftreg1[threadid][index] = (specshiftreg1[threadid][index] + 1) % LEV2SIZE;
	}
	if ((bnk / 2) % 2)
	{
	    specshiftreg2[threadid][index] = (specshiftreg2[threadid][index] + 1) % LEV2SIZE;
	}
	if ((bnk / 4) % 2)
	{
	    specshiftreg3[threadid][index] = (specshiftreg3[threadid][index] + 1) % LEV2SIZE;
	}
	if ((bnk / 8) % 2)
	{
	    specshiftreg4[threadid][index] = (specshiftreg4[threadid][index] + 1) % LEV2SIZE;
	}
	if ((bnk / 16) % 2)
	{
	    specshiftreg5[threadid][index] = (specshiftreg5[threadid][index] + 1) % LEV2SIZE;
	}

	return bnk;
    }
    else
    {
	index = (pc >> 2) % LEV1SIZE;
	xorbits = (pc >> 2) - ((pc >> (2 + XORSIZE)) << XORSIZE);

	lev2index1 = (shiftreg1[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index2 = (shiftreg2[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index3 = (shiftreg3[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index4 = (shiftreg4[threadid][index] ^ xorbits) % LEV2SIZE;
	lev2index5 = (shiftreg5[threadid][index] ^ xorbits) % LEV2SIZE;

	bnk = 0;
	if ((CACHEPORTS > 1) && (lev2pred1[lev2index1] >= 2))
	    bnk++;
	if ((CACHEPORTS > 2) && (lev2pred2[lev2index2] >= 2))
	    bnk += 2;
	if ((CACHEPORTS > 4) && (lev2pred3[lev2index3] >= 2))
	    bnk += 4;
	if ((CACHEPORTS > 8) && (lev2pred4[lev2index4] >= 2))
	    bnk += 8;
	if ((CACHEPORTS > 16) && (lev2pred5[lev2index5] >= 2))
	    bnk += 16;
	return bnk;
    }
}

void
bank_update (md_addr_t pc, int bnk, int threadid)
{
    int index, xorbits, lev2index1, lev2index2, lev2index3, lev2index4, lev2index5;

    index = (pc >> 2) % LEV1SIZE;
    xorbits = (pc >> 2) - ((pc >> (2 + XORSIZE)) << XORSIZE);
    lev2index1 = (shiftreg1[threadid][index] ^ xorbits) % LEV2SIZE;
    lev2index2 = (shiftreg2[threadid][index] ^ xorbits) % LEV2SIZE;
    lev2index3 = (shiftreg3[threadid][index] ^ xorbits) % LEV2SIZE;
    lev2index4 = (shiftreg4[threadid][index] ^ xorbits) % LEV2SIZE;
    lev2index5 = (shiftreg5[threadid][index] ^ xorbits) % LEV2SIZE;

    shiftreg1[threadid][index] = (shiftreg1[threadid][index] << 1) % LEV2SIZE;
    shiftreg2[threadid][index] = (shiftreg2[threadid][index] << 1) % LEV2SIZE;
    shiftreg3[threadid][index] = (shiftreg3[threadid][index] << 1) % LEV2SIZE;
    shiftreg4[threadid][index] = (shiftreg4[threadid][index] << 1) % LEV2SIZE;
    shiftreg5[threadid][index] = (shiftreg5[threadid][index] << 1) % LEV2SIZE;
    if ((CACHEPORTS > 1) && (bnk % 2))
    {
	lev2pred1[lev2index1]++;
	lev2pred1[lev2index1] = min2 (3, lev2pred1[lev2index1]);
	shiftreg1[threadid][index] = (shiftreg1[threadid][index] + 1) % LEV2SIZE;
    }
    else
    {
	lev2pred1[lev2index1]--;
	lev2pred1[lev2index1] = max2 (0, lev2pred1[lev2index1]);
    }
    if ((CACHEPORTS > 2) && ((bnk / 2) % 2))
    {
	lev2pred2[lev2index2]++;
	lev2pred2[lev2index2] = min2 (3, lev2pred2[lev2index2]);
	shiftreg2[threadid][index] = (shiftreg2[threadid][index] + 1) % LEV2SIZE;
    }
    else
    {
	lev2pred2[lev2index2]--;
	lev2pred2[lev2index2] = max2 (0, lev2pred2[lev2index2]);
    }
    if ((CACHEPORTS > 4) && ((bnk / 4) % 2))
    {
	lev2pred3[lev2index3]++;
	lev2pred3[lev2index3] = min2 (3, lev2pred3[lev2index3]);
	shiftreg3[threadid][index] = (shiftreg3[threadid][index] + 1) % LEV2SIZE;
    }
    else
    {
	lev2pred3[lev2index3]--;
	lev2pred3[lev2index3] = max2 (0, lev2pred3[lev2index3]);
    }
    if ((CACHEPORTS > 8) && ((bnk / 8) % 2))
    {
	lev2pred4[lev2index4]++;
	lev2pred4[lev2index4] = min2 (3, lev2pred4[lev2index4]);
	shiftreg4[threadid][index] = (shiftreg4[threadid][index] + 1) % LEV2SIZE;
    }
    else
    {
	lev2pred4[lev2index4]--;
	lev2pred4[lev2index4] = max2 (0, lev2pred4[lev2index4]);
    }
    if ((CACHEPORTS > 16) && ((bnk / 16) % 2))
    {
	lev2pred5[lev2index5]++;
	lev2pred5[lev2index5] = min2 (3, lev2pred5[lev2index5]);
	shiftreg5[threadid][index] = (shiftreg5[threadid][index] + 1) % LEV2SIZE;
    }
    else
    {
	lev2pred5[lev2index5]--;
	lev2pred5[lev2index5] = max2 (0, lev2pred5[lev2index5]);
    }
}

void
bankpred_init ()
{
    int i;

    for (i = 0; i < LEV2SIZE; i++)
    {
	if ((i % 4) == 0)
	{
	    lev2pred1[i] = 1;
	    lev2pred2[i] = 1;
	}
	else if ((i % 4) == 1)
	{
	    lev2pred1[i] = 2;
	    lev2pred2[i] = 1;
	}
	else if ((i % 4) == 2)
	{
	    lev2pred1[i] = 1;
	    lev2pred2[i] = 2;
	}
	else
	{
	    lev2pred1[i] = 2;
	    lev2pred2[i] = 2;
	}
    }
}

void
ilp_init ()
{
    int i, j;
    context *current;

    for (i = 0; i < ITABLESIZE; i++)
    {
	distilptable[i] = 1;
	distilptabletagid[i].tag = 0;
	distilptabletagid[i].threadid = -1;
	distilpcounter[i] = 0;
	distilpsamples[i] = 0;
    }
    for (j = 0; j < numcontexts; j++)
    {
	current = thecontexts[j];
	for (i = 0; i < OWINDOWSIZE; i++)
	{
	    current->oldwindowilp[i] = 0;
	    current->oldwindowbr[i] = 0;
	    current->oldwindowpc[i] = 0;
	}
	current->owindow_head = 0;
    }
    distilpcount = 0;
}

double
std_dev (int a, int b, int c, int d)
{
    double sum, avg;
    double aa, bb, cc, dd;

    aa = (double) a;
    bb = (double) b;
    cc = (double) c;
    dd = (double) d;

    avg = (aa + bb + cc + dd) / 4.0;
    sum = (aa - avg) * (aa - avg) + (bb - avg) * (bb - avg) + (cc - avg) * (cc - avg) + (dd - avg) * (dd - avg);
    sum = sum / 4.0;
    sum = sqrt (sum);
    return sum;
}

#ifndef NEW_INTERCONNECT
void
st_ring_lat (struct RUU_station *rs)
{
    int myreg, tempv, from, i, j, rowf;
    counter_t now, nowc;
    int found, k;

#ifdef N_WAY_STEER
    int limit_cluster;
#endif

#ifdef  PARTITIONED_LSQ
    int lsq_part_thrd;
#endif

#ifdef  IGNORE_MEM_COMM_LATENCY
    return;
#endif


/*092004:mwr*/

#ifdef   THRD_WAY_CACHE
#ifdef  PARTITIONED_LSQ
    lsq_part_thrd = (n_cache_limit_thrd[rs->threadid] / lsq_part_bank) + ((n_cache_limit_thrd[rs->threadid] % lsq_part_bank == 0) ? 0 : 1);
    if (lsq_part_thrd < 2)
	return;
#else //PARTITIONED_LSQ
    if (n_cache_limit_thrd[rs->threadid] < 2)
	return;
#endif //PARTITIONED_LSQ
#endif //THRD_WAY_CACHE

    if (uni_lat_comm)
    {
	for (i = 0; i < CLUSTERS; i++)
	    rs->st_reach[i] = sim_cycle + 1;
	rs->st_reach[rs->cluster] = sim_cycle;
	return;
    }
    if (two_lat_comm)
    {
	for (i = 0; i < CLUSTERS; i++)
	    rs->st_reach[i] = sim_cycle + 2;
	rs->st_reach[rs->cluster] = sim_cycle;
	rs->st_reach[CL_ADD (rs->cluster, 1)] = sim_cycle + 1;
	rs->st_reach[CL_SUB (rs->cluster, 1)] = sim_cycle + 1;
	return;
    }

    if (GRID)
    {
	myreg = 0;
	rs->st_reach[rs->cluster] = sim_cycle;

	if (ST_RING_BW)
	{
	    from = rs->cluster;
	    rowf = from / ROWCOUNT;

	    now = sim_cycle;
	    for (i = from; i < (rowf + 1) * ROWCOUNT; i++)
	    {
		if (i < (rowf + 1) * ROWCOUNT - 1)
		{
#ifdef  TRACE_REG
		    while (ring1[i][(now % TIMEWINDOW)] && (!myreg || (reg1[i][(now % TIMEWINDOW)][0] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring1[i][(now % TIMEWINDOW)] = 1;
		    reg1[i][(now % TIMEWINDOW)][0] = myreg;
#else
		    while (ring1[i][(now % TIMEWINDOW)] && (!myreg || (reg1[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring1[i][(now % TIMEWINDOW)] = 1;
		    reg1[i][(now % TIMEWINDOW)] = myreg;
#endif
		    now = now + hop_time;
		    rs->st_reach[(i + 1) % CLUSTERS] = now;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}

		nowc = now;
		for (j = i; j < (CLUSTERS - ROWCOUNT); j = j + ROWCOUNT)
		{
		    while (ring3[j][(nowc % TIMEWINDOW)] && (!myreg || (reg3[j][(nowc % TIMEWINDOW)] != myreg)) && ((nowc - sim_cycle) < TIMEWINDOW))
		    {
			nowc++;
		    }
		    ring3[j][(nowc % TIMEWINDOW)] = 1;
		    reg3[j][(nowc % TIMEWINDOW)] = myreg;
		    nowc = nowc + hop_time;
		    rs->st_reach[(j + ROWCOUNT) % CLUSTERS] = nowc;
		    if ((nowc - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
		nowc = now;
		for (j = i; j >= ROWCOUNT; j = j - ROWCOUNT)
		{
		    while (ring4[j][(nowc % TIMEWINDOW)] && (!myreg || (reg4[j][(nowc % TIMEWINDOW)] != myreg)) && ((nowc - sim_cycle) < TIMEWINDOW))
		    {
			nowc++;
		    }
		    ring4[j][(nowc % TIMEWINDOW)] = 1;
		    reg4[j][(nowc % TIMEWINDOW)] = myreg;
		    nowc = nowc + hop_time;
		    rs->st_reach[(j - ROWCOUNT)] = nowc;
		    if ((nowc - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
	    }			//  for (i = from; i < (rowf+1)*ROWCOUNT; i++)

	    now = sim_cycle;
	    for (i = from; i >= rowf * ROWCOUNT; i--)
	    {
		if (i > rowf * ROWCOUNT)
		{
#ifdef  TRACE_REG
		    while (ring2[i][(now % TIMEWINDOW)] && (!myreg || (reg2[i][(now % TIMEWINDOW)][0] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring2[i][(now % TIMEWINDOW)] = 1;
		    reg2[i][(now % TIMEWINDOW)][0] = myreg;
#else
		    while (ring2[i][(now % TIMEWINDOW)] && (!myreg || (reg2[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring2[i][(now % TIMEWINDOW)] = 1;
		    reg2[i][(now % TIMEWINDOW)] = myreg;
#endif
		    now = now + hop_time;
		    rs->st_reach[(i - 1) % CLUSTERS] = now;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}

		nowc = now;
		for (j = i; j < (CLUSTERS - ROWCOUNT); j = j + ROWCOUNT)
		{
		    while (ring3[j][(nowc % TIMEWINDOW)] && (!myreg || (reg3[j][(nowc % TIMEWINDOW)] != myreg)) && ((nowc - sim_cycle) < TIMEWINDOW))
		    {
			nowc++;
		    }
		    ring3[j][(nowc % TIMEWINDOW)] = 1;
		    reg3[j][(nowc % TIMEWINDOW)] = myreg;
		    nowc = nowc + hop_time;
		    rs->st_reach[(j + ROWCOUNT) % CLUSTERS] = nowc;
		    if ((nowc - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
		nowc = now;
		for (j = i; j >= ROWCOUNT; j = j - ROWCOUNT)
		{
		    while (ring4[j][(nowc % TIMEWINDOW)] && (!myreg || (reg4[j][(nowc % TIMEWINDOW)] != myreg)) && ((nowc - sim_cycle) < TIMEWINDOW))
		    {
			nowc++;
		    }
		    ring4[j][(nowc % TIMEWINDOW)] = 1;
		    reg4[j][(nowc % TIMEWINDOW)] = myreg;
		    nowc = nowc + hop_time;
		    rs->st_reach[(j - ROWCOUNT)] = nowc;
		    if ((nowc - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
	    }			//  for (i = from; i >= rowf*ROWCOUNT; i--)
	}			// if(ST_RING_BW)
	else
	{
	    from = rs->cluster;
	    rowf = from / ROWCOUNT;

	    now = sim_cycle;
	    for (i = from; i < (rowf + 1) * ROWCOUNT; i++)
	    {
		if (i < (rowf + 1) * ROWCOUNT - 1)
		{
		    now = now + hop_time;
		    rs->st_reach[(i + 1) % CLUSTERS] = now;
		}

		nowc = now;
		for (j = i; j < (CLUSTERS - ROWCOUNT); j = j + ROWCOUNT)
		{
		    nowc = nowc + hop_time;
		    rs->st_reach[(j + ROWCOUNT) % CLUSTERS] = nowc;
		}
		nowc = now;
		for (j = i; j >= ROWCOUNT; j = j - ROWCOUNT)
		{
		    nowc = nowc + hop_time;
		    rs->st_reach[(j - ROWCOUNT)] = nowc;
		}
	    }

	    now = sim_cycle;
	    for (i = from; i >= rowf * ROWCOUNT; i--)
	    {
		if (i > rowf * ROWCOUNT)
		{
		    now = now + hop_time;
		    rs->st_reach[(i - 1) % CLUSTERS] = now;
		}

		nowc = now;
		for (j = i; j < (CLUSTERS - ROWCOUNT); j = j + ROWCOUNT)
		{
		    nowc = nowc + hop_time;
		    rs->st_reach[(j + ROWCOUNT) % CLUSTERS] = nowc;
		}
		nowc = now;
		for (j = i; j >= ROWCOUNT; j = j - ROWCOUNT)
		{
		    nowc = nowc + hop_time;
		    rs->st_reach[(j - ROWCOUNT)] = nowc;
		}
	    }
	}			// else-if(ST_RING_BW)
    }				// if(GRID)

    else
    {				/* No grid. Ring interconnect. */
	myreg = 0;
	rs->st_reach[rs->cluster] = sim_cycle;
#ifdef  COMM_BUFF
	comm1qram_access_cl[rs->cluster] = 1;
	comm2qram_access_cl[rs->cluster] = 1;
#endif

	if (ST_RING_BW)
	{
	    /* First go in increasing order for 8 hops. */
	    tempv = (CLUSTERS / 2) - 1;
	    now = sim_cycle;
	    from = rs->cluster;
#ifdef THRD_WAY_CACHE
	    if (n_cache_limit_thrd[rs->threadid] <= CLUSTERS / 2)
		limit_cluster = (n_cache_start_thrd[rs->threadid] + n_cache_limit_thrd[rs->threadid] + 1) % CLUSTERS;
	    else		//Ali:  not that accuret need to be modified
		limit_cluster = -1;
	    for (i = from, j = 0; j <= tempv && i != limit_cluster; j++, i = (i + 1) % CLUSTERS)
	    {
#else
	    for (i = from, j = 0; j <= tempv; j++, i = (i + 1) % CLUSTERS)
	    {
#endif
#ifdef INST_TRANS_RATE
#ifdef  TRACE_REG
		if (!myreg)
		{
		    found = 0;
		    while ((ring1[i][(now % TIMEWINDOW)] >= link_width) && ((now - sim_cycle) < TIMEWINDOW))
			now++;
		}
		else
		{
		    found = 0;
		    while (ring1[i][(now % TIMEWINDOW)] > 0)
		    {
			for (k = 0; k < ring1[i][now % TIMEWINDOW]; k++)
			{
			    if (reg1[i][(now % TIMEWINDOW)][k] == myreg)
			    {
				found = 1;
				break;
			    }
			}
			if (found || (ring1[i][(now % TIMEWINDOW)] < link_width))
			    break;
			else
			    now++;
		    }
		}
		if (!found)
		{
		    if (ring1[i][(now % TIMEWINDOW)] < link_width)
		    {
			link_width_dist[ring1[i][(now % TIMEWINDOW)]]++;
			reg1[i][(now % TIMEWINDOW)][ring1[i][(now % TIMEWINDOW)]] = myreg;
			ring1[i][(now % TIMEWINDOW)]++;
		    }
		    else
		    {
			fprintf (stderr, "DE_CENT ring1 exceed its limit in cyc %lu\n", (unsigned long) sim_cycle);
			exit (1);
		    }
		}
#else
		while ((ring1[i][(now % TIMEWINDOW)] >= link_width) && (!myreg || (reg1[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    now++;
		if (ring1[i][(now % TIMEWINDOW)] < link_width)
		{
		    link_width_dist[ring1[i][(now % TIMEWINDOW)]]++;
		    ring1[i][(now % TIMEWINDOW)]++;
		    reg1[i][(now % TIMEWINDOW)] = myreg;
		}
#endif
#else
		while (ring1[i][(now % TIMEWINDOW)] && (!myreg || (reg1[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		{
		    now++;
		}
		ring1[i][(now % TIMEWINDOW)] = 1;
		reg1[i][(now % TIMEWINDOW)] = myreg;
#endif
		now = now + hop_time;
		rs->st_reach[(i + 1) % CLUSTERS] = now;
		if ((now - sim_cycle) >= TIMEWINDOW)
		{
		    num_bw_of++;
		    break;
		}
	    }

	    tempv = (CLUSTERS / 2) - 2;
	    now = sim_cycle;
	    from = rs->cluster;
#ifdef THRD_WAY_CACHE
	    if (n_cache_limit_thrd[rs->threadid] <= CLUSTERS / 2)
		limit_cluster = (n_cache_start_thrd[rs->threadid] + CLUSTERS - 1) % CLUSTERS;
	    else		//Ali:  not that accuret need to be modified
		limit_cluster = -1;
	    for (i = from, j = 0; j <= tempv && i != limit_cluster; j++, i = (CLUSTERS + i - 1) % CLUSTERS)
	    {
#else
	    for (i = from, j = 0; j <= tempv; j++, i = (CLUSTERS + i - 1) % CLUSTERS)
	    {
#endif
#ifdef INST_TRANS_RATE
#ifdef  TRACE_REG
		if (!myreg)
		{
		    found = 0;
		    while ((ring2[i][(now % TIMEWINDOW)] >= link_width) && ((now - sim_cycle) < TIMEWINDOW))
			now++;
		}
		else
		{
		    found = 0;
		    while (ring2[i][(now % TIMEWINDOW)] > 0)
		    {
			for (k = 0; k < ring2[i][now % TIMEWINDOW]; k++)
			{
			    if (reg2[i][(now % TIMEWINDOW)][k] == myreg)
			    {
				found = 1;
				break;
			    }
			}
			if (found || (ring2[i][(now % TIMEWINDOW)] < link_width))
			    break;
			else
			    now++;
		    }
		}
		if (!found)
		{
		    if (ring2[i][(now % TIMEWINDOW)] < link_width)
		    {
			link_width_dist[ring2[i][(now % TIMEWINDOW)]]++;
			reg2[i][(now % TIMEWINDOW)][ring2[i][(now % TIMEWINDOW)]] = myreg;
			ring2[i][(now % TIMEWINDOW)]++;
		    }
		    else
		    {
			fprintf (stderr, "ring2 exceed its limit in cyc %lu\n", (unsigned long) sim_cycle);
			exit (1);
		    }
		}
#else
		while ((ring2[i][(now % TIMEWINDOW)] >= link_width) && (!myreg || (reg2[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    now++;
		if (ring2[i][(now % TIMEWINDOW)] < link_width)
		{
		    link_width_dist[ring2[i][(now % TIMEWINDOW)]]++;
		    ring2[i][(now % TIMEWINDOW)]++;
		    reg2[i][(now % TIMEWINDOW)] = myreg;
		}
#endif
#else
		while (ring2[i][(now % TIMEWINDOW)] && (!myreg || (reg2[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		{
		    now++;
		}
		ring2[i][(now % TIMEWINDOW)] = 1;
		reg2[i][(now % TIMEWINDOW)] = myreg;
#endif
		now = now + hop_time;
		rs->st_reach[(CLUSTERS + i - 1) % CLUSTERS] = now;
		if ((now - sim_cycle) >= TIMEWINDOW)
		{
		    num_bw_of++;
		    break;
		}
	    }
	}
	else
	{
	    /* First go in increasing order for 8 hops. */
	    tempv = (CLUSTERS / 2) - 1;
	    now = sim_cycle;
	    from = rs->cluster;
	    for (i = from, j = 0; j <= tempv; j++, i = (i + 1) % CLUSTERS)
	    {
		now = now + hop_time;
		rs->st_reach[(i + 1) % CLUSTERS] = now;
	    }

	    tempv = (CLUSTERS / 2) - 2;
	    now = sim_cycle;
	    from = rs->cluster;
	    for (i = from, j = 0; j <= tempv; j++, i = (CLUSTERS + i - 1) % CLUSTERS)
	    {
		now = now + hop_time;
		rs->st_reach[(CLUSTERS + i - 1) % CLUSTERS] = now;
	    }
	}
    }

    return;
}

int
ring_lat (int from, int to, int myreg, int offset)
{
    int tempv, delay, i, dir, j, rowf, rowt, colf, colt;
    counter_t now;
    int found, k;

#ifdef  IGNORE_ALL_COMM_LATENCY
    return 0;
#endif
    if (from == to)
	return 0;
    if (uni_lat_comm)
	return 1;
    if (two_lat_comm)
    {
	if ((CL_ADD (from, 1) == to) || (CL_SUB (from, 1) == to))
	    return 1;
	return 2;
    }


    if (GRID)
    {				/*    0  1  2  3                   N4
				   4  5  6  7              W2              E1
				   8  9 10 11                      S3
				   12 13 14 15 */


	rowf = from / ROWCOUNT;
	colf = from % ROWCOUNT;
	rowt = to / ROWCOUNT;
	colt = to % ROWCOUNT;

	/* First send data along the row, then along the column. */

	tempv = abs (colf - colt);
	if ((colf - colt) < 0)
	{
	    dir = 1;
	}
	else
	{
	    dir = 2;
	}
	if (actual_clusters == 4)
	{
	    if ((colf - colt) == -3)
	    {
		tempv = 1;
		dir = 2;
	    }
	    if ((colf - colt) == 3)
	    {
		tempv = 1;
		dir = 1;
	    }
	}

	tempv = tempv - 1;
	delay = (tempv * 2 * COMM_LATENCY) / CLUSTERS + hop_time;

	if (RING_BW)
	{
	    if (dir == 1)
	    {
		now = sim_cycle + offset;
		for (i = from, j = 0; j <= tempv; j++, i = (i + 1) % CLUSTERS)
		{
#ifdef TRACE_REG
		    while (ring1[i][(now % TIMEWINDOW)] && (!myreg || (reg1[i][(now % TIMEWINDOW)][0] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring1[i][(now % TIMEWINDOW)] = 1;
		    reg1[i][(now % TIMEWINDOW)][0] = myreg;
#else
		    while (ring1[i][(now % TIMEWINDOW)] && (!myreg || (reg1[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring1[i][(now % TIMEWINDOW)] = 1;
		    reg1[i][(now % TIMEWINDOW)] = myreg;
#endif
		    now = now + hop_time;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
	    }
	    else
	    {
		now = sim_cycle + offset;
		for (i = from, j = 0; j <= tempv; j++, i = (CLUSTERS + i - 1) % CLUSTERS)
		{
#ifdef TRACE_REG
		    while (ring2[i][(now % TIMEWINDOW)] && (!myreg || (reg2[i][(now % TIMEWINDOW)][0] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring2[i][(now % TIMEWINDOW)] = 1;
		    reg2[i][(now % TIMEWINDOW)][0] = myreg;
#else
		    while (ring2[i][(now % TIMEWINDOW)] && (!myreg || (reg2[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring2[i][(now % TIMEWINDOW)] = 1;
		    reg2[i][(now % TIMEWINDOW)] = myreg;
#endif
		    now = now + hop_time;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
	    }
	}

	tempv = abs (rowf - rowt);
	if ((rowf - rowt) < 0)
	{
	    dir = 1;
	}
	else
	{
	    dir = 2;
	}
	tempv = tempv - 1;
	delay += (tempv * 2 * COMM_LATENCY) / CLUSTERS + hop_time;
	tot_del += delay;

	if (RING_BW)
	{
	    if (dir == 1)
	    {
		for (i = (rowf * ROWCOUNT) + colt, j = 0; j <= tempv; j++, i = (i + ROWCOUNT) % CLUSTERS)
		{
		    while (ring3[i][(now % TIMEWINDOW)] && (!myreg || (reg3[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring3[i][(now % TIMEWINDOW)] = 1;
		    reg3[i][(now % TIMEWINDOW)] = myreg;
		    now = now + hop_time;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
	    }
	    else
	    {
		for (i = (rowf * ROWCOUNT) + colt, j = 0; j <= tempv; j++, i = (CLUSTERS + i - ROWCOUNT) % CLUSTERS)
		{
		    while (ring4[i][(now % TIMEWINDOW)] && (!myreg || (reg4[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring4[i][(now % TIMEWINDOW)] = 1;
		    reg4[i][(now % TIMEWINDOW)] = myreg;
		    now = now + hop_time;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}
	    }
	    delay = now - sim_cycle - offset;
	}

	tot_del_bw += delay;

	if (delay < 1)
	    fatal ("delay is %ld", delay);

	return delay;

    }

    else
    {				/* Ring interconnect. */

//  fprintf(stderr,"%d fr %d to %d  %d\n",myreg,from, to,sim_cycle);
	tempv = abs (from - to);
	if (tempv > (CLUSTERS / 2))
	{
	    tempv = CLUSTERS - tempv;
	    if ((from - to) < 0)
	    {
		dir = 2;
	    }
	    else
	    {
		dir = 1;
	    }
	}
	else
	{
	    if ((from - to) < 0)
	    {
		dir = 1;
	    }
	    else
	    {
		dir = 2;
	    }
	}
	total_hop += tempv;
	stat_add_sample (hop_dist, tempv);
	now = sim_cycle + offset;
#ifdef OPTICAL_TENT
	if (optical_comm && (tempv > (CLUSTERS / 4)))
	{
	    comm_opt_qram_access_cl[from] = 1;
	    tempv = tempv - CLUSTERS / 4;
	    found = 0;
	    if (!myreg)
	    {
		while ((optical_link[from][(now % TIMEWINDOW)] >= optical_link_width) && ((now - sim_cycle) < TIMEWINDOW))
		    now++;
	    }
	    else
	    {
		while (optical_link[from][(now % TIMEWINDOW)] > 0)
		{
		    for (k = 0; k < optical_link[from][now % TIMEWINDOW]; k++)
		    {
			if (opt_reg[from][(now % TIMEWINDOW)][k] == myreg)
			{
			    found = 1;
			    break;
			}
		    }
		    if (found || (optical_link[from][(now % TIMEWINDOW)] < optical_link_width))
			break;
		    else
			now++;
		}
	    }
	    if (!found)
	    {
		if (optical_link[from][(now % TIMEWINDOW)] < optical_link_width)
		{
		    optical_link_width_dist[optical_link[from][(now % TIMEWINDOW)]]++;
//      fprintf(stderr,"added to opt link dist \n");

		    opt_reg[from][(now % TIMEWINDOW)][optical_link[from][(now % TIMEWINDOW)]] = myreg;
		    optical_link[from][(now % TIMEWINDOW)]++;
		}
		else
		{
		    fprintf (stderr, "optical_link exceeded its limit in cycle %llu\n", sim_cycle);
		    fatal ("bad optical limit");
		}
	    }

	    from = (from + (CLUSTERS / 2)) % CLUSTERS;
	    dir = dir == 1 ? 2 : 1;
	    now += optical_hop_time;
	}
#endif
	tempv = tempv - 1;
	delay = (tempv * 2 * COMM_LATENCY) / CLUSTERS + hop_time;
	tot_del += delay;

	if (RING_BW)
	{
	    if (dir == 1)
	    {
#ifdef  COMM_BUFF
		comm1qram_access_cl[from] = 1;
#endif
		for (i = from, j = 0; j <= tempv; j++, i = (i + 1) % CLUSTERS)
		{
#ifdef INST_TRANS_RATE
#ifdef  TRACE_REG
		    found = 0;
		    if (!myreg)
		    {
			while ((ring1[i][(now % TIMEWINDOW)] >= link_width) && ((now - sim_cycle) < TIMEWINDOW))
			    now++;
		    }
		    else
		    {
			while (ring1[i][(now % TIMEWINDOW)] > 0)
			{
			    for (k = 0; k < ring1[i][now % TIMEWINDOW]; k++)
			    {
				if (reg1[i][(now % TIMEWINDOW)][k] == myreg)
				{
				    found = 1;
				    break;
				}
			    }
			    if (found || (ring1[i][(now % TIMEWINDOW)] < link_width))
				break;
			    else
				now++;
			}
		    }
		    if (!found)
		    {
			if (ring1[i][(now % TIMEWINDOW)] < link_width)
			{
			    link_width_dist[ring1[i][(now % TIMEWINDOW)]]++;
			    reg1[i][(now % TIMEWINDOW)][ring1[i][(now % TIMEWINDOW)]] = myreg;
			    ring1[i][(now % TIMEWINDOW)]++;
			}
			else
			{
			    fprintf (stderr, "ring1 exceed its limit in cyc %lu\n", (unsigned long) sim_cycle);
			    exit (1);
			}
		    }
#else
		    while ((ring1[i][(now % TIMEWINDOW)] >= link_width) && (!myreg || (reg1[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
			now++;
		    if (ring1[i][(now % TIMEWINDOW)] < link_width)
		    {
			link_width_dist[ring1[i][(now % TIMEWINDOW)]]++;
			ring1[i][(now % TIMEWINDOW)]++;
			reg1[i][(now % TIMEWINDOW)] = myreg;
		    }
#endif
#else
		    while (ring1[i][(now % TIMEWINDOW)] && (!myreg || (reg1[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring1[i][(now % TIMEWINDOW)] = 1;
		    reg1[i][(now % TIMEWINDOW)] = myreg;
#endif
		    now = now + hop_time;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }
		}		//for (i = from,j=0; j<=tempv; j++, i=(i+1)%CLUSTERS)
	    }			//if (dir == 1) {
	    else
	    {
#ifdef  COMM_BUFF
		comm2qram_access_cl[from] = 1;
#endif
		for (i = from, j = 0; j <= tempv; j++, i = (CLUSTERS + i - 1) % CLUSTERS)
		{
#ifdef INST_TRANS_RATE
#ifdef  TRACE_REG
		    found = 0;
		    if (!myreg)
		    {
			while ((ring2[i][(now % TIMEWINDOW)] >= link_width) && ((now - sim_cycle) < TIMEWINDOW))
			    now++;
		    }
		    else
		    {
			while (ring2[i][(now % TIMEWINDOW)] > 0)
			{
			    for (k = 0; k < ring2[i][now % TIMEWINDOW]; k++)
			    {
				if (reg2[i][(now % TIMEWINDOW)][k] == myreg)
				{
				    found = 1;
				    break;
				}
			    }
			    if (found || (ring2[i][(now % TIMEWINDOW)] < link_width))
				break;
			    else
				now++;
			}
		    }
		    if (!found)
		    {
			if (ring2[i][(now % TIMEWINDOW)] < link_width)
			{
			    link_width_dist[ring2[i][(now % TIMEWINDOW)]]++;
			    reg2[i][(now % TIMEWINDOW)][ring2[i][(now % TIMEWINDOW)]] = myreg;
			    ring2[i][(now % TIMEWINDOW)]++;
			}
			else
			{
			    fprintf (stderr, "ring2 exceed its limit in cyc %lu\n", (unsigned long) sim_cycle);
			    exit (1);
			}
		    }
#else
		    while ((ring2[i][(now % TIMEWINDOW)] >= link_width) && (!myreg || (reg2[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
			now++;
		    if (ring2[i][(now % TIMEWINDOW)] < link_width)
		    {
			link_width_dist[ring2[i][(now % TIMEWINDOW)]]++;
			ring2[i][(now % TIMEWINDOW)]++;
			reg2[i][(now % TIMEWINDOW)] = myreg;
		    }
#endif
#else
		    while (ring2[i][(now % TIMEWINDOW)] && (!myreg || (reg2[i][(now % TIMEWINDOW)] != myreg)) && ((now - sim_cycle) < TIMEWINDOW))
		    {
			now++;
		    }
		    ring2[i][(now % TIMEWINDOW)] = 1;
		    reg2[i][(now % TIMEWINDOW)] = myreg;
#endif
		    now = now + hop_time;
		    if ((now - sim_cycle) >= TIMEWINDOW)
		    {
			num_bw_of++;
			break;
		    }		//if ((now-sim_cycle) >= TIMEWINDOW
		}		//for (i = from,j=0; j<=tempv; j++, i=(CLUSTERS+i-1)%CLUSTERS
	    }			//else-if  (dir == 1) {
	    delay = now - sim_cycle - offset;
	}			//End RING_BW

	/*
	   for (j=sim_cycle; j < (sim_cycle+20);j++) {
	   for (i=0;i<CLUSTERS;i++) {
	   fprintf(stderr,"%d:%d,%d:%d,%d ",i,ring1[i][j%TIMEWINDOW],reg1[i][j%TIMEWINDOW],ring2[i][j%TIMEWINDOW],reg2[i][j%TIMEWINDOW]);
	   }
	   fprintf(stderr,"\n");
	   }
	   fprintf(stderr,"Delay %d\n",delay);
	 */

	tot_del_bw += delay;

	if (delay < 1)
	    fatal ("delay is %ld", delay);
	stat_add_sample (delay_dist, delay);
	total_delay += delay;
	total_number_delay++;
	return delay;
    }
}
void
ring_free ()
{
    int i, j;

    for (i = 0; i < CLUSTERS; i++)
    {
#ifdef  COMM_BUFF
#ifdef  PWR_ACCESS
	if (ring1[i][((long) sim_cycle % TIMEWINDOW)])
	    comm1qram_access_cl[i] = 2 * ring1[i][((long) sim_cycle % TIMEWINDOW)];
	if (ring2[i][((long) sim_cycle % TIMEWINDOW)])
	    comm2qram_access_cl[i] = 2 * ring2[i][((long) sim_cycle % TIMEWINDOW)];
#else //PWR_ACCESS
	if (ring1[i][((long) sim_cycle % TIMEWINDOW)])
	    comm1qram_access_cl[i] = 1;
	if (ring2[i][((long) sim_cycle % TIMEWINDOW)])
	    comm2qram_access_cl[i] = 1;
#endif //  PWR_ACCESS
#endif
#ifdef  COMM_LINK_PWR
	if (ring1[i][((long) sim_cycle % TIMEWINDOW)])
	    comm1link_access_cl[i] = 1;
	if (ring2[i][((long) sim_cycle % TIMEWINDOW)])
	    comm2link_access_cl[i] = 1;
#endif
	ring1[i][((long) sim_cycle % TIMEWINDOW)] = 0;
	ring2[i][((long) sim_cycle % TIMEWINDOW)] = 0;
	ring3[i][((long) sim_cycle % TIMEWINDOW)] = 0;
	ring4[i][((long) sim_cycle % TIMEWINDOW)] = 0;
#ifdef  TRACE_REG
	for (j = 0; j < link_width; j++)
	{
	    reg1[i][((long) sim_cycle % TIMEWINDOW)][j] = 0;
	    reg2[i][((long) sim_cycle % TIMEWINDOW)][j] = 0;
	}
#else
	reg1[i][((long) sim_cycle % TIMEWINDOW)] = 0;
	reg2[i][((long) sim_cycle % TIMEWINDOW)] = 0;
#endif
	reg3[i][((long) sim_cycle % TIMEWINDOW)] = 0;
	reg4[i][((long) sim_cycle % TIMEWINDOW)] = 0;
#ifdef OPTICAL_TENT
	optical_free ();
#endif
    }
}
#endif /* ! NEW_INTERCONNECT */
void
cache_change_config (int newsize)
{
    struct cache_t *tempcache;

    if (newsize == actual_clusters)
	return;

    fprintf (stderr, "Changing config from %d to %d\n", actual_clusters, newsize);
    flushes += (long) simple_cache_flush (cache_dl1);
    num_flushes++;

    tempcache = cache_dl1;
    cache_dl1 = cache_create (tempcache->name, tempcache->nsets, (original_bsize * newsize) / CLUSTERS,
			      /* balloc */ FALSE, /* usize */ 0, tempcache->assoc,
			      tempcache->policy, dl1_access_fn, /* hit lat */ cache_dl1_lat, 0);
    cache_dl1->bus_free = tempcache->bus_free;
    cache_dl1->hits = tempcache->hits;
    cache_dl1->misses = tempcache->misses;
    cache_dl1->replacements = tempcache->replacements;
    cache_dl1->writebacks = tempcache->writebacks;
    cache_dl1->invalidations = tempcache->invalidations;
    cache_dl1->rdb = tempcache->rdb;
    cache_dl1->wrb = tempcache->wrb;
    cache_dl1->pipedelay = tempcache->pipedelay;
    cache_dl1->wrbufsize = tempcache->wrbufsize;
    cache_dl1->lastuse = tempcache->lastuse;

    actual_cacheports = (CACHEPORTS * newsize) / CLUSTERS;
    return;
}

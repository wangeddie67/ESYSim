/*
 * Implements Token Coherenece with the "TokenB" 
 * protocol described by Martin, Hill, and Wood at ISCA'03
 *
 * $Log: tokencoherence.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:11  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:51  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.11  2004/08/19 20:56:21  grbriggs
 * Split up some corner cases. A few other corner case improvements too.
 *
 * Revision 1.10  2004/08/06 21:46:30  grbriggs
 * Some good improvements on the corner cases
 *
 * Revision 1.9  2004/08/06 18:10:49  grbriggs
 * Fix corner cases. Describe the ones that still exist.
 *
 * Revision 1.8  2004/08/04 19:20:26  grbriggs
 * Still tweaking those corner cases.
 *
 * Revision 1.7  2004/08/03 22:07:30  grbriggs
 * Reduce corner cases, improve accuracy.
 *
 * Revision 1.6  2004/08/02 23:57:36  grbriggs
 * Allow you to turn off those debug messages.
 *
 * Revision 1.5  2004/08/02 21:54:52  grbriggs
 * Allow it to forget the past instead of saving it all.
 * Make a few corner cases closer to "correct".
 *
 * Revision 1.4  2004/07/29 05:12:54  grbriggs
 * Version Alpha Preview 2, with many more corner cases non-fatal.
 *
 * Revision 1.3  2004/07/28 19:14:09  grbriggs
 * This is version Alpha Preview 1 :-)
 *
 * Revision 1.2  2004/07/28 04:54:38  grbriggs
 * Still a work in progress.
 *
 */

//#define TC_DEBUG

#include "headers.h"

#define COMMITPREDBIAS 230 /* can't go at all close to TIMEWINDOW with this, or ring_lat will fatal! */

#define CANREAD(blockinfo)	(blockinfo && \
				blockinfo->valid && \
				(blockinfo->i_am_owner || blockinfo->nonowner_token_count))
#define CANWRITE(blockinfo)	(blockinfo && \
				blockinfo->valid && \
				blockinfo->i_am_owner && \
				(blockinfo->nonowner_token_count == TC_TOTAL_NONOWNER_TOKEN_COUNT))

#define CHECK_CACHE_NUM(n)	if(((n)<0) || ((n)>7)) fatal("cache number %d is outside range",n)
//#define CHECK_CACHE_NUM(n)	if(((n)<0) || ((n)>7)) fatal("cache number %d is outside range",n)
#define CHECK_CACHE_NUM2(n)	if(((n)<0) || ((n)>7)) fatal("cache number %d is outside range",n)

//#define CHECK_CACHE_NUM(n)	if(((n)<0) || ((n)>7)) {corner_case_count[17]++; return;}
//#define CHECK_CACHE_NUM2(n)	if(((n)<0) || ((n)>7)) {corner_case_count[17]++; return 0;}


#if 0
tc_arbiter sharedmemoryarbiter[TIMEWINDOW];
#endif
//int cache_blockinfo_table_num_caches;
//int cache_blockinfo_table_num_blocks;
#if 0
counter_t farthest_prediction = 0; //sim_cycle that has been predicted out to
counter_t oldest_prediction = 0; //for de-allocation purposes
#endif

/* Proto */
tc_blockinfo * block_predict_alloc(md_addr_t addr, int cache_number, counter_t t);
void adjust_pipeline_usage(md_addr_t addr,int cache_num, int is_write, int adjust, counter_t cycle, int casefuture, int casemissing);

/* Statistics */

#define CORNER_CASES 51
counter_t corner_case_count[CORNER_CASES];
counter_t total_delays_added = 0;
counter_t commitpredict_error = 0;
counter_t commitpredict_abs_error = 0;
counter_t yield_for_excl = 0;
counter_t block_alloc_count = 0;
counter_t block_free_count = 0;
counter_t block_read_count = 0;
counter_t block_write_count = 0;
counter_t block_ready_count = 0;
counter_t commit_count = 0;
struct stat_stat_t *tc_delay_dist = NULL;
struct stat_stat_t *tc_twprox_dist = NULL;
struct stat_stat_t *tc_futureconflict_dist = NULL;
struct stat_stat_t *tc_cornercase_dist = NULL;

int tc_futureconflict_max = 0;


void tc_reg_stats(struct stat_sdb_t *sdb) {
	  stat_reg_counter(sdb, "tc_corner_case_count0",
		   ":-( earliest blockinfo modified twice in same cycle",
		   &corner_case_count[0], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count1",
		   "blockinfo only exists in the future. tc_block_alloc",
		   &corner_case_count[1], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count2",
		   "new blockinfo exists in the future. tc_access(read)",
		   &corner_case_count[2], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count3",
		   ":-( lost owner token while 'write in pipeline' flagged (counted for every time frame)",
		   &corner_case_count[3], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count4",
		   ":-( lost all tokens while 'read in pipeline' flagged (counted for every time frame)",
		   &corner_case_count[4], 0, NULL);
	  /*stat_reg_counter(sdb, "tc_corner_case_count5",
		   "",
		   &corner_case_count[5], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count6",
		   "",
		   &corner_case_count[6], 0, NULL); fixed*/
	  stat_reg_counter(sdb, "tc_corner_case_count7",
		   "a block would have been given more tokens than the max. this was detected, and the appropriate number of tokens were un-sent",
		   &corner_case_count[7], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count8",
		   "same as case 7, except counted for every future time instance in which there would be more than the max",
		   &corner_case_count[8], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count9",
		   "did a very fake persisitant request for a write (gave self an owner token)",
		   &corner_case_count[9], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count10",
		   "did part of a very fake persistant request for a read (gave self valid bit)",
		   &corner_case_count[10], 0, NULL); 
	  stat_reg_counter(sdb, "tc_corner_case_count11",
		   "did part of a very fake persistant request for a read (gave self a token)",
		   &corner_case_count[11], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count12",
		   "SimpleScalar updated it's cache in the wrong order, so we had to get a cache block from the future.",
		   &corner_case_count[12], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count13",
		   "a block gave away more owner tokens than it had to give, because it didn't know it would have to give them soon enough",
		   &corner_case_count[13], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count14",
		   ":-( received an owner token, but already had the owner token",
		   &corner_case_count[14], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count15",
		   ":-( gave out an owner token, but did not have it",
		   &corner_case_count[15], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count16",
		   "instruction that was supposedly in the pipeline never committed. this is counted each time we try to check the commit time, not per uncommitted instruction",
		   &corner_case_count[16], 0, NULL);
	  /*stat_reg_counter(sdb, "tc_corner_case_count17",
		   "",
		   &corner_case_count[17], 0, NULL); this is a fatal instead */
	  stat_reg_counter(sdb, "tc_corner_case_count18",
		   "Could not find blockinfo for main memory",
		   &corner_case_count[18], 0, NULL); 
	  stat_reg_counter(sdb, "tc_corner_case_count19",
		   "tried to give tokens to a block that doesn't exist yet !",
		   &corner_case_count[19], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count20",
		   "attempt to predict commit before block is in cache",
		   &corner_case_count[20], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count21",
		   "any blockinfo modified twice in same cycle (like case zero)",
		   &corner_case_count[21], 0, NULL); 
	  stat_reg_counter(sdb, "tc_corner_case_count22",
		   "SimpleScalar accessed a cache block but never loaded into the cache?",
		   &corner_case_count[22], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count23",
		   "tc_block_free called on nonexistant block",
		   &corner_case_count[23], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count24",
		   "attempted to travel backwards thru time by using info from future to save information into the past. fixed by redirecting the save into the future",
		   &corner_case_count[24], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count25",
		   "attempt to predict commit before block is in cache, but at least we found a future block to use",
		   &corner_case_count[25], 0, NULL);
	  /*stat_reg_counter(sdb, "tc_corner_case_count26",
		   "new blockinfo exists in the future",
		   &corner_case_count[26], 0, NULL); fixed*/
	  stat_reg_counter(sdb, "tc_corner_case_count27",
		   "new blockinfo exists in the future. during block alloc",
		   &corner_case_count[27], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count28",
		   "new blockinfo exists in the future. during main memory block alloc",
		   &corner_case_count[28], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count29",
		   "new blockinfo exists in the future. during block predict alloc",
		   &corner_case_count[29], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count30",
		   "new blockinfo exists in the future. during block free",
		   &corner_case_count[30], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count31",
		   "new blockinfo exists in the future. when considering main memory for block free",
		   &corner_case_count[31], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count32",
		   "new blockinfo exists in the future. tc_access(write)",
		   &corner_case_count[32], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count33",
		   "simplescalar is putting the same block into the same cache a second time?",
		   &corner_case_count[33], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count34",
		   "simplescalar freed a block that was already freed",
		   &corner_case_count[34], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count35",
		   "attempt to de-predict commit before block is in cache, but at least we found a future block to use (see #25)",
		   &corner_case_count[35], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count36",
		   "attempt to de-predict commit before block is in cache (see #20)",
		   &corner_case_count[36], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count37",
		   "attempt to real commit before block is in cache, but at least we found a future block to use",
		   &corner_case_count[37], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count38",
		   "attempt to real commit before block is in cache",
		   &corner_case_count[38], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count39",
		   "lost owner token while 'write in pipeline' flagged (not counted for every time frame)",
		   &corner_case_count[39], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count40",
		   "lost all tokens while 'read in pipeline' flagged (not counted for every time frame)",
		   &corner_case_count[40], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count41",
		   "attempt to issue before block is in cache, but at least we found a future block to use",
		   &corner_case_count[41], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count42",
		   "attempt to issue before block is in cache",
		   &corner_case_count[42], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count43",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[43], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count44",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[44], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count45",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[45], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count46",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[46], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count47",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[47], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count48",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[48], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count49",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[49], 0, NULL);
	  stat_reg_counter(sdb, "tc_corner_case_count50",
		   "blockinfo only exists in the future. ",
		   &corner_case_count[50], 0, NULL);


	  tc_cornercase_dist = stat_reg_sdist(sdb, "tc_cornercase_dist",
                              "Misc. Corner Cases",
                              /* initial value */0,
                              /* print format */PF_ALL,
                              /* format */NULL,
                              /* print fn */NULL);

	  stat_reg_counter(sdb, "tc_total_delays_added",
		   "total delay due to token passing comm",
		   &total_delays_added, 0, NULL);
	  stat_reg_counter(sdb, "tc_yield_for_excl",
		   "Number of times a block yielded all tokens to give someone else Excl. access",
		   &yield_for_excl, 0, NULL);

	  stat_reg_counter(sdb, "tc_block_alloc_count",
		   "Number of times a block was alloced",
		   &block_alloc_count, 0, NULL);

	  stat_reg_counter(sdb, "tc_block_free_count",
		   "Number of times a block was freeed",
		   &block_free_count, 0, NULL);

	  stat_reg_counter(sdb, "tc_block_read_count",
		   "Number of times a block was read",
		   &block_read_count, 0, NULL);

	  stat_reg_counter(sdb, "tc_block_write_count",
		   "Number of times a block was written",
		   &block_write_count, 0, NULL);
	  
	  stat_reg_counter(sdb, "tc_block_ready_count",
		   "Number of times a block already had the right tokens for the operation (zero delay needed)",
		   &block_ready_count, 0, NULL);

	  stat_reg_counter(sdb, "tc_commit_count",
		   "Number of loads/stores committed",
		   &commit_count, 0, NULL);


	  stat_reg_counter(sdb, "tc_commitpredict_abs_error",
		   "Accumulated (absolute) errors in commit time prediction",
		   &commitpredict_abs_error, 0, NULL);
	  stat_reg_formula(sdb, "avg_tc_commitpredict_abs_error",
		   "Error in commit time prediction, avg per instruction",
		   "tc_commitpredict_abs_error / tc_commit_count", NULL);

	  stat_reg_counter(sdb, "tc_commitpredict_error",
		   "Accumulated errors in commit time prediction",
		   &commitpredict_error, 0, NULL);
	  stat_reg_formula(sdb, "avg_tc_commitpredict_error",
		   "Error in commit time prediction, avg per instruction",
		   "tc_commitpredict_error / tc_commit_count", NULL);
	  
	  tc_delay_dist = stat_reg_dist(sdb, "tc_delays_added",
                              "Distribution of delays added",
                              /* initial value */0,
                              /* array size */26,
                              /* bucket size */50,
                              /* print format */PF_ALL,
                              /* format */NULL,
                              /* index map */NULL,
                              /* print fn */NULL);


	  tc_twprox_dist = stat_reg_dist(sdb, "tc_twprox_dist",
                              "TIMEWINDOW proximity (bad as things get closer to 5000)",
                              /* initial value */0,
                              /* array size */26,
                              /* bucket size */200,
                              /* print format */PF_ALL,
                              /* format */NULL,
                              /* index map */NULL,
                              /* print fn */NULL);
#if 1
	  tc_futureconflict_dist = stat_reg_dist(sdb, "tc_futureconflict_dist",
                              "When getblockinfo ignores change information that exists in the future, this is how far into the future the info was",
                              /* initial value */0,
                              /* array size */41,
                              /* bucket size */100,
                              /* print format */PF_ALL,
                              /* format */NULL,
                              /* index map */NULL,
                              /* print fn */NULL);
#else
	  tc_futureconflict_dist = stat_reg_sdist(sdb, "tc_futureconflict_dist",
                              "When getblockinfo ignores change information that exists in the future, this is how far into the future the info was.",
                              /* initial value */0,
                              /* print format */PF_ALL,
                              /* format */NULL,
                              /* print fn */NULL);
#endif
	  
  	  stat_reg_int(sdb, "tc_futureconflict_max",
	       "max value in tc_futureconflict_dist",
	       &tc_futureconflict_max, 0, NULL);
}


hashtable_entry ** hashtable;
int hashtable_size;
md_addr_t tc_blockaddr_mask;
int cache_block_size;
#define SHAREDMEM_NUMBER (-1)
#define HASHFN(key)						\
  (((key >> 24) ^ (key >> 16) ^ (key >> 8) ^ key) & (hashtable_size-1))

void put_blockinfo(md_addr_t addr, int cache_number, tc_blockinfo * blockinfo, counter_t cycle) {
	int pos = HASHFN(addr);
	hashtable_entry * hte;
	if ((pos>=hashtable_size) || (pos<0)) fatal("bad hash function");
	hte = hashtable[pos];

	//find matching entry
	while (hte && !(hte->addr == addr && hte->cache_number == cache_number))
		hte=hte->next;

	if (!hte) {
		//add new entry to front of hash bucket
		hte = (hashtable_entry *) malloc(sizeof(hashtable_entry));
		if (!hte) fatal("out of memory");
		hte->addr = addr;
		hte->cache_number = cache_number;
		hte->blockinfo_head = NULL;
		hte->next = hashtable[pos];
		hashtable[pos] = hte;
	}

	//now we have an hte for this address and cache_number

	//the blockinfo linked lists are ordered from oldest info at head to newest at tail
	if (blockinfo->state_as_of == -1)
		blockinfo->state_as_of = cycle;
	if (blockinfo->state_as_of != cycle)
		fatal("block has old timestamp");

	if (!hte->blockinfo_head) {
		//it was empty, add a single entry
		hte->blockinfo_head = blockinfo;
		blockinfo->next = NULL;
	} else if (hte->blockinfo_head->state_as_of > cycle) {
		//the whole list is newer than this, so add this to the head
		if (blockinfo == hte->blockinfo_head) fatal("infinite loop");
		blockinfo->next = hte->blockinfo_head;
		hte->blockinfo_head = blockinfo;
	} else {
		//need to skip past older entries
		tc_blockinfo * prev = hte->blockinfo_head; //we already know that prev is older than cycle
		while(prev->next && prev->next->state_as_of < cycle) {
			prev = prev->next;
		}
		if (hte->blockinfo_head->state_as_of == cycle) corner_case_count[0]++;
		if (prev->state_as_of ==  cycle) corner_case_count[21]++;

		//okay, now we want to insert into the linked list just after prev
		blockinfo->next = prev->next;
		prev->next = blockinfo;
	}

}

counter_t get_blockinfo_futureexistance(md_addr_t addr, int cache_number, counter_t cycle) {
	int pos = HASHFN(addr);
	hashtable_entry * hte; 
	if ((pos>=hashtable_size) || (pos<0)) fatal("bad hash function");
	hte = hashtable[pos];

	//find matching entry
	while (hte && !(hte->addr == addr && hte->cache_number == cache_number))
		hte=hte->next;

	if (!hte || !hte->blockinfo_head) {
		return 0;
	} 

	if (hte->blockinfo_head->state_as_of > cycle) {
		return hte->blockinfo_head->state_as_of;
	} else {
		fatal("this function only will tell you about the future");
	}
}

tc_blockinfo * get_blockinfo(md_addr_t addr, int cache_number, counter_t cycle, int future_conflict_corner_case, int future_only_case) {
	int pos = HASHFN(addr);
	hashtable_entry * hte; 
	if ((pos>=hashtable_size) || (pos<0)) fatal("bad hash function");
	hte = hashtable[pos];

	//sanity check
	if (cycle < sim_cycle)
		fatal("cannot access the past; it was erased (cycle=%lld, sim_cycle=%lld).",cycle,sim_cycle);

	//find matching entry
	while (hte && !(hte->addr == addr && hte->cache_number == cache_number))
		hte=hte->next;

	if (!hte || !hte->blockinfo_head) {
		return NULL;
	} 

	if (hte->blockinfo_head->state_as_of > cycle) {
		//the whole list is newer than this
		if (future_only_case)
			corner_case_count[future_only_case]++; //newer information ignored
		return NULL;
	} else {
		//need to skip past older entries
		tc_blockinfo * prev; 


		/* mandatory cleaning of historic information.
		 * it's safe to clean if we have less old information 
		 * that is always relavent (i.e., not in the future) */
		while (	hte->blockinfo_head->next && 
			hte->blockinfo_head->state_as_of < sim_cycle &&
			hte->blockinfo_head->next->state_as_of < sim_cycle) {
			prev = hte->blockinfo_head;
			hte->blockinfo_head = prev->next;
			free(prev);
		}

		prev = hte->blockinfo_head; //we already know that prev is older than cycle
		while(prev->next && prev->next->state_as_of < cycle) {
			prev = prev->next;
		}

		if (prev->next && future_conflict_corner_case) {
			tc_blockinfo * future = prev;
			int important_info_ignored = 0;
			counter_t future_cycle = 0;
		
			/* what kind of newer information?? */
			while(future->next) {
			    if (future_conflict_corner_case == 2 /* special read-access hint used by tc_access*/) {
			    	if (CANREAD(future) != CANREAD(future->next)) {
					important_info_ignored++;
					if (!future_cycle) future_cycle = future->state_as_of;
				}
			    } else if (future_conflict_corner_case == 32 /* special write-access hint used by tc_access*/) {
				if (CANWRITE(future) != CANWRITE(future->next)) {
					important_info_ignored++;
					if (!future_cycle) future_cycle = future->state_as_of;
				}
			    } else {
				if (	(CANREAD(future) != CANREAD(future->next)) ||
					(CANWRITE(future) != CANWRITE(future->next))) {
					important_info_ignored++;
					if (!future_cycle) future_cycle = future->state_as_of;
				}
			    }
			    future=future->next;
			}
			
			if (important_info_ignored) {
				corner_case_count[future_conflict_corner_case]++; //newer information ignored
				stat_add_sample(tc_futureconflict_dist, future_cycle - prev->state_as_of);
				if (future_cycle - prev->state_as_of > tc_futureconflict_max) 
					tc_futureconflict_max = future_cycle - prev->state_as_of;
			}
		}

		return prev;
	}

}


tc_blockinfo * get_blockinfo_modifynow(tc_blockinfo * blockinfo, counter_t cycle) {
	tc_blockinfo * newblock;

	if (blockinfo->state_as_of > cycle) {
		//fatal("attempted to travel backwards thru time");
		corner_case_count[24]++;
		return blockinfo;
	}
	
	/* find the right point in time */
	while(blockinfo->next && (blockinfo->next->state_as_of < cycle))
		blockinfo = blockinfo->next;

	
	if (blockinfo->state_as_of == cycle) {
		/* we already are at exactly the right time */
		return blockinfo;
	}

	newblock = malloc(sizeof(tc_blockinfo));
	if (!newblock) fatal("out of memory");

	newblock->valid = blockinfo->valid;
	newblock->i_am_owner = blockinfo->i_am_owner;
	newblock->nonowner_token_count = blockinfo->nonowner_token_count;
	newblock->addr = blockinfo->addr;
	newblock->read_in_pipeline = blockinfo->read_in_pipeline;
	newblock->write_in_pipeline = blockinfo->write_in_pipeline;
	newblock->modified = blockinfo->modified;
	newblock->exists = blockinfo->exists;

	newblock->state_as_of = cycle;
	newblock->next=blockinfo->next;
	blockinfo->next=newblock;
	if (blockinfo->next->state_as_of < blockinfo->state_as_of)
		fatal("mistakenly attached new block in the wrong place");
	return newblock;
}

/* returns the number of non_owner tokens that it failed to give */
int give_tokens(int nonowner_tokens,int owner_token,int has_data,md_addr_t addr, int cache_num,counter_t resp_time, int cc) {
	tc_blockinfo * blockinfo = get_blockinfo(addr,cache_num,resp_time,0,0); 
					//we dont mind if future blocks exist, we're going to visit them explicitly

	int max_failed = 0;
	int case3=0;
        int case4=0;

	if (!blockinfo) {
	
		/* this corner case is simple scalar's fault, for doing things in the wrong order */
		counter_t future_cycle = get_blockinfo_futureexistance(addr,cache_num,resp_time);

		if (future_cycle) {
			blockinfo = get_blockinfo(addr,cache_num,future_cycle,0,0); //see above note
			if (blockinfo) corner_case_count[50]++;
		} 
		if (!blockinfo) {
			corner_case_count[19]++;
			return 0;
		}

	}

	/* NICK- you can delay token giving with this:

	while (blockinfo->next &&
			((owner_token==-1 && blockinfo->write_in_pipeline) ||
			 (blockinfo->nonowner_token_count + nonowner_tokens <= 0 && (blockinfo->i_am_owner + owner_token <= 0) && blockinfo->read_in_pipeline))) {
		blockinfo=blockinfo->next;
		if (resp_time < blockinfo->state_as_of)
			resp_time=blockinfo->state_as_of;
	}

	TODO - somehow return this delay value, so that the receiver of the tokens given will actually experience the delay too. see tc_Access for how give_tokens is used.
	*/

		
	if (blockinfo->state_as_of != resp_time)
		blockinfo = get_blockinfo_modifynow(blockinfo,resp_time);

	while(blockinfo) {
		blockinfo->nonowner_token_count += nonowner_tokens;
		if (blockinfo->nonowner_token_count < 0) {
			corner_case_count[13]++;
			//fatal("lost some tokens");
			blockinfo->nonowner_token_count = 0; //FIXME?
		}
		if (blockinfo->nonowner_token_count > TC_TOTAL_NONOWNER_TOKEN_COUNT) {
			//fatal("too many tokens");
			int extra = (blockinfo->nonowner_token_count - TC_TOTAL_NONOWNER_TOKEN_COUNT);
			if (extra > max_failed) max_failed=extra;
			blockinfo->nonowner_token_count = TC_TOTAL_NONOWNER_TOKEN_COUNT;
			//FIXME: this might not be quite right
			corner_case_count[8]++;
		}
		
		if (owner_token==1) {
			if (blockinfo->i_am_owner) corner_case_count[14]++; //fatal("duplicate owner token?");
			blockinfo->i_am_owner = 1;
		} else if (owner_token==-1) {
			if (!blockinfo->i_am_owner) corner_case_count[15]++; //fatal("removed owner token, but did not have it");
			if (blockinfo->write_in_pipeline) {
				corner_case_count[3]++; 
				stat_add_sample(tc_cornercase_dist, cc);
				case3++;
			}
			blockinfo->i_am_owner = 0;
		}

		if (has_data && ((nonowner_tokens>0)||(owner_token==1))) {
			blockinfo->valid=1;
		} else if (!blockinfo->nonowner_token_count && !blockinfo->i_am_owner) {
			blockinfo->valid=0;
		}

		if (blockinfo->read_in_pipeline && !(blockinfo->i_am_owner || blockinfo->nonowner_token_count)) {
			corner_case_count[4]++;
			stat_add_sample(tc_cornercase_dist, cc+1);
			case4++;
		}

		blockinfo=blockinfo->next;
	}

	if(max_failed) corner_case_count[7]++;
	if(case3) {
		corner_case_count[39]++;
	} else if(case4) {
		corner_case_count[40]++;
	}
	return max_failed;
}

extern int spec_benchmarks;
void tc_commit_predict(md_addr_t addr,int cache_num, int is_write, counter_t cycle, counter_t issue_cycle) {

	if (cycle < sim_cycle)
		fatal("can't commit_predict in the past");
	if (issue_cycle < sim_cycle)
		fatal("why are we issuing in the past??");
	if (issue_cycle > cycle)
		fatal("can't commit before issue");

#ifdef TC_DEBUG
	printf("commit predict cycle=%lld, cache=%d,addr=%llx\n",cycle,cache_num,addr);
#endif

	cycle += COMMITPREDBIAS;
	adjust_pipeline_usage(addr,cache_num, is_write, 1, issue_cycle, 41, 42);
	adjust_pipeline_usage(addr,cache_num, is_write, -1, cycle, 25, 20);
}
	
void adjust_pipeline_usage(md_addr_t addr,int cache_num, int is_write, int adjust, counter_t cycle, int casefuture, int casemissing) {
	tc_blockinfo * blockinfo;
	CHECK_CACHE_NUM(cache_num);
	
	addr &= tc_blockaddr_mask;
	blockinfo = get_blockinfo(addr,cache_num,cycle,0,0);
					//we dont mind if future blocks exist, we're going to visit them explicitly

	if (!blockinfo) {
		/* this corner case is simple scalar's fault, for doing things in the wrong order */
		counter_t future_cycle = get_blockinfo_futureexistance(addr,cache_num,cycle);

		if (future_cycle) {
			blockinfo = get_blockinfo(addr,cache_num,future_cycle,0,0);
			corner_case_count[casefuture]++;
		}
	}
	if (!blockinfo) {
		//fatal("attempt to predict commit before block is in cache");
		corner_case_count[casemissing]++;
		
		blockinfo = block_predict_alloc(addr,cache_num,cycle);
		//return;
	}
	
	if (blockinfo->state_as_of != cycle)
		blockinfo = get_blockinfo_modifynow(blockinfo,cycle);

	while(blockinfo) {

		if (is_write)
			blockinfo->write_in_pipeline+=adjust;
		else
			blockinfo->read_in_pipeline+=adjust;

		blockinfo=blockinfo->next;
	}

}

void tc_commit_real(md_addr_t addr,int cache_num, int is_write, counter_t predicted_cycle, counter_t real_cycle) {
	addr &= tc_blockaddr_mask;
	CHECK_CACHE_NUM(cache_num);
	predicted_cycle += COMMITPREDBIAS;

	//if (predicted_cycle == 0)
	//	fatal("predicted_cycle is 0");
	if (real_cycle < sim_cycle)
		fatal("can't commit_real in the past");

	//TODO: make this a less severe problem?


	/* update the commit info. we don't know it's impact yet. */
						 /* we don't want to update the past, so we use sim_cycle instead if needed */
	adjust_pipeline_usage(addr,cache_num, is_write, 1, predicted_cycle<sim_cycle?sim_cycle:predicted_cycle, 35, 36);
	adjust_pipeline_usage(addr,cache_num, is_write, -1, real_cycle, 37, 38);

	//TODO: how to track the inaccuracy so we know how bad it is?

	commitpredict_error += (predicted_cycle-real_cycle);
	commitpredict_abs_error += (predicted_cycle>real_cycle?predicted_cycle-real_cycle:real_cycle-predicted_cycle);
	commit_count++;
}

void tc_init(int num_caches, int num_blocks, int block_size) {
	int i;

	for (i=0;i<CORNER_CASES;i++)
		corner_case_count[i]=0;

	hashtable_size = num_caches*num_blocks;
	hashtable = (hashtable_entry **) calloc(hashtable_size,sizeof(hashtable_entry *));
	if (!hashtable)
		fatal("Could not allocate %d bytes for hash table.",hashtable_size*sizeof(hashtable_entry *));
	for (i=0;i<hashtable_size;i++)
		hashtable[i]=NULL;

	tc_blockaddr_mask = ~((md_addr_t) block_size - 1);
	cache_block_size = block_size;
}

tc_blockinfo * block_malloc(md_addr_t addr) {
	tc_blockinfo * newblock;
	newblock = malloc(sizeof(tc_blockinfo));
	if (!newblock) fatal ("out of memory");
	newblock->next = NULL;
	newblock->valid = 0;
	newblock->i_am_owner = 0;
	newblock->nonowner_token_count = 0;
	newblock->addr = addr;
#if 0
	newblock->read_starve_cycle = -1;
	newblock->write_starve_cycle = -1;
#endif
	newblock->read_in_pipeline = 0;
	newblock->write_in_pipeline = 0;
	newblock->modified = 0;
	newblock->exists = 1;

	newblock->state_as_of = -1;

	return newblock;
}

/*
void put_sharedmemory_block(md_addr_t addr, tc_blockinfo * sm_block, counter_t cycle);
tc_blockinfo * get_sharedmemory_block(md_addr_t addr,counter_t t);
void sharedmemory_block_refcount(tc_blockinfo * sm_block,int adjust,counter_t t);
*/

/* when a new block comes into cache, call this */
void tc_block_alloc(md_addr_t addr, int cache_number, counter_t t) {
	tc_blockinfo * sm_block;
	tc_blockinfo * blockinfo;
	CHECK_CACHE_NUM(cache_number);
/*
	if (!md_valid_addr(addr,0)) {
		if (spec_benchmarks) fatal("invalid addr");
		return;
	}
*/

	if (!addr) fatal("addr == 0");
	
	addr &= tc_blockaddr_mask;

	
	
#ifdef TC_DEBUG
	printf("block alloc cycle=%lld, cache=%d,addr=%llx\n",t,cache_number,addr);
#endif

	blockinfo = get_blockinfo(addr,cache_number,t,27,1);
	if (blockinfo && !blockinfo->exists) {
		blockinfo->exists = 1;
	} else if (blockinfo && blockinfo->exists) {
		//block is already in cache?? might be due to out of order free?
		blockinfo->exists++;
		corner_case_count[33]++;
	} else {
		blockinfo = block_malloc(addr);
		put_blockinfo(addr,cache_number,blockinfo,t);
	}

	sm_block = get_blockinfo(addr,SHAREDMEM_NUMBER,t,28,43);
	if (!sm_block) {
		sm_block = block_malloc(addr);
		sm_block->valid = 1;
		sm_block->i_am_owner = 1;
		sm_block->nonowner_token_count = TC_TOTAL_NONOWNER_TOKEN_COUNT;
		sm_block->addr = addr;
		put_blockinfo(addr,SHAREDMEM_NUMBER,sm_block,t);
	} 
		
	block_alloc_count++;
}

tc_blockinfo * block_predict_alloc(md_addr_t addr, int cache_number, counter_t t) {
	tc_blockinfo * blockinfo;
	CHECK_CACHE_NUM(cache_number);
	addr &= tc_blockaddr_mask;

	blockinfo = get_blockinfo(addr,cache_number,t,29,44);
	if (blockinfo) {
		fatal("block_predict_alloc should not have been called, alloc already done");
	} else {
		blockinfo = block_malloc(addr);
		blockinfo->exists=0;
		put_blockinfo(addr,cache_number,blockinfo,t);
	}
	return blockinfo;
}

/* when block kicked out of cache, call this */
/* usage note: You had better be doing a block replacement,
 * because leaving nulls in our state table does not enforce ordering correctly */
void tc_block_free(md_addr_t addr, int cache_number, counter_t cycle)
{
	tc_blockinfo * block;
	tc_blockinfo * sm_block;
	CHECK_CACHE_NUM(cache_number);
/*
	if (!md_valid_addr(addr,0)) {
		if (spec_benchmarks) fatal("invalid addr");
		return;
	}
*/
	if (!addr) fatal("addr == 0");
	
	addr &= tc_blockaddr_mask;

#ifdef TC_DEBUG
	printf("block free cycle=%lld, cache=%d, addr=%llx\n",cycle,cache_number,addr);
#endif
	block_free_count++;

	block = get_blockinfo(addr,cache_number,cycle,30,45);
	if (!block) {
		corner_case_count[23]++;
		return;
		//fatal("can't find the cache node block info");
	}
	sm_block = get_blockinfo(addr,SHAREDMEM_NUMBER,cycle,31,46);
	if (!sm_block) {
		fatal("can't find the memory node block info; cycle=%lld, cache=%d, addr=%llx",cycle,cache_number,addr);
	} else if (block->addr != sm_block->addr) {
		fatal("address mismatch");
	} else {
		counter_t resp_time;
		
		sm_block=get_blockinfo_modifynow(sm_block,cycle);
		if (block->i_am_owner) {
			sm_block->i_am_owner = 1;
			sm_block->valid = 1;
		}
		sm_block->nonowner_token_count += block->nonowner_token_count;
		
		//TODO: free sm_block if unused

		//free(block);
		//TODO: free block
		if(block->exists) {
			block->exists--;
		} else {
			corner_case_count[34]++;
		}
		
		/* here we send away all the tokens */
		//TODO: use FE lat macro
		resp_time = cycle + ring_lat(cache_number*2, 0, 0, cycle - sim_cycle);

		give_tokens(-block->nonowner_token_count,
			block->i_am_owner?-1:0,0,addr,cache_number,cycle,0);
		give_tokens(block->nonowner_token_count,
			block->i_am_owner?1:0,block->valid,addr,SHAREDMEM_NUMBER,resp_time,2);

	}
}


/* if is_write is nonzero, then it's a write instead of a read */
/* if is_hint is nonzero, then it's only a hint. hints still cost bandwidth of course */
/* returns latency */
int tc_access(md_addr_t addr, int cache_number,int cluster,  int is_write, int is_hint, counter_t cycle) {
	tc_blockinfo *blockinfo;
	int delay;
	int old_ccc_2 = corner_case_count[2];
	int old_ccc_32 = corner_case_count[32];
	CHECK_CACHE_NUM(cache_number);

	if (!addr) fatal("addr == 0");
/*
	if (!md_valid_addr(addr,0)) {
		if (spec_benchmarks) fatal("invalid addr");
		return 0;
	}
*/
	addr &= tc_blockaddr_mask;
	blockinfo = get_blockinfo(addr,cache_number,cycle,is_write?32:2,47); //note 2 and 32 are "Special", see the function
	
#ifdef TC_DEBUG
	printf("%s access cycle=%lld, cache=%d,addr=%llx\n",is_write?"write":"read",cycle,cache_number,addr);
#endif

	if(is_write) {
		++block_write_count;
	} else {
		++block_read_count;
	}

	if (!blockinfo) {
		/* this corner case is simple scalar's fault, for doing things in the wrong order */
		counter_t future_cycle = get_blockinfo_futureexistance(addr,cache_number,cycle);

		if (future_cycle) {
			blockinfo = get_blockinfo(addr,cache_number,future_cycle,is_write?32:2,48); //see above note
			corner_case_count[12]++;
		}
	}
#if 0
	if (!blockinfo) {
		/* this corner case is simple scalar's fault, for doing things in the wrong order */
		blockinfo = get_blockinfo(addr,cache_number,cycle+200);
		corner_case_count[12]++;
	}
	if (!blockinfo) {
		/* this corner case is simple scalar's fault, for doing things in the wrong order */
		blockinfo = get_blockinfo(addr,cache_number,cycle+5001);
		corner_case_count[12]++;
	}
#endif
	if (!blockinfo) {
		//fatal("block not alloc'ed yet?");
		corner_case_count[22]++;
		delay = 0;
	} else if (addr != blockinfo->addr) {
		fatal("address mismatch");
	} else if (!is_write && CANREAD(blockinfo))
	{
		/*
		if (!is_hint) {
			if (blockinfo->state_as_of < cycle) blockinfo = get_blockinfo_modifynow(blockinfo,cycle);
			//++blockinfo->read_in_pipeline; //todo: propogate this forward in time instead of corner case
			//if (blockinfo->next) corner_case_count[5]++;
			while(blockinfo) {
				++blockinfo->read_in_pipeline;
				blockinfo=blockinfo->next;
			}
		}
		*/
		delay = 0; //we don't need to request anything

		/* we un-increment corner case future-warning, since we dont care about the future */
		corner_case_count[2] = old_ccc_2;

	} else if (is_write && CANWRITE(blockinfo))
	{
		/*
		if (!is_hint) {
			if (blockinfo->state_as_of < cycle) blockinfo = get_blockinfo_modifynow(blockinfo,cycle);
			//++blockinfo->write_in_pipeline;  //todo: propogate this forward in time instead of corner case
			//if (blockinfo->next) corner_case_count[6]++;
			while(blockinfo) {
				++blockinfo->write_in_pipeline;
				blockinfo=blockinfo->next;
			}
		}
		*/
		delay = 0; //we don't need to request anything

		/* we un-increment corner case future-warning, since we dont care about the future */
		corner_case_count[32] = old_ccc_32;

	} else  {

		//need to get a token+data from somebody
#if 0
		if (is_write) {
			if (blockinfo->write_starve_cycle == -1) {
				blockinfo->write_starve_cycle = sim_cycle + STARVE_CYCLES;
			}
		} else {
			if (blockinfo->read_starve_cycle == -1) {
				blockinfo->read_starve_cycle = sim_cycle + STARVE_CYCLES;
			}
		}
#endif

		{
			int i;
			counter_t arrivals[CLUSTERS];
			flood_lat(arrivals,cluster,cycle);
			/* careful -- how does cache_number correspond to cluster? */
			for (i=-1;i<CLUSTERS/2;i++) { //FIXME: assumes 2 clusters per cache
				int cancelled_tokens=0;
				counter_t arrival;
				tc_blockinfo * responder;
				arrival = (i==-1?COMM_CL_TO_FE_LAT(cluster)+cycle:MIN(arrivals[i*2],arrivals[i*2+1]));
				responder = get_blockinfo(addr,i,arrival,0,49); /* used to be corner case 26. however,
										give_tokens can deal with future blocks existing,
										and we don't care about the future below,
										so we don't need to call it a corner case here */
				if (i==-1 && !responder) {
					corner_case_count[18]++;
					return 0;
					//fatal("Could not find blockinfo for main memory");
				}
				if (responder && i !=cache_number) {
					int has_data;
					//responder = get_blockinfo_modifynow(responder,arrival);

#define STATE_S (responder->nonowner_token_count && !responder->i_am_owner)
#define STATE_O	((responder->nonowner_token_count < TC_TOTAL_NONOWNER_TOKEN_COUNT) && responder->i_am_owner)
#define STATE_M	((responder->nonowner_token_count == TC_TOTAL_NONOWNER_TOKEN_COUNT) && responder->i_am_owner)


					if (is_write) { /* Excl request (for write) */
						//tc_ack_excl_request(responder,arrival);
						if (STATE_S || STATE_O || STATE_M) {
						/* state S: send all tokens, but no data  */
						/* state O or M: send all tokens plus data */
							counter_t resp_time;

							has_data = !STATE_S;

							while (responder->write_in_pipeline || responder->read_in_pipeline) {
								if (!responder->next) {
									//fatal("instruction never committed");
									corner_case_count[16]++;
									break;
								} else {
									responder = responder->next;
								}
								arrival = responder->state_as_of;
							}
							
							if (i == -1) 
								resp_time = arrival + COMM_FE_TO_CL_LAT_DELAYED(cluster,arrival-sim_cycle);
							else if (has_data)
								resp_time = arrival +
									cl_comm_lat(i*2, cluster, arrival - sim_cycle, cache_block_size+8, data_token);
							else
								resp_time = arrival +
									cl_comm_lat(i*2, cluster, arrival - sim_cycle, 8, bare_token);
							
							give_tokens(-responder->nonowner_token_count,
									responder->i_am_owner?-1:0,0,addr,i,arrival,4);
							cancelled_tokens = give_tokens(responder->nonowner_token_count,
									responder->i_am_owner?1:0,has_data,addr,cache_number,resp_time,6);
							if (cancelled_tokens) give_tokens(cancelled_tokens,0,0,addr,i,arrival,8);
							yield_for_excl++;
						} else {
							//state I
						}

					} else { /* Share request (for read) */
						//tc_ack_share_request(responder,arrival);
						
						if (STATE_S) {
							/* state S: do nothing */
						} else if (STATE_O || (STATE_M && !responder->modified)) {
							counter_t resp_time;
							
							/* send one token plus data */
							while (responder->write_in_pipeline || /* if we want to write, or */
								(responder->read_in_pipeline && /* if we want to read          */
								 !((responder->nonowner_token_count >= 2) /* but we dont have 2 tokens */
								    ||(responder->nonowner_token_count && responder->i_am_owner)))) { 
								//the above allows read to proceed w/o delay if enough extra tokens

								if (!responder->next) {
									//fatal("instruction never committed");
									corner_case_count[16]++;
									break;
								} else {
									responder = responder->next;
								}
								arrival = responder->state_as_of;
							}
							
							if (i == -1) 
								resp_time = arrival + COMM_FE_TO_CL_LAT_DELAYED(cluster,arrival-sim_cycle);
							else resp_time = arrival +
									cl_comm_lat(i*2, cluster, arrival - sim_cycle,cache_block_size+8,data_token );
							
							if(responder->nonowner_token_count) {
								give_tokens(-1,0,0,addr,i,arrival,10);
								cancelled_tokens = give_tokens(1,0,1,addr,cache_number,resp_time,12);
								if (cancelled_tokens) give_tokens(cancelled_tokens,0,0,addr,i,arrival,14);
							} else if (responder->i_am_owner) {
								give_tokens(0,-1,0,addr,i,arrival,16);
								give_tokens(0,1,1,addr,cache_number,resp_time,18);
							}
						} else if (STATE_M && responder->modified) {
							counter_t resp_time;
							/* if modified, send all tokens and data.*/
							while (responder->write_in_pipeline || responder->read_in_pipeline) {
								if (!responder->next) {
									//fatal("instruction never committed");
									corner_case_count[16]++;
									break;
								} else {
									responder = responder->next;
								}
								arrival = responder->state_as_of;
							}
							
							if (i == -1) 
								resp_time = arrival + COMM_FE_TO_CL_LAT_DELAYED(cluster,arrival-sim_cycle);
							else resp_time = arrival +
									cl_comm_lat(i*2, cluster, arrival - sim_cycle,cache_block_size+8,data_token);
							
							give_tokens(-responder->nonowner_token_count,
									-1,0,addr,i,arrival,20);
							cancelled_tokens = give_tokens(responder->nonowner_token_count,
									1,1,addr,cache_number,resp_time,22);
							if (cancelled_tokens) give_tokens(cancelled_tokens,0,0,addr,i,arrival,24);
						} else {
							//State I
						}


					}
				}
			}
		}

		{ //pre_scan_cycle
		counter_t pre_scan_cycle = blockinfo->state_as_of;

		//now scan thru time to discover delay
		while(!(is_write?CANWRITE(blockinfo):CANREAD(blockinfo))){
			if (!blockinfo->next) {
				//fatal("if only we had persistant request, we wouldnt have gotten stuck forever?");

				//add a persistent request penalty of 100 cycles. note that this type of
				// persistent request is caused by our foolish simulation method, not by a true stall
				if (blockinfo->state_as_of - cycle < 100) {
					blockinfo = get_blockinfo_modifynow(blockinfo,cycle + 100);
				}

				if (is_write) {
					blockinfo->i_am_owner = 1;
					blockinfo->nonowner_token_count = TC_TOTAL_NONOWNER_TOKEN_COUNT;
					blockinfo->valid=1;
					corner_case_count[9]++;
				} else { /* is read */
					if (!blockinfo->valid) {
						blockinfo->valid = 1;
						corner_case_count[10]++;
					} else {
						blockinfo->nonowner_token_count = 1;
						corner_case_count[11]++;
					}
				}
			} else {
				blockinfo=blockinfo->next;
			}
		}

		delay = blockinfo->state_as_of - cycle;

		if (delay < 0) {
			fatal("attempted to return a negative delay. "
				"blockinfo->state_as_of=%lld, cycle=%lld, pre_scan_cycle=%lld",
				blockinfo->state_as_of,cycle,pre_scan_cycle); 
		}
		} //pre_scan_cycle

		if (is_write) {
			//++blockinfo->write_in_pipeline;
			if (!is_hint)
				blockinfo->modified = 1;
		} /*else {
			++blockinfo->read_in_pipeline;
		}*/
	}

	total_delays_added += delay;
	stat_add_sample(tc_delay_dist, delay);
	stat_add_sample(tc_twprox_dist, cycle-sim_cycle);

	if (delay == 0)
		block_ready_count++;

	return delay;
}

#if 0
void arbiter_handle_activation_request(int from_cluster, md_addr_t addr, tc_arbiter * arbiter) {
	if (!arbiter->head) {
		arbiter->head = malloc(sizeof(tc_arbiter_entry));
		if (!arbiter->head) fatal("out of memory");
		arbiter->head->addr = addr;
		arbiter->head->next = NULL;

		//TODO: announce this
	} else {
		tc_arbiter_entry * arbiter_list_pos = arbiter->head;
		tc_arbiter_entry * arbiter_entry = malloc(sizeof(tc_arbiter_entry));
		if (!arbiter_entry) fatal("out of memory");
		arbiter_entry->addr = addr;
		arbiter_entry->next = NULL;
		while (arbiter_list_pos->next)
			arbiter_list_pos = arbiter_list_pos->next; //get to end of linked list
		arbiter_list_pos->next = arbiter_entry;
	}
}

void arbiter_handle_deactivation(tc_arbiter * arbiter) {
	if (!arbiter->head);
	tc_arbiter_entry * old_head = arbiter->head;
	arbiter->head = arbiter->head->next; /* delete list head */

	//TODO: announce deactivation

	free(old_head);

	if (arbiter->head) {
		//TODO: annouce next activation
	}
}
#endif

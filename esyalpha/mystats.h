#include <stdio.h>
#include "machine.h"

/* */
#define PRIVATE	1
#define MIGRATE 2
#define SHARED_READ_ONLY	3
#define SHARED_READ_WRITE	4
typedef struct ADDR_TABLE_ENTRY{
md_addr_t addr;
int valid;	/* */
int read;	/*How many accumulated reads performed when it's in the local cache */
int write;	/*How many accumulated writes performed when it's in the local cache */
int private_access;
int private_access_r;
int private_access_w;
int srw_access;
int srw_access_r;
int srw_access_w;
int private_count;
int srw_count;
int word;	/*How many words it's been touched during the life time */
int use;	/*How many times this line is hold in the local cache */
int type;	/*Private, shared-read-only or shared-read-write */
int owner;
} addr_table_entry; 

struct ADDR_TABLE{
addr_table_entry mem[1048576]; /* 32K address */
counter_t count;
}addr_table;

counter_t Cache_access[101]; /*bincount for cache block access */
counter_t word_utilization[101];
counter_t Cache_access_p[101]; /*bincount for cache block access */
counter_t word_utilization_p[101];
counter_t Cache_access_m[101]; /*bincount for cache block access */
counter_t word_utilization_m[101];
counter_t Cache_access_s[101]; /*bincount for cache block access */
counter_t word_utilization_s[101];
counter_t Cache_access_srw[101]; /*bincount for cache block access */
counter_t word_utilization_srw[101];
counter_t Cache_acc_srw_private[101];
counter_t Cache_acc_srw_srw[101];
counter_t Cache_acc_srw_pri_r[101];
counter_t Cache_acc_srw_pri_w[101];
counter_t Cache_acc_srw_r[101];
counter_t Cache_acc_srw_w[101];
counter_t Cache_dynamic_acc_private[101];
counter_t Cache_dynamic_acc_migrate[101];
counter_t Cache_dynamic_acc_shared[101];
counter_t Cache_dynamic_acc_srw[101];
counter_t private_count;
counter_t migrate_count;
counter_t shared_count;
counter_t shared_rw_count;
counter_t private_others;
counter_t private_dynamic_accesses;
counter_t migrate_dynamic_accesses;
counter_t shared_dynamic_accesses;
counter_t sharedrw_dynamic_accesses;
counter_t line_inflight_accesses;
counter_t line_accesses;
counter_t type_changed;
counter_t private_to_shared_rw;
counter_t shared_rw_to_private;
counter_t inflight_private;
counter_t inflight_migrate;
counter_t inflight_shared_rw;
counter_t inflight_shared_only;
counter_t shared_rw_constant;


void stats_allocate(md_addr_t addr, int read, int write, int word, int type, int owner);
void stats_update(md_addr_t addr, int read, int write, int word, int type, int owner, int hit_count);
int stats_type_check(int old_type, int type, int old_owner, int owner);
int stats_compare(md_addr_t addr);
void stats_do(md_addr_t addr, int read, int write, int word, int type, int owner);
void stats_collect();

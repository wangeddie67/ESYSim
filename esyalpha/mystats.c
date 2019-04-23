#include "mystats.h"

/* */
stats_init()
{
	addr_table.count = 0;
}

void inflight_stats(int type, int read, int write)
{
	int access_num = read + write;
	line_inflight_accesses ++;
	switch (type)
	{
		case PRIVATE: 
			inflight_private ++;
			if(access_num < 100)	
				Cache_dynamic_acc_private[access_num] ++;
			else
				Cache_dynamic_acc_private[100] ++;
			break;
		case MIGRATE:
			inflight_migrate ++;
			if(access_num < 100)	
				Cache_dynamic_acc_migrate[access_num] ++;
			else
				Cache_dynamic_acc_migrate[100] ++;
			break;
		case SHARED_READ_ONLY:
			inflight_shared_only ++;
			if(access_num < 100)	
				Cache_dynamic_acc_shared[access_num] ++;
			else
				Cache_dynamic_acc_shared[100] ++;
			break;
		case SHARED_READ_WRITE:
			inflight_shared_rw ++;		
			if(access_num < 100)	
				Cache_dynamic_acc_srw[access_num] ++;
			else
				Cache_dynamic_acc_srw[100] ++;
			break;
		default:
			panic("can not be here\n");
			break;
	}	
	return;	
}

void stats_allocate(md_addr_t addr, int read, int write, int word, int type, int owner)
{
	if(addr_table.mem[addr_table.count].valid)
		panic("allocate wrong entry: address is different!\n");
	if(read != 0 || write != 0 || word != 0)
	{
		addr_table.mem[addr_table.count].valid = 1;
		addr_table.mem[addr_table.count].addr = addr;
		addr_table.mem[addr_table.count].read = read;
		addr_table.mem[addr_table.count].write = write;
		addr_table.mem[addr_table.count].word = word;
		addr_table.mem[addr_table.count].use ++;
		addr_table.mem[addr_table.count].type = type;
		addr_table.mem[addr_table.count].owner = owner;
		if(type == PRIVATE)
		{
			addr_table.mem[addr_table.count].private_access = read + write;
			addr_table.mem[addr_table.count].private_access_r = read;
			addr_table.mem[addr_table.count].private_access_w = write;
			addr_table.mem[addr_table.count].private_count ++;
		}
		if(type == SHARED_READ_WRITE)
		{
			addr_table.mem[addr_table.count].srw_access = read + write;
			addr_table.mem[addr_table.count].srw_access_r = read;
			addr_table.mem[addr_table.count].srw_access_w = write;
			addr_table.mem[addr_table.count].srw_count ++;
		}
		addr_table.count ++;	
		inflight_stats(type, read, write);
		line_accesses ++;
	}
	if(addr_table.count >= 1048576)
	{
		stats_collect();
		panic("Overflow in address table\n");
	}
}
void stats_update(md_addr_t addr, int read, int write, int word, int type, int owner, int hit_count)
{
	int new_type;
	if(addr_table.mem[hit_count].addr != addr)
		panic("Update wrong entry: address is different!\n");
	if(read != 0 || write != 0 || word != 0)
	{
		addr_table.mem[hit_count].read += read;
		addr_table.mem[hit_count].write += write;
		addr_table.mem[hit_count].word += word;
		addr_table.mem[hit_count].use ++;
		new_type = stats_type_check(addr_table.mem[hit_count].type, type, addr_table.mem[hit_count].owner, owner);
		if(type == PRIVATE)
		{
			addr_table.mem[hit_count].private_access += read + write;
			addr_table.mem[hit_count].private_access_r += read;
			addr_table.mem[hit_count].private_access_w += write;
			addr_table.mem[hit_count].private_count ++;
		}
		if(type == SHARED_READ_WRITE)
		{
			addr_table.mem[hit_count].srw_access += read + write;
			addr_table.mem[hit_count].srw_access_r += read;
			addr_table.mem[hit_count].srw_access_w += write;
			addr_table.mem[hit_count].srw_count ++;
		}
		addr_table.mem[hit_count].type = new_type;
		addr_table.mem[hit_count].owner = owner;
		inflight_stats(type, read, write);
	}
}

int stats_type_check(int old_type, int type, int old_owner, int owner)
{
	int new_type;
	if(old_type == SHARED_READ_WRITE || type == SHARED_READ_WRITE)
	{
		new_type = SHARED_READ_WRITE;
		if(old_type == PRIVATE)
			private_to_shared_rw ++;
		else if(type == PRIVATE)
			shared_rw_to_private ++;	
		if(old_type == SHARED_READ_WRITE && type == SHARED_READ_WRITE)
			shared_rw_constant ++;
	}
	else if(old_type == SHARED_READ_ONLY || type == SHARED_READ_ONLY)
		new_type = SHARED_READ_ONLY;
	else if(old_type == MIGRATE || old_owner != owner)
		new_type = MIGRATE;
	else
		new_type = PRIVATE;
	if(new_type != old_type)
		type_changed ++;
	return new_type;
}

int stats_compare(md_addr_t addr)
{
	int i, hit_count = -1; 
	for(i=0;i<addr_table.count;i++)
	{
		if(addr_table.mem[i].addr == addr)
		{
			hit_count = i;
			break;
		}
	}
	return hit_count;
}

void stats_do(md_addr_t addr, int read, int write, int word, int type, int owner)
{
	int hit_count;
	hit_count = stats_compare(addr);
	if(hit_count == -1)
		stats_allocate(addr, read, write, word, type, owner);
	else
		stats_update(addr, read, write, word, type, owner, hit_count);	
}

void stats_collect()
{
	int i=0, c, wc;
	float access = 0, word_use, private_access = 0, srw_access = 0, private_access_r = 0, private_access_w = 0;
	float srw_access_r = 0, srw_access_w = 0;
	for(i=0;i<addr_table.count;i++)
	{
		access = ((float)addr_table.mem[i].read + (float)addr_table.mem[i].write)/((float)addr_table.mem[i].use);
		if(addr_table.mem[i].private_count)
		{
			private_access = ((float)addr_table.mem[i].private_access)/((float)addr_table.mem[i].private_count);
			private_access_r = ((float)addr_table.mem[i].private_access_r)/((float)addr_table.mem[i].private_count);
			private_access_w = ((float)addr_table.mem[i].private_access_w)/((float)addr_table.mem[i].private_count);
		}
		if(addr_table.mem[i].srw_count)
		{
			srw_access = ((float)addr_table.mem[i].srw_access)/((float)addr_table.mem[i].srw_count);
			srw_access_r = ((float)addr_table.mem[i].srw_access_r)/((float)addr_table.mem[i].srw_count);
			srw_access_w = ((float)addr_table.mem[i].srw_access_w)/((float)addr_table.mem[i].srw_count);
		}
		c = (int)(access) - 1;	
		if(c<100)
			Cache_access[c] ++;
		else 
			Cache_access[100] ++;
		word_use = (float)addr_table.mem[i].word/((float)addr_table.mem[i].use);
		wc = (int)(word_use) - 1;
		if(wc<100)
			word_utilization[wc] ++;
		else
			word_utilization[100] ++;	
		switch (addr_table.mem[i].type)
		{
			case PRIVATE:
				if(c<100)
					Cache_access_p[c] ++;
				else 
					Cache_access_p[100] ++;
				if(wc<100)
					word_utilization_p[wc] ++;
				else
					word_utilization_p[100] ++;	
				private_count ++;	
				private_dynamic_accesses += addr_table.mem[i].use;
				break;
			case MIGRATE:
				if(c<100)
					Cache_access_m[c] ++;
				else 
					Cache_access_m[100] ++;
				if(wc<100)
					word_utilization_m[wc] ++;
				else
					word_utilization_m[100] ++;	
				migrate_count ++;		
				migrate_dynamic_accesses += addr_table.mem[i].use;
				break;
			case SHARED_READ_ONLY:
				if(c<100)
					Cache_access_s[c] ++;
				else
					Cache_access_s[100] ++;
				if(wc<100)
					word_utilization_s[wc] ++;
				else
					word_utilization_s[100] ++;	
				shared_count ++;
				shared_dynamic_accesses += addr_table.mem[i].use;
				break;
			case SHARED_READ_WRITE:
				if(c<100)
					Cache_access_srw[c] ++;
				else
					Cache_access_srw[100] ++;
				if(wc<100)
					word_utilization_srw[wc] ++;
				else
					word_utilization_srw[100] ++;	
				shared_rw_count ++;
				sharedrw_dynamic_accesses += addr_table.mem[i].use;
				if(addr_table.mem[i].private_count)
				{
					int pc = (int)(private_access) - 1;
					if(pc<100)
						Cache_acc_srw_private[pc] ++;
					else 
						Cache_acc_srw_private[100] ++;
					int pc_r = (int)(private_access_r) - 1;
					if(pc<100)
						Cache_acc_srw_pri_r[pc_r] ++;
					else 
						Cache_acc_srw_pri_r[100] ++;
					int pc_w = (int)(private_access_w) - 1;
					if(pc<100)
						Cache_acc_srw_pri_w[pc_w] ++;
					else 
						Cache_acc_srw_pri_w[100] ++;
				}
				if(addr_table.mem[i].srw_count)
				{
					int srwc = (int)(srw_access) - 1;
					if(srwc<100)
						Cache_acc_srw_srw[srwc] ++;
					else
						Cache_acc_srw_srw[100] ++;
					int srwc_r = (int)(srw_access_r) - 1;
					if(srwc<100)
						Cache_acc_srw_r[srwc_r] ++;
					else
						Cache_acc_srw_r[100] ++;
					int srwc_w = (int)(srw_access_w) - 1;
					if(srwc<100)
						Cache_acc_srw_w[srwc_w] ++;
					else
						Cache_acc_srw_w[100] ++;
				}
				break;
			default:
				if(c<100)
					Cache_access_p[c] ++;
				else
					Cache_access_p[100] ++;
				if(wc<100)
					word_utilization_p[wc] ++;
				else
					word_utilization_p[100] ++;	
				private_others ++;
				break;
		}
	}
}


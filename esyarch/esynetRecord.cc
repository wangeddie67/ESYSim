/*
 * esynetRecord.cpp
 *
 *  Created on: 2016年11月30日
 *      Author: root
 */
#include <stdlib.h>
#include "esynetRecord.h"



esynetRecord::esynetRecord(int actualid):
    valid(0),hop(0),actualid(actualid),sim_cycle(0),
    l1icache_access(0),l1icache_hit(0),l1icache_miss(0),
    l1dcache_access(0),l1dcache_hit(0),l1dcache_miss(0),
    l2icache_access(0),l2icache_hit(0),l2icache_miss(0),
    l2dcache_access(0),l2dcache_hit(0),l2dcache_miss(0),
    mem_access(0),msg_send(0),msg_recv(0),msg_probe(0),
    add_integer(0),multi_integer(0),add_float(0),multi_float(0),inst(0)
{
    // TODO Auto-generated constructor stub
}


esynetRecord::esynetRecord(const esynetRecord& item):
    valid(item.valid),
    hop(item.hop),
    actualid(item.actualid),
    tileid(item.tileid),
    sim_cycle(item.sim_cycle),
    l1icache_access(item.l1icache_access),
    l1icache_hit(item.l1icache_hit),
    l1icache_miss(item.l1icache_miss),
    l1dcache_access(item.l1dcache_access),
    l1dcache_hit(item.l1dcache_hit),
    l1dcache_miss(item.l1dcache_miss),
    l2icache_access(item.l2icache_access),
    l2icache_hit(item.l2icache_hit),
    l2icache_miss(item.l2icache_miss),
    l2dcache_access(item.l2dcache_access),
    l2dcache_hit(item.l2dcache_hit),
    l2dcache_miss(item.l2dcache_miss),
    mem_access(item.mem_access),
    msg_send(item.msg_send),
    msg_recv(item.msg_recv),
    msg_probe(item.msg_probe),
    add_integer(item.add_integer),
    multi_integer(item.multi_integer),
    add_float(item.add_float),
    multi_float(item.multi_float),
    inst(item.inst)
{
    // TODO Auto-generated constructor stub
}

void esynetRecord::esynetInitData()
{
    valid = 0;
    l1icache_access = 0;
    l1icache_hit = 0;
    l1icache_miss = 0;
    l1dcache_access = 0;
    l1dcache_hit = 0;
    l1dcache_miss = 0;
    l2icache_access = 0;
    l2icache_hit = 0;
    l2icache_miss = 0;
    l2dcache_access = 0;
    l2dcache_hit = 0;
    l2dcache_miss = 0;
    mem_access = 0;
    msg_send = 0;
    msg_recv = 0;
    msg_probe = 0;
    add_integer = 0;
    multi_integer = 0;
    add_float = 0;
    multi_float = 0;
    inst = 0;
}

void esynetRecord::esynetWriteIn(FILE* recdfile, int hasinst)
{
//	if(hasinst && inst){
//		fprintf(recdfile, "%d %lld %d %d %d %d %d %d %d %d %d %d %d %d %d %d 
//    %d %d %d %d %d %d %d %d %d\n",
//			valid, hop, tileid, sim_cycle,
//			l1icache_access, l1icache_hit, l1icache_miss,
//			l1dcache_access, l1dcache_hit, l1dcache_miss,
//			l2icache_access, l2icache_hit, l2icache_miss,
//			l2dcache_access, l2dcache_hit, l2dcache_miss,
//			mem_access, msg_send, msg_recv, msg_probe,
//			add_integer,multi_integer,add_float,multi_float,
//			inst
//			);
//	}
//	else{
//		if(msg_recv!=0)	printf("msg recv\n",msg_recv);
//		if(msg_send!=0)	printf("msg send\n",msg_send);
    fprintf(recdfile, 
        "%d %lld %d %d %d %d %d %d %d %d %d %d \
%d %d %d %d %d %d %d %d %d %d %d %d\n",
        valid, hop, tileid, sim_cycle,
        l1icache_access, l1icache_hit, l1icache_miss,
        l1dcache_access, l1dcache_hit, l1dcache_miss,
        l2icache_access, l2icache_hit, l2icache_miss,
        l2dcache_access, l2dcache_hit, l2dcache_miss,
        mem_access, msg_send, msg_recv, msg_probe,
        add_integer, multi_integer, add_float, multi_float
    );
//	}
}

int
esynetRecord::operator!=(esynetRecord* comp)
{
    if(l1icache_access == comp->l1icache_access &&
        l1icache_hit == comp->l1icache_hit && 
        l1icache_miss == comp->l1icache_miss &&
        l1dcache_access == comp->l1dcache_access &&
        l1dcache_hit == comp->l1dcache_hit && 
        l1dcache_miss == comp->l1dcache_miss &&
        l2icache_access == comp->l2icache_access &&
        l2icache_hit == comp->l2icache_hit && 
        l2icache_miss == comp->l2icache_miss &&
        l2dcache_access == comp->l2dcache_access &&
        l2dcache_hit == comp->l2dcache_hit && 
        l2dcache_miss == comp->l2dcache_miss &&
        mem_access == comp->mem_access &&
        msg_send == comp->msg_send && 
        msg_recv == comp->msg_recv && 
        msg_probe == comp->msg_probe)
    {
        return 0;
    }
    return 1;
}

esynetRecord::~esynetRecord() 
{
	// TODO Auto-generated destructor stub
}

EsyDataItemSoCRecord* esynetRecord::toSoCRecord()
{
    EsyDataItemSoCRecord * t_item = new EsyDataItemSoCRecord(
        valid, hop, tileid, sim_cycle,
        l1icache_access, l1icache_hit, l1icache_miss,
        l1dcache_access, l1dcache_hit, l1dcache_miss,
        l2icache_access, l2icache_hit, l2icache_miss,
        l2dcache_access, l2dcache_hit, l2dcache_miss,
        mem_access, msg_send, msg_recv, msg_probe,
        add_integer, multi_integer, add_float, multi_float);
    
    return t_item;
}



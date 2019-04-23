/*
 * esynetRecord.h
 *
 *  Created on: 2016年11月30日
 *      Author: root
 */

#ifndef ESYSIM_OPENSOURCE_ESYARCH_ESYNETRECORD_H_
#define ESYSIM_OPENSOURCE_ESYARCH_ESYNETRECORD_H_

#include <stdio.h>
#include "esy_interdata.h"


class esynetRecord 
{
public:
    /*valid equals 1: record changed and should be calculated as record
        * valid equals 2: record no changing and keep to last state*/
    int valid;
    //esynet文件记录格式
    int hop;
    int actualid;
    int tileid;
    int sim_cycle;
    int l1icache_access;
    int l1icache_hit;
    int l1icache_miss;
    int l1dcache_access;
    int l1dcache_hit;
    int l1dcache_miss;
    int l2icache_access;
    int l2icache_hit;
    int l2icache_miss;
    int l2dcache_access;
    int l2dcache_hit;
    int l2dcache_miss;
    int mem_access;
    int msg_send;
    int msg_recv;
    int msg_probe;
    int add_integer;
    int multi_integer;
    int add_float;
    int multi_float;
    int inst;
    esynetRecord(int actualid);
    int operator!=(esynetRecord* comp);
    void esynetInitData();
    void esynetWriteIn(FILE* recdfile,int hasinst);
    virtual ~esynetRecord();
    EsyDataItemSoCRecord * toSoCRecord();
    esynetRecord(const esynetRecord& item);
};


#endif /* ESYSIM_OPENSOURCE_ESYARCH_ESYNETRECORD_H_ */

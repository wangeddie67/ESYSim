#include <stdio.h>
#include <iostream>
#include <string.h>
#include  "host.h"
#include "machine.h"
using namespace std;

extern "C" void pushStore (md_addr_t pc, md_addr_t addr, counter_t sim_cycle, int width);
extern "C" void verifyLoad (md_addr_t pc, md_addr_t addr, counter_t sim_cycle, int width);

extern void AddStoreRef (md_addr_t pc, md_addr_t loadPC, counter_t commDist);

#define TABLE_LENGTH    2048
#define SHIFT_LENGTH    3
#define ASSOC           32
class entry
{
public:
    md_addr_t pc;
    md_addr_t addr;
    counter_t timeStamp;
    int valid;
    int width;

    /* Constructor */
    entry ()
    {
        pc = 0;
        addr = 0;
        timeStamp = 0;
        valid = 0;
        width = 0;
    }

    /* De-Constructor */
    ~entry ()
    {
    }
};

class hashTable
{
public:
    entry table[TABLE_LENGTH][ASSOC];
    counter_t evictCount;
    int histTable[10000];

    /* Constructor */
    hashTable ()
    {
        evictCount = 0;
        for (int i = 0; i < 10000; i++)
        {
            histTable[i] = 0;
        }
    }

    /* De-Constructor */
    ~hashTable ()
    {
    }
};


hashTable storeBuffer;

void
pushStore (md_addr_t pc, md_addr_t addr, counter_t sim_cycle, int width)
{
    int index;
    int i;

    index = (addr >> SHIFT_LENGTH) & (TABLE_LENGTH - 1);
    int found = 0;
    counter_t oldest_timeStamp;
    int oldest_entry;

    /* Search in the cache if the entry is already present */
    for (i = 0; i < ASSOC; i++)
    {
        if (storeBuffer.table[index][i].valid == 1)
        {
            int bigMemWidth = (width > storeBuffer.table[index][i].width) ? width : storeBuffer.table[index][i].width;
            md_addr_t locAddr = (storeBuffer.table[index][i].addr & ~(bigMemWidth - 1));
            md_addr_t extAddr = (addr & ~(bigMemWidth - 1));

            if (locAddr == extAddr)
            {
                if (!found)
                {
                    storeBuffer.table[index][i].pc = pc;
                    storeBuffer.table[index][i].timeStamp = sim_cycle;
                    storeBuffer.table[index][i].addr = addr;
                    storeBuffer.table[index][i].width = width;
                    found = 1;
                }
                else
                {
                    storeBuffer.table[index][i].valid = 0;
                    storeBuffer.table[index][i].pc = 0;
                    storeBuffer.table[index][i].timeStamp = 0;
                    storeBuffer.table[index][i].addr = 0;
                    storeBuffer.table[index][i].width = 0;
                }
            }
        }
    }

    if (found)
        return;

    oldest_timeStamp = storeBuffer.table[index][0].timeStamp;
    oldest_entry = 0;

    /* Insert the store in first possible empty block */
    for (i = 0; i < ASSOC; i++)
    {
        if (storeBuffer.table[index][i].valid == 0)
        {
            storeBuffer.table[index][i].pc = pc;
            storeBuffer.table[index][i].addr = addr;
            storeBuffer.table[index][i].timeStamp = sim_cycle;
            storeBuffer.table[index][i].valid = 1;
            storeBuffer.table[index][i].width = width;
            return;
        }
        if (storeBuffer.table[index][i].timeStamp < oldest_timeStamp)
        {
            oldest_timeStamp = storeBuffer.table[index][i].timeStamp;
            oldest_entry = i;
        }
    }

    /* No empty blocks present, evict one of them and insert the new */
    storeBuffer.table[index][oldest_entry].pc = pc;
    storeBuffer.table[index][oldest_entry].addr = addr;
    storeBuffer.table[index][oldest_entry].timeStamp = sim_cycle;
    storeBuffer.table[index][oldest_entry].valid = 1;
    storeBuffer.table[index][oldest_entry].width = width;
    storeBuffer.evictCount++;
    return;
}

void
verifyLoad (md_addr_t pc, md_addr_t addr, counter_t sim_cycle, int width)
{
    int index;
    int i;

    index = (addr >> SHIFT_LENGTH) & (TABLE_LENGTH - 1);

    for (i = 0; i < ASSOC; i++)
    {
        /* If the cache block is valid for current address and distance between store and
         ** load is maximum 300 instructions*/
        if (storeBuffer.table[index][i].valid == 1)
        {
            int bigMemWidth = (width > storeBuffer.table[index][i].width) ? width : storeBuffer.table[index][i].width;
            md_addr_t locAddr = (storeBuffer.table[index][i].addr & ~(bigMemWidth - 1));
            md_addr_t extAddr = (addr & ~(bigMemWidth - 1));

            if (locAddr == extAddr && (sim_cycle - storeBuffer.table[index][i].timeStamp < 5000))
                AddStoreRef (storeBuffer.table[index][i].pc, pc, sim_cycle - storeBuffer.table[index][i].timeStamp);
        }
    }

}

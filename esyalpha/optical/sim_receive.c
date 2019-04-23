/* map the receiver for packet */
#include "point-point.h"
extern struct cache_t *cache_dl2;
int receiver_map(long src, long des, int receiver, md_addr_t addr)
{
    /* Decide which receiver on the destination should be used to receiver the packet */
    int receiver_id;
    if(Map_algr == RANDOM)
    {
        /* random algorism: randomly mapping a receiver at each sending time */
        srand(time(NULL));
        receiver_id = rand()%receiver;
    }
    else if(Map_algr == FIXED)
    {
        /* Fixed algorism: round robin */
        receiver_id = src % receiver;
        //if(src%boundery == 0 || src%boundery == 1 || src%boundery == 2 || src%boundery == 3)
        //	receiver_id = (receiver_id+1)%receiver;
    }
    return receiver_id;
}

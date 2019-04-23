/* SIM_CONFLICTION_CHECK.C */

#include "point-point.h"

void conflict_type_change(Flit_list *flit_list, int PID)
{
    if(flit_list->tail == NULL)
        panic("NETWORK: CONFLICTION TYPE CHANGE: No confliction packet here!");

    if((flit_list->tail)->flit_type == CONF_TYPE)
    {
#ifdef CONF_PREDICTOR
        Flit *flit;
        flit = flit_list->head;
        while(flit)
        {
            flit->PID = flit->PID | PID;
            flit = flit->next;
        }
#endif
        return;
    }
    else
    {
        Flit *flit;
        flit = flit_list->head;
        while(flit)
        {
#ifdef CONF_PREDICTOR
            flit->PID = flit->PID | PID;
#endif
            flit->flit_type = CONF_TYPE;
            flit = flit->next;
        }
    }
    return;
}





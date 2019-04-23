/* flit list handling functions */
#include "point-point.h"
extern counter_t sim_cycle;
int add_flit(Flit *flit, Flit_list *flit_list )
{
    /* adding the flit into the flit list at the senders */

    flit_list->flits_in_list ++;
    if ((flit_list->head == NULL)||(flit_list->tail == NULL))
    {
        /* flit list is empty */
        flit_list->head = flit;
        flit_list->tail = flit;
    }
    else
    {
        (flit_list->tail)->next = flit;
        (flit_list->tail) = flit;
    }
    return 1;
}
int free_flit(Flit *flit, Flit_list *flit_list )
{
    /* free the flit from the sender flit list, put it into channel */

    if(flit_list->flits_in_list < 0)
        return 0;
    /* free the flit from the data flit list */
    flit_list->flits_in_list --;
    flit_list->head = flit->next;
    flit->next = free_flit_list.head;
    free_flit_list.head = flit;
    currentFlits--;

    if(flit_list->head == NULL)
        flit_list->tail = NULL;
    return 1;
}

void allocate_free_flit_list()
{
    int i;
    free_flit_list.head = (Flit *) malloc(sizeof(Flit));
    Flit * next = free_flit_list.head;
    for(i = 1; i < maxFlits; i++)
    {
        next->next = (Flit *) malloc(sizeof(Flit));
        next = next->next;
        next->next = NULL;
    }
}

Flit * allocate_flit()
{
    if(currentFlits >= maxFlits)
        panic("Out of free flits\n");
    Flit * temp = free_flit_list.head;
    free_flit_list.head = free_flit_list.head->next;
    return temp;
}


/*              MESSAGE HANDLING FUNCTIONS              */
#include "point-point.h"

/* adding message to central message list */
void add_message(counter_t start_time, int msg_type, Packet packet)
{
    Msg *message;
    message = (Msg *)malloc(sizeof(Msg));
    message->msg_type = msg_type;
    message->when = start_time;
    message->packet = packet;
    message->next = NULL;
    message->pre = NULL;


    if((msg_list->head == NULL) && (msg_list->tail == NULL))
    {
        /* message list is empty */
        msg_list->head = message;
        message->next = NULL;
        message->pre = NULL;
        msg_list->tail = message;
    }
    else
    {
        /* add message to tail */
        message->pre = msg_list->tail;
        msg_list->tail->next = message;
        msg_list->tail = message;
    }
}

/* free message from central message list */
void free_message(Msg *oldmsg)
{
    if(msg_list->head == oldmsg)
    {
        msg_list->head = oldmsg->next;

        free(oldmsg);

        if(msg_list->head == NULL)
            msg_list->tail = NULL;
        else
            msg_list->head->pre = NULL;
    }
    else
    {
        if(oldmsg->next != NULL)
        {
            oldmsg->pre->next = oldmsg->next;
            oldmsg->next->pre = oldmsg->pre;
        }
        else
        {
            if(msg_list->tail == oldmsg)
                msg_list->tail = oldmsg->pre;
            oldmsg->pre->next = oldmsg->next;
        }

        free(oldmsg);
    }
}


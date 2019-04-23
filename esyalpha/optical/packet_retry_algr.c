#include <stdio.h>
#include "point-point.h"

/* EXPONENTIAL BACKOFF ALGORITHMS */
/* Yongfa L and Deyu M, 2006, IJCSNS paper */

int exponential_2(int n)
{
    int i = 0;
    double result = 1.0;
    int time_slot = 0;
    for(i=0; i<n; i++)
        result = result*1.1;
    result = first_slot_num * result;
    int randnum=rand()%10000;
    float prob = (float)randnum/(float)10000;
    time_slot = (int)(prob*result);
    return (time_slot);
}
int m_p(int n)
{
    int result = 0;
    if(n > 0)
        result = exponential_2(n-1) + m_p(n-1);

    return result;
}

int n_p(int n)
{
    int result = 0;
    if(n > 0)
        result = exponential_2(n) + n_p(n-1);

    return result;
}
int m_p_10(int n)
{
    int i = 0, result = m_p(10);
    for(i=0; i<n-10; i++)
    {
        result += exponential_2(10);
    }
    return result;
}
int n_p_10(int n)
{
    int i = 0, result = n_p(10);
    for(i=0; i<n-10; i++)
    {
        result += exponential_2(10);
    }
    return result;
}
int packet_retry_algr(Packet packet)
{
    int m, n, p; /* m and n denote minimum and maximum of the window respectively */
    /* p is the pth step of the collision resolution */
    int slot_num = 0;

    p = packet.retry_num;
    if(TBEB_algr == 0)
    {
        /* truncated binary exponential back-off algorithm */
        m = m_p(p-1) + exponential_2(p-1);
        n = n_p(p-1) + exponential_2(p);
        if(p > 10)
        {
            m = m_p_10(p-1) + exponential_2(10);
            n = n_p_10(p-1) + exponential_2(10);
        }
        slot_num = rand()%n+m;
    }
    else if(TBEB_algr == 1)
    {
        /* TBEB algorithm with dynamically setting intial window*/

    }
    else if(TBEB_algr == 2)
    {
        /* TBEB algorithm with dynamically setting intial window and end window*/

    }
    return slot_num;
}

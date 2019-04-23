/* ************* PACKET.C ************************/

#include "point-point.h"

void add_packet(Packet packet, long des)
{
    packet_queue[des][packet_queue_tail[des]] = packet;
    packet_queue_tail[des] = (packet_queue_tail[des]+1)%packet_queue_size;
    packet_queue_num[des] ++;
    if(packet_queue_num[des] > packet_queue_size)
        panic("NETWORK: Packet queue is out of space!");
}

void free_packet(long des)
{
    Packet packet = packet_queue[des][packet_queue_head[des]];
    //printf("free packet %d, src %d, des %d, start time is %d\n", packet.msgno, packet.src, packet.des, packet.start_time);
    packet_queue_head[des] = (packet_queue_head[des]+1)%packet_queue_size;
    packet_queue_num[des] --;
    if(packet_queue_num[des] < 0)
        panic("NETWORK: Packet num in queue can not be smaller than zero!");
}



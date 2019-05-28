#include <exception>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "esynet_global.h"
#include "esynet_random_unit.h"
#include "esynet_config.h"
#include "esynet_foundation.h"
#include "esynet_event_queue.h"

int esynetFinishedMessage(
    long src, long dst, long long int stTime, long long int msgNo);

void esynetConfigInit( EsynetConfig * config, char * command );

void esynetSRGenInit( EsynetSRGen * srgen, EsynetConfig * config );

void esynetNetworkInit( EsyNetworkCfg * network, EsynetConfig * config );
void esynetNetworkInit2( EsyNetworkCfg * network, EsynetConfig * config );

void esynetMessQueueInit( EsynetEventQueue * messq, EsynetConfig * config );

void esynetMessQueueFinish( EsynetEventQueue * messq, EsynetConfig * config );

int esynetRunSim( EsynetEventQueue * messq, EsynetFoundation * found, 
				  long long int sim_cycle);

vector< EsynetEvent > esynetAcceptList();

void esynetClearAcceptList();

/*************************************************
  Function : 
    void MessQueue::insertMsg( int src, int dest, long long int sim_cycle,
        long packetSize, long long int msgNo, long long addr, long vc )
  Description: 
    insert EVG message with specfic $src$, $dest$, $sim_cycle$, $size$,
    $msgNo$, $addr$ and $vc$. Packet is not larger than 10 cyclcs.
  Calls :
    void MessQueue::addMessage(const MessEvent &x);
  Input :
              int src         source id
              int dest        destination id
    long long int sim_cycle   event time
             long packetSize  packet size
    long long int msgNo       message num er
        long long addr        not used
             long vc          virtual channel in source
*************************************************/
/* insert message, called by Simplescalar */
void esynetInjectPacket( EsynetEventQueue* messq, int src, int dest, long long int sim_cycle, long packetSize, long int msgNo, long long addr, long vc );

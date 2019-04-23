#include <exception>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "esynet_global.h"
#include "esynet_random_unit.h"
#include "esynet_config.h"
#include "esynet_foundation.h"
#include "esynet_mess_queue.h"

int esynetFinishedMessage(
    long src, long dst, long long int stTime, long long int msgNo);

void esynetConfigInit( EsynetConfig * config, char * command );

void esynetSRGenInit( EsynetSRGen * srgen, EsynetConfig * config );

void esynetNetworkInit( EsyNetworkCfg * network, EsynetConfig * config );
void esynetNetworkInit2( EsyNetworkCfg * network, EsynetConfig * config );

void esynetMessQueueInit( EsynetMessQueue * messq, EsynetConfig * config );

void esynetMessQueueFinish( EsynetMessQueue * messq, EsynetConfig * config );

int esynetRunSim( EsynetMessQueue * messq, EsynetFoundation * found, 
				  long long int sim_cycle);

vector< EsynetMessEvent > esynetAcceptList();

void esynetClearAcceptList();


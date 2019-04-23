/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#include "esy_argument.h"
#include "esy_linktool.h"
#include "esy_networkcfg.h"
#include "esynet_interface.h"
#include "esymap_config.h"
#include "esymap_task_machine.h"
#include "esymap_task.h"
#include "esymap_statistic.h"

/* global define of double sim_cycle */
/* used by power cacluation */
extern "C" {
double sim_cycle;
}

/* name space */
using namespace std;

/*************************************************
  Function :
    int main( int argc, char *argv[] )
  Description:
    main function of esynet
  Input:
         int argc  argument counter
    char * argv[]  argument list
    ConfigType type  type of variable set to inject
  Calls:
    bool MessQueue::openEventTrace()
    void MessQueue::simulator( long long int sim_cycle )
    bool MessQueue::closeEventTrace()
    void SimFoundation::simulationResults()
*************************************************/
int main(int argc, char *argv[])
{
    /* Parser input arguments */
    EsymapConfig argu_list(argc, argv);

    /* set random seed */
    EsynetSRGen random_gen(argu_list.randomSeed());

    /* generate network configuration */
    EsyNetworkCfg network_cfg;
    esynetNetworkInit(&network_cfg, &argu_list);

    /* network simulation platform */
    EsynetFoundation sim_net(&network_cfg, &argu_list);

    /* message queue */
    EsynetMessQueue network_mess_queue(0.0, &sim_net, &argu_list);
    esynetMessQueueInit(&network_mess_queue, &argu_list);

    esymap_statistic m_esymap_statistic(&argu_list);

    vector<EsynetTaskMachine> task_pe_list;
    for (size_t i = 0; i < network_cfg.routerCount(); i++)
    {
        task_pe_list.push_back(EsynetTaskMachine(&network_cfg, i, &argu_list, &m_esymap_statistic));
    }
    SimTask sim_task(&network_cfg, &argu_list, task_pe_list);
    sim_task.init();

    /* print network simulation configuration */
    cout << sim_net;

    /* begin simulation */
    LINK_PROGRESS_INIT
    cout << "simulation begin" << endl;
    /* initializate event finish flag */
    int event_finish_flag = 0;

    /* loop simulation cycle */
    for (sim_cycle = 0; sim_cycle <= argu_list.simLength(); sim_cycle = sim_cycle + argu_list.simulationPeriod())
    {
        /* print simulation progress */
        LINK_PROGRESS_UPDATE(sim_cycle, argu_list.simLength())

        /* simulation one cycle */
        esynetRunSim(&network_mess_queue, &sim_net, (long long int)sim_cycle);
        vector<EsynetMessEvent> accept_list = esynetAcceptList();

        /* mapping function */
        sim_task.runMapping(sim_cycle);
        vector<EsynetMessEvent> result_event = sim_task.eventQueue();
        sim_task.clearEventQueue();

        for (size_t i = 0; i < result_event.size(); i++)
        {
            network_mess_queue.addMessage(result_event[i]);
        }
        /* task machine */
        for (size_t i = 0; i < network_cfg.routerCount(); i++)
        {
            task_pe_list[i].task_runAfterRouter(sim_cycle);
            vector<EsynetMessEvent> result_event = task_pe_list[i].eventQueue();
            task_pe_list[i].clearEventQueue();

            for (size_t i = 0; i < result_event.size(); i++)
            {
                network_mess_queue.addMessage(result_event[i]);
            }
        }

        for (int i = 0; i < accept_list.size(); i++)
        {
            task_pe_list[accept_list[i].desId()].task_receivePacket(accept_list[i].flit());
        }
        esynetClearAcceptList();

        if (sim_task.simulation_over())
        {
            break;
        }
        
		/* synchronize */
		if ( argu_list.eventTraceCoutEnable() )
		{
			LINK_SYNCHORNOIZE( 'n' );
		}
    } /* for ( sim_cycle = 0; sim_cycle <= argu_list.simLength(); */

    /* finished simulation */
    LINK_PROGRESS_END
    cout << "simulation end" << endl;

    esynetMessQueueFinish(&network_mess_queue, &argu_list);

    /* print configuration value */
    argu_list.printValue2Console(cout);
    /* print simulation result */

    sim_net.simulationResults();
    m_esymap_statistic.print();
    m_esymap_statistic.print(argu_list.get_result_file_name());
    sim_task.print_in_main();

    /* return */
    LINK_RETURN(0)
}

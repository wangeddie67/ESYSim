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
#include "esynet_config.h"
#include "esynet_foundation.h"
#include "esynet_global.h"
#include "esynet_mess_queue.h"
#include "esynet_random_unit.h"
#include "esynet_ecc_unit.h"

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
int main( int argc, char *argv[] )
{
	/* Parser input arguments */
	EsynetConfig argu_list(argc, argv);

	/* set random seed */
	EsynetSRGen random_gen(argu_list.randomSeed());

	/* generate network configuration */
	EsyNetworkCfg network_cfg;
	if ( argu_list.networkCfgFileEnable() )
	{
		EsyXmlError t_err = 
			network_cfg.readXml( argu_list.networkCfgFileName() );
		if (t_err.hasError()) 
		{
			cerr << "Error: cannot read file " << 
				argu_list.networkCfgFileName() << endl;
			return 0;
		}
	}
	else
	{
		EsyNetworkCfgRouter t_template_router;
		network_cfg.setDataPathWidth( argu_list.dataPathWidth() );
		network_cfg.setTopology(EsyNetworkCfg::NOC_TOPOLOGY_2DTORUS);
		network_cfg.setSize( argu_list.aryNumber() );
		for ( long i = 0; i < argu_list.physicalPortNumber(); i ++ )
		{
			EsyNetworkCfgPort::RouterPortDirection port_dir;
			bool port_ni;
			if ( i == 0 )
			{
				port_dir = EsyNetworkCfgPort::ROUTER_PORT_SOUTHWEST;
				port_ni = true;
			}
			else
			{
				switch ( i % 4 )
				{
					case 0:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_EAST; break;
					case 1:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_NORTH; break;
					case 2:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_SOUTH; break;
					case 3:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_WEST; break;
				}
				port_ni = false;
			}
			t_template_router.appendPort(EsyNetworkCfgPort(
				argu_list.virtualChannelNumber(), 
				argu_list.virtualChannelNumber(), port_dir,
				argu_list.inbufferSize(), argu_list.outbufferSize(), 
				port_ni ) );
		}
		network_cfg.setTemplateRouter( t_template_router );
		network_cfg.updateRouter();
	}

	if ( argu_list.eccEnable() )
	{
		network_cfg.setDataPathWidth( EsynetECCBufferUnit::dataPathWithECC(
			argu_list.eccName(), network_cfg.dataPathWidth() ) );
	}

	/* network simulation platform */
	EsynetFoundation sim_net(&network_cfg, &argu_list);
	/* message queue */
	EsynetMessQueue network_mess_queue(0.0, &sim_net, &argu_list);

	/* print network simulation configuration */
	cout << sim_net;

	/* open event trace */
	if ( argu_list.eventTraceEnable() )
	{
		network_mess_queue.openEventTrace();
	} /* if ( argu_list.eventTraceEnable() ) */
	/* open output trace */
	if ( argu_list.outputTraceEnable() )
	{
		network_mess_queue.openOutputTrace();
	} /* if ( argu_list.outputTraceEnable() ) */

	/* open benchmark trace */
	EsyDataFileIStream< EsyDataItemBenchmark > * tracein = NULL;
	EsyDataItemBenchmark t_item;

	if ( argu_list.inputTraceEnable() )
	{
		/* open becnhmark trace */
		tracein = new EsyDataFileIStream< EsyDataItemBenchmark >(
			argu_list.inputTraceBufferSize(), argu_list.inputTraceFileName(),
			BENCHMARK_EXTENSION, !argu_list.inputTraceTextEnable() );
		/* open failure, directly quit */
		if ( !tracein->openSuccess() )
		{
			cout << "Cannot open traffic trace file (T1) " << 
				argu_list.inputTraceFileName() << endl;
			return 0;
		} /* if ( !tracein->open_success() ) */
		/* benchmark is empty, directly quit */
		t_item = tracein->getNextItem();
		if ( t_item.time() == -1 )
		{
			cout << "Cannot open traffic trace file (T2) " << 
				argu_list.inputTraceFileName() << endl;
			return 0;
		} /* if ( t_item.m_time == -1 ) */
	} /* if ( argu_list.trafficTraceEnable() ) */


	/* begin simulation */
	LINK_PROGRESS_INIT
	cout << "simulation begin" << endl;
	/* initializate event finish flag */
	int event_finish_flag = 0;

	/* loop simulation cycle */
	for ( sim_cycle = 0; sim_cycle <= argu_list.simLength(); 
			sim_cycle = sim_cycle + argu_list.simulationPeriod() )
	{
		/* print simulation progress */
		LINK_PROGRESS_UPDATE( sim_cycle, argu_list.simLength() )
		/* insert traffic trace */
		if ( argu_list.inputTraceEnable() )
		{
			/* loop all the packages before simulation cycle */
			while ( sim_cycle >= t_item.time() && !event_finish_flag )
			{
				network_mess_queue.addMessage( 
					EsynetMessEvent::generateEvgMessage(sim_cycle,
					t_item.src(), t_item.dst(), t_item.size(), -1, 
					sim_cycle ) );
				
				/* read information about next packet */
				t_item = tracein->getNextItem();
				/* read information about next packet */
				if ( t_item.time() == -1 )
				{
					event_finish_flag = 1;
				} /* if ( t_item.m_time == -1 ) */
			} /* while ( sim_cycle >= t_item.m_time && !event_finish_flag ) */
		} /* if ( argu_list.trafficTraceEnable() ) */

		/* simulation one cycle */
		network_mess_queue.simulator( (long long int)sim_cycle );
		
		/* check simulation finished condition */
		if ( argu_list.latencyMeasurePacket() > 0 && 
			argu_list.throughputMeasurePacket() > 0 )
		{
			/* latency and throughput measurement has finished */
			if ( sim_net.latencyMeasureState() == 
				 EsynetFoundation::MEASURE_END &&
				 sim_net.throughputMeasureState() == 
				 EsynetFoundation::MEASURE_END )
			{
				break;
			} /* if ( sim_net.latencyMeasureFinished() >= */
		} /* if ( argu_list.latencyMeasurePacket() > 0 && */
		if ( argu_list.injectedPacket() > 0 )
		{
			if ( sim_net.limitedInjectionState() == 
				 EsynetFoundation::MEASURE_END )
			{
				break;
			}
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

	/* close event trace */
	if ( argu_list.eventTraceEnable() )
	{
		network_mess_queue.closeEventTrace();
	} /* if ( argu_list.eventTraceEnable() ) */
	/* close output trace */
	if ( argu_list.outputTraceEnable() )
	{
		network_mess_queue.closeOutputTrace();
	} /* if ( argu_list.eventTraceEnable() ) */

	/* print configuration value */
	argu_list.printValue2Console( cout );
	/* print simulation result */
	sim_net.simulationResults();

	/* return */
	return 0;
}

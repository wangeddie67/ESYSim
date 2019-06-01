/*
 * File name : esynet_main.h
 * Function : Main function of ESYNet.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>
 */

#include "esy_argument.h"
#include "esy_linktool.h"
#include "esy_networkcfg.h"
#include "esynet_config.h"
#include "esynet_foundation.h"
#include "esynet_global.h"
#include "esynet_event_queue.h"
#include "esynet_random_unit.h"

/**
 * @ingroup ESYNET_GROUP
 * @file esynet_main.cc
 * @brief Main function for ESYNet.
 */

// global define of double sim_cycle, used by power cacluation */
extern "C" {
double sim_cycle;
}

/**
 * @brief Main function for ESYNet.
 * @param argc Argument number.
 * @param argv Argument list.
 * @return 0.
 */
int main( int argc, char *argv[] )
{
    // Parser input arguments.
    EsynetConfig argu_list( argc, argv );

    // Set random seed.
    EsynetSRGen random_gen( argu_list.randomSeed() );

    // Generate network configuration.
    EsyNetworkCfg network_cfg;
    if ( argu_list.networkCfgFileEnable() )
    {
        EsyXmlError t_err = network_cfg.readXml( argu_list.networkCfgFileName() );
        if ( t_err.hasError() )
        {
            cerr << "Error: cannot read file " << argu_list.networkCfgFileName() << endl;
            return 0;
        }
    }
    else
    {
        network_cfg = EsyNetworkCfg( argu_list.topology(),
                                     argu_list.aryNumber(),
                                     argu_list.dataPathWidth(),
                                     argu_list.simulationPeriod(),
                                     argu_list.physicalPortNumber(),
                                     argu_list.virtualChannelNumber(),
                                     argu_list.virtualChannelNumber(),
                                     argu_list.inbufferSize(),
                                     argu_list.outbufferSize() );
    }

    // Network simulation platform 
    EsynetFoundation sim_net( &network_cfg, &argu_list );
    cout << sim_net;

    // Print network configuration to file.
    if ( argu_list.networkCfgOutputFileEnable() )
    {
        EsyXmlError t_err = network_cfg.writeXml( argu_list.networkCfgFileName() );
        if ( t_err.hasError() )
        {
            cerr << "Error: cannot create file " << argu_list.networkCfgFileName() << endl;
            return 0;
        }
    }

    // message queue 
    EsynetEventQueue network_mess_queue( 0.0, &sim_net, &argu_list );

    // begin simulation
    cout << "**** simulation begin ****" << endl;
    LINK_PROGRESS_INIT

    // loop simulation cycle
    for ( sim_cycle = 0; sim_cycle <= argu_list.simLength(); sim_cycle = sim_cycle + argu_list.simulationPeriod() )
    {
        // print simulation progress
        LINK_PROGRESS_UPDATE( sim_cycle, argu_list.simLength() )

        // simulation one cycle
        network_mess_queue.simulator( (long long int)sim_cycle );

        // check simulation finished condition
        if ( argu_list.latencyMeasurePacket() > 0 && argu_list.throughputMeasurePacket() > 0 )
        {
            // latency and throughput measurement has finished
            if ( sim_net.latencyMeasureState() == EsynetFoundation::MEASURE_END &&
                 sim_net.throughputMeasureState() == EsynetFoundation::MEASURE_END )
            {
                break;
            }
        }
        if ( argu_list.injectedPacket() > 0 )
        {
            if ( sim_net.limitedInjectionState() == EsynetFoundation::MEASURE_END )
            {
                break;
            }
        }

        // synchronize
        if ( argu_list.eventTraceCoutEnable() )
        {
            LINK_SYNCHORNOIZE( 'n' );
        }
    }

    // finished simulation
    LINK_PROGRESS_END
    cout << "**** simulation end ****" << endl;

    // print configuration value
    argu_list.printValue2Console( cout );
    // print simulation result
    sim_net.simulationResults();

    // return
    return 0;
}

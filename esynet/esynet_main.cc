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

/**
 * @ingroup ESYNET_GROUP
 * @file esynet_main.cc
 * @brief Main function for ESYNet.
 */

#include "esy_argument.h"
#include "esy_linktool.h"
#include "esy_netcfg.h"
#include "esynet_config.h"
#include "esynet_foundation.h"
#include "esynet_global.h"
#include "esynet_event_queue.h"
#include "esynet_random_unit.h"

extern "C" {
double sim_cycle;   /*!< @brief Global simulation cycle, used by power model. */
}

/**
 * @brief Main function for ESYNet.
 * @param argc Argument number.
 * @param argv Argument list.
 * @return Error code.
 * @retval 0 Program Success.
 * @retval 2 No such file or directory.
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
            return 2;
        }
    }
    else
    {
        network_cfg = EsyNetworkCfg( argu_list.topology(),
                                     argu_list.networkSize(),
                                     1.0,
                                     argu_list.physicalPortNumber(),
                                     argu_list.virtualChannelNumber(),
                                     argu_list.virtualChannelNumber(),
                                     argu_list.inbufferSize(),
                                     argu_list.outbufferSize(),
                                     1.0,
                                     argu_list.niBufferSize(),
                                     argu_list.niReadDelay()
                                   );
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
            return 2;
        }
    }

    // Message queue 
    EsynetEventQueue network_mess_queue( 0.0, &sim_net, &argu_list );

    // Simulation begin
    cout << "**** simulation begin ****" << endl;
    LINK_PROGRESS_INIT

    // Loop simulation cycle
    for ( sim_cycle = 0; sim_cycle <= argu_list.simLength(); sim_cycle = sim_cycle + argu_list.simulationPeriod() )
    {
        // Print simulation progress
        LINK_PROGRESS_UPDATE( sim_cycle, argu_list.simLength() )

        // Simulation one cycle
        network_mess_queue.simulator( (long long int)sim_cycle );

        // Check simulation finished condition
        if ( argu_list.latencyMeasurePacket() > 0 && argu_list.throughputMeasurePacket() > 0 )
        {
            // Latency and throughput measurement has finished
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

        // Synchronize for on-line visualization
        if ( argu_list.eventTraceCoutEnable() )
        {
            LINK_SYNCHORNOIZE( 'n' );
        }
    }

    // Simulation finish
    LINK_PROGRESS_END
    cout << "**** simulation end ****" << endl;

    // Print configuration value
    argu_list.printValue2Console( cout );
    // Print simulation result
    sim_net.simulationResults();

    // Return
    return 0;
}

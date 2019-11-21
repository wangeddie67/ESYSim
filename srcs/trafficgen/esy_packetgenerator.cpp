/*
 * File name : esy_packetgenerator.cpp
 * Function : Timing model of the packet generator.
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
 * @ingroup ESY_TRAFFIC_GEN
 * @file esy_packetgenerator.cpp
 * @brief Implement timing packet generator.
 */

#include <cmath>
#include <iostream>
#include "esy_packetgenerator.h"


EsyPacketGenerator::EsyPacketGenerator( long ni_count, 
                                        const std::vector< long >& network_size,
                                        EsynetTrafficProfile profile,
                                        double pir,
                                        long packet_size,
                                        long injected_packet,
                                        EsyRandGen* rand_gen
                                      )
    : EsyPacketGeneratorFun( ni_count,
                             network_size,
                             profile,
                             pir,
                             packet_size,
                             rand_gen )
    , m_injected_packet( injected_packet )
    , m_packet_count( 0 )
    , m_enable( true )
    , mp_last_pac()
    , m_max_time( 0 )
{
}

std::vector< EsyNetworkPacketPtr >
EsyPacketGenerator::generatePacket( uint64_t sim_cycle )
{
    std::vector< EsyNetworkPacketPtr > pac_list;
    if ( !m_enable )
    {
        return pac_list;
    }

    m_max_time += 1;

    if ( profile() == TP_TRACE )
    {
        if ( mp_last_pac != NULL )
        {
            if ( mp_last_pac->time() <= sim_cycle )
            {
                pac_list.push_back( mp_last_pac );
                mp_last_pac = NULL;
            }
            else
            {
                return pac_list;
            }
        }
        if ( !readStream() )
        {
            return pac_list;
        }
        while( true )
        {
            EsyNetworkPacketPtr pac = readPacketFromFile();
            if ( pac == NULL )
            {
                std::cout << "End of traffic trace file @ "
                          << sim_cycle << std::endl;
                m_enable = false;
                break;
            }
            else
            {
                if ( pac->time() <= sim_cycle )
                {
                    pac_list.push_back( pac );
                }
                else
                {
                    mp_last_pac = pac;
                    break;
                }
            }
        }
    }
    else
    {
        for ( long id = 0; id < niCount(); id ++ )
        {
            // detemine if there is a new packet, if the random number is lower
            // then packet injection rate, shot will be TRUE;
            // Thus, the packet inject rate ranges from 0 to 1 packet/cycle/router.
            bool shot = ( randGen()->flatDouble( 0, 1 ) < pir() );

            // if there is a new packet, generate new packet.
            if ( shot )
            {
                EsyNetworkPacketPtr new_pac = genPacketProfiles( id );
                new_pac->setTime( sim_cycle );
                pac_list.push_back( new_pac );
                writePacketToFile( new_pac );
            }
        }
    }

    m_packet_count += pac_list.size();
    if ( m_injected_packet >= 0 && m_packet_count >= m_injected_packet )
    {
        m_enable = false;
    }

    return pac_list;
}

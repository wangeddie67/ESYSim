/*
 * File name : esy_packetgenerator.h
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
 * @ingroup ESYNET_TRAFFIC_GENERATOR
 * @file esynet_packet_gen.h
 * @brief Declare the packet generator.
 */

#ifndef TRAFFICGEN_ESY_PACKETGENERATOR_H_
#define TRAFFICGEN_ESY_PACKETGENERATOR_H_

#include "esy_packetgenerator_fun.h"

/**
 * @ingroup ESYNET_TRAFFIC_GENERATOR
 * @brief Packet generator.
 */
class EsyPacketGenerator : public EsyPacketGeneratorFun
{
private:
    long m_injected_packet; /*!< @brief Limitation injected packet */
    long m_packet_count;    /*!< @brief Counter of injected packets. */
    bool m_enable;  /*!< @brief True if packet generator is enabled. */

    EsyNetworkPacketPtr mp_last_pac;

public:
    /**
     * @brief constructor function
     * @param network_cfg Reference to network configuration structure.
     * @param argument_cfg Reference to option list.
     */
    EsyPacketGenerator( long ni_count, 
                        const std::vector< long >& network_size,
                        EsynetTrafficProfile profile,
                        double pir,
                        long packet_size,
                        long injected_packet,
                        EsyRandGen* mp_rand_gen
                      );

    std::vector< EsyNetworkPacketPtr > generatePacket( double sim_cycle );
};

#endif // TRAFFICGEN_ESY_PACKETGENERATOR_H_

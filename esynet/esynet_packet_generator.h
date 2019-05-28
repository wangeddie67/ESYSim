/*
 * File name : esynet_packet_generator.h
 * Function : Define the packet generator.
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

#ifndef ESYNETPACKETGENERATOR_H
#define ESYNETPACKETGENERATOR_H

/* including head file */
#include "esy_interdata.h"
#include "esy_networkcfg.h"
#include "esy_iodatafile.h"
#include "esynet_config.h"
#include "esynet_sim_base.h"

class EsynetPacketGenerator
{
/* Properties */
private:
    /* General Information */
    /* Unique identification number */
    EsyNetworkCfg * m_network_cfg;  /*!< @brief Pointer to network configuration structure. */
    EsynetConfig * m_argu_cfg;  /*!< @brief Pointer to argument list .*/
    long m_ni_count;    /*!< @brief The number of NI in the network. */

    EsyDataFileIStream< EsyDataItemBenchmark > * m_tracein; /*!< @brief Pointer to benchmark trace interface. */

    bool m_enable;  /*!< @brief True if packet generator is enabled */
    long m_count;

    EsyDataItemBenchmark m_item;

public:
    /**
     * @brief constructor function
     * @param network_cfg Pointer to network configuration structure.
     * @param argument_cfg Pointer to option list.
     */
    EsynetPacketGenerator( EsyNetworkCfg * network_cfg, EsynetConfig * argument_cfg );

    /**
     * @brief Generate packet for one cycle
     * @return Packet generated in this cycle.
     */
    std::vector<EsynetFlit> generatePacket(double sim_cycle);
    /**
     * @brief Generate packet for one router at one cycle according to traffic profiles
     * @param id NI id.
     * @return Packet generated in this cycle.
     */
    std::vector<EsynetFlit> genPacketTrafficProfiles(long id);
};

#endif // ESYNETPACKETGENERATOR_H

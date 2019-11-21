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
 * @ingroup ESY_TRAFFIC_GEN
 * @file esy_packetgenerator.h
 * @brief Declare the timing packet generator.
 */

#ifndef TRAFFICGEN_ESY_PACKETGENERATOR_H_
#define TRAFFICGEN_ESY_PACKETGENERATOR_H_

#include "esy_packetgenerator_fun.h"

/**
 * @ingroup ESY_TRAFFIC_GEN
 * @brief Packet generator.
 */
class EsyPacketGenerator : public EsyPacketGeneratorFun
{
private:
    long m_injected_packet; /*!< @brief Limitation injected packet */
    long m_packet_count;    /*!< @brief Counter of injected packets. */
    bool m_enable;  /*!< @brief True if packet generator is enabled. */

    EsyNetworkPacketPtr mp_last_pac;    /*!< @brief Last read packet from file. */
    uint64_t m_max_time;    /*!< @brief Maximum cycle to call generator. */

public:
    /**
     * @brief Construct function with specified fields.
     * @param ni_count Total number of network interface
     * @param network_size Network size.
     * @param profile Traffic profiles.
     * @param pir Packet injection rate.
     * @param packet_size Size of generated packet.
     * @param injected_packet Total injected packet.
     * @param rand_gen Pointer to random generator.
     */
    EsyPacketGenerator( long ni_count, 
                        const std::vector< long >& network_size,
                        EsynetTrafficProfile profile,
                        double pir,
                        long packet_size,
                        long injected_packet,
                        EsyRandGen* rand_gen
                      );

    /**
     * @brief Generate packet for specified cycle.
     * 
     * Generate packet for specified cycle @a sim_cycle.
     * - If traffic profile is TRACE, read packet from trace file.
     * - Otherwise, generate packet by random generator and traffic profiles.
     * 
     * @param sim_cycle Simulation cycle for this call.
     * @return Vector of generated packet. If there is no packet generated, 
     * returned vector is empty.
     */
    std::vector< EsyNetworkPacketPtr > generatePacket( uint64_t sim_cycle );

    /**
     * @brief Open trace file to read.
     * 
     * Option trace file to read. Set #m_max_time by the maximum time from 
     * file.
     * 
     * @param name file name to open.
     * @return True if the file is opened correctly.
     */
    bool openReadFile( const std::string& name )
    {
        m_max_time = EsyPacketGeneratorFun::openReadFile( name );
        return m_max_time >= 0;
    }
    /**
     * @brief Open trace file to write.
     * @param name file name to open.
     * @return True if the file is opened correctly.
     */
    bool openWriteFile( const std::string& name )
    {
        return EsyPacketGeneratorFun::openWriteFile( name );
    }
    /**
     * @brief Close trace file to read.
     * @return True if the file is closed correctly.
     */
    bool closeReadFile()
    {
        return EsyPacketGeneratorFun::closeReadFile();
    }
    /**
     * @brief Close trace file to write.
     *
     * Write #m_max_time to the head of file.
     * 
     * @return True if the file is closed correctly.
     */
    bool closeWriteFile()
    {
        return EsyPacketGeneratorFun::closeWriteFile( m_max_time );
    }
};

#endif // TRAFFICGEN_ESY_PACKETGENERATOR_H_

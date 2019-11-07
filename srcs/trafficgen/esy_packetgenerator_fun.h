/*
 * File name : esy_packetgenerator_fun.h
 * Function : Function model of the packet generator.
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

#ifndef TRAFFICGEN_ESY_PACKETGENERATOR_FUN_H_
#define TRAFFICGEN_ESY_PACKETGENERATOR_FUN_H_

#include "../randgen/esy_random.h"
#include <fstream>
#include <memory>

class EsyNetworkPacket
{
private:
    uint64_t m_time;
    uint32_t m_src_ni;
    uint32_t m_dst_ni;
    uint32_t m_length;

public:
    EsyNetworkPacket( uint64_t time,
                      uint32_t src,
                      uint32_t dst,
                      uint32_t length )
        : m_time( time )
        , m_src_ni( src )
        , m_dst_ni( dst )
        , m_length( length )
    {}

    uint64_t time() const { return m_time; }
    uint32_t srcNi() const { return m_src_ni; }
    uint32_t dstNi() const { return m_dst_ni; }
    uint32_t length() const { return m_length; }

    void setTime( uint64_t time ) { m_time = time; }
};

typedef std::shared_ptr< EsyNetworkPacket > EsyNetworkPacketPtr;

/**
 * @ingroup ESYNET_TRAFFIC_GENERATOR
 * @brief Packet generator.
 */
class EsyPacketGeneratorFun
{
public:
    /**
    * @brief Traffic generation profiles.
    */
    enum EsynetTrafficProfile
    {
        TP_UNIFORM,     /*!< @brief Uniform traffic. */
        TP_TRANSPOSE1,  /*!< @brief Transpose traffic 1.
            (x,y) -> (Y-1-y,X-1-x), X,Y is the size of network */
        TP_TRANSPOSE2,  /*!< @brief Transpose traffic 2. (x,y) -> (y,x) */
        TP_BITREVERSAL, /*!< @brief Bit-reversal traffic. */
        TP_BUTTERFLY,   /*!< @brief Butterfly traffic. */
        TP_SHUFFLE,     /*!< @brief Shuffly traffic. */
        TP_TRACE,       /*!< @brief Based on trace. */
    };

private:
    long m_ni_count;  /*!< @brief number of nodes. */
    std::vector< long > m_network_size; /*!< @brief Network size. */
    EsynetTrafficProfile m_traffic_profile; /*!< @brief Traffic profile. */
    double m_packet_inject_rate;    /*!< @brief Packet injection rate. */
    long m_packet_size; /*!< @brief Packet size. */

    EsyRandGen* mp_rand_gen;    /*!< @brief Pointer to random generator. */

    uint64_t m_max_time;            /*!< @brief Maximum time in trace file. */
    std::ifstream m_read_stream;    /*!< @brief Read file stream. */
    std::ofstream m_write_stream;   /*!< @brief Write file stream. */

public:
    /**
     * @brief constructor function
     * @param network_cfg Reference to network configuration structure.
     * @param argument_cfg Reference to option list.
     */
    EsyPacketGeneratorFun( long ni_count, 
                           std::vector< long > network_size,
                           EsynetTrafficProfile profile,
                           double pir,
                           long packet_size,
                           EsyRandGen* rand_gen
                         );

    long niCount() const { return m_ni_count; }
    EsynetTrafficProfile profile() const { return m_traffic_profile; }
    double pir() const { return m_packet_inject_rate; }
    double pacSize() const { return m_packet_size; }
    EsyRandGen* randGen() const { return mp_rand_gen; }

    /**
     * @brief Generate packet for one router at one cycle according to traffic
     *        profiles
     * @param id NI id.
     * @return Packet generated in this cycle.
     */
    EsyNetworkPacketPtr genPacketProfiles( long id );

    std::ifstream& readStream()  { return m_read_stream; }
    std::ofstream& writeStream() { return m_write_stream; }
    bool openReadFile( const std::string& name );
    bool openWriteFile( const std::string& name );
    bool closeReadFile();
    bool closeWriteFile();
    EsyNetworkPacketPtr readPacketFromFile();
    bool writePacketToFile( EsyNetworkPacketPtr pac );
};

#endif // TRAFFICGEN_ESY_PACKETGENERATOR_FUN_H_

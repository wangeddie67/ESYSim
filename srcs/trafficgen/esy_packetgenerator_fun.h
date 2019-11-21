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
 * @defgroup ESY_TRAFFIC_GEN Traffic Generator
 */
/**
 * @ingroup ESY_TRAFFIC_GEN
 * @file esy_packetgenerator_fun.h
 * @brief Declare the packet generator.
 */

#ifndef TRAFFICGEN_ESY_PACKETGENERATOR_FUN_H_
#define TRAFFICGEN_ESY_PACKETGENERATOR_FUN_H_

#include "../randgen/esy_random.h"
#include <fstream>
#include <memory>

/**
 * @ingroup ESY_TRAFFIC_GEN
 * @brief Generated packet.
 */
class EsyNetworkPacket
{
private:
    uint64_t m_time;    /*!< @brief Time to inject packet. */
    uint32_t m_src_ni;  /*!< @brief Source NI. */
    uint32_t m_dst_ni;  /*!< @brief Destination NI. */
    uint32_t m_length;  /*!< @brief Packet length. */

public:
    /**
     * @brief Construct function with specified fields.
     * @param time   Time to inject packet.
     * @param src    Source NI.
     * @param dst    Destination NI.
     * @param length Packet length.
     */
    EsyNetworkPacket( uint64_t time,
                      uint32_t src,
                      uint32_t dst,
                      uint32_t length )
        : m_time( time )
        , m_src_ni( src )
        , m_dst_ni( dst )
        , m_length( length )
    {}

    /**
     * @name Function to access variables.
     * @{
     */
    /**
     * @brief Return time to inject packet. 
     */
    uint64_t time() const { return m_time; }
    /**
     * @brief Return source NI.
     */
    uint32_t srcNi() const { return m_src_ni; }
    /**
     * @brief Return destination NI.
     */
    uint32_t dstNi() const { return m_dst_ni; }
    /**
     * @brief Return length.
     */
    uint32_t length() const { return m_length; }
    /**
     * @}
     */

    /**
     * @name Function to set variables.
     * @{
     */
    /**
     * @brief Set time to inject packet.
     */
    void setTime( uint64_t time ) { m_time = time; }
    /**
     * @}
     */
};

/**
 * @ingroup ESY_TRAFFIC_GEN
 * @brief Shared pointer of generated packets.
 */
typedef std::shared_ptr< EsyNetworkPacket > EsyNetworkPacketPtr;

/**
 * @ingroup ESY_TRAFFIC_GEN
 * @brief Function model of packet generator.
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

    std::ifstream m_read_stream;    /*!< @brief Read file stream. */
    std::ofstream m_write_stream;   /*!< @brief Write file stream. */
    uint8_t m_fstream_buf[ 256 ];  /*!< @brief Data buffer for file stream. */

public:
    /**
     * @brief Construct function with specified fields.
     * @param ni_count Total number of network interface
     * @param network_size Network size.
     * @param profile Traffic profiles.
     * @param pir Packet injection rate.
     * @param packet_size Size of generated packet.
     * @param rand_gen Pointer to random generator.
     */
    EsyPacketGeneratorFun( long ni_count, 
                           std::vector< long > network_size,
                           EsynetTrafficProfile profile,
                           double pir,
                           long packet_size,
                           EsyRandGen* rand_gen
                         );

    /**
     * @name Function to access variables.
     * @{
     */
    /**
     * @brief Return number of NIs.
     */
    long niCount() const { return m_ni_count; }
    /**
     * @brief Return traffic profile.
     */
    EsynetTrafficProfile profile() const { return m_traffic_profile; }
    /**
     * @brief Return packet injected rate.
     */
    double pir() const { return m_packet_inject_rate; }
    /**
     * @brief Return packet size.
     */
    double pacSize() const { return m_packet_size; }
    /**
     * @brief Return pointer to random generator.
     */
    EsyRandGen* randGen() const { return mp_rand_gen; }
    /**
     * @brief Return input stream of trace file to read.
     */
    std::ifstream& readStream()  { return m_read_stream; }
    /**
     * @brief Return output stream of trace file to write.
     */
    std::ofstream& writeStream() { return m_write_stream; }
    /**
     * @}
     */

    /**
     * @brief Generate packet for one router at one cycle according to traffic
     *        profiles
     * @param id NI id.
     * @return Packet generated in this cycle.
     */
    EsyNetworkPacketPtr genPacketProfiles( long id );

    /**
     * @brief Open trace file to read.
     * @param name file name to open.
     * @return The maximum time from file.
     * @retval 0 if the file is not open correctly.
     */
    uint64_t openReadFile( const std::string& name );
    /**
     * @brief Open trace file to write.
     * @param name file name to open.
     * @return True if the file is opened correctly.
     */
    bool openWriteFile( const std::string& name );
    /**
     * @brief Close trace file to read.
     * @return True if the file is closed correctly.
     */
    bool closeReadFile();
    /**
     * @brief Close trace file to write.
     *
     * Write @a max_time to the head of trace file.
     * 
     * @param max_time Maximum cycle to write at the head of trace file.
     * @return True if the file is closed correctly.
     */
    bool closeWriteFile( uint64_t max_time );

    /**
     * @brief Read one packet from trace file.
     * @return Shared pointer of the read network packet.
     * @retval Null-pointer if no packet is read.
     */
    EsyNetworkPacketPtr readPacketFromFile();
    /**
     * @brief Write one packet from trace file.
     * @param pac Shared pointer of the read network packet.
     * @return True if the packet is write correctly.
     */
    bool writePacketToFile( EsyNetworkPacketPtr pac );
};

#endif // TRAFFICGEN_ESY_PACKETGENERATOR_FUN_H_

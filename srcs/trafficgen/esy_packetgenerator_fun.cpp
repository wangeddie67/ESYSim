/*
 * File name : esy_packetgenerator_fun.cpp
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
 * @ingroup ESY_TRAFFIC_GEN
 * @file esy_packetgenerator_fun.cpp
 * @brief Implement the packet generator.
 */

#include <cmath>
#include "esy_packetgenerator_fun.h"
#include "esy_traffic.pb.h"


EsyPacketGeneratorFun::EsyPacketGeneratorFun( long ni_count, 
                                              std::vector< long > network_size,
                                              EsynetTrafficProfile profile,
                                              double pir,
                                              long packet_size,
                                              EsyRandGen* rand_gen
                                            )
    : m_ni_count( ni_count )
    , m_network_size( network_size )
    , m_traffic_profile( profile )
    , m_packet_inject_rate( pir )
    , m_packet_size( packet_size )
    , mp_rand_gen( rand_gen )
{
    if ( profile == TP_UNIFORM )
    {
        assert( mp_rand_gen != NULL );
    }
    else if ( profile == TP_TRACE )
    {
    }
    else
    {
        assert( m_network_size.size() == 2 );
        assert( m_ni_count == (m_network_size[ 0 ] * m_network_size[ 1 ]) );
    }
}

/**
 * @name Bit Operation Functions
 * @{
 */
/**
 * @brief Set the w-th bit in integer x as v.
 * @param x integer to operate
 * @param w the position of bit to operate
 * @param v the value of bit
 */
inline void setBit( int &x, int w, int v )
{
    int mask = 1 << w;

    // if the v is 1, set w-th bit 1.
    if ( v == 1 )
    {
        x = x | mask;
    }
    // if the v is 0, set w-th bit 0.
    else if ( v == 0 )
    {
        x = x & ~mask;
    }
}

/**
 * @brief Get the value of the w-th bit in integer x.
 * @param x integer to operate
 * @param w the position of bit to operate
 * @return the value of the bit.
 */
inline int getBit( int x, int w )
{
    return ( x >> w ) & 1;
}

/**
 * @brief Return the log2(x) and round up to a integer.
 * @param x the digital to log
 * @return log2(x) and round up
 */
inline double log2Ceil( double x )
{
    return ceil( log( x ) / log( 2.0 ) );
}
/**
 * @}
 */

EsyNetworkPacketPtr
EsyPacketGeneratorFun::genPacketProfiles( long id )
{
    assert( m_traffic_profile != TP_TRACE );

    // coordinate of source id.
    int nbits = (int)log2Ceil( m_ni_count );
    long axis_x = id % m_network_size[ 0 ];
    long axis_y = id / m_network_size[ 0 ];
    long size_x = m_network_size[ 0 ];
    long size_y = m_network_size[ 1 ];

    // generate new packet.
    int dst = id;
    switch ( m_traffic_profile )
    {
    // Determine the destination in new packet according to Random rule.
    case TP_UNIFORM : 
        dst = mp_rand_gen->flatLong( 0, m_ni_count );
        break;
    // Determine the destination in new packet according to Transpose1 rule
    // (x,y) -> (Y-1-y,X-1-x), X,Y is the size of network.
    case TP_TRANSPOSE1: 
        {
            std::vector< long > dst_cord( 2, 0 );
            long dst_x = size_y - 1 - axis_y;
            long dst_y = size_x - 1 - axis_x;
            dst = dst_x + dst_y * m_network_size[ 0 ];
        }
        break;
    // Determine the destination in new packet according to Transpose2 rule
    // (x,y) -> (y,x).
    case TP_TRANSPOSE2: 
        {
            std::vector< long > dst_cord( 2, 0 );
            long dst_x = axis_y;
            long dst_y = axis_x;
            dst = dst_x + dst_y * m_network_size[ 0 ];
        }
        break;
    // Determine the destination in new packet according to bit reversal rule.
    case TP_BITREVERSAL: 
        {
            for ( int l_bit = 0; l_bit < nbits; l_bit ++ )
            {
                setBit( dst, l_bit, getBit( id, nbits - l_bit - 1 ) );
            }
        }
        break;
    // Determine the destination in new packet according to suffle rule
    // loop left shift.
    case TP_SHUFFLE: 
        {
            for ( int l_bit = 0; l_bit < nbits - 1; l_bit ++ )
            {
                setBit( dst, l_bit + 1, getBit( id, l_bit ) );
            } 
            setBit( dst, 0, getBit( id, nbits - 1 ) );
        }
        break;
    // Determine the destination in new packet according to butterfly rule
    // swap the MSB and LSB.
    case TP_BUTTERFLY: 
        {
            for ( int l_bit = 1; l_bit < nbits - 1; l_bit ++ )
            {
                setBit(dst, l_bit, getBit(id, l_bit) );
            }
            setBit( dst, 0, getBit( id, nbits - 1 ) );
            setBit( dst, nbits - 1, getBit( id, 0 ) );
        }
        break;
    // if unknown traffic, call asseration.
    default:
        break;
    }

    EsyNetworkPacket* pac = new EsyNetworkPacket( 0, id, dst, m_packet_size );

    return EsyNetworkPacketPtr( pac );
}

uint64_t 
EsyPacketGeneratorFun::openReadFile( const std::string& name )
{
    m_read_stream.open( name, std::ios::in | std::ios::binary );
    uint64_t max_time;
    m_read_stream.read( (char*)&max_time, sizeof( uint64_t ) );
    if ( m_read_stream )
    {
        return max_time;
    }
    else
    {
        return 0;
    }
}

bool
EsyPacketGeneratorFun::openWriteFile( const std::string& name )
{
    m_write_stream.open( name, std::ios::out | std::ios::binary );
    uint64_t max_time = 0;
    m_write_stream.write( (char*)&max_time, sizeof( uint64_t ) );
    if ( m_write_stream )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
EsyPacketGeneratorFun::closeReadFile()
{
    if ( m_read_stream )
    {
        m_read_stream.close();
    }
    return true;
}

bool
EsyPacketGeneratorFun::closeWriteFile( uint64_t max_time )
{
    if ( m_write_stream )
    {
        m_write_stream.flush();
        m_write_stream.seekp( 0, std::ios::beg );
        uint64_t max_time_t = max_time;
        m_write_stream.write( (char*)&max_time_t, sizeof(uint64_t) );
        m_write_stream.flush();
        m_write_stream.close();
    }
    return true;
}

EsyNetworkPacketPtr
EsyPacketGeneratorFun::readPacketFromFile()
{
    m_read_stream.read( (char*)m_fstream_buf, sizeof( uint8_t ) );
    uint8_t pac_size = m_fstream_buf[ 0 ];
    m_read_stream.read( (char*)m_fstream_buf + 1,
                        sizeof( uint8_t ) * pac_size );
    if ( m_read_stream.eof() )
    {
        return EsyNetworkPacketPtr( NULL );
    }

    esy_traffic::NetworkPacket pac_bp;
    if ( pac_bp.ParseFromArray( m_fstream_buf + 1, pac_size ) )
    {
        EsyNetworkPacket* pac = new EsyNetworkPacket( pac_bp.time(),
                                                      pac_bp.src_ni(),
                                                      pac_bp.dst_ni(),
                                                      pac_bp.length() );
        return EsyNetworkPacketPtr( pac );
    }
    else
    {
        return EsyNetworkPacketPtr( NULL );
    }
}

bool
EsyPacketGeneratorFun::writePacketToFile( EsyNetworkPacketPtr pac )
{
    esy_traffic::NetworkPacket pac_bp;
    pac_bp.set_time( pac->time() );
    pac_bp.set_src_ni( pac->srcNi() );
    pac_bp.set_dst_ni( pac->dstNi() );
    pac_bp.set_length( pac->length() );

    uint8_t pac_size = pac_bp.ByteSize();
    m_fstream_buf[0] = pac_size;
    pac_bp.SerializeToArray( m_fstream_buf + 1, pac_size );
    m_write_stream.write( (char*)m_fstream_buf,
                          sizeof(uint8_t) * (pac_size + 1) );

    if ( m_write_stream )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * File name : esynet_packet_generator.cc
 * Function : Implement the packet generator.
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
 * @file esynet_packet_generator.cc
 * @brief Implement the packet generator.
 */

#include "esynet_packet_generator.h"
#include "esynet_random_unit.h"


EsynetPacketGenerator::EsynetPacketGenerator( EsyNetworkCfg * network_cfg, EsynetConfig * argument_cfg )
    : m_network_cfg( network_cfg )
    , m_argu_cfg( argument_cfg )
    , m_ni_count( network_cfg->setNiCount() )
    , m_tracein( NULL )
    , m_enable( !argument_cfg->trafficInjectDisable() )
    , m_count( 0 )
{
    if ( argument_cfg->inputTraceEnable() )
    {
        // open becnhmark trace
        m_tracein = new EsyDataFileIStream< EsyDataItemBenchmark >(
            argument_cfg->inputTraceBufferSize(), argument_cfg->inputTraceFileName(), BENCHMARK_EXTENSION, !argument_cfg->inputTraceTextEnable() );
        // open failure, close traffic generator
        if ( !m_tracein->openSuccess() )
        {
            cout << "Cannot open traffic trace file (T1) " << argument_cfg->inputTraceFileName() << endl;
            m_enable = false;
        }
        // benchmark is empty, close traffic generator
        m_item = m_tracein->getNextItem();
        if ( m_item.time() == -1 )
        {
            cout << "Cannot open traffic trace file (T2) " << argument_cfg->inputTraceFileName() << endl;
            m_enable = false;
        }
    }
}

EsynetPacketGenerator::~EsynetPacketGenerator()
{
    if ( m_tracein != NULL )
    {
        delete m_tracein;
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

std::vector< EsynetFlit >
EsynetPacketGenerator::generatePacket(double sim_cycle)
{
    std::vector< EsynetFlit > pac_list;
    if ( m_enable )
    {
        // if enable input trace, read from file
        if ( m_argu_cfg->inputTraceEnable() )
        {
            /* loop all the packages before simulation cycle */
            while ( sim_cycle >= m_item.time() )
            {
                EsynetFlit flit_t( m_count, m_item.size(), EsynetFlit::FLIT_HEAD, m_item.src(), m_item.dst(), -1, esynet::EsynetPayload() );
                pac_list.push_back( flit_t );
                m_count += 1;

                // read next packet. If the item is error, disable packet generator.
                m_item = m_tracein->getNextItem();
                if ( m_item.time() == -1 )
                {
                    m_enable = false;
                }
            }
        }
        // otherwise, generate by traffic profiles.
        else
        {
            for ( int id = 0; id < m_ni_count; id ++ )
            {
                std::vector< EsynetFlit > pac_list_one = genPacketTrafficProfiles( id );
                for ( int i = 0; i < pac_list_one.size(); i ++ )
                {
                    pac_list.push_back( pac_list_one[ i ] );
                }
                if ( m_argu_cfg->injectedPacket() >= 0 && m_count >= m_argu_cfg->injectedPacket() )
                {
                    m_enable = false;
                }
            }
        }
    }
    return pac_list;
}

std::vector<EsynetFlit>
EsynetPacketGenerator::genPacketTrafficProfiles( long id )
{
    std::vector< EsynetFlit > pac_list;
    if ( m_enable )
    {
        // detemine if there is a new packet, if the random number is lower then pir, shot will be TRUE;
        // Thus, the packet inject rate ranges from 0 to 1 packet/cycle/router.
        bool shot = ( EsynetSRGenFlatDouble( 0, 1 ) < m_argu_cfg->trafficPir() );
        /* if there is a new packet, determine each field for the new packet. */
        if ( shot )
        {
            // coordinate of source id.
            int nbits = (int)log2Ceil( m_ni_count );
            std::vector< long > src_cord = m_network_cfg->seq2Coord( id );

            // generate new packet.
            int dst = id;
            switch ( (esynet::EsynetTrafficProfile)m_argu_cfg->trafficRule() )
            {
            // Determine the destination in new packet according to Random rule.
            case esynet::TP_UNIFORM : 
                dst = EsynetSRGenFlatLong( 0, m_ni_count );
                break;
            // Determine the destination in new packet according to Transpose1 rule (x,y) -> (Y-1-y,X-1-x), X,Y is the size of network.
            case esynet::TP_TRANSPOSE1: 
                {
                    vector< long > dst_cord( 2, 0 );
                    dst_cord[ EsyNetworkCfg::AX_X ] = m_network_cfg->size( EsyNetworkCfg::AX_Y ) - 1 - src_cord[ EsyNetworkCfg::AX_Y ];
                    dst_cord[ EsyNetworkCfg::AX_Y ] = m_network_cfg->size( EsyNetworkCfg::AX_X ) - 1 - src_cord[ EsyNetworkCfg::AX_X ];
                    dst = m_network_cfg->coord2Seq( dst_cord );
                }
                break;
            // Determine the destination in new packet according to Transpose2 rule (x,y) -> (y,x).
            case esynet::TP_TRANSPOSE2: 
                {
                    vector< long > dst_cord( 2, 0 );
                    dst_cord[ EsyNetworkCfg::AX_X ] = src_cord[ EsyNetworkCfg::AX_Y ];
                    dst_cord[ EsyNetworkCfg::AX_Y ] = src_cord[ EsyNetworkCfg::AX_X ];
                    dst = m_network_cfg->coord2Seq( dst_cord );
                }
                break;
            // Determine the destination in new packet according to bit reversal rule.
            case esynet::TP_BITREVERSAL: 
                {
                    for ( int l_bit = 0; l_bit < nbits; l_bit ++ )
                    {
                        setBit( dst, l_bit, getBit( id, nbits - l_bit - 1 ) );
                    }
                }
                break;
            // Determine the destination in new packet according to suffle rule loop left shift.
            case esynet::TP_SHUFFLE: 
                {
                    for ( int l_bit = 0; l_bit < nbits - 1; l_bit ++ )
                    {
                        setBit( dst, l_bit + 1, getBit( id, l_bit ) );
                    } 
                    setBit( dst, 0, getBit( id, nbits - 1 ) );
                }
                break;
            // Determine the destination in new packet according to butterfly rule swap the MSB and LSB.
            case esynet::TP_BUTTERFLY: 
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
            // insert new packets
            EsynetFlit flit_t(m_count, m_argu_cfg->packetSize(), EsynetFlit::FLIT_HEAD, id, dst, -1, esynet::EsynetPayload());
            m_count += 1;
            pac_list.push_back(flit_t);
        }
    }
    return pac_list;
}

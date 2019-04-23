/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#include "esynet_packet_generator.h"

/* Bit Operation Functions */
/* Set the w-th bit in integer x as v */
void setBit( int &x, int w, int v );
/* Get the value of the w-th bit in integer x */
int getBit( int x, int w );
/* Return the log2(x) and round up to a integer */
double log2Ceil( double x );

EsynetPacketGenerator::EsynetPacketGenerator(
	EsyNetworkCfg * network_cfg, EsynetConfig * argument_cfg ) :
	m_network_cfg(network_cfg), m_argu_cfg(argument_cfg),
	m_enable(!argument_cfg->trafficInjectDisable() &&
		!argument_cfg->inputTraceEnable()),
	m_count(0)
{}

/*************************************************
  Function :
    void EsynetNI::setBit(int &x, int w, int v)
  Description :
    Set the w-th bit in integer x as v.
  Called By:
    void EsynetNI::trafficBitReversal(BenchmarkItem * p)
    void EsynetNI::trafficShuffle(BenchmarkItem * p)
    void EsynetNI::trafficButterfly(BenchmarkItem * p)
  Input:
    int& x  integer to operate
    int  w  the position of bit to operate
    int  v  the value of bit
*************************************************/
void setBit( int &x, int w, int v )
{
	/* bit mask of w-th bit in integer */
	int mask = 1 << w;

	/* if the v is 1, set w-th bit 1. */
	if ( v == 1 )
	{
		x = x | mask;
	} /* if ( v == 1 ) */
	/* if the v is 0, set w-th bit 0. */
	else if ( v == 0 )
	{
		x = x & ~mask;
	} /* else if ( v == 0 ) */
	/* if the v is neither 0 nor 1, call assertion report. */
}

/*************************************************
  Function :
    int EsynetNI::getBit(int x, int w)
  Description :
    Get the value of the w-th bit in integer x.
  Called By :
    void EsynetNI::trafficBitReversal(BenchmarkItem * p)
    void EsynetNI::trafficShuffle(BenchmarkItem * p)
    void EsynetNI::trafficButterfly(BenchmarkItem * p)
  Input :
    int  x  integer to operate
    int  w  the position of bit to operate
  Return :
    int  v  the value of the bit
*************************************************/
int getBit( int x, int w )
{
	return ( x >> w ) & 1;
}

/*************************************************
  Function :
    double EsynetNI::log2Ceil(double x)
  Description :
    Return the log2(x) and round up to a integer.
  Called By :
    void EsynetNI::trafficBitReversal(BenchmarkItem * p)
    void EsynetNI::trafficShuffle(BenchmarkItem * p)
    void EsynetNI::trafficButterfly(BenchmarkItem * p)
  Input :
    double  x  the digital to log
  Return
    double  log2(x) and round up
*************************************************/
double log2Ceil( double x )
{
	return ceil( log( x ) / log( 2.0 ) );
}

/*************************************************
  Function :
    void EsynetNI::generatePacket()
  Description :
    Generate packets.
  Calls :
    double SRGen::flatDouble( double low, double high )
    const BenchmarkItem & EsynetNI::trafficRandom()
    const BenchmarkItem & EsynetNI::trafficTranspose1()
    const BenchmarkItem & EsynetNI::trafficTranspose2()
    const BenchmarkItem & EsynetNI::trafficBitReversal()
    const BenchmarkItem & EsynetNI::trafficShuffle()
    const BenchmarkItem & EsynetNI::trafficButterfly()
  Called By:
    void SimRouter::runBeforeRouter()
*************************************************/

vector< EsynetFlit > EsynetPacketGenerator::generatePacket()
{
	vector< EsynetFlit > pac_list;
	if (m_enable)
	{
		for (int id = 0; id < m_network_cfg->routerCount(); id ++)
		{
			vector<EsynetFlit> pac_list_one = generatePacket(id);
			for (int i = 0; i < pac_list_one.size(); i ++)
			{
				pac_list.push_back(pac_list_one[i]);
			}
			m_count += pac_list_one.size();
			if (m_argu_cfg->injectedPacket() >= 0 && m_count >= m_argu_cfg->injectedPacket())
			{
				m_enable = false;
			}
		}
	}
	return pac_list;
}

vector<EsynetFlit> EsynetPacketGenerator::generatePacket(long id)
{
	vector< EsynetFlit > pac_list;
	if (m_enable)
	{
		/* detemine if there is a new packet, if the random number is lower then 
		* pir, shot will be TRUE;
		* Thus, the packet inject rate ranges from 0 to 1 packet/cycle/router. */
		bool shot = (EsynetSRGenFlatDouble(0, 1) < m_argu_cfg->trafficPir() );
		/* if there is a new packet, determine each field for the new packet. */
		if ( shot )
		{
			/* size of source id (bit) */
			int nbits = (int)log2Ceil( m_network_cfg->routerCount() );
			/* coordinate of source id. */
			vector< long > src_cord = m_network_cfg->seq2Coord( id );
			
			/* generate new packet */
			int dst = id;
			switch ( (TrafficRule)m_argu_cfg->trafficRule() )
			{
			/* Determine the destination in new packet according to Random rule */
			case TRAFFIC_RANDOM : 
				dst = EsynetSRGenFlatLong(0, m_network_cfg->routerCount());
				break;
			/* Determine the destination in new packet according to Transpose1 rule
			* (x,y) -> (Y-1-y,X-1-x), X,Y is the size of network */
			case TRAFFIC_TRANSPOSE1: 
				{
					vector< long > dst_cord( 2, 0 );
					dst_cord[ EsyNetworkCfg::AX_X ] = 
						m_network_cfg->size( EsyNetworkCfg::AX_Y ) - 1 
							- src_cord[ EsyNetworkCfg::AX_Y ];
					dst_cord[ EsyNetworkCfg::AX_Y ] = 
						m_network_cfg->size( EsyNetworkCfg::AX_X ) - 1 
							- src_cord[ EsyNetworkCfg::AX_X ];
					dst = m_network_cfg->coord2Seq( dst_cord );
				}
				break;
			/* Determine the destination in new packet according to Transpose2 rule
			* (x,y) -> (y,x) */
			case TRAFFIC_TRANSPOSE2: 
				{
					vector< long > dst_cord( 2, 0 );;
					dst_cord[ EsyNetworkCfg::AX_X ] = 
						src_cord[ EsyNetworkCfg::AX_Y ];
					dst_cord[ EsyNetworkCfg::AX_Y ] = 
						src_cord[ EsyNetworkCfg::AX_X ];
					dst = m_network_cfg->coord2Seq( dst_cord );
				}
				break;
			/* Determine the destination in new packet according to bit reversal 
			* rule */
			case TRAFFIC_BIT_REVERSAL: 
				{
					for ( int l_bit = 0; l_bit < nbits; l_bit ++ )
					{
						setBit( dst, l_bit, getBit( id, nbits - l_bit - 1 ) );
					}
				}
				break;
			/* Determine the destination in new packet according to suffle rule
			* loop left shift */
			case TRAFFIC_SHUFFLE: 
				{
					for ( int l_bit = 0; l_bit < nbits - 1; l_bit ++ )
					{
						setBit( dst, l_bit + 1, getBit( id, l_bit ) );
					} 
					setBit( dst, 0, getBit( id, nbits - 1 ) );
				}
				break;
			/* Determine the destination in new packet according to butterfly rule
			* swap the MSB and LSB */
			case TRAFFIC_BUTTERFLY: 
				{
					for ( int l_bit = 1; l_bit < nbits - 1; l_bit ++ )
					{
						setBit(dst, l_bit, getBit(id, l_bit) );
					}
					setBit( dst, 0, getBit( id, nbits - 1 ) );
					setBit( dst, nbits - 1, getBit( id, 0 ) );
				}
				break;
			/* if unknown traffic, call asseration */
			default: break;
			} /* switch ( m_traffic ) */
			/* insert new packets */
			EsynetFlit flit_t(m_count, m_argu_cfg->packetSize(), EsynetFlit::FLIT_HEAD, 
							  id, dst, -1, -1, DataType());
			pac_list.push_back(flit_t);
		} /* if ( shot ) */
	}
	return pac_list;
}

/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#include "esynet_statistics.h"

EsynetRouterStatistic::EsynetRouterStatistic() :
    m_bist_empty_counter( 0 ),
    m_bist_empty_cycle( 0.0 ),
    m_bist_recover_counter( 0 ),
    m_bist_recover_cycle( 0.0 )
{
}

EsynetRouterStatistic::EsynetRouterStatistic(const EsynetRouterStatistic &t) :
    m_bist_empty_counter( t.bistEmptyCounter() ),
    m_bist_empty_cycle( t.bistEmptyCycle() ),
    m_bist_recover_counter( t.bistRecoverCounter() ),
    m_bist_recover_cycle( t.bistRecoverCycle() )
{
}

void EsynetRouterStatistic::add(const EsynetRouterStatistic& t)
{
    m_bist_empty_counter += t.bistEmptyCounter();
    m_bist_empty_cycle += t.bistEmptyCycle();
    m_bist_recover_counter += t.bistRecoverCounter();
    m_bist_recover_cycle += t.bistRecoverCycle();
}

EsynetNIStatistic::EsynetNIStatistic() :
    m_injected_packet( 0 ),
    m_inject_start_time( -1 ),
    m_inject_stop_time( -1 ),
    m_accepted_packet( 0 ),
    m_accept_start_time( -1 ),
    m_accept_stop_time( -1 ),
    m_total_delay( 0.0 ),
    m_max_delay( 0.0 ),
    m_inject_ack_packet( 0 ),
    m_accept_ack_packet( 0 ),
    m_retransmission_packet( 0 ),
    m_drop_flit( 0 ),
    m_nondrop_packet( 0 ),
    m_nondrop_ack_packet( 0 )
{
}

EsynetNIStatistic::EsynetNIStatistic(const EsynetNIStatistic &t) :
    m_injected_packet( t.injectedPacket() ),
    m_inject_start_time( t.injectStartTime() ),
    m_inject_stop_time( t.injectStopTime() ),
    m_accepted_packet( t.acceptedPacket() ),
    m_accept_start_time( t.acceptStartTime() ),
    m_accept_stop_time( t.acceptStopTime() ),
    m_total_delay( t.totalDelay() ),
    m_max_delay( t.maxDelay() ),
    m_inject_ack_packet( t.injectAckPacket() ),
    m_accept_ack_packet( t.acceptAckPacket() ),
    m_retransmission_packet( t.retransmissionPacket() ),
    m_drop_flit( t.dropFlit() ),
    m_nondrop_packet( t.nonDropPacket() ),
    m_nondrop_ack_packet( t.nonDropAckPacket() )
{
}

void EsynetNIStatistic::add(const EsynetNIStatistic& t)
{
    m_injected_packet += t.injectedPacket();
	if ( t.injectStartTime() < m_inject_start_time && t.injectStartTime() > 0 )
	{
		m_inject_start_time = t.injectStartTime();
	}
	if ( t.injectStopTime() > m_inject_stop_time )
	{
		m_inject_stop_time = t.injectStopTime();
	}
	
    m_accepted_packet += t.acceptedPacket();
	if ( t.acceptStartTime() < m_accept_start_time && t.acceptStartTime() > 0 )
	{
		m_accept_start_time = t.acceptStartTime();
	}
	if ( t.acceptStopTime() > m_accept_stop_time )
	{
		m_accept_stop_time = t.acceptStopTime();
	}
	
    m_total_delay += t.totalDelay();
	if ( t.maxDelay() > m_max_delay )
	{
		m_max_delay = t.maxDelay();
	}

	m_inject_ack_packet += t.injectAckPacket();
    m_accept_ack_packet += t.acceptAckPacket();
    m_retransmission_packet += t.retransmissionPacket();

	m_drop_flit += t.dropFlit();
    m_nondrop_packet += t.nonDropPacket();
    m_nondrop_ack_packet += t.nonDropAckPacket();
}

EsynetFoundationStatistic::EsynetFoundationStatistic() :
    m_injected_packet( 0 ),
    m_inject_start_time( -1 ),
    m_inject_stop_time( -1 ),
    
    m_accepted_packet( 0 ),
    m_accept_start_time( -1 ),
    m_accept_stop_time( -1 ),
    
    m_e2e_total_delay( 0.0 ),
    m_e2e_max_delay( 0.0 ),
    
    m_accept_mark_packet( 0 ),
    m_total_mark_delay( 0.0 ),

    m_throughput_measure_start_packet( 0 ),
    m_throughput_measure_stop_packet( 0 ),
    m_throughput_measure_start_time( 0.0 ),
    m_throughput_measure_stop_time( 0.0 )
{
}

EsynetFoundationStatistic::EsynetFoundationStatistic(const EsynetFoundationStatistic &t) :
    m_injected_packet( t.injectedPacket() ),
    m_inject_start_time( t.injectStartTime() ),
    m_inject_stop_time( t.injectStopTime() ),
    
    m_accepted_packet( t.acceptedPacket() ),
    m_accept_start_time( t.acceptStartTime() ),
    m_accept_stop_time( t.acceptStopTime() ),
    
    m_e2e_total_delay( t.totalE2EDelay() ),
    m_e2e_max_delay( t.maxE2EDelay() ),

    m_accept_mark_packet( t.acceptMarkPacket() ),
    m_total_mark_delay( t.totalMarkDelay() ),
    
    m_throughput_measure_start_packet( t.throughputMeasureStartPacket() ),
    m_throughput_measure_stop_packet( t.throughputMeasureStopPacket() ),
    m_throughput_measure_start_time( t.throughputMeasureStartTime() ),
    m_throughput_measure_stop_time( t.throughputMeasureStopTime() )
{
}

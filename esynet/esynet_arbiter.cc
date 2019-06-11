/*
 * File name : esynet_arbiter.cc
 * Function : Implement arbiter module.
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
 * @ingroup ESYNET_ARBITER
 * @file esynet_arbiter.cc
 * @brief Implement arbiter.
 */

#include "esynet_arbiter.h"
#include "esynet_random_unit.h"

EsynetArbiter::EsynetArbiter( esynet::EsynetArbiterType type, int size )
    : m_type( type )
    , m_size( size )
    , m_request( size )
{
    if ( type == esynet::AR_MATRIX )
    {
        m_state_matrix.resize( m_size );
        for ( int i = 0; i < m_size; i ++ )
        {
            m_state_matrix[ i ].resize( m_size );
            for ( int j = 0; j < m_size; j ++ )
            {
                m_state_matrix[ i ][ j ] = (i < j) ? 1 : 0;
            }
        }
    }
    else if ( type == esynet::AR_RR )
    {
        m_state_vector.resize( m_size, false );
        if ( m_size > 0 )
        {
            m_state_vector[ 0 ] = true;
        }
    }
}

EsynetArbiter::EsynetArbiter(const EsynetArbiter& t) 
    : m_type( t.m_type )
    , m_size( t.m_size )
    , m_state_matrix( t.m_state_matrix )
    , m_state_vector( t.m_state_vector )
    , m_vc_map( t.m_vc_map )
    , m_request( t.m_request )
{
}

bool EsynetArbiter::reqestIsValid()
{
    for ( int i = 0; i < m_size; i ++ )
    {
        if ( m_request[ i ] )
        {
            return true;
        }
    }
    return false;
}

int EsynetArbiter::grant()
{
    if ( !reqestIsValid() )
    {
        return -1;
    }
    int win_t = -1;
    if ( m_type == esynet::AR_MATRIX )
    {
        for ( int r = 0; r < m_size; r ++ )
        {
            bool dis = false;
            for ( int i = 0; i < m_size; i ++ )
            {
                if ( i == r )
                {
                    continue;
                }
                dis = dis || ( m_state_matrix[ i ][ r ] && m_request[ i ] );
            }
            bool g = m_request[ r ] && (!dis);
            if ( g )
            {
                for ( int j = 0; j < m_size; j ++ )
                {
                    if ( j == r )
                    {
                        continue;
                    }
                    m_state_matrix[ r ][ j ] = false;
                    m_state_matrix[ j ][ r ] = true;
                }
                win_t = r;
                break;
            }
        }
    }
    else if ( m_type == esynet::AR_RR )
    {
        int pri = 0;
        for ( ; pri < m_size; pri ++ )
        {
            if ( m_state_vector[ pri ] )
            {
                break;
            }
        }

        bool carry;
        for ( int roffset = 0; roffset < m_size; roffset ++ )
        {
            int r = ( roffset + pri ) % m_size;
            bool g = m_request[ r ] && ( carry || m_state_vector[ r ] );
            if ( g )
            {
                for ( int i = 0; i < m_size; i ++ )
                {
                    m_state_vector[ i ] = false;
                }
                m_state_vector[ (r + 1) % m_size ] = true;
                win_t = r;
                break;
            }
            else
            {
                carry = ( carry || m_state_vector[ r ] ) && ( !m_request[ r ] );
            }
        }
    }
    else 
    {
        vector< int > req_vector;
        for ( int r = 0; r < m_size; r ++ )
        {
            if ( m_request[ r ] )
            {
                req_vector.push_back( r );
            }
        }
        int win = EsynetSRGenFlatLong( 0, req_vector.size() );
        win_t = req_vector[ win ];
    }
    clearRequest();
    return win_t;
}

esynet::EsynetVC EsynetArbiter::grantVc()
{
    int win = grant();
    if ( win >= 0 )
    {
        return m_vc_map[ win ];
    }
    else
    {
        return esynet::EsynetVC ( -1, -1 );
    }
}


void EsynetArbiter::setHighestPriority( int port )
{
    if ( m_type == esynet::AR_MATRIX )
    {
        for ( int i = 0; i < m_size; i ++ )
        {
            if ( i == port )
            {
                continue;
            }
            m_state_matrix[ i ][ port ] = false;
            m_state_matrix[ port ][ i ] = true;
        }
    }
    else if ( m_type == esynet::AR_RR )
    {
        for ( int i = 0; i < m_size; i ++ )
        {
            m_state_vector[ i ] = false;
        }
        m_state_vector[ port ] = true;
    }
}

void EsynetArbiter::setHighestPriority( const esynet::EsynetVC & port )
{
    for ( size_t i = 0; i < m_size; i ++ )
    {
        if ( m_vc_map[ i ] == port )
        {
            setHighestPriority( i );
        }
    }
}

void EsynetArbiter::setLowestPriority( int port )
{
    if ( m_type == esynet::AR_MATRIX )
    {
        for ( int i = 0; i < m_size; i ++ )
        {
            if ( i == port )
            {
                continue;
            }
            m_state_matrix[ i ][ port ] = true;
            m_state_matrix[ port ][ i ] = false;
        }
    }
    else if ( m_type == esynet::AR_RR )
    {
        for ( int i = 0; i < m_size; i ++ )
        {
            m_state_vector[ i ] = false;
        }
        m_state_vector[ (port + 1) % m_size ] = true;
    }
}

void EsynetArbiter::setLowestPriority( const esynet::EsynetVC & port )
{
    for ( size_t i = 0; i < m_size; i ++ )
    {
        if ( m_vc_map[ i ] == port )
        {
            setLowestPriority( i );
        }
    }
}

void EsynetArbiter::setInputPortMap( const EsyNetworkCfgRouter & router_cfg )
{
    int seq = 0;
    m_vc_map.clear();
    for ( int phy = 0; phy < router_cfg.portNum(); phy ++ )
    {
        for ( int vc = 0; vc < router_cfg.port( phy ).inputVcNumber(); vc ++ )
        {
            m_vc_map[ seq ] = esynet::EsynetVC ( phy, vc );
            seq ++;
        }
    }
}

void EsynetArbiter::setOutputPortMap( const EsyNetworkCfgRouter & router_cfg )
{
    int seq = 0;
    m_vc_map.clear();
    for ( int phy = 0; phy < router_cfg.portNum(); phy ++ )
    {
        for ( int vc = 0; vc < router_cfg.port( phy ).outputVcNumber(); vc ++ )
        {
            m_vc_map[ seq ] = esynet::EsynetVC ( phy, vc );
            seq ++;
        }
    }
}

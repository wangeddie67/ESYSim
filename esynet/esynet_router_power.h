/*
 * File name : esynet_router_power.h
 * Function : Interface to Orion power model.
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
 * @ingroup ESYNET_POWER
 * @file esynet_router_power.h
 * @brief Define interface to power model.
 */

#ifndef ESYNET_ROUTER_POWER_H
#define ESYNET_ROUTER_POWER_H

#include "esynet_global.h"
extern "C" {
#include "SIM_power.h"
#include "SIM_router_power.h"
#include "SIM_power_router.h"
}

#include <cstdio>

/**
 * @ingroup ESYNET_POWER
 * @brief Calculate power consumption in a router. Interface to Orion 2.0 power module.
 */
class EsynetRouterPower
{
private:
    double m_link_length;   /*!< @brief Link length. */
    long m_flit_size;       /*!< @brief Size of a flit, in 64 bits. */

    /**
     * @name Orion data structure.
     * @{
     */
    SIM_power_router_info_t m_router_info;  /*!< @brief Structure for router information. */
    SIM_power_router_t m_router_power;      /*!< @brief Power structure for routers. */
    SIM_power_arbiter_t m_arbiter_vc_power; /*!< @brief Power structure for arbiters. */
    SIM_power_bus_t m_link_power;           /*!< @brief Power structure for link between routers. */
    /**
     * @}
     */

    /**
     * @name Last data to calculate the bit-flipping for energy consumption.
     * @{
     */
    std::vector< esynet::EsynetPayload > m_buffer_write;    /*!< @brief Last write data on buffer for each port. */
    std::vector< esynet::EsynetPayload > m_buffer_read;     /*!< @brief Last read data on buffer for each port. */
    std::vector< esynet::EsynetPayload > m_crossbar_write;  /*!< @brief Last write data on crossbar for each port. */
    std::vector< esynet::EsynetPayload > m_crossbar_read;   /*!< @brief Last read data on crossbar for each port. */
    std::vector< esynet::EsynetPayload > m_link_traversal;  /*!< @brief Last transmit data on link for each port. */
    std::vector< long > m_crossbar_input;   /*!< @brief Last input port of traveral. */
    std::vector< std::vector< esynet::EsynetAtomType > > m_arbiter_vc_req;  /*!< @brief Last arbiter request for each port and vc. */
    std::vector< std::vector< unsigned long > > m_arbiter_vc_grant;         /*!< @brief Last arbiter grant for each port and vc. */
    /**
     * @}
     */

public:
    /**
     * @name Construction function.
     * @{
     */
    /**
     * @brief Construct empty structure.
     */
    EsynetRouterPower()
        : m_link_length()
        , m_flit_size()
        , m_router_info()
        , m_router_power()
        , m_arbiter_vc_power()
        , m_link_power()
        , m_buffer_write()
        , m_buffer_read()
        , m_crossbar_write()
        , m_crossbar_read()
        , m_link_traversal()
        , m_crossbar_input()
        , m_arbiter_vc_req()
        , m_arbiter_vc_grant()
    {}
    /**
     * @brief Construct structure.
     * @param phy Number of physical channels.
     * @param vc Number of virtual channels for each physical channel.
     * @param flit_size Size of flit, in 64 bit.
     * @param length Link length.
     */
    EsynetRouterPower( long phy, long vc, long flit_size, double length )
        : m_link_length( length )
        , m_flit_size( flit_size )
        , m_router_info()
        , m_router_power()
        , m_arbiter_vc_power()
        , m_link_power()
        , m_buffer_write()
        , m_buffer_read()
        , m_crossbar_write()
        , m_crossbar_read()
        , m_link_traversal()
        , m_crossbar_input()
        , m_arbiter_vc_req()
        , m_arbiter_vc_grant()
    {
        // Initlizate router information structure and router power structure.
        FUNC(SIM_router_power_init, &m_router_info, &m_router_power);

        // initlizate last variables to [phy][m_flit_size] = 0 */
        m_buffer_write.resize( phy );
        m_buffer_read.resize( phy );
        m_crossbar_read.resize( phy );
        m_crossbar_write.resize( phy );
        m_link_traversal.resize( phy );
        m_crossbar_input.resize( phy, 0 );
        for ( long l_port = 0; l_port < phy; l_port ++ )
        {
            m_buffer_write[ l_port ].resize( m_flit_size, 0 );
            m_buffer_read[ l_port ].resize( m_flit_size, 0 );
            m_crossbar_read[ l_port ].resize( m_flit_size, 0 );
            m_crossbar_write[ l_port ].resize( m_flit_size, 0 );
            m_link_traversal[ l_port ].resize( m_flit_size, 0 );
        }

        // Initlizate arbiter power structure 
        SIM_arbiter_init( &m_arbiter_vc_power, 1, 1, phy * vc, 0, NULL );

        // Initlizate last variables to [phy][vc] = 1
        m_arbiter_vc_req.resize( phy );
        m_arbiter_vc_grant.resize( phy );
        for ( long l_port = 0; l_port < phy; l_port ++ )
        {
            m_arbiter_vc_req[ l_port ].resize( vc, 1 );
            m_arbiter_vc_grant[ l_port ].resize( vc, 1 );
        }

        // Initlizate link power structure.
        SIM_bus_init( &m_link_power, GENERIC_BUS, IDENT_ENC, 1, 0, 1, 1, m_link_length, 0 );
    }
    /**
     * @}
     */

    /**
     * @name Functions to register energy consumption once.
     * @{
     */
    /**
     * @brief Register buffer read power once.
     * @param in_port Buffer read port.
     * @param read_d Constant reference to read data.
     */
    inline void powerBufferRead( long in_port, const esynet::EsynetPayload& read_d )
    {
        // Loop all long word in read data.
        for( long l_flit = 0; l_flit < m_flit_size; l_flit ++ )
        {
            // Regist power.
            FUNC( SIM_buf_power_data_read, &(m_router_info.in_buf_info), &(m_router_power.in_buf), read_d[ l_flit ] );
            // Record last data to read.
            m_buffer_read[ in_port ][ l_flit ] = read_d[ l_flit ];
        }
    }
    /**
     * @brief Register buffer write power once.
     * @param in_port Buffer write port.
     * @param write_d Constant reference to write data.
     */
    inline void powerBufferWrite( long in_port, const esynet::EsynetPayload& write_d )
    {
        // Loop all long word in write data.
        for ( long l_flit = 0; l_flit < m_flit_size; l_flit ++ )
        {
            // Regist power.
            FUNC( SIM_buf_power_data_write, &(m_router_info.in_buf_info), &(m_router_power.in_buf),
                 (char*)(&m_buffer_write[ in_port ][ l_flit ]), (char*)(&m_buffer_write[ in_port ][ l_flit ]), (char*)(&write_d[ l_flit ]) );
            // Record last data to write.
            m_buffer_write[ in_port ][ l_flit ] = write_d[ l_flit ];
        }
    }
    /**
     * @brief Register crossbar traversal power once.
     * @param in_port Input port.
     * @param out_port Output port.
     * @param trav_d Constant reference to traversal data.
     */
    inline void powerCrossbarTraversal( long in_port, long out_port, const esynet::EsynetPayload & trav_d )
    {
        // Loop all long word in traversal data.
        for ( long l_flit = 0; l_flit < m_flit_size; l_flit ++ )
        {
            // Regist power.
            SIM_crossbar_record( &(m_router_power.crossbar), 1, trav_d[ l_flit ], m_crossbar_read[ in_port ][ l_flit ], 1, 1 );
            SIM_crossbar_record( &(m_router_power.crossbar), 0, trav_d[ l_flit ], m_crossbar_write[ out_port ][ l_flit ], 
                                 m_crossbar_input[ out_port ], in_port );
            // Record last data to traversal
            m_crossbar_read[ in_port ][ l_flit ] = trav_d[ l_flit ];
            m_crossbar_write[ out_port ][ l_flit ] = trav_d[ l_flit ];
            m_crossbar_input[ out_port ] = in_port;
        }
    }
    /**
     * @brief Register arbiter power once.
     * @param pc Physical port.
     * @param vc Virtual channel.
     * @param req Request word.
     * @param gra Grant index.
     */
    inline void powerVCArbiter( long pc, long vc, esynet::EsynetAtomType req, unsigned long gra )
    {
        // Regist power.
        SIM_arbiter_record( &m_arbiter_vc_power, req, m_arbiter_vc_req[ pc ][ vc ], gra, m_arbiter_vc_grant[ pc ][ vc ] );
        // Record last data to traversal.
        m_arbiter_vc_req[ pc ][ vc ] = req;
        m_arbiter_vc_grant[ pc ][ vc ] = gra;
    }
    /**
     * @brief Register link traversal power once.
     * @param in_port Input port.
     * @param read_d Constant reference to traversal data.
     */
    inline void powerLinkTraversal( long in_port, const esynet::EsynetPayload & read_d )
    {
        // Loop all long word in traversal data.
        for(long l_flit = 0; l_flit < m_flit_size; l_flit++)
        {
            // Regist power.
            SIM_bus_record( &m_link_power, m_link_traversal[ in_port ][ l_flit ], read_d[ l_flit ] );
            // Record last data to traversal.
            m_link_traversal[ in_port ][ l_flit ] = read_d[ l_flit ];
        }
    }
    /**
     * @}
     */

    /**
     * @name Function to calculate power.
     * @{
     */
    /**
     * @brief Report arbiter power.
     * @param sim_cycle Simulation cycle.
     * @param telem Not clear.
     * @return Arbiter power.
     */
    inline double powerArbiterReport( unsigned long long sim_cycle, int telem )
    {
        return SIM_arbiter_report( sim_cycle, & m_arbiter_vc_power, telem );
    }
    /**
     * @brief Report buffer power.
     * @param sim_cycle Simulation cycle.
     * @param telem Not clear.
     * @return Buffer power.
     */
    inline double powerBufferReport( unsigned long long sim_cycle, int telem )
    {
        return SIM_array_power_report( sim_cycle, &(m_router_info.in_buf_info), &(m_router_power.in_buf), telem );
    }
    /**
     * @brief Report crossbar power.
     * @param sim_cycle Simulation cycle.
     * @param telem Not clear.
     * @return Crossbar power.
     */
    inline double powerCrossbarReport( unsigned long long sim_cycle, int telem )
    {
        return SIM_crossbar_report( sim_cycle, &(m_router_power.crossbar), telem );
    }
    /**
     * @brief Report link power.
     * @param sim_cycle Simulation cycle.
     * @param telem Not clear.
     * @return Link power.
     */
    inline double powerLinkReport( unsigned long long sim_cycle, int telem )
    {
        return SIM_bus_report( sim_cycle, &m_link_power, telem );
    }
    /**
     * @brief Report total router power.
     * @param sim_cycle Simulation cycle.
     * @param telem Not clear.
     * @return Router power.
     */
    inline double powerReport( unsigned long long sim_cycle, int telem )
    {
        return powerArbiterReport( sim_cycle, telem ) + powerBufferReport( sim_cycle, telem )
            + powerCrossbarReport( sim_cycle, telem ) + powerLinkReport( sim_cycle, telem );
    }
    /**
     * @}
     */
};

#endif

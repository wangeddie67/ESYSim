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
 * @brief Define interface to power model
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

/*************************************************
  Class Name :
    PowerTemplate

  Description :
    Class to calculate power consumption in a router.

  Properties
    m_flit_size        : long
    m_router_info      : SIM_power_router_info_t
    m_router_power     : SIM_power_router_t
    m_arbiter_vc_power : SIM_power_arbiter_t
    m_link_power       : SIM_power_bus_t
    m_buffer_write     : vector< DataType >
    m_buffer_read      : vector< DataType >
    m_crossbar_read    : vector< DataType >
    m_crossbar_write   : vector< DataType >
    m_link_traversal   : vector< DataType >
    m_crossbar_input   : vector< long >
    m_arbiter_vc_req   : vector< vector< AtomType > >
    m_arbiter_vc_grant : vector< vector< unsigned long > >

  Public functions :
           PowerTemplate()
           PowerTemplate( long a, long b, long c )

      void powerBufferRead( long in_port, const DataType & read_d )
      void powerBufferWrite( long in_port, const DataType & write_d )
      void powerCrossbarTrav( long in_port, long out_port, const DataType & trav_d )
      void powerVCArbit( long pc, long vc, AtomType req, unsigned long gra )
      void powerLinkTraversal( long in_port, const DataType & read_d )
    double powerBufferReport( int telem )
    double powerLinkReport( int telem )
    double powerCrossbarReport( int telem )
    double powerArbiterReport( int telem )
    double powerReport( int id, FILE *fd, int telem )
*************************************************/
class EsynetRouterPower
{
/* Properties */
private:
    double m_link_length;
    /* size of a flit, (x 64 bits) */
    long m_flit_size;
    /* router information structure */
    SIM_power_router_info_t m_router_info;
    /* router power structure */
    SIM_power_router_t m_router_power;
    /* arbiter power structure */
    SIM_power_arbiter_t m_arbiter_vc_power;
    /* bus power structure */
    SIM_power_bus_t m_link_power;

    /* last data for each port */
    /* buffer write */
    std::vector< esynet::EsynetPayload > m_buffer_write;
    /* buffer read */
    std::vector< esynet::EsynetPayload > m_buffer_read;
    /* crossbar read */
    std::vector< esynet::EsynetPayload > m_crossbar_read;
    /* crossbar write */
    std::vector< esynet::EsynetPayload > m_crossbar_write;
    /* link traversal */
    std::vector< esynet::EsynetPayload > m_link_traversal;

    /* crossbar input */
    std::vector< long > m_crossbar_input;
    /* arbiter request */
    std::vector< std::vector< esynet::EsynetAtomType > > m_arbiter_vc_req;
    /* arbiter grant */
    std::vector< std::vector< unsigned long > > m_arbiter_vc_grant;

public:
    /* constructor */
    EsynetRouterPower();
    EsynetRouterPower(long a, long b, long c, double l);
    /* register power once */
    /* buffer read power */
    void powerBufferRead( long in_port, const esynet::EsynetPayload & read_d );
    /* buffer write power */
    void powerBufferWrite( long in_port, const esynet::EsynetPayload & write_d );
    /* crossbar traversal power */
    void powerCrossbarTraversal( long in_port, long out_port, const esynet::EsynetPayload & trav_d );
    /* vc arbiter power */
    void powerVCArbiter( long pc, long vc, esynet::EsynetAtomType req, unsigned long gra );
    /* link traversal power */
    void powerLinkTraversal( long in_port, const esynet::EsynetPayload & read_d );
    /* report power */
    /* report buffer power */
    double powerBufferReport( unsigned long long sim_cycle, int telem );
    /* report link power */
    double powerLinkReport( unsigned long long sim_cycle, int telem );
    /* report crossbar power */
    double powerCrossbarReport( unsigned long long sim_cycle, int telem );
    /* report arbiter power */
    double powerArbiterReport( unsigned long long sim_cycle, int telem );
    /* report power to file */
    double powerReport( unsigned long long sim_cycle, int id, FILE *fd, int telem );
};

#endif

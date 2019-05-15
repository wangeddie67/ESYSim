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


#include "esynet_router_power.h"
#include <iostream>

/*************************************************
  Function :
    PowerTemplate::PowerTemplate()
  Description :
    Construct an empty entity for power template.
*************************************************/
EsynetRouterPower::EsynetRouterPower() :
    m_link_length(),
    m_flit_size(),
    m_router_info(),
    m_router_power(),
    m_arbiter_vc_power(),
    m_link_power(),
    m_buffer_write(),
    m_buffer_read(),
    m_crossbar_read(),
    m_crossbar_write(),
    m_link_traversal(),
    m_crossbar_input(),
    m_arbiter_vc_req(),
    m_arbiter_vc_grant()
{
}

/*************************************************
  Function :
    PowerTemplate::PowerTemplate(long phy, long vc, long flit_size)
  Description :
    Construct an entity for input template with phy physical ports
    and vc virtual channels.
  Inputs:
    long phy         the number of physical ports
    long vc          the number of virtual channels for each port
    long flit_size   size of flit, x 64 bits
*************************************************/
EsynetRouterPower::EsynetRouterPower(long phy, long vc, long flit_size, double length) :
    m_link_length( length ),
    m_flit_size( flit_size ),
    m_router_info(),
    m_router_power(),
    m_arbiter_vc_power(),
    m_link_power(),
    m_buffer_write(),
    m_buffer_read(),
    m_crossbar_read(),
    m_crossbar_write(),
    m_link_traversal(),
    m_crossbar_input(),
    m_arbiter_vc_req(),
    m_arbiter_vc_grant()

{
    /* initlizate router information structure and router power structure */
    FUNC(SIM_router_power_init, &m_router_info, &m_router_power);
    /* initlizate variables to size [phy][m_flit_size] = 0 */
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
    } /* for( long l_port = 0; l_port < phy; l_port ++ ) */
    
    /* initlizate arbiter power structure */
    SIM_arbiter_init( &m_arbiter_vc_power, 1, 1, phy * vc, 0, NULL );
    /* initlizate variables to [phy][vc] = 1 */
    m_arbiter_vc_req.resize( phy );
    m_arbiter_vc_grant.resize( phy );
    for ( long l_port = 0; l_port < phy; l_port ++ )
    {
        m_arbiter_vc_req[ l_port ].resize( vc, 1 );
        m_arbiter_vc_grant[ l_port ].resize( vc, 1 );
    } /* for( long l_port = 0; l_port < phy; l_port ++ ) */

    /* initlizate link power structure */
    SIM_bus_init( &m_link_power, GENERIC_BUS, IDENT_ENC, 1,
            0, 1, 1, m_link_length, 0 );
}

/*************************************************
  Function :
    void PowerTemplate::powerBufferRead(long in_port, const DataType & read_d)
  Description :
    Register buffer read power once
  Called By :
    void SimRouter::flitOutbuffer()
  Inputs:
                long in_port  buffer read port
    const DataType & read_d   data to read
*************************************************/
void EsynetRouterPower::powerBufferRead(long in_port, const EsynetPayload & read_d)
{
    /* loop all long word */
    for( long l_flit = 0; l_flit < m_flit_size; l_flit ++ )
    {
        /* regist power */
        FUNC(SIM_buf_power_data_read, &(m_router_info.in_buf_info), &(m_router_power.in_buf), read_d[l_flit]);
        /* record last data to read */
        m_buffer_read[ in_port ][ l_flit ] = read_d[ l_flit ];
    } /* for( long l_flit = 0; l_flit < m_flit_size; l_flit ++ ) */
}

/*************************************************
  Function :
    void PowerTemplate::powerLinkTraversal(long in_port, const DataType & read_d)
  Description :
    Register buffer link traversal power once
  Called By :
    void SimRouter::flitTraversal(long i)
  Inputs:
                long in_port  buffer read port
    const DataType & read_d   data to read
*************************************************/
void EsynetRouterPower::powerLinkTraversal(long in_port, const EsynetPayload & read_d)
{
    /* loop all long word */
    for(long l_flit = 0; l_flit < m_flit_size; l_flit++)
    {
        /* regist power */
        SIM_bus_record( &m_link_power, m_link_traversal[ in_port ][ l_flit ], read_d[ l_flit ] );
        /* record last data to traversal */
        m_link_traversal[ in_port ][ l_flit ] = read_d[ l_flit ];
    } /* for( long l_flit = 0; l_flit < m_flit_size; l_flit ++ ) */
}

/*************************************************
  Function :
    void PowerTemplate::powerBufferWrite(long in_port, const DataType & write_d)
  Description :
    Register buffer write power once
  Called By :
    void SimRouter::injectPacket(long flit_id, long sor_add, long des_add,
        double start_time, long size, long vs, long ack)
    void SimRouter::receiveFlit(long phy, long vc, const Flit & flit)
  Inputs :
                long in_port  buffer write port
    const DataType & write_d  data to write
*************************************************/
void EsynetRouterPower::powerBufferWrite(long in_port, const EsynetPayload & write_d)
{
    /* loop all long word */
    for ( long l_flit = 0; l_flit < m_flit_size; l_flit ++ )
    {
        /* regist power */
        FUNC(SIM_buf_power_data_write, &(m_router_info.in_buf_info),
            &(m_router_power.in_buf), (char *)(&m_buffer_write[in_port][l_flit]),
            (char *)(&m_buffer_write[in_port][l_flit]),
            (char *)(&write_d[l_flit]));
        /* record last data to write */
        m_buffer_write[ in_port ][ l_flit ] = write_d[ l_flit ];
    } /* for( long l_flit = 0; l_flit < m_flit_size; l_flit ++ ) */
}

/*************************************************
  Function :
    void PowerTemplate::powerCrossbarTraversal(long in_port, long out_port,
        const DataType & trav_d)
  Description :
    Register crossbar traversal power once
  Called By :
    void SimRouter::flitOutbuffer()
  Inputs:
                long in_port   input port
                long out_port  output port
    const DataType & trav_d    data to traversal
*************************************************/
void EsynetRouterPower::powerCrossbarTraversal(long in_port, long out_port, const EsynetPayload & trav_d)
{
    /* loop all long word */
    for ( long l_flit = 0; l_flit < m_flit_size; l_flit ++ )
    {
        /* regist power */
        SIM_crossbar_record( &(m_router_power.crossbar), 1, trav_d[ l_flit ], m_crossbar_read[ in_port ][ l_flit ], 1, 1 );
        SIM_crossbar_record( &(m_router_power.crossbar), 0, trav_d[ l_flit ],
                m_crossbar_write[ out_port ][ l_flit ], m_crossbar_input[ out_port ],
                in_port );
        /* record last data to traversal */
        m_crossbar_read[ in_port ][ l_flit ] = trav_d[ l_flit ];
        m_crossbar_write[ out_port ][ l_flit ] = trav_d[ l_flit ];
        m_crossbar_input[ out_port ] = in_port;
    } /* for( long l_flit = 0; l_flit < m_flit_size; l_flit ++ ) */
}

/*************************************************
  Function :
    void PowerTemplate::powerVCArbiter(long pc, long vc, AtomType req,
        unsigned long gra)
  Description :
    Register arbiter power once
  Called By:
    void SimRouter::vcArbitration()
  Inputs:
             long pc   physical port
             long vc   virtual channel
         AtomType req  request word
    unsigned long gra  grant
*************************************************/
void EsynetRouterPower::powerVCArbiter(long pc, long vc, EsynetAtomType req, unsigned long gra )
{
    /* regist power */
    SIM_arbiter_record( &m_arbiter_vc_power, req, m_arbiter_vc_req[ pc ][ vc ], gra, m_arbiter_vc_grant[ pc ][ vc ]);
    /* record last data to arbiter */
    m_arbiter_vc_req[ pc ][ vc ] = req;
    m_arbiter_vc_grant[ pc ][ vc ] = gra;
}

/*************************************************
  Function :
    double PowerTemplate::powerArbiterReport(int telem)
  Description :
    Report arbiter power
  Called By :
    double PowerTemplate::powerReport( int id, FILE *fd, int telem )
  Inputs:
    int telem
  Return:
    double  arbiter power
*************************************************/
double EsynetRouterPower::powerArbiterReport( unsigned long long sim_cycle, int telem)
{
    return SIM_arbiter_report( sim_cycle, & m_arbiter_vc_power, telem );
}

/*************************************************
  Function :
    double PowerTemplate::powerBufferReport(int telem)
  Description :
    Report buffer power
  Called By :
    double PowerTemplate::powerReport( int id, FILE *fd, int telem )
  Inputs:
    int telem
  Return:
    double  buffer power
*************************************************/
double EsynetRouterPower::powerBufferReport( unsigned long long sim_cycle, int telem)
{
    return SIM_array_power_report( sim_cycle, &(m_router_info.in_buf_info), &(m_router_power.in_buf), telem );
}

/*************************************************
  Function :
    double PowerTemplate::powerCrossbarReport(int telem)
  Description :
    Report crossbar power
  Called By :
    double PowerTemplate::powerReport( int id, FILE *fd, int telem )
  Inputs:
    int telem
  Return:
    double  crossbar power
*************************************************/
double EsynetRouterPower::powerCrossbarReport( unsigned long long sim_cycle, int telem)
{
    return SIM_crossbar_report( sim_cycle, &(m_router_power.crossbar), telem );
}

/*************************************************
  Function :
    double PowerTemplate::powerLinkReport(int telem)
  Description :
    Report link power
  Called By :
    double PowerTemplate::powerReport( int id, FILE *fd, int telem )
  Inputs:
    int telem
  Return:
    double link power
*************************************************/
double EsynetRouterPower::powerLinkReport( unsigned long long sim_cycle, int telem)
{
    return SIM_bus_report( sim_cycle, &m_link_power, telem );
}

/*************************************************
  Function :
    double PowerTemplate::powerReport(int id, FILE *fd, int telem)
  Description :
    Write power report to file fd
  Inputs :
    int id     router id
    File *fd   File pointer
    int telem
  Return :
    double  total power
*************************************************/
double EsynetRouterPower::powerReport( unsigned long long sim_cycle, int id, FILE *fd, int telem)
{
    double total_power = 0;
    double indv_power = 0;
    indv_power = powerArbiterReport(sim_cycle, telem);
    fprintf(fd,"  %d arbiter power: %g\n", id, indv_power);
    total_power += indv_power;
    indv_power = powerBufferReport(sim_cycle, telem);
    fprintf(fd,"  %d buffer power: %g\n", id, indv_power);
    total_power += indv_power;
    indv_power = powerCrossbarReport(sim_cycle, telem);
    fprintf(fd,"  %d crossbar power: %g\n", id, indv_power);
    total_power += indv_power;
    indv_power = powerLinkReport(sim_cycle, telem);
    fprintf(fd,"  %d link power: %g\n", id, indv_power);
    total_power += indv_power;
    fprintf(fd,"%d total router power: %g\n", id, total_power);
    return total_power;
}

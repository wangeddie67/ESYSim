/*
 * File name : esynet_router_unit.h
 * Function : Declaire router structure.
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

#ifndef ESYNET_ROUTER_UNIT_H
#define ESYNET_ROUTER_UNIT_H

#include "esy_networkcfg.h"
#include "esynet_config.h"
#include "esynet_port_unit.h"
#include "esynet_router_power.h"
#include "esynet_sim_base.h"
#include "esynet_statistics.h"
#include "esynet_arbiter.h"

/**
 * @brief Router module.
 */
class EsynetRouter : public EsynetSimBaseUnit
{
private:
    EsyNetworkCfg * m_network_cfg;      /*!< @brief Pointer to network configuration structure. */
    EsyNetworkCfgRouter * m_router_cfg; /*!< @brief Pointer to router configuration structure. */
    EsynetConfig * m_argu_cfg;          /*!< @brief Pointer to argument list. */

    long m_router_id;   /*!< @brief Router id. */
    esynet::EsynetAddr m_router_addr;   /*!< @brief Router address in axis. */

    std::vector< EsynetInputPort > m_input_port;    /*!< @brief Input ports. */
    std::vector< EsynetOutputPort > m_output_port;  /*!< @brief Output ports. */
    EsynetRouterPower m_power_unit;             /*!< @brief Power module. */
    EsynetRouterStatistic m_statistics_unit;    /*!< @brief Statistic module. */

    std::vector< std::vector< EsynetArbiter > > m_vc_input_arbiter;     /*!< @brief Virtual channel arbiter at output virtual channels. */
    std::vector< std::vector< EsynetArbiter > > m_vc_output_arbiter;    /*!< @brief Virtual channel arbiter at input virtual channels. */
    std::vector< EsynetArbiter > m_port_input_arbiter;  /*!< @brief Port arbiter at output ports. */
    std::vector< EsynetArbiter > m_port_output_arbiter; /*!< @brief Port arbiter at input ports. */

    esynet::EsynetFlowControlType m_flow_control;   /*!< @brief Flow control method. */
    void (EsynetRouter::*m_curr_algorithm)(long, long, long, long); /*!< @brief Routing algorithm function. */

    std::vector< std::vector< std::pair< long, esynet::EsynetVC > > > m_routing_table; /*!< @brief Routing table. First item is source address, second item is routing decision. */

public:
    /**
     * @brief Constructor Function.
     */
    EsynetRouter();
    /**
     * @brief Constructor a router.
     * @param network_cfg Pointer to network configurations structure.
     * @param router_id Router id.
     * @param argument_cfg Pointer to argument list.
     */
    EsynetRouter( EsyNetworkCfg * network_cfg, long router_id, EsynetConfig * argument_cfg );

    /**
     * @brief Return reference to specified output port.
     */
    inline EsynetOutputPort & outputPort( long phy ) { return m_output_port[ phy ]; }
    /**
     * @brief Return constant reference to specified output port.
     */
    inline const EsynetOutputPort & outputPort( long phy ) const { return m_output_port[ phy ]; }
    /**
     * @brief Return constant reference to statistics unit.
     */
    inline const EsynetRouterStatistic & statistics() const { return m_statistics_unit; }
    /**
     * @brief Return reference to power unit.
     */
    inline EsynetRouterPower & powerUnit() { return m_power_unit; }

    /**
     * @brief Set neighbor of input port.
     * @param port input port.
     * @param addr pair (router id, port id) of connected port.
     */
    inline void setInputNeighbor( long port, const esynet::EsynetVC & addr )  { m_input_port[ port ].setInputNeighbor( addr ); }
    /**
     * @brief Set neighbor of output port.
     * @param port output port.
     * @param addr pair (router id, port id) of connected port.
     */
    inline void setOutputNeighbor( long port, const esynet::EsynetVC & addr ) { m_output_port[ port ].setOutputNeighbor( addr ); }
    /**
     * @brief Add one item in routing table.
     * @param dst   Destination router.
     * @param src   Source router.
     * @param routing_vc    Routing decision < port, vc >
     */
    inline void appendRoutingTable( long dst, long src, const esynet::EsynetVC& routing_vc )
    {
        m_routing_table[ dst ].push_back( std::pair< long, esynet::EsynetVC >( src, routing_vc ) );
    }
    /**
     * @brief Check the valid of routing table. Return False if there is missing item
     */
    bool routingTableCheck();
    /**
     * @name Event handler function.
     * @{
     */
    /**
     * @brief Receive flit.
     * @param phy physical port.
     * @param vc virtual channel.
     * @param flit constant reference to flit.
     */
    void receiveFlit( long phy, long vc, const EsynetFlit & flit );
    /**
     * @brief Receive credit of physical port phy and virtual channel vc.
     * @param phy physical port.
     * @param vc Virtual channel.
     * @param credit Credit value.
     */
    void receiveCredit( long phy, long vc, long credit );
    /**
     * @}
     */

    /**
     * @brief Simulate pipeline.
     */
    void routerSimPipeline();

private:
    /**
     * @brief Return credit of connected port to specified output physical port and virtual channel.
     */
    inline long creditCounter( long phy, long vc ) const { return m_output_port[ phy ][ vc ].creditCounter(); }
    /**
     * @brief Return credit of connected port of specified output virtual channel.
     */
    inline long creditCounter( const esynet::EsynetVC port ) const { return m_output_port[ port.first ][ port.second ].creditCounter(); }
    /**
     * @brief Return reference to specified input physical port and virtual channel.
     */
    inline EsynetInputVirtualChannel & inputVirtualChannel( long phy, long vc ) { return m_input_port[ phy ][ vc ]; }
    /**
     * @brief Return constant reference to specified input physical port and virtual channel.
     */
    inline const EsynetInputVirtualChannel & inputVirtualChannel( long phy, long vc ) const { return m_input_port[ phy ][ vc ]; }


    /* operate function of vc */
    /* get vc */
    esynet::EsynetVC vcSelection( long a, long b );

    /**
     * @name Pipeline functions
     * @{
     */
    /**
     * @brief routing decision.
     */
    void routingDecision();
    /**
     * @brief virtual channel arbitration
     */
    void vcArbitration();
    /**
     * @brief switch arbitration
     */
    void swArbitration();
    /**
     * @brief flit cross switch
     */
    void flitOutbuffer();
    /**
     * @brief flit traversal
     */
    void flitTraversal();
    /**
     * @}
     */

    /**
     * @name Routing algorithm function
     * @{
     */
    /**
     * @brief Single ring routing algorithm.
     * @param des_t Destination router of packet.
     * @param sor_t Source router of packet.
     * @param s_ph input physical channel.
     * @param s_vc input virtual channel.
     */
    void algorithmSingleRing( long des_t, long sor_t,long s_ph, long s_vc );
    /**
     * @brief Double ring routing algorithm.
     * @param des_t Destination router of packet.
     * @param sor_t Source router of packet.
     * @param s_ph input physical channel.
     * @param s_vc input virtual channel.
     */
    void algorithmDoubleRing( long des_t, long sor_t,long s_ph, long s_vc );
    /**
     * @brief XY routing algorithm.
     * @param des_t Destination router of packet.
     * @param sor_t Source router of packet.
     * @param s_ph input physical channel.
     * @param s_vc input virtual channel.
     */
    void algorithmXY( long des_t, long sor_t,long s_ph, long s_vc );
    /**
     * @brief TXY routing algorithm.
     * @param des_t Destination router of packet.
     * @param sor_t Source router of packet.
     * @param s_ph input physical channel.
     * @param s_vc input virtual channel.
     */
    void algorithmTXY( long des_t, long sor_t, long s_ph, long s_vc );
    /**
     * @brief DyXY routing algorithm.
     * @param des_t Destination router of packet.
     * @param sor_t Source router of packet.
     * @param s_ph input physical channel.
     * @param s_vc input virtual channel.
     */
    void algorithmDyXY( long des_t, long sor_t, long s_ph, long s_vc );
    /**
     * @brief Table based routing algorithm.
     * @param des_t Destination router of packet.
     * @param sor_t Source router of packet.
     * @param s_ph input physical channel.
     * @param s_vc input virtual channel.
     */
    void algorithmTable( long des_t, long sor_t, long s_ph, long s_vc );
    /**
     * @}
     */

    /**
     * @brief print configuration of router to stream
     */
    friend ostream& operator<<(ostream& os, const EsynetRouter & sr);
};


#endif

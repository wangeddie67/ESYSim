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

#include <map>
#include <sstream>
#include <cmath>
#include <cstdlib>

class EsynetRouter : public EsynetSimBaseUnit
{
/* Properties */
private:
    EsyNetworkCfg * m_network_cfg;
    EsyNetworkCfgRouter * m_router_cfg;
    EsynetConfig * m_argu_cfg;

    /* router id */
    long m_router_id;
    EsynetAddr m_router_addr;

    std::vector< EsynetInputPort > m_input_port;    /*!< @brief Input ports. */
    std::vector< EsynetOutputPort > m_output_port;  /*!< @brief Output ports. */

    /* Stastics parameters */
    /* power module */
    EsynetRouterPower m_power_unit;
    /* statics module */
    EsynetRouterStatistic m_statistics_unit;

    /* arbiter */
    std::vector< std::vector< EsynetArbiter > > m_vc_input_arbiter;
    std::vector< std::vector< EsynetArbiter > > m_vc_output_arbiter;
    std::vector< EsynetArbiter > m_port_input_arbiter;
    std::vector< EsynetArbiter > m_port_output_arbiter;

    /* the routing algorithm is now used, this is a pointer, points to
        * the routing procedure */
    void (EsynetRouter::*m_curr_algorithm)(long, long, long, long);
/* Public Functions */
public:
    /* constructor */
    EsynetRouter();
    EsynetRouter( EsyNetworkCfg * network_cfg, long router_id, EsynetConfig * argument_cfg );

    inline long creditCounter( long phy, long vc ) const { return m_output_port[ phy ][ vc ].creditCounter(); }
    inline long creditCounter( const EsynetVC port ) const { return m_output_port[ port.first ][ port.second ].creditCounter(); }

    /* function to access m_input_port */
    inline EsynetInputVirtualChannel & inputVirtualChannel( long phy, long vc ) { return m_input_port[ phy ][ vc ]; }
    inline const EsynetInputVirtualChannel & inputVirtualChannel( long phy, long vc ) const { return m_input_port[ phy ][ vc ]; }
    /* function to access m_input_port */
    inline EsynetOutputPort & outputPort( long phy ) { return m_output_port[ phy ]; }
    inline const EsynetOutputPort & outputPort( long phy ) const { return m_output_port[ phy ]; }
    /* function to access m_statistics_unit */
    inline const EsynetRouterStatistic & statistics() const { return m_statistics_unit; }
    /* function to access m_power_unit */
    inline EsynetRouterPower & powerUnit() { return m_power_unit; }

    inline void setInputNeighbor( long port, const EsynetVC & addr )  { m_input_port[ port ].setInputNeighbor( addr ); }
    inline void setOutputNeighbor( long port, const EsynetVC & addr ) { m_output_port[ port ].setOutputNeighbor( addr ); }


    /* event handler function */
    /* receive flit */
    void receiveFlit(long a, long b, const EsynetFlit & c);
    void receiveFlitInBuffer(long a, long b, const EsynetFlit & c);
    /* receive credit processing of physical port phy and virtual channel vc */
    void receiveCredit( long phy, long vc, long credit );

    /* operate function of vc */
    /* get vc */
    EsynetVC vcSelection( long a, long b );

    /* pipeline functions */
    /* simulate pipeline */
    void routerSimPipeline();
    /* routing decision */
    void routingDecision();
    /* virtual channel arbitration */
    void vcArbitration();
    /* switch arbitration */
    void swArbitration();
    /* flit cross switch */
    void flitOutbuffer();
    /* flit traversal */
    void flitTraversal();

    /* routing algorithm */
    /* XY, TXY, DyXY in sim_routing_xy.cc */
    void algorithmXY( long des_t, long sor_t,long s_ph, long s_vc );
    void algorithmTXY( long des_t, long sor_t, long s_ph, long s_vc );
    void algorithmDyXY( long des_t, long sor_t, long s_ph, long s_vc );
    void algorithmTable( long des_t, long sor_t, long s_ph, long s_vc );

    /* routing function called path execution */
    void routingAlgorithm( long des_t, long sor_t, long s_ph, long s_vc ) { (this->*m_curr_algorithm)( des_t, sor_t, s_ph, s_vc ); }

    /* print configuration of router to stream */
    friend ostream& operator<<(ostream& os, const EsynetRouter & sr);
};


#endif

/*
 * File name : esynet_ni_unit.h
 * Function : Declare NI module.
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
 * @ingroup ESYNET_NI
 * @file esynet_ni_unit.h
 * @brief Declare NI module.
 */

#ifndef ESYNET_NI_UNIT_H
#define ESYNET_NI_UNIT_H

#include "esy_interdata.h"
#include "esy_netcfg.h"
#include "esynet_config.h"
#include "esynet_sim_base.h"
#include "esynet_statistics.h"

#include <cmath>

/**
 * @ingroup ESYNET_NI
 * @brief Network interface module.
 */
class EsynetNI : public EsynetSimBaseUnit
{
private:
    long m_id;      /*!< @brief Network interface id. */
    long m_router;  /*!< @brief Connected router id. */
    long m_port;    /*!< @brief Connected router port. */

    EsyNetworkCfg * m_network_cfg;      /*!< @brief Pointer to network configuration. */
    EsyNetworkCfgRouter * m_router_cfg; /*!< @brief Pointer to configurtion of connected router. */
    EsynetConfig * m_argu_cfg;          /*!< @brief Pointer to argument list. */

    std::vector< EsynetFlit > m_inject_queue;   /*!< @brief Injected flit queue. */
    long m_inject_vc;   /*!< @brief Selected virtual channel. */
    std::vector< long > m_vc_counter;   /*!< @brief Credit counter */

    std::vector< EsynetFlit > m_eject_queue;    /*!< @brief Ejected flit queue */

    std::vector< EsynetEvent > m_accept_list;   /*!< @brief Ejected flit queue. */

    EsynetNIStatistic m_statistic;  /*!< @brief Statistic structure for network interface. */

    esynet::EsynetPayload m_init_data;  /*!< @brief Initializate random number. */
    long m_flit_size;   /*!< @brief Flit size, in 64 bits. */

    bool m_flit_on_link;    /*!< @brief True if there is flit on link. */

public:
    /**
     * @brief Constructor function.
     * @param network_cfg Pointer to network configuration structure.
     * @param id NI id.
     * @param router Connected router id.
     * @param port Connected port.
     * @param argument_cfg Pointer to argument structure.
     */
    EsynetNI( EsyNetworkCfg * network_cfg, long id, long router, long port, EsynetConfig * argument_cfg );

    /**
     * @name Function to access variables
     * @{
     */
    /**
     * @brief Return connected router.
     */
    inline long router() const { return m_router; }
    /**
     * @brief Return connected port.
     */
    inline long port() const { return m_port; }
    /**
     * @brief Return statistic module.
     */
    inline const EsynetNIStatistic & statistic() { return m_statistic; }
    /**
     * @brief Return accepted list.
     */
    inline const std::vector< EsynetEvent > acceptList() const { return m_accept_list; }
    /**
     * @brief Return true if link is clear.
     */
    bool isEmpty();
    /**
     * @brief Clear accepted list.
     */
    inline void clearAcceptList() { m_accept_list.clear(); }
    /**
     * @brief Clear flit on link.
     */
    inline void clearFlitOnLink() { m_flit_on_link = false; }
    /**
     * @}
     */

    /**
     * @name Entry to function flow
     * @{
     */
    /**
     * @brief Inject new packet into the NI. Entry to handle EVG event.
     * @param b New packet.
     */
    void injectPacket( const EsynetFlit& b );
    /**
     * @brief Functions to simulate ni pipeline. Entry to handle ROUTER event.
     */
    void niSimPipeline();
    /**
     * @brief Receive flit from another router. Entry to handle WIRE event.
     * @param vc Virtual channel.
     * @param b  Received flit.
     */
    void receiveFlit( long vc, const EsynetFlit & b );
    /**
     * @brief Receive credit from router. Entry to handle CREDIT event.
     * @param vc virtual channel.
     * @param credit Credit value.
     */
    void receiveCredit( long vc, long credit ) { m_vc_counter[ vc ] = credit ;}
    /**
     * @brief Receive flit from receive flit queue. Entry to handle NIREAD event.
     */
    void receiveNiInterrupt();
    /**
     * @}
     */

protected:
    /**
     * @brief Receive packet.
     * @param b Received flit.
     */
    void receivePacket( const EsynetFlit & b );
};

#endif

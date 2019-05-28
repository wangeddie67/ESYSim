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

#ifndef ESYNET_NI_UNIT_H
#define ESYNET_NI_UNIT_H

/* including head file */
#include "esy_interdata.h"
#include "esy_networkcfg.h"
#include "esynet_config.h"
#include "esynet_sim_base.h"
#include "esynet_statistics.h"

/* include library file */
#include <cmath>

/* namespace */
using namespace std;

/**
 * @brief Network interface module.
 */
class EsynetNI : public EsynetSimBaseUnit
{

private:
    /* General Information */
    long m_id;  /*!< @brief Network interface id. */
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
    bool m_empty_require;

public:
    /* constructor function */
    EsynetNI( EsyNetworkCfg * network_cfg, long id, long router, long port, EsynetConfig * argument_cfg );

    inline long router() const { return m_router; }
    inline long port() const { return m_port; }

    const EsynetNIStatistic & statistic() { return m_statistic; }
    /* functions run before simulationrouter */
    void runBeforeRouter();
    /* functions run before simulation of router */
    void runAfterRouter();

    /* inject packet */
    void injectPacket(const EsynetFlit& b);
    /* receive packet */
    void receiveFlit(long vc, const EsynetFlit & b);
    void receivePacketHandler();
    void receivePacket(const EsynetFlit & b);
    /* receive credit */
    void receiveCredit( long vc, long credit ) { m_vc_counter[ vc ] = credit ;}
    /* flit traversal */
    void flitTraversal();
    
    const std::vector< EsynetEvent > acceptList() const { return m_accept_list; }
    void clearAcceptList() { m_accept_list.clear(); }

    /* get suggest vc */
    long suggestVC();

    void setEmptyReqire( bool req ) { m_empty_require = req; }
    bool isEmpty();

    void clearFlitOnLink() { m_flit_on_link = false; }
};

#endif

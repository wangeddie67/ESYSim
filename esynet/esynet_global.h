/*
 * File name : esynet_global.h
 * Function : Define the global enumerate, type and constant value.
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

#ifndef ESYNET_GLOBAL_H
#define ESYNET_GLOBAL_H

#include <vector>

/**
 * @defgroup ESYNET_GROUP Esynet
 */
/**
 * @ingroup ESYNET_GROUP
 * @defgroup ESYNET_GLOBAL Global Definition
 */
/**
 * @ingroup ESYNET_GROUP
 * @defgroup ESYNET_SIM_ENGINE Simulation Engine
 */
/**
 * @ingroup ESYNET_GROUP
 * @defgroup ESYNET_OPTIONS Options
 */
/**
 * @ingroup ESYNET_GROUP
 * @defgroup ESYNET_NETWORK Network Platform
 */
/**
 * @ingroup ESYNET_GROUP
 * @defgroup ESYNET_ROUTER Router
 */
/**
 * @ingroup ESYNET_ROUTER
 * @defgroup ESYNET_PORT Port structure
 */
/**
 * @ingroup ESYNET_NETWORK
 * @defgroup ESYNET_POWER Power model
 */
/**
 * @ingroup ESYNET_NETWORK
 * @defgroup ESYNET_STATISTICS Statistics unit
 */

/**
 * @ingroup ESYNET_GLOBAL
 * @file esynet_global.h
 * @brief Define global enumerates, types, and constant values.
 */

/**
 * @ingroup ESYNET_GLOBAL
 * @brief Namespace for global enumerates, types, and constant values.
 */
namespace esynet
{
/**
 * @name Enumerate definition
 * @{
 */
/**
 * @brief Virtual channel status.
 */
enum EsynetVCStatus
{
    VCS_INIT,       /*!< @brief Initialization status. */
    VCS_ROUTING,    /*!< @brief Wait for routing determine. */
    VCS_VC_AB,      /*!< @brief Wait for virtual channel arbitration. */
    VCS_SW_AB,      /*!< @brief Wait for switch arbitration. */
    VCS_SW_TR,      /*!< @brief Transport a packet through switch. */
};
/**
 * @brief Virtual channel usage state.
 */
enum EsynetVCUsage
{
    VC_USED,    /*!< @brief assigned. */
    VC_FREE     /*!< @brief free. */
};
/**
 * @brief Routing algorithm.
 */
enum EsynetRoutingAlg
{
    RA_TABLE,   /*!< @brief Routing algorithm based on table. */
    RA_XY,      /*!< @brief Extend XY routing algorithm (1D, 2D, multi-diamension). */
    RA_TXY,     /*!< @brief Extend Torus XY routing algorith (1D, 2D, multi-diamension). */
    RA_DYXY,    /*!< @brief DyXY routing algorithm (2D). */
};
/**
 * @brief Arbiter Type.
 */
enum EsynetArbiterType
{
    AR_RANDOM,     /*!< @brief Random arbiter. */
    AR_RR,         /*!< @brief Round-robin arbiter. */
    AR_MATRIX,    /*!< @brief Matrix arbiter. */
};
/**
 * @brief Traffic generation profiles.
 */
enum EsynetTrafficProfile
{
    TP_UNIFORM,     /*!< @brief Uniform traffic. */
    TP_TRANSPOSE1,  /*!< @brief Transpose traffic 1. (x,y) -> (Y-1-y,X-1-x), X,Y is the size of network */
    TP_TRANSPOSE2,  /*!< @brief Transpose traffic 2. (x,y) -> (y,x) */
    TP_BITREVERSAL, /*!< @brief Bit-reversal traffic. */
    TP_BUTTERFLY,   /*!< @brief Butterfly traffic. */
    TP_SHUFFLE,     /*!< @brief Shuffly traffic. */
};
/**
 * @}
 */

/**
 * @name Type definition
 * @{
 */
typedef std::vector< long > EsynetAddr;   /*!< @brief Address type.  */
typedef std::pair< long, long > EsynetVC; /*!< @brief Virtual channel index. first is physical port id, second is virtual channel id. */
typedef unsigned long long EsynetAtomType;    /*!< @brief Data type. */
typedef std::vector< EsynetAtomType > EsynetPayload;   /*!< @brief Data type of python flit. */
/**
 * @}
 */

/**
 * @name Constant value definition
 * @{
 */
#define VC_NULL EsynetVC(-1, -1)  /*!< @brief Not valid virtual channel index. */
#define WIRE_DELAY_ 0.9     /*!< @brief Delay of link transmission event, 0.9 cycle constantly. */
#define PIPE_DELAY_ 1.0     /*!< @brief Default pipeline event delay, 1.0 cycle constantly. */
#define CREDIT_DELAY_ 1.0   /*!< @brief Delay of credit transmission event, 1.0 cycle constantly. */

/* Macro define */
#define BUFF_BOUND_ 100
#define REPORT_PERIOD_ 2000
#define S_ELPS_ 0.00000001
#define ATOM_WIDTH_ 64    //bus width for power model 
#define ZERO_ 0
#define MAX_64_ 0xffffffffffffffffLL
#define CORR_EFF_ 0.8
#define POWER_NOM_ 1
/**
 * @}
 */

/*
#define BUFF_BOUND_ 100
#define WIRE_DELAY_ 2.0         0.9
#define PIPE_DELAY_ 1.0        1.0
#define CREDIT_DELAY_ 2.0     1.0
#define REPORT_PERIOD_ 2000
#define S_ELPS_ 0.00000001
#define ATOM_WIDTH_ 64
#define ZERO_ 0
#define MAX_64_ 0xffffffffffffffffLL
#define CORR_EFF_ 0.8
#define POWER_NOM_ 1e9
*/
}

#endif

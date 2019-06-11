/*
 * File name : esynet_arbiter.h
 * Function : Declare arbiter module.
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
 * @file esynet_arbiter.h
 * @brief Declare arbiter.
 */

#ifndef ESYNET_ARBITER_H
#define ESYNET_ARBITER_H

#include "esynet_global.h"
#include "esy_networkcfg.h"

#include <vector>
#include <map>

/**
 * @ingroup ESYNET_ARBITER
 * @brief Arbiter.
 */
class EsynetArbiter
{
private:
    esynet::EsynetArbiterType m_type;   /*!< @brief Type of arbiter. */
    int m_size;                         /*!< @brief Number of request signals. */

    std::vector< vector< bool > > m_state_matrix;   /*!< @brief Matrix D-FF for matrix arbiter. */
    std::vector< bool > m_state_vector;             /*!< @brief Vector D-FF for Round-robin arbiter. */

    std::map< int, esynet::EsynetVC > m_vc_map; /*!< @brief Mapping between request signals and virtual channel. */
    std::vector< bool > m_request;              /*!< @brief Request signals. */

public:
    /**
     * @name Construction functions
     * @{
     */
    /**
     * @brief Construction functions.
     * @param type Type of arbiter.
     * @param size Number of request signals.
     */
    EsynetArbiter( esynet::EsynetArbiterType type, int size );
    /**
     * @brief Construction one arbiter by copying from t
     * @param t Instance to copy.
     */
    EsynetArbiter( const EsynetArbiter & t );
    /**
     * @}
     */

    /**
     * @name Functions to set mapping between signals and virtual channels
     * @{
     */
    /**
     * @brief Set map between request signals and input ports.
     * @param router_cfg Const reference to router configuration structure.
     */
    void setInputPortMap( const EsyNetworkCfgRouter & router_cfg );
    /**
     * @brief Set map between request signals and output ports.
     * @param router_cfg Const reference to router configuration structure.
     */
    void setOutputPortMap( const EsyNetworkCfgRouter & router_cfg );
    /**
     * @}
     */

    /**
     * @name Functions to set and access request.
     * @{
     */
    /**
     * @brief Return const reference to request signals.
     */
    inline const std::vector< bool > & request() const { return m_request; }
    /**
     * @brief Set request signals.
     */
    inline void setRequest( const std::vector< bool > & req ) { m_request = req; }
    /**
     * @brief Clear all request signals.
     */
    inline void clearRequest() { for ( size_t i = 0; i < m_size; i ++ ) m_request[ i ] = false; }
    /**
     * @brief Set the request signal specified by index a.
     */
    inline void setRequest( int a ) { m_request[ a ] = true; }
    /**
     * @brief Set the request signal specified by virtual channel a.
     */
    inline void setRequest( const esynet::EsynetVC & a ) 
    {
        for ( size_t i = 0; i < m_size; i ++ )
        {
            if ( m_vc_map[ i ] == a )
            {
                m_request[ i ] = true;
            }
        }
    }
    /**
     * @brief Return true if request is not empty.
     */
    bool reqestIsValid();
    /**
     * @brief Clear the request signal specified by index a.
     */
    inline void clearRequest( int a ) { m_request[ a ] = false; }
    /**
     * @brief Clear the request signal specified by virtual channel a.
     */
    inline void clearRequest( const esynet::EsynetVC & a )
    { 
        for ( size_t i = 0; i < m_size; i ++ ) 
        {
            if ( m_vc_map[ i ] == a ) 
            {
                m_request[ i ] = false;
            }
        }
    }
    /**
     * @}
     */

    /**
     * @name Functions to adjust the priority temporarily.
     * @{
     */
    /**
     * @brief Set the specified request signal with highest priority.
     * @param port Index to specify request signal.
     */
    void setHighestPriority( int port );
    /**
     * @brief Set the specified request signal with highest priority.
     * @param port Virtual channel to specify request signal.
     */
    void setHighestPriority( const esynet::EsynetVC & port );
    /**
     * @brief Set the specified request signal with lowest priority.
     * @param port Index to specify request signal.
     */
    void setLowestPriority( int port );
    /**
     * @brief Set the specified request signal with lowest priority.
     * @param port Virtual channel to specify request signal.
     */
    void setLowestPriority( const esynet::EsynetVC & port );
    /**
     * @}
     */

    /**
     * @name Arbitration algorithm.
     * @{
     */
    /**
     * @brief Return the granted signal by index and update arbiter status.
     */
    int grant();
    /**
     * @brief Return the granted signal by virtual channel and update arbiter status.
     */
    esynet::EsynetVC grantVc();
    /**
     * @}
     */
};

#endif

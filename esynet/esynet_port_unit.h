/*
 * File name : esynet_port_unit.h
 * Function : Define port module.
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
 * @ingroup ESYNET_PORT
 * @file esynet_port_unit.h
 * @brief Define port module.
 */

#ifndef ESYNET_PORT_UNIT_H
#define ESYNET_PORT_UNIT_H

#include "esy_netcfg.h"
#include "esynet_flit.h"

/**
 * @ingroup ESYNET_PORT
 * @brief Input virtual channel structure.
 */
class EsynetInputVirtualChannel
{
private:
    std::vector< EsynetFlit > m_input_buffer;  /*!< @brief input buffer, each entry is one flit. */
    esynet::EsynetVCStatus m_vc_state; /*!< @brief the state of each input VC. */
    std::vector< esynet::EsynetVC > m_routing;  /*!< @brief routing requirement, insert by routing algorithm. Each entry is one requiment. */
    esynet::EsynetVC m_connection;  /*!< @brief the chosen virtual channel. */

public:
    /**
     * @name constructor functions
     * @{
     */
    /**
     * @brief Construct one port with default value.
     */
    EsynetInputVirtualChannel()
        : m_input_buffer()
        , m_vc_state( esynet::VCS_INIT )
        , m_routing()
        , m_connection( esynet::VC_NULL )
    {}
    /**
     * @}
     */

    /**
     * @name Functions to access and set input buffer
     * @{
     */
    /**
     * @brief Return the number of flits in buffer.
     */
    inline long flitCount() const { return (long)m_input_buffer.size(); }
    /**
     * @brief Return constant reference of the first flit in buffer.
     */
    inline const EsynetFlit& getFlit() const { return m_input_buffer[ 0 ]; }
    /**
     * @brief Return reference of the first flit in buffer.
     */
    inline EsynetFlit& getFlit() { return m_input_buffer[ 0 ]; }
    /**
     * @brief Add a flit to buffer.
     */
    inline void addFlit(const EsynetFlit & flit) { m_input_buffer.push_back(flit); }
    /**
     * @brief Remove first flit from buffer.
     */
    inline void removeFlit() { m_input_buffer.erase( m_input_buffer.begin() ); }
    /**
     * @}
     */

    /**
     * @name Functions to access and set virtual status.
     * @{
     */
    /**
     * @brief Return states for virtual channel.
     */
    inline esynet::EsynetVCStatus state() const { return m_vc_state; }
    /**
     * @brief Set state of virtual channel.
     */
    inline void updateState( esynet::EsynetVCStatus state ) { m_vc_state = state; }
    /**
     * @}
     */

    /**
     * @name Functions to access and set routing requirement.
     * @{
     */
    /**
     * @brief Return constant reference of routing requirement.
     */
    inline const std::vector< esynet::EsynetVC > & routing() const { return m_routing; }
    /**
     * @brief Insert routing requirement.
     */
    inline void addRouting( const esynet::EsynetVC& routing ) { m_routing.push_back( routing ); }
    /**
     * @brief Clear routing requirement.
     */
    inline void clearRouting() { m_routing.clear(); }

    /**
     * @name Functions to access and set chosen virtual channel.
     * @{
     */
    /**
     * @brief Return chosen virtual channel.
     */
    const esynet::EsynetVC & connection() const { return m_connection; }
    /**
     * @brief Set chosen virtual channel.
     */
    void assignConnection( esynet::EsynetVC crouting ) { m_connection = crouting; }
    /**
     * @brief Clear chosen virtual channel.
     */
    void clearConnection() { m_connection = esynet::VC_NULL; }
    /**
     * @}
     */
};

/**
 * @ingroup ESYNET_PORT
 * @brief Input physical port structure.
 */
class EsynetInputPort : public std::vector< EsynetInputVirtualChannel >
{
private:
    esynet::EsynetVC m_neighbor_addr;   /*!< @brief Connected physical port and router. First is router id, second is physical port id. */
    long m_input_buffer_size;   /*!< @brief Size of input buffer. */

public:
    /**
     * @name constructor functions
     * @{
     */
    /**
     * @brief Construct one empty port.
     */
    EsynetInputPort()
        : m_neighbor_addr( esynet::VC_NULL )
        , m_input_buffer_size( 0 )
    {}
    /**
     * @brief Construct one port with specified configuration.
     * @param port_cfg Port configuration structure.
     */
    EsynetInputPort( const EsyNetCfgPort & port_cfg)
        : m_neighbor_addr( esynet::VC_NULL )
        , m_input_buffer_size( port_cfg.inputBuffer() )
    {
        // generate virtual channel
        for ( int i = 0; i < port_cfg.inputVcNumber(); i ++ ) 
        {
            push_back( EsynetInputVirtualChannel() );
        }
    }
    /**
     * @}
     */

    /**
     * @name Function to access neighbor router and port.
     * @{
     */
    /**
     * @brief Return pair of input neighbor id and port id.
     */
    inline const esynet::EsynetVC & inputNeighbor() const { return m_neighbor_addr; }
    /**
     * @brief Return neighbor router id.
     */
    inline long inputNeighborRouter() const { return m_neighbor_addr.first; }
    /**
     * @brief Return neighbor port id.
     */
    inline long inputNeighborPort() const { return m_neighbor_addr.second; }
    /**
     * @brief Set pair of input neighbor id and port id.
     */
    inline void setInputNeighbor( const esynet::EsynetVC & addr ) { m_neighbor_addr = addr; }
    /**
     * @}
     */

    /**
     * @brief Return number of virtual channel.
     */
    inline long vcNumber() const { return (long)size(); }
    /**
     * @brief Return input buffer size.
     */
    inline long inputBufferSize() const { return m_input_buffer_size; }
    /**
     * @brief Return true is all virtual channels are empty.
     */
    inline bool isEmpty() const
    {
        for ( size_t i = 0; i < size(); i ++ )
        {
            if ( at( i ).flitCount() > 0 )
            {
                return false;
            }
        }
        return true;
    }
};

/**
 * @ingroup ESYNET_PORT
 * @brief Output virtual channel structure.
 */
class EsynetOutputVirtualChannel
{
private:
    esynet::EsynetVC m_vc_assign;   /*!< @brief Assigned input virtual channel. */
    esynet::EsynetVCUsage m_vc_usage;   /*!< @brief Virtual channel usage type. */
    long m_credit_counter;  /*!< @brief Credit counter. */

public:
    /**
     * @name constructor functions
     * @{
     */
    /**
     * @brief Construct one port with default value.
     */
    EsynetOutputVirtualChannel()
        : m_vc_assign( esynet::VC_NULL )
        , m_vc_usage( esynet::VC_FREE )
        , m_credit_counter( 0 )
    {}
    /**
     * @brief Construct one port with specified configuration.
     * @param port_cfg Port configuration structure.
     */
    EsynetOutputVirtualChannel( const EsyNetCfgPort & port_cfg)
        : m_vc_assign( esynet::VC_NULL )
        , m_vc_usage( esynet::VC_FREE )
        , m_credit_counter( port_cfg.inputBuffer() )
    {}
    /**
     * @}
     */

    /**
     * @name Function to access and set assigned virtual channel and usage type
     * @{
     */
    /**
     * @brief Return usage state.
     */
    inline esynet::EsynetVCUsage usage() const { return m_vc_usage; }
    /**
     * @brief Return assigned input virtual channel.
     */
    inline const esynet::EsynetVC & assign() const { return m_vc_assign; }
    /**
     * @brief Assigned input virtual channel with this virtual channel.
     */
    void acquire( const esynet::EsynetVC & channel ) { m_vc_usage = esynet::VC_USED; m_vc_assign = channel; }
    /**
     * @brief Release assignment of this virtual channel.
     */
    void release() { m_vc_usage = esynet::VC_FREE; m_vc_assign = esynet::VC_NULL; }
    /**
     * @}
     */

    /**
     * @name Function to access and set credit counter.
     * @{
     */
    /**
     * @brief Return credit counter.
     */
    inline long creditCounter() const { return m_credit_counter; }
    /**
     * @brief Increase credit counter by 1.
     */
    inline void incCreditCounter() { m_credit_counter ++; }
    /**
     * @brief Decrease credit counter by 1.
     */
    inline void decCreditCounter() { m_credit_counter --; }
    /**
     * @brief Set credict counter.
     */
    inline void setCreditCounter( long credit ) { m_credit_counter = credit; }
    /**
     * @}
     */
};

/**
 * @ingroup ESYNET_PORT
 * @brief Output physical port structure.
 */
class EsynetOutputPort : public std::vector< EsynetOutputVirtualChannel >
{
private:
    std::vector< EsynetFlit > m_output_buffer;  /*!< @brief Output buffer. Each entry is one flit. */
    std::vector< esynet::EsynetVC > m_out_add;  /*!< @brief Output address of packets in output buffer. */
    esynet::EsynetVC m_neighbor_addr;   /*!< @brief Connected physical port and router. First is router id, second is physical port id. */
    long m_output_buffer_size;   /*!< @brief Size of output buffer. */
    bool m_flit_on_link;    /*!< @brief True if there is one flit on the link. */

public:
    /**
     * @name constructor functions
     * @{
     */
    /**
     * @brief Construct one empty port.
     */
    EsynetOutputPort()
        : m_output_buffer()
        , m_out_add()
        , m_neighbor_addr( esynet::VC_NULL )
        , m_output_buffer_size( 0 )
        , m_flit_on_link( false )
    {}
    /**
     * @brief Construct one port with specified configuration.
     * @param port_cfg Port configuration structure.
     */
    EsynetOutputPort( const EsyNetCfgPort & port_cfg)
        : m_output_buffer()
        , m_out_add()
        , m_neighbor_addr( esynet::VC_NULL )
        , m_output_buffer_size( port_cfg.outputBuffer() )
        , m_flit_on_link( false )
    {
        for ( int i = 0; i < port_cfg.outputVcNumber(); i ++ )
        {
            push_back( EsynetOutputVirtualChannel(port_cfg) );
        }
    }

    /**
     * @name Functions to access and set output buffer
     * @{
     */
    /**
     * @brief Return the number of flits in buffer.
     */
    inline long flitCount() const { return (long)m_output_buffer.size(); }
    /**
     * @brief Return constant reference of the first flit in buffer.
     */
    inline const EsynetFlit& getFlit() const { return m_output_buffer[ 0 ]; }
    /**
     * @brief Return reference of the first flit in buffer.
     */
    inline EsynetFlit& getFlit() { return m_output_buffer[ 0 ]; }
    /**
     * @brief Add a flit to buffer.
     */
    inline void addFlit(const EsynetFlit & flit) { m_output_buffer.push_back(flit); }
    /**
     * @brief Remove first flit from buffer.
     */
    inline void removeFlit() { m_output_buffer.erase( m_output_buffer.begin() ); }
    /**
     * @}
     */

    /**
     * @name Functions to access and set output address
     * @{
     */
    /**
     * @brief Return the output address of first flit in buffer.
     */
    inline const esynet::EsynetVC & getAdd() const { return m_out_add[ 0 ]; }
    /**
     * @brief Remove the output address of first flit in buffer.
     */
    inline void removeAdd() { m_out_add.erase( m_out_add.begin() ); }
    /**
     * @brief Add the output address of last flit in buffer.
     */
    inline void addAdd( const esynet::EsynetVC & vc ) { m_out_add.push_back( vc ); }
    /**
     * @}
     */

    /**
     * @name Function to access neighbor router and port.
     * @{
     */
    /**
     * @brief Return pair of output neighbor id and port id.
     */
    inline const esynet::EsynetVC & outputNeighbor() const { return m_neighbor_addr; }
    /**
     * @brief Return neighbor router id.
     */
    inline long outputNeighborRouter() const { return m_neighbor_addr.first; }
    /**
     * @brief Return neighbor port id.
     */
    inline long outputNeighborPort() const { return m_neighbor_addr.second; }
    /**
     * @brief Set pair of output neighbor id and port id.
     */
    inline void setOutputNeighbor( const esynet::EsynetVC & addr ) { m_neighbor_addr = addr; }
    /**
     * @}
     */

    /**
     * @name Function to access flit on link
     * @{
     */
    /**
     * @brief Return true if there is flit on link.
     */
    inline bool flitOnLink() const { return m_flit_on_link; }
    /**
     * @brief Set flit on link.
     */
    inline void setFlitOnLink() { m_flit_on_link = true; }
    /**
     * @brief Clear flit on link.
     */
    inline void clearFlitOnLink() { m_flit_on_link = false; }
    /**
     * @}
     */

    /**
     * @brief Return number of virtual channel.
     */
    inline long vcNumber() const { return (long)size(); }
    /**
     * @brief Return input buffer size.
     */
    inline long outputBufferSize() const { return m_output_buffer_size; }
    /**
     * @brief Return true is all virtual channels are empty.
     */
    inline bool isEmpty() const
    {
        if ( m_flit_on_link )
        {
            return false;
        }
        if ( flitCount() > 0 )
        {
            return false;
        }
        for ( size_t i = 0; i < size(); i ++ )
        {
            if ( at( i ).usage() != esynet::VC_FREE )
            {
                return false;
            }
        }
        return true;
    }
};

#endif

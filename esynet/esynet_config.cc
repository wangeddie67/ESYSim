/*
 * File name : esynet_config.cc
 * Function : Implement command line option parser.
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
 * @ingroup ESYNET_OPTIONS
 * @file esynet_config.cc
 * @brief Implement options for ESYNet.
 */

#include "esynet_config.h"
#include "esy_interdata.h"
#include "esy_networkcfg.h"
#include "esy_faultcfg.h"

EsynetConfig::EsynetConfig( EsynetConfig::ConfigType type )
    : m_topology( EsyNetworkCfg::NT_MESH_2D )
    , m_ary_number( 2, 8 )
    , m_physical_port_number( 5 )
    , m_virtual_channel_number( 1 )
    , m_in_buffer_size( 12 )
    , m_out_buffer_size( 12 )
    , m_data_width( 32 )
    , m_link_length( 1000 )
    , m_routing_alg( esynet::RA_XY )
    , m_routing_table( "../example/routing" )
    , m_arbiter( esynet::AR_MATRIX )
    , m_flow_control( esynet::FC_WORMWHOLE )
    , m_ni_buffer_size( 1 )
    , m_ni_read_delay( 0 )
    , m_network_cfg_file_enable( false )
    , m_network_cfg_file_out_enable( false )
    , m_network_cfg_file_name( "../example/routercfg" )
    , m_rand_seed( 1 )
    , m_sim_length( 10000 )
    , m_simulation_period( 1.0 )
    , m_injected_packet( -1 )
    , m_warmup_packet( 0 )
    , m_latency_measure_packet( -1 )
    , m_throughput_measure_packet( -1 )
    , m_traffic_injection_disable( false )
    , m_input_trace_enable( false )
    , m_input_trace_text_enable( false )
    , m_input_trace_buffer_size( 10000 )
    , m_input_trace_file_name( "../example/benchmark" )
    , m_traffic_rule( 0 )
    , m_traffic_pir( 0.1 )
    , m_packet_size( 5 )
    , m_output_trace_enable( false )
    , m_output_trace_text_enable( false )
    , m_output_trace_buffer_size( 10000 )
    , m_output_trace_file_name( "../example/benchmark" )
    , m_event_trace_enable( false )
    , m_event_trace_text_enable( false )
    , m_event_trace_buffer_size( 10000 )
    , m_event_trace_file_name( "../example/event" )
    , m_event_trace_cout_enable( false )
{
    insertVariables( type );
}

EsynetConfig::EsynetConfig( int argc, char * const argv[], ConfigType type )
    : m_topology( EsyNetworkCfg::NT_MESH_2D )
    , m_ary_number( 2, 8 )
    , m_physical_port_number( 5 )
    , m_virtual_channel_number( 1 )
    , m_in_buffer_size( 12 )
    , m_out_buffer_size( 12 )
    , m_data_width( 32 )
    , m_link_length( 1000 )
    , m_routing_alg( esynet::RA_XY )
    , m_routing_table( "../example/routing" )
    , m_arbiter( esynet::AR_MATRIX )
    , m_flow_control( esynet::FC_WORMWHOLE )
    , m_ni_buffer_size( 1 )
    , m_ni_read_delay( 0 )
    , m_network_cfg_file_enable( false )
    , m_network_cfg_file_out_enable( false )
    , m_network_cfg_file_name( "../example/routercfg" )
    , m_rand_seed( 1 )
    , m_sim_length( 10000 )
    , m_simulation_period( 1.0 )
    , m_injected_packet( -1 )
    , m_warmup_packet( 0 )
    , m_latency_measure_packet( -1 )
    , m_throughput_measure_packet( -1 )
    , m_traffic_injection_disable( false )
    , m_input_trace_enable( false )
    , m_input_trace_text_enable( false )
    , m_input_trace_buffer_size( 10000 )
    , m_input_trace_file_name( "../example/benchmark" )
    , m_traffic_rule( 0 )
    , m_traffic_pir( 0.1 )
    , m_packet_size( 5 )
    , m_output_trace_enable( false )
    , m_output_trace_text_enable( false )
    , m_output_trace_buffer_size( 10000 )
    , m_output_trace_file_name( "../example/benchmark" )
    , m_event_trace_enable( false )
    , m_event_trace_text_enable( false )
    , m_event_trace_buffer_size( 10000 )
    , m_event_trace_file_name( "../example/event" )
    , m_event_trace_cout_enable( false )
{
    insertVariables( type );

    if ( analyse( argc, argv ) != RESULT_OK )
    {
        exit( 0 );
    }

    if ( !preDefineCheck() )
    {
        exit( 0 );
    }
}

bool EsynetConfig::preDefineCheck()
{
    switch (m_topology)
    {
        case EsyNetworkCfg::NT_SWITCH :
            {
                m_ary_number.resize( 1 );
                m_ary_number[ 0 ] = 1;
                if ( m_routing_alg != esynet::RA_TABLE )
                {
                    std::cout << "Only table-based routing algorithm works on the single switch." << std::endl;
                    return false;
                }
            }
            break;
        case EsyNetworkCfg::NT_RING :
            {
                m_ary_number.resize( 1 );
                if ( m_physical_port_number != 3 )
                {
                    std::cout << "The router of ring NoC must have 3 physical channels." << std::endl;
                    m_physical_port_number = 3;
                }
                if ( m_routing_alg != esynet::RA_SINGLERING && m_routing_alg != esynet::RA_DOUBLERING )
                {
                    std::cout << "Only single/double ring routing algorithm works on the ring NoC." << std::endl;
                    return false;
                }
            }
            break;
        case EsyNetworkCfg::NT_MESH_2D :
            {
                m_ary_number.resize( 2 );
                if ( m_ary_number[ 1 ] == 0 )
                {
                    m_ary_number[ 1 ] = m_ary_number[ 0 ];
                }
                if ( ( m_routing_alg != esynet::RA_XY ) && ( m_routing_alg != esynet::RA_DYXY ) )
                {
                    std::cout << "Only XY and DyXY routing algorithm work on the Mesh NoC." << std::endl;
                    return false;
                }
            }
            break;
        case EsyNetworkCfg::NT_TORUS_2D :
            {
                m_ary_number.resize( 2 );
                if ( m_ary_number[ 1 ] == 0 )
                {
                    m_ary_number[ 1 ] = m_ary_number[ 0 ];
                }
                if ( m_routing_alg != esynet::RA_TXY )
                {
                    std::cout << "Only Torus XY routing algorithm works on the Torus NoC." << std::endl;
                    return false;
                }
            }
            break;
        case EsyNetworkCfg::NT_MESH_DIA :
            {
                if ( m_routing_alg != esynet::RA_XY )
                {
                    std::cout << "Only extended XY routing algorithm works on the mult-dim Mesh NoC." << std::endl;
                    return false;
                }
            }
            break;
        case EsyNetworkCfg::NT_TORUS_DIA :
            {
                if ( m_routing_alg != esynet::RA_TXY )
                {
                    std::cout << "Only extended Torus XY routing algorithm works on the mult-dim Torus NoC." << std::endl;
                    return false;
                }
            }
            break;
    }

    return true;
}

void EsynetConfig::insertVariables(ConfigType type)
{
    setHead( "Network-on-Chip Simulator" );
    setVersion( "4.0" );

    // Network configuration group
    if (( type & CONFIG_NETWORK_CFG ) > 0 )
    {
        // Topology
        m_topology.addOption( EsyNetworkCfg::NT_SWITCH,    "Switch" );
        m_topology.addOption( EsyNetworkCfg::NT_RING,      "Ring" );
        m_topology.addOption( EsyNetworkCfg::NT_MESH_2D,   "2DMesh" );
        m_topology.addOption( EsyNetworkCfg::NT_TORUS_2D,  "2DTorus" );
        m_topology.addOption( EsyNetworkCfg::NT_MESH_DIA,  "DiaMesh" );
        m_topology.addOption( EsyNetworkCfg::NT_TORUS_DIA, "DiaTorus" );
        insertEnum( "-topology", "Code for topology", &m_topology);
        // Network size
        insertLongVector( "-array_size", "Size of network in diamension", &m_ary_number
            , "-!network_cfg_file_enable", 1 );
        // Number of physical port
        insertLong( "-phy_number", "Number of physical port of a router", &m_physical_port_number
            , "-!network_cfg_file_enable", 1 );
        // Number of virtual channel for each physical port
        insertLong( "-vc_number", "VC per physical port", &m_virtual_channel_number
            , "-!network_cfg_file_enable", 1 );
        // Input buffer size
        insertLong( "-in_buffer_size", "Buffer size of each VC", &m_in_buffer_size
            , "-!network_cfg_file_enable", 1 );
        // Output buffer size
        insertLong( "-out_buffer_size", "Output buffer size", &m_out_buffer_size
            , "-!network_cfg_file_enable", 1 );
        // Width of data path
        insertLong( "-data_path_width", "width of data path, i.e. num. of bits", &m_data_width );
        // Link length
        insertDouble( "-link_length", "link length #num", &m_link_length );
        // Routing algorithm
        m_routing_alg.addOption(esynet::RA_SINGLERING, "SingleRing" );
        m_routing_alg.addOption(esynet::RA_DOUBLERING, "DoubleRing" );
        m_routing_alg.addOption(esynet::RA_XY, "XY");
        m_routing_alg.addOption(esynet::RA_TXY, "TXY");
        m_routing_alg.addOption(esynet::RA_DYXY, "DyXY");
        m_routing_alg.addOption(esynet::RA_TABLE, "Table");
        insertEnum("-routing_alg", "Code of chosen routing algorithm", &m_routing_alg );
        // Routing table
        insertOpenFile( "-routing_table", "Routing table file name", &m_routing_table, "", 1, "routing" );
        // Arbiter
        m_arbiter.addOption(esynet::AR_RANDOM, "Random");
        m_arbiter.addOption(esynet::AR_RR, "RR");
        m_arbiter.addOption(esynet::AR_MATRIX, "Matrix");
        insertEnum("-arbiter", "Code of aribiter", &m_arbiter );
        // Flow control
        m_flow_control.addOption(esynet::FC_WORMWHOLE, "WormWhole");
        m_flow_control.addOption(esynet::FC_RING, "Ring");
        insertEnum("-flow_control", "Code of flow control", &m_flow_control );
        // NI buffer size
        insertLong( "-ni_buffer_size", "Buffer size of NI, #unit", &m_ni_buffer_size );
        // NI read delay
        insertLong( "-ni_read_delay", "Read delay of NI, #cycle", &m_ni_read_delay );
        // Network configuration file enable
        insertBool( "-network_cfg_file_enable", "Enable network configuration file", &m_network_cfg_file_enable );
        // Network configuration output enable
        insertBool( "-network_cfg_out_file_enable", "Enable network configuration output file", &m_network_cfg_file_out_enable );
        // Network configuration file
        insertOpenFile( "-network_cfg_file_name", "Network configuration file name", &m_network_cfg_file_name
            , "-network_cfg_file_enable.+-network_cfg_out_file_enable", 1, NETCFG_EXTENSION );
    }

    // Simulation control group
    if ( ( type & CONFIG_SIM_CONTROL ) > 0 )
    {
        // Random seed
        insertLong( "-random_seed", "random seed used for generate random number", &m_rand_seed );
        // Simulation period
        insertDouble( "-simulation_period", "simulation period, #cycle", &m_simulation_period );
        // Number of Injected packet
        insertLong( "-injected_packet", "total number of injected packets", &m_injected_packet );
        // Number of Warmup packet
        insertLong( "-warmup_packet", "packet number in warmup phase", &m_warmup_packet );
        // Number of latency measurement packet
        insertLong( "-latency_measure_packet", "packet number for latency measurement", &m_latency_measure_packet );
        // Number of throughput measurement packet
        insertLong( "-throughput_measure_packet", "packet number for throughput measurement", &m_throughput_measure_packet );
    }

    // Simulation length. if use interface, this is not used.
    if ( ( type & CONFIG_SIM_LENGTH ) > 0 )
    {
        // Simulation length
        insertDouble( "-sim_length", "simulation length, i.e. 10,000,000 cycles", &m_sim_length );
    }

    // Traffic injection group.
    if ( ( type & CONFIG_TRAFFIC_INJECTION ) > 0 )
    {
        // Disable traffic injection
        insertBool( "-traffic_injection_disable", "disable traffic injection", &m_traffic_injection_disable );
        // Enable input benchmark trace
        insertBool( "-input_trace_enable", "input trace file is used instead of PE", &m_input_trace_enable
            , "-!traffic_injection_disable", 1 );
        // Enable input benchmark trace in text format
        insertBool( "-input_trace_file_text_enable", "input trace file is text format", &m_input_trace_text_enable
            , "-!traffic_injection_disable -input_trace_enable", 1 );
        // Input benchmark buffer size
        insertLong( "-input_trace_buffer_size", "buffer size of input trace interface", &m_input_trace_buffer_size
            , "-!traffic_injection_disable -input_trace_enable", 1 );
        // Input benchmark file
        insertOpenFileIF( "-input_trace_file_name", "including information about packet injection", &m_input_trace_file_name
            , "-!traffic_injection_disable -input_trace_enable", 1, BENCHMARK_EXTENSION );
        // Traffic profile to generate 
        m_traffic_rule.addOption(esynet::TP_UNIFORM, "Uniform");
        m_traffic_rule.addOption(esynet::TP_TRANSPOSE1, "Transpose1");
        m_traffic_rule.addOption(esynet::TP_TRANSPOSE2, "Transpose2");
        m_traffic_rule.addOption(esynet::TP_BITREVERSAL, "Bitreversal");
        m_traffic_rule.addOption(esynet::TP_BUTTERFLY, "Butterfly");
        m_traffic_rule.addOption(esynet::TP_SHUFFLE, "Shuffle");
        insertEnum("-traffic_rule", "rule to generate trace", &m_traffic_rule
            , "-!traffic_injection_disable -!input_trace_enable", 1 );
        // Packet injection rate
        insertDouble( "-traffic_pir", "packet inject rate (packet/cycle/node)", &m_traffic_pir
            , "-!traffic_injection_disable -!input_trace_enable", 1 );
        // packet size
        insertLong( "-packet_size", "number of flits in a packet", &m_packet_size
            , "-!traffic_injection_disable -!input_trace_enable", 1 );
    }
    else
    {
        m_traffic_injection_disable = true;
    }

    // Output traffic file group
    if ( ( type & CONFIG_OUTPUT_TRAFFIC_FILE ) > 0 )
    {
        string depend = "";
        if ( ( type & CONFIG_TRAFFIC_INJECTION ) > 0 )
        {
            depend = "-!input_trace_enable.+";
        }
        // Enable output benchmark file
        insertBool( "-output_trace_enable", "enable output traffic trace", &m_output_trace_enable, depend );
        // Enable output benchmark file in text format
        insertBool( "-output_trace_file_text_enable", "output trace file is text format", &m_output_trace_text_enable
            , depend + "-output_trace_enable", 1 );
        // Output benchmark file buffer size
        insertLong( "-output_trace_buffer_size", "buffer size of output trace interface", &m_output_trace_buffer_size
            , depend + "-output_trace_enable", 1 );
        // Output benchmark file
        insertOpenFileIF( "-output_trace_file_name", "record information about packet injection", &m_output_trace_file_name
            , depend + "-output_trace_enable", 1, BENCHMARK_EXTENSION );
    }

    // Event trace file group
    if ( ( type & CONFIG_EVENT_TRACE_FILE ) > 0 )
    {
        // Enable event trace
        insertBool( "-event_trace_enable", "enable event trace", &m_event_trace_enable );
        // Enable event trace in text format
        insertBool( "-event_trace_file_text_enable", "event trace file is text format", &m_event_trace_text_enable
            , "-event_trace_enable", 1 ); 
        // Event trace buffer size
        insertLong( "-event_trace_buffer_size", "buffer size of event trace interface", &m_event_trace_buffer_size
            , "-event_trace_enable", 1 );
        // Event trace file name
        insertNewFileIF( "-event_trace_file_name", "event trace file name", &m_event_trace_file_name
            , "-event_trace_enable", 1, EVENTTRACE_EXTENSION );
        // Enable on-time event trace
        insertBool( "-event_trace_cout_enable", "enable event trace to cout", &m_event_trace_cout_enable );
    }
}

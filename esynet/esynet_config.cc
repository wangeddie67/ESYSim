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
    : m_topology( esynet::NT_MESH_2D )
    , m_router_num( 64 )
    , m_ary_number( 2, 8 )
    , m_physical_port_number( 5 )
    , m_virtual_channel_number( 1 )
    , m_in_buffer_size( 12 )
    , m_out_buffer_size( 12 )
    , m_data_width( 32 )
    , m_link_length( 1000 )
    , m_routing_alg( esynet::RA_XY )
    , m_arbiter( esynet::AR_MARTRIX )
    , m_ni_buffer_size( 1 )
    , m_ni_read_delay( 0 )
    , m_network_cfg_file_enable( false )
    , m_network_cfg_file_name( "../example/routercfg.txt" )
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
    : m_topology( esynet::NT_MESH_2D )
    , m_router_num( 64 )
    , m_ary_number( 2, 8 )
    , m_physical_port_number( 5 )
    , m_virtual_channel_number( 1 )
    , m_in_buffer_size( 12 )
    , m_out_buffer_size( 12 )
    , m_data_width( 32 )
    , m_link_length( 1000 )
    , m_routing_alg( esynet::RA_XY )
    , m_arbiter( esynet::AR_MARTRIX )
    , m_ni_buffer_size( 1 )
    , m_ni_read_delay( 0 )
    , m_network_cfg_file_enable( false )
    , m_network_cfg_file_name( "../example/routercfg.txt" )
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
        case esynet::NT_SWITCH :
            {
                m_router_num = 1;
                if ( m_routing_alg != esynet::RA_TABLE )
                {
                    std::cout << "Only table-based routing algorithm works on the single switch." << std::endl;
                    return false;
                }
            }
            break;
        case esynet::NT_SINGLE_RING :
            {
                if ( m_routing_alg != esynet::RA_XY )
                {
                    std::cout << "Only extended XY routing algorithm works on the single ring NoC." << std::endl;
                    return false;
                }
            }
            break;
        case esynet::NT_DOUBLE_RING :
            {
                if ( m_routing_alg != esynet::RA_TXY )
                {
                    std::cout << "Only extended Torus XY routing algorithm works on the double ring NoC." << std::endl;
                    return false;
                }
            }
            break;
        case esynet::NT_MESH_2D :
            {
                if ( (m_routing_alg != esynet::RA_XY) || (m_routing_alg != esynet::RA_DYXY ) )
                {
                    std::cout << "Only XY and DyXY routing algorithm work on the Mesh NoC." << std::endl;
                    return false;
                }
            }
            break;
        case esynet::NT_TORUS_2D :
            {
                if ( m_routing_alg != esynet::RA_TXY )
                {
                    std::cout << "Only Torus XY routing algorithm works on the Torus NoC." << std::endl;
                    return false;
                }
            }
            break;
        case esynet::NT_MESH_DIA :
            {
                if ( m_routing_alg != esynet::RA_XY )
                {
                    std::cout << "Only extended XY routing algorithm works on the mult-dim Mesh NoC." << std::endl;
                    return false;
                }
            }
            break;
        case esynet::NT_TORUS_DIA :
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

    if (( type & CONFIG_NETWORK_CFG ) > 0 )
    {
        m_topology.addOption(esynet::NT_SWITCH, "Switch");
        m_topology.addOption(esynet::NT_SINGLE_RING, "SingleRing");
        m_topology.addOption(esynet::NT_DOUBLE_RING, "DoubleRing");
        m_topology.addOption(esynet::NT_MESH_2D, "2DMesh");
        m_topology.addOption(esynet::NT_TORUS_2D, "2DTorus");
        m_topology.addOption(esynet::NT_MESH_DIA, "DiaMesh");
        m_topology.addOption(esynet::NT_TORUS_DIA, "DiaTorus");
        insertEnum( "-topology", "Code for topology", &m_topology);
        insertLong( "-router_num", "Total number of routers", &m_router_num, "-!network_cfg_file_enable", 1 );
        insertLongVector( "-array_size", "Size of network in diamension", &m_ary_number, "-!network_cfg_file_enable", 1 );
        insertLong( "-phy_number", "Number of physical port of a router", &m_physical_port_number, "-!network_cfg_file_enable", 1 );
        insertLong( "-vc_number", "VC per physical port", &m_virtual_channel_number, "-!network_cfg_file_enable", 1 );
        insertLong( "-in_buffer_size", "Buffer size of each VC", &m_in_buffer_size, "-!network_cfg_file_enable", 1 );
        insertLong( "-out_buffer_size", "Output buffer size", &m_out_buffer_size, "-!network_cfg_file_enable", 1 );
        insertLong( "-data_path_width", "width of data path, i.e. num. of bits", &m_data_width );
        insertDouble( "-link_length", "link length #num", &m_link_length );
        m_routing_alg.addOption(esynet::RA_XY, "XY");
        m_routing_alg.addOption(esynet::RA_TXY, "TXY");
        m_routing_alg.addOption(esynet::RA_DYXY, "DyXY");
        m_routing_alg.addOption(esynet::RA_TABLE, "Table");
        insertEnum("-routing_alg", "Code of chosen routing algorithm", &m_routing_alg );
        m_routing_alg.addOption(esynet::AR_RANDOM, "Random");
        m_routing_alg.addOption(esynet::AR_RR, "RR");
        m_routing_alg.addOption(esynet::AR_MARTRIX, "Matrix");
        insertEnum("-arbiter", "Code of aribiter", &m_arbiter );
        insertLong( "-ni_buffer_size", "Buffer size of NI, #unit", &m_ni_buffer_size );
        insertLong( "-ni_read_delay", "Read delay of NI, #cycle", &m_ni_read_delay );
        insertBool( "-network_cfg_file_enable", "Enable network configuration file", &m_network_cfg_file_enable );
        insertOpenFile( "-router_cfg_file_name", "Router configuration file name", &m_network_cfg_file_name, "-network_cfg_file_enable", 1, NETCFG_EXTENSION );
    }

    if ( ( type & CONFIG_SIM_CONTROL ) > 0 )
    {
        insertLong( "-random_seed", "random seed used for generate random number", &m_rand_seed );
        insertDouble( "-sim_length", "simulation length, i.e. 10,000,000 cycles", &m_sim_length );
        insertDouble( "-simulation_period", "simulation period, #cycle", &m_simulation_period );
        insertLong( "-injected_packet", "total number of injected packets", &m_injected_packet );
        insertLong( "-warmup_packet", "packet number in warmup phase", &m_warmup_packet );
        insertLong( "-latency_measure_packet", "packet number for latency measurement", &m_latency_measure_packet );
        insertLong( "-throughput_measure_packet", "packet number for throughput measurement", &m_throughput_measure_packet );
    }
    
    if ( ( type & CONFIG_MAX_HOP ) > 0 )
    {
        m_sim_length = 20;
        insertDouble( "-max_hop", "maximun hop count, i.e. 20 hops", &m_sim_length );
    }

    if ( ( type & CONFIG_TRAFFIC_INJECTION ) > 0 )
    {
        insertBool( "-traffic_injection_disable", "disable traffic injection", &m_traffic_injection_disable );
        insertBool( "-input_trace_enable", "input trace file is used instead of PE", &m_input_trace_enable, "-!traffic_injection_disable", 1 );
        insertBool( "-input_trace_file_text_enable", "input trace file is text format", &m_input_trace_text_enable, "-!traffic_injection_disable -input_trace_enable", 1 );
        insertLong( "-input_trace_buffer_size", "buffer size of input trace interface", &m_input_trace_buffer_size, "-!traffic_injection_disable -input_trace_enable", 1 );
        insertOpenFileIF( "-input_trace_file_name", "including information about packet injection", &m_input_trace_file_name, "-!traffic_injection_disable -input_trace_enable", 1, BENCHMARK_EXTENSION );
        m_traffic_rule.addOption(esynet::TP_UNIFORM, "Uniform");
        m_traffic_rule.addOption(esynet::TP_TRANSPOSE1, "Transpose1");
        m_traffic_rule.addOption(esynet::TP_TRANSPOSE2, "Transpose2");
        m_traffic_rule.addOption(esynet::TP_BITREVERSAL, "Bitreversal");
        m_traffic_rule.addOption(esynet::TP_BUTTERFLY, "Butterfly");
        m_traffic_rule.addOption(esynet::TP_SHUFFLE, "Shuffle");
        insertEnum("-traffic_rule", "rule to generate trace", &m_traffic_rule, "-!traffic_injection_disable -!input_trace_enable", 1 );
        insertDouble( "-traffic_pir", "packet inject rate (packet/cycle/node)", &m_traffic_pir, "-!traffic_injection_disable -!input_trace_enable", 1 );
        insertLong( "-packet_size", "number of flits in a packet", &m_packet_size, "-!traffic_injection_disable -!input_trace_enable", 1 );
    }
    else
    {
        m_traffic_injection_disable = true;
    }

    if ( ( type & CONFIG_OUTPUT_TRAFFIC_FILE ) > 0 )
    {
        string depend = "";
        if ( ( type & CONFIG_TRAFFIC_INJECTION ) > 0 )
        {
            depend = "-!input_trace_enable.+";
        }
        insertBool( "-output_trace_enable", "enable output traffic trace", &m_output_trace_enable, depend );
        insertBool( "-output_trace_file_text_enable", "output trace file is text format", &m_output_trace_text_enable, depend + "-output_trace_enable", 1 );
        insertLong( "-output_trace_buffer_size", "buffer size of output trace interface", &m_output_trace_buffer_size, depend + "-output_trace_enable", 1 );
        insertOpenFileIF( "-output_trace_file_name", "record information about packet injection", &m_output_trace_file_name, depend + "-output_trace_enable", 1, BENCHMARK_EXTENSION );
    }

    if ( ( type & CONFIG_PATH_PAIR_FILE ) > 0 )
    {
        insertBool( "-path_pair_file_text_enable", "path pair file is text format", &m_input_trace_text_enable );
        m_input_trace_file_name = "../example/pathpair";
        insertOpenFileIF( "-path_pair_file_name", "path pair file name, including the information about packet injection", &m_input_trace_file_name, "", 0, PATHPAIR_EXTENSION );
    }

    if ( ( type & CONFIG_EVENT_TRACE_FILE ) > 0 )
    {
        insertBool( "-event_trace_enable", "enable event trace", &m_event_trace_enable );
        insertBool( "-event_trace_file_text_enable", "event trace file is text format", &m_event_trace_text_enable, "-event_trace_enable", 1 ); 
        insertLong( "-event_trace_buffer_size", "buffer size of event trace interface", &m_event_trace_buffer_size, "-event_trace_enable", 1 );
        insertNewFileIF( "-event_trace_file_name", "event trace file name", &m_event_trace_file_name, "-event_trace_enable", 1, EVENTTRACE_EXTENSION );
        insertBool( "-event_trace_cout_enable", "enable event trace to cout", &m_event_trace_cout_enable );
    }
    
    if ( ( type & CONFIG_PATH_EVENT_FILE ) > 0 )
    {
        m_event_trace_file_name = "../example/pathevent";
        insertNewFileIF( "-path_trace_file_name", "path trace file name", &m_event_trace_file_name, "", 0, PATHTRACE_EXTENSION );
    }
}

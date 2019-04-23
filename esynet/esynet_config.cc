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

#include "esynet_config.h"
#include "esy_interdata.h"
#include "esy_networkcfg.h"
#include "esy_faultcfg.h"

EsynetConfig::EsynetConfig(EsynetConfig::ConfigType type) :
	m_router_num( 64 ),
	m_ary_number( 2, 8 ),
	m_physical_port_number( 7 ),
	m_virtual_channel_number( 1 ),
	m_inbuffer_size( 12 ),
	m_outbuffer_size( 12 ),
	m_data_width( 32 ),
	m_link_length( 1000 ),
	m_routing_alg( RA_XY ),
	m_vc_share( 0 ),  /* MONO */

	m_network_cfg_file_enable( false ),
	m_network_cfg_file_name( "../example/routercfg.txt" ),

	m_ran_seed( 1 ),
	m_sim_length( 10000 ),
	m_simulation_period( 1.0 ),

	m_traffic_injection_disable( false ),
	m_input_trace_enable( false ),
	m_input_trace_text_enable( false ),
	m_input_trace_buffer_size( 10000 ),
	m_input_trace_file_name( "../example/benchmark" ),
	m_traffic_rule( 0 ),
	m_traffic_pir( 0.1 ),
	m_packet_size( 5 ),
	
	m_output_trace_enable( false ),
	m_output_trace_text_enable( false ),
	m_output_trace_buffer_size( 10000 ),
	m_output_trace_file_name( "../example/benchmark" ),

	m_event_trace_enable( false ),
	m_event_trace_text_enable( false ),
	m_event_trace_buffer_size( 10000 ),
	m_event_trace_file_name( "../example/event" ),
	m_event_trace_cout_enable( false ),

	m_fault_inject_enable( false ),
	m_fault_inject_file_name( "../example/faultinject" ),

	m_ecc_enable( false ),
	m_ecc_fip_enable( false ),
	m_ecc_name( ECC_HM74 ),
	m_ecc_strategy( ECC_H2H ),
	m_ecc_space( 1 ),

	m_sw_enable( false ),
	m_sw_fip_enable( false ),
	m_sw_capacity( 0, 0 ),

	m_e2e_ack_enable( false ),
	m_e2e_retrans_enable( false ),
	m_e2e_retrans_timer_max( m_sim_length ),

	m_bist_enable( false ),
	m_bist_type_code( BIST_TARRA ),
	m_bist_interval( 10000 ),
	m_bist_timing( 1, 500 ),
	m_bist_flit( 100 ),
	m_bist_offset( 0 ),

	m_injected_packet( -1 ),
	m_warmup_packet( 0 ),
	m_latency_measure_packet( -1 ),
	m_throughput_measure_packet( -1 ),
	
	m_ni_buffer_size( 1 ),
	m_ni_read_delay( 0 )
{
	insertVariables( type );
}

EsynetConfig::EsynetConfig(int argc, char * const argv[], ConfigType type) :
	m_router_num( 64 ),
	m_ary_number( 2, 8 ),
	m_physical_port_number( 7 ),
	m_virtual_channel_number( 1 ),
	m_inbuffer_size( 12 ),
	m_outbuffer_size( 12 ),
	m_data_width( 32 ),
	m_link_length( 1000 ),
	m_routing_alg( RA_XY ),
	m_vc_share( 0 ),  /* MONO */

	m_network_cfg_file_enable( false ),
	m_network_cfg_file_name( "../example/routercfg.txt" ),

	m_ran_seed( 1 ),
	m_sim_length( 10000 ),
	m_simulation_period( 1.0 ),

	m_traffic_injection_disable( false ),
	m_input_trace_enable( false ),
	m_input_trace_text_enable( false ),
	m_input_trace_buffer_size( 10000 ),
	m_input_trace_file_name( "../example/benchmark" ),
	m_traffic_rule( 0 ),
	m_traffic_pir( 0.1 ),
	m_packet_size( 5 ),
	
	m_output_trace_enable( false ),
	m_output_trace_text_enable( false ),
	m_output_trace_buffer_size( 10000 ),
	m_output_trace_file_name( "../example/benchmark" ),

	m_event_trace_enable( false ),
	m_event_trace_text_enable( false ),
	m_event_trace_buffer_size( 10000 ),
	m_event_trace_file_name( "../example/event" ),
	m_event_trace_cout_enable( false ),

	m_fault_inject_enable( false ),
	m_fault_inject_file_name( "../example/faultinject" ),

	m_ecc_enable( false ),
	m_ecc_fip_enable( false ),
	m_ecc_name( ECC_HM74 ),
	m_ecc_strategy( ECC_H2H ),
	m_ecc_space( 1 ),
	
	m_sw_enable( false ),
	m_sw_fip_enable( false ),
	m_sw_capacity( 0, 0 ),

	m_e2e_ack_enable( false ),
	m_e2e_retrans_enable( false ),
	m_e2e_retrans_timer_max( m_sim_length ),

	m_bist_enable( false ),
	m_bist_type_code( BIST_TARRA ),
	m_bist_interval( 10000 ),
	m_bist_timing( 1, 500 ),
	m_bist_flit( 100 ),
	m_bist_offset( 0 ),

	m_injected_packet( -1 ),
	m_warmup_packet( 0 ),
	m_latency_measure_packet( -1 ),
	m_throughput_measure_packet( -1 ),

	m_ni_buffer_size( 1 ),
	m_ni_read_delay( 0 )
{
	insertVariables( type );

	if ( analyse( argc, argv ) != RESULT_OK )
	{
		exit( 0 );
	}

	preDefineCheck();
}

void EsynetConfig::preDefineCheck()
{
	m_vc_share = 0;
	if ( m_e2e_retrans_enable )
	{
		m_e2e_ack_enable = m_e2e_retrans_enable;
	}
}


void EsynetConfig::insertVariables(ConfigType type)
{
	setHead( "Reliability Network-on-Chip Simulator" );
	setVersion( "3.0" );

	if (( type & CONFIG_NETWORK_CFG ) > 0 )
	{
		insertBool( "-network_cfg_file_enable", 
			"enable network configuration file", 
			&m_network_cfg_file_enable );
		insertOpenFile( "-router_cfg_file_name", 
			"router configuration file name", 
			&m_network_cfg_file_name, "-network_cfg_file_enable", 1,
			NETCFG_EXTENSION );
		insertLong( "-router_num", 
			"Total number of routers",
			&m_router_num, "-!network_cfg_file_enable", 1 );
		insertLongVector( "-array_size", 
			"k-ary network", 
			&m_ary_number, "-!network_cfg_file_enable", 1 );
		insertLong( "-phy_number", 
			"number of physical port of a router", 
			&m_physical_port_number, "-!network_cfg_file_enable", 1 );
		insertLong( "-vc_number", 
			"VC per physical port", 
			&m_virtual_channel_number, "-!network_cfg_file_enable", 1 );
		insertLong( "-in_buffer_size", 
			"buffer size of each VC", 
			&m_inbuffer_size, "-!network_cfg_file_enable", 1 );
		insertLong( "-out_buffer_size", 
			"output buffer size", 
			&m_outbuffer_size, "-!network_cfg_file_enable", 1 );
	}

	insertLong( "-data_path_width", 
		"width of data path, i.e. num. of bits", 
		&m_data_width );
	insertDouble( "-link_length", 
		"link length #num", 
		&m_link_length );
	m_routing_alg.addOption(RA_XY, "XY");
	m_routing_alg.addOption(RA_TXY, "TXY");
	m_routing_alg.addOption(RA_DYXY, "DyXY");
	m_routing_alg.addOption(RA_FTLR, "FTLR");
	m_routing_alg.addOption(RA_HIPFAR, "HiPFaR");
	m_routing_alg.addOption(RA_CORERESCUER, "CoreRescuer");
	m_routing_alg.addOption(RA_ERESCUER, "ERescuer");
	m_routing_alg.addOption(RA_ALTERTEST, "AlterTest");
	m_routing_alg.addOption(RA_NONBLOCK, "NonBlock");
    m_routing_alg.addOption(RA_FREESLOT, "FreeSlot");
	insertEnum("-routing_alg",
		"Code of chosen routing algorithm", 
		&m_routing_alg );
	insertLong( "-random_seed", 
		"random seed used for generate random number", 
		&m_ran_seed );
	insertDouble( "-simulation_period", 
		"simulation period, #cycle", 
		&m_simulation_period );
	
	insertLong( "-ni_buffer_size", "Buffer size of NI, #unit", 
		&m_ni_buffer_size );
	insertLong( "-ni_read_delay", "Read delay of NI, #cycle",
		&m_ni_read_delay );

	if ( ( type & CONFIG_SIM_LENGTH ) > 0 )
	{
		insertDouble( "-sim_length", 
			"simulation length, i.e. 10,000,000 cycles", 
			&m_sim_length );
	}
	
	if ( ( type & CONFIG_MAX_HOP ) > 0 )
	{
		m_sim_length = 20;
		insertDouble( "-max_hop", 
			"maximun hop count, i.e. 20 hops", 
			&m_sim_length );
	}

	if ( ( type & CONFIG_TRAFFIC_INJECTION ) > 0 )
	{
		insertBool( "-traffic_injection_disable", 
			"disable traffic injection",
			&m_traffic_injection_disable );
		insertBool( "-input_trace_enable", 
			"input trace file is used instead of PE", 
			&m_input_trace_enable, "-!traffic_injection_disable", 1 );
		insertBool( "-input_trace_file_text_enable", 
			"input trace file is text format", 
			&m_input_trace_text_enable, 
			"-!traffic_injection_disable -input_trace_enable", 1 );
		insertLong( "-input_trace_buffer_size", 
			"buffer size of input trace interface", 
			&m_input_trace_buffer_size, 
			"-!traffic_injection_disable -input_trace_enable", 1 );
		insertOpenFileIF( "-input_trace_file_name", 
			"including information about packet injection", 
			&m_input_trace_file_name, 
			"-!traffic_injection_disable -input_trace_enable", 1, 
			BENCHMARK_EXTENSION );
		m_traffic_rule.addOption(TP_UNIFORM, "Uniform");
		m_traffic_rule.addOption(TP_TRANSPOSE1, "Transpose1");
		m_traffic_rule.addOption(TP_TRANSPOSE2, "Transpose2");
		m_traffic_rule.addOption(TP_BITREVERSAL, "Bitreversal");
		m_traffic_rule.addOption(TP_BUTTERFLY, "Butterfly");
		m_traffic_rule.addOption(TP_SHUFFLE, "Shuffle");
		insertEnum("-traffic_rule",
			"rule to generate trace", 
			&m_traffic_rule, 
			"-!traffic_injection_disable -!input_trace_enable", 1 );
		insertDouble( "-traffic_pir", "packet inject rate (packet/cycle/node)", 
			&m_traffic_pir, 
			"-!traffic_injection_disable -!input_trace_enable", 1 );
		insertLong( "-packet_size", "number of flits in a packet", 
			&m_packet_size, 
			"-!traffic_injection_disable -!input_trace_enable", 1 );
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
		insertBool( "-output_trace_enable", 
			"enable output traffic trace",
			&m_output_trace_enable, depend );
		insertBool( "-output_trace_file_text_enable", 
			"output trace file is text format", 
			&m_output_trace_text_enable, 
			depend + "-output_trace_enable", 1 );
		insertLong( "-output_trace_buffer_size", 
			"buffer size of output trace interface", 
			&m_output_trace_buffer_size, 
			depend + "-output_trace_enable", 1 );
		insertOpenFileIF( "-output_trace_file_name", 
			"record information about packet injection", 
			&m_output_trace_file_name, 
			depend + "-output_trace_enable", 1,
			BENCHMARK_EXTENSION );
	}
	
	if ( ( type & CONFIG_PATH_PAIR_FILE ) > 0 )
	{
		insertBool( "-path_pair_file_text_enable", 
			"path pair file is text format", 
			&m_input_trace_text_enable );
		m_input_trace_file_name = "../example/pathpair";
		insertOpenFileIF( "-path_pair_file_name", 
			"path pair file name, including the information about \
			packet injection", 
			&m_input_trace_file_name, "", 0,
			PATHPAIR_EXTENSION );
	}

	if ( ( type & CONFIG_EVENT_TRACE_FILE ) > 0 )
	{
		insertBool( "-event_trace_enable", 
			"enable event trace", 
			&m_event_trace_enable );
		insertBool( "-event_trace_file_text_enable", 
			"event trace file is text format", 
			&m_event_trace_text_enable, "-event_trace_enable", 1 ); 
		insertLong( "-event_trace_buffer_size", 
			"buffer size of event trace interface", 
			&m_event_trace_buffer_size, "-event_trace_enable", 1 );
		insertNewFileIF( "-event_trace_file_name", 
			"event trace file name", 
			&m_event_trace_file_name, "-event_trace_enable", 1,
			EVENTTRACE_EXTENSION );
		insertBool( "-event_trace_cout_enable", 
			"enable event trace to cout", 
			&m_event_trace_cout_enable );
	}
	
	if ( ( type & CONFIG_PATH_EVENT_FILE ) > 0 )
	{
		m_event_trace_file_name = "../example/pathevent";
		insertNewFileIF( "-path_trace_file_name", 
			"path trace file name", 
			&m_event_trace_file_name, "", 0,
			PATHTRACE_EXTENSION );
	}

	if ( ( type & CONFIG_FAULT_INJECTION_FILE ) > 0 )
	{
		insertBool( "-fault_inject_enable", 
			"enable fault injection", 
			&m_fault_inject_enable );
		insertOpenFileIF( "-fault_inject_file_name", 
			"fault injection file name", 
			&m_fault_inject_file_name, "-fault_inject_enable", 1, 
			FAULTCFG_EXTENSION );
		insertStringVector( "-fault_inject_inline",
			"fault injection items, in line format",
			&m_fault_inject_line, "-fault_inject_enable", 1 );
	}

	if ( ( type & CONFIG_ECC ) > 0 )
	{
		insertBool( "-ecc_enable", 
			"Enable Error Correcting Code", 
			&m_ecc_enable );
		insertBool( "-ecc_fip_enable", 
			"Enable Fault Injection of ECC", 
			&m_ecc_fip_enable, "-ecc_enable", 1 );
		m_ecc_name.addOption(ECC_HM74, "HM74");
		m_ecc_name.addOption(ECC_HM128, "HM128");
		m_ecc_name.addOption(ECC_HM2116, "HM2116");
		m_ecc_name.addOption(ECC_HM3832, "HM3832");
		insertEnum("-ecc_name",
			"name of Error Correcting Code", 
			&m_ecc_name, "-ecc_enable", 1 );
		m_ecc_strategy.addOption(ECC_H2H, "H2H");
		m_ecc_strategy.addOption(ECC_E2E, "E2E");
		m_ecc_strategy.addOption(ECC_COUNT, "COUNT");
		m_ecc_strategy.addOption(ECC_CROSS, "CROSS");
		m_ecc_strategy.addOption(ECC_SLOPE, "SLOPE");
		m_ecc_strategy.addOption(ECC_SQUARE, "SQUARE");
		insertEnum("-ecc_strategy",
			"strategy to place Error Correcting Code unit",
			&m_ecc_strategy, "-ecc_enable", 1 );
		insertLong( "-ecc_space", 
			"space for ecc strategy, invalid for E2E or H2H", 
			&m_ecc_space, "-ecc_enable", 1 );
	}

	if ( ( type & CONFIG_SW ) > 0 )
	{
		insertBool( "-sw_enable", 
			"Enable Spare Wire", 
			&m_sw_enable );
		insertBool( "-sw_fip_enable", 
			"Enable Fault Injection of SW", 
			&m_ecc_fip_enable, "-sw_enable", 1 );
		insertLongVector( "-sw_capacity",
			"Capacity of SW. First is the size of group and second is the number of spare wires.",
			&m_sw_capacity, "-sw_enable", 1);
	}

	if ( ( type & CONFIG_E2E ) > 0 )
	{
		insertBool( "-e2e_ack_enable", 
			"enable acknoledge packets", 
			&m_e2e_ack_enable );
		insertBool( "-e2e_retrans_enable", 
			"enable retransmission mechanism", 
			&m_e2e_retrans_enable, "-e2e_ack_enable", 1 );
		insertDouble( "-e2e_retrans_timer_max", 
			"maximum value for retransmission timer", 
			&m_e2e_retrans_timer_max, "-e2e_ack_enable", 1 );
	}

	if ( ( type & CONFIG_BIST ) > 0 )
	{
		insertBool( "-bist_enable", 
			"enable Build-in Self-Test", 
			&m_bist_enable );
		m_bist_type_code.addOption(BIST_TARRA, "TARRA");
		m_bist_type_code.addOption(BIST_BLOCKING, "Blocking");
		m_bist_type_code.addOption(BIST_NONBLOCK, "NonBlock");
		m_bist_type_code.addOption(BIST_FREESLOT, "FreeSlot");
		insertEnum( "-bist_type_code", 
			"BIST type code", 
			&m_bist_type_code, "-bist_enable", 1 );
		insertLong( "-bist_interval", 
			"time between BIST for two routers", 
			&m_bist_interval, "-bist_enable", 1 );
		insertLongVector( "-bist_timing", 
			"timing for Build-in Self-Test", 
			&m_bist_timing, "-bist_enable", 1 );
		insertLongVector( "-bist_flit", 
			"testing flit for Build-in Self-Test to one destination", 
			&m_bist_flit, "-bist_enable", 1 );
		insertLong( "-bist_offset", "offset of testing sequence",
			&m_bist_offset, "-bist_enable", 1 );
	}

	if ( ( type & CONFIG_END_CONDITION ) > 0 )
	{
		insertLong( "-injected_packet", 
			"total number of injected packets", 
			&m_injected_packet );
		insertLong( "-warmup_packet", 
			"packet number in warmup phase", 
			&m_warmup_packet );
		insertLong( "-latency_measure_packet", 
			"packet number for latency measurement", 
			&m_latency_measure_packet );
		insertLong( "-throughput_measure_packet", 
			"packet number for throughput measurement", 
			&m_throughput_measure_packet );
	}
}

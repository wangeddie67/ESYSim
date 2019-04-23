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

#ifndef ESYNET_CONFIG_H
#define ESYNET_CONFIG_H

#include "esy_argument.h"
#include "esynet_global.h"

#include <cstdlib>

using namespace std;
using namespace esynet;

/**
 * @ingroup argumentinterface
 * @brief Arguments group for esynet.
 *
 * Arguments group for esynet. It can register and parse arguments from
 * commands. ESYNet also provides a tool to calculate the path directly, called
 * ESYNetPath. This class also provides arguments for ESYNetPath.
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Type </th>
 *   <th> Meaning </th>
 *   <th> Default </th>
 *   <th> Access </th>
 *   <th> Network/Path </th>
 * </tr>
 * <tr>
 *   <td> long #m_router_num </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Total number of routers. </td>
 *   <td> 64 </td>
 *   <td> #routerNum() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> vector<long> #m_ary_number </td>
 *   <td> ArgumentItem::LONG_VECTOR_TYPE </td>
 *   <td> k-ary network size. </td>
 *   <td> 8,8 </td>
 *   <td> #aryNumber(), #cubeNumber() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> long #m_physical_port_number </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Number of physical port of a router. </td>
 *   <td> 7 </td>
 *   <td> #physicalPortNumber() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> long #m_virtual_channel_number </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Number of VC of a physical port. </td>
 *   <td> 1 </td>
 *   <td> #virtualChannelNumber() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> long #m_inbuffer_size </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Size of input buffer for each VC. </td>
 *   <td> 12 </td>
 *   <td> #inbufferSize() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> long #m_outbuffer_size </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Size of output buffer for physical port. </td>
 *   <td> 12 </td>
 *   <td> #outbufferSize() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> long #m_flit_size </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Size of flit, i.e. num. of 64 bits. </td>
 *   <td> 1 </td>
 *   <td> #flitSize() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> long #m_data_width </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Width of data path, i.e. num. of bits. </td>
 *   <td> 32 </td>
 *   <td> #dataPathWidth() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> double #m_link_length </td>
 *   <td> ArgumentItem::DOUBLE_TYPE </td>
 *   <td> Link length. </td>
 *   <td> 1000 </td>
 *   <td> #linkLength() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> string #m_routing_alg </td>
 *   <td> ArgumentItem::STRING_TYPE </td>
 *   <td> Code of chosen routing algorithm. </td>
 *   <td> "XY" </td>
 *   <td> #routingAlg() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> bool #m_router_cfg_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable router configuration. </td>
 *   <td> false </td>
 *   <td> #routerCfgEnable() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> string #m_router_cfg_file_name </td>
 *   <td> ArgumentItem::OPEN_FILE_TYPE </td>
 *   <td> Direction of router configuration file. </td>
 *   <td> false </td>
 *   <td> #routerCfgEnable() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> long #m_ran_seed </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Random seed used for generate random number. </td>
 *   <td> 1 </td>
 *   <td> #randomSeed() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> double #m_sim_length </td>
 *   <td rowspan="2"> ArgumentItem::DOUBLE_TYPE </td>
 *   <td> Simulation length, i.e. 10,000,000 cycles. </td>
 *   <td> 10000 </td>
 *   <td rowspan="2"> #simLength() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> Maximun hop count, i.e. 20 hops. </td>
 *   <td> 20 </td>
 *   <td> Path </td>
 * </tr>
 * <tr>
 *   <td> double #m_simulation_period </td>
 *   <td> ArgumentItem::DOUBLE_TYPE </td>
 *   <td> Simulation period, i.e. cycle. </td>
 *   <td> 1.0 </td>
 *   <td> #simulationPeriod() </td>
 *   <td> Both </td>
 * </tr>
 * <tr>
 *   <td> bool #m_traffic_trace_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Traffic trace file is used instead of PE. </td>
 *   <td> false </td>
 *   <td> #trafficTraceEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> bool #m_traffic_trace_text_enable </td>
 *   <td rowspan="2"> ArgumentItem::BOOL_TYPE </td>
 *   <td> Input trace file is text format. </td>
 *   <td rowspan="2"> false </td>
 *   <td rowspan="2"> #trafficTraceTextEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> Path pair file is text format. </td>
 *   <td> Path </td>
 * </tr>
 * <tr>
 *   <td> long #m_traffic_trace_buffer_size </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Buffer size of input trace interface. </td>
 *   <td> 10000 </td>
 *   <td> #trafficTraceBufferSize() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> string #m_traffic_trace_file_name </td>
 *   <td rowspan="2"> ArgumentItem::OPEN_FILE_IF_TYPE </td>
 *   <td> Direction of input trace file. </td>
 *   <td rowspan="2"> ../example/benchmark </td>
 *   <td rowspan="2"> #trafficTraceFileName() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> Direction of path pair file. </td>
 *   <td> Path </td>
 * </tr>
 * <tr>
 *   <td> long #m_traffic_rule </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Rule to generate trace. </td>
 *   <td> 0 </td>
 *   <td> #trafficRule() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> double #m_traffic_pir </td>
 *   <td> ArgumentItem::DOUBLE_TYPE </td>
 *   <td> Packet injection rate (packet/cycle/node). </td>
 *   <td> 0.1 </td>
 *   <td> #trafficPir() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_packet_size </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Number of flits in a packet. </td>
 *   <td> 5 </td>
 *   <td> #packetSize() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> bool #m_event_trace_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable event trace. </td>
 *   <td> false </td>
 *   <td> #eventTraceEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> bool #m_event_trace_text_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Event trace file is text format. </td>
 *   <td> false </td>
 *   <td> #eventTraceTextEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_event_trace_buffer_size </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Buffer size of event trace interface. </td>
 *   <td> 10000 </td>
 *   <td> #eventTraceBufferSize() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> string #m_event_trace_file_name </td>
 *   <td rowspan="2"> ArgumentItem::NEW_FILE_IF_TYPE </td>
 *   <td> Direction of event trace file. </td>
 *   <td> ../example/event </td>
 *   <td rowspan="2"> #eventTraceFileName() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> Direction of path trace file. </td>
 *   <td> ../example/pathevent </td>
 *   <td> Path </td>
 * </tr>
 * <tr>
 *   <td> bool #m_fault_inject_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable fault injection. </td>
 *   <td> false </td>
 *   <td> #faultInjectEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> string #m_fault_inject_file_name </td>
 *   <td> ArgumentItem::OPEN_FILE_IF_TYPE </td>
 *   <td> Direction of fault injection file. </td>
 *   <td> ../example/faultinject </td>
 *   <td> #faultInjectFileName() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> vector< string > #m_fault_inject_line </td>
 *   <td> ArgumentItem::STRING_VECTOR_TYPE </td>
 *   <td> Fault injection items in command line. </td>
 *   <td> vector< string >() </td>
 *   <td> #faultInjectLine() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> bool #m_ecc_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable Error Correcting Code. </td>
 *   <td> false </td>
 *   <td> #eccEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> bool #m_ecc_fip_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable Fault Injection of ECC units. </td>
 *   <td> false </td>
 *   <td> #eccEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> string #m_ecc_name </td>
 *   <td> ArgumentItem::STRING_TYPE </td>
 *   <td> Name of Error Correcting Code. </td>
 *   <td> HM74 </td>
 *   <td> #eccName() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> string #m_ecc_strategy </td>
 *   <td> ArgumentItem::STRING_TYPE </td>
 *   <td> Name of strategy to place Error Correcting Code units. </td>
 *   <td> H2H </td>
 *   <td> #eccStrategy() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_ecc_space </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Space for ecc strategy, invalid for E2E or H2H. </td>
 *   <td> 1 </td>
 *   <td> #eccSpace() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> bool #m_e2e_ack_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable acknoledge packets. </td>
 *   <td> false </td>
 *   <td> #e2eAckEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> bool #m_e2e_retrans_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable retransmission mechanism. </td>
 *   <td> false </td>
 *   <td> #e2eRetransEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> double #m_e2e_retrans_timer_max </td>
 *   <td> ArgumentItem::DOUBLE_TYPE </td>
 *   <td> Overflow value for retransmission timer. </td>
 *   <td> #m_sim_length </td>
 *   <td> #e2eRetransTimerMax() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> bool #m_bist_enable </td>
 *   <td> ArgumentItem::BOOL_TYPE </td>
 *   <td> Enable Build-in Self-Test. </td>
 *   <td> false </td>
 *   <td> #bistEnable() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_bist_type_code </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Type of Build-in Self-Test. </td>
 *   <td> 1 (TARRA) </td>
 *   <td> #bistTypeCode() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_bist_interval </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Time between BIST for two routers. </td>
 *   <td> 500 </td>
 *   <td> #bistInterval() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> vector< long > #m_bist_timing </td>
 *   <td> ArgumentItem::LONG_VECTOR_TYPE </td>
 *   <td> Time for Build-in Self-Test. </td>
 *   <td> 500 </td>
 *   <td> #bistTiming() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_injected_packet </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Total number of injected packets. </td>
 *   <td> -1 </td>
 *   <td> #injectedPacket() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_warmup_packet </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Number of packets in warmup phase. </td>
 *   <td> 0 </td>
 *   <td> #warmUpPacket() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_latency_measure_packet </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Number of packets for latency measurement. </td>
 *   <td> -1 </td>
 *   <td> #latencyMeasurePacket() </td>
 *   <td> Network </td>
 * </tr>
 * <tr>
 *   <td> long #m_throughput_measure_packet </td>
 *   <td> ArgumentItem::LONG_TYPE </td>
 *   <td> Number of packets for throughput measurement. </td>
 *   <td> -1 </td>
 *   <td> #throughputMeasurePacket() </td>
 *   <td> Network </td>
 * </tr>
 * </table>
 */
class EsynetConfig : public EsyArgumentList
{
public:
	/*!
		* \brief Type of argument group.
		*/
	enum ConfigType {
		CONFIG_NETWORK_CFG = 0x001L,
		CONFIG_SIM_LENGTH = 0x002L,
		CONFIG_TRAFFIC_INJECTION = 0x004L,
		CONFIG_OUTPUT_TRAFFIC_FILE = 0x008L,
		CONFIG_PATH_PAIR_FILE = 0x010L,
		CONFIG_EVENT_TRACE_FILE = 0x020L,
		CONFIG_PATH_EVENT_FILE = 0x040L,
		CONFIG_FAULT_INJECTION_FILE = 0x080L,
		CONFIG_ECC = 0x100L,
		CONFIG_SW = 0x2000L,
		CONFIG_E2E = 0x200L,
		CONFIG_BIST = 0x400L,
		CONFIG_END_CONDITION = 0x800l,
		CONFIG_MAX_HOP = 0x1000L,
		CONFIG_NETWORK = 
			CONFIG_NETWORK_CFG | CONFIG_SIM_LENGTH | CONFIG_TRAFFIC_INJECTION |
			CONFIG_OUTPUT_TRAFFIC_FILE | CONFIG_EVENT_TRACE_FILE | 
			CONFIG_FAULT_INJECTION_FILE | CONFIG_ECC | CONFIG_SW | CONFIG_E2E |
			CONFIG_BIST | CONFIG_END_CONDITION,
		CONFIG_PATH = 
			CONFIG_NETWORK_CFG | CONFIG_MAX_HOP | CONFIG_PATH_PAIR_FILE |
			CONFIG_PATH_EVENT_FILE | CONFIG_FAULT_INJECTION_FILE,
		CONFIG_INTERFACE = 
			CONFIG_NETWORK_CFG | CONFIG_SIM_LENGTH |
			CONFIG_OUTPUT_TRAFFIC_FILE | CONFIG_EVENT_TRACE_FILE | 
			CONFIG_FAULT_INJECTION_FILE | CONFIG_ECC | CONFIG_SW | CONFIG_E2E |
			CONFIG_BIST,
	};

private:
	/** @name Network configuration */
	///@{
	long m_router_num; /**< \brief total number of router in network */
	vector< long > m_ary_number; /**< \brief size of network in each dimension 
		*/
	long m_physical_port_number; /**< \brief the number of physical ports in
		each router */
	long m_virtual_channel_number; /**< \brief the number of virtual channels
		in each physical port */
	long m_inbuffer_size; /**< \brief input buffer size of each VC */
	long m_outbuffer_size; /**< \brief output buffer size */
	long m_data_width; /**< \brief data path width, bits */
	double m_link_length; /**< \brief link length in um */
	EsyArgumentEnum m_routing_alg; /**< \brief choose one of the routing 
		algorithms 	*/
	long m_vc_share; /**< \brief VC sharing, based on routing algorithms */
	///@}

	/** @name Network configuration file */
	///@{
	bool m_network_cfg_file_enable; /**< \brief enable network configuration */
	string m_network_cfg_file_name; /**< \brief network configuration file name 
		*/
	///@}

	/** @name Simulation configuration and control */
	///@{
	long m_ran_seed; /**< \brief random seed used for generate random number */
	double m_sim_length; /**< \brief simulation length i.e. 10,000,000 cycles */
	double m_simulation_period; /**< \brief simulation period for pipeline
		(unit: cycle) */
	long m_injected_packet; /**< \brief total number of packet inject */
	long m_warmup_packet; /**< \brief warming up packet count */
	long m_latency_measure_packet; /**< \brief the number of packets to measure
		latency */
	long m_throughput_measure_packet; /**< \brief the number of packets to
		measure throughput */
	///@}

	/** @name Traffic injection */
	///@{
	bool m_traffic_injection_disable; /**< \brief traffic injection disable */
	bool m_input_trace_enable; /**< \brief input trace enable */
	bool m_input_trace_text_enable; /**< \brief input trace text enable */
	long m_input_trace_buffer_size; /**< \brief input trace buffer size */
	string m_input_trace_file_name; /**< \brief input trace file name */
	EsyArgumentEnum m_traffic_rule; /**< \brief rule to generate traffic */
	double m_traffic_pir; /**< \brief traffic packet inject rate, range 0 to 1
		packet/router/cycle */
	long m_packet_size; /**< \brief the number of flits in a packet */
	///@}

	/** @name Traffic trace */
	///@{
	bool m_output_trace_enable; /**< \brief output trace enable */
	bool m_output_trace_text_enable; /**< \brief output trace text enable */
	long m_output_trace_buffer_size; /**< \brief output trace buffer size */
	string m_output_trace_file_name; /**< \brief output trace file name */
	///@}

	/** @name Event trace */
	///@{
	bool m_event_trace_enable; /**< \brief event trace enable (false definely) 
		*/
	bool m_event_trace_text_enable; /**< \brief event trace text enable */
	long m_event_trace_buffer_size; /**< \brief event trace buffer size */
	string m_event_trace_file_name; /**< \brief event trace file name */
	bool m_event_trace_cout_enable; /**< \brief event trace cout enable */
	///@}

	/** @name Fault injection */
	///@{
	bool m_fault_inject_enable; /**< \brief fault injection enable flag */
	string m_fault_inject_file_name; /**< \brief fault injection file name */
	vector< string > m_fault_inject_line; /**< \brief fault injection
		configuration in line */
	///@}

	/** @name Error Correction Coding */
	///@{
	bool m_ecc_enable; /**< \brief ecc enable */
	bool m_ecc_fip_enable; /**< \brief ecc fault injection */
	EsyArgumentEnum m_ecc_name; /**< \brief ecc coding name */
	EsyArgumentEnum m_ecc_strategy; /**< \brief ecc location strategy */
	long m_ecc_space;     /**< \brief ecc space name */
	///@}

	/** @name Spare Wire */
	///@{
	bool m_sw_enable; /**< \brief spare wire enable */
	bool m_sw_fip_enable; /**< \brief spare wire fault injection */
	vector< long > m_sw_capacity; /**< \brief capacity of sw. */
	///@}

	/** @name ACK and retransmission */
	///@{
	bool m_e2e_ack_enable; /**< \brief ack enable */
	bool m_e2e_retrans_enable; /**< \brief retransmission enable */
	double m_e2e_retrans_timer_max; /**< \brief maximum value of retransmission
		timer */
	///@}

	/** @name Build-in Self-Test */
	///@{
	bool m_bist_enable; /**< \brief period bist */
	EsyArgumentEnum m_bist_type_code; /**< \brief period type code */
	long m_bist_interval; /**< \brief bist period duration */
	vector< long > m_bist_timing; /**< \brief router bist duration */
	vector< long > m_bist_flit; /**< \brief injected testing flit for bist */
	long m_bist_offset; /**< \brief offset of testing sequence */
	///@}

	/** @name NI buffer size */
	///@{
	long m_ni_buffer_size; /**< \brief size of buffer in ni */
	long m_ni_read_delay; /**< \brief delay of ni read */
	///@}

public:
	/** @name Constructors */
	///@{
	/*!
		* \brief Construct an emptity entity with default value.
		* \param type  type of argument group.
		*/
	EsynetConfig(ConfigType type = CONFIG_NETWORK);
	/*!
		* \brief Construct an entity and fill the value by parsing the commands.
		* \param argc  counter of arguments.
		* \param argv  value list of arguments.
		* \param type  type of argument group.
		*/
	EsynetConfig(int argc, char * const argv[],
		ConfigType type = CONFIG_NETWORK );
	///@}

	/*!
		* \brief Register variables for parsing.
		* \param type  type of argument group.
		*/
	void insertVariables( ConfigType type = CONFIG_NETWORK );

	/** @name Functions to access variables */
	///@{
	/*!
		* \brief Access #m_router_num.
		* \return #m_router_num.
		*/
	long routerNum() const { return m_router_num; }
	/*!
		* \brief Access #m_ary_number.
		* \return #m_ary_number.
		*/
	const vector<long> & aryNumber() const {return m_ary_number;}
	/*!
		* \brief Access number of dimension.
		* \return number of dimension.
		*/
	long cubeNumber() const {return (long) m_ary_number.size();}
	/*!
		* \brief Access #m_physical_port_number.
		* \return #m_physical_port_number.
		*/
	long physicalPortNumber() const { return m_physical_port_number; }
	/*!
		* \brief Access #m_virtual_channel_number.
		* \return #m_virtual_channel_number.
		*/
	long virtualChannelNumber() const { return m_virtual_channel_number; }
	/*!
		* \brief Access #m_inbuffer_size.
		* \return #m_inbuffer_size.
		*/
	long inbufferSize() const { return m_inbuffer_size; }
	/*!
		* \brief Access #m_outbuffer_size.
		* \return #m_outbuffer_size.
		*/
	long outbufferSize() const { return m_outbuffer_size; }
	/*!
		* \brief Access #m_data_width.
		* \return #m_data_width.
		*/
	long dataPathWidth() const { return m_data_width; }
	/*!
		* \brief Access #m_link_length.
		* \return #m_link_length.
		*/
	double linkLength() const { return m_link_length; }
	/*!
		* \brief Access #m_link_length.
		* \return #m_link_length.
		*/
	double simLength() const { return m_sim_length; }
	/*!
		* \brief Access #m_sim_length.
		* \return #m_sim_length.
		*/
	RoutingAlg routingAlg() const { return (RoutingAlg)(long)m_routing_alg; }
	/*!
		* \brief Access #m_vc_share.
		* \return #m_vc_share.
		*/
	long vcShare() const { return m_vc_share; }
	/*!
		* \brief Access #m_traffic_injection_enable.
		* \return #m_traffic_injection_enable.
		*/
	bool trafficInjectDisable() const { return m_traffic_injection_disable; }
	/*!
		* \brief Access #m_input_trace_enable.
		* \return #m_input_trace_enable.
		*/
	bool inputTraceEnable() const { return m_input_trace_enable; }
	/*!
		* \brief Access #m_input_trace_text_enable.
		* \return #m_input_trace_text_enable.
		*/
	bool inputTraceTextEnable() const { return m_input_trace_text_enable; }
	/*!
		* \brief Access #m_input_trace_buffer_size.
		* \return #m_input_trace_buffer_size.
		*/
	long inputTraceBufferSize() const { return m_input_trace_buffer_size; }
	/*!
		* \brief Access #m_input_trace_file_name.
		* \return #m_input_trace_file_name.
		*/
	const string & inputTraceFileName() const 
		{ return m_input_trace_file_name; }
	/*!
		* \brief Access #m_output_trace_enable.
		* \return #m_output_trace_enable.
		*/
	bool outputTraceEnable() const { return m_output_trace_enable; }
	/*!
		* \brief Access #m_output_trace_text_enable.
		* \return #m_output_trace_text_enable.
		*/
	bool outputTraceTextEnable() const { return m_output_trace_text_enable; }
	/*!
		* \brief Access #m_output_trace_buffer_size.
		* \return #m_output_trace_buffer_size.
		*/
	long outputTraceBufferSize() const { return m_output_trace_buffer_size; }
	/*!
		* \brief Access #m_output_trace_file_name.
		* \return #m_output_trace_file_name.
		*/
	const string & outputTraceFileName() const 
		{ return m_output_trace_file_name; }
	/*!
		* \brief Access #m_event_trace_text_enable.
		* \return #m_event_trace_text_enable.
		*/
	bool eventTraceTextEnable() const { return m_event_trace_text_enable; }
	/*!
		* \brief Access #m_event_trace_buffer_size.
		* \return #m_event_trace_buffer_size.
		*/
	long eventTraceBufferSize() const { return m_event_trace_buffer_size; }
	/*!
		* \brief Access #m_event_trace_file_name.
		* \return #m_event_trace_file_name.
		*/
	const string &  eventTraceFileName() const 
		{ return m_event_trace_file_name; }
	/*!
		* \brief Access #m_event_trace_cout_enable.
		* \return #m_event_trace_cout_enable.
		*/
	bool  eventTraceCoutEnable() const 
		{ return m_event_trace_cout_enable; }
	/*!
		* \brief Access #m_event_trace_enable.
		* \return #m_event_trace_enable.
		*/
	bool eventTraceEnable() const { return m_event_trace_enable; }
	/*!
		* \brief Access #m_simulation_period.
		* \return #m_simulation_period.
		*/
	double simulationPeriod() const { return m_simulation_period; }
	/*!
		* \brief Access #m_router_cfg_enable.
		* \return #m_router_cfg_enable.
		*/
	bool networkCfgFileEnable() const { return m_network_cfg_file_enable; }
	/*!
		* \brief Access #m_router_cfg_file_name.
		* \return #m_router_cfg_file_name.
		*/
	const string & networkCfgFileName() const 
		{ return m_network_cfg_file_name; }
	/*!
		* \brief Access #m_traffic_rule.
		* \return #m_traffic_rule.
		*/
	TrafficProfile trafficRule() const 
		{ return (TrafficProfile)(long)m_traffic_rule; }
	/*!
		* \brief Access #m_traffic_pir.
		* \return #m_traffic_pir.
		*/
	double trafficPir() const { return m_traffic_pir; }
	/*!
		* \brief Access #m_packet_size.
		* \return #m_packet_size.
		*/
	long packetSize() const { return m_packet_size; }
	/*!
		* \brief Access #m_warmup_packet.
		* \return #m_warmup_packet.
		*/
	long injectedPacket() const { return m_injected_packet; }
	/*!
		* \brief Access #m_warmup_packet.
		* \return #m_warmup_packet.
		*/
	long warmUpPacket() const { return m_warmup_packet; }
	/*!
		* \brief Access #m_latency_measure_packet.
		* \return #m_latency_measure_packet.
		*/
	long latencyMeasurePacket() const { return m_latency_measure_packet; }
	/*!
		* \brief Access #m_throughput_measure_packet.
		* \return #m_throughput_measure_packet.
		*/
	long throughputMeasurePacket() const { return m_throughput_measure_packet; }
	/*!
		* \brief Access #m_ran_seed.
		* \return #m_ran_seed.
		*/
	long randomSeed() const { return m_ran_seed; }
	/*!
		* \brief Access #m_e2e_ack_enable.
		* \return #m_e2e_ack_enable.
		*/
	bool e2eAckEnable() const { return m_e2e_ack_enable; }
	/*!
		* \brief Access #m_e2e_retrans_enable.
		* \return #m_e2e_retrans_enable.
		*/
	bool e2eRetransEnable() const { return m_e2e_retrans_enable; }
	/*!
		* \brief Access #m_e2e_retrans_timer_max.
		* \return #m_e2e_retrans_timer_max.
		*/
	double e2eRetransTimerMax() const { return m_e2e_retrans_timer_max; }
	/*!
		* \brief Access #m_bist_enable.
		* \return #m_bist_enable.
		*/
	bool bistEnable() const { return m_bist_enable; }
	/*!
		* \brief Access #m_bist_type_code.
		* \return #m_bist_type_code.
		*/
	BISTMethod bistTypeCode() const 
		{ return (BISTMethod)(long)m_bist_type_code; }
	/*!
		* \brief Access #m_bist_interval.
		* \return #m_bist_interval.
		*/
	long bistInterval() const { return m_bist_interval; }
	/*!
		* \brief Access the first item of #m_bist_timing.
		* \return #m_bist_timing[0].
		*/
	long bistDuration() const { return m_bist_timing[ 0 ]; }
	/*!
		* \brief Access #m_bist_timing.
		* \return #m_bist_timing.
		*/
	const vector< long > & bistTiming() const { return m_bist_timing; }
	/*!
		* \brief Access #m_bist_flit.
		* \return #m_bist_flit.
		*/
	const vector< long > & bistFlit() const { return m_bist_flit; }
	/*!
		* \brief Access #m_bist_offset.
		* \return #m_bist_offset.
	*/
	long bistOffset() const { return m_bist_offset; }
	/*!
		* \brief Access #m_fault_inject_enable.
		* \return #m_fault_inject_enable.
		*/
	bool faultInjectEnable() const { return m_fault_inject_enable; }
	/*!
		* \brief Access #m_fault_inject_file_name.
		* \return #m_fault_inject_file_name.
		*/
	const string & faultInjectFileName() const 
		{ return m_fault_inject_file_name; }
	/*!
		* \brief Access #m_fault_inject_line.
		* \return #m_fault_inject_line.
		*/
	const vector< string > & faultInjectLine() const 
		{ return m_fault_inject_line; }
	/*!
		* \brief Access #m_ecc_enable.
		* \return #m_ecc_enable.
		*/
	bool eccEnable() const { return m_ecc_enable; }
	/*!
		* \brief Access #m_ecc_fip_enable.
		* \return #m_ecc_fip_enable.
		*/
	bool eccFipEnable() const { return m_ecc_fip_enable; }
	/*!
		* \brief Access #m_ecc_name.
		* \return #m_ecc_name.
		*/
	ECCMethod eccName() const { return (ECCMethod)(long)m_ecc_name; }
	/*!
		* \brief Access #m_ecc_strategy.
		* \return #m_ecc_strategy.
		*/
	ECCStrategy eccStrategy() const 
		{ return (ECCStrategy)(long)m_ecc_strategy; }
	/*!
		* \brief Access #m_ecc_space.
		* \return #m_ecc_space.
		*/
	long eccSpace() const { return m_ecc_space; }
	
	long niBufferSize() const { return m_ni_buffer_size; }
	long niReadDelay() const { return m_ni_read_delay; }
	
	bool swEnable() const { return m_sw_enable; }
	bool swFipEnable() const { return m_sw_fip_enable; }
	const vector< long > swCapacity() const { return m_sw_capacity; }
	///@}
	
	void preDefineCheck();
};

#endif

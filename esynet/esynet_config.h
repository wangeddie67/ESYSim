/*
 * File name : esynet_config.h
 * Function : Declaire command line option parser.
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
 * @file esynet_config.h
 * @brief Define options for ESYNet.
 */

#ifndef ESYNET_CONFIG_H
#define ESYNET_CONFIG_H

#include "esy_argument.h"
#include "esy_networkcfg.h"
#include "esynet_global.h"

/**
 * @ingroup ESYNET_OPTIONS
 * @brief Command line option parser.
 */
class EsynetConfig : public EsyArgumentList
{
public:
    /**
     * @brief Enable option groups
     */
    enum ConfigType {
        CONFIG_NETWORK_CFG = 0x001L,            /*!< @brief Network configuration options. */
        CONFIG_SIM_CONTROL = 0x002L,            /*!< @brief Simulation control. */
        CONFIG_SIM_LENGTH = 0x080L,             /*!< @brief Simulation length. Not used if interface. */
        CONFIG_TRAFFIC_INJECTION = 0x004L,      /*!< @brief Traffic generation options. Not used if interface. */
        CONFIG_OUTPUT_TRAFFIC_FILE = 0x008L,    /*!< @brief Output benchmark options. */
        CONFIG_EVENT_TRACE_FILE = 0x020L,       /*!< @brief Event trace options. */
        CONFIG_NETWORK = CONFIG_NETWORK_CFG | CONFIG_SIM_CONTROL | CONFIG_SIM_LENGTH
            | CONFIG_TRAFFIC_INJECTION | CONFIG_OUTPUT_TRAFFIC_FILE | CONFIG_EVENT_TRACE_FILE, /*!< @brief Default combination for ESYNet. */
        CONFIG_INTERFACE = CONFIG_NETWORK_CFG | CONFIG_SIM_CONTROL
            | CONFIG_OUTPUT_TRAFFIC_FILE | CONFIG_EVENT_TRACE_FILE,  /*!< @brief Default combination for integration APIs. */
    };

private:
    /**
     * @name Network configuration options
     * @{
     */
    EsyArgumentEnum m_topology;             /**< @brief Network topology. */
    std::vector< long > m_network_size;       /**< @brief Size of network in each dimension. */
    long m_physical_port_number;            /**< @brief The number of physical ports in each router. */
    long m_virtual_channel_number;          /**< @brief The number of virtual channels in each physical port. */
    long m_in_buffer_size;                  /**< @brief Input buffer size of each virtual channel. */
    long m_out_buffer_size;                 /**< @brief Output buffer size of each virtual channel. */
    long m_data_width;                      /**< @brief Data path width, bits */
    double m_link_length;                   /**< @brief Link length in um */
    EsyArgumentEnum m_routing_alg;          /**< @brief Routing algorithms. */
    std::string m_routing_table;            /**< @brief Routing table file name. */
    EsyArgumentEnum m_arbiter;              /**< @brief Arbiter type. */
    EsyArgumentEnum m_switch;               /**< @brief Flow control method. */
    long m_ni_buffer_size;                  /**< @brief Size of buffer in ni. */
    long m_ni_read_delay;                   /**< @brief Delay of ni read. */
    bool m_network_cfg_file_enable;         /**< @brief Enable network configuration. */
    bool m_network_cfg_file_out_enable;     /**< @brief Enable output network configuration. */
    std::string m_network_cfg_file_name;    /**< @brief Network configuration file name. */
    /**
     * @}
     */

    /**
     * @name Simulation configuration and control options
     * @{
     */
    long m_rand_seed;                   /**< @brief Random seed used for generate random number */
    double m_sim_length;                /**< @brief Simulation length i.e. 10,000,000 cycles */
    double m_simulation_period;         /**< @brief Simulation period for pipeline (unit: cycle) */
    long m_injected_packet;             /**< @brief Total number of packet inject */
    long m_warmup_packet;               /**< @brief Warming up packet count */
    long m_latency_measure_packet;      /**< @brief The number of packets to measure latency */
    long m_throughput_measure_packet;   /**< @brief The number of packets to measure throughput */
    /**
     * @}
     */

    /** 
     * @name Traffic injection options
     * @{
     */
    bool m_traffic_injection_disable;       /**< @brief traffic injection disable */
    bool m_input_trace_enable;              /**< @brief input trace enable */
    bool m_input_trace_text_enable;         /**< @brief input trace text enable */
    long m_input_trace_buffer_size;         /**< @brief input trace buffer size */
    std::string m_input_trace_file_name;    /**< @brief input trace file name */
    EsyArgumentEnum m_traffic_rule;         /**< @brief rule to generate traffic */
    double m_traffic_pir;                   /**< @brief traffic packet inject rate, range 0 to 1 packet/router/cycle */
    long m_packet_size;                     /**< @brief the number of flits in a packet */
    /**
     * @}
     */

    /**
     * @name Output traffic benchmark options
     * @{
     */
    bool m_output_trace_enable;             /**< @brief Output benchmark enable */
    bool m_output_trace_text_enable;        /**< @brief Output benchmark text enable */
    long m_output_trace_buffer_size;        /**< @brief Output benchmark buffer size */
    std::string m_output_trace_file_name;   /**< @brief Output benchmark file name */
    /**
     * @}
     */

    /**
     * @name Event trace options
     * @{
     */
    bool m_event_trace_enable;              /**< @brief Event trace enable (false definely) */
    bool m_event_trace_text_enable;         /**< @brief Event trace text enable */
    long m_event_trace_buffer_size;         /**< @brief Event trace buffer size */
    std::string m_event_trace_file_name;    /**< @brief Event trace file name */
    bool m_event_trace_cout_enable;         /**< @brief Event trace cout enable */
    /**
     * @}
     */

public:
    /**
     * @name Construction function
     * @{
     */
    /**
     * @brief Construct an emptity entity with default value.
     * @param type  type of argument group.
     */
    EsynetConfig(ConfigType type = CONFIG_NETWORK);
    /**
     * @brief Construct an entity and fill the value by parsing the commands.
     * @param argc  counter of arguments.
     * @param argv  value list of arguments.
     * @param type  type of argument group.
     */
    EsynetConfig(int argc, char * const argv[], ConfigType type = CONFIG_NETWORK );
    /**
     * @}
     */

    /**
     * @brief Register variables for parsing.
     * @param type  type of argument group.
     */
    void insertVariables( ConfigType type = CONFIG_NETWORK );

    /**
     * @name Functions to access network configuration options
     * @{
     */
    /**
     * @brief Return enumerate of network topology.
     */
    inline EsyNetworkCfg::NoCTopology topology() const { return ( EsyNetworkCfg::NoCTopology )( long )m_topology; }
    /**
     * @brief Return the size of the network in diamensions.
     */
    inline const std::vector<long> & networkSize() const { return m_network_size; }
    /**
     * @brief Return the number of diamension.
     */
    inline long cubeNumber() const {return (long) m_network_size.size();}
    /**
     * @brief Return the number of physical ports for one router.
     */
    inline long physicalPortNumber() const { return m_physical_port_number; }
    /**
     * @brief Return the number of virtual channels of one physical port.
     */
    inline long virtualChannelNumber() const { return m_virtual_channel_number; }
    /**
     * @brief Return the size of the input buffer for one virtual channel.
     */
    inline long inbufferSize() const { return m_in_buffer_size; }
    /**
     * @brief Return the size of the output buffer for one virtual channel.
     */
    inline long outbufferSize() const { return m_out_buffer_size; }
    /**
     * @brief Return the width of data path in bit.
     */
    inline long dataPathWidth() const { return m_data_width; }
    /**
     * @brief Return the length of link in um.
     */
    inline double linkLength() const { return m_link_length; }
    /**
     * @brief Return enumerate for routing algorithm.
     */
    inline esynet::EsynetRoutingAlg routingAlg() const { return ( esynet::EsynetRoutingAlg )( long )m_routing_alg; }
    /**
     * @brief Return routing table file name.
     */
    inline const std::string & routingTable() const { return m_routing_table; }
    /**
     * @brief Return enumerate for arbiter.
     */
    inline esynet::EsynetArbiterType arbiter() const { return ( esynet::EsynetArbiterType )( long )m_arbiter; }
    /**
     * @brief Return enumerate for flow control method.
     */
    inline esynet::EsynetSwitchType switchMethod() const { return ( esynet::EsynetSwitchType )( long )m_switch; }
    /**
     * @brief Return size of NI buffer.
     */
    inline long niBufferSize() const { return m_ni_buffer_size; }
    /**
     * @brief Delay before NI read out data.
     */
    inline long niReadDelay() const { return m_ni_read_delay; }
    /**
     * @brief Return TRUE if network configuration is generated.
     */
    inline bool networkCfgOutputFileEnable() const { return m_network_cfg_file_out_enable; }
    /**
     * @brief Return TRUE if network configuration is used.
     */
    inline bool networkCfgFileEnable() const { return m_network_cfg_file_enable; }
    /**
     * @brief Return file name of network configuration file.
     */
    inline const std::string & networkCfgFileName() const { return m_network_cfg_file_name; }
    /**
     * @}
     */

    /**
     * @name Functions to access simulation configuration and control options
     * @{
     */
    /**
     * @brief Return the random seed.
     */
    inline long randomSeed() const { return m_rand_seed; }
    /**
     * @brief Retuen the simulation length in cycle.
     */
    inline double simLength() const { return m_sim_length; }
    /**
     * @brief Return the simulation period.
     */
    inline double simulationPeriod() const { return m_simulation_period; }
    /**
     * @brief Return the number of total injected packets.
     */
    inline long injectedPacket() const { return m_injected_packet; }
    /**
     * @brief Return the number of packets for warming up phase.
     */
    inline long warmUpPacket() const { return m_warmup_packet; }
    /**
     * @brief Return the number of packets for latency measurement.
     */
    inline long latencyMeasurePacket() const { return m_latency_measure_packet; }
    /**
     * @brief Return the number of packets for throughput measurement.
     */
    inline long throughputMeasurePacket() const { return m_throughput_measure_packet; }
    /**
     * @}
     */

    /**
     * @name Functions to access event trace options
     * @{
     */
    /**
     * @brief Return True if disable traffic injection by simulator.
     */
    inline bool trafficInjectDisable() const { return m_traffic_injection_disable; }
    /**
     * @brief Return True if enable traffic from input benchmark.
     */
    inline bool inputTraceEnable() const { return m_input_trace_enable; }
    /**
     * @brief Return True if input benchmark file is text format.
     */
    inline bool inputTraceTextEnable() const { return m_input_trace_text_enable; }
    /**
     * @brief Return the buffer size for input benchmark interface.
     */
    inline long inputTraceBufferSize() const { return m_input_trace_buffer_size; }
    /**
     * @brief Return the file name of input benchmark.
     */
    inline const string & inputTraceFileName() const { return m_input_trace_file_name; }
    /**
     * @brief Return the traffic rule.
     */
    inline esynet::EsynetTrafficProfile trafficRule() const { return ( esynet::EsynetTrafficProfile )( long )m_traffic_rule; }
    /**
     * @brief Return packet injection rate (packet/cycle/router), ranging from 0 to 1.
     */
    inline double trafficPir() const { return m_traffic_pir; }
    /**
     * @brief Return size of packet.
     */
    inline long packetSize() const { return m_packet_size; }
    /**
     * @}
     */

    /**
     * @name Functions to access output traffic benchmark options
     * @{
     */
    /**
     * @brief Return True if enable the output benchmark.
     */
    inline bool outputTraceEnable() const { return m_output_trace_enable; }
    /**
     * @brief Return True if the output benchmark file is text format.
     */
    inline bool outputTraceTextEnable() const { return m_output_trace_text_enable; }
    /**
     * @brief Return buffer size of output benchmark interface.
     */
    inline long outputTraceBufferSize() const { return m_output_trace_buffer_size; }
    /**
     * @brief Return file name of output benchmark interface.
     */
    inline const string & outputTraceFileName() const { return m_output_trace_file_name; }
    /**
     * @}
     */

    /**
     * @name Functions to access network configuration options
     * @{
     */
    /**
     * @brief Return True if enable output event trace.
     */
    inline bool eventTraceEnable() const { return m_event_trace_enable; }
    /**
     * @brief Return True if output event trace file in text format.
     */
    inline bool eventTraceTextEnable() const { return m_event_trace_text_enable; }
    /**
     * @brief Return buffer size of output event trace interface.
     */
    inline long eventTraceBufferSize() const { return m_event_trace_buffer_size; }
    /**
     * @brief Return the file name of output event trace.
     */
    inline const string &  eventTraceFileName() const { return m_event_trace_file_name; }
    /**
     * @brief Return True if enable print event trace to standard output.
     */
    inline bool eventTraceCoutEnable() const { return m_event_trace_cout_enable; }
    /**
     * @}
     */

    /**
     * @brief Options check by predefined rules. Function prints error information.
     * @return True if the options pass check rules.
     */
    bool preDefineCheck();
};

#endif

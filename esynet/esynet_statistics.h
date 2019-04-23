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

#ifndef ESYNET_STATISTICS_H
#define ESYNET_STATISTICS_H

using namespace std;

/** @defgroup statisticunit Statistic Units */

/**
 * @ingroup statisticunit
 * @brief  Structure for statistics variable for a router.
 */
class EsynetRouterStatistic
{
protected:
    /** @name Variables for BIST for TARRA
     *  @brief In TARRA, the router should be emptied before test and recover
     * after the test. These variables check the average time for that. */
    ///@{
    long m_bist_empty_counter; /**< \brief counter of empty phase. */
    double m_bist_empty_cycle; /**< \brief cycles of empty phase. */
    long m_bist_recover_counter; /**< \brief counter of recover phase. */
    double m_bist_recover_cycle; /**< \brief cycles of recover phase. */
    ///@}

public:
    /** @name Constructor  */
    ///@{
    /*!
     * \brief Create an empty structure.
     */
	EsynetRouterStatistic();
    /*!
     * \brief Create a new structure by copying from exist structure.
     * \param t  structure to copy.
     */
	EsynetRouterStatistic(const EsynetRouterStatistic & t);
    ///@}

    /** @name Functions to access variables  */
    ///@{
    /*!
     * \brief Access the counter of empty phase for BIST #m_bist_empty_counter.
     * \return the counter of empty phase for BIST #m_bist_empty_counter.
     */
    long bistEmptyCounter() const { return m_bist_empty_counter; }
    /*!
     * \brief Increase the counter of empty phase for BIST #m_bist_empty_counter
     * by 1.
     */
    void increaseBistEmptyCounter() { m_bist_empty_counter ++; }
    /*!
     * \brief Access the cycles of empty phase for BIST #m_bist_empty_counter.
     * \return the cycles of empty phase for BIST #m_bist_empty_counter.
     */
    double bistEmptyCycle() const { return m_bist_empty_cycle; }
    /*!
     * \brief Increase the cycle of empty phase for BIST #m_bist_empty_counter
     * by the cycle for one empty phase.
     * \param a  the cycle for one empty phase.
     */
    void increaseBistEmptyCycle( double a ) { m_bist_empty_cycle += a; }
    /*!
     * \brief Access the counter of recover phase for BIST
     * #m_bist_recover_counter.
     * \return the counter of recover phase for BIST #m_bist_recover_counter.
     */
    long bistRecoverCounter() const { return m_bist_recover_counter; }
    /*!
     * \brief Increase the counter of recover phase for BIST
     * #m_bist_recover_counter by 1.
     */
    void increaseBistRecoverCounter() { m_bist_recover_counter ++; }
    /*!
     * \brief Access the cycles of recover phase for BIST #m_bist_recover_cycle.
     * \return the cycles of recover phase for BIST #m_bist_recover_cycle.
     */
    double bistRecoverCycle() const { return m_bist_recover_cycle; }
    /*!
     * \brief Increase the cycle of recover phase for BIST #m_bist_recover_cycle
     * by the cycle for one recover phase.
     * \param a  the cycle for one recover phase.
     */
    void increaseBistRecoverCycle( double a ) { m_bist_recover_cycle += a; }
    ///@}

    /*!
     * \brief Add the value of fields from exist structure.
     * \param a  structure to add.
     */
	void add(const EsynetRouterStatistic& a);
};

/**
 * @ingroup statisticunit
 * @brief  Structure for statistics variable for a process element.
 */
class EsynetNIStatistic
{
protected:
    /** @name Variables for packets
     *  @brief Statistic latency and throughput of one packet without ack and
     * retransmission */
    ///@{
    long m_injected_packet; /**< \brief counter of injected packets */
    double m_inject_start_time;
	double m_inject_stop_time;
    
    long m_accepted_packet; /**< \brief counter of accepted packets */
    double m_accept_start_time;	/**< \brief time of first accept packet */
    double m_accept_stop_time;	/**< \brief time of last accept packet */

    double m_total_delay; /**< \brief delay of accepted packet */
    double m_max_delay; /**< \brief max delay of accepted packet */
    ///@}

    /** @name Variables for acknowledge and retransmission packets
     *  @brief Statistic latency and throughput of one packet considering ack
     * and retransmission */
    ///@{
    long m_inject_ack_packet; /**< \brief counter of injected ACK packets */
    long m_accept_ack_packet; /**< \brief counter of accepted ACK packets */
    long m_retransmission_packet; /**< \brief counter of retransmission
        packets */
    ///@}

    /** @name Variables for drop packets */
    ///@{
    long m_drop_flit; /**< \brief counter of dropped flit */
    long m_nondrop_packet; /**< \brief counter of non-dropped packet */
    long m_nondrop_ack_packet; /**< \brief counter of non-dropped ACK packet */
    ///@}

public:
    /** @name Constructor  */
    ///@{
    /*!
     * \brief Create an empty structure.
     */
	EsynetNIStatistic();
    /*!
     * \brief Create a new structure by copying from exist structure.
     * \param t  structure to copy.
     */
	EsynetNIStatistic(const EsynetNIStatistic & t);
    ///@}

    /** @name Functions to access variables  */
    ///@{
    long injectedPacket() const { return m_injected_packet; }
    long injectStartTime() const { return m_inject_start_time; }
    long injectStopTime() const { return m_inject_stop_time; }
    
    long acceptedPacket() const { return m_accepted_packet; }
    double acceptStartTime() const { return m_accept_start_time; }
    double acceptStopTime() const { return m_accept_stop_time; }
    
    double totalDelay() const { return m_total_delay; }
    double maxDelay() const { return m_max_delay; }

    long injectAckPacket() const { return m_inject_ack_packet; }
    long acceptAckPacket() const { return m_accept_ack_packet; }
    long retransmissionPacket() const { return m_retransmission_packet; }
    
    long dropFlit() const { return m_drop_flit; }
    long nonDropPacket() const { return m_nondrop_packet; }
    long nonDropAckPacket() const { return m_nondrop_ack_packet; }
    ///@}
        
    /** @name Functions to update variables  */
    ///@{
    void incInjectPacket(double time) 
	{ 
		m_injected_packet ++; 
		if (m_inject_start_time < 0.0)
		{
			m_inject_start_time = time;
		}
		m_inject_stop_time = time;
	}
	void incAcceptPacket(double time, double delay)
	{
		m_accepted_packet ++;
		if (m_accept_start_time < 0.0)
		{
			m_accept_start_time = time;
		}
		m_accept_stop_time = time;
		m_total_delay += delay;
		if (delay > m_max_delay)
		{
			m_max_delay = delay;
		}
	}
    void incInjectAckPacket() { m_inject_ack_packet ++; }
    void incAcceptAckPacket() { m_accept_ack_packet ++; }
    void incDropFlit() { m_drop_flit ++; }
    void incNonDropPacket() { m_nondrop_packet ++; }
    void incNonDropAckPacket() { m_nondrop_ack_packet ++; }
    void incRetransmissionPacket() { m_retransmission_packet ++; }
    ///@}

    /*!
     * \brief Add the value of fields from exist structure.
     * \param a  structure to add.
     */
	void add(const EsynetNIStatistic& a);
};

/**
 * @ingroup statisticunit
 * @brief  Structure for statistics variable for a foundation platform.
 */
class EsynetFoundationStatistic
{
protected:
    /** @name Variables for injected packets and measurement control */
    ///@{
    long m_injected_packet;     /**< \brief counter of injected packet */
    double m_inject_start_time; /**< \brief time of first injected packet */
    double m_inject_stop_time;  /**< \brief time of last injected packet */

    long m_accepted_packet;
	double m_accept_start_time;
	double m_accept_stop_time;
	
	double m_e2e_total_delay;
	double m_e2e_max_delay;

	
    long m_accept_mark_packet; /**< \brief counter of accepted packets with
        marked for latency measurement. */
    double m_total_mark_delay; /**< \brief delay of accepted packets with
        marked for latency measurement. */

    long m_throughput_measure_start_packet; /**< \brief counter of injected
        packet before throughput measurement start */
    long m_throughput_measure_stop_packet;  /**< \brief counter of injected
        packet before throughput measurement finished */
    double m_throughput_measure_start_time; /**< \brief time of throughput
        measurement start */
    double m_throughput_measure_stop_time;  /**< \brief time of throughput
        measurement finished */
    ///@}

public:
    /** @name Constructor  */
    ///@{
    /*!
     * \brief Create an empty structure.
     */
	EsynetFoundationStatistic();
    /*!
     * \brief Create a new structure by copying from exist structure.
     * \param t  structure to copy.
     */
	EsynetFoundationStatistic(const EsynetFoundationStatistic & t);
    ///@}

    /** @name Functions to access variables  */
    ///@{
    long injectedPacket() const { return m_injected_packet; }
    double injectStartTime() const { return m_inject_start_time; }
    double injectStopTime() const { return m_inject_stop_time; }
    
    long acceptedPacket() const { return m_accepted_packet; }
    double acceptStartTime() const { return m_accept_start_time; }
    double acceptStopTime() const { return m_accept_stop_time; }
    
    double totalE2EDelay() const { return m_e2e_total_delay; }
    double maxE2EDelay() const { return m_e2e_max_delay; }
    
    long acceptMarkPacket() const { return m_accept_mark_packet; }
    double totalMarkDelay() const { return m_total_mark_delay; }
    
    long throughputMeasureStartPacket() const
        { return m_throughput_measure_start_packet; }
    long throughputMeasureStopPacket() const
        { return m_throughput_measure_stop_packet; }
    double throughputMeasureStartTime() const
        { return m_throughput_measure_start_time; }
    double throughputMeasureStopTime() const
        { return m_throughput_measure_stop_time; }
    ///@}
    
    
    /** @name Functions to update variables  */
    ///@{
    void incInjectPacket(double time) 
	{ 
		m_injected_packet ++; 
		if (m_inject_start_time < 0.0)
		{
			m_inject_start_time = time;
		}
		m_inject_stop_time = time;
	}
	void incAcceptPacket(double time, double delay)
	{
		m_accepted_packet ++;
		if (m_accept_start_time < 0.0)
		{
			m_accept_start_time = time;
		}
		m_accept_stop_time = time;
		m_e2e_total_delay += delay;
		if (delay > m_e2e_max_delay)
		{
			m_e2e_max_delay = delay;
		}
	}
	void incAcceptMarkPacket(double delay)
	{
		m_accept_mark_packet ++;
		m_total_mark_delay += delay;
	}
    
    void setThroughputMeasureStartPacket( long a )
        { m_throughput_measure_start_packet = a; }
    void setThroughputMeasureStopPacket( long a )
        { m_throughput_measure_stop_packet = a; }
    void setThroughputMeasureStartTime( double a )
        { m_throughput_measure_start_time = a; }
    void setThroughputMeasureStopTime( double a )
        { m_throughput_measure_stop_time = a; }
    ///@}
};

#endif

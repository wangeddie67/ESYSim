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

#ifndef ESYNET_FAULT_UNIT_H
#define ESYNET_FAULT_UNIT_H

#include "esy_faultcfg.h"
#include "esynet_global.h"
#include "esynet_random_unit.h"

using namespace std;
using namespace esynet;

/** @defgroup fip Fault Injection Point */

/**
 * @ingroup fip
 * \brief State machine of fault injection point.
 *
 * This class models the behavior of the state machine of one fault injection
 * point. The fip has two state, true or false. True means the fip is faulty and
 * the value of corresponding bit has changed.
 *
 * The state machine can have at least one states. Each state corresponds to
 * one output (true or false). The switch between states is controlled by
 * transfer probability matrix. In each cycle, one random number will generate
 * to determine the next states. The initilization state is 0.
 *
 * If the state machine only has one state, the FIP is living or faulty
 * constantly. If the state machine has two states, one of them is living and
 * another one is faulty. More complex model can be described by more states.
 */
class EsynetFaultStateMachine
{
protected:
    vector< vector< double > > m_state_trans_matrix; /**< \brief transfer
        probability matrix [current status][next status] */
    vector< bool > m_state_out;  /**< \brief outputs of status, true means
        faulty and false means living [status] */
    long m_state;  /**< \brief current status */

public:
    /*!
     * \brief Construct a empty state machine.
     *
     * Set the state machine with constatnt false/living state.
     */
	EsynetFaultStateMachine();
    /*!
     * \brief Configure the outputs list and probability matrix with specified
     * fault configure item.
     *
     * \param faultcfg  specified fault configuration item.
     */
    void configFault( EsyFaultConfigItem & faultcfg );
    /*!
     * \brief Set constant FIP with only one state.
     * \param state constant state.
     */
    void setConstantState( bool state );

    /** @name Functions to access variables */
    ///@{
    /*!
     * \brief Access transfer probability matrix #m_state_trans_matrix.
     * \return #m_state_trans_matrix.
     */
    const vector< vector< double > > & stateTransMatrix() const
        { return m_state_trans_matrix; }
    /*!
     * \brief Access current state #m_state.
     * \return #m_state.
     */
    long currentState() const { return m_state; }
    /*!
     * \brief Set current state #m_state.
     * \param state current state.
     */
    void setCurrentState( long state ) { m_state = state; }
    /*!
     * \brief Access vector of outputs of state #m_state_out.
     * \return #m_state_out.
     */
    const vector< bool > & stateOut() const { return m_state_out; }
    /*!
     * \brief Access the output of current state.
     * \return output of current state.
     */
    bool currentStateOut() const { return m_state_out[ m_state ]; }
    /*!
     * \brief Access the number of state.
     * \return number of state.
     */
    long stateNumber() const { return m_state_out.size(); }
    ///@}

    /*!
     * \brief Calculate next status.
     *
     * The range [0,1) is divided into several segments. Each segment
     * corresponds to next state. The width of segment is the conditional
     * probability from current status to next.
     *
     * One random number is generated among the range [0,1). If the random
     * number lies in the segment corresponding to state b, b is the next
     * state.
     *
     * \return  next status.
     */
    long nextState();
    /*!
     * \brief Calculate the output of next status.
     * \return  the output of next status.
     * \sa #nextState()
     */
    bool nextStateOut() { return m_state_out[ nextState() ]; }
    /*!
     * \brief Check whether the FIP is const living.
     *
     * If the FIP is living constantly, the status of this FIP does not effert
     * the simulation, so that it could be ignored to reduce the workload.
     *
     * \return  If the FIP is living constantly, return TRUE; otherwise, return
     * FALSE.
     */
    bool constLiving() const
        { return ( stateNumber() == 1 ) && ( m_state_out[ 0 ] == false ); }

    /** @name Overload operators */
    ///@{
    /*!
     * \brief operator! : check the output of current state is False/Living.
     * \return  If the output of current state is false, return true.
     */
    bool operator! () const { return !currentStateOut(); }
    /*!
     * \brief operator bool : check the output of current state is True/Faulty.
     * \return  If the output of current state is true, return true.
     */
    operator bool () const { return currentStateOut(); }
    ///@}

    /*!
     * \brief operator << : print the configuration of this FIP to output
     * stream.
     * \param os  output stream.
     * \param sr  fault state machine.
     * \return  output stream after printing.
     */
	friend ostream& operator<<(ostream& os, const EsynetFaultStateMachine & sr);
};

/**
 * @ingroup fip
 * \brief State machine of router fault injection point.
 *
 * Present the router is faulty or not, used by fault tolerant routing
 * algorithm, build-in self-test usually.
 */
class EsynetRouterFaultUnit : public EsynetFaultStateMachine
{
protected:
    long m_id; /**< \brief router id */

public:
    /*!
     * \brief Construct an empty fault injection unit.
     */
	EsynetRouterFaultUnit() :
		EsynetFaultStateMachine(), m_id(-1)
    {}
    /*!
     * \brief Construct a fault injection unit for specified router.
     * \param id router id.
     */
	EsynetRouterFaultUnit(long id) :
		EsynetFaultStateMachine(), m_id(id)
    {}
    /*!
     * \brief Config the fault injection unit by the best match unit in fault
     * list.
     *
     * Choose the best match item. The best match item should have the same
     * type FaultConfigItem::FAULT_ROUTER, and available to all routers or the
     * same router.
     * The item with smaller range has the better match.
     *
     * \param faultlist  list of fault configuration items.
     */
    void configFaultList( EsyFaultConfigList & faultlist );

    /*!
     * \brief operator << : print the configuration of this FIP to output
     * stream.
     * \param os  output stream.
     * \param sr  fault unit.
     * \return  output stream after printing.
     */
	friend ostream& operator<<(ostream& os, const EsynetRouterFaultUnit & sr);
};

/**
 * @ingroup fip
 * \brief State machine of port fault injection point.
 *
 * Present the port is faulty or not, used by fault tolerant routing
 * algorithm, build-in self-test usually.
 */
class EsynetPortFaultUnit : public EsynetFaultStateMachine
{
protected:
    long m_id;   /**< \brief id of router */
    long m_port; /**< \brief id of port */

public:
    /*!
     * \brief Construct an empty fault injection unit.
     */
    EsynetPortFaultUnit() :
		EsynetFaultStateMachine()
    {}
    /*!
     * \brief Construct a fault injection unit for specified port of specified
     * router.
     * \param id  router id.
     * \param port  port id.
     */
    EsynetPortFaultUnit( long id, long port ) :
		EsynetFaultStateMachine(), m_id(id), m_port(port)
    {}
    /*!
     * \brief Config the fault injection unit by the best match unit in fault
     * list.
     *
     * Choose the best match item. The best match item should have the same
     * type FaultConfigItem::FAULT_ROUTER, and
     * - available to all ports of all routers, or
     * - available to the same ports of all routers, or
     * - available to all ports of the same router, or
     * - the same port of the same router.
     *
     * The item with smaller range has the better match.
     *
     * \param faultlist  list of fault configuration items.
     */
	void configFaultList(EsyFaultConfigList & faultlist);

    /*!
     * \brief operator << : print the configuration of this FIP to output
     * stream.
     * \param os  output stream.
     * \param sr  fault unit.
     * \return  output stream after printing.
     */
	friend ostream& operator<<(ostream& os, const EsynetPortFaultUnit & sr);
};

/**
 * @ingroup fip
 * \brief State machine of link fault injection point.
 *
 * Present the error bits of data path. It contains the same number of FIPs
 * with the width of data path. Each FIP presents the state of each bit.
 *
 * This item is used also in #ECCEncoder, #ECCInterDecoder, #ECCFinalDecoder to
 * present the fault injection points in ECC units.
 */
class EsynetLinkFaultUnit : public vector< EsynetFaultStateMachine >
{
protected:
	EsyFaultConfigItem::FaultType m_type; /**< \brief type of fault injection
     * points.

     * Options: FaultConfigItem::FAULT_LINE, FaultConfigItem::FAULT_ECCENC,
     * FaultConfigItem::FAULT_ECCINTERDEC, FaultConfigItem::FAULT_ECCFINALDEC.
     */
    long m_id;   /**< \brief id of router. */
    long m_port; /**< \brief id of port. */
    bool m_const_flag; /**< \brief flag that all FIPs are constant. */

public:
    /*!
     * \brief Construct a fault injection unit for specified point on data path.
     * \param type  type of fault injection points.
     * \param id  router id.
     * \param port  port id.
     * \param size  number of FIPs, as well as data width.
     */
	EsynetLinkFaultUnit(
		EsyFaultConfigItem::FaultType type, long id, long port, int size
    ) :
		vector< EsynetFaultStateMachine >(size, EsynetFaultStateMachine()),
        m_type( type ), m_id( id ), m_port( port ), m_const_flag( true )
    {}
    /*!
     * \brief Access the fault injection point at specified bit.
     * \param id  bit id.
     * \return  Reference to the specified fault injection point.
     */
	EsynetFaultStateMachine & faultPoint(long id) { return at(id); }
	bool isConst() { return m_const_flag;}
    /*!
     * \brief Config the fault injection unit by the best match unit in fault
     * list.
     *
     * Choose the best match item. The best match item should have the same
     * type, and
     * - available to all bits of all ports of all routers, or
     * - available to all bits of all ports of the same router, or
     * - available to all bits of the same port of all routers, or
     * - available to all bits of the same port of the same router, or
     * - available to the same bit of all ports of all routers, or
     * - available to the same bit of all ports of the same router, or
     * - available to the same bit of the same port of all routers, or
     * - available to the same bit of the same port of the same router
     *
     * The item with smaller range has the better match.
     *
     * \param faultlist  list of fault configuration items.
     */
	void configFaultList(EsyFaultConfigList & faultlist);

    /*!
     * \brief Calculate the next states of each FIPs.
     *
     * If all the items are constant, this function is skipped.
     * \sa EsynetFaultStateMachine::nextState()
     */
    void nextState();
    /*!
     * \brief Update the fault pattern in flit structure.
     *
     * Set the same bit in fault pattern if the FIP of this bit is faulty.
     * Otherwise, the bit in fault pattern is not changed. No bit is clear
     * althrough the FIP is living.
     *
     * \param pattern  reference to the fault pattern.
     */
    void updateFaultPattern( DataType & pattern );
	long faultBitCount();

	long stateBitCount(long state);

    /*!
     * \brief operator << : print the configuration of this FIP to output
     * stream.
     * \param os  output stream.
     * \param sr  fault unit.
     * \return  output stream after printing.
     * \sa #operator <<(ostream &, const EsynetFaultStateMachine &)
     */
	friend ostream& operator<<(ostream& os, const EsynetLinkFaultUnit & sr);
};

#endif

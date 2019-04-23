/*
 Copyright @ UESTC

 ESY-series Many-core System-on-chip Simulation Environment is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.

 ESY-series Many-core System-on-chip Simulation Environment is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/.
 */

#ifndef INTERFACE_FAULTCFG_H
#define INTERFACE_FAULTCFG_H

#include "esy_iodatafile.h"
#include "esy_iodatafile_item.h"

using namespace std;

/** @defgroup faultcfginterface Fault Configuration Interface.
 * @{
 */

/**
 * @ingroup datafileinterface
 * @ingroup faultcfginterface
 * @brief Item in fault configuration list.
 *
 * Holds parameters and value of one fault injection unit. Each item
 * contains following information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Type </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> long #m_type </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Type of fault injection unit (Router, Link, Port, etc.). </td>
 *   <td> #faultType() </td>
 * </tr>
 * <tr>
 *   <td> long #m_router </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Router Id of faulty injection unit. </td>
 *   <td> #router() </td>
 * </tr>
 * <tr>
 *   <td> long #m_port </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Port Id of faulty injection unit. </td>
 *   <td> #port() </td>
 * </tr>
 * <tr>
 *   <td> long #m_vc </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Virtual Channel of faulty injection unit. </td>
 *   <td> #vc() </td>
 * </tr>
 * <tr>
 *   <td> long #m_bit </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Bit of faulty injection unit. </td>
 *   <td> #bit() </td>
 * </tr>
 * <tr>
 *   <td> vector< long > #m_state </td>
 *   <td> DataFormatItem::LONG_VECTOR_TYPE </td>
 *   <td> Status list of finite state machine. </td>
 *   <td> #state() </td>
 * </tr>
 * <tr>
 *   <td> vector< double > #m_transfer </td>
 *   <td> DataFormatItem::DOUBLE_VECTOR_TYPE </td>
 *   <td> Transform probability matrix of finite state machine. </td>
 *   <td> #transfer(), #transferMatrix() </td>
 * </tr>
 * </table>
 *
 * For each field, -1 means the configuration is available to all options.
 *
 * Fault configuration item is also a data item. transform matrix is converted
 * to one dimension to store. As status list and transform matrix are variable
 * length vector, this item can only be stored in .txt format file.
 *
 * \sa FaultConfigList
 */
class EsyFaultConfigItem : public EsyDataListItem
{
public:
	/*!
	 * \brief Type of faulty injection unit
	 */
	enum FaultType
	{
		FAULT_ROUTER,       /**< router, specified router */
		FAULT_PORT,         /**< port, specified router, port */
		FAULT_LINE,         /**< line, specified router, port, bit */
		FAULT_BUFFER,       /**< buffer, specified router, port, vc, bit */
		FAULT_SWITCH,       /**< switch, specified router, port, bit */
		FAULT_ECCENC,       /**< encoder of ECC, specified router, port, bit */
		FAULT_ECCINTERDEC,  /**< inter decoder of ECC, specified router, port,
			bit */
		FAULT_ECCFINALDEC,  /**< final decoder of ECC, specified router, port,
			bit */
		FAULT_TYPE_NUM /**< count of types */
	};

	#define FAULTCFG_EXTENSION "fcfg"

protected:
	static string const_fault_type[ FAULT_TYPE_NUM ]; /**< \brief Const string
		list for fault type.

		Access functions: #faultTypeStr() */
	long m_type;   /**< \brief Fault type. */
	long m_router; /**< \brief Router id. */
	long m_port;   /**< \brief Port id. */
	long m_vc;     /**< \brief Virtual channel. */
	long m_bit;    /**< \brief Bit id. */
	vector< long > m_state; /**< \brief Status list, 0 or 1 for each state. */
	vector< double > m_transfer;  /**< \brief Transform probability matrix. */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item.
	 */
	EsyFaultConfigItem();
	/*!
	 * \brief Constructs an item with specified value for fields.
	 * \param faultType  fault type.
	 * \param router     router id.
	 * \param port       port id.
	 * \param vc         vc id.
	 * \param bit        bit id.
	 * \param state      status list.
	 * \param transfer   tranform matrix in 1 dimension.
	 */
	EsyFaultConfigItem(
		FaultType faultType, long router, long port, long vc, long bit,
		const vector< long > & state, const vector< double > & transfer );
	/*!
	 * \brief Constructs an item with specified value for fields.
	 *
	 * Converts transform probability matrix to 1 dimension and sets
	 * #m_transfer.
	 *
	 * \param faultType  fault type.
	 * \param router     router id.
	 * \param port       port id.
	 * \param vc         vc id.
	 * \param bit        bit id.
	 * \param state      status list.
	 * \param transfer_matrix   tranform matrix in 2 dimensions [current][next].
	 */
	EsyFaultConfigItem(
		FaultType faultType, long router, long port, long vc, long bit,
		const vector< long > & state,
		const vector< vector< double > > & transfer_matrix );
	/*!
	 * \brief Constructs an item by parsing a string line.
	 *
	 * Parses the item in string line and sets fields.
	 *
	 * \param line  string line to parse.
	 */
	EsyFaultConfigItem(const vector< string >& line);
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsyFaultConfigItem(const EsyFaultConfigItem & t);
	///@}

	/** @name functions to access variables. */
	///@{
	/*!
	 * \brief Access fault type of fault injection unit #m_type.
	 * \return fault type #m_type.
	 */
	FaultType faultType() const { return (FaultType)m_type; }
	/*!
	 * \brief Access router id of fault injection unit #m_router.
	 * \return router id #m_router.
	 */
	long router() const { return m_router; }
	/*!
	 * \brief Access port id of fault injection unit #m_port.
	 * \return port id #m_port.
	 */
	long port() const { return m_port; }
	/*!
	 * \brief Access virtual channel of fault injection unit #m_vc.
	 * \return virtual channel #m_vc.
	 */
	long vc() const { return m_vc; }
	/*!
	 * \brief Access bit id of fault injection unit #m_bit.
	 * \return bit id #m_bit.
	 */
	long bit() const { return m_bit; }
	/*!
	 * \brief Access status list of fault injection unit #m_state.
	 * \return status list #m_state.
	 */
	const vector< long > & state() const { return m_state; }
	/*!
	 * \brief Access transform probability matrix #m_transfer.
	 * \return transform probability matrix #m_transfer in one dimension.
	 */
	const vector< double > & transfer() const { return m_transfer; }
	/*!
	 * \brief Access transform probability matrix #m_transfer.
	 * \return transform probability matrix #m_transfer in two dimension.
	 * [current][next]
	 */
	vector< vector< double > > transferMatrix();
	///@}

	/*!
	 * \brief Access string of specified fault type.
	 * \return string of specified fault type.
	 */
	static const string & faultTypeStr(FaultType type)
	{ return const_fault_type[ type ]; }

	/*!
	 * \brief Check whether the finit state machine is const.
	 *
	 * If there is only one state, machine is constant at this state.
	 *
	 * \return TRUE or FALSE.
	 */
	bool isConstant() const { return (m_state.size() <= 1); }

protected:
	/*!
	 * \brief Register the variables in the item.
	 */
	void insertVariables();
};

/**
 * @ingroup faultcfginterface
 * @brief Interface to fault configuration file.
 *
 * This interface generates and parses the fault configuration file to guide
 * the fault injection units in the simulator. This interface also parses the
 * configuration items in command line.
 *
 * \sa FaultConfigItem
 */
class EsyFaultConfigList : public vector< EsyFaultConfigItem >
{
public:
	/*!
	 * \brief Constructs an empty item.
	 */
	EsyFaultConfigList() :
		vector< EsyFaultConfigItem >()
	{}

	/*!
	 * \brief Gets the best match item for specified fault injection unit.
	 *
	 * If one item has the same field as specified fault injection unit, this
	 * item is the best match unit. Otherwise, the best match item can cover
	 * the specified fault injection unit and also has the smallest range. As
	 * shown in following table, the item with lowest priority is the best
	 * match item.
	 *
	 * <table>
	 * <tr>
	 *   <td> type </td>
	 *   <td> router </td>
	 *   <td> port </td>
	 *   <td> vc </td>
	 *   <td> bit </td>
	 *   <td> priority </td>
	 * </tr>
	 * <tr>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> 0 </td>
	 * </tr>
	 * <tr>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> > </td>
	 *   <td> 1 </td>
	 * </tr>
	 * <tr>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> > </td>
	 *   <td> - </td>
	 *   <td> 2 </td>
	 * </tr>
	 * <tr>
	 *   <td> = </td>
	 *   <td> = </td>
	 *   <td> > </td>
	 *   <td> - </td>
	 *   <td> - </td>
	 *   <td> 3 </td>
	 * </tr>
	 * <tr>
	 *   <td> = </td>
	 *   <td> > </td>
	 *   <td> - </td>
	 *   <td> - </td>
	 *   <td> - </td>
	 *   <td> 4 </td>
	 * </tr>
	 * <tr>
	 *   <td> > </td>
	 *   <td> - </td>
	 *   <td> - </td>
	 *   <td> - </td>
	 *   <td> - </td>
	 *   <td> 5 </td>
	 * </tr>
	 * </table>
	 * ">" means larger range and "=" means same range.
	 *
	 * \param type   fault type
	 * \param router router id
	 * \param port   port id
	 * \param vc     virtual channel id
	 * \param bit    bit id
	 * \return  the best match item in fault configuration list.
	 */
	EsyFaultConfigItem bestMatchItem(EsyFaultConfigItem::FaultType type,
		long router = -1, long port = -1, long vc = -1, long bit = -1 );
	/*!
	 * \brief Gets the items covered by specified parameters.
	 *
	 * If the field is -1, which means all the options in this field are covered.
	 * Otherwise, the value in item must be as same as the specified field.
	 *
	 * \param type   fault type
	 * \param router router id
	 * \param port   port id
	 * \param vc     virtual channel id
	 * \param bit    bit id
	 * \return  the list of items covered by specified parameters.
	 */
	EsyFaultConfigList subItemList(EsyFaultConfigItem::FaultType type,
		long router = -1, long port = -1, long vc = -1, long bit = -1 );

	/** @name Functions to access fault configuration file. */
	///@{
	/*!
	 * \brief Write fault configuration items to fault configuration file.
	 * \param file  path of fault configuration file.
	 * \sa ODataFileStream
	 */
	void writeFile( const string & file );
	/*!
	 * \brief Read fault configuration items from fault configuration file.
	 * \param file  path to fault configuration files.
	 * \sa IDataFileStream
	 */
	void readFile( const string & file );
	///@}

	/*!
	 * \brief Add items from arguments list.
	 * \param line  Arguments list for fault configuration.
	 */
	void addInLine( const vector< string > & line );
};

/** @} */

#endif

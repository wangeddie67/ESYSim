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

#ifndef INTERFACE_BENCHMARK_H
#define INTERFACE_BENCHMARK_H

#include "esy_iodatafile.h"

using namespace std;

/**
 * @ingroup datafileinterface
 * @brief Item in packet list for benchmark.
 *
 * Holds parameters and value of one packet. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Type </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> double #m_time </td>
 *   <td> DataFormatItem::DOUBLE_TYPE </td>
 *   <td> Injection time, unit in simulation cycle. </td>
 *   <td> #time() </td>
 * </tr>
 * <tr>
 *   <td> char #m_src </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Source router id. </td>
 *   <td> #src() </td>
 * </tr>
 * <tr>
 *   <td> char #m_dst </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Destination router id. </td>
 *   <td> #dst() </td>
 * </tr>
 * <tr>
 *   <td> char #m_size </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Packet size, unit in flit. </td>
 *   <td> #size() </td>
 * </tr>
 * </table>
 */
class EsyDataItemBenchmark : public EsyDataListItem
{
	#define BENCHMARK_EXTENSION "bench"
protected:
    double m_time;  /**< \brief Packet injection time, unit in simulation cycle. */
    short m_src;  /**< \brief Source router id. */
    short m_dst;  /**< \brief Destination router id. */
    short m_size;  /**< \brief Packet size, unit in flit. */
public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item.
     */
	EsyDataItemBenchmark();
    /*!
     * \brief Constructs an item with specified value for fields.
     * \param time  injection time.
     * \param src   source router id.
     * \param dst   destination router id.
     * \param size  packet size.
     */
	EsyDataItemBenchmark(double time, short src, short dst, short size);
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
	EsyDataItemBenchmark(const EsyDataItemBenchmark & t);
    ///@}
    
    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access packet injection time #m_time.
     * \return packet injection time #m_time.
     */
    double time() const { return m_time; }
    /*!
     * \brief Access source router id #m_src.
     * \return source router id #m_src.
     */
    short src() const { return m_src; }
    /*!
     * \brief Access destination router id #m_dst.
     * \return destination router id #m_dst.
     */
    short dst() const { return m_dst; }
    /*!
     * \brief Access packet size #m_size.
     * \return packet size #m_size.
     */
    short size() const { return m_size; }
    ///@}

protected:
    /*!
     * \brief Register the variables in the item.
     */
    void insertVariables();
};

/**
 * @ingroup datafileinterface
 * @brief Item in event trace list.
 *
 * Holds parameters and value of one event item. One event item presents
 * - Simulation events from simulators;
 * - State changes of fault injection units;
 * - Application execution.
 * Therefore, some fields have multiple means for different types of event.
 *
 * Each item contains following information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Type </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> double #m_time </td>
 *   <td> DataFormatItem::DOUBLE_TYPE </td>
 *   <td> Occupation time of event, unit in simulation cycle. </td>
 *   <td> #time() </td>
 * </tr>
 * <tr>
 *   <td> char #m_type </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Type code of event. </td>
 *   <td> #type() </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> char #m_src </td>
 *   <td rowspan="2"> DataFormatItem::NUM8_TYPE </td>
 *   <td> Source router id of one event. </td>
 *   <td> #src() </td>
 * </tr>
 * <tr>
 *   <td> Type of fault injection unit. </td>
 *   <td> #faultyType() </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> char #m_src_pc </td>
 *   <td rowspan="2"> DataFormatItem::NUM8_TYPE </td>
 *   <td> Source physical channel id of one event. </td>
 *   <td> #srcPc() </td>
 * </tr>
 * <tr>
 *   <td> Router id of fault injection unit. </td>
 *   <td> #faultyRouter() </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> char #m_src_vc </td>
 *   <td rowspan="2"> DataFormatItem::NUM8_TYPE </td>
 *   <td> Source virtual channel id of one event. </td>
 *   <td> #srcVc() </td>
 * </tr>
 * <tr>
 *   <td> Physical port id of fault injection unit. </td>
 *   <td> #faultyPort() </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> char #m_dst </td>
 *   <td rowspan="2"> DataFormatItem::NUM8_TYPE </td>
 *   <td> Destination router id of one event. </td>
 *   <td> #dst() </td>
 * </tr>
 * <tr>
 *   <td> Virtual channel id of fault injection unit. </td>
 *   <td> #faultyVC() </td>
 * </tr>
 * <tr>
 *   <td rowspan="2"> char #m_dst_pc </td>
 *   <td rowspan="2"> DataFormatItem::NUM8_TYPE </td>
 *   <td> Destination physical channel id of one event. </td>
 *   <td> #dstPc() </td>
 * </tr>
 * <tr>
 *   <td> Bit id of fault injection unit. </td>
 *   <td> #faultyBit() </td>
 * </tr>
 * <tr>
 *   <td> char #m_dst_vc </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Destination virtual channel id of one event. </td>
 *   <td> #dstVc() </td>
 * </tr>
 * <tr>
 *   <td> long #m_flit_id </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Unique id of flit. </td>
 *   <td> #flitId() </td>
 * </tr>
 * <tr>
 *   <td> char #m_flit_size </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Size of the packet, unit in flit. </td>
 *   <td> #flitSize() </td>
 * </tr>
 * <tr>
 *   <td> char #m_flit_type </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Type of flit. </td>
 *   <td> #flitType() </td>
 * </tr>
 * <tr>
 *   <td> char #m_flit_src </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Source router id of flit. </td>
 *   <td> #flitSrc() </td>
 * </tr>
 * <tr>
 *   <td> char #m_flit_dst </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Destination router id of flit. </td>
 *   <td> #flitDst() </td>
 * </tr>
 * <tr>
 *   <td> char #m_flit_flag </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Flag of one flit. </td>
 *   <td> #flitFlag() </td>
 * </tr>
 * <tr>
 *   <td> double #m_start_time </td>
 *   <td> DataFormatItem::DOUBLE_TYPE </td>
 *   <td> Generation time of flit. </td>
 *   <td> #flitStartTime() </td>
 * </tr>
 * </table>
 */
class EsyDataItemEventtrace : public EsyDataListItem
{
	#define EVENTTRACE_EXTENSION "event"
protected:

    double m_time;  /**< \brief Occupation time of event, unit in simulation cycle.*/
    char m_type;  /**< \brief Type code of event. */
    short m_src;  /**< \brief Source router id of one event, or Type of fault 
		injection unit. */
    char m_src_pc;  /**< \brief Source physical channel id of one event, or Router id
        of fault injection unit.*/
    char m_src_vc;  /**< \brief Source virtual channel id of one event, or Physical
        port id of fault injection unit. */
    short m_dst;  /**< \brief Destination router id of one event, or Virtual 
		channel id of fault injection unit. */
    char m_dst_pc;  /**< \brief Destination physical channel id of one event, or Bit id
        of fault injection unit.*/
    char m_dst_vc;  /**< \brief Destination virtual channel id of one event. */
    long m_flit_id;  /**< \brief Unique id of flit. */
    short m_flit_size;  /**< \brief Size of the packet, unit in flit. */
    char m_flit_type;  /**< \brief Type of flit. */
    short m_flit_src;  /**< \brief Source router id of flit. */
    short m_flit_dst;  /**< \brief Destination router id of flit. */
    char m_flit_flag;  /**< \brief Flag of one flit. */
    double m_start_time;  /**< \brief Generation time of flit. */

public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item.
     */
	EsyDataItemEventtrace();
    /*!
     * \brief Constructs an item with specified value for fields.
     * \param time  Occupation time of event, unit in simulation cycle.
     * \param type  Type code of event.
     * \param src  Source router id of one event, or Type of fault injection
        unit.
     * \param src_pc  Source physical channel id of one event, or Router id
        of fault injection unit.
     * \param src_vc  Source virtual channel id of one event, or Physical
        port id of fault injection unit.
     * \param dst  Destination router id of one event, or Virtual channel id
        of fault injection unit.
     * \param dst_pc  Destination physical channel id of one event, or Bit id
        of fault injection unit.
     * \param dst_vc  Destination virtual channel id of one event.
     * \param flit_id  Unique id of flit.
     * \param flit_size  Size of the packet, unit in flit.
     * \param flit_type  Type of flit.
     * \param flit_src  Source router id of flit.
     * \param flit_dst  Destination router id of flit.
     * \param flit_flag  Flag of one flit.
     * \param start_time  Generation time of flit.
     */
	EsyDataItemEventtrace(double time, char type,
        short src, char src_pc, char src_vc,
        short dst, char dst_pc, char dst_vc,
        long flit_id, short flit_size, char flit_type,
        short flit_src, short flit_dst, char flit_flag,
        double start_time );
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
	EsyDataItemEventtrace(const EsyDataItemEventtrace & t);
    ///@}

    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access occupation time of event #m_time.
     * \return occupation time of event #m_time.
     */
    double time() const { return m_time; }
    /*!
     * \brief Access type of event #m_type.
     * \return type of event #m_type.
     */
    char type() const { return m_type; }
    /*!
     * \brief Access source router id of event #m_src.
     * \return source router id of event #m_src.
     */
    short src() const { return m_src; }
    /*!
     * \brief Access source physical channel id of event #m_src_pc.
     * \return source physical channel id of event #m_src_pc.
     */
    char srcPc() const { return m_src_pc; }
    /*!
     * \brief Access source virtual channel id of event #m_src_vc.
     * \return source virtual channel id of event #m_src_vc.
     */
    char srcVc() const { return m_src_vc; }
    /*!
     * \brief Access destination router id of event #m_dst.
     * \return destination router id of event #m_dst.
     */
    short dst() const { return m_dst; }
    /*!
     * \brief Access destination physical channel id of event #m_dst_pc.
     * \return destination physical channel id of event #m_dst_pc.
     */
    char dstPc() const { return m_dst_pc; }
    /*!
     * \brief Access destination virtual channel id of event #m_dst_vc.
     * \return destination virtual channel id of event #m_dst_vc.
     */
    char dstVc() const { return m_dst_vc; }
    /*!
     * \brief Access unique id of flit #m_flit_id.
     * \return unique id of flit #m_flit_id.
     */
    long flitId() const { return m_flit_id; }
    /*!
     * \brief Access size of the packet #m_flit_size, unit in flit.
     * \return size of the packet #m_flit_size.
     */
    short flitSize() const { return m_flit_size; }
    /*!
     * \brief Access type of flit #m_flit_type.
     * \return type of flit #m_flit_type.
     */
    char flitType() const { return m_flit_type; }
    /*!
     * \brief Access source router id of flit #m_flit_src.
     * \return source router id of flit #m_flit_src.
     */
    short flitSrc() const { return m_flit_src; }
    /*!
     * \brief Access destination router id of flit #m_flit_dst.
     * \return destination router id of flit #m_flit_dst.
     */
    short flitDst() const { return m_flit_dst; }
    /*!
     * \brief Access flag of one flit #m_flit_flag.
     * \return flag of one flit #m_flit_flag.
     */
    char flitFlag() const { return m_flit_flag; }
    /*!
     * \brief Access generation time of flit #m_start_time.
     * \return generation time of flit #m_start_time.
     */
    double flitStartTime() const { return m_start_time; }
    /*!
     * \brief Access type of fault injection unit #m_src.
     * \return type of fault injection unit #m_src.
     */
    char faultyType() const { return m_src; }
    /*!
     * \brief Access router id of fault injection unit #m_src_pc.
     * \return router id of fault injection unit #m_src_pc.
     */
    char faultyRouter() const { return m_src_pc; }
    /*!
     * \brief Access physical port id of fault injection unit #m_src_vc.
     * \return physical port id of fault injection unit #m_src_vc.
     */
    char faultyPort() const { return m_src_vc; }
    /*!
     * \brief Access virtual channel id of fault injection unit #m_dst.
     * \return virtual channel id of fault injection unit #m_dst.
     */
    char faultyVC() const { return m_dst; }
    /*!
     * \brief Access bit id of fault injection unit #m_dst_pc.
     * \return bit id of fault injection unit #m_dst_pc.
     */
    char faultyBit() const { return m_dst_pc; }
    ///@}

protected:
    /*!
     * \brief Register the variables in the item.
     */
    void insertVariables();
};

/**
 * @ingroup datafileinterface
 * @brief Item in path pair list for path analysis and trace.
 *
 * Holds parameters and value of one path. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Type </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> char #m_src </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Source router id of path. </td>
 *   <td> #src() </td>
 * </tr>
 * <tr>
 *   <td> char #m_dst </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> Destination router id of path. </td>
 *   <td> #dst() </td>
 * </tr>
 * <tr>
 *   <td> char #m_hop </td>
 *   <td> DataFormatItem::NUM8_TYPE </td>
 *   <td> The number of routers from source to destination. </td>
 *   <td> #hop() </td>
 * </tr>
 * </table>
 */
class EsyDataItemPathpair : public EsyDataListItem
{
	#define PATHPAIR_EXTENSION "ppair"
protected:
    short m_src; /**< \brief Source router id of path. */
    short m_dst; /**< \brief Destination router id of path. */
    short m_hop; /**< \brief The number of routers from source to destination. 
		*/

public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item.
     */
	EsyDataItemPathpair();
    /*!
     * \brief Constructs an item with specified value for fields.
     * \param src  Source router id of path.
     * \param dst  Destination router id of path.
     * \param hop  The number of routers from source to destination.
     */
	EsyDataItemPathpair(short src, short dst, short hop);
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
	EsyDataItemPathpair(const EsyDataItemPathpair & t);
    ///@}

    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access source router id of path #m_src.
     * \return source router id of path #m_src.
     */
    short src() const { return m_src; }
    /*!
     * \brief Access destination router id of path #m_dst.
     * \return destination router id of path #m_dst.
     */
    short dst() const { return m_dst; }
    /*!
     * \brief Access the number of routers from source to destination #m_hop.
     * \return the number of routers from source to destination #m_hop.
     */
    short hop() const { return m_hop; }
    ///@}

protected:
    /*!
     * \brief Register the variables in the item.
     */
    void insertVariables();
};

/**
 * @ingroup datafileinterface
 * @brief One hop in path.
 *
 * Hop describes the turn in one router, from input channel to output channel.
 * Holds parameters and value of one hop in path. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> long #m_addr </td>
 *   <td> Router id of hop. </td>
 *   <td> #addr() </td>
 * </tr>
 * <tr>
 *   <td> long #m_in_pc </td>
 *   <td> Input physical channel of hop. </td>
 *   <td> #inputPc() </td>
 * </tr>
 * <tr>
 *   <td> long #m_in_vc </td>
 *   <td> Input virtual channel of hop. </td>
 *   <td> #inputVc() </td>
 * </tr>
 * <tr>
 *   <td> long #m_out_pc </td>
 *   <td> Output physical channel of hop. </td>
 *   <td> #outputPc(), #setOutputPc() </td>
 * </tr>
 * <tr>
 *   <td> long #m_out_vc </td>
 *   <td> Output virtual channel of hop. </td>
 *   <td> #outputVc(), #setOutputVc() </td>
 * </tr>
 * </table>
 */
class EsyDataItemHop
{
protected:
    long m_addr;    /**< \brief Router id of hop. */
    long m_in_pc;   /**< \brief Input physical channel of hop. */
    long m_in_vc;   /**< \brief Input virtual channel of hop. */
    long m_out_pc;  /**< \brief Output physical channel of hop. */
    long m_out_vc;  /**< \brief Output virtual channel of hop. */

public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item.
     */
	EsyDataItemHop();
    /*!
     * \brief Constructs an item with specified value for fields.
     * \param addr    Router id.
     * \param in_pc   Input physical channel.
     * \param in_vc   Input virtual channel.
     * \param out_pc  Output physical channel.
     * \param out_vc  Output virtual channel.
     */
	EsyDataItemHop(long addr, long in_pc, long in_vc, long out_pc, long out_vc);
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
	EsyDataItemHop(const EsyDataItemHop & t);
    ///@}

    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access router id of hop #m_addr.
     * \return router id of hop #m_addr.
     */
    long addr() const { return m_addr; }
    /*!
     * \brief Access input physical channel of hop #m_in_pc.
     * \return input physical channel of hop #m_in_pc.
     */
    long inputPc() const { return m_in_pc; }
    /*!
     * \brief Access input virtual channel of hop #m_in_vc.
     * \return input virtual channel of hop #m_in_vc.
     */
    long inputVc() const { return m_in_vc; }
    /*!
     * \brief Access output physical channel of hop #m_out_pc.
     * \return output physical channel of hop #m_out_pc.
     */
    long outputPc() const { return m_out_pc; }
    /*!
     * \brief Access output virtual channel of hop #m_out_vc.
     * \return output virtual channel of hop #m_out_vc.
     */
    long outputVc() const { return m_out_vc; }
    /*!
     * \brief Set output physical channel of hop #m_out_pc.
     * \param pc output physical channel.
     */
    void setOutputPc( char pc ) { m_out_pc = pc; }
    /*!
     * \brief Set output virtual channel of hop #m_out_vc.
     * \param vc output virtual channel.
     */
    void setOutputVc( char vc ) { m_out_vc = vc; }
    ///@}

    /*!
     * \brief Overload operator == function. Compare value of two items.
     * \param t  item to compare.
     * \return If t has the same value as this item at all field, return TRUE.
     * Otherwise, return FALSE.
     */
    bool operator ==( const EsyDataItemHop & t )
    {
        if (m_addr != t.m_addr  ||
            m_in_pc != t.m_in_pc || m_in_vc != t.m_in_vc ||
            m_out_pc!= t.m_out_pc|| m_out_vc!= t.m_out_vc )
        {
            return false;
        }
        return true;
    }
};

/**
 * @ingroup datafileinterface
 * @brief One hop in path. Item in path trace model.
 *
 * Hop describes the turn in one router, from input channel to output channel.
 * Holds parameters and value of one hop in path. Also, this item has the
 * traffic count and faulty flag. Each item contains following information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> long #m_traffic </td>
 *   <td> Count of flits through hop. </td>
 *   <td> #traffic(), #incTraffic() </td>
 * </tr>
 * <tr>
 *   <td> bool #m_faulty </td>
 *   <td> Faulty flag of hop. </td>
 *   <td> #faulty(), #setFaulty() </td>
 * </tr>
 * </table>
 *
 * \sa PathtraceHop
 */
class EsyDataItemTraffic : public EsyDataItemHop
{
protected:
    long m_traffic;  /**< \brief Count of flits through hop */
    bool m_faulty;   /**< \brief Faulty flag of hop. */

public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item.
     */
	EsyDataItemTraffic();
    /*!
     * \brief Constructs an item with specified value for fields.
     * \param id      Router id.
     * \param in_pc   Input physical channel.
     * \param in_vc   Input virtual channel.
     * \param out_pc  Output physical channel.
     * \param out_vc  Output virtual channel.
     * \param count   Traffic count, flits go through this hop.
     */
	EsyDataItemTraffic(long id, long in_pc, long in_vc,
                          long out_pc, long out_vc, long count );
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
	EsyDataItemTraffic(const EsyDataItemTraffic & t);
    ///@}

    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access the number of flits through hop #m_traffic.
     * \return the number of flits through hop #m_traffic.
     */
    long traffic() const { return m_traffic; }
    /*!
     * \brief Increase the number of flits through hop #m_traffic by 1.
     */
    void incTraffic() {m_traffic ++;}
    /*!
     * \brief Access faulty flag of hop #m_faulty.
     * \return faulty flag of hop #m_faulty.
     */
    bool faulty() const { return m_faulty; }
    /*!
     * \brief Set faulty flag of hop #m_faulty.
     * \param faulty faulty flag of hop.
     */
    void setFaulty( bool faulty ) { m_faulty = faulty; }
};

/**
 * @ingroup datafileinterface
 * @brief Item in path list for path analysis and trace.
 *
 * Holds parameters and value of one path and hops. Hops are stored in a vector
 * of NUM8 which can be converted to a vector of PathtraceHop. Each item
 * contains following information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Type </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> long #m_src </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Source router id of path. </td>
 *   <td> #src() </td>
 * </tr>
 * <tr>
 *   <td> long #m_dst </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Destination router id of path. </td>
 *   <td> #dst() </td>
 * </tr>
 * <tr>
 *   <td> vector< long > #m_trace </td>
 *   <td> DataFormatItem::LONG_VECTOR_TYPE </td>
 *   <td> Trace of hop. </td>
 *   <td> #trace(), #pathhoplist() </td>
 * </tr>
 * </table>
 *
 * \sa PathtraceHop
 */
class EsyDataItemPathtrace : public EsyDataListItem
{
	#define PATHTRACE_EXTENSION "path"
protected:
    long m_src;  /**< \brief Source router id of path. */
    long m_dst;  /**< \brief Destination router id of path. */
    vector< long > m_trace;  /**< \brief Trace of hop.
     *
     * One hop has five NUM8 fields. Connect fields together end to end. The
     * first five fields belong to first hop, the second five fields belong
     * to second hop, and so on.
     */

public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item.
     */
	EsyDataItemPathtrace();
    /*!
     * \brief Constructs an item with specified value for fields.
     * \param src    Source router id.
     * \param dst    Destination router id.
     * \param trace  Trace of hop. Already in one vector of NUM8.
     */
	EsyDataItemPathtrace(long src, long dst, const vector< long > & trace);
    /*!
     * \brief Constructs an item with specified value for fields.
     *
     * Function converts the vector of PathtraceHop to vector of NUM8 at first,
     * then sets the fields.
     *
     * \param src    Source router id.
     * \param dst    Destination router id.
     * \param trace  Trace of hop. A vector of PathtraceHop.
     */
	EsyDataItemPathtrace(long src, long dst, const vector< EsyDataItemHop > & trace);
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
	EsyDataItemPathtrace(const EsyDataItemPathtrace & t);
    ///@}

    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access source router id of path #m_src.
     * \return source router id of path #m_src.
     */
    long src() const { return m_src; }
    /*!
     * \brief Access destination router id of path #m_dst.
     * \return destination router id of path #m_dst.
     */
    long dst() const { return m_dst; }
    /*!
     * \brief Access hop trace of path #m_trace.
     * \return hop trace of path #m_trace in a vector of NUM8.
     */
    const vector< long > & trace() const { return m_trace; }
    /*!
     * \brief Access hop trace of path #m_trace.
     * \return hop trace of path #m_trace in a vector of PathtraceHop.
     */
    vector< EsyDataItemHop > pathhoplist() const;
    ///@}

protected:
    /*!
     * \brief Register the variables in the item.
     */
    void insertVariables();
};

/**
 * @ingroup datafileinterface
 * @brief Item in traffic window list for event trace anslysis.
 *
 * Holds parameters and value of the traffic distribution in one specified 
 * period. The flit switched in the start cycle is counted but in the end cycle 
 * is not counted. Each item contains following information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Type </th>
 *   <th> Meaning </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> long #m_start </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> Start cycle of period. </td>
 *   <td> #start() </td>
 * </tr>
 * <tr>
 *   <td> long #m_end </td>
 *   <td> DataFormatItem::LONG_TYPE </td>
 *   <td> End cycle of period. </td>
 *   <td> #end() </td>
 * </tr>
 * <tr>
 *   <td> vector< long > m_traffic </td>
 *   <td> DataFormatItem::LONG_VECTOR_TYPE </td>
 *   <td> Traffic distribution. Flit switched in routers. </td>
 *   <td> traffic() </td>
 * </tr>
 * </table>
 */
class EsyDataItemTrafficWindow : public EsyDataListItem
{
	#define WINDOW_EXTENSION "win"
protected:
    double m_start;  /**< \brief Start cycle of period. */
    double m_end;    /**< \brief End cycle of period. */
    vector< long > m_inject;  /**< \brief Inject traffic distribution.
     *
     * Flit injected by each router. 
     */
    vector< long > m_accept;  /**< \brief Accept traffic distribution.
     *
     * Flit accepted by each router. 
     */
    vector< long > m_through;  /**< \brief Through traffic distribution.
     *
     * Flit through each router. 
     */
    
public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item. The size of traffic is specified.
     */
    EsyDataItemTrafficWindow();
    /*!
     * \brief Constructs an item with specified value for start and end cycle. 
     * \param start  Start cycle of this window. 
     * \param end    End cycle of this window.
     * \param size   Number of routers. Size of m_traffic.
     */
    EsyDataItemTrafficWindow( double start, double end, long size );
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
    EsyDataItemTrafficWindow( const EsyDataItemTrafficWindow & t );
    ///@}
    
    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access start cycle of this window #m_start.
     * \return start cycle of this window #m_start.
     */
    double start() const { return m_start; }
    /*!
     * \brief Access end cycle of this window #m_end.
     * \return end cycle of this window #m_end.
     */
    double end() const { return m_end; }
    /*!
     * \brief Access inject traffic distribution #m_inject.
     * \return inject traffic distribution #m_inject.
     */
    const vector< long > & injectTraffic() const { return m_inject; }
    /*!
     * \brief Increase the number of flit injected by router id.
     * \param id  router id.
     * \param a   Inject flit.
     */
    void increaseInjectTraffic( long id, long a ) { m_inject[ id ] += a; } 
    /*!
     * \brief Access accept traffic distribution #m_accept.
     * \return accept traffic distribution #m_accept.
     */
    const vector< long > & acceptTraffic() const { return m_accept; }
    /*!
     * \brief Increase the number of flit accepted by router id.
     * \param id  router id.
     * \param a   Accept flit.
     */
    void increaseAcceptTraffic( long id, long a ) { m_accept[ id ] += a; } 
    /*!
     * \brief Access through traffic distribution #m_through.
     * \return through traffic distribution #m_through.
     */
    const vector< long > & throughTraffic() const { return m_through; }
    /*!
     * \brief Increase the number of flit through router id.
     * \param id  router id.
     * \param a   Through flit.
     */
    void increaseThroughTraffic( long id, long a ) { m_through[ id ] += a; } 

protected:
    /*!
     * \brief Register the variables in the item.
     */
    void insertVariables();
};

class EsyDataItemSoCRecord : public EsyDataListItem
{
	#define SOCRECORD_EXTENSION "record"
protected:
	char m_valid;

	double m_hop;
	short m_actual_id;
	double m_sim_cycle;

	char m_il1_access;
	char m_il1_hit;
	char m_il1_miss;

	char m_dl1_access;
	char m_dl1_hit;
	char m_dl1_miss;

	char m_il2_access;
	char m_il2_hit;
	char m_il2_miss;

	char m_dl2_access;
	char m_dl2_hit;
	char m_dl2_miss;

	char m_mem_access;
	char m_msg_send;
	char m_msg_recv;
	char m_msg_probe;

	char m_add_integer;
	char m_multi_integar;
	char m_add_float;
	char m_multi_float;

public:
	EsyDataItemSoCRecord();
	EsyDataItemSoCRecord(
		char valid, double hop, short actual_id, double sim_cycle,
		char il1_access, char il1_hit, char il1_miss,
		char dl1_access, char dl1_hit, char dl1_miss,
		char il2_access, char il2_hit, char il2_miss,
		char dl2_access, char dl2_hit, char dl2_miss,
		char mem_access, char msg_send, char msg_recv, char msg_probe,
		char add_integar, char multi_integar, char add_float, char multi_float
	);
	EsyDataItemSoCRecord(const EsyDataItemSoCRecord & t);

	char valid() const { return m_valid; }

	double hop() const { return m_hop; }
	short actualId() const { return m_actual_id; }
	double simCycle() const { return m_sim_cycle; }

	void setSimCycle( double cycle ) { m_sim_cycle = cycle; }

	char il1Access() const { return m_il1_access; }
	char il1Hit() const { return m_il1_hit; }
	char il1Miss() const { return m_il1_miss; }

	char dl1Access() const { return m_dl1_access; }
	char dl1Hit() const { return m_dl1_hit; }
	char dl1Miss() const { return m_dl1_miss; }

	char il2Access() const { return m_il2_access; }
	char il2Hit() const { return m_il2_hit; }
	char il2Miss() const { return m_il2_miss; }

	char dl2Access() const { return m_dl2_access; }
	char dl2Hit() const { return m_dl2_hit; }
	char dl2Miss() const { return m_dl2_miss; }

	char memAccess() const { return m_mem_access; }
	char msgSend() const { return m_msg_send; }
	char msgRecv() const { return m_msg_recv; }
	char msgProbe() const { return m_msg_probe; }

	char addInteger() const { return m_add_integer; }
	char multiIntegar() const { return m_multi_integar; }
	char addFloat() const { return m_add_float; }
	char multiFloat() const { return m_multi_float; }

protected:
	void insertVariables();
};

class EsyDataItemRecordWindow : public EsyDataListItem
{
    #define RECORDWIN_EXTENSION "rewin"
protected:
    double m_start;  /**< \brief Start cycle of period. */
    double m_end;    /**< \brief End cycle of period. */
    
    vector< long > m_il1_acces;
    vector< long > m_il1_access;
    vector< long > m_il1_hit;
    vector< long > m_il1_miss;

    vector< long > m_dl1_access;
    vector< long > m_dl1_hit;
    vector< long > m_dl1_miss;

    vector< long > m_il2_access;
    vector< long > m_il2_hit;
    vector< long > m_il2_miss;

    vector< long > m_dl2_access;
    vector< long > m_dl2_hit;
    vector< long > m_dl2_miss;

    vector< long > m_mem_access;
    vector< long > m_msg_send;
    vector< long > m_msg_recv;
    vector< long > m_msg_probe;

    vector< long > m_add_integer;
    vector< long > m_multi_integar;
    vector< long > m_add_float;
    vector< long > m_multi_float;

public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Constructs an empty item. The size of traffic is specified.
     */
    EsyDataItemRecordWindow();
    /*!
     * \brief Constructs an item with specified value for start and end cycle. 
     * \param start  Start cycle of this window. 
     * \param end    End cycle of this window.
     * \param size   Number of routers. Size of m_traffic.
     */
    EsyDataItemRecordWindow( double start, double end, long size );
    /*!
     * \brief Constructs an item by copying an exist item.
     * \param t  entity to copy.
     */
    EsyDataItemRecordWindow( const EsyDataItemRecordWindow & t );
    ///@}
    
    /** @name Functions to access variables. */
    ///@{
    /*!
     * \brief Access start cycle of this window #m_start.
     * \return start cycle of this window #m_start.
     */
    double start() const { return m_start; }
    /*!
     * \brief Access end cycle of this window #m_end.
     * \return end cycle of this window #m_end.
     */
    double end() const { return m_end; }
    
    const vector< long >& il1Access() const { return m_il1_access; }
    const vector< long >& il1Hit() const { return m_il1_hit; }
    const vector< long >& il1Miss() const { return m_il1_miss; }

    const vector< long >& dl1Access() const { return m_dl1_access; }
    const vector< long >& dl1Hit() const { return m_dl1_hit; }
    const vector< long >& dl1Miss() const { return m_dl1_miss; }

    const vector< long >& il2Access() const { return m_il2_access; }
    const vector< long >& il2Hit() const { return m_il2_hit; }
    const vector< long >& il2Miss() const { return m_il2_miss; }

    const vector< long >& dl2Access() const { return m_dl2_access; }
    const vector< long >& dl2Hit() const { return m_dl2_hit; }
    const vector< long >& dl2Miss() const { return m_dl2_miss; }

    const vector< long >& memAccess() const { return m_mem_access; }
    const vector< long >& msgSend() const { return m_msg_send; }
    const vector< long >& msgRecv() const { return m_msg_recv; }
    const vector< long >& msgProbe() const { return m_msg_probe; }

    const vector< long >& addInteger() const { return m_add_integer; }
    const vector< long >& multiIntegar() const { return m_multi_integar; }
    const vector< long >& addFloat() const { return m_add_float; }
    const vector< long >& multiFloat() const { return m_multi_float; }

    void increaseIl1Access(long id, long a) { m_il1_access[ id ] += a; }
    void increaseIl1Hit(long id, long a) { m_il1_hit[ id ] += a; }
    void increaseIl1Miss(long id, long a) { m_il1_miss[ id ] += a; }

    void increaseDl1Access(long id, long a) { m_dl1_access[ id ] += a; }
    void increaseDl1Hit(long id, long a) { m_dl1_hit[ id ] += a; }
    void increaseDl1Miss(long id, long a) { m_dl1_miss[ id ] += a; }

    void increaseIl2Access(long id, long a) { m_il2_access[ id ] += a; }
    void increaseIl2Hit(long id, long a) { m_il2_hit[ id ] += a; }
    void increaseIl2Miss(long id, long a) { m_il2_miss[ id ] += a; }

    void increaseDl2Access(long id, long a) { m_dl2_access[ id ] += a; }
    void increaseDl2Hit(long id, long a) { m_dl2_hit[ id ] += a; }
    void increaseDl2Miss(long id, long a) { m_dl2_miss[ id ] += a; }

    void increaseMemAccess(long id, long a) { m_mem_access[ id ] += a; }
    void increaseMsgSend(long id, long a) { m_msg_send[ id ] += a; }
    void increaseMsgRecv(long id, long a) { m_msg_recv[ id ] += a; }
    void increaseMsgProbe(long id, long a) { m_msg_probe[ id ] += a; }

    void increaseAddInteger(long id, long a) { m_add_integer[ id ] += a; }
    void increaseMultiIntegar(long id, long a) { m_multi_integar[ id ] += a; }
    void increaseAddFloat(long id, long a) { m_add_float[ id ] += a; }
    void increaseMultiFloat(long id, long a) { m_multi_float[ id ] += a; }    

protected:
    /*!
     * \brief Register the variables in the item.
     */
    void insertVariables();
};

#endif

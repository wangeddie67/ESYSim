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

#ifndef INTERFACE_IODATAFILE_ITEM_H
#define INTERFACE_IODATAFILE_ITEM_H

#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>

using namespace std;

/** @defgroup datafileinterface Date File Interface. */

/**
 * @ingroup datafileinterface
 * @brief One variable in data file interface.
 *
 * Holds parameters of one variable in the data item with specified format.
 * Control the hehavior to access the variable in the file in binary or text
 * format.
 *
 * <table>
 * <tr>
 *   <th rowspan="3"> Data type </th>
 *   <th rowspan="3"> Variable type </th>
 *   <th colspan="3"> Binary format </th>
 *   <th colspan="3"> Text format </th>
 * </tr>
 * <tr>
 *   <td colspan="3"> Put the binary code of variable in file directly.
 * no sperate between values. </td>
 *   <td colspan="3"> Values are sperated by "\t" and ends with "\t". If the
 * length of vector is variable, add one end character as the end of values.
 * </td>
 * </tr>
 * <tr>
 *   <th> Size </th>
 *   <th> Write function </th>
 *   <th> Read functiont </th>
 *   <th> Format </th>
 *   <th> Write function </th>
 *   <th> Read functiont </th>
 * </tr>
 * <tr>
 *   <td> #LONG_TYPE </td>
 *   <td> long </td>
 *   <td> 4 bytes </td>
 *   <td> #writeValueBinLong() </td>
 *   <td> #readValueBinLong() </td>
 *   <td>  </td>
 *   <td> #writeValueTextLong() </td>
 *   <td> #readValueTextLong() </td>
 * </tr>
 * <tr>
 *   <td> #DOUBLE_TYPE </td>
 *   <td> double </td>
 *   <td> 8 bytes </td>
 *   <td> #writeValueBinDouble() </td>
 *   <td> #readValueBinDouble() </td>
 *   <td>  </td>
 *   <td> #writeValueTextDouble() </td>
 *   <td> #readValueTextDouble() </td>
 * </tr>
 * <tr>
 *   <td> #BOOL_TYPE </td>
 *   <td> bool </td>
 *   <td> 1 byte </td>
 *   <td> #writeValueBinBool() </td>
 *   <td> #readValueBinBool() </td>
 *   <td> 0-FALSE, 1-TRUE </td>
 *   <td> #writeValueTextBool() </td>
 *   <td> #readValueTextBool() </td>
 * </tr>
 * <tr>
 *   <td> #CHAR_TYPE </td>
 *   <td> char </td>
 *   <td> 1 byte </td>
 *   <td> #writeValueBinChar() </td>
 *   <td> #readValueBinChar() </td>
 *   <td> ASCII character </td>
 *   <td> #writeValueTextChar() </td>
 *   <td> #readValueTextChar() </td>
 * </tr>
 * <tr>
 *   <td> #NUM8_TYPE </td>
 *   <td> char </td>
 *   <td> 1 byte </td>
 *   <td> #writeValueBinNum8() </td>
 *   <td> #readValueBinNum8() </td>
 *   <td> Digital number </td>
 *   <td> #writeValueTextNum8() </td>
 *   <td> #readValueTextNum8() </td>
 * </tr>
 * <tr>
 *   <td> #LONG_VECTOR_TYPE </td>
 *   <td> vector< long > </td>
 *   <td> 4 bytes * size </td>
 *   <td> #writeValueBinLongVector() </td>
 *   <td> #readValueBinLongVector() </td>
 *   <td>  </td>
 *   <td> #writeValueTextLongVector() </td>
 *   <td> #readValueTextLongVector() </td>
 * </tr>
 * <tr>
 *   <td> #DOUBLE_VECTOR_TYPE </td>
 *   <td> vector< double > </td>
 *   <td> 8 bytes * size </td>
 *   <td> #writeValueBinDoubleVector() </td>
 *   <td> #readValueBinDoubleVector() </td>
 *   <td>  </td>
 *   <td> #writeValueTextDoubleVector() </td>
 *   <td> #readValueTextDoubleVector() </td>
 * </tr>
 * <tr>
 *   <td> #BOOL_VECTOR_TYPE </td>
 *   <td> vector< bool > </td>
 *   <td> 1 byte * size </td>
 *   <td> #writeValueBinBoolVector() </td>
 *   <td> #readValueBinBoolVector() </td>
 *   <td> 0-FALSE, 1-TRUE </td>
 *   <td> #writeValueTextBoolVector() </td>
 *   <td> #readValueTextBoolVector() </td>
 * </tr>
 * <tr>
 *   <td> #CHAR_VECTOR_TYPE </td>
 *   <td> vector< char > </td>
 *   <td> 1 byte * size </td>
 *   <td> #writeValueBinCharVector() </td>
 *   <td> #readValueBinCharVector() </td>
 *   <td> ASCII character </td>
 *   <td> #writeValueTextCharVector() </td>
 *   <td> #readValueTextCharVector() </td>
 * </tr>
 * <tr>
 *   <td> #NUM8_VECTOR_TYPE </td>
 *   <td> vector< char > </td>
 *   <td> 1 byte * size </td>
 *   <td> #writeValueBinNum8Vector() </td>
 *   <td> #readValueBinNum8Vector() </td>
 *   <td> Digital number </td>
 *   <td> #writeValueTextNum8Vector() </td>
 *   <td> #readValueTextNum8Vector() </td>
 * </tr>
 * </table>
 *
 * \sa DataListItem, IDataFileStream, ODataFileStream
 */
class EsyDataFormatItem
{
public:
	/*!
	 * \brief Type of data.
	 */
	enum DataType
	{
		LONG_TYPE,          /**< long */
		SHORT_TYPE,         /**< short */
		DOUBLE_TYPE,        /**< double */
		BOOL_TYPE,          /**< bool */
		CHAR_TYPE,          /**< char */
		NUM8_TYPE,          /**< char, parse as a number */
		LONG_VECTOR_TYPE,   /**< vector< long > */
		SHORT_VECTOR_TYPE,         /**< short */
		DOUBLE_VECTOR_TYPE, /**< vector< double > */
		BOOL_VECTOR_TYPE,   /**< vector< bool > */
		CHAR_VECTOR_TYPE,   /**< vector< char > */
		NUM8_VECTOR_TYPE    /**< vector< char >, parse as a number */
	};

protected:
	DataType m_type;  /**< \brief Type of data */
	long m_size;  /**< \brief Size of vector, 0 means variable length vector. */
	void * mp_var;  /**< \brief Pointer to the variable. */
	char m_end;  /**< \brief End character for variable length vector. */
	bool m_is_var_vector; /**< \brief Flag of variable length vector. */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item.
	 */
	EsyDataFormatItem() :
		m_type(), m_size( 1 ), mp_var(), m_end( 0 ), m_is_var_vector( false )
	{}
	/*!
	 * \brief Constructs an item for non-vector type.
	 * \param type  type of data.
	 * \param var   pointer to variable.
	 */
	EsyDataFormatItem( DataType type, void * var ) :
		m_type( type ), m_size( 1 ), mp_var( var ), m_end( 0 ),
		m_is_var_vector( false)
	{}
	/*!
	 * \brief Constructs an item for constant length vector type.
	 * \param type  type of data.
	 * \param size  size of vector.
	 * \param var   pointer to variable.
	 */
	EsyDataFormatItem( DataType type, long size, void * var ) :
		m_type( type ), m_size( size ), mp_var( var ), m_end( 0 ),
		m_is_var_vector( false )
	{}
	/*!
	 * \brief Constructs an item for variable length vector type.
	 * \param type  type of data.
	 * \param var   pointer to variable.
	 * \param end   end character of vector.
	 */
	EsyDataFormatItem( DataType type, void * var, char end ) :
		m_type( type ), m_size( 0 ), mp_var( var ), m_end( end ),
		m_is_var_vector( true )
	{}
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsyDataFormatItem( const EsyDataFormatItem & t ) :
		m_type( t.m_type ), m_size( t.m_size ), mp_var( t.mp_var ), 
		m_end( t.m_end ), m_is_var_vector( t.m_is_var_vector )
	{}
	///@}

	/** @name Functions to write/read variables to/from stream. */
	///@{
	/*!
	 * \brief Write value to stream os in binary format or text format.
	 * \param os  interface to output stream.
	 * \param binary  binary format or not.
	 * \sa writeValueBinLong(), writeValueBinShort(), writeValueBinDouble(), 
	 * writeValueBinBool(), writeValueBinChar(), writeValueBinNum8(), 
	 * writeValueBinLongVector(), writeValueBinShortVector(),
	 * writeValueBinDoubleVector(), writeValueBinBoolVector(),
	 * writeValueBinCharVector(), writeValueBinNum8Vector(),
	 * writeValueTextLong(), writeValueTextShort, writeValueTextDouble(), 
	 * writeValueTextBool(), writeValueTextChar(), writeValueTextNum8(), 
	 * writeValueTextLongVector(), writeValueTextShortVector(),
	 * writeValueTextDoubleVector(), writeValueTextBoolVector(),
	 * writeValueTextCharVector(), writeValueTextNum8Vector().
	 */
	void writeValue( ofstream& os, bool binary = true );
	/*!
	 * \brief Read value from stream os in binary format or text format.
	 * \param is  interface to input stream.
	 * \param binary  binary format or not.
	 * \sa readValueBinLong(), readValueBinShort(), readValueBinDouble(), 
	 * readValueBinBool(), readValueBinChar(), readValueBinNum8(), 
	 * readValueBinLongVector(), readValueBinShortVector(),
	 * readValueBinDoubleVector(), readValueBinBoolVector(),
	 * readValueBinCharVector(), readValueBinNum8Vector(),
	 * readValueTextLong(), readValueTextShort(), readValueTextDouble(), 
	 * readValueTextBool(), readValueTextChar(), readValueTextNum8(), 
	 * readValueTextLongVector(), readValueTextShortVector(),
	 * readValueTextDoubleVector(), readValueTextBoolVector(),
	 * readValueTextCharVector(), readValueTextNum8Vector().
	 */
	void readValue( ifstream& is, bool binary = true );
	
	void writeValue( stringstream & os );
	void readValue( stringstream & is );
	///@}

protected:
	/** @name Functions to write variables to output stream in binary format. */
	///@{
	/*!
	 * \brief write long variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinLong( ofstream& os );
	/*!
	 * \brief write short variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinShort( ofstream& os );
	/*!
	 * \brief write double variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinDouble( ofstream& os );
	/*!
	 * \brief write bool variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinBool( ofstream& os );
	/*!
	 * \brief write char variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinChar( ofstream& os );
	/*!
	 * \brief write num8 variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinNum8( ofstream& os );
	/*!
	 * \brief write long vector variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinLongVector( ofstream& os );
	/*!
	 * \brief write short vector variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinShortVector( ofstream& os );
	/*!
	 * \brief write double vector variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinDoubleVector( ofstream& os );
	/*!
	 * \brief write bool vector variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinBoolVector( ofstream& os );
	/*!
	 * \brief write char vector variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinCharVector( ofstream& os );
	/*!
	 * \brief write num8 vector variable to output stream in binary format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueBinNum8Vector( ofstream& os );
	///@}

	/** @name Functions to write variables to output stream in text format. */
	///@{
	/*!
	 * \brief write long variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextLong( ofstream& os );
	/*!
	 * \brief write short variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextShort( ofstream& os );
	/*!
	 * \brief write double variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextDouble( ofstream& os );
	/*!
	 * \brief write bool variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextBool( ofstream& os );
	/*!
	 * \brief write char variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextChar( ofstream& os );
	/*!
	 * \brief write num8 variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextNum8( ofstream& os );
	/*!
	 * \brief write long vector variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextLongVector( ofstream& os );
	/*!
	 * \brief write short vector variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextShortVector( ofstream& os );
	/*!
	 * \brief write double vector variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextDoubleVector( ofstream& os );
	/*!
	 * \brief write bool vector variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextBoolVector( ofstream & os );
	/*!
	 * \brief write char vector variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextCharVector( ofstream & os );
	/*!
	 * \brief write num8 vector variable to output stream in text format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueTextNum8Vector( ofstream & os );
	///@}

	/** @name Functions to write variables to output stream in string format. */
	///@{
	/*!
	 * \brief write long variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringLong( stringstream& os );
	/*!
	 * \brief write short variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringShort( stringstream& os );
	/*!
	 * \brief write double variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringDouble( stringstream& os );
	/*!
	 * \brief write bool variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringBool( stringstream& os );
	/*!
	 * \brief write char variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringChar( stringstream& os );
	/*!
	 * \brief write num8 variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringNum8( stringstream& os );
	/*!
	 * \brief write long vector variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringLongVector( stringstream& os );
	/*!
	 * \brief write short vector variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringShortVector( stringstream& os );
	/*!
	 * \brief write double vector variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringDoubleVector( stringstream& os );
	/*!
	 * \brief write bool vector variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringBoolVector( stringstream & os );
	/*!
	 * \brief write char vector variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringCharVector( stringstream & os );
	/*!
	 * \brief write num8 vector variable to output stream in string format.
	 * \param os interface to output stream.
	 * \sa writeValue().
	 */
	void writeValueStringNum8Vector( stringstream & os );
	///@}

	/** @name Functions to read variables from input stream in binary format. */
	///@{
	/*!
	 * \brief read long variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinLong( ifstream& is );
	/*!
	 * \brief read short variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinShort( ifstream& is );
	/*!
	 * \brief read double variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinDouble( ifstream& is );
	/*!
	 * \brief read bool variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinBool( ifstream& is );
	/*!
	 * \brief read char variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinChar( ifstream& is );
	/*!
	 * \brief read num8 variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinNum8( ifstream& is );
	/*!
	 * \brief read long vector variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinLongVector( ifstream& is );
	/*!
	 * \brief read short vector variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinShortVector( ifstream& is );
	/*!
	 * \brief read double vector variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinDoubleVector( ifstream& is );
	/*!
	 * \brief read bool vector variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinBoolVector( ifstream& is );
	/*!
	 * \brief read char vector variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinCharVector( ifstream& is );
	/*!
	 * \brief read num8 vector variable from input stream in binary format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueBinNum8Vector( ifstream& is );
	///@}

	/** @name Functions to read variables from input stream in text format */
	///@{
	/*!
	 * \brief read long variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextLong( ifstream& is );
	/*!
	 * \brief read short variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextShort( ifstream& is );
	/*!
	 * \brief read double variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextDouble( ifstream& is );
	/*!
	 * \brief read bool variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextBool( ifstream& is );
	/*!
	 * \brief read char variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextChar( ifstream& is );
	/*!
	 * \brief read num8 variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextNum8( ifstream& is );
	/*!
	 * \brief read long vector variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextLongVector( ifstream& is );
	/*!
	 * \brief read short vector variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextShortVector( ifstream& is );
	/*!
	 * \brief read double vector variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextDoubleVector( ifstream& is );
	/*!
	 * \brief read bool vector variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextBoolVector( ifstream & is );
	/*!
	 * \brief read char vector variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextCharVector( ifstream & is );
	/*!
	 * \brief read num8 vector variable from input stream in text format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueTextNum8Vector( ifstream & is );
	///@}

	/** @name Functions to read variables from input stream in string format */
	///@{
	/*!
	 * \brief read long variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringLong( stringstream & is );
	/*!
	 * \brief read short variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringShort( stringstream& is );
	/*!
	 * \brief read double variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringDouble( stringstream& is );
	/*!
	 * \brief read bool variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringBool( stringstream& is );
	/*!
	 * \brief read char variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringChar( stringstream& is );
	/*!
	 * \brief read num8 variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringNum8( stringstream& is );
	/*!
	 * \brief read long vector variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringLongVector( stringstream& is );
	/*!
	 * \brief read short vector variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringShortVector( stringstream& is );
	/*!
	 * \brief read double vector variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringDoubleVector( stringstream& is );
	/*!
	 * \brief read bool vector variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringBoolVector( stringstream & is );
	/*!
	 * \brief read char vector variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringCharVector( stringstream & is );
	/*!
	 * \brief read num8 vector variable from input stream in string format
	 * \param is interface to input stream.
	 * \sa readValue().
	 */
	void readValueStringNum8Vector( stringstream & is );
	///@}
};

/**
 * @ingroup datafileinterface
 * @brief Item in data file interface, which contains several variables. These
 * varialbes form a record in data file.
 *
 * \sa DataFormatItem, IDataFileStream, ODataFileStream
 */
class EsyDataListItem : public vector< EsyDataFormatItem >
{
public:	
	/*!
	 * \brief Constructs an empty item.
	 */
	EsyDataListItem() :
		vector< EsyDataFormatItem >()
	{}

	~EsyDataListItem()
	{
		vector< EsyDataFormatItem > t;
		swap( t );
	}

	/** @name Functions to insert non-vector variables into item. */
	///@{
	/*!
	 * \brief Insert long varialbe into item.
	 * \param var Pointer to long variable.
	 * \sa insertFixedVariable()
	 */
	void insertLong( long * var );
	/*!
	 * \brief Insert short varialbe into item.
	 * \param var Pointer to short variable.
	 * \sa insertFixedVariable()
	 */
	void insertShort( short * var );
	/*!
	 * \brief Insert double varialbe into item.
	 * \param var Pointer to double variable.
	 * \sa insertFixedVariable()
	 */
	void insertDouble( double * var );
	/*!
	 * \brief Insert bool varialbe into item.
	 * \param var Pointer to bool variable.
	 * \sa insertFixedVariable()
	 */
	void insertBool( bool * var );
	/*!
	 * \brief Insert char varialbe into item.
	 * \param var Pointer to char variable.
	 * \sa insertFixedVariable()
	 */
	void insertChar( char * var );
	/*!
	 * \brief Insert NUM8 varialbe into item.
	 * \param var Pointer to NUM8 variable.
	 * \sa insertFixedVariable()
	 */
	void insertNum8( char * var );
	///@}

	/** @name Functions to insert constant length vector variables into item. */
	///@{
	/*!
	 * \brief Insert long vector variables with specified size into item.
	 * \param var Pointer to long vector variable.
	 * \param size Size of vector.
	 * \sa insertFixedVariable()
	 */
	void insertLongVector( vector<long> * var, long size );
	/*!
	 * \brief Insert short vector variables with specified size into item.
	 * \param var Pointer to short vector variable.
	 * \param size Size of vector.
	 * \sa insertFixedVariable()
	 */
	void insertShortVector( vector<short> * var, long size );
	/*!
	 * \brief Insert double vector variables with specified size into item.
	 * \param var Pointer to double vector variable.
	 * \param size Size of vector.
	 * \sa insertFixedVariable()
	 */
	void insertDoubleVector( vector<double> * var, long size );
	/*!
	 * \brief Insert bool vector variables with specified size into item.
	 * \param var Pointer to bool vector variable.
	 * \param size Size of vector.
	 * \sa insertFixedVariable()
	 */
	void insertBoolVector( vector<bool> * var, long size );
	/*!
	 * \brief Insert char vector variables with specified size into item.
	 * \param var Pointer to char vector variable.
	 * \param size Size of vector.
	 * \sa insertFixedVariable()
	 */
	void insertCharVector( vector<char> * var, long size );
	/*!
	 * \brief Insert NUM8 vector variables with specified size into item.
	 * \param var Pointer to NUM8 vector variable.
	 * \param size Size of vector.
	 * \sa insertFixedVariable()
	 */
	void insertNum8Vector( vector<char> * var, long size );
	///@}

	/** @name Functions to insert variable length vector variables into item. */
	///@{
	/*!
	 * \brief insert long vector variables with variable size into item
	 * \param var Pointer to long vector variables.
	 * \param end End character at the end of values.
	 * \sa insertVariableVector()
	 */
	void insertLongVector( vector<long> * var, char end );
	/*!
	 * \brief insert short vector variables with variable size into item
	 * \param var Pointer to short vector variables.
	 * \param end End character at the end of values.
	 * \sa insertVariableVector()
	 */
	void insertShortVector( vector<short> * var, char end );
	/*!
	 * \brief insert double vector variables with variable size into item
	 * \param var Pointer to double vector variables.
	 * \param end End character at the end of values.
	 * \sa insertVariableVector()
	 */
	void insertDoubleVector( vector<double> * var, char end );
	/*!
	 * \brief insert bool vector variables with variable size into item
	 * \param var Pointer to bool vector variables.
	 * \param end End character at the end of values.
	 * \sa insertVariableVector()
	 */
	void insertBoolVector( vector<bool> * var, char end );
	/*!
	 * \brief insert char vector variables with variable size into item
	 * \param var Pointer to char vector variables.
	 * \param end End character at the end of values.
	 * \sa insertVariableVector()
	 */
	void insertCharVector( vector<char> * var, char end );
	/*!
	 * \brief insert NUM8 vector variables with variable size into item
	 * \param var Pointer to NUM8 vector variables.
	 * \param end End character at the end of values.
	 * \sa insertVariableVector()
	 */
	void insertNum8Vector( vector<char> * var, char end );
	///@}

	/** @name Functions to write/read variables to/from stream. */
	///@{
	/*!
	 * \brief Write values of item to stream os in binary format or text format.
	 * \param os  interface to output stream.
	 * \param binary  binary format or not.
	 */
	void writeItem( ofstream& os, bool binary = true );
	/*!
	 * \brief Read values of item from stream os in binary format or text 
	 * format.
	 * \param is  interface to input stream.
	 * \param binary  binary format or not.
	 */
	void readItem( ifstream& is, bool binary = true );
	///@}
	
	string writeItem();
	void readItem( const string & is );

protected:
	/*!
	 * \brief Insert variables with fixed size. If size is 1, a non-vector
	 * variable is inserted.
	 * \param type  data type
	 * \param var   pointer to variable
	 * \param size  size of variable
	 * \sa
	 * insertLong(), insertShort(), insertDouble(), insertBool(), insertChar(), 
	 * insertNum8(),
	 * insertLongVector(vector<long> *var, long size),
	 * insertShortVector(vector<short> *var, long size),
	 * insertDoubleVector(vector<double> *var, long size),
	 * insertBoolVector(vector<bool> *var, long size),
	 * insertCharVector(vector<char> *var, long size),
	 * insertNum8Vector(vector<char> *var, long size)
	 */
	void insertFixedVariable(
		EsyDataFormatItem::DataType type, void * var, long size);
	/*!
	 * \brief Insert variables with variables size.
	 * \param type  data type
	 * \param var   pointer to variable
	 * \param end   end character of values
	 * \sa
	 * insertLongVector(vector<long> *var, char end),
	 * insertShortVector(vector<short> *var, char end),
	 * insertDoubleVector(vector<double> *var, char end),
	 * insertBoolVector(vector<bool> *var, char end),
	 * insertCharVector(vector<char> *var, char end),
	 * insertNum8Vector(vector<char> *var, char end)
	 */
	void insertVariableVector(
		EsyDataFormatItem::DataType type, void * var, char end);
};

#endif

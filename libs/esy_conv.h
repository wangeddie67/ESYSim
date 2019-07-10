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

#ifndef INTERFACE_CONV_H
#define INTERFACE_CONV_H

#include <string>

using namespace std;

/*!
 * \brief Converts strings to other types or converts other typs to strings.
 *
 * To convert one variable type to another, use should create a new entity with
 * the variable to covert. By assigned Conv to any variable, the type can be
 * converted antumatically.
 */
class EsyConvert
{
private:
    string m_str; /**< \brief temporay value to convert */

public:
    /** @name Constructor
      *
      * Converts from string, int, double, long to string.
      */
    ///@{
    /*!
     * \brief Constructs an entity from string.
     * \param str value to convert.
     */
	EsyConvert(const string & str);
    /*!
     * \brief Constructs an entity from int.
     * \param dbl value to convert.
     */
	EsyConvert(const int & dbl);
    /*!
     * \brief Constructs an entity from double.
     * \param dbl value to convert.
     */
	EsyConvert(const double & dbl);
    /*!
     * \brief Constructs an entity from long.
     * \param dbl value to convert.
     */
	EsyConvert(const long & dbl);
    ///@}

    /** @name Convert string to other types
      *
      * Converts from string to bool, signed char, unsigned char, short,
      * unsigned short, int, unsigned int, long, unsigned lont, float, double,
      * string and char*.
      */
    ///@{
    /*!
     * \brief Converts to bool type.
     */
    operator bool();
    /*!
     * \brief Converts to signed char type.
     */
    operator signed char();
    /*!
     * \brief Converts to unsigned char type.
     */
    operator unsigned char();
    /*!
     * \brief Converts to short type.
     */
    operator short();
    /*!
     * \brief Converts to unsigned short type.
     */
    operator unsigned short();
    /*!
     * \brief Converts to int type.
     */
    operator int();
    /*!
     * \brief Converts to unsigned int type.
     */
    operator unsigned();
    /*!
     * \brief Converts to long type.
     */
    operator long();
    /*!
     * \brief Converts to unsigned long type.
     */
    operator unsigned long();
    /*!
     * \brief Converts to long long type.
     */
    operator long long();
    /*!
     * \brief Converts to unsigned long long type.
     */
    operator unsigned long long();
    /*!
     * \brief Converts to float type.
     */
    operator float();
    /*!
     * \brief Converts to double type.
     */
    operator double();
    /*!
     * \brief Converts to string type.
     */
    operator string();
    /*!
     * \brief Converts to char* type.
     * \return  pointer to char list.
     */
    const char* c_str() const { return m_str.c_str(); }
    ///@}
};

#endif

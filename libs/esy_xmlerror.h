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

#ifndef INTERFACE_XML_ERROR_H
#define INTERFACE_XML_ERROR_H

#include <string>
#include <vector>

/*!
 * \brief Error information interface to handle XML errors.
 */
class EsyXmlError
{
public:
    /*!
     * \brief error code
     */
    enum ErrorCode{
        NONEERROR,      /**< no error */
        FILEOPENERROR,  /**< fail to not open file */
        DOMERROR,       /**< fail to parse XML file */
        XMLTAGERROR     /**< xml tag is not correct */
    };

protected:
    ErrorCode m_error_type;    /**< \brief error code */
    std::vector< std::string > m_error_msg;  /**< \brief error messages */
    std::string m_error_class;          /**< \brief error class */
    std::string m_error_fun;            /**< \brief error function */

public:
    /*! @name Constructor */
    ///@{
    /*!
     * \brief constructs a structure for no error.
     */
    EsyXmlError()
        : m_error_type( NONEERROR )
        , m_error_msg()
        , m_error_class()
        , m_error_fun()
    {}
    /*!
     * \brief constructs a structure for one error.
     * \param err         error code
     * \param msg         error messages
     * \param pare_class  class where error occurs
     * \param pare_fun    function where error occurs
     */
    EsyXmlError( ErrorCode err, const std::vector< std::string > & msg, const std::string & pare_class, const std::string & pare_fun )
        : m_error_type( err )
        , m_error_msg( msg )
        , m_error_class( pare_class )
        , m_error_fun( pare_fun )
    {}
    /*!
     * \brief constructs a structure for one file error.
     * \param file        direction of error file
     * \param pare_class  class where error occurs
     * \param pare_fun    function where error occurs
     */
    EsyXmlError( const std::string & file, const std::string & pare_class, const std::string & pare_fun )
        : m_error_type( FILEOPENERROR )
        , m_error_msg()
        , m_error_class( pare_class )
        , m_error_fun( pare_fun )
    {
        m_error_msg.push_back( file );
    }
    /*!
     * \brief constructs a structure for one XML parse error.
     * \param desc        direction of error
     * \param file        direction of error file
     * \param row         row of error position in file
     * \param col         column of error position in file
     * \param pare_class  class where error occurs
     * \param pare_fun    function where error occurs
     */
    EsyXmlError( const std::string & desc, const std::string & file, const std::string & row, const std::string & col
                , const std::string & pare_class, const std::string & pare_fun )
        : m_error_type( DOMERROR )
        , m_error_msg()
        , m_error_class( pare_class )
        , m_error_fun( pare_fun )
    {
        m_error_msg.push_back( desc );
        m_error_msg.push_back( file );
        m_error_msg.push_back( row );
        m_error_msg.push_back( col );
    }
    /*!
     * \brief constructs a structure for XML tag error.
     * \param tag         error tag
     * \param file        direction of error file
     * \param pare_class  class where error occurs
     * \param pare_fun    function where error occurs
     */
    EsyXmlError( const std::string & tag, const std::string & file, const std::string & pare_class, const std::string & pare_fun )
        : m_error_type( XMLTAGERROR )
        , m_error_msg()
        , m_error_class( pare_class )
        , m_error_fun( pare_fun )
    {
        m_error_msg.push_back( tag );
        m_error_msg.push_back( file );
    }
    ///@}

    /*! Function to access variables */
    ///@{
    /*!
     * \brief Access code of error #m_error_type.
     * \return  code of error #m_error_type.
     */
    ErrorCode errorType() const { return m_error_type; }
    /*!
     * \brief Access error message #m_error_msg.
     * \return error message #m_error_msg.
     */
    const std::vector< std::string > & errorMsg() const { return m_error_msg; }
    /*!
     * \brief Access error class #m_error_class.
     * \return error class #m_error_class.
     */
    const std::string & errorClass() const { return m_error_class; }
    /*!
     * \brief Access error function #m_error_fun.
     * \return error function #m_error_fun.
     */
    const std::string & errorFun() const { return m_error_fun; }
    ///@}

    /*!
     * \brief Check the error code.
     * \return If error code is NoError, return false; otherwise, return true.
     */
    bool hasError() { return !( m_error_type == NONEERROR ); }
    /*!
     * \brief generate the error title.
     * \return error title.
     */
    std::string title();
    /*!
     * \brief generate the error text.
     * \return  error text.
     */
    std::string text();
};

#endif

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

#include "esy_xmlerror.h"

string EsyXmlError::title()
{
    if ( m_error_type == NONEERROR )
        return string( "No Error" );
    else if ( m_error_type == FILEOPENERROR )
        return string( "File Open Error" );
    else if ( m_error_type == DOMERROR )
        return string( "XML Dom Error" );
    else if ( m_error_type == XMLTAGERROR )
        return string( "XML Tag Error" );
    else
        return string( "Unknown Error" );
}

string EsyXmlError::text()
{
    if ( m_error_type == NONEERROR )
    {
        return string( "Process success." );
    }
    else if ( m_error_type == FILEOPENERROR )
    {
        if ( m_error_msg .size() < 1 )
        {
            return string( "Error information is not complete." );
        }
        else
        {
            return string( "Can not open file " + m_error_msg[ 0 ] + "\n" +
                "Position: " + m_error_class + "::" + m_error_fun );
        }
    }
    else if ( m_error_type == DOMERROR )
    {
        if ( m_error_msg .size() < 4 )
        {
            return string( "Error information is not complete." );
        }
        else
        {
            return string( "Word \"" + m_error_msg[ 1 ] + "\"in file " +
                    m_error_msg[0] + ", line:" + m_error_msg[2] + " Col:" +
                    m_error_msg[3] + "\n" +
                    "Position: " + m_error_class + "::" + m_error_fun );
        }
    }
    else if ( m_error_type == XMLTAGERROR )
    {
        if ( m_error_msg .size() < 2 )
        {
            return string( "Error information is not complete." );
        }
        else
        {
            return string( "Tag \"" + m_error_msg[ 1 ] + "\"in file " +
                    m_error_msg[0] + "\n" +
                "Position: " + m_error_class + "::" + m_error_fun );
        }
    }
    else
    {
        string t_str( "Error informations: \n" );
        for ( size_t i = 0; i < m_error_msg .size(); i ++ )
        {
            t_str += m_error_msg[ i ] + "\n";
        }
        return t_str;
    }
}


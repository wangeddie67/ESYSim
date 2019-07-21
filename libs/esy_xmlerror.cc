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

std::string EsyXmlError::title()
{
    switch ( m_error_type )
    {
        case NONEERROR:     return std::string( "No Error" ); break;
        case FILEOPENERROR: return std::string( "File Open Error" ); break;
        case DOMERROR:      return std::string( "XML Dom Error" ); break;
        case XMLTAGERROR:   return std::string( "XML Tag Error" ); break;
        default: return std::string( "Unknown Error" ); break;
    }
}

std::string EsyXmlError::text()
{
    switch ( m_error_type )
    {
        case NONEERROR:
            return std::string( "Process success." );
            break;
        case FILEOPENERROR:
            if ( m_error_msg.size() < 1 )
            {
                return std::string( "Error information is not complete." );
            }
            else
            {
                return std::string( "Can not open file " + m_error_msg[ 0 ] + "\n" + "Position: " + m_error_class + "::" + m_error_fun );
            }
            break;
        case DOMERROR:
            if ( m_error_msg.size() < 4 )
            {
                return std::string( "Error information is not complete." );
            }
            else
            {
                return std::string( 
                    "Word \"" + m_error_msg[ 1 ] + "\"in file " + m_error_msg[0] + ", line:" + m_error_msg[2] + " Col:" + m_error_msg[3] + "\n" +
                    "Position: " + m_error_class + "::" + m_error_fun );
            }
            break;
        case XMLTAGERROR:
            if ( m_error_msg .size() < 2 )
            {
                return std::string( "Error information is not complete." );
            }
            else
            {
                return std::string( "Tag \"" + m_error_msg[ 1 ] + "\"in file " + m_error_msg[0] + "\n" +
                    "Position: " + m_error_class + "::" + m_error_fun );
            }
            break;
        default:
            {
                std::string t_str( "Error informations: \n" );
                for ( size_t i = 0; i < m_error_msg .size(); i ++ )
                {
                    t_str += m_error_msg[ i ] + "\n";
                }
                return t_str;
            }
            break;
    }
}


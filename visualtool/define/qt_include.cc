#include "qt_include.h"

QString global_work_direction = "../example";

QString ErrorHandler::title()
{
    if ( m_error_type == NONEERROR )
    {
        return QString( "No Error" );
    }
    else if ( m_error_type == FILEOPENERROR )
    {
        return QString( "File Open Error" );
    }
    else if ( m_error_type == DOMERROR )
    {
        return QString( "XML Dom Error" );
    }
    else if ( m_error_type == XMLTAGERROR )
    {
        return QString( "XML Tag Error" );
    }
    else
    {
        return QString( "Unknown Error" );
    }
}

QString ErrorHandler::text()
{
    if ( m_error_type == NONEERROR )
    {
        return QString( "Process success." );
    }
    else if ( m_error_type == FILEOPENERROR )
    {
        if ( m_error_msg .size() < 1 )
        {
            return QString( "Error information is not complete." );
        }
        else
        {
            return QString( "Can not open file " + m_error_msg[ 0 ] + "\n" +
                    "Position: " + m_error_class + "::" + m_error_fun );
        }
    }
    else if ( m_error_type == DOMERROR )
    {
        if ( m_error_msg .size() < 4 )
        {
            return QString( "Error information is not complete." );
        }
        else
        {
            return QString( "Word \"" + m_error_msg[ 1 ] +
                "\"in file " + m_error_msg[0] + ", line:" + m_error_msg[2] +
                " Col:" + m_error_msg[3] + "\n" +
                "Position: " + m_error_class + "::" + m_error_fun );
        }
    }
    else if ( m_error_type == XMLTAGERROR )
    {
        if ( m_error_msg .size() < 2 )
        {
            return QString( "Error information is not complete." );
        }
        else
        {
            return QString( "Tag \"" + m_error_msg[ 1 ] +
                "\"in file " + m_error_msg[0] + "\n" +
                "Position: " + m_error_class + "::" + m_error_fun );
        }
    }
    else
    {
        QString t_str( "Error informations: \n" );
        for ( int i = 0; i < m_error_msg .size(); i ++ )
        {
            t_str += m_error_msg[ i ] + "\n";
        }
        return t_str;
    }
}

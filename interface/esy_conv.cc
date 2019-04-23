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

#include "esy_conv.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

template <typename T>
T convert(const string & s)
{
    stringstream iss( s.c_str() );
    double dbl;
    iss >> dbl;

    if ( iss.bad() )
    {
        cerr << "variable convert fault. variable is \"" << s << "\"."
            << endl;
        exit( 0 );
    }

    return static_cast< T >( dbl );
}

template <typename T>
string antiConvert(const T & dbl)
{
    stringstream iss;
    iss << dbl;
    string s;
    iss >> s;

    if ( iss.bad() )
    {
        cerr << "variable convert fault. variable is \"" << dbl << "\"."
            << endl;
        exit( 0 );
    }

    return s;
}

EsyConvert::EsyConvert(const string & str) :
    m_str( str )
{}

EsyConvert::EsyConvert(const int &dbl) :
    m_str( antiConvert< int >( dbl ) )
{}

EsyConvert::EsyConvert(const double &dbl) :
    m_str( antiConvert< double >( dbl ) )
{}

EsyConvert::EsyConvert(const long &dbl) :
    m_str( antiConvert< long >( dbl ) )
{}

EsyConvert::operator bool()
{ 
	double t = EsyConvert(m_str);
    if ( t == 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

EsyConvert::operator signed char()
{ 
    return convert< char >( m_str );
}

EsyConvert::operator unsigned char()
{ 
    return convert< unsigned char >( m_str );
}

EsyConvert::operator short()
{ 
    return convert< short >( m_str );
}

EsyConvert::operator unsigned short()
{ 
    return convert< unsigned short >( m_str );
}

EsyConvert::operator int()
{ 
    return convert< int >( m_str );
}

EsyConvert::operator unsigned()
{ 
    return convert< unsigned >( m_str );
}

EsyConvert::operator long()
{ 
    return convert< long >( m_str );
}

EsyConvert::operator unsigned long()
{ 
    return convert< unsigned long >( m_str );
}

EsyConvert::operator long long()
{
    return convert< long long >( m_str );
}

EsyConvert::operator unsigned long long()
{
    return convert< unsigned long long >( m_str );
}

EsyConvert::operator float()
{ 
    return convert< float >( m_str );
}

EsyConvert::operator double()
{ 
    return convert< double >( m_str );
}

EsyConvert::operator string()
{
    return m_str;
}



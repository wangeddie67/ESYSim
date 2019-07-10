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

#include "esy_iodatafile.h"

void EsyDataFormatItem::writeValue( ofstream& os, bool binary )
{
	if ( binary )
	{
		switch( m_type )
		{
			case LONG_TYPE		: writeValueBinLong( os ); break;
			case SHORT_TYPE		: writeValueBinShort( os ); break;
			case DOUBLE_TYPE	: writeValueBinDouble( os ); break;
			case BOOL_TYPE		: writeValueBinBool( os ); break;
			case CHAR_TYPE		: writeValueBinChar( os ); break;
			case NUM8_TYPE		: writeValueBinNum8( os ); break;
			case LONG_VECTOR_TYPE	: writeValueBinLongVector( os ); break;
			case SHORT_VECTOR_TYPE	: writeValueBinShortVector( os ); break;
			case DOUBLE_VECTOR_TYPE	: writeValueBinDoubleVector( os ); break;
			case BOOL_VECTOR_TYPE	: writeValueBinBoolVector( os ); break;
			case CHAR_VECTOR_TYPE	: writeValueBinCharVector( os ); break;
			case NUM8_VECTOR_TYPE	: writeValueBinNum8Vector( os ); break;
		}
	}
	else
	{
		switch( m_type )
		{
			case LONG_TYPE		: writeValueTextLong( os ); break;
			case SHORT_TYPE		: writeValueTextShort( os ); break;
			case DOUBLE_TYPE	: writeValueTextDouble( os ); break;
			case BOOL_TYPE		: writeValueTextBool( os ); break;
			case CHAR_TYPE		: writeValueTextChar( os ); break;
			case NUM8_TYPE		: writeValueTextNum8( os ); break;
			case LONG_VECTOR_TYPE	: writeValueTextLongVector( os ); break;
			case SHORT_VECTOR_TYPE	: writeValueTextShortVector( os ); break;
			case DOUBLE_VECTOR_TYPE	: writeValueTextDoubleVector( os ); break;
			case BOOL_VECTOR_TYPE	: writeValueTextBoolVector( os ); break;
			case CHAR_VECTOR_TYPE	: writeValueTextCharVector( os ); break;
			case NUM8_VECTOR_TYPE	: writeValueTextNum8Vector( os ); break;
		}
	}
}
// write in binary format
#define writeValueBinSingle( Name, ArguT ) \
void EsyDataFormatItem::writeValueBin##Name( ofstream& os ) \
{ \
    os.write( (char *)mp_var, sizeof( ArguT ) ); \
}
writeValueBinSingle( Long, long )
writeValueBinSingle( Short, short )
writeValueBinSingle( Double, double )
writeValueBinSingle( Bool, bool )
writeValueBinSingle( Char, char )
writeValueBinSingle( Num8, char )
#undef writeValueBinSingle

#define writeValueBinVector( Name, ArguT ) \
void EsyDataFormatItem::writeValueBin##Name##Vector( ofstream& os ) \
{ \
	vector< ArguT > *tp_var = (vector< ArguT > *)mp_var; \
    for ( int i = (int)tp_var->size(); i < m_size; i ++ ) \
    { \
        tp_var->push_back( (ArguT)-1 ); \
    } \
    for ( int i = 0; i < m_size; i ++ ) \
    { \
        ArguT t_v = tp_var->at( i ); \
        os.write( (char *)( &t_v ), sizeof( ArguT ) ); \
    } \
}
writeValueBinVector( Long, long )
writeValueBinVector( Short, short )
writeValueBinVector( Double, double )
writeValueBinVector( Bool, bool )
writeValueBinVector( Char, char )
writeValueBinVector( Num8, char )
#undef writeValueBinVector

#define writeValueTextSingle( Name, ArguT, TempT ) \
void EsyDataFormatItem::writeValueText##Name( ofstream& os ) \
{ \
    TempT t_var = *( (ArguT *)mp_var ); \
    os << t_var << "\t"; \
}
writeValueTextSingle( Long, long, long )
writeValueTextSingle( Short, short, short )
writeValueTextSingle( Double, double, double )
writeValueTextSingle( Bool, bool, bool )
writeValueTextSingle( Char, char, char )
writeValueTextSingle( Num8, char, int )
#undef writeValueTextSingle

#define writeValueTextVector( Name, ArguT, TempT ) \
void EsyDataFormatItem::writeValueText##Name##Vector( ofstream& os ) \
{ \
    vector< ArguT > *tp_var = (vector< ArguT > *)mp_var; \
    for ( int i = (int)tp_var->size(); i < m_size; i ++ ) \
    { \
        tp_var->push_back( (ArguT)-1 ); \
    } \
    m_size = (int)tp_var->size(); \
    os << (TempT)tp_var->at( 0 ); \
    for ( int i = 1; i < m_size; i ++ ) \
    { \
        os << '\t' << (TempT)tp_var->at( i ); \
    } \
	if (m_is_var_vector)\
    { \
        os << '\t' << m_end << '\t';\
    } \
	else\
    { \
        os << '\t'; \
    } \
}
writeValueTextVector( Long, long, long )
writeValueTextVector( Short, short, short )
writeValueTextVector( Double, double, double )
writeValueTextVector( Bool, bool, bool )
writeValueTextVector( Char, char, char )
writeValueTextVector( Num8, char, int )
#undef writeValueTextVector

void EsyDataFormatItem::writeValue( stringstream& os )
{
	switch( m_type )
	{
		case LONG_TYPE		: writeValueStringLong( os ); break;
		case SHORT_TYPE		: writeValueStringShort( os ); break;
		case DOUBLE_TYPE	: writeValueStringDouble( os ); break;
		case BOOL_TYPE		: writeValueStringBool( os ); break;
		case CHAR_TYPE		: writeValueStringChar( os ); break;
		case NUM8_TYPE		: writeValueStringNum8( os ); break;
		case LONG_VECTOR_TYPE	: writeValueStringLongVector( os ); break;
		case SHORT_VECTOR_TYPE	: writeValueStringShortVector( os ); break;
		case DOUBLE_VECTOR_TYPE	: writeValueStringDoubleVector( os ); break;
		case BOOL_VECTOR_TYPE	: writeValueStringBoolVector( os ); break;
		case CHAR_VECTOR_TYPE	: writeValueStringCharVector( os ); break;
		case NUM8_VECTOR_TYPE	: writeValueStringNum8Vector( os ); break;
	}
}

#define writeValueStringSingle( Name, ArguT, TempT ) \
void EsyDataFormatItem::writeValueString##Name( stringstream& os ) \
{ \
    TempT t_var = *( (ArguT *)mp_var ); \
    os << t_var << "\t"; \
}
writeValueStringSingle( Long, long, long )
writeValueStringSingle( Short, short, short )
writeValueStringSingle( Double, double, double )
writeValueStringSingle( Bool, bool, bool )
writeValueStringSingle( Char, char, char )
writeValueStringSingle( Num8, char, int )
#undef writeValueStringSingle

#define writeValueStringVector( Name, ArguT, TempT ) \
void EsyDataFormatItem::writeValueString##Name##Vector( stringstream& os ) \
{ \
    vector< ArguT > *tp_var = (vector< ArguT > *)mp_var; \
    for ( int i = (int)tp_var->size(); i < m_size; i ++ ) \
    { \
        tp_var->push_back( (ArguT)-1 ); \
    } \
    m_size = (int)tp_var->size(); \
    os << (TempT)tp_var->at( 0 ); \
    for ( int i = 1; i < m_size; i ++ ) \
    { \
        os << '\t' << (TempT)tp_var->at( i ); \
    } \
	if (m_is_var_vector)\
    { \
        os << '\t' << m_end << '\t';\
    } \
	else\
    { \
        os << '\t'; \
    } \
}
writeValueStringVector( Long, long, long )
writeValueStringVector( Short, short, short )
writeValueStringVector( Double, double, double )
writeValueStringVector( Bool, bool, bool )
writeValueStringVector( Char, char, char )
writeValueStringVector( Num8, char, int )
#undef writeValueStringVector

void EsyDataFormatItem::readValue( ifstream& is, bool binary )
{
	if ( binary )
	{
		switch( m_type )
		{
			case LONG_TYPE		: readValueBinLong( is ); break;
			case SHORT_TYPE		: readValueBinShort( is ); break;
			case DOUBLE_TYPE	: readValueBinDouble( is ); break;
			case BOOL_TYPE		: readValueBinBool( is ); break;
			case CHAR_TYPE		: readValueBinChar( is ); break;
			case NUM8_TYPE		: readValueBinNum8( is ); break;
			case LONG_VECTOR_TYPE	: readValueBinLongVector( is ); break;
			case SHORT_VECTOR_TYPE	: readValueBinShortVector( is ); break;
			case DOUBLE_VECTOR_TYPE	: readValueBinDoubleVector( is ); break;
			case BOOL_VECTOR_TYPE	: readValueBinBoolVector( is ); break;
			case CHAR_VECTOR_TYPE	: readValueBinCharVector( is ); break;
			case NUM8_VECTOR_TYPE	: readValueBinNum8Vector( is ); break;
		}
	}
	else
	{
		switch( m_type )
		{
			case LONG_TYPE		: readValueTextLong( is ); break;
			case SHORT_TYPE		: readValueTextShort( is ); break;
			case DOUBLE_TYPE	: readValueTextDouble( is ); break;
			case BOOL_TYPE		: readValueTextBool( is ); break;
			case CHAR_TYPE		: readValueTextChar( is ); break;
			case NUM8_TYPE		: readValueTextNum8( is ); break;
			case LONG_VECTOR_TYPE	: readValueTextLongVector( is ); break;
			case SHORT_VECTOR_TYPE	: readValueTextShortVector( is ); break;
			case DOUBLE_VECTOR_TYPE	: readValueTextDoubleVector( is ); break;
			case BOOL_VECTOR_TYPE	: readValueTextBoolVector( is ); break;
			case CHAR_VECTOR_TYPE	: readValueTextCharVector( is ); break;
			case NUM8_VECTOR_TYPE	: readValueTextNum8Vector( is ); break;
		}
	}
}

#define readValueBinSingle(Name, ArguT) \
void EsyDataFormatItem::readValueBin##Name( ifstream& is ) \
{ \
    is.read( (char*)mp_var, sizeof( ArguT ) ); \
}
readValueBinSingle( Long, long )
readValueBinSingle( Short, short )
readValueBinSingle( Double, double )
readValueBinSingle( Bool, bool )
readValueBinSingle( Char, char )
readValueBinSingle( Num8, char )
#undef readValueBinSingle

#define readValueBinVector(Name, ArguT) \
void EsyDataFormatItem::readValueBin##Name##Vector( ifstream& is ) \
{ \
    vector< ArguT > *tp_var = (vector< ArguT > *)mp_var; \
	tp_var ->clear(); \
	for ( int i = 0; i < m_size; i ++ ) \
	{ \
		ArguT t_value; \
		is.read( (char *)( &t_value ), sizeof( ArguT ) ); \
		tp_var ->push_back( t_value ); \
    } \
}
readValueBinVector( Long, long )
readValueBinVector( Short, short )
readValueBinVector( Double, double )
readValueBinVector( Bool, bool )
readValueBinVector( Char, char )
readValueBinVector( Num8, char )
#undef readValueBinVector

#define readValueTextSingle( Name, ArguT, TempT ) \
void EsyDataFormatItem::readValueText##Name( ifstream& is ) \
{ \
    TempT t_var; \
    is >> t_var; \
    *( (ArguT *)mp_var ) = t_var; \
}
readValueTextSingle( Long, long, long )
readValueTextSingle( Short, short, short )
readValueTextSingle( Double, double, double )
readValueTextSingle( Bool, bool, bool )
readValueTextSingle( Char, char, char )
readValueTextSingle( Num8, char, int )
#undef readValueTextSingle

#define readValueTextVector( Name, ArguT, TempT ) \
void EsyDataFormatItem::readValueText##Name##Vector( ifstream& is ) \
{ \
	vector< ArguT > *tp_var = (vector< ArguT > *)mp_var; \
    tp_var->clear(); \
    TempT t_value; \
    if ( !m_is_var_vector ) \
    { \
		for ( int i = 0; i < m_size; i ++ ) \
		{ \
			is >> t_value; \
            tp_var->push_back( t_value ); \
        } \
    } \
    else \
    { \
        char c; \
        for ( ; ; ) \
        { \
            if ( is.eof() ) \
            { \
                break; \
            } \
            c = is.get(); \
            if ( c == '\t' ) \
            { \
                c = is.get(); \
            } \
            if ( c == m_end ) \
            { \
                break; \
            } \
            else \
            { \
                is.unget(); \
                is >> t_value; \
                tp_var->push_back( t_value ); \
            } \
        } \
    } \
}
readValueTextVector( Long, long, long )
readValueTextVector( Short, short, short )
readValueTextVector( Double, double, double )
readValueTextVector( Bool, bool, bool )
readValueTextVector( Char, char, char )
readValueTextVector( Num8, char, int )
#undef readValueTextVector

void EsyDataFormatItem::readValue( stringstream& is )
{
	switch( m_type )
	{
		case LONG_TYPE		: readValueStringLong( is ); break;
		case SHORT_TYPE		: readValueStringShort( is ); break;
		case DOUBLE_TYPE	: readValueStringDouble( is ); break;
		case BOOL_TYPE		: readValueStringBool( is ); break;
		case CHAR_TYPE		: readValueStringChar( is ); break;
		case NUM8_TYPE		: readValueStringNum8( is ); break;
		case LONG_VECTOR_TYPE	: readValueStringLongVector( is ); break;
		case SHORT_VECTOR_TYPE	: readValueStringShortVector( is ); break;
		case DOUBLE_VECTOR_TYPE	: readValueStringDoubleVector( is ); break;
		case BOOL_VECTOR_TYPE	: readValueStringBoolVector( is ); break;
		case CHAR_VECTOR_TYPE	: readValueStringCharVector( is ); break;
		case NUM8_VECTOR_TYPE	: readValueStringNum8Vector( is ); break;
	}
}

#define readValueStringSingle( Name, ArguT, TempT ) \
void EsyDataFormatItem::readValueString##Name( stringstream& is ) \
{ \
    TempT t_var; \
    is >> t_var; \
    *( (ArguT *)mp_var ) = t_var; \
}
readValueStringSingle( Long, long, long )
readValueStringSingle( Short, short, short )
readValueStringSingle( Double, double, double )
readValueStringSingle( Bool, bool, bool )
readValueStringSingle( Char, char, char )
readValueStringSingle( Num8, char, int )
#undef readValueStringSingle

#define readValueStringVector( Name, ArguT, TempT ) \
void EsyDataFormatItem::readValueString##Name##Vector( stringstream& is ) \
{ \
	vector< ArguT > *tp_var = (vector< ArguT > *)mp_var; \
    tp_var->clear(); \
    TempT t_value; \
    if ( !m_is_var_vector ) \
    { \
		for ( int i = 0; i < m_size; i ++ ) \
		{ \
			is >> t_value; \
            tp_var->push_back( t_value ); \
        } \
    } \
    else \
    { \
        char c; \
        for ( ; ; ) \
        { \
            if ( is.eof() ) \
            { \
                break; \
            } \
            c = is.get(); \
            if ( c == '\t' ) \
            { \
                c = is.get(); \
            } \
            if ( c == m_end ) \
            { \
                break; \
            } \
            else \
            { \
                is.unget(); \
                is >> t_value; \
                tp_var->push_back( t_value ); \
            } \
        } \
    } \
}
readValueStringVector( Long, long, long )
readValueStringVector( Short, short, short )
readValueStringVector( Double, double, double )
readValueStringVector( Bool, bool, bool )
readValueStringVector( Char, char, char )
readValueStringVector( Num8, char, int )
#undef readValueStringVector

void EsyDataListItem::insertFixedVariable(
        EsyDataFormatItem::DataType type, void * var, long size )
{
    push_back( EsyDataFormatItem( type, size, var ) );
}
void EsyDataListItem::insertVariableVector(
        EsyDataFormatItem::DataType type, void * var, char end )
{
    push_back( EsyDataFormatItem( type, var, end ) );
}

#define insertVariableSingle( Name, ArguT, Type ) \
void EsyDataListItem::insert##Name( ArguT * var ) \
{ \
    insertFixedVariable( EsyDataFormatItem::Type, (void *)var, 1 ); \
}
insertVariableSingle( Long, long, LONG_TYPE )
insertVariableSingle( Short, short, SHORT_TYPE )
insertVariableSingle( Double, double, DOUBLE_TYPE )
insertVariableSingle( Bool, bool, BOOL_TYPE )
insertVariableSingle( Char, char, CHAR_TYPE )
insertVariableSingle( Num8, char, NUM8_TYPE )
#undef insertVariableSingle

#define insertVariableVector( Name, ArguT, Type ) \
void EsyDataListItem::insert##Name##Vector( vector< ArguT > * var, long size ) \
{ \
    insertFixedVariable( EsyDataFormatItem::Type, (void *)var, size ); \
} \
\
void EsyDataListItem::insert##Name##Vector( vector< ArguT > * var, char end ) \
{ \
    insertVariableVector( EsyDataFormatItem::Type, (void *)var, end ); \
}

insertVariableVector( Long, long, LONG_VECTOR_TYPE )
insertVariableVector( Short, short, SHORT_VECTOR_TYPE )
insertVariableVector( Double, double, DOUBLE_VECTOR_TYPE )
insertVariableVector( Bool, bool, BOOL_VECTOR_TYPE )
insertVariableVector( Char, char, CHAR_VECTOR_TYPE )
insertVariableVector( Num8, char, NUM8_VECTOR_TYPE )
#undef insertVariableVector

void EsyDataListItem::readItem( ifstream& is, bool binary )
{
    for ( size_t i = 0; i < size() ;i ++ )
    {
        at( i ).readValue( is, binary );
    }
}

void EsyDataListItem::writeItem( ofstream& os, bool binary )
{
    for ( size_t i = 0; i < size() ;i ++ )
    {
        at( i ).writeValue( os, binary );
    }
}

void EsyDataListItem::readItem( const string & is )
{
    stringstream ss_is;
    ss_is << is;
    for ( size_t i = 0; i < size() ;i ++ )
    {
        at( i ).readValue( ss_is );
    }
}

string EsyDataListItem::writeItem()
{
    stringstream ss_os;
    for ( size_t i = 0; i < size() ;i ++ )
    {
        at( i ).writeValue( ss_os );
    }
    return ss_os.str();
}

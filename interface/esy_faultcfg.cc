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
#include "esy_faultcfg.h"
#include "iostream"

#include <fstream>

string EsyFaultConfigItem::const_fault_type[ FAULT_TYPE_NUM ] =
	{ "Router", "Port", "Line", "Buffer", "Switch",
	  "Encoder", "Inter Decoder", "Final Decoder" };

EsyFaultConfigItem::EsyFaultConfigItem() :
	EsyDataListItem(),
	m_type( -1 ), m_router( -1 ), m_port( -1 ), m_vc( -1 ), m_bit( -1 ),
	m_state( 1, false ), m_transfer( 1, 1.0 )
{
	insertVariables();
}

EsyFaultConfigItem::EsyFaultConfigItem(
	FaultType type, long router, long port, long vc, long bit,
	const vector< long > & state, const vector< double > & transfer
) :
	EsyDataListItem(), m_type(type),
	m_router( router ), m_port( port ), m_vc( vc ), m_bit( bit ),
	m_state( state ), m_transfer( transfer )
{
	insertVariables();	
}

EsyFaultConfigItem::EsyFaultConfigItem(
	FaultType type, long router, long port, long vc, long bit,
	const vector< long > & state,
	const vector< vector< double > > & transfer_matrix
) :
	EsyDataListItem(), m_type(type),
	m_router( router ), m_port( port ), m_vc( vc ), m_bit( bit ),
	m_state( state ), m_transfer()
{
	for ( size_t i = 0; i <  transfer_matrix.size(); i ++ )
	{
		for ( size_t j = 0; j < transfer_matrix[ i ].size(); j ++ )
		{
			m_transfer.push_back( transfer_matrix[ i ][ j ] );
		}
	}
	insertVariables();
}

EsyFaultConfigItem::EsyFaultConfigItem(const vector< string >& line) :
	EsyDataListItem(),
	m_type(), m_router(), m_port(), m_vc(), m_bit(), m_state(), m_transfer()
{
	m_type = EsyConvert( line[ 0 ] );
	m_router = EsyConvert(line[1]);
	m_port = EsyConvert(line[2]);
	m_vc = EsyConvert(line[3]);
	m_bit = EsyConvert(line[4]);

	size_t i = 5;
	m_state.clear();
	for ( ; i < line.size(); i ++ )
	{
		if ( line[ i ] == string("e") )
		{
			break;
		}
		m_state.push_back( EsyConvert( line[ i ] ) );
	}
	i ++;
	m_transfer.clear();
	for ( ; i < line.size(); i ++ )
	{
		if ( line[ i ] == string("e") )
		{
			break;
		}
		m_transfer.push_back(EsyConvert(line[i]));
	}
	insertVariables();
}

EsyFaultConfigItem::EsyFaultConfigItem(const EsyFaultConfigItem & t) :
	EsyDataListItem(), m_type( t.faultType() ),
	m_router( t.router() ), m_port( t.port() ), m_vc( t.vc() ), m_bit( t.bit() ),
	m_state( t.state() ), m_transfer( t.transfer() )
{
	insertVariables();
}

void EsyFaultConfigItem::insertVariables()
{
	insertLong( &m_type );
	insertLong( &m_router );
	insertLong( &m_port );
	insertLong( &m_vc );
	insertLong( &m_bit );
	insertLongVector( &m_state, 'e' );
	insertDoubleVector( &m_transfer, 'e' );
}

vector< vector< double > > EsyFaultConfigItem::transferMatrix()
{
	vector< vector< double > > matrix;
	matrix.resize( m_state.size() );
	long p = 0;
	for ( size_t i = 0; i < m_state.size(); i ++ )
	{
		for ( size_t j = 0; j < m_state.size(); j ++ )
		{
			matrix[ i ].push_back( m_transfer[ p ] );
			p ++;
		}
	}
	return matrix;
}

EsyFaultConfigItem EsyFaultConfigList::bestMatchItem(
	EsyFaultConfigItem::FaultType type,
	long router, long port, long vc, long bit )
{
	EsyFaultConfigItem t_item;

	for ( size_t i = 0; i < size(); i ++ )
	{
		if ( at( i ).faultType() == type && at( i ).router() == router &&
			 at( i ).port() == port && at( i ).vc() == vc &&
			 at( i ).bit() == bit )
		{
			return at( i );
		}

		if ( ( at( i ).faultType() == -1 || at( i ).faultType() == type ) &&
			 ( at( i ).router() == -1 || at( i ).router() == router ) &&
			 ( at( i ).port() == -1 || at( i ).port() == port ) &&
			 ( at( i ).vc() == -1 || at( i ).vc() == vc ) &&
			 ( at( i ).bit() == -1 || at( i ).bit() == bit ) )
		{
			if ( at( i ).faultType() > t_item.faultType() )
			{
				t_item = at( i );
			}
			else if ( at( i ).faultType() == t_item.faultType() )
			{
				if ( at( i ).router() > t_item.router() )
				{
					t_item = at( i );
				}
				else if ( at( i ).router() == t_item.router() )
				{
					if ( at( i ).port() > t_item.port() )
					{
						t_item = at( i );
					}
					else if ( at( i ).port() == t_item.port() )
					{
						if ( at( i ).vc() > t_item.vc() )
						{
							t_item = at( i );
						}
						else if ( at( i ).vc() == t_item.vc() )
						{
							if ( at( i ).bit() >= t_item.bit() )
							{
								t_item = at( i );
							}
						}
					}
				}
			}
		}
	}

	return t_item;
}

EsyFaultConfigList EsyFaultConfigList::subItemList(
	EsyFaultConfigItem::FaultType type,
	long router, long port, long vc, long bit )
{
	EsyFaultConfigList t_list;

	for ( size_t i = 0; i < size(); i ++ )
	{
		if ( ( type != -1 && at( i ).faultType() != type ) ||
			 ( router != -1 && at( i ).router() != router ) ||
			 ( port != -1 && at( i ).port() != port ) ||
			 ( vc != -1 && at( i ).vc() != vc ) ||
			 ( bit != -1 && at( i ).bit() != bit ) )
		{
			continue;
		}
		t_list.push_back( at( i ) );
	}

	return t_list;
}

void EsyFaultConfigList::writeFile(const string & file)
{
	EsyDataFileOStream< EsyFaultConfigItem > m_file(
		1000, file, FAULTCFG_EXTENSION, false, true);
	for ( size_t i = 0; i < size(); i ++ )
	{
		m_file.addNextItem( at( i ) );
	}
	m_file.flushBuffer();
}

void EsyFaultConfigList::readFile(const string & file)
{
	EsyDataFileIStream< EsyFaultConfigItem > m_file(
		1000, file, FAULTCFG_EXTENSION, false);
	if ( !m_file.openSuccess() )
	{
		cerr << "Cannot open fault configuration file " << file << endl;
		return;
	}
	while ( ! m_file.eof() )
	{
		EsyFaultConfigItem t_item = m_file.getNextItem();
		push_back( t_item );
		if ( m_file.eof() )
		{
			break;
		}
	}
}

void EsyFaultConfigList::addInLine(const vector< string >& line)
{
	vector< string > line_str;
	int count = 0;
	for ( size_t i = 0; i < line.size(); i ++ )
	{
		line_str.push_back( line[ i ] );
		if ( line[ i ] == "e" )
		{
			if ( count == 0 )
			{
				count ++;
			}
			else
			{
				EsyFaultConfigItem new_item(line_str);
				push_back( new_item );
				count = 0;
				line_str.clear();
			}
		}
	}
}


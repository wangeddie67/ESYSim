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

#ifndef INTERFACE_LINKTOOL_H
#define INTERFACE_LINKTOOL_H

#define MIN_INTERFACE_PROGRESS	0
#define MAX_INTERFACE_PROGRESS 100

#include <iostream>
#include <vector>
#include <cstdarg>

using namespace std;

/** @defgroup link Link Tool Interface.
 * @{
 */

extern unsigned int _link_progress_; /**< \brief Progress of the tool */
extern vector< double > _link_result_; /**< \brief Results of the tool */

#define LINK_PROGRESS_INIT	\
	_link_progress_ = MIN_INTERFACE_PROGRESS; \
	cout << "PP" << MIN_INTERFACE_PROGRESS << endl;

#define LINK_PROGRESS_UPDATE( current, total ) \
	if ( (unsigned int)(current * 100 / total) > _link_progress_ ) \
	{ \
        _link_progress_ = (unsigned int)(current * 100 / total); \
		cout << "PP" << _link_progress_ << endl; \
	}
	
#define LINK_PROGRESS_END \
	_link_progress_ = MAX_INTERFACE_PROGRESS; \
	cout << "PP" << MAX_INTERFACE_PROGRESS << endl;

#define LINK_RESULT_CLEAR \
	_link_result_ .clear();

#define LINK_RESULT_APPEND( argc, ... ) \
	_link_result_append_( argc, ##__VA_ARGS__ );
	
void _link_result_append_( int argc, ... );

#define LINK_RESULT_OUT		\
	cout << std::fixed << "RR"; \
	for ( size_t  i = 0; i < _link_result_ .size(); i ++ ) \
	{ \
		if ( i == 0 ) \
			cout << _link_result_[ i ]; \
		else \
			cout << "|" << _link_result_[ i ]; \
	} \
	cout << endl;
	
#define LINK_RETURN( code ) \
	LINK_RESULT_OUT \
	return code;

#define LINK_TRACE_PRINT( str ) \
	cout << "TR" << str << endl;

#define LINK_RECORD_PRINT( str ) \
    cout << "RE" << str << endl;

#define LINK_SYNCHORNOIZE( ch ) \
	cout << "SY" << endl; \
	while (1) \
	{ \
		char n; \
		cin >> n; \
		if (n == ch ) \
		{ \
			break; \
		} \
	}
	
/** @} */

#endif

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

#include "esy_linktool.h"

unsigned int _link_progress_;
vector< double > _link_result_;

void _link_result_append_( int argc, ... )
{
	va_list ap;
	va_start( ap, argc );
	for ( int i = 0; i < argc; i ++ )
	{
		_link_result_ .push_back( va_arg( ap, double ) );
	}
	va_end( ap );
}

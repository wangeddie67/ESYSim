/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#include "esy_argument.h"
#include "esy_linktool.h"
#include "stdlib.h"

/* name space */
using namespace std;

/*************************************************
  Function :
    int main( int argc, char *argv[] )
  Description:
    main function of esynet
  Input:
         int argc  argument counter
    char * argv[]  argument list
    ConfigType type  type of variable set to inject
  Calls:
    bool MessQueue::openEventTrace()
    void MessQueue::simulator( long long int sim_cycle )
    bool MessQueue::closeEventTrace()
    void SimFoundation::simulationResults()
*************************************************/
int main(int argc, char *argv[])
{
    string a_config_file( "TGFF/simple" );

    EsyArgumentList t_cfg;
    t_cfg.setHead( "Task Graphics for Free" );
    t_cfg.setVersion( "3.0" );
    t_cfg.insertOpenFile( "-option_file", 
         "file path of option file", &a_config_file );

    if ( t_cfg.analyse( argc, argv ) != EsyArgumentList::RESULT_OK )
    {
        return 0;
    }

    string cmd = "./TGFF/tgff " +  a_config_file;
    LINK_PROGRESS_INIT
    system( cmd.c_str() );
    LINK_PROGRESS_END

    /* return */
    LINK_RETURN(0)
}

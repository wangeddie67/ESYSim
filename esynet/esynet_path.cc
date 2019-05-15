/************************************************************
  File Name :
    path.cc
  Description :
    Main function for path simulation
  Global Define :
    double sim_cycle
  Functions:
    int main( int argc, char *argv[] )
***********************************************************/

/* including head file */
#include "esynet_global.h"
#include "esynet_random_unit.h"
#include "esynet_config.h"
#include "esynet_foundation.h"
#include "esynet_event_queue.h"

#include "esy_interdata.h"
#include "esy_linktool.h"

/* global define of double sim_cycle */
/* used by power cacluation */
extern "C" {
double sim_cycle;
}

/*************************************************
  Function : 
    int main( int argc, char *argv[] )
  Description: 
    main function of path
  Input:
         int argc  argument counter
    char * argv[]  argument list
    ConfigType type  type of variable set to inject
  Calls:
    void routingAlgorithm( long des_t, long sor_t, long s_ph, long s_vc )
*************************************************/
int main(int argc, char *argv[])
{
    /* Parser input arguments */
    EsynetConfig argu_list(argc, argv, EsynetConfig::CONFIG_PATH);
    /* set random seed */
    EsynetSRGen random_gen(argu_list.randomSeed() );
    /* generate network configuration */
    EsyNetworkCfg network_cfg;
    if ( argu_list.networkCfgFileEnable() )
    {
        network_cfg.readXml( argu_list.networkCfgFileName() );
    }
    else
    {
		EsyNetworkCfgRouter t_template_router;
		network_cfg.setDataPathWidth( argu_list.dataPathWidth() );
		network_cfg.setTopology(EsyNetworkCfg::NOC_TOPOLOGY_2DTORUS);
		network_cfg.setSize( argu_list.aryNumber() );
		for ( long i = 0; i < argu_list.physicalPortNumber(); i ++ )
		{
			EsyNetworkCfgPort::RouterPortDirection port_dir;
			bool port_ni;
			if ( i == 0 )
			{
				port_dir = EsyNetworkCfgPort::ROUTER_PORT_SOUTHWEST;
				port_ni = true;
			}
			else
			{
				switch ( i % 4 )
				{
					case 0:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_EAST; break;
					case 1:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_NORTH; break;
					case 2:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_SOUTH; break;
					case 3:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_WEST; break;
				}
				port_ni = false;
			}
			t_template_router.appendPort(EsyNetworkCfgPort(
				argu_list.virtualChannelNumber(), 
				argu_list.virtualChannelNumber(), port_dir,
				argu_list.inbufferSize(), argu_list.outbufferSize(), 
				port_ni ) );
		}
		network_cfg.setTemplateRouter( t_template_router );
		network_cfg.updateRouter();
    }
    
    /* network simulation platform */
    EsynetFoundation sim_net( &network_cfg, &argu_list );
    /* message queue */
    EsynetEventQueue network_mess_queue( 0.0, &sim_net, &argu_list );

    /* maximum number of source-destination pair */
    /* taken 2 from router number */    
    long maxpair = argu_list.routerNum() * ( argu_list.routerNum() - 1 );
    /* open path pair file */
    EsyDataFileIStream< EsyDataItemPathpair > pathin( maxpair, 
        argu_list.inputTraceFileName(), PATHPAIR_EXTENSION,
		!argu_list.inputTraceTextEnable() );
    if ( !pathin.openSuccess() )
    {
        cout << "Cannot open path pair file " << argu_list.inputTraceFileName()
            << endl;
        return 0;
    } /* if ( !pathin.open_success() ) */
    
    /* open output file */
    EsyDataFileOStream< EsyDataItemPathtrace > pathout( maxpair, 
        argu_list.eventTraceFileName(), false, true );

    /* maximum hop */
    long maxhop = (long)argu_list.simLength();

    /* initialization statistics variables */
    /* counter of avaiable path */
    long totalpath = 0;
    /* counter of support path : source-destination */
    long supportpath = 0;
    /* counter of shorest path */
    long shortestpath = 0;
    
    /* begin simulation */
    LINK_PROGRESS_INIT
    
    /* loop all path pair */
    while( !pathin.eof() )
    {
        /* print simulation progress */
        LINK_PROGRESS_UPDATE( totalpath, maxpair );
        
        /* next path pir */
        EsyDataItemPathpair t_pair = pathin.getNextItem();
        /* check the path is avaiable */
        /* empty item, break loop */
        if ( t_pair.src() == -1 || t_pair.dst() == -1 )
        {
            break;
        }
        /* source and destination is not aviable, abondon pair */
        if ( network_cfg.validRouterId( t_pair.src() ) ||
             network_cfg.validRouterId( t_pair.dst() ) )
        {
            continue;
        }
        /* source and destination is same, abondon pair */
        if (t_pair.src() == t_pair.dst())
        {
            continue;
        }
        /* source router is faulty, abondon pair */
        if ( sim_net.router( t_pair.src() ).routerFault() )
        {
            continue;
        }
        /* destination router is faulty, abondon pair */
        if ( sim_net.router( t_pair.dst() ).routerFault() )
        {
            continue;
        }
        /* avaiable paths */
        totalpath ++;

        /* calculate path */
        /* initilization variables */
        /* address for each step */
        long addr = t_pair.src();
        /* port for each step */
        long port = 0;
        /* virtual channel for each step */
        long vc = 0;
        /* path vector */
        vector< EsyDataItemHop > path;

        /* loop all step */
        while ( t_pair.dst() != addr )
        {
            /* calculate router algorithm */
            /* result is added in routing vector in router $addr$ */
            sim_net.router( addr ).
                routingAlgorithm( t_pair.dst(), t_pair.src(), port, vc );
            
            /* if routing have result, calculate next step based on the first choice */
            if ( sim_net.router( addr ).inputVirtualChannel( port, vc ).
                routing().size() > 0 )
            {
                /* register a new step in path vector */
                EsynetVC selrouting = sim_net.router( addr ).
                    inputVirtualChannel( port, vc ).routing()[ 0 ];
                path.push_back( EsyDataItemHop( (char)addr, (char)port, 
                    (char)vc, (char)selrouting.first, (char)selrouting.second ) );
                /* clear routing */
                sim_net.router( addr ).inputVirtualChannel( port, vc ). 
                    clearRouting();

                /* calculate next address, port and vc */
                addr = network_cfg.router( addr ).port( selrouting.first ).
                    neighborRouter();
                port = network_cfg.router( addr ).port( selrouting.first ).
                    neighborPort();
                vc = selrouting.second;
            } /* if ( sim_net.router( addr ).inputPort().routing( port, vc ) */

            /* check quit condition */
            /* output port is not available */
            if ( port == -1 )
            {
                break;
            } /* if ( port == -1 ) */
            /* got maximum number of path */
            if ( (long)( path.size() ) >= maxhop )
            {
                break;
            } /* if ( (long)( path.size() ) >= maxhop ) */
        } /* while ( t_pair .m_dst != addr ) */
        
        /* if address for last step is equal to pair, the path is support */
        if ( t_pair.dst() == addr )
        {
            /* increase support path counter */
            supportpath ++;
            /* if size of path is shorter than mahanton distance, */
            /* increase shortest path counter */
            if ( (long)( path.size() ) <= t_pair.hop() )
            {
                shortestpath ++;
            } /* if ( (long)( path.size() ) <= t_pair.m_hop ) */
            /* add last step to local NI */
            path.push_back( 
                EsyDataItemHop( (char)addr, (char)port, (char)vc, 0, 0 ) );
        } /* if ( t_pair.m_dst == addr ) */

        /* add new path */
        pathout.addNextItem( EsyDataItemPathtrace( 
            t_pair.src(), t_pair.dst(), path ) );
    } /* while( !pathin.eof() ) */
    
    /* close path output file */
    pathout.flushBuffer();
    pathout.close();
    /* close path input file */
    pathin.close();

    /* finished simulation */
    LINK_PROGRESS_END
    /* print statistics result */
    LINK_RESULT_APPEND( 3, (double)totalpath, (double)supportpath, 
        double(shortestpath) )
    
    /* return */
    LINK_RETURN( 0 )
}

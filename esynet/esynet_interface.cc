#include "esynet_interface.h"

using namespace esynet;

vector< EsynetEvent > g_esynet_accept_list;

void esynetConfigInit( EsynetConfig * config, char * command )
{
	config->analyse( command );
}

void esynetSRGenInit( EsynetSRGen * srgen, EsynetConfig * config )
{
	srgen->setSeed( config->randomSeed() );
}

void esynetNetworkInit2( EsyNetworkCfg * network, EsynetConfig * config )
{
    if (config->eccEnable())
    {
        long withwithoutecc = network->dataPathWidth();
        long groupwithoutcode = 1;
        long groupwithcode = 1;
        if (config->eccName() == ECC_HM74)
        {
            groupwithoutcode = 4;
            groupwithcode = 7;
        }
        else if (config->eccName() == ECC_HM128)
        {
            groupwithoutcode = 8;
            groupwithcode = 12;
        }
        else if (config->eccName() == ECC_HM2116)
        {
            groupwithoutcode = 16;
            groupwithcode = 22;
        }
        else if (config->eccName() == ECC_HM3832)
        {
            groupwithoutcode = 32;
            groupwithcode = 39;
        }

        long group = withwithoutecc / groupwithoutcode;
        if (withwithoutecc % groupwithoutcode > 0)
        {
            network->setDataPathWidth((group + 1) * groupwithcode);
        }
        else
        {
            network->setDataPathWidth(group * groupwithcode);
        }
    }
}

void esynetNetworkInit( EsyNetworkCfg * network, EsynetConfig * config )
{
	if ( config->networkCfgFileEnable() )
	{
		EsyXmlError t_err = network->readXml( 
			config->networkCfgFileName() );
		if ( t_err.hasError() )
		{
			cerr << "Error: cannot read file " << 
				config->networkCfgFileName() << endl;
			exit( 0 );
		}
	}
	else
	{
		EsyNetworkCfgRouter t_template_router;
		network->setDataPathWidth( config->dataPathWidth() );
		network->setTopology( EsyNetworkCfg::NOC_TOPOLOGY_2DMESH );
		network->setSize( config->networkSize() );
		for ( long i = 0; i < config->physicalPortNumber(); i ++ )
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
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_EAST; 
						break;
					case 1:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_NORTH; 
						break;
					case 2:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_SOUTH; 
						break;
					case 3:
						port_dir = EsyNetworkCfgPort::ROUTER_PORT_WEST; 
						break;
				}
				port_ni = false;
			}
			t_template_router.appendPort( EsyNetworkCfgPort( 
				config->virtualChannelNumber(), config->virtualChannelNumber(), 
				port_dir, config->inbufferSize(),  config->outbufferSize(), 
				port_ni ) );
		}
		network->setTemplateRouter( t_template_router );
		network->updateRouter();
	}

    if (config->eccEnable())
    {
        long withwithoutecc = network->dataPathWidth();
        long groupwithoutcode = 1;
        long groupwithcode = 1;
        if (config->eccName() == ECC_HM74)
        {
            groupwithoutcode = 4;
            groupwithcode = 7;
        }
        else if (config->eccName() == ECC_HM128)
        {
            groupwithoutcode = 8;
            groupwithcode = 12;
        }
        else if (config->eccName() == ECC_HM2116)
        {
            groupwithoutcode = 16;
            groupwithcode = 22;
        }
        else if (config->eccName() == ECC_HM3832)
        {
            groupwithoutcode = 32;
            groupwithcode = 39;
        }

        long group = withwithoutecc / groupwithoutcode;
        if (withwithoutecc % groupwithoutcode > 0)
        {
            network->setDataPathWidth((group + 1) * groupwithcode);
        }
        else
        {
            network->setDataPathWidth(group * groupwithcode);
        }
    }
}

void esynetMessQueueInit( EsynetEventQueue * messq, EsynetConfig * config )
{
	/* open event trace */
	if ( config->eventTraceEnable() )
	{
		messq->openEventTrace();
	} /* if ( argu_list.eventTraceEnable() ) */
    if ( config->outputTraceEnable())
    {
        messq->openOutputTrace();
    } /* if ( argu_list.outputTraceEnable() ) */
}

void esynetInjectPacket( EsynetEventQueue* messq, int src, int dest, long long int sim_cycle, long packetSize, long int msgNo, long long addr, long vc )
{
    messq->insertEvent( EsynetEvent::generateEvgEvent( sim_cycle, src, dest, packetSize, msgNo, (double)sim_cycle ) );
}

void esynetMessQueueFinish( EsynetEventQueue* messq, EsynetConfig* config )
{
	/* open event trace */
	if ( config->eventTraceEnable() )
	{
		messq->closeEventTrace();
	} /* if ( argu_list.eventTraceEnable() ) */
    if ( config->outputTraceEnable())
    {
        messq->closeOutputTrace();
    } /* if ( argu_list.outputTraceEnable() ) */
}

int esynetRunSim( EsynetEventQueue * messq, EsynetFoundation * found, 
				  long long int sim_cycle)
{
	messq->simulator(sim_cycle);
	
	vector< EsynetEvent > accept_list = found->acceptList();
	for ( int i = 0; i < accept_list.size(); i ++ )
	{
		g_esynet_accept_list.push_back( accept_list[ i ] );
	}
	
	return 0;
}

vector< EsynetEvent > esynetAcceptList()
{
	return g_esynet_accept_list;
}

void esynetClearAcceptList()
{
	g_esynet_accept_list.clear();
}

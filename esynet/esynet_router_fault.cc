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

#include "esynet_router_unit.h"

/*************************************************
  Function :
    void EsynetRouter::configFault( const FaultCfgSet & fault )
  Description :
    Config fault injection point.
  Called by:
    SimFoundation::SimFoundation()
  Inputs:
    const FaultCfgSet & fault  fault configuration set
*************************************************/
void EsynetRouter::configFault(EsyFaultConfigList & fault)
{
    /* if fault injection is not enable, quit */
    if ( !m_argu_cfg->faultInjectEnable() )
    {
        return;
    } /* if ( !m_fault_injection_enable ) */

    /* router fault configuration */
    m_router_fault_unit.configFaultList( fault );
    if ( m_router_fault_unit.currentStateOut() )
    {
		addEvent(EsynetMessEvent(m_current_time, ET_FT_INJECTION,
			EsyFaultConfigItem::FAULT_ROUTER, m_router_id, -1, -1, -1, -1,
			EsynetFlit()));
    }
            
    /* insert port fault and link fault configuration */
    for ( size_t l_port = 0; l_port < m_input_port.size(); l_port ++ )
    {
        /* port fault unit */
        m_port_fault_units[ l_port ].configFaultList( fault );
        if ( m_port_fault_units[ l_port ].currentStateOut() )
        {
			addEvent(EsynetMessEvent(m_current_time, ET_FT_INJECTION,
				EsyFaultConfigItem::FAULT_PORT, m_router_id, l_port, -1, -1, -1,
				EsynetFlit()));
        }
        
        if ( !m_router_cfg->port( l_port ).networkInterface() )
        {
            /* link fault unit */
            m_link_fault_units[ l_port ].configFaultList( fault );
            for ( size_t bit = 0; bit < m_link_fault_units[ l_port ].size();
                 bit ++ )
            {
                if ( m_link_fault_units[ l_port ][ bit ].currentStateOut() )
                {
					addEvent(EsynetMessEvent(m_current_time, ET_FT_INJECTION,
						EsyFaultConfigItem::FAULT_PORT, m_router_id,
						l_port, -1, bit, -1, EsynetFlit()));
                }
            }

            /* ecc unit */
            m_ecc_units[ l_port ].configFaultList( fault );
            for ( size_t bit = 0; bit < m_ecc_units[ l_port ].size();
                 bit ++ )
            {
                if ( m_ecc_units[ l_port ][ bit ].currentStateOut() )
                {
					addEvent(EsynetMessEvent(m_current_time, ET_FT_INJECTION,
						EsyFaultConfigItem::FAULT_ECCINTERDEC, m_router_id,
						l_port, -1, bit, -1, EsynetFlit()));
                }
            }
        }

        /* switch fault unit */
        m_switch_fault_units[ l_port ].configFaultList( fault );
        for ( size_t bit = 0; bit < m_switch_fault_units[ l_port ].size(); bit ++ )
        {
            if ( m_switch_fault_units[ l_port ][ bit ].currentStateOut() )
            {
				addEvent(EsynetMessEvent(m_current_time, ET_FT_INJECTION,
					EsyFaultConfigItem::FAULT_SWITCH, m_router_id, l_port, -1,
					bit, -1, EsynetFlit()));
            }
        }

    } /* insert port fault and link fault configuration */
}

void EsynetRouter::faultStateUpdate()
{
    bool last_state = m_router_fault_unit.currentStateOut();
    m_router_fault_unit.nextState();
    if ( last_state != m_router_fault_unit.currentStateOut() )
    {
		addEvent(EsynetMessEvent(m_current_time,
            (m_router_fault_unit.currentStateOut())?
            ET_FT_INJECTION:ET_FT_RECOVERY,
            EsyFaultConfigItem::FAULT_ROUTER, m_router_id, -1, -1, -1, -1, 
			EsynetFlit()));
    }
    /* loop port */
    for ( size_t phy = 0; phy < m_port_fault_units.size(); phy ++ )
    {
        bool last_state = m_port_fault_units[ phy ].currentStateOut();
        m_port_fault_units[ phy ].nextState();
        if ( last_state != m_port_fault_units[ phy ].currentStateOut() )
        {
            addEvent( EsynetMessEvent( m_current_time, 
                (m_port_fault_units[ phy ].currentStateOut())?
                ET_FT_INJECTION:ET_FT_RECOVERY,
                EsyFaultConfigItem::FAULT_PORT, m_router_id, phy, -1, -1, -1,
				EsynetFlit()));
        }
    }

    for ( size_t phy = 0; phy < m_link_fault_units.size(); phy ++ )
    {
        if ( m_router_cfg->port( phy ).networkInterface() )
        {
            continue;
        }
        vector< bool > last_state;
        for ( size_t bit = 0; bit < m_link_fault_units[ phy ].size(); bit ++ )
        {
            last_state.push_back(
                m_link_fault_units[ phy ][ bit ].currentStateOut() );
        }
        m_link_fault_units[ phy ].nextState();
        for ( size_t bit = 0; bit < m_link_fault_units[ phy ].size(); bit ++ )
        {
            if ( last_state[ bit ] != 
                m_link_fault_units[ phy ][ bit ].currentStateOut() )
            {
                addEvent( EsynetMessEvent( m_current_time, 
                    (m_link_fault_units[ phy ][ bit ].currentStateOut())?
                    ET_FT_INJECTION:ET_FT_RECOVERY,
                    EsyFaultConfigItem::FAULT_LINE, m_router_id, phy, -1, bit, 
					-1, EsynetFlit()));
            }
        }
        
        if (!m_link_fault_units[ phy ].isConst())
		{
			if (m_link_fault_units[ phy ].stateBitCount( 0 ) == m_link_fault_units[ phy ].size())
			{
				m_port_fault_units[ phy ].setConstantState( false );
			}
		}
    }
    
    for ( size_t phy = 0; phy < m_switch_fault_units.size(); phy ++ )
    {
        vector< bool > last_state;
        for ( size_t bit = 0; bit < m_switch_fault_units[ phy ].size(); bit ++ )
        {
            last_state.push_back(
                m_switch_fault_units[ phy ][ bit ].currentStateOut() );
        }
        m_switch_fault_units[ phy ].nextState();
        for ( size_t bit = 0; bit < m_switch_fault_units[ phy ].size(); bit ++ )
        {
            if ( last_state[ bit ] != 
                m_switch_fault_units[ phy ][ bit ].currentStateOut() )
            {
                addEvent( EsynetMessEvent( m_current_time, 
                    (m_switch_fault_units[ phy ][ bit ].currentStateOut())?
                    ET_FT_INJECTION:ET_FT_RECOVERY,
                    EsyFaultConfigItem::FAULT_SWITCH, m_router_id, phy, -1, bit, 
					-1, EsynetFlit()));
            }
        }
    }

    for ( size_t phy = 0; phy < m_ecc_units.size(); phy ++ )
    {
        vector< bool > last_state;
        for ( size_t bit = 0; bit < m_ecc_units[ phy ].size(); bit ++ )
        {
            last_state.push_back(
                m_ecc_units[ phy ][ bit ].currentStateOut() );
        }
        m_ecc_units[ phy ].nextState();
        for ( size_t bit = 0; bit < m_ecc_units[ phy ].size(); bit ++ )
        {
            if ( last_state[ bit ] != 
                m_ecc_units[ phy ][ bit ].currentStateOut() )
            {
                addEvent( EsynetMessEvent( m_current_time, 
                    (m_ecc_units[ phy ][ bit ].currentStateOut())?
                    ET_FT_INJECTION:ET_FT_RECOVERY,
                    EsyFaultConfigItem::FAULT_ECCINTERDEC, m_router_id, 
					phy, -1, bit, -1, EsynetFlit()));
            }
        }
    }
}

void EsynetRouter::portFaultInjection(long phy, EsynetFlit & flit_t)
{
    /* fault pattern local */
    DataType flit_fault_pattern = flit_t.faultPattern();
    m_link_fault_units[ phy ].updateFaultPattern( flit_fault_pattern );
	if (m_argu_cfg->swEnable())
	{
		for (int i = 0; i < flit_fault_pattern.size(); i ++)
		{
			flit_fault_pattern[i] &= (~ m_spare_line_pattern[phy][i]);
		}
	}
    flit_t.setFaultPattern( flit_fault_pattern );
    
    for ( int i = 0; i < m_network_cfg->flitSize( ATOM_WIDTH_ ); i ++ ) 
    {
        if ( flit_fault_pattern[i] > 0 ) 
        {
            flit_t.setFaulty();
            break;
        }
    }
}

void EsynetRouter::switchFaultInjection(long phy, EsynetFlit & flit_t)
{
    /* fault pattern local */
    DataType flit_fault_pattern = flit_t.faultPattern();
    m_switch_fault_units[ phy ].updateFaultPattern( flit_fault_pattern );
    flit_t.setFaultPattern( flit_fault_pattern );
    
    for ( int i = 0; i < m_network_cfg->flitSize( ATOM_WIDTH_ ); i ++ ) 
    {
        if ( flit_fault_pattern[i] > 0 ) 
        {
            flit_t.setFaulty();
            break;
        }
    }
}

#if 0
    /* interleaving */
    DataType inpattern = local_fault_pattern;
    DataType outpattern(flit_size);
    if (interleaving_enable_)
    {
        long outgroupwidth = (flit_size * ATOM_WIDTH_) /
            interleaving_group_width_;
        /* loop bit */
        for (int bit = 0; bit < flit_size * ATOM_WIDTH_; bit ++)
        {
            /* input bit position */
            long inbyteid = bit / ATOM_WIDTH_;
            long inbitid = bit % ATOM_WIDTH_;
            /* output bit position */
            long outgroupid = bit % interleaving_group_width_;
            long outgroupbit = bit / interleaving_group_width_;
            long outbit = outgroupid * outgroupwidth + outgroupbit;
            long outbypteid = outbit / ATOM_WIDTH_;
            long outbitid = outbit % ATOM_WIDTH_;
            /* get value */
            long bitvalue = inpattern[inbyteid] & (0x01ULL << inbitid);
            /* if the value is 1, add in outpattern */
            if (bitvalue > 0)
            {
                outpattern[outbypteid] = outpattern[outbypteid] |
                    (0x01ULL << outbitid);
            }
        } /* end of  for (int bit = 0; bit < flit_size_ * ATOM_WIDTH_; bit 
++) */
        local_fault_pattern = outpattern;
    } /* end of  if (interleaving_enable_) */

    /* spareline */
    if (spare_line_enable_)
    {
        /* all words in flit */
        for (size_t word = 0; word < local_fault_pattern.size(); word ++)
        {
            /* inject fault */
            local_fault_pattern[word] = local_fault_pattern[word] &
                (~spare_line_pattern_[phy][word]);
        } /* end of  for (word = 0; word < local_fault_pattern.size(); word 
++) */
    }

    /* current fault pattern */
    DataType fault_pattern = flit_t.faultPattern();
    /* all words in flit */
    for (size_t word = 0; word < fault_pattern.size(); word ++)
    {
        /* inject fault */
        fault_pattern[word] = fault_pattern[word] | 
local_fault_pattern[word];
    } /* end of  for (word = 0; word < fault_pattern.size(); word ++) */

    #endif


void EsynetRouter::reportFault( long phy )
{
	bool temp_detect_enable = true;
	if (!temp_detect_enable)
	{
		return;
	}
    /* spare line */
    if (m_argu_cfg->swEnable())
    {
        /* clear previous configurtion g_all.tx*/
        for (int word = 0; word < m_spare_line_pattern[phy].size(); word ++)
        {
            m_spare_line_pattern[phy][word] = 0x0;
        }
        m_port_fault_units[phy].setConstantState(false);
        /* configure permanent fault to spare line */
        /* total number of bit and group */
		
        long totalgroup = m_network_cfg->dataPathWidth() / m_argu_cfg->swCapacity()[0];
        /* loop all group and bit */
        for (int group = 0; group < totalgroup; group ++)
        {
            /* count faults in a group */
            long groupfault = 0;
            long checkbit = group * m_argu_cfg->swCapacity()[0];
            for (int gbit = 0; gbit < m_argu_cfg->swCapacity()[0]; gbit ++)
            {
                long byteid = checkbit / ATOM_WIDTH_;
                long bitid = checkbit % ATOM_WIDTH_;
                /* if bitvalue is 1, increase the number of fault */
                if ( m_link_fault_units[phy].faultPoint( checkbit ).currentStateOut() )
                {
					groupfault ++;
					/* if all spare line have been used, stop Configuration
					* and enable the port fault */
					if (groupfault > m_argu_cfg->swCapacity()[1])
					{
						m_port_fault_units[phy].setConstantState(true);
					}
					else
					{
						m_spare_line_pattern[phy][byteid] = 
							m_spare_line_pattern[phy][byteid] | (0x01ULL << bitid);
					}
				}

				checkbit ++;
			}
			
        } /* end of  for (int group = 0; group < totalgroup; group ++) */
    } /* end of  if (spare_line_enable_) */
    /* if permanet fault is not zero, set port fault */
    else
    {
        m_port_fault_units[phy].setConstantState(false);
		for (int bit = 0; bit < m_network_cfg->dataPathWidth(); bit ++)
        {
            /* input bit position */
            long byteid = bit / ATOM_WIDTH_;
            long bitid = bit % ATOM_WIDTH_;
            /* if the value is 1, add in spare line patten */
            if ( m_link_fault_units[phy].faultPoint( bit ).currentStateOut() )
            {
				m_port_fault_units[phy].setConstantState(true);
                break;
            }
        }
    }
}
/*
void EsynetRouter::checkFault()
{
//    m_router_fault = true;
    for (int phy = 0; phy < m_port_num; phy ++)
    {
//        if (m_port_fault[phy] == false)
        {
//            m_router_fault = false;
            return;
        }
    }
}
*/

void EsynetRouter::eccBuffer()
{
    for ( size_t phy = 0; phy < m_input_port.size(); phy ++ )
    {
        if ( m_ecc_units[ phy ].eccEnable() && 
            m_ecc_units[ phy ].bufferState() )
        {
			EsynetFlit flit_t = m_ecc_units[ phy ].getFlit();
			if (flit_t.drop())
			{
				reportFault(phy);
			}
            receiveFlitInBuffer( phy, m_ecc_units[ phy ].vcToGo(), flit_t );
        }
    }
}

void EsynetRouter::eccLocation( ECCStrategy strategy )
{
    for ( size_t phy = 0; phy < m_input_port.size(); phy ++ )
    {
        if ( m_router_cfg->port( phy ).networkInterface() )
        {
            continue;
        }
        if ( m_router_cfg->port( phy ).neighborRouter() == -1 ||
            m_router_cfg->port( phy ).neighborPort() == -1 )
        {
            continue;
        }
        
		long y = m_router_addr[EsyNetworkCfg::AX_Y];
		long x = m_router_addr[EsyNetworkCfg::AX_X];
        
        /* hop to hop */    
		if (strategy == ECC_H2H || strategy == ECC_COUNT )
        {
            m_ecc_units[ phy ].setECCEnable();
        }
        /* cross */
		else if (strategy == ECC_CROSS)
        {
            if ( abs( m_argu_cfg->eccSpace() - y - m_network_cfg->size(0) + 1 
				+ x ) % m_argu_cfg->eccSpace() == 0 )
            {
                m_ecc_units[ phy ].setECCEnable();
            }            
            if ( abs( m_argu_cfg->eccSpace() - y - x ) % 
                m_argu_cfg->eccSpace() == 0 )
            {
                m_ecc_units[ phy ].setECCEnable();
            }
        }
        /* slope */
		else if (strategy == ECC_SLOPE )
        {
            if ( abs( m_argu_cfg->eccSpace() - y - x ) % 
                m_argu_cfg->eccSpace() == 0 )
            {
                m_ecc_units[ phy ].setECCEnable();
            }
        }
        /* square */
		else if (strategy == ECC_SQUARE )
        {
            if ( m_router_cfg->port( phy ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_NORTH)
            {
                if ( y % m_argu_cfg->eccSpace() == 0 )
                {
                    m_ecc_units[ phy ].setECCEnable();
                }
            }
            else if ( m_router_cfg->port( phy ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_SOUTH)
            {
                if ( (y + 1) % m_argu_cfg->eccSpace() == 0 )
                {
                    m_ecc_units[ phy ].setECCEnable();
                }
            }
            else if ( m_router_cfg->port( phy ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_WEST)
            {
                if ( x % m_argu_cfg->eccSpace() == 0 )
                {
                    m_ecc_units[ phy ].setECCEnable();
                }
            }
            else if ( m_router_cfg->port( phy ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_EAST)
            {
                if ( (x + 1) % m_argu_cfg->eccSpace() == 0 )
                {
                    m_ecc_units[ phy ].setECCEnable();
                }
            }
        }
    }
}

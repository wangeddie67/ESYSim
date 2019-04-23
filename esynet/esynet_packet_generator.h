/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifndef ESYNETPACKETGENERATOR_H
#define ESYNETPACKETGENERATOR_H

/* including head file */
#include "esy_interdata.h"
#include "esy_networkcfg.h"
#include "esynet_config.h"
#include "esynet_ecc_unit.h"
#include "esynet_sim_base.h"

class EsynetPacketGenerator
{
/* Public Types */
public:
	/* traffic rule to generate traffic */
	enum TrafficRule {
		/* random */
		TRAFFIC_RANDOM,         
		/* (x,y) -> (Y-1-y,X-1-x), X,Y is the size of network */
		TRAFFIC_TRANSPOSE1,     
		/* (x,y) -> (y,x) */
		TRAFFIC_TRANSPOSE2,     
		/* bit reversal */
		TRAFFIC_BIT_REVERSAL,   
		/* loop left shift */
		TRAFFIC_SHUFFLE,        
		/* swap the MSB and LSB */
		TRAFFIC_BUTTERFLY       
	};

/* Properties */
private:
	/* General Information */
	/* Unique identification number */
	EsyNetworkCfg * m_network_cfg;
	EsynetConfig * m_argu_cfg;

	bool m_enable;
	long m_count;

/* Public functions */
public:
	/* constructor function */
	EsynetPacketGenerator(EsyNetworkCfg * network_cfg, EsynetConfig * argument_cfg);
	void setEnable() {m_enable = true;}
	void setDisable() {m_enable = false;}

	/* generate packet */
	vector<EsynetFlit> generatePacket();
	vector<EsynetFlit> generatePacket(long id);
};

#endif // ESYNETPACKETGENERATOR_H

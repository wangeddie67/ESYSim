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

void EsynetRouter::algorithmGradient(long des, long src, long s_ph, long s_vc)
{
/*	add_type des_t = id2addr( des );
	add_type sor_t = id2addr( src );
	long xoffset = des_t[0] - address_[0];
	long yoffset = des_t[1] - address_[1];

	int pri[5];

	if (xoffset == 0 && yoffset == 0)
	{
		pri[0] = 4; pri[1] = 0; pri[2] = 0; pri[3] = 0; pri[4] = 0;
	}
	//   zone-1
	else if (xoffset <= 0 && yoffset >  0 && abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
	{
		pri[0] = 0; pri[1] = 3; pri[2] = 1; pri[3] = 2; pri[4] = 4;
	}
	//   zone-2
	else if (xoffset <  0 && yoffset >= 0 && abs(static_cast<int>(yoffset)) <  abs(static_cast<int>(xoffset)))
	{
		pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 2; pri[4] = 3;
	}
	//   zone-3
	else if (xoffset <  0 && yoffset <= 0 && abs(static_cast<int>(yoffset)) <  abs(static_cast<int>(xoffset)))
	{
		pri[0] = 0; pri[1] = 4; pri[2] = 2; pri[3] = 3; pri[4] = 1;
	}
	//   zone-4
	else if (xoffset <= 0 && yoffset <  0 && abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
	{
		pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
	}
	//   zone-5
	else if (xoffset >= 0 && yoffset <  0 && abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
	{
		pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 4; pri[4] = 1;
	}
	//   zone-6
	else if (xoffset >  0 && yoffset <= 0 && abs(static_cast<int>(yoffset)) <  abs(static_cast<int>(xoffset)))
	{
		pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 3; pri[4] = 2;
	}
	//   zone-7
	else if (xoffset >  0 && yoffset >= 0 && abs(static_cast<int>(yoffset)) <  abs(static_cast<int>(xoffset)))
	{
		pri[0] = 0; pri[1] = 2; pri[2] = 4; pri[3] = 1; pri[4] = 3;
	}
	//   zone-8
	else
	{
		pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 1; pri[4] = 4;
	}
	
	// check port
	for (int i = 1; i < 5; i ++)
		pri[i] = pri[i] * farthest_neighbor::wpfn().ft(address_[0], address_[1], i);

	if (s_ph != 0)
		pri[s_ph] = 0;

	int maxpri = pri[1];
	int maxindex = 1;
	for (int i = 1; i < 5; i ++)
	{
		if (pri[i] > maxpri)
		{
			maxpri = pri[i];
			maxindex = i;
		}
	}
	if (maxpri <= 0)
		maxindex = s_ph;
	
	input_module_.add_routing(s_ph, s_vc, VC_type(maxindex,s_vc));*/
}


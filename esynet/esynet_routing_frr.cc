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

//#define MAX(x,y) ((x>=y)?x:y)
//#define MIN(x,y) ((x>=y)?y:x)
// adaptive-order farthest neighbor algorithm
//***************************************************************************//
void EsynetRouter::algorithmAFRR(long des, long src, long s_ph, long s_vc)
{
/*	add_type des_t = id2addr( des );
	add_type sor_t = id2addr( src );
	long xoffset = des_t[0] - address_[0];
	long yoffset = des_t[1] - address_[1];
	long x = address_[0];
	long y = address_[1];

	int pri[5];

	// modified gradient routing
	if (xoffset > 0)
	{
		if (yoffset > 0)
		{
			// region 1
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 1; pri[4] = 4;
			}
			// region 2
			else
			{
				pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 2; pri[4] = 3;
			}
		}
		else if (yoffset < 0)
		{
			// region 4
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 4; pri[4] = 1;
			}
			// region 3
			else
			{
				pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 3; pri[4] = 2;
			}
		}
		else
		// line 2
		{
			pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 2; pri[4] = 3;
		}
	}
	else if (xoffset < 0)
	{
		if (yoffset > 0)
		{
			// region 8
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
			}
			// region 7
			else
			{
				pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 2; pri[4] = 3;
			}
		}
		else if (yoffset < 0)
		{
			// region 5
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
			}
			// region 6
			else
			{
				pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 3; pri[4] = 2;
			}
		}
		else
		// line 1
		{
			pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 2; pri[4] = 3;
		}
	}
	else
	{
		if (yoffset > 0)
		// line 4
		{
			pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
		}
		else if (yoffset < 0)
		// line 3
		{
			pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
		}
		else
		// local
		{
			pri[0] = 4; pri[1] = 0; pri[2] = 0; pri[3] = 0; pri[4] = 0;
		}
	}

	// local
	if (yoffset == 0 && xoffset == 0)
	{
	}
	else if (abs(static_cast<int>(yoffset)) > 0 && abs(static_cast<int>(xoffset) > 0))
	// region: 1, 2, 3, 4, 5, 6, 7, 8
	{
		// region: 1, 2
		if (yoffset > 0 && xoffset > 0)
		{
			// port 2
			long rowhop = 0;
			for (long k = x + 1; k <= farthest_neighbor::wpfn().fn(x, y, 2); k ++)
			{
				if (farthest_neighbor::wpfn().fn(k, y, 4) >= des_t[1])
				{
					rowhop = abs(static_cast<int>(k - x)) + abs(static_cast<int>(des_t[1] - y)) + abs(static_cast<int>(k - des_t[0]));
					break;
				}
			}
			// port 4
			long colhop = 0;
			for (long k = y + 1; k <= farthest_neighbor::wpfn().fn(x, y, 4); k ++)
			{
				if (farthest_neighbor::wpfn().fn(x, k, 2) >= des_t[0])
				{
					colhop = abs(static_cast<int>(k - y)) + abs(static_cast<int>(des_t[0] - x)) + abs(static_cast<int>(k - des_t[1]));
					break;
				}
			}
			if (rowhop > 0)
			{
				if (colhop > 0)
				{
					if (rowhop > colhop)
						pri[4] = pri[4] + 2;
					else if (rowhop < colhop)
						pri[2] = pri[2] + 2;
				}
				else
					pri[2] = pri[2] + 2;
			}
			else
				if (colhop > 0)
					pri[4] = pri[4] + 2;
		}
		// region: 3, 4
		else if (yoffset < 0 && xoffset > 0)
		{
			// port 2
			long rowhop = 0;
			for (long k = x + 1; k <= farthest_neighbor::wpfn().fn(x, y, 2); k ++)
			{
				if (farthest_neighbor::wpfn().fn(k, y, 3) <= des_t[1])
				{
					rowhop = abs(static_cast<int>(k - x)) + abs(static_cast<int>(des_t[1] - y)) + abs(static_cast<int>(k - des_t[0]));
					break;
				}
			}
			// port 3
			long colhop = 0;
			for (long k = y - 1; k >= farthest_neighbor::wpfn().fn(x, y, 3); k --)
			{
				if (farthest_neighbor::wpfn().fn(x, k, 2) >= des_t[0])
				{
					colhop = abs(static_cast<int>(k - y)) + abs(static_cast<int>(des_t[0] - x)) + abs(static_cast<int>(k - des_t[1]));
					break;
				}
			}
			if (rowhop > 0)
			{
				if (colhop > 0)
				{
					if (rowhop > colhop)
						pri[3] = pri[3] + 2;
					else if (rowhop < colhop)
						pri[2] = pri[2] + 2;
				}
				else
					pri[2] = pri[2] + 2;
			}
			else
				if (colhop > 0)
					pri[3] = pri[3] + 2;
		}
		// region: 5, 6
		else if (yoffset < 0 && xoffset < 0)
		{
			// port 1
			long rowhop = 0;
			for (long k = x - 1; k >= farthest_neighbor::wpfn().fn(x, y, 1); k --)
			{
				if (farthest_neighbor::wpfn().fn(k, y, 3) <= des_t[1])
				{
					rowhop = abs(static_cast<int>(k - x)) + abs(static_cast<int>(des_t[1] - y)) + abs(static_cast<int>(k - des_t[0]));
					break;
				}
			}
			// port 3
			long colhop = 0;
			for (long k = y - 1; k >= farthest_neighbor::wpfn().fn(x, y, 3); k --)
			{
				if (farthest_neighbor::wpfn().fn(x, k, 1) <= des_t[0])
				{
					colhop = abs(static_cast<int>(k - y)) + abs(static_cast<int>(des_t[0] - x)) + abs(static_cast<int>(k - des_t[1]));
					break;
				}
			}
			if (rowhop > 0)
			{
				if (colhop > 0)
				{
					if (rowhop > colhop)
						pri[3] = pri[3] + 2;
					else if (rowhop < colhop)
						pri[1] = pri[1] + 2;
				}
				else
					pri[1] = pri[1] + 2;
			}
			else
				if (colhop > 0)
					pri[3] = pri[3] + 2;
		}
		// region: 7, 8
		else if (yoffset > 0 && xoffset < 0)
		{
			// port 1
			long rowhop = 0;
			for (long k = x - 1; k >= farthest_neighbor::wpfn().fn(x, y, 1); k --)
			{
				if (farthest_neighbor::wpfn().fn(k, y, 4) >= des_t[1])
				{
					rowhop = abs(static_cast<int>(k - x)) + abs(static_cast<int>(des_t[1] - y)) + abs(static_cast<int>(k - des_t[0]));
					break;
				}
			}
			// port 4
			long colhop = 0;
			for (long k = y + 1; k <= farthest_neighbor::wpfn().fn(x, y, 4); k ++)
			{
				if (farthest_neighbor::wpfn().fn(x, k, 1) <= des_t[0])
				{
					colhop = abs(static_cast<int>(k - y)) + abs(static_cast<int>(des_t[0] - x)) + abs(static_cast<int>(k - des_t[1]));
					break;
				}
			}
			if (rowhop > 0)
			{
				if (colhop > 0)
				{
					if (rowhop > colhop)
						pri[4] = pri[4] + 2;
					else if (rowhop < colhop)
						pri[1] = pri[1] + 2;
				}
				else
					pri[1] = pri[1] + 2;
			}
			else
				if (colhop > 0)
					pri[4] = pri[4] + 2;
		}
	}
	// line
	else if (yoffset == 0 || xoffset == 0)
	{
		// line 1
		if (yoffset == 0 && xoffset < 0)
		{
			// port 1
			if (farthest_neighbor::wpfn().fn(x, y, 1) <= des_t[0])
				pri[1] = pri[1] + 3;
			for (long k = 1; k <= MAX(y, ary_size_[0] - y - 1); k ++)
			{
				int gotflag = 0;
				// port 3
				if (y - k >= 0)
					if (farthest_neighbor::wpfn().fn(x, y - k, 1) <= des_t[0])
					{
						pri[3] = pri[3] + 3;
						gotflag = 1;
					}
				// port 4
				if (y + k < ary_size_[0])
					if (farthest_neighbor::wpfn().fn(x, y + k, 1) <= des_t[0])
					{
						pri[4] = pri[4] + 3;
						gotflag = 1;
					}
				if (gotflag == 1)
					break;
			}
		}
		// line 2
		else if (yoffset == 0 && xoffset > 0)
		{
			// port 2
			if (farthest_neighbor::wpfn().fn(x, y, 2) >= des_t[0])
				pri[2] = pri[2] + 3;
			for (long k = 1; k <= MAX(y, ary_size_[0] - y - 1); k ++)
			{
				int gotflag = 0;
				// port 4
				if (y + k < ary_size_[0])
					if (farthest_neighbor::wpfn().fn(x, y + k, 2) >= des_t[0])
					{
						pri[4] = pri[4] + 3;
						gotflag = 1;
					}
				// port 3
				if (y - k >= 0)
					if (farthest_neighbor::wpfn().fn(x, y - k, 2) >= des_t[0])
					{
						pri[3] = pri[3] + 3;
						gotflag = 1;
					}
				if (gotflag == 1)
					break;
			}
		}
		// line 3
		else if (xoffset == 0 && yoffset < 0)
		{
			// port 3
			if (farthest_neighbor::wpfn().fn(x, y, 3) <= des_t[1])
				pri[3] = pri[3] + 3;
			for (long k = 1; k <= MAX(x, ary_size_[1] - x - 1); k ++)
			{
				int gotflag = 0;
				// port 2
				if (x + k < ary_size_[1])
					if (farthest_neighbor::wpfn().fn(x + k, y, 3) <= des_t[1])
					{
						pri[2] = pri[2] + 3;
						gotflag = 1;
					}
				// port 1
				if (x - k >= 0)
					if (farthest_neighbor::wpfn().fn(x - k, y, 3) <= des_t[1])
					{
						pri[1] = pri[1] + 3;
						gotflag = 1;
					}
				if (gotflag == 1)
					break;
			}
		}
		// line 4
		else // xoffset == 0 && yoffset > 0
		{
			// port 4
			if (farthest_neighbor::wpfn().fn(x, y, 4) >= des_t[1])
				pri[4] = pri[4] + 3;
			for (long k = 1; k <= MAX(x, ary_size_[1] - x - 1); k ++)
			{
				int gotflag = 0;
				// port 1
				if (x - k >= 0)
					if (farthest_neighbor::wpfn().fn(x - k, y, 4) >= des_t[1])
					{
						pri[1] = pri[1] + 3;
						gotflag = 1;
					}
				// port 2
				if (x + k < ary_size_[1])
					if (farthest_neighbor::wpfn().fn(x + k, y, 4) >= des_t[1])
					{
						pri[2] = pri[2] + 3;
						gotflag = 1;
					}
				if (gotflag == 1)
					break;
			}
		}
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

// // n-step farthest neighbor algorithm
// //***************************************************************************//
// void sim_router_template::nstep_fn(const add_type & des_t,
// 		const add_type & sor_t, long s_ph, long s_vc)
// {
// 	long nstep = configuration::globalPointer().nstep();
// 	long xoffset = des_t[0] - address_[0];
// 	long yoffset = des_t[1] - address_[1];
// 	long x = address_[0];
// 	long y = address_[1];
// 
// 	int pri[5];
// 
// 	// modified gradient routing
// 	if (xoffset > 0)
// 	{
// 		if (yoffset > 0)
// 		{
// 			// region 1
// 			if (abs(yoffset) >= abs(xoffset))
// 			{
// 				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 1; pri[4] = 4;
// 			}
// 			// region 2
// 			else
// 			{
// 				pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 2; pri[4] = 3;
// 			}
// 		}
// 		else if (yoffset < 0)
// 		{
// 			// region 4
// 			if (abs(yoffset) >= abs(xoffset))
// 			{
// 				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 4; pri[4] = 1;
// 			}
// 			// region 3
// 			else
// 			{
// 				pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 3; pri[4] = 2;
// 			}
// 		}
// 		else
// 		// line 2
// 		{
// 			pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 2; pri[4] = 3;
// 		}
// 	}
// 	else if (xoffset < 0)
// 	{
// 		if (yoffset > 0)
// 		{
// 			// region 8
// 			if (abs(yoffset) >= abs(xoffset))
// 			{
// 				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
// 			}
// 			// region 7
// 			else
// 			{
// 				pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 2; pri[4] = 3;
// 			}
// 		}
// 		else if (yoffset < 0)
// 		{
// 			// region 5
// 			if (abs(yoffset) >= abs(xoffset))
// 			{
// 				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
// 			}
// 			// region 6
// 			else
// 			{
// 				pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 3; pri[4] = 2;
// 			}
// 		}
// 		else
// 		// line 1
// 		{
// 			pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 2; pri[4] = 3;
// 		}
// 	}
// 	else
// 	{
// 		if (yoffset > 0)
// 		// line 4
// 		{
// 			pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
// 		}
// 		else if (yoffset < 0)
// 		// line 3
// 		{
// 			pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
// 		}
// 		else
// 		// local
// 		{
// 			pri[0] = 4; pri[1] = 0; pri[2] = 0; pri[3] = 0; pri[4] = 0;
// 		}
// 	}
// 
// 	// local
// 	if (yoffset == 0 && xoffset == 0)
// 	{
// 	}
// 	else if (abs(yoffset) > 0 && abs(xoffset) > 0)
// 	// region: 1, 2, 3, 4, 5, 6, 7, 8
// 	{
// 		// region: 1, 2
// 		if (yoffset > 0 && xoffset > 0)
// 		{
// 			// port 2
// 			long rowhop = 0;
// 			if (yoffset == 1 && farthest_neighbor::wpfn().fn(x,y,2) >= des_t[0])
// 			{
// 				rowhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = x + 1; k <= min(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2)); k ++)
// 				{
// 					if (farthest_neighbor::wpfn().fn(k, y, 4) >= des_t[1])
// 					{
// 						rowhop = abs(k - x) + abs(des_t[1] - y) + abs(k - des_t[0]);
// 						break;
// 					}
// 				}
// 			}
// 			// port 4
// 			long colhop = 0;
// 			if (xoffset == 1 && farthest_neighbor::wpfn().fn(x,y,4) >= des_t[1])
// 			{
// 				colhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = y + 1; k <= min(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4)); k ++)
// 				{
// 					if (farthest_neighbor::wpfn().fn(x, k, 2) >= des_t[0])
// 					{
// 						colhop = abs(k - y) + abs(des_t[0] - x) + abs(k - des_t[1]);
// 						break;
// 					}
// 				}
// 			}
// 			if (rowhop > 0)
// 			{
// 				if (colhop > 0)
// 				{
// 					if (rowhop > colhop)
// 						pri[4] = pri[4] + 2;
// 					else if (rowhop < colhop)
// 						pri[2] = pri[2] + 2;
// 				}
// 				else
// 					pri[2] = pri[2] + 2;
// 			}
// 			else
// 				if (colhop > 0)
// 					pri[4] = pri[4] + 2;
// 		}
// 		// region: 3, 4
// 		else if (yoffset < 0 && xoffset > 0)
// 		{
// 			// port 2
// 			long rowhop = 0;
// 			if (yoffset == -1 && farthest_neighbor::wpfn().fn(x,y,2) >= des_t[0])
// 			{
// 				rowhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = x + 1; k <= min(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2)); k ++)
// 				{
// 					if (farthest_neighbor::wpfn().fn(k, y, 3) <= des_t[1])
// 					{
// 						rowhop = abs(k - x) + abs(des_t[1] - y) + abs(k - des_t[0]);
// 						break;
// 					}
// 				}
// 			}
// 			// port 3
// 			long colhop = 0;
// 			if (xoffset == 1 && farthest_neighbor::wpfn().fn(x,y,3) <= des_t[1])
// 			{
// 				colhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = y - 1; k >= max(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3)); k --)
// 				{
// 					if (farthest_neighbor::wpfn().fn(x, k, 2) >= des_t[0])
// 					{
// 						colhop = abs(k - y) + abs(des_t[0] - x) + abs(k - des_t[1]);
// 						break;
// 					}
// 				}
// 			}
// 			if (rowhop > 0)
// 			{
// 				if (colhop > 0)
// 				{
// 					if (rowhop > colhop)
// 						pri[3] = pri[3] + 2;
// 					else if (rowhop < colhop)
// 						pri[2] = pri[2] + 2;
// 				}
// 				else
// 					pri[2] = pri[2] + 2;
// 			}
// 			else
// 				if (colhop > 0)
// 					pri[3] = pri[3] + 2;
// 		}
// 		// region: 5, 6
// 		else if (yoffset < 0 && xoffset < 0)
// 		{
// 			// port 1
// 			long rowhop = 0;
// 			if (yoffset == -1 && farthest_neighbor::wpfn().fn(x,y,1) <= des_t[0])
// 			{
// 				rowhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = x - 1; k >= max(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1)); k --)
// 				{
// 					if (farthest_neighbor::wpfn().fn(k, y, 3) <= des_t[1])
// 					{
// 						rowhop = abs(k - x) + abs(des_t[1] - y) + abs(k - des_t[0]);
// 						break;
// 					}
// 				}
// 			}
// 			// port 3
// 			long colhop = 0;
// 			if (xoffset == -1 && farthest_neighbor::wpfn().fn(x,y,3) <= des_t[1])
// 			{
// 				colhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = y - 1; k >= max(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3)); k --)
// 				{
// 					if (farthest_neighbor::wpfn().fn(x, k, 1) <= des_t[0])
// 					{
// 						colhop = abs(k - y) + abs(des_t[0] - x) + abs(k - des_t[1]);
// 						break;
// 					}
// 				}
// 			}
// 			if (rowhop > 0)
// 			{
// 				if (colhop > 0)
// 				{
// 					if (rowhop > colhop)
// 						pri[3] = pri[3] + 2;
// 					else if (rowhop < colhop)
// 						pri[1] = pri[1] + 2;
// 				}
// 				else
// 					pri[1] = pri[1] + 2;
// 			}
// 			else
// 				if (colhop > 0)
// 					pri[3] = pri[3] + 2;
// 		}
// 		// region: 7, 8
// 		else if (yoffset > 0 && xoffset < 0)
// 		{
// 			// port 1
// 			long rowhop = 0;
// 			if (yoffset == 1 && farthest_neighbor::wpfn().fn(x,y,1) <= des_t[0])
// 			{
// 				rowhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = x - 1; k >= max(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1)); k --)
// 				{
// 					if (farthest_neighbor::wpfn().fn(k, y, 4) >= des_t[1])
// 					{
// 						rowhop = abs(k - x) + abs(des_t[1] - y) + abs(k - des_t[0]);
// 						break;
// 					}
// 				}
// 			}
// 			// port 4
// 			long colhop = 0;
// 			if (xoffset == -1 && farthest_neighbor::wpfn().fn(x,y,4) >= des_t[1])
// 			{
// 				colhop = abs(yoffset) + abs(xoffset);
// 			}
// 			else
// 			{
// 				for (long k = y + 1; k <= min(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4)); k ++)
// 				{
// 					if (farthest_neighbor::wpfn().fn(x, k, 1) <= des_t[0])
// 					{
// 						colhop = abs(k - y) + abs(des_t[0] - x) + abs(k - des_t[1]);
// 						break;
// 					}
// 				}
// 			}
// 			if (rowhop > 0)
// 			{
// 				if (colhop > 0)
// 				{
// 					if (rowhop > colhop)
// 						pri[4] = pri[4] + 2;
// 					else if (rowhop < colhop)
// 						pri[1] = pri[1] + 2;
// 				}
// 				else
// 					pri[1] = pri[1] + 2;
// 			}
// 			else
// 				if (colhop > 0)
// 					pri[4] = pri[4] + 2;
// 		}
// 	}
// 	// line
// 	else if (yoffset == 0 || xoffset == 0)
// 	{
// 		// line 1
// 		if (yoffset == 0 && xoffset < 0)
// 		{
// 			// port 1
// 			if (farthest_neighbor::wpfn().ft(x, y, 1) == 1)
// 			{
// 				if (x > 0)
// 				{
// 					if (farthest_neighbor::wpfn().ft(x - 1, y, 1) == 1)
// 					{
// 						pri[1] = pri[1] + 3;
// 					}
// 				}
// 			}
// 				pri[1] = pri[1] + 3;
// 			for (long k = 1; k <= nstep; k ++)
// 			{
// 				int gotflag = 0;
// 				// port 3
// 				if (y - k >= 0)
// 					if (farthest_neighbor::wpfn().fn(x, y - k, 1) <= des_t[0])
// 					{
// 						pri[3] = pri[3] + 3;
// 						gotflag = 1;
// 					}
// 				// port 4
// 				if (y + k < ary_size_)
// 					if (farthest_neighbor::wpfn().fn(x, y + k, 1) <= des_t[0])
// 					{
// 						pri[4] = pri[4] + 3;
// 						gotflag = 1;
// 					}
// 				if (gotflag == 1)
// 					break;
// 			}
// 		}
// 		// line 2
// 		else if (yoffset == 0 && xoffset > 0)
// 		{
// 			// port 2
// 			if (farthest_neighbor::wpfn().fn(x, y, 2) >= des_t[0])
// 				pri[2] = pri[2] + 3;
// 			for (long k = 1; k <= nstep; k ++)
// 			{
// 				int gotflag = 0;
// 				// port 4
// 				if (y + k < ary_size_)
// 					if (farthest_neighbor::wpfn().fn(x, y + k, 2) >= des_t[0])
// 					{
// 						pri[4] = pri[4] + 3;
// 						gotflag = 1;
// 					}
// 				// port 3
// 				if (y - k >= 0)
// 					if (farthest_neighbor::wpfn().fn(x, y - k, 2) >= des_t[0])
// 					{
// 						pri[3] = pri[3] + 3;
// 						gotflag = 1;
// 					}
// 				if (gotflag == 1)
// 					break;
// 			}
// 		}
// 		// line 3
// 		else if (xoffset == 0 && yoffset < 0)
// 		{
// 			// port 3
// 			if (farthest_neighbor::wpfn().fn(x, y, 3) <= des_t[1])
// 				pri[3] = pri[3] + 3;
// 			for (long k = 1; k <= nstep; k ++)
// 			{
// 				int gotflag = 0;
// 				// port 2
// 				if (x + k < ary_size_)
// 					if (farthest_neighbor::wpfn().fn(x + k, y, 3) <= des_t[1])
// 					{
// 						pri[2] = pri[2] + 3;
// 						gotflag = 1;
// 					}
// 				// port 1
// 				if (x - k >= 0)
// 					if (farthest_neighbor::wpfn().fn(x - k, y, 3) <= des_t[1])
// 					{
// 						pri[1] = pri[1] + 3;
// 						gotflag = 1;
// 					}
// 				if (gotflag == 1)
// 					break;
// 			}
// 		}
// 		// line 4
// 		else // xoffset == 0 && yoffset > 0
// 		{
// 			// port 4
// 			if (farthest_neighbor::wpfn().fn(x, y, 4) >= des_t[1])
// 				pri[4] = pri[4] + 3;
// 			for (long k = 1; k <= nstep; k ++)
// 			{
// 				int gotflag = 0;
// 				// port 1
// 				if (x - k >= 0)
// 					if (farthest_neighbor::wpfn().fn(x - k, y, 4) >= des_t[1])
// 					{
// 						pri[1] = pri[1] + 3;
// 						gotflag = 1;
// 					}
// 				// port 2
// 				if (x + k < ary_size_)
// 					if (farthest_neighbor::wpfn().fn(x + k, y, 4) >= des_t[1])
// 					{
// 						pri[2] = pri[2] + 3;
// 						gotflag = 1;
// 					}
// 				if (gotflag == 1)
// 					break;
// 			}
// 		}
// 	}
// 	
// 	// check port
// 	for (int i = 1; i < 5; i ++)
// 		pri[i] = pri[i] * farthest_neighbor::wpfn().ft(address_[0], address_[1], i);
// 
// 	if (s_ph != 0)
// 		pri[s_ph] = 0;
// 
// 	int maxpri = pri[1];
// 	int maxindex = 1;
// 	for (int i = 1; i < 5; i ++)
// 	{
// 		if (pri[i] > maxpri)
// 		{
// 			maxpri = pri[i];
// 			maxindex = i;
// 		}
// 	}
// 	if (maxpri <= 0)
// 		maxindex = s_ph;
// 	
// 	input_module_.add_routing(s_ph, s_vc, VC_type(maxindex,s_vc));
// }

// n-step farthest neighbor algorithm
//***************************************************************************//
void EsynetRouter::algorithmNFRR(long des, long src, long s_ph, long s_vc)
{
/*	add_type des_t = id2addr( des );
	add_type sor_t = id2addr( src );
	long nstep = configuration::globalPointer().nstep();
	long xoffset = des_t[0] - address_[0];
	long yoffset = des_t[1] - address_[1];
	long x = address_[0];
	long y = address_[1];

	int pri[5];
	long searchwidth[2][4];
	
	// modified gradient routing
	if (xoffset > 0)
	{
		if (yoffset > 0)
		{
			// region 1 : ES
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 1; pri[4] = 4;
			}
			// region 2 : SE
			else
			{
				pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 2; pri[4] = 3;
			}
			searchwidth[0][0] = MAX(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1));
			searchwidth[0][1] = MIN(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2));
			searchwidth[0][2] =  1; searchwidth[0][3] = 4;
			searchwidth[1][0] = MAX(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3));
			searchwidth[1][1] = MIN(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4));
			searchwidth[1][2] =  1; searchwidth[1][3] = 2;
		}
		else if (yoffset < 0)
		{
			// region 4 : WS
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 4; pri[4] = 1;
			}
			// region 3 : SW
			else
			{
				pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 3; pri[4] = 2;
			}
			searchwidth[0][0] = MAX(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1));
			searchwidth[0][1] = MIN(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2));
			searchwidth[0][2] =  1; searchwidth[0][3] = 3;
			searchwidth[1][0] = MIN(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4));
			searchwidth[1][1] = MAX(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3));
			searchwidth[1][2] = -1; searchwidth[1][3] = 2;
		}
		else
		// line 2 : DS
		{
			pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 2; pri[4] = 3;
			searchwidth[0][0] = x; searchwidth[0][1] = x;
			searchwidth[0][2] = 0; searchwidth[0][3] = 0;
			searchwidth[1][0] = MAX(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3));
			searchwidth[1][1] = MIN(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4));
			searchwidth[1][2] =  1; searchwidth[1][3] = 2;
		}
	}
	else if (xoffset < 0)
	{
		if (yoffset > 0)
		{
			// region 8 : EN
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
			}
			// region 7 : NE
			else
			{
				pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 2; pri[4] = 3;
			}
			searchwidth[0][0] = MIN(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2));
			searchwidth[0][1] = MAX(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1));
			searchwidth[0][2] = -1; searchwidth[0][3] = 4;
			searchwidth[1][0] = MAX(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3));
			searchwidth[1][1] = MIN(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4));
			searchwidth[1][2] =  1; searchwidth[1][3] = 1;
		}
		else if (yoffset < 0)
		{
			// region 5 : WN
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
			}
			// region 6 : NW
			else
			{
				pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 3; pri[4] = 2;
			}
			searchwidth[0][0] = MIN(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2));
			searchwidth[0][1] = MAX(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1));
			searchwidth[0][2] = -1; searchwidth[0][3] = 3;
			searchwidth[1][0] = MIN(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4));
			searchwidth[1][1] = MAX(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3));
			searchwidth[1][2] = -1; searchwidth[1][3] = 1;
		}
		else
		// line 1 : DN
		{
			pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 3; pri[4] = 2;
			searchwidth[0][0] = x; searchwidth[0][1] = x;
			searchwidth[0][2] = 0; searchwidth[0][3] = 0;
			searchwidth[1][0] = MAX(y - nstep, farthest_neighbor::wpfn().fn(x, y, 3));
			searchwidth[1][1] = MIN(y + nstep, farthest_neighbor::wpfn().fn(x, y, 4));
			searchwidth[1][2] = 1; searchwidth[1][3] = 1;
		}
	}
	else
	{
		if (yoffset > 0)
		// line 4 : DE
		{
			pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
			searchwidth[0][0] = MAX(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1));
			searchwidth[0][1] = MIN(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2));
			searchwidth[0][2] = 1; searchwidth[0][3] = 4;
			searchwidth[1][0] = y; searchwidth[1][1] = y;
			searchwidth[1][2] = 0; searchwidth[1][3] = 0;
		}
		else if (yoffset < 0)
		// line 3 : DW
		{
			pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 4; pri[4] = 1;
			searchwidth[0][0] = MAX(x - nstep, farthest_neighbor::wpfn().fn(x, y, 1));
			searchwidth[0][1] = MIN(x + nstep, farthest_neighbor::wpfn().fn(x, y, 2));
			searchwidth[0][2] = 1; searchwidth[0][3] = 3;
			searchwidth[1][0] = y; searchwidth[1][1] = y;
			searchwidth[1][2] = 0; searchwidth[1][3] = 0;
		}
		else
		// local
		{
			pri[0] = 4; pri[1] = 0; pri[2] = 0; pri[3] = 0; pri[4] = 0;
			searchwidth[0][0] = x; searchwidth[0][1] = x;
			searchwidth[0][2] = 0; searchwidth[0][3] = 0;
			searchwidth[1][0] = y; searchwidth[1][1] = y;
			searchwidth[1][2] = 0; searchwidth[1][3] = 0;
		}
	}

	// search width
	long minhops[2][3] = {{0,0,2}, {0,0,2}};
	// search X axis
	if (searchwidth[0][3] != 0)
	{
		int i = searchwidth[0][0];
		while(true)
		{
//			if (i != x || abs(static_cast<int>(xoffset)) <= 1)
//			{
				if ((searchwidth[0][3] == 3 && farthest_neighbor::wpfn().fn(i, y, 3) <= des_t[1]) ||
					(searchwidth[0][3] == 4 && farthest_neighbor::wpfn().fn(i, y, 4) >= des_t[1]))
				{
					if (nstep >= 2 && i==x && abs(static_cast<int>(y-des_t[1])) <= nstep && i != des_t[0])
					{}
					else
					{
						long hops = abs(static_cast<int>(i - x)) + abs(static_cast<int>(y - des_t[1])) + abs(static_cast<int>(i - des_t[0]));
						if (minhops[0][0] == 0 || minhops[0][0] >= hops)
						{
							minhops[0][0] = hops; minhops[0][1] = i;
						}
					}
				}
//			}
			if (i == searchwidth[0][1])
				break;
			else
				i = i + searchwidth[0][2];
		}
	}
	// search Y axis
	if (searchwidth[1][3] != 0)
	{
		int i = searchwidth[1][0];
		while(true)
		{
//			if (i != y || abs(static_cast<int>(yoffset)) <= 1)
//			{
				if ((searchwidth[1][3] == 1 && farthest_neighbor::wpfn().fn(x, i, 1) <= des_t[0]) ||
					(searchwidth[1][3] == 2 && farthest_neighbor::wpfn().fn(x, i, 2) >= des_t[0]))
				{
					if (nstep >= 2 && i==y && abs(static_cast<int>(x-des_t[0])) <= nstep && i != des_t[1])
					{}
					else
					{
						long hops = abs(static_cast<int>(i - y)) + abs(static_cast<int>(x - des_t[0])) + abs(static_cast<int>(i - des_t[1]));
						if (minhops[1][0] == 0 || minhops[1][0] >= hops)
						{
							minhops[1][0] = hops; minhops[1][1] = i;
						}
					}
				}
//			}
			if (i == searchwidth[1][1])
				break;
			else
				i = i + searchwidth[1][2];
		}
	}
	
	// add priority
	if (minhops[0][0] > 0)
	{
		if (minhops[1][0] > 0)
		{
			if (minhops[0][0] > minhops[1][0])
				minhops[1][2] += 2;
			else if (minhops[0][0] < minhops[1][0])
				minhops[0][2] += 2;
		}
		else
			minhops[0][2] += 2;
	}
	else
		if (minhops[1][0] > 0)
			minhops[1][2] += 2;

	// set priority
	if (minhops[0][0] > 0)
	{
		if (minhops[0][1] < x)
			pri[1] += minhops[0][2];
		else if (minhops[0][1] > x)
			pri[2] += minhops[0][2];
		else
			pri[searchwidth[0][3]] += minhops[0][2];
	}
	if (minhops[1][0] > 0)
	{
		if (minhops[1][1] < y)
			pri[3] += minhops[1][2];
		else if (minhops[1][1] > y)
			pri[4] += minhops[1][2];
		else
			pri[searchwidth[1][3]] += minhops[1][2];
	}
//	cout << des_t[0] << " " << des_t[1] << "|" << x << " " << y << "|" << minhops[0][0] << " " << minhops[0][1] << "|" << minhops[1][0] << " " << minhops[1][1] << "|" << searchwidth[0][0] << " " << searchwidth[0][1] << "|" << searchwidth[1][0] << " " << searchwidth[1][1] << "|" << pri[1] << " " << pri[2] << " " << pri[3] << " " << pri[4] << endl;
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



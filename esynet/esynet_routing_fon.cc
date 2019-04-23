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

// FON neighbor algorithm
//***************************************************************************//
void EsynetRouter::algorithmFON(long des, long src, long s_ph, long s_vc)
{
/*	add_type des_t = id2addr( des );
	add_type sor_t = id2addr( src );
	long nstep = configuration::globalPointer().nstep();
	long xoffset = des_t[0] - address_[0];
	long yoffset = des_t[1] - address_[1];
	long x = address_[0];
	long y = address_[1];

	int dproductive[2];
	int dorthogonal[2];
	// (4) get_prefer_Dir(nc, nd)
	// modified gradient routing
	if (xoffset > 0)
	{
		if (yoffset > 0)
		{
			// region 1
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				dproductive[0] = 4; dproductive[1] = 2; dorthogonal[0] = 1; dorthogonal[1] = 3;
			}
			// region 2
			else
			{
				dproductive[0] = 2; dproductive[1] = 4; dorthogonal[0] = 3; dorthogonal[1] = 1;
			}
		}
		else if (yoffset < 0)
		{
			// region 4
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				dproductive[0] = 3; dproductive[1] = 2; dorthogonal[0] = 1; dorthogonal[1] = 4;
			}
			// region 3
			else
			{
				dproductive[0] = 2; dproductive[1] = 3; dorthogonal[0] = 4; dorthogonal[1] = 1;
			}
		}
		else
		// line 2
		{
			dproductive[0] = 2; dproductive[1] = 1; dorthogonal[0] = 4; dorthogonal[1] = 3;
		}
	}
	else if (xoffset < 0)
	{
		if (yoffset > 0)
		{
			// region 8
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				dproductive[0] = 4; dproductive[1] = 1; dorthogonal[0] = 2; dorthogonal[1] = 3;
			}
			// region 7
			else
			{
				dproductive[0] = 1; dproductive[1] = 4; dorthogonal[0] = 3; dorthogonal[1] = 2;
			}
		}
		else if (yoffset < 0)
		{
			// region 5
			if (abs(static_cast<int>(yoffset)) >= abs(static_cast<int>(xoffset)))
			{
				dproductive[0] = 3; dproductive[1] = 1; dorthogonal[0] = 2; dorthogonal[1] = 4;
			}
			// region 6
			else
			{
				dproductive[0] = 1; dproductive[1] = 3; dorthogonal[0] = 4; dorthogonal[1] = 2;
			}
		}
		else
		// line 1
		{
			dproductive[0] = 1; dproductive[1] = 2; dorthogonal[0] = 4; dorthogonal[1] = 3;
		}
	}
	else
	{
		if (yoffset > 0)
		// line 4
		{
			dproductive[0] = 4; dproductive[1] = 3; dorthogonal[0] = 1; dorthogonal[1] = 2;
		}
		else if (yoffset < 0)
		// line 3
		{
			dproductive[0] = 3; dproductive[1] = 4; dorthogonal[0] = 1; dorthogonal[1] = 2;
		}
		else
		// local
		{
			dproductive[0] = 0; dproductive[1] = 0; dorthogonal[0] = 0; dorthogonal[1] = 0;
		}
	}

	// (1) If packet reaches destination then
	if (yoffset == 0 && xoffset == 0)
	{
		//(2) Route packet to local port
		input_module_.add_routing(s_ph, s_vc, VC_type(0, s_vc));
	}
	// (5) if n_{c} and n_{d} are in the same row or column then
	else if (yoffset == 0 || xoffset == 0)
	{
		// (6) if d_{productive} is faulty then
		if (farthest_neighbor::wpfn().ft(x, y, dproductive[0]) < 1)
		{
			// (7) if both of orthogonal links are not faulty then
			if (farthest_neighbor::wpfn().ft(x, y, dorthogonal[0]) == 1 && farthest_neighbor::wpfn().ft(x, y, dorthogonal[1]) == 1)
			{
				// (8) if d_{in} \notin d_{orthogonal}
				if (s_ph != dorthogonal[0] && s_ph != dorthogonal[1])
				{
					int checkfon[2] = {0,0};
					// (9) for each d_i \in d_{orthogonal} then
					for (int k = 0; k < 2; k ++)
					{
						// (10) Check_Fon(d_i \in d_{orthogonal})
						switch(dorthogonal[k])
						{
							case 1: 
								if (x > 0)
									if (farthest_neighbor::wpfn().ft(x - 1, y, dproductive[0]) == 1)
										checkfon[k] = 1;
								break;
							case 2: 
								if (x < ary_size_[1] - 1)
									if (farthest_neighbor::wpfn().ft(x + 1, y, dproductive[0]) == 1)
										checkfon[k] = 1;
								break;
							case 3: 
								if (y > 0)
									if (farthest_neighbor::wpfn().ft(x, y - 1, dproductive[0]) == 1)
										checkfon[k] = 1;
								break;
							case 4: 
								if (y < ary_size_[0] - 1)
									if (farthest_neighbor::wpfn().ft(x, y + 1, dproductive[0]) == 1)
										checkfon[k] = 1;
								break;
						}
					}
					// (11) Choose one orthogonal link based on FoN_from vectors and stress value
					if (checkfon[0] == 0 && checkfon[1] == 1)
						input_module_.add_routing(s_ph, s_vc, VC_type(dorthogonal[1], s_vc));
					else
						input_module_.add_routing(s_ph, s_vc, VC_type(dorthogonal[0], s_vc));
				}
				// (12) d_{in} is one of orthogonal links then
				else if (s_ph == dorthogonal[0] || s_ph == dorthogonal[1])
				{
					long dorthogonalnotdin;
					if (s_ph != dorthogonal[0])
						dorthogonalnotdin = dorthogonal[0];
					if (s_ph != dorthogonal[1])
						dorthogonalnotdin = dorthogonal[1];

					int checkfon = 0;
					// Check_FoN(d_{orthogonal} \neq d_{in})
					switch(dorthogonalnotdin)
					{
						case 1: 
							if (x > 0)
								if (farthest_neighbor::wpfn().ft(x - 1, y, dproductive[0]) == 1)
									checkfon = 1;
							break;
						case 2: 
							if (x < ary_size_[1] - 1)
								if (farthest_neighbor::wpfn().ft(x + 1, y, dproductive[0]) == 1)
									checkfon = 1;
							break;
						case 3: 
							if (y > 0)
								if (farthest_neighbor::wpfn().ft(x, y - 1, dproductive[0]) == 1)
									checkfon = 1;
							break;
						case 4: 
							if (y < ary_size_[0] - 1)
								if (farthest_neighbor::wpfn().ft(x, y + 1, dproductive[0]) == 1)
									checkfon = 1;
							break;
					}

					// (13) if Check_FoN(d_{orthogonal_i\neq d_{in}) OK then
					if (checkfon == 1)
					{
						// (14) Choose d_{orthogonal_i}
						input_module_.add_routing(s_ph, s_vc, VC_type(dorthogonalnotdin, s_vc));
					}
					// (15) else if the remaining link d_r is not fault then
					else if (farthest_neighbor::wpfn().ft(x, y, dproductive[1]) == 1)
					{
						// (23) Choose d_r
						input_module_.add_routing(s_ph, s_vc, VC_type(dproductive[1], s_vc));
					}
					// (24) else
					else
					{
						// (25) Route back from d_{in}
						input_module_.add_routing(s_ph, s_vc, VC_type(s_ph, s_vc));
					}
				}
			}
			// (19) else if only one orthogonal link d_i is not faulty then
			else if (farthest_neighbor::wpfn().ft(x, y, dorthogonal[0]) == 1 || farthest_neighbor::wpfn().ft(x, y, dorthogonal[1]) == 1)
			{
				long d_i;
				if (farthest_neighbor::wpfn().ft(x, y, dorthogonal[0]) == 1)
					d_i = dorthogonal[0];
				if (farthest_neighbor::wpfn().ft(x, y, dorthogonal[1]) == 1)
					d_i = dorthogonal[1];

				int checkfon = 0;
				// Check_FoN(d_i)
				switch(d_i)
				{
					case 1: 
						if (x > 0)
							if (farthest_neighbor::wpfn().ft(x - 1, y, dproductive[0]) == 1)
								checkfon = 1;
						break;
					case 2: 
						if (x < ary_size_[1] - 1)
							if (farthest_neighbor::wpfn().ft(x + 1, y, dproductive[0]) == 1)
								checkfon = 1;
						break;
					case 3: 
						if (y > 0)
							if (farthest_neighbor::wpfn().ft(x, y - 1, dproductive[0]) == 1)
								checkfon = 1;
						break;
					case 4: 
						if (y < ary_size_[0] - 1)
							if (farthest_neighbor::wpfn().ft(x, y + 1, dproductive[0]) == 1)
								checkfon = 1;
						break;
				}

				// (20) if d_{in} \neq d_i and Check_FoN(d_i) OK then
				if (s_ph != d_i && checkfon == 1)
				{
					// (21) Choose d_i
					input_module_.add_routing(s_ph, s_vc, VC_type(d_i, s_vc));
				}
				// (22) else if the remaining link d_r is not fault then
				else if (farthest_neighbor::wpfn().ft(x, y, dproductive[1]) == 1)
				{
					// (23) Choose d_r
					input_module_.add_routing(s_ph, s_vc, VC_type(dproductive[1], s_vc));
				}
				// (24) else
				else
				{
					// (25) Route back from d_{in}
					input_module_.add_routing(s_ph, s_vc, VC_type(s_ph, s_vc));
				}
			}
			// (26) the remaining link d_r is not fault then
			else if (farthest_neighbor::wpfn().ft(x, y, dproductive[1]) == 1)
			{
				// (27) Chose d_r
				input_module_.add_routing(s_ph, s_vc, VC_type(dproductive[1], s_vc));
			}
			// (28) else
			else
			{
				// (29) Route back from d_{in}
				input_module_.add_routing(s_ph, s_vc, VC_type(s_ph, s_vc));
			}
		}
		// (30) else
		else
		{
			// (31) Choose d_{productive}
			input_module_.add_routing(s_ph, s_vc, VC_type(dproductive[0], s_vc));
		}
	}
	// (32) else
	else
	{
		// (33) if both of d_{productive} are not faulty then
		if (farthest_neighbor::wpfn().ft(x, y, dproductive[0]) == 1 && farthest_neighbor::wpfn().ft(x, y, dproductive[1]) == 1)
		{
			long dproductivestep[2][2];
			long xoffsetstep = xoffset;
			long yoffsetstep = yoffset;
			// (34) for each d_i \in d_{productive}
			for (int k = 0; k < 2; k ++)
			{
				// (35) n_i \leftarrow get_neighbor(d_i)
				switch (dproductive[k])
				{
					case 1: xoffsetstep ++; break;
					case 2: xoffsetstep --; break;
					case 3: yoffsetstep ++; break;
					case 4: yoffsetstep --; break;
				}
				// (36) d_{ni_productive} \leftarrow get_prefer_Dir(n_i,n_d)
				if (xoffsetstep > 0)
				{
					if (yoffsetstep > 0)
					{
						// region 1
						if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
						{
							dproductivestep[k][0] = 4; dproductivestep[k][1] = 2; 
						}
						// region 2
						else
						{
							dproductivestep[k][0] = 2; dproductivestep[k][1] = 4; 
						}
					}
					else if (yoffsetstep < 0)
					{
						// region 4
						if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
						{
							dproductivestep[k][0] = 3; dproductivestep[k][1] = 2; 
						}
						// region 3
						else
						{
							dproductivestep[k][0] = 2; dproductivestep[k][1] = 3; 
						}
					}
					else
					// line 2
					{
						dproductivestep[k][0] = 2; dproductivestep[k][1] = 1; 
					}
				}
				else if (xoffsetstep < 0)
				{
					if (yoffsetstep > 0)
					{
						// region 8
						if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
						{
							dproductivestep[k][0] = 4; dproductivestep[k][1] = 1; 
						}
						// region 7
						else
						{
							dproductivestep[k][0] = 1; dproductivestep[k][1] = 4; 
						}
					}
					else if (yoffsetstep < 0)
					{
						// region 5
						if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
						{
							dproductivestep[k][0] = 3; dproductivestep[k][1] = 1; 
						}
						// region 6
						else
						{
							dproductivestep[k][0] = 1; dproductivestep[k][1] = 3; 
						}
					}
					else
					// line 1
					{
						dproductivestep[k][0] = 1; dproductivestep[k][1] = 2; 
					}
				}
				else
				{
					if (yoffsetstep > 0)
					// line 4
					{
						dproductivestep[k][0] = 4; dproductivestep[k][1] = 3; 
					}
					else if (yoffsetstep < 0)
					// line 3
					{
						dproductivestep[k][0] = 3; dproductivestep[k][1] = 4; 
					}
					else
					// local
					{
						dproductivestep[k][0] = 0; dproductivestep[k][1] = 0; 
					}
				}

			}
			
			// (37) if d_{in} \notin d_{productive} then
			if (s_ph != dproductive[0] && s_ph != dproductive[1])
			{
				int checkfon[2][2] = {{0,0},{0,0}};
				// (38) for each d_j \in d_{ni_productive} then
				for (int k = 0; k < 2; k ++)
				{
					// (39) Check_Fon(d_j)
					for (int l = 0; l < 2; l ++)
					{
						switch(dproductive[k])
						{
							case 1: 
								if (x > 0)
									if (farthest_neighbor::wpfn().ft(x - 1, y, dproductivestep[k][l]) == 1)
										checkfon[k][l] = 1;
								break;
							case 2: 
								if (x < ary_size_[1] - 1)
									if (farthest_neighbor::wpfn().ft(x + 1, y, dproductivestep[k][l]) == 1)
										checkfon[k][l] = 1;
								break;
							case 3: 
								if (y > 0)
									if (farthest_neighbor::wpfn().ft(x, y - 1, dproductivestep[k][l]) == 1)
										checkfon[k][l] = 1;
								break;
							case 4: 
								if (y < ary_size_[0] - 1)
									if (farthest_neighbor::wpfn().ft(x, y + 1, dproductivestep[k][l]) == 1)
										checkfon[k][l] = 1;
								break;
						}
					}
				}
				// (40) Choose one productive link based on FoN_from vectors and streaaa value
				if (checkfon[0][0] == 0 && checkfon[0][1] == 0 && (checkfon[1][0] == 1 || checkfon[1][1] == 1))
					input_module_.add_routing(s_ph, s_vc, VC_type(dproductive[1], s_vc));
				else
					input_module_.add_routing(s_ph, s_vc, VC_type(dproductive[0], s_vc));
			}
			// (41) else if d_{in} is one of productive links then
			else if (s_ph == dproductive[0] || s_ph == dproductive[1])
			{
				long dproductivenotdin;
				if (s_ph != dproductive[0])
					dproductivenotdin = dproductive[0];
				if (s_ph != dproductive[1])
					dproductivenotdin = dproductive[1];

				int checkfon = 0;
				// Check_FoN(d_{orthogonal} \neq d_{in})
				switch(dproductivenotdin)
				{
					case 1: 
						if (x > 0)
							if (farthest_neighbor::wpfn().ft(x - 1, y, dproductive[0]) == 1)
								checkfon = 1;
						break;
					case 2: 
						if (x < ary_size_[1] - 1)
							if (farthest_neighbor::wpfn().ft(x + 1, y, dproductive[0]) == 1)
								checkfon = 1;
						break;
					case 3: 
						if (y > 0)
							if (farthest_neighbor::wpfn().ft(x, y - 1, dproductive[0]) == 1)
								checkfon = 1;
						break;
					case 4: 
						if (y < ary_size_[0] - 1)
							if (farthest_neighbor::wpfn().ft(x, y + 1, dproductive[0]) == 1)
								checkfon = 1;
						break;
				}

				// (42) if Check_FoN(d_{productive_i\neq d_{in}) OK then
				if (checkfon == 1)
				{
					// (43) Choose d_{orthogonal_i}
					input_module_.add_routing(s_ph, s_vc, VC_type(dproductivenotdin, s_vc));
				}
				// (44) else
				else
				{
					// (45) Choose one of remaining links based on fault status and stress value
					input_module_.add_routing(s_ph, s_vc, VC_type(dorthogonal[0], s_vc));
				}
			}
		}
		// (46) only one d_{productive} is not faulty then
		else if (farthest_neighbor::wpfn().ft(x, y, dproductive[0]) == 1 || farthest_neighbor::wpfn().ft(x, y, dproductive[1]) == 1)
		{
			long d_i;
			if (farthest_neighbor::wpfn().ft(x, y, dproductive[0]) == 1)
				d_i = dproductive[0];
			if (farthest_neighbor::wpfn().ft(x, y, dproductive[1]) == 1)
				d_i = dproductive[1];

			long dproductivestep[2];
			long xoffsetstep = xoffset;
			long yoffsetstep = yoffset;

			switch (d_i)
			{
				case 1: xoffsetstep ++; break;
				case 2: xoffsetstep --; break;
				case 3: yoffsetstep ++; break;
				case 4: yoffsetstep --; break;
			}

			if (xoffsetstep > 0)
			{
				if (yoffsetstep > 0)
				{
					// region 1
					if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
					{
						dproductivestep[0] = 4; dproductivestep[1] = 2; 
					}
					// region 2
					else
					{
						dproductivestep[0] = 2; dproductivestep[1] = 4; 
					}
				}
				else if (yoffsetstep < 0)
				{
					// region 4
					if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
					{
						dproductivestep[0] = 3; dproductivestep[1] = 2; 
					}
					// region 3
					else
					{
						dproductivestep[0] = 2; dproductivestep[1] = 3; 
					}
				}
				else
				// line 2
				{
					dproductivestep[0] = 2; dproductivestep[1] = 1; 
				}
			}
			else if (xoffsetstep < 0)
			{
				if (yoffsetstep > 0)
				{
					// region 8
					if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
					{
						dproductivestep[0] = 4; dproductivestep[1] = 1; 
					}
					// region 7
					else
					{
						dproductivestep[0] = 1; dproductivestep[1] = 4; 
					}
				}
				else if (yoffsetstep < 0)
				{
					// region 5
					if (abs(static_cast<int>(yoffsetstep)) >= abs(static_cast<int>(xoffsetstep)))
					{
						dproductivestep[0] = 3; dproductivestep[1] = 1; 
					}
					// region 6
					else
					{
						dproductivestep[0] = 1; dproductivestep[1] = 3; 
					}
				}
				else
				// line 1
				{
					dproductivestep[0] = 1; dproductivestep[1] = 2; 
				}
			}
			else
			{
				if (yoffsetstep > 0)
				// line 4
				{
					dproductivestep[0] = 4; dproductivestep[1] = 3; 
				}
				else if (yoffsetstep < 0)
				// line 3
				{
					dproductivestep[0] = 3; dproductivestep[1] = 4; 
				}
				else
				// local
				{
					dproductivestep[0] = 0; dproductivestep[1] = 0; 
				}
			}
			
			int checkfon = 0;
			for (int k = 0; k < 2; k ++)
			{
			// Check_FoN(d_i)
				switch(d_i)
				{
					case 1: 
						if (x > 0)
							if (farthest_neighbor::wpfn().ft(x - 1, y, dproductivestep[k]) == 1)
								checkfon = 1;
						break;
					case 2: 
						if (x < ary_size_[1] - 1)
							if (farthest_neighbor::wpfn().ft(x + 1, y, dproductivestep[k]) == 1)
								checkfon = 1;
						break;
					case 3: 
						if (y > 0)
							if (farthest_neighbor::wpfn().ft(x, y - 1, dproductivestep[k]) == 1)
								checkfon = 1;
						break;
					case 4: 
						if (y < ary_size_[0] - 1)
							if (farthest_neighbor::wpfn().ft(x, y + 1, dproductivestep[k]) == 1)
								checkfon = 1;
						break;
				}
			}
			// (47) if d_{in} \neq d_{productive_i} and Check_FoN(d_{productive_i}) OK then
			if (s_ph != d_i && checkfon == 1)
			{
				// (48) Choose d_{productive_i}
				input_module_.add_routing(s_ph, s_vc, VC_type(d_i, s_vc));
			}
			// (49) else if the remaining link d_r is not fault then
			else if (farthest_neighbor::wpfn().ft(x, y, dproductive[1]) == 1)
			{
				// (50) Choose d_r
				input_module_.add_routing(s_ph, s_vc, VC_type(dproductive[1], s_vc));
			}
			// (51) else
			else
			{
				// (52) Route back from d_{in}
				input_module_.add_routing(s_ph, s_vc, VC_type(s_ph, s_vc));
			}
		}
		// (53) else
		else
		{
			// (54) Choose one link of the remaining links based on fault status and stress value
			input_module_.add_routing(s_ph, s_vc, VC_type(dorthogonal[0], s_vc));
		}
	}*/
}



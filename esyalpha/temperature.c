#include "RC.h"
#include "flp.h"
#include "util.h"

#include <stdio.h>
#include <math.h>
#include <strings.h>
#include "headers.h"

/* model specific globals	*/
static double factor_pack = C_FACTOR;	/* thermal capacitance fitting factor for package 	*/
static double factor_chip;				/* thermal capacitance fitting factor for silicon	*/
static double factor_int;				/* thermal capacitance fitting factor for interface	*/

/* maximum power density possible (say 150W for a 10mm x 10mm chip)	*/
//#define MAX_PD	(1.5e6)
/* 1500W for us --gjb */
#define MAX_PD	(1.5e7)

static double **b, **c, **inva, **invb, max_slope;

/* creates 3 matrices: invA, B, C: dT + A^-1*BT = A^-1*Power, 
 * C = A^-1 * B. note that A is a diagonal matrix (no lateral
 * capacitances. all capacitances are to ground). so, inva[i][i]
 * (= 1/a[i][i]) is just enough.
 *
 * NOTE: EXTRA nodes: 1 interface bottom, 5 spreader and 5 heat sink nodes
 * (north, south, east, west and bottom).
 */

void create_RC_matrices(flp_t *flp, int omit_lateral)
{
	int i, j, k = 0, n = flp->n_units;
	int **border;
	double **len, *gx, *gy, **g, *c_ver, **t, *gx_sp, *gy_sp;
	double r_sp1, r_sp2, r_hs;	/* lateral resistances to spreader and heatsink	*/

	/* NOTE: *_mid - the vertical R/C from CENTER nodes of spreader 
	 * and heatsink. *_per - the vertical R/C from PERIPHERAL (n,s,e,w) nodes
	 */
	double r_sp_per, r_hs_mid, r_hs_per, c_sp_per, c_hs_mid, c_hs_per;
	double gn_sp=0, gs_sp=0, ge_sp=0, gw_sp=0;

	double w_chip = get_total_width (flp);	/* x-axis	*/
	double l_chip = get_total_height (flp);	/* y-axis	*/

	border = imatrix(n, 4);
	len = matrix(n, n);		/* len[i][j] = length of shared edge bet. i & j	*/
	gx = vector(n);			/* lumped conductances in x direction	*/
	gy = vector(n);			/* lumped conductances in y direction	*/
	gx_sp = vector(n);		/* lateral conductances in the spreader	layer */
	gy_sp = vector(n);
	g = matrix(NL*n+EXTRA, NL*n+EXTRA);	/* g[i][j] = conductance bet. nodes i & j */
	c_ver = vector(NL*n+EXTRA);	/* vertical capacitance	*/

	b = matrix(NL*n+EXTRA, NL*n+EXTRA);	/* B, C, INVA  and INVB are (NL*n+EXTRA)x(NL*n+EXTRA) matrices	*/
	c = matrix(NL*n+EXTRA, NL*n+EXTRA);
	inva = matrix(NL*n+EXTRA, NL*n+EXTRA);
	invb = matrix(NL*n+EXTRA, NL*n+EXTRA);
	t = matrix (NL*n+EXTRA, NL*n+EXTRA);	/* copy of B	*/

	/* compute the silicon fitting factor - see pg 10 of the UVA CS tech report - CS-TR-2003-08	*/
	factor_chip = C_FACTOR * ((SPEC_HEAT_INT / SPEC_HEAT_SI) * (w_chip + 0.88 * t_interface) \
				* (l_chip + 0.88 * t_interface) * t_interface / ( w_chip * l_chip * t_chip) + 1);

	/* fitting factor for interface	 - same rationale as above */
	factor_int = C_FACTOR * ((SPEC_HEAT_CU / SPEC_HEAT_INT) * (w_chip + 0.88 * t_spreader) \
				* (l_chip + 0.88 * t_spreader) * t_spreader / ( w_chip * l_chip * t_interface) + 1);

	/*printf("fitting factors : %lf, %lf\n", factor_chip, factor_int);	*/

	/* gx's and gy's of blocks	*/
	for (i = 0; i < n; i++) {
		/* at the silicon layer	*/
		if (omit_lateral) {
			gx[i] = gy[i] = 0;
		}
		else {
			gx[i] = 1.0/getr(K_SI, flp->units[i].height, flp->units[i].width, l_chip, t_chip);
			gy[i] = 1.0/getr(K_SI, flp->units[i].width, flp->units[i].height, w_chip, t_chip);
		}

		/* at the spreader layer	*/
		gx_sp[i] = 1.0/getr(K_CU, flp->units[i].height, flp->units[i].width, l_chip, t_spreader);
		gy_sp[i] = 1.0/getr(K_CU, flp->units[i].width, flp->units[i].height, w_chip, t_spreader);
	}

	/* shared lengths between blocks	*/
	for (i = 0; i < n; i++) 
		for (j = i; j < n; j++) 
			len[i][j] = len[j][i] = get_shared_len(flp, i, j);

	/* lateral R's of spreader and sink */
	r_sp1 = getr(K_CU, (s_spreader+3*w_chip)/4.0, (s_spreader-w_chip)/4.0, w_chip, t_spreader);
	r_sp2 = getr(K_CU, (3*s_spreader+w_chip)/4.0, (s_spreader-w_chip)/4.0, (s_spreader+3*w_chip)/4.0, t_spreader);
	r_hs = getr(K_CU, (s_sink+3*s_spreader)/4.0, (s_sink-s_spreader)/4.0, s_spreader, t_sink);

	/* vertical R's and C's of spreader and sink */
	r_sp_per = RHO_CU * t_spreader * 4.0 / (s_spreader * s_spreader - w_chip*l_chip);
	c_sp_per = factor_pack * SPEC_HEAT_CU * t_spreader * (s_spreader * s_spreader - w_chip*l_chip) / 4.0;
	r_hs_mid = RHO_CU * t_sink / (s_spreader*s_spreader);
	c_hs_mid = factor_pack * SPEC_HEAT_CU * t_sink * (s_spreader * s_spreader);
	r_hs_per = RHO_CU * t_sink * 4.0 / (s_sink * s_sink - s_spreader*s_spreader);
	c_hs_per = factor_pack * SPEC_HEAT_CU * t_sink * (s_sink * s_sink - s_spreader*s_spreader) / 4.0;

	/* short the R's from block centers to a particular chip edge	*/
	for (i = 0; i < n; i++) {
		if (eq(flp->units[i].bottomy + flp->units[i].height, l_chip)) {
			gn_sp += gy_sp[i];
			border[i][2] = 1;	/* block is on northern border 	*/
		}	
		if (eq(flp->units[i].bottomy, 0)) {
			gs_sp += gy_sp[i];
			border[i][3] = 1;	/* block is on southern border	*/
		}	
		if (eq(flp->units[i].leftx + flp->units[i].width, w_chip)) {
			ge_sp += gx_sp[i];
			border[i][1] = 1;	/* block is on eastern border	*/
		}	
		if (eq(flp->units[i].leftx, 0)) {
			gw_sp += gx_sp[i];
			border[i][0] = 1;	/* block is on western border	*/
		}	
	}

	/* overall R and C between nodes */
	for (i = 0; i < n; i++) {
		double area = (flp->units[i].height * flp->units[i].width);
		/* 
		 * amongst functional units	in the various layers
		 * resistances in the interface layer are assumed 
		 * to be infinite
		 */
		for (j = 0; j < n; j++) {
			double part = 0, part_sp = 0;
			if (is_horiz_adj(flp, i, j)) {
				part = gx[i] / flp->units[i].height;
				part_sp = gx_sp[i] / flp->units[i].height;
			}
			else if (is_vert_adj(flp, i,j))  {
				part = gy[i] / flp->units[i].width;
				part_sp = gy_sp[i] / flp->units[i].width;
			}
			g[i][j] = part * len[i][j];
			g[HSP*n+i][HSP*n+j] = part_sp * len[i][j];
		}

 		/* vertical g's in the silicon layer	*/
		g[i][IFACE*n+i]=g[IFACE*n+i][i]=2.0/(RHO_SI * t_chip / area);
 		/* vertical g's in the interface layer	*/
		g[IFACE*n+i][HSP*n+i]=g[HSP*n+i][IFACE*n+i]=2.0/(RHO_INT * t_interface / area);
		/* vertical g's in the spreader layer	*/
		g[HSP*n+i][NL*n+SP_B]=g[NL*n+SP_B][HSP*n+i]=2.0/(RHO_CU * t_spreader / area);

		/* C's from functional units to ground	*/
		c_ver[i] = factor_chip * SPEC_HEAT_SI * t_chip * area;
		/* C's from interface portion of the functional units to ground	*/
		c_ver[IFACE*n+i] = factor_int * SPEC_HEAT_INT * t_interface * area;
		/* C's from spreader portion of the functional units to ground	*/
		c_ver[HSP*n+i] = factor_pack * SPEC_HEAT_CU * t_spreader * area;

		/* lateral g's from block center (spreader layer) to peripheral (n,s,e,w) spreader nodes	*/
		g[HSP*n+i][NL*n+SP_N]=g[NL*n+SP_N][HSP*n+i]=2.0*border[i][2]/((1.0/gy_sp[i])+r_sp1*gn_sp/gy_sp[i]);
		g[HSP*n+i][NL*n+SP_S]=g[NL*n+SP_S][HSP*n+i]=2.0*border[i][3]/((1.0/gy_sp[i])+r_sp1*gs_sp/gy_sp[i]);
		g[HSP*n+i][NL*n+SP_E]=g[NL*n+SP_E][HSP*n+i]=2.0*border[i][1]/((1.0/gx_sp[i])+r_sp1*ge_sp/gx_sp[i]);
		g[HSP*n+i][NL*n+SP_W]=g[NL*n+SP_W][HSP*n+i]=2.0*border[i][0]/((1.0/gx_sp[i])+r_sp1*gw_sp/gx_sp[i]);
	}

	/* max slope (max_power * max_vertical_R / vertical RC time constant) for silicon	*/
	max_slope = MAX_PD / (factor_chip * t_chip * SPEC_HEAT_SI);

	/* vertical g's and C's between central nodes	*/
 	/* between spreader bottom and sink bottom	*/
	g[NL*n+SINK_B][NL*n+SP_B]=g[NL*n+SP_B][NL*n+SINK_B]=2.0/r_hs_mid;
 	/* from spreader bottom to ground	*/
	c_ver[NL*n+SP_B]=c_hs_mid;
 	/* from sink bottom to ground	*/
	c_ver[NL*n+SINK_B] = factor_pack * c_convec;

	/* g's and C's from peripheral(n,s,e,w) nodes	*/
	for (i = 1; i <= 4; i++) {
 		/* vertical g's between peripheral spreader nodes and spreader bottom */
		g[NL*n+SP_B-i][NL*n+SP_B]=g[NL*n+SP_B][NL*n+SP_B-i]=2.0/r_sp_per;
 		/* lateral g's between peripheral spreader nodes and peripheral sink nodes	*/
		g[NL*n+SP_B-i][NL*n+SINK_B-i]=g[NL*n+SINK_B-i][NL*n+SP_B-i]=2.0/(r_hs + r_sp2);
 		/* vertical g's between peripheral sink nodes and sink bottom	*/
		g[NL*n+SINK_B-i][NL*n+SINK_B]=g[NL*n+SINK_B][NL*n+SINK_B-i]=2.0/r_hs_per;
 		/* from peripheral spreader nodes to ground	*/
		c_ver[NL*n+SP_B-i]=c_sp_per;
 		/* from peripheral sink nodes to ground	*/
		c_ver[NL*n+SINK_B-i]=c_hs_per;
	}

	/* calculate matrices A, B such that A(dT) + BT = POWER */

	for (i = 0; i < NL*n+EXTRA; i++) {
		for (j = 0; j < NL*n+EXTRA; j++) {
			if (i==j) {
				inva[i][j] = 1.0/c_ver[i];
				if (i == NL*n+SINK_B)	/* sink bottom */
					b[i][j] += 1.0 / r_convec;
				for (k = 0; k < NL*n+EXTRA; k++) {
					if ((g[i][k]==0.0)||(g[k][i])==0.0) 
						continue;
					else 
					/* here is why the 2.0 factor comes when calculating g[][]	*/
						b[i][j] += 1.0/((1.0/g[i][k])+(1.0/g[k][i]));
				}
			} else {
				inva[i][j]=0.0;
				if ((g[i][j]==0.0)||(g[j][i])==0.0)
					b[i][j]=0.0;
				else
					b[i][j]=-1.0/((1.0/g[i][j])+(1.0/g[j][i]));
			}
		}
	}

	/* we are always going to use the eqn dT + A^-1 * B T = A^-1 * POWER. so, store  C = A^-1 * B	*/
	matmult(c, inva, b, NL*n+EXTRA);
	/* we will also be needing INVB so store it too	*/
	copy_matrix(t, b, NL*n+EXTRA, NL*n+EXTRA);
	matinv(invb, t, NL*n+EXTRA);
/*	dump_vector(c_ver, NL*n+EXTRA);	*/
/*	dump_matrix(g, NL*n+EXTRA, NL*n+EXTRA);	*/
/*	dump_matrix(c, NL*n+EXTRA, NL*n+EXTRA);	*/

	/* cleanup */
	free_matrix(t, NL*n+EXTRA);
	free_matrix(g, NL*n+EXTRA);
	free_matrix(len, n);
	free_imatrix(border, n);
	free_vector(c_ver);
	free_vector(gx);
	free_vector(gy);
	free_vector(gx_sp);
	free_vector(gy_sp);
}

/* setting internal node power numbers	*/
void set_internal_power (double *pow, int n_units)
{
	int i;
	for (i=n_units; i < NL*n_units+SINK_B; i++)
		pow[i] = 0;
	pow[NL*n_units+SINK_B] = ambient / r_convec;
}

/* power and temp should both be alloced using hotspot_vector. 
 * 'b' is the 'thermal conductance' matrix. i.e, b * temp = power
 *  => temp = invb * power
 */
void steady_state_temp(double *power, double *temp, int n_units) 
{
	/* set power numbers for the virtual nodes */
	set_internal_power(power, n_units);

	/* find temperatures	*/
	matvectmult(temp, invb, power, NL*n_units+EXTRA);
}

/* required precision in degrees	*/
#define PRECISION	0.001
#define TOO_LONG	100000
#define MIN_ITER	1

/* compute_temp: solve for temperature from the equation dT + CT = inv_A * Power 
 * Given the temperature (temp) at time t, the power dissipation per cycle during the 
 * last interval (time_elapsed), find the new temperature at time t+time_elapsed.
 * power and temp should both be alloced using hotspot_vector
 */

void compute_temp(double *power, double *temp, int n_units, double time_elapsed)
{
	int i;
	double *pow, h, n_iter;
	pow = vector(NL*n_units+EXTRA);
	
	/* set power numbers for the virtual nodes */
	set_internal_power(power, n_units);

	/* find (inv_A)*POWER */
	matvectmult(pow, inva, power, NL*n_units+EXTRA);

 	/* step size for 4th-order Runge-Kutta  - assume worst case	*/
	h = PRECISION / max_slope;
	n_iter = time_elapsed / h;
	n_iter = (n_iter > MIN_ITER) ? n_iter : MIN_ITER;	/* do at least MIN_ITER iterations	*/
	h = time_elapsed / n_iter;
	
	if (n_iter >= TOO_LONG)
		fprintf(stderr, "warning: calling interval too large, performing %.0f iterations - it may take REALLY long\n", n_iter);
	
	/* Obtain temp at time (t+h). 
	 * Instead of getting the temperature at t+h directly, we do it 
	 * in n_iter steps to reduce the error due to rk4
	 */
	for (i = 0; i < n_iter; i++) 	
		rk4(c, temp, pow, NL*n_units+EXTRA, h, temp);

	free_vector(pow);
}

/* differs from 'vector()' in that memory for internal nodes is also allocated	*/
double *hotspot_vector(int n_units)
{
	return vector(NL*n_units+EXTRA);
}

/* sets the temperature of a vector 'temp' allocated using 'hotspot_vector'	*/
void set_temp(double *temp, int n_units, double val)
{
	int i;
	for(i=0; i < NL*n_units + EXTRA; i++)
		temp[i] = val;
}

/* dump temperature vector alloced using 'hotspot_vector' to 'file' */ 
void dump_temp(flp_t *flp, double *temp, char *file)
{
	int i;
	char str[STR_SIZE];
	FILE *fp = fopen (file, "w");
	if (!fp) {
		sprintf (str,"error: %s could not be opened for writing", file);
		fatal(str);
	}
	/* on chip temperatures	*/
	for (i=0; i < flp->n_units; i++)
		fprintf(fp, "%s\t%.1f\n", flp->units[i].name, temp[i]);

	/* interface temperatures	*/
	for (i=0; i < flp->n_units; i++)
		fprintf(fp, "iface_%s\t%.1f\n", flp->units[i].name, temp[IFACE*flp->n_units+i]);

	/* spreader temperatures	*/
	for (i=0; i < flp->n_units; i++)
		fprintf(fp, "hsp_%s\t%.1f\n", flp->units[i].name, temp[HSP*flp->n_units+i]);

	/* internal node temperatures	*/
	for (i=0; i < EXTRA; i++) {
		sprintf(str, "inode_%d", i);
		fprintf(fp, "%s\t%.1f\n", str, temp[i+NL*flp->n_units]);
	}
	fclose(fp);	
}

/* 
 * read temperature vector alloced using 'hotspot_vector' from 'file'
 * which was dumped using 'dump_vector'. values are clipped to thermal
 * threshold based on 'clip'
 */ 
void read_temp(flp_t *flp, double *temp, char *file, int clip)
{
	int i, idx;
	double max=0, val;
	char str[STR_SIZE], name[STR_SIZE];
	FILE *fp = fopen (file, "r");
	if (!fp) {
		sprintf (str,"error: %s could not be opened for reading", file);
		fatal(str);
	}	

	/* find max temp on the chip	*/
	for (i=0; i < flp->n_units; i++) {
		fgets(str, STR_SIZE, fp);
		if (feof(fp))
			fatal("not enough lines in temperature file");
		if (sscanf(str, "%s%lf", name, &val) != 2)
			fatal("invalid temperature file format: ");
		idx = get_blk_index(flp, name);
		if (idx >= 0)
			temp[idx] = val;
		else	/* since get_blk_index calls fatal, the line below cannot be reached	*/
			fatal ("unit in temperature file not found in floorplan");
		if (temp[idx] > max)
			max = temp[idx];
	}

	/* interface material temperatures	*/
	for (i=0; i < flp->n_units; i++) {
		fgets(str, STR_SIZE, fp);
		if (feof(fp))
			fatal("not enough lines in temperature file");
		if (sscanf(str, "iface_%s%lf", name, &val) != 2)
			fatal("invalid temperature file format: missing interface");
		idx = get_blk_index(flp, name);
		if (idx >= 0)
			temp[idx+IFACE*flp->n_units] = val;
		else	/* since get_blk_index calls fatal, the line below cannot be reached	*/
			fatal ("unit in temperature file not found in floorplan");
	}

	/* heat spreader temperatures	*/
	for (i=0; i < flp->n_units; i++) {
		fgets(str, STR_SIZE, fp);
		if (feof(fp))
			fatal("not enough lines in temperature file");
		if (sscanf(str, "hsp_%s%lf", name, &val) != 2)
			fatal("3invalid temperature file format");
		idx = get_blk_index(flp, name);
		if (idx >= 0)
			temp[idx+HSP*flp->n_units] = val;
		else	/* since get_blk_index calls fatal, the line below cannot be reached	*/
			fatal ("unit in temperature file not found in floorplan");
	}

	/* internal node temperatures	*/	
	for (i=0; i < EXTRA; i++) {
		fgets(str, STR_SIZE, fp);
		if (feof(fp))
			fatal("not enough lines in temperature file");
		if (sscanf(str, "%s%lf", name, &val) != 2)
			fatal("4invalid temperature file format");
		sprintf(str, "inode_%d", i);
		if (strcasecmp(str, name))
			fatal("5invalid temperature file format");
		temp[i+NL*flp->n_units] = val;	
	}

	fclose(fp);	

	/* clipping	*/
	if (clip && (max > thermal_threshold)) {
		/* if max has to be brought down to thermal_threshold, 
		 * (w.r.t the ambient) what is the scale down factor?
		 */
		double factor = (thermal_threshold - ambient) / (max - ambient);
	
		/* scale down all temperature differences (from ambient) by the same factor	*/
		for (i=0; i < NL*flp->n_units + EXTRA; i++)
			temp[i] = (temp[i]-ambient)*factor + ambient;
	}
}

void cleanup_hotspot(int n_units)
{
	free_matrix(inva, NL*n_units+EXTRA);
	free_matrix(b, NL*n_units+EXTRA);
	free_matrix(invb, NL*n_units+EXTRA);
	free_matrix(c, NL*n_units+EXTRA);
}

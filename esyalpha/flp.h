/*
 * $Log: flp.h,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:13  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:56  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22  21:20:25  etan
 * Initial revision
 *
 * Revision 1.8  2004/07/13 21:19:34  grbriggs
 * Increase MAX_UNITS
 *
 * Revision 1.7  2004/06/11 16:31:51  grbriggs
 * Fix the definition of print_flp_fig so that it compiles.
 *
 */

#ifndef __FLP_H_
#define __FLP_H_

/* for the use of "FILE" */
#include <stdio.h>

#define STR_SIZE	256
/* it's ok if max_units is larger than needed */
#define MAX_UNITS	520 /*16 clusters * 28 thermal nodes each + 32 l2cache + 11 heatsink*/

/* functional unit placement definition	*/
typedef struct unit_t_st
{
	char name[STR_SIZE];
	double width;
	double height;
	double leftx;
	double bottomy;
}unit_t;

/* floorplan definition	*/
typedef struct flp_t_st
{
	unit_t *units;
	int n_units;
} flp_t;

/* routines	*/

/* 
 * print the floorplan in a FIG like format 
 * that can be read by tofig.pl to produce 
 * an xfig output 
 */
void print_flp_fig (flp_t *flp, FILE *outfile);

/* read floorplan file and allocate memory	*/
flp_t *read_flp(char *file);

/* memory uninitialization	*/
void free_flp(flp_t *flp);

/* get unit index from its name	*/
int get_blk_index(flp_t *flp, char *name);

/* test if unit exists, from its name */
int exists_blk_index(flp_t *flp, char *name);

/* are the units horizontally adjacent?	*/
int is_horiz_adj(flp_t *flp, int i, int j);

/* are the units vertically adjacent?	*/
int is_vert_adj (flp_t *flp, int i, int j);

/* shared length between units	*/
double get_shared_len(flp_t *flp, int i, int j);

/* total chip width	*/
double get_total_width(flp_t *flp);

/* total chip height */
double get_total_height(flp_t *flp);

#endif

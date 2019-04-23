/*
 * $Log: cmt.c,v $
 * Revision 1.1.1.1  2011/03/29 01:04:34  huq
 * setup project CMP
 *
 * Revision 1.1.1.1  2010/02/18 21:22:15  xue
 * CMP_NETWORK
 *
 * Revision 1.1.1.1  2008/10/20 15:44:52  garg
 * dir structure
 *
 * Revision 1.1  2004/09/22 21:20:25  etan
 * Initial revision
 *
 * Revision 1.16  2004/08/03 18:57:38  grbriggs
 * Fix compiler warnings.
 *
 * Revision 1.15  2004/07/25 01:32:36  grbriggs
 * Make n_way_adjacent also assign banks to threads via the same policy.
 *
 * Revision 1.14  2004/07/22 16:37:02  ninelson
 * added n_way_adjacent.
 *
 * Revision 1.13  2004/07/12 18:05:33  ninelson
 * N_WAY to n_way
 *
 * Revision 1.12  2004/06/30 15:28:56  ninelson
 *  Longer error message.
 *
 * Revision 1.11  2004/06/24 20:34:27  ninelson
 * More changes for MTA
 *
 * Revision 1.10  2004/06/24 19:03:58  grbriggs
 * Add a warning for an unimplemented combination of #defines.
 *
 * Revision 1.9  2004/06/23 17:52:19  grbriggs
 * Oodles of code from Ali has been added now.
 *
 */

#include "headers.h"

context* get_next_thread()
{
}
void calculate_priority(void)
{
}
void calculate_interval_priority(void)
{
}
void update_examin_n_limit(void)
{
}
void n_way_cache_adapt(void)
{
}

void MTA_printInfo(int id)
{
}

void update_n_limit(void)
{
}
void general_stat(void)
{
 
}
void  steer_init(void)
{
}

void  update_steer_list(void)
{
}

int get_cluster(int in1, int in2, int in3, struct RUU_station *rs, int threadid)
{
}

void check_RUU_LSQ_match(void)
{
}

void check_reg(void)
{
}


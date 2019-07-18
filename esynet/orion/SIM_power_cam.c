#include <math.h>
#include "SIM_power.h"
#include "SIM_power_array_internal.h"
#include "SIM_power_array.h"


/*============================== wordlines ==============================*/

/* each time one wordline 1->0, another wordline 0->1, so no 1/2 */
double SIM_cam_wordline_cap( u_int cols, double wire_cap, double tx_width )
{
  double Ctotal, Cline, psize, nsize;

  /* part 1: line cap, including gate cap of pass tx's and metal cap */ 
  Ctotal = Cline = SIM_power_gatecappass( tx_width, 2 ) * cols + wire_cap;
  
  /* part 2: input driver */
  psize = SIM_power_driver_size( Cline, Period / 8 );
  nsize = psize * Wdecinvn / Wdecinvp; 
  /* WHS: 20 should go to PARM */
  Ctotal += SIM_power_draincap( nsize, NCH, 1 ) + SIM_power_draincap( psize, PCH, 1 ) +
            SIM_power_gatecap( nsize + psize, 20 );

  return Ctotal;
}

/*============================== wordlines ==============================*/



/*============================== tag comparator ==============================*/

/* tag and tagbar switch simultaneously, so no 1/2 */
double SIM_cam_comp_tagline_cap( u_int rows, double taglinelength )
{
  double Ctotal;

  /* part 1: line cap, including drain cap of pass tx's and metal cap */
  Ctotal = rows * SIM_power_gatecap( Wcomparen2, 2 ) + CC3M2metal * taglinelength;

  /* part 2: input driver */
  Ctotal += SIM_power_draincap( Wcompdrivern, NCH, 1 ) + SIM_power_draincap( Wcompdriverp, PCH, 1 ) +
	    SIM_power_gatecap( Wcompdrivern + Wcompdriverp, 1 );

  return Ctotal;
}


/* upon mismatch, matchline 1->0, then 0->1 on next precharging, so no 1/2 */
double SIM_cam_comp_mismatch_cap( u_int n_bits, u_int n_pre, double matchline_len )
{
  double Ctotal;

  /* part 1: drain cap of precharge tx */
  Ctotal = n_pre * SIM_power_draincap( Wmatchpchg, PCH, 1 );
  
  /* part 2: drain cap of comparator tx */
  Ctotal += n_bits * ( SIM_power_draincap( Wcomparen1, NCH, 1 ) + SIM_power_draincap( Wcomparen1, NCH, 2 ));

  /* part 3: metal cap of matchline */
  Ctotal += CC3M3metal * matchline_len;
  
  /* FIXME: I don't understand the Wattch code here */
  /* part 4: nor gate of valid output */
  Ctotal += SIM_power_gatecap( Wmatchnorn + Wmatchnorp, 10 );

  return Ctotal;
}


/* WHS: subtle difference of valid output between cache and inst window:
 *   fully-associative cache: nor all matchlines of the same port
 *   instruction window:      nor all matchlines of the same tag line */   
/* upon miss, valid output switches twice in one cycle, so no 1/2 */
double SIM_cam_comp_miss_cap( u_int assoc )
{
  /* drain cap of valid output */
  return ( assoc * SIM_power_draincap( Wmatchnorn, NCH, 1 ) + SIM_power_draincap( Wmatchnorp, PCH, assoc ));
}

/*============================== tag comparator ==============================*/



/*============================== memory cell ==============================*/

/* WHS: use Wmemcella and Wmemcellbscale to compute tx width of memory cell */
double SIM_cam_tag_mem_cap( u_int read_ports, u_int write_ports, int share_rw, u_int end, int only_write )
{
  double Ctotal;

  /* part 1: drain capacitance of pass transistors */
  if ( only_write )
    Ctotal = SIM_power_draincap( Wmemcellw, NCH, 1 ) * write_ports;
  else {
    Ctotal = SIM_power_draincap( Wmemcellr, NCH, 1 ) * read_ports * end / 2;
    if ( ! share_rw )
      Ctotal += SIM_power_draincap( Wmemcellw, NCH, 1 ) * write_ports;
  }

  /* has coefficient ( 1/2 * 2 ) */
  /* part 2: drain capacitance of memory cell */
  Ctotal += SIM_power_draincap( Wmemcella, NCH, 1 ) + SIM_power_draincap( Wmemcella * Wmemcellbscale, PCH, 1 );

  /* has coefficient ( 1/2 * 2 ) */
  /* part 3: gate capacitance of memory cell */
  Ctotal += SIM_power_gatecap( Wmemcella, 1 ) + SIM_power_gatecap( Wmemcella * Wmemcellbscale, 1 );

  /* has coefficient ( 1/2 * 2 ) */
  /* part 4: gate capacitance of comparator */
  Ctotal += SIM_power_gatecap( Wcomparen1, 2 ) * read_ports;

  return Ctotal;
}


double SIM_cam_data_mem_cap( u_int read_ports, u_int write_ports )
{
  double Ctotal;

  /* has coefficient ( 1/2 * 2 ) */
  /* part 1: drain capacitance of pass transistors */
  Ctotal = SIM_power_draincap( Wmemcellw, NCH, 1 ) * write_ports;

  /* has coefficient ( 1/2 * 2 ) */
  /* part 2: drain capacitance of memory cell */
  Ctotal += SIM_power_draincap( Wmemcella, NCH, 1 ) + SIM_power_draincap( Wmemcella * Wmemcellbscale, PCH, 1 );

  /* has coefficient ( 1/2 * 2 ) */
  /* part 3: gate capacitance of memory cell */
  Ctotal += SIM_power_gatecap( Wmemcella, 1 ) + SIM_power_gatecap( Wmemcella * Wmemcellbscale, 1 );

  /* part 4: gate capacitance of output driver */
  Ctotal += ( SIM_power_gatecap( Woutdrvnandn, 1 ) + SIM_power_gatecap( Woutdrvnandp, 1 ) +
              SIM_power_gatecap( Woutdrvnorn, 1 ) + SIM_power_gatecap( Woutdrvnorp, 1 )) / 2 * read_ports;

  return Ctotal;
}

/*============================== memory cell ==============================*/

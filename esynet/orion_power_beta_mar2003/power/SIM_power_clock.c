#include <math.h>
#include "SIM_power.h"


/* very rough clock power estimates */
double SIM_power_total_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double pipereg_clockcap=0;
  double global_buffercap = 0;
  double Clockpower;

  double num_piperegs;

  int npreg_width = SIM_power_logtwo(PARM(RUU_size));

  /* Assume say 8 stages (kinda low now).
     FIXME: this could be a lot better; user could input
     number of pipestages, etc  */

  /* assume 8 pipe stages and try to estimate bits per pipe stage */
  /* pipe stage 0/1 */
  num_piperegs = PARM(ruu_issue_width)*PARM(inst_length) + PARM(data_width);
  /* pipe stage 1/2 */
  num_piperegs += PARM(ruu_issue_width)*(PARM(inst_length) + 3 * PARM(RUU_size));
  /* pipe stage 2/3 */
  num_piperegs += PARM(ruu_issue_width)*(PARM(inst_length) + 3 * PARM(RUU_size));
  /* pipe stage 3/4 */
  num_piperegs += PARM(ruu_issue_width)*(3 * npreg_width + (BIGONE << PARM(opcode_length)));
  /* pipe stage 4/5 */
  num_piperegs += PARM(ruu_issue_width)*(2*PARM(data_width) + (BIGONE << PARM(opcode_length)));
  /* pipe stage 5/6 */
  num_piperegs += PARM(ruu_issue_width)*(PARM(data_width) + (BIGONE << PARM(opcode_length)));
  /* pipe stage 6/7 */
  num_piperegs += PARM(ruu_issue_width)*(PARM(data_width) + (BIGONE << PARM(opcode_length)));
  /* pipe stage 7/8 */
  num_piperegs += PARM(ruu_issue_width)*(PARM(data_width) + (BIGONE << PARM(opcode_length)));

  /* assume 50% extra in control signals (rule of thumb) */
  num_piperegs = num_piperegs * 1.5;

  /* WHS: 10.0 should go to PARM */
  pipereg_clockcap = num_piperegs * 4*SIM_power_gatecap(10.0,0);

  /* estimate based on 3% of die being in clock metal */
  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  /* another estimate */
  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*SIM_power_gatecap(1000.0,10.0)+16*SIM_power_gatecap(200,10.0)+16*8*2*SIM_power_gatecap(100.0,10.00) + 2*SIM_power_gatecap(.29*1e6,10.0);
  /* global_clockcap is computed within each array structure for pre-charge tx's*/
  Ctotal = Cline+global_clockcap+pipereg_clockcap+global_buffercap;

  if(verbose)
    fprintf(stderr,"num_piperegs == %f\n",num_piperegs);

  /* add I_ADD Clockcap and F_ADD Clockcap */
  Clockpower = Ctotal*Powerfactor + PARM(res_ialu)*I_ADD_CLOCK + PARM(res_fpalu)*F_ADD_CLOCK;

  if(verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Clockpower);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
    fprintf(stderr," Global Clock Cap (Explicit) (W): %g\n",global_clockcap*Powerfactor+I_ADD_CLOCK+F_ADD_CLOCK);
    fprintf(stderr," Global Clock Cap (Implicit) (W): %g\n",pipereg_clockcap*Powerfactor);
  }
  return(Clockpower);
}


/* very rough global clock power estimates */
double SIM_power_global_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double global_buffercap = 0;

  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*SIM_power_gatecap(1000.0,10.0)+16*SIM_power_gatecap(200,10.0)+16*8*2*SIM_power_gatecap(100.0,10.00) + 2*SIM_power_gatecap(.29*1e6,10.0);
  Ctotal = Cline+global_buffercap;

  if(verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Ctotal*Powerfactor);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
  }

  return(Ctotal*Powerfactor);
}

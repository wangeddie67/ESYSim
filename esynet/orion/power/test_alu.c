#include <stdio.h>
#include "SIM_power_ALU.h"


int main(int argc, char **argv)
{
  SIM_power_ALU_t alu;

  SIM_power_ALU_init(&alu, PLX_ALU, 8);

  printf("maximum energy: %g\n", SIM_ALU_stat_energy(&alu, 1));
  printf("average energy: %g\n", SIM_ALU_stat_energy(&alu, 0));

  exit(0);
}

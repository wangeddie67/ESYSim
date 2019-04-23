#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "SIM_power.h"
#include "SIM_power_router.h"
#include "SIM_router_power.h"


char path[2048];
u_int print_depth = 4;
int max_flag = AVG_ENERGY, plot_flag = 1;
double load = 1;
char name[6] = {'r','o','u','t','e','r'};

void init_router_power()
{
  FUNC(SIM_router_power_init, &GLOB(router_info), &GLOB(router_power));
}


void router_power_stat()
{
  SIM_router_stat_energy(&GLOB(router_info), &GLOB(router_power), print_depth, name, max_flag, load, plot_flag, PARM(Freq));
}

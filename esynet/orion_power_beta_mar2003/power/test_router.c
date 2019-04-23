#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "SIM_power.h"
#include "SIM_power_router.h"
#include "SIM_router_power.h"


char path[2048];
extern char *optarg;
extern int optind;


int main(int argc, char **argv)
{
  int max_flag = AVG_ENERGY, plot_flag = 0;
  u_int print_depth = 0;
  double load = 1;
  char *name, opt;
  
  /* parse options */
  while ((opt = getopt(argc, argv, "+pmd:l:")) != -1) {
    switch (opt) {
      case 'p': plot_flag = 1; break;
      case 'm': max_flag = MAX_ENERGY; break;
      case 'd': print_depth = atoi(optarg); break;
      case 'l': load = atof(optarg); break;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "test_router: [-pm] [-d print_depth] [-l load] <router_name>\n");
    exit(1);
  }
  else {
    name = argv[optind];
  }

  FUNC(SIM_router_power_init, &GLOB(router_info), &GLOB(router_power));

  SIM_router_stat_energy(&GLOB(router_info), &GLOB(router_power), print_depth, name, max_flag, load, plot_flag, PARM(Freq));
  //load = SIM_reg_stat_energy(&GLOB(router_info).in_buf_info, &GLOB(router_power).in_buf, 0, 1, 0, NULL, 0);
  //printf("%g\n", load);

  exit(0);
}

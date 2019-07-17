#ifndef _SIM_ROUTER_POWER_H
#define _SIM_ROUTER_POWER_H

#include "SIM_port.h"

#if (PARM(POWER_STATS))

#include <library_LIB/LIB_defines.h>
#include <SIM_power_router.h>


/* global variables */
extern GLOBDEF(SIM_power_router_t, router_power);
extern GLOBDEF(SIM_power_router_info_t, router_info);


/* function prototypes */
extern int FUNC(SIM_router_power_init, SIM_power_router_info_t *info, SIM_power_router_t *router);
extern int FUNC(SIM_buf_power_data_read, SIM_power_array_info_t *info, SIM_power_array_t *arr, LIB_Type_max_uint data);
extern int FUNC(SIM_buf_power_data_write, SIM_power_array_info_t *info, SIM_power_array_t *arr, char *data_line, char *old_data, char *new_data);

#endif	/* PARM(POWER_STATS) */

#endif	/* _SIM_ROUTER_POWER_H */

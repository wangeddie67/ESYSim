#ifndef _SIM_REG_POWER_H
#define _SIM_REG_POWER_H

#include "SIM_port.h"

#if ( PARM( POWER_STATS ))

#include <library_LIB/LIB_defines.h>
#include <SIM_power_array.h>

#define P_DATA_T	int


/*@
 * data type: register file port state
 * 
 *    row_addr       -- row address
 *    col_addr       -- column address
 *    tag_addr       -- unused placeholder
 *    tag_line       -- unused placeholder
 *    data_line_size -- register size in char
 *    data_line      -- value of data bitline
 *
 * NOTE:
 *   (1) data_line is only used by write port
 */
typedef struct {
	LIB_Type_max_uint row_addr;
	LIB_Type_max_uint col_addr;
	LIB_Type_max_uint tag_addr;
	LIB_Type_max_uint tag_line;
	u_int data_line_size;
	P_DATA_T data_line;
} SIM_reg_port_state_t;


/* global variables */
extern GLOBDEF( SIM_power_array_t, reg_power );
extern GLOBDEF( SIM_power_array_info_t, reg_info );
extern GLOBDEF( SIM_reg_port_state_t, reg_read_port )[PARM(read_port)];
extern GLOBDEF( SIM_reg_port_state_t, reg_write_port )[PARM(write_port)];


/* function prototypes */
extern int FUNC(SIM_reg_power_init, SIM_power_array_info_t *info, SIM_power_array_t *arr, SIM_reg_port_state_t *read_port, SIM_reg_port_state_t *write_port);
extern int FUNC(SIM_reg_power_report, SIM_power_array_info_t *info, SIM_power_array_t *arr);
extern int FUNC(SIM_reg_avg_power, SIM_power_array_info_t *info, SIM_power_array_t *arr, int rw);
/* wrapper functions */
extern int FUNC( SIM_reg_power_dec, SIM_power_array_info_t *info, SIM_power_array_t *arr, u_int port, LIB_Type_max_uint row_addr, int rw );
extern int FUNC( SIM_reg_power_data_read, SIM_power_array_info_t *info, SIM_power_array_t *arr, P_DATA_T data );
extern int FUNC( SIM_reg_power_data_write, SIM_power_array_info_t *info, SIM_power_array_t *arr, u_int port, LIB_Type_max_uint old_data, LIB_Type_max_uint new_data );
extern int FUNC( SIM_reg_power_output, SIM_power_array_info_t *info, SIM_power_array_t *arr, P_DATA_T data );

#endif	/* PARM( POWER_STATS ) */

#endif	/* _SIM_REG_POWER_H */

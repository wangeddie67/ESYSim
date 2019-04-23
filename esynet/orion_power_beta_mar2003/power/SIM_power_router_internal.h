#ifndef _SIM_POWER_ROUTER_INTERNAL_H
#define _SIM_POWER_ROUTER_INTERNAL_H

#include <library_LIB/LIB_defines.h>
#include "SIM_power_misc.h"
#include "SIM_power_array.h"

typedef struct {
	int model;
	u_int n_in;
	u_int n_out;
	u_int data_width;
	u_int degree;	/* only used by multree crossbar */
	int connect_type;
	int trans_type;	/* only used by transmission gate connection */
	LIB_Type_max_uint n_chg_in;
	LIB_Type_max_uint n_chg_int;
	LIB_Type_max_uint n_chg_out;
	LIB_Type_max_uint n_chg_ctr;
	double e_chg_in;
	double e_chg_int;
	double e_chg_out;
	double e_chg_ctr;
	/* redundant field */
	u_int depth;	/* only used by multree crossbar */
	LIB_Type_max_uint mask;
} SIM_power_crossbar_t;

/* carry_in is the internal carry transition node */
typedef struct {
	int model;
	u_int req_width;
	LIB_Type_max_uint n_chg_req;
	LIB_Type_max_uint n_chg_grant;
	/* internal node of round robin arbiter */
	LIB_Type_max_uint n_chg_carry;
	LIB_Type_max_uint n_chg_carry_in;
	/* internal node of matrix arbiter */
	LIB_Type_max_uint n_chg_mint;
	double e_chg_req;
	double e_chg_grant;
	double e_chg_carry;
	double e_chg_carry_in;
	double e_chg_mint;
	/* priority flip flop */
	SIM_power_ff_t pri_ff;
	/* request queue */
	SIM_power_array_t queue;
	/* redundant field */
	LIB_Type_max_uint mask;
} SIM_power_arbiter_t;

/* structural-level report functions */
extern int SIM_crossbar_init(SIM_power_crossbar_t *crsbar, int model, u_int n_in, u_int n_out, u_int data_width, u_int degree, int connect_type, int trans_type, double in_len, double out_len, double *req_len);
extern double SIM_crossbar_report(unsigned long long sim_cycle, SIM_power_crossbar_t *crsbar, int telem);
extern int SIM_arbiter_init(SIM_power_arbiter_t *arb, int arbiter_model, int ff_model, u_int req_width, double length, SIM_power_array_info_t *info);
extern double SIM_arbiter_report(unsigned long long sim_cycle, SIM_power_arbiter_t *arb, int telem);

#endif	/* _SIM_POWER_ROUTER_INTERNAL_H */

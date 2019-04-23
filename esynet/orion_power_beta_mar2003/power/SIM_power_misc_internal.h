#ifndef _SIM_POWER_MISC_INTERNAL_H
#define _SIM_POWER_MISC_INTERNAL_H

#include <sys/types.h>
#include <library_LIB/LIB_defines.h>

/* maximum level of selection arbiter tree */
#define MAX_SEL_LEVEL	5


/*
 * FIXME: cannot handle bus wider than 64-bit
 */
typedef struct {
	int model;
	int encoding;
	u_int data_width;
	u_int grp_width;
	LIB_Type_max_uint n_switch;
	double e_switch;
	/* redundant field */
	u_int bit_width;
	LIB_Type_max_uint bus_mask;
} SIM_power_bus_t;

typedef struct {
	int model;
	u_int width;
	LIB_Type_max_uint n_anyreq;
	LIB_Type_max_uint n_chgreq;
	LIB_Type_max_uint n_grant;
	LIB_Type_max_uint n_enc[MAX_SEL_LEVEL];
	double e_anyreq;
	double e_chgreq;
	double e_grant;
	double e_enc[MAX_SEL_LEVEL];
} SIM_power_sel_t;

typedef struct {
	int model;
	LIB_Type_max_uint n_switch;
	LIB_Type_max_uint n_keep_1;
	LIB_Type_max_uint n_keep_0;
	LIB_Type_max_uint n_clock;
	double e_switch;
	double e_keep_1;
	double e_keep_0;
	double e_clock;
} SIM_power_ff_t;


/* flip-flop model interface */
extern int SIM_fpfp_init(SIM_power_ff_t *ff, int model, double load);
extern int SIM_fpfp_clear_stat(SIM_power_ff_t *ff);
extern double SIM_fpfp_report(SIM_power_ff_t *ff);

/* instruction window interface */
extern int SIM_iwin_sel_record(SIM_power_sel_t *sel, u_int req, int en, int last_level);
extern double SIM_iwin_sel_report(SIM_power_sel_t *sel);

#endif	/* _SIM_POWER_MISC_INTERNAL_H */

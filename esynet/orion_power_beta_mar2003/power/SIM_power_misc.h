#ifndef _SIM_POWER_MISC_H
#define _SIM_POWER_MISC_H

#include <sys/types.h>
#include <library_LIB/LIB_defines.h>
#include "SIM_power_misc_internal.h"


typedef enum {
	RESULT_BUS = 1,
	GENERIC_BUS,
	BUS_MAX_MODEL
} SIM_power_bus_model_t;

typedef enum {
	GENERIC_SEL = 1,
	SEL_MAX_MODEL
} SIM_power_sel_model_t;

typedef enum {
	NEG_DFF = 1,	/* negative egde-triggered D flip-flop */
	FF_MAX_MODEL
} SIM_power_ff_model_t;

typedef enum {
	IDENT_ENC = 1,	/* identity encoding */
	TRANS_ENC,	/* transition encoding */
	BUSINV_ENC,	/* bus inversion encoding */
	BUS_MAX_ENC
} SIM_power_bus_enc_t;


/* bus model interface */
extern int SIM_bus_init(SIM_power_bus_t *bus, int model, int encoding, u_int width, u_int grp_width, u_int n_snd, u_int n_rcv, double length, double time);
extern int SIM_bus_record(SIM_power_bus_t *bus, LIB_Type_max_uint old_state, LIB_Type_max_uint new_state);
extern double SIM_bus_report(unsigned long long sim_cycle, SIM_power_bus_t *bus, int telem);
extern LIB_Type_max_uint SIM_bus_state(SIM_power_bus_t *bus, LIB_Type_max_uint old_data, LIB_Type_max_uint old_state, LIB_Type_max_uint new_data);

#endif	/* _SIM_POWER_MISC_H */

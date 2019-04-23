#ifndef _SIM_POWER_PERMU_H
#define _SIM_POWER_PERMU_H

#include <library_LIB/LIB_defines.h>
#include "SIM_power.h"


typedef enum {
	OMFLIP_PERMU = 1,
	GRP_PERMU,
	PERMU_MAX_MODEL
} SIM_power_permu_model_t;

#define FIRST_STG	0x01
#define SECOND_STG	0x02
#define THIRD_STG	0x04
#define FOURTH_STG	0x08
#define ALL_PASS	0x0F
#define is_omega(x)	(!(x))
#define is_flip(x)	(x)

typedef struct {
	u_int data_width;
	LIB_Type_max_uint n_chg_in;
	LIB_Type_max_uint n_chg_out;
	LIB_Type_max_uint n_chg_stg;
	LIB_Type_max_uint n_chg_int;
	LIB_Type_max_uint n_chg_pass;
	LIB_Type_max_uint n_chg_ctr;
	double e_chg_in;
	double e_chg_out;
	double e_chg_stg;
	double e_chg_int;
	double e_chg_pass;
	double e_chg_ctr;
	/* state variable */
	LIB_Type_max_uint in;
	LIB_Type_max_uint out;
	LIB_Type_max_uint stg[3];
	LIB_Type_max_uint inn[4];
    u_int pass;
	LIB_Type_max_uint ctr[4];
	/* redundant field */
	LIB_Type_max_uint data_mask;
	LIB_Type_max_uint ctr_mask;
} SIM_power_omflip_t;

#define ZBIT	1
#define WBIT	0
#define N_ONEHOT	(32 + 16 + 8 + 4 + 2 + 1)

/* WHS: maximum bit width: 64 */
typedef struct {
	u_int data_width;
	LIB_Type_max_uint n_chg_in;
	LIB_Type_max_uint n_chg_zin[6];	/* z/w input node */
	LIB_Type_max_uint n_chg_lin[6];	/* left one-hot node */
	LIB_Type_max_uint n_chg_rin[6];	/* right one-hot node */
	LIB_Type_max_uint n_chg_oin;	/* OR gate input node */
	LIB_Type_max_uint n_chg_out;
	double e_chg_in;
	double e_chg_zin[6];
	double e_chg_lin[6];
	double e_chg_rin[6];
	double e_chg_oin;
	double e_chg_out;
	/* state variable */
	LIB_Type_max_uint in;
	LIB_Type_max_uint zin[6];
	LIB_Type_max_uint win[6];
	/* one-hot numbers */
	u_int lin[N_ONEHOT];
	u_int rin[N_ONEHOT];
	LIB_Type_max_uint zoin;
	LIB_Type_max_uint woin;
	LIB_Type_max_uint out;
	/* redundant field */
	u_int n_stg;
	LIB_Type_max_uint mask;
} SIM_power_grp_t;

typedef struct {
	int model;
	union {
		SIM_power_omflip_t omflip;
		SIM_power_grp_t grp;
	      } u;
} SIM_power_permu_t;


extern int SIM_permu_init(SIM_power_permu_t *permu, int model, u_int data_width);
extern int SIM_permu_record(SIM_power_permu_t *permu, LIB_Type_max_uint in, u_int mode, LIB_Type_max_uint op, int reset);
extern LIB_Type_max_uint SIM_permu_record_test(SIM_power_permu_t *permu, LIB_Type_max_uint in, u_int mode, LIB_Type_max_uint op);
extern double SIM_permu_report(SIM_power_permu_t *permu);
extern double SIM_permu_max_energy(SIM_power_permu_t *permu);

#endif	/* _SIM_POWER_PERMU_H */

#ifndef _SIM_POWER_ALU_H
#define _SIM_POWER_ALU_H

#include <library_LIB/LIB_defines.h>
#include "SIM_power.h"

/* PLX ALU transistor size */
#define WaluNOTp	(2 * Lamda)
#define WaluNOTn	(1 * Lamda)
#define Walu2NANDp	(2 * Lamda)
#define Walu2NANDn	(2 * Lamda)
#define Walu3NANDp	(3 * Lamda)
#define Walu3NANDn	(2 * Lamda)
#define Walu4NANDp	(4 * Lamda)
#define Walu4NANDn	(2 * Lamda)
#define Walu2NORp	(4 * Lamda)
#define Walu2NORn	(1 * Lamda)
#define Walu3NORp	(6 * Lamda)
#define Walu3NORn	(1 * Lamda)
#define Walu4NORp	(8 * Lamda)
#define Walu4NORn	(1 * Lamda)
#define WaluXORp	(8 * Lamda)
#define WaluXORn	(2 * Lamda)
#define WaluPASSp	(3 * Lamda)
#define WaluPASSn	(3 * Lamda)

typedef enum {
	PLX_ALU = 1,
	ALU_MAX_MODEL
} SIM_power_ALU_model_t;

/* PLX block type */
enum { PLX_CTR, PLX_IMM, PLX_LOGIC, PLX_SIGN, PLX_SHIFT, PLX_2COM, PLX_ADD, PLX_SUBW,
       PLX_SAT, PLX_AVG, PLX_OUT, PLX_BLK };

typedef struct {
	int model;
	u_int data_width;
	LIB_Type_max_uint n_chg_blk[PLX_BLK];
	double e_blk[PLX_BLK];
	/* state */
	LIB_Type_max_uint a_d1;
	LIB_Type_max_uint a_d2;
	LIB_Type_max_uint l_d1;
	LIB_Type_max_uint l_d2;
	u_int type;
} SIM_power_ALU_t;

extern int SIM_power_ALU_init(SIM_power_ALU_t *alu, int model, u_int data_width);
extern int SIM_power_ALU_record(SIM_power_ALU_t *alu, LIB_Type_max_uint d1, LIB_Type_max_uint d2, u_int type);
extern double SIM_power_ALU_report(SIM_power_ALU_t *alu);
extern double SIM_ALU_stat_energy(SIM_power_ALU_t *alu, int max);

#endif	/* _SIM_POWER_ALU_H */

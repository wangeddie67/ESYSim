#ifndef _SIM_POWER_ARRAY_INTERNAL_H
#define _SIM_POWER_ARRAY_INTERNAL_H

#include <sys/types.h>
#include "LIB_defines.h"

#define SIM_ARRAY_READ		0
#define SIM_ARRAY_WRITE		1

#define SIM_ARRAY_RECOVER	1

/* read/write */
#define SIM_ARRAY_RW		0
/* only write */
#define SIM_ARRAY_WO		1


/* Used to pass values around the program */
typedef struct
{
    int cache_size;
    int number_of_sets;
    int associativity;
    int block_size;
} time_parameter_type;

typedef struct
{
    double access_time,cycle_time;
    int best_Ndwl,best_Ndbl;
    int best_Nspd;
    int best_Ntwl,best_Ntbl;
    int best_Ntspd;
    double decoder_delay_data,decoder_delay_tag;
    double dec_data_driver,dec_data_3to8,dec_data_inv;
    double dec_tag_driver,dec_tag_3to8,dec_tag_inv;
    double wordline_delay_data,wordline_delay_tag;
    double bitline_delay_data,bitline_delay_tag;
    double sense_amp_delay_data,sense_amp_delay_tag;
    double senseext_driver_delay_data;
    double compare_part_delay;
    double drive_mux_delay;
    double selb_delay;
    double data_output_delay;
    double drive_valid_delay;
    double precharge_delay;
} time_result_type;


typedef struct
{
    int model;
    u_int n_bits;
    LIB_Type_max_uint n_chg_output;
    LIB_Type_max_uint n_chg_addr;
    LIB_Type_max_uint n_chg_l1;
    double e_chg_output;
    double e_chg_addr;
    double e_chg_l1;
    u_int n_in_1st;
    u_int n_in_2nd;
    u_int n_out_0th;
    u_int n_out_1st;
    /* redundant field */
    LIB_Type_max_uint addr_mask;
} SIM_power_dec_t;

typedef struct
{
    int model;
    int share_rw;
    LIB_Type_max_uint n_read;
    LIB_Type_max_uint n_write;
    double e_read;
    double e_write;
} SIM_power_wordline_t;

typedef struct
{
    int model;
    int share_rw;
    u_int end;
    LIB_Type_max_uint n_col_write;
    LIB_Type_max_uint n_col_read;
    LIB_Type_max_uint n_col_sel;
    double e_col_write;
    double e_col_read;
    double e_col_sel;
} SIM_power_bitline_t;

typedef struct
{
    int model;
    LIB_Type_max_uint n_access;
    double e_access;
} SIM_power_amp_t;

typedef struct
{
    int model;
    u_int n_bits;
    u_int assoc;
    LIB_Type_max_uint n_access;
    LIB_Type_max_uint n_match;
    LIB_Type_max_uint n_mismatch;
    LIB_Type_max_uint n_miss;
    LIB_Type_max_uint n_bit_match;
    LIB_Type_max_uint n_bit_mismatch;
    LIB_Type_max_uint n_chg_addr;
    double e_access;
    double e_match;
    double e_mismatch;
    double e_miss;
    double e_bit_match;
    double e_bit_mismatch;
    double e_chg_addr;
    /* redundant field */
    LIB_Type_max_uint comp_mask;
} SIM_power_comp_t;

typedef struct
{
    int model;
    u_int end;
    LIB_Type_max_uint n_switch;
    double e_switch;
} SIM_power_mem_t;

typedef struct
{
    int model;
    u_int assoc;
    LIB_Type_max_uint n_mismatch;
    LIB_Type_max_uint n_chg_addr;
    double e_mismatch;
    double e_chg_addr;
} SIM_power_mux_t;

typedef struct
{
    int model;
    u_int item_width;
    LIB_Type_max_uint n_select;
    LIB_Type_max_uint n_chg_data;
    LIB_Type_max_uint n_out_1;
    LIB_Type_max_uint n_out_0;
    double e_select;
    double e_chg_data;
    double e_out_1;
    double e_out_0;
    /* redundant field */
    LIB_Type_max_uint out_mask;
} SIM_power_out_t;

typedef struct
{
    int model;
    LIB_Type_max_uint n_charge;
    double e_charge;
} SIM_power_arr_pre_t;


/* ==================== function prototypes ==================== */
/* low-level function from cacti */
extern void SIM_power_calculate_time(time_result_type*, time_parameter_type*);

/* structural-level record functions */
extern int SIM_array_dec_record( SIM_power_dec_t *dec, LIB_Type_max_uint prev_addr, LIB_Type_max_uint curr_addr );
extern int SIM_array_wordline_record( SIM_power_wordline_t *wordline, int rw, LIB_Type_max_uint n_switch );
extern int SIM_array_bitline_record( SIM_power_bitline_t *bitline, int rw, u_int cols, LIB_Type_max_uint old_value, LIB_Type_max_uint new_value );
extern int SIM_array_amp_record( SIM_power_amp_t *amp, u_int cols );
extern int SIM_array_comp_global_record( SIM_power_comp_t *comp, LIB_Type_max_uint prev_value, LIB_Type_max_uint curr_value, int miss );
extern int SIM_array_comp_local_record( SIM_power_comp_t *comp, LIB_Type_max_uint prev_tag, LIB_Type_max_uint curr_tag, LIB_Type_max_uint input, int recover );
extern int SIM_array_mux_record( SIM_power_mux_t *mux, LIB_Type_max_uint prev_addr, LIB_Type_max_uint curr_addr, int miss );
extern int SIM_array_outdrv_global_record( SIM_power_out_t *outdrv, LIB_Type_max_uint data );
extern int SIM_array_outdrv_local_record( SIM_power_out_t *outdrv, LIB_Type_max_uint prev_data, LIB_Type_max_uint curr_data, int recover );
extern int SIM_array_mem_record( SIM_power_mem_t *mem, LIB_Type_max_uint prev_value, LIB_Type_max_uint curr_value, u_int cols );
extern int SIM_array_pre_record( SIM_power_arr_pre_t *pre, LIB_Type_max_uint n_charge );

/* structural-level report functions */
extern double SIM_array_dec_report(SIM_power_dec_t *dec, int telem);
extern double SIM_array_wordline_report(SIM_power_wordline_t *wordline, int telem);
extern double SIM_array_bitline_report(SIM_power_bitline_t *bitline, int telem);
extern double SIM_array_amp_report(SIM_power_amp_t *amp, int telem);
extern double SIM_array_comp_report(SIM_power_comp_t *comp, int telem);
extern double SIM_array_mux_report(SIM_power_mux_t *mux, int telem);
extern double SIM_array_outdrv_report(SIM_power_out_t *outdrv, int telem);
extern double SIM_array_mem_report(SIM_power_mem_t *mem, int telem);
extern double SIM_array_pre_report(SIM_power_arr_pre_t *pre, int telem);

extern double SIM_Array_Dec_Report(SIM_power_dec_t *dec );
extern double SIM_Array_Wordline_Report(SIM_power_wordline_t *wordline );
extern double SIM_Array_Bitline_Report(SIM_power_bitline_t *bitline );
extern double SIM_Array_Amp_Report(SIM_power_amp_t *amp );
extern double SIM_Array_Comp_Report(SIM_power_comp_t *comp );
extern double SIM_Array_Mux_Report(SIM_power_mux_t *mux );
extern double SIM_Array_Outdrv_Report(SIM_power_out_t *outdrv );
extern double SIM_Array_Mem_Report(SIM_power_mem_t *mem );
extern double SIM_Array_Pre_Report(SIM_power_arr_pre_t *pre );

extern double SIM_cam_wordline_cap( u_int cols, double wire_cap, double tx_width );
extern double SIM_cam_comp_tagline_cap( u_int rows, double taglinelength );
extern double SIM_cam_comp_mismatch_cap( u_int n_bits, u_int n_pre, double matchline_len );
extern double SIM_cam_comp_miss_cap( u_int assoc );
extern double SIM_cam_tag_mem_cap( u_int read_ports, u_int write_ports, int share_rw, u_int end, int only_write );
extern double SIM_cam_data_mem_cap( u_int read_ports, u_int write_ports );

#endif	/* _SIM_POWER_ARRAY_INTERNAL_H */

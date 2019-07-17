#ifndef _SIM_POWER_ARRAY_H
#define _SIM_POWER_ARRAY_H

#include <sys/types.h>
#include <library_LIB/LIB_defines.h>
#include "SIM_power_array_internal.h"


/* WHS: all model types begin with 1 because 0 is reserved for SIM_ARRAY_NO_MODEL */
#define SIM_ARRAY_NO_MODEL	0

/*@
 * data type: decoder model types
 *
 *   GENERIC_DEC   -- default type
 *   DEC_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	GENERIC_DEC = 1,
	DEC_MAX_MODEL
} SIM_power_dec_model_t;

/*@
 * data type: multiplexor model types
 *
 *   GENERIC_MUX   -- default type
 *   MUX_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	GENERIC_MUX = 1,
	MUX_MAX_MODEL
} SIM_power_mux_model_t;

/*@
 * data type: sense amplifier model types
 *
 *   GENERIC_AMP   -- default type
 *   AMP_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	GENERIC_AMP = 1,
	AMP_MAX_MODEL
} SIM_power_amp_model_t;

/*@
 * data type: wordline model types
 *
 *   CACHE_RW_WORDLINE  -- default type
 *   CACHE_WO_WORDLINE  -- write data wordline only, for fully-associative data bank
 *   CAM_RW_WORDLINE    -- both R/W tag wordlines, for fully-associative write-back
 *                         tag bank
 *   CAM_WO_WORDLINE    -- write tag wordline only, for fully-associative write-through
 *                         tag bank
 *   WORDLINE_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	CACHE_RW_WORDLINE = 1,
	CACHE_WO_WORDLINE,
	CAM_RW_WORDLINE,
	CAM_WO_WORDLINE,
	WORDLINE_MAX_MODEL
} SIM_power_wordline_model_t;

/*@
 * data type: bitline model types
 * 
 *   RW_BITLINE	       -- default type
 *   WO_BITLINE        -- write bitline only, for fully-associative data bank and
 *                        fully-associative write-through tag bank
 *   BITLINE_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	RW_BITLINE = 1,
	WO_BITLINE,
	BITLINE_MAX_MODEL
} SIM_power_bitline_model_t;

/*@
 * data type: precharge model types
 *
 *   PRE_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	SINGLE_BITLINE = 1,
	EQU_BITLINE,
	SINGLE_OTHER,
	PRE_MAX_MODEL
} SIM_power_pre_model_t;

/*@
 * data type: memory cell model types
 *
 *   NORMAL_MEM     -- default type
 *   CAM_TAG_RW_MEM -- read/write memory cell connected with tag comparator, for
 *                     fully-associative write-back tag bank
 *   CAM_TAG_WO_MEM -- write-only memory cell connected with tag comparator, for
 *                     fully-associative write-through tag bank
 *   CAM_DATA_MEM   -- memory cell connected with output driver, for fully-associative
 *                     data bank
 *   CAM_ATTACH_MEM -- memory cell of fully-associative array valid bit, use bit, etc.
 *   MEM_MAX_MODEL  -- upper bound of model type
 */
typedef enum {
	NORMAL_MEM = 1,
	CAM_TAG_RW_MEM,
	CAM_TAG_WO_MEM,
	CAM_DATA_MEM,
	CAM_ATTACH_MEM,
	MEM_MAX_MODEL
} SIM_power_mem_model_t;

/*@
 * data type: tag comparator model types
 *
 *   CACHE_COMP     -- default type
 *   CAM_COMP       -- cam-style tag comparator, for fully-associative array
 *   COMP_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	CACHE_COMP = 1,
	CAM_COMP,
	COMP_MAX_MODEL
} SIM_power_comp_model_t;

/*@
 * data type: output driver model types
 *
 *   CACHE_OUTDRV     -- default type
 *   CAM_OUTDRV       -- output driver connected with memory cell, for fully-associative
 *                       array
 *   REG_OUTDRV       -- output driver connected with bitline, for register files
 *   OUTDRV_MAX_MODEL -- upper bound of model type
 */
typedef enum {
	CACHE_OUTDRV = 1,
	CAM_OUTDRV,
	REG_OUTDRV,
	OUTDRV_MAX_MODEL
} SIM_power_outdrv_model_t;

typedef struct {
	SIM_power_dec_t row_dec;
	SIM_power_dec_t col_dec;
	SIM_power_wordline_t data_wordline;
	SIM_power_wordline_t tag_wordline;
	SIM_power_bitline_t data_bitline;
	SIM_power_bitline_t tag_bitline;
	SIM_power_mem_t data_mem;
	/* tag address memory cells */
	SIM_power_mem_t tag_mem;
	/* tag various bits memory cells, different with tag_mem for fully-associative cache */
	SIM_power_mem_t tag_attach_mem;
	SIM_power_amp_t data_amp;
	SIM_power_amp_t tag_amp;
	SIM_power_comp_t comp;
	SIM_power_mux_t mux;
	SIM_power_out_t outdrv;
	SIM_power_arr_pre_t row_dec_pre;
	SIM_power_arr_pre_t col_dec_pre;
	SIM_power_arr_pre_t data_bitline_pre;
	SIM_power_arr_pre_t tag_bitline_pre;
	SIM_power_arr_pre_t data_colsel_pre;
	SIM_power_arr_pre_t tag_colsel_pre;
	SIM_power_arr_pre_t comp_pre;
} SIM_power_array_t;

typedef struct {
	/* parameters shared by data array and tag array */
	int share_rw;
	u_int read_ports;
	u_int write_ports;
	u_int n_set;
	u_int blk_bits;
	u_int assoc;
	int row_dec_model;
	/* parameters specific to data array */
	u_int data_width;
	int col_dec_model;
	int mux_model;
	int outdrv_model;
	/* parameters specific to tag array */
	u_int tag_addr_width;
	u_int tag_line_width;
	int comp_model;
	/* data set of common parameters */
	u_int data_ndwl;
	u_int data_ndbl;
	u_int data_nspd;
	u_int data_n_share_amp;
	u_int data_end;
	int data_wordline_model;
	int data_bitline_model;
	int data_amp_model;
	int data_mem_model;
	/* tag set of common parameters */
	u_int tag_ndwl;
	u_int tag_ndbl;
	u_int tag_nspd;
	u_int tag_n_share_amp;
	u_int tag_end;
	int tag_wordline_model;
	int tag_bitline_model;
	int tag_amp_model;
	int tag_mem_model;
	int tag_attach_mem_model;
	/* parameters for precharging */
	int row_dec_pre_model;
	int col_dec_pre_model;
	int data_bitline_pre_model;
	int tag_bitline_pre_model;
	int data_colsel_pre_model;
	int tag_colsel_pre_model;
	int comp_pre_model;
	/* some redundant fields */
	u_int n_item;		/* # of items in a block */
	u_int eff_data_cols;	/* # of data columns driven by one wordline */
	u_int eff_tag_cols;	/* # of tag columns driven by one wordline */
	/* flags used by prototype array model */
	u_int use_bit_width;
	u_int valid_bit_width;
	int write_policy;	/* 1 if write back (have dirty bits), 0 otherwise */
	/* call back functions */
	/* these fields have no physical counterparts, they are
	 * here just because this is the most convenient place */
	u_int (*get_entry_valid_bit)( void* );
	u_int (*get_entry_dirty_bit)( void* );
	u_int (*get_set_use_bit)( void*, int );
	LIB_Type_max_uint (*get_entry_tag)( void* );
	LIB_Type_max_uint (*get_set_tag)( void*, int );
	/* fields which will be filled by initialization */
	double data_arr_width;
	double tag_arr_width;
	double data_arr_height;
	double tag_arr_height;
} SIM_power_array_info_t;

/*@
 * data type: array port state
 * 
 *  - row_addr       -- input to row decoder
 *    col_addr       -- input to column decoder, if any
 *    tag_addr       -- input to tag comparator
 * $+ tag_line       -- value of tag bitline
 *  # data_line_size -- size of data_line in char
 *  # data_line      -- value of data bitline
 *
 * legend:
 *   -: only used by non-fully-associative array
 *   +: only used by fully-associative array
 *   #: only used by fully-associative array or RF array
 *   $: only used by write-through array
 *
 * NOTE:
 *   (1) *_addr may not necessarily be an address
 *   (2) data_line_size is the allocated size of data_line in simulator,
 *       which must be no less than the physical size of data line
 *   (3) each instance of module should define an instance-specific data
 *       type with non-zero-length data_line and cast it to this type
 */
typedef struct {
	LIB_Type_max_uint row_addr;
	LIB_Type_max_uint col_addr;
	LIB_Type_max_uint tag_addr;
	LIB_Type_max_uint tag_line;
    u_int data_line_size;
#ifdef _WIN32
    char data_line[1024];
#else
    char data_line[0];
#endif
} SIM_array_port_state_t;

/*@
 * data type: array set state
 * 
 *   entry           -- pointer to some entry structure if an entry is selected for
 *                      r/w, NULL otherwise
 *   entry_set       -- pointer to corresponding set structure
 * + write_flag      -- 1 if entry is already written once, 0 otherwise
 * + write_back_flag -- 1 if entry is already written back, 0 otherwise
 *   valid_bak       -- valid bit of selected entry before operation
 *   dirty_bak       -- dirty bit of selected entry, if any, before operation
 *   tag_bak         -- tag of selected entry before operation
 *   use_bak         -- use bits of all entries before operation
 *
 * legend:
 *   +: only used by fully-associative array
 *
 * NOTE:
 *   (1) entry is interpreted by modules, if some module has no "entry structure",
 *       then make sure this field is non-zero if some entry is selected
 *   (2) tag_addr may not necessarily be an address
 *   (3) each instance of module should define an instance-specific data
 *       type with non-zero-length use_bit and cast it to this type
 */
typedef struct {
	void *entry;
	void *entry_set;
	int write_flag;
	int write_back_flag;
	u_int valid_bak;
	u_int dirty_bak;
	LIB_Type_max_uint tag_bak;
#ifdef _WIN32
    u_int use_bak[1024];
#else
	u_int use_bak[0];
#endif
} SIM_array_set_state_t;


/* ==================== function prototypes ==================== */
/* prototype-level record functions */
extern int SIM_power_array_dec( SIM_power_array_info_t *info, SIM_power_array_t *arr, SIM_array_port_state_t *port, LIB_Type_max_uint row_addr, int rw );
extern int SIM_power_array_data_read( SIM_power_array_info_t *info, SIM_power_array_t *arr, LIB_Type_max_uint data );
extern int SIM_power_array_data_write( SIM_power_array_info_t *info, SIM_power_array_t *arr, SIM_array_set_state_t *aset, u_int n_item, char *data_line, char *old_data, char *new_data );
extern int SIM_power_array_tag_read( SIM_power_array_info_t *info, SIM_power_array_t *arr, SIM_array_set_state_t *aset );
extern int SIM_power_array_tag_update( SIM_power_array_info_t *info, SIM_power_array_t *arr, SIM_array_port_state_t *port, SIM_array_set_state_t *aset );
extern int SIM_power_array_tag_compare( SIM_power_array_info_t *info, SIM_power_array_t *arr, SIM_array_port_state_t *port, LIB_Type_max_uint tag_input, LIB_Type_max_uint col_addr, SIM_array_set_state_t *aset );
extern int SIM_power_array_output( SIM_power_array_info_t *info, SIM_power_array_t *arr, u_int data_size, u_int length, void *data_out, void *data_all );

/* state manupilation functions */
extern int SIM_array_port_state_init( SIM_power_array_info_t *info, SIM_array_port_state_t *port );
extern int SIM_array_set_state_init( SIM_power_array_info_t *info, SIM_array_set_state_t *aset );

extern int SIM_array_power_init( SIM_power_array_info_t *info, SIM_power_array_t *arr );
extern double SIM_array_power_report(unsigned long long sim_cycle, SIM_power_array_info_t *info, SIM_power_array_t *arr, int telem );
extern int SIM_array_clear_stat(SIM_power_array_t *arr);

extern double SIM_reg_stat_energy(SIM_power_array_info_t *info, SIM_power_array_t *arr, double n_read, double n_write, int print_depth, char *path, int max_avg);

#endif	/* _SIM_POWER_ARRAY_H */

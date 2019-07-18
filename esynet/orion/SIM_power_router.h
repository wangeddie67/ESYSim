#ifndef _SIM_POWER_ROUTER_H
#define _SIM_POWER_ROUTER_H

#include "LIB_defines.h"
#include "SIM_power.h"
#include "SIM_power_router_internal.h"

typedef enum {
	MATRIX_CROSSBAR = 1,
	MULTREE_CROSSBAR,
	CROSSBAR_MAX_MODEL
} SIM_power_crossbar_model_t;

typedef enum {
	RR_ARBITER = 1,
	MATRIX_ARBITER,
	QUEUE_ARBITER,
	ARBITER_MAX_MODEL
} SIM_power_arbiter_model_t;

/* connection type */
typedef enum {
	TRANS_GATE,	/* transmission gate connection */
	TRISTATE_GATE,	/* tri-state gate connection */
} SIM_power_connect_t;

typedef struct {
	SIM_power_crossbar_t crossbar;
	SIM_power_array_t in_buf;
	SIM_power_array_t cache_in_buf;
	SIM_power_array_t mc_in_buf;
	SIM_power_array_t io_in_buf;
	SIM_power_array_t out_buf;
	SIM_power_array_t central_buf;
	SIM_power_crossbar_t in_cbuf_crsbar;
	SIM_power_crossbar_t out_cbuf_crsbar;
	SIM_power_arbiter_t in_arb;
	SIM_power_arbiter_t cache_in_arb;
	SIM_power_arbiter_t mc_in_arb;
	SIM_power_arbiter_t io_in_arb;
	SIM_power_arbiter_t out_arb;
	/* FIXME: this should be put in SIM_power_array_t */
	SIM_power_ff_t cbuf_ff;
} SIM_power_router_t;

typedef struct {
	u_int n_in;
	u_int n_cache_in;
	u_int n_mc_in;
	u_int n_io_in;
	u_int n_out;
	u_int n_cache_out;
	u_int n_mc_out;
	u_int n_io_out;
	u_int flit_width;
	/* virtual channel parameters */
	u_int n_v_channel;
	u_int n_v_class;
	u_int cache_class;
	u_int mc_class;
	u_int io_class;
	int in_share_buf;	/* whether input virtual classes share buffer */
	int out_share_buf;	/* whether output virtual classes share buffer */
	int in_share_switch;	/* whether input virtual classes share switch */
	int out_share_switch;	/* whether output virtual classes share switch */
	/* crossbar parameters */
	int crossbar_model;
	u_int degree;	/* only used by multree crossbar */
	int connect_type;
	int trans_type;	/* only used by transmission gate connection */
	double crossbar_in_len;
	double crossbar_out_len;
	/* buffer parameters */
	int in_buf;
	int cache_in_buf;
	int mc_in_buf;
	int io_in_buf;
	int out_buf;
	/* assume no buffering for local output ports */
	int central_buf;
	SIM_power_array_info_t in_buf_info;
	SIM_power_array_info_t cache_in_buf_info;
	SIM_power_array_info_t mc_in_buf_info;
	SIM_power_array_info_t io_in_buf_info;
	SIM_power_array_info_t out_buf_info;
	SIM_power_array_info_t central_buf_info;
	u_int pipe_depth;
	/* FIXME: this should be put in SIM_power_array_info_t */
	int cbuf_ff_model;
	/* arbiter parameters */
	int in_arb_model;
	int out_arb_model;
	int in_arb_ff_model;
	int out_arb_ff_model;
	SIM_power_array_info_t in_arb_queue_info;
	SIM_power_array_info_t cache_in_arb_queue_info;
	SIM_power_array_info_t mc_in_arb_queue_info;
	SIM_power_array_info_t io_in_arb_queue_info;
	SIM_power_array_info_t out_arb_queue_info;
	/* redundant fields */
	u_int n_total_in;
	u_int n_total_out;
	u_int in_n_switch;
	u_int cache_n_switch;
	u_int mc_n_switch;
	u_int io_n_switch;
	u_int n_switch_in;
	u_int n_switch_out;
} SIM_power_router_info_t;


extern int SIM_router_power_init(SIM_power_router_info_t *info, SIM_power_router_t *router);
extern int SIM_router_power_report(unsigned long long sim_cycle, SIM_power_router_info_t *info, SIM_power_router_t *router);
extern double SIM_router_stat_energy(SIM_power_router_info_t *info, SIM_power_router_t *router, int print_depth, char *path, int max_avg, double e_fin, int plot_flag, double freq);

extern int SIM_crossbar_record(SIM_power_crossbar_t *xb, int io, LIB_Type_max_uint new_data, LIB_Type_max_uint old_data, u_int new_port, u_int old_port);
extern int SIM_arbiter_record(SIM_power_arbiter_t *arb, LIB_Type_max_uint new_req, LIB_Type_max_uint old_req, u_int new_grant, u_int old_grant);

#endif	/* _SIM_POWER_ROUTER_H */

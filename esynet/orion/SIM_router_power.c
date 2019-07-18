/*
 * NOTES: (1) to avoid parameter explosion, we assume same model types for all arrays
 *        (2) assume no array folding
 *        (3) assume n_v_class is the largest class #
 */

#include "SIM_port.h"

#if ( PARM( POWER_STATS ))

#include <stdio.h>
#include <math.h>
#include "LIB_defines.h"
#include <SIM_power_router.h>


/* global variables */
GLOBDEF( SIM_power_router_t, router_power );
GLOBDEF( SIM_power_router_info_t, router_info );


//static int SIM_buf_port_state_init(SIM_power_array_info_t *info, SIM_buf_port_state_t *port, u_int n_port);


static int SIM_buf_set_para(SIM_power_array_info_t *info, int share_buf, u_int n_read_port, u_int n_write_port, u_int n_entry, u_int line_width, int outdrv)
{
  /* ==================== set parameters ==================== */
  /* general parameters */
  info->share_rw = 0;
  info->read_ports = n_read_port;
  info->write_ports = n_write_port;
  info->n_set = n_entry;
  info->blk_bits = line_width;
  info->assoc = 1;
  info->data_width = line_width;
  info->data_end = PARM(data_end);

  /* no sub-array partition */
  info->data_ndwl = 1;
  info->data_ndbl = 1;
  info->data_nspd = 1;

  info->data_n_share_amp = 1;

  /* model parameters */
  if (share_buf) {
    info->row_dec_model = PARM(row_dec_model);
    info->row_dec_pre_model = PARM(row_dec_pre_model);
  }
  else {
    info->row_dec_model = SIM_NO_MODEL;
    info->row_dec_pre_model = SIM_NO_MODEL;
  }
  info->data_wordline_model = PARM( wordline_model );
  info->data_bitline_model = PARM( bitline_model );
  info->data_bitline_pre_model = PARM( bitline_pre_model );
  info->data_mem_model = PARM( mem_model );
#if (PARM(data_end) == 2)
  info->data_amp_model = PARM(amp_model);
#else
  info->data_amp_model = SIM_NO_MODEL;
#endif	/* PARM(data_end) == 2 */
  if (outdrv)
    info->outdrv_model = PARM(outdrv_model);
  else
    info->outdrv_model = SIM_NO_MODEL;

  info->data_colsel_pre_model = SIM_NO_MODEL;
  info->col_dec_model = SIM_NO_MODEL;
  info->col_dec_pre_model = SIM_NO_MODEL;
  info->mux_model = SIM_NO_MODEL;

  /* FIXME: not true for shared buffer */
  /* no tag array */
  info->tag_wordline_model = SIM_NO_MODEL;
  info->tag_bitline_model = SIM_NO_MODEL;
  info->tag_bitline_pre_model = SIM_NO_MODEL;
  info->tag_mem_model = SIM_NO_MODEL;
  info->tag_attach_mem_model = SIM_NO_MODEL;
  info->tag_amp_model = SIM_NO_MODEL;
  info->tag_colsel_pre_model = SIM_NO_MODEL;
  info->comp_model = SIM_NO_MODEL;
  info->comp_pre_model = SIM_NO_MODEL;

  
  /* ==================== set flags ==================== */
  info->write_policy = 0;	/* no dirty bit */

  
  /* ==================== compute redundant fields ==================== */
  info->n_item = info->blk_bits / info->data_width;
  info->eff_data_cols = info->blk_bits * info->assoc * info->data_nspd;

  
  /* ==================== call back functions ==================== */
  info->get_entry_valid_bit = NULL;
  info->get_entry_dirty_bit = NULL;
  info->get_entry_tag = NULL;
  info->get_set_tag = NULL;
  info->get_set_use_bit = NULL;


  /* initialize state variables */
  //if (read_port) SIM_buf_port_state_init(info, read_port, info->read_ports);
  //if (write_port) SIM_buf_port_state_init(info, write_port, info->write_ports);

  return 0;
}


int FUNC(SIM_router_power_init, SIM_power_router_info_t *info, SIM_power_router_t *router)
{
  u_int line_width;
  int share_buf, outdrv;
  
  /* PHASE 1: set parameters */
  /* general parameters */
  info->n_in = PARM(in_port);
  info->n_cache_in = PARM(cache_in_port);
  info->n_mc_in = PARM(mc_in_port);
  info->n_io_in = PARM(io_in_port);
  info->n_total_in = PARM(in_port) + PARM(cache_in_port) + PARM(mc_in_port) + PARM(io_in_port);
  info->n_out = PARM(out_port);
  info->n_cache_out = PARM(cache_out_port);
  info->n_mc_out = PARM(mc_out_port);
  info->n_io_out = PARM(io_out_port);
  info->n_total_out = PARM(out_port) + PARM(cache_out_port) + PARM(mc_out_port) + PARM(io_out_port);
  info->flit_width = PARM(flit_width);

  /* virtual channel parameters */
  info->n_v_channel = MAX(PARM(v_channel), 1);
  info->n_v_class = MAX(info->n_v_channel, PARM(v_class));
  info->cache_class = MAX(PARM(cache_class), 1);
  info->mc_class = MAX(PARM(mc_class), 1);
  info->io_class = MAX(PARM(io_class), 1);
  /* shared buffer implies buffer has tags, so no virtual class -> no sharing */
  /* separate buffer & shared switch implies buffer has tri-state output driver, so no v class -> no sharing */
  if (info->n_v_class > 1) {
    info->in_share_buf = PARM(in_share_buf);
    info->out_share_buf = PARM(out_share_buf);
    info->in_share_switch = PARM(in_share_switch);
    info->out_share_switch = PARM(out_share_switch);
  }
  else {
    info->in_share_buf = 0;
    info->out_share_buf = 0;
    info->in_share_switch = 0;
    info->out_share_switch = 0;
  }

  /* crossbar */
  info->crossbar_model = PARM(crossbar_model);
  info->degree = PARM(crsbar_degree);
  info->connect_type = PARM(connect_type);
  info->trans_type = PARM(trans_type);
  info->crossbar_in_len = PARM(crossbar_in_len);
  info->crossbar_out_len = PARM(crossbar_out_len);

  /* input buffer */
  info->in_buf = PARM(in_buf);
#if (PARM(in_buf))
  outdrv = !info->in_share_buf && info->in_share_switch;
  SIM_buf_set_para(&info->in_buf_info, info->in_share_buf, PARM(in_buf_rport), 1, PARM(in_buf_set), PARM(flit_width), outdrv);
#endif	/* PARM(in_buf) */

#if (PARM(cache_in_port))
  info->cache_in_buf = PARM(cache_in_buf);
#if (PARM(cache_in_buf))
#if (PARM(cache_class) > 1)
  share_buf = info->in_share_buf;
  outdrv = !share_buf && info->in_share_switch;
#else
  outdrv = share_buf = 0;
#endif	/* PARM(cache_class) > 1 */
  SIM_buf_set_para(&info->cache_in_buf_info, share_buf, PARM(cache_in_buf_rport), 1, PARM(cache_in_buf_set), PARM(flit_width), outdrv);
#endif	/* PARM(cache_in_buf) */
#else
  info->cache_in_buf = 0;
#endif	/* PARM(cache_in_port) */

#if (PARM(mc_in_port))
  info->mc_in_buf = PARM(mc_in_buf);
#if (PARM(mc_in_buf))
#if (PARM(mc_class) > 1)
  share_buf = info->in_share_buf;
  outdrv = !share_buf && info->in_share_switch;
#else
  outdrv = share_buf = 0;
#endif	/* PARM(mc_class) > 1 */
  SIM_buf_set_para(&info->mc_in_buf_info, share_buf, PARM(mc_in_buf_rport), 1, PARM(mc_in_buf_set), PARM(flit_width), outdrv);
#endif	/* PARM(mc_in_buf) */
#else
  info->mc_in_buf = 0;
#endif	/* PARM(mc_in_port) */

#if (PARM(io_in_port))
  info->io_in_buf = PARM(io_in_buf);
#if (PARM(io_in_buf))
#if (PARM(io_class) > 1)
  share_buf = info->in_share_buf;
  outdrv = !share_buf && info->in_share_switch;
#else
  outdrv = share_buf = 0;
#endif	/* PARM(io_class) > 1 */
  SIM_buf_set_para(&info->io_in_buf_info, share_buf, PARM(io_in_buf_rport), 1, PARM(io_in_buf_set), PARM(flit_width), outdrv);
#endif	/* PARM(io_in_buf) */
#else
  info->io_in_buf = 0;
#endif	/* PARM(io_in_port) */

  /* output buffer */
  info->out_buf = PARM(out_buf);
#if (PARM(out_buf))
  /* output buffer has no tri-state buffer anyway */
  SIM_buf_set_para(&info->out_buf_info, info->out_share_buf, 1, PARM(out_buf_wport), PARM(out_buf_set), PARM(flit_width), 0);
#endif	/* PARM(out_buf) */
  
  /* central buffer */
  info->central_buf = PARM(central_buf);
#if (PARM(central_buf))
  info->pipe_depth = PARM(pipe_depth);
  /* central buffer is always shared */
  SIM_buf_set_para(&info->central_buf_info, 1, PARM(cbuf_rport), PARM(cbuf_wport), PARM(cbuf_set), PARM(cbuf_width) * PARM(flit_width), 0);
  /* dirty hack */
  info->cbuf_ff_model = NEG_DFF;
#endif	/* PARM(central_buf) */

  /* input port arbiter */
  if (info->n_v_class > 1) {
    if (info->in_arb_model = PARM(in_arb_model)) {
      if (PARM(in_arb_model) == QUEUE_ARBITER) {
        SIM_buf_set_para(&info->in_arb_queue_info, 0, 1, 1, info->n_v_class, SIM_power_logtwo(info->n_v_class), 0);
	if (info->cache_class > 1)
          SIM_buf_set_para(&info->cache_in_arb_queue_info, 0, 1, 1, info->cache_class, SIM_power_logtwo(info->cache_class), 0);
	if (info->mc_class > 1)
          SIM_buf_set_para(&info->mc_in_arb_queue_info, 0, 1, 1, info->mc_class, SIM_power_logtwo(info->mc_class), 0);
	if (info->io_class > 1)
          SIM_buf_set_para(&info->io_in_arb_queue_info, 0, 1, 1, info->io_class, SIM_power_logtwo(info->io_class), 0);

        info->in_arb_ff_model = SIM_NO_MODEL;
      }
      else
        info->in_arb_ff_model = PARM(in_arb_ff_model);
    }
    else
      info->in_arb_ff_model = SIM_NO_MODEL;
  }
  else {
    info->in_arb_model = SIM_NO_MODEL;
    info->in_arb_ff_model = SIM_NO_MODEL;
  }

  /* output port arbiter */
#if ((PARM(in_port) + PARM(cache_in_port) + PARM(mc_in_port) + PARM(io_in_port)) > 2)
  if (info->out_arb_model = PARM(out_arb_model)) {
    if (PARM(out_arb_model) == QUEUE_ARBITER) {
      line_width = SIM_power_logtwo(info->n_total_in - 1);
      SIM_buf_set_para(&info->out_arb_queue_info, 0, 1, 1, info->n_total_in - 1, line_width, 0);
      info->out_arb_ff_model = SIM_NO_MODEL;
    }
    else
      info->out_arb_ff_model = PARM(out_arb_ff_model);
  }
  else
    info->out_arb_ff_model = SIM_NO_MODEL;
#else
  info->out_arb_model = SIM_NO_MODEL;
  info->out_arb_ff_model = SIM_NO_MODEL;
#endif	/* (PARM(in_port) + PARM(cache_in_port) + PARM(mc_in_port) + PARM(io_in_port)) > 2 */
  
  /* redundant fields */
  if (info->in_buf) {
    if (info->in_share_buf)
      info->in_n_switch = info->in_buf_info.read_ports;
    else if (info->in_share_switch)
      info->in_n_switch = 1;
    else
      info->in_n_switch = info->n_v_class;
  }
  else
    info->in_n_switch = 1;

  if (info->cache_in_buf) {
    if (info->in_share_buf)
      info->cache_n_switch = info->cache_in_buf_info.read_ports;
    else if (info->in_share_switch)
      info->cache_n_switch = 1;
    else
      info->cache_n_switch = info->cache_class;
  }
  else
    info->cache_n_switch = 1;

  if (info->mc_in_buf) {
    if (info->in_share_buf)
      info->mc_n_switch = info->mc_in_buf_info.read_ports;
    else if (info->in_share_switch)
      info->mc_n_switch = 1;
    else
      info->mc_n_switch = info->mc_class;
  }
  else
    info->mc_n_switch = 1;

  if (info->io_in_buf) {
    if (info->in_share_buf)
      info->io_n_switch = info->io_in_buf_info.read_ports;
    else if (info->in_share_switch)
      info->io_n_switch = 1;
    else
      info->io_n_switch = info->io_class;
  }
  else
    info->io_n_switch = 1;

  info->n_switch_in = info->n_in * info->in_n_switch + info->n_cache_in * info->cache_n_switch +
	              info->n_mc_in * info->mc_n_switch + info->n_io_in * info->io_n_switch;
  
  /* no buffering for local output ports */
  info->n_switch_out = info->n_cache_out + info->n_mc_out + info->n_io_out;
  if (info->out_buf) {
    if (info->out_share_buf)
      info->n_switch_out += info->n_out * info->out_buf_info.write_ports;
    else if (info->out_share_switch)
      info->n_switch_out += info->n_out;
    else
      info->n_switch_out += info->n_out * info->n_v_class;
  }
  else
    info->n_switch_out += info->n_out;

  /* PHASE 2: initialization */
  SIM_router_power_init(info, router);

  return 0;
}



/* ==================== buffer (wrapper functions) ==================== */

/* record read data activity */
int FUNC(SIM_buf_power_data_read, SIM_power_array_info_t *info, SIM_power_array_t *arr, LIB_Type_max_uint data)
{
  /* precharge */
  SIM_array_pre_record(&arr->data_bitline_pre, info->blk_bits);
  /* drive the wordline */
  SIM_power_array_dec(info, arr, NULL, 0, SIM_ARRAY_READ);
  /* read data */
  SIM_power_array_data_read(info, arr, data);

  return 0;
}


/* record write data bitline and memory cell activity */
int FUNC(SIM_buf_power_data_write, SIM_power_array_info_t *info, SIM_power_array_t *arr, char *data_line, char *old_data, char *new_data)
{
  /* drive the wordline */
  SIM_power_array_dec(info, arr, NULL, 0, SIM_ARRAY_WRITE);
  /* write data */
  SIM_power_array_data_write(info, arr, NULL, PARM(flit_width) / 8, data_line, old_data, new_data);

  return 0;
}

/* WHS: missing data output wrapper function */

/* ==================== buffer (wrapper functions) ==================== */

#endif	/* PARM(POWER_STATS) */

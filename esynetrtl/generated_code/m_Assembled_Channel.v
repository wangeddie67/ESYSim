`timescale 1ns / 1ps
`include "parameters.v"

module m_Assembled_Channel (
	 router_id,
	 CLK,
	 RST,

	  in_neighbor_credits_p0,

	  in_neighbor_credits_p1,

	  in_neighbor_credits_p2,

	  in_neighbor_credits_p3,

	  in_neighbor_credits_p4,

	//sa signals:
	 out_sa_enable,
	  out_sa_request,
	 in_sa_grant,

	//st signals:
	  out_st_request,
	  in_st_credits_feedback,
	 out_st_data_valid,

	in_data,
	out_data,

	  out_buffer_credits_sharing,
	 in_received_data_valid,

	out_rr_request,
	in_rr_result

);
	//end of io declaration


	// Parameters
	parameter P_ROUTER_ID = 0;
	parameter P_CHANNEL = 0;
	`ifdef FT_ECC
		parameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH;
	`else 
		parameter P_DATA_WIDTH = `FLIT_WIDTH;
	`endif
	
	
	input [`FLIT_SRC_WIDTH - 1 : 0] router_id;
	input CLK;
	input RST;

	input [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p0;

	input [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p1;

	input [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p2;

	input [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p3;

	input [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p4;

	//sa signals:
	output out_sa_enable;
	output [`CHANNELS - 1:0] out_sa_request;
	input in_sa_grant;

	//st signals:
	output [`CHANNELS - 1:0] out_st_request;
	input [`BUFFERSIZE_WIDTH - 1:0] in_st_credits_feedback;
	output out_st_data_valid;

	input [P_DATA_WIDTH - 1: 0] in_data;
	output [P_DATA_WIDTH - 1: 0] out_data;

	output [`BUFFERSIZE_WIDTH - 1:0] out_buffer_credits_sharing;
	input in_received_data_valid;

	output [`CHANNELS - 1:0] out_rr_request;
	input [`CHANNELS - 1:0] in_rr_result;

	//state machine
	//wire
	wire [`STATES_WIDTH - 1:0]  channel_current_state;

	//buffer
	//wire
	wire channel_command_write;
	wire channel_command_read;
	wire [`BUFFERSIZE_WIDTH - 1:0]  channel_buffer_credits_local;

	//RoutingCalculation 
	//wire 
	wire channel_rc_enable;
	wire [`FLIT_SRC_WIDTH - 1 : 0] local_id;
	wire channel_rc_success;
	wire [`CHANNELS - 1:0] channel_rc_result;


	// Assignments
	assign local_id = router_id;

	//module
	m_RoutingCalculation #(
	//.P_LOCAL_ID(P_ROUTER_ID),
	.P_LOCAL_CHANNEL(P_CHANNEL) 
	)
	routingcalc (
	.RST(RST),

	.enable(channel_rc_enable),
	.local_in(local_id),

	.src_in(out_data[`FLIT_SRC_WIDTH + `FLIT_DST_WIDTH + `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_DST_WIDTH + `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH]),
	.dst_in(out_data[`FLIT_DST_WIDTH + `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH]),

	.credits_0(in_neighbor_credits_p0),

	.credits_1(in_neighbor_credits_p1),

	.credits_2(in_neighbor_credits_p2),

	.credits_3(in_neighbor_credits_p3),

	.credits_4(in_neighbor_credits_p4),

	.success(channel_rc_success),
	.result(channel_rc_result)
);


	m_Buffer #(
	//.P_LOCAL_ID(P_ROUTER_ID),
	.P_LOCAL_CHANNEL(P_CHANNEL),
	.P_DATA_WIDTH(P_DATA_WIDTH)
	)
	buffer (
	.RST(RST),
	.CLK(CLK),
	.command_write(channel_command_write),
	.command_read(channel_command_read),
	.buffer_credit_level(channel_buffer_credits_local),
	.data_in(in_data),
	`ifdef FT_ECC
	.data_out(channel_intercoder_data_in)
	`else
	.data_out(out_data)
	`endif
	
);


	//module
	m_StateMachine	#(
	//.P_LOCAL_ID(P_ROUTER_ID),
	.P_LOCAL_CHANNEL(P_CHANNEL)
	)
	statemachine (
	.router_id(router_id),
	.RST(RST),
	.CLK(CLK),

	// Data Handling + Channel/BufferControl
	.data_valid(in_received_data_valid),
	.command_write(channel_command_write),
	.command_read(channel_command_read),
	.buffer_credits_local(channel_buffer_credits_local),
	.buffer_credits_sharing(out_buffer_credits_sharing),
	.data_in(out_data[`FLIT_WIDTH - 1 : 0]),

	// RouteCalculation signals
	.rc_enable(channel_rc_enable),
	.rc_success(channel_rc_success),
	.rc_result(channel_rc_result),

	.sa_enable(out_sa_enable),
	.sa_request(out_sa_request),
	.sa_grant(in_sa_grant),

	.st_request(out_st_request),
	.st_data_valid(out_st_data_valid),
	.st_credits_feedback(in_st_credits_feedback),

	.rr_request(out_rr_request),
	.rr_result(in_rr_result),

	// Assertion Unit error
		`ifdef FT_RC_TOLERANT
			`ifdef FT_SECURE_SM
			.error_assertion_packet(channel_error_PACKET),
			.error_assertion_rc(channel_error_RC),
			.error_assertion_sa(in_router_assertion_error_SA),
			`endif
		`else
			`ifdef FT_SECURE_SM
			.error_assertion_packet(1'b0,)
			.error_assertion_rc(1'b0),
			.error_assertion_sa(1'b0),
			`endif
		`endif
		
		`ifdef FT_ECC
		// ECC Error
		.error_ecc(channel_error_ECC),
		`else
		.error_ecc(1'b0),
		`endif

	.current_state(channel_current_state)
	
);


	m_LogModule #(
	//.P_LOCAL_ID(P_ROUTER_ID),
	.P_LOCAL_CHANNEL(P_CHANNEL) 
	) logger (
	.router_id(router_id),
	.RST(RST),
	.CLK(CLK),


	.buffer_data_in(in_data[`FLIT_WIDTH - 1 : 0]),
	.buffer_write(channel_command_write),
	.buffer_credits(channel_buffer_credits_local),
	.statemachine_data_in(out_data[`FLIT_WIDTH - 1 : 0]),
	.rc_success(channel_rc_success),
	.rc_result(channel_rc_result),
	.sa_grant(in_sa_grant),
	.st_data_valid(out_st_data_valid),
	.current_state(channel_current_state)
	
);
	endmodule

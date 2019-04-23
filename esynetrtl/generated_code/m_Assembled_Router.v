`timescale 1ns / 1ps
`include "parameters.v"

	module m_Assembled_Router(
	router_id,
	//define local port io, port number : 0
	p0_in_data_valid,
	p0_out_data_valid,
	p0_in_data,
	p0_out_data,
	p0_out_credits_feedback,
	p0_in_credits_feedback,

	//io declaration for port 1
	p1_in_data,
	p1_out_data,

	p1_in_data_valid,
	p1_out_data_valid,

	p1_in_credits_feedback,
	p1_out_credits_feedback,


	//io declaration for port 2
	p2_in_data,
	p2_out_data,

	p2_in_data_valid,
	p2_out_data_valid,

	p2_in_credits_feedback,
	p2_out_credits_feedback,


	//io declaration for port 3
	p3_in_data,
	p3_out_data,

	p3_in_data_valid,
	p3_out_data_valid,

	p3_in_credits_feedback,
	p3_out_credits_feedback,


	//io declaration for port 4
	p4_in_data,
	p4_out_data,

	p4_in_data_valid,
	p4_out_data_valid,

	p4_in_credits_feedback,
	p4_out_credits_feedback,


	RST,
	CLK

);

	//parameters
	//parameter P_ROUTER_ID = 0;
	`ifdef FT_ECC
			parameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH;
	`else 
		parameter P_DATA_WIDTH = `FLIT_WIDTH;
	`endif

	input [`FLIT_SRC_WIDTH-1 : 0] router_id;
	//define local port io; port number : 0
	input p0_in_data_valid;
	output p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] p0_in_data;
	output [P_DATA_WIDTH - 1: 0] p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1 : 0] p0_out_credits_feedback;

	//io declaration for port 1
	input [P_DATA_WIDTH - 1: 0] p1_in_data;
	output [P_DATA_WIDTH - 1: 0] p1_out_data;

	input  p1_in_data_valid;
	output  p1_out_data_valid;

	input [`BUFFERSIZE_WIDTH - 1 : 0] p1_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1 : 0] p1_out_credits_feedback;


	//io declaration for port 2
	input [P_DATA_WIDTH - 1: 0] p2_in_data;
	output [P_DATA_WIDTH - 1: 0] p2_out_data;

	input  p2_in_data_valid;
	output  p2_out_data_valid;

	input [`BUFFERSIZE_WIDTH - 1 : 0] p2_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1 : 0] p2_out_credits_feedback;


	//io declaration for port 3
	input [P_DATA_WIDTH - 1: 0] p3_in_data;
	output [P_DATA_WIDTH - 1: 0] p3_out_data;

	input  p3_in_data_valid;
	output  p3_out_data_valid;

	input [`BUFFERSIZE_WIDTH - 1 : 0] p3_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1 : 0] p3_out_credits_feedback;


	//io declaration for port 4
	input [P_DATA_WIDTH - 1: 0] p4_in_data;
	output [P_DATA_WIDTH - 1: 0] p4_out_data;

	input  p4_in_data_valid;
	output  p4_out_data_valid;

	input [`BUFFERSIZE_WIDTH - 1 : 0] p4_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1 : 0] p4_out_credits_feedback;


	input RST;
	input CLK;



	//D_FF_sync
	//wire
	//Mux for port 0
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p0_output_buffer;
	assign p0_out_data = p0_output_buffer[P_DATA_WIDTH - 1 : 0];
	assign p0_out_data_valid = p0_output_buffer[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1];

	//Mux for port 1
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p1_output_buffer;
	assign p1_out_data = p1_output_buffer[P_DATA_WIDTH - 1 : 0];
	assign p1_out_data_valid = p1_output_buffer[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1];

	//Mux for port 2
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p2_output_buffer;
	assign p2_out_data = p2_output_buffer[P_DATA_WIDTH - 1 : 0];
	assign p2_out_data_valid = p2_output_buffer[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1];

	//Mux for port 3
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p3_output_buffer;
	assign p3_out_data = p3_output_buffer[P_DATA_WIDTH - 1 : 0];
	assign p3_out_data_valid = p3_output_buffer[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1];

	//Mux for port 4
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p4_output_buffer;
	assign p4_out_data = p4_output_buffer[P_DATA_WIDTH - 1 : 0];
	assign p4_out_data_valid = p4_output_buffer[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1];

	//Round Robin
	//wire
	wire [`CHANNELS - 1 : 0] intern_rr_request [`CHANNELS - 1 : 0];
	wire [`CHANNELS - 1 : 0] intern_rr_result;

	//crossbar
	//wire
	//for port 0
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p0;
	wire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p0;

	//for port 1
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p1;
	wire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p1;

	//for port 2
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p2;
	wire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p2;

	//for port 3
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p3;
	wire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p3;

	//for port 4
	wire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p4;
	wire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p4;

	//switch allocation
	//wire
	wire grant_p0;
	wire grant_p1;
	wire grant_p2;
	wire grant_p3;
	wire grant_p4;
	//data out mux_2to1
	//wire
	//Mux for port 0
	//Mux for port 1
	//Mux for port 2
	//Mux for port 3
	//Mux for port 4
	//Module Declaration
	//channels

	//Channel p0
	//wire declaration;
	wire out_sa_enable_p0;
	wire [`CHANNELS - 1:0] out_sa_request_p0;
	wire [`CHANNELS - 1:0] out_st_request_p0;
	wire out_st_data_valid_p0;
	wire [P_DATA_WIDTH - 1: 0] out_data_p0;
	wire [`CHANNELS - 1:0] out_rr_request_p0;
	//module declaration
	m_Assembled_Channel
		#(
	//.P_ROUTER_ID(P_ROUTER_ID),
	.P_CHANNEL(0),
	.P_DATA_WIDTH(P_DATA_WIDTH)
	)
	channel_p0
	(
	.router_id(router_id),
	.RST(RST),
	.CLK(CLK),
	.in_neighbor_credits_p0(p0_in_credits_feedback),
	.in_neighbor_credits_p1(p1_in_credits_feedback),
	.in_neighbor_credits_p2(p2_in_credits_feedback),
	.in_neighbor_credits_p3(p3_in_credits_feedback),
	.in_neighbor_credits_p4(p4_in_credits_feedback),
	//sa signals:
	.out_sa_enable(out_sa_enable_p0),
	.out_sa_request(out_sa_request_p0),
	.in_sa_grant(grant_p0),

	//st signals:
	.out_st_request(out_st_request_p0),
	.in_st_credits_feedback(crossbar_feedback_out_p0),
	.out_st_data_valid(out_st_data_valid_p0),

	.in_data(p0_in_data),
	.out_data(out_data_p0),

	.out_buffer_credits_sharing(p0_out_credits_feedback),
	.in_received_data_valid(p0_in_data_valid),

	.out_rr_request(out_rr_request_p0),
	.in_rr_result(intern_rr_result)
);

	//Channel p1
	//wire declaration;
	wire out_sa_enable_p1;
	wire [`CHANNELS - 1:0] out_sa_request_p1;
	wire [`CHANNELS - 1:0] out_st_request_p1;
	wire out_st_data_valid_p1;
	wire [P_DATA_WIDTH - 1: 0] out_data_p1;
	wire [`CHANNELS - 1:0] out_rr_request_p1;
	//module declaration
	m_Assembled_Channel
		#(
	//.P_ROUTER_ID(P_ROUTER_ID),
	.P_CHANNEL(1),
	.P_DATA_WIDTH(P_DATA_WIDTH)
	)
	channel_p1
	(
	.router_id(router_id),
	.RST(RST),
	.CLK(CLK),
	.in_neighbor_credits_p0(p0_in_credits_feedback),
	.in_neighbor_credits_p1(p1_in_credits_feedback),
	.in_neighbor_credits_p2(p2_in_credits_feedback),
	.in_neighbor_credits_p3(p3_in_credits_feedback),
	.in_neighbor_credits_p4(p4_in_credits_feedback),
	//sa signals:
	.out_sa_enable(out_sa_enable_p1),
	.out_sa_request(out_sa_request_p1),
	.in_sa_grant(grant_p1),

	//st signals:
	.out_st_request(out_st_request_p1),
	.in_st_credits_feedback(crossbar_feedback_out_p1),
	.out_st_data_valid(out_st_data_valid_p1),

	.in_data(p1_in_data),
	.out_data(out_data_p1),

	.out_buffer_credits_sharing(p1_out_credits_feedback),
	.in_received_data_valid(p1_in_data_valid),

	.out_rr_request(out_rr_request_p1),
	.in_rr_result(intern_rr_result)
);

	//Channel p2
	//wire declaration;
	wire out_sa_enable_p2;
	wire [`CHANNELS - 1:0] out_sa_request_p2;
	wire [`CHANNELS - 1:0] out_st_request_p2;
	wire out_st_data_valid_p2;
	wire [P_DATA_WIDTH - 1: 0] out_data_p2;
	wire [`CHANNELS - 1:0] out_rr_request_p2;
	//module declaration
	m_Assembled_Channel
		#(
	//.P_ROUTER_ID(P_ROUTER_ID),
	.P_CHANNEL(2),
	.P_DATA_WIDTH(P_DATA_WIDTH)
	)
	channel_p2
	(
	.router_id(router_id),
	.RST(RST),
	.CLK(CLK),
	.in_neighbor_credits_p0(p0_in_credits_feedback),
	.in_neighbor_credits_p1(p1_in_credits_feedback),
	.in_neighbor_credits_p2(p2_in_credits_feedback),
	.in_neighbor_credits_p3(p3_in_credits_feedback),
	.in_neighbor_credits_p4(p4_in_credits_feedback),
	//sa signals:
	.out_sa_enable(out_sa_enable_p2),
	.out_sa_request(out_sa_request_p2),
	.in_sa_grant(grant_p2),

	//st signals:
	.out_st_request(out_st_request_p2),
	.in_st_credits_feedback(crossbar_feedback_out_p2),
	.out_st_data_valid(out_st_data_valid_p2),

	.in_data(p2_in_data),
	.out_data(out_data_p2),

	.out_buffer_credits_sharing(p2_out_credits_feedback),
	.in_received_data_valid(p2_in_data_valid),

	.out_rr_request(out_rr_request_p2),
	.in_rr_result(intern_rr_result)
);

	//Channel p3
	//wire declaration;
	wire out_sa_enable_p3;
	wire [`CHANNELS - 1:0] out_sa_request_p3;
	wire [`CHANNELS - 1:0] out_st_request_p3;
	wire out_st_data_valid_p3;
	wire [P_DATA_WIDTH - 1: 0] out_data_p3;
	wire [`CHANNELS - 1:0] out_rr_request_p3;
	//module declaration
	m_Assembled_Channel
		#(
	//.P_ROUTER_ID(P_ROUTER_ID),
	.P_CHANNEL(3),
	.P_DATA_WIDTH(P_DATA_WIDTH)
	)
	channel_p3
	(
	.router_id(router_id),
	.RST(RST),
	.CLK(CLK),
	.in_neighbor_credits_p0(p0_in_credits_feedback),
	.in_neighbor_credits_p1(p1_in_credits_feedback),
	.in_neighbor_credits_p2(p2_in_credits_feedback),
	.in_neighbor_credits_p3(p3_in_credits_feedback),
	.in_neighbor_credits_p4(p4_in_credits_feedback),
	//sa signals:
	.out_sa_enable(out_sa_enable_p3),
	.out_sa_request(out_sa_request_p3),
	.in_sa_grant(grant_p3),

	//st signals:
	.out_st_request(out_st_request_p3),
	.in_st_credits_feedback(crossbar_feedback_out_p3),
	.out_st_data_valid(out_st_data_valid_p3),

	.in_data(p3_in_data),
	.out_data(out_data_p3),

	.out_buffer_credits_sharing(p3_out_credits_feedback),
	.in_received_data_valid(p3_in_data_valid),

	.out_rr_request(out_rr_request_p3),
	.in_rr_result(intern_rr_result)
);

	//Channel p4
	//wire declaration;
	wire out_sa_enable_p4;
	wire [`CHANNELS - 1:0] out_sa_request_p4;
	wire [`CHANNELS - 1:0] out_st_request_p4;
	wire out_st_data_valid_p4;
	wire [P_DATA_WIDTH - 1: 0] out_data_p4;
	wire [`CHANNELS - 1:0] out_rr_request_p4;
	//module declaration
	m_Assembled_Channel
		#(
	//.P_ROUTER_ID(P_ROUTER_ID),
	.P_CHANNEL(4),
	.P_DATA_WIDTH(P_DATA_WIDTH)
	)
	channel_p4
	(
	.router_id(router_id),
	.RST(RST),
	.CLK(CLK),
	.in_neighbor_credits_p0(p0_in_credits_feedback),
	.in_neighbor_credits_p1(p1_in_credits_feedback),
	.in_neighbor_credits_p2(p2_in_credits_feedback),
	.in_neighbor_credits_p3(p3_in_credits_feedback),
	.in_neighbor_credits_p4(p4_in_credits_feedback),
	//sa signals:
	.out_sa_enable(out_sa_enable_p4),
	.out_sa_request(out_sa_request_p4),
	.in_sa_grant(grant_p4),

	//st signals:
	.out_st_request(out_st_request_p4),
	.in_st_credits_feedback(crossbar_feedback_out_p4),
	.out_st_data_valid(out_st_data_valid_p4),

	.in_data(p4_in_data),
	.out_data(out_data_p4),

	.out_buffer_credits_sharing(p4_out_credits_feedback),
	.in_received_data_valid(p4_in_data_valid),

	.out_rr_request(out_rr_request_p4),
	.in_rr_result(intern_rr_result)
);

	//assignment
	//solve intern_rr_request
	assign intern_rr_request[`P0] = out_rr_request_p0;
	assign intern_rr_request[`P1] = out_rr_request_p1;
	assign intern_rr_request[`P2] = out_rr_request_p2;
	assign intern_rr_request[`P3] = out_rr_request_p3;
	assign intern_rr_request[`P4] = out_rr_request_p4;


	//solve intern_rr_result
	//inter_rr_result[0]
	assign intern_rr_result[`P0] = ({
		intern_rr_request[`P4][`P0],
		intern_rr_request[`P3][`P0],
		intern_rr_request[`P2][`P0],
		intern_rr_request[`P1][`P0],
		intern_rr_request[`P0][`P0]} != 0);

	//inter_rr_result[1]
	assign intern_rr_result[`P1] = ({
		intern_rr_request[`P4][`P1],
		intern_rr_request[`P3][`P1],
		intern_rr_request[`P2][`P1],
		intern_rr_request[`P1][`P1],
		intern_rr_request[`P0][`P1]} != 0);

	//inter_rr_result[2]
	assign intern_rr_result[`P2] = ({
		intern_rr_request[`P4][`P2],
		intern_rr_request[`P3][`P2],
		intern_rr_request[`P2][`P2],
		intern_rr_request[`P1][`P2],
		intern_rr_request[`P0][`P2]} != 0);

	//inter_rr_result[3]
	assign intern_rr_result[`P3] = ({
		intern_rr_request[`P4][`P3],
		intern_rr_request[`P3][`P3],
		intern_rr_request[`P2][`P3],
		intern_rr_request[`P1][`P3],
		intern_rr_request[`P0][`P3]} != 0);

	//inter_rr_result[4]
	assign intern_rr_result[`P4] = ({
		intern_rr_request[`P4][`P4],
		intern_rr_request[`P3][`P4],
		intern_rr_request[`P2][`P4],
		intern_rr_request[`P1][`P4],
		intern_rr_request[`P0][`P4]} != 0);

	
	
	//module declaration
	m_SwitchAllocation	switchAllocation (
	//connection for port 0
	.enable_p0(out_sa_enable_p0),
	.request_p0(out_sa_request_p0),
	.grant_p0(grant_p0),

	//connection for port 1
	.enable_p1(out_sa_enable_p1),
	.request_p1(out_sa_request_p1),
	.grant_p1(grant_p1),

	//connection for port 2
	.enable_p2(out_sa_enable_p2),
	.request_p2(out_sa_request_p2),
	.grant_p2(grant_p2),

	//connection for port 3
	.enable_p3(out_sa_enable_p3),
	.request_p3(out_sa_request_p3),
	.grant_p3(grant_p3),

	//connection for port 4
	.enable_p4(out_sa_enable_p4),
	.request_p4(out_sa_request_p4),
	.grant_p4(grant_p4),

	
	.CLK(CLK),
	.RST(RST)
	);


	//module declaration
	m_Crossbar #(
	.P_DATA_WIDTH(P_DATA_WIDTH + `DATA_VALID_WIDTH),
	.P_FEEDBACK_WIDTH(`BUFFERSIZE_WIDTH)
	) crossbar (
	//for port 0
	.request_0(out_st_request_p0),
	.data_in_0({out_st_data_valid_p0,out_data_p0}),
	.feedback_out_0(crossbar_feedback_out_p0),
	.data_out_0(crossbar_out_data_p0),
	.feedback_in_0(p0_in_credits_feedback),

	//for port 1
	.request_1(out_st_request_p1),
	.data_in_1({out_st_data_valid_p1,out_data_p1}),
	.feedback_out_1(crossbar_feedback_out_p1),
	.data_out_1(crossbar_out_data_p1),
	.feedback_in_1(p1_in_credits_feedback),

	//for port 2
	.request_2(out_st_request_p2),
	.data_in_2({out_st_data_valid_p2,out_data_p2}),
	.feedback_out_2(crossbar_feedback_out_p2),
	.data_out_2(crossbar_out_data_p2),
	.feedback_in_2(p2_in_credits_feedback),

	//for port 3
	.request_3(out_st_request_p3),
	.data_in_3({out_st_data_valid_p3,out_data_p3}),
	.feedback_out_3(crossbar_feedback_out_p3),
	.data_out_3(crossbar_out_data_p3),
	.feedback_in_3(p3_in_credits_feedback),

	//for port 4
	.request_4(out_st_request_p4),
	.data_in_4({out_st_data_valid_p4,out_data_p4}),
	.feedback_out_4(crossbar_feedback_out_p4),
	.data_out_4(crossbar_out_data_p4),
	.feedback_in_4(p4_in_credits_feedback));

	//Mux Module declaration 
	
	//Mux for port 0
	//Mux for port 1
	//Mux for port 2
	//Mux for port 3
	//Mux for port 4


	//D_FF_sync  for port 0
	m_D_FF_sync #(
	.P_DATA_WIDTH(`DATA_VALID_WIDTH + P_DATA_WIDTH)
	) output_buffer_data_p0(
	.CLK(CLK),
	.RST(RST),
	.D(crossbar_out_data_p0),
	.Q(p0_output_buffer)
	);

	//D_FF_sync  for port 1
	m_D_FF_sync #(
	.P_DATA_WIDTH(`DATA_VALID_WIDTH + P_DATA_WIDTH)
	) output_buffer_data_p1(
	.CLK(CLK),
	.RST(RST),
	.D(crossbar_out_data_p1),
	.Q(p1_output_buffer)
	);

	//D_FF_sync  for port 2
	m_D_FF_sync #(
	.P_DATA_WIDTH(`DATA_VALID_WIDTH + P_DATA_WIDTH)
	) output_buffer_data_p2(
	.CLK(CLK),
	.RST(RST),
	.D(crossbar_out_data_p2),
	.Q(p2_output_buffer)
	);

	//D_FF_sync  for port 3
	m_D_FF_sync #(
	.P_DATA_WIDTH(`DATA_VALID_WIDTH + P_DATA_WIDTH)
	) output_buffer_data_p3(
	.CLK(CLK),
	.RST(RST),
	.D(crossbar_out_data_p3),
	.Q(p3_output_buffer)
	);

	//D_FF_sync  for port 4
	m_D_FF_sync #(
	.P_DATA_WIDTH(`DATA_VALID_WIDTH + P_DATA_WIDTH)
	) output_buffer_data_p4(
	.CLK(CLK),
	.RST(RST),
	.D(crossbar_out_data_p4),
	.Q(p4_output_buffer)
	);

	endmodule

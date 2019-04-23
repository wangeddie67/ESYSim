`timescale 1ns / 1ps
`include "parameters.v"

module m_Assembled_NoC (
	 CLK,
	 RST,

	//local port signals of router_0
	 r0_p0_in_data_valid,
	 r0_p0_out_data_valid,
	r0_p0_in_data,
	r0_p0_out_data,
	r0_p0_in_credits_feedback,
	r0_p0_out_credits_feedback,

	//local port signals of router_1
	 r1_p0_in_data_valid,
	 r1_p0_out_data_valid,
	r1_p0_in_data,
	r1_p0_out_data,
	r1_p0_in_credits_feedback,
	r1_p0_out_credits_feedback,

	//local port signals of router_2
	 r2_p0_in_data_valid,
	 r2_p0_out_data_valid,
	r2_p0_in_data,
	r2_p0_out_data,
	r2_p0_in_credits_feedback,
	r2_p0_out_credits_feedback,

	//local port signals of router_3
	 r3_p0_in_data_valid,
	 r3_p0_out_data_valid,
	r3_p0_in_data,
	r3_p0_out_data,
	r3_p0_in_credits_feedback,
	r3_p0_out_credits_feedback,

	//local port signals of router_4
	 r4_p0_in_data_valid,
	 r4_p0_out_data_valid,
	r4_p0_in_data,
	r4_p0_out_data,
	r4_p0_in_credits_feedback,
	r4_p0_out_credits_feedback,

	//local port signals of router_5
	 r5_p0_in_data_valid,
	 r5_p0_out_data_valid,
	r5_p0_in_data,
	r5_p0_out_data,
	r5_p0_in_credits_feedback,
	r5_p0_out_credits_feedback,

	//local port signals of router_6
	 r6_p0_in_data_valid,
	 r6_p0_out_data_valid,
	r6_p0_in_data,
	r6_p0_out_data,
	r6_p0_in_credits_feedback,
	r6_p0_out_credits_feedback,

	//local port signals of router_7
	 r7_p0_in_data_valid,
	 r7_p0_out_data_valid,
	r7_p0_in_data,
	r7_p0_out_data,
	r7_p0_in_credits_feedback,
	r7_p0_out_credits_feedback,

	//local port signals of router_8
	 r8_p0_in_data_valid,
	 r8_p0_out_data_valid,
	r8_p0_in_data,
	r8_p0_out_data,
	r8_p0_in_credits_feedback,
	r8_p0_out_credits_feedback,

	//local port signals of router_9
	 r9_p0_in_data_valid,
	 r9_p0_out_data_valid,
	r9_p0_in_data,
	r9_p0_out_data,
	r9_p0_in_credits_feedback,
	r9_p0_out_credits_feedback,

	//local port signals of router_10
	 r10_p0_in_data_valid,
	 r10_p0_out_data_valid,
	r10_p0_in_data,
	r10_p0_out_data,
	r10_p0_in_credits_feedback,
	r10_p0_out_credits_feedback,

	//local port signals of router_11
	 r11_p0_in_data_valid,
	 r11_p0_out_data_valid,
	r11_p0_in_data,
	r11_p0_out_data,
	r11_p0_in_credits_feedback,
	r11_p0_out_credits_feedback,

	//local port signals of router_12
	 r12_p0_in_data_valid,
	 r12_p0_out_data_valid,
	r12_p0_in_data,
	r12_p0_out_data,
	r12_p0_in_credits_feedback,
	r12_p0_out_credits_feedback,

	//local port signals of router_13
	 r13_p0_in_data_valid,
	 r13_p0_out_data_valid,
	r13_p0_in_data,
	r13_p0_out_data,
	r13_p0_in_credits_feedback,
	r13_p0_out_credits_feedback,

	//local port signals of router_14
	 r14_p0_in_data_valid,
	 r14_p0_out_data_valid,
	r14_p0_in_data,
	r14_p0_out_data,
	r14_p0_in_credits_feedback,
	r14_p0_out_credits_feedback,

	//local port signals of router_15
	 r15_p0_in_data_valid,
	 r15_p0_out_data_valid,
	r15_p0_in_data,
	r15_p0_out_data,
	r15_p0_in_credits_feedback,
	r15_p0_out_credits_feedback
);


	//parameters
	`ifdef FT_ECC
	    parameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH; 
	`else 
		parameter P_DATA_WIDTH = `FLIT_WIDTH;
	`endif

	//define zero signals
	wire [P_DATA_WIDTH-1 : 0] d_zero;
	wire [`BUFFERSIZE_WIDTH-1 : 0] cf_zero;

	assign d_zero = 'b0;
	assign cf_zero = 'b0;
	
	
	input CLK;
	input RST;

	//local port signals of router_0
	input r0_p0_in_data_valid;
	output r0_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r0_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r0_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r0_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r0_p0_out_credits_feedback;

	//local port signals of router_1
	input r1_p0_in_data_valid;
	output r1_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r1_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r1_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r1_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r1_p0_out_credits_feedback;

	//local port signals of router_2
	input r2_p0_in_data_valid;
	output r2_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r2_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r2_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r2_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r2_p0_out_credits_feedback;

	//local port signals of router_3
	input r3_p0_in_data_valid;
	output r3_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r3_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r3_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r3_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r3_p0_out_credits_feedback;

	//local port signals of router_4
	input r4_p0_in_data_valid;
	output r4_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r4_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r4_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r4_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r4_p0_out_credits_feedback;

	//local port signals of router_5
	input r5_p0_in_data_valid;
	output r5_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r5_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r5_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r5_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r5_p0_out_credits_feedback;

	//local port signals of router_6
	input r6_p0_in_data_valid;
	output r6_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r6_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r6_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r6_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r6_p0_out_credits_feedback;

	//local port signals of router_7
	input r7_p0_in_data_valid;
	output r7_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r7_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r7_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r7_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r7_p0_out_credits_feedback;

	//local port signals of router_8
	input r8_p0_in_data_valid;
	output r8_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r8_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r8_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r8_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r8_p0_out_credits_feedback;

	//local port signals of router_9
	input r9_p0_in_data_valid;
	output r9_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r9_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r9_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r9_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r9_p0_out_credits_feedback;

	//local port signals of router_10
	input r10_p0_in_data_valid;
	output r10_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r10_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r10_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r10_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r10_p0_out_credits_feedback;

	//local port signals of router_11
	input r11_p0_in_data_valid;
	output r11_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r11_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r11_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r11_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r11_p0_out_credits_feedback;

	//local port signals of router_12
	input r12_p0_in_data_valid;
	output r12_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r12_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r12_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r12_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r12_p0_out_credits_feedback;

	//local port signals of router_13
	input r13_p0_in_data_valid;
	output r13_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r13_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r13_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r13_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r13_p0_out_credits_feedback;

	//local port signals of router_14
	input r14_p0_in_data_valid;
	output r14_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r14_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r14_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r14_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r14_p0_out_credits_feedback;

	//local port signals of router_15
	input r15_p0_in_data_valid;
	output r15_p0_out_data_valid;
	input [P_DATA_WIDTH - 1: 0] r15_p0_in_data;
	output [P_DATA_WIDTH - 1: 0] r15_p0_out_data;
	input [`BUFFERSIZE_WIDTH - 1 : 0] r15_p0_in_credits_feedback;
	output [`BUFFERSIZE_WIDTH - 1: 0] r15_p0_out_credits_feedback;

	//router_0 port_1:
	wire [P_DATA_WIDTH - 1: 0] r0_p1_out_data;
	wire r0_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r0_p1_out_credits_feedback;

	//router_0 port_2:
	wire [P_DATA_WIDTH - 1: 0] r0_p2_out_data;
	wire r0_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r0_p2_out_credits_feedback;

	//router_0 port_3:
	wire [P_DATA_WIDTH - 1: 0] r0_p3_out_data;
	wire r0_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r0_p3_out_credits_feedback;

	//router_0 port_4:
	wire [P_DATA_WIDTH - 1: 0] r0_p4_out_data;
	wire r0_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r0_p4_out_credits_feedback;

	//router_1 port_1:
	wire [P_DATA_WIDTH - 1: 0] r1_p1_out_data;
	wire r1_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r1_p1_out_credits_feedback;

	//router_1 port_2:
	wire [P_DATA_WIDTH - 1: 0] r1_p2_out_data;
	wire r1_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r1_p2_out_credits_feedback;

	//router_1 port_3:
	wire [P_DATA_WIDTH - 1: 0] r1_p3_out_data;
	wire r1_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r1_p3_out_credits_feedback;

	//router_1 port_4:
	wire [P_DATA_WIDTH - 1: 0] r1_p4_out_data;
	wire r1_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r1_p4_out_credits_feedback;

	//router_2 port_1:
	wire [P_DATA_WIDTH - 1: 0] r2_p1_out_data;
	wire r2_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r2_p1_out_credits_feedback;

	//router_2 port_2:
	wire [P_DATA_WIDTH - 1: 0] r2_p2_out_data;
	wire r2_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r2_p2_out_credits_feedback;

	//router_2 port_3:
	wire [P_DATA_WIDTH - 1: 0] r2_p3_out_data;
	wire r2_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r2_p3_out_credits_feedback;

	//router_2 port_4:
	wire [P_DATA_WIDTH - 1: 0] r2_p4_out_data;
	wire r2_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r2_p4_out_credits_feedback;

	//router_3 port_1:
	wire [P_DATA_WIDTH - 1: 0] r3_p1_out_data;
	wire r3_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r3_p1_out_credits_feedback;

	//router_3 port_2:
	wire [P_DATA_WIDTH - 1: 0] r3_p2_out_data;
	wire r3_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r3_p2_out_credits_feedback;

	//router_3 port_3:
	wire [P_DATA_WIDTH - 1: 0] r3_p3_out_data;
	wire r3_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r3_p3_out_credits_feedback;

	//router_3 port_4:
	wire [P_DATA_WIDTH - 1: 0] r3_p4_out_data;
	wire r3_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r3_p4_out_credits_feedback;

	//router_4 port_1:
	wire [P_DATA_WIDTH - 1: 0] r4_p1_out_data;
	wire r4_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r4_p1_out_credits_feedback;

	//router_4 port_2:
	wire [P_DATA_WIDTH - 1: 0] r4_p2_out_data;
	wire r4_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r4_p2_out_credits_feedback;

	//router_4 port_3:
	wire [P_DATA_WIDTH - 1: 0] r4_p3_out_data;
	wire r4_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r4_p3_out_credits_feedback;

	//router_4 port_4:
	wire [P_DATA_WIDTH - 1: 0] r4_p4_out_data;
	wire r4_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r4_p4_out_credits_feedback;

	//router_5 port_1:
	wire [P_DATA_WIDTH - 1: 0] r5_p1_out_data;
	wire r5_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r5_p1_out_credits_feedback;

	//router_5 port_2:
	wire [P_DATA_WIDTH - 1: 0] r5_p2_out_data;
	wire r5_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r5_p2_out_credits_feedback;

	//router_5 port_3:
	wire [P_DATA_WIDTH - 1: 0] r5_p3_out_data;
	wire r5_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r5_p3_out_credits_feedback;

	//router_5 port_4:
	wire [P_DATA_WIDTH - 1: 0] r5_p4_out_data;
	wire r5_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r5_p4_out_credits_feedback;

	//router_6 port_1:
	wire [P_DATA_WIDTH - 1: 0] r6_p1_out_data;
	wire r6_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r6_p1_out_credits_feedback;

	//router_6 port_2:
	wire [P_DATA_WIDTH - 1: 0] r6_p2_out_data;
	wire r6_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r6_p2_out_credits_feedback;

	//router_6 port_3:
	wire [P_DATA_WIDTH - 1: 0] r6_p3_out_data;
	wire r6_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r6_p3_out_credits_feedback;

	//router_6 port_4:
	wire [P_DATA_WIDTH - 1: 0] r6_p4_out_data;
	wire r6_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r6_p4_out_credits_feedback;

	//router_7 port_1:
	wire [P_DATA_WIDTH - 1: 0] r7_p1_out_data;
	wire r7_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r7_p1_out_credits_feedback;

	//router_7 port_2:
	wire [P_DATA_WIDTH - 1: 0] r7_p2_out_data;
	wire r7_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r7_p2_out_credits_feedback;

	//router_7 port_3:
	wire [P_DATA_WIDTH - 1: 0] r7_p3_out_data;
	wire r7_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r7_p3_out_credits_feedback;

	//router_7 port_4:
	wire [P_DATA_WIDTH - 1: 0] r7_p4_out_data;
	wire r7_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r7_p4_out_credits_feedback;

	//router_8 port_1:
	wire [P_DATA_WIDTH - 1: 0] r8_p1_out_data;
	wire r8_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r8_p1_out_credits_feedback;

	//router_8 port_2:
	wire [P_DATA_WIDTH - 1: 0] r8_p2_out_data;
	wire r8_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r8_p2_out_credits_feedback;

	//router_8 port_3:
	wire [P_DATA_WIDTH - 1: 0] r8_p3_out_data;
	wire r8_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r8_p3_out_credits_feedback;

	//router_8 port_4:
	wire [P_DATA_WIDTH - 1: 0] r8_p4_out_data;
	wire r8_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r8_p4_out_credits_feedback;

	//router_9 port_1:
	wire [P_DATA_WIDTH - 1: 0] r9_p1_out_data;
	wire r9_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r9_p1_out_credits_feedback;

	//router_9 port_2:
	wire [P_DATA_WIDTH - 1: 0] r9_p2_out_data;
	wire r9_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r9_p2_out_credits_feedback;

	//router_9 port_3:
	wire [P_DATA_WIDTH - 1: 0] r9_p3_out_data;
	wire r9_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r9_p3_out_credits_feedback;

	//router_9 port_4:
	wire [P_DATA_WIDTH - 1: 0] r9_p4_out_data;
	wire r9_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r9_p4_out_credits_feedback;

	//router_10 port_1:
	wire [P_DATA_WIDTH - 1: 0] r10_p1_out_data;
	wire r10_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r10_p1_out_credits_feedback;

	//router_10 port_2:
	wire [P_DATA_WIDTH - 1: 0] r10_p2_out_data;
	wire r10_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r10_p2_out_credits_feedback;

	//router_10 port_3:
	wire [P_DATA_WIDTH - 1: 0] r10_p3_out_data;
	wire r10_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r10_p3_out_credits_feedback;

	//router_10 port_4:
	wire [P_DATA_WIDTH - 1: 0] r10_p4_out_data;
	wire r10_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r10_p4_out_credits_feedback;

	//router_11 port_1:
	wire [P_DATA_WIDTH - 1: 0] r11_p1_out_data;
	wire r11_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r11_p1_out_credits_feedback;

	//router_11 port_2:
	wire [P_DATA_WIDTH - 1: 0] r11_p2_out_data;
	wire r11_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r11_p2_out_credits_feedback;

	//router_11 port_3:
	wire [P_DATA_WIDTH - 1: 0] r11_p3_out_data;
	wire r11_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r11_p3_out_credits_feedback;

	//router_11 port_4:
	wire [P_DATA_WIDTH - 1: 0] r11_p4_out_data;
	wire r11_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r11_p4_out_credits_feedback;

	//router_12 port_1:
	wire [P_DATA_WIDTH - 1: 0] r12_p1_out_data;
	wire r12_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r12_p1_out_credits_feedback;

	//router_12 port_2:
	wire [P_DATA_WIDTH - 1: 0] r12_p2_out_data;
	wire r12_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r12_p2_out_credits_feedback;

	//router_12 port_3:
	wire [P_DATA_WIDTH - 1: 0] r12_p3_out_data;
	wire r12_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r12_p3_out_credits_feedback;

	//router_12 port_4:
	wire [P_DATA_WIDTH - 1: 0] r12_p4_out_data;
	wire r12_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r12_p4_out_credits_feedback;

	//router_13 port_1:
	wire [P_DATA_WIDTH - 1: 0] r13_p1_out_data;
	wire r13_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r13_p1_out_credits_feedback;

	//router_13 port_2:
	wire [P_DATA_WIDTH - 1: 0] r13_p2_out_data;
	wire r13_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r13_p2_out_credits_feedback;

	//router_13 port_3:
	wire [P_DATA_WIDTH - 1: 0] r13_p3_out_data;
	wire r13_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r13_p3_out_credits_feedback;

	//router_13 port_4:
	wire [P_DATA_WIDTH - 1: 0] r13_p4_out_data;
	wire r13_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r13_p4_out_credits_feedback;

	//router_14 port_1:
	wire [P_DATA_WIDTH - 1: 0] r14_p1_out_data;
	wire r14_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r14_p1_out_credits_feedback;

	//router_14 port_2:
	wire [P_DATA_WIDTH - 1: 0] r14_p2_out_data;
	wire r14_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r14_p2_out_credits_feedback;

	//router_14 port_3:
	wire [P_DATA_WIDTH - 1: 0] r14_p3_out_data;
	wire r14_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r14_p3_out_credits_feedback;

	//router_14 port_4:
	wire [P_DATA_WIDTH - 1: 0] r14_p4_out_data;
	wire r14_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r14_p4_out_credits_feedback;

	//router_15 port_1:
	wire [P_DATA_WIDTH - 1: 0] r15_p1_out_data;
	wire r15_p1_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r15_p1_out_credits_feedback;

	//router_15 port_2:
	wire [P_DATA_WIDTH - 1: 0] r15_p2_out_data;
	wire r15_p2_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r15_p2_out_credits_feedback;

	//router_15 port_3:
	wire [P_DATA_WIDTH - 1: 0] r15_p3_out_data;
	wire r15_p3_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r15_p3_out_credits_feedback;

	//router_15 port_4:
	wire [P_DATA_WIDTH - 1: 0] r15_p4_out_data;
	wire r15_p4_out_data_valid;
	wire [`BUFFERSIZE_WIDTH - 1:0] r15_p4_out_credits_feedback;

//declare router0
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_0;
	assign router_id_0 = 0;
	m_Assembled_Router_faulty	router_0(
	.router_id(router_id_0),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r0_p0_in_data),
	.p0_out_data(r0_p0_out_data),
	.p0_in_data_valid(r0_p0_in_data_valid),
	.p0_out_data_valid(r0_p0_out_data_valid),
	.p0_in_credits_feedback(r0_p0_in_credits_feedback),
	.p0_out_credits_feedback(r0_p0_out_credits_feedback),

//port_1
	.p1_in_data(d_zero),
	.p1_out_data(),
	.p1_in_data_valid(1'b0),
	.p1_out_data_valid(),
	.p1_in_credits_feedback(cf_zero),
	.p1_out_credits_feedback(),

//port_2
	.p2_in_data(r4_p1_out_data),
	.p2_out_data(r0_p2_out_data),
	.p2_in_data_valid(r4_p1_out_data_valid),
	.p2_out_data_valid(r0_p2_out_data_valid),
	.p2_in_credits_feedback(r4_p1_out_credits_feedback),
	.p2_out_credits_feedback(r0_p2_out_credits_feedback),

//port_3
	.p3_in_data(d_zero),
	.p3_out_data(),
	.p3_in_data_valid(1'b0),
	.p3_out_data_valid(),
	.p3_in_credits_feedback(cf_zero),
	.p3_out_credits_feedback(),

//port_4
	.p4_in_data(r1_p3_out_data),
	.p4_out_data(r0_p4_out_data),
	.p4_in_data_valid(r1_p3_out_data_valid),
	.p4_out_data_valid(r0_p4_out_data_valid),
	.p4_in_credits_feedback(r1_p3_out_credits_feedback),
	.p4_out_credits_feedback(r0_p4_out_credits_feedback)
);

//declare router1
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_1;
	assign router_id_1 = 1;
	m_Assembled_Router_faulty	router_1(
	.router_id(router_id_1),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r1_p0_in_data),
	.p0_out_data(r1_p0_out_data),
	.p0_in_data_valid(r1_p0_in_data_valid),
	.p0_out_data_valid(r1_p0_out_data_valid),
	.p0_in_credits_feedback(r1_p0_in_credits_feedback),
	.p0_out_credits_feedback(r1_p0_out_credits_feedback),

//port_1
	.p1_in_data(d_zero),
	.p1_out_data(),
	.p1_in_data_valid(1'b0),
	.p1_out_data_valid(),
	.p1_in_credits_feedback(cf_zero),
	.p1_out_credits_feedback(),

//port_2
	.p2_in_data(r5_p1_out_data),
	.p2_out_data(r1_p2_out_data),
	.p2_in_data_valid(r5_p1_out_data_valid),
	.p2_out_data_valid(r1_p2_out_data_valid),
	.p2_in_credits_feedback(r5_p1_out_credits_feedback),
	.p2_out_credits_feedback(r1_p2_out_credits_feedback),

//port_3
	.p3_in_data(r0_p4_out_data),
	.p3_out_data(r1_p3_out_data),
	.p3_in_data_valid(r0_p4_out_data_valid),
	.p3_out_data_valid(r1_p3_out_data_valid),
	.p3_in_credits_feedback(r0_p4_out_credits_feedback),
	.p3_out_credits_feedback(r1_p3_out_credits_feedback),

//port_4
	.p4_in_data(r2_p3_out_data),
	.p4_out_data(r1_p4_out_data),
	.p4_in_data_valid(r2_p3_out_data_valid),
	.p4_out_data_valid(r1_p4_out_data_valid),
	.p4_in_credits_feedback(r2_p3_out_credits_feedback),
	.p4_out_credits_feedback(r1_p4_out_credits_feedback)
);

//declare router2
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_2;
	assign router_id_2 = 2;
	m_Assembled_Router_faulty	router_2(
	.router_id(router_id_2),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r2_p0_in_data),
	.p0_out_data(r2_p0_out_data),
	.p0_in_data_valid(r2_p0_in_data_valid),
	.p0_out_data_valid(r2_p0_out_data_valid),
	.p0_in_credits_feedback(r2_p0_in_credits_feedback),
	.p0_out_credits_feedback(r2_p0_out_credits_feedback),

//port_1
	.p1_in_data(d_zero),
	.p1_out_data(),
	.p1_in_data_valid(1'b0),
	.p1_out_data_valid(),
	.p1_in_credits_feedback(cf_zero),
	.p1_out_credits_feedback(),

//port_2
	.p2_in_data(r6_p1_out_data),
	.p2_out_data(r2_p2_out_data),
	.p2_in_data_valid(r6_p1_out_data_valid),
	.p2_out_data_valid(r2_p2_out_data_valid),
	.p2_in_credits_feedback(r6_p1_out_credits_feedback),
	.p2_out_credits_feedback(r2_p2_out_credits_feedback),

//port_3
	.p3_in_data(r1_p4_out_data),
	.p3_out_data(r2_p3_out_data),
	.p3_in_data_valid(r1_p4_out_data_valid),
	.p3_out_data_valid(r2_p3_out_data_valid),
	.p3_in_credits_feedback(r1_p4_out_credits_feedback),
	.p3_out_credits_feedback(r2_p3_out_credits_feedback),

//port_4
	.p4_in_data(r3_p3_out_data),
	.p4_out_data(r2_p4_out_data),
	.p4_in_data_valid(r3_p3_out_data_valid),
	.p4_out_data_valid(r2_p4_out_data_valid),
	.p4_in_credits_feedback(r3_p3_out_credits_feedback),
	.p4_out_credits_feedback(r2_p4_out_credits_feedback)
);

//declare router3
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_3;
	assign router_id_3 = 3;
	m_Assembled_Router_faulty	router_3(
	.router_id(router_id_3),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r3_p0_in_data),
	.p0_out_data(r3_p0_out_data),
	.p0_in_data_valid(r3_p0_in_data_valid),
	.p0_out_data_valid(r3_p0_out_data_valid),
	.p0_in_credits_feedback(r3_p0_in_credits_feedback),
	.p0_out_credits_feedback(r3_p0_out_credits_feedback),

//port_1
	.p1_in_data(d_zero),
	.p1_out_data(),
	.p1_in_data_valid(1'b0),
	.p1_out_data_valid(),
	.p1_in_credits_feedback(cf_zero),
	.p1_out_credits_feedback(),

//port_2
	.p2_in_data(r7_p1_out_data),
	.p2_out_data(r3_p2_out_data),
	.p2_in_data_valid(r7_p1_out_data_valid),
	.p2_out_data_valid(r3_p2_out_data_valid),
	.p2_in_credits_feedback(r7_p1_out_credits_feedback),
	.p2_out_credits_feedback(r3_p2_out_credits_feedback),

//port_3
	.p3_in_data(r2_p4_out_data),
	.p3_out_data(r3_p3_out_data),
	.p3_in_data_valid(r2_p4_out_data_valid),
	.p3_out_data_valid(r3_p3_out_data_valid),
	.p3_in_credits_feedback(r2_p4_out_credits_feedback),
	.p3_out_credits_feedback(r3_p3_out_credits_feedback),

//port_4
	.p4_in_data(d_zero),
	.p4_out_data(),
	.p4_in_data_valid(1'b0),
	.p4_out_data_valid(),
	.p4_in_credits_feedback(cf_zero),
	.p4_out_credits_feedback()
);

//declare router4
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_4;
	assign router_id_4 = 4;
	m_Assembled_Router_faulty	router_4(
	.router_id(router_id_4),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r4_p0_in_data),
	.p0_out_data(r4_p0_out_data),
	.p0_in_data_valid(r4_p0_in_data_valid),
	.p0_out_data_valid(r4_p0_out_data_valid),
	.p0_in_credits_feedback(r4_p0_in_credits_feedback),
	.p0_out_credits_feedback(r4_p0_out_credits_feedback),

//port_1
	.p1_in_data(r0_p2_out_data),
	.p1_out_data(r4_p1_out_data),
	.p1_in_data_valid(r0_p2_out_data_valid),
	.p1_out_data_valid(r4_p1_out_data_valid),
	.p1_in_credits_feedback(r0_p2_out_credits_feedback),
	.p1_out_credits_feedback(r4_p1_out_credits_feedback),

//port_2
	.p2_in_data(r8_p1_out_data),
	.p2_out_data(r4_p2_out_data),
	.p2_in_data_valid(r8_p1_out_data_valid),
	.p2_out_data_valid(r4_p2_out_data_valid),
	.p2_in_credits_feedback(r8_p1_out_credits_feedback),
	.p2_out_credits_feedback(r4_p2_out_credits_feedback),

//port_3
	.p3_in_data(d_zero),
	.p3_out_data(),
	.p3_in_data_valid(1'b0),
	.p3_out_data_valid(),
	.p3_in_credits_feedback(cf_zero),
	.p3_out_credits_feedback(),

//port_4
	.p4_in_data(r5_p3_out_data),
	.p4_out_data(r4_p4_out_data),
	.p4_in_data_valid(r5_p3_out_data_valid),
	.p4_out_data_valid(r4_p4_out_data_valid),
	.p4_in_credits_feedback(r5_p3_out_credits_feedback),
	.p4_out_credits_feedback(r4_p4_out_credits_feedback)
);

//declare router5
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_5;
	assign router_id_5 = 5;
	m_Assembled_Router_faulty	router_5(
	.router_id(router_id_5),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r5_p0_in_data),
	.p0_out_data(r5_p0_out_data),
	.p0_in_data_valid(r5_p0_in_data_valid),
	.p0_out_data_valid(r5_p0_out_data_valid),
	.p0_in_credits_feedback(r5_p0_in_credits_feedback),
	.p0_out_credits_feedback(r5_p0_out_credits_feedback),

//port_1
	.p1_in_data(r1_p2_out_data),
	.p1_out_data(r5_p1_out_data),
	.p1_in_data_valid(r1_p2_out_data_valid),
	.p1_out_data_valid(r5_p1_out_data_valid),
	.p1_in_credits_feedback(r1_p2_out_credits_feedback),
	.p1_out_credits_feedback(r5_p1_out_credits_feedback),

//port_2
	.p2_in_data(r9_p1_out_data),
	.p2_out_data(r5_p2_out_data),
	.p2_in_data_valid(r9_p1_out_data_valid),
	.p2_out_data_valid(r5_p2_out_data_valid),
	.p2_in_credits_feedback(r9_p1_out_credits_feedback),
	.p2_out_credits_feedback(r5_p2_out_credits_feedback),

//port_3
	.p3_in_data(r4_p4_out_data),
	.p3_out_data(r5_p3_out_data),
	.p3_in_data_valid(r4_p4_out_data_valid),
	.p3_out_data_valid(r5_p3_out_data_valid),
	.p3_in_credits_feedback(r4_p4_out_credits_feedback),
	.p3_out_credits_feedback(r5_p3_out_credits_feedback),

//port_4
	.p4_in_data(r6_p3_out_data),
	.p4_out_data(r5_p4_out_data),
	.p4_in_data_valid(r6_p3_out_data_valid),
	.p4_out_data_valid(r5_p4_out_data_valid),
	.p4_in_credits_feedback(r6_p3_out_credits_feedback),
	.p4_out_credits_feedback(r5_p4_out_credits_feedback)
);

//declare router6
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_6;
	assign router_id_6 = 6;
	m_Assembled_Router_faulty	router_6(
	.router_id(router_id_6),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r6_p0_in_data),
	.p0_out_data(r6_p0_out_data),
	.p0_in_data_valid(r6_p0_in_data_valid),
	.p0_out_data_valid(r6_p0_out_data_valid),
	.p0_in_credits_feedback(r6_p0_in_credits_feedback),
	.p0_out_credits_feedback(r6_p0_out_credits_feedback),

//port_1
	.p1_in_data(r2_p2_out_data),
	.p1_out_data(r6_p1_out_data),
	.p1_in_data_valid(r2_p2_out_data_valid),
	.p1_out_data_valid(r6_p1_out_data_valid),
	.p1_in_credits_feedback(r2_p2_out_credits_feedback),
	.p1_out_credits_feedback(r6_p1_out_credits_feedback),

//port_2
	.p2_in_data(r10_p1_out_data),
	.p2_out_data(r6_p2_out_data),
	.p2_in_data_valid(r10_p1_out_data_valid),
	.p2_out_data_valid(r6_p2_out_data_valid),
	.p2_in_credits_feedback(r10_p1_out_credits_feedback),
	.p2_out_credits_feedback(r6_p2_out_credits_feedback),

//port_3
	.p3_in_data(r5_p4_out_data),
	.p3_out_data(r6_p3_out_data),
	.p3_in_data_valid(r5_p4_out_data_valid),
	.p3_out_data_valid(r6_p3_out_data_valid),
	.p3_in_credits_feedback(r5_p4_out_credits_feedback),
	.p3_out_credits_feedback(r6_p3_out_credits_feedback),

//port_4
	.p4_in_data(r7_p3_out_data),
	.p4_out_data(r6_p4_out_data),
	.p4_in_data_valid(r7_p3_out_data_valid),
	.p4_out_data_valid(r6_p4_out_data_valid),
	.p4_in_credits_feedback(r7_p3_out_credits_feedback),
	.p4_out_credits_feedback(r6_p4_out_credits_feedback)
);

//declare router7
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_7;
	assign router_id_7 = 7;
	m_Assembled_Router_faulty	router_7(
	.router_id(router_id_7),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r7_p0_in_data),
	.p0_out_data(r7_p0_out_data),
	.p0_in_data_valid(r7_p0_in_data_valid),
	.p0_out_data_valid(r7_p0_out_data_valid),
	.p0_in_credits_feedback(r7_p0_in_credits_feedback),
	.p0_out_credits_feedback(r7_p0_out_credits_feedback),

//port_1
	.p1_in_data(r3_p2_out_data),
	.p1_out_data(r7_p1_out_data),
	.p1_in_data_valid(r3_p2_out_data_valid),
	.p1_out_data_valid(r7_p1_out_data_valid),
	.p1_in_credits_feedback(r3_p2_out_credits_feedback),
	.p1_out_credits_feedback(r7_p1_out_credits_feedback),

//port_2
	.p2_in_data(r11_p1_out_data),
	.p2_out_data(r7_p2_out_data),
	.p2_in_data_valid(r11_p1_out_data_valid),
	.p2_out_data_valid(r7_p2_out_data_valid),
	.p2_in_credits_feedback(r11_p1_out_credits_feedback),
	.p2_out_credits_feedback(r7_p2_out_credits_feedback),

//port_3
	.p3_in_data(r6_p4_out_data),
	.p3_out_data(r7_p3_out_data),
	.p3_in_data_valid(r6_p4_out_data_valid),
	.p3_out_data_valid(r7_p3_out_data_valid),
	.p3_in_credits_feedback(r6_p4_out_credits_feedback),
	.p3_out_credits_feedback(r7_p3_out_credits_feedback),

//port_4
	.p4_in_data(d_zero),
	.p4_out_data(),
	.p4_in_data_valid(1'b0),
	.p4_out_data_valid(),
	.p4_in_credits_feedback(cf_zero),
	.p4_out_credits_feedback()
);

//declare router8
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_8;
	assign router_id_8 = 8;
	m_Assembled_Router_faulty	router_8(
	.router_id(router_id_8),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r8_p0_in_data),
	.p0_out_data(r8_p0_out_data),
	.p0_in_data_valid(r8_p0_in_data_valid),
	.p0_out_data_valid(r8_p0_out_data_valid),
	.p0_in_credits_feedback(r8_p0_in_credits_feedback),
	.p0_out_credits_feedback(r8_p0_out_credits_feedback),

//port_1
	.p1_in_data(r4_p2_out_data),
	.p1_out_data(r8_p1_out_data),
	.p1_in_data_valid(r4_p2_out_data_valid),
	.p1_out_data_valid(r8_p1_out_data_valid),
	.p1_in_credits_feedback(r4_p2_out_credits_feedback),
	.p1_out_credits_feedback(r8_p1_out_credits_feedback),

//port_2
	.p2_in_data(r12_p1_out_data),
	.p2_out_data(r8_p2_out_data),
	.p2_in_data_valid(r12_p1_out_data_valid),
	.p2_out_data_valid(r8_p2_out_data_valid),
	.p2_in_credits_feedback(r12_p1_out_credits_feedback),
	.p2_out_credits_feedback(r8_p2_out_credits_feedback),

//port_3
	.p3_in_data(d_zero),
	.p3_out_data(),
	.p3_in_data_valid(1'b0),
	.p3_out_data_valid(),
	.p3_in_credits_feedback(cf_zero),
	.p3_out_credits_feedback(),

//port_4
	.p4_in_data(r9_p3_out_data),
	.p4_out_data(r8_p4_out_data),
	.p4_in_data_valid(r9_p3_out_data_valid),
	.p4_out_data_valid(r8_p4_out_data_valid),
	.p4_in_credits_feedback(r9_p3_out_credits_feedback),
	.p4_out_credits_feedback(r8_p4_out_credits_feedback)
);

//declare router9
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_9;
	assign router_id_9 = 9;
	m_Assembled_Router_faulty	router_9(
	.router_id(router_id_9),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r9_p0_in_data),
	.p0_out_data(r9_p0_out_data),
	.p0_in_data_valid(r9_p0_in_data_valid),
	.p0_out_data_valid(r9_p0_out_data_valid),
	.p0_in_credits_feedback(r9_p0_in_credits_feedback),
	.p0_out_credits_feedback(r9_p0_out_credits_feedback),

//port_1
	.p1_in_data(r5_p2_out_data),
	.p1_out_data(r9_p1_out_data),
	.p1_in_data_valid(r5_p2_out_data_valid),
	.p1_out_data_valid(r9_p1_out_data_valid),
	.p1_in_credits_feedback(r5_p2_out_credits_feedback),
	.p1_out_credits_feedback(r9_p1_out_credits_feedback),

//port_2
	.p2_in_data(r13_p1_out_data),
	.p2_out_data(r9_p2_out_data),
	.p2_in_data_valid(r13_p1_out_data_valid),
	.p2_out_data_valid(r9_p2_out_data_valid),
	.p2_in_credits_feedback(r13_p1_out_credits_feedback),
	.p2_out_credits_feedback(r9_p2_out_credits_feedback),

//port_3
	.p3_in_data(r8_p4_out_data),
	.p3_out_data(r9_p3_out_data),
	.p3_in_data_valid(r8_p4_out_data_valid),
	.p3_out_data_valid(r9_p3_out_data_valid),
	.p3_in_credits_feedback(r8_p4_out_credits_feedback),
	.p3_out_credits_feedback(r9_p3_out_credits_feedback),

//port_4
	.p4_in_data(r10_p3_out_data),
	.p4_out_data(r9_p4_out_data),
	.p4_in_data_valid(r10_p3_out_data_valid),
	.p4_out_data_valid(r9_p4_out_data_valid),
	.p4_in_credits_feedback(r10_p3_out_credits_feedback),
	.p4_out_credits_feedback(r9_p4_out_credits_feedback)
);

//declare router10
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_10;
	assign router_id_10 = 10;
	m_Assembled_Router_faulty	router_10(
	.router_id(router_id_10),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r10_p0_in_data),
	.p0_out_data(r10_p0_out_data),
	.p0_in_data_valid(r10_p0_in_data_valid),
	.p0_out_data_valid(r10_p0_out_data_valid),
	.p0_in_credits_feedback(r10_p0_in_credits_feedback),
	.p0_out_credits_feedback(r10_p0_out_credits_feedback),

//port_1
	.p1_in_data(r6_p2_out_data),
	.p1_out_data(r10_p1_out_data),
	.p1_in_data_valid(r6_p2_out_data_valid),
	.p1_out_data_valid(r10_p1_out_data_valid),
	.p1_in_credits_feedback(r6_p2_out_credits_feedback),
	.p1_out_credits_feedback(r10_p1_out_credits_feedback),

//port_2
	.p2_in_data(r14_p1_out_data),
	.p2_out_data(r10_p2_out_data),
	.p2_in_data_valid(r14_p1_out_data_valid),
	.p2_out_data_valid(r10_p2_out_data_valid),
	.p2_in_credits_feedback(r14_p1_out_credits_feedback),
	.p2_out_credits_feedback(r10_p2_out_credits_feedback),

//port_3
	.p3_in_data(r9_p4_out_data),
	.p3_out_data(r10_p3_out_data),
	.p3_in_data_valid(r9_p4_out_data_valid),
	.p3_out_data_valid(r10_p3_out_data_valid),
	.p3_in_credits_feedback(r9_p4_out_credits_feedback),
	.p3_out_credits_feedback(r10_p3_out_credits_feedback),

//port_4
	.p4_in_data(r11_p3_out_data),
	.p4_out_data(r10_p4_out_data),
	.p4_in_data_valid(r11_p3_out_data_valid),
	.p4_out_data_valid(r10_p4_out_data_valid),
	.p4_in_credits_feedback(r11_p3_out_credits_feedback),
	.p4_out_credits_feedback(r10_p4_out_credits_feedback)
);

//declare router11
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_11;
	assign router_id_11 = 11;
	m_Assembled_Router_faulty	router_11(
	.router_id(router_id_11),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r11_p0_in_data),
	.p0_out_data(r11_p0_out_data),
	.p0_in_data_valid(r11_p0_in_data_valid),
	.p0_out_data_valid(r11_p0_out_data_valid),
	.p0_in_credits_feedback(r11_p0_in_credits_feedback),
	.p0_out_credits_feedback(r11_p0_out_credits_feedback),

//port_1
	.p1_in_data(r7_p2_out_data),
	.p1_out_data(r11_p1_out_data),
	.p1_in_data_valid(r7_p2_out_data_valid),
	.p1_out_data_valid(r11_p1_out_data_valid),
	.p1_in_credits_feedback(r7_p2_out_credits_feedback),
	.p1_out_credits_feedback(r11_p1_out_credits_feedback),

//port_2
	.p2_in_data(r15_p1_out_data),
	.p2_out_data(r11_p2_out_data),
	.p2_in_data_valid(r15_p1_out_data_valid),
	.p2_out_data_valid(r11_p2_out_data_valid),
	.p2_in_credits_feedback(r15_p1_out_credits_feedback),
	.p2_out_credits_feedback(r11_p2_out_credits_feedback),

//port_3
	.p3_in_data(r10_p4_out_data),
	.p3_out_data(r11_p3_out_data),
	.p3_in_data_valid(r10_p4_out_data_valid),
	.p3_out_data_valid(r11_p3_out_data_valid),
	.p3_in_credits_feedback(r10_p4_out_credits_feedback),
	.p3_out_credits_feedback(r11_p3_out_credits_feedback),

//port_4
	.p4_in_data(d_zero),
	.p4_out_data(),
	.p4_in_data_valid(1'b0),
	.p4_out_data_valid(),
	.p4_in_credits_feedback(cf_zero),
	.p4_out_credits_feedback()
);

//declare router12
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_12;
	assign router_id_12 = 12;
	m_Assembled_Router_faulty	router_12(
	.router_id(router_id_12),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r12_p0_in_data),
	.p0_out_data(r12_p0_out_data),
	.p0_in_data_valid(r12_p0_in_data_valid),
	.p0_out_data_valid(r12_p0_out_data_valid),
	.p0_in_credits_feedback(r12_p0_in_credits_feedback),
	.p0_out_credits_feedback(r12_p0_out_credits_feedback),

//port_1
	.p1_in_data(r8_p2_out_data),
	.p1_out_data(r12_p1_out_data),
	.p1_in_data_valid(r8_p2_out_data_valid),
	.p1_out_data_valid(r12_p1_out_data_valid),
	.p1_in_credits_feedback(r8_p2_out_credits_feedback),
	.p1_out_credits_feedback(r12_p1_out_credits_feedback),

//port_2
	.p2_in_data(d_zero),
	.p2_out_data(),
	.p2_in_data_valid(1'b0),
	.p2_out_data_valid(),
	.p2_in_credits_feedback(cf_zero),
	.p2_out_credits_feedback(),

//port_3
	.p3_in_data(d_zero),
	.p3_out_data(),
	.p3_in_data_valid(1'b0),
	.p3_out_data_valid(),
	.p3_in_credits_feedback(cf_zero),
	.p3_out_credits_feedback(),

//port_4
	.p4_in_data(r13_p3_out_data),
	.p4_out_data(r12_p4_out_data),
	.p4_in_data_valid(r13_p3_out_data_valid),
	.p4_out_data_valid(r12_p4_out_data_valid),
	.p4_in_credits_feedback(r13_p3_out_credits_feedback),
	.p4_out_credits_feedback(r12_p4_out_credits_feedback)
);

//declare router13
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_13;
	assign router_id_13 = 13;
	m_Assembled_Router_faulty	router_13(
	.router_id(router_id_13),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r13_p0_in_data),
	.p0_out_data(r13_p0_out_data),
	.p0_in_data_valid(r13_p0_in_data_valid),
	.p0_out_data_valid(r13_p0_out_data_valid),
	.p0_in_credits_feedback(r13_p0_in_credits_feedback),
	.p0_out_credits_feedback(r13_p0_out_credits_feedback),

//port_1
	.p1_in_data(r9_p2_out_data),
	.p1_out_data(r13_p1_out_data),
	.p1_in_data_valid(r9_p2_out_data_valid),
	.p1_out_data_valid(r13_p1_out_data_valid),
	.p1_in_credits_feedback(r9_p2_out_credits_feedback),
	.p1_out_credits_feedback(r13_p1_out_credits_feedback),

//port_2
	.p2_in_data(d_zero),
	.p2_out_data(),
	.p2_in_data_valid(1'b0),
	.p2_out_data_valid(),
	.p2_in_credits_feedback(cf_zero),
	.p2_out_credits_feedback(),

//port_3
	.p3_in_data(r12_p4_out_data),
	.p3_out_data(r13_p3_out_data),
	.p3_in_data_valid(r12_p4_out_data_valid),
	.p3_out_data_valid(r13_p3_out_data_valid),
	.p3_in_credits_feedback(r12_p4_out_credits_feedback),
	.p3_out_credits_feedback(r13_p3_out_credits_feedback),

//port_4
	.p4_in_data(r14_p3_out_data),
	.p4_out_data(r13_p4_out_data),
	.p4_in_data_valid(r14_p3_out_data_valid),
	.p4_out_data_valid(r13_p4_out_data_valid),
	.p4_in_credits_feedback(r14_p3_out_credits_feedback),
	.p4_out_credits_feedback(r13_p4_out_credits_feedback)
);

//declare router14
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_14;
	assign router_id_14 = 14;
	m_Assembled_Router_faulty	router_14(
	.router_id(router_id_14),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r14_p0_in_data),
	.p0_out_data(r14_p0_out_data),
	.p0_in_data_valid(r14_p0_in_data_valid),
	.p0_out_data_valid(r14_p0_out_data_valid),
	.p0_in_credits_feedback(r14_p0_in_credits_feedback),
	.p0_out_credits_feedback(r14_p0_out_credits_feedback),

//port_1
	.p1_in_data(r10_p2_out_data),
	.p1_out_data(r14_p1_out_data),
	.p1_in_data_valid(r10_p2_out_data_valid),
	.p1_out_data_valid(r14_p1_out_data_valid),
	.p1_in_credits_feedback(r10_p2_out_credits_feedback),
	.p1_out_credits_feedback(r14_p1_out_credits_feedback),

//port_2
	.p2_in_data(d_zero),
	.p2_out_data(),
	.p2_in_data_valid(1'b0),
	.p2_out_data_valid(),
	.p2_in_credits_feedback(cf_zero),
	.p2_out_credits_feedback(),

//port_3
	.p3_in_data(r13_p4_out_data),
	.p3_out_data(r14_p3_out_data),
	.p3_in_data_valid(r13_p4_out_data_valid),
	.p3_out_data_valid(r14_p3_out_data_valid),
	.p3_in_credits_feedback(r13_p4_out_credits_feedback),
	.p3_out_credits_feedback(r14_p3_out_credits_feedback),

//port_4
	.p4_in_data(r15_p3_out_data),
	.p4_out_data(r14_p4_out_data),
	.p4_in_data_valid(r15_p3_out_data_valid),
	.p4_out_data_valid(r14_p4_out_data_valid),
	.p4_in_credits_feedback(r15_p3_out_credits_feedback),
	.p4_out_credits_feedback(r14_p4_out_credits_feedback)
);

//declare router15
	wire [`FLIT_SRC_WIDTH-1 : 0] router_id_15;
	assign router_id_15 = 15;
	m_Assembled_Router_faulty	router_15(
	.router_id(router_id_15),
	.RST(RST),
	.CLK(CLK),
	.faultClock(CLK),

	.p0_in_data(r15_p0_in_data),
	.p0_out_data(r15_p0_out_data),
	.p0_in_data_valid(r15_p0_in_data_valid),
	.p0_out_data_valid(r15_p0_out_data_valid),
	.p0_in_credits_feedback(r15_p0_in_credits_feedback),
	.p0_out_credits_feedback(r15_p0_out_credits_feedback),

//port_1
	.p1_in_data(r11_p2_out_data),
	.p1_out_data(r15_p1_out_data),
	.p1_in_data_valid(r11_p2_out_data_valid),
	.p1_out_data_valid(r15_p1_out_data_valid),
	.p1_in_credits_feedback(r11_p2_out_credits_feedback),
	.p1_out_credits_feedback(r15_p1_out_credits_feedback),

//port_2
	.p2_in_data(d_zero),
	.p2_out_data(),
	.p2_in_data_valid(1'b0),
	.p2_out_data_valid(),
	.p2_in_credits_feedback(cf_zero),
	.p2_out_credits_feedback(),

//port_3
	.p3_in_data(r14_p4_out_data),
	.p3_out_data(r15_p3_out_data),
	.p3_in_data_valid(r14_p4_out_data_valid),
	.p3_out_data_valid(r15_p3_out_data_valid),
	.p3_in_credits_feedback(r14_p4_out_credits_feedback),
	.p3_out_credits_feedback(r15_p3_out_credits_feedback),

//port_4
	.p4_in_data(d_zero),
	.p4_out_data(),
	.p4_in_data_valid(1'b0),
	.p4_out_data_valid(),
	.p4_in_credits_feedback(cf_zero),
	.p4_out_credits_feedback()
);

	endmodule
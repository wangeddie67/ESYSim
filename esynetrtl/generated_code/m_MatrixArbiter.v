

`timescale 1ns / 1ps
`include "parameters.v"


module m_MatrixArbiter (
		CLK,
		RST,

		request_vector,
		grant_vector
	);


	// Inputs and Outputs
	input CLK;
	input RST;

	input [`CHANNELS - 1 : 0] request_vector;
	output [`CHANNELS - 1 : 0] grant_vector;


	// Wire + Variables
	reg intern_SET;

	// Request_0
	wire request_0;
	// the wire grant_0 equals intern_00_and_out
	wire intern_0_or_out;
	wire intern_00_and_out;
	wire intern_01_and_out;
	wire intern_02_and_out;
	wire intern_03_and_out;
	wire intern_04_and_out;
	wire intern_01_subU_out;
	wire intern_02_subU_out;
	wire intern_03_subU_out;
	wire intern_04_subU_out;

	// Request_1
	wire request_1;
	// the wire grant_1 equals intern_11_and_out
	wire intern_1_or_out;
	wire intern_10_and_out;
	wire intern_11_and_out;
	wire intern_12_and_out;
	wire intern_13_and_out;
	wire intern_14_and_out;
	wire intern_10_subU_out;
	wire intern_12_subU_out;
	wire intern_13_subU_out;
	wire intern_14_subU_out;

	// Request_2
	wire request_2;
	// the wire grant_2 equals intern_22_and_out
	wire intern_2_or_out;
	wire intern_20_and_out;
	wire intern_21_and_out;
	wire intern_22_and_out;
	wire intern_23_and_out;
	wire intern_24_and_out;
	wire intern_20_subU_out;
	wire intern_21_subU_out;
	wire intern_23_subU_out;
	wire intern_24_subU_out;

	// Request_3
	wire request_3;
	// the wire grant_3 equals intern_33_and_out
	wire intern_3_or_out;
	wire intern_30_and_out;
	wire intern_31_and_out;
	wire intern_32_and_out;
	wire intern_33_and_out;
	wire intern_34_and_out;
	wire intern_30_subU_out;
	wire intern_31_subU_out;
	wire intern_32_subU_out;
	wire intern_34_subU_out;

	// Request_4
	wire request_4;
	// the wire grant_4 equals intern_44_and_out
	wire intern_4_or_out;
	wire intern_40_and_out;
	wire intern_41_and_out;
	wire intern_42_and_out;
	wire intern_43_and_out;
	wire intern_44_and_out;
	wire intern_40_subU_out;
	wire intern_41_subU_out;
	wire intern_42_subU_out;
	wire intern_43_subU_out;


	// Assignments
	assign request_0 = request_vector[0];
	assign request_1 = request_vector[1];
	assign request_2 = request_vector[2];
	assign request_3 = request_vector[3];
	assign request_4 = request_vector[4];

	assign grant_vector[0] = intern_00_and_out;
	assign grant_vector[1] = intern_11_and_out;
	assign grant_vector[2] = intern_22_and_out;
	assign grant_vector[3] = intern_33_and_out;
	assign grant_vector[4] = intern_44_and_out;

	// Request_0
	and and_00 (intern_00_and_out, request_0, ~intern_0_or_out);
	and and_01 (intern_01_and_out, request_0, intern_01_subU_out);
	and and_02 (intern_02_and_out, request_0, intern_02_subU_out);
	and and_03 (intern_03_and_out, request_0, intern_03_subU_out);
	and and_04 (intern_04_and_out, request_0, intern_04_subU_out);

	// Request_1
	and and_10 (intern_10_and_out, request_1, intern_10_subU_out);
	and and_11 (intern_11_and_out, request_1, ~intern_1_or_out);
	and and_12 (intern_12_and_out, request_1, intern_12_subU_out);
	and and_13 (intern_13_and_out, request_1, intern_13_subU_out);
	and and_14 (intern_14_and_out, request_1, intern_14_subU_out);

	// Request_2
	and and_20 (intern_20_and_out, request_2, intern_20_subU_out);
	and and_21 (intern_21_and_out, request_2, intern_21_subU_out);
	and and_22 (intern_22_and_out, request_2, ~intern_2_or_out);
	and and_23 (intern_23_and_out, request_2, intern_23_subU_out);
	and and_24 (intern_24_and_out, request_2, intern_24_subU_out);

	// Request_3
	and and_30 (intern_30_and_out, request_3, intern_30_subU_out);
	and and_31 (intern_31_and_out, request_3, intern_31_subU_out);
	and and_32 (intern_32_and_out, request_3, intern_32_subU_out);
	and and_33 (intern_33_and_out, request_3, ~intern_3_or_out);
	and and_34 (intern_34_and_out, request_3, intern_34_subU_out);

	// Request_4
	and and_40 (intern_40_and_out, request_4, intern_40_subU_out);
	and and_41 (intern_41_and_out, request_4, intern_41_subU_out);
	and and_42 (intern_42_and_out, request_4, intern_42_subU_out);
	and and_43 (intern_43_and_out, request_4, intern_43_subU_out);
	and and_44 (intern_44_and_out, request_4, ~intern_4_or_out);

	or or_0 (intern_0_or_out 
		,intern_10_and_out 
		,intern_20_and_out 
		,intern_30_and_out 
		,intern_40_and_out 
	);
	or or_1 (intern_1_or_out 
		,intern_01_and_out 
		,intern_21_and_out 
		,intern_31_and_out 
		,intern_41_and_out 
	);
	or or_2 (intern_2_or_out 
		,intern_02_and_out 
		,intern_12_and_out 
		,intern_32_and_out 
		,intern_42_and_out 
	);
	or or_3 (intern_3_or_out 
		,intern_03_and_out 
		,intern_13_and_out 
		,intern_23_and_out 
		,intern_43_and_out 
	);
	or or_4 (intern_4_or_out 
		,intern_04_and_out 
		,intern_14_and_out 
		,intern_24_and_out 
		,intern_34_and_out 
	);


	// Behavior
	always @ (RST) begin
		if (RST) begin
			intern_SET = 1;
		end else begin
			intern_SET = 0;
		end
	end


	// Module Decleration
	// Row: 0 Column: 0 NO SUBUNIT AT THIS POSITION
	// Row: 0 Column: 1 SubUnit: 01
	m_MatrixArbiter_subUnit subU_01 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_11_and_out)
		,.g_in_2(intern_00_and_out)
		,.w_out(intern_01_subU_out)
	);
	// Row: 0 Column: 2 SubUnit: 02
	m_MatrixArbiter_subUnit subU_02 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_22_and_out)
		,.g_in_2(intern_00_and_out)
		,.w_out(intern_02_subU_out)
	);
	// Row: 0 Column: 3 SubUnit: 03
	m_MatrixArbiter_subUnit subU_03 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_33_and_out)
		,.g_in_2(intern_00_and_out)
		,.w_out(intern_03_subU_out)
	);
	// Row: 0 Column: 4 SubUnit: 04
	m_MatrixArbiter_subUnit subU_04 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_44_and_out)
		,.g_in_2(intern_00_and_out)
		,.w_out(intern_04_subU_out)
	);
	// Row: 1 Column: 0 SubUnit: 10
	m_MatrixArbiter_subUnit subU_10 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_00_and_out)
		,.g_in_2(intern_11_and_out)
		,.w_out(intern_10_subU_out)
	);
	// Row: 1 Column: 1 NO SUBUNIT AT THIS POSITION
	// Row: 1 Column: 2 SubUnit: 12
	m_MatrixArbiter_subUnit subU_12 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_22_and_out)
		,.g_in_2(intern_11_and_out)
		,.w_out(intern_12_subU_out)
	);
	// Row: 1 Column: 3 SubUnit: 13
	m_MatrixArbiter_subUnit subU_13 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_33_and_out)
		,.g_in_2(intern_11_and_out)
		,.w_out(intern_13_subU_out)
	);
	// Row: 1 Column: 4 SubUnit: 14
	m_MatrixArbiter_subUnit subU_14 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_44_and_out)
		,.g_in_2(intern_11_and_out)
		,.w_out(intern_14_subU_out)
	);
	// Row: 2 Column: 0 SubUnit: 20
	m_MatrixArbiter_subUnit subU_20 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_00_and_out)
		,.g_in_2(intern_22_and_out)
		,.w_out(intern_20_subU_out)
	);
	// Row: 2 Column: 1 SubUnit: 21
	m_MatrixArbiter_subUnit subU_21 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_11_and_out)
		,.g_in_2(intern_22_and_out)
		,.w_out(intern_21_subU_out)
	);
	// Row: 2 Column: 2 NO SUBUNIT AT THIS POSITION
	// Row: 2 Column: 3 SubUnit: 23
	m_MatrixArbiter_subUnit subU_23 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_33_and_out)
		,.g_in_2(intern_22_and_out)
		,.w_out(intern_23_subU_out)
	);
	// Row: 2 Column: 4 SubUnit: 24
	m_MatrixArbiter_subUnit subU_24 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_44_and_out)
		,.g_in_2(intern_22_and_out)
		,.w_out(intern_24_subU_out)
	);
	// Row: 3 Column: 0 SubUnit: 30
	m_MatrixArbiter_subUnit subU_30 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_00_and_out)
		,.g_in_2(intern_33_and_out)
		,.w_out(intern_30_subU_out)
	);
	// Row: 3 Column: 1 SubUnit: 31
	m_MatrixArbiter_subUnit subU_31 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_11_and_out)
		,.g_in_2(intern_33_and_out)
		,.w_out(intern_31_subU_out)
	);
	// Row: 3 Column: 2 SubUnit: 32
	m_MatrixArbiter_subUnit subU_32 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_22_and_out)
		,.g_in_2(intern_33_and_out)
		,.w_out(intern_32_subU_out)
	);
	// Row: 3 Column: 3 NO SUBUNIT AT THIS POSITION
	// Row: 3 Column: 4 SubUnit: 34
	m_MatrixArbiter_subUnit subU_34 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_44_and_out)
		,.g_in_2(intern_33_and_out)
		,.w_out(intern_34_subU_out)
	);
	// Row: 4 Column: 0 SubUnit: 40
	m_MatrixArbiter_subUnit subU_40 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_00_and_out)
		,.g_in_2(intern_44_and_out)
		,.w_out(intern_40_subU_out)
	);
	// Row: 4 Column: 1 SubUnit: 41
	m_MatrixArbiter_subUnit subU_41 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_11_and_out)
		,.g_in_2(intern_44_and_out)
		,.w_out(intern_41_subU_out)
	);
	// Row: 4 Column: 2 SubUnit: 42
	m_MatrixArbiter_subUnit subU_42 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_22_and_out)
		,.g_in_2(intern_44_and_out)
		,.w_out(intern_42_subU_out)
	);
	// Row: 4 Column: 3 SubUnit: 43
	m_MatrixArbiter_subUnit subU_43 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_33_and_out)
		,.g_in_2(intern_44_and_out)
		,.w_out(intern_43_subU_out)
	);
	// Row: 4 Column: 4 NO SUBUNIT AT THIS POSITION


endmodule

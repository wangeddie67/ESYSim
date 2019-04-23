
`timescale 1ns / 1ps
`include "parameters.v"


module m_RR_MatrixArbiter (
		CLK,
		RST,

		request_vector,
		grant_vector
	);


	// Inputs and Outputs
	input CLK;
	input RST;

	input [2 - 1 : 0] request_vector;
	output [2 - 1 : 0] grant_vector;


	// Wire + Variables
	reg intern_SET;

	// Request_0
	wire request_0;
	// the wire grant_0 equals intern_00_and_out
	wire intern_0_or_out;
	wire intern_00_and_out;
	wire intern_01_and_out;
	wire intern_01_subU_out;

	// Request_1
	wire request_1;
	// the wire grant_1 equals intern_11_and_out
	wire intern_1_or_out;
	wire intern_10_and_out;
	wire intern_11_and_out;
	wire intern_10_subU_out;


	// Assignments
	assign request_0 = request_vector[0];
	assign request_1 = request_vector[1];

	assign grant_vector[0] = intern_00_and_out;
	assign grant_vector[1] = intern_11_and_out;

	// Request_0
	and and_00 (intern_00_and_out, request_0, ~intern_0_or_out);
	and and_01 (intern_01_and_out, request_0, intern_01_subU_out);

	// Request_1
	and and_10 (intern_10_and_out, request_1, intern_10_subU_out);
	and and_11 (intern_11_and_out, request_1, ~intern_1_or_out);

	or or_0 (intern_0_or_out 
		,intern_10_and_out 
	);
	or or_1 (intern_1_or_out 
		,intern_01_and_out 
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


endmodule

    
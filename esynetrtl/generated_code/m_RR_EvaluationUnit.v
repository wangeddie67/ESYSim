
`timescale 1ns / 1ps

`include "parameters.v"

module m_RR_EvaluationUnit(
	RST,
	CLK,
	request_0,
	request_1,
	grant_0,
	grant_1
);

	// Parameters
	parameter	P_ROUTER_ID = 0;
	parameter	P_CHANNEL_ID = 0;

	// Port Definition
	input RST;
	input CLK;
	input [`CHANNELS - 1 : 0] request_0;
	input [`CHANNELS - 1 : 0] request_1;
	output grant_0;
	output grant_1;

	// Variables	 
	wire [2 - 1 : 0] request_vector;
	wire [2 - 1 : 0] grant_vector;
	
	// Assignments
	// Assignments for matrixArbiter inputs
	assign request_vector[0] = (request_0 != 0);
	assign request_vector[1] = (request_1 != 0);
	// Assignments for matrixArbiter outputs
	assign grant_0 = grant_vector[0];
	assign grant_1 = grant_vector[1];

	
	// Behavior
	initial begin:Initialization
		// Empty
	end

	// Module Decleration
	m_RR_MatrixArbiter rr_matrix(
		.RST(RST)
		,.CLK(CLK)

		,.request_vector(request_vector)
		,.grant_vector(grant_vector)
	);

	
endmodule

    
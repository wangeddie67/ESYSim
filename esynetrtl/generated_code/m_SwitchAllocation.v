`timescale 1ns / 1ps
`include "parameters.v"

module m_SwitchAllocation (
	input enable_p0,
	input [`CHANNELS - 1 : 0] request_p0,
	output grant_p0,

	input enable_p1,
	input [`CHANNELS - 1 : 0] request_p1,
	output grant_p1,

	input enable_p2,
	input [`CHANNELS - 1 : 0] request_p2,
	output grant_p2,

	input enable_p3,
	input [`CHANNELS - 1 : 0] request_p3,
	output grant_p3,

	input enable_p4,
	input [`CHANNELS - 1 : 0] request_p4,
	output grant_p4,

	input CLK,
	input RST
);

	// Parameters
	parameter	P_ROUTER_ID = 0;

	wire [`CHANNELS - 1 : 0] enable_vector; 
	wire [`CHANNELS - 1 : 0] request_vector [`CHANNELS - 1 : 0];
	wire [`CHANNELS - 1 : 0] grant_vector[`CHANNELS - 1 : 0];
	wire [`CHANNELS - 1 : 0] matrixHold_vector; 

//port0 assignment :
	assign enable_vector[`P0] = enable_p0;

	assign request_vector[`P0] = request_p0;

	assign grant_p0 = 
	grant_vector[`P4][`P0] ^
	grant_vector[`P3][`P0] ^
	grant_vector[`P2][`P0] ^
	grant_vector[`P1][`P0] ^
	grant_vector[`P0][`P0] ;


//port1 assignment :
	assign enable_vector[`P1] = enable_p1;

	assign request_vector[`P1] = request_p1;

	assign grant_p1 = 
	grant_vector[`P4][`P1] ^
	grant_vector[`P3][`P1] ^
	grant_vector[`P2][`P1] ^
	grant_vector[`P1][`P1] ^
	grant_vector[`P0][`P1] ;


//port2 assignment :
	assign enable_vector[`P2] = enable_p2;

	assign request_vector[`P2] = request_p2;

	assign grant_p2 = 
	grant_vector[`P4][`P2] ^
	grant_vector[`P3][`P2] ^
	grant_vector[`P2][`P2] ^
	grant_vector[`P1][`P2] ^
	grant_vector[`P0][`P2] ;


//port3 assignment :
	assign enable_vector[`P3] = enable_p3;

	assign request_vector[`P3] = request_p3;

	assign grant_p3 = 
	grant_vector[`P4][`P3] ^
	grant_vector[`P3][`P3] ^
	grant_vector[`P2][`P3] ^
	grant_vector[`P1][`P3] ^
	grant_vector[`P0][`P3] ;


//port4 assignment :
	assign enable_vector[`P4] = enable_p4;

	assign request_vector[`P4] = request_p4;

	assign grant_p4 = 
	grant_vector[`P4][`P4] ^
	grant_vector[`P3][`P4] ^
	grant_vector[`P2][`P4] ^
	grant_vector[`P1][`P4] ^
	grant_vector[`P0][`P4] ;




	// Generating-Block: Generates MatrixArbiter; 1 per Out Channel
	genvar i_var;
	generate
	for (i_var = 0; i_var < `CHANNELS; i_var = i_var + 1) begin:GENERATINGEVALUATION
	m_EvaluationUnit # (
	//.P_ROUTER_ID(P_ROUTER_ID),
	.P_CHANNEL_ID(i_var)
	) evalUnit (
	.enables({enable_vector[`P4],enable_vector[`P3],enable_vector[`P2],enable_vector[`P1],enable_vector[`P0]}),
	.requests({request_vector[`P4][i_var],request_vector[`P3][i_var],request_vector[`P2][i_var],request_vector[`P1][i_var],request_vector[`P0][i_var]}),
	.grants(grant_vector[i_var]),
	.matrixHold(matrixHold_vector[i_var]),

	.RST(RST),
	.CLK(CLK)
);

		end
	endgenerate

	endmodule

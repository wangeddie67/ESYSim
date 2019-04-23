
	`timescale 1ns / 1ps

	`include "parameters.v"

	module m_EvaluationUnit(
		RST,
		CLK,
		
		enables,
		requests,
		grants,
		matrixHold
	);

	// Parameters
	parameter	P_ROUTER_ID = 0;
	parameter	P_CHANNEL_ID = 0;

	// Port Definition
	input RST;
	input CLK;
		
	input [`CHANNELS - 1 : 0] enables;
	input [`CHANNELS - 1 : 0] requests;
	output [`CHANNELS - 1 : 0] grants;
	output matrixHold;

	// Variables
	wire [`CHANNELS - 1 : 0] evaluationStatus; 	
	wire [`CHANNELS - 1 : 0] request_vector; 
		// Assignments
	assign evaluationStatus[`P0]= ~enables[`P0] & requests[`P0];
	assign evaluationStatus[`P1]= ~enables[`P1] & requests[`P1];
	assign evaluationStatus[`P2]= ~enables[`P2] & requests[`P2];
	assign evaluationStatus[`P3]= ~enables[`P3] & requests[`P3];
	assign evaluationStatus[`P4]= ~enables[`P4] & requests[`P4];
	
	assign matrixHold = 
	evaluationStatus[`P0] ^
	evaluationStatus[`P1] ^
	evaluationStatus[`P2] ^
	evaluationStatus[`P3] ^
	evaluationStatus[`P4];

	assign request_vector = (matrixHold) ? evaluationStatus : requests;
	
	// Behavior
	initial begin:Initialization
		// Empty
	end

	// Module Decleration
	m_MatrixArbiter arbiter (
		.CLK(CLK)
		,.RST(RST)
		
		,.request_vector(request_vector)
		,.grant_vector(grants)
	);

	endmodule		
	
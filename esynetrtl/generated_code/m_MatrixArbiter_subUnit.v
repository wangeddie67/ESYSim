
`timescale 1ns / 1ps

`include "parameters.v"

module m_MatrixArbiter_subUnit(
		RST,
		CLK,
		SET,
		
		g_in_1,
		g_in_2,
		
		w_out
	);
	
	// Parameters

	// Port Definition
	input RST;
	input CLK;
	input SET;
	
	input g_in_1;
	input g_in_2;
	
	output w_out;
	
	
	// Variables	 
	wire or_out;
	wire and_out;

	// Assignments
	or subUnit_or (or_out, g_in_1, w_out);
	and subUnit_and (and_out, ~g_in_2, or_out);

	// Behavior
	initial begin:Initialization
		// Empty
	end
	
	// Module Decleration
	m_D_FF_mod dFF (
		.CLK(CLK)
		,.RST(RST)
		,.SET(SET)
		,.D(and_out)
		,.Q(w_out)
	);
	
endmodule
    
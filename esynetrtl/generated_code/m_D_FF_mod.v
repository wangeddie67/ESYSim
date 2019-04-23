
`timescale 1ns / 1ps

`include "parameters.v"

module m_D_FF_mod(
		CLK,
		RST,
		SET,
		D,
		Q
	);

	// Parameters
	
	// Port Definition
	input CLK;
	input RST;
	input SET;
	input D;
	output reg Q;
	
	// Variables	 
	
	// Assignments
	
	// Behavior
	initial begin:Initialization
		// Empty
	end
	
	
	// if not set or reset, d will be hold/displayed for one cycle at the outport q
	always @ (posedge RST or posedge  SET or posedge CLK) begin
		if (SET) begin
			Q <= 1;
		end else if (RST) begin
			Q <= 0;
		end else begin
			Q <= D;
		end
	end

endmodule
    
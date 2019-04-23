
`timescale 1ns / 1ps

`include "parameters.v"

module m_D_FF_sync(
		CLK,
		RST,
		D,
		Q
	);

	// Parameters
	parameter P_DATA_WIDTH = 1; 
	
	// Port Definition
	input CLK;
	input RST;
	input [P_DATA_WIDTH - 1 : 0] D;
	output reg [P_DATA_WIDTH - 1 : 0] Q;
	
	// Variables	 
	
	// Assignments
	
	// Behavior
	initial begin:Initialization
		// Empty
	end
	
	
	// if not reset, D will be hold/displayed for one cycle at the outport Q
	always @ (posedge RST or posedge CLK) begin
		if (RST) begin
			Q <= 0;
		end else begin
			Q <= D;
		end
	end

endmodule
    
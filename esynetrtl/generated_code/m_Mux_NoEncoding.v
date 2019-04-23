

`timescale 1ns / 1ps


`include "parameters.v"
module m_Mux_NoEncoding(
		select,
		data_out,
		data_in_0,
		data_in_1,
		data_in_2,
		data_in_3,
		data_in_4,
		data_in_5
	);

	// Parameters
	parameter P_DATA_WIDTH = 8;

	// Port Definitions
	input [5 - 1 : 0] select;
	input [P_DATA_WIDTH - 1 : 0] data_in_0;
	input [P_DATA_WIDTH - 1 : 0] data_in_1;
	input [P_DATA_WIDTH - 1 : 0] data_in_2;
	input [P_DATA_WIDTH - 1 : 0] data_in_3;
	input [P_DATA_WIDTH - 1 : 0] data_in_4;
	input [P_DATA_WIDTH - 1 : 0] data_in_5;
	output reg [P_DATA_WIDTH - 1 : 0] data_out;

	// Variables

	// Assignments

	// Behavior
	initial begin:Initialization
		// Empty 
	end

	// Switch block: Puts the selected in data to outport
	always @ (select 
		or data_in_0
		or data_in_1
		or data_in_2
		or data_in_3
		or data_in_4
		or data_in_5
	) begin:MUX
		case (select)
			5'b00001 : data_out = data_in_0;
			5'b00010 : data_out = data_in_1;
			5'b00100 : data_out = data_in_2;
			5'b01000 : data_out = data_in_3;
			5'b10000 : data_out = data_in_4;
			// In "IDLE" position selector == 0 -> "last" port is selected which should be connected to 0
			5'b00000 : data_out = data_in_5;
			default: data_out = 0;
		endcase
	end

endmodule

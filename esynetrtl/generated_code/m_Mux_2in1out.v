
`timescale 1ns / 1ps

`include "parameters.v"

module m_Mux_2in1out(
	select,
	data_in_0,
	data_in_1,
	data_out
	);

	
	// Parameters
	parameter P_DATA_WIDTH = 8; 
		
	// Port Definition
	input select;
	
	input [P_DATA_WIDTH - 1 : 0] data_in_0;
	input [P_DATA_WIDTH - 1 : 0] data_in_1;

	output reg [P_DATA_WIDTH - 1 : 0] data_out;

	// Variables	

	// Assignments
	//assign data_out = (select) ? data_in_0 : data_in_1;

	// Behavior
	initial begin:Initialization
		// Empty 
	end

	// Switch block: Puts the selected in data to outport
	always @ (select 
						or data_in_0
						or data_in_1 
					) begin:MUX
		case (select)
			0 : 	data_out = data_in_0;
			1 : 	data_out = data_in_1;
			default: data_out = 0;
		endcase
	end	

endmodule
    
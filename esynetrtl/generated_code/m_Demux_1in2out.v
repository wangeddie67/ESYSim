
`timescale 1ns / 1ps

`include "parameters.v"

module m_Demux_1in2out(
		select,
		data_in,
		data_out_0,
		data_out_1
	);

	
	// Parameters
	parameter P_DATA_WIDTH = 8; 
		
	// Port Definition
	input	select;
	input [P_DATA_WIDTH - 1 : 0] data_in;

	output reg [P_DATA_WIDTH - 1 : 0] data_out_0;
	output reg [P_DATA_WIDTH - 1 : 0] data_out_1;

	// Variables	

	// Assignments

	// Behavior
	initial begin:Initialization
		// Empty 
	end

	// Depending on the select, the data_in will be switched to data_out_0 or data_out_1
	always @ (select or data_in) begin:DEMUX
		case (select)
			0 : 	begin
				data_out_0 = data_in;
				data_out_1 = 0;				// Sets other output to zero
			end 
			1 : 	begin
				data_out_0 = 0;
				data_out_1 = data_in;
			end
		endcase
	end

endmodule
    
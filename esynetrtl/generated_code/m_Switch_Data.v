

`timescale 1ns / 1ps


`include "parameters.v"
module m_Switch_Data(
		sel_0,
		sel_1,
		sel_2,
		sel_3,
		sel_4,

		data_in_0,
		data_in_1,
		data_in_2,
		data_in_3,
		data_in_4,

		data_out_0,
		data_out_1,
		data_out_2,
		data_out_3,
		data_out_4
	);

	// Parameters
	parameter P_DATA_WIDTH = 8;

	// Port Definitions
	input [5 - 1 : 0] sel_0;
	input [5 - 1 : 0] sel_1;
	input [5 - 1 : 0] sel_2;
	input [5 - 1 : 0] sel_3;
	input [5 - 1 : 0] sel_4;

	input [P_DATA_WIDTH - 1 : 0] data_in_0;
	input [P_DATA_WIDTH - 1 : 0] data_in_1;
	input [P_DATA_WIDTH - 1 : 0] data_in_2;
	input [P_DATA_WIDTH - 1 : 0] data_in_3;
	input [P_DATA_WIDTH - 1 : 0] data_in_4;

	output [P_DATA_WIDTH - 1 : 0] data_out_0;
	output [P_DATA_WIDTH - 1 : 0] data_out_1;
	output [P_DATA_WIDTH - 1 : 0] data_out_2;
	output [P_DATA_WIDTH - 1 : 0] data_out_3;
	output [P_DATA_WIDTH - 1 : 0] data_out_4;

	// Variables
	wire [5 - 1 : 0] select [5 - 1 : 0];
	wire [P_DATA_WIDTH - 1 : 0] data_in [5 - 1 : 0];
	wire [P_DATA_WIDTH - 1 : 0] data_out [5 - 1 : 0];
	wire [P_DATA_WIDTH - 1 : 0] const_0;

	// Assignments
	assign select[0]=sel_0;
	assign select[1]=sel_1;
	assign select[2]=sel_2;
	assign select[3]=sel_3;
	assign select[4]=sel_4;

	assign data_in[0]=data_in_0;
	assign data_in[1]=data_in_1;
	assign data_in[2]=data_in_2;
	assign data_in[3]=data_in_3;
	assign data_in[4]=data_in_4;

	assign data_out_0=data_out[0];
	assign data_out_1=data_out[1];
	assign data_out_2=data_out[2];
	assign data_out_3=data_out[3];
	assign data_out_4=data_out[4];

	assign const_0 = 0;

	// Behavior
	initial begin:Initialization
		// Empty 
	end

	// Generating-Block: Generates MUX for switching the feedback
	// The select is directly controlled by the State machine
	genvar i;
	generate
		for (i = 0; i < 5; i = i + 1) begin:GENERATINGMUX
			m_Mux_NoEncoding
			# (
				.P_DATA_WIDTH(P_DATA_WIDTH)
			) mux_data (
				.select(select[i])
				,.data_out(data_out[i])
				,.data_in_0(data_in[0])
				,.data_in_1(data_in[1])
				,.data_in_2(data_in[2])
				,.data_in_3(data_in[3])
				,.data_in_4(data_in[4])
				,.data_in_5(const_0)
			);
		end
	endgenerate

endmodule

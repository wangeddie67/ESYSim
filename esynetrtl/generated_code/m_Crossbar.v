

`timescale 1ns / 1ps


`include "parameters.v"
module m_Crossbar(
		request_0,
		request_1,
		request_2,
		request_3,
		request_4,

		data_in_0,
		data_in_1,
		data_in_2,
		data_in_3,
		data_in_4,

		data_out_0,
		data_out_1,
		data_out_2,
		data_out_3,
		data_out_4,

		feedback_in_0,
		feedback_in_1,
		feedback_in_2,
		feedback_in_3,
		feedback_in_4,

		feedback_out_0,
		feedback_out_1,
		feedback_out_2,
		feedback_out_3,
		feedback_out_4
	);

	// Parameters
	parameter P_DATA_WIDTH = `FLIT_WIDTH + `DATA_VALID_WIDTH;
	parameter P_FEEDBACK_WIDTH = `BUFFERSIZE_WIDTH;

	// Port Definitions
	input [5 - 1 : 0] request_0;
	input [5 - 1 : 0] request_1;
	input [5 - 1 : 0] request_2;
	input [5 - 1 : 0] request_3;
	input [5 - 1 : 0] request_4;

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

	input [P_FEEDBACK_WIDTH - 1 : 0] feedback_in_0;
	input [P_FEEDBACK_WIDTH - 1 : 0] feedback_in_1;
	input [P_FEEDBACK_WIDTH - 1 : 0] feedback_in_2;
	input [P_FEEDBACK_WIDTH - 1 : 0] feedback_in_3;
	input [P_FEEDBACK_WIDTH - 1 : 0] feedback_in_4;

	output [P_FEEDBACK_WIDTH - 1 : 0] feedback_out_0;
	output [P_FEEDBACK_WIDTH - 1 : 0] feedback_out_1;
	output [P_FEEDBACK_WIDTH - 1 : 0] feedback_out_2;
	output [P_FEEDBACK_WIDTH - 1 : 0] feedback_out_3;
	output [P_FEEDBACK_WIDTH - 1 : 0] feedback_out_4;

	// Variables
	wire [5 - 1 : 0] request [5 - 1 : 0];
	reg [5 - 1 : 0] select [5 - 1 : 0];

	// Assignments
	assign request[0] = request_0;
	assign request[1] = request_1;
	assign request[2] = request_2;
	assign request[3] = request_3;
	assign request[4] = request_4;

	// Behavior
	initial begin:Initialization
		// Empty 
	end

	// Data Logic - Mux
	genvar j_var;
	generate
		for(j_var = 0; j_var < 5; j_var = j_var + 1) begin:LOGIC
			// This piece of code looks at all request lines and if one shows "its" ID  it selects it, this assumes that SA workes without faults
			always @ (
				request[0] or
				request[1] or
				request[2] or
				request[3] or
				request[4]
			) begin:MUX_LOGIC
				if(request[0] == (5'b00001 << j_var)) begin
					select[j_var] = 5'b00001;
				end else if(request[1] == (5'b00001 << j_var)) begin
					select[j_var] = 5'b00010;
				end else if(request[2] == (5'b00001 << j_var)) begin
					select[j_var] = 5'b00100;
				end else if(request[3] == (5'b00001 << j_var)) begin
					select[j_var] = 5'b01000;
				end else if(request[4] == (5'b00001 << j_var)) begin
					select[j_var] = 5'b10000;
				end else begin
					select[j_var] = 5'b00000;
				end
			end
		end
	endgenerate

	// Module Initialization
	m_Switch_Data
	#(
		.P_DATA_WIDTH(P_DATA_WIDTH)
	) dataSwitch (
		.sel_0(select[0])
		,.sel_1(select[1])
		,.sel_2(select[2])
		,.sel_3(select[3])
		,.sel_4(select[4])

		,.data_in_0(data_in_0)
		,.data_in_1(data_in_1)
		,.data_in_2(data_in_2)
		,.data_in_3(data_in_3)
		,.data_in_4(data_in_4)

		,.data_out_0(data_out_0)
		,.data_out_1(data_out_1)
		,.data_out_2(data_out_2)
		,.data_out_3(data_out_3)
		,.data_out_4(data_out_4)
	);

	m_Switch_Feedback
	#(
		.P_FEEDBACK_WIDTH(P_FEEDBACK_WIDTH)
	) feedbackSwitch (
		.sel_0(request[0])
		,.sel_1(request[1])
		,.sel_2(request[2])
		,.sel_3(request[3])
		,.sel_4(request[4])

		,.feedback_in_0(feedback_in_0)
		,.feedback_in_1(feedback_in_1)
		,.feedback_in_2(feedback_in_2)
		,.feedback_in_3(feedback_in_3)
		,.feedback_in_4(feedback_in_4)

		,.feedback_out_0(feedback_out_0)
		,.feedback_out_1(feedback_out_1)
		,.feedback_out_2(feedback_out_2)
		,.feedback_out_3(feedback_out_3)
		,.feedback_out_4(feedback_out_4)
	);

endmodule

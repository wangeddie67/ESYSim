
`timescale 1ns / 1ps

`include "parameters.v"

module m_RoutingCalculation(
		RST,
		enable,

		local_in,
		
		src_in,
		dst_in,

		credits_0,
		credits_1,
		credits_2,
		credits_6,
		credits_3,
		credits_4,
		credits_5,

		success,
		result
		);

	// Port Definition
	input RST;
	input enable;
	
	input [`FLIT_SRC_WIDTH - 1 : 0] local_in;

	input [`FLIT_SRC_WIDTH - 1 : 0] src_in;
	input [`FLIT_DST_WIDTH - 1 : 0] dst_in;
	
	// The width is not - 1, so the range from 0 till BUFFERSIZE is provided
	input [`BUFFERSIZE_WIDTH - 1: 0] credits_0;	
	input [`BUFFERSIZE_WIDTH - 1: 0] credits_1;	
	input [`BUFFERSIZE_WIDTH - 1: 0] credits_2;	
	input [`BUFFERSIZE_WIDTH - 1: 0] credits_6;
	input [`BUFFERSIZE_WIDTH - 1: 0] credits_3;
	input [`BUFFERSIZE_WIDTH - 1: 0] credits_4;
	input [`BUFFERSIZE_WIDTH - 1: 0] credits_5;
	
	output reg success;
	output reg [`CHANNELS - 1:0] result;

	// Parameters
	parameter 	P_LOCAL_ID = 0;
	parameter		P_LOCAL_CHANNEL = 0;		// e.g. 0, 1, 2, 3,...
    
	parameter 	choose_NONE 	= 7'b0000000;
	parameter 	choose_0 		= 7'b0000001;
	parameter 	choose_1 		= 7'b0000010;
	parameter 	choose_2 		= 7'b0000100;
	parameter 	choose_3 		= 7'b0001000;
	parameter 	choose_4 		= 7'b0010000;
	parameter 	choose_5 		= 7'b0100000;
	parameter 	choose_6 		= 7'b1000000;
    
	// Variables
	wire [(`FLIT_SRC_WIDTH/2) -1 : 0] local_X;	// LSBs of Local Position
	wire [(`FLIT_SRC_WIDTH/2) -1 : 0] local_Y;	// MSBs of Local Position
	wire [(`FLIT_DST_WIDTH/2) -1 : 0] dst_X;	// LSBs of Destination Position
	wire [(`FLIT_DST_WIDTH/2) -1 : 0] dst_Y;	// MSBs of Destination Position
	wire [(`FLIT_SRC_WIDTH/2) -1 : 0] src_X;	// LSBs of Source Position
	wire [(`FLIT_SRC_WIDTH/2) -1 : 0] src_Y;	// MSBs of Source Position
	
	// Assignments
	assign local_X = local_in[(`FLIT_DST_WIDTH/2) - 1 : 0];
	assign local_Y = local_in[`FLIT_DST_WIDTH - 1 : (`FLIT_DST_WIDTH/2)];
	assign dst_X = dst_in[(`FLIT_DST_WIDTH/2) - 1 : 0];					// Splits destination data into X and Y
	assign dst_Y = dst_in[`FLIT_DST_WIDTH - 1 : (`FLIT_DST_WIDTH/2)];
	assign src_X = src_in[(`FLIT_SRC_WIDTH/2) - 1 : 0];
	assign src_Y = src_in[`FLIT_SRC_WIDTH - 1 : (`FLIT_SRC_WIDTH/2)];
	
	// Behavior
	initial begin:Initialization
		 // Empty 
	end

	always @ (RST or enable or dst_X or dst_Y or src_X or local_X or local_Y) begin:ROUTINGROUTINE
		if (RST) begin
			success = 0;
			result = choose_NONE;
			
		end else begin
			if (enable) begin																				// Starts routing calculation
				if (dst_X == local_X && dst_Y == local_Y) begin				// Destination equals local address
					result = choose_0;
				end else begin
					if((local_X == dst_X) && (local_Y != dst_Y)) begin	// Packet wants straight to north or south
						if(local_Y > dst_Y) begin												 // Packet wants to North
							if (src_X < dst_X) begin											 // Packet comes from west
								result = choose_1;
							end else begin																	// Packet comes from east
								result = choose_2;
							end
						end else begin
							if (src_X < dst_X) begin											 // Packet comes from west
								result = choose_3;
							end else begin																	// Packet comes from east
								result = choose_4;
							end
						end
					end else if((local_X != dst_X) && (local_Y == dst_Y)) begin // Packet wants straight to east or west
						if (local_X > dst_X) begin
							result = choose_5;
						end else begin
							result = choose_6;
						end
					end	else begin													// Diagonal destination
						if(local_X > dst_X) begin						 // Packet wants west
							if(local_Y > dst_Y) begin					 // Packet wants north
								if (src_X < dst_X) begin				 // Packet comes from west
									if(credits_5 > credits_1)begin
										result = choose_5;
									end else begin									// Packet comes from east
										result = choose_1;
									end
								end else begin
									if(credits_5 > credits_2)begin
										result = choose_5;
									end else begin
										result = choose_2;
									end
								end
							end else begin											// Packet wants south
								if (src_X < dst_X) begin
									if(credits_5 > credits_3)begin
										result = choose_5;
									end else begin
										result = choose_3;
									end
								end else begin
									if(credits_5 > credits_4)begin
										result = choose_5;
									end else begin
										result = choose_4;
									end
								end
							end
						end else begin												// Packet wants east		 
							if(local_Y > dst_Y) begin					 // Packet wants north 
								if (src_X < dst_X) begin
									if(credits_6 > credits_1)begin
										result = choose_6;
									end else begin
										result = choose_1;
									end
								end else begin
									if(credits_6 > credits_2)begin
										result = choose_6;
									end else begin
										result = choose_2;
									end
								end
							end else begin									// Packet wants south
								if (src_X < dst_X) begin
									if(credits_6 > credits_3)begin
										result = choose_6;
									end else begin
										result = choose_3;
									end
								end else begin
									if(credits_6 > credits_4)begin
										result = choose_6;
									end else begin
										result = choose_4;
									end
								end
							end
						end
					end				
				end

				
				success = 1;												// Calculation is finished
					
			end else begin											// If not enabled -> all outputs go to zero
				success = 0;												
				result = choose_NONE;
				
			end
		end
	end
endmodule

    
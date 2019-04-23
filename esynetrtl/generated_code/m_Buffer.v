
`timescale 1ns / 1ps

`include "parameters.v"

module m_Buffer(
		RST,
		CLK,
		
		command_write,
		command_read,
		
		buffer_credit_level,
		
		data_in,
		data_out
	);

	// Parameters
	parameter	P_LOCAL_ID = 0;
	parameter	P_LOCAL_CHANNEL = 0;		// e.g. 0, 1, 2, 3.... 
	`ifdef FT_ECC
		parameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH;
	`else 
		parameter P_DATA_WIDTH = `FLIT_WIDTH;
	`endif

	// Port Definition
	input RST;
	input CLK;

	input command_write;
	input command_read;

	output [`BUFFERSIZE_WIDTH - 1 : 0] buffer_credit_level;
	
  input 	[P_DATA_WIDTH - 1: 0] data_in;
  output	[P_DATA_WIDTH - 1: 0] data_out;

	// Variables	 
	reg [P_DATA_WIDTH - 1 : 0] fifoMem [`BUFFERSIZE - 1 : 0];		// Storage
	reg [`BUFFERSIZE_WIDTH : 0] i;											// Too end the loop it has to go above the limit BUFFERSIZE_WIDTH - 1!
	wire [P_DATA_WIDTH - 1 : 0] fifo_out;								// Tmp storage for out data
	reg [`BUFFERSIZE_WIDTH - 1 : 0] credits;
	wire full;
	wire empty;
	reg [`BUFFERSIZE_WIDTH - 1: 0] pointer_write;
  reg [`BUFFERSIZE_WIDTH - 1: 0] pointer_read;
	
	
	// Assignments
	// IDEA: VC can be further deactivated by assigning all input data to a variable, enable decides if the variable equals the input or is zero
	assign data_out = (!empty) ? fifo_out : 'b0;						// Output Mux
	assign fifo_out  = (pointer_read < `BUFFERSIZE ) ? fifoMem[pointer_read] : 'b0;
	assign full = (credits == 0);
	assign empty = (credits ==  `BUFFERSIZE);
	assign buffer_credit_level = credits;
	
  
	// Behavior
	initial begin:Initialization
		// Empty
	end

	
	// Write handle block
	always @ (posedge RST or posedge CLK) begin:WRITEBLOCK
		if ( RST ) begin
			pointer_write <= 0;
			for (i = 0; i < `BUFFERSIZE; i = i + 1) begin
				fifoMem[i] <= 'b0;
			end
			
		end else if (command_write && !full) begin

			fifoMem[pointer_write] <= data_in;
						
			// Reset of write pointer
			if (pointer_write  >= (`BUFFERSIZE - 1)) begin			
				pointer_write <= 0;
			end else begin
				pointer_write <= pointer_write + 1;
			end
			// $display("buffer data_in:%b data_out:%b",data_in,data_out);
		end
	end

	// Read handle block
	always @ (posedge RST or posedge CLK) begin:READBLOCK
		if ( RST ) begin
			pointer_read <= 0;
			
		end else if (command_read && !empty) begin
			
			// Reset of read pointer
			if (pointer_read  >= (`BUFFERSIZE - 1)) begin
				pointer_read <= 0;
			end else begin
				pointer_read <= pointer_read + 1;	
			end
			// $display("buffer data_in:%b data_out:%b",data_in,data_out);
			// $display("%4d: Router %0d, channel %0s - Buffer: ReadPointer: %0d, Reading/Sending: %34b!", $time, P_LOCAL_ID, P_LOCAL_CHANNEL, pointer_read, fifo_out);
		end
		
	end
	
	
	// Credit handle block
	always @ (posedge RST or posedge CLK) begin:CREDITBLOCK
		if( RST )
			credits <= `BUFFERSIZE;

		else if((!full && command_write) && ( !empty && command_read ))
			credits <= credits;

		else if(!full && command_write)
			credits <= credits - 1;

		else if(!empty && command_read)
			credits <= credits + 1;

		else
			credits <= credits;
		
	//	$display("%4d: Router %0d, channel %0s - Buffer: CREDITLEVEL: %0d!", $time, P_LOCAL_ID, P_LOCAL_CHANNEL, credits);
	end
	
endmodule

    
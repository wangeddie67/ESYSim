#include "rtl_gen.h"
#include <iostream>
#include <fstream>
#include<cmath>
#include<cstdlib>
#include<cstring>
#include<cfloat>

void NOC_GEN::copyFile()
{
    m_Buffer();
    m_D_FF_mod();
    m_D_FF_sync();
    m_Demux_1in2out();
    m_Mux_2in1out();
    m_StateMachine();
    m_LogModule();
    m_RoutingCalculation();
    //m_FaultInjectionUnit();
    m_RR_MatrixArbiter();
    m_RR_EvaluationUnit();
    m_MatrixArbiter_subUnit();
    m_RandomInjector();
    m_PacketInjector();
    m_ClockGenerator();  
}

void NOC_GEN::m_Buffer()
{
    ofile.open( FILE_PATH + "m_Buffer.v" );
    ofile
    <<R"(
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

    )";
    ofile.close();
}

void NOC_GEN::m_D_FF_mod()
{
    ofile.open( FILE_PATH + "m_D_FF_mod.v" );
    ofile
    <<R"(
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
    )";
    ofile.close();
}
void NOC_GEN::m_D_FF_sync()
{
    ofile.open( FILE_PATH + "m_D_FF_sync.v" );
    ofile
    <<R"(
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
    )";
    ofile.close();
}
void NOC_GEN::m_Demux_1in2out()
{
    ofile.open( FILE_PATH + "m_Demux_1in2out.v" );
    ofile
    <<R"(
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
    )";
    ofile.close();
}
void NOC_GEN::m_Mux_2in1out()
{
    ofile.open( FILE_PATH + "m_Mux_2in1out.v" );
    ofile
    <<R"(
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
    )";
    ofile.close();
}
void NOC_GEN::m_StateMachine()
{
    ofile.open( FILE_PATH + "m_StateMachine.v" );
    ofile
    <<R"(
`timescale 1ns / 1ps

`include "parameters.v"

module m_StateMachine(
		router_id,
		RST,
		CLK,
		
		// Data Handling + Channel/BufferControl
		data_valid,
		command_write,
		command_read,
		buffer_credits_local,
		buffer_credits_sharing,					
		data_in,
		
		// RouteCalculation signals
		rc_enable,
		rc_success,
		rc_result,								
		
		// SwitchAllocation signals
		sa_enable,
		sa_request,
		sa_grant,
		
		// Switch Transaction signals
		st_request,
		st_data_valid,
		st_credits_feedback,

		// RR Allocation signals
		rr_request,
		rr_result,
		
		// Assertion Error
		error_ecc,
		
		// Display state 
		current_state
		
	);

	input [`FLIT_SRC_WIDTH-1 : 0] router_id;
	// Parameters
	// Position Parameters
	parameter		P_LOCAL_ID = 0;
	wire P_LOCAL_X ;
	wire P_LOCAL_Y ;
	assign P_LOCAL_X = router_id[(`FLIT_DST_WIDTH/2) - 1 : 0];
	assign P_LOCAL_Y = router_id[`FLIT_DST_WIDTH - 1 : (`FLIT_DST_WIDTH/2)];
	parameter		P_LOCAL_CHANNEL = 0;		// 0, 1, 2, 3...
	
	// Statemachine Parameters
	parameter	 IDLE_RC = 2'b01,	// IDLE + Route Calculation
							SA = 2'b10,				// Switch Allocation
							ST = 2'b11;				// Switch Transaction
	
	// Flit Parameters
	parameter	 HEADFLIT = 2'b10;
	parameter	 BODYFLIT = 2'b11;
	parameter	 TAILFLIT = 2'b01;
	
	
	// Port Definition
	input RST;
	input CLK;
	
	input data_valid;
	output command_write;
	output reg command_read;
	output [`BUFFERSIZE_WIDTH - 1:0] buffer_credits_sharing;
	input [`BUFFERSIZE_WIDTH - 1:0] buffer_credits_local;
	input [`FLIT_WIDTH - 1:0] data_in;
	
	output reg rc_enable;
	input rc_success;
	input [`CHANNELS - 1:0] rc_result;								
	
	output reg sa_enable;
	output reg [`CHANNELS - 1:0] sa_request;								
	input sa_grant;
	
	output reg [`CHANNELS - 1:0] st_request;		
	output reg st_data_valid;							
	input [`BUFFERSIZE_WIDTH - 1:0] st_credits_feedback;
	
	output reg [`CHANNELS - 1:0] rr_request;
	input [`CHANNELS - 1:0] rr_result;
	
	// Assertion Error
	input	error_ecc;
	
	output [`STATES_WIDTH - 1:0] current_state;
	
	
	// Variables	
	reg [`STATES_WIDTH - 1:0] next_state;
	reg [`STATES_WIDTH - 1:0] state;

	wire [`FLIT_TYPE_WIDTH - 1 : 0] flit_type;
		
	reg [`CHANNELS - 1:0] rc_stored_result;
	reg [`CHANNELS - 1:0] next_rc_stored_result;
	
	reg rr_grant;
	wire next_rr_grant;
	
	// Assignments
	
	// Local credits get directly shared with the outside
	assign buffer_credits_sharing = buffer_credits_local;

	// Output signal of state for logging
	assign current_state = state;

	// Buffer write handler
	assign command_write = (RST) ? 0 : data_valid;	
	
	// Prepares the flit_type for the evaulation mechanism
	assign flit_type = data_in[`FLIT_TYPE_WIDTH + `FLIT_DATA_WIDTH - 1: `FLIT_DATA_WIDTH];
	
	// Compares the rr_result and the local rr_request to enable rr_grant 
	assign next_rr_grant = ((rr_request & rr_result) != 0);
	
	
	// Behavior
	initial begin:INITIALIZATION
		// Empty
	end
	
	// This block is responsible for the reset of the States and that the next cycle becomes the state
	always @(posedge RST or posedge CLK) begin: SET_STATE
		if (RST) begin
			state <= IDLE_RC;
			
		end else begin
			state <= next_state;
			
		end
	end
	
	// This block stores the rc result for later use
	always @(posedge RST or posedge CLK) begin: STORE_REQUEST
		if (RST) begin
			rc_stored_result <= 0;
			rr_grant <= 0;
		end else begin
			rc_stored_result <= next_rc_stored_result;
			rr_grant <= next_rr_grant;
		end
	end
	
	// Depending on the current state, this block sets/changes signals during a state (Mealy)
	always @(*) begin: SET_ASYNC_OUTPUTS
	if (RST) begin:RESET_OUTPUTSIGNALS
						
			command_read = 0;
			
			rc_enable = 0;
			next_rc_stored_result = 0;
			
			sa_request = 0;
			sa_enable = 0;
			
			st_request = 0;
			st_data_valid = 0;
			
			rr_request = 0;
			
		end else begin
			case(state)
				IDLE_RC:begin
									// command_read = 0; 
			
									// rc_enable = 0;
									// rc_stored_result = 0;
									
									sa_request = 0;
									sa_enable = 0;
									
									st_request = 0;
									st_data_valid = 0;
									
									rr_request = 0;
									
									if(buffer_credits_local < `BUFFERSIZE) begin
										if(flit_type == HEADFLIT && !error_ecc) begin
											rc_enable = 1;
											command_read = 0; 
										end else begin
											rc_enable = 0;
											command_read = 1; // Continue to read till a HEADFLIT is found
											// synopsys translate_off
											// $display("%4d: Router %0d, channel %0d - StateMachine: Packetdrop active!", $time, P_LOCAL_ID, P_LOCAL_CHANNEL);
											// synopsys translate_on 
										end
									end else begin
										rc_enable = 0;
										command_read = 0; 
									end

									
									// `else
									// if(buffer_credits_local < `BUFFERSIZE) begin
										// if(flit_type == HEADFLIT) begin
											// rc_enable = 1;
											// command_read = 0; 
										// end else begin
											// rc_enable = 0;
											// // this could cause an error, if it is still high even when it shouldn't!
											// command_read = 1; // Continue to read till a HEADFLIT is found; 
										// end
									// end else begin
										// rc_enable = 0;
										// command_read = 0; 
									// end
									// `endif
									
									if (rc_success) begin
										next_rc_stored_result = rc_result;
									end else begin
										next_rc_stored_result = 0;
									end
									
								end
								

				SA:			begin
									command_read = 0;
			
									rc_enable = 0;
									next_rc_stored_result = rc_stored_result;
									
									sa_request = rc_stored_result;
									sa_enable = 1;
									
									st_request = 0;
									st_data_valid = 0;
									
									rr_request = 0;
									
									// if (sa_grant) begin
										// sa_enable = 0;
									// end else begin
										// sa_request = rc_stored_result;
										// sa_enable = 1;
									// end
									
								end
								
				ST:			begin
									// command_read = 0;

									rc_enable = 0;
									next_rc_stored_result = rc_stored_result;
									
									sa_request = rc_stored_result;
									sa_enable = 0;
									
									st_request = rc_stored_result;
									st_data_valid = 0;
									
									rr_request = 0;
									
									if (rr_grant) begin	
										// The channel got the grant this cycle and will send out a flit_type
										// Next cycle there will be one flit less in the buffer, to justify another roundrobin_request we need at least 2 flit in the buffer -> BUFFERSIZE - 1
										if (buffer_credits_local < `BUFFERSIZE - 1) begin
											// Due to the output buffer there will be a delay of 2 cycles
											// 1 cycle (= this cycle) till the flit is in the output buffer
											// 1 cycle till the traveled from the output buffer to the destination
											// So the neighbor has to have 2 slots free (one could be filled by the output buffer and the other one will be filled by our flit of this cycle)
											if (st_credits_feedback > 4'd2) begin
												rr_request = rc_stored_result;
											end else begin
												rr_request = 0;
											end
										end
										command_read = 1;
										st_data_valid = 1;
									end else begin
									// No flit get send out this cycle
										// The local level gets checked, if it isn't empty
										if (buffer_credits_local < `BUFFERSIZE) begin
											// We don't send out a flit this cycle, the buffer could store 1 flit so we check if the credits are above 1
											if (st_credits_feedback > 4'd1) begin
												rr_request = rc_stored_result;
											end else begin
												rr_request = 0;
											end
										end
										command_read = 0;
										st_data_valid = 0;
									end
								end
								
				default:	 begin
															
									command_read = 0;
									
									rc_enable = 0;
									next_rc_stored_result = 0;
									
									sa_request = 0;
									sa_enable = 0;
									
									st_request = 0;
									st_data_valid = 0;
									
									rr_request = 0;
									
									// synopsys translate_off
									// $display("%4d: StateMachine SET_ASYNC_OUTPUTS entered an undefined state in router %0d, channel %0d!", $time, P_LOCAL_ID, P_LOCAL_CHANNEL);
									// synopsys translate_on
								end
			endcase
		end
	end
	
	// Depending on the current state, this routine evalutes the input/triggeres for the next state
	always @(*) begin: SET_NEXT_STATE
		// synopsys translate_off
		// $display("%4d: Router %0d, channel %0d - StateMachine: Entered SET_NEXT_STATE-Block!", $time, P_LOCAL_ID, P_LOCAL_CHANNEL);
		// synopsys translate_on
		case(state)
			IDLE_RC:begin
								if (buffer_credits_local < `BUFFERSIZE) begin
									if (flit_type == HEADFLIT) begin 
										if(rc_success) begin
											next_state = SA;
										end else begin
											next_state = IDLE_RC;
										end
										
									end else begin
										next_state = IDLE_RC;
									end
									
								end else begin
									next_state = IDLE_RC;
								end
							end
							
			SA:		 begin
								if(sa_grant) begin
									next_state = ST;
								end else begin
									next_state = SA;
								end
							end
							
							
			ST:		 begin
								if((flit_type == TAILFLIT) && (rr_grant)) begin
									next_state = IDLE_RC;
								end else begin
									next_state = ST;
								end
							end
	
			default:begin
								next_state = IDLE_RC;
							end
		endcase
	
	end
	 
endmodule

    )";
    ofile.close();
}
void NOC_GEN::m_LogModule()
{
    ofile.open( FILE_PATH + "m_LogModule.v" );
    ofile
    <<R"(
`timescale 1ns / 1ps

`include "parameters.v"

module m_LogModule(
    RST,
    CLK,
    router_id,
    
    
    buffer_data_in,
    buffer_write,
    buffer_credits,
    
    statemachine_data_in,
    
    rc_success,
    rc_result,
    
    sa_grant,
    
    st_data_valid,
    
    current_state
  );

  // Parameters
  // Position Parameters
  input [`FLIT_SRC_WIDTH - 1 : 0] router_id;
  wire  [`FLIT_SRC_WIDTH - 1 : 0]  P_LOCAL_ID;
  assign P_LOCAL_ID = router_id;
  parameter    P_LOCAL_CHANNEL = 0;    // 0, 1, 2, 3...
  
  // Statemachine Parameters
  parameter   IDLE_RC = 2'b01,  // IDLE + Route Calculation
              SA = 2'b10,        // Switch Allocation
              ST = 2'b11;        // Switch Transaction
  
  // Flit Parameters
  parameter   HEADFLIT = 2'b10;
  parameter   BODYFLIT = 2'b11;
  parameter   TAILFLIT = 2'b01;       
  
  // Port Definition
  input RST;
  input CLK;
  
  input [`FLIT_WIDTH - 1: 0] buffer_data_in;
  input buffer_write;
  input [`BUFFERSIZE_WIDTH - 1:0] buffer_credits;
    
  input [`FLIT_WIDTH - 1: 0] statemachine_data_in;
  
  input rc_success;
  input [`CHANNELS - 1:0] rc_result;    
    
  input sa_grant;
  
  input st_data_valid;
  
  input [`STATES_WIDTH - 1:0] current_state;

  // Variables   
  // Variables for logging
  // synopsys translate_off
  reg [`CHANNELS_WIDTH : 0] log_intern_pc;
  reg log_intern_vc;
  reg [`INPUT_INTEGER - 1 : 0] log_out_router_mod;
  reg [`CHANNELS_WIDTH : 0] log_out_pc;
  reg log_out_vc;
  reg [`STATES_WIDTH - 1:0] log_state_now;
  reg[`STATES_WIDTH - 1:0] log_state_previous;
  
  reg [`FLIT_ID_WIDTH - 1: 0] log_flitID;
  reg [`FLIT_TYPE_WIDTH - 1: 0] log_flitType;
  
  reg [`STATES_WIDTH - 1:0] previous_state;
  
  // synopsys translate_on

  // Assignments
  

  // Behavior
  initial begin:Initialization
    // Empty
  end

  reg [2:0] P_LOCAL_PORT;
  reg  P_LOCAL_VC;


  always @ (posedge CLK) begin: FLIT_RECEIVED
    if(buffer_write && (buffer_credits != 0)) begin
      // NoCLog format: m_time, m_type, m_src, m_src_pv, m_src_vc, m_dst, m_dst_pv, m_dst_vc, ID
      if (P_LOCAL_CHANNEL == 0) begin
        // if it comes from extern it is a special case
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_RECEIVE, 
        -1, -1, -1, 
        P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        buffer_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH], buffer_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH]);
      
      end else if(P_LOCAL_CHANNEL == 1) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_RECEIVE, 
        -1, -1, -1, 
        P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        buffer_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH], buffer_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH]);
      
      end else if(P_LOCAL_CHANNEL == 2) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_RECEIVE, 
        -1, -1, -1, 
        P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        buffer_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH], buffer_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH]);
      
      end else if(P_LOCAL_CHANNEL == 3) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_RECEIVE, 
        -1, -1, -1, 
        P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        buffer_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH], buffer_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH]);
      
      end else if(P_LOCAL_CHANNEL == 4) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_RECEIVE, 
        -1, -1, -1, 
        P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        buffer_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH], buffer_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH]);
      
      end else if(P_LOCAL_CHANNEL == 5) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_RECEIVE, 
        -1, -1, -1, 
        P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        buffer_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH], buffer_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH]);
      
      end else if(P_LOCAL_CHANNEL == 6) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_RECEIVE, 
        -1, -1, -1, 
        P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        buffer_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH], buffer_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH]);
      
      end else begin
        $display("Logging Error in Router %0d, Channel %0d", P_LOCAL_ID, P_LOCAL_CHANNEL);
      end
    end
  end
)";

if(p_port_num == 5)
    ofile
    <<R"( 
  always@(posedge CLK)
  begin
    case(P_LOCAL_CHANNEL)
    0: begin P_LOCAL_PORT = 3'b0; P_LOCAL_VC=1'b0; end
    1: begin P_LOCAL_PORT = 3'b1; P_LOCAL_VC=1'b0; end
    2: begin P_LOCAL_PORT = 3'b1; P_LOCAL_VC=1'b1; end
    3: begin P_LOCAL_PORT = 3'b10; P_LOCAL_VC=1'b0; end
    4: begin P_LOCAL_PORT = 3'b10; P_LOCAL_VC=1'b1; end
    5: begin P_LOCAL_PORT = 3'b11; P_LOCAL_VC=1'b0; end
    6: begin P_LOCAL_PORT = 3'b100; P_LOCAL_VC=1'b0; end
    default: begin P_LOCAL_PORT =3'b111;  P_LOCAL_VC=1'b0; end
    endcase
  end

  always @ (posedge RST or posedge rc_success) begin
    #1 ;
    if (RST) begin
      log_intern_pc = 0;
      log_intern_vc = 0;
      log_out_router_mod = 0;
      log_out_pc = 0;
      log_out_vc = 0;
    end else begin
      log_flitID =  statemachine_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH];
      log_flitType =  statemachine_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH];
      case(rc_result)
        7'b0000001: begin 
                  log_intern_pc = 0;
                  log_intern_vc = 0;
                  log_out_router_mod = 0;
                  log_out_pc = 0;
                  log_out_vc = 0;
                 end
        7'b0000010: begin
                  log_intern_pc = 1;
                  log_intern_vc = 0;
                  log_out_router_mod = -`MESH_SIZE_X;
                  log_out_pc = 2;
                  log_out_vc = 0;
                 end
        7'b0000100: begin
                  log_intern_pc = 1;
                  log_intern_vc = 1;
                  log_out_router_mod = -`MESH_SIZE_X;
                  log_out_pc = 2;
                  log_out_vc = 1;
                 end
        7'b0001000: begin
                  log_intern_pc = 2;
                  log_intern_vc = 0;
                  log_out_router_mod = `MESH_SIZE_X;
                  log_out_pc = 1;
                  log_out_vc = 0;
                 end
        7'b0010000: begin
                  log_intern_pc = 2;
                  log_intern_vc = 1;
                  log_out_router_mod = `MESH_SIZE_X;
                  log_out_pc = 1;
                  log_out_vc = 1;
                 end
        7'b0100000: begin
                  log_intern_pc = 3;
                  log_intern_vc = 0;
                  log_out_router_mod = -1;
                  log_out_pc = 4;
                  log_out_vc = 0;
                 end
        7'b1000000: begin
                  log_intern_pc = 4;
                  log_intern_vc = 0;
                  log_out_router_mod = 1;
                  log_out_pc = 3;
                  log_out_vc = 0;
                 end
        default: begin
                  // Emtpy
                end
      endcase
      )";
else if(p_port_num == 7)
      ofile
      <<R"(
    always@(posedge CLK)
    begin
        case(P_LOCAL_CHANNEL)
        0: begin P_LOCAL_PORT = 3'b0; P_LOCAL_VC=1'b0; end
        1: begin P_LOCAL_PORT = 3'b1; P_LOCAL_VC=1'b0; end
        2: begin P_LOCAL_PORT = 3'b1; P_LOCAL_VC=1'b1; end
        3: begin P_LOCAL_PORT = 3'b10; P_LOCAL_VC=1'b0; end
        4: begin P_LOCAL_PORT = 3'b10; P_LOCAL_VC=1'b1; end
        5: begin P_LOCAL_PORT = 3'b11; P_LOCAL_VC=1'b0; end
        6: begin P_LOCAL_PORT = 3'b100; P_LOCAL_VC=1'b0; end
        default: begin P_LOCAL_PORT =3'b111; ;  P_LOCAL_VC=1'b0; end
        endcase
    end

    always @ (posedge RST or posedge rc_success) begin
    #1 ;
    if (RST) begin
      log_intern_pc = 0;
      log_intern_vc = 0;
      log_out_router_mod = 0;
      log_out_pc = 0;
      log_out_vc = 0;
    end else begin
      log_flitID =  statemachine_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH];
      log_flitType =  statemachine_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH];
      case(rc_result)
        7'b0000001: begin 
                  log_intern_pc = 0;
                  log_intern_vc = 0;
                  log_out_router_mod = 0;
                  log_out_pc = 0;
                  log_out_vc = 0;
                 end
        7'b0000010: begin
                  log_intern_pc = 1;
                  log_intern_vc = 0;
                  log_out_router_mod = -`MESH_SIZE_X;
                  log_out_pc = 2;
                  log_out_vc = 0;
                 end
        7'b0000100: begin
                  log_intern_pc = 1;
                  log_intern_vc = 1;
                  log_out_router_mod = -`MESH_SIZE_X;
                  log_out_pc = 2;
                  log_out_vc = 1;
                 end
        7'b0001000: begin
                  log_intern_pc = 2;
                  log_intern_vc = 0;
                  log_out_router_mod = `MESH_SIZE_X;
                  log_out_pc = 1;
                  log_out_vc = 0;
                 end
        7'b0010000: begin
                  log_intern_pc = 2;
                  log_intern_vc = 1;
                  log_out_router_mod = `MESH_SIZE_X;
                  log_out_pc = 1;
                  log_out_vc = 1;
                 end
        7'b0100000: begin
                  log_intern_pc = 3;
                  log_intern_vc = 0;
                  log_out_router_mod = -1;
                  log_out_pc = 4;
                  log_out_vc = 0;
                 end
        7'b1000000: begin
                  log_intern_pc = 4;
                  log_intern_vc = 0;
                  log_out_router_mod = 1;
                  log_out_pc = 3;
                  log_out_vc = 0;
                 end
        default: begin
                  // Emtpy
                end
      endcase
      )";

ofile
<<R"(
      // NoCLog format: m_time, m_type, m_src, m_src_pv, m_src_vc, m_dst, m_dst_pv, m_dst_vc, ID
      if (P_LOCAL_CHANNEL == 0) begin
        // if it comes from extern it is a special case
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 1) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 2) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 3) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID , log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 4) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 5) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 6) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else begin
        $display("Logging Error in Router %0d, Channel %0d", P_LOCAL_ID, P_LOCAL_CHANNEL);
      end
    end
  end
  
  
  always @ (posedge CLK) begin
    // NoCLog format: m_time, m_type, m_src, m_src_pv, m_src_vc, m_dst, m_dst_pv, m_dst_vc, ID
    if(sa_grant) begin
      if (P_LOCAL_CHANNEL == 0) begin
        // if it comes from extern it is a special case
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 1) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 2) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 3) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID , log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 4) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 5) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else if(P_LOCAL_CHANNEL == 6) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
      end else begin
        $display("Logging Error in Router %0d, Channel %0d", P_LOCAL_ID, P_LOCAL_CHANNEL);
      end
    end
  end
  
   // Switch transaction
  always @ (posedge CLK) begin
    // Changes the value to the new flit after each round
    log_flitID =  statemachine_data_in[`FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_FLAGS_WIDTH];
    log_flitType =  statemachine_data_in[`FLIT_WIDTH - 1: `FLIT_DATA_WIDTH];
      
    if (st_data_valid)begin
      if (P_LOCAL_CHANNEL == 0) begin
        // if it comes from extern it is a special case
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SWITCH, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SEND, P_LOCAL_ID, log_intern_pc, log_intern_vc, P_LOCAL_ID + log_out_router_mod, log_out_pc, log_out_vc, log_flitType, log_flitID);
        
      end else if(P_LOCAL_CHANNEL == 1) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SWITCH, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SEND, P_LOCAL_ID, log_intern_pc, log_intern_vc, P_LOCAL_ID + log_out_router_mod, log_out_pc, log_out_vc, log_flitType, log_flitID);
        
      end else if(P_LOCAL_CHANNEL == 2) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SWITCH, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SEND, P_LOCAL_ID, log_intern_pc, log_intern_vc, P_LOCAL_ID + log_out_router_mod, log_out_pc, log_out_vc, log_flitType, log_flitID);
        
      end else if(P_LOCAL_CHANNEL == 3) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SWITCH, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID , log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SEND, P_LOCAL_ID, log_intern_pc, log_intern_vc, P_LOCAL_ID + log_out_router_mod, log_out_pc, log_out_vc, log_flitType, log_flitID);
        
      end else if(P_LOCAL_CHANNEL == 4) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SWITCH, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SEND, P_LOCAL_ID, log_intern_pc, log_intern_vc, P_LOCAL_ID + log_out_router_mod, log_out_pc, log_out_vc, log_flitType, log_flitID);
        
      end else if(P_LOCAL_CHANNEL == 5) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SWITCH, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SEND, P_LOCAL_ID, log_intern_pc, log_intern_vc, P_LOCAL_ID + log_out_router_mod, log_out_pc, log_out_vc, log_flitType, log_flitID);
        
      end else if(P_LOCAL_CHANNEL == 6) begin
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SWITCH, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_SEND, P_LOCAL_ID, log_intern_pc, log_intern_vc, P_LOCAL_ID + log_out_router_mod, log_out_pc, log_out_vc, log_flitType, log_flitID);
        
      end else begin
        $display("Logging Error in Router %0d, Channel %0d", P_LOCAL_ID, P_LOCAL_CHANNEL);
        
      end
      
       // Routing calc/assign end
      if (statemachine_data_in[`FLIT_TYPE_WIDTH + `FLIT_DATA_WIDTH - 1: `FLIT_DATA_WIDTH] == TAILFLIT) begin
        if (P_LOCAL_CHANNEL == 0) begin
          // if it comes from extern it is a special case
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN_END , P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING_END, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          
        end else if(P_LOCAL_CHANNEL == 1) begin
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN_END , P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING_END, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          
        end else if(P_LOCAL_CHANNEL == 2) begin
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN_END , P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING_END, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          
        end else if(P_LOCAL_CHANNEL == 3) begin
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN_END , P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID , log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING_END, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID , log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          
        end else if(P_LOCAL_CHANNEL == 4) begin
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN_END , P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING_END, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          
        end else if(P_LOCAL_CHANNEL == 5) begin
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN_END , P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING_END, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          
        end else if(P_LOCAL_CHANNEL == 6) begin
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ASSIGN_END , P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
          $display("NoCLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", $time, `LOG_FLIT_ROUTING_END, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, log_intern_pc, log_intern_vc, log_flitType, log_flitID);
        end else begin
          $display("Logging Error in Router %0d, Channel %0d", P_LOCAL_ID, P_LOCAL_CHANNEL);
        end
      end  
    end
  end
  

  // State Routine
  // TOO MUCH SPAM DONT USE CLK always @ (posedge RST or posedge CLK) begin  // will be triggered every cycle
  always @ (posedge RST or current_state) begin  // triggered if state changes
    if(RST) begin
      log_state_now = 1;
      log_state_previous = 1;
      
    end else begin
      
      case(current_state) 
        IDLE_RC: log_state_now = 1;
        SA: log_state_now = 2;
        ST: log_state_now = 3;
        default:;
      endcase
     
      
    // NoCLog format: m_time, m_type, m_src, m_src_pv, m_src_vc, m_dst, m_dst_pv, m_dst_vc, ID
      if (P_LOCAL_CHANNEL == 0) begin
        // if it comes from extern it is a special case
        $display("StateLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_STATE_CHANGED, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        0, 0, log_state_previous, log_state_now);
        
      end else if(P_LOCAL_CHANNEL == 1) begin
        $display("StateLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_STATE_CHANGED, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        0, 0, log_state_previous, log_state_now);
        
      end else if(P_LOCAL_CHANNEL == 2) begin
        $display("StateLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_STATE_CHANGED, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        0, 0, log_state_previous, log_state_now);
        
      end else if(P_LOCAL_CHANNEL == 3) begin
        $display("StateLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_STATE_CHANGED, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        0, 0, log_state_previous, log_state_now);
        
      end else if(P_LOCAL_CHANNEL == 4) begin
        $display("StateLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_STATE_CHANGED, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        0, 0, log_state_previous, log_state_now);
        
      end else if(P_LOCAL_CHANNEL == 5) begin
        $display("StateLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_STATE_CHANGED, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        0, 0, log_state_previous, log_state_now);
        
      end else if(P_LOCAL_CHANNEL == 6) begin
        $display("StateLog: %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d", 
        $time, `LOG_FLIT_STATE_CHANGED, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, P_LOCAL_ID, P_LOCAL_PORT, P_LOCAL_VC, 
        0, 0, log_state_previous, log_state_now);
        
      end else begin
        $display("Logging Error in Router %0d, Channel %0d", P_LOCAL_ID, P_LOCAL_CHANNEL);
      end
      
      // Storing the state for the next cycle
      log_state_previous = log_state_now;
      
    end
  end

endmodule
)";
    ofile.close();
}
void NOC_GEN::m_RoutingCalculation()
{
    ofile.open( FILE_PATH + "m_RoutingCalculation.v" );
    ofile
    <<R"(
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
    )";

    if(p_port_num == 5)
    ofile
    <<R"(
	parameter 	choose_NONE 	= 7'b0000000;
	parameter 	choose_0 		= 7'b0000001;
	parameter 	choose_1 		= 7'b0000010;
	parameter 	choose_2 		= 7'b0000100;
	parameter 	choose_3 		= 7'b0001000;
	parameter 	choose_4 		= 7'b0010000;
	parameter 	choose_5 		= 7'b0100000;
	parameter 	choose_6 		= 7'b1000000;
    )";
    else if(p_port_num == 7)
    ofile
    <<R"(
	parameter 	choose_NONE 	= 7'b0000000;
	parameter 	choose_0 		= 7'b0000001;
	parameter 	choose_1 		= 7'b0000010;
	parameter 	choose_3 		= 7'b0000100;
	parameter 	choose_5 		= 7'b0001000;
	parameter 	choose_6 		= 7'b0010000;
	parameter 	choose_2 		= 7'b0100000;
	parameter 	choose_4 		= 7'b1000000;
    )";

    ofile
    <<R"(
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

    )";
    ofile.close();
}

void NOC_GEN::m_ClockGenerator()
{
    ofile.open( FILE_PATH + "m_ClockGenerator.v" );
    ofile
    <<R"(

`timescale 1ns / 1ps

`include "parameters.v"

module m_ClockGenerator(
		enable,
		clkOutput
	);

  // Parameters
  parameter P_CLK_PERIOD = `CLK_PERIOD;

	
	// Port Definition
	input enable;
	output reg clkOutput;
		
	
	// Variables
	reg activate_CLK_generator;
	
	
	// Behaviour
	initial clkOutput = 0;
	
	
	// Control Unit for the enable signal; activates and inactivates clockgenerator
	always @(enable) begin: ControlUnit
		if(enable == 1) begin
			activate_CLK_generator = 1;
		// end else if (enable == 0) begin 		// TODO : Redundant -> Delete										
			// activate_CLK_generator = 0;
			// clkOutput = 0;
		end else begin
			activate_CLK_generator = 0;
			clkOutput = 0;
		end
	end
	
	
	// Generates clock signal or put clk output to low
	always @(activate_CLK_generator)begin: ClockGenerator
		
		if (activate_CLK_generator == 1) begin 								// Checks if clock generator is activated
			clkOutput = 1;
			while(activate_CLK_generator == 1) begin 						// While loop for generating clock signal
					#(P_CLK_PERIOD/2) clkOutput = ~clkOutput;
			end
			clkOutput = 0;
		end else begin
			clkOutput = 0;
		end
				
	end

endmodule

    )";
    ofile.close();
}
void NOC_GEN::m_FaultInjectionUnit()
{
    ofile.open( FILE_PATH + "m_FaultInjectionUnit.sv" );
    ofile
    <<R"(
//ATTENTION: The used "RNG" (RandomNumberGenerator) results in questa sim always in the same results for every run, this needs to be reviewed

`timescale 1ns / 1ps

`include "parameters.v"


module m_FaultInjectionUnit(
		CLK,
		in,
		out
	);

	// Ports
	input			CLK;
	input			in;
	output		out;

	// Parameters
	// Set by declaration:
	parameter MODULEID = "";
	parameter [31:0] PROB_C2F = `PROP2FAULTY;		//Propability to switch to the faulty state from the correct state
	parameter [31:0] PROB_F2C = `PROP2CORRECT; 	//Propability to switch to the correct state from the faulty state
	parameter [31:0] SEED = `SEED;
	// Set normal:
	parameter STATES = 2;
	parameter STATE_WIDTH = $clog2(STATES);
	parameter 	CORRECT = 1'b0,		// everything is working correct - fault mode disabled
							FAULTY = 1'b1;		// incoming bits are getting flipped for the output - fault mode enabled


	// Variables
	reg [STATE_WIDTH - 1:0] state;
	int unsigned rand_numb; 
	reg faultyBit;

	// Assignments
	assign out = in ^ faultyBit;	// If faulty bit is high, input gets flipped

	// Behaviour
	initial begin
		// Setting up the FIU
		state = CORRECT;
		faultyBit = 0;
		// Initializing the random generator	(can be changed by giving a different parameter)
		rand_numb = $random(SEED);
		// $display("FaultInjectionUnit %s got initialized", MODULEID);
	end

	// Responsible for changing the state of this fault-unit
	always @(posedge CLK) begin: STAY_LEAVE_STATE
		rand_numb = $random();
		//		$display("%d %d %d", propaToFaultyState, propaToCorrectState, rand_numb);
		if (state == CORRECT) begin
			if (rand_numb > PROB_C2F) begin
				state = CORRECT;
				faultyBit = 0;
			end else begin
				state = FAULTY;
				faultyBit = 1;
				$display("FaultInjectionUnit %s switched to FAULTY State", MODULEID);
			end
		end	else begin
			if (rand_numb > PROB_F2C) begin
				state = FAULTY;
				faultyBit = 1;
			end else begin
				state = CORRECT;
				faultyBit = 0;
				// $display("FaultInjectionUnit %s switched to CORRECT State", MODULEID);
			end
		end
	end

endmodule

    )";
    ofile.close();
}
void NOC_GEN::m_PacketInjector()
{
    ofile.open( FILE_PATH + "m_PacketInjector.sv" );
    ofile
    <<R"(
`timescale 1ns / 1ps

`include "parameters.v"

module m_PacketInjector(
		RST,
		CLK,
		startSignal,
		packet_destination,
		packet_id,
		flit_amount,
		credit_feedback,
		data_valid,
		data_out,
		finished
	);

	// Parameters
	parameter	P_LOCAL_ID = 0;			// Position of this packet injector
	parameter 	P_HEADFLIT = 2'b10;
	parameter 	P_BODYFLIT = 2'b11;
	parameter 	P_TAILFLIT = 2'b01;
  parameter   IDLE = 2'b01,
              ACTIVE = 2'b10,
              FINISH = 2'b11;
  
  
	// Port Definition
	input RST;
	input CLK;
	input startSignal;
	input [`FLIT_DST_WIDTH - 1 : 0] packet_destination;

	
	input [`FLIT_ID_WIDTH - 1 : 0] packet_id;
	input [`BUFFERSIZE_WIDTH - 1 : 0] flit_amount;

	input [`BUFFERSIZE_WIDTH - 1 : 0] credit_feedback;
	output reg data_valid;
	output reg [`FLIT_WIDTH - 1 : 0] data_out;

	output	reg finished;
		
	// Variables	
  reg [1:0] state;
  reg [1:0] next_state;
  // Flit variables
	reg [`FLIT_TYPE_WIDTH - 1: 0] flit_type;
	reg [`FLIT_SRC_WIDTH - 1: 0] flit_src;
	reg [`FLIT_DST_WIDTH - 1: 0] flit_dst;
	reg [`FLIT_RESERVED_WIDTH - 1: 0] flit_reserved;
	reg [`FLIT_ID_WIDTH - 1: 0] id;
	reg [`FLIT_FLAGS_WIDTH - 1: 0] flit_flags;
	reg [`FLIT_PAYLOAD_WIDTH - 1 - `FLIT_ID_WIDTH - `FLIT_FLAGS_WIDTH: 0] flit_payload; // This is done to have the FLIT_ID_WIDTH always at the same place
	reg [`FLIT_CRC_WIDTH - 1 - `FLIT_ID_WIDTH - `FLIT_FLAGS_WIDTH: 0] flit_checksum;
	
	reg [`FLIT_WIDTH - 1 : 0] flit_tmp;
	reg sendingActive;
	// reg resend_flit;
	reg [`BUFFERSIZE_WIDTH - 1 : 0] amount;
	reg [`FLIT_DST_WIDTH - 1 : 0] destination;
	reg [`FLIT_WIDTH - 1 : 0] mem [`BUFFERSIZE - 1 : 0];
  reg [`BUFFERSIZE_WIDTH - 1: 0] mem_pointer;
	integer i;
	
	// Assignments
	// Credit level handler
	assign data_out = (sendingActive) ? mem[mem_pointer] : 0;
  
	// Behavior
	initial begin:Initialization
		// Empty
	end
	
	always @ (RST or posedge CLK) begin
		if (RST) begin
			// Reset values
			flit_type = 0;
      flit_src = P_LOCAL_ID;
      flit_dst = 0;
      flit_reserved = 0;
			id = 0;
      flit_flags = 4'b1111;
      flit_payload = 0;
      flit_checksum = 20'b11110000111100001111;
      sendingActive = 0;
			flit_tmp = 0;
			amount = 0;
			destination = 0;
		
		// Check for if packetinjector is enabled (and no other sending is active)
		end else if (startSignal && !sendingActive)begin
			if (flit_amount < 0 || flit_amount > 5) begin
				$display("%4d: PacketInjector from router %0d: The allowed amount of flits for one packet is between 0 and (inclusive) 5 excluding HF and TF!", $time, P_LOCAL_ID);
			end else begin
				amount = flit_amount;
				destination = packet_destination;
        flit_dst = destination;
				id = packet_id;
        
        // Loading the buffer with the packet
        for (i = 0; i < (amount + 2); i = i + 1) begin
          if (i == 0) begin         //CREATE_HEADFLIT
            flit_type = P_HEADFLIT;
            flit_tmp = {flit_type, flit_src, flit_dst, flit_reserved, id, flit_flags};
          end else if (i == amount + 2 - 1 ) begin  //CREATE TAILFLIT
            flit_type = P_TAILFLIT;
            flit_tmp = {flit_type, flit_checksum, id, flit_flags};
          end else begin      // Else BODYFLIT
            flit_type = P_BODYFLIT;
            flit_payload = i; 
            flit_tmp = {flit_type, flit_payload, id, flit_flags};
          end
          
          mem[i] = flit_tmp;      // The +1 is that the sending starts with the correct flit
        end
        
				sendingActive = 1;
			end
		end 
	end
	
	// Sending logic
	always @ (RST or posedge CLK) begin
		if (RST) begin
      state = IDLE;
		end else begin
      state = next_state;
    end
  end
  
  always @ (RST or posedge CLK) begin
    if (RST) begin
      mem_pointer <= 0;
			for (i = 0; i < `BUFFERSIZE; i = i + 1) begin
				mem[i] <= 0;
			end
    end else begin
      if (data_valid && sendingActive) begin
        mem_pointer <= mem_pointer + 1;
      end else if (!data_valid && sendingActive) begin
        mem_pointer <= mem_pointer;
      end else begin
        mem_pointer <= 0;
      end
    end
  end
  
  always @ (RST or sendingActive or state or credit_feedback or mem[mem_pointer]) begin
    if(RST) begin
      data_valid  = 0;
      finished = 0;
      next_state = IDLE;
      
    end else begin
      case(state)
        IDLE:     begin
                    if(sendingActive) begin
                      // Important Logmessage for PacketData
                      $display("PacketInjectLog: %0d %0d %0d %0d %0d %0d", id, (flit_amount + 2), P_LOCAL_ID, destination, 0, $time);
                      next_state = ACTIVE;
                    end else begin
                      next_state = IDLE;
                    end
                    finished = 0;
                    data_valid = 0;
                  end
        
        ACTIVE:   begin
                    if(credit_feedback > 4'd1) begin 
                      finished = 0;
                      data_valid = 1;
                      if (mem[mem_pointer][33:32] == `FLIT_TAILFLIT) begin   // Checking for TAILFLIT
                        next_state = FINISH;      // if found, next state is FINISH
                      end else begin
                        next_state = ACTIVE;
                      end
                      
                    end else begin
                      finished = 0;
                      data_valid = 0;
                      next_state = ACTIVE;
                    end
                  end
        
        FINISH:   begin
                    finished = 1;
                    data_valid = 0;
                    sendingActive = 0;  // This should be placed somewhere else!
                    next_state = IDLE;
                  end
                  
        default:  begin
                  end
      endcase
    end 
	end
		
endmodule
    )";
    ofile.close();
}
void NOC_GEN::m_RandomInjector()
{
    ofile.open( FILE_PATH + "m_RandomInjector.v" );
    ofile
    <<R"(
`timescale 1ns / 1ps

`include "parameters.v"

module m_RandomInjector(
		RST,
		CLK,
		enable,
		
		id_request,
		id_grant, 
		id_result, 
		
		credit_feedback,
		data_valid,
		data_out,
    finished_round
	);

	// Parameters
	parameter P_LOCAL_ID = 0;
	parameter P_LOCAL_PORT = 0;	// 0, 1, 2...
	parameter P_SENDTHRESHOLD = `PACKETINJECTTHRESHOLD;

	// Port Definition
	input RST;
	input CLK;
	
	input enable;

	output reg id_request;
	input id_grant;
	input [`FLIT_ID_WIDTH - 1: 0] id_result;
	
	
	
	input [`BUFFERSIZE_WIDTH - 1 : 0] credit_feedback;
	
	output data_valid;
	output [`FLIT_WIDTH - 1 : 0] data_out;
	output finished_round;
  

	// Variables	 
	reg startTrigger;
	reg [`FLIT_DST_WIDTH - 1 : 0] random_packet_destination;
	reg [`FLIT_ID_WIDTH - 1 : 0] packet_id;
	reg [`BUFFERSIZE_WIDTH - 1 : 0] random_flit_amount;
	reg sendingActive;
	
	wire finished;
	integer sendThreshold;
	integer counter;

	// Assignments
	assign finished_round = finished;

	// Behavior
	initial begin:Initialization
		// Empty
	end
	
	
	always @ (RST or posedge CLK) begin
		if (RST) begin
			// Resetting of values
			sendThreshold = 0;
			startTrigger = 0;
			sendingActive = 0;
			id_request = 0;
			counter = 0;
			random_packet_destination = 0;
			packet_id = 0;
			random_flit_amount = 0;
		end else if (enable && !sendingActive) begin
			// Random Start
			// Check if counter is over threshold (parameters.v - file) - shall be used as a kind of delay for random sending
			if (counter >= `PACKETINJDELAY) begin			
				sendThreshold = {$random} % (1001);	// Create random value
				
				// Check if random value goes over threshold (parameters.v - file)
				if (sendThreshold <= P_SENDTHRESHOLD) begin	
					// prepare sending
					sendingActive = 1;
					random_packet_destination = {$random} % (`ROUTERS );
					// Get flit ID
					id_request = 1;
					wait(id_grant);	// This won't be synthezised 
					packet_id = id_result;
					id_request = 0;
					random_flit_amount = {$random} % (`BODYFLITAMOUNT + 1);			// +1 to include the parameter itself to the values
					// Start sending
					startTrigger = 1;
					// $display("%4d: Router %0d, channel %0d - RandomInjector: Start injecting a packet - PACKETDATA{ID:%0d, SIZE:%0d, SRC:%0d, DST:%0d, STARTTIME:%0d}!", $time, P_LOCAL_ID, P_LOCAL_PORT, packet_id, (random_flit_amount + 2), P_LOCAL_ID, random_packet_destination, $time);
					
					counter = 0;
				end
			end
			counter = counter + 1;
		end else if (!enable && !sendingActive) begin	 // Else if sending is finished and never enabled
			startTrigger = 0;
			
			random_packet_destination = 0;
			packet_id = 0;
			random_flit_amount = 0;
		end
	end

	always @ (posedge finished) begin
		startTrigger = 0;
		sendingActive = 0;
	end
	
	// Module Decleration
	m_PacketInjector #(
		.P_LOCAL_ID(P_LOCAL_ID)
	) packInj1(
		.RST(RST)
		,.CLK(CLK)
		,.startSignal(startTrigger)
		,.packet_destination(random_packet_destination)
		,.packet_id(packet_id)
		,.flit_amount(random_flit_amount)
		,.credit_feedback(credit_feedback)
		,.data_valid(data_valid)
		,.data_out(data_out)
		,.finished(finished)
	);

endmodule

    )";
    ofile.close();
}
void NOC_GEN::m_MatrixArbiter_subUnit()
{
    ofile.open( FILE_PATH + "m_MatrixArbiter_subUnit.v" );
    ofile
    <<R"(
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
    )";
    ofile.close();
}
void NOC_GEN::m_RR_EvaluationUnit()
{
    ofile.open( FILE_PATH + "m_RR_EvaluationUnit.v" );
    ofile
    <<R"(
`timescale 1ns / 1ps

`include "parameters.v"

module m_RR_EvaluationUnit(
	RST,
	CLK,
	request_0,
	request_1,
	grant_0,
	grant_1
);

	// Parameters
	parameter	P_ROUTER_ID = 0;
	parameter	P_CHANNEL_ID = 0;

	// Port Definition
	input RST;
	input CLK;
	input [`CHANNELS - 1 : 0] request_0;
	input [`CHANNELS - 1 : 0] request_1;
	output grant_0;
	output grant_1;

	// Variables	 
	wire [2 - 1 : 0] request_vector;
	wire [2 - 1 : 0] grant_vector;
	
	// Assignments
	// Assignments for matrixArbiter inputs
	assign request_vector[0] = (request_0 != 0);
	assign request_vector[1] = (request_1 != 0);
	// Assignments for matrixArbiter outputs
	assign grant_0 = grant_vector[0];
	assign grant_1 = grant_vector[1];

	
	// Behavior
	initial begin:Initialization
		// Empty
	end

	// Module Decleration
	m_RR_MatrixArbiter rr_matrix(
		.RST(RST)
		,.CLK(CLK)

		,.request_vector(request_vector)
		,.grant_vector(grant_vector)
	);

	
endmodule

    )";
    ofile.close();
}
void NOC_GEN::m_RR_MatrixArbiter()
{
    ofile.open( FILE_PATH + "m_RR_MatrixArbiter.v" );
    ofile
    <<R"(
`timescale 1ns / 1ps
`include "parameters.v"


module m_RR_MatrixArbiter (
		CLK,
		RST,

		request_vector,
		grant_vector
	);


	// Inputs and Outputs
	input CLK;
	input RST;

	input [2 - 1 : 0] request_vector;
	output [2 - 1 : 0] grant_vector;


	// Wire + Variables
	reg intern_SET;

	// Request_0
	wire request_0;
	// the wire grant_0 equals intern_00_and_out
	wire intern_0_or_out;
	wire intern_00_and_out;
	wire intern_01_and_out;
	wire intern_01_subU_out;

	// Request_1
	wire request_1;
	// the wire grant_1 equals intern_11_and_out
	wire intern_1_or_out;
	wire intern_10_and_out;
	wire intern_11_and_out;
	wire intern_10_subU_out;


	// Assignments
	assign request_0 = request_vector[0];
	assign request_1 = request_vector[1];

	assign grant_vector[0] = intern_00_and_out;
	assign grant_vector[1] = intern_11_and_out;

	// Request_0
	and and_00 (intern_00_and_out, request_0, ~intern_0_or_out);
	and and_01 (intern_01_and_out, request_0, intern_01_subU_out);

	// Request_1
	and and_10 (intern_10_and_out, request_1, intern_10_subU_out);
	and and_11 (intern_11_and_out, request_1, ~intern_1_or_out);

	or or_0 (intern_0_or_out 
		,intern_10_and_out 
	);
	or or_1 (intern_1_or_out 
		,intern_01_and_out 
	);


	// Behavior
	always @ (RST) begin
		if (RST) begin
			intern_SET = 1;
		end else begin
			intern_SET = 0;
		end
	end


	// Module Decleration
	// Row: 0 Column: 0 NO SUBUNIT AT THIS POSITION
	// Row: 0 Column: 1 SubUnit: 01
	m_MatrixArbiter_subUnit subU_01 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(intern_SET)
		,.g_in_1(intern_11_and_out)
		,.g_in_2(intern_00_and_out)
		,.w_out(intern_01_subU_out)
	);
	// Row: 1 Column: 0 SubUnit: 10
	m_MatrixArbiter_subUnit subU_10 (
		.RST(RST)
		,.CLK(CLK)
		,.SET(1'b0)
		,.g_in_1(intern_00_and_out)
		,.g_in_2(intern_11_and_out)
		,.w_out(intern_10_subU_out)
	);
	// Row: 1 Column: 1 NO SUBUNIT AT THIS POSITION


endmodule

    )";
    ofile.close();
}

void NOC_GEN::dc_router ()
{
	ofile.open(TCL_PATH + "dc_router.tcl");
	ofile
	<<R"(
	#-----------This is a tcl script of dc,synopsys-----------

	#-------------------------------------------------------------------------------------------
	#-----------------------1 Specify library---------------------------------------------------
	#-------------------------------------------------------------------------------------------

	)"
	<<"\t#path for designware\n"
	<<"\tset synthetic_library "+SL+"\n\n"

	<<"\t#target library\n"
	<<"\tset target_library "+TL+"\n\n"

    <<"\t#path for link_library \n"
    <<"\tset link_library "+LL+"\n\n"

	<<"\t#path for symbol_library\n"
	<<"\tset symbol_library "+SYML+"\n\n"

	<<"\t#search path\n"
	<<"\tset search_path "+SEARCH_PATH+"\n\n"

	<<R"(
	#--------------------------1.1 naming rules-----------------------------------------

	#define name rules
	define_name_rules verilog -allowed "A-Z a-z 0-9 _" -first_restricted "0-9 _ \ []" \
	-case_insensitive -max_length 30 \
	-target_bus_naming_style {%s[%d]} \
	-type net -prefix "N" -remove_chars \
	-map {{{"__","_"},{"reg_reg","r_reg"}}}


	#define bus style
	set bus_naming_style {%s[%d]}

	#visible port names
	set verilogout_show_unconnected_pins ture

	#verilog specific statements
	set verilogout_no_tri ture
	set verilogout_equation false

	#------------------------1.2 work direct-----------------------------------------

	#set your work dir
	)"

	<<"\tset my_work_dir "+ DC_PATH +"\n"

	<<R"(
	file mkdir $my_work_dir/temp/work

	define_design_lib DEFAULT -path $my_work_dir/temp/work


	#------------------------------------------------------------------------------------
	#----------------------------2 Read design and link----------------------------------
	#------------------------------------------------------------------------------------

	#set active_design mkIQRouterCore
	set active_design m_Assembled_Router
	# set active_design m_Assembled_NoC

	echo "PP60"
	# analyze and elaborate the source files
	# FINDME
	# Parameters
	analyze -f verilog -lib work $my_work_dir/vsrc/parameters.v

	# SubModules
	analyze -f verilog -lib work $my_work_dir/vsrc/m_D_FF_mod.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_D_FF_sync.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Demux_1in2out.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Mux_2in1out.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Mux_NoEncoding.v

	echo "PP65"
	# Single Modules
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Buffer.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_RoutingCalculation.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_StateMachine.v

	# Grouped Modules
	## RoundRobin for output N/S
	analyze -f verilog -lib work $my_work_dir/vsrc/m_MatrixArbiter_subUnit.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_RR_MatrixArbiter.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_RR_EvaluationUnit.v

	echo "PP70"
	## Switch Allocation
	analyze -f verilog -lib work $my_work_dir/vsrc/m_MatrixArbiter.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_EvaluationUnit.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_SwitchAllocation.v
	## Crossbar
	# analyze -f verilog -lib work $my_work_dir/vsrc/m_Switch_7in7out.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Switch_Feedback_7in7out.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Crossbar.v

	analyze -f verilog -lib work $my_work_dir/vsrc/m_Switch_Data.v 
	# special

	echo "PP75"

	# SubTopLevel Modules
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Assembled_Channel.v
	analyze -f verilog -lib work $my_work_dir/vsrc/m_Assembled_Router.v

	# Top Module
	# analyze -f verilog -lib work $my_work_dir/vsrc/m_Assembled_NoC.v


	elaborate $active_design

	set current_design  $active_design

	link

	uniquify

	# write the unmapped or the GTECH
	file mkdir $my_work_dir/temp/unmapped

	write -format ddc -hier -output $my_work_dir/temp/unmapped/$active_design.ddc

	#------------------------------------------------------------------------------------
	#---------------------------3 Define environment and constrain-----------------------
	#------------------------------------------------------------------------------------
	)"
	<<"\tset clk CLK\n"
	<<"\tset CLK_PERIOD "+CLK_PERIOD+"\n"
	<<"\tset MAX_FANOUT "+MAX_FANOUT+"\n"

	<<R"(
	create_clock  -name "$clk" -period $CLK_PERIOD [get_ports $clk]
	set_dont_touch_network [get_clocks $clk]
	set_clock_transition 0.1 [get_clocks $clk]
	set_clock_uncertainty 0.3 [get_clocks $clk]
	set_clock_latency 0.3 [get_clocks $clk]
	set_drive 0 $clk

	set_fix_hold $clk

	set_auto_disable_drc_nets -clock true -constant false
	set_max_transition 0.6 [get_designs *]
	set_max_fanout $MAX_FANOUT [get_designs *]




	#----------------------------3.1 design environment----------------------------------
	#set_min_library [list /home/greencomputing/TSMC45/Front_End/timing_power_noise/NLDM/tcbn45gsbwp_120a/tcbn45gsbwpbc.db /home/greencomputing/TSMC45/Front_End/timing_power_noise/NLDM/tcbn45gsbwp_120a/tcbn45gsbwpbc0d990d99.db] -min_version [list /home/greencomputing/TSMC45/Front_End/timing_power_noise/NLDM/tcbn45gsbwp_120a/tcbn45gsbwpwcz0d9.db /home/greencomputing/TSMC45/Front_End/timing_power_noise/NLDM/tcbn45gsbwp_120a/tcbn45gsbwpwcz0d90d9.db]

	#set_min_library /home/greencomputing/TSMC45/Front_End/timing_power_noise/NLDM/tcbn45gsbwp_120a/tcbn45gsbwpwcz0d9.db -min_version /home/greencomputing/TSMC45/Front_End/timing_power_noise/NLDM/tcbn45gsbwp_120a/tcbn45gsbwpbc.db

	#set_operating_conditions -min WCZ0D9COM -max BCCOM

	#set_wire_load_model -name TSMC8K_Lowk_Conservative -min
	#set_wire_load_model -name TSMC512K_Lowk_Aggresive -max

	set_wire_load_mode  top



	#----------------------------3.2 design constrain------------------------------------


	#   set input and output delay for all I/O
	set all_in_ex_clk [remove_from_collection [all_inputs] [get_ports $clk]]

	#set_input_delay 0.8 -max  -clock $clk   $all_in_ex_clk
	#set_input_delay 0.4 -min  -clock $clk   $all_in_ex_clk

	#set_output_delay 0.8 -max -clock $clk [all_outputs]
	#set_output_delay 0.4 -min -clock $clk [all_outputs]

	########## set the I/O attribute ###############
	set_drive 0.1 [all_inputs]
	set_fanout_load  3  [all_outputs]
	set_load 0.4 [all_outputs]      -pin_load
	set_port_fanout_number 1 [all_outputs]

	#set max fanout
	set high_fanout_net_threshold  1000

	#----------------------------3.3 verilog specific statements--------------------------
	foreach_in_collection design [ get_designs "*" ] {
	current_design $design
	set_fix_multiple_port_nets -all -buffer_constants
	}


	echo "PP80"

	current_design $active_design
	set_fix_multiple_port_nets -all -buffer_constants

	#-----------------------------3.4 Optimize strategy------------------------------------

	# Optimize the design to meet the constraints
	file mkdir $my_work_dir/temp/mapped
	file mkdir $my_work_dir/temp/dcout_files
	set_svf $my_work_dir/temp/dcout_files/$active_design.svf

	# 不允许删除常量的触发器
	set compile_seqmap_propagate_high_effort false
	set compile_seqmap_propagate_constants false


	#--------------------------------------------------------------------------------------
	#-------------------------------4 Compile----------------------------------------------
	#--------------------------------------------------------------------------------------
	#not synthesize RS filp-flop
	#set_dont_use {slow_1v08c125/DFFSRXL slow_1v08c125/DFFSRX1 slow_1v08c125/DFFSRX2 slow_1v08c125/DFFSRX4 slow_1v08c125/SDFFSRHQX1 slow_1v08c125/SDFFSRHQX2 slow_1v08c125/SDFFSRHQX4 slow_1v08c125/SDFFSRHQX8}

	ungroup -all -flatten

	compile -map_effort high -area_effort low -incremental_mapping
	# compile_ultra -timing_high_effort_script -retime

	set current_design $active_design

	#--------------------------------------------------------------------------------------
	#------------------------------5 output and save data----------------------------------
	#--------------------------------------------------------------------------------------

	# Write current design to compile.db
	change_names -rules verilog -hierarchy

	write  -format ddc -hier -output $my_work_dir/temp/mapped/$active_design.ddc
	write  -format verilog -output $my_work_dir/temp/mapped/$active_design.v

	write_sdf -version 2.1 $my_work_dir/temp/dcout_files/$active_design.sdf
	write_sdc $my_work_dir/temp/dcout_files/$active_design.sdc

	file mkdir $my_work_dir/temp/report
	set my_work_report $my_work_dir/temp/report

	echo "PP95"

	report_constraint -all_violator -verbose >> $my_work_report/report_constraint_$active_design
	report_timing -delay max >  $my_work_report/report_timing_setuptime_$active_design
	report_timing -delay min >  $my_work_report/report_timing_holdtime_$active_design
	report_area >  $my_work_report/report_area_$active_design
	report_design >  $my_work_report/report_design_$active_design
	report_net >  $my_work_report/report_net_$active_design
	report_cell >  $my_work_report/report_cell_$active_design
	report_power >  $my_work_report/report_power_$active_design
	report_power -hier >  $my_work_report/report_power_hier_$active_design
	report_power -net >  $my_work_report/report_net_power_$active_design
	report_power -cell >  $my_work_report/report_cell_power_$active_design
	report_net_fanout >  $my_work_report/report_net_fanout_$active_design
	report_annotated_transition >  $my_work_report/report_transition_$active_design

	exit
	)";
	ofile.close();

    cout << "generate dc_router.tcl successfully\n";
}


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

    
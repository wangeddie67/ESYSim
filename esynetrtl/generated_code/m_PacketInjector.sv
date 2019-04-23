
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
    
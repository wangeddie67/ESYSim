
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

    
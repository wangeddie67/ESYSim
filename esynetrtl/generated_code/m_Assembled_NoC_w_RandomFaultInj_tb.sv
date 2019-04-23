
`timescale 1ns / 1ps
`include "parameters.v"


module m_Assembled_NoC_w_RandomFaultInj_tb();

	// Parameters
	parameter P_STATUSARRAY_LENGTH = 2 ** `FLIT_ID_WIDTH;
	`ifdef FT_ECC
		parameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH;
	`else
		parameter P_DATA_WIDTH = `FLIT_WIDTH;
	`endif


	// Functions & Tasks


	// Wires & Variables
	// =General=
	reg tb_CLK_enable;
	wire tb_CLK;
	reg tb_RST;

	reg [`FLIT_ID_WIDTH - 1 : 0] tb_packetID;
	reg [`ROUTERS - 1 : 0][`FLIT_ID_WIDTH - 1 : 0] tb_ejected_id;
	reg [`ROUTERS - 1 : 0][`FLIT_DST_WIDTH - 1 : 0] tb_ejected_dst;
	reg [`ROUTERS - 1 : 0] tb_inj_ACK;
	reg [`ROUTERS - 1 : 0] tb_eje_ACK;
	reg [P_STATUSARRAY_LENGTH - 1 : 0] tb_ID_status;
	semaphore generateID;
	semaphore logInjectedData;
	semaphore logEjectedData;
	integer i;
	integer alarm_counter;
	integer tb_packets_injected;
	integer tb_packets_ejected;
	integer tb_packets_ejected_wrong;
	integer tb_ACKs_injected;
	integer tb_ACKs_ejected;
	integer tb_flits_injected;
	integer tb_flits_ejected;
	integer tb_occupied_ids;
	integer tb_active_packets;
	integer tb_active_flits;
	integer tb_active_bodyflits;
	integer tb_get_ids;
	integer tb_set_ids;
		// ===NoC===
	// NoC Local Port: 0
	wire [P_DATA_WIDTH - 1: 0] tb_r0_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r0_p0_out_data;

	wire tb_r0_p0_in_data_valid;
	wire tb_r0_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r0_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r0_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 0
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r0_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r0_p0_postDecoder_data;
	wire tb_r0_po_ecc_error;
	`endif


	// NoC Local Port: 1
	wire [P_DATA_WIDTH - 1: 0] tb_r1_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r1_p0_out_data;

	wire tb_r1_p0_in_data_valid;
	wire tb_r1_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r1_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r1_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 1
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r1_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r1_p0_postDecoder_data;
	wire tb_r1_po_ecc_error;
	`endif


	// NoC Local Port: 2
	wire [P_DATA_WIDTH - 1: 0] tb_r2_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r2_p0_out_data;

	wire tb_r2_p0_in_data_valid;
	wire tb_r2_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r2_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r2_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 2
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r2_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r2_p0_postDecoder_data;
	wire tb_r2_po_ecc_error;
	`endif


	// NoC Local Port: 3
	wire [P_DATA_WIDTH - 1: 0] tb_r3_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r3_p0_out_data;

	wire tb_r3_p0_in_data_valid;
	wire tb_r3_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r3_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r3_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 3
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r3_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r3_p0_postDecoder_data;
	wire tb_r3_po_ecc_error;
	`endif


	// NoC Local Port: 4
	wire [P_DATA_WIDTH - 1: 0] tb_r4_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r4_p0_out_data;

	wire tb_r4_p0_in_data_valid;
	wire tb_r4_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r4_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r4_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 4
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r4_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r4_p0_postDecoder_data;
	wire tb_r4_po_ecc_error;
	`endif


	// NoC Local Port: 5
	wire [P_DATA_WIDTH - 1: 0] tb_r5_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r5_p0_out_data;

	wire tb_r5_p0_in_data_valid;
	wire tb_r5_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r5_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r5_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 5
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r5_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r5_p0_postDecoder_data;
	wire tb_r5_po_ecc_error;
	`endif


	// NoC Local Port: 6
	wire [P_DATA_WIDTH - 1: 0] tb_r6_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r6_p0_out_data;

	wire tb_r6_p0_in_data_valid;
	wire tb_r6_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r6_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r6_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 6
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r6_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r6_p0_postDecoder_data;
	wire tb_r6_po_ecc_error;
	`endif


	// NoC Local Port: 7
	wire [P_DATA_WIDTH - 1: 0] tb_r7_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r7_p0_out_data;

	wire tb_r7_p0_in_data_valid;
	wire tb_r7_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r7_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r7_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 7
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r7_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r7_p0_postDecoder_data;
	wire tb_r7_po_ecc_error;
	`endif


	// NoC Local Port: 8
	wire [P_DATA_WIDTH - 1: 0] tb_r8_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r8_p0_out_data;

	wire tb_r8_p0_in_data_valid;
	wire tb_r8_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r8_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r8_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 8
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r8_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r8_p0_postDecoder_data;
	wire tb_r8_po_ecc_error;
	`endif


	// NoC Local Port: 9
	wire [P_DATA_WIDTH - 1: 0] tb_r9_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r9_p0_out_data;

	wire tb_r9_p0_in_data_valid;
	wire tb_r9_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r9_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r9_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 9
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r9_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r9_p0_postDecoder_data;
	wire tb_r9_po_ecc_error;
	`endif


	// NoC Local Port: 10
	wire [P_DATA_WIDTH - 1: 0] tb_r10_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r10_p0_out_data;

	wire tb_r10_p0_in_data_valid;
	wire tb_r10_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r10_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r10_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 10
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r10_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r10_p0_postDecoder_data;
	wire tb_r10_po_ecc_error;
	`endif


	// NoC Local Port: 11
	wire [P_DATA_WIDTH - 1: 0] tb_r11_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r11_p0_out_data;

	wire tb_r11_p0_in_data_valid;
	wire tb_r11_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r11_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r11_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 11
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r11_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r11_p0_postDecoder_data;
	wire tb_r11_po_ecc_error;
	`endif


	// NoC Local Port: 12
	wire [P_DATA_WIDTH - 1: 0] tb_r12_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r12_p0_out_data;

	wire tb_r12_p0_in_data_valid;
	wire tb_r12_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r12_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r12_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 12
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r12_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r12_p0_postDecoder_data;
	wire tb_r12_po_ecc_error;
	`endif


	// NoC Local Port: 13
	wire [P_DATA_WIDTH - 1: 0] tb_r13_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r13_p0_out_data;

	wire tb_r13_p0_in_data_valid;
	wire tb_r13_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r13_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r13_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 13
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r13_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r13_p0_postDecoder_data;
	wire tb_r13_po_ecc_error;
	`endif


	// NoC Local Port: 14
	wire [P_DATA_WIDTH - 1: 0] tb_r14_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r14_p0_out_data;

	wire tb_r14_p0_in_data_valid;
	wire tb_r14_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r14_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r14_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 14
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r14_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r14_p0_postDecoder_data;
	wire tb_r14_po_ecc_error;
	`endif


	// NoC Local Port: 15
	wire [P_DATA_WIDTH - 1: 0] tb_r15_p0_in_data;
	wire [P_DATA_WIDTH - 1: 0] tb_r15_p0_out_data;

	wire tb_r15_p0_in_data_valid;
	wire tb_r15_p0_out_data_valid;

	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r15_p0_out_credits_feedback;
	`ifdef FT_RETRANSMISSION
	wire [`BUFFERSIZE_WIDTH - 1: 0] tb_r15_p0_in_credits_feedback;
	`endif

	// ECC signals for NoC Local Port: 15
	`ifdef FT_ECC
	wire [`FLIT_WIDTH - 1: 0] tb_r15_p0_preEncoder_data;
	wire [`FLIT_WIDTH - 1: 0] tb_r15_p0_postDecoder_data;
	wire tb_r15_po_ecc_error;
	`endif



	// Variable to set credits if there is no RetransmissionUnit
	`ifndef FT_RETRANSMISSION
	reg [`ROUTERS - 1 : 0][`BUFFERSIZE_WIDTH - 1:0] tb_r_p0_in_credits_feedback;	 // Set the in credits to 4
	`endif

			// ===Random Injectors===
	reg [`ROUTERS - 1 : 0] tb_PacketInj_enable;

			wire [`ROUTERS - 1 : 0] tb_PacketInj_get_id_enable;
	reg [`ROUTERS - 1 : 0] tb_PacketInj_get_id_grant;
	reg [`ROUTERS - 1 : 0] [`FLIT_ID_WIDTH - 1 : 0] tb_PacketInj_get_id_data;
	wire [`ROUTERS - 1 : 0] [`FLIT_WIDTH - 1: 0] tb_PacketEject_data;
	`ifdef FT_RETRANSMISSION
	wire [`ROUTERS - 1 : 0] tb_PacketInj_set_id_enable;
	reg [`ROUTERS - 1 : 0] tb_PacketInj_set_id_grant;
	wire [`ROUTERS - 1 : 0] [`FLIT_ID_WIDTH - 1 : 0] tb_PacketInj_set_id_data;
	`endif		
		// Assignments
	`ifdef FT_ECC
	assign tb_PacketEject_data[0] = tb_r0_p0_postDecoder_data;
	assign tb_PacketEject_data[1] = tb_r1_p0_postDecoder_data;
	assign tb_PacketEject_data[2] = tb_r2_p0_postDecoder_data;
	assign tb_PacketEject_data[3] = tb_r3_p0_postDecoder_data;
	assign tb_PacketEject_data[4] = tb_r4_p0_postDecoder_data;
	assign tb_PacketEject_data[5] = tb_r5_p0_postDecoder_data;
	assign tb_PacketEject_data[6] = tb_r6_p0_postDecoder_data;
	assign tb_PacketEject_data[7] = tb_r7_p0_postDecoder_data;
	assign tb_PacketEject_data[8] = tb_r8_p0_postDecoder_data;
	assign tb_PacketEject_data[9] = tb_r9_p0_postDecoder_data;
	assign tb_PacketEject_data[10] = tb_r10_p0_postDecoder_data;
	assign tb_PacketEject_data[11] = tb_r11_p0_postDecoder_data;
	assign tb_PacketEject_data[12] = tb_r12_p0_postDecoder_data;
	assign tb_PacketEject_data[13] = tb_r13_p0_postDecoder_data;
	assign tb_PacketEject_data[14] = tb_r14_p0_postDecoder_data;
	assign tb_PacketEject_data[15] = tb_r15_p0_postDecoder_data;
	`else
	assign tb_PacketEject_data[0] = tb_r0_p0_out_data;
	assign tb_PacketEject_data[1] = tb_r1_p0_out_data;
	assign tb_PacketEject_data[2] = tb_r2_p0_out_data;
	assign tb_PacketEject_data[3] = tb_r3_p0_out_data;
	assign tb_PacketEject_data[4] = tb_r4_p0_out_data;
	assign tb_PacketEject_data[5] = tb_r5_p0_out_data;
	assign tb_PacketEject_data[6] = tb_r6_p0_out_data;
	assign tb_PacketEject_data[7] = tb_r7_p0_out_data;
	assign tb_PacketEject_data[8] = tb_r8_p0_out_data;
	assign tb_PacketEject_data[9] = tb_r9_p0_out_data;
	assign tb_PacketEject_data[10] = tb_r10_p0_out_data;
	assign tb_PacketEject_data[11] = tb_r11_p0_out_data;
	assign tb_PacketEject_data[12] = tb_r12_p0_out_data;
	assign tb_PacketEject_data[13] = tb_r13_p0_out_data;
	assign tb_PacketEject_data[14] = tb_r14_p0_out_data;
	assign tb_PacketEject_data[15] = tb_r15_p0_out_data;
	`endif

	assign tb_active_packets = tb_packets_injected - tb_packets_ejected - tb_packets_ejected_wrong;
	assign tb_active_flits = tb_flits_injected - tb_flits_ejected;
	assign tb_active_bodyflits = tb_active_flits - (tb_active_packets * 2);  // HF + TF



	// Behaviour
	initial begin
		// Semaphores
		generateID = new(1);
		logInjectedData = new(1);
		logEjectedData = new(1);
		// PacketID
		tb_occupied_ids = 0;
		tb_set_ids = 0;
		tb_get_ids = 0;
		tb_packetID = 0;
		alarm_counter = 0;
		// Counting variables
		tb_packets_ejected = 0;
		tb_packets_injected = 0;
		tb_ACKs_ejected = 0;
		tb_ACKs_injected = 0;
		tb_flits_injected = 0;
		tb_flits_ejected = 0;
		tb_packets_ejected_wrong = 0;

		// Destination check variables
		for(i=0; i < P_STATUSARRAY_LENGTH; i = i + 1) begin
			tb_ejected_id[i] <= 0;
			tb_ejected_dst[i] <= 0;
		end

		`ifdef FT_RETRANSMISSION
		for(i=0; i < P_STATUSARRAY_LENGTH; i = i + 1) begin
			tb_inj_ACK[i] <= 0;
			tb_eje_ACK[i] <= 0;
		end
		`endif
		// Testbench initialization
	tb_CLK_enable = 0;

	// Changing the ingoing credit_feedback to 4 -> the extern connection can reveice "all" flits
	`ifndef FT_RETRANSMISSION
	for(i=0; i < `ROUTERS; i = i + 1) begin
	tb_r_p0_in_credits_feedback[i] = 4;
	end
	`endif


		// Sets all possible flits id as "available"/not in the network
		for(i=0; i < P_STATUSARRAY_LENGTH; i = i + 1) begin
			tb_ID_status[i] = 1;
		end

		// Initiate phase of all the Random Injectors
		for(i=0; i < `ROUTERS; i = i + 1) begin
			tb_PacketInj_get_id_grant[i] = 0;
			tb_PacketInj_get_id_data[i] = 0;
			tb_PacketInj_enable[i] = 1;
		end

		// // // // // // Additional Behavior
		// Switching all Random Injectors off
		// for(i=0; i < `ROUTERS; i = i + 1) begin
			// tb_PacketInj_enable[i] = 0;
		// end
		//tb_PacketInj_enable[26] = 1;
		//tb_PacketInj_enable[27] = 1;
		//tb_PacketInj_enable[28] = 1;
		# 10;
		# 10;
		tb_CLK_enable = 1;
		tb_RST = 1;
		# 10;
		tb_RST = 0;
		#3000;
		for(i=0; i < `ROUTERS; i = i + 1) begin
			tb_PacketInj_enable[i] = 0;
		end
		#8000;
		$finish;
	end	

		// Additional Interface Logic for Router: 0
	// FlitIDProvider-Routine for PacketInjector: 0
	always @ (tb_PacketInj_get_id_enable[0]) begin
		if (tb_PacketInj_get_id_enable[0]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 0);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[0] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[0] = 1;
		end else begin
			tb_PacketInj_get_id_grant[0] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 0
	always @ (tb_PacketInj_set_id_enable[0]) begin
		if (tb_PacketInj_set_id_enable[0]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[0]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[0] = 1;
		end else begin
			tb_PacketInj_set_id_grant[0] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 0
	always @ (posedge tb_CLK) begin
		if (tb_r0_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r0_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r0_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[0] = 1;
				end else begin
					tb_inj_ACK[0] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r0_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r0_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[0]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 0);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 0
	always @ (posedge tb_CLK) begin
		if (tb_r0_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[0][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[0] = tb_PacketEject_data[0][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[0][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[0] = 1;
				end else begin
					tb_eje_ACK[0] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[0][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[0][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[0]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[0] != 6'd0) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 0);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 0
	always @ (posedge tb_CLK) begin
		if (tb_r0_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[0][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[0] = tb_PacketEject_data[0][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[0][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[0][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[0]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 0);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 1
	// FlitIDProvider-Routine for PacketInjector: 1
	always @ (tb_PacketInj_get_id_enable[1]) begin
		if (tb_PacketInj_get_id_enable[1]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 1);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[1] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[1] = 1;
		end else begin
			tb_PacketInj_get_id_grant[1] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 1
	always @ (tb_PacketInj_set_id_enable[1]) begin
		if (tb_PacketInj_set_id_enable[1]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[1]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[1] = 1;
		end else begin
			tb_PacketInj_set_id_grant[1] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 1
	always @ (posedge tb_CLK) begin
		if (tb_r1_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r1_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r1_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[1] = 1;
				end else begin
					tb_inj_ACK[1] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r1_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r1_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[1]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 1);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 1
	always @ (posedge tb_CLK) begin
		if (tb_r1_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[1][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[1] = tb_PacketEject_data[1][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[1][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[1] = 1;
				end else begin
					tb_eje_ACK[1] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[1][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[1][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[1]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[1] != 6'd1) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 1);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 1
	always @ (posedge tb_CLK) begin
		if (tb_r1_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[1][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[1] = tb_PacketEject_data[1][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[1][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[1][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[1]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 1);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 2
	// FlitIDProvider-Routine for PacketInjector: 2
	always @ (tb_PacketInj_get_id_enable[2]) begin
		if (tb_PacketInj_get_id_enable[2]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 2);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[2] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[2] = 1;
		end else begin
			tb_PacketInj_get_id_grant[2] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 2
	always @ (tb_PacketInj_set_id_enable[2]) begin
		if (tb_PacketInj_set_id_enable[2]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[2]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[2] = 1;
		end else begin
			tb_PacketInj_set_id_grant[2] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 2
	always @ (posedge tb_CLK) begin
		if (tb_r2_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r2_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r2_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[2] = 1;
				end else begin
					tb_inj_ACK[2] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r2_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r2_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[2]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 2);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 2
	always @ (posedge tb_CLK) begin
		if (tb_r2_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[2][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[2] = tb_PacketEject_data[2][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[2][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[2] = 1;
				end else begin
					tb_eje_ACK[2] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[2][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[2][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[2]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[2] != 6'd2) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 2);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 2
	always @ (posedge tb_CLK) begin
		if (tb_r2_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[2][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[2] = tb_PacketEject_data[2][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[2][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[2][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[2]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 2);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 3
	// FlitIDProvider-Routine for PacketInjector: 3
	always @ (tb_PacketInj_get_id_enable[3]) begin
		if (tb_PacketInj_get_id_enable[3]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 3);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[3] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[3] = 1;
		end else begin
			tb_PacketInj_get_id_grant[3] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 3
	always @ (tb_PacketInj_set_id_enable[3]) begin
		if (tb_PacketInj_set_id_enable[3]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[3]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[3] = 1;
		end else begin
			tb_PacketInj_set_id_grant[3] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 3
	always @ (posedge tb_CLK) begin
		if (tb_r3_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r3_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r3_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[3] = 1;
				end else begin
					tb_inj_ACK[3] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r3_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r3_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[3]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 3);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 3
	always @ (posedge tb_CLK) begin
		if (tb_r3_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[3][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[3] = tb_PacketEject_data[3][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[3][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[3] = 1;
				end else begin
					tb_eje_ACK[3] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[3][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[3][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[3]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[3] != 6'd3) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 3);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 3
	always @ (posedge tb_CLK) begin
		if (tb_r3_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[3][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[3] = tb_PacketEject_data[3][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[3][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[3][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[3]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 3);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 4
	// FlitIDProvider-Routine for PacketInjector: 4
	always @ (tb_PacketInj_get_id_enable[4]) begin
		if (tb_PacketInj_get_id_enable[4]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 4);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[4] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[4] = 1;
		end else begin
			tb_PacketInj_get_id_grant[4] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 4
	always @ (tb_PacketInj_set_id_enable[4]) begin
		if (tb_PacketInj_set_id_enable[4]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[4]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[4] = 1;
		end else begin
			tb_PacketInj_set_id_grant[4] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 4
	always @ (posedge tb_CLK) begin
		if (tb_r4_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r4_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r4_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[4] = 1;
				end else begin
					tb_inj_ACK[4] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r4_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r4_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[4]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 4);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 4
	always @ (posedge tb_CLK) begin
		if (tb_r4_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[4][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[4] = tb_PacketEject_data[4][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[4][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[4] = 1;
				end else begin
					tb_eje_ACK[4] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[4][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[4][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[4]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[4] != 6'd4) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 4);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 4
	always @ (posedge tb_CLK) begin
		if (tb_r4_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[4][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[4] = tb_PacketEject_data[4][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[4][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[4][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[4]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 4);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 5
	// FlitIDProvider-Routine for PacketInjector: 5
	always @ (tb_PacketInj_get_id_enable[5]) begin
		if (tb_PacketInj_get_id_enable[5]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 5);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[5] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[5] = 1;
		end else begin
			tb_PacketInj_get_id_grant[5] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 5
	always @ (tb_PacketInj_set_id_enable[5]) begin
		if (tb_PacketInj_set_id_enable[5]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[5]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[5] = 1;
		end else begin
			tb_PacketInj_set_id_grant[5] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 5
	always @ (posedge tb_CLK) begin
		if (tb_r5_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r5_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r5_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[5] = 1;
				end else begin
					tb_inj_ACK[5] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r5_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r5_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[5]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 5);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 5
	always @ (posedge tb_CLK) begin
		if (tb_r5_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[5][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[5] = tb_PacketEject_data[5][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[5][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[5] = 1;
				end else begin
					tb_eje_ACK[5] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[5][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[5][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[5]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[5] != 6'd5) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 5);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 5
	always @ (posedge tb_CLK) begin
		if (tb_r5_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[5][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[5] = tb_PacketEject_data[5][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[5][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[5][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[5]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 5);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 6
	// FlitIDProvider-Routine for PacketInjector: 6
	always @ (tb_PacketInj_get_id_enable[6]) begin
		if (tb_PacketInj_get_id_enable[6]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 6);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[6] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[6] = 1;
		end else begin
			tb_PacketInj_get_id_grant[6] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 6
	always @ (tb_PacketInj_set_id_enable[6]) begin
		if (tb_PacketInj_set_id_enable[6]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[6]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[6] = 1;
		end else begin
			tb_PacketInj_set_id_grant[6] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 6
	always @ (posedge tb_CLK) begin
		if (tb_r6_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r6_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r6_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[6] = 1;
				end else begin
					tb_inj_ACK[6] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r6_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r6_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[6]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 6);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 6
	always @ (posedge tb_CLK) begin
		if (tb_r6_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[6][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[6] = tb_PacketEject_data[6][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[6][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[6] = 1;
				end else begin
					tb_eje_ACK[6] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[6][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[6][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[6]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[6] != 6'd6) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 6);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 6
	always @ (posedge tb_CLK) begin
		if (tb_r6_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[6][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[6] = tb_PacketEject_data[6][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[6][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[6][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[6]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 6);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 7
	// FlitIDProvider-Routine for PacketInjector: 7
	always @ (tb_PacketInj_get_id_enable[7]) begin
		if (tb_PacketInj_get_id_enable[7]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 7);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[7] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[7] = 1;
		end else begin
			tb_PacketInj_get_id_grant[7] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 7
	always @ (tb_PacketInj_set_id_enable[7]) begin
		if (tb_PacketInj_set_id_enable[7]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[7]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[7] = 1;
		end else begin
			tb_PacketInj_set_id_grant[7] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 7
	always @ (posedge tb_CLK) begin
		if (tb_r7_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r7_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r7_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[7] = 1;
				end else begin
					tb_inj_ACK[7] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r7_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r7_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[7]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 7);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 7
	always @ (posedge tb_CLK) begin
		if (tb_r7_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[7][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[7] = tb_PacketEject_data[7][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[7][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[7] = 1;
				end else begin
					tb_eje_ACK[7] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[7][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[7][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[7]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[7] != 6'd7) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 7);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 7
	always @ (posedge tb_CLK) begin
		if (tb_r7_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[7][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[7] = tb_PacketEject_data[7][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[7][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[7][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[7]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 7);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 8
	// FlitIDProvider-Routine for PacketInjector: 8
	always @ (tb_PacketInj_get_id_enable[8]) begin
		if (tb_PacketInj_get_id_enable[8]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 8);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[8] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[8] = 1;
		end else begin
			tb_PacketInj_get_id_grant[8] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 8
	always @ (tb_PacketInj_set_id_enable[8]) begin
		if (tb_PacketInj_set_id_enable[8]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[8]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[8] = 1;
		end else begin
			tb_PacketInj_set_id_grant[8] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 8
	always @ (posedge tb_CLK) begin
		if (tb_r8_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r8_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r8_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[8] = 1;
				end else begin
					tb_inj_ACK[8] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r8_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r8_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[8]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 8);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 8
	always @ (posedge tb_CLK) begin
		if (tb_r8_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[8][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[8] = tb_PacketEject_data[8][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[8][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[8] = 1;
				end else begin
					tb_eje_ACK[8] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[8][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[8][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[8]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[8] != 6'd8) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 8);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 8
	always @ (posedge tb_CLK) begin
		if (tb_r8_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[8][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[8] = tb_PacketEject_data[8][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[8][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[8][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[8]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 8);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 9
	// FlitIDProvider-Routine for PacketInjector: 9
	always @ (tb_PacketInj_get_id_enable[9]) begin
		if (tb_PacketInj_get_id_enable[9]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 9);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[9] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[9] = 1;
		end else begin
			tb_PacketInj_get_id_grant[9] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 9
	always @ (tb_PacketInj_set_id_enable[9]) begin
		if (tb_PacketInj_set_id_enable[9]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[9]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[9] = 1;
		end else begin
			tb_PacketInj_set_id_grant[9] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 9
	always @ (posedge tb_CLK) begin
		if (tb_r9_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r9_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r9_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[9] = 1;
				end else begin
					tb_inj_ACK[9] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r9_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r9_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[9]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 9);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 9
	always @ (posedge tb_CLK) begin
		if (tb_r9_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[9][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[9] = tb_PacketEject_data[9][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[9][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[9] = 1;
				end else begin
					tb_eje_ACK[9] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[9][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[9][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[9]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[9] != 6'd9) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 9);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 9
	always @ (posedge tb_CLK) begin
		if (tb_r9_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[9][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[9] = tb_PacketEject_data[9][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[9][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[9][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[9]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 9);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 10
	// FlitIDProvider-Routine for PacketInjector: 10
	always @ (tb_PacketInj_get_id_enable[10]) begin
		if (tb_PacketInj_get_id_enable[10]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 10);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[10] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[10] = 1;
		end else begin
			tb_PacketInj_get_id_grant[10] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 10
	always @ (tb_PacketInj_set_id_enable[10]) begin
		if (tb_PacketInj_set_id_enable[10]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[10]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[10] = 1;
		end else begin
			tb_PacketInj_set_id_grant[10] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 10
	always @ (posedge tb_CLK) begin
		if (tb_r10_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r10_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r10_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[10] = 1;
				end else begin
					tb_inj_ACK[10] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r10_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r10_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[10]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 10);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 10
	always @ (posedge tb_CLK) begin
		if (tb_r10_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[10][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[10] = tb_PacketEject_data[10][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[10][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[10] = 1;
				end else begin
					tb_eje_ACK[10] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[10][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[10][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[10]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[10] != 6'd10) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 10);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 10
	always @ (posedge tb_CLK) begin
		if (tb_r10_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[10][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[10] = tb_PacketEject_data[10][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[10][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[10][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[10]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 10);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 11
	// FlitIDProvider-Routine for PacketInjector: 11
	always @ (tb_PacketInj_get_id_enable[11]) begin
		if (tb_PacketInj_get_id_enable[11]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 11);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[11] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[11] = 1;
		end else begin
			tb_PacketInj_get_id_grant[11] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 11
	always @ (tb_PacketInj_set_id_enable[11]) begin
		if (tb_PacketInj_set_id_enable[11]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[11]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[11] = 1;
		end else begin
			tb_PacketInj_set_id_grant[11] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 11
	always @ (posedge tb_CLK) begin
		if (tb_r11_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r11_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r11_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[11] = 1;
				end else begin
					tb_inj_ACK[11] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r11_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r11_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[11]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 11);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 11
	always @ (posedge tb_CLK) begin
		if (tb_r11_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[11][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[11] = tb_PacketEject_data[11][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[11][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[11] = 1;
				end else begin
					tb_eje_ACK[11] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[11][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[11][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[11]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[11] != 6'd11) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 11);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 11
	always @ (posedge tb_CLK) begin
		if (tb_r11_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[11][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[11] = tb_PacketEject_data[11][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[11][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[11][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[11]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 11);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 12
	// FlitIDProvider-Routine for PacketInjector: 12
	always @ (tb_PacketInj_get_id_enable[12]) begin
		if (tb_PacketInj_get_id_enable[12]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 12);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[12] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[12] = 1;
		end else begin
			tb_PacketInj_get_id_grant[12] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 12
	always @ (tb_PacketInj_set_id_enable[12]) begin
		if (tb_PacketInj_set_id_enable[12]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[12]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[12] = 1;
		end else begin
			tb_PacketInj_set_id_grant[12] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 12
	always @ (posedge tb_CLK) begin
		if (tb_r12_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r12_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r12_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[12] = 1;
				end else begin
					tb_inj_ACK[12] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r12_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r12_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[12]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 12);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 12
	always @ (posedge tb_CLK) begin
		if (tb_r12_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[12][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[12] = tb_PacketEject_data[12][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[12][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[12] = 1;
				end else begin
					tb_eje_ACK[12] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[12][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[12][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[12]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[12] != 6'd12) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 12);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 12
	always @ (posedge tb_CLK) begin
		if (tb_r12_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[12][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[12] = tb_PacketEject_data[12][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[12][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[12][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[12]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 12);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 13
	// FlitIDProvider-Routine for PacketInjector: 13
	always @ (tb_PacketInj_get_id_enable[13]) begin
		if (tb_PacketInj_get_id_enable[13]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 13);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[13] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[13] = 1;
		end else begin
			tb_PacketInj_get_id_grant[13] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 13
	always @ (tb_PacketInj_set_id_enable[13]) begin
		if (tb_PacketInj_set_id_enable[13]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[13]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[13] = 1;
		end else begin
			tb_PacketInj_set_id_grant[13] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 13
	always @ (posedge tb_CLK) begin
		if (tb_r13_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r13_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r13_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[13] = 1;
				end else begin
					tb_inj_ACK[13] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r13_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r13_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[13]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 13);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 13
	always @ (posedge tb_CLK) begin
		if (tb_r13_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[13][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[13] = tb_PacketEject_data[13][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[13][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[13] = 1;
				end else begin
					tb_eje_ACK[13] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[13][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[13][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[13]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[13] != 6'd13) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 13);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 13
	always @ (posedge tb_CLK) begin
		if (tb_r13_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[13][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[13] = tb_PacketEject_data[13][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[13][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[13][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[13]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 13);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 14
	// FlitIDProvider-Routine for PacketInjector: 14
	always @ (tb_PacketInj_get_id_enable[14]) begin
		if (tb_PacketInj_get_id_enable[14]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 14);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[14] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[14] = 1;
		end else begin
			tb_PacketInj_get_id_grant[14] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 14
	always @ (tb_PacketInj_set_id_enable[14]) begin
		if (tb_PacketInj_set_id_enable[14]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[14]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[14] = 1;
		end else begin
			tb_PacketInj_set_id_grant[14] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 14
	always @ (posedge tb_CLK) begin
		if (tb_r14_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r14_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r14_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[14] = 1;
				end else begin
					tb_inj_ACK[14] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r14_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r14_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[14]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 14);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 14
	always @ (posedge tb_CLK) begin
		if (tb_r14_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[14][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[14] = tb_PacketEject_data[14][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[14][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[14] = 1;
				end else begin
					tb_eje_ACK[14] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[14][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[14][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[14]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[14] != 6'd14) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 14);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 14
	always @ (posedge tb_CLK) begin
		if (tb_r14_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[14][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[14] = tb_PacketEject_data[14][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[14][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[14][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[14]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 14);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// Additional Interface Logic for Router: 15
	// FlitIDProvider-Routine for PacketInjector: 15
	always @ (tb_PacketInj_get_id_enable[15]) begin
		if (tb_PacketInj_get_id_enable[15]) begin
			generateID.get(1);
			alarm_counter = 0;
			// Control if packet id is already in the network, if so the suggested id gets incremented
			while(tb_ID_status[tb_packetID] == 0) begin
				tb_packetID = tb_packetID + 1;
				alarm_counter = alarm_counter + 1;
				if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin
					$display("TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!", $time, 15);
				end
			end
			// The chosen id gets deactivated in the id array before it gets provided to the Random Injector
			tb_ID_status[tb_packetID] = 0;
			tb_occupied_ids = tb_occupied_ids + 1;
			tb_get_ids = tb_get_ids + 1;
			tb_PacketInj_get_id_data[15] = tb_packetID;
			generateID.put(1);
			tb_PacketInj_get_id_grant[15] = 1;
		end else begin
			tb_PacketInj_get_id_grant[15] = 0;
		end
	end
	`ifdef FT_RETRANSMISSION
	// FlitIDRecycler-Routine for PacketInjector: 15
	always @ (tb_PacketInj_set_id_enable[15]) begin
		if (tb_PacketInj_set_id_enable[15]) begin
			// The set id gets activated again because it is now free for reuse
			generateID.get(1);
			tb_ID_status[tb_PacketInj_set_id_data[15]] = 1;
			tb_occupied_ids = tb_occupied_ids - 1;
			tb_set_ids = tb_set_ids + 1;
			generateID.put(1);
			tb_PacketInj_set_id_grant[15] = 1;
		end else begin
			tb_PacketInj_set_id_grant[15] = 0;
		end
	end
	`endif

	// PacketInject Counter for NoC Interface 15
	always @ (posedge tb_CLK) begin
		if (tb_r15_p0_in_data_valid) begin
			logInjectedData.get(1);
			// Packet/Flit injection should be without any fault due to being in the testbench

			// Check for Headflit
			if (tb_r15_p0_in_data[33:32] == `FLIT_HEADFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_r15_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet
					tb_inj_ACK[15] = 1;
				end else begin
					tb_inj_ACK[15] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_r15_p0_in_data[33:32] == `FLIT_BODYFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_r15_p0_in_data[33:32] == `FLIT_TAILFLIT) begin
				tb_flits_injected = tb_flits_injected + 1;
				tb_packets_injected = tb_packets_injected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_inj_ACK[15]) begin
					tb_ACKs_injected = tb_ACKs_injected + 1;
				end
				`endif

			end else begin
				$display("TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!", $time, 15);
			end

			logInjectedData.put(1);
		end
	end

	// PacketEject Counter for NoC Interface 15
	always @ (posedge tb_CLK) begin
		if (tb_r15_p0_out_data_valid) begin
			logEjectedData.get(1);
			// How to handle faults?

			// Check for Headflit
			if (tb_PacketEject_data[15][33:32] == `FLIT_HEADFLIT) begin
				tb_ejected_dst[15] = tb_PacketEject_data[15][25:20];
				tb_flits_ejected = tb_flits_ejected + 1;
				`ifdef FT_RETRANSMISSION
				if (tb_PacketEject_data[15][0] == 1'b1) begin         // Flag for ACK packet
					tb_eje_ACK[15] = 1;
				end else begin
					tb_eje_ACK[15] = 0;
				end
				`endif

			// Check for Bodyflit
			end else if (tb_PacketEject_data[15][33:32] == `FLIT_BODYFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[15][33:32] == `FLIT_TAILFLIT) begin
				tb_flits_ejected = tb_flits_ejected + 1;
				tb_packets_ejected = tb_packets_ejected + 1;

				`ifdef FT_RETRANSMISSION
				if (tb_eje_ACK[15]) begin
					tb_ACKs_ejected = tb_ACKs_ejected + 1;
				end
				`endif

				// Check if the flit arrived at the correct position
				if (tb_ejected_dst[15] != 6'd15) begin
					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;
				end

			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 15);
			end

			logEjectedData.put(1);
		end
	end

	`ifndef FT_RETRANSMISSION
	// // PacketEjectHandler for NoC Interface 15
	always @ (posedge tb_CLK) begin
		if (tb_r15_p0_out_data_valid) begin
			logEjectedData.get(1);

			// Check for Headflit
			if (tb_PacketEject_data[15][33:32] == `FLIT_HEADFLIT) begin
				// Expecting that the head flit data is correct
				tb_ejected_id[15] = tb_PacketEject_data[15][15:4];

			// Check for Bodyflit
			end else if (tb_PacketEject_data[15][33:32] == `FLIT_BODYFLIT) begin

			// Check for Tailflit + Additional Logic
			end else if (tb_PacketEject_data[15][33:32] == `FLIT_TAILFLIT) begin
				// The ejected id gets activated again because it is now free for reuse
				generateID.get(1);
				tb_ID_status[tb_ejected_id[15]] = 1;
				tb_set_ids = tb_set_ids + 1;
				tb_occupied_ids = tb_occupied_ids - 1;
				generateID.put(1);
			end else begin
				$display("TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!", $time, 15);
			end
			logEjectedData.put(1);
		end
	end
	`endif
	// MODULE DECLERATION
	// Network On Chip 
	m_Assembled_NoC noc1 (
	//Router 0),
	.r0_p0_in_data(tb_r0_p0_in_data),
	.r0_p0_out_data(tb_r0_p0_out_data),
	.r0_p0_in_data_valid(tb_r0_p0_in_data_valid),
	.r0_p0_out_data_valid(tb_r0_p0_out_data_valid),
	.r0_p0_out_credits_feedback(tb_r0_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r0_p0_in_credits_feedback(tb_r0_p0_in_credits_feedback),
	`else
	.r0_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[0]),
	`endif
	
	
	//Router 1),
	.r1_p0_in_data(tb_r1_p0_in_data),
	.r1_p0_out_data(tb_r1_p0_out_data),
	.r1_p0_in_data_valid(tb_r1_p0_in_data_valid),
	.r1_p0_out_data_valid(tb_r1_p0_out_data_valid),
	.r1_p0_out_credits_feedback(tb_r1_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r1_p0_in_credits_feedback(tb_r1_p0_in_credits_feedback),
	`else
	.r1_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[1]),
	`endif
	
	
	//Router 2),
	.r2_p0_in_data(tb_r2_p0_in_data),
	.r2_p0_out_data(tb_r2_p0_out_data),
	.r2_p0_in_data_valid(tb_r2_p0_in_data_valid),
	.r2_p0_out_data_valid(tb_r2_p0_out_data_valid),
	.r2_p0_out_credits_feedback(tb_r2_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r2_p0_in_credits_feedback(tb_r2_p0_in_credits_feedback),
	`else
	.r2_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[2]),
	`endif
	
	
	//Router 3),
	.r3_p0_in_data(tb_r3_p0_in_data),
	.r3_p0_out_data(tb_r3_p0_out_data),
	.r3_p0_in_data_valid(tb_r3_p0_in_data_valid),
	.r3_p0_out_data_valid(tb_r3_p0_out_data_valid),
	.r3_p0_out_credits_feedback(tb_r3_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r3_p0_in_credits_feedback(tb_r3_p0_in_credits_feedback),
	`else
	.r3_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[3]),
	`endif
	
	
	//Router 4),
	.r4_p0_in_data(tb_r4_p0_in_data),
	.r4_p0_out_data(tb_r4_p0_out_data),
	.r4_p0_in_data_valid(tb_r4_p0_in_data_valid),
	.r4_p0_out_data_valid(tb_r4_p0_out_data_valid),
	.r4_p0_out_credits_feedback(tb_r4_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r4_p0_in_credits_feedback(tb_r4_p0_in_credits_feedback),
	`else
	.r4_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[4]),
	`endif
	
	
	//Router 5),
	.r5_p0_in_data(tb_r5_p0_in_data),
	.r5_p0_out_data(tb_r5_p0_out_data),
	.r5_p0_in_data_valid(tb_r5_p0_in_data_valid),
	.r5_p0_out_data_valid(tb_r5_p0_out_data_valid),
	.r5_p0_out_credits_feedback(tb_r5_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r5_p0_in_credits_feedback(tb_r5_p0_in_credits_feedback),
	`else
	.r5_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[5]),
	`endif
	
	
	//Router 6),
	.r6_p0_in_data(tb_r6_p0_in_data),
	.r6_p0_out_data(tb_r6_p0_out_data),
	.r6_p0_in_data_valid(tb_r6_p0_in_data_valid),
	.r6_p0_out_data_valid(tb_r6_p0_out_data_valid),
	.r6_p0_out_credits_feedback(tb_r6_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r6_p0_in_credits_feedback(tb_r6_p0_in_credits_feedback),
	`else
	.r6_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[6]),
	`endif
	
	
	//Router 7),
	.r7_p0_in_data(tb_r7_p0_in_data),
	.r7_p0_out_data(tb_r7_p0_out_data),
	.r7_p0_in_data_valid(tb_r7_p0_in_data_valid),
	.r7_p0_out_data_valid(tb_r7_p0_out_data_valid),
	.r7_p0_out_credits_feedback(tb_r7_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r7_p0_in_credits_feedback(tb_r7_p0_in_credits_feedback),
	`else
	.r7_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[7]),
	`endif
	
	
	//Router 8),
	.r8_p0_in_data(tb_r8_p0_in_data),
	.r8_p0_out_data(tb_r8_p0_out_data),
	.r8_p0_in_data_valid(tb_r8_p0_in_data_valid),
	.r8_p0_out_data_valid(tb_r8_p0_out_data_valid),
	.r8_p0_out_credits_feedback(tb_r8_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r8_p0_in_credits_feedback(tb_r8_p0_in_credits_feedback),
	`else
	.r8_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[8]),
	`endif
	
	
	//Router 9),
	.r9_p0_in_data(tb_r9_p0_in_data),
	.r9_p0_out_data(tb_r9_p0_out_data),
	.r9_p0_in_data_valid(tb_r9_p0_in_data_valid),
	.r9_p0_out_data_valid(tb_r9_p0_out_data_valid),
	.r9_p0_out_credits_feedback(tb_r9_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r9_p0_in_credits_feedback(tb_r9_p0_in_credits_feedback),
	`else
	.r9_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[9]),
	`endif
	
	
	//Router 10),
	.r10_p0_in_data(tb_r10_p0_in_data),
	.r10_p0_out_data(tb_r10_p0_out_data),
	.r10_p0_in_data_valid(tb_r10_p0_in_data_valid),
	.r10_p0_out_data_valid(tb_r10_p0_out_data_valid),
	.r10_p0_out_credits_feedback(tb_r10_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r10_p0_in_credits_feedback(tb_r10_p0_in_credits_feedback),
	`else
	.r10_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[10]),
	`endif
	
	
	//Router 11),
	.r11_p0_in_data(tb_r11_p0_in_data),
	.r11_p0_out_data(tb_r11_p0_out_data),
	.r11_p0_in_data_valid(tb_r11_p0_in_data_valid),
	.r11_p0_out_data_valid(tb_r11_p0_out_data_valid),
	.r11_p0_out_credits_feedback(tb_r11_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r11_p0_in_credits_feedback(tb_r11_p0_in_credits_feedback),
	`else
	.r11_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[11]),
	`endif
	
	
	//Router 12),
	.r12_p0_in_data(tb_r12_p0_in_data),
	.r12_p0_out_data(tb_r12_p0_out_data),
	.r12_p0_in_data_valid(tb_r12_p0_in_data_valid),
	.r12_p0_out_data_valid(tb_r12_p0_out_data_valid),
	.r12_p0_out_credits_feedback(tb_r12_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r12_p0_in_credits_feedback(tb_r12_p0_in_credits_feedback),
	`else
	.r12_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[12]),
	`endif
	
	
	//Router 13),
	.r13_p0_in_data(tb_r13_p0_in_data),
	.r13_p0_out_data(tb_r13_p0_out_data),
	.r13_p0_in_data_valid(tb_r13_p0_in_data_valid),
	.r13_p0_out_data_valid(tb_r13_p0_out_data_valid),
	.r13_p0_out_credits_feedback(tb_r13_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r13_p0_in_credits_feedback(tb_r13_p0_in_credits_feedback),
	`else
	.r13_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[13]),
	`endif
	
	
	//Router 14),
	.r14_p0_in_data(tb_r14_p0_in_data),
	.r14_p0_out_data(tb_r14_p0_out_data),
	.r14_p0_in_data_valid(tb_r14_p0_in_data_valid),
	.r14_p0_out_data_valid(tb_r14_p0_out_data_valid),
	.r14_p0_out_credits_feedback(tb_r14_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r14_p0_in_credits_feedback(tb_r14_p0_in_credits_feedback),
	`else
	.r14_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[14]),
	`endif
	
	
	//Router 15),
	.r15_p0_in_data(tb_r15_p0_in_data),
	.r15_p0_out_data(tb_r15_p0_out_data),
	.r15_p0_in_data_valid(tb_r15_p0_in_data_valid),
	.r15_p0_out_data_valid(tb_r15_p0_out_data_valid),
	.r15_p0_out_credits_feedback(tb_r15_p0_out_credits_feedback),
	`ifdef FT_RETRANSMISSION
	.r15_p0_in_credits_feedback(tb_r15_p0_in_credits_feedback),
	`else
	.r15_p0_in_credits_feedback(tb_r_p0_in_credits_feedback[15]),
	`endif
	
	
	.RST(tb_RST),
	.CLK(tb_CLK)
	);


	// RandomInjector: 0
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(0)
		,.P_LOCAL_PORT(0)
	) randInj_0 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[0])

		,.id_request(tb_PacketInj_get_id_enable[0])
		,.id_grant(tb_PacketInj_get_id_grant[0])
		,.id_result(tb_PacketInj_get_id_data[0])

		,.credit_feedback(tb_r0_p0_out_credits_feedback)
		,.data_valid(tb_r0_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r0_p0_preEncoder_data)
		`else
			,.data_out(tb_r0_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 0
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(0)
		,.P_LOCAL_PORT(0)
	) retransM_0 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[0])

		`ifdef FT_ECC
			,.error(tb_r0_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[0])
		,.get_id_grant(tb_PacketInj_get_id_grant[0])
		,.get_id_data(tb_PacketInj_get_id_data[0])

		,.set_id_enable(tb_PacketInj_set_id_enable[0])
		,.set_id_grant(tb_PacketInj_set_id_grant[0])
		,.set_id_data(tb_PacketInj_set_id_data[0])

		,.sending_data_valid(tb_r0_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r0_p0_preEncoder_data)
		`else
			,.sending_data(tb_r0_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r0_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[0])
		`else
			,.receiving_data(tb_r0_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r0_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r0_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 0
	m_ECC_Unit_Encoder ecc_encoderUnit_0(
		.in(tb_r0_p0_preEncoder_data)
		,.out_encoded(tb_r0_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_0(
		.in_encoded(tb_r0_p0_out_data)
		,.out_decoded(tb_r0_p0_postDecoder_data)
		,.out_error(tb_r0_p0_ecc_error)
	);
	`endif


	// RandomInjector: 1
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(1)
		,.P_LOCAL_PORT(0)
	) randInj_1 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[1])

		,.id_request(tb_PacketInj_get_id_enable[1])
		,.id_grant(tb_PacketInj_get_id_grant[1])
		,.id_result(tb_PacketInj_get_id_data[1])

		,.credit_feedback(tb_r1_p0_out_credits_feedback)
		,.data_valid(tb_r1_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r1_p0_preEncoder_data)
		`else
			,.data_out(tb_r1_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 1
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(1)
		,.P_LOCAL_PORT(0)
	) retransM_1 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[1])

		`ifdef FT_ECC
			,.error(tb_r1_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[1])
		,.get_id_grant(tb_PacketInj_get_id_grant[1])
		,.get_id_data(tb_PacketInj_get_id_data[1])

		,.set_id_enable(tb_PacketInj_set_id_enable[1])
		,.set_id_grant(tb_PacketInj_set_id_grant[1])
		,.set_id_data(tb_PacketInj_set_id_data[1])

		,.sending_data_valid(tb_r1_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r1_p0_preEncoder_data)
		`else
			,.sending_data(tb_r1_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r1_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[1])
		`else
			,.receiving_data(tb_r1_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r1_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r1_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 1
	m_ECC_Unit_Encoder ecc_encoderUnit_1(
		.in(tb_r1_p0_preEncoder_data)
		,.out_encoded(tb_r1_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_1(
		.in_encoded(tb_r1_p0_out_data)
		,.out_decoded(tb_r1_p0_postDecoder_data)
		,.out_error(tb_r1_p0_ecc_error)
	);
	`endif


	// RandomInjector: 2
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(2)
		,.P_LOCAL_PORT(0)
	) randInj_2 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[2])

		,.id_request(tb_PacketInj_get_id_enable[2])
		,.id_grant(tb_PacketInj_get_id_grant[2])
		,.id_result(tb_PacketInj_get_id_data[2])

		,.credit_feedback(tb_r2_p0_out_credits_feedback)
		,.data_valid(tb_r2_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r2_p0_preEncoder_data)
		`else
			,.data_out(tb_r2_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 2
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(2)
		,.P_LOCAL_PORT(0)
	) retransM_2 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[2])

		`ifdef FT_ECC
			,.error(tb_r2_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[2])
		,.get_id_grant(tb_PacketInj_get_id_grant[2])
		,.get_id_data(tb_PacketInj_get_id_data[2])

		,.set_id_enable(tb_PacketInj_set_id_enable[2])
		,.set_id_grant(tb_PacketInj_set_id_grant[2])
		,.set_id_data(tb_PacketInj_set_id_data[2])

		,.sending_data_valid(tb_r2_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r2_p0_preEncoder_data)
		`else
			,.sending_data(tb_r2_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r2_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[2])
		`else
			,.receiving_data(tb_r2_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r2_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r2_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 2
	m_ECC_Unit_Encoder ecc_encoderUnit_2(
		.in(tb_r2_p0_preEncoder_data)
		,.out_encoded(tb_r2_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_2(
		.in_encoded(tb_r2_p0_out_data)
		,.out_decoded(tb_r2_p0_postDecoder_data)
		,.out_error(tb_r2_p0_ecc_error)
	);
	`endif


	// RandomInjector: 3
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(3)
		,.P_LOCAL_PORT(0)
	) randInj_3 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[3])

		,.id_request(tb_PacketInj_get_id_enable[3])
		,.id_grant(tb_PacketInj_get_id_grant[3])
		,.id_result(tb_PacketInj_get_id_data[3])

		,.credit_feedback(tb_r3_p0_out_credits_feedback)
		,.data_valid(tb_r3_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r3_p0_preEncoder_data)
		`else
			,.data_out(tb_r3_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 3
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(3)
		,.P_LOCAL_PORT(0)
	) retransM_3 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[3])

		`ifdef FT_ECC
			,.error(tb_r3_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[3])
		,.get_id_grant(tb_PacketInj_get_id_grant[3])
		,.get_id_data(tb_PacketInj_get_id_data[3])

		,.set_id_enable(tb_PacketInj_set_id_enable[3])
		,.set_id_grant(tb_PacketInj_set_id_grant[3])
		,.set_id_data(tb_PacketInj_set_id_data[3])

		,.sending_data_valid(tb_r3_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r3_p0_preEncoder_data)
		`else
			,.sending_data(tb_r3_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r3_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[3])
		`else
			,.receiving_data(tb_r3_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r3_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r3_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 3
	m_ECC_Unit_Encoder ecc_encoderUnit_3(
		.in(tb_r3_p0_preEncoder_data)
		,.out_encoded(tb_r3_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_3(
		.in_encoded(tb_r3_p0_out_data)
		,.out_decoded(tb_r3_p0_postDecoder_data)
		,.out_error(tb_r3_p0_ecc_error)
	);
	`endif


	// RandomInjector: 4
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(4)
		,.P_LOCAL_PORT(0)
	) randInj_4 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[4])

		,.id_request(tb_PacketInj_get_id_enable[4])
		,.id_grant(tb_PacketInj_get_id_grant[4])
		,.id_result(tb_PacketInj_get_id_data[4])

		,.credit_feedback(tb_r4_p0_out_credits_feedback)
		,.data_valid(tb_r4_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r4_p0_preEncoder_data)
		`else
			,.data_out(tb_r4_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 4
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(4)
		,.P_LOCAL_PORT(0)
	) retransM_4 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[4])

		`ifdef FT_ECC
			,.error(tb_r4_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[4])
		,.get_id_grant(tb_PacketInj_get_id_grant[4])
		,.get_id_data(tb_PacketInj_get_id_data[4])

		,.set_id_enable(tb_PacketInj_set_id_enable[4])
		,.set_id_grant(tb_PacketInj_set_id_grant[4])
		,.set_id_data(tb_PacketInj_set_id_data[4])

		,.sending_data_valid(tb_r4_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r4_p0_preEncoder_data)
		`else
			,.sending_data(tb_r4_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r4_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[4])
		`else
			,.receiving_data(tb_r4_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r4_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r4_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 4
	m_ECC_Unit_Encoder ecc_encoderUnit_4(
		.in(tb_r4_p0_preEncoder_data)
		,.out_encoded(tb_r4_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_4(
		.in_encoded(tb_r4_p0_out_data)
		,.out_decoded(tb_r4_p0_postDecoder_data)
		,.out_error(tb_r4_p0_ecc_error)
	);
	`endif


	// RandomInjector: 5
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(5)
		,.P_LOCAL_PORT(0)
	) randInj_5 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[5])

		,.id_request(tb_PacketInj_get_id_enable[5])
		,.id_grant(tb_PacketInj_get_id_grant[5])
		,.id_result(tb_PacketInj_get_id_data[5])

		,.credit_feedback(tb_r5_p0_out_credits_feedback)
		,.data_valid(tb_r5_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r5_p0_preEncoder_data)
		`else
			,.data_out(tb_r5_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 5
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(5)
		,.P_LOCAL_PORT(0)
	) retransM_5 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[5])

		`ifdef FT_ECC
			,.error(tb_r5_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[5])
		,.get_id_grant(tb_PacketInj_get_id_grant[5])
		,.get_id_data(tb_PacketInj_get_id_data[5])

		,.set_id_enable(tb_PacketInj_set_id_enable[5])
		,.set_id_grant(tb_PacketInj_set_id_grant[5])
		,.set_id_data(tb_PacketInj_set_id_data[5])

		,.sending_data_valid(tb_r5_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r5_p0_preEncoder_data)
		`else
			,.sending_data(tb_r5_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r5_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[5])
		`else
			,.receiving_data(tb_r5_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r5_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r5_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 5
	m_ECC_Unit_Encoder ecc_encoderUnit_5(
		.in(tb_r5_p0_preEncoder_data)
		,.out_encoded(tb_r5_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_5(
		.in_encoded(tb_r5_p0_out_data)
		,.out_decoded(tb_r5_p0_postDecoder_data)
		,.out_error(tb_r5_p0_ecc_error)
	);
	`endif


	// RandomInjector: 6
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(6)
		,.P_LOCAL_PORT(0)
	) randInj_6 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[6])

		,.id_request(tb_PacketInj_get_id_enable[6])
		,.id_grant(tb_PacketInj_get_id_grant[6])
		,.id_result(tb_PacketInj_get_id_data[6])

		,.credit_feedback(tb_r6_p0_out_credits_feedback)
		,.data_valid(tb_r6_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r6_p0_preEncoder_data)
		`else
			,.data_out(tb_r6_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 6
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(6)
		,.P_LOCAL_PORT(0)
	) retransM_6 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[6])

		`ifdef FT_ECC
			,.error(tb_r6_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[6])
		,.get_id_grant(tb_PacketInj_get_id_grant[6])
		,.get_id_data(tb_PacketInj_get_id_data[6])

		,.set_id_enable(tb_PacketInj_set_id_enable[6])
		,.set_id_grant(tb_PacketInj_set_id_grant[6])
		,.set_id_data(tb_PacketInj_set_id_data[6])

		,.sending_data_valid(tb_r6_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r6_p0_preEncoder_data)
		`else
			,.sending_data(tb_r6_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r6_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[6])
		`else
			,.receiving_data(tb_r6_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r6_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r6_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 6
	m_ECC_Unit_Encoder ecc_encoderUnit_6(
		.in(tb_r6_p0_preEncoder_data)
		,.out_encoded(tb_r6_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_6(
		.in_encoded(tb_r6_p0_out_data)
		,.out_decoded(tb_r6_p0_postDecoder_data)
		,.out_error(tb_r6_p0_ecc_error)
	);
	`endif


	// RandomInjector: 7
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(7)
		,.P_LOCAL_PORT(0)
	) randInj_7 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[7])

		,.id_request(tb_PacketInj_get_id_enable[7])
		,.id_grant(tb_PacketInj_get_id_grant[7])
		,.id_result(tb_PacketInj_get_id_data[7])

		,.credit_feedback(tb_r7_p0_out_credits_feedback)
		,.data_valid(tb_r7_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r7_p0_preEncoder_data)
		`else
			,.data_out(tb_r7_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 7
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(7)
		,.P_LOCAL_PORT(0)
	) retransM_7 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[7])

		`ifdef FT_ECC
			,.error(tb_r7_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[7])
		,.get_id_grant(tb_PacketInj_get_id_grant[7])
		,.get_id_data(tb_PacketInj_get_id_data[7])

		,.set_id_enable(tb_PacketInj_set_id_enable[7])
		,.set_id_grant(tb_PacketInj_set_id_grant[7])
		,.set_id_data(tb_PacketInj_set_id_data[7])

		,.sending_data_valid(tb_r7_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r7_p0_preEncoder_data)
		`else
			,.sending_data(tb_r7_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r7_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[7])
		`else
			,.receiving_data(tb_r7_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r7_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r7_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 7
	m_ECC_Unit_Encoder ecc_encoderUnit_7(
		.in(tb_r7_p0_preEncoder_data)
		,.out_encoded(tb_r7_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_7(
		.in_encoded(tb_r7_p0_out_data)
		,.out_decoded(tb_r7_p0_postDecoder_data)
		,.out_error(tb_r7_p0_ecc_error)
	);
	`endif


	// RandomInjector: 8
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(8)
		,.P_LOCAL_PORT(0)
	) randInj_8 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[8])

		,.id_request(tb_PacketInj_get_id_enable[8])
		,.id_grant(tb_PacketInj_get_id_grant[8])
		,.id_result(tb_PacketInj_get_id_data[8])

		,.credit_feedback(tb_r8_p0_out_credits_feedback)
		,.data_valid(tb_r8_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r8_p0_preEncoder_data)
		`else
			,.data_out(tb_r8_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 8
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(8)
		,.P_LOCAL_PORT(0)
	) retransM_8 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[8])

		`ifdef FT_ECC
			,.error(tb_r8_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[8])
		,.get_id_grant(tb_PacketInj_get_id_grant[8])
		,.get_id_data(tb_PacketInj_get_id_data[8])

		,.set_id_enable(tb_PacketInj_set_id_enable[8])
		,.set_id_grant(tb_PacketInj_set_id_grant[8])
		,.set_id_data(tb_PacketInj_set_id_data[8])

		,.sending_data_valid(tb_r8_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r8_p0_preEncoder_data)
		`else
			,.sending_data(tb_r8_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r8_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[8])
		`else
			,.receiving_data(tb_r8_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r8_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r8_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 8
	m_ECC_Unit_Encoder ecc_encoderUnit_8(
		.in(tb_r8_p0_preEncoder_data)
		,.out_encoded(tb_r8_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_8(
		.in_encoded(tb_r8_p0_out_data)
		,.out_decoded(tb_r8_p0_postDecoder_data)
		,.out_error(tb_r8_p0_ecc_error)
	);
	`endif


	// RandomInjector: 9
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(9)
		,.P_LOCAL_PORT(0)
	) randInj_9 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[9])

		,.id_request(tb_PacketInj_get_id_enable[9])
		,.id_grant(tb_PacketInj_get_id_grant[9])
		,.id_result(tb_PacketInj_get_id_data[9])

		,.credit_feedback(tb_r9_p0_out_credits_feedback)
		,.data_valid(tb_r9_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r9_p0_preEncoder_data)
		`else
			,.data_out(tb_r9_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 9
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(9)
		,.P_LOCAL_PORT(0)
	) retransM_9 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[9])

		`ifdef FT_ECC
			,.error(tb_r9_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[9])
		,.get_id_grant(tb_PacketInj_get_id_grant[9])
		,.get_id_data(tb_PacketInj_get_id_data[9])

		,.set_id_enable(tb_PacketInj_set_id_enable[9])
		,.set_id_grant(tb_PacketInj_set_id_grant[9])
		,.set_id_data(tb_PacketInj_set_id_data[9])

		,.sending_data_valid(tb_r9_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r9_p0_preEncoder_data)
		`else
			,.sending_data(tb_r9_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r9_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[9])
		`else
			,.receiving_data(tb_r9_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r9_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r9_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 9
	m_ECC_Unit_Encoder ecc_encoderUnit_9(
		.in(tb_r9_p0_preEncoder_data)
		,.out_encoded(tb_r9_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_9(
		.in_encoded(tb_r9_p0_out_data)
		,.out_decoded(tb_r9_p0_postDecoder_data)
		,.out_error(tb_r9_p0_ecc_error)
	);
	`endif


	// RandomInjector: 10
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(10)
		,.P_LOCAL_PORT(0)
	) randInj_10 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[10])

		,.id_request(tb_PacketInj_get_id_enable[10])
		,.id_grant(tb_PacketInj_get_id_grant[10])
		,.id_result(tb_PacketInj_get_id_data[10])

		,.credit_feedback(tb_r10_p0_out_credits_feedback)
		,.data_valid(tb_r10_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r10_p0_preEncoder_data)
		`else
			,.data_out(tb_r10_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 10
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(10)
		,.P_LOCAL_PORT(0)
	) retransM_10 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[10])

		`ifdef FT_ECC
			,.error(tb_r10_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[10])
		,.get_id_grant(tb_PacketInj_get_id_grant[10])
		,.get_id_data(tb_PacketInj_get_id_data[10])

		,.set_id_enable(tb_PacketInj_set_id_enable[10])
		,.set_id_grant(tb_PacketInj_set_id_grant[10])
		,.set_id_data(tb_PacketInj_set_id_data[10])

		,.sending_data_valid(tb_r10_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r10_p0_preEncoder_data)
		`else
			,.sending_data(tb_r10_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r10_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[10])
		`else
			,.receiving_data(tb_r10_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r10_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r10_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 10
	m_ECC_Unit_Encoder ecc_encoderUnit_10(
		.in(tb_r10_p0_preEncoder_data)
		,.out_encoded(tb_r10_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_10(
		.in_encoded(tb_r10_p0_out_data)
		,.out_decoded(tb_r10_p0_postDecoder_data)
		,.out_error(tb_r10_p0_ecc_error)
	);
	`endif


	// RandomInjector: 11
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(11)
		,.P_LOCAL_PORT(0)
	) randInj_11 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[11])

		,.id_request(tb_PacketInj_get_id_enable[11])
		,.id_grant(tb_PacketInj_get_id_grant[11])
		,.id_result(tb_PacketInj_get_id_data[11])

		,.credit_feedback(tb_r11_p0_out_credits_feedback)
		,.data_valid(tb_r11_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r11_p0_preEncoder_data)
		`else
			,.data_out(tb_r11_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 11
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(11)
		,.P_LOCAL_PORT(0)
	) retransM_11 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[11])

		`ifdef FT_ECC
			,.error(tb_r11_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[11])
		,.get_id_grant(tb_PacketInj_get_id_grant[11])
		,.get_id_data(tb_PacketInj_get_id_data[11])

		,.set_id_enable(tb_PacketInj_set_id_enable[11])
		,.set_id_grant(tb_PacketInj_set_id_grant[11])
		,.set_id_data(tb_PacketInj_set_id_data[11])

		,.sending_data_valid(tb_r11_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r11_p0_preEncoder_data)
		`else
			,.sending_data(tb_r11_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r11_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[11])
		`else
			,.receiving_data(tb_r11_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r11_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r11_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 11
	m_ECC_Unit_Encoder ecc_encoderUnit_11(
		.in(tb_r11_p0_preEncoder_data)
		,.out_encoded(tb_r11_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_11(
		.in_encoded(tb_r11_p0_out_data)
		,.out_decoded(tb_r11_p0_postDecoder_data)
		,.out_error(tb_r11_p0_ecc_error)
	);
	`endif


	// RandomInjector: 12
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(12)
		,.P_LOCAL_PORT(0)
	) randInj_12 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[12])

		,.id_request(tb_PacketInj_get_id_enable[12])
		,.id_grant(tb_PacketInj_get_id_grant[12])
		,.id_result(tb_PacketInj_get_id_data[12])

		,.credit_feedback(tb_r12_p0_out_credits_feedback)
		,.data_valid(tb_r12_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r12_p0_preEncoder_data)
		`else
			,.data_out(tb_r12_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 12
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(12)
		,.P_LOCAL_PORT(0)
	) retransM_12 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[12])

		`ifdef FT_ECC
			,.error(tb_r12_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[12])
		,.get_id_grant(tb_PacketInj_get_id_grant[12])
		,.get_id_data(tb_PacketInj_get_id_data[12])

		,.set_id_enable(tb_PacketInj_set_id_enable[12])
		,.set_id_grant(tb_PacketInj_set_id_grant[12])
		,.set_id_data(tb_PacketInj_set_id_data[12])

		,.sending_data_valid(tb_r12_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r12_p0_preEncoder_data)
		`else
			,.sending_data(tb_r12_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r12_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[12])
		`else
			,.receiving_data(tb_r12_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r12_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r12_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 12
	m_ECC_Unit_Encoder ecc_encoderUnit_12(
		.in(tb_r12_p0_preEncoder_data)
		,.out_encoded(tb_r12_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_12(
		.in_encoded(tb_r12_p0_out_data)
		,.out_decoded(tb_r12_p0_postDecoder_data)
		,.out_error(tb_r12_p0_ecc_error)
	);
	`endif


	// RandomInjector: 13
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(13)
		,.P_LOCAL_PORT(0)
	) randInj_13 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[13])

		,.id_request(tb_PacketInj_get_id_enable[13])
		,.id_grant(tb_PacketInj_get_id_grant[13])
		,.id_result(tb_PacketInj_get_id_data[13])

		,.credit_feedback(tb_r13_p0_out_credits_feedback)
		,.data_valid(tb_r13_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r13_p0_preEncoder_data)
		`else
			,.data_out(tb_r13_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 13
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(13)
		,.P_LOCAL_PORT(0)
	) retransM_13 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[13])

		`ifdef FT_ECC
			,.error(tb_r13_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[13])
		,.get_id_grant(tb_PacketInj_get_id_grant[13])
		,.get_id_data(tb_PacketInj_get_id_data[13])

		,.set_id_enable(tb_PacketInj_set_id_enable[13])
		,.set_id_grant(tb_PacketInj_set_id_grant[13])
		,.set_id_data(tb_PacketInj_set_id_data[13])

		,.sending_data_valid(tb_r13_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r13_p0_preEncoder_data)
		`else
			,.sending_data(tb_r13_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r13_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[13])
		`else
			,.receiving_data(tb_r13_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r13_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r13_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 13
	m_ECC_Unit_Encoder ecc_encoderUnit_13(
		.in(tb_r13_p0_preEncoder_data)
		,.out_encoded(tb_r13_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_13(
		.in_encoded(tb_r13_p0_out_data)
		,.out_decoded(tb_r13_p0_postDecoder_data)
		,.out_error(tb_r13_p0_ecc_error)
	);
	`endif


	// RandomInjector: 14
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(14)
		,.P_LOCAL_PORT(0)
	) randInj_14 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[14])

		,.id_request(tb_PacketInj_get_id_enable[14])
		,.id_grant(tb_PacketInj_get_id_grant[14])
		,.id_result(tb_PacketInj_get_id_data[14])

		,.credit_feedback(tb_r14_p0_out_credits_feedback)
		,.data_valid(tb_r14_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r14_p0_preEncoder_data)
		`else
			,.data_out(tb_r14_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 14
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(14)
		,.P_LOCAL_PORT(0)
	) retransM_14 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[14])

		`ifdef FT_ECC
			,.error(tb_r14_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[14])
		,.get_id_grant(tb_PacketInj_get_id_grant[14])
		,.get_id_data(tb_PacketInj_get_id_data[14])

		,.set_id_enable(tb_PacketInj_set_id_enable[14])
		,.set_id_grant(tb_PacketInj_set_id_grant[14])
		,.set_id_data(tb_PacketInj_set_id_data[14])

		,.sending_data_valid(tb_r14_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r14_p0_preEncoder_data)
		`else
			,.sending_data(tb_r14_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r14_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[14])
		`else
			,.receiving_data(tb_r14_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r14_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r14_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 14
	m_ECC_Unit_Encoder ecc_encoderUnit_14(
		.in(tb_r14_p0_preEncoder_data)
		,.out_encoded(tb_r14_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_14(
		.in_encoded(tb_r14_p0_out_data)
		,.out_decoded(tb_r14_p0_postDecoder_data)
		,.out_error(tb_r14_p0_ecc_error)
	);
	`endif


	// RandomInjector: 15
	`ifndef FT_RETRANSMISSION
	m_RandomInjector 
	#(
		.P_LOCAL_ID(15)
		,.P_LOCAL_PORT(0)
	) randInj_15 (
		.RST(tb_RST)
		,.CLK(tb_CLK)

		,.enable(tb_PacketInj_enable[15])

		,.id_request(tb_PacketInj_get_id_enable[15])
		,.id_grant(tb_PacketInj_get_id_grant[15])
		,.id_result(tb_PacketInj_get_id_data[15])

		,.credit_feedback(tb_r15_p0_out_credits_feedback)
		,.data_valid(tb_r15_p0_in_data_valid)
		`ifdef FT_ECC
			,.data_out(tb_r15_p0_preEncoder_data)
		`else
			,.data_out(tb_r15_p0_in_data)
		`endif

		,.finished_round()
	);
	`endif

	// RetransmissionUnit: 15
	`ifdef FT_RETRANSMISSION
	m_RetransmissionUnit 
	#(
		.P_LOCAL_ID(15)
		,.P_LOCAL_PORT(0)
	) retransM_15 (
		.RST(tb_RST)
		,.CLK(tb_CLK)
		,.enable(tb_PacketInj_enable[15])

		`ifdef FT_ECC
			,.error(tb_r15_p0_ecc_error)
		`else
			,.error(1'b0)
		`endif

		,.get_id_enable(tb_PacketInj_get_id_enable[15])
		,.get_id_grant(tb_PacketInj_get_id_grant[15])
		,.get_id_data(tb_PacketInj_get_id_data[15])

		,.set_id_enable(tb_PacketInj_set_id_enable[15])
		,.set_id_grant(tb_PacketInj_set_id_grant[15])
		,.set_id_data(tb_PacketInj_set_id_data[15])

		,.sending_data_valid(tb_r15_p0_in_data_valid)
		`ifdef FT_ECC
			,.sending_data(tb_r15_p0_preEncoder_data)
		`else
			,.sending_data(tb_r15_p0_in_data)
		`endif

		,.receiving_data_valid(tb_r15_p0_out_data_valid)
		`ifdef FT_ECC
			,.receiving_data(tb_PacketEject_data[15])
		`else
			,.receiving_data(tb_r15_p0_out_data)
		`endif

		,.sending_credit_feedback(tb_r15_p0_in_credits_feedback)
		,.receiving_credit_feedback(tb_r15_p0_out_credits_feedback)
	);
	`endif

	`ifdef FT_ECC
	// ECC NoC Local Port: 15
	m_ECC_Unit_Encoder ecc_encoderUnit_15(
		.in(tb_r15_p0_preEncoder_data)
		,.out_encoded(tb_r15_p0_in_data)
	);
	`endif

	`ifdef FT_ECC
	m_ECC_Unit_Decoder ecc_decoderUnit_15(
		.in_encoded(tb_r15_p0_out_data)
		,.out_decoded(tb_r15_p0_postDecoder_data)
		,.out_error(tb_r15_p0_ecc_error)
	);
	`endif


	// Clock Generator
	m_ClockGenerator clk1(
	.enable(tb_CLK_enable)
	,.clkOutput(tb_CLK)
	);
	endmodule

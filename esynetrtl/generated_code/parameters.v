	//`define FAULT_ENABLE 1
	//define FAULT_ENABLE to enable the fault clock connection
	//CLOCK 
	`define CLK_PERIOD 2 

	`define MESH_SIZE_X 4 
	`define MESH_SIZE_Y 4 

	// Routers 
	`define ROUTERS 16 

	// Ports 
	`define PORTS 5 
	// Virtual Channels 
	 `define CHANNELS 5

	 `define CHANNELS_WIDTH 3

	// Buffer Size == CREDIT Size 
	`define BUFFERSIZE 12 
	`define BUFFERSIZE_WIDTH 4 
	// State Machine States 
	`define STATES 10 
	`define STATES_WIDTH 4
	 
	 
	// Channels Numeration Position Definition 
`define P0 0
`define P1 1
`define P2 2
`define P3 3
`define P4 4
	//===========SIGNALS============ 
	`define DATA_VALID_WIDTH 1 
	//========GENERAL FLIT VALUES========= 
	// General 
	`define FLIT_WIDTH 34 
	`define FLIT_TYPE_WIDTH 2 
	`define FLIT_DATA_WIDTH 32 
	// HEADFLIT 10 
	`define FLIT_HEADFLIT 2
`define FLIT_SRC_WIDTH 4
`define FLIT_DST_WIDTH 4
`define FLIT_RESERVED_WIDTH 8

	`define FLIT_ID_WIDTH 12
	`define FLIT_FLAGS_WIDTH 4
	// `define FLIT_VC_WIDTH 1

	// BODYFLIT 11
	`define FLIT_BODYFLIT 3
	`define FLIT_PAYLOAD_WIDTH 32

	// TAILFLIT 01
	`define FLIT_TAILFLIT 1
	`define FLIT_CRC_WIDTH 32

	// Additional bits for the ECC checksum
	`define FLIT_ECC_CHECKSUM_WIDTH 27  // ECC(7,4), 34bits + virtual padding bits (flit type) -> 9 x 4 segments -> 9 x 3 checksum => 27
	`define ECC_DATA_SEGMENT_WIDTH 4
	`define ECC_CHECKSUM_SEGMENT_WIDTH 3
	
	//============LOGGING==============
	// Junshi Wang Logging States
	`define LOG_FLIT_RECEIVE 20
	`define LOG_FLIT_SWITCH 21
	`define LOG_FLIT_SEND 23
	`define LOG_FLIT_ROUTING 30
	`define LOG_FLIT_ROUTING_END 31
	`define LOG_FLIT_ASSIGN 34
	`define LOG_FLIT_ASSIGN_END 35
	`define LOG_FLIT_STATE_CHANGED 40

	// Junshi Wang StateMachine State Decoding
	`define VCS_INIT 0
	`define VCS_ROUTING 1
	`define VCS_VC_AB 2
	`define VCS_SW_AB 3
	`define VCS_SW_TR 4

	// Junshi Wang Flit Decoding
	`define FLIT_HEAD 0
	`define FLIT_BODY 1
	`define FLIT_TAIL 2

	//========TESTBENCH WITH RANDOM INJECTION==========
	// Parameters for the Random Fault Injection
	`define PACKETINJECTTHRESHOLD 100
	`define PACKETINJDELAY 0
	`define BODYFLITAMOUNT 5

	//========VALUES FOR RETRANSMISSION==========
	`define RESEND_CYCLE_LIMIT 150

	//========TESTBENCH WITH RANDOM INJECTION==========
	// Parameters for the FaultInjectionUnit
	// Right now only 32 bits values possible, adjust the propability values accordingly 
	// (64 bit values can be reached by doing it like this: num_64 = {$urandom, $urandom };)
	// Max unsigned 32 bit: 2**32 = 4294967296 -> 0..4294967295
	// Transient Error: Values form 0..1; PROP2FAULTY = ~0.0001, PROP2CORRECT = ~0.9
	// For our system: Values from 0..4294967295; PROP2FAULTY = 4294967295 * 0.0001 = ~ 429496, PROP2CORRECT = 3865470565 (both values rounded off)
	// Questa sim displays a warning that values of type 32 bit unsigned will be cut to 32 bit signed. They are displayed right in the testbench and adding an ".0" to the values lets the warning disappear?! 

	// This values are changed now directly in the injection script
	`define PROP2FAULTY 429496						// If the rand_numb (random number) is below this value the state is changed to faulty
	`define PROP2CORRECT 3865470565				// If the rand_numb (random number) is below this value the state is changed to correct
																				// Note: These values display two different propabilities. They can be changed that there is e.g. a higher propability that the faulty state is activated and a low propability that the correct state is reached or vice versa.
	`define SEED 234

	//============IFDEF-Section==============
	// Activates the faulty version of the module
	// `define FAULTY_STATEMACHINE			
	// `define FAULTY_BUFFER								// Activates the m_Buffer_faulty module in the m_Assembled_Channel
	// `define FAULTY_ROUTINGCALCULATION		
	// `define FAULTY_CROSSBAR
	// `define FAULTY_SWITCHALLOCATION
	// `define FAULTY_ROUNDROBIN
	// `define FAULTY_MUX
	// `define FAULTY_ASSERTIONUNIT
	// `define FAULTY_ERROR_COUNTER
	// `define FAULTY_ASSERTIONUNIT_PACKET	// SPecial used assertion unit
	// `define FAULTY_ECC
	// `define FAULTY_PACKETASSERTIONUNIT

	// Activates the different Fault Tolerance implementations
	// `define FT_RETRANSMISSION 			// Adds retransmission units to the testbench for the NOC
	// `define FT_ECC									// Adds ECC elements to the Channel and Testbench + unlucks packet drop by SM
	// `define FT_SECURE_SM						// Replaces standard SM with a sturdier version of itself
	// `define FT_RC_TOLERANT					// Adds a RoutingCalculation unit which can tolerate faulty routers/links + adds several asserionUnits, error counters and isFaulty-signals to the router and noc

	//=============MISC===============
	// it is not possible to declare an input as an integer...this is part of a "workaround"
	`define INPUT_INTEGER 32

	// Activates action logging in the some modules
	// `define USE_LOGGING 1

	// `define DEBUG 1
	`define TB_DEBUG_ENABLE 1
	`define UNUSED 0
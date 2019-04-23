
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

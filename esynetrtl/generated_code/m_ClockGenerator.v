

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

    

	#-----------This is a tcl script of dc,synopsys-----------

	#-------------------------------------------------------------------------------------------
	#-----------------------1 Specify library---------------------------------------------------
	#-------------------------------------------------------------------------------------------

		#path for designware
	set synthetic_library {/home/junkaizhan/software/synopsys/DC_2015_06/libraries/syn/dw_foundation.sldb}

	#target library
	set target_library [list /home/junkaizhan/technology/tcbn65gplus_digital/Front_End/timing_power_noise/NLDM/tcbn65gplus_200a/tcbn65gplustc.db]

	#path for link_library 
	set link_library fhewhfkjasdhfksdaj *  hf

	#path for symbol_library
	set symbol_library [list /home/junkaizhan/technology/tcbn65gplus_digital/Front_End/timing_power_noise/NLDM/tcbn65gplus_200a/tcbn65gplustc.db]

	#search path
	set search_path [list /home/junkaizhan/software/synopsys/DC_2015_06/libraries/syn  /home/junkaizhan/software/synopsys/DC_2015_06/dw/  /home/junkaizhan/technology/tcbn65gplus_digital/Front_End/timing_power_noise/NLDM/tcbn65gplus_200a ]


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
		set my_work_dir ./

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
		set clk CLK
	set CLK_PERIOD 2
	set MAX_FANOUT 10

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
	
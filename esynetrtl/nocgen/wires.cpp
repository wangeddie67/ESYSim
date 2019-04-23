	//state machine wire
	ofile
	<<"\t//state machine\n"
	<<"\t//wire\n"
	<<"\twire [`STATES_WIDTH - 1:0]  channel_current_state;\n\n";

    	//buffer wire
	ofile
	<<"\t//buffer\n"
	<<"\t//wire\n"
	<<"\twire channel_command_write;\n"
	<<"\twire channel_command_read;\n"
	<<"\twire [`BUFFERSIZE_WIDTH - 1:0]  channel_buffer_credits_local;\n\n";

    	//RoutingCalculation wire
	ofile
	<<"\t//RoutingCalculation \n"
	<<"\t//wire \n"
	<<"\twire channel_rc_enable;\n"
	<<"\twire [`FLIT_SRC_WIDTH - 1 : 0] local_id;\n"
	<<"\twire channel_rc_success;\n"
	<<"\twire [`CHANNELS - 1:0] channel_rc_result;\n\n\n";

    	//D_FF_sync 
	//wire
	ofile
	<<"\t//D_FF_sync\n"
	<<"\t//wire\n";
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		ofile
		<< "\t//Mux for port " << i << "\n";
		for (int j= 1 ; j<= port.inputVcNumber(); j++)
		{
			if( port.inputVcNumber()==1 )
			{
				string postfix = "";
			}
			else
			{
				string postfix = "_vc"+j;
			}
		
			ofile
			<< "\twire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p"<<i<<"_output_buffer;\n"
			<< "\tassign p"<<i<<"_out_data = p"<<i<<"_output_buffer[P_DATA_WIDTH - 1 : 0];\n"
			<< "\tassign p"<<i<<"_out_data_valid = p"<<i<<"_output_buffer[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1];\n\n";
		}
	}

    	//data out mux wire
	ofile
	<<"\t//data out mux_2to1\n"
	<<"\t//wire\n";
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		ofile
			<< "\t//Mux for port " << i << "\n";
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\twire intern_set_p"<<i<<";\n"
			<<"\tassign intern_set_p"<<i<<"="
			<<"\t(!crossbar_out_data_p"<<i<<"_vc1[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1] && "
			<<"\tcrossbar_out_data_p"<<i<<"_vc2[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1] ) ? 1 : 0;\n\n"
			<<"\twire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0]  crossbar_out_data_p"<<i<<";\n"
			<<"\t\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			//do no thing
		}
	}

    	//crossbar
	//wire
	ofile
	<<"\t//crossbar\n"
	<<"\t//wire\n";

	for(int i =0;i<p_port_num ; i++)
	{
		port = router.port(i);
		ofile
		<<"\t//for port "<<i<<"\n";
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\twire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p"<<i<<"_vc1;\n"
			<<"\twire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p"<<i<<"_vc2;\n"
			<<"\twire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p"<<i<<"_vc1;\n"
			<<"\twire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p"<<i<<"_vc2;\n\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\twire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] crossbar_out_data_p"<<i<<";\n"
			<<"\twire [`BUFFERSIZE_WIDTH - 1:0] crossbar_feedback_out_p"<<i<<";\n\n";
		}
	}

    	//switch allocation wire
	ofile
	<<"\t//switch allocation\n"
	<<"\t//wire\n";

	for(int i =0;i<p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\twire grant_p"<<i<<"_vc1;\n"
			<<"\twire grant_p"<<i<<"_vc2;\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\twire grant_p"<<i<<";\n";
		}
	}
    //round robin wire
    	ofile
	<<"\t//Round Robin\n"
	<<"\t//wire\n"
	<<"\twire [`CHANNELS - 1 : 0] intern_rr_request [`CHANNELS - 1 : 0];\n"
	<<"\twire [`CHANNELS - 1 : 0] intern_rr_result;\n\n";
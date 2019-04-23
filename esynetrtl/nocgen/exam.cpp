	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			if(i == p_port_num - 1)
			{
				ofile
				<<"enable_vector[`P"<<i<<"_VC1],"
				<<"enable_vector[`P"<<i<<"_VC2]),";
			}
			else
			{
				ofile
				<<"enable_vector[`P"<<i<<"_VC1],"
				<<"enable_vector[`P"<<i<<"_VC2],";
			}
		}
		else if(port.inputVcNumber() == 1)
		{
			if(i == p_port_num - 1)
			{
				ofile
				<<"enable_vector[`P"<<i<<"],";
			}
			else
			{
				ofile
				<<"enable_vector[`P"<<i<<"_VC],";
			}
		}
	}

	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		ofile
		<< "\t//Mux for port " << i << "\n";
		for (int j= 1 ; j<= port.inputVcNumber(); j++)
		{
			if( port.inputVcNumber()=1 )
			{
				string postfix = "";
			}
			else
			{
				string postfix = "_vc"+j;
			}
		
			ofile
			<< "\twire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p"<<i<<postfix<<"_output_buffer\n\n";
		}
	}
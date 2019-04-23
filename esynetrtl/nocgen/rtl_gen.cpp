#include "rtl_gen.h"
#include <iostream>
#include <fstream>
#include<cmath>
#include<cstdlib>
#include<cstring>
#include<cfloat>

using namespace std;

void NOC_GEN::setPath(string OUTPUT_PATH)
{
    	FILE_PATH = OUTPUT_PATH + "/";
}

int log2(int x)
{
	if(x<=0){
        cout << "ERROR! X must be bigger than 0"<<endl;
        return 0;
	}
	else
	{
		int i = ceil(log(x)/log(2));
		return i;
	}
}

//only used in router generation
void NOC_GEN::channelInst(int port_num, int vc_num, int channel_id)
{
	if(vc_num == 2)
	{
		// for vc 1
		ofile
		<<"\t//Channel p"<< port_num <<"_vc1\n"
		<<"\t//wire declaration;\n"
		<<"\twire out_sa_enable_p"<<port_num<<"_vc1;\n"
		<<"\twire [`CHANNELS - 1:0] out_sa_request_p"<<port_num<<"_vc1;\n"
		<<"\twire [`CHANNELS - 1:0] out_st_request_p"<<port_num<<"_vc1;\n"
		<<"\twire out_st_data_valid_p"<<port_num<<"_vc1;\n"
		<<"\twire [P_DATA_WIDTH - 1: 0] out_data_p"<<port_num<<"_vc1;\n"
		<<"\twire [`CHANNELS - 1:0] out_rr_request_p"<<port_num<<"_vc1;\n"
		<<"\t//module declaration\n"
		<<"\tm_Assembled_Channel\n"
		<<"\t	#(\n"
		<<"\t//.P_ROUTER_ID(P_ROUTER_ID),\n"
		<<"\t.P_CHANNEL("<<channel_id<<"),\n"
		<<"\t.P_DATA_WIDTH(P_DATA_WIDTH)\n"
		<<"\t)\n"
		<<"\tchannel_p"<< port_num <<"_vc1\n"
		<<"\t(\n"
		<<"\t.router_id(router_id),\n"
		<<"\t.RST(RST),\n"
		<<"\t.CLK(CLK),\n";
		//input credits
		for(int j =0 ; j < p_port_num ; j++)
		{
			port = router.port(j);
			if(port.inputVcNumber() == 2)
			{
				ofile
				<<"\t.in_neighbor_credits_p"<<j<<"_vc1(p"<<j<<"_vc1_in_credits_feedback),\n"
				<<"\t.in_neighbor_credits_p"<<j<<"_vc2(p"<<j<<"_vc2_in_credits_feedback),\n";
			}
			else if(port.inputVcNumber() == 1)
			{
				ofile
					<< "\t.in_neighbor_credits_p" << j << "(p" << j << "_in_credits_feedback),\n";
			}
		}
		ofile
		<< "\t//sa signals:\n"
		<< "\t.out_sa_enable(out_sa_enable_p"<<port_num<<"_vc1),\n"
		<< "\t.out_sa_request(out_sa_request_p"<<port_num<<"_vc1),\n"
		<< "\t.in_sa_grant(grant_p"<< port_num <<"_vc1),\n\n"

		<< "\t//st signals:\n"
		<< "\t.out_st_request(out_st_request_p"<<port_num<<"_vc1),\n"
		<< "\t.in_st_credits_feedback(crossbar_feedback_out_p"<< port_num <<"_vc1),\n"
		<< "\t.out_st_data_valid(out_st_data_valid_p"<<port_num<<"_vc1),\n\n"

		<< "\t.in_data(p"<<port_num<<"_vc1_in_data),\n"
		<< "\t.out_data(out_data_p"<<port_num<<"_vc1),\n\n"

		<< "\t.out_buffer_credits_sharing(p"<<port_num<<"_vc1_out_credits_feedback),\n"
		<< "\t.in_received_data_valid(p"<<port_num<<"_vc1_in_data_valid),\n\n"

		<< "\t.out_rr_request(out_rr_request_p"<<port_num<<"_vc1),\n"
		<< "\t.in_rr_result(intern_rr_result)\n);\n\n";
		
		//for vc2
		ofile
		<<"\t//Channel p"<< port_num <<"_vc2\n"
		<<"\t//wire declaration;\n"
		<<"\twire out_sa_enable_p"<<port_num<<"_vc2;\n"
		<<"\twire [`CHANNELS - 1:0] out_sa_request_p"<<port_num<<"_vc2;\n"
		<<"\twire [`CHANNELS - 1:0] out_st_request_p"<<port_num<<"_vc2;\n"
		<<"\twire out_st_data_valid_p"<<port_num<<"_vc2;\n"
		<<"\twire [P_DATA_WIDTH - 1: 0] out_data_p"<<port_num<<"_vc2;\n"
		<<"\twire [`CHANNELS - 1:0] out_rr_request_p"<<port_num<<"_vc2;\n"

		<<"\t//module declaration\n"
		<<"\tm_Assembled_Channel\n"
		<<"\t	#(\n"
		<<"\t//.P_ROUTER_ID(P_ROUTER_ID),\n"
		<<"\t.P_CHANNEL("<<(channel_id+1)<<"),\n"
		<<"\t.P_DATA_WIDTH(P_DATA_WIDTH)\n"
		<<"\t)\n"
		<<"\tchannel_p"<< port_num <<"_vc2\n"
		<<"\t(\n"
		<<"\t.router_id(router_id),\n"
		<<"\t.RST(RST),\n"
		<<"\t.CLK(CLK),\n";
		//input credits
		for(int j =0 ; j < p_port_num ; j++)
		{
			port = router.port(j);
			if(port.inputVcNumber() == 2)
			{
				ofile
				<<"\t.in_neighbor_credits_p"<<j<<"_vc1(p"<<j<<"_vc1_in_credits_feedback),\n"
				<<"\t.in_neighbor_credits_p"<<j<<"_vc2(p"<<j<<"_vc2_in_credits_feedback),\n";
			}
			else if(port.inputVcNumber() == 1)
			{
				ofile
					<< "\t.in_neighbor_credits_p" << j << "(p" << j << "_in_credits_feedback),\n";
			}
		}
		ofile
		<< "\t//sa signals:\n"
		<< "\t.out_sa_enable(out_sa_enable_p"<<port_num<<"_vc2),\n"
		<< "\t.out_sa_request(out_sa_request_p"<<port_num<<"_vc2),\n"
		<< "\t.in_sa_grant(grant_p"<<port_num<<"_vc2),\n\n"

		<< "\t//st signals:\n"
		<< "\t.out_st_request(out_st_request_p"<<port_num<<"_vc2),\n"
		<< "\t.in_st_credits_feedback(crossbar_feedback_out_p"<< port_num <<"_vc2),\n"
		<< "\t.out_st_data_valid(out_st_data_valid_p"<<port_num<<"_vc2),\n\n"

		<< "\t.in_data(p"<<port_num<<"_vc2_in_data),\n"
		<< "\t.out_data(out_data_p"<<port_num<<"_vc2),\n\n"

		<< "\t.out_buffer_credits_sharing(p"<<port_num<<"_vc2_out_credits_feedback),\n"
		<< "\t.in_received_data_valid(p"<<port_num<<"_vc2_in_data_valid),\n\n"

		<< "\t.out_rr_request(out_rr_request_p"<<port_num<<"_vc2),\n"
		<< "\t.in_rr_result(intern_rr_result)\n);\n\n";
	}
	//for port without vc
	else if(vc_num == 1)
	{
		ofile
		<<"\t//Channel p"<< port_num <<"\n"
		<<"\t//wire declaration;\n"
		<<"\twire out_sa_enable_p"<<port_num<<";\n"
		<<"\twire [`CHANNELS - 1:0] out_sa_request_p"<<port_num<<";\n"
		<<"\twire [`CHANNELS - 1:0] out_st_request_p"<<port_num<<";\n"
		<<"\twire out_st_data_valid_p"<<port_num<<";\n"
		<<"\twire [P_DATA_WIDTH - 1: 0] out_data_p"<<port_num<<";\n"
		<<"\twire [`CHANNELS - 1:0] out_rr_request_p"<<port_num<<";\n"

		<<"\t//module declaration\n"
		<<"\tm_Assembled_Channel\n"
		<<"\t	#(\n"
		<<"\t//.P_ROUTER_ID(P_ROUTER_ID),\n"
		<<"\t.P_CHANNEL("<<channel_id<<"),\n"
		<<"\t.P_DATA_WIDTH(P_DATA_WIDTH)\n"
		<<"\t)\n"
		<<"\tchannel_p"<< port_num <<"\n"
		<<"\t(\n"
		<<"\t.router_id(router_id),\n"
		<<"\t.RST(RST),\n"
		<<"\t.CLK(CLK),\n";
		//input credits
		for(int j =0 ; j < p_port_num ; j++)
		{
			port = router.port(j);
			if(port.inputVcNumber() == 2)
			{
				ofile
				<<"\t.in_neighbor_credits_p"<<j<<"_vc1(p"<<j<<"_vc1_in_credits_feedback),\n"
				<<"\t.in_neighbor_credits_p"<<j<<"_vc2(p"<<j<<"_vc2_in_credits_feedback),\n";
			}
			else if(port.inputVcNumber() == 1)
			{
				ofile
					<< "\t.in_neighbor_credits_p" << j << "(p" << j << "_in_credits_feedback),\n";
			}
		}
		ofile
		<< "\t//sa signals:\n"
		<< "\t.out_sa_enable(out_sa_enable_p"<<port_num<<"),\n"
		<< "\t.out_sa_request(out_sa_request_p"<<port_num<<"),\n"
		<< "\t.in_sa_grant(grant_p"<<port_num<<"),\n\n"

		<< "\t//st signals:\n"
		<< "\t.out_st_request(out_st_request_p"<<port_num<<"),\n"
		<< "\t.in_st_credits_feedback(crossbar_feedback_out_p"<< port_num <<"),\n"
		<< "\t.out_st_data_valid(out_st_data_valid_p"<<port_num<<"),\n\n"

		<< "\t.in_data(p"<<port_num<<"_in_data),\n"
		<< "\t.out_data(out_data_p"<<port_num<<"),\n\n"

		<< "\t.out_buffer_credits_sharing(p"<<port_num<<"_out_credits_feedback),\n"
		<< "\t.in_received_data_valid(p"<<port_num<<"_in_data_valid),\n\n"

		<< "\t.out_rr_request(out_rr_request_p"<<port_num<<"),\n"
		<< "\t.in_rr_result(intern_rr_result)\n);\n\n";
	}
}


EsyXmlError NOC_GEN::readXml(const string & fname)
{   
    //read Xml file
    EsyXmlError err = noc.readXml(fname);
    if(err.hasError())
    {
        cout<< "Error: Xml file read error"<<endl;
    }
	noc.updateRouter();

    x_size = noc.size()[0];
    y_size = noc.size()[1];
    cout <<"x size :" << x_size <<endl;
    cout <<"y size :" << y_size <<endl;

	buffer_size = noc.maxInputBuffer();	
    cout <<"buffer size :" << buffer_size <<endl;

	buffer_size_width = log2(buffer_size+1);
    cout <<"buffer size width :" << buffer_size_width <<endl;

    router_num = noc.routerCount();
    cout <<"router number :" << router_num <<endl;

	router = noc.templateRouter();
	
    //physcal port number of a router
    p_port_num = router.portNum();
    cout <<"physical port number :" << p_port_num <<endl;

	a_port_num = 0;
	for(int i = 0; i< p_port_num ; i++)
	{
		port = router.port(i);
		a_port_num = a_port_num +port.inputVcNumber();
	}
    cout <<"port number(include virtual port) :" << a_port_num <<endl;

	return err;
}

void NOC_GEN::rtlGen()
{
	 generateMux(a_port_num);

     generatingFeedbackSwitch (a_port_num);

     generateDataSwitch (a_port_num);

     generateCrossbar (a_port_num);
	 
     generateMatrixArbiter (a_port_num);

     generateEvaluationUnit ();

	 generateNoc(true);

	 generateNoc(false);

	 generateRouter();

	 generateChannel();

	 //generateRoutingCalculation();

	 generateSwitchAllocation();

	 generateParameters();

	 generateNocTestbench();
}

void NOC_GEN::generateRouter()
{
	//generate router rtl code
    ofile.open( FILE_PATH + ROUTER_FILE_NAME );

    ofile 
    << "`timescale 1ns / 1ps"<<endl
    <<"`include \"parameters.v\"\n\n"
	<<"\tmodule m_Assembled_Router(\n"

	//router id
	<<"\trouter_id,\n"
    //local io port
    <<"\t//define local port io, port number : 0\n"
    <<"\tp0_in_data_valid,\n"
    <<"\tp0_out_data_valid,\n"
    <<"\tp0_in_data,\n"
    <<"\tp0_out_data,\n"
    <<"\tp0_out_credits_feedback,\n" 
    <<"\tp0_in_credits_feedback,\n\n" ;

    //other ports
	router = noc.templateRouter();
	for (int i = 1; i < p_port_num; i++)
    {
		port = router.port(i);
        ofile
		<<"\t//io declaration for port "<<i<<"\n"
		<<"\tp"<<i<<"_in_data,\n"
        <<"\tp"<<i<<"_out_data,\n\n"
        <<"\tp"<<i<<"_in_data_valid,\n"
        <<"\tp"<<i<<"_out_data_valid,\n\n";
        if(port.inputVcNumber() == 2)
        {
            ofile
            <<"\tp"<<i<<"_in_set_Channel,\n"
            <<"\tp"<<i<<"_out_set_Channel,\n\n"
            <<"\tp"<<i<<"_vc1_in_credits_feedback,\n"
            <<"\tp"<<i<<"_vc2_in_credits_feedback,\n\n"
            <<"\tp"<<i<<"_vc1_out_credits_feedback,\n"
            <<"\tp"<<i<<"_vc2_out_credits_feedback,\n";
        }else if(port.inputVcNumber() == 1)
        {
            ofile
            <<"\tp"<<i<<"_in_credits_feedback,\n"
            <<"\tp"<<i<<"_out_credits_feedback,\n\n";
        }
		ofile
		<<endl;
    }
    ofile
	<<"\tRST,\n"
	<<"\tCLK\n\n"	
    <<");\n\n";
    //end of io declaration

    //define parameters
	ofile
		<< "\t//parameters\n"
		<< "\t//parameter P_ROUTER_ID = 0;\n"
		<< "\t`ifdef FT_ECC\n"
		<< "\t		parameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH;\n"
		<< "\t`else \n"
		<< "\t	parameter P_DATA_WIDTH = `FLIT_WIDTH;\n"
		<< "\t`endif\n\n";

	//define inout
	ofile
	<<"\tinput [`FLIT_SRC_WIDTH-1 : 0] router_id;\n";
	//local io port
	ofile
    <<"\t//define local port io; port number : 0\n"
    <<"\tinput p0_in_data_valid;\n"
    <<"\toutput p0_out_data_valid;\n"
    <<"\tinput [P_DATA_WIDTH - 1: 0] p0_in_data;\n"
    <<"\toutput [P_DATA_WIDTH - 1: 0] p0_out_data;\n"
    <<"\tinput [`BUFFERSIZE_WIDTH - 1 : 0] p0_in_credits_feedback;\n" 
    <<"\toutput [`BUFFERSIZE_WIDTH - 1 : 0] p0_out_credits_feedback;\n\n" ;

    //other ports
	router = noc.templateRouter();
	for (int i = 1; i < p_port_num; i++)
    {
		port = router.port(i);
        ofile
		<<"\t//io declaration for port "<<i<<"\n"
		<<"\tinput [P_DATA_WIDTH - 1: 0] p"<<i<<"_in_data;\n"
        <<"\toutput [P_DATA_WIDTH - 1: 0] p"<<i<<"_out_data;\n\n"
        <<"\tinput  p"<<i<<"_in_data_valid;\n"
        <<"\toutput  p"<<i<<"_out_data_valid;\n\n";
        if(port.inputVcNumber() == 2)
        {
            ofile
            <<"\tinput  p"<<i<<"_in_set_Channel;\n"
            <<"\toutput  p"<<i<<"_out_set_Channel;\n\n"
            <<"\tinput [`BUFFERSIZE_WIDTH - 1 : 0] p"<<i<<"_vc1_in_credits_feedback;\n"
            <<"\tinput [`BUFFERSIZE_WIDTH - 1 : 0] p"<<i<<"_vc2_in_credits_feedback;\n\n"
            <<"\toutput [`BUFFERSIZE_WIDTH - 1 : 0] p"<<i<<"_vc1_out_credits_feedback;\n"
            <<"\toutput [`BUFFERSIZE_WIDTH - 1 : 0] p"<<i<<"_vc2_out_credits_feedback;\n";
        }else if(port.inputVcNumber() == 1)
        {
            ofile
            <<"\tinput [`BUFFERSIZE_WIDTH - 1 : 0] p"<<i<<"_in_credits_feedback;\n"
            <<"\toutput [`BUFFERSIZE_WIDTH - 1 : 0] p"<<i<<"_out_credits_feedback;\n\n";
        }
		ofile
		<<endl;
    }
    ofile
	<<"\tinput RST;\n"
	<<"\tinput CLK;\n\n"	
    <<"\n\n";


	//wire

	ofile
	<<"\t//D_FF_sync\n"
	<<"\t//wire\n";
	for(int i =0 ; i < p_port_num ; i++)
	{
		ofile
		<< "\t//Mux for port " << i << "\n";
			ofile
			<< "\twire [P_DATA_WIDTH + `DATA_VALID_WIDTH - 1: 0] p"<<i<<"_output_buffer;\n"
			<< "\tassign p"<<i<<"_out_data = p"<<i<<"_output_buffer[P_DATA_WIDTH - 1 : 0];\n"
			<< "\tassign p"<<i<<"_out_data_valid = p"<<i<<"_output_buffer[`DATA_VALID_WIDTH + P_DATA_WIDTH - 1];\n\n";
	}

    //round robin wire
    ofile
	<<"\t//Round Robin\n"
	<<"\t//wire\n"
	<<"\twire [`CHANNELS - 1 : 0] intern_rr_request [`CHANNELS - 1 : 0];\n"
	<<"\twire [`CHANNELS - 1 : 0] intern_rr_result;\n\n";

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

	ofile
	<<"\t//Module Declaration\n";

	//define modules

	//input demux
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\twire [P_DATA_WIDTH - 1: 0]  p"<<i<<"_vc1_in_data;\n"
			<<"\twire [P_DATA_WIDTH - 1: 0]  p"<<i<<"_vc2_in_data;\n\n"

			<<"\twire  p"<<i<<"_vc1_in_data_valid;\n"
			<<"\twire  p"<<i<<"_vc2_in_data_valid;\n\n"

			<<"\tm_Demux_1in2out  #(\n"
			<<"\t.P_DATA_WIDTH(P_DATA_WIDTH)\n"
			<<"\t) \n"
			<<"\tinDemux_p"<<i<<"_data (\n"
			<<"\t.select(p"<<i<<"_in_set_Channel),\n"
			<<"\t.data_in(p"<<i<<"_in_data),\n"
			<<"\t.data_out_0(p"<<i<<"_vc1_in_data),\n"
			<<"\t.data_out_1(p"<<i<<"_vc2_in_data)\n"
			<<"\t);\n\n"

			<<"\tm_Demux_1in2out  #(\n"
			<<"\t.P_DATA_WIDTH(`DATA_VALID_WIDTH)\n"
			<<"\t) \n"
			<<"\tinDemux_p"<<i<<"_data_valid (\n"
			<<"\t.select(p"<<i<<"_in_set_Channel),\n"
			<<"\t.data_in(p"<<i<<"_in_data_valid),\n"
			<<"\t.data_out_0(p"<<i<<"_vc1_in_data_valid),\n"
			<<"\t.data_out_1(p"<<i<<"_vc2_in_data_valid)\n"
			<<"\t);\n\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			//do no thing
		}
	}

	//channel
	ofile
	<<"\t//channels\n\n";
	router = noc.templateRouter();
	int j = 0;
	for (int i = 0; i< p_port_num ; i++)
	{
		port = router.port(i);
		channelInst(i,port.inputVcNumber(),j);
        port = router.port(i);
		j = j + port.inputVcNumber();
	}
    
	//Round Robin

	ofile
	<<"\t//assignment\n"
	<<"\t//solve intern_rr_request\n";
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tassign intern_rr_request[`P"<<i<<"_VC1] = out_rr_request_p"<<i<<"_vc1;\n"
			<<"\tassign intern_rr_request[`P"<<i<<"_VC2] = out_rr_request_p"<<i<<"_vc2;\n";
		
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\tassign intern_rr_request[`P"<<i<<"] = out_rr_request_p"<<i<<";\n";
		}
	}
	ofile
	<<"\n"
	<<"\n"

	//solve inter_rr_result
	<<"\t//solve intern_rr_result\n";
	for (int i = 0; i< p_port_num ; i++)
	{
		port = router.port(i);
		ofile
			<< "\t//inter_rr_result[" << i << "]\n";
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tm_RR_EvaluationUnit"
			<<"\tRR_p"<<i<<" (\n"
			<<"\t.RST(RST),\n"
			<<"\t.CLK(CLK),\n"
			<<"\t.request_0({\n";
			for (int j = (p_port_num-1); j>= 1 ; j--)
			{
				port = router.port(j);
				if(port.inputVcNumber() == 2)
				{
					ofile
					<<"\t\tintern_rr_request[`P"<<j<<"_VC1][`P"<<i<<"_VC1],\n"
					<<"\t\tintern_rr_request[`P"<<j<<"_VC2][`P"<<i<<"_VC1],\n";
				} else if(port.inputVcNumber() == 1)
				{
					ofile
					<<"\t\tintern_rr_request[`P"<<j<<"][`P"<<i<<"_VC1],\n";
				}
			}
			ofile
			<<"\t\tintern_rr_request[`P0][`P"<<i<<"_VC1]}),\n\n";

			ofile
			<<"\t.request_1({\n";
			for (int j = (p_port_num-1); j>= 1 ; j--)
			{
				port = router.port(j);
				if(port.inputVcNumber() == 2)
				{
					ofile
					<<"\t\tintern_rr_request[`P"<<j<<"_VC1][`P"<<i<<"_VC2],\n"
					<<"\t\tintern_rr_request[`P"<<j<<"_VC2][`P"<<i<<"_VC2],\n";
				} else if(port.inputVcNumber() == 1)
				{
					ofile
					<<"\t\tintern_rr_request[`P"<<j<<"][`P"<<i<<"_VC2],\n";
				}
			}
			ofile
			<<"\t\tintern_rr_request[`P0][`P"<<i<<"_VC2]}),\n\n";

			ofile
			<<"\t.grant_0(intern_rr_result[`P"<<i<<"_VC1]),\n"
			<<"\t.grant_1(intern_rr_result[`P"<<i<<"_VC2])\n"
			<<"\t);\n\n\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\tassign intern_rr_result[`P"<<i<<"] = ({\n";

			for (int j = (p_port_num-1); j>= 1 ; j--)
			{
				port = router.port(j);
				if(port.inputVcNumber() == 2)
				{
					ofile
					<<"\t\tintern_rr_request[`P"<<j<<"_VC1][`P"<<i<<"],\n"
					<<"\t\tintern_rr_request[`P"<<j<<"_VC2][`P"<<i<<"],\n";
				} else if(port.inputVcNumber() == 1)
				{
					ofile
					<<"\t\tintern_rr_request[`P"<<j<<"][`P"<<i<<"],\n";
				}
			}
			ofile
			<<"\t\tintern_rr_request[`P0][`P"<<i<<"]} != 0);\n\n";
		}
	}
	//end of round robin

	//switch allocation
	ofile
	<<"\t\n"
	<<"\t\n"
	<<"\t//module declaration\n"
	<<"\tm_SwitchAllocation"
	<<"\tswitchAllocation (\n";
	//start module declaration
	for(int i =0;i<p_port_num ; i++)
	{
		port = router.port(i);
		ofile
		<<"\t//connection for port "<<i<<"\n";
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\t.enable_p"<<i<<"_vc1(out_sa_enable_p"<<i<<"_vc1),\n"
			<<"\t.enable_p"<<i<<"_vc2(out_sa_enable_p"<<i<<"_vc2),\n"

			<<"\t.request_p"<<i<<"_vc1(out_sa_request_p"<<i<<"_vc1),\n"
			<<"\t.request_p"<<i<<"_vc2(out_sa_request_p"<<i<<"_vc2),\n"


			<<"\t.grant_p"<<i<<"_vc1(grant_p"<<i<<"_vc1),\n"
			<<"\t.grant_p"<<i<<"_vc2(grant_p"<<i<<"_vc2),\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\t.enable_p"<<i<<"(out_sa_enable_p"<<i<<"),\n"
			<<"\t.request_p"<<i<<"(out_sa_request_p"<<i<<"),\n"
			<<"\t.grant_p"<<i<<"(grant_p"<<i<<"),\n";
		}
		ofile << endl;
	}
	ofile
	<<"\t\n"
	<<"\t.CLK(CLK),\n"
	<<"\t.RST(RST)\n"
	<<"\t);\n\n\n";
	//end of switch allocation

	//crossbar
	//module
	ofile
	<<"\t//module declaration\n"
	<<"\tm_Crossbar #(\n"
	<<"\t.P_DATA_WIDTH(P_DATA_WIDTH + `DATA_VALID_WIDTH),\n"
	<<"\t.P_FEEDBACK_WIDTH(`BUFFERSIZE_WIDTH)\n"
	<<"\t) crossbar (\n";
	//net connection
    j = 0;
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		ofile
		<<"\t//for port "<<i<<"\n";
		if(port.inputVcNumber() == 2)
		{
			//for vc1
			ofile
			<<"\t.request_"<<j<<"(out_st_request_p"<<i<<"_vc1),\n"
			<<"\t.data_in_"<<j<<"({out_st_data_valid_p"<<i<<"_vc1,out_data_p"<<i<<"_vc1}),\n"
			<<"\t.data_out_"<<j<<"(crossbar_out_data_p"<<i<<"_vc1),\n"
			<<"\t.feedback_out_"<<j<<"(crossbar_feedback_out_p"<<i<<"_vc1),\n"
			<<"\t.feedback_in_"<<j<<"(p"<<i<<"_vc1_in_credits_feedback),\n\n";
			j++;
			//for vc2
			ofile
				<< "\t.request_" << j << "(out_st_request_p" << i << "_vc2),\n"
				<< "\t.data_in_" << j << "({out_st_data_valid_p"<<i<<"_vc2,out_data_p" << i << "_vc2}),\n"
				<<"\t.feedback_out_"<<j<<"(crossbar_feedback_out_p"<<i<<"_vc2),\n"
				<< "\t.data_out_" << j << "(crossbar_out_data_p" << i << "_vc2),\n";
			if(i == p_port_num - 1)
				ofile
				<<"\t.feedback_in_"<<j<<"(p"<<i<<"_vc2_in_credits_feedback));\n\n";
			else
				ofile
				<<"\t.feedback_in_"<<j<<"(p"<<i<<"_vc2_in_credits_feedback),\n\n";
			j++;

		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
				<< "\t.request_" << j << "(out_st_request_p" << i << "),\n"
				<< "\t.data_in_" << j << "({out_st_data_valid_p"<<i<<",out_data_p" << i << "}),\n"
				<<"\t.feedback_out_"<<j<<"(crossbar_feedback_out_p"<<i<<"),\n"
				<< "\t.data_out_" << j << "(crossbar_out_data_p" << i << "),\n";
			if(i == p_port_num - 1)
				ofile
				<<"\t.feedback_in_"<<j<<"(p"<<i<<"_in_credits_feedback));\n\n";
			else
				ofile
				<<"\t.feedback_in_"<<j<<"(p"<<i<<"_in_credits_feedback),\n\n";
			j++;
		}
	}
	//end of corssbar

	//data out mux


	ofile
	<<"\t//Mux Module declaration \n"
	<<"\t\n";

	//module
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		ofile
			<< "\t//Mux for port " << i << "\n";
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tm_Mux_2in1out #(\n"
			<<"\t.P_DATA_WIDTH(P_DATA_WIDTH + `DATA_VALID_WIDTH)\n"
			<<"\t) outMux_p"<<i<<" (\n"
			<<"\t.select(intern_set_p"<<i<<"),\n"
			<<"\t.data_in_0(crossbar_out_data_p"<<i<<"_vc1),\n"
			<<"\t.data_in_1(crossbar_out_data_p"<<i<<"_vc2),\n"
			<<"\t.data_out(crossbar_out_data_p"<<i<<")\n"
			<<"\t);\n"
			<<"\t\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			//do no thing
		}
	}
	ofile
	<<endl <<endl;
	//end of mux 

	//D_FF_sync 

	//module
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		ofile
			<< "\t//D_FF_sync  for port " << i << "\n"
			<< "\tm_D_FF_sync #(\n"
			<< "\t.P_DATA_WIDTH(`DATA_VALID_WIDTH + P_DATA_WIDTH)\n"
			<< "\t) output_buffer_data_p" << i << "(\n"
			<< "\t.CLK(CLK),\n"
			<< "\t.RST(RST),\n"
			<< "\t.D(crossbar_out_data_p" << i << "),\n"
			<< "\t.Q(p"<<i<<"_output_buffer)\n"
			<< "\t);\n\n";

		if(port.inputVcNumber() == 2)
		{
			ofile
				<< "\tm_D_FF_sync #(\n"
				<< "\t.P_DATA_WIDTH(1)\n"
				<< "\t) output_buffer_set_channel_p" << i << "(\n"
				<< "\t.CLK(CLK),\n"
				<< "\t.RST(RST),\n"
				<< "\t.D(intern_set_p" << i << "),\n"
				<< "\t.Q(p" << i << "_out_set_Channel)\n"
				<< "\t);\n\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			//do no thing
		}
	}
	ofile
	<<"\tendmodule\n";
    //end of router file

	cout<<"generate Router successfully\n";

    ofile.close();
}

void NOC_GEN::generateChannel()
{
	router = noc.templateRouter();
	ofile.open(FILE_PATH+CHANNEL_FILE_NAME);


	ofile
    <<"`timescale 1ns / 1ps\n"
    <<"`include \"parameters.v\"\n\n"
    <<"module m_Assembled_Channel (\n"
	//io port declare
    <<"\t router_id,\n"
    <<"\t CLK,\n"
    <<"\t RST,\n\n";
	for(int i=0 ; i<p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\t  in_neighbor_credits_p"<<i<<"_vc1,\n"
			<<"\t  in_neighbor_credits_p"<<i<<"_vc2,\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\t  in_neighbor_credits_p"<<i<<",\n";
		}
		ofile << endl;
	}
	ofile
		<< "\t//sa signals:\n"
		<< "\t out_sa_enable,\n"
		<< "\t  out_sa_request,\n"
		<< "\t in_sa_grant,\n\n"

		<< "\t//st signals:\n"
		<< "\t  out_st_request,\n"
		<< "\t  in_st_credits_feedback,\n"
		<< "\t out_st_data_valid,\n\n"

		<< "\tin_data,\n"
		<< "\tout_data,\n\n"

		<< "\t  out_buffer_credits_sharing,\n"
		<< "\t in_received_data_valid,\n\n"

		<< "\tout_rr_request,\n"
		<< "\tin_rr_result\n\n);\n"
		<< "\t//end of io declaration\n\n\n";
		//end of io declaration
		//parameters
	ofile
		<< "\t// Parameters\n"
		<< "\tparameter P_ROUTER_ID = 0;\n"
		<< "\tparameter P_CHANNEL = 0;\n"
		<< "\t`ifdef FT_ECC\n"
		<< "\t\tparameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH;\n"
		<< "\t`else \n"
		<< "\t\tparameter P_DATA_WIDTH = `FLIT_WIDTH;\n"
		<< "\t`endif\n"
		<< "\t\n"
		<< "\t\n";


	//io define
	ofile
		<<"\tinput [`FLIT_SRC_WIDTH - 1 : 0] router_id;\n"
		<<"\tinput CLK;\n"
		<<"\tinput RST;\n\n";
	for(int i=0 ; i<p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tinput [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p"<<i<<"_vc1;\n"
			<<"\tinput [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p"<<i<<"_vc2;\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\tinput [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p"<<i<<";\n";
		}
		ofile << endl;
	}
	ofile
		<< "\t//sa signals:\n"
		<< "\toutput out_sa_enable;\n"
		<< "\toutput [`CHANNELS - 1:0] out_sa_request;\n"
		<< "\tinput in_sa_grant;\n\n"

		<< "\t//st signals:\n"
		<< "\toutput [`CHANNELS - 1:0] out_st_request;\n"
		<< "\tinput [`BUFFERSIZE_WIDTH - 1:0] in_st_credits_feedback;\n"
		<< "\toutput out_st_data_valid;\n\n"

		<< "\tinput [P_DATA_WIDTH - 1: 0] in_data;\n"
		<< "\toutput [P_DATA_WIDTH - 1: 0] out_data;\n\n"

		<< "\toutput [`BUFFERSIZE_WIDTH - 1:0] out_buffer_credits_sharing;\n"
		<< "\tinput in_received_data_valid;\n\n"

		<< "\toutput [`CHANNELS - 1:0] out_rr_request;\n"
		<< "\tinput [`CHANNELS - 1:0] in_rr_result;\n\n";


	//wire define
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

	//RoutingCalculation 
	// Assignments
	ofile
	<<"\t// Assignments\n"
	<<"\tassign local_id = router_id;\n\n"


	// module
	<<"\t//module\n"
	<<"\tm_RoutingCalculation #(\n"
	<<"\t//.P_LOCAL_ID(P_ROUTER_ID),\n"
	<<"\t.P_LOCAL_CHANNEL(P_CHANNEL) \n"
	<<"\t)\n"
	<<"\troutingcalc (\n"
	<<"\t.RST(RST),\n\n"
	<<"\t.enable(channel_rc_enable),\n"
	<<"\t.local_in(local_id),\n\n"
	<<"\t.src_in(out_data[`FLIT_SRC_WIDTH + `FLIT_DST_WIDTH + `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_DST_WIDTH + `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH]),\n"
	<<"\t.dst_in(out_data[`FLIT_DST_WIDTH + `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH - 1: `FLIT_RESERVED_WIDTH + `FLIT_ID_WIDTH + `FLIT_FLAGS_WIDTH]),\n\n";
	int j = 0;
	for(int i=0 ; i<p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
				<< "\t.credits_" << j << "(in_neighbor_credits_p" << i << "_vc1),\n";
			j++;
			ofile
				<< "\t.credits_" << j << "(in_neighbor_credits_p" << i << "_vc2),\n";
			j++;
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
				<< "\t.credits_" << j << "(in_neighbor_credits_p" << i << "),\n";
			j++;
		}
		ofile << endl;
	}
	ofile
	<<"\t.success(channel_rc_success),\n"
	<<"\t.result(channel_rc_result)\n);\n\n\n";
	//end of RoutingCalculation module

	//buffer 

	//buffer module
	ofile
	<<"\tm_Buffer #(\n"
	<<"\t//.P_LOCAL_ID(P_ROUTER_ID),\n"
	<<"\t.P_LOCAL_CHANNEL(P_CHANNEL),\n"
	<<"\t.P_DATA_WIDTH(P_DATA_WIDTH)\n"
	<<"\t)\n"
	<<"\tbuffer (\n"
	<<"\t.RST(RST),\n"
	<<"\t.CLK(CLK),\n"
	<<"\t.command_write(channel_command_write),\n"
	<<"\t.command_read(channel_command_read),\n"
	<<"\t.buffer_credit_level(channel_buffer_credits_local),\n"
	<<"\t.data_in(in_data),\n"
	<<"\t`ifdef FT_ECC\n"
	<<"\t.data_out(channel_intercoder_data_in)\n"
	<<"\t`else\n"
	<<"\t.data_out(out_data)\n"
	<<"\t`endif\n"
	<<"\t\n);\n\n\n";

	//end of buffer module


	//state machine

	//module
	ofile
	<<"\t//module\n"
	<<"\tm_StateMachine	#(\n"
	<<"\t//.P_LOCAL_ID(P_ROUTER_ID),\n"
	<<"\t.P_LOCAL_CHANNEL(P_CHANNEL)\n"
	<<"\t)\n"
	<<"\tstatemachine (\n"
	<<"\t.router_id(router_id),\n"
	<<"\t.RST(RST),\n"
	<<"\t.CLK(CLK),\n\n"

	<<"\t// Data Handling + Channel/BufferControl\n"
	<<"\t.data_valid(in_received_data_valid),\n"
	<<"\t.command_write(channel_command_write),\n"
	<<"\t.command_read(channel_command_read),\n"
	<<"\t.buffer_credits_local(channel_buffer_credits_local),\n"
	<<"\t.buffer_credits_sharing(out_buffer_credits_sharing),\n"
	<<"\t.data_in(out_data[`FLIT_WIDTH - 1 : 0]),\n\n"

	<<"\t// RouteCalculation signals\n"
	<<"\t.rc_enable(channel_rc_enable),\n"
	<<"\t.rc_success(channel_rc_success),\n"
	<<"\t.rc_result(channel_rc_result),\n\n"

	<<"\t.sa_enable(out_sa_enable),\n"
	<<"\t.sa_request(out_sa_request),\n"
	<<"\t.sa_grant(in_sa_grant),\n\n"

	<<"\t.st_request(out_st_request),\n"
	<<"\t.st_data_valid(out_st_data_valid),\n"
	<<"\t.st_credits_feedback(in_st_credits_feedback),\n\n"

	<<"\t.rr_request(out_rr_request),\n"
	<<"\t.rr_result(in_rr_result),\n\n"

	<<"	// Assertion Unit error\n"
    <<"		`ifdef FT_RC_TOLERANT\n"
    <<"			`ifdef FT_SECURE_SM\n"
    <<"			.error_assertion_packet(channel_error_PACKET),\n"
    <<"			.error_assertion_rc(channel_error_RC),\n"
    <<"			.error_assertion_sa(in_router_assertion_error_SA),\n"
    <<"			`endif\n"
    <<"		`else\n"
    <<"			`ifdef FT_SECURE_SM\n"
    <<"			.error_assertion_packet(1'b0,)\n"
    <<"			.error_assertion_rc(1'b0),\n"
    <<"			.error_assertion_sa(1'b0),\n"
    <<"			`endif\n"
    <<"		`endif\n"
    <<"		\n"
    <<"		`ifdef FT_ECC\n"
    <<"		// ECC Error\n"
    <<"		.error_ecc(channel_error_ECC),\n"
    <<"		`else\n"
    <<"		.error_ecc(1'b0),\n"
    <<"		`endif\n\n"

	<<"\t.current_state(channel_current_state)\n"
	<<"\t\n);\n\n\n";


	//log module
	ofile
	//module
	<< "\tm_LogModule #(\n"
	<< "\t//.P_LOCAL_ID(P_ROUTER_ID),\n"
	<< "\t.P_LOCAL_CHANNEL(P_CHANNEL) \n"
	<< "\t) logger (\n"
	<< "\t.router_id(router_id),\n"
	<< "\t.RST(RST),\n"
	<< "\t.CLK(CLK),\n\n\n"
	<< "\t.buffer_data_in(in_data[`FLIT_WIDTH - 1 : 0]),\n"
	<< "\t.buffer_write(channel_command_write),\n"
	<< "\t.buffer_credits(channel_buffer_credits_local),\n"
	<< "\t.statemachine_data_in(out_data[`FLIT_WIDTH - 1 : 0]),\n"
	<< "\t.rc_success(channel_rc_success),\n"
	<< "\t.rc_result(channel_rc_result),\n"
	<< "\t.sa_grant(in_sa_grant),\n"
	<< "\t.st_data_valid(out_st_data_valid),\n"
	<< "\t.current_state(channel_current_state)\n"
    << "\t\n);\n";

	ofile
	<<"\tendmodule\n";

	cout<<"generate Channel successfully\n";

	ofile.close();
}

void NOC_GEN::generateSwitchAllocation()
{
	router = noc.templateRouter();
	ofile.open(FILE_PATH+Switch_Allocation_FILE_NAME);
	ofile
    <<"`timescale 1ns / 1ps\n"
    <<"`include \"parameters.v\"\n\n"
    <<"module m_SwitchAllocation (\n";
	//io port declare
	for(int i =0;i<p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tinput enable_p"<<i<<"_vc1,\n"
			<<"\tinput enable_p"<<i<<"_vc2,\n"

			<<"\tinput [`CHANNELS - 1 : 0] request_p"<<i<<"_vc1,\n"
			<<"\tinput [`CHANNELS - 1 : 0] request_p"<<i<<"_vc2,\n"


			<<"\toutput grant_p"<<i<<"_vc1,\n"
			<<"\toutput grant_p"<<i<<"_vc2,\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\tinput enable_p"<<i<<",\n"
			<<"\tinput [`CHANNELS - 1 : 0] request_p"<<i<<",\n"
			<<"\toutput grant_p"<<i<<",\n";
		}
		ofile << endl;
	}
	ofile
    <<"\tinput CLK,\n"
    <<"\tinput RST\n);\n\n";

	//parameters

	ofile
	<< "\t// Parameters\n"
	<< "\tparameter	P_ROUTER_ID = 0;\n\n"

	// Variables	 
	<< "\twire [`CHANNELS - 1 : 0] enable_vector; \n"
	<< "\twire [`CHANNELS - 1 : 0] request_vector [`CHANNELS - 1 : 0];\n"
	<< "\twire [`CHANNELS - 1 : 0] grant_vector[`CHANNELS - 1 : 0];\n"
	<< "\twire [`CHANNELS - 1 : 0] matrixHold_vector; \n\n";

	// Assignments
	// Assigning inputs to flexible variable
		for(int i =0;i<p_port_num ; i++)
		{
		ofile
		<<"//port"<<i<<" assignment :\n";
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tassign enable_vector[`P"<<i<<"_VC1] = enable_p"<<i<<"_vc1;\n"
			<<"\tassign enable_vector[`P"<<i<<"_VC2] = enable_p"<<i<<"_vc2;\n\n"
			<<"\tassign request_vector[`P"<<i<<"_VC1] = request_p"<<i<<"_vc1;\n"
			<<"\tassign request_vector[`P"<<i<<"_VC2] = request_p"<<i<<"_vc2;\n\n"
			<<"\tassign grant_p"<<i<<"_vc1 = \n";
			for(int j = (p_port_num-1);j>=0 ; j--)
			{
				if(router.port(j).inputVcNumber()==2)
				{
					//if it is the last line of assignment
					if(j == 0)
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"_VC1][`P"<<i<<"_VC1] ^\n"
					<<"\tgrant_vector[`P"<<j<<"_VC2][`P"<<i<<"_VC1] ;\n\n";
					}
					else
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"_VC1][`P"<<i<<"_VC1] ^\n"
					<<"\tgrant_vector[`P"<<j<<"_VC2][`P"<<i<<"_VC1] ^\n";
					}
				}
				else if(router.port(j).inputVcNumber() == 1)
				{
					//if it is the last line of assignment
					if(j == 0)
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"][`P"<<i<<"_VC1] ;\n\n";
					}
					else
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"][`P"<<i<<"_VC1] ^\n";
					}
				}
			}

			ofile
			<<"\tassign grant_p"<<i<<"_vc2 = \n";
			for(int j = (p_port_num-1);j>=0 ; j--)
			{
				if(router.port(j).inputVcNumber()==2)
				{
					//if it is the last line of assignment
					if(j == 0)
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"_VC1][`P"<<i<<"_VC2] ^\n"
					<<"\tgrant_vector[`P"<<j<<"_VC2][`P"<<i<<"_VC2] ;\n\n";
					}
					else
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"_VC1][`P"<<i<<"_VC2] ^\n"
					<<"\tgrant_vector[`P"<<j<<"_VC2][`P"<<i<<"_VC2] ^\n";
					}
				}
				else if(router.port(j).inputVcNumber() == 1)
				{
					//if it is the last line of assignment
					if(j == 0)
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"][`P"<<i<<"_VC2] ;\n\n";
					}
					else
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"][`P"<<i<<"_VC2] ^\n";
					}
				}
			}
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\tassign enable_vector[`P"<<i<<"] = enable_p"<<i<<";\n\n"
			<<"\tassign request_vector[`P"<<i<<"] = request_p"<<i<<";\n\n";

			ofile
			<<"\tassign grant_p"<<i<<" = \n";
			for(int j = (p_port_num-1);j>=0 ; j--)
			{
				if(router.port(j).inputVcNumber()==2)
				{
					//if it is the last line of assignment
					if(j == 0)
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"_VC1][`P"<<i<<"] ^\n"
					<<"\tgrant_vector[`P"<<j<<"_VC2][`P"<<i<<"] ;\n";
					}
					else
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"_VC1][`P"<<i<<"] ^\n"
					<<"\tgrant_vector[`P"<<j<<"_VC2][`P"<<i<<"] ^\n";
					}
				}
				else if(router.port(j).inputVcNumber() == 1)
				{
					//if it is the last line of assignment
					if(j == 0)
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"][`P"<<i<<"] ;\n";
					}
					else
					{
					ofile
					<<"\tgrant_vector[`P"<<j<<"][`P"<<i<<"] ^\n";
					}
				}
			}
		}
		ofile << endl<<endl;
	}

	ofile
	<<"\n\n\t// Generating-Block: Generates MatrixArbiter; 1 per Out Channel\n"
	<<"\tgenvar i_var;\n"
	<<"\tgenerate\n"
	<<"\tfor (i_var = 0; i_var < `CHANNELS; i_var = i_var + 1) begin:GENERATINGEVALUATION\n"
	<<"\tm_EvaluationUnit # (\n"
	<<"\t//.P_ROUTER_ID(P_ROUTER_ID),\n"
	<<"\t.P_CHANNEL_ID(i_var)\n"
	<<"\t) evalUnit (\n"
	<<"\t.enables({";
	for(int i =(p_port_num-1) ; i >=0 ; i--)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			if(i == 0)
			{
				ofile
				<<"enable_vector[`P"<<i<<"_VC2],"
				<<"enable_vector[`P"<<i<<"_VC1]}),\n";
			}
			else
			{
				ofile
				<<"enable_vector[`P"<<i<<"_VC2],"
				<<"enable_vector[`P"<<i<<"_VC1],";
			}
		}
		else if(port.inputVcNumber() == 1)
		{
			if(i == 0)
			{
				ofile
				<<"enable_vector[`P"<<i<<"]}),\n";
			}
			else
			{
				ofile
				<<"enable_vector[`P"<<i<<"],";
			}
		}
	}

	ofile
	<<"\t.requests({";
	for(int i =(p_port_num-1) ; i >=0 ; i--)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			if(i == 0)
			{
				ofile
				<<"request_vector[`P"<<i<<"_VC2][i_var],\n"
				<<"request_vector[`P"<<i<<"_VC1][i_var]}),\n";
			}
			else
			{
				ofile
				<<"request_vector[`P"<<i<<"_VC2][i_var],"
				<<"request_vector[`P"<<i<<"_VC1][i_var],";
			}
		}
		else if(port.inputVcNumber() == 1)
		{
			if(i == 0)
			{
				ofile
				<<"request_vector[`P"<<i<<"][i_var]}),\n";
			}
			else
			{
				ofile
				<<"request_vector[`P"<<i<<"][i_var],";
			}
		}
	}
	ofile
		<< "\t.grants(grant_vector[i_var]),\n"
		<< "\t.matrixHold(matrixHold_vector[i_var]),\n\n"
		<< "\t.RST(RST),\n"
		<< "\t.CLK(CLK)\n);\n\n"
		<< "\t\tend\n"
		<< "\tendgenerate\n\n"
		<< "\tendmodule\n";

	cout<<"generate Switch Allocation successfully\n";

	ofile.close();

}

void NOC_GEN::generateRoutingCalculation()
{
	router = noc.templateRouter();
	ofile.open(FILE_PATH+Routing_Calculation_FILE_NAME);
	ofile
    <<"`timescale 1ns / 1ps\n"
    <<"`include \"parameters.v\"\n\n"
    <<"module m_RoutingCalculation (\n"
    <<"\tinput enable,\n"
    <<"\tinput RST,\n\n";
	for(int i=0 ; i<p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tinput [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p"<<i<<"_vc1,\n"
			<<"\tinput [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p"<<i<<"_vc2,\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
			<<"\tinput [`BUFFERSIZE_WIDTH - 1:0] in_neighbor_credits_p"<<i<<",\n";
		}
	}
	ofile
	<< "\tinput [`FLIT_SRC_WIDTH - 1 : 0] local_in,\n"
	<< "\tinput [`FLIT_SRC_WIDTH - 1 : 0] src_in,\n"
	<< "\tinput [`FLIT_DST_WIDTH - 1 : 0] dst_in,\n"
	<< "\toutput reg success,\n"
	<< "\toutput reg [`CHANNELS - 1:0] result\n);\n"

	<< "\t// Parameters\n"
	<< "\tparameter 	P_LOCAL_ID = 0;\n"
	<< "\tparameter		P_LOCAL_CHANNEL = 0;		// e.g. 0, 1, 2, 3,...\n"


	<< "\tendmodule\n";

	cout<<"generate Routing Calculation successfully\n";

	ofile.close();
}

void NOC_GEN::generateNoc(bool fault_en)
{
	//start generate noc.v

	int neighborPort;
	int neighborRouter;

	if(fault_en)
    	ofile.open(FILE_PATH+NOC_FILE_NAME_FAULTY);
	else
    	ofile.open(FILE_PATH+NOC_FILE_NAME);

    ofile
    <<"`timescale 1ns / 1ps\n"
    <<"`include \"parameters.v\"\n\n"
    <<"module m_Assembled_NoC (\n"
    <<"\t CLK,\n"
    <<"\t RST,\n\n";
    //declare io
    for(int i = 0 ; i<router_num ; i++)
    {
        ofile
        <<"\t//local port signals of router_"<<i<<endl
        <<"\t r"<<i<<"_p0_in_data_valid,\n"
        <<"\t r"<<i<<"_p0_out_data_valid,\n"
        <<"\tr"<<i<<"_p0_in_data,\n"
        <<"\tr"<<i<<"_p0_out_data,\n"
        <<"\tr"<<i<<"_p0_in_credits_feedback,\n";
        if(i==(router_num-1))
        ofile
        <<"\tr"<<i<<"_p0_out_credits_feedback\n);\n\n";
        else
        ofile
        <<"\tr"<<i<<"_p0_out_credits_feedback,\n\n";
    }
    ofile<<endl;

    //define parameters
	ofile
		<< "\t//parameters\n"
		<< "\t`ifdef FT_ECC\n"
		<< "\t    parameter P_DATA_WIDTH = `FLIT_ECC_CHECKSUM_WIDTH + `FLIT_WIDTH; \n"
		<< "\t`else \n"
		<< "\t	parameter P_DATA_WIDTH = `FLIT_WIDTH;\n"
		<< "\t`endif\n\n";

	//define zero signals
	ofile
		<<"\t//define zero signals\n"
		<<"\twire [P_DATA_WIDTH-1 : 0] d_zero;\n"
		<<"\twire [`BUFFERSIZE_WIDTH-1 : 0] cf_zero;\n\n"
		<<"\tassign d_zero = 'b0;\n"
		<<"\tassign cf_zero = 'b0;\n"
		<<"\t\n"
		<<"\t\n";

	//define io
	ofile
	<<"\tinput CLK;\n"
    <<"\tinput RST;\n\n";
    //declare io
    for(int i = 0 ; i<router_num ; i++)
    {
        ofile
        <<"\t//local port signals of router_"<<i<<endl
        <<"\tinput r"<<i<<"_p0_in_data_valid;\n"
        <<"\toutput r"<<i<<"_p0_out_data_valid;\n"
        <<"\tinput [P_DATA_WIDTH - 1: 0] r"<<i<<"_p0_in_data;\n"
        <<"\toutput [P_DATA_WIDTH - 1: 0] r"<<i<<"_p0_out_data;\n"
        <<"\tinput [`BUFFERSIZE_WIDTH - 1 : 0] r"<<i<<"_p0_in_credits_feedback;\n"
        <<"\toutput [`BUFFERSIZE_WIDTH - 1: 0] r"<<i<<"_p0_out_credits_feedback;\n\n";
    }

    //declar wire
    for(int i = 0 ; i<router_num ; i++)
    {
        router = noc.router(i);
        for(int j = 1 ; j<p_port_num ; j++)
        {
            port = router.port(j);
            ofile
            <<"\t//router_"<<i<<" port_"<<j<<":\n"
            <<"\twire [P_DATA_WIDTH - 1: 0] r"<<i<<"_p"<<j<<"_out_data;\n"
            <<"\twire r"<<i<<"_p"<<j<<"_out_data_valid;\n";
            if(port.inputVcNumber() == 2)
            {
                ofile
                <<"\twire [`BUFFERSIZE_WIDTH - 1:0] r"<<i<<"_p"<<j<<"_vc1_out_credits_feedback;\n"
                <<"\twire [`BUFFERSIZE_WIDTH - 1:0] r"<<i<<"_p"<<j<<"_vc2_out_credits_feedback;\n"
                <<"\twire  r"<<i<<"_p"<<j<<"_out_set_channel;\n";
            }else
            {
                ofile
                <<"\twire [`BUFFERSIZE_WIDTH - 1:0] r"<<i<<"_p"<<j<<"_out_credits_feedback;\n";
            }
            ofile << endl;
        }
    }

    //declare module
    for(int i = 0 ; i<router_num ; i++)
    {
        router = noc.router(i);
		ofile
			<< "//declare router" << i << endl
			<< "\twire [`FLIT_SRC_WIDTH-1 : 0] router_id_"<<i<<";\n"
			<< "\tassign router_id_"<<i<<" = "<<i<<";\n";

		if(fault_en)
			ofile << "\tm_Assembled_Router_faulty";
		else
			ofile << "\tm_Assembled_Router";

		ofile
			<< "\trouter_" << i << "(\n"
			<< "\t.router_id(router_id_"<<i<<"),\n"
			<< "\t.RST(RST),\n"
			<< "\t.CLK(CLK),\n";

		if(fault_en)
			ofile << "\t.faultClock(CLK),\n\n";

			//local port
		ofile
			<< "\t.p0_in_data(r" << i << "_p0_in_data),\n"
			<< "\t.p0_out_data(r" << i << "_p0_out_data),\n"
			<< "\t.p0_in_data_valid(r" << i << "_p0_in_data_valid),\n"
			<< "\t.p0_out_data_valid(r" << i << "_p0_out_data_valid),\n"
			<< "\t.p0_in_credits_feedback(r" << i << "_p0_in_credits_feedback),\n"
			<< "\t.p0_out_credits_feedback(r" << i << "_p0_out_credits_feedback),\n\n";
        //other ports
        for(int j = 1 ; j<p_port_num ; j++)
        {
            port = router.port(j);
            neighborPort = port.neighborPort();
            neighborRouter = port.neighborRouter();
            if(neighborPort == -1)
            {
				port = noc.templateRouter().port(j);
                ofile
                <<"//port_"<<j<<endl
                <<"\t.p"<<j<<"_in_data(d_zero),\n"
                <<"\t.p"<<j<<"_out_data(),\n"

                <<"\t.p"<<j<<"_in_data_valid(1'b0),\n"
                <<"\t.p"<<j<<"_out_data_valid(),\n";

                if(port.inputVcNumber() == 2)
                {
                    ofile
                    <<"\t.p"<<j<<"_in_set_Channel(1'b0),\n"
                    <<"\t.p"<<j<<"_vc1_in_credits_feedback(cf_zero),\n"
                    <<"\t.p"<<j<<"_vc2_in_credits_feedback(cf_zero),\n"

                    <<"\t.p"<<j<<"_out_set_Channel(),\n"
                    <<"\t.p"<<j<<"_vc1_out_credits_feedback(),\n"
                    <<"\t.p"<<j<<"_vc2_out_credits_feedback(";
                    if( j==(p_port_num-1) )
                    ofile
                    <<")\n);\n";
                    else
                    ofile
                    <<"),\n";
                }else
                {
                    ofile
                    <<"\t.p"<<j<<"_in_credits_feedback(cf_zero),\n"
                    <<"\t.p"<<j<<"_out_credits_feedback(";
                    if( j==(p_port_num-1) )
                    ofile                
                    <<")\n);\n";
                    else
                    ofile                
                    <<"),\n";   
                }
                ofile << endl;
            }else
            {
                ofile
                <<"//port_"<<j<<endl
                <<"\t.p"<<j<<"_in_data("
                <<"r"<<neighborRouter<<"_p"<<neighborPort<<"_out_data),\n"
                <<"\t.p"<<j<<"_out_data("
                <<"r"<<i<<"_p"<<j<<"_out_data),\n"

                <<"\t.p"<<j<<"_in_data_valid("
                <<"r"<<neighborRouter<<"_p"<<neighborPort<<"_out_data_valid),\n"
                <<"\t.p"<<j<<"_out_data_valid("
                <<"r"<<i<<"_p"<<j<<"_out_data_valid),\n";

                if(port.inputVcNumber() == 2)
                {
                    ofile
                    <<"\t.p"<<j<<"_in_set_Channel("
                    <<"r"<<neighborRouter<<"_p"<<neighborPort<<"_out_set_channel),\n"
                    <<"\t.p"<<j<<"_vc1_in_credits_feedback("
                    <<"r"<<neighborRouter<<"_p"<<neighborPort<<"_vc1_out_credits_feedback),\n"
                    <<"\t.p"<<j<<"_vc2_in_credits_feedback("
                    <<"r"<<neighborRouter<<"_p"<<neighborPort<<"_vc2_out_credits_feedback),\n"

                    <<"\t.p"<<j<<"_out_set_Channel("
                    <<"r"<<i<<"_p"<<j<<"_out_set_channel),\n"
                    <<"\t.p"<<j<<"_vc1_out_credits_feedback("
                    <<"r"<<i<<"_p"<<j<<"_vc1_out_credits_feedback),\n"
                    <<"\t.p"<<j<<"_vc2_out_credits_feedback(";
                    if( j==(p_port_num-1) )
                    ofile
                    <<"r"<<i<<"_p"<<j<<"_vc2_out_credits_feedback)\n);\n";
                    else
                    ofile
                    <<"r"<<i<<"_p"<<j<<"_vc2_out_credits_feedback),\n";
                }else
                {
                    ofile
                    <<"\t.p"<<j<<"_in_credits_feedback("
                    <<"r"<<neighborRouter<<"_p"<<neighborPort<<"_out_credits_feedback),\n"
                    <<"\t.p"<<j<<"_out_credits_feedback(";
                    if( j==(p_port_num-1) )
                    ofile                
                    <<"r"<<i<<"_p"<<j<<"_out_credits_feedback)\n);\n";
                    else
                    ofile                
                    <<"r"<<i<<"_p"<<j<<"_out_credits_feedback),\n";   
                }
                ofile << endl;
            }
        }
    }
    ofile
    <<"\tendmodule";
	ofile.close();
}

void NOC_GEN::writeHelper(string writeString)
{
	ofile << writeString << endl;
}

string NOC_GEN::createVector(int bitIndex, int portNumber)
{
    string result = to_string(portNumber) + "'b";
    if (bitIndex < 0)
    {
        for(int i=0 ; i< portNumber ; i++)
            result += "0";
        return result;
    }
    else
    {
        for(int i=0
			; i< portNumber ; i++)
            if ( (portNumber - i - 1) == bitIndex )
                result += "1";
            else
                result += "0";
        return result;
    }
}

void NOC_GEN::generateMux(int demandedPorts)
{
	// Writes to file and auto adds a newline..."hacky"/ bad coding style but..yeah
	// Start writing
	// File Informationen
    ofile.open(FILE_PATH+MUX_FILE_NAME);
	string selector;

	// Includes
	writeHelper("\n");
	writeHelper("`timescale 1ns / 1ps");
	writeHelper("\n");
	writeHelper("`include \"parameters.v\"");

	// Start of the module
	writeHelper("module m_Mux_NoEncoding(");

	// Ports
	writeHelper("\t\tselect,");
	writeHelper("\t\tdata_out,");
	// for x in range(0, demandedPorts + 1):
	// 	if (x == demandedPorts):
	// 		writeHelper("\t\tdata_in_" + to_string(x));
	// 	else:
	// 		writeHelper("\t\tdata_in_" + to_string(x) + ",");
	for (int x = 0; x <= demandedPorts ; x++)
	{
		if (x == demandedPorts)
			writeHelper("\t\tdata_in_" + to_string(x));
		else
		writeHelper("\t\tdata_in_" + to_string(x) + ",");
	}

	writeHelper("\t);");

	// Parameter
	writeHelper("");
	writeHelper("\t// Parameters");
	writeHelper("\tparameter P_DATA_WIDTH = 8;");

	// Port Definition
	writeHelper("");
	writeHelper("\t// Port Definitions");
	writeHelper("\tinput [" + to_string(demandedPorts) + " - 1 : 0] select;");
	for (int x = 0; x <= demandedPorts; x++)
		writeHelper("\tinput [P_DATA_WIDTH - 1 : 0] data_in_" + to_string(x) + ";");
	writeHelper("\toutput reg [P_DATA_WIDTH - 1 : 0] data_out;");

	// Variables
	writeHelper("");
	writeHelper("\t// Variables");

	// Assignments
	writeHelper("");
	writeHelper("\t// Assignments");

	// Behavior
	writeHelper("");
	writeHelper("\t// Behavior");
	writeHelper("\tinitial begin:Initialization");
	writeHelper("\t\t// Empty ");
	writeHelper("\tend");

	writeHelper("");
	writeHelper("\t// Switch block: Puts the selected in data to outport");
	writeHelper("\talways @ (select ");
	// for x in range(0, demandedPorts + 1):
	for (int x = 0; x <= demandedPorts; x++)
		writeHelper("\t\tor data_in_" + to_string(x));
	writeHelper("\t) begin:MUX");
	writeHelper("\t\tcase (select)");
	// for x in range(0, demandedPorts + 1):
	for (int x = 0; x <= demandedPorts; x++)
    {
		selector = to_string(demandedPorts) + "'b";
        if (x != (demandedPorts))
        {
            for (int j = 1; j <= demandedPorts; j++)
                // if x == (demandedPorts - j):
                if (x == (demandedPorts - j))
                    selector += "1";
                else
                    selector += "0";
        }
        else
        {
            for (int j = 1; j <= demandedPorts; j++)
                selector += "0";
            writeHelper("\t\t\t// In \"IDLE\" position selector == 0 -> \"last\" port is selected which should be connected to 0");
        }

	    writeHelper("\t\t\t" + selector + " : data_out = data_in_" + to_string(x) + ";");
    }

	writeHelper("\t\t\tdefault: data_out = 0;");

	writeHelper("\t\tendcase");
	writeHelper("\tend");

	// End of the module
	writeHelper("");
	writeHelper("endmodule");
    ofile.close();
	// Write output End
    cout << "generate mux successfully\n";
}

void NOC_GEN::generatingFeedbackSwitch(int demandedPorts)
{

	
		// Writes to file and auto adds a newline..."hacky"/ bad coding style but..yeah
	
	// Start writing
    ofile.open(FILE_PATH+Feedback_Switch_FILE_NAME);

	// Includes
	writeHelper("\n");
	writeHelper("`timescale 1ns / 1ps");
	writeHelper("\n");
	writeHelper("`include \"parameters.v\"");

	// Start of the module
	writeHelper("module m_Switch_Feedback(");
	
	// Ports
	// for x in range(0, demandedPorts):
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\t\tsel_" + to_string(x) + ",");
		
	writeHelper("");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\t\tfeedback_in_" + to_string(x) + ",");
	
	writeHelper("");
	for(int x = 0; x<demandedPorts ; x++)
		if (x == demandedPorts - 1)
			writeHelper("\t\tfeedback_out_" + to_string(x));
		else
			writeHelper("\t\tfeedback_out_" + to_string(x) + ",");
			
	writeHelper("\t);");
	
	// Parameter
	writeHelper("");
	writeHelper("\t// Parameters");
	writeHelper("\tparameter P_FEEDBACK_WIDTH = 8;");
	
	// Port Definition
	writeHelper("");
	writeHelper("\t// Port Definitions");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\tinput [" + to_string(demandedPorts) + " - 1 : 0] sel_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\tinput [P_FEEDBACK_WIDTH - 1 : 0] feedback_in_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\toutput [P_FEEDBACK_WIDTH - 1 : 0] feedback_out_" + to_string(x) + ";");
	
	// Variables
	writeHelper("");
	writeHelper("\t// Variables");
	writeHelper("\twire [" + to_string(demandedPorts) + " - 1 : 0] select [" + to_string(demandedPorts) + " - 1 : 0];");
	writeHelper("\twire [P_FEEDBACK_WIDTH - 1 : 0] feedback_in [" + to_string(demandedPorts) + " - 1 : 0];");
	writeHelper("\twire [P_FEEDBACK_WIDTH - 1 : 0] feedback_out [" + to_string(demandedPorts) + " - 1 : 0];");
	writeHelper("\twire [P_FEEDBACK_WIDTH - 1 : 0] const_0;");
	
	// Assignments
	writeHelper("");
	writeHelper("\t// Assignments");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\tassign select[" + to_string(x) + "]=sel_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\tassign feedback_in[" + to_string(x) + "]=feedback_in_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\tassign feedback_out_" + to_string(x) + "=feedback_out[" + to_string(x) + "];");
	
	writeHelper("");
	writeHelper("\tassign const_0 = 0;");
	
	// Behavior
	writeHelper("");
	writeHelper("\t// Behavior");
	writeHelper("\tinitial begin:Initialization");
	writeHelper("\t\t// Empty ");
	writeHelper("\tend");
	
	writeHelper("");
    writeHelper("\t// Generating-Block: Generates MUX for switching the feedback");
	writeHelper("\t// The select is directly controlled by the State machine");
	writeHelper("\tgenvar i;");
	writeHelper("\tgenerate");
	writeHelper("\t\tfor (i = 0; i < " + to_string(demandedPorts) + "; i = i + 1) begin:GENERATINGMUX");
	writeHelper("\t\t\tm_Mux_NoEncoding");
	writeHelper("\t\t\t# (");
	writeHelper("\t\t\t\t.P_DATA_WIDTH(P_FEEDBACK_WIDTH)");
	writeHelper("\t\t\t) mux_feedback (");
	writeHelper("\t\t\t\t.select(select[i])");
	writeHelper("\t\t\t\t,.data_out(feedback_out[i])");
	for(int x = 0; x<demandedPorts ; x++)
		writeHelper("\t\t\t\t,.data_in_" + to_string(x) + "(feedback_in[" + to_string(x) + "])");
	writeHelper("\t\t\t\t,.data_in_" + to_string(demandedPorts) + "(const_0)");
	writeHelper("\t\t\t);");
	writeHelper("\t\tend");
	writeHelper("\tendgenerate");

	
	// End of the module
	writeHelper("");
	writeHelper("endmodule");
	
	// Write output End
	ofile.close();

	cout<<"generate Feedback Switch successfully\n";

}

void NOC_GEN::generateDataSwitch(int demandedPorts)
{
		
	// Start writing
	ofile.open(FILE_PATH + Data_Switch_FILE_NAME);


	// Includes
	writeHelper("\n");
	writeHelper("`timescale 1ns / 1ps");
	writeHelper("\n");
	writeHelper("`include \"parameters.v\"");

	// Start of the module
	writeHelper("module m_Switch_Data(");
	
	// Ports
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\t\tsel_" + to_string(x) + ",");
		
	writeHelper("");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\t\tdata_in_" + to_string(x) + ",");
	
	writeHelper("");
	for(int x = 0 ; x<demandedPorts ; x++)
		if (x == demandedPorts - 1)
			writeHelper("\t\tdata_out_" + to_string(x));
		else
			writeHelper("\t\tdata_out_" + to_string(x) + ",");
			
	writeHelper("\t);");
	
	// Parameter
	writeHelper("");
	writeHelper("\t// Parameters");
	writeHelper("\tparameter P_DATA_WIDTH = 8;");
	
	// Port Definition
	writeHelper("");
	writeHelper("\t// Port Definitions");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\tinput [" + to_string(demandedPorts) + " - 1 : 0] sel_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\tinput [P_DATA_WIDTH - 1 : 0] data_in_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\toutput [P_DATA_WIDTH - 1 : 0] data_out_" + to_string(x) + ";");
	
	// Variables
	writeHelper("");
	writeHelper("\t// Variables");
	writeHelper("\twire [" + to_string(demandedPorts) + " - 1 : 0] select [" + to_string(demandedPorts) + " - 1 : 0];");
	writeHelper("\twire [P_DATA_WIDTH - 1 : 0] data_in [" + to_string(demandedPorts) + " - 1 : 0];");
	writeHelper("\twire [P_DATA_WIDTH - 1 : 0] data_out [" + to_string(demandedPorts) + " - 1 : 0];");
	writeHelper("\twire [P_DATA_WIDTH - 1 : 0] const_0;");
	
	// Assignments
	writeHelper("");
	writeHelper("\t// Assignments");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\tassign select[" + to_string(x) + "]=sel_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\tassign data_in[" + to_string(x) + "]=data_in_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\tassign data_out_" + to_string(x) + "=data_out[" + to_string(x) + "];");
	
	writeHelper("");
	writeHelper("\tassign const_0 = 0;");
	
	// Behavior
	writeHelper("");
	writeHelper("\t// Behavior");
	writeHelper("\tinitial begin:Initialization");
	writeHelper("\t\t// Empty ");
	writeHelper("\tend");
	
	writeHelper("");
	writeHelper("\t// Generating-Block: Generates MUX for switching the feedback");
	writeHelper("\t// The select is directly controlled by the State machine");
	writeHelper("\tgenvar i;");
	writeHelper("\tgenerate");
	writeHelper("\t\tfor (i = 0; i < " + to_string(demandedPorts) + "; i = i + 1) begin:GENERATINGMUX");
	writeHelper("\t\t\tm_Mux_NoEncoding");
	writeHelper("\t\t\t# (");
	writeHelper("\t\t\t\t.P_DATA_WIDTH(P_DATA_WIDTH)");
	writeHelper("\t\t\t) mux_data (");
	writeHelper("\t\t\t\t.select(select[i])");
	writeHelper("\t\t\t\t,.data_out(data_out[i])");
	for(int x = 0 ; x<demandedPorts ; x++)
		writeHelper("\t\t\t\t,.data_in_" + to_string(x) + "(data_in[" + to_string(x) + "])");
	writeHelper("\t\t\t\t,.data_in_" + to_string(demandedPorts) + "(const_0)");
	writeHelper("\t\t\t);");
	writeHelper("\t\tend");
	writeHelper("\tendgenerate");

	
	// End of the module
	writeHelper("");
	writeHelper("endmodule");
	
	// Write output End
	ofile.close();

	cout<<"generate Data Switch successfully\n";
}

void  NOC_GEN::generateCrossbar(int demandedPorts)
{	
	// Start writing
	ofile.open(FILE_PATH + Crossbar_FILE_NAME);

	// Includes
	writeHelper("\n");
	writeHelper("`timescale 1ns / 1ps");
	writeHelper("\n");
	writeHelper("`include \"parameters.v\"");

	// Start of the module
	writeHelper("module m_Crossbar(");
	
	// Ports
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\t\trequest_" + to_string(x) + ",");
		
	writeHelper("");
	for (int x = 0; x < demandedPorts; x++)
		writeHelper("\t\tdata_in_" + to_string(x) + ",");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\t\tdata_out_" + to_string(x) + ",");
		
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\t\tfeedback_in_" + to_string(x) + ",");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		if (x == demandedPorts - 1)
			writeHelper("\t\tfeedback_out_" + to_string(x));
		else
			writeHelper("\t\tfeedback_out_" + to_string(x) + ",");
			
	writeHelper("\t);");
	
	// Parameter
	writeHelper("");
	writeHelper("\t// Parameters");
	writeHelper("\tparameter P_DATA_WIDTH = `FLIT_WIDTH + `DATA_VALID_WIDTH;");
	writeHelper("\tparameter P_FEEDBACK_WIDTH = `BUFFERSIZE_WIDTH;");
	
	// Port Definition
	writeHelper("");
	writeHelper("\t// Port Definitions");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\tinput [" + to_string(demandedPorts) + " - 1 : 0] request_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\tinput [P_DATA_WIDTH - 1 : 0] data_in_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\toutput [P_DATA_WIDTH - 1 : 0] data_out_" + to_string(x) + ";");
		
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\tinput [P_FEEDBACK_WIDTH - 1 : 0] feedback_in_" + to_string(x) + ";");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\toutput [P_FEEDBACK_WIDTH - 1 : 0] feedback_out_" + to_string(x) + ";");
	
	// Variables
	writeHelper("");
	writeHelper("\t// Variables");
	writeHelper("\twire [" + to_string(demandedPorts) + " - 1 : 0] request [" + to_string(demandedPorts) + " - 1 : 0];");
	writeHelper("\treg [" + to_string(demandedPorts) + " - 1 : 0] select [" + to_string(demandedPorts) + " - 1 : 0];");
	
	// Assignments
	writeHelper("");
	writeHelper("\t// Assignments");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\tassign request[" + to_string(x) + "] = request_" + to_string(x) + ";");

	
	// Behavior
	writeHelper("");
	writeHelper("\t// Behavior");
	writeHelper("\tinitial begin:Initialization");
	writeHelper("\t\t// Empty ");
	writeHelper("\tend");
	
	writeHelper("");
	writeHelper("\t// Data Logic - Mux");
	writeHelper("\tgenvar j_var;");
	writeHelper("\tgenerate");
	writeHelper("\t\tfor(j_var = 0; j_var < " + to_string(demandedPorts) + "; j_var = j_var + 1) begin:LOGIC");
	writeHelper("\t\t\t// This piece of code looks at all request lines and if one shows \"its\" ID  it selects it, this assumes that SA workes without faults");
	writeHelper("\t\t\talways @ (");
	for(int x =0 ;x<demandedPorts;x++)
		if (x == (demandedPorts - 1))
			writeHelper("\t\t\t\trequest[" + to_string(x) + "]");
		else
			writeHelper("\t\t\t\trequest[" + to_string(x) + "] or");
	writeHelper("\t\t\t) begin:MUX_LOGIC");
	for(int x =0 ;x<=demandedPorts;x++)
		if (x == 0)
		{
			writeHelper("\t\t\t\tif(request[" + to_string(x) + "] == (" + createVector(0, demandedPorts) + " << j_var)) begin");
			writeHelper("\t\t\t\t\tselect[j_var] = " + createVector(x, demandedPorts) + ";");
		}
		else if(x == demandedPorts)
		{
			writeHelper("\t\t\t\tend else begin");
			writeHelper("\t\t\t\t\tselect[j_var] = " + createVector(-1, demandedPorts) + ";");
			writeHelper("\t\t\t\tend");
		}
		else
		{
			writeHelper("\t\t\t\tend else if(request[" + to_string(x) + "] == (" + createVector(0, demandedPorts) + " << j_var)) begin");
			writeHelper("\t\t\t\t\tselect[j_var] = " + createVector(x, demandedPorts) + ";");
		}

	writeHelper("\t\t\tend");
	writeHelper("\t\tend");
	writeHelper("\tendgenerate");
	
	// Module Initialization
	writeHelper("");
	writeHelper("\t// Module Initialization");
	writeHelper("\tm_Switch_Data");
	writeHelper("\t#(");
	writeHelper("\t\t.P_DATA_WIDTH(P_DATA_WIDTH)");
	writeHelper("\t) dataSwitch (");
	
	for(int x =0 ;x<demandedPorts;x++)
		if (x == 0)
			writeHelper("\t\t.sel_" + to_string(x) + "(select[" + to_string(x) + "])");
		else
			writeHelper("\t\t,.sel_" + to_string(x) + "(select[" + to_string(x) + "])");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\t\t,.data_in_" + to_string(x) + "(data_in_" + to_string(x) + ")");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\t\t,.data_out_" + to_string(x) + "(data_out_" + to_string(x) + ")");
	
	writeHelper("\t);");
	
	writeHelper("");
	
	writeHelper("\tm_Switch_Feedback");
	writeHelper("\t#(");
	writeHelper("\t\t.P_FEEDBACK_WIDTH(P_FEEDBACK_WIDTH)");
	writeHelper("\t) feedbackSwitch (");
	
	for(int x =0 ;x<demandedPorts;x++)
		if (x == 0)
			writeHelper("\t\t.sel_" + to_string(x) + "(request[" + to_string(x) + "])");
		else
			writeHelper("\t\t,.sel_" + to_string(x) + "(request[" + to_string(x) + "])");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\t\t,.feedback_in_" + to_string(x) + "(feedback_in_" + to_string(x) + ")");
	
	writeHelper("");
	for(int x =0 ;x<demandedPorts;x++)
		writeHelper("\t\t,.feedback_out_" + to_string(x) + "(feedback_out_" + to_string(x) + ")");
	
	writeHelper("\t);");
	

	
	// End of the module
	writeHelper("");
	writeHelper("endmodule");
	
	// Write output End
	ofile.close();

	cout<<"generate Crossbar successfully\n";

}

void NOC_GEN::generateParameters()
{
	router = noc.templateRouter();

	ofile.open(FILE_PATH + PARAMETERS_FILE_NAME);
	ofile
	<<"\t//`define FAULT_ENABLE 1\n"
	<<"\t//define FAULT_ENABLE to enable the fault clock connection\n"

	<<"\t//CLOCK \n"
	<<"\t`define CLK_PERIOD 2 \n\n"

	<<"\t`define MESH_SIZE_X "<<x_size<<" \n"
	<<"\t`define MESH_SIZE_Y "<<y_size<<" \n\n"

	<<"\t// Routers \n"
	<<"\t`define ROUTERS "<<router_num<<" \n\n"

	<<"\t// Ports \n"
	<<"\t`define PORTS "<<p_port_num<<" \n"
	//<<"\t`define PORTS_WIDTH "<<<<" \n"


	<<"\t// Virtual Channels \n"
	<<"\t `define CHANNELS "<<a_port_num<<"\n\n"
	<<"\t `define CHANNELS_WIDTH "<<log2(a_port_num)<<"\n\n"
	

	<<"\t// Buffer Size == CREDIT Size \n"
	<<"\t`define BUFFERSIZE "<<buffer_size<<" \n"
	<<"\t`define BUFFERSIZE_WIDTH "<<buffer_size_width<<" \n"



	<<"\t// State Machine States \n"
	<<"\t`define STATES "<< 10 <<" \n"
	<<"\t`define STATES_WIDTH "<< log2(10) <<"\n"
	<<"\t \n"
	<<"\t \n"

	<<"\t// Channels Numeration Position Definition \n";
	int j = 0;
	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
				<< "`define P" << i << "_VC1 " << j << "\n";
			j++;
			ofile
				<< "`define P" << i << "_VC2 " << j << "\n";
			j++;
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
				<< "`define P" << i << " " << j << "\n";
			j++;
		}
	}
	ofile
	<<"\t//===========SIGNALS============ \n"
	<<"\t`define DATA_VALID_WIDTH 1 \n"

	<<"\t//========GENERAL FLIT VALUES========= \n"
	<<"\t// General \n"
	<<"\t`define FLIT_WIDTH "<<flit_size<<" \n"
	//can't find at esy_networkcfg.h
	<<"\t`define FLIT_TYPE_WIDTH 2 \n"	
	//can't find at esy_networkcfg.h
	<<"\t`define FLIT_DATA_WIDTH 32 \n"

	<<"\t// HEADFLIT 10 \n"
	<<"\t`define FLIT_HEADFLIT 2\n"
	<<"`define FLIT_SRC_WIDTH "<<log2(x_size) + log2(y_size)<<endl
	<<"`define FLIT_DST_WIDTH "<<log2(x_size) + log2(y_size)<<endl
    <<"`define FLIT_RESERVED_WIDTH "<<16 - log2(x_size)*2 - log2(y_size)*2<<endl
	<<R"(
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
	`define UNUSED 0)";

	cout<<"generate Parameters successfully\n";


	ofile.close();
}

void NOC_GEN::generateMatrixArbiter(int demandedPorts)
{
	ofile.open(FILE_PATH + Matrix_Arbiter_FILE_NAME);

	// Size of the arbiter matrix
	int matrixSize = demandedPorts;
		
	// Includes
	writeHelper("\n");
	writeHelper("`timescale 1ns / 1ps");
	writeHelper("`include \"parameters.v\"");
	writeHelper("\n");
	
	// Heading of the network verilog file
	writeHelper("module m_MatrixArbiter (");
	writeHelper("\t\tCLK,");
	writeHelper("\t\tRST,");
	writeHelper("");
	writeHelper("\t\trequest_vector,");
	writeHelper("\t\tgrant_vector");

	
	writeHelper("\t);");
	
	
	// Parameters	
	writeHelper("\n");
	// Inputs + Outputs
	writeHelper("\t// Inputs and Outputs");
	writeHelper("\tinput CLK;");
	writeHelper("\tinput RST;");
			
	writeHelper("");
	writeHelper("\tinput [`CHANNELS - 1 : 0] request_vector;");
	writeHelper("\toutput [`CHANNELS - 1 : 0] grant_vector;");
	
	// Wires	 + Variables
	writeHelper("\n");
	writeHelper("\t// Wire + Variables");
	writeHelper("\treg intern_SET;");
	
	for(int x =0 ; x<matrixSize ; x++)
	{
		writeHelper("");
		writeHelper("\t// Request_" + to_string(x));
		writeHelper("\twire request_" + to_string(x)  + ";");
		writeHelper("\t// the wire grant_" + to_string(x)  + " equals intern_" + to_string(x) + to_string(x) + "_and_out");
		writeHelper("\twire intern_" + to_string(x)  + "_or_out;");
		
		for(int y =0 ; y<matrixSize ; y++)			
			writeHelper("\twire intern_" + to_string(x) + to_string(y) + "_and_out;");
			
		for(int y =0 ; y<matrixSize ; y++)
			if (x != y)
				writeHelper("\twire intern_" + to_string(x) + to_string(y) + "_subU_out;");
	}	
			

	// Assignments
	writeHelper("\n");
	writeHelper("\t// Assignments");
	
	// Request Vector
	for(int x =0 ; x<matrixSize ; x++)
		writeHelper("\tassign request_" + to_string(x)  + " = request_vector[" + to_string(x) + "];");
	
	// Grant Vector
	writeHelper("");
	for(int x =0 ; x<matrixSize ; x++)
		writeHelper("\tassign grant_vector[" + to_string(x) + "] = intern_" + to_string(x) + to_string(x) + "_and_out;");
	
	// And Gates
	for(int x =0 ; x<matrixSize ; x++)
	{
		writeHelper("");
		writeHelper("\t// Request_" + to_string(x));
	
		for(int y =0 ; y<matrixSize ; y++)
			if (x != y)
				writeHelper("\tand and_" + to_string(x) + to_string(y) + " (intern_" + to_string(x) + to_string(y) + "_and_out, request_" + to_string(x)  + ", intern_" + to_string(x) + to_string(y) + "_subU_out);");
			else
				writeHelper("\tand and_" + to_string(x) + to_string(y) + " (intern_" + to_string(x) + to_string(y) + "_and_out, request_" + to_string(x)  + ", ~intern_" + to_string(x)  + "_or_out);");
	}	
	
	// Or Gates
	writeHelper("");
	for(int x =0 ; x<matrixSize ; x++)
	{
		writeHelper("\tor or_" + to_string(x) + " (intern_" + to_string(x)  + "_or_out ");
		for(int y =0 ; y<matrixSize ; y++)
			if(x != y)
				writeHelper("\t\t,intern_" + to_string(y) + to_string(x) + "_and_out ");

		writeHelper("\t);");
	}
	
	// Behavior
	writeHelper("\n");
	writeHelper("\t// Behavior");
	writeHelper("\talways @ (RST) begin");
	writeHelper("\t	if (RST) begin");
	writeHelper("\t		intern_SET = 1;");
	writeHelper("\t	end else begin");
	writeHelper("\t		intern_SET = 0;");
	writeHelper("\t	end");
	writeHelper("\tend");
	
	// Module Declaration + WireConnection
	writeHelper("\n");
	writeHelper("\t// Module Decleration");
	for(int x =0 ; x<matrixSize ; x++)
	{
		for(int y =0 ; y<matrixSize ; y++)
		{
			if (x != y)
			{
				writeHelper("\t// Row: " + to_string(x) + " Column: " + to_string(y) + " SubUnit: " + to_string(x) + to_string(y));
				writeHelper("\tm_MatrixArbiter_subUnit subU_" + to_string(x) + to_string(y) + " (");
					
				writeHelper("\t\t.RST(RST)");
				writeHelper("\t\t,.CLK(CLK)");
			    if (y > x)
					writeHelper("\t\t,.SET(intern_SET)");
				else
					writeHelper("\t\t,.SET(1'b0)");				
				writeHelper("\t\t,.g_in_1(intern_" + to_string(y) + to_string(y) + "_and_out)");
				writeHelper("\t\t,.g_in_2(intern_" + to_string(x) + to_string(x) + "_and_out)");
				writeHelper("\t\t,.w_out(intern_" + to_string(x) + to_string(y) + "_subU_out)");
				
				writeHelper("\t);");
			}
			else
				writeHelper("\t// Row: " + to_string(x) + " Column: " + to_string(y) + " NO SUBUNIT AT THIS POSITION");

		}	

	}
			
			
	// Module Declaration End
	writeHelper("\n");
	writeHelper("endmodule");

	cout<<"generate Matrix Arbiter successfully\n";

	ofile.close();

}

void NOC_GEN::generateEvaluationUnit ()
{
	ofile.open(FILE_PATH + Evaluation_Unit_FILE_NAME);
	ofile
	<<R"(
	`timescale 1ns / 1ps

	`include "parameters.v"

	module m_EvaluationUnit(
		RST,
		CLK,
		
		enables,
		requests,
		grants,
		matrixHold
	);

	// Parameters
	parameter	P_ROUTER_ID = 0;
	parameter	P_CHANNEL_ID = 0;

	// Port Definition
	input RST;
	input CLK;
		
	input [`CHANNELS - 1 : 0] enables;
	input [`CHANNELS - 1 : 0] requests;
	output [`CHANNELS - 1 : 0] grants;
	output matrixHold;

	// Variables
	wire [`CHANNELS - 1 : 0] evaluationStatus; 	
	wire [`CHANNELS - 1 : 0] request_vector; 
	)";

	ofile
	<<"\t// Assignments\n";

	router = noc.templateRouter();

	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tassign evaluationStatus[`P"<<i<<"_VC1]"
			<<"= ~enables[`P"<<i<<"_VC1] & requests[`P"<<i<<"_VC1];\n"
			<<"\tassign evaluationStatus[`P"<<i<<"_VC2]"
			<<"= ~enables[`P"<<i<<"_VC2] & requests[`P"<<i<<"_VC2];\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			ofile
				<< "\tassign evaluationStatus[`P" << i << "]"
				<< "= ~enables[`P" << i << "] & requests[`P" << i << "];\n";
		}
	}

	ofile
	<<"\t\n"
	<<"\tassign matrixHold = \n";

	for(int i =0 ; i < p_port_num ; i++)
	{
		port = router.port(i);
		if(port.inputVcNumber() == 2)
		{
			ofile
			<<"\tevaluationStatus[`P"<<i<<"_VC1] ^\n";
			if(i == p_port_num - 1)
				ofile
				<<"\tevaluationStatus[`P"<<i<<"_VC2];\n";
			else
				ofile
				<<"\tevaluationStatus[`P"<<i<<"_VC2] ^\n";
		}
		else if(port.inputVcNumber() == 1)
		{
			if(i == p_port_num - 1)
				ofile
				<<"\tevaluationStatus[`P"<<i<<"];\n";
			else
				ofile
				<<"\tevaluationStatus[`P"<<i<<"] ^\n";
		}
	}

	ofile
	<<R"(
	assign request_vector = (matrixHold) ? evaluationStatus : requests;
	
	// Behavior
	initial begin:Initialization
		// Empty
	end

	// Module Decleration
	m_MatrixArbiter arbiter (
		.CLK(CLK)
		,.RST(RST)
		
		,.request_vector(request_vector)
		,.grant_vector(grants)
	);

	endmodule		
	)";

	cout<<"generate Evaluation Unit successfully\n";

	ofile.close();

}

void NOC_GEN::generateNocTestbench ()
{
	ofile.open(FILE_PATH + NOC_TB_FILE_NAME);
	ofile
	<<R"(
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
	)";


	//Noc wire
	ofile
	<<"\t// ===NoC===\n";
	for(int i = 0; i< router_num; i++)
	{
		ofile
		<<"\t// NoC Local Port: "<<i<<"\n"
		<<"\twire [P_DATA_WIDTH - 1: 0] tb_r"<<i<<"_p0_in_data;\n"
		<<"\twire [P_DATA_WIDTH - 1: 0] tb_r"<<i<<"_p0_out_data;\n\n"

		<<"\twire tb_r"<<i<<"_p0_in_data_valid;\n"
		<<"\twire tb_r"<<i<<"_p0_out_data_valid;\n\n"

		<<"\twire [`BUFFERSIZE_WIDTH - 1: 0] tb_r"<<i<<"_p0_out_credits_feedback;\n"

		<<"\t`ifdef FT_RETRANSMISSION\n"
		<<"\twire [`BUFFERSIZE_WIDTH - 1: 0] tb_r"<<i<<"_p0_in_credits_feedback;\n"
		<<"\t`endif\n\n"

		<<"\t// ECC signals for NoC Local Port: "<<i<<"\n"
		<<"\t`ifdef FT_ECC\n"
		<<"\twire [`FLIT_WIDTH - 1: 0] tb_r"<<i<<"_p0_preEncoder_data;\n"
		<<"\twire [`FLIT_WIDTH - 1: 0] tb_r"<<i<<"_p0_postDecoder_data;\n"
		<<"\twire tb_r"<<i<<"_po_ecc_error;\n"
		<<"\t`endif\n\n\n";
	}
	//end of wire define

	ofile
		<< R"(
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
	)";

	//assignments
	ofile
		<< "\t// Assignments\n"
		<< "\t`ifdef FT_ECC\n";
	for(int i = 0; i< router_num; i++)
	{
		ofile
		<<"\tassign tb_PacketEject_data["<<i<<"] = tb_r"<<i<<"_p0_postDecoder_data;\n";
	}
	ofile
	<<"\t`else\n";
	for(int i = 0; i< router_num; i++)
	{
		ofile
		<<"\tassign tb_PacketEject_data["<<i<<"] = tb_r"<<i<<"_p0_out_data;\n";
	}
	ofile
	<<"\t`endif\n\n"
	<<"\tassign tb_active_packets = tb_packets_injected - tb_packets_ejected - tb_packets_ejected_wrong;\n"
	<<"\tassign tb_active_flits = tb_flits_injected - tb_flits_ejected;\n"
	<<"\tassign tb_active_bodyflits = tb_active_flits - (tb_active_packets * 2);  // HF + TF\n\n\n";
	ofile
	<<R"(
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
	)";
	ofile
	<<"\t// Testbench initialization\n"
	<<"\ttb_CLK_enable = 0;\n\n"

	<<"\t// Changing the ingoing credit_feedback to 4 -> the extern connection can reveice \"all\" flits\n"
	<<"\t`ifndef FT_RETRANSMISSION\n"
	<<"\tfor(i=0; i < `ROUTERS; i = i + 1) begin\n"
	<<"\ttb_r_p0_in_credits_feedback[i] = 4;\n"
	<<"\tend\n"
	<<"\t`endif\n\n";

	ofile
	<<R"(
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

	)";

	for(int i = 0 ; i<router_num ; i++)
	{
		ofile
		<<"\t// Additional Interface Logic for Router: "<<i<<"\n"
		<<"\t// FlitIDProvider-Routine for PacketInjector: "<<i<<"\n"
		<<"\talways @ (tb_PacketInj_get_id_enable["<<i<<"]) begin\n"
		<<"\t	if (tb_PacketInj_get_id_enable["<<i<<"]) begin\n"
		<<"\t		generateID.get(1);\n"
		<<"\t		alarm_counter = 0;\n"
		<<"\t		// Control if packet id is already in the network, if so the suggested id gets incremented\n"
		<<"\t		while(tb_ID_status[tb_packetID] == 0) begin\n"
		<<"\t			tb_packetID = tb_packetID + 1;\n"
		<<"\t			alarm_counter = alarm_counter + 1;\n"
		<<"\t			if (alarm_counter > (P_STATUSARRAY_LENGTH - 1)) begin\n"
		<<"\t				$display(\"TestbenchLog: %0d - Warning: No free packet id found for packet injection in router %0d!\", $time, "<<i<<");\n"
		<<"\t			end\n"
		<<"\t		end\n"
		<<"\t		// The chosen id gets deactivated in the id array before it gets provided to the Random Injector\n"
		<<"\t		tb_ID_status[tb_packetID] = 0;\n"
		<<"\t		tb_occupied_ids = tb_occupied_ids + 1;\n"
		<<"\t		tb_get_ids = tb_get_ids + 1;\n"
		<<"\t		tb_PacketInj_get_id_data["<<i<<"] = tb_packetID;\n"
		<<"\t		generateID.put(1);\n"
		<<"\t		tb_PacketInj_get_id_grant["<<i<<"] = 1;\n"
		<<"\t	end else begin\n"
		<<"\t		tb_PacketInj_get_id_grant["<<i<<"] = 0;\n"
		<<"\t	end\n"
		<<"\tend\n"

		<<"\t`ifdef FT_RETRANSMISSION\n"
		<<"\t// FlitIDRecycler-Routine for PacketInjector: "<<i<<"\n"
		<<"\talways @ (tb_PacketInj_set_id_enable["<<i<<"]) begin\n"
		<<"\t	if (tb_PacketInj_set_id_enable["<<i<<"]) begin\n"
		<<"\t		// The set id gets activated again because it is now free for reuse\n"
		<<"\t		generateID.get(1);\n"
		<<"\t		tb_ID_status[tb_PacketInj_set_id_data["<<i<<"]] = 1;\n"
		<<"\t		tb_occupied_ids = tb_occupied_ids - 1;\n"
		<<"\t		tb_set_ids = tb_set_ids + 1;\n"
		<<"\t		generateID.put(1);\n"
		<<"\t		tb_PacketInj_set_id_grant["<<i<<"] = 1;\n"
		<<"\t	end else begin\n"
		<<"\t		tb_PacketInj_set_id_grant["<<i<<"] = 0;\n"
		<<"\t	end\n"
		<<"\tend\n"
		<<"\t`endif\n\n";

		ofile
		<<"\t// PacketInject Counter for NoC Interface "<<i<<"\n"
		<<"	always @ (posedge tb_CLK) begin\n"
		<<"		if (tb_r"<<i<<"_p0_in_data_valid) begin\n"
		<<"			logInjectedData.get(1);\n"
		<<"			// Packet/Flit injection should be without any fault due to being in the testbench\n"
		<<"\n"
		<<"			// Check for Headflit\n"
		<<"			if (tb_r"<<i<<"_p0_in_data[33:32] == `FLIT_HEADFLIT) begin\n"
		<<"				tb_flits_injected = tb_flits_injected + 1;\n"
		<<"				`ifdef FT_RETRANSMISSION\n"
		<<"				if (tb_r"<<i<<"_p0_in_data[0] == 1'b1) begin         // Flag for ACK packet\n"
		<<"					tb_inj_ACK["<<i<<"] = 1;\n"
		<<"				end else begin\n"
		<<"					tb_inj_ACK["<<i<<"] = 0;\n"
		<<"				end\n"
		<<"				`endif\n"
		<<"\n"
		<<"			// Check for Bodyflit\n"
		<<"			end else if (tb_r"<<i<<"_p0_in_data[33:32] == `FLIT_BODYFLIT) begin\n"
		<<"				tb_flits_injected = tb_flits_injected + 1;\n"
		<<"\n"
		<<"			// Check for Tailflit + Additional Logic\n"
		<<"			end else if (tb_r"<<i<<"_p0_in_data[33:32] == `FLIT_TAILFLIT) begin\n"
		<<"				tb_flits_injected = tb_flits_injected + 1;\n"
		<<"				tb_packets_injected = tb_packets_injected + 1;\n"
		<<"\n"
		<<"				`ifdef FT_RETRANSMISSION\n"
		<<"				if (tb_inj_ACK["<<i<<"]) begin\n"
		<<"					tb_ACKs_injected = tb_ACKs_injected + 1;\n"
		<<"				end\n"
		<<"				`endif\n"
		<<"\n"
		<<"			end else begin\n"
		<<"				$display(\"TestbenchLog: %0d - Garbage got injected at NoC Interface (Router %0d)!\", $time, "<<i<<");\n"
		<<"			end\n"
		<<"\n"
		<<"			logInjectedData.put(1);\n"
		<<"		end\n"
		<<"	end\n"
		<<"\n"
		<<"	// PacketEject Counter for NoC Interface "<<i<<"\n"
		<<"	always @ (posedge tb_CLK) begin\n"
		<<"		if (tb_r"<<i<<"_p0_out_data_valid) begin\n"
		<<"			logEjectedData.get(1);\n"
		<<"			// How to handle faults?\n"
		<<"\n"
		<<"			// Check for Headflit\n"
		<<"			if (tb_PacketEject_data["<<i<<"][33:32] == `FLIT_HEADFLIT) begin\n"
		<<"				tb_ejected_dst["<<i<<"] = tb_PacketEject_data["<<i<<"][25:20];\n"
		<<"				tb_flits_ejected = tb_flits_ejected + 1;\n"
		<<"				`ifdef FT_RETRANSMISSION\n"
		<<"				if (tb_PacketEject_data["<<i<<"][0] == 1'b1) begin         // Flag for ACK packet\n"
		<<"					tb_eje_ACK["<<i<<"] = 1;\n"
		<<"				end else begin\n"
		<<"					tb_eje_ACK["<<i<<"] = 0;\n"
		<<"				end\n"
		<<"				`endif\n"
		<<"\n"
		<<"			// Check for Bodyflit\n"
		<<"			end else if (tb_PacketEject_data["<<i<<"][33:32] == `FLIT_BODYFLIT) begin\n"
		<<"				tb_flits_ejected = tb_flits_ejected + 1;\n"
		<<"\n"
		<<"			// Check for Tailflit + Additional Logic\n"
		<<"			end else if (tb_PacketEject_data["<<i<<"][33:32] == `FLIT_TAILFLIT) begin\n"
		<<"				tb_flits_ejected = tb_flits_ejected + 1;\n"
		<<"				tb_packets_ejected = tb_packets_ejected + 1;\n"
		<<"\n"
		<<"				`ifdef FT_RETRANSMISSION\n"
		<<"				if (tb_eje_ACK["<<i<<"]) begin\n"
		<<"					tb_ACKs_ejected = tb_ACKs_ejected + 1;\n"
		<<"				end\n"
		<<"				`endif\n"
		<<"\n"
		<<"				// Check if the flit arrived at the correct position\n"
		<<"				if (tb_ejected_dst["<<i<<"] != 6'd"<<i<<") begin\n"
		<<"					tb_packets_ejected_wrong = tb_packets_ejected_wrong + 1;\n"
		<<"				end\n"
		<<"\n"
		<<"			end else begin\n"
		<<"				$display(\"TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!\", $time, "<<i<<");\n"
		<<"			end\n"
		<<"\n"
		<<"			logEjectedData.put(1);\n"
		<<"		end\n"
		<<"	end\n"
		<<"\n"
		<<"	`ifndef FT_RETRANSMISSION\n"
		<<"	// // PacketEjectHandler for NoC Interface "<<i<<"\n"
		<<"	always @ (posedge tb_CLK) begin\n"
		<<"		if (tb_r"<<i<<"_p0_out_data_valid) begin\n"
		<<"			logEjectedData.get(1);\n"
		<<"\n"
		<<"			// Check for Headflit\n"
		<<"			if (tb_PacketEject_data["<<i<<"][33:32] == `FLIT_HEADFLIT) begin\n"
		<<"				// Expecting that the head flit data is correct\n"
		<<"				tb_ejected_id["<<i<<"] = tb_PacketEject_data["<<i<<"][15:4];\n"
		<<"\n"
		<<"			// Check for Bodyflit\n"
		<<"			end else if (tb_PacketEject_data["<<i<<"][33:32] == `FLIT_BODYFLIT) begin\n"
		<<"\n"
		<<"			// Check for Tailflit + Additional Logic\n"
		<<"			end else if (tb_PacketEject_data["<<i<<"][33:32] == `FLIT_TAILFLIT) begin\n"
		<<"				// The ejected id gets activated again because it is now free for reuse\n"
		<<"				generateID.get(1);\n"
		<<"				tb_ID_status[tb_ejected_id["<<i<<"]] = 1;\n"
		<<"				tb_set_ids = tb_set_ids + 1;\n"
		<<"				tb_occupied_ids = tb_occupied_ids - 1;\n"
		<<"				generateID.put(1);\n"
		<<"			end else begin\n"
		<<"				$display(\"TestbenchLog: %0d - Garbage got ejected at NoC Interface (Router %0d)!\", $time, "<<i<<");\n"
		<<"			end\n"
		<<"			logEjectedData.put(1);\n"
		<<"		end\n"
		<<"	end\n"
		<<"	`endif\n";
	}

	ofile
	<<"\t// MODULE DECLERATION\n"
	<<"\t// Network On Chip \n"
	<<"\tm_Assembled_NoC noc1 (\n";
	for(int i = 0 ; i< router_num;i++)
	{
		ofile
		<<"\t//Router "<<i<<"),\n"
		<<"\t.r"<<i<<"_p0_in_data(tb_r"<<i<<"_p0_in_data),\n"
		<<"\t.r"<<i<<"_p0_out_data(tb_r"<<i<<"_p0_out_data),\n"

		<<"\t.r"<<i<<"_p0_in_data_valid(tb_r"<<i<<"_p0_in_data_valid),\n"
		<<"\t.r"<<i<<"_p0_out_data_valid(tb_r"<<i<<"_p0_out_data_valid),\n"

		<<"\t.r"<<i<<"_p0_out_credits_feedback(tb_r"<<i<<"_p0_out_credits_feedback),\n"
		<<"\t`ifdef FT_RETRANSMISSION\n"
		<<"\t.r"<<i<<"_p0_in_credits_feedback(tb_r"<<i<<"_p0_in_credits_feedback),\n"
		<<"\t`else\n"
		<<"\t.r"<<i<<"_p0_in_credits_feedback(tb_r_p0_in_credits_feedback["<<i<<"]),\n"
		<<"\t`endif\n"
		<<"\t\n"
		<<"\t\n";
	}
	ofile
	<<"\t.RST(tb_RST),\n"
	<<"\t.CLK(tb_CLK)\n"
	<<"\t);\n\n\n";

	for(int i = 0; i<router_num ; i++)
	{
	ofile
    <<"	// RandomInjector: "<<i<<"\n"
    <<"	`ifndef FT_RETRANSMISSION\n"
    <<"	m_RandomInjector \n"
    <<"	#(\n"
    <<"		.P_LOCAL_ID("<<i<<")\n"
    <<"		,.P_LOCAL_PORT(0)\n"
    <<"	) randInj_"<<i<<" (\n"
    <<"		.RST(tb_RST)\n"
    <<"		,.CLK(tb_CLK)\n"
    <<"\n"
    <<"		,.enable(tb_PacketInj_enable["<<i<<"])\n"
    <<"\n"
    <<"		,.id_request(tb_PacketInj_get_id_enable["<<i<<"])\n"
    <<"		,.id_grant(tb_PacketInj_get_id_grant["<<i<<"])\n"
    <<"		,.id_result(tb_PacketInj_get_id_data["<<i<<"])\n"
    <<"\n"
    <<"		,.credit_feedback(tb_r"<<i<<"_p0_out_credits_feedback)\n"
    <<"		,.data_valid(tb_r"<<i<<"_p0_in_data_valid)\n"
    <<"		`ifdef FT_ECC\n"
    <<"			,.data_out(tb_r"<<i<<"_p0_preEncoder_data)\n"
    <<"		`else\n"
    <<"			,.data_out(tb_r"<<i<<"_p0_in_data)\n"
    <<"		`endif\n"
    <<"\n"
    <<"		,.finished_round()\n"
    <<"	);\n"
    <<"	`endif\n"
    <<"\n"
    <<"	// RetransmissionUnit: "<<i<<"\n"
    <<"	`ifdef FT_RETRANSMISSION\n"
    <<"	m_RetransmissionUnit \n"
    <<"	#(\n"
    <<"		.P_LOCAL_ID("<<i<<")\n"
    <<"		,.P_LOCAL_PORT(0)\n"
    <<"	) retransM_"<<i<<" (\n"
    <<"		.RST(tb_RST)\n"
    <<"		,.CLK(tb_CLK)\n"
    <<"		,.enable(tb_PacketInj_enable["<<i<<"])\n"
    <<"\n"
    <<"		`ifdef FT_ECC\n"
    <<"			,.error(tb_r"<<i<<"_p0_ecc_error)\n"
    <<"		`else\n"
    <<"			,.error(1'b0)\n"
    <<"		`endif\n"
    <<"\n"
    <<"		,.get_id_enable(tb_PacketInj_get_id_enable["<<i<<"])\n"
    <<"		,.get_id_grant(tb_PacketInj_get_id_grant["<<i<<"])\n"
    <<"		,.get_id_data(tb_PacketInj_get_id_data["<<i<<"])\n"
    <<"\n"
    <<"		,.set_id_enable(tb_PacketInj_set_id_enable["<<i<<"])\n"
    <<"		,.set_id_grant(tb_PacketInj_set_id_grant["<<i<<"])\n"
    <<"		,.set_id_data(tb_PacketInj_set_id_data["<<i<<"])\n"
    <<"\n"
    <<"		,.sending_data_valid(tb_r"<<i<<"_p0_in_data_valid)\n"
    <<"		`ifdef FT_ECC\n"
    <<"			,.sending_data(tb_r"<<i<<"_p0_preEncoder_data)\n"
    <<"		`else\n"
    <<"			,.sending_data(tb_r"<<i<<"_p0_in_data)\n"
    <<"		`endif\n"
    <<"\n"
    <<"		,.receiving_data_valid(tb_r"<<i<<"_p0_out_data_valid)\n"
    <<"		`ifdef FT_ECC\n"
    <<"			,.receiving_data(tb_PacketEject_data["<<i<<"])\n"
    <<"		`else\n"
    <<"			,.receiving_data(tb_r"<<i<<"_p0_out_data)\n"
    <<"		`endif\n"
    <<"\n"
    <<"		,.sending_credit_feedback(tb_r"<<i<<"_p0_in_credits_feedback)\n"
    <<"		,.receiving_credit_feedback(tb_r"<<i<<"_p0_out_credits_feedback)\n"
    <<"	);\n"
    <<"	`endif\n"
    <<"\n"
    <<"	`ifdef FT_ECC\n"
    <<"	// ECC NoC Local Port: "<<i<<"\n"
    <<"	m_ECC_Unit_Encoder ecc_encoderUnit_"<<i<<"(\n"
    <<"		.in(tb_r"<<i<<"_p0_preEncoder_data)\n"
    <<"		,.out_encoded(tb_r"<<i<<"_p0_in_data)\n"
    <<"	);\n"
    <<"	`endif\n"
    <<"\n"
    <<"	`ifdef FT_ECC\n"
    <<"	m_ECC_Unit_Decoder ecc_decoderUnit_"<<i<<"(\n"
    <<"		.in_encoded(tb_r"<<i<<"_p0_out_data)\n"
    <<"		,.out_decoded(tb_r"<<i<<"_p0_postDecoder_data)\n"
    <<"		,.out_error(tb_r"<<i<<"_p0_ecc_error)\n"
    <<"	);\n"
    <<"	`endif\n\n\n";
	}

	ofile
	<<"\t// Clock Generator\n"
	<<"\tm_ClockGenerator clk1(\n"
	<<"\t.enable(tb_CLK_enable)\n"
	<<"\t,.clkOutput(tb_CLK)\n"
	<<"\t);\n"
	<<"\tendmodule\n";

	ofile.close();

	cout<<"generate testbench successfully\n";
}

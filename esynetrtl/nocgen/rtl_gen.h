#ifndef RTL_GEN_H
#define RTL_GEN_H

#endif // RTL_GEN_H

#include "../interface/esy_networkcfg.h"
#include <iostream>
#include <fstream>

using namespace std;

class NOC_GEN
{
private:
    EsyNetworkCfgPort port;
    EsyNetworkCfgRouter router;
    EsyNetworkCfg noc;
    ofstream ofile;
    int x_size;
    int y_size;
    int router_num;

    int p_port_num; //physical port number
    int a_port_num; //all port number, including Virtual port

    int buffer_size;
    int buffer_size_width;

    long flit_size = 34;

    string  NOC_FILE_NAME = "m_Assembled_NoC.v";
    string  NOC_FILE_NAME_FAULTY = "m_Assembled_NoC_faulty.v";

	string  ROUTER_FILE_NAME = "m_Assembled_Router.v";

	string  CHANNEL_FILE_NAME = "m_Assembled_Channel.v";
    string  MUX_FILE_NAME = "m_Mux_NoEncoding.v";

	string  Switch_Allocation_FILE_NAME = "m_SwitchAllocation.v";
	string  Crossbar_FILE_NAME = "m_Crossbar.v";
	string  Routing_Calculation_FILE_NAME = "rc.v";
	string  Matrix_Arbiter_FILE_NAME = "m_MatrixArbiter.v";
	string  Evaluation_Unit_FILE_NAME = "m_EvaluationUnit.v";

    string  Feedback_Switch_FILE_NAME = "m_Switch_Feedback_7in7out.v";
    string  Data_Switch_FILE_NAME = "m_Switch_Data.v";

    string  NOC_TB_FILE_NAME = "m_Assembled_NoC_w_RandomFaultInj_tb.sv";

    string  PARAMETERS_FILE_NAME = "parameters.v";
    string  FILE_PATH = "./";

    string  postfix;
	void writeHelper(string writeString); 

    string createVector(int bitIndex, int portNumber);

public:
    string  DC_PATH = "~/workspace/ftnoc";

    string  TCL_PATH = "~/workspace/ftnoc/scripts/";

    string  SL = "{/home/junkaizhan/software/synopsys/DC_2015_06/libraries/syn/dw_foundation.sldb}";

    string  TL = "[list /home/junkaizhan/technology/tcbn65gplus_digital/Front_End/timing_power_noise/NLDM/tcbn65gplus_200a/tcbn65gplustc.db]";

    string  LL = "{* /home/junkaizhan/technology/tcbn65gplus_digital/Front_End/timing_power_noise/NLDM/tcbn65gplus_200a/tcbn65gplustc.db /home/junkaizhan/software/synopsys/DC_2015_06/libraries/syn/dw_foundation.sldb}";

    string  SYML = "[list /home/junkaizhan/technology/tcbn65gplus_digital/Front_End/timing_power_noise/NLDM/tcbn65gplus_200a/tcbn65gplustc.db]";

    string  SEARCH_PATH = "[list /home/junkaizhan/software/synopsys/DC_2015_06/libraries/syn  /home/junkaizhan/software/synopsys/DC_2015_06/dw/  /home/junkaizhan/technology/tcbn65gplus_digital/Front_End/timing_power_noise/NLDM/tcbn65gplus_200a ]";

    string  CLK_PERIOD = "2";

    string  MAX_FANOUT = "10";

    //enter file name without postfix 
    EsyXmlError readXml(const string & fname);

	void generateMux(int demandedPorts);

    void generatingFeedbackSwitch (int demandedPorts);

    void generateDataSwitch (int demandedPorts);

    void generateCrossbar (int demandedPorts);

    void generateMatrixArbiter (int demandedPorts);

    void generateEvaluationUnit ();

	void generateNoc(bool fault_en);

	void generateRouter();

	void generateChannel();

	void generateRoutingCalculation();

	void generateSwitchAllocation();

	void generateParameters();

    void rtlGen();

    void channelInst(int port_num, int vc_num, int channel_id);

    void generateNocTestbench();

    void setPath(string OUTPUT_PATH);

    //copyFile functions
    void copyFile();
    void m_Buffer();
    void m_D_FF_mod();
    void m_D_FF_sync();
    void m_Demux_1in2out();
    void m_Mux_2in1out();
    void m_StateMachine();
    void m_LogModule();
    void m_RoutingCalculation();
    void m_FaultInjectionUnit();
    void m_RR_MatrixArbiter();
    void m_RR_EvaluationUnit();
    void m_MatrixArbiter_subUnit();
    void m_RandomInjector();
    void m_PacketInjector();
    void m_ClockGenerator(); 
    void dc_router();
};

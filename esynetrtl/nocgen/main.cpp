
#include <iostream>
//#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include "esy_networkcfg.h"
#include "esy_argument.h"
#include "rtl_gen.h"
#include "stdio.h"

using namespace std;


int main(int argc, char *argv[])
{
    NOC_GEN noc;

    ofstream file;

    string XML_FILE="./rtl";
    string VERILOG_PATH="./generated_code";


    string VERILOG_PATH_REF = "-verilog_path";
    string XML_FILE_REF = "-xml";
    string DC_REF = "-dc";
    string HELP_REF = "-help";

    string SL_REF = "-synthetic_library";
    string TL_REF = "-target_library";
    string LL_REF = "-link_library";
    string SYML_REF = "-symbol_library";
    string SEARCH_PATH_REF = "-search_path";

    string DC_PATH_REF = "-dc_path";
    string CLK_PERIOD_REF = "-clk_period";
    string MAX_FANOUT_REF = "-max_fanout";

    bool dc_enable = false;;

    EsyArgumentList arg_list;
    arg_list.insertOpenFile( "-network_cfg_file", 
        "path of network configuration file", &XML_FILE, "", 0, 
        NETCFG_EXTENSION);
    arg_list.insertString( "-verilog_path", 
        "path of output noc design", &VERILOG_PATH);

    arg_list.insertBool( "-dc_enable", "enable Design Compiler", 
        &dc_enable);
    arg_list.insertString( "-dc_path", 
        "path of Design Compiler", &(noc.DC_PATH), "-dc_enable", 1);
    arg_list.insertString( "-synthetic_library", 
        "path of synthetic library", &(noc.SL), "-dc_enable", 1);
    arg_list.insertString( "-target_library", 
        "path of target library", &(noc.TL), "-dc_enable", 1);
    arg_list.insertString( "-link_library", 
        "path of link library", &(noc.LL), "-dc_enable", 1);
    arg_list.insertString( "-symbol_library", 
        "path of symbol library", &(noc.SYML), "-dc_enable", 1);
    arg_list.insertString( "-search_path", 
        "path for search library", &(noc.SEARCH_PATH), "-dc_enable", 1);
    arg_list.insertString( "-clk_period", 
        "clock period for synthesis", &(noc.CLK_PERIOD), "-dc_enable", 1);
    arg_list.insertString( "-max_fanout", 
        "maximum fanout", &(noc.MAX_FANOUT), "-dc_enable", 1);

    if ( arg_list.analyse( argc, argv ) != EsyArgumentList::RESULT_OK )
    {
        return 0;
    }
    
    noc.TCL_PATH = noc.DC_PATH;
    noc.TCL_PATH += "/scripts/";

//--------------------------------------------------------------------
//                          check path
//--------------------------------------------------------------------

    file.open("./check_path.sh");
    file
    <<"if [ ! -d \""<<VERILOG_PATH<<"\" ]; then\n"
    <<"  echo \"path \""<<VERILOG_PATH<<"\" don't exist, creating it...\"\n"
    <<"  mkdir -p "<<VERILOG_PATH<<"\n"
    <<"fi\n\n";
    file.close();

    system("sh check_path.sh");

    cout << "check output file path..." << endl;
    cout <<  "PP10" << endl << endl;

//--------------------------------------------------------------------
//                          generate code
//--------------------------------------------------------------------

    bool xml_error;

    cout<<"xml file: "<<XML_FILE<<endl;
    xml_error = noc.readXml(XML_FILE).hasError();

    if(xml_error)
        return 0;

    cout << "PP20" << endl << endl;

    noc.setPath(VERILOG_PATH);
    noc.copyFile();
    noc.rtlGen();

    cout << "PP50" << endl << endl;

//--------------------------------------------------------------------
//                        design compiler
//--------------------------------------------------------------------

    if(dc_enable)
    {
        cout<<"begin to execute Design Compiler..."<<endl;
        noc.dc_router();
        system("sh dc_run.sh");
    }
    else
        cout<<"Design Compiler is not enabled"<<endl;

    cout << "PP100" << endl << endl;

    return 0;
}

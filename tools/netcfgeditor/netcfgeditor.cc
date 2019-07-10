
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "esy_netcfg.h"

char makeSelection( const std::string & options )
{
    while ( true )
    {
        std::cout << "    Make selection (";
        for ( int i = 0; i < options.size(); i ++ )
        {
            if ( i == 0 )
            {
                std::cout << options[ i ];
            }
            else
            {
                std::cout << "/" << options[ i ];
            }
        }
        std::cout << ") : ";

        char space[ 1024 ];
        std::cin.getline( space, 1024 );
        std::string a( space );

        if ( a.size() != 1 )
        {
            std::cout << "Error: Only one character is accepted. Try again." << std::endl;
            continue;
        }

        char res = a[ 0 ];
        for ( int i = 0; i < options.size(); i ++ )
        {
            if ( res == options[ i ] )
            {
                return res;
            }
        }

        std::cout << "Error: Wrong option. Try again." << std::endl;
    }
}

long getIntegar( const std::string & options, long min, long max )
{
    while ( true )
    {
        std::cout << "    " << options << " (" << min << " - " << max << ") : ";

        char space[ 1024 ];
        std::cin.getline( space, 1024 );
        std::string a( space );

        bool validcheck = true;
        for ( int i = 0; i < a.size(); i ++ )
        {
            if ( a[ i ] < '0' || a[ i ] > '9' )
            {
                std::cout << "Error: Only one integar value is accepted. Try again." << std::endl;
                validcheck = false;
                break;
            }
        }
        if ( !validcheck )
        {
            continue;
        }

        long value;
        std::stringstream ss( a );
        ss >> value;
        
        if ( value >= min && value <= max )
        {
            return value;
        }
        
        std::cout << "Error: Wrong value. Try again." << std::endl;
    }
}

double getDouble( const std::string & options, double min, double max )
{
    while ( true )
    {
        std::cout << "    " << options << " (" << min << " - " << max << ") : ";

        char space[ 1024 ];
        std::cin.getline( space, 1024 );
        std::string a( space );

        bool validcheck = true;
        long dotcount = 0;
        for ( int i = 0; i < a.size(); i ++ )
        {
            if ( ( a[ i ] < '0' || a[ i ] > '9' ) && a[ i ] != '.' )
            {
                std::cout << "Error: Only one double value is accepted. Try again." << std::endl;
                validcheck = false;
                break;
            }
            if ( a [ i ] == '.' )
            {
                dotcount ++;
                if ( dotcount > 1 )
                {
                    std::cout << "Error: Only one double value is accepted. Try again." << std::endl;
                    validcheck = false;
                    break;
                }
            }
        }
        if ( !validcheck )
        {
            continue;
        }

        double value;
        std::stringstream ss( a );
        ss >> value;
        
        if ( value >= min && value <= max )
        {
            return value;
        }
        
        std::cout << "Error: Wrong value. Try again." << std::endl;
    }
}

int main( int argc, char * argv[])
{
    system( "dialog --title \"Welcome\" "
        " --msgbox \"Network configuration editor\" 20 50" );
    
    // Get network configuration file name
    if ( argc != 2 )
    {
        std::cout << "Usage: ./netcfgeditor <file_name>" << std::endl;
    }
    std::string net_cfg_file_name( argv[ 1 ] );

    long sel = system( "dialog --title \"Select first i\" "
        " --menu \"Make selection\" 20 50 5 "
        " 1 \"Create new network configuration\" "
        " 2 \"Modify network configuration\" "
        " 3 \"exit\" " );
    std::cout << sel << std::endl;
    // First Page
    std::cout << "Network Configuration Editor" << std::endl;
    std::cout << std::endl;
    std::cout << "    A. Create new network configuration" << std::endl;
    std::cout << "    B. Modify network configuration" << std::endl;
    std::cout << "       (Heterogeneous configuration only)" << std::endl;
    std::cout << std::endl;
    char res1 = makeSelection( "AaBb" );
    std::cout << "=========================================" << std::endl;

    EsyNetworkCfg* net_cfg = NULL;

    if ( res1 == 'A' || res1 == 'a' )
    {
        EsyNetworkCfg::NoCTopology topology = EsyNetworkCfg::NT_SWITCH;
        std::vector< long > net_size;
        long router_num = 1;

        double pipe_cycle = 1.0;
        long phy_port = 0;
        long input_vc_num = 1;
        long output_vc_num = 1;
        long input_buffer = 12;
        long output_buffer = 12;
        
        double ni_pipe_cycle = 1.0;
        long ni_buffer_size = 12;
        long ni_interrupt_delay = 0;

        // Step 1 Configuration topology
        std::cout << "Create New Network Configuration" << std::endl;
        std::cout << "Step 1: Configuration Topology" << std::endl;

        // Select topology
        std::cout << std::endl;
        std::cout << "Select topology: " << std::endl;
        std::cout << std::endl;
        std::string argu = "";
        for ( int i = 0; i < EsyNetworkCfg::NT_COUNT; i ++ )
        {
            std::cout << "    " << i << ". " << EsyNetworkCfg::nocTopologyStrVector( i ) << std::endl;
            argu += ( '0' + i );
        }
        std::cout << std::endl;
        char topo_code = makeSelection( argu );
        topology = ( EsyNetworkCfg::NoCTopology )( topo_code - '0' );

        // Specifiy size
        if ( topology == EsyNetworkCfg::NT_SWITCH )
        {
            net_size.push_back( 1 );
            router_num = 1;
        }
        else
        {
            std::cout << std::endl;
            std::cout << "Specify network size" << std::endl;
            std::cout << std::endl;

            long dim_size;
            if ( topology == EsyNetworkCfg::NT_RING || topology == EsyNetworkCfg::NT_IRREGULAR )
            {
                dim_size = getIntegar( "Specifiy number of routers", 1, 1024 );
                net_size.push_back( dim_size );
                router_num = dim_size;
            }
            else if ( topology == EsyNetworkCfg::NT_MESH_2D || topology == EsyNetworkCfg::NT_TORUS_2D )
            {
                dim_size = getIntegar( "Specifiy size of AX_X (lower)", 1, 128 );
                net_size.push_back( dim_size );
                router_num *= dim_size;

                dim_size = getIntegar( "Specifiy size of AX_Y (higher)", 1, 128 );
                net_size.push_back( dim_size );
                router_num *= dim_size;
            }
            else if ( topology == EsyNetworkCfg::NT_MESH_DIA || topology == EsyNetworkCfg::NT_TORUS_DIA )
            {
                long dim = getIntegar( "Specifiy Diamension", 1, 128 );

                for ( int i = 0; i < dim; i ++ )
                {
                    std::stringstream ss;
                    ss << "Specifiy size of diamension [" << i << "] ";
                    if ( dim > 1 )
                    {
                        if ( i == 0 )
                        {
                            ss << " (lower) ";
                        }
                        else if ( i == dim - 1)
                        {
                            ss << " (higher)";
                        }
                        else
                        {
                            ss << "         ";
                        }
                    }
                    dim_size = getIntegar( ss.str(), 1, 128 );
                    net_size.push_back( dim_size );
                    router_num *= dim_size;
                }
            }
        }

        // Confirm configuration 
        std::cout << std::endl;
        std::cout << "Confirm Topology: " << std::endl;
        std::cout << std::endl;
        std::cout << "    Topology : " << EsyNetworkCfg::nocTopologyStrVector( topology ) << std::endl;
        if ( net_size.size() > 1 )
        {
            std::cout << "    Size (Lower->Higher) : " ;
            for ( int i = 0; i < net_size.size(); i ++ )
            {
                std::cout << net_size[ i ] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "    Router count : " << router_num << std::endl;
        std::cout << "=========================================" << std::endl;

        // Step 2 Configuration template router
        std::cout << "Create New Network Configuration" << std::endl;
        std::cout << "Step 2: Configuration Template Router" << std::endl;
        std::cout << "" << std::endl;

        pipe_cycle = getDouble( "Specifiy pipeline cycle", 0.0, 10.0 );
        phy_port = getIntegar( "Specifiy number of physical ports", 1, 100 );
        input_vc_num = getIntegar( "Specifiy number of virtual channels for input ports", 1, 100 );
        output_vc_num = getIntegar( "Specifiy number of virtual channels for output ports", 1, 100 );
        input_buffer = getIntegar( "Specifiy size of the input buffer", 1, 100 );
        output_buffer = getIntegar( "Specifiy size of the output buffer", 1, 100 );

        // Confirm configuration 
        std::cout << std::endl;
        std::cout << "Confirm Template Router: " << std::endl;
        std::cout << std::endl;
        std::cout << "    Pipeline cycle : " << pipe_cycle << std::endl;
        std::cout << "    Physical port : " << phy_port << std::endl;
        std::cout << "    Input  VC number : " << input_vc_num  << std::endl;
        std::cout << "    Output VC number : " << output_vc_num << std::endl;
        std::cout << "    Input  buffer : " << input_buffer  << std::endl;
        std::cout << "    Output buffer : " << output_buffer << std::endl;
        std::cout << "=========================================" << std::endl;

        std::cout << "Create New Network Configuration" << std::endl;
        std::cout << "Step 3: Configuration Template Network Interface" << std::endl;
        std::cout << "" << std::endl;

        ni_pipe_cycle = getDouble( "Specifiy pipeline cycle", 0.0, 10.0 );
        ni_buffer_size = getIntegar( "Specifiy size of the ejection buffer", 1, 100 );
        ni_interrupt_delay = getDouble( "Specifiy interrupt delay", 1.0, 1000.0 );

        // Confirm configuration 
        std::cout << std::endl;
        std::cout << "Confirm Template Network Interface: " << std::endl;
        std::cout << std::endl;
        std::cout << "    Pipeline cycle : " << ni_pipe_cycle << std::endl;
        std::cout << "    Ejection buffer : " << ni_buffer_size  << std::endl;
        std::cout << "    Interrupt cycle : " << ni_interrupt_delay << std::endl;
        std::cout << "=========================================" << std::endl;

        std::cout << "Create New Network Configuration" << std::endl;
        std::cout << "Step 4: Create network configuration file" << std::endl;
        std::cout << "" << std::endl;

        net_cfg = new EsyNetworkCfg( topology, net_size, 
                                     pipe_cycle, phy_port, input_vc_num, output_vc_num, input_buffer, output_buffer,
                                     ni_pipe_cycle, ni_buffer_size, ni_interrupt_delay
                                   );

        std::cout << *net_cfg;
        std::cout << "=========================================" << std::endl;
    }
    else if ( res1 == 'B' || res1 == 'b' )
    {
        std::cout << "Modify Network Configuration" << std::endl;
        std::cout << "Open Network Configuration File" << std::endl;
        std::cout << std::endl;

        net_cfg = new EsyNetworkCfg;
        net_cfg->readXml( net_cfg_file_name );
        std::cout << *net_cfg;
        std::cout << "=========================================" << std::endl;
    }
    else
    {
        exit( 0 );
    }

    while ( true )
    {
        std::cout << "Modify network configuration" << std::endl;
        std::cout << std::endl;
        // main selection
        std::cout << "    A. Template Router Configuration" << std::endl;
        std::cout << "    B. Template NI Configuration" << std::endl;
        std::cout << "    C. Special Router Configuration" << std::endl;
        std::cout << "    D. Special NI Configuration" << std::endl;
        std::cout << "    E. Reset Network Configuration" << std::endl;
        std::cout << "    F. Save and Quit" << std::endl;
        std::cout << std::endl;
        char res2 = makeSelection( "AaBbCcDdEeFf" );
        std::cout << "=========================================" << std::endl;

        if ( res2 == 'A' || res2 == 'a' )
        {
            std::cout << "Modify network configuration" << std::endl;
            std::cout << "Configuration of template router" << std::endl;
            std::cout << std::endl;
            std::cout << "Select Field to Modify:" << std::endl;
            std::cout << std::endl;
            EsyNetworkCfgRouter& template_router = net_cfg->templateRouter();
            std::cout << "    0. Pipeline cycle : " << template_router.pipeCycle() << std::endl;
            std::cout << "    1. Physical port : " << template_router.portNum() << std::endl;
            std::cout << "    2. Input  VC number : " << template_router.maxInputVcNum()  << std::endl;
            std::cout << "    3. Output VC number : " << template_router.maxOutputVcNum() << std::endl;
            std::cout << "    4. Input  buffer : " << template_router.maxInputBuffer()  << std::endl;
            std::cout << "    5. Output buffer : " << template_router.maxOutputBuffer() << std::endl;
            std::cout << "    Y. Confirm configuration." << std::endl;
            char resA = makeSelection( "012345Yy" );
            std::cout << "=========================================" << std::endl;
        }
        else if ( res2 == 'B' || res2 == 'b' )
        {
            std::cout << "Modify network configuration" << std::endl;
            std::cout << "Configuration of NI B" << std::endl;
            std::cout << "" << std::endl;
            char resA4 = makeSelection( "a" );
            std::cout << "=========================================" << std::endl;
        }
        else if ( res2 == 'C' || res2 == 'c' )
        {
            std::cout << "Modify network configuration" << std::endl;
            std::cout << "Reset Configuration" << std::endl;
            std::cout << "" << std::endl;
            char resA4 = makeSelection( "a" );
            std::cout << "=========================================" << std::endl;
        }
        else
        {
            std::cout << "Modify network configuration" << std::endl;
            std::cout << "Save modification" << std::endl;
            std::cout << std::endl;
            std::cout << "    File path: " << net_cfg_file_name << std::endl;
            net_cfg->writeXml( net_cfg_file_name );
            std::cout << "=========================================" << std::endl;
            break;
        }
    }

    exit( 0 );
}

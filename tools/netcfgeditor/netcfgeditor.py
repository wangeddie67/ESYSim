
#! /usr/bin/env python3

import locale
import os
from dialog import Dialog

locale.setlocale( locale.LC_ALL, '' )

# Configuration window
d = Dialog( dialog="dialog", autowidgetsize=True )
d.set_background_title( "Network Configuration Editor" )

# Select module
code, tags = d.menu( "Create or Open File",
                    choices=[( "1", "Create new network configuration" ),
                             ( "2", "Modify network configuration" )] )

if ( code != d.OK ) :
    exit( 0 )

if ( tags == "1" ) :
    d.msgbox( "Create new" )

    topology = 0;
    net_size = [ 1 ];
    router_count = 1;
    
    pipe_cycle = 1.0;
    phy_port = 0;
    input_vc_num = 1;
    output_vc_num = 1;
    input_buffer = 12;
    output_buffer = 12;
    
    ni_pipe_cycle = 1.0;
    ni_buffer_size = 12;
    ni_interrupt_delay = 0;

    while True :
        code, tags = d.menu( title="Create Network Configuration",
            text="Select network topology:",
            choices=[( "0", "Switch" ),
                ( "1", "Ring" ),
                ( "2", "2D Mesh" ),
                ( "3", "2D Torus" ),
                ( "4", "Multi-Dia Mesh" ),
                ( "5", "Multi-Dia Torus" ),
                ( "6", "Irregular" )] )

        if code == d.OK:
            topology = int( tags );
            break;

    # switch
    if topology == 0 :
        net_size = [ 1 ]
        router_num = 1
        phy_port = 5
    # ring and irregular
    elif topology == 1 or topology == 6:
        while True :
            code, val_list = d.form( title="Create Network Configuration", 
                text="Specify network size",
                elements=[( "Routers", 1, 1, "1", 1, 20, 5, 2)])

            if code == d.OK:
                val_item = int( val_list[ 0 ] )
                net_size = [ val_item ]
                router_num = val_item
                phy_port = 3
                break;
    # 2D mesh and 2D torus
    elif topology == 2 or topology == 3:
        while True :
            code, val_list = d.form( title="Create Network Configuration", 
                text="Specify network size",
                elements=[( "AX_X (lower)",  1, 1, "8", 1, 20, 5, 2),
                    ( "AX_Y (higher)", 2, 1, "8", 2, 20, 5, 2)] )

            if code == d.OK:
                net_size = []
                router_num = 1
                for val_item_str in val_list :
                    val_item = int( val_item_str )
                    net_size.append( val_item )
                    router_num *= val_item
                phy_port = 5
                break;
    # 2D mesh and 2D torus
    elif topology == 4 or topology == 5:
        dim = 3
        while True :
            code, val_list = d.form( title="Create Network Configuration", 
                text="Specify Diamension",
                elements=[( "Diamension", 1, 1, "3", 1, 20, 5, 2)] )
            
            if code == d.OK:
                dim = int( val_list[ 0 ] )
                break;

        elements = []
        for i in range( 0, dim ):
            label = "Diamension [" + str( i ) + "] "
            if dim > 1:
                if i == 0:
                    label += " (lower)"
                elif i == dim - 1:
                    label += " (higher)"
            elements.append( ( label, i + 1, 1, "2", i + 1, 20, 5, 2 ) )
        while True :
            code, val_list = d.form( title="Create Network Configuration", 
                text="Specify network size",
                elements=elements )
            
            if code == d.OK:
                net_size = []
                router_num = 1
                for val_item_str in val_list :
                    val_item = int( val_item_str )
                    net_size.push_back( val_item )
                    router_num *= val_item
                phy_port = dim * 2 + 1
                break;

    while True :
        code, val_list = d.form( title="Create Network Configuration", 
            text="Specify Template Router",
            elements=[( "Pipeline", 1, 1, "1.0", 1, 20, 5, 4 ),
                      ( "Physical Ports", 2, 1, str(phy_port), 2, 20, 5, 2 ),
                      ( "Input VC", 3, 1, "1", 3, 20, 5, 2 ),
                      ( "Output VC", 4, 1, "1", 4, 20, 5, 2 ),
                      ( "Input Buf Size", 5, 1, "10", 5, 20, 5, 3 ),
                      ( "Output Buf Size", 6, 1, "10", 6, 20, 5, 3 )] )

        if code == d.OK:
            pipe_cycle = float( val_list[ 0 ] )
            phy_port = int( val_list[ 1 ] )
            input_vc_num = int( val_list[ 2 ] )
            output_vc_num = int( val_list[ 3 ] )
            input_buffer = int( val_list[ 4 ] )
            output_buffer = int( val_list[ 5 ] )
            break;

    while True :
        code, val_list = d.form( title="Create Network Configuration", 
            text="Specify Template Network Interface",
            elements=[( "Pipeline", 1, 1, "1.0", 1, 20, 5, 4 ),
                      ( "Buffer Size", 2, 1, "10", 2, 20, 5, 3 ),
                      ( "Interrupt Delay", 3, 1, "10", 3, 20, 5, 3 )] )

        if code == d.OK:
            ni_pipe_cycle = float( val_list[ 0 ] )
            ni_buffer_size = int( val_list[ 1 ] )
            ni_interrupt_delay = float( val_list[ 2 ] )
            break;

    code, path = d.fselect( "" )

    #os.system( "netcfgeditor -n path" )

else :
    code, path = d.fselect( "" )
    print( path )

d.msgbox( "Modify new" )



import os
from dialog import Dialog

def is_int( str ) :
    try :
        int( str )
        return True
    except ValueError :
        return False

def is_float( str ) :
    try :
        if str == 'Nan' :
            return False
        float( str )
        return True
    except ValueError :
        return False

def checkValue( d, string, type='int', min_value=None, max_value=None, value_name=None ) :
    value = 0
    if type == 'int' :
        if is_int( string ) :
            value = int( string )
        else :
            text = "Input value "
            if value_name :
                text += "of {0} ".format( value_name )
            text = "is not an integar number. Please input again."
            d.msgbox( text )
            return False
    if type == 'float' :
        if is_float( string ) :
            value = float( string )
        else :
            text = "Input value "
            if value_name :
                text += "of {0} ".format( value_name )
            text = "is not a float number. Please input again."
            d.msgbox( text )
            return False

    if min_value :
        if value < min_value :
            text = "Input value "
            if value_name :
                text += "of {0} is output range ".format( value_name )
            if max_value :
                text += "({0} <= value <= {1}). Please input again.".format( min_value, max_value )
            else :
                text += "({0} <= value). Please input again.".format( min_value )
            d.msgbox( text )
            return False
    if max_value :
        if value > max_value :
            text = "Input value "
            if value_name :
                text += "of {0} is output range ".format( value_name )
            if min_value :
                text += "({0} <= value <= {1}). Please input again.".format( min_value, max_value )
            else :
                text += "(value <= {0}). Please input again.".format( max_value )
            d.msgbox( text )
            return False

    return True

##
# @brief Page to configure topology
# @param d Dialog instance
# @param title  Page title
# @return Dictionary of configured variables (key = \"topology\"). If push cannel, return empty dictionary.
def pageTopology( d, title ) :
    text="Select network topology:"
    choices = [ ( 'Switch'   , 'Single Switch' ),
                ( 'Ring'     , 'Double Ring' ),
                ( '2DMesh'   , '2D Mesh' ),
                ( '2DTorus'  , '2D Torus' ),
                ( 'DiaMesh'  , 'Multi-diamension Mesh, dim >= 1' ),
                ( 'DiaTorus' , 'Multi-diamension Torus, dim >= 1' ),
                ( 'Irregular', 'Not diamensional topology, like tree' ) ]
    # Call dialog
    code, tags = d.menu( title=title, text=text, choices=choices )
    # If push OK, return the selected topology.
    if code == d.OK :
        return { "topology" : tags }
    # If push Cancel, return empty dictionary.
    else :
        return {}

##
# @brief Page to configure size
# @param d Dialog instance
# @param title  Page title
# @param topology  Topology string (str= \"Switch\", \"Ring\", \"2DMesh\", \"2DTorus\", \"DiaMesh\", \"DiaTorus\", \"Irregular\".
# @return Dictionary of configured variables (key = \"net_size\", \"router_count\", \"port_num\"). If push cannel, return empty dictionary.
def pageNetSize( d, title, topology ) :
    text="Specify network size"
    elements=[]

    # Switch
    if topology == "Switch" :
        return { "net_size"     : "1",
                 "router_count" : "1",
                 "port_num"     : "5" }
    # Ring and Irregular
    elif topology == "Ring" or topology == "Irregular" :
        elements.append( ( "Routers", 1, 1, "1", 1, 20, 5, 2 ) )
        # Call dialog
        while True :
            code, val_list = d.form( title=title, text=text, elements=elements )
            if code == d.OK :
                if checkValue( d, val_list[ 0 ], 'int', min_value=1, value_name="Routers" ) :
                    return { "net_size"     : val_list[ 0 ],
                             "router_count" : val_list[ 0 ],
                             "port_num"     : "3" }
            else :
                return {}
    # 2D mesh and 2D torus
    elif topology == "2DMesh" or topology == "2DTorus" :
        elements.append( ( "AX_X (lower)",  1, 1, "8", 1, 20, 5, 2) )
        elements.append( ( "AX_Y (higher)", 2, 1, "8", 2, 20, 5, 2) )
        # Call dialog
        while True :
            code, val_list = d.form( title=title, text=text, elements=elements )
            if code == d.OK:
                if checkValue( d, val_list[ 0 ], 'int', min_value=1, value_name="AX_X" ) \
                    and checkValue( d, val_list[ 1 ], 'int', min_value=1, value_name="AX_Y" ) :
                    net_size = val_list[ 0 ] + " " + val_list[ 1 ]
                    router_count = str( int( val_list[ 0 ] ) * int( val_list[ 1 ] ) )
                    return { "net_size"     : net_size,
                             "router_count" : router_count,
                             "port_num"     : "5" }
            else :
                return {}
    # 2D mesh and 2D torus
    elif topology == "DiaMesh" or topology == "DiaTorus":
        dim_text = "Specify Diamension"
        dim_elements = [ ( "Diamension", 1, 1, "3", 1, 20, 5, 2) ]
        while True:
            code, val_list = d.form( title=title, text=dim_text, elements=dim_elements )
            if code == d.OK :
                if checkValue( d, val_list[ 0 ], 'int', min_value=1, value_name="Diamension" ) :
                    dim = int( val_list[ 0 ] )
                    break
            else :
                return {}

        for i in range( 0, dim ) :
            label = "Diamension [{0}] ".format( i )
            if dim > 1:
                if i == 0:
                    label += " (lower)"
                elif i == dim - 1:
                    label += " (higher)"
            elements.append( ( label, i + 1, 1, "2", i + 1, 20, 5, 2 ) )

        while True:
            code, val_list = d.form( title=title, text=text, elements=elements )
            if code == d.OK :
                net_size = ""
                router_num = 1
                wrongvalue = False
                for val_item_str in val_list :
                    if not checkValue( d, val_item_str, 'int', 1 ) :
                        wrongvalue = True
                        break;
                    net_size += val_item_str + " "
                    router_num *= int( val_item_str )
                if not wrongvalue:
                    return { "net_size"     : net_size,
                             "router_count" : str( router_num ),
                             "port_num"     : str( dim * 2 + 1 ) }
            else :
                return {}
    # unknown topology
    else :
        return {}

##
# @brief Page to configure router
# @param d Dialog instance
# @param title  Page title
# @param phy_port Default number of physical port. string
# @param default Default parameters
# @param mode  Page mode: \"New\", \"Modify\"
# @return Dictionary of configured variables (key = \"pipe_cycle\", \"port_num\", \"in_vc_num\", \"out_vc_num\", \"in_buf_size\", \"out_buf_size\").
#         If push cannel, return empty dictionary.
def pageRouter( d, title, phy_port="3", default={}, mode="New" ) :
    text = "Specify Router"
    if mode == "New" :
        elements = [ ( "Pipeline",        1, 1, "1.0",    1, 20, 5, 4 ),
                     ( "Physical Ports",  2, 1, phy_port, 2, 20, 5, 2 ),
                     ( "Input VC",        3, 1, "1",      3, 20, 5, 2 ),
                     ( "Output VC",       4, 1, "1",      4, 20, 5, 2 ),
                     ( "Input Buf Size",  5, 1, "10",     5, 20, 5, 3 ),
                     ( "Output Buf Size", 6, 1, "10",     6, 20, 5, 3 ) ]
    elif mode == "Modify" :
        elements = [ ( "Pipeline",       1, 1, default[ "pipe_cycle" ], 1, 20, 5, 4 ),
                     ( "Physical Ports", 2, 1, default[ "port_num" ],   2, 20, 5, 2 ) ]
    else :
        return {}

    # Router page
    while True:
        code, val_list = d.form( title=title, text=text, elements=elements )
        if code == d.OK:
            if mode == "New" :
                if checkValue( d, val_list[ 0 ], 'float', min_value=0.0, value_name='Pipeline' ) \
                    and checkValue( d, val_list[ 1 ], 'int', min_value=int(phy_port), value_name='Physical ports' ) \
                    and checkValue( d, val_list[ 2 ], 'int', min_value=1, value_name='Input VC' ) \
                    and checkValue( d, val_list[ 3 ], 'int', min_value=1, value_name='Output VC' ) \
                    and checkValue( d, val_list[ 4 ], 'int', min_value=1, value_name='Input Buf Size' ) \
                    and checkValue( d, val_list[ 5 ], 'int', min_value=1, value_name='Output Buf Size' ) :
                    return { "pipe_cycle"   : val_list[ 0 ],
                             "port_num"     : val_list[ 1 ],
                             "in_vc_num"    : val_list[ 2 ],
                             "out_vc_num"   : val_list[ 3 ],
                             "in_buf_size"  : val_list[ 4 ],
                             "out_buf_size" : val_list[ 5 ] }
            elif mode == "Modify" :
                if checkValue( d, val_list[ 0 ], 'float', min_value=0.0, value_name='Pipeline' ) \
                    and checkValue( d, val_list[ 1 ], 'int', min_value=int(phy_port), value_name='Physical ports' ) :
                    return { "pipe_cycle" : val_list[ 0 ],
                             "port_num"   : val_list[ 1 ] }
            else:
                return {}
        else :
            return {}

##
# @brief Page to configure network interface
# @param d Dialog instance
# @param title  Page title
# @param default Default parameters
# @param mode  Page mode: \"New\", \"Modify\", \"ModTemp"
# @return Dictionary of configured variables (key = \"ni_pipe_cycle\", \"ni_buf_size\", \"ni_int_delay\", \"conn_router\", \"conn_port\").
#         If push cannel, return empty dictionary.
def pageNI( d, title, default={}, mode="New" ) :
    text = "Specify Network interface"
    if mode == "New" :
        elements = [ ( "Pipeline",        1, 1, "1.0", 1, 20, 5, 4 ),
                     ( "Buffer Size",     2, 1, "10",  2, 20, 5, 3 ),
                     ( "Interrupt Delay", 3, 1, "100", 3, 20, 5, 3 ) ]
    elif mode == "Modify" or mode == "ModTemp" :
        elements = [ ( "Pipeline",        1, 1, default[ "ni_pipe_cycle" ], 1, 20, 5, 4 ),
                     ( "Buffer Size",     2, 1, default[ "ni_buf_size" ],   2, 20, 5, 3 ),
                     ( "Interrupt Delay", 3, 1, default[ "ni_int_delay" ],  3, 20, 5, 3 ) ]
        if mode == "Modify" :
            elements += [ ( "Connect Router",  4, 1, default[ "ni_conn_router"], 4, 20, 5, 3 ),
                          ( "Connect Port",    5, 1, default[ "ni_conn_port" ],  5, 20, 5, 3 ) ]
    else :
        return {}

    while True :
        code, val_list = d.form( title=title, text=text, elements=elements )
        if code == d.OK :
            if mode == "New" or mode == "ModTemp" :
                if checkValue( d, val_list[ 0 ], 'float', min_value=0.0, value_name="Pipeline" ) \
                    and checkValue( d, val_list[ 1 ], 'int', min_value=1, value_name="Buffer Size" ) \
                    and checkValue( d, val_list[ 2 ], 'float', min_value=0.0, value_name="Interrupt Delay" ) :
                    return { "ni_pipe_cycle" : val_list[ 0 ],
                             "ni_buf_size"   : val_list[ 1 ],
                             "ni_int_delay"  : val_list[ 2 ] }
            elif mode == "Modify" :
                if checkValue( d, val_list[ 0 ], 'float', min_value=0.0, value_name="Pipeline" ) \
                    and checkValue( d, val_list[ 1 ], 'int', min_value=1, value_name="Buffer Size" ) \
                    and checkValue( d, val_list[ 2 ], 'float', min_value=0.0, value_name="Interrupt Delay" ) \
                    and checkValue( d, val_list[ 3 ], 'int', min_value=0, value_name="Connect Router" ) \
                    and checkValue( d, val_list[ 4 ], 'int', min_value=0, value_name="Connect Port" ):
                    return { "ni_pipe_cycle" : val_list[ 0 ],
                             "ni_buf_size"   : val_list[ 1 ],
                             "ni_int_delay"  : val_list[ 2 ],
                             "conn_router"   : val_list[ 3 ],
                             "conn_port"     : val_list[ 4 ] }
            else :
                return {}
        else :
            return {}

##
# @brief Page to configure port
# @param d Dialog instance
# @param title  Page title
# @param default Default parameters
# @param temp True, the port belongs to the template router.
# @return Dictionary of configured variables (key = \"pipe_cycle\", \"port_num\", \"in_vc_num\", \"out_vc_num\", \"in_buf_size\", \"out_buf_size\").
#         If push cannel, return empty dictionary.
def pagePort( d, title, default={}, temp=False ) :
    text = "Specify Port"
    elements = [ ( "Input VC",        1, 1, default[ "in_vc_num" ],     1, 20, 5, 2 ),
                 ( "Output VC",       2, 1, default[ "out_vc_num" ],    2, 20, 5, 2 ),
                 ( "Input Buf Size",  3, 1, default[ "in_buf_size" ],   3, 20, 5, 3 ),
                 ( "Output Buf Size", 4, 1, default[ "out_buf_size" ],  4, 20, 5, 3 ),
                 ( "Axis",            5, 1, default[ "axis" ],          5, 20, 5, 2 ),
                 ( "Axis Dir (U/D)",  6, 1, default[ "axis_dir" ][ 0 ], 6, 20, 5, 1 ),
                 ( "NI (T/F)",        7, 1, default[ "ni" ][ 0 ],       7, 20, 5, 1 ) ]
    if not temp :
        elements.append( ( "Connect router",  8, 1, default[ "conn_router" ],   8, 20, 5, 3 ) )
        elements.append( ( "Connect Port",    9, 1, default[ "conn_port" ],     9, 20, 5, 3 ) )

    while True:
        code, val_list = d.form( title=title, text=text, elements=elements )
        if code == d.OK:
            if checkValue( d, val_list[ 0 ], 'int', min_value=1, value_name="Input VC" ) \
                and checkValue( d, val_list[ 1 ], 'int', min_value=1, value_name="Output VC" ) \
                and checkValue( d, val_list[ 2 ], 'int', min_value=1, value_name="Input Buf Size" ) \
                and checkValue( d, val_list[ 3 ], 'int', min_value=1, value_name="Output Buf Size" ) \
                and checkValue( d, val_list[ 4 ], 'int', min_value=1, value_name="Axis" ) :
                ret_dict = { "in_vc_num"    : val_list[ 0 ],
                             "out_vc_num"   : val_list[ 1 ],
                             "in_buf_size"  : val_list[ 2 ],
                             "out_buf_size" : val_list[ 3 ],
                             "axis"         : val_list[ 4 ] }
                if val_list[ 5 ] == "U" or val_list[ 5 ] == "u" :
                    ret_dict[ "axis_dir" ] = "Upward"
                elif val_list[ 5 ] == "D" or val_list[ 5 ] == "d" :
                    ret_dict[ "axis_dir" ] = "Downward"
                if val_list[ 6 ] == "T" or val_list[ 6 ] == "t" :
                    ret_dict[ "ni" ] = "1"
                elif val_list[ 6 ] == "F" or val_list[ 6 ] == "f" :
                    ret_dict[ "ni" ] = "0"
                if not temp :
                    ret_dict[ "conn_router" ] = val_list[ 7 ]
                    ret_dict[ "conn_router" ] = val_list[ 8 ]

                return ret_dict
        else :
            return {}


def pageUpdate( d, title ) :
    code = d.yesno( text="Do you want to update the network configuration?" )
    if code == d.OK :
        return True
    else :
        return False


def pageSelComp( d, text, min_max=[] ) :
    # text to display
    if min_max :
        text_range = "{0} ({1}-{2}) :".format( text, min_max[ 0 ], min_max[ 1 ] )
    else :
        text_range = text

    while True :
        # page inputbpx
        code, string = d.inputbox( text=text_range )
        if code == d.OK :
            if checkValue( d, string, 'int', min_max[0], min_max[1] ) :
                return code, int( string )
        else :
            return code, 0


def pageFileSelect( d, title ) :
    while True :
        code, path = d.fselect( title=title, filepath="" )
        if code == d.OK:
            if path != "" :
                path = os.path.splitext( path )[ 0 ]
                return path
        else :
            return ""

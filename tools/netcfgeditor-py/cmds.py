
import os

net_cfg_editor_path = "/home/snake1989/ESYSim/bin/netcfgeditor"

def parseDict( text ) :
    lines = text.split( "\n" )

    out_dict = {}

    for line in lines:
        line_strs = line.split( " " )
        valid_strs = []
        for t in line_strs:
            if t != "":
                valid_strs.append( t )

        if len( valid_strs ) >= 3 and valid_strs[ 1 ] == "=" :
            key = valid_strs[ 0 ]
            if len( valid_strs ) == 3:
                value = valid_strs[ 2 ]
            else:
                value = valid_strs[ 2: ]
            out_dict[ key ] = value

    return out_dict


key2option_dict = { "topology"       : "-topology",
                    "net_size"       : "-network_size",
                    "pipe_cycle"     : "-pipe_cycle",
                    "port_num"       : "-phy_number",
                    "in_vc_num"      : "-in_vc_number",
                    "out_vc_num"     : "-out_vc_number",
                    "in_buf_size"    : "-in_buffer_size",
                    "out_buf_size"   : "-out_buffer_size",
                    "axis"           : "-axis",
                    "axis_dir"       : "-axis_dir",
                    "conn_router"    : "-connect_router",
                    "conn_port"      : "-connect_port",
                    "ni_pipe_cycle"  : "-ni_pipe_cycle",
                    "ni_buf_size"    : "-ni_buffer_size",
                    "ni_int_delay"   : "-ni_interrupt_delay",
                    "ni_conn_router" : "-ni_connect_router",
                    "ni_conn_port"   : "-ni_connect_port"
                   }

def createNetcfg( path, net_cfg_dict = {} ) :
    # Command
    keys = [ "topology", "net_size", "pipe_cycle", "port_num", "in_vc_num", "out_vc_num",
             "in_buf_size", "out_buf_size", "ni_pipe_cycle", "ni_buf_size", "ni_int_delay" ]
    cmd = net_cfg_editor_path + " -create"
    for key in keys :
        if key in net_cfg_dict :
            cmd += " " + key2option_dict[ key ] + " " + net_cfg_dict[ key ]
    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    # Show output
    return output.read()

def viewNetCfg( path ) :
    # command
    cmd = net_cfg_editor_path + " -view"
    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    # Show output
    return output.read()

def searchNetCfg( path ) :
    # command
    cmd = net_cfg_editor_path + " -search"
    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    net_cfg_out = output.read()
    # Get parameters from output
    return parseDict( net_cfg_out )

def searchRouterCfg( path, router_id=0, temp_router=False ) :
    # command
    cmd = net_cfg_editor_path + " -search"
    if temp_router :
        cmd += " -template_router"
    else :
        cmd += " -router_id " + str( router_id )
    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    router_cfg_out = output.read()
    # Get perameters from output
    return parseDict( router_cfg_out )

def searchNICfg( path, ni_id=0, temp_ni=False ) :
    # command
    cmd = net_cfg_editor_path + " -search"
    if temp_ni :
        cmd += " -template_ni"
    else :
        cmd += " -ni_id " + str( ni_id )
    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    ni_cfg_out = output.read()
    # Get perameters from output
    return parseDict( ni_cfg_out )

def searchPortCfg( path, port_id=0, router_id=0, temp_router=False ) :
    # command
    cmd = net_cfg_editor_path + " -search"
    if temp_router :
        cmd += " -template_router -port_id " + str( port_id )
    else :
        cmd += " -router_id " + str( router_id ) +  " -port_id " + str( port_id )
    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    port_cfg_out = output.read()
    # Get perameters from output
    return parseDict( port_cfg_out )

def modifyRouterCfg( path, router_cfg={}, router_id=0, temp_router=False, update=False ) :
    keys = [ "pipe_cycle", "port_num" ]
    # command
    cmd = net_cfg_editor_path + " -modify"
    if temp_router :
        cmd += " -template_router"
    else :
        cmd += " -router_id " + str( router_id )

    for key in keys :
        if key in router_cfg :
            cmd += " " + key2option_dict[ key ] + " " + router_cfg[ key ]

    if temp_router and update :
        cmd += " -update"

    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    # Show output
    return output.read()

def modifyNICfg( path, ni_cfg={}, ni_id=0, temp_ni=False, update=False ) :
    keys = [ "ni_pipe_cycle", "ni_buf_size", "ni_int_delay" ]
    if not temp_ni :
        keys += [ "conn_router", "conn_port" ]
    # command
    cmd = net_cfg_editor_path + " -modify"
    if temp_ni :
        cmd += " -template_ni"
    else :
        cmd += " -ni_id " + str( ni_id )

    for key in keys :
        if key in ni_cfg :
            cmd += " " + key2option_dict[ key ] + " " + ni_cfg[ key ]

    if temp_ni and update :
        cmd += " -update"

    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    # Show output
    return output.read()

def modifyPortCfg( path, port_cfg={}, port_id=0, router_id=0, temp_router=False, update=False ) :
    keys = [ "in_vc_num", "out_vc_num", "in_buf_size", "out_buf_size", "axis", "axis_dir" ]
    if not temp_router :
        keys += [ "conn_router", "conn_port" ]
    # command
    cmd = net_cfg_editor_path + " -modify"
    if temp_router :
        cmd += " -template_router -port_id " + str( port_id )
    else :
        cmd += " -router_id " + str( router_id ) +  " -port_id " + str( port_id )

    for key in keys :
        if key in port_cfg :
            cmd += " " + key2option_dict[ key ] + " " + port_cfg[ key ]

    if port_cfg[ "ni" ] == '1' :
        cmd += " -connect_ni"
    else :
        cmd += " -not_connect_ni"

    if temp_router and update :
        cmd += " -update"

    cmd += " -network_cfg_file_name " + path
    # Execution
    output = os.popen( cmd )
    # Show output
    return output.read()

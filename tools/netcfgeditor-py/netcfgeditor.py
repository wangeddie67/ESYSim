#!/usr/bin/env python3

import locale
from dialog import Dialog
import pages
import cmds

##
# @brief Create newwork configuration
# @return Dictionary of parameters.
def createNewCfg() :
    # Create new network configuration

    title = "Create Network Configuration"
    # Topology page
    topology_dict = pages.pageTopology( d, title )
    if not topology_dict :
        exit( 10 )

    # network size page
    netsize_dict = pages.pageNetSize( d, title, topology_dict[ "topology" ] )
    if not netsize_dict :
        exit( 10 )

    # Router page
    router_dict = pages.pageRouter( d, title, netsize_dict[ "port_num" ], default={}, mode="New" )
    if not router_dict :
        exit( 10 )

    # NI page
    ni_dict = pages.pageNI( d, title, default={}, mode="New" )
    if not ni_dict:
        exit( 10 )

    # netcfgeditor command
    net_cfg_dict = {}
    net_cfg_dict.update( topology_dict )
    net_cfg_dict.update( netsize_dict )
    net_cfg_dict.update( router_dict )
    net_cfg_dict.update( ni_dict )
    return net_cfg_dict


if __name__ == "__main__" :
    locale.setlocale( locale.LC_ALL, '' )

    # Configuration window
    d = Dialog( dialog="dialog", autowidgetsize=True )
    d.set_background_title( "Network Configuration Editor" )

    # Select model: 1-Create new; 2-Modify
    code, tags = d.menu( title="Get Start",
                         text="Create or Open File",
                         choices=[ ( "Create", "Create new network configuration" ),
                                   ( "Modify", "Modify network configuration" ) ] )
    if ( code != d.OK ) :
        exit( 10 )
    
    # Create network configuration
    if ( tags == "Create" ) :
        # Get parameters
        net_cfg_dict = createNewCfg()
        # New network configuration file
        path = pages.pageFileSelect( d, title="New network configuration file" )
        if path == "" :
            exit( 10 )
        # Create network configuration file
        net_cfg_out = cmds.createNetcfg( path, net_cfg_dict )
        # Print file
        d.scrollbox( title="Network configuration file", text=net_cfg_out )

    else :
        # New network configuration file
        path = pages.pageFileSelect( d, title="Open network configuration file" )
        if path == "" :
            exit( 10 )
        # View network configuration file
        net_cfg_out = cmds.viewNetCfg( path )
        # Print file
        d.scrollbox( title="Network configuration file", text=net_cfg_out )

    while True:
        net_cfg_dict = cmds.searchNetCfg( path )
        router_count = int( net_cfg_dict["router_count"] )
        ni_count = int( net_cfg_dict["ni_count"] )

        # Select Components to modify
        code, tags = d.menu( "Select Components to Modify",
                             choices=[ ( "0", "Template Router" ),
                                       ( "1", "Template Network Interface" ),
                                       ( "2", "Router (0-{0})".format( router_count -1 ) ),
                                       ( "3", "Network Interface (0-{0})".format( ni_count -1 ) ) ] )
        # Quit
        if code != d.OK :
            break

        # Modify configuration of the template router
        if tags == "0" :
            # Get template router configuration
            router_cfg_dict = cmds.searchRouterCfg( path, temp_router=True )
            port_count = int( router_cfg_dict[ "port_num" ] )

            # Select Components to modify
            choices = [ ( "R", "Router arguments" ) ]
            for i in range( 0, port_count ) :
                choices.append( ( str( i ), "Port {0}".format( i ) ) )
            code, tags = d.menu( "Select Components to Modify - Template Router", choices=choices )
            if code == d.OK :
                if tags == "R" :
                    # Router page
                    title = "Modify template router"
                    router_cfg_dict_new = pages.pageRouter( d, title=title, default=router_cfg_dict, mode="Modify" )
                    if router_cfg_dict_new:
                        # Update
                        update = pages.pageUpdate( d, title )
                        # Modify router configuration
                        router_cfg_out = cmds.modifyRouterCfg( path, router_cfg=router_cfg_dict_new, temp_router=True, update=update )
                        if router_cfg_out :
                            d.scrollbox( title=title, text=router_cfg_out )
                else :
                    port_id = int( tags )
                    # Search port configuration
                    port_cfg_dict = cmds.searchPortCfg( path, port_id=port_id, temp_router=True )
                    # Configuration page
                    title = "Modify port {0} of template router".format( port_id )
                    port_cfg_dict_new = pages.pagePort(d, title, port_cfg_dict, True )
                    if port_cfg_dict_new:
                        # Update
                        update = pages.pageUpdate( d, title )
                        # Modify port configuration
                        port_cfg_out = cmds.modifyPortCfg( path, port_cfg_dict_new, port_id=port_id, temp_router=True, update=update )
                        if port_cfg_out :
                            d.scrollbox( title=title, text=port_cfg_out )
            else :
                break

        # Modify configuration of the template NI
        elif tags == "1" :
            # Search NI configuration
            ni_cfg_dict = cmds.searchNICfg( path, temp_ni=True )
            # Configuration page
            title = "Modify template NI"
            ni_cfg_dict_new = pages.pageNI( d, title, default=ni_cfg_dict, mode="ModTemp" )
            if ni_cfg_dict_new:
                # Update
                update = pages.pageUpdate( d, title )
                # Modify ni configuration
                ni_cfg_out = cmds.modifyNICfg( path, ni_cfg=ni_cfg_dict_new, temp_ni=True, update=update )
                if ni_cfg_out :
                    d.scrollbox( title=title, text=ni_cfg_out )

        # Modify configuration of specified router
        elif tags == "2" :
            code, router_id = pages.pageSelComp( d, text="Select router to modify", min_max=[ 0, router_count - 1 ] )
            if code == d.OK :
                # Get router configuration
                router_cfg_dict = cmds.searchRouterCfg( path, router_id=router_id )
                port_count = int( router_cfg_dict[ "port_num" ] )

                # Select Components to modify
                choices = [ ( "R", "Router arguments" ) ]
                for i in range( 0, port_count ) :
                    choices.append( ( str( i ), "Port {0}".format( i ) ) )
                code, tags = d.menu( "Select Components to Modify - Router {0}".format( router_id ), choices=choices )
                if code == d.OK :
                    if tags == "R" :
                        # Router page
                        title = "Modify router {0}".format( router_id )
                        router_cfg_dict_new = pages.pageRouter( d, title, default=router_cfg_dict, mode="Modify" )
                        if router_cfg_dict_new:
                            # Modify router configuration
                            router_cfg_out = cmds.modifyRouterCfg( path, router_cfg=router_cfg_dict_new, router_id=router_id )
                            if router_cfg_out :
                                d.scrollbox( title=title, text=router_cfg_out )
                    else :
                        port_id = int( tags )
                        # Search port configuration
                        port_cfg_dict = cmds.searchPortCfg( path, router_id=router_id, port_id=port_id )
                        # Configuration page
                        title = "Modify port {0} of router {1}".format( port_id, router_id )
                        port_cfg_dict_new = pages.pagePort(d, title, port_cfg_dict, True )
                        if port_cfg_dict_new:
                            # Modify port configuration
                            port_cfg_out = cmds.modifyPortCfg( path, port_cfg_dict_new, port_id=port_id, router_id=router_id )
                            if port_cfg_out :
                                d.scrollbox( title=title, text=port_cfg_out )
                else :
                    break
            else :
                break

        # Modify configuration of specified NI
        elif tags == "3" :
            code, ni_id = pages.pageSelComp( d, text="Select NI to modify", min_max=[ 0, router_count - 1 ] )
            if code == d.OK :
                # Search NI configuration
                ni_cfg_dict = cmds.searchNICfg( path, ni_id=ni_id )
                # Configuration page
                title = "Modify NI {0}".format( ni_id )
                ni_cfg_dict_new = pages.pageNI( d, title, default=ni_cfg_dict, mode="Modify" )
                if ni_cfg_dict_new:
                    # Modify NI configuration
                    ni_cfg_out = cmds.modifyNICfg( path, ni_cfg_dict_new, ni_id=ni_id )
                    if ni_cfg_out :
                        d.scrollbox( title=title, text=ni_cfg_out )

// **** **** **** **** **** **** **** **** **** **** **** **** **** //
//      Visual Network-on-chip Development Studio                   //
//                                                                  //
//  File name    : flow_tool.h                                     //
//  Last Modified: 2014.07.20                                       //
//  Purpose      : implement class integrator                       //
//  Author       : Jun-shi Wang                                     //
//  Organization : Green Computing and Communication @ UESTC-SCIE   //
//  VisualNoC is Copyright(c)UESTC-SCIE-GCC 2014,                   //
//                                                                  //
//  All rights reserved, and is distributed as free software under  //
//  the license in the file "License",which is included in the      //
//  distribution.                                                   //
// **** **** **** **** **** **** **** **** **** **** **** **** **** //

#ifndef FLOWTOOL_H
#define FLOWTOOL_H

// flow tree item string
#define FT_NETWORK  QString( "Network" )
    #define FT_NETWORK_NEW      QString( "New Network" )
    #define FT_NETWORK_OPEN     QString( "Open Network" )
    #define FT_NETWORK_SAVE     QString( "Save Network" )
    #define FT_NETWORK_SAVEAS   QString( "Save As Network" )
    #define FT_NETWORK_CONFIG   QString( "Configure Network" )
    #define FT_NETWORK_CLOSE    QString( "Close Network" )
#define FT_ANALYSE	QString( "Analyse" )
    #define FT_ANALYSE_ANIMATION_ANALYSE    QString( "Replay Simulation" )
    #define FT_ANALYSE_PATH_DISPLAY         QString( "Replay Path" )
    #define FT_ANALYSE_BENCHMARK            QString( "Benchmark Result" )
    #define FT_ANALYSE_EVENTTRACE           QString( "Event Trace Result" )
    #define FT_ANALYSE_PATH                 QString( "Path Result" )
#define FT_TOOLS	QString( "Tools" )
    #define FT_TOOLS_CFG            QString( "Tools Configuration" )
    #define FT_TOOLS_RENEW_TOOLS    QString( "Renew Tools" )
    #define FT_TOOLS_LINK_TOOLS     QString( "Link Tools" )
    #define FT_TOOLS_ENV_CONFIG     QString( "Set Work Direction")
#define FT_HELP		QString( "Help" )
    #define FT_ABOUT    QString( "About" )

#define FTT_NAME_COL    0
#define FTT_CONFIGURE   QString( "Configure" )
#define FTT_SIMULATE    QString( "Simulate" )
#define FTT_ANALYSE     QString( "Analyse" )

#endif // FLOWTOOL_H

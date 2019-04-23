#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T18:21:09
#
#-------------------------------------------------

DEFINES += QWT_DLL

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport concurrent

TARGET = visualnoc
TEMPLATE = app

SOURCES += \
    nocwindow.cc \
    nocmain.cc \
    define/qt_include.cc \
    define/gui_component.cc \
    ../interface/esy_argument.cc \
    ../interface/esy_conv.cc \
    ../interface/esy_iodatafile_item.cc \
    ../interface/esy_iodatafile.cc \
    ../interface/esy_interdata.cc \
    ../interface/esy_networkcfg.cc \
    ../interface/esy_xmlerror.cc \
    ../interface/esy_faultcfg.cc \
    ../tinyxml/tinystr.cpp \
    ../tinyxml/tinyxml.cpp \
    ../tinyxml/tinyxmlparser.cpp \
    ../tinyxml/tinyxmlerror.cpp \
    modelsview/network_cfg_tree.cc \
    modelsview/path_trace_table.cc \
    modelsview/router_state_table.cc \
    modelsview/link_tool_table.cc \
    modelsview/argument_list_table.cc \
    modelsview/analyse_result_table.cc \
    modelsview/faults_state_table.cc \
    modelsview/faults_list_table.cc \
    modelsview/noc_graphics_config_tree.cc \
    modelsview/application_gantt_chart.cc \
    subdialogs/noc_dialog.cc \
    subdialogs/process_dialog.cc \
    subdialogs/tools_para_dialog.cc \
    subdialogs/link_tool_dialog.cc \
    subdialogs/faults_config_dialog.cc \
    subdialogs/graphics_config_dialog.cc \
    subdialogs/noc_analyse_dialog.cc \
    subwidgets/noc_view.cc \
    subwidgets/port_graphics_item.cc \
    subwidgets/result_tab_widget.cc \
    subwidgets/router_graphics_item.cc \
    subwidgets/tools_config_dialog.cc \
    subwidgets/analyse_plot.cc \
    subwidgets/automation_control_panel.cc \
    subwidgets/analyse_fliter_panel.cc \
    threads/analyse_pathtrace_thread.cc \
    threads/analyse_benchmark_thread.cc \
    threads/analyse_trafficwindow_thread.cc \
    threads/analyse_eventtrace_thread.cc

HEADERS += \
    nocwindow.h \
    define/qt_include.h \
    define/version_include.h \
    define/gui_component.h \
    ../interface/esy_argument.h \
    ../interface/esy_conv.h \
    ../interface/esy_iodatafile_item.h \
    ../interface/esy_iodatafile.h \
    ../interface/esy_interdata.h \
    ../interface/esy_networkcfg.h \
    ../interface/esy_xmlerror.h \
    ../interface/esy_eventtype.h \
    ../interface/esy_faultcfg.h \
    ../tinyxml/tinystr.h \
    ../tinyxml/tinyxml.h \
    modelsview/network_cfg_tree.h \
    modelsview/path_trace_table.h \
    modelsview/router_state_table.h \
    modelsview/link_tool_table.h \
    modelsview/argument_list_table.h \
    modelsview/analyse_result_table.h \
    modelsview/faults_state_table.h \
    modelsview/faults_list_table.h \
    modelsview/graphics_config_tree.h \
    modelsview/application_gantt_chart.h \
    subdialogs/noc_analyse_dialog.h \
    subdialogs/noc_dialog.h \
    subdialogs/process_dialog.h \
    subdialogs/result_base_dialog.h \
    subdialogs/tools_para_dialog.h \
    subdialogs/link_tool_dialog.h \
    subdialogs/faults_config_dialog.h \
    subdialogs/graphics_config_dialog.h \
    subwidgets/noc_view.h \
    subwidgets/port_graphics_item.h \
    subwidgets/result_tab_widget.h \
    subwidgets/router_graphics_item.h \
    subwidgets/tools_config_dialog.h \
    subwidgets/analyse_plot.h \
    subwidgets/automation_control_panel.h \
    subwidgets/analyse_fliter_panel.h \
    threads/basic_thread.h \
    threads/analyse_pathtrace_thread.h \
    threads/analyse_eventtrace_thread.h \
    threads/analyse_benchmark_thread.h \
    threads/analyse_trafficwindow_thread.h

RC_FILE = logo.rc
    
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
DESTDIR = ./

INCLUDEPATH += ../interface
INCLUDEPATH += ../tinyxml
INCLUDEPATH += ./qwt/src

LIBS += -L"./qwt/lib/" -lqwt

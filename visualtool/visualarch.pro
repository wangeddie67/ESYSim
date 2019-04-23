#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T18:21:09
#
#-------------------------------------------------

DEFINES += QWT_DLL

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport concurrent

TARGET = visualarch
TEMPLATE = app

SOURCES += \
    archwindow.cc \
    archmain.cc \
    define/qt_include.cc \
    ../interface/esy_argument.cc \
    ../interface/esy_conv.cc \
    ../interface/esy_iodatafile_item.cc \
    ../interface/esy_iodatafile.cc \
    ../interface/esy_interdata.cc \
    ../interface/esy_networkcfg.cc \
    ../interface/esy_xmlerror.cc \
    ../interface/esy_faultcfg.cc \
    ../interface/esy_soccfg.cc \
    ../tinyxml/tinystr.cpp \
    ../tinyxml/tinyxml.cpp \
    ../tinyxml/tinyxmlparser.cpp \
    ../tinyxml/tinyxmlerror.cpp \
    define/gui_component.cc \
    modelsview/link_tool_table.cc \
    modelsview/arch_graphics_config_tree.cc \
    subdialogs/link_tool_dialog.cc \
    subdialogs/arch_analyse_dialog.cc \
    subdialogs/soc_dialog.cc \
    subdialogs/process_dialog.cc \
    modelsview/argument_list_table.cc \
    subwidgets/tools_config_dialog.cc \
    subwidgets/result_tab_widget.cc \
    subdialogs/tools_para_dialog.cc \
    subdialogs/graphics_config_dialog.cc \
    threads/analyse_record_thread.cc \
    subwidgets/analyse_plot.cc \
    subwidgets/automation_control_panel.cc \
    modelsview/analyse_result_table.cc \
    subwidgets/soc_view.cc \
    modelsview/soc_cfg_tree.cc \
    modelsview/tile_state_table.cc \
    subwidgets/tile_graphics_item.cc \
    subwidgets/network_graphics_item.cc \
    modelsview/application_gantt_chart.cc \
    threads/analyse_recordwindow_thread.cc

HEADERS += \
    define/qt_include.h \
    define/version_include.h \
    archwindow.h \
    ../interface/esy_argument.h \
    ../interface/esy_conv.h \
    ../interface/esy_iodatafile_item.h \
    ../interface/esy_iodatafile.h \
    ../interface/esy_interdata.h \
    ../interface/esy_networkcfg.h \
    ../interface/esy_xmlerror.h \
    ../interface/esy_eventtype.h \
    ../interface/esy_faultcfg.h \
    ../interface/esy_soccfg.h \
    ../tinyxml/tinystr.h \
    ../tinyxml/tinyxml.h \
    define/gui_component.h \
    modelsview/link_tool_table.h \
    modelsview/graphics_config_tree.h \
    subdialogs/result_base_dialog.h \
    subdialogs/link_tool_dialog.h \
    subdialogs/arch_analyse_dialog.h \
    subdialogs/soc_dialog.h \
    subdialogs/process_dialog.h \
    modelsview/argument_list_table.h \
    subwidgets/tools_config_dialog.h \
    subwidgets/result_tab_widget.h \
    subdialogs/tools_para_dialog.h \
    subdialogs/graphics_config_dialog.h \
    threads/analyse_record_thread.h \
    subwidgets/analyse_plot.h \
    subwidgets/automation_control_panel.h \
    modelsview/analyse_result_table.h \
    subwidgets/soc_view.h \
    modelsview/soc_cfg_tree.h \
    modelsview/tile_state_table.h \
    threads/basic_thread.h \
    subwidgets/tile_graphics_item.h \
    subwidgets/network_graphics_item.h \
    modelsview/application_gantt_chart.h \
    threads/analyse_recordwindow_thread.h

RC_FILE = logo.rc
    
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
DESTDIR = ./

INCLUDEPATH += ../interface
INCLUDEPATH += ../tinyxml
INCLUDEPATH += ./qwt/src

LIBS += -L"./qwt/lib/" -lqwt

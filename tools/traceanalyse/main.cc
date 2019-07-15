
#include <vector>
#include <string>
#include <iostream>

#include "esy_argument.h"
#include "esy_linktool.h"

#include "path.h"
#include "window.h"
#include "reduce.h"

using namespace std;

int main( int argc, char ** argv )
{
    string a_event_trace_file( "../example/event" );
    long a_event_trace_buffer_size = 10000;
    bool a_event_trace_text_flag = false;

    bool a_path_enable = false;
    string a_path_trace_file( "../example/pathevent" );
    long a_path_trace_buffer_size = 1000;

    bool a_reduce_enable = false;
    long a_reduce_buffer_size = 10000;
    bool a_reduce_text_enable = false;

    bool a_window_enable = false;
    long a_window_buffer_size = 100;
    string a_window_file( "../example/trafficwindow" );
    long a_window_width = 100;
    long a_window_step = 100;
    long a_router_num = 64;

    EsyArgumentList t_cfg;
    t_cfg.setHead("Analyse the event trace and generate path trace, reduced \
event trace or window trace.");
    t_cfg.setVersion("2.0");
    t_cfg.insertOpenFileIF("-event_trace_file_name", 
        "file path of event trace file", &a_event_trace_file, "", 0,
        EVENTTRACE_EXTENSION);
    t_cfg.insertLong("-event_trace_buffer_size", 
         "buffer size of event trace interface", &a_event_trace_buffer_size);
    t_cfg.insertBool("-event_trace_file_text_enable", 
         "event trace file is text format", &a_event_trace_text_flag);

    t_cfg.insertBool("-path_enable", 
         "enable path analyzer", &a_path_enable);
    t_cfg.insertNewFileIF( "-path_trace_file_name", 
         "File path of path pair", &a_path_trace_file, "-path_enable", 1,
         PATHTRACE_EXTENSION);
    t_cfg.insertLong( "-path_trace_buffer_size", 
         "buffer size of event trace interface", &a_path_trace_buffer_size,
         "-path_enable", 1);

    t_cfg.insertBool("-reduce_enable", 
         "enable reduce analyzer", &a_reduce_enable);
    t_cfg.insertLong( "-reduce_buffer_size", 
         "buffer size of reduced trace interface", &a_reduce_buffer_size,
         "-reduce_enable", 1);
    t_cfg.insertBool("-reduce_text_enable", 
         "reduced trace is text format", &a_reduce_text_enable,
         "-reduce_enable", 1);

    t_cfg.insertBool("-window_enable", 
        "enable window analyzer", &a_window_enable);
    t_cfg.insertLong("-window_buffer_size",
        "buffer size of window file interface", &a_window_buffer_size,
        "-window_enable", 1);
    t_cfg.insertNewFileIF( "-window_file_name", 
         "File path of window file", &a_window_file, "-window_enable", 1,
         WINDOW_EXTENSION);
    t_cfg.insertLong( "-window_width", 
         "width of window", &a_window_width, "-window_enable", 1);
    t_cfg.insertLong( "-window_step", 
         "step of window", &a_window_step, "-window_enable", 1);
    t_cfg.insertLong( "-router_number", 
         "number of router", &a_router_num, "-window_enable", 1);

    if ( t_cfg.analyse( argc, argv ) != EsyArgumentList::RESULT_OK )
    {
        return 0;
    }

    EsyDataFileIStream< EsyDataItemEventtrace > eventin( 
        a_event_trace_buffer_size, a_event_trace_file, EVENTTRACE_EXTENSION,
        !a_event_trace_text_flag );
    if (!eventin .openSuccess())
    {
        cout << "Cannot open traffic trace file " << a_event_trace_file << endl;
        return 0;
    }

    
    PathAnalyser * path_analyser;
    if (a_path_enable)
    {
        path_analyser = new PathAnalyser(
            a_path_trace_buffer_size, a_path_trace_file);
    }
    ReduceAnalyser * reduce_analyser;
    if (a_reduce_enable)
    {
        reduce_analyser = new ReduceAnalyser(
            a_reduce_buffer_size, a_event_trace_file, a_reduce_text_enable);
    }
    WindowAnalyser * window_analyser;
    if (a_window_enable)
    {
        window_analyser = new WindowAnalyser(
            a_window_buffer_size, a_window_file, a_router_num,
            a_window_width, a_window_step);
    }
    
    LINK_PROGRESS_INIT

    while ( !eventin.eof() )
    {
        LINK_PROGRESS_UPDATE( eventin.pos(), eventin.size() )
        
        EsyDataItemEventtrace t_event_item = eventin.getNextItem();
        if ( t_event_item.time() < 0 )
        {
            break;
        }
        
        if (a_path_enable)
        {
            path_analyser->analyse(t_event_item);
        }
        if (a_reduce_enable)
        {
            reduce_analyser->analyse(t_event_item);
        }
        if (a_window_enable)
        {
            window_analyser->analyse(t_event_item);
        }
    }

    LINK_PROGRESS_END

    if (a_path_enable)
    {
        path_analyser->finish();
    }
    if (a_reduce_enable)
    {
        reduce_analyser->finish();
    }
    if (a_window_enable)
    {
        window_analyser->finish();
    }

    LINK_RETURN( 0 )
}


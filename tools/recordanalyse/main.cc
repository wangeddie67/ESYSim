
#include <vector>
#include <string>
#include <iostream>

#include "esy_argument.h"
#include "esy_linktool.h"

#include "window.h"

using namespace std;

int main( int argc, char ** argv )
{
    string a_record_file( "../example/socrecord" );
    long a_record_buffer_size = 10000;
    bool a_record_text_flag = false;

    bool a_window_enable = false;
    long a_window_buffer_size = 100;
    string a_window_file( "../example/recordwindow" );
    long a_window_width = 100;
    long a_window_step = 100;
    long a_tile_num = 64;
    
    bool a_total_item = 0;

    EsyArgumentList t_cfg;
    t_cfg.setHead("Analyse the record trace and generate window trace.");
    t_cfg.setVersion("2.0");
    t_cfg.insertOpenFileIF("-record_file_name", "file path of record file", 
        &a_record_file, "", 0, SOCRECORD_EXTENSION);
    t_cfg.insertLong("-record_buffer_size", 
         "buffer size of record interface", &a_record_buffer_size);
    t_cfg.insertBool("-record_file_text_enable", 
         "record file is text format", &a_record_text_flag);

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
    t_cfg.insertLong( "-tile_number", 
         "number of tile", &a_tile_num, "-window_enable", 1);
    
    t_cfg.insertBool( "-display_total_hops", 
        "display total hops in record file", &a_total_item );

    if ( t_cfg.analyse( argc, argv ) != EsyArgumentList::RESULT_OK )
    {
        return 0;
    }

    EsyDataFileIStream< EsyDataItemSoCRecord > eventin( 
        a_record_buffer_size, a_record_file, SOCRECORD_EXTENSION,
        !a_record_text_flag);
    if (!eventin .openSuccess())
    {
        cerr << "Cannot open traffic trace file " << a_record_file << endl;
        return 0;
    }
    
    if (a_total_item)
    {
        cerr << "Total hop is " << eventin.total() << "." << endl;
        return 1;
    }

    long totalwindow = eventin.total() / a_window_step;;
    if (totalwindow >= 10000)
    {
        cerr << "Do not support window more than 10000" << endl;
        cerr << "Total hop is " << eventin.total() << "." << endl;
        return 1;
    }

    WindowAnalyser * window_analyser;
    if (a_window_enable)
    {
        window_analyser = new WindowAnalyser(
            a_window_buffer_size, a_window_file, a_tile_num,
            a_window_width, a_window_step);
    }
    
    LINK_PROGRESS_INIT

    while ( !eventin.eof() )
    {
        LINK_PROGRESS_UPDATE( eventin.pos(), eventin.size() )
        
        EsyDataItemSoCRecord t_event_item = eventin.getNextItem();
        if ( t_event_item.hop() < 0 )
        {
            break;
        }
        
        if (a_window_enable)
        {
            window_analyser->analyse(t_event_item);
        }
    }

    LINK_PROGRESS_END

    if (a_window_enable)
    {
        window_analyser->finish();
    }

    LINK_RETURN( 0 )
}


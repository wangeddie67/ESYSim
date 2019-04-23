
#include <vector>
#include <string>
#include <iostream>

#include "esy_interdata.h"
#include "esy_argument.h"
#include "esy_linktool.h"

using namespace std;

#include "esy_eventtype.h"

class WindowAnalyser
{
private:
    EsyDataFileOStream< EsyDataItemRecordWindow > * mp_windowout;
    vector< EsyDataItemRecordWindow > m_window_list;
    long m_window_count;
    long m_window_width;
    long m_window_step;
    long m_tile_num;

public:
    WindowAnalyser(long buffer_size, const string & path_file, long tile_num, 
                   long window_width, long window_step);
    void analyse(const EsyDataItemSoCRecord & item);
    void finish();
};


#include <vector>
#include <string>
#include <iostream>

#include "esy_interdata.h"
#include "esy_argument.h"
#include "esy_linktool.h"

using namespace std;

#include "esy_eventtype.h"

class ReduceAnalyser
{
private:
    EsyDataFileOStream< EsyDataItemEventtrace > * mp_eventout;
    long long m_total_event;

public:
    ReduceAnalyser(long buffer_size, const string & path_file, 
                   bool text_enable);
    void analyse(const EsyDataItemEventtrace & item);
    void finish();
};


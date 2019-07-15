
#include <vector>
#include <string>
#include <iostream>

#include "esy_interdata.h"
#include "esy_argument.h"
#include "esy_linktool.h"

using namespace std;

#include "esy_eventtype.h"

class PathtraceItemHop
{
public:
    char m_src;
    char m_dst;
    vector< EsyDataItemHop > m_hop;

    PathtraceItemHop() :
        m_src( -1 ), m_dst( -1 ), m_hop()
    {}
    PathtraceItemHop( char src, char dst ) :
        m_src( src ), m_dst( dst ), m_hop()
    {}
};

class PathAnalyser
{
private:
    EsyDataFileOStream< EsyDataItemPathtrace > *mp_pathout;
    vector< PathtraceItemHop > m_path_list;
    vector< long > m_path_state;
    long m_path_list_offset;

public:
    PathAnalyser(long buffer_size, const string & path_file);
    void analyse(const EsyDataItemEventtrace & item);
    void finish();
};


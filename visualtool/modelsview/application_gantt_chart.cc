#include "application_gantt_chart.h"


QStringList ProcessorItem::const_pe_status_list = QStringList() <<
    "" << "Wait" << "Receive" << "Computing" << "Send" << "Finish" << "Release";

void ProcessorItem::setTaskId( long i )
{
    if ( m_task_id == PE_RELEASE && i != PE_RELEASE )
    {
        qsrand( m_app_id );
        int r = qrand() % 150 + 100;
        qsrand( m_app_id + 555 );
        int g = qrand() % 150 + 100;
        qsrand( m_app_id + 2512 );
        int b = qrand() % 150 + 100 ;
        m_color = QColor( r, g, b );
    }
    m_task_id = i;
}


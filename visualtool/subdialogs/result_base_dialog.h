#ifndef RESULT_BASE_DIALOG_H
#define RESULT_BASE_DIALOG_H

#include "../define/qt_include.h"

class ResultBaseDialog : public QDialog
{
    Q_OBJECT
public:
    enum ResultTypeEnum
    {
        RESULT_DISPLAY_CONFIG,
        RESULT_FAULT_CONFIG,
        RESULT_ONLINE_EVENTTRACE,
        RESULT_REPLAY_EVENTTRACE,
        RESULT_REPLAY_PATH,
        RESULT_PATH_OVERLAP,
        RESULT_PORT_TRAFFIC,

        RESULT_ANALYSE_BENCHMARK,
        RESULT_ANALYSE_EVENTTRACE,
        RESULT_ANALYSE_WINDOW,
        RESULT_TOOLS_PARA,
        RESULT_RENEW_TOOLS,
        RESULT_NONE
    };

protected:
    ResultTypeEnum m_result_type;

    QList< ToolBarTab * > mp_toolbar_tab_list;

public:
    ResultBaseDialog(QWidget *parent = 0) :
        QDialog(parent)
    {
    }
    ResultBaseDialog(ResultTypeEnum type, QWidget *parent = 0) :
        QDialog(parent), m_result_type(type)
    {
    }

    ResultTypeEnum resultType()  { return m_result_type; }

    const QList< ToolBarTab * > & toolBarTabList()
        { return mp_toolbar_tab_list; }
signals:

public slots:

};

#endif // RESULT_BASE_DIALOG_H

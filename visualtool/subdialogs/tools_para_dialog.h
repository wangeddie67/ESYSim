#ifndef TOOL_PARA_DIALOG_h
#define TOOL_PARA_DIALOG_h

#include "../define/qt_include.h"
#include "result_base_dialog.h"
#include "../subwidgets/tools_config_dialog.h"
#include "../subdialogs/process_dialog.h"

class ToolsParaDialog : public ResultBaseDialog
{
	Q_OBJECT
private:
    QList< ProcessDialog * > mp_process_dialog_list;
    QScrollArea* mp_scrollarea;
    ToolsConfigDialog* mp_tools_config_dialog;
    QVBoxLayout* mp_layout;
    QToolBox* mp_toolbox;
    int m_runIndex;
    int m_finishIndex;
    int m_processCount;
public:
    ToolsParaDialog( QString fname = QString() );
signals:
	void indexProcessChanged(int pro, int index);
private slots:
	void resizeEvent(QResizeEvent *event);
    void addProcessDialogs();
    void addResultDialog();
    void processProgessUpdate( int index, int pro );
    void processFinished( int index, QProcess::ExitStatus exitStatus );
    void ProcessError( int index );
};
#endif

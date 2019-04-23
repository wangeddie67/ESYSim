#ifndef PROCESS_DIALOG_H
#define PROCESS_DIALOG_H

#include "../define/qt_include.h"
#include "result_base_dialog.h"
#include "esy_iodatafile.h"
#include "esy_interdata.h"

class ProcessDialog : public ResultBaseDialog
{
	Q_OBJECT

public:
    ProcessDialog(int index, const QString & direction, const QString & command,
                  bool trace_enable = false);
	~ProcessDialog();

public slots:
	void start();

signals:
    void processProgressChanged(int index, int pro);
    void processFinished(int index, QProcess::ExitStatus exitStatus);
    void processCerr(int);

private:
    QProgressBar * mp_progress_bar;
    QTextEdit * mp_exec_text_edit;
    QTextEdit * mp_cout_text_edit;
    QToolBox * mp_tool_box;
    QProcess * mp_process;

    int m_process_index;
    QString m_direction_string;
    QString m_command_string;
    QStringList m_args_string_list;
    QStringList m_outputs_string;

    bool m_trace_enable;
    QList< EsyDataItemEventtrace > m_eventtrace_list;
    QList< EsyDataItemSoCRecord > m_socrecord_list;
    bool m_wait_for_syn;
    bool m_finished;
    long m_progress;

public:
    QStringList outputString() const { return m_outputs_string; }
    void setExecText(QString exec) { mp_exec_text_edit ->setText(exec); }

    bool isWaitForSyn() const { return m_wait_for_syn; }
    QList< EsyDataItemEventtrace > & eventList() { return m_eventtrace_list; }
    QList< EsyDataItemSoCRecord > & recordList() { return m_socrecord_list; }
    long progress() { return m_progress; }
    void synchornizeNext();
    bool isFinished() { return m_finished; }

private slots:
    void readProcessCout();
    void readProcessCerr();
    void processFinish(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif 

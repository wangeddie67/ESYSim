#ifndef PARALLELCLASS_H
#define PARALLELCLASS_H

#include <QObject>
#include "processclass.h"
#include <QFile>
#include <QTextStream>
#include "esy_argument.h"
#include <QCoreApplication>
#include <QTime>

class ChildProcess
{
public:
	ProcessClass * process;
	int progress;
	time_t last_time;
	time_t speed;
	QString command;
public:
	ChildProcess() :
		process(), progress( 0 ), last_time( 0 ), speed( 0 ), command()
	{}
};


class ParallelClass : public QObject
{
	Q_OBJECT
private:
	QList< ChildProcess > mp_process_dialog_list;
	int m_runIndex;
	int m_finishIndex;
	long m_processCount;
	long m_available_pc;
	QString m_outputFileName;

	bool m_log_enable;
	QString m_log_folder;

	QTime m_time;
public:
	ParallelClass( int argc, char *argv[] );
	void run();
signals:
	void indexProcessChanged(int pro, int index);
private slots:
	void addProcessDialogs();
	void processProgessUpdate( int index, int pro );
	void processFinished( int index, QProcess::ExitStatus exitStatus );
	void ProcessError( int index );
};

#endif // PARALLELCLASS_H

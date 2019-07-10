#ifndef PROCESSCLASS_H
#define PROCESSCLASS_H

#include <QObject>
#include <QProcess>
#include <iostream>
#include <QFile>
#include <QTextStream>

using namespace std;

class ProcessClass : public QObject
{
	Q_OBJECT

public:
	ProcessClass( int index, const QString & direction, const QString & command,
				bool log = false, const QString & logdir = "" );
	~ProcessClass();

public slots:
	void start();

signals:
	void processProgressChanged( int index, int pro );
	void processFinished( int index, QProcess::ExitStatus exitStatus );
	void processCerr( int );

private:
	QProcess * mp_process;

	int m_process_index;
	QString m_direction_string;
	QString m_command_string;
	bool m_log_enable;
	QString m_log_folder;
	QStringList m_args_string_list;
	QStringList m_outputs_string;

	QFile m_log_file;
	QTextStream m_log_stream;

public:
	QStringList outputString() const { return m_outputs_string; }

private slots:
	void readProcessCout();
	void readProcessCerr();
	void processFinish( int exitCode, QProcess::ExitStatus exitStatus );
};


#endif // PROCESSCLASS_H

#include "processclass.h"

ProcessClass::ProcessClass( int index, const QString & direction,
	const QString & command, bool log, const QString & logdir
) :
	m_process_index( index ), m_direction_string( direction ),
	m_command_string( command ),
	m_log_enable( log ), m_log_folder( logdir )
{
	mp_process = new QProcess;
	mp_process ->setWorkingDirectory( m_direction_string );
	connect( mp_process, SIGNAL( readyReadStandardOutput() ),
			this, SLOT( readProcessCout() ) );
	connect( mp_process, SIGNAL( readyReadStandardError() ),
			this, SLOT( readProcessCerr() ) );
	connect( mp_process, SIGNAL( finished(int,QProcess::ExitStatus) ),
			this, SLOT( processFinish(int, QProcess::ExitStatus) ) );
}

ProcessClass::~ProcessClass()
{
	if( mp_process == 0 )
	{
		return;
	}
	if( mp_process->state() != QProcess::NotRunning )
	{
		mp_process->close();
		mp_process->waitForFinished( 1000 );
	}
	delete mp_process;
}

void ProcessClass::start()
{
	if ( m_log_enable )
	{
		QString logfilename = m_log_folder + "/process" +
				QString::number(m_process_index) + ".log";
		m_log_file.setFileName( logfilename );
		if ( m_log_file.open( QIODevice::WriteOnly ) )
		{
			m_log_stream.setDevice( &m_log_file );
		}
	}

	if ( m_command_string == "" )
	{
		cerr<< "Error : You must set ready before starting!" << endl;
		return;
	}
	QString comm;
	if ( m_direction_string.isEmpty() )
	{
		comm = m_command_string;
	}
	else
	{
		comm = m_direction_string + "/" + m_command_string;
	}
	mp_process->start( comm );
	if ( m_log_enable )
	{
		m_log_stream << "command: " << comm << endl;
		m_log_stream << "log: " << m_log_file.fileName() << endl;
		m_log_stream.flush();
	}
	emit processProgressChanged( m_process_index, 0 );
}

void ProcessClass::readProcessCout()
{
	QString output;
	output = mp_process ->readAllStandardOutput();
	if ( m_log_enable )
	{
		m_log_stream << output;
		m_log_stream.flush();
	}

	QStringList list = output.split( "\n" );
	foreach( QString out, list )
	{
		QRegExp pp_rx( "PP(\\d+)" );
		if ( out.indexOf( pp_rx ) >= 0 )
		{
			emit processProgressChanged( m_process_index, pp_rx.cap( 1 ).toInt() );
		}
		QRegExp rr_rx( "RR(((-?\\d+)(\\.\\d+)?)(\\|((-?\\d+)(\\.\\d+)?))*)" );
		if ( out.indexOf( rr_rx ) >= 0 )
		{
			m_outputs_string.append(rr_rx.cap( 1 ).split( "|" ) );
		}
	}
}

void ProcessClass::readProcessCerr()
{
	QString error;
	error = mp_process ->readAllStandardError();
	cerr << "Error : Process " <<
		m_process_index << " has error output:" << endl;
	cerr << ">> " << error.toStdString() << endl;
	emit processCerr( m_process_index );
}

void ProcessClass::processFinish( int exitCode, QProcess::ExitStatus exitStatus )
{
	Q_UNUSED( exitCode )

	if ( m_log_enable )
	{
		m_log_file.close();
	}
	if ( exitStatus == QProcess::CrashExit )
	{
		cerr << "Process Crashed : Process " <<
			m_process_index << " Crashed!" << endl;
	}
	emit processFinished( m_process_index, exitStatus );
}


#include "parallelclass.h"

ParallelClass::ParallelClass( int argc, char *argv[] ) :
	m_processCount( 0 ), m_available_pc( 0 )
{
	string a_arg_file_path = "";
	string a_out_file_path = "";
	bool a_log_enable = false;
	string a_log_file_dir = ".";

	EsyArgumentList a_arg;
	a_arg.setHead( "Parallel Tools based on Qt." );
	a_arg.setVersion( "1.0" );
	a_arg.insertLong( "-pc_num_avail",
		"Available process number, must greater than 0", &m_available_pc );
	a_arg.insertString( "-arg_file_path",
		"File path of argument list file", &a_arg_file_path );
	a_arg.insertString( "-out_file_path",
		"Output file path", &a_out_file_path );
	a_arg.insertBool( "-log_enable",
		"Enable log file for each processes", &a_log_enable );
	a_arg.insertString( "-log_file_dir",
		"Log file folder direction", &a_log_file_dir );

	// check input argument
	if ( a_arg .analyse( argc, argv ) != EsyArgumentList::RESULT_OK )
		qApp->exit();
	if ( m_available_pc == 0 || a_arg_file_path == "" )
	{
		cout << "Error arguments!" << endl;
		a_arg.printHelp2Console( cout );
		qApp ->exit();
	}

	QString arg_file_path;
	arg_file_path = QString::fromStdString(a_arg_file_path);

	m_outputFileName = QString::fromStdString( a_out_file_path );

	// read cfgfile
	QFile inFile( arg_file_path );
	if ( !inFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		cout<< "Warning : Can't open argument file " <<
			arg_file_path .toStdString() << endl;
		qApp->exit();
	}
	QTextStream cfgStream( &inFile );

	// create process pool
	while( !cfgStream.atEnd() )
	{
		ChildProcess temp;
		temp.command = cfgStream.readLine();
		if( temp.command.isNull() )
			break;

		cout << "process " << m_processCount << " : ";
		if ( temp.command.length() > 10 )
		{
			cout << temp.command.left(10).toStdString() << "..." << endl;
		}
		else
		{
			cout << temp.command.toStdString() << endl;
		}
		mp_process_dialog_list.append( temp );
		m_processCount ++;
	}
	inFile.close();
	cout << "pc_num: " << m_processCount << endl;

	m_log_enable = a_log_enable;
	m_log_folder = QString::fromStdString( a_log_file_dir );
}

void ParallelClass::addProcessDialogs()
{
	for ( int t_process_index = 0;
		t_process_index < m_processCount; t_process_index ++ )
	{
		ProcessClass * temp = new ProcessClass( t_process_index,
			"", mp_process_dialog_list[ t_process_index ].command,
			m_log_enable, m_log_folder );
		mp_process_dialog_list[ t_process_index ].process = temp;

		connect( temp, SIGNAL( processProgressChanged(int,int) ),
				 this, SLOT( processProgessUpdate(int,int) ) );
		connect( temp, SIGNAL( processFinished(int,QProcess::ExitStatus) ),
				 this, SLOT( processFinished(int,QProcess::ExitStatus) ) );
		connect( temp, SIGNAL( processCerr(int) ),
				 this, SLOT( ProcessError(int) ) );
	}

	for ( m_runIndex = 0; m_runIndex < qMin( m_available_pc, m_processCount );
		  m_runIndex ++ )
	{
		mp_process_dialog_list[ m_runIndex ] .process ->start();
		cout << "Start Process " << m_runIndex << endl;
		cout << "Argument : " <<
			mp_process_dialog_list[ m_runIndex ].command.toStdString() << endl;
	}
	m_finishIndex = 0;
}

void ParallelClass::processProgessUpdate( int index, int pro )
{
	mp_process_dialog_list[ index ] .progress = pro;
}

void ParallelClass::ProcessError( int index )
{
	cout << "Process " << index << " Error.";
}

void ParallelClass::processFinished( int index, QProcess::ExitStatus exitStatus )
{
	if ( exitStatus == QProcess::NormalExit )
	{
		cout << "Process " << index << " Finished Normally." << endl;
	}
	else
	{
		cout << "Process " << index << " Crashed." << endl;
	}

	if ( m_runIndex < m_processCount )
	{
		mp_process_dialog_list[ m_runIndex ].process ->start();
		cout << "Start Process " << m_runIndex << endl;
		cout << "Argument : " <<
			mp_process_dialog_list[ m_runIndex ].command.toStdString() << endl;
		m_runIndex ++;
	}

	m_finishIndex ++ ;
	if ( m_finishIndex == m_processCount )
	{
		QFile outFile( m_outputFileName );
		if ( !outFile.open( QIODevice::WriteOnly ) )
		{
			cout<< "Warning : Can't open result file " <<
				m_outputFileName.toStdString() << endl;
			return;
		}

		QTextStream outStream( &outFile );
		for ( int t_process_index = 0;
			t_process_index < mp_process_dialog_list.size(); t_process_index ++ )
		{
			ProcessClass * t_process =
				mp_process_dialog_list[ t_process_index ].process;
			for ( int j = 0; j < t_process->outputString().size(); j ++ )
			{
				outStream << t_process->outputString()[ j ] << "\t";
				cout << t_process->outputString()[ j ].toStdString() << "\t";
			}
			outStream << endl;
			cout << endl;
		}
		outFile.close();

		cout << "Finish Time: " <<
			QTime::currentTime().toString().toStdString() << endl;
		cout << "Duration: " <<  m_time.elapsed()/1000.0 << "seconds" << endl;
		qApp ->exit();
	}
}

void ParallelClass::run()
{
	m_time.start();
	cout << "Start Time: " <<
		QTime::currentTime().toString().toStdString() << endl;

	addProcessDialogs();
}

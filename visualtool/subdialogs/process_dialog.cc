#include "../define/qt_include.h"
#include "process_dialog.h"

ProcessDialog::ProcessDialog( int index, const QString & direction,
                              const QString & command, bool trace_enable ) :
    m_process_index( index ), m_direction_string( direction ), m_command_string( command ),
    m_trace_enable( trace_enable ), m_wait_for_syn( false ), m_finished( false ),
    m_progress( 0 )
{
    m_result_type = RESULT_RENEW_TOOLS;

	mp_progress_bar = new QProgressBar;
    mp_progress_bar->setRange( 0,100 );
    mp_progress_bar->setValue( 0 );

    mp_exec_text_edit = new QTextEdit( direction + "/" + command );
    mp_cout_text_edit = new QTextEdit();
    mp_cout_text_edit->setReadOnly( true );
    mp_exec_text_edit->setReadOnly( true );

    QHBoxLayout* top_layout = new QHBoxLayout;
    top_layout->addWidget( mp_progress_bar );
    top_layout->addWidget( mp_exec_text_edit );
    top_layout->setStretch(0, 1);
    top_layout->setStretch(1, 3);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout( top_layout );
    layout->addWidget( mp_cout_text_edit );
    layout->setStretch(0, 1);
    layout->setStretch(1, 3);

    setLayout( layout );

    mp_process = new QProcess;
    mp_process ->setWorkingDirectory( m_direction_string );
    connect( mp_process, SIGNAL( readyReadStandardOutput() ), this, SLOT( readProcessCout() ) );
    connect( mp_process, SIGNAL( readyReadStandardError() ), this, SLOT( readProcessCerr() ) );
    connect( mp_process, SIGNAL( finished(int,QProcess::ExitStatus) ), this, SLOT( processFinish(int, QProcess::ExitStatus) ) );
}

ProcessDialog::~ProcessDialog()
{
	if( mp_process == 0 )
		return;
	if( mp_process ->state() != QProcess::NotRunning )
	{
		mp_process ->close();
		mp_process ->waitForFinished( 1000 );
	}
	delete mp_process;
}

void ProcessDialog::start()
{
    if ( m_command_string == "" )
	{
		QMessageBox::warning(this, "Error", "You must set ready before starting!");
		return;
	}
	if ( m_direction_string .isEmpty() )
	{
		mp_process ->start( m_command_string );
	}
	else
	{
		mp_process ->start( m_direction_string + "/" + m_command_string );
	}
    emit processProgressChanged( m_process_index, 0 );
}

void ProcessDialog::readProcessCout()
{
    QString output = mp_process ->readAllStandardOutput();
    QStringList list = output .split( "\n" );
    QString output_after_re = "";
    foreach( QString out, list )
	{
        bool matched = false;
        QRegExp pp_rx( "PP(\\d+)" );
        if ( out .indexOf( pp_rx ) >= 0 )
		{
            m_progress = pp_rx.cap( 1 ).toInt();
            mp_progress_bar ->setValue(m_progress);
            matched = true;
            emit processProgressChanged(m_process_index, m_progress);
		}
        QRegExp rr_rx( "RR(((-?\\d+)(\\.\\d+)?)(\\|((-?\\d+)(\\.\\d+)?))*)" );
        if ( out .indexOf( rr_rx ) >= 0 )
		{
            m_outputs_string .append( rr_rx .cap( 1 ) .split( "|" ) );
		}
        QRegExp tr_rx( "TR(.*)" );
        if ( out.indexOf( tr_rx ) >= 0 )
        {
            matched = true;
            if (m_trace_enable)
            {
                EsyDataItemEventtrace t_item;
                t_item.readItem( tr_rx.cap(1).toStdString() );
                m_eventtrace_list.append( t_item );
            }
        }
        QRegExp re_rx( "RE(.*)" );
        if ( out.indexOf( re_rx ) >= 0 )
        {
            matched = true;
            if (m_trace_enable)
            {
                EsyDataItemSoCRecord t_item;
                t_item.readItem( re_rx.cap(1).toStdString() );
                m_socrecord_list.append( t_item );
            }
        }
        QRegExp sy_rx( "SY" );
        if ( out.indexOf( sy_rx ) >= 0 )
        {
            matched = true;
            m_wait_for_syn = true;
            if (m_eventtrace_list.size() < 10000 &&
                m_socrecord_list.size() < 10000)
            {
                synchornizeNext();
            }
        }
        if (!matched)
        {
            if ( out.length() > 0 )
            {
                output_after_re = output_after_re + out + '\n';
            }
        }
    }
    if (output_after_re.length() > 0)
    {
        mp_cout_text_edit->setText( mp_cout_text_edit->toPlainText() + output_after_re );
        QTextCursor cursor = mp_cout_text_edit->textCursor();
        cursor .movePosition( QTextCursor::End );
        mp_cout_text_edit->setTextCursor( cursor );
    }
}

void ProcessDialog::readProcessCerr()
{
	QString error;
    error = mp_process ->readAllStandardError();
    mp_cout_text_edit ->setText( mp_cout_text_edit ->toPlainText() + error );
	QTextCursor cursor = mp_cout_text_edit ->textCursor();
	cursor .movePosition( QTextCursor::End );
	mp_cout_text_edit ->setTextCursor( cursor );
    QMessageBox::warning( this, "Error",
        QString( "Process %1 has error output:\n%2" ).arg( m_process_index ).arg( error ) );
    emit processCerr( m_process_index );
}

void ProcessDialog::processFinish( int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED( exitCode )
    m_finished = true;

    if ( exitStatus == QProcess::CrashExit )
        QMessageBox::warning( this, "Process Crashed",
            QString( "Process %1 Crashed!" ).arg( m_process_index ) );
    emit processFinished( m_process_index, exitStatus );
}

void ProcessDialog::synchornizeNext()
{
    mp_process->write( "n\n", 2 );
    m_wait_for_syn = false;
}

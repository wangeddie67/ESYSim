#include "../define/qt_include.h"
#include "tools_para_dialog.h"
#include "../subwidgets/tools_config_dialog.h"
#include "../modelsview/analyse_result_table.h"

ToolsParaDialog::ToolsParaDialog( QString fname )
{
    m_result_type = RESULT_TOOLS_PARA;

    mp_tools_config_dialog = new ToolsConfigDialog( fname );

    mp_scrollarea = new QScrollArea( this );
    mp_scrollarea ->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    mp_toolbox = new QToolBox( this );
    mp_toolbox ->addItem( mp_tools_config_dialog, QIcon( QPixmap( "images/link.png" ) ), "Configure Tool" );
    mp_scrollarea ->setWidget( mp_toolbox );
    mp_scrollarea ->setWidgetResizable( true );
    mp_tools_config_dialog ->parentWidget() ->parentWidget() ->setMinimumHeight(500);

    QHBoxLayout layout( this );
    layout .addWidget( mp_scrollarea );
    mp_scrollarea ->resize( width(), height() );

    connect( mp_tools_config_dialog, SIGNAL( accepted() ), this, SLOT(addProcessDialogs()) );
}

void ToolsParaDialog::resizeEvent( QResizeEvent *event )
{
    mp_scrollarea ->resize( event ->size() );
    QDialog::resize( event->size() );
}

void ToolsParaDialog::addProcessDialogs()
{
    m_processCount = mp_tools_config_dialog ->processCount();
    for ( int t_process_index = 0; t_process_index < m_processCount; t_process_index ++ )
	{
        ProcessDialog * temp = new ProcessDialog( t_process_index,
            mp_tools_config_dialog ->processDirection(), mp_tools_config_dialog ->processCommand( t_process_index ) );
        mp_process_dialog_list .append(temp);
        mp_toolbox ->addItem( temp, QIcon( QPixmap( "images/pause.png" ) ), QString( "Process " ) + QString::number( t_process_index ) );
        temp ->parentWidget() ->parentWidget() ->setMinimumHeight(150);
        connect( temp, SIGNAL( processProgressChanged(int,int) ), this, SLOT( processProgessUpdate(int,int) ) );
        connect( temp, SIGNAL( processFinished(int,QProcess::ExitStatus) ), this, SLOT( processFinished(int,QProcess::ExitStatus) ) );
        connect( temp, SIGNAL( processCerr(int) ), this, SLOT( ProcessError(int) ) );
	}

    for ( m_runIndex = 0; m_runIndex < qMin( mp_tools_config_dialog ->availableNum(), m_processCount ); m_runIndex ++ )
    {
        mp_process_dialog_list[ m_runIndex ] ->start();
    }
    m_finishIndex = 0;
}

void ToolsParaDialog::addResultDialog()
{
    ResultTableView * result_table = new ResultTableView( this );
    result_table->horizontalHeader()->setHidden( true );
    result_table->verticalHeader()->setHidden( false );

    ResultTableModel * result_model = new ResultTableModel( this );
    for ( int i = 0; i < mp_process_dialog_list.size(); i ++ )
    {
        result_model->appendDataItem(
                    mp_process_dialog_list[ i ]->outputString() );
        result_model->setHeaderList(
                    mp_process_dialog_list[ i ]->outputString() );
    }
    result_table->setModel( result_model );

    mp_toolbox->addItem( result_table, QString( "Result Table" ) );
    result_table->parentWidget()->parentWidget()->setMinimumHeight( 200 );
}

void ToolsParaDialog::processProgessUpdate( int index, int pro )
{
    mp_toolbox ->setItemIcon( index + 1,
                              QIcon( QPixmap( "images/play.png" ) ) );
    mp_toolbox ->setItemText( index + 1,
                 QString( "Process %1 (%2%)" ).arg( index ).arg( pro ) );
}

void ToolsParaDialog::ProcessError( int index )
{
    mp_toolbox ->setItemIcon( index + 1, QIcon( QPixmap( "images/close.png" ) ) );
}

void ToolsParaDialog::processFinished( int index, QProcess::ExitStatus exitStatus )
{
    if ( exitStatus == QProcess::NormalExit )
    {
        mp_toolbox ->setItemIcon(
                    index + 1, QIcon( QPixmap( "images/check.png" ) ) );
    }
    else
    {
        mp_toolbox ->setItemIcon(
                    index + 1, QIcon( QPixmap( "images/close.png" ) ) );
    }

    if ( m_runIndex < m_processCount )
	{
        mp_process_dialog_list[ m_runIndex ] ->start();
        m_runIndex ++;
	}

    m_finishIndex ++ ;
    if ( m_finishIndex == m_processCount )
	{
        addResultDialog();

        QFile outFile( mp_tools_config_dialog ->getOutputFileName() );
        if ( !outFile.open( QIODevice::WriteOnly ) )
		{
            QMessageBox::warning( this, "Warning",
                QString( "Can't open file " ) +
                    mp_tools_config_dialog->getOutputFileName() );
			return;
		}
        QTextStream outStream( &outFile );
        for ( int t_process_index = 0;
              t_process_index < mp_process_dialog_list .size();
              t_process_index ++ )
		{
            for ( int j = 0;
                j < mp_process_dialog_list[
                    t_process_index ]->outputString().size();
                j ++ )
            {
                outStream << mp_process_dialog_list[
                    t_process_index ]->outputString()[ j ] << "\t";
            }
            outStream << endl;
		}
		outFile.close();
	}
}

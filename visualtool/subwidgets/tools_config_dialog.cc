#include "tools_config_dialog.h"
#include <QRegExp>
#include <QGroupBox>

// ---- constructor and destructor ----//
ToolsConfigDialog::ToolsConfigDialog(QString fname, bool onlyone)
{
    m_onlyone_model = onlyone;
	// add components
	addComponents();
	// read argument file
    m_cfg_file_dir = fname;

    mp_argu_model = new ArgumentListModel( m_onlyone_model, this );
    ErrorHandler t_error = mp_argu_model->readArgumentFile( fname );
    if ( t_error.hasError() )
    {
        QMessageBox::warning(this, t_error.title(), t_error.text(),
                             QMessageBox::Ok );
        return;
    }
    mp_argu_table->setModel( mp_argu_model );
    changeItemHandler();
    connect( mp_argu_model, SIGNAL( dataChanged(QModelIndex,QModelIndex) ),
             this, SLOT( changeItemHandler() ) );
}

ToolsConfigDialog::~ToolsConfigDialog()
{
}
// ---- constructor and destructor ----//

// ---- components initialization functions ---- //
// add components
void ToolsConfigDialog::addComponents()
{
	// add compoents
	// tree widget
    mp_argu_table = new ArgumentListTableView( m_onlyone_model, this );

    // command table
    mp_command_tablewidget = new QTableWidget( 0, 1, this );
    mp_command_tablewidget->horizontalHeader()->hide();
    mp_command_tablewidget->setAlternatingRowColors( true );
    mp_command_tablewidget->horizontalHeader()->
            setSectionResizeMode( QHeaderView::Stretch );
    mp_command_tablewidget->verticalHeader()->
            setSectionResizeMode( QHeaderView::ResizeToContents );
    connect( mp_command_tablewidget->horizontalHeader(),
             SIGNAL( sectionResized(int,int,int) ),
             mp_command_tablewidget, SLOT( resizeRowsToContents() ) );
    mp_command_label = new QLabel( "0 Command", this );
    mp_command_label ->setBuddy( mp_command_tablewidget );

    if (!m_onlyone_model)
    {
        // insert loop button
        mp_insert_loop_pushbutton = new QPushButton("Insert Loop Variable");
        connect( mp_insert_loop_pushbutton, SIGNAL( clicked() ),
                 this, SLOT( insertLoopVarHandler() ) );

        // parallel count
        mp_process_spinbox = new QSpinBox( this );
        mp_process_spinbox ->setMinimum( 1 );
        mp_process_spinbox ->setValue( 1 );
        mp_process_label = new QLabel( "Process Count", this );
        mp_process_label ->setBuddy( mp_process_spinbox );
        connect( mp_process_spinbox, SIGNAL( valueChanged(const QString &) ),
                 this, SLOT( changeItemHandler() ) );
    }

    // ok button
    mp_ok_pushbutton = new QPushButton( "OK", this );
    mp_ok_pushbutton-> setFixedSize( 100, 30 );
    connect( mp_ok_pushbutton, SIGNAL( clicked() ),
             this, SLOT( okButtonHandler() ) );

	// layout
	// horizontal layout
    mp_button_layout = new QHBoxLayout();
    mp_button_layout->addStretch();
    mp_button_layout->addWidget( mp_ok_pushbutton );
    if (!m_onlyone_model)
    {
        // result file edit
        QLabel* result_file_label = new QLabel( "Result Output File:" );
        mp_result_file_lineedit = new FileLineEdit( FileLineEdit::NEW_FILE,
            "results.xls", global_work_direction,
            "excel file (*.xls);;All file (*.*)" );

        // horizontal layout
        QHBoxLayout* process_spinbox_layout = new QHBoxLayout();
        process_spinbox_layout->addWidget( mp_process_label );
        process_spinbox_layout->addWidget( mp_process_spinbox );
        // temp horizontal layout
        // vertical layout
        mp_parallel_layout = new QVBoxLayout();
        mp_parallel_layout->addWidget( mp_insert_loop_pushbutton );
        mp_parallel_layout->addLayout( process_spinbox_layout );
        mp_parallel_layout->addWidget( result_file_label );
        mp_parallel_layout->addWidget( mp_result_file_lineedit );
        mp_parallel_layout->addStretch();
    }
	// vertical layout
    QVBoxLayout* command_layout = new QVBoxLayout();
    command_layout->addWidget( mp_command_label );
    command_layout->addWidget( mp_command_tablewidget );
	// horizontal layout
    QHBoxLayout* config_layout = new QHBoxLayout();
    if (!m_onlyone_model)
    {
        config_layout->addLayout( mp_parallel_layout, 2 );
    }
    config_layout->addLayout( command_layout, 9 );
	// vertical layout
    mp_table_layout = new QVBoxLayout();
    mp_table_layout ->addWidget( mp_argu_table );
    mp_table_layout ->addLayout( config_layout );
    mp_table_layout ->addLayout( mp_button_layout );
	
    setLayout( mp_table_layout );
	
    setWindowTitle( "Arguments Configuration" );
	
    resize( 1000, 500 );
}

void ToolsConfigDialog::resizeEvent( QResizeEvent * )
{
    mp_argu_table->updateColumnWidth();
}

// ---- slots functions ---- //
// push ok button, check result before close
void ToolsConfigDialog::okButtonHandler()
{
    mp_argu_model->writeArgumentFile( m_cfg_file_dir );
    mp_command_tablewidget->setEditTriggers( QAbstractItemView::NoEditTriggers );
    mp_ok_pushbutton->setEnabled(false);

    if (isModal())
    {
        accept();
    }
    else
    {
        emit accepted();
    }
}

// update command when items changed
void ToolsConfigDialog::changeItemHandler()
{
    // sort of loop level > 0, first is item seq, second is loop level
    QList< QPair< int, LoopLevelClass > > looplevelsort;
    // loop level list
    int total_commands = 1;
    for ( int t_argu_index = 0; t_argu_index < mp_argu_model->rowCount();
          t_argu_index ++ )
	{
        ArgumentListModelItem * t_item = mp_argu_model->arguItem( t_argu_index );
        if ( t_item->loop() > 0 )
        {
            QString valuetext = t_item->value();
            QRegExp seq_re( "\\(seq (\\d+) (\\d+) (\\d+)\\)" );
            QStringList loopvaluestring;
            if ( valuetext.indexOf( seq_re ) >= 0 )
            {
                for ( int i = seq_re.cap( 1 ).toInt();
                      i <= seq_re.cap( 3 ).toInt();
                      i = i + seq_re.cap( 2 ).toInt() )
                {
                    loopvaluestring.append( QString::number( i ) );
                }
            }
            else
            {
                loopvaluestring = valuetext.trimmed().split( " " );
            }
            looplevelsort.append( QPair<int, LoopLevelClass>( t_argu_index,
                LoopLevelClass( t_item->loop(),
                    loopvaluestring.count(), loopvaluestring ) ) );

            total_commands *= loopvaluestring .count();
        }
    }
    // qsort
    qSort( looplevelsort.begin(), looplevelsort.end(), loopLevelLessThan );
    // command level
    mp_command_label->setText( QString( "%1 commands" ).arg( total_commands ) );
    // command table widget
    mp_command_tablewidget ->setRowCount( total_commands );
    for ( int t_command_index = 0; t_command_index < total_commands;
          t_command_index ++ )
    {
        if ( mp_command_tablewidget ->item( t_command_index, 0 ) == 0 )
        {
            mp_command_tablewidget ->setItem( t_command_index, 0,
                new QTableWidgetItem( "" ) );
        }
    }

    for ( int t_command_index = 0; t_command_index < total_commands;
          t_command_index ++ )
    {
        QString command = mp_argu_model->exec();
        // build command
        for ( int t_argu_index = 0; t_argu_index < mp_argu_model->rowCount();
              t_argu_index ++ )
        {
            ArgumentListModelItem * t_item =
                    mp_argu_model->arguItem( t_argu_index );
            if ( t_item->opt().isEmpty() )
            {
               continue;
            }
			if ( !t_item->isShown() )
			{
				continue;
			}

            if ( t_item->optType() == EsyArgumentItem::BOOL_TYPE )  // bool type
            {
                if ( t_item->value().toInt() )
                {
                    command += " " + t_item->opt();
                }
            }
            else    // text type
            {
                command += " " + t_item->opt();
                int t_level_index;
                for ( t_level_index = 0; t_level_index < looplevelsort.count();
                      t_level_index ++ )
                {
                    if ( looplevelsort[ t_level_index ].first == t_argu_index )
                    {
                        command += " " +
                            looplevelsort[ t_level_index ].second.currentString();
                        break;
                    }
                }
                if ( t_level_index == looplevelsort.count() )
                {
                    command += " " + t_item->value();
                }
            }
        }
        // replace $d
        QRegExp replace_re( "\\$(\\d+)" );
        for ( int i = 0; i < 10; i ++ )
        {
            if ( command.indexOf( replace_re ) < 0 )
            {
                break;
            }
            int replace_index = replace_re.cap( 1 ) .toInt();
            if ( replace_index >= mp_argu_model->rowCount() )
            {
                break;
            }
            int t_level_index;
            for ( t_level_index = 0; t_level_index < looplevelsort.count();
                  t_level_index ++ )
            {
                if ( looplevelsort[ t_level_index ] .first == replace_index )
                {
                    command.replace( QString( "$%1" ).arg( replace_index ),
                        looplevelsort[ t_level_index ].second.currentString() );
                    break;
                }
            }
            if ( t_level_index == looplevelsort .count() )
            {
                ArgumentListModelItem * t_item =
                        mp_argu_model->arguItem( replace_index );
                command.replace( QString( "$%1" ).arg( replace_index ),
                                 t_item->value() );
            }
        }
        mp_command_tablewidget->item( t_command_index, 0 )->setText( command );
        for ( int i = 0; i < looplevelsort .count(); i ++ )
        {
            if ( looplevelsort[ i ] .second .increase() == false )
            {
                break;
            }
        }
   }
}

void ToolsConfigDialog::insertLoopVarHandler()
{
    mp_argu_model->appendItem( new ArgumentListModelItem( "",
        EsyArgumentItem::STRING_TYPE, "", "", 0 ) );
}
// ---- slots functions ---- //

// sort funtcions
bool loopLevelLessThan( const QPair<int, LoopLevelClass> &s1,
                        const QPair<int, LoopLevelClass> &s2 )
{
    return ( s1.second.m_level < s2.second.m_level );
}

// **** TrafficGeneratorDialog ****//


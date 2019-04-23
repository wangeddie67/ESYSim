#include "automation_control_panel.h"

#define AS_TIMER	1000

AutomationControlPanel::AutomationControlPanel(
    ControlType type, QString stepunit, QString jumpunit, QWidget *parent) :
    ToolBarBox("Control Panel", -1, parent), m_jump_flag( false )
{
    mp_load_pd = new QProgressBar();
    mp_load_pd->setFixedWidth( 200 );

    mp_cycle_label = new QLabel( "", this );

    mp_step_label = new QLabel( "Step", this );
    mp_step_edit = new QLineEdit( QString::number( AS_TIMER ), this );
    mp_step_edit->setAlignment( Qt::AlignRight );
    mp_step_unit_label = new QLabel( stepunit, this );

    mp_prevrun_button = new QPushButton( "<<", this );
    mp_prevrun_button->setEnabled( false );
    mp_prevrun_button->setFixedWidth( 40 );
    mp_previous_button = new QPushButton( "<", this );
    mp_previous_button->setEnabled( false );
    mp_previous_button->setFixedWidth( 40 );
    mp_stop_button = new QPushButton( "||", this );
    mp_stop_button->setEnabled( false );
    mp_stop_button->setFixedWidth( 40 );
    mp_next_button = new QPushButton( ">", this );
    mp_next_button->setEnabled( false );
    mp_next_button->setFixedWidth( 40 );
    mp_nextrun_button = new QPushButton( ">>", this );
    mp_nextrun_button->setEnabled( false );
    mp_nextrun_button->setFixedWidth( 40 );
    connect( mp_prevrun_button, SIGNAL(clicked()), this,
             SLOT( prevRunButtonClickedHandler() ) );
    connect( mp_previous_button, SIGNAL(clicked()), this,
             SLOT( previousButtonClickedHandler() ) );
    connect( mp_stop_button, SIGNAL(clicked()), this,
             SLOT( stopButtonClickedHandler() ) );
    connect( mp_next_button, SIGNAL(clicked()), this,
             SLOT( nextButtonClickedHandler() ) );
    connect( mp_nextrun_button, SIGNAL(clicked()), this,
             SLOT( nextrunButtonClickedHandler() ) );

    connect( mp_prevrun_button, SIGNAL(clicked()), this,
             SIGNAL( prevRunButtonClicked() ) );
    connect( mp_previous_button, SIGNAL(clicked()), this,
             SIGNAL( previousButtonClicked() ) );
    connect( mp_stop_button, SIGNAL(clicked()), this,
             SIGNAL( stopButtonClicked() ) );
    connect( mp_next_button, SIGNAL(clicked()), this,
             SIGNAL( nextButtonClicked() ) );
    connect( mp_nextrun_button, SIGNAL(clicked()), this,
             SIGNAL( nextrunButtonClicked() ) );

    mp_jump_label = new QLabel( "Jump to", this );
    mp_jump_edit = new QLineEdit( "0", this );
    mp_jump_edit->setAlignment( Qt::AlignRight );
    mp_jump_button = new QPushButton( "Jump", this );
    mp_jump_button->setEnabled( false );
    mp_jump_button->setFixedWidth( 40 );
    connect( mp_jump_button, SIGNAL(clicked()), this,
             SLOT( jumpButtonClickedHandler() ) );
    connect( mp_jump_button, SIGNAL(clicked()), this,
             SIGNAL( jumpButtonClicked() ) );
    mp_jump_unit_label = new QLabel( jumpunit, this );

    mp_analyse_button = new QPushButton( "Analyse", this );
    mp_analyse_button->setEnabled( false );
    mp_analyse_button->setFixedWidth( 100 );
    connect( mp_analyse_button, SIGNAL( clicked() ), this,
             SLOT( analyseButtonClickedHandler() ) );
    connect( mp_analyse_button, SIGNAL( clicked() ), this,
             SIGNAL( analyseButtonClicked() ) );

    QHBoxLayout * mp_step_layout = new QHBoxLayout();
    mp_step_layout->addWidget( mp_step_label );
    mp_step_layout->addWidget( mp_step_edit );
    mp_step_layout->addWidget( mp_step_unit_label );

    QHBoxLayout * mp_button_layout = new QHBoxLayout();
    mp_button_layout->addWidget(mp_prevrun_button);
    mp_button_layout->addWidget(mp_previous_button);
    mp_button_layout->addWidget(mp_stop_button);
    mp_button_layout->addWidget(mp_next_button);
    mp_button_layout->addWidget(mp_nextrun_button);
    mp_button_layout->addWidget(mp_analyse_button);
    mp_button_layout->setAlignment( mp_analyse_button, Qt::AlignRight );
    mp_button_layout->setSpacing( 0 );

    QHBoxLayout * mp_jump_layout = new QHBoxLayout();
    mp_jump_layout->addWidget( mp_jump_label );
    mp_jump_layout->addWidget( mp_jump_edit );
    mp_jump_layout->addWidget( mp_jump_unit_label );
    mp_jump_layout->addWidget( mp_jump_button );

    QGridLayout * main_layout = new QGridLayout();
    main_layout->setMargin( 0 );
    main_layout->setSpacing( 5 );

    main_layout->addWidget(mp_load_pd, 0, 0, 1, 1);
    main_layout->addLayout(mp_button_layout, 1, 0, 1, 1);
    main_layout->addWidget(mp_cycle_label, 0, 1, 1, 1);
    main_layout->addLayout(mp_step_layout, 0, 2, 1, 1);
    main_layout->addLayout(mp_jump_layout, 1, 2, 1, 1);

    setControlType( type );

    mp_box_layout->addLayout( main_layout );

    connect( this, SIGNAL(analyseButtonClicked()), this,
             SIGNAL(anyButtonClicked()) );
    connect( this, SIGNAL(prevRunButtonClicked()), this,
             SIGNAL(anyButtonClicked()) );
    connect( this, SIGNAL(previousButtonClicked()), this,
             SIGNAL(anyButtonClicked()) );
    connect( this, SIGNAL(stopButtonClicked()), this,
             SIGNAL(anyButtonClicked()) );
    connect( this, SIGNAL(nextButtonClicked()), this,
             SIGNAL(anyButtonClicked()) );
    connect( this, SIGNAL(nextrunButtonClicked()), this,
             SIGNAL(anyButtonClicked()) );
    connect( this, SIGNAL(jumpButtonClicked()), this,
             SIGNAL(anyButtonClicked()) );

    initTimer();
}

void AutomationControlPanel::setControlType(ControlType type)
{
    mp_cycle_label->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_step_label->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_step_edit->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_step_unit_label->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_prevrun_button->setVisible( (type == ReplayControl) );
    mp_previous_button->setVisible( (type == ReplayControl) );
    mp_stop_button->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_next_button->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_nextrun_button->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_jump_label->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_jump_edit->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_jump_button->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_jump_unit_label->setVisible( (type == ReplayControl || type == OnlineControl) );
    mp_analyse_button->setVisible( (type == AnalyseControl) );
}

void AutomationControlPanel::enableAllComponent( bool enable )
{
    mp_step_edit->setEnabled( enable );

    mp_prevrun_button->setEnabled( enable );
    mp_previous_button->setEnabled( enable );
    mp_stop_button->setEnabled( enable );
    mp_next_button->setEnabled( enable );
    mp_nextrun_button->setEnabled( enable );

    mp_jump_button->setEnabled( enable );
    mp_jump_edit->setEnabled( enable );

    mp_analyse_button->setEnabled( enable );
}

void AutomationControlPanel::prevRunButtonClickedHandler()
{
    mp_step_edit->setEnabled( false );

    mp_prevrun_button->setEnabled( false );
    mp_previous_button->setEnabled( false );
    mp_stop_button->setEnabled( true );
    mp_next_button->setEnabled( false );
    mp_nextrun_button->setEnabled( false );

    mp_jump_button->setEnabled( false );
    mp_jump_edit->setEnabled( false );
}

void AutomationControlPanel::previousButtonClickedHandler()
{
}

void AutomationControlPanel::stopButtonClickedHandler()
{
    enableAllComponent( true );
    m_jump_flag = false;
    stopTimer();
}

void AutomationControlPanel::nextButtonClickedHandler()
{
}

void AutomationControlPanel::nextrunButtonClickedHandler()
{
    mp_step_edit->setEnabled( false );

    mp_prevrun_button->setEnabled( false );
    mp_previous_button->setEnabled( false );
    mp_stop_button->setEnabled( true );
    mp_next_button->setEnabled( false );
    mp_nextrun_button->setEnabled( false );

    mp_jump_button->setEnabled( false );
    mp_jump_edit->setEnabled( false );
}

void AutomationControlPanel::jumpButtonClickedHandler()
{
    m_jump_flag = true;

    mp_step_edit->setEnabled( true );

    mp_prevrun_button->setEnabled( false );
    mp_previous_button->setEnabled( false );
    mp_stop_button->setEnabled( true );
    mp_next_button->setEnabled( false );
    mp_nextrun_button->setEnabled( false );

    mp_jump_button->setEnabled( false );
    mp_jump_edit->setEnabled( false );
}

void AutomationControlPanel::analyseButtonClickedHandler()
{
    mp_analyse_button->setEnabled( false );
}

void AutomationControlPanel::initTimer()
{
    // set timer
    mp_nextrun_timer = new QTimer( this );
    mp_nextrun_timer->setInterval( stepValue() );
    mp_nextrun_timer->setSingleShot( true );
    connect( mp_nextrun_timer, SIGNAL( timeout() ), this, SIGNAL( nextRunTimeOut() ) );

    mp_prevrun_timer = new QTimer( this );
    mp_prevrun_timer->setInterval( stepValue() );
    mp_prevrun_timer->setSingleShot( true );
    connect( mp_prevrun_timer, SIGNAL( timeout() ), this, SIGNAL( prevRunTimeOut() ) );
}

void AutomationControlPanel::updatePrevTimer()
{
    if ( m_jump_flag )
    {
        mp_prevrun_timer->setInterval( 1 );
    }
    else
    {
        mp_prevrun_timer->setInterval( stepValue() );
    }
    mp_prevrun_timer->start();
}

void AutomationControlPanel::updateNextTimer()
{
    if ( m_jump_flag )
    {
        mp_nextrun_timer->setInterval( 1 );
    }
    else
    {
        mp_nextrun_timer->setInterval( stepValue() );
    }
    mp_nextrun_timer->start();
}

void AutomationControlPanel::stopTimer()
{
    mp_prevrun_timer->stop();
    mp_nextrun_timer->stop();
}

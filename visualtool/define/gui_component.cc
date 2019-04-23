#include "gui_component.h"

#include <QAction>
#include <QPainter>
#include <QHeaderView>
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMouseEvent>
#include <qmath.h>
#include <QApplication>

ToolBarSplitter::ToolBarSplitter( QWidget * parent ) :
    QWidget( parent )
{
    setFixedWidth( 3 );
}

void ToolBarSplitter::paintEvent( QPaintEvent * event )
{
    Q_UNUSED( event );

    QPainter painter( this );
    QPen pen = painter.pen();
    pen.setColor( Qt::darkGray );
    painter.setPen( pen );

    painter.drawLine( 1, 0, 1, size().height() );
}

ToolBarButton::ToolBarButton(
    const QIcon & icon, const QString & text, int code,
    ToolBarButton::SizeEnum size, QWidget * parent
    ):
    QWidget( parent ), m_with_menu( false )
{
    mp_main_button = new QToolButton( this );
    mp_main_button->setAutoRaise( true );
    if ( size == LargeButton )
    {
        mp_main_button->setIconSize( QSize( 45, 45 ) );
        mp_main_button->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    }
    else
    {
        mp_main_button->setIconSize( QSize( 20, 20 ) );
        mp_main_button->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    }
    connect( mp_main_button, SIGNAL( triggered(QAction*) ),
        this, SIGNAL( triggered(QAction*) ) );
    connect( mp_main_button, SIGNAL( clicked() ), this, SIGNAL( clicked() ) );

    QAction * main_action = new QAction( icon, text, this );
    main_action->setData( QVariant( code ) );
    mp_main_button->setDefaultAction( main_action );
    if ( size == LargeButton )
    {
        mp_main_button->setText( main_action->text() + "\n" );
        connect( main_action, SIGNAL( changed() ),
            this, SLOT( actionChangedHandler() ) );
    }

    QBoxLayout * layout = new QBoxLayout( QBoxLayout::LeftToRight );
    if ( size == LargeButton )
    {
        layout->setDirection( QBoxLayout::TopToBottom );
    }
    else
    {
        layout->setDirection( QBoxLayout::LeftToRight );
    }
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    layout->addWidget( mp_main_button );
    setLayout( layout );

    if ( size == LargeButton )
    {
        setFixedHeight( 72 );
    }
    else
    {
        setFixedHeight( 24 );
    }
}

ToolBarButton::ToolBarButton(
    QMenu * menu, SizeEnum size, QWidget * parent
    ):
    QWidget( parent ), m_with_menu( true )
{
    this->addActions( menu->actions() );

    mp_main_button = new QToolButton( this );
    mp_main_button->setAutoRaise( true );
    if ( size == LargeButton )
    {
        mp_main_button->setIconSize( QSize( 45, 45 ) );
    }
    else
    {
        mp_main_button->setIconSize( QSize( 20, 20 ) );
    }
    mp_main_button->setToolButtonStyle( Qt::ToolButtonIconOnly );
    connect( mp_main_button, SIGNAL( triggered(QAction*) ),
        this, SIGNAL( triggered(QAction*) ) );
    connect( mp_main_button, SIGNAL( clicked() ), this, SIGNAL( clicked() ) );

    mp_menu_button = new QToolButton( this );
    mp_menu_button->setAutoRaise( true );
    mp_menu_button->setPopupMode( QToolButton::InstantPopup );
    mp_menu_button->setToolButtonStyle( Qt::ToolButtonTextOnly );
    mp_menu_button->setObjectName( "menu_button" );

    QAction * main_action = new QAction(
        menu->actions()[ 0 ]->icon(),  menu->actions()[ 0 ]->text(), this );
    addAction( main_action );
    main_action->setData(  menu->actions()[ 0 ]->data() );
    mp_main_button->setDefaultAction( main_action );
    if ( size == LargeButton )
    {
        mp_main_button->setText( main_action->text() + "\n" );
        connect( main_action, SIGNAL( changed() ),
            this, SLOT( actionChangedHandler() ) );
    }

    mp_menu_button->setMenu( menu );
    mp_menu_button->setDefaultAction( menu->actions()[ 0 ] );
    if ( size == LargeButton )
    {
        for ( int i = 0; i < menu->actions().size(); i ++ )
        {
            connect( menu->actions()[ i ], SIGNAL( changed() ),
                this, SLOT( actionChangedHandler() ) );
        }
    }
    connect( menu, SIGNAL( triggered(QAction*) ),
        this, SLOT( setDefaultAction(QAction*) ) );

    QBoxLayout * layout = new QBoxLayout( QBoxLayout::LeftToRight );
    if ( size == LargeButton )
    {
        layout->setDirection( QBoxLayout::TopToBottom );
    }
    else
    {
        layout->setDirection( QBoxLayout::LeftToRight );
    }
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    layout->addWidget( mp_main_button );
    layout->addWidget( mp_menu_button );
    layout->setAlignment( mp_main_button, Qt::AlignHCenter );
    layout->setAlignment( mp_menu_button, Qt::AlignHCenter );
    setLayout( layout );

    if ( size == LargeButton )
    {
        setFixedHeight( 72 );
        mp_main_button->setFixedHeight( 40 );
        mp_menu_button->setFixedHeight( 32 );
        setStyleSheet( "QToolButton#menu_button::menu-indicator \
            {subcontrol-position: bottom;}" );
    }
    else
    {
        setFixedHeight( 24 );
    }
}

void ToolBarButton::setDefaultAction( QAction * action )
{
    mp_main_button->defaultAction()->setIcon( action->icon() );
    mp_main_button->defaultAction()->setText( action->text() );
    mp_main_button->defaultAction()->setData( action->data() );
    if ( m_with_menu )
    {
        mp_menu_button->setDefaultAction( action );
        mp_menu_button->setText( action->text() + "\n" );
    }
}

void ToolBarButton::actionChangedHandler()
{
    if ( m_with_menu )
    {
        mp_menu_button->setText(
                    mp_menu_button->defaultAction()->text() + "\n" );
    }
    else
    {
        mp_main_button->setText(
                    mp_main_button->defaultAction()->text() + "\n" );
    }
}

ToolBarButtonScrollArea::ToolBarButtonScrollArea(
        QList< ToolBarButton * > buttonlist, QWidget * parent ) :
    QScrollArea( parent )
{
    QVBoxLayout * button_layout = new QVBoxLayout();
    button_layout->setMargin( 0 );
    button_layout->setSpacing( 0 );

    QWidget * button_widget = new QWidget( this );
    button_widget->setLayout( button_layout );

    for ( int i = 0; i < buttonlist.size(); i ++ )
    {
        button_layout->addWidget( buttonlist[ i ] );
        button_layout->setAlignment( buttonlist[ i ], Qt::AlignLeft );
        connect( buttonlist[ i ], SIGNAL(triggered(QAction*)),
                 this, SIGNAL(triggered(QAction*)) );
    }

    button_layout->addStretch();

    setWidget( button_widget );
    setWidgetResizable( true );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );


    setFixedHeight( 72 );
}

ToolBarBox::ToolBarBox(const QString & title, int code, QWidget *parent) :
    QWidget( parent ), m_code( code )
{
    mp_box_layout = new QHBoxLayout;
    mp_box_layout->setMargin( 0 );
    mp_box_layout->setSpacing( 0 );

    QLabel * title_label = new QLabel( title, this );
    title_label->setAlignment( Qt::AlignCenter | Qt::AlignHCenter );
    title_label->setFixedHeight( 15 );

    QVBoxLayout * title_layout = new QVBoxLayout;
    title_layout->setMargin( 0 );
    title_layout->setSpacing( 0 );
    title_layout->addStretch( 0 );
    title_layout->addLayout( mp_box_layout );
    title_layout->addStretch( 0 );
    title_layout->addWidget( title_label );

    QHBoxLayout * split_layout = new QHBoxLayout;
    split_layout->setMargin( 0 );
    split_layout->setSpacing( 2 );
    split_layout->addLayout( title_layout );
    split_layout->addWidget( new ToolBarSplitter( this ) );

    setLayout( split_layout );
}

void ToolBarBox::addButtonScrollArea(ToolBarButtonScrollArea * button_list)
{
    QVBoxLayout * layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget( button_list );
    addActions( button_list->actions() );
    connect( button_list, SIGNAL( triggered(QAction*) ),
             this, SIGNAL(triggered(QAction*)) );
    layout->addStretch();
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    mp_box_layout->addLayout( layout );
    m_removeable_widget.append( button_list );
}

void ToolBarBox::addLargeButton(ToolBarButton * widget)
{
    QVBoxLayout * layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget( widget );
    layout->addStretch();
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    mp_box_layout->addLayout( layout );
    connect(widget, SIGNAL(triggered(QAction*)),
            this, SIGNAL(triggered(QAction*)) );
    addAction( widget->defaultAction() );
}

void ToolBarBox::addSmallButton(QList< ToolBarButton * > widget_list)
{
    QVBoxLayout * layout;
    for ( int i = 0; i < widget_list.size(); i ++ )
    {
        if ( i % 3 == 0 )
        {
            layout = new QVBoxLayout;
            layout->addStretch();
        }

        if ( widget_list[ i ] )
        {
            layout->addWidget( widget_list[ i ] );
            layout->setAlignment( widget_list[ i ], Qt::AlignLeft );
            connect(widget_list[ i ], SIGNAL(triggered(QAction*)),
                    this, SIGNAL(triggered(QAction*)) );
            addAction( widget_list[ i ]->defaultAction() );
        }
        else
        {
            QWidget * t = new QWidget;
            t->setFixedHeight( 24 );
            layout->addWidget( t );
            layout->setAlignment( t, Qt::AlignLeft );
        }

        if ( i % 3 == 2 || i == (widget_list.size() - 1) )
        {
            layout->addStretch();
            layout->setMargin( 0 );
            layout->setSpacing( 0 );
            mp_box_layout->addLayout( layout );
        }
    }
}

void ToolBarBox::removeWidgets()
{
    while ( m_removeable_widget.size() > 0 )
    {
        mp_box_layout->removeWidget( m_removeable_widget[ 0 ] );
        delete m_removeable_widget[ 0 ];
        m_removeable_widget.removeAt( 0 );
    }
}

ToolBarTab::ToolBarTab( const QString & name, int code, QWidget *parent) :
    QWidget( parent ), m_code( code ), m_tab_name( name )
{
    mp_tab_layout = new QHBoxLayout( this );
    mp_tab_layout->setMargin( 3 );
    mp_tab_layout->setSpacing( 2 );
    mp_tab_layout->addStretch( 1 );
    setLayout( mp_tab_layout );
}

void ToolBarTab::addToolBarBox(ToolBarBox * box)
{
    mp_tab_layout->insertWidget( mp_tab_layout->count() - 1, box );
    connect( box, SIGNAL(triggered(QAction*)),
             this, SIGNAL(triggered(QAction*)) );
    addActions( box->actions() );
    m_box_list.append( box );
}

ToolBarBox * ToolBarTab::boxOfCode(int code)
{
    for ( int i = 0; i < m_box_list.size(); i ++ )
    {
        if ( m_box_list[ i ]->boxCode() == code )
        {
            return m_box_list[ i ];
        }
    }
    return 0;
}

ToolBarWidget::ToolBarWidget( QWidget * parent ) :
    QTabWidget( parent )
{
    setStyleSheet( "QTabBar::tab {min-width: 60px; min-height: 20px;} \
                   QTabBar{ font-size: 14px; }" );
    setFixedHeight( 130 );
}

void ToolBarWidget::addToolBarTab( ToolBarTab *tab )
{
    addTab( tab, tab->tabName() );
    connect( tab, SIGNAL(triggered(QAction*)),
             this, SIGNAL(triggered(QAction*)) );
    addActions( tab->actions() );
    m_tab_list.append( tab );
}

void ToolBarWidget::removeToolBarTab( int index )
{
    removeTab( index );
    m_tab_list.removeAt( index );
}

ToolBarTab * ToolBarWidget::tabOfCode(int code)
{
    for ( int i = 0; i < m_tab_list.size(); i ++ )
    {
        if ( m_tab_list[ i ]->tabCode() == code )
        {
            return m_tab_list[ i ];
        }
    }
    return 0;
}

void ToolBarWidget::setTabVisible(int code, bool visible)
{
    if ( visible )
    {
        for ( int i = 0; i < m_tab_list.size(); i ++ )
        {
            if ( m_tab_list[ i ]->tabCode() == code )
            {
                insertTab( i, m_tab_list[ i ], m_tab_list[ i ]->tabName() );
            }
        }
    }
    else
    {
        for ( int i = 0; i < count(); i ++ )
        {
            if ( ((ToolBarTab *)widget( i ))->tabCode() == code )
            {
                removeTab( i );
            }
        }
    }
}

RibbonMainWindow::RibbonMainWindow(QWidget * parent, Qt::WindowFlags flags) :
    QMainWindow( parent, flags ),
    mp_toolbar_widget( 0 ), mp_central_widget( 0 )
{
    setStyleSheet( "RibbonMainWindow::separator \
        { background: black; width: 1px; height: 1px; }");

    mp_main_layout = new QVBoxLayout;
    mp_main_layout->setMargin( 0 );
    mp_main_layout->setSpacing( 0 );

    QWidget * central_widget = new QWidget;
    central_widget->setMouseTracking( true );
    central_widget->setLayout( mp_main_layout );

    setCentralWidget( central_widget );
}

void RibbonMainWindow::setRibbonCentralWidget(QWidget * widget)
{
    mp_main_layout->removeWidget( mp_central_widget );
    mp_main_layout->insertWidget( 3, widget );
    mp_central_widget = widget;
}

void RibbonMainWindow::setRibbonToolbarWidget(ToolBarWidget * widget)
{
    mp_main_layout->removeWidget( mp_toolbar_widget );
    mp_main_layout->insertWidget( 2, widget );
    mp_toolbar_widget = widget;
}

FileLineEdit::FileLineEdit( FileType type, const QString & contents,
    const QString & path, const QString & fliter, QWidget * parent
) :
    QLineEdit( contents, parent ),
    m_type( type ), m_path( path ), m_fliter( fliter )
{
    mp_file_button = new QPushButton(
        QIcon( QPixmap( "images/open.png" ) ), QString(), this );
    connect( mp_file_button, SIGNAL(clicked()),
             this, SLOT(fileButtonClickHandler()) );
    if ( m_type == NEW_FILE_IF || m_type == OPEN_FILE_IF )
    {
        m_fliter =
            QString( "binary file (*.bin);;text file (*.txt);;All file (*.*)" );
    }
}

void FileLineEdit::resizeEvent( QResizeEvent *event )
{
    mp_file_button->move(
        QPoint( event->size().width() - event->size().height(), 0 ) );
    mp_file_button->resize(
        event->size().height(), event->size().height() );
}

void FileLineEdit::fileButtonClickHandler()
{
    QString fname;
    if ( m_type == NEW_FILE || m_type == NEW_FILE_IF )
    {
        fname = QFileDialog::getSaveFileName( this,
                    "New File for Option", m_path, m_fliter );
    }
    else
    {
        fname = QFileDialog::getOpenFileName( this,
                    "Open File for Option", m_path, m_fliter );
    }
    if ( fname == "" )
    {
        return;
    }

    if ( m_type == NEW_FILE_IF || m_type == OPEN_FILE_IF )
    {
        int index = fname.lastIndexOf( "." );
        if ( index > -1 )
            fname = fname.left( index );
    }
    setText( fname );
}

NumLineEdit::NumLineEdit(
    NumType type, const QString &contents, QWidget *parent
) :
    QLineEdit( contents,  parent ), m_type( type )
{
    if ( m_type == INT_NUM )
    {
        setValidator( new QRegExpValidator( QRegExp( QString(
            "^(\\(seq \\d+ \\d+ \\d+\\))|(\\$\\d+)|(((-?\\d+) ?)+)$" ) ),
            this ) );
    }
    else if ( m_type == DOUBLE_NUM )
    {
        setValidator( new QRegExpValidator( QRegExp( QString(
            "^(\\$\\d+)|(((-?\\d+)(\\.\\d+)? ?)+)$" ) ), this ) );
    }
}

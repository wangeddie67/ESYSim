#include "analyse_fliter_panel.h"

AnalyseFilterPanel::AnalyseFilterPanel(EsyNetCfg * cfg, QWidget *parent) :
    ToolBarBox( "Analyse Filter", -1, parent ), mp_network_cfg( cfg )
{
    mp_src_id_label = new QLabel( "Src Addr", this );
    mp_dst_id_label = new QLabel( "Dst Addr", this );
    mp_src_id_combobox = new QComboBox( this );
    mp_dst_id_combobox = new QComboBox( this );

    mp_addr_label = new QLabel( "Router Addr", this );
    mp_addr_combobox = new QComboBox( this );

    setRouterCount( mp_network_cfg->routerCount() );

    mp_input_phy_label = new QLabel( "Input port", this );
    mp_output_phy_label = new QLabel( "Output port", this );
    mp_input_phy_combobox = new QComboBox( this );
    mp_output_phy_combobox = new QComboBox( this );
    setPhyCount( mp_network_cfg->maxPcNum() );

    mp_input_vc_label = new QLabel( "Input vc", this );
    mp_output_vc_label = new QLabel( "Output vc", this );
    mp_input_vc_combobox = new QComboBox( this );
    mp_output_vc_combobox = new QComboBox( this );
    setVcCount( mp_network_cfg->maxVcNum() );

    mp_app_label = new QLabel( "App Id", this );
    mp_app_combobox = new QComboBox( this );
    mp_app_combobox->addItem( "All" );

    QGridLayout * main_layout = new QGridLayout();
    main_layout->setMargin( 0 );
    main_layout->setSpacing( 5 );

    main_layout->addWidget( mp_src_id_label,    0, 0, 1, 1 );
    main_layout->addWidget( mp_src_id_combobox, 0, 1, 1, 1 );
    main_layout->addWidget( mp_dst_id_label,    1, 0, 1, 1 );
    main_layout->addWidget( mp_dst_id_combobox, 1, 1, 1, 1 );

    main_layout->addWidget( mp_addr_label,    0, 2, 1, 1 );
    main_layout->addWidget( mp_addr_combobox, 0, 3, 1, 1 );

    main_layout->addWidget( mp_input_phy_label,    0, 4, 1, 1 );
    main_layout->addWidget( mp_input_phy_combobox, 0, 5, 1, 1 );
    main_layout->addWidget( mp_input_vc_label,     0, 6, 1, 1 );
    main_layout->addWidget( mp_input_vc_combobox,  0, 7, 1, 1 );

    main_layout->addWidget( mp_output_phy_label,    1, 4, 1, 1 );
    main_layout->addWidget( mp_output_phy_combobox, 1, 5, 1, 1 );
    main_layout->addWidget( mp_output_vc_label,     1, 6, 1, 1 );
    main_layout->addWidget( mp_output_vc_combobox,  1, 7, 1, 1 );

    main_layout->addWidget( mp_app_label,    1, 2, 1, 1 );
    main_layout->addWidget( mp_app_combobox, 1, 3, 1, 1 );

    mp_box_layout->addLayout( main_layout);

    connect(mp_addr_combobox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(routerIdChangedHandler(int)));
    connect(mp_input_phy_combobox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(inputPhyChangedHandler(int)));
    connect(mp_output_phy_combobox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(outputPhyChangedHandler(int)));
}

void AnalyseFilterPanel::setVisibleFlag( const FliterFlags & flag )
{
    mp_src_id_label->setVisible( flag.testFlag( FLITER_SOUR ) );
    mp_src_id_combobox->setVisible( flag.testFlag( FLITER_SOUR ) );
    mp_dst_id_label->setVisible( flag.testFlag( FLITER_DEST ) );
    mp_dst_id_combobox->setVisible( flag.testFlag( FLITER_DEST ) );
    mp_addr_label->setVisible(
        flag.testFlag( FLITER_ROUTER ) || flag.testFlag( FLITER_INTER ) );
    mp_addr_combobox->setVisible(
        flag.testFlag( FLITER_ROUTER ) || flag.testFlag( FLITER_INTER ) );
    mp_input_phy_label->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_input_phy_combobox->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_input_vc_label->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_input_vc_combobox->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_output_phy_label->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_output_phy_combobox->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_output_vc_label->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_output_vc_combobox->setVisible( flag.testFlag( FLITER_INTER ) );
    mp_app_label->setVisible( flag.testFlag( FLITER_APP ) );
    mp_app_combobox->setVisible( flag.testFlag( FLITER_APP ) );

    setVisible( !(flag == 0) );
}

void AnalyseFilterPanel::setRouterCount( long count )
{
    mp_src_id_combobox->clear();
    mp_dst_id_combobox->clear();
    mp_addr_combobox->clear();

    mp_src_id_combobox->addItem( "All" );
    mp_dst_id_combobox->addItem( "All" );
    mp_addr_combobox->addItem( "All" );
    for ( int i = 0; i < count; i ++ )
    {
        mp_src_id_combobox->addItem( QString::number( i ) );
        mp_dst_id_combobox->addItem( QString::number( i ) );
        mp_addr_combobox->addItem( QString::number( i ) );
    }
}

void AnalyseFilterPanel::setPhyCount( long count )
{
    mp_input_phy_combobox->clear();
    mp_output_phy_combobox->clear();

    mp_input_phy_combobox->addItem( "All" );
    mp_output_phy_combobox->addItem( "All" );
    for ( int i = 0; i < count; i ++ )
    {
        mp_input_phy_combobox->addItem( QString::number( i ) );
        mp_output_phy_combobox->addItem( QString::number( i ) );
    }
}

void AnalyseFilterPanel::setVcCount( long count )
{
    mp_input_vc_combobox->clear();
    mp_output_vc_combobox->clear();

    mp_input_vc_combobox->addItem( "All" );
    mp_output_vc_combobox->addItem( "All" );
    for ( int i = 0; i < count; i ++ )
    {
        mp_input_vc_combobox->addItem( QString::number( i ) );
        mp_output_vc_combobox->addItem( QString::number( i ) );
    }
}

void AnalyseFilterPanel::setAppCount( long count )
{
    mp_app_combobox->clear();
    mp_app_combobox->addItem( "All" );
    for ( int i = 0; i < count; i ++ )
    {
        mp_app_combobox->addItem( QString::number( i ) );
    }
}

void AnalyseFilterPanel::routerIdChangedHandler( int index )
{
    int input_phy_index = mp_input_phy_combobox->currentIndex();
    int input_vc_index = mp_input_vc_combobox->currentIndex();
    int output_phy_index = mp_output_phy_combobox->currentIndex();
    int output_vc_index = mp_output_vc_combobox->currentIndex();

    if ( index > 0 )
    {
        setPhyCount( mp_network_cfg->router( index - 1 ).portNum() );
        setVcCount( mp_network_cfg->router( index - 1).maxVcNum() );
    }
    else
    {
        setPhyCount( mp_network_cfg->maxPcNum() );
        setVcCount( mp_network_cfg->maxVcNum() );
    }

    mp_input_phy_combobox->setCurrentIndex( input_phy_index );
    mp_input_vc_combobox->setCurrentIndex( input_vc_index );
    mp_output_phy_combobox->setCurrentIndex( output_phy_index );
    mp_output_vc_combobox->setCurrentIndex( output_vc_index );
}

void AnalyseFilterPanel::inputPhyChangedHandler( int index )
{
    int vc_index = mp_input_vc_combobox->currentIndex();

    long count;
    if ( mp_addr_combobox->currentIndex() > 0 )
    {
        if ( index > 0 )
        {
            count = mp_network_cfg->router( mp_addr_combobox->currentIndex() - 1)
                    .port( index - 1 ).inputVcNumber();
        }
        else
        {
            count = mp_network_cfg->router( mp_addr_combobox->currentIndex() - 1)
                    .maxVcNum();
        }
    }
    else
    {
        count = mp_network_cfg->maxVcNum();
    }

    mp_input_vc_combobox->clear();
    mp_input_vc_combobox->addItem( "All" );
    for ( int i = 0; i < count; i ++ )
    {
        mp_input_vc_combobox->addItem( QString::number( i ) );
    }
    mp_input_vc_combobox->setCurrentIndex( vc_index );
}

void AnalyseFilterPanel::outputPhyChangedHandler( int index )
{
    int vc_index = mp_output_vc_combobox->currentIndex();

    long count;
    if ( mp_addr_combobox->currentIndex() > 0 )
    {
        if ( index > 0 )
        {
            count = mp_network_cfg->router( mp_addr_combobox->currentIndex() - 1)
                    .port( index - 1 ).outputVcNumber();
        }
        else
        {
            count = mp_network_cfg->router( mp_addr_combobox->currentIndex() - 1)
                    .maxVcNum();
        }
    }
    else
    {
        count = mp_network_cfg->maxVcNum();
    }

    mp_output_vc_combobox->clear();
    mp_output_vc_combobox->addItem( "All" );
    for ( int i = 0; i < count; i ++ )
    {
        mp_output_vc_combobox->addItem( QString::number( i ) );
    }
    mp_output_vc_combobox->setCurrentIndex( vc_index );
}

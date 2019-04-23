#include "faults_config_dialog.h"
#include "esy_faultcfg.h"

QStringList FaultsConfigDialog::const_typical_fault_type_str = QStringList() <<
    "Custom" << "Const-Living" << "Const-Faulty" << "2-States";

// ---- constructor and destructor ----//
FaultsConfigDialog::FaultsConfigDialog(
        QString fname, const EsyNetworkCfg & network_cfg ) :
    ResultBaseDialog( ResultBaseDialog::RESULT_FAULT_CONFIG ),
    m_network_cfg( network_cfg ), m_cfg_file_dir( fname ),
    m_current_index( -1 ), m_edit_enable( false )
{
	// add components
	addComponents();

    EsyFaultConfigList fault_cfg_list;
    fault_cfg_list.readFile( m_cfg_file_dir.toStdString() );
    mp_faults_model->setFaultCfgList( fault_cfg_list );
}

// ---- constructor and destructor ----//

// ---- components initialization functions ---- //
// add components
void FaultsConfigDialog::addComponents()
{
    mp_type_label = new QLabel( "Type" );
    mp_type_combobox = new QComboBox( this );
    mp_type_combobox->addItem( "All" );
    for ( int i = 0; i < EsyFaultConfigItem::FAULT_TYPE_NUM; i ++ )
    {
        mp_type_combobox->addItem( QString::fromStdString(
             EsyFaultConfigItem::faultTypeStr( (EsyFaultConfigItem::FaultType)i ) ) );
    }
    connect( mp_type_combobox, SIGNAL( currentIndexChanged(int) ),
             this, SLOT(typeComboBoxChangedHandler(int)) );

    mp_router_label = new QLabel( "Router" );
    mp_router_combobox = new QComboBox( this );
    mp_router_combobox->addItem( "All" );
    for ( int i = 0; i < m_network_cfg.routerCount(); i ++ )
    {
        mp_router_combobox->addItem( QString::number( i ) );
    }
    connect( mp_router_combobox, SIGNAL( currentIndexChanged(int) ),
             this, SLOT(routerComboBoxChangedHandler(int)) );

    mp_port_label = new QLabel( "Port" );
    mp_port_combobox = new QComboBox( this );
    connect( mp_port_combobox, SIGNAL( currentIndexChanged(int) ),
             this, SLOT(portComboBoxChangedHandler(int)) );

    mp_vc_label = new QLabel( "VC" );
    mp_vc_combobox = new QComboBox( this );
    connect( mp_vc_combobox, SIGNAL( currentIndexChanged(int) ),
             this, SLOT(vcComboBoxChangedHandler(int)) );

    routerComboBoxChangedHandler( 0 );

    mp_bit_label = new QLabel( "Bit" );
    mp_bit_combobox = new QComboBox( this );
    mp_bit_combobox->addItem( "All" );
    for ( int i = 0; i < m_network_cfg.dataPathWidth(); i ++ )
    {
        mp_bit_combobox->addItem( QString::number( i ) );
    }

    mp_typical_combobox = new QComboBox( this );
    for ( int i = 0; i < FaultsConfigDialog::TYPICALFAULT_TYPE_NUM; i ++ )
    {
        mp_typical_combobox->addItem( const_typical_fault_type_str[ i ] );
    }
    connect( mp_typical_combobox, SIGNAL( currentIndexChanged(int) ),
             this, SLOT(typicalComboBoxChangedHandler(int)) );

    mp_state_spinbox = new QSpinBox( this );
    connect( mp_state_spinbox, SIGNAL(valueChanged(int)),
             this, SLOT(stateSpinboxChangedHandler(int)) );

    mp_state_table = new FaultStateTableView( this );
    mp_state_model = new FaultStateModel( this );
    mp_state_table->setModel( mp_state_model );
    connect( mp_state_table, SIGNAL(doubleClicked(QModelIndex)),
             mp_state_model, SLOT(changeItemStates(QModelIndex)) );

    mp_matrix_table = new FaultMatrixTableView( this );
    mp_matrix_model = new FaultMatrixModel( this );
    mp_matrix_table->setModel( mp_matrix_model );

    mp_faults_table = new FaultListTableView( this );
    mp_faults_model = new FaultListModel( this );
    mp_faults_table->setModel( mp_faults_model );

    mp_insert_pushbutton = new QPushButton( "Insert", this );
    mp_insert_pushbutton->setFixedWidth( 150 );
    connect( mp_insert_pushbutton, SIGNAL(clicked()),
             this, SLOT(insertButtonClickHandler()) );

    mp_delete_pushbutton = new QPushButton( "Delete", this );
    mp_delete_pushbutton->setFixedWidth( 150 );
    connect( mp_delete_pushbutton, SIGNAL(clicked()),
             this, SLOT(deleteButtonClickHandler()) );

    mp_edit_pushbutton = new QPushButton( "Edit", this );
    mp_edit_pushbutton->setFixedWidth( 150 );
    connect( mp_edit_pushbutton, SIGNAL(clicked()),
             this, SLOT(editButtonClickHandler()) );

    mp_save_pushbutton = new QPushButton( "Save File", this );
    mp_save_pushbutton->setFixedWidth( 150 );
    connect( mp_save_pushbutton, SIGNAL(clicked()),
             this, SLOT(saveButtonClickHandler()) );

    mp_ok_pushbutton = new QPushButton( "Ok", this );
    mp_ok_pushbutton->setFixedWidth( 150 );
    connect( mp_ok_pushbutton, SIGNAL(clicked()),
             this, SLOT(okButtonClickHandler()) );

    mp_cancel_pushbutton = new QPushButton( "Cancel", this );
    mp_cancel_pushbutton->setFixedWidth( 150 );
    connect( mp_cancel_pushbutton, SIGNAL(clicked()),
             this, SLOT(cancelButtonClickHandler()) );

    // layout
    QGridLayout * mp_comp_para_layout = new QGridLayout();
    mp_comp_para_layout->addWidget( mp_type_label, 0, 0, 1, 1 );
    mp_comp_para_layout->addWidget( mp_type_combobox, 0, 1, 1, 1 );
    mp_comp_para_layout->addWidget( mp_router_label, 1, 0, 1, 1 );
    mp_comp_para_layout->addWidget( mp_router_combobox, 1, 1, 1, 1 );
    mp_comp_para_layout->addWidget( mp_port_label, 2, 0, 1, 1 );
    mp_comp_para_layout->addWidget( mp_port_combobox, 2, 1, 1, 1 );
    mp_comp_para_layout->addWidget( mp_vc_label, 3, 0, 1, 1 );
    mp_comp_para_layout->addWidget( mp_vc_combobox, 3, 1, 1, 1 );
    mp_comp_para_layout->addWidget( mp_bit_label, 4, 0, 1, 1 );
    mp_comp_para_layout->addWidget( mp_bit_combobox, 4, 1, 1, 1 );
    mp_comp_para_layout->addWidget( new QLabel( "Typical Model" ), 5, 0, 1, 2 );
    mp_comp_para_layout->addWidget( mp_typical_combobox, 6, 0, 1, 2 );
    mp_comp_para_layout->addWidget( new QLabel( "States" ), 7, 0, 1, 1 );
    mp_comp_para_layout->addWidget( mp_state_spinbox, 7, 1, 1, 1 );
    mp_comp_para_layout->addWidget( new QLabel( "State Value" ), 8, 0, 1, 2 );
    mp_comp_para_layout->addWidget( mp_state_table, 9, 0, 1, 2 );

    QHBoxLayout * mp_button_layout = new QHBoxLayout();
    mp_button_layout->addStretch();
    mp_button_layout->addWidget( mp_ok_pushbutton );
    mp_button_layout->addWidget( mp_cancel_pushbutton );

    QGridLayout * mp_fault_table_layout = new QGridLayout();
    QLabel * matrix_head_label = new QLabel( "Transfer Matrix" );
    matrix_head_label->setAlignment( Qt::AlignCenter );
    mp_fault_table_layout->addWidget( matrix_head_label, 0, 0, 1, 2 );
    QLabel * matrix_head_x_label = new QLabel( "Next State" );
    matrix_head_x_label->setAlignment( Qt::AlignCenter );
    mp_fault_table_layout->addWidget( matrix_head_x_label, 1, 1, 1, 1 );
    mp_fault_table_layout->addWidget(
                new QLabel( "Current\nState" ), 2, 0, 1, 1 );
    mp_fault_table_layout->addWidget( mp_matrix_table, 2, 1, 1, 1 );

    QGridLayout * mp_fault_item_layout = new QGridLayout();
    mp_fault_item_layout->addLayout( mp_comp_para_layout, 0, 0, 1, 1 );
    mp_fault_item_layout->addLayout( mp_fault_table_layout, 0, 1, 1, 1 );
    mp_fault_item_layout->addLayout( mp_button_layout, 1, 0, 1, 2 );
    mp_fault_item_layout->setColumnStretch( 0, 20 );
    mp_fault_item_layout->setColumnStretch( 1, 80 );

    QVBoxLayout * mp_fault_list_layout = new QVBoxLayout();
    mp_fault_list_layout->addWidget( new QLabel( "Faults List" ) );
    mp_fault_list_layout->addWidget( mp_faults_table );
    mp_fault_list_layout->addWidget( mp_insert_pushbutton );
    mp_fault_list_layout->addWidget( mp_delete_pushbutton );
    mp_fault_list_layout->addWidget( mp_edit_pushbutton );
    mp_fault_list_layout->addWidget( mp_save_pushbutton );
    mp_fault_list_layout->setAlignment( mp_insert_pushbutton, Qt::AlignCenter );
    mp_fault_list_layout->setAlignment( mp_delete_pushbutton, Qt::AlignCenter );
    mp_fault_list_layout->setAlignment( mp_edit_pushbutton, Qt::AlignCenter );
    mp_fault_list_layout->setAlignment( mp_save_pushbutton, Qt::AlignCenter );

	// temp horizontal layout
	// vertical layout
    QHBoxLayout * mp_faults_layout = new QHBoxLayout();
    mp_faults_layout->addLayout( mp_fault_list_layout );
    mp_faults_layout->setStretch( 0, 25 );
    mp_faults_layout->addLayout( mp_fault_item_layout );
    mp_faults_layout->setStretch( 1, 75 );

    setLayout( mp_faults_layout );

    setEditComponentEnable( false );
}

void FaultsConfigDialog::typeComboBoxChangedHandler( int index )
{
    int t_index = index - 1;
    if ( t_index == -1 || t_index == EsyFaultConfigItem::FAULT_PORT ||
         t_index == EsyFaultConfigItem::FAULT_LINE ||
         t_index == EsyFaultConfigItem::FAULT_BUFFER ||
         t_index == EsyFaultConfigItem::FAULT_SWITCH )
    {
        mp_port_label->setVisible( true );
        mp_port_combobox->setVisible( true );
    }
    else
    {
        mp_port_label->setVisible( false );
        mp_port_combobox->setVisible( false );
        mp_port_combobox->setCurrentIndex( 0 );
    }
    if ( t_index == -1 || t_index == EsyFaultConfigItem::FAULT_BUFFER )
    {
        mp_vc_label->setVisible( true );
        mp_vc_combobox->setVisible( true );
    }
    else
    {
        mp_vc_label->setVisible( false );
        mp_vc_combobox->setVisible( false );
        mp_vc_combobox->setCurrentIndex( 0 );
    }
    if ( t_index == -1 || t_index == EsyFaultConfigItem::FAULT_LINE ||
         t_index == EsyFaultConfigItem::FAULT_BUFFER ||
         t_index == EsyFaultConfigItem::FAULT_SWITCH )
    {
        mp_bit_label->setVisible( true );
        mp_bit_combobox->setVisible( true );
    }
    else
    {
        mp_bit_label->setVisible( false );
        mp_bit_combobox->setVisible( false );
        mp_bit_combobox->setCurrentIndex( 0 );
    }
}

void FaultsConfigDialog::routerComboBoxChangedHandler( int index )
{
    int t_index = index - 1;
    if ( t_index < -1 )
    {
        t_index = -1;
    }

    int current_port_item = mp_port_combobox->currentIndex();
    if ( current_port_item < 0 )
    {
        current_port_item = 0;
    }
    mp_port_combobox->clear();
    mp_port_combobox->addItem( "All" );
    if ( t_index == -1 )
    {
        for ( int i = 0; i < m_network_cfg.maxPcNum(); i ++ )
        {
            mp_port_combobox->addItem( QString::number( i ) );
        }
    }
    else
    {
        for ( int i = 0; i < m_network_cfg.router( t_index ).portNum(); i ++ )
        {
            mp_port_combobox->addItem( QString::number( i ) );
        }
    }
    mp_port_combobox->setCurrentIndex( current_port_item );
}

void FaultsConfigDialog::portComboBoxChangedHandler( int index )
{
    int t_index = index - 1;
    if ( t_index < -1 )
    {
        t_index = -1;
    }

    int current_router_item = mp_router_combobox->currentIndex();
    int current_vc_item = mp_vc_combobox->currentIndex();
    if ( current_vc_item < 0 )
    {
        current_vc_item = 0;
    }
    mp_vc_combobox->clear();
    mp_vc_combobox->addItem( "All" );
    if ( current_router_item == 0 )
    {
        for ( int i = 0; i < m_network_cfg.maxVcNum(); i ++ )
        {
            mp_vc_combobox->addItem( QString::number( i ) );
        }
    }
    else
    {
        if ( t_index == -1 )
        {
            for ( int i = 0;
                  i < m_network_cfg.router( current_router_item - 1 ).maxVcNum();
                  i ++ )
            {
                mp_vc_combobox->addItem( QString::number( i ) );
            }
        }
        else
        {
            for ( int i = 0;
                  i < m_network_cfg.router( current_router_item - 1 ).
					  port( t_index ).maxVcNumber();
                  i ++ )
            {
                mp_vc_combobox->addItem( QString::number( i ) );
            }
        }
    }
    mp_vc_combobox->setCurrentIndex( current_vc_item );
}

void FaultsConfigDialog::vcComboBoxChangedHandler( int index )
{
    Q_UNUSED( index );
}

void FaultsConfigDialog::stateSpinboxChangedHandler( int value )
{
    mp_state_model->resizeStates( value );
    mp_matrix_model->resizeStates( value );
}

void FaultsConfigDialog::typicalComboBoxChangedHandler( int index )
{
    switch ( index )
    {
    case TYPICALFAULT_CONST_LIVING :
        mp_state_spinbox->setValue( 1 );
        mp_state_model->setStateFaulty( 0, false );
        mp_matrix_model->setTransferProb( 0, 0, 1.0 );
        break;
    case TYPICALFAULT_CONST_FAULTY :
        mp_state_spinbox->setValue( 1 );
        mp_state_model->setStateFaulty( 0, true );
        mp_matrix_model->setTransferProb( 0, 0, 1.0 );
        break;
    case TYPICALFAULT_TWO_STATES :
        mp_state_spinbox->setValue( 2 );
        mp_state_model->setStateFaulty( 0, false );
        mp_state_model->setStateFaulty( 1, true );
        mp_matrix_model->setTransferProb( 0, 0, 1.0 );
        mp_matrix_model->setTransferProb( 0, 1, 0.0 );
        mp_matrix_model->setTransferProb( 1, 0, 1.0 );
        mp_matrix_model->setTransferProb( 1, 1, 0.0 );
        break;
    }

    mp_state_spinbox->setEnabled( index == TYPICALFAULT_CUSTOM );
}

void FaultsConfigDialog::setEditComponentEnable( bool enable )
{
    mp_type_combobox->setEnabled( enable );
    mp_router_combobox->setEnabled( enable );
    mp_port_combobox->setEnabled( enable );
    mp_vc_combobox->setEnabled( enable );
    mp_bit_combobox->setEnabled( enable );

    mp_typical_combobox->setEnabled( enable );
    mp_state_spinbox->setEnabled( enable &&
        (mp_typical_combobox->currentIndex() == TYPICALFAULT_CUSTOM) );

    mp_state_table->setEnabled( enable );
    mp_matrix_table->setEnabled( enable );

    mp_insert_pushbutton->setEnabled( !enable );
    mp_delete_pushbutton->setEnabled( !enable );
    mp_edit_pushbutton->setEnabled( !enable );
    mp_save_pushbutton->setEnabled( !enable );

    mp_ok_pushbutton->setEnabled( enable );
    mp_cancel_pushbutton->setEnabled( enable );
}

void FaultsConfigDialog::insertButtonClickHandler()
{
    m_current_index = -1;
    setEditComponentEnable( true );
}

void FaultsConfigDialog::deleteButtonClickHandler()
{
    QModelIndex t_index = mp_faults_table->currentIndex();
    if ( t_index == QModelIndex() )
    {
        return;
    }
    mp_faults_model->removeRow( t_index.row() );
}

void FaultsConfigDialog::editButtonClickHandler()
{
    QModelIndex t_index = mp_faults_table->currentIndex();
    if ( t_index == QModelIndex() )
    {
        return;
    }
    m_current_index = t_index.row();
    EsyFaultConfigItem t_item =
        ((FaultListModelItem *)(mp_faults_model->item( m_current_index )))->
        getEsyFaultConfigItem();

    setEditComponentEnable( true );

    mp_type_combobox->setCurrentIndex( t_item.faultType() + 1 );
    mp_router_combobox->setCurrentIndex( t_item.router() + 1 );
    mp_port_combobox->setCurrentIndex( t_item.port() + 1 );
    mp_vc_combobox->setCurrentIndex( t_item.vc() + 1 );
    mp_bit_combobox->setCurrentIndex( t_item.bit() + 1 );
    mp_state_spinbox->setValue( (int)t_item.state().size() );
    mp_state_model->setStatesVector( t_item.state() );
    mp_matrix_model->setStatesMatrix( t_item.transferMatrix() );
}

void FaultsConfigDialog::saveButtonClickHandler()
{
    EsyFaultConfigList fault_cfg_list = mp_faults_model->getFaultCfgList();
    fault_cfg_list.writeFile( m_cfg_file_dir.toStdString() );
}

void FaultsConfigDialog::okButtonClickHandler()
{
    EsyFaultConfigItem t_item(
        (EsyFaultConfigItem::FaultType)(mp_type_combobox->currentIndex() - 1),
        mp_router_combobox->currentIndex() - 1,
        mp_port_combobox->currentIndex() - 1,
        mp_vc_combobox->currentIndex() - 1,
        mp_bit_combobox->currentIndex() - 1,
        mp_state_model->getStatesVector(),
        mp_matrix_model->getStatesMatrix() );

    if ( m_current_index == -1 )
    {
        mp_faults_model->appendItem( t_item );
//        m_fault_cfg_list.push_back( t_item );
//        mp_faults_model->resize( (int)m_fault_cfg_list.size() );
    }
    else
    {
        mp_faults_model->updateItem( m_current_index, t_item );
//        m_fault_cfg_list[ m_current_index ] = t_item;
    }
//    mp_faults_model->update();

    setEditComponentEnable( false );
}

void FaultsConfigDialog::cancelButtonClickHandler()
{
    setEditComponentEnable( false );
}

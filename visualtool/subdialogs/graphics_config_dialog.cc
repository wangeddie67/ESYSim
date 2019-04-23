#include "graphics_config_dialog.h"

GraphicsConfigDialog::GraphicsConfigDialog(QWidget *parent) :
    QDialog(parent)
{
    addComponent();

    mp_config_table_model = new GraphicsConfigModel( this );
    mp_config_table_tree->setModel( mp_config_table_model );
    ErrorHandler error = mp_config_table_model->readFile();
    if ( error.hasError() )
    {
        mp_config_table_model->setDefaultValue();
    }
    mp_config_table_tree->expandAll();
    connect( mp_default_button, SIGNAL(clicked()),
             mp_config_table_model, SLOT(setDefaultValue()) );
}

void GraphicsConfigDialog::addComponent()
{
    mp_config_table_tree = new GraphicsConfigTreeView( this );

    mp_default_button = new QPushButton( "Default" );
    mp_default_button->setFixedWidth( 100 );
    mp_cancel_button = new QPushButton( "Cancel" );
    mp_cancel_button->setFixedWidth( 100 );
    connect( mp_cancel_button, SIGNAL(clicked()),
             this, SLOT(reject()) );
    mp_ok_button = new QPushButton( "Ok" );
    mp_ok_button->setFixedWidth( 100 );
    connect( mp_ok_button, SIGNAL(clicked()),
             this, SLOT(okButtonHandler()) );

    QHBoxLayout * mp_button_layout = new QHBoxLayout();
    mp_button_layout->addStretch();
    mp_button_layout->addWidget( mp_default_button );
    mp_button_layout->addWidget( mp_cancel_button );
    mp_button_layout->addWidget( mp_ok_button );

    QVBoxLayout * mp_main_layout = new QVBoxLayout();
    mp_main_layout->addWidget( mp_config_table_tree );
    mp_main_layout->addLayout( mp_button_layout );

    setLayout( mp_main_layout );

    setFixedSize( QSize( 600, 400 ) );
}

void GraphicsConfigDialog::okButtonHandler()
{
    mp_config_table_model->writeFile();
    accept();
}

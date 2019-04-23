/*
 T his *program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA  02110-1301, USA.

 ---
 Copyright (C) 2015, Junshi Wang <>
 */

/*************************************************
  File Name :
	link_tool_dialog.cc
  Description :
	Link tool interface by Qt
  Function List :
	ErrorHandler LinkToolModelItem::readArgumentFile( QString argufile )

	int LinkToolModel::rowCount( const QModelIndex &parent ) const
	int LinkToolModel::columnCount( const QModelIndex &parent ) const
	QVariant LinkToolModel::data(const QModelIndex &index, int role) const
	QVariant LinkToolModel::headerData(int section, Qt::Orientation orientation,
		int role) const
	Qt::ItemFlags LinkToolModel::flags( const QModelIndex &index ) const
	ErrorHandler LinkToolModel::readLinkToolFile()
	ErrorHandler LinkToolModel::writeLinkToolFile()
	QString LinkToolModel::renewShell()

	QWidget *FatherComboxDelegate::createEditor( QWidget *parent,
		const QStyleOptionViewItem &option, const QModelIndex &index ) const
	void FatherComboxDelegate::setEditorData(QWidget *editor,
		const QModelIndex &index) const
	void FatherComboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const
	void FatherComboxDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex & index ) const
*************************************************/

/* including head file */
#include "link_tool_dialog.h"
#include "../modelsview/argument_list_table.h"
#include "esy_argument.h"

/*************************************************
  Function :
	LinkToolDialog::LinkToolDialog( LinkToolModel * model, LinkToolDialogType type )
  Description :
	Construct a link tool configuration dialog
  Input :
	   LinkToolModel * model  link tool list model
	LinkToolDialogType type   link tool type
*************************************************/
LinkToolDialog::LinkToolDialog( LinkToolModel * model,
	LinkToolDelegate::LinkToolViewType type ) :
	m_type( type )
{
	/* if model is specific, connect it with view */
	if ( model )
	{
		mp_linktool_model = model;
	} /* if ( model ) */
	/* if not, generate new model */
	else
	{
		mp_linktool_model = new LinkToolModel( this );
		ErrorHandler t_error = mp_linktool_model->readLinkToolFile();
		if ( t_error.hasError() )
		{
			QMessageBox::warning(this, t_error .title(), t_error.text(), QMessageBox::Ok );
		} /* if ( t_error == true ) */
	} /* else ( model ) */

	mp_linktool_view = new LinkToolTableView( this );
	mp_linktool_view->setModel( mp_linktool_model );
	mp_linktool_view->setItemDelegate( new LinkToolDelegate( m_type, this ) );

	mp_layout = new QVBoxLayout();
	mp_layout->addWidget( mp_linktool_view );

	mp_button_layout = new QHBoxLayout();

	if ( m_type == LinkToolDelegate::LINKTOOLVIEW_EDIT )
	{
		mp_add_button = new QPushButton( "Add" );
		mp_add_button->setFixedWidth( 100 );
		connect( mp_add_button, SIGNAL( clicked() ), this,
			SLOT( addButtonClickedHandler() ) );
		mp_button_layout->addWidget( mp_add_button );

		mp_remove_button = new QPushButton( "Remove" );
		mp_remove_button->setFixedWidth( 100 );
		connect( mp_remove_button, SIGNAL( clicked() ), this,
			SLOT( removeButtonClickedHandler() ) );
		mp_button_layout->addWidget( mp_remove_button );
	} /* if ( m_type == LINKTOOLDIALOG_EDIT ) */

	mp_ok_button = new QPushButton( "OK" );
	mp_ok_button->setFixedWidth( 100 );
	mp_button_layout->addWidget( mp_ok_button );
	connect( mp_ok_button, SIGNAL( clicked() ), this, SLOT( okButtonClickedHandler() ) );

	mp_cancel_button = new QPushButton( "Cancel" );
	mp_cancel_button->setFixedWidth( 100 );
	mp_button_layout->addWidget( mp_cancel_button );
	connect( mp_cancel_button, SIGNAL( clicked() ), this, SLOT( reject() ) );

	mp_layout->addLayout( mp_button_layout );

	setLayout( mp_layout );
	setFixedWidth( 500 );
}

/*************************************************
  Function :
	void LinkToolDialog::addButtonClickedHandler()
  Description :
	add a new tools to model
*************************************************/
void LinkToolDialog::addButtonClickedHandler()
{
	/* get the path of configuration file of new tool */
	QString path;
	path = QFileDialog::getOpenFileName( this, tr( "add configure file" ), ".",
		"Link Tool Configure Files (*." + QString( LINKCFG_EXTENSION ) + ")" );
	path = path.left( path.lastIndexOf( "." ) );

	/* generate new item */
	LinkToolModelItem * newitem = new LinkToolModelItem;
	/* get information about new tool */
	ErrorHandler t_error = newitem->readArgumentFile( path );
	/* if there is no error while reading argument file, insert the item into model */
	if ( t_error.hasError() )
	{
		QMessageBox::warning( this, t_error.title(), t_error.text(), QMessageBox::Ok );
		delete newitem;
	} /* if ( t_error == true ) */
	/* other wise, abandoned */
	else
	{
		mp_linktool_model->appendItem( newitem );
	} /* else ( t_error == true ) */
}

/*************************************************
  Function :
	void LinkToolDialog::removeButtonClickedHandler()
  Description :
	remove selected tools out of model
*************************************************/
void LinkToolDialog::removeButtonClickedHandler()
{
	/* count selected item */
	int total = 0;
	QString msg( "Are you sure to remove following tools:\n" );
	for ( int i = 0; i < mp_linktool_model->rowCount(); i ++ )
	{
		LinkToolModelItem * linktool_item = mp_linktool_model->linkToolItem( i );
		if ( linktool_item == 0 )
		{
			continue;
		} /* if ( linktool_item == 0 ) */
		if ( linktool_item->checkState() == Qt::Checked )
		{
			total ++;
			msg += linktool_item->name() + "\n";
		} /* if ( linktool_item->checkState() == Qt::Checked ) */
	} /* for ( int i = 0; i < mp_linktool_model->size(); i ++ ) */

	/* if there is no item select, stop directly */
	if ( total == 0 )
	{
		return;
	} /* if ( total == 0 ) */

	/* ask for confirmation */
	int ret = QMessageBox::question( this, "Confirm", msg,
		QMessageBox::Yes | QMessageBox::No,  QMessageBox::Yes );

	/* remove items */
	if ( ret == QMessageBox::Yes )
	{
		for ( int i = 0; i < mp_linktool_model->rowCount(); i ++ )
		{
			LinkToolModelItem * linktool_item = mp_linktool_model->linkToolItem( i );
			if ( linktool_item == 0 )
			{
				continue;
			} /* if ( linktool_item == 0 ) */
			if ( linktool_item->checkState() == Qt::Checked )
			{
				mp_linktool_model->removeRow( i );
				i --;
			} /* if ( linktool_item->checkState() == Qt::Checked ) */
		} /* for ( int i = 0; i < mp_linktool_model->size(); i ++ ) */
	} /* if ( ret == QMessageBox::Yes ) */
}

/*************************************************
  Function :
	void LinkToolDialog::okButtonClickedHandler()
  Description :
	write model to file
*************************************************/
void LinkToolDialog::okButtonClickedHandler()
{
	if ( m_type == LinkToolDelegate::LINKTOOLVIEW_EDIT )
	{
		ErrorHandler t_error = mp_linktool_model->writeLinkToolFile();
		if ( t_error.hasError() )
		{
			QMessageBox::warning(this, t_error .title(), t_error.text(), QMessageBox::Ok );
		} /* if ( t_error == true ) */
	} /* if ( m_type == LINKTOOLDIALOG_EDIT ) */
	accept();
}

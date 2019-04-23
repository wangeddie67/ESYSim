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

#define ARGUMENT_DECLARATION_VERSION  "1.0"
#define ARGUMENT_DECLARATION_ENCODING  "UTF-8"

#define ARGUMENT_ROOT  "Argumentconfiguration"
#define ARGUMENT_ROOT_NAME  "name"
#define ARGUMENT_ROOT_VERSION  "version"
#define ARGUMENT_ROOT_EXEC  "exec"
#define ARGUMENT_ROOT_COMPILE  "compile"

#define ARGUMENT_ARGU  "Argu"

#define ARGUMENT_ARGU_ARGUMENT  "argument"
#define ARGUMENT_ARGU_LOOP  "loop"
#define ARGUMENT_ARGU_VALUE  "value"
#define ARGUMENT_ARGU_NOTE  "note"
#define ARGUMENT_ARGU_TYPE  "type"
#define ARGUMENT_ARGU_DEPARG  "darg"
#define ARGUMENT_ARGU_DEPVAL  "dval"
#define ARGUMENT_ARGU_EXTENSION "extension"

#include "argument_list_table.h"

QStringList ArgumentListModel::const_viewheader_list =
	QStringList() << "Argument" << "Loop" << "Value" << "Help";

int ArgumentListModel::rowCount( const QModelIndex &parent ) const
{
	if ( parent == QModelIndex() )
	{
		return invisibleRootItem()->rowCount();
	}
	else
	{
		return 0;
	}
}

int ArgumentListModel::columnCount( const QModelIndex &parent ) const
{
	Q_UNUSED( parent )
	return ARGULIST_COLUMN_COUNT;
}

QVariant ArgumentListModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();
	int col = index.column();

	switch( role )
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		{
			ArgumentListModelItem * t_item =
					(ArgumentListModelItem *)item( row, 0 );

			QString t_str;
			switch( col )
			{
			case ARGULIST_OPT:
				for ( int i = 0; i < t_item->dependTab(); i ++ )
				{
					t_str += "    ";
				}
				t_str += t_item->opt();
				break;
			case ARGULIST_LOOP:
				if ( t_item->optType() == EsyArgumentItem::STRING_TYPE ||
					 t_item->optType() == EsyArgumentItem::DOUBLE_TYPE ||
					 t_item->optType() == EsyArgumentItem::LONG_TYPE ||
					 t_item->optType() == EsyArgumentItem::ENUM_TYPE )
				{
					t_str = QString::number( t_item->loop() ); break;
				}
				else
				{
					t_str = ""; break;
				}
			case ARGULIST_VALUE:
				if ( t_item->optType() != EsyArgumentItem::BOOL_TYPE )
				{
					t_str = t_item->value(); break;
				}
				else
				{
					t_str = ""; break;
				}
			case ARGULIST_NOTE: t_str = t_item->note(); break;
			}
			return t_str;
			break;
		}
	case Qt::CheckStateRole :
		{
			ArgumentListModelItem * t_item =
					(ArgumentListModelItem *)item( row, 0 );

			if ( col == ARGULIST_VALUE &&
				 t_item->optType() == EsyArgumentItem::BOOL_TYPE  )
			{
				if ( t_item->value().toInt() )
				{
					return Qt::Checked;
				}
				else
				{
					return Qt::Unchecked;
				}
			}
			break;
		}
	}
	return QVariant();
}

bool ArgumentListModel::setData(const QModelIndex &index,
								const QVariant &value, int role)
{
	int row = index.row();
	int col = index.column();

	switch( role )
	{
	case Qt::EditRole:
		{
			ArgumentListModelItem * t_item =
					(ArgumentListModelItem *)item( row, 0 );
			switch( col )
			{
			case ARGULIST_LOOP: t_item->setLoop( value.toInt() ); break;
			case ARGULIST_VALUE: t_item->setValue( value.toString() ); break;
			}
			emit dataChanged( index, index );
			return true;
		}
	case Qt::CheckStateRole :
		{
			ArgumentListModelItem * t_item =
					(ArgumentListModelItem *)item( row, 0 );

			if ( col == ARGULIST_VALUE &&
				 t_item->optType() == EsyArgumentItem::BOOL_TYPE  )
			{
				itemFromIndex( index )->setCheckState(
					(Qt::CheckState)value.toInt() );
				t_item->setValue( QString::number( value.toInt() ) );

				updateRow();
			}
			emit dataChanged( index, index );
		}
	}

	return false;
}

QVariant ArgumentListModel::headerData( int section,
	Qt::Orientation orientation, int role) const
{
	switch( role )
	{
	case Qt::DisplayRole:
		{
			if ( orientation == Qt::Horizontal )
			{
				return const_viewheader_list[ section ];
				break;
			}
			else
			{
				return QString::number( section );
			}
		}
	}
	return QVariant();
}

Qt::ItemFlags ArgumentListModel::flags( const QModelIndex &index ) const
{
	if ( !index.isValid() )
	{
		return 0;
	}

	ArgumentListModelItem * t_item =
			(ArgumentListModelItem *)item( index.row(), 0 );

	if ( index.column() == ARGULIST_VALUE &&
		 t_item->optType() == EsyArgumentItem::BOOL_TYPE  )
	{
		return  Qt::ItemIsEnabled | Qt::ItemIsSelectable |
				Qt::ItemIsUserCheckable;
	}
	else if ( index.column() == ARGULIST_LOOP ||
			  index.column() == ARGULIST_VALUE )
	{
		return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	}
	else
	{
		return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
}

void ArgumentListModel::appendItem( ArgumentListModelItem * item )
{
	QList< QStandardItem *> items;
	items << item;
	for ( int i = 1; i < ARGULIST_COLUMN_COUNT; i ++ )
	{
		items << new QStandardItem;
	}
	appendRow( items );
}

ErrorHandler ArgumentListModel::readArgumentFile( const QString & file )
{
	QFile argureadfile( file + "." + LINKCFG_EXTENSION );
	if ( !argureadfile.open( QFile::ReadOnly | QFile::Text ) )
	{
		return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() <<
			file, "ArgumentListModel", "readArgumentFile" );
	}
	QString errorMsg;
	int errorLine;
	int errorColumn;
	QDomDocument t_doc;
	if ( !t_doc.setContent( &argureadfile, &errorMsg,
							&errorLine, &errorColumn ) )
	{
		argureadfile.close();
		return ErrorHandler( ErrorHandler::DOMERROR,
			QStringList() << file << errorMsg <<
				QString::number( errorLine ) << QString::number( errorColumn ),
			"ArgumentListModel", "readArgumentFile" );
	}
	argureadfile.close();

	QDomElement t_head = t_doc.documentElement();
	if ( t_head.tagName() != ARGUMENT_ROOT )
		return ErrorHandler( ErrorHandler::XMLTAGERROR,
			QStringList() << file << t_head.tagName(),
			"ArgumentListModel", "readArgumentFile" );

	m_name = t_head.attribute( ARGUMENT_ROOT_NAME );
	m_version = t_head.attribute( ARGUMENT_ROOT_VERSION );
	m_direction = QDir().relativeFilePath(
				QFileInfo( file ).absoluteDir().absolutePath() );
	m_exec = t_head.attribute( ARGUMENT_ROOT_EXEC );
	m_config = QFileInfo( file ).fileName();
	m_compile = t_head.attribute( ARGUMENT_ROOT_COMPILE );

	QDomNodeList t_argu_list = t_head.childNodes();
	for ( int argu_index = 0; argu_index < t_argu_list.count(); argu_index ++ )
	{
		QDomElement t_argu_element = t_argu_list.at( argu_index ).toElement();
		if ( t_argu_element.tagName() == ARGUMENT_ARGU )
		{
			appendItem( new ArgumentListModelItem(
				t_argu_element.attribute( ARGUMENT_ARGU_ARGUMENT ),
				(EsyArgumentItem::ArgumentType)(
					t_argu_element.attribute(ARGUMENT_ARGU_TYPE).toInt() ),
				t_argu_element.attribute( ARGUMENT_ARGU_NOTE ).
					replace( "\\n", "\n" ),
				t_argu_element.attribute( ARGUMENT_ARGU_VALUE ),
				t_argu_element.attribute( ARGUMENT_ARGU_LOOP ).toInt(),
				t_argu_element.attribute( ARGUMENT_ARGU_DEPARG ),
				t_argu_element.attribute( ARGUMENT_ARGU_DEPVAL ).toInt(),
				t_argu_element.attribute( ARGUMENT_ARGU_EXTENSION ) ) );
		}
	}
	return ErrorHandler();
}

ErrorHandler ArgumentListModel::writeArgumentFile( const QString & file )
{
	QFile argureadfile( file + "." + LINKCFG_EXTENSION );
	if ( !argureadfile.open( QFile::WriteOnly | QFile::Text ) )
	{
		return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() <<
			file, "ArgumentListModel", "writeArgumentFile" );
	}

	QDomDocument t_doc;
	QDomElement t_root = t_doc.createElement( ARGUMENT_ROOT );

	t_root.setAttribute( ARGUMENT_ROOT_NAME, m_name );
	t_root.setAttribute( ARGUMENT_ROOT_VERSION,  m_version );
	t_root.setAttribute( ARGUMENT_ROOT_EXEC, m_exec);
	t_root.setAttribute( ARGUMENT_ROOT_COMPILE, m_compile );
	t_doc.appendChild( t_root );

	for ( int t_argu_index = 0; t_argu_index < rowCount(); t_argu_index ++ )
	{
		ArgumentListModelItem * t_item =
				(ArgumentListModelItem *)item( t_argu_index, 0 );
		if ( t_item->opt().isEmpty() )
		{
			continue;
		}
		QDomElement t_element = t_doc.createElement( ARGUMENT_ARGU );
		t_element.setAttribute( ARGUMENT_ARGU_ARGUMENT, t_item->opt() );
		t_element.setAttribute( ARGUMENT_ARGU_TYPE, t_item->optType() );
		t_element.setAttribute( ARGUMENT_ARGU_NOTE, t_item->note() );
		t_element.setAttribute( ARGUMENT_ARGU_LOOP, t_item->loop() );
		t_element.setAttribute( ARGUMENT_ARGU_VALUE, t_item->value() );
		t_element.setAttribute( ARGUMENT_ARGU_DEPARG, t_item->dependArg() );
		t_element.setAttribute( ARGUMENT_ARGU_DEPVAL,
			QString::number( t_item->dependTab() ) );
		t_element.setAttribute( ARGUMENT_ARGU_EXTENSION, t_item->extension() );

		t_root.appendChild( t_element );
	}

	QTextStream out( &argureadfile );
	t_doc.save( out, 4 );
	argureadfile.close();

	return ErrorHandler();
}

void ArgumentListModel::updateRow()
{
	QString bool_str;
	for ( int row = 0; row < rowCount(); row ++ )
	{
		ArgumentListModelItem * t_item =
			(ArgumentListModelItem *)item( row, 0 );
		if ( t_item->optType() == EsyArgumentItem::BOOL_TYPE )
		{
			if ( (bool)t_item->value().toInt() )
			{
				bool_str += t_item->opt() + " ";
			}
			else
			{
				QString t_opt = t_item->opt();
				t_opt.replace( "-", "-!" );
				bool_str += t_opt + " ";
			}
		}
	}

	for ( int row = 0; row < rowCount(); row ++ )
	{
		ArgumentListModelItem * t_item =
			(ArgumentListModelItem *)item( row, 0 );
		if ( t_item->dependArg() != "" )
		{
			QRegExp depend_exp( t_item->dependArg() );

			if ( depend_exp.indexIn( bool_str ) >= 0 )
			{
				t_item->setShown( true );
				emit showRow( row );
			}
			else
			{
				t_item->setShown( false );
				emit hideRow( row );
			}
		}
	}
}

QWidget *ArgumentListDelegate::createEditor( QWidget *parent,
	const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	Q_UNUSED( option )

	if ( index.column() == ArgumentListModel::ARGULIST_VALUE )
	{
		ArgumentListModel * t_model = (ArgumentListModel *)index.model();
		ArgumentListModelItem * t_item = t_model->arguItem( index.row() );
		// value line edit
		if ( t_item->optType() == EsyArgumentItem::BOOL_TYPE )
		{
			return 0;
		}
		else if ( t_item->optType() == EsyArgumentItem::NEW_FILE_TYPE )
		{
			QFileDialog * t_dialog = new QFileDialog( 0, "New File for Option",
				global_work_direction,
				"Specified file (*." + t_item->extension() + ")" );
			t_dialog->setAcceptMode( QFileDialog::AcceptSave );
			return t_dialog;
		}
		else if ( t_item->optType() == EsyArgumentItem::OPEN_FILE_TYPE )
		{
			QFileDialog * t_dialog = new QFileDialog( 0, "Open File for Option",
				global_work_direction,
				"Specified file (*." + t_item->extension() + ")" );
			t_dialog->setAcceptMode( QFileDialog::AcceptOpen );
			return t_dialog;
		}
		else if ( t_item->optType() == EsyArgumentItem::NEW_FILE_IF_TYPE )
		{
			QFileDialog * t_dialog = new QFileDialog( 0,
				"New Interface File for Option", global_work_direction,
				"Specified File (*." + t_item->extension() + "b *."
					+ t_item->extension() + "t)" );
			t_dialog->setAcceptMode( QFileDialog::AcceptSave );
			return t_dialog;
		}
		else if ( t_item->optType() == EsyArgumentItem::OPEN_FILE_IF_TYPE )
		{
			QFileDialog * t_dialog = new QFileDialog( 0,
				"Open Interface File for Option", global_work_direction,
				"Specified File (*." + t_item->extension() + "b *."
					+ t_item->extension() + "t)" );
			t_dialog->setAcceptMode( QFileDialog::AcceptOpen );
			return t_dialog;
		}
		else if ( t_item->optType() == EsyArgumentItem::LONG_TYPE ||
				  t_item->optType() == EsyArgumentItem::DOUBLE_TYPE ||
				  t_item->optType() == EsyArgumentItem::LONG_VECTOR_TYPE ||
				  t_item->optType() == EsyArgumentItem::DOUBLE_VECTOR_TYPE )
		{
			NumLineEdit::NumType t_num_type;
			switch( t_item->optType() )
			{
			case EsyArgumentItem::LONG_TYPE:
				t_num_type = NumLineEdit::INT_NUM; break;
			case EsyArgumentItem::DOUBLE_TYPE:
				t_num_type = NumLineEdit::DOUBLE_NUM; break;
			case EsyArgumentItem::LONG_VECTOR_TYPE:
				t_num_type = NumLineEdit::INT_NUM; break;
			case EsyArgumentItem::DOUBLE_VECTOR_TYPE:
				t_num_type = NumLineEdit::DOUBLE_NUM; break;
			default: t_num_type = NumLineEdit::INT_NUM; break;
			}
			NumLineEdit * valueedit = new NumLineEdit( t_num_type,
				t_item->value(), parent );
			valueedit->setFrame( false );
			return valueedit;
		}
		else
		{
			QLineEdit * valueedit = new QLineEdit( parent );
			valueedit->setFrame( false );
			return valueedit;
		}
	}
	else if ( index.column() == ArgumentListModel::ARGULIST_LOOP )
	{
		ArgumentListModel * t_model = (ArgumentListModel *)index.model();
		ArgumentListModelItem * t_item = t_model->arguItem( index.row() );
		if ( t_item->optType() == EsyArgumentItem::STRING_TYPE ||
			 t_item->optType() == EsyArgumentItem::DOUBLE_TYPE ||
			 t_item->optType() == EsyArgumentItem::LONG_TYPE ||
			 t_item->optType() == EsyArgumentItem::ENUM_TYPE )
		{
			QSpinBox * box = new QSpinBox( parent );
			box->setFrame( false );
			box->setMinimum( 0 );
			return box;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void ArgumentListDelegate::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	QString t_str = index.model()->data(index, Qt::DisplayRole).toString();

	if ( index.column() == ArgumentListModel::ARGULIST_VALUE )
	{
		ArgumentListModel * t_model = (ArgumentListModel *)index.model();
		ArgumentListModelItem * t_item = t_model->arguItem( index.row() );
		if ( t_item->optType() == EsyArgumentItem::NEW_FILE_TYPE ||
				  t_item->optType() == EsyArgumentItem::OPEN_FILE_TYPE ||
				  t_item->optType() == EsyArgumentItem::NEW_FILE_IF_TYPE ||
				  t_item->optType() == EsyArgumentItem::OPEN_FILE_IF_TYPE )
		{
			QFileDialog * valueedit = (QFileDialog *)editor;
			valueedit->selectFile( t_str );
		}
		else if ( t_item->optType() == EsyArgumentItem::LONG_TYPE ||
				  t_item->optType() == EsyArgumentItem::DOUBLE_TYPE ||
				  t_item->optType() == EsyArgumentItem::LONG_VECTOR_TYPE ||
				  t_item->optType() == EsyArgumentItem::DOUBLE_VECTOR_TYPE )
		{
			NumLineEdit * valueedit = (NumLineEdit *)editor;
			valueedit->setText( t_str );
		}
		else if ( t_item->optType() != EsyArgumentItem::BOOL_TYPE )
		{
			QLineEdit * valueedit = (QLineEdit *)editor;
			valueedit->setText( t_str );
		}
	}
	else if ( index.column() == ArgumentListModel::ARGULIST_LOOP )
	{
		ArgumentListModel * t_model = (ArgumentListModel *)index.model();
		ArgumentListModelItem * t_item = t_model->arguItem( index.row() );
		if ( t_item->optType() == EsyArgumentItem::STRING_TYPE ||
			 t_item->optType() == EsyArgumentItem::DOUBLE_TYPE ||
			 t_item->optType() == EsyArgumentItem::LONG_TYPE ||
			 t_item->optType() == EsyArgumentItem::ENUM_TYPE )
		{
			QSpinBox * box = (QSpinBox *)(editor);
			box->setValue( t_str.toInt() );
		}
	}
}

void ArgumentListDelegate::setModelData(QWidget *editor,
	QAbstractItemModel *model, const QModelIndex &index) const
{
	QVariant t_var;

	if ( index.column() == ArgumentListModel::ARGULIST_VALUE )
	{
		ArgumentListModel * t_model = (ArgumentListModel *)index.model();
		ArgumentListModelItem * t_item = t_model->arguItem( index.row() );

		if ( t_item->optType() == EsyArgumentItem::BOOL_TYPE )
		{
		}
		else if ( t_item->optType() == EsyArgumentItem::NEW_FILE_TYPE ||
				  t_item->optType() == EsyArgumentItem::OPEN_FILE_TYPE ||
				  t_item->optType() == EsyArgumentItem::NEW_FILE_IF_TYPE ||
				  t_item->optType() == EsyArgumentItem::OPEN_FILE_IF_TYPE )
		{
			QFileDialog * valueedit = (QFileDialog *)editor;
			if ( valueedit->selectedFiles().size() > 0 )
			{
				QString t_str = valueedit->selectedFiles()[ 0 ];
				t_var = t_str.left( t_str.lastIndexOf( "." ) );
			}
		}
		else if ( t_item->optType() == EsyArgumentItem::LONG_TYPE ||
				  t_item->optType() == EsyArgumentItem::DOUBLE_TYPE ||
				  t_item->optType() == EsyArgumentItem::LONG_VECTOR_TYPE ||
				  t_item->optType() == EsyArgumentItem::DOUBLE_VECTOR_TYPE )
		{
			NumLineEdit * valueedit = (NumLineEdit *)editor;
			t_var = valueedit->text();
		}
		else
		{
			QLineEdit * valueedit = (QLineEdit *)editor;
			t_var = valueedit->text();
		}
	}
	else if ( index.column() == ArgumentListModel::ARGULIST_LOOP )
	{
		ArgumentListModel * t_model = (ArgumentListModel *)index.model();
		ArgumentListModelItem * t_item = t_model->arguItem( index.row() );
		if ( t_item->optType() == EsyArgumentItem::STRING_TYPE ||
			 t_item->optType() == EsyArgumentItem::DOUBLE_TYPE ||
			 t_item->optType() == EsyArgumentItem::LONG_TYPE ||
			 t_item->optType() == EsyArgumentItem::ENUM_TYPE )
		{
			QSpinBox * box = (QSpinBox *)(editor);
			t_var = box->value();
		}
	}

	model->setData( index, t_var, Qt::EditRole );
}

void ArgumentListDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED( index )
	editor->setGeometry(option.rect);
}

ArgumentListTableView::ArgumentListTableView(QWidget *parent) :
	QTableView( parent )
{
	horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
	verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
	setAlternatingRowColors( true );
	setItemDelegate( new ArgumentListDelegate( this ) );
	connect( horizontalHeader(), SIGNAL( sectionResized(int,int,int) ),
			 this, SLOT( resizeRowsToContents() ) );
	connect( horizontalHeader(), SIGNAL( sectionResized(int,int,int) ),
			 this, SLOT( updateColumnWidth() ) );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
}

void ArgumentListTableView::updateColumnWidth()
{
	int col_width = width() - verticalHeader()->width() -
		columnWidth( ArgumentListModel::ARGULIST_OPT ) -
		columnWidth( ArgumentListModel::ARGULIST_LOOP ) -
		columnWidth( ArgumentListModel::ARGULIST_VALUE ) -
		verticalScrollBar()->width();
	setColumnWidth( ArgumentListModel::ARGULIST_NOTE, col_width );
}

void ArgumentListTableView::setModel(QAbstractItemModel *model)
{
	QTableView::setModel( model );

	ArgumentListModel * t_model = (ArgumentListModel *)model;
	connect( t_model, SIGNAL(showRow(int)), this, SLOT(showRow(int)) );
	connect( t_model, SIGNAL(hideRow(int)), this, SLOT(hideRow(int)) );

	resizeColumnsToContents();
	updateColumnWidth();

	t_model->updateRow();
}

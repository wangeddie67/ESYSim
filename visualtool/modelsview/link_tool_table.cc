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

#include "link_tool_table.h"
#include "esy_argument.h"

#include "../modelsview/argument_list_table.h"

#define LINKTOOL_FILE_PATH  "configxml/linktools.xml"

#define LINKTOOL_ROOT  "Toolsconfiguration"
#define LINKTOOL_PROCESSING_XML "xml"
#define LINKTOOL_PROCESSING_VALUE "version=\"1.0\" encoding=\"UTF-8\""
#define LINKTOOL_TOOL  "Tool"
#define LINKTOOL_TOOL_NAME  "name"
#define LINKTOOL_TOOL_FATHER  "father"
#define LINKTOOL_TOOL_VERSION  "version"
#define LINKTOOL_TOOL_DIRECTION  "direction"
#define LINKTOOL_TOOL_EXEC  "exec"
#define LINKTOOL_TOOL_CONFIGNAME  "configName"
#define LINKTOOL_TOOL_COMPILE  "compile"

#define LINKTOOL_XML_INDENT   4

QStringList LinkToolModelItem::const_category_options =
	QStringList() << "Configuration" << "Simulation" << "Debug" << "Analysis";

int LinkToolModelItem::categoryNum() const
{
	for ( int i = 0; i < const_category_options.size(); i ++ )
	{
		if ( const_category_options[ i ] == m_category )
		{
			return i;
		}
	}
	return -1;
}

QStringList LinkToolModel::const_viewheader_list = QStringList() <<
	"Name" << "Category" << "Version" << "Direction" << "Execution" <<
    "Configuration" << "Compiler";

ErrorHandler LinkToolModelItem::readArgumentFile( const QString & file )
{
    ArgumentListModel t_model( false, 0 );
	ErrorHandler t_error = t_model.readArgumentFile( file );
	if ( !t_error.hasError() )
	{
		m_name = t_model.name();
		m_version = t_model.version();
		m_direction = QDir().relativeFilePath(
			QFileInfo( file ).absoluteDir().absolutePath() );
		m_exec = t_model.exec();
		m_config = QFileInfo( file ).fileName();
		m_compile = t_model.compile();
	}

	return ErrorHandler();
}

int LinkToolModel::rowCount( const QModelIndex &parent ) const
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

int LinkToolModel::columnCount( const QModelIndex &parent ) const
{
	Q_UNUSED( parent )
	return LINKTOOL_COLUMN_COUNT;
}

QVariant LinkToolModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();
	int col = index.column();

	switch( role )
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		{
			LinkToolModelItem * t_item = (LinkToolModelItem *)item( row, 0 );

			QString t_str;
			switch( col )
			{
			case LINKTOOL_NAME: t_str = t_item->name(); break;
			case LINKTOOL_CATEGORY: t_str = t_item->category(); break;
			case LINKTOOL_VERSION: t_str = t_item->version(); break;
			case LINKTOOL_DIR: t_str = t_item->direction(); break;
			case LINKTOOL_EXEC: t_str = t_item->exec(); break;
			case LINKTOOL_CONFIG: t_str = t_item->config(); break;
			case LINKTOOL_COMPILE: t_str = t_item->compile(); break;
			}
			return t_str;
			break;
		}
	case Qt::CheckStateRole:
		{
			LinkToolModelItem * t_item = (LinkToolModelItem *)item( row, 0 );
			if ( col == LINKTOOL_NAME )
			{
				return t_item->checkState();
			}
		}
	}
	return QVariant();
}

bool LinkToolModel::setData(
		const QModelIndex &index, const QVariant &value, int role)
{
	int row = index.row();
	int col = index.column();

	switch( role )
	{
	case Qt::EditRole:
		{
			LinkToolModelItem * t_item = (LinkToolModelItem *)item( row, 0 );
			if ( col == LINKTOOL_CATEGORY )
			{
				t_item->setCategory( value.toString() );
				emit dataChanged( index, index );
				return true;
			}
			return false;
		}
	case Qt::CheckStateRole:
		{
			LinkToolModelItem * t_item = (LinkToolModelItem *)item( row, 0 );
			if ( col == LINKTOOL_NAME )
			{
				t_item->setCheckState( (Qt::CheckState)value.toInt() );
				emit dataChanged( index, index );
				return true;
			}
			return false;
		}
	}

	return false;
}

QVariant LinkToolModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	switch( role )
	{
	case Qt::DisplayRole:
		{
			if ( orientation == Qt::Horizontal )
			{
                if ( section < LINKTOOL_COLUMN_COUNT )
                {
                    return const_viewheader_list[ section ];
                }
                else
                {
                    return  "";
                }
                break;
			}
		}
	}
	return QVariant();
}

Qt::ItemFlags LinkToolModel::flags( const QModelIndex &index ) const
{
	if ( !index.isValid() )
	{
		return 0;
	}

	if ( index.column() == LINKTOOL_NAME )
	{
		return  Qt::ItemIsEnabled | Qt::ItemIsSelectable |
				Qt::ItemIsUserCheckable;
	}
	else if ( index.column() == LINKTOOL_CATEGORY )
	{
		return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	}
	else
	{
		return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
}

void LinkToolModel::appendItem( LinkToolModelItem * item )
{
	QList< QStandardItem *> items;
	items << item;
	for ( int i = 1; i < LINKTOOL_COLUMN_COUNT; i ++ )
	{
		items << new QStandardItem;
	}
	appendRow( items );
}

ErrorHandler LinkToolModel::readLinkToolFile()
{
	QFile linktoolsfile( LINKTOOL_FILE_PATH );
	if ( !linktoolsfile.open( QFile::ReadOnly | QFile::Text ) )
	{
		return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() <<
			LINKTOOL_FILE_PATH, "LinkToolModel", "readLinkTool" );
	}

	QString errorMsg;
	int errorLine;
	int errorColumn;
	QDomDocument linktool_doc;
	if ( !linktool_doc.setContent(
			 &linktoolsfile, &errorMsg, &errorLine, &errorColumn ) )
	{
		linktoolsfile.close();
		return ErrorHandler( ErrorHandler::DOMERROR, QStringList() <<
			"linktools.xml" << errorMsg << QString::number( errorLine ) <<
			QString::number( errorColumn ), "LinkToolModel", "readFile" );
	}
	linktoolsfile.close();

	QDomElement linktool_head = linktool_doc.documentElement();
	if ( linktool_head.tagName() != LINKTOOL_ROOT )
	{
		return ErrorHandler( ErrorHandler::XMLTAGERROR, QStringList() <<
			"linktools.xml" << linktool_head.tagName(), "LinkToolModel",
			"readFile" );
	}

	QDomNodeList linktool_list = linktool_head.elementsByTagName( LINKTOOL_TOOL );
	for ( int i = 0; i < linktool_list.size(); i ++ )
	{
		QDomElement linktool_item = linktool_list.at( i ).toElement();
		QDir dir( linktool_item.attribute( LINKTOOL_TOOL_DIRECTION ) );
		QString argufile = dir.absoluteFilePath(
			linktool_item.attribute( LINKTOOL_TOOL_CONFIGNAME ) );

		LinkToolModelItem * model_item = new LinkToolModelItem();
		ErrorHandler t_error = model_item->readArgumentFile( argufile );

		if ( t_error.hasError() )
		{
			qDebug() << t_error.text() << endl;
			delete model_item;
		}
		else
		{
			model_item->setCategory(
						linktool_item.attribute( LINKTOOL_TOOL_FATHER ) );
			appendItem( model_item );
		}
	}

	return ErrorHandler();
}

ErrorHandler LinkToolModel::writeLinkToolFile()
{
	QFile linktoolsfile( LINKTOOL_FILE_PATH );
	if ( !linktoolsfile .open( QFile::WriteOnly | QFile::Text ) )
	{
		return ErrorHandler( ErrorHandler::FILEOPENERROR,
			QStringList() << "linktools.xml", "LinkToolModel", "writeFile" );
	}

	QDomDocument t_doc;
	QDomProcessingInstruction t_process = t_doc.createProcessingInstruction(
		LINKTOOL_PROCESSING_XML, LINKTOOL_PROCESSING_VALUE );
	t_doc.appendChild( t_process );

	QDomElement t_root = t_doc .createElement( LINKTOOL_ROOT );
	t_doc .appendChild( t_root );

	for ( int i = 0; i < rowCount(); i ++ )
	{
		LinkToolModelItem *t_item = linkToolItem( i );
		if ( t_item == 0 )
		{
			continue;
		}

		QDomElement t_element = t_doc .createElement( LINKTOOL_TOOL );
		t_element.setAttribute( LINKTOOL_TOOL_FATHER, t_item->category() );
		t_element.setAttribute( LINKTOOL_TOOL_DIRECTION, t_item->direction() );
		t_element.setAttribute( LINKTOOL_TOOL_CONFIGNAME, t_item->config() );
		t_root.appendChild( t_element );
	}

	QTextStream t_text( &linktoolsfile );
	t_doc.save( t_text, LINKTOOL_XML_INDENT );

	return ErrorHandler();
}

QString LinkToolModel::renewScript()
{
	QString renew_shell;
	QDir dir;
	QString pwd = dir.currentPath();

	int total = 0;
	for ( int i = 0; i < rowCount(); i ++ )
	{
		LinkToolModelItem * linktool_item = linkToolItem( i );
		if ( linktool_item == 0 )
		{
			continue;
		}
		if ( linktool_item->checkState() == Qt::Checked )
		{
			total ++;
		}
	}

	int seq = 0;
	for ( int i = 0; i < rowCount(); i ++ )
	{
		LinkToolModelItem * linktool_item = linkToolItem( i );
		if ( linktool_item == 0 )
		{
			continue;
		}
		if ( linktool_item->checkState() == Qt::Checked )
		{
			renew_shell += "echo\n";
			renew_shell += "echo \"compile tool : " + linktool_item->name() +
				" verison: " + linktool_item->version() + ("\"\n");
			renew_shell += "cd " + linktool_item->direction() + "\n";
			renew_shell += linktool_item->compile() + " clean\n";
			renew_shell += linktool_item->compile() + "\n";
			renew_shell += "cd " + pwd + "\n";
			seq ++;
			renew_shell += "echo PP" +
					QString::number( seq * 100 / total ) + "\n";
		}
	}
	return renew_shell;
}

QWidget *LinkToolDelegate::createEditor( QWidget *parent,
	const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	Q_UNUSED( option )
	Q_UNUSED( index )

	if ( index.column() == LinkToolModel::LINKTOOL_CATEGORY &&
		 m_type == LINKTOOLVIEW_EDIT )
	{
		QComboBox * box = new QComboBox( parent );
		box->setFrame( false );
		box->addItems( LinkToolModelItem::categoryStringList() );

		return box;
	}
	else
	{
		return 0;
	}
}

void LinkToolDelegate::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	QString text = index.model()->data(index, Qt::EditRole).toString();

	if ( index.column() == LinkToolModel::LINKTOOL_CATEGORY )
	{
		QComboBox *box = (QComboBox *)(editor);
		box->setCurrentText( text );
	}
}

void LinkToolDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	if ( index.column() == LinkToolModel::LINKTOOL_CATEGORY )
	{
		QComboBox *box = (QComboBox *)(editor);
		model->setData( index, box->currentText(), Qt::EditRole );
	}
}

void LinkToolDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED( index )
	editor->setGeometry(option.rect);
}

LinkToolTableView::LinkToolTableView(QWidget *parent) :
	QTableView( parent )
{
	verticalHeader()->setHidden( true );
	setAlternatingRowColors( true );
	setSelectionBehavior( QAbstractItemView::SelectRows );
	horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
}

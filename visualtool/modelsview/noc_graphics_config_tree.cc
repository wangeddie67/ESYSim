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

#include "graphics_config_tree.h"
#include "esy_argument.h"

#include "../modelsview/argument_list_table.h"

#define GRAPHICS_FILE_PATH  "configxml/graphics_config.xml"

#define GRAPHICS_ROOT  "Graphicsconfiguration"
#define GRAPHICS_PROCESSING_XML "xml"
#define GRAPHICS_PROCESSING_VALUE "version=\"1.0\" encoding=\"UTF-8\""
#define GRAPHICS_ITEM  "Item"
#define GRAPHICS_ITEM_NAME  "name"
#define GRAPHICS_STYLE  "Style"
#define GRAPHICS_STYLE_NAME  "Style"
#define GRAPHICS_STYLE_PENCOLOR  "PenColor"
#define GRAPHICS_STYLE_PENWIDTH  "PenWidth"
#define GRAPHICS_STYLE_PENSTYLE  "PenStyle"
#define GRAPHICS_STYLE_BRUSHCOLOR  "BrushColor"
#define GRAPHICS_STYLE_BRUSHSTYLE  "BrushStyle"

#define GRAPHICS_XML_INDENT   4

QStringList GraphicsConfigModel::const_viewheader_list = QStringList() <<
    "Item" << "Pen color" << "Pen width" << "Pen style" << "Brush color" <<
    "Brush style";

QStringList GraphicsConfigDelegate::const_brushstyle_options = QStringList() <<
    "Solid" << "Dense1" << "Dense2" << "Dense3" << "Dense4" << "Dense5" <<
    "Dense6" << "Dense7" << "Hor" << "Ver" << "Cross" << "BDiag" << "FDiag" <<
    "DiagCross";

QStringList GraphicsConfigDelegate::const_penstyle_options = QStringList() <<
    "Solid" << "Dash" << "Dot" << "DashDot" << "DashDotDot";

void GraphicsConfigModelItem::appendItem( GraphicsConfigModelItem * item )
{
    QList< QStandardItem *> items;
    items << item;
    for ( int i = 1; i < GraphicsConfigModel::GRAPHICS_COLUMN_COUNT; i ++ )
    {
        items << new QStandardItem;
    }
    appendRow( items );
}

void GraphicsConfigModelItem::readFile( QDomElement * root )
{
    m_style = root->attribute( GRAPHICS_STYLE_NAME );
    QDomNodeList t_cfg_list = root->childNodes();
    for ( int cfg_index = 0; cfg_index < t_cfg_list.count(); cfg_index ++ )
    {
        QDomElement t_cfg_element = t_cfg_list.at( cfg_index ).toElement();
        if ( t_cfg_element.tagName() == GRAPHICS_STYLE_PENCOLOR )
        {
            m_pen_color = QColor( t_cfg_element.text().toUInt() );
        }
        else if ( t_cfg_element.tagName() == GRAPHICS_STYLE_PENWIDTH )
        {
            m_pen_width = t_cfg_element.text().toDouble();
        }
        else if ( t_cfg_element.tagName() == GRAPHICS_STYLE_PENSTYLE )
        {
            m_pen_style = (Qt::PenStyle)t_cfg_element.text().toInt();
        }
        else if ( t_cfg_element.tagName() == GRAPHICS_STYLE_BRUSHCOLOR )
        {
            m_brush_color = QColor( t_cfg_element.text().toUInt() );
        }
        else if ( t_cfg_element.tagName() == GRAPHICS_STYLE_BRUSHSTYLE )
        {
            m_brush_style = (Qt::BrushStyle)t_cfg_element.text().toInt();
        }
    }
}

void GraphicsConfigModelItem::writeFile( QDomDocument * doc, QDomElement * root )
{
    root->setAttribute( GRAPHICS_STYLE_NAME, m_style );

    QDomElement t_pen_color_item =
        doc->createElement( GRAPHICS_STYLE_PENCOLOR );
    QDomText t_pen_color_text = doc->createTextNode(
        QString::number( m_pen_color.rgba() ) );
    t_pen_color_item.appendChild( t_pen_color_text );
    root->appendChild( t_pen_color_item );

    QDomElement t_pen_width_item =
        doc->createElement( GRAPHICS_STYLE_PENWIDTH );
    QDomText t_pen_width_text = doc->createTextNode(
        QString::number( m_pen_width ) );
    t_pen_width_item.appendChild( t_pen_width_text );
    root->appendChild( t_pen_width_item );

    QDomElement t_pen_style_item =
        doc->createElement( GRAPHICS_STYLE_PENSTYLE );
    QDomText t_pen_style_text = doc->createTextNode(
        QString::number( m_pen_style ) );
    t_pen_style_item.appendChild( t_pen_style_text );
    root->appendChild( t_pen_style_item );

    QDomElement t_brush_color_item =
        doc->createElement( GRAPHICS_STYLE_BRUSHCOLOR );
    QDomText t_brush_color_text = doc->createTextNode(
        QString::number( m_brush_color.rgba() ) );
    t_brush_color_item.appendChild( t_brush_color_text );
    root->appendChild( t_brush_color_item );

    QDomElement t_brush_style_item =
        doc->createElement( GRAPHICS_STYLE_BRUSHSTYLE );
    QDomText t_brush_style_text = doc->createTextNode(
        QString::number( m_brush_style ) );
    t_brush_style_item.appendChild( t_brush_style_text );
    root->appendChild( t_brush_style_item );
}

int GraphicsConfigModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent == QModelIndex() )
    {
        return invisibleRootItem()->rowCount();
    }
    else if ( parent.parent() == QModelIndex() )
    {
        return itemFromIndex( parent )->rowCount();
    }
    else
    {
        return 0;
    }
}

int GraphicsConfigModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent )
    return GRAPHICS_COLUMN_COUNT;
}

QVariant GraphicsConfigModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    switch( role )
    {
    case Qt::DisplayRole:
        {
            QString t_str;
            if ( index.parent() == QModelIndex() )
            {
                GraphicsConfigModelItem * t_item =
                    (GraphicsConfigModelItem *)item( row, 0 );
                if ( index.column() == GRAPHICS_NAME )
                {
                    t_str = t_item->item();
                }
            }
            else
            {
                GraphicsConfigModelItem * t_item = (GraphicsConfigModelItem *)
                    item( index.parent().row() )->child( row, 0 );
                switch( col )
                {
                case GRAPHICS_NAME: t_str = t_item->style();
                    break;
                case GRAPHICS_PENCOLOR:
                    t_str = QString::number( t_item->penColor().rgba() );
                    break;
                case GRAPHICS_PENWIDTH:
                    t_str = QString::number( t_item->penWidth() );
                    break;
                case GRAPHICS_PENSTYLE: t_str =
                    GraphicsConfigDelegate::const_penstyle_options[
                        t_item->penStyle() - 1 ]; break;
                case GRAPHICS_BRUSHCOLOR:
                    t_str = QString::number( t_item->brushColor().rgba() );
                    break;
                case GRAPHICS_BRUSHSTYLE: t_str =
                    GraphicsConfigDelegate::const_brushstyle_options[
                        t_item->brushStyle() - 1 ];
                    break;
                }
            }
            return t_str;
            break;
        }
    case Qt::EditRole:
        {
            GraphicsConfigModelItem * t_item = (GraphicsConfigModelItem *)
                item( index.parent().row() )->child( row, 0 );

            QString t_str;
            switch( col )
            {
            case GRAPHICS_PENCOLOR:
                t_str = QString::number( t_item->penColor().rgba() ); break;
            case GRAPHICS_PENWIDTH:
                t_str = QString::number( t_item->penWidth() ); break;
            case GRAPHICS_PENSTYLE:
                t_str = QString::number( t_item->penStyle() ); break;
            case GRAPHICS_BRUSHCOLOR:
                t_str = QString::number( t_item->brushColor().rgba() ); break;
            case GRAPHICS_BRUSHSTYLE:
                t_str = QString::number( t_item->brushStyle() ); break;
            }
            return t_str;
            break;
        }
    }
    return QVariant();
}

bool GraphicsConfigModel::setData(const QModelIndex &index,
                                      const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();

    switch( role )
    {
    case Qt::EditRole:
        {
            GraphicsConfigModelItem * t_item = (GraphicsConfigModelItem *)
                item( index.parent().row() )->child( row, 0 );

            switch( col )
            {
            case GRAPHICS_PENCOLOR:
                t_item->setPenColor( QColor( value.toUInt() ) ); break;
            case GRAPHICS_PENWIDTH:
                t_item->setPenWidth( value.toDouble() ); break;
            case GRAPHICS_PENSTYLE: t_item->setPenStyle(
                (Qt::PenStyle)(value.toInt() ) ); break;
            case GRAPHICS_BRUSHCOLOR:
                t_item->setBrushColor( QColor( value.toUInt() ) ); break;
            case GRAPHICS_BRUSHSTYLE: t_item->setBrushStyle(
                (Qt::BrushStyle)(value.toInt() ) ); break;
            }
            break;
        }
    }

    return false;
}

QVariant GraphicsConfigModel::headerData(int section,
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
        }
    }
    return QVariant();
}

Qt::ItemFlags GraphicsConfigModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() )
    {
        return 0;
    }

    if ( index.column() == GRAPHICS_NAME || index.parent() == QModelIndex() )
    {
        return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else
    {
        return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
}

void GraphicsConfigModel::appendItem( GraphicsConfigModelItem * item )
{
    QList< QStandardItem *> items;
    items << item;
    for ( int i = 1; i < GRAPHICS_COLUMN_COUNT; i ++ )
    {
        items << new QStandardItem;
    }
    appendRow( items );
}

GraphicsConfigModelItem * GraphicsConfigModel::configItem(
        const QString &item_name, const QString &style )
{
    for ( int i = 0; i < invisibleRootItem()->rowCount(); i ++ )
    {
        GraphicsConfigModelItem * model_item =
                (GraphicsConfigModelItem * )item( i, 0 );
        for ( int j = 0; j < model_item->rowCount(); j ++ )
        {
            GraphicsConfigModelItem * style_item =
                   ( (GraphicsConfigModelItem * )model_item->child( j, 0 ) );
            if ( style_item->item() == item_name &&
                 style_item->style() == style )
            {
                return style_item;
            }
        }
    }
    return new GraphicsConfigModelItem;
}

void GraphicsConfigModel::copy( GraphicsConfigModel * model )
{
    clear();
    for ( int i = 0; i < model->invisibleRootItem()->rowCount(); i ++ )
    {
        GraphicsConfigModelItem * model_item =
                (GraphicsConfigModelItem * )model->item( i, 0 );
        GraphicsConfigModelItem * new_model_item =
                new GraphicsConfigModelItem;
        new_model_item->setItem( model_item->item() );
        appendItem( new_model_item );

        for ( int j = 0; j < model_item->rowCount(); j ++ )
        {
            GraphicsConfigModelItem * style_item =
                    (GraphicsConfigModelItem * )model_item->child( j, 0 );
            GraphicsConfigModelItem * new_style_item =
                new GraphicsConfigModelItem( style_item->item(),
                    style_item->style(), style_item->penColor(),
                    style_item->penWidth(), style_item->penStyle(),
                    style_item->brushColor(), style_item->brushStyle() );
            new_model_item->appendItem( new_style_item );
        }
    }
    return;
}

ErrorHandler GraphicsConfigModel::readFile()
{
    QFile grahpicsconfigfile( GRAPHICS_FILE_PATH );
    if ( !grahpicsconfigfile.open( QFile::ReadOnly | QFile::Text ) )
    {
        return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() <<
            GRAPHICS_FILE_PATH, "GraphicsConfigModel", "readFile" );
    }

    QString errorMsg;
    int errorLine;
    int errorColumn;
    QDomDocument graphics_config_doc;
    if ( !graphics_config_doc.setContent( &grahpicsconfigfile,
                                   &errorMsg, &errorLine, &errorColumn ) )
    {
        grahpicsconfigfile.close();
        return ErrorHandler( ErrorHandler::DOMERROR, QStringList() <<
            GRAPHICS_FILE_PATH << errorMsg << QString::number( errorLine ) <<
            QString::number( errorColumn ), "GraphicsConfigModel", "readFile" );
    }
    grahpicsconfigfile.close();

    QDomElement graphics_config_head = graphics_config_doc.documentElement();
    if ( graphics_config_head.tagName() != GRAPHICS_ROOT )
    {
        return ErrorHandler( ErrorHandler::XMLTAGERROR, QStringList() <<
            "linktools.xml" << graphics_config_head.tagName(),
            "GraphicsConfigModel", "readFile" );
    }

    QDomNodeList graphics_config_list =
            graphics_config_head.elementsByTagName( GRAPHICS_ITEM );
    for ( int i = 0; i < graphics_config_list.size(); i ++ )
    {
        QDomElement graphics_config_item =
                graphics_config_list.at( i ).toElement();
        GraphicsConfigModelItem * model_item = new GraphicsConfigModelItem();
        model_item->setItem(
                    graphics_config_item.attribute( GRAPHICS_ITEM_NAME ) );
        appendItem( model_item );

        QDomNodeList graphics_config_style_list =
                graphics_config_item.elementsByTagName( GRAPHICS_STYLE );
        for ( int j = 0; j < graphics_config_style_list.size(); j ++ )
        {
            QDomElement t_element =
                    graphics_config_style_list.at( j ).toElement();
            GraphicsConfigModelItem * t_item = new GraphicsConfigModelItem();
            t_item->setItem( model_item->item() );
            t_item->readFile( &t_element );
            model_item->appendItem( t_item );
        }
    }

    return ErrorHandler();
}

ErrorHandler GraphicsConfigModel::writeFile()
{
    QFile grahpicsconfigfile( GRAPHICS_FILE_PATH );
    if ( !grahpicsconfigfile.open( QFile::WriteOnly | QFile::Text ) )
    {
        return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() <<
            GRAPHICS_FILE_PATH, "GraphicsConfigModel", "writeFile" );
    } /* if ( !linktoolsfile .open( QFile::WriteOnly | QFile::Text ) ) */

    QDomDocument t_doc;
    QDomProcessingInstruction t_process = t_doc.createProcessingInstruction(
        GRAPHICS_PROCESSING_XML, GRAPHICS_PROCESSING_VALUE );
    t_doc.appendChild( t_process );

    QDomElement t_root = t_doc.createElement( GRAPHICS_ROOT );
    t_doc.appendChild( t_root );

    for ( int i = 0; i < invisibleRootItem()->rowCount(); i ++ )
    {
        GraphicsConfigModelItem *t_item = (GraphicsConfigModelItem *)item( i );
        if ( t_item == 0 )
        {
            continue;
        }

        QDomElement t_element = t_doc.createElement( GRAPHICS_ITEM );
        t_element.setAttribute( GRAPHICS_ITEM_NAME, t_item->item() );
        for ( int j = 0; j < t_item->rowCount(); j ++ )
        {
            GraphicsConfigModelItem * t_style_item =
                    (GraphicsConfigModelItem *)t_item->child( j );
            QDomElement t_style_element = t_doc.createElement( GRAPHICS_STYLE );
            t_style_item->writeFile( &t_doc, &t_style_element );
            t_element.appendChild( t_style_element );
        }
        t_root.appendChild( t_element );
    }

    QTextStream t_text( &grahpicsconfigfile );
    t_doc.save( t_text, GRAPHICS_XML_INDENT );

    return ErrorHandler();
}

void GraphicsConfigModel::setDefaultValue()
{
    clear();
    GraphicsConfigModelItem * router_item = new GraphicsConfigModelItem();
    router_item->setItem( "Router" );
    appendItem( router_item );
    router_item->appendItem( new GraphicsConfigModelItem( "Router", "Normal",
        Qt::black, 1, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    router_item->appendItem( new GraphicsConfigModelItem( "Router", "Selected",
        Qt::black, 3, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    router_item->appendItem( new GraphicsConfigModelItem( "Router", "Faulty",
        Qt::red, 3, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );

    GraphicsConfigModelItem * port_item = new GraphicsConfigModelItem();
    port_item->setItem( "Port" );
    appendItem( port_item );
    port_item->appendItem( new GraphicsConfigModelItem( "Port", "Normal",
        Qt::black, 1, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    port_item->appendItem( new GraphicsConfigModelItem( "Port", "Faulty",
        Qt::red, 1, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );

    GraphicsConfigModelItem * require_item = new GraphicsConfigModelItem();
    require_item->setItem( "Require" );
    appendItem( require_item );
    require_item->appendItem( new GraphicsConfigModelItem( "Require", "Normal",
        Qt::black, 1, Qt::DashLine, Qt::white, Qt::SolidPattern ) );
    require_item->appendItem( new GraphicsConfigModelItem( "Require", "Faulty",
        Qt::red, 10, Qt::DashLine, Qt::white, Qt::SolidPattern ) );

    GraphicsConfigModelItem * assign_item = new GraphicsConfigModelItem();
    assign_item->setItem( "Assign" );
    appendItem( assign_item );
    assign_item->appendItem( new GraphicsConfigModelItem( "Assign", "Normal",
        Qt::black, 3, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    assign_item->appendItem( new GraphicsConfigModelItem( "Assign", "Faulty",
        Qt::red, 10, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );

    GraphicsConfigModelItem * ni_item = new GraphicsConfigModelItem();
    ni_item->setItem( "NI" );
    appendItem( ni_item );
    ni_item->appendItem( new GraphicsConfigModelItem( "NI", "Normal",
        Qt::black, 1, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    ni_item->appendItem( new GraphicsConfigModelItem( "NI", "Faulty",
        Qt::red, 1, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );

    GraphicsConfigModelItem * link_item = new GraphicsConfigModelItem();
    link_item->setItem( "Link" );
    appendItem( link_item );
    link_item->appendItem( new GraphicsConfigModelItem( "Link", "Normal",
        Qt::black, 2, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    link_item->appendItem( new GraphicsConfigModelItem( "Link", "Faulty",
        Qt::red, 10, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );

    GraphicsConfigModelItem * path_item = new GraphicsConfigModelItem();
    path_item->setItem( "Path" );
    appendItem( path_item );
    path_item->appendItem( new GraphicsConfigModelItem( "Path", "Normal",
        Qt::blue, 3, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    path_item->appendItem( new GraphicsConfigModelItem( "Path", "Faulty",
        Qt::red, 10, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );

    GraphicsConfigModelItem * flit_item = new GraphicsConfigModelItem();
    flit_item->setItem( "Flit" );
    appendItem( flit_item );
    flit_item->appendItem( new GraphicsConfigModelItem( "Flit", "Head",
        Qt::black, 1, Qt::SolidLine, Qt::green, Qt::SolidPattern ) );
    flit_item->appendItem( new GraphicsConfigModelItem( "Flit", "Body",
        Qt::black, 1, Qt::SolidLine, Qt::white, Qt::SolidPattern ) );
    flit_item->appendItem( new GraphicsConfigModelItem( "Flit", "Tail",
        Qt::black, 1, Qt::SolidLine, Qt::blue, Qt::SolidPattern ) );
    flit_item->appendItem( new GraphicsConfigModelItem( "Flit", "Ack",
        Qt::black, 1, Qt::SolidLine, Qt::yellow, Qt::SolidPattern ) );
    flit_item->appendItem( new GraphicsConfigModelItem( "Flit", "Drop",
        Qt::red, 1, Qt::SolidLine, Qt::red, Qt::SolidPattern ) );
}

QWidget *GraphicsConfigDelegate::createEditor( QWidget *parent,
    const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    Q_UNUSED( option )
    Q_UNUSED( index )

    switch( index.column() )
    {
    case GraphicsConfigModel::GRAPHICS_PENCOLOR:
    case GraphicsConfigModel::GRAPHICS_BRUSHCOLOR:
        {
            QColorDialog * box = new QColorDialog( parent );
            box->setOptions( QColorDialog::ShowAlphaChannel |
                             QColorDialog::DontUseNativeDialog );
            box->setWindowTitle( "Select Color" );
            return box;
            break;
        }
    case GraphicsConfigModel::GRAPHICS_PENWIDTH:
        {
            NumLineEdit * box =
                new NumLineEdit( NumLineEdit::DOUBLE_NUM, "", parent );
            box->setFrame( false );
            return box;
            break;
        }
    case GraphicsConfigModel::GRAPHICS_PENSTYLE:
        {
            QComboBox * box = new QComboBox( parent );
            box->setFrame( false );
            box->addItems( const_penstyle_options );
            return box;
            break;
        }
    case GraphicsConfigModel::GRAPHICS_BRUSHSTYLE:
        {
            QComboBox * box = new QComboBox( parent );
            box->setFrame( false );
            box->addItems( const_brushstyle_options );
            return box;
            break;
        }
    }

    return 0;
}

void GraphicsConfigDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, Qt::EditRole);

    switch( index.column() )
    {
    case GraphicsConfigModel::GRAPHICS_PENCOLOR:
    case GraphicsConfigModel::GRAPHICS_BRUSHCOLOR:
        {
            QColorDialog * box = (QColorDialog *)editor;
            box->setCurrentColor( QColor( value.toUInt() ) );
            break;
        }
    case GraphicsConfigModel::GRAPHICS_PENWIDTH:
        {
            NumLineEdit * box = (NumLineEdit *)editor;
            box->setText( value.toString() );
            break;
        }
    case GraphicsConfigModel::GRAPHICS_PENSTYLE:
    case GraphicsConfigModel::GRAPHICS_BRUSHSTYLE:
        {
            QComboBox *box = (QComboBox *)(editor);
            box->setCurrentIndex( value.toInt() - 1 );
            break;
        }
    }
}

void GraphicsConfigDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant value;

    switch( index.column() )
    {
    case GraphicsConfigModel::GRAPHICS_PENCOLOR:
    case GraphicsConfigModel::GRAPHICS_BRUSHCOLOR:
    {
        QColorDialog * box = (QColorDialog *)editor;
        value.setValue( box->currentColor().rgba() );
        break;
    }
    case GraphicsConfigModel::GRAPHICS_PENWIDTH:
        {
            NumLineEdit * box = (NumLineEdit *)editor;
            value.setValue( box->text() );
            break;
        }
    case GraphicsConfigModel::GRAPHICS_PENSTYLE:
    case GraphicsConfigModel::GRAPHICS_BRUSHSTYLE:
        {
            QComboBox *box = (QComboBox *)(editor);
            value.setValue( box->currentIndex() + 1 );
            break;
        }
    }

    model->setData( index, value, Qt::EditRole );
}

void GraphicsConfigDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
    Q_UNUSED( index )
    editor->setGeometry(option.rect);
}

QSize GraphicsConfigDelegate::sizeHint( const QStyleOptionViewItem &option,
                                    const QModelIndex &index ) const
{
    Q_UNUSED( option )
    Q_UNUSED( index )

    QSize t_size = QStyledItemDelegate::sizeHint( option, index );
    t_size.setHeight( 30 );
    return t_size;
}

void GraphicsConfigDelegate::paint( QPainter *painter,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if ( index.parent() != QModelIndex() &&
         index.column() == GraphicsConfigModel::GRAPHICS_PENCOLOR )
    {
        QStandardItemModel * t_model = (QStandardItemModel *)index.model();
        GraphicsConfigModelItem * t_item = (GraphicsConfigModelItem *)
            (t_model->itemFromIndex( index.parent() )->child( index.row() ) );

        painter->setPen( t_item->linePen() );

        QRect t_rect = option.rect;

        painter->drawLine( t_rect.x() + 10, t_rect.y() + t_rect.height() / 2,
            t_rect.x() + t_rect.width() - 10, t_rect.y() + t_rect.height() / 2 );
    }
    else if ( index.parent() != QModelIndex() &&
         index.column() == GraphicsConfigModel::GRAPHICS_BRUSHCOLOR )
    {
        QStandardItemModel * t_model = (QStandardItemModel *)index.model();
        GraphicsConfigModelItem * t_item = (GraphicsConfigModelItem *)
            (t_model->itemFromIndex( index.parent() )->child( index.row() ) );

        QRect t_rect = option.rect;

        painter->setPen( t_item->linePen() );

        painter->drawRect( t_rect.x() + 5, t_rect.y() + 5,
            t_rect.width() - 10, t_rect.height() - 10 );
        painter->fillRect( t_rect.x() + 5, t_rect.y() + 5,
            t_rect.width() - 10, t_rect.height() - 10, t_item->fillBrush() );
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

GraphicsConfigTreeView::GraphicsConfigTreeView( QWidget * parent ) :
    QTreeView( parent )
{
    setAlternatingRowColors( true );
    header()->setSectionResizeMode( QHeaderView::ResizeToContents );
    setItemDelegate( new GraphicsConfigDelegate );
}

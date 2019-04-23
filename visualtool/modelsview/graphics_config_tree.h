/*
 T his program is free s*oftware; you can redistribute it and/or
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

#ifndef GRAPHICS_CONFIG_TREE_H
#define GRAPHICS_CONFIG_TREE_H

#include "../define/qt_include.h"

/*!
 * \brief item in graphics configuration model.
 *
 * Contains the information and value of one item in the graphics configuration.
 * Each item has following fields:
 * - item name ( router, port, ...)
 * - item style ( fault, living, ...)
 * - pen color,
 * - pen width,
 * - pen style,
 * - brush color,
 * - brush style
 */
class GraphicsConfigModelItem : public QStandardItem
{
protected:
    QString m_item;  /**< name of time, e.g. router, port, link, ... */
    QString m_style;  /**< style of time, e.g. fault, living, ... */
    QColor m_pen_color;  /**< pen color */
    qreal m_pen_width;  /**< pen width */
    Qt::PenStyle m_pen_style;  /**< pen style */
    QColor m_brush_color;  /**< brush color */
    Qt::BrushStyle m_brush_style;  /**< brush style */

public:
    /** @name construct functions
     * Construct an new item.
     */
    ///@{
    /*!
     * \brief constructs an empty item with default set.
     */
    GraphicsConfigModelItem() :
        QStandardItem(), m_item( "" ), m_style( "" ), m_pen_color( Qt::black ),
        m_pen_width( 1 ), m_pen_style( Qt::SolidLine ),
        m_brush_color( Qt::white ), m_brush_style( Qt::SolidPattern )
    {}
    /*!
     * \brief constructs an item with specified set.
     * \param item         name of item
     * \param style        style of item
     * \param pen_color    pen color
     * \param pen_width    pen width
     * \param pen_style    pen style
     * \param brush_color  brush color
     * \param brush_style  brush style
     */
    GraphicsConfigModelItem(
        const QString & item, const QString & style,
        const QColor & pen_color, qreal pen_width, Qt::PenStyle pen_style,
        const QColor & brush_color, Qt::BrushStyle brush_style
    ) :
        QStandardItem(), m_item( item ), m_style( style ),
        m_pen_color( pen_color ), m_pen_width( pen_width ),
        m_pen_style( pen_style ), m_brush_color( brush_color ),
        m_brush_style( brush_style )
    {}
    ///@}

    /** @name functions to access variables.
     * Set and return variables.
     */
    ///@{
    /*!
     * \brief returns name of item.
     * \return  name of item
     */
    QString item() const { return m_item; }
    /*!
     * \brief sets name of item.
     * \param item  name of item
     */
    void setItem( const QString & item ) { m_item = item; }
    /*!
     * \brief returns style of item.
     * \return  style of item
     */
    QString style() const { return m_style; }
    /*!
     * \brief sets style of item.
     * \param style  style of item
     */
    void setStyle( const QString & style ) { m_style = style; }
    /*!
     * \brief returns color of pen.
     * \return  color of pen
     */
    QColor penColor() const { return m_pen_color; }
    /*!
     * \brief sets color of pen.
     * \param color  color of pen
     */
    void setPenColor( const QColor & color ) { m_pen_color = color; }
    /*!
     * \brief returns width of pen.
     * \return  width of pen
     */
    qreal penWidth() const { return m_pen_width; }
    /*!
     * \brief sets width of pen.
     * \param width  width of pen
     */
    void setPenWidth( qreal width ) { m_pen_width = width; }
    /*!
     * \brief returns style of pen.
     * \return  style of pen
     */
    Qt::PenStyle penStyle() const { return m_pen_style; }
    /*!
     * \brief sets style of pen.
     * \param style  style of pen
     */
    void setPenStyle( Qt::PenStyle style ) { m_pen_style = style; }
    /*!
     * \brief returns color of brush.
     * \return  color of brush
     */
    QColor brushColor() const { return m_brush_color; }
    /*!
     * \brief sets color of brush.
     * \param color  color of brush
     */
    void setBrushColor( const QColor & color ) { m_brush_color = color; }
    /*!
     * \brief returns style of brush.
     * \return   style of brush
     */
    Qt::BrushStyle brushStyle() const { return m_brush_style; }
    /*!
     * \brief sets style of brush.
     * \param style  style of brush
     */
    void setBrushStyle( Qt::BrushStyle style ) { m_brush_style = style; }
    /*!
     * \brief returns the pen based oncolor, width and style.
     * \return  pen to draw
     */
    QPen linePen() const { return QPen( m_pen_color, m_pen_width, m_pen_style ); }
    /*!
     * \brief returns the brush based on color, style.
     * \return  brush to draw
     */
    QBrush fillBrush() const { return QBrush( m_brush_color, m_brush_style ); }
    ///@}

    /*!
     * \brief add one graphics config item as child.
     * \param item  item to add
     */
    void appendItem( GraphicsConfigModelItem * item );

    /** @name functions to access graphics configuration file
     * read and write one item and its children to the graphics configuration
     * file, XML format
     */
    ///@{
    /*!
     * \brief read graphics configuration under root in XML format.
     * \param root  the pointer to the root of dom tree for read
     */
    void readFile( QDomElement * root );
    /*!
     * \brief write graphics configuration to root in doc in XML format.
     * \param doc   the pointer to the dom tree of the file
     * \param root  the pointer to the root of dom tree for write
     */
    void writeFile( QDomDocument * doc, QDomElement * root );
    ///@}
};

/*!
 * \brief model of graphics configuration
 *
 * The model of configuration for graphics item in the program.
 */
class GraphicsConfigModel : public QStandardItemModel
{
    Q_OBJECT

public:
    /*!
     * \brief column of graphics configuration view
     */
    enum GraphicsConfigViewColumn
    {
        GRAPHICS_NAME = 0,   /**< name and style of item */
        GRAPHICS_PENCOLOR,   /**< color of pen */
        GRAPHICS_PENWIDTH,   /**< width of pen */
        GRAPHICS_PENSTYLE,   /**< style of pen */
        GRAPHICS_BRUSHCOLOR, /**< color of brush */
        GRAPHICS_BRUSHSTYLE, /**< style of brush */
        GRAPHICS_COLUMN_COUNT /**< total count of column */
    };

protected:
    static QStringList const_viewheader_list;  /**< const string list for
        graphics configuration view header */

public:
    /** @name construct functions
     * Construct an new model.
     */
    ///@{
    /*!
     * \brief constructs a empty model
     * \param parent  pointer to the parent object
     */
    GraphicsConfigModel( QObject *parent ) :
        QStandardItemModel( parent )
    {}
    ///@}

    /** @name functions for MVC
     * reimplementation functions for MVC.
     */
    ///@{
    /*!
     * \brief Returns the number of rows under the given parent.
     * \param parent  the parent index of model
     * \return  the total number of items or 0.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const ;
    /*!
     * \brief returns the number of columns for the children of the given parent.
     * \param parent  the parent index of model
     * \return  GRAPHICS_COLUMN_COUNT
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
     *
     * For the children of root, only name of item is shown in the name column.
     * For other items, fields are shown in columns in string format.
     *
     * \param index  model index of item
     * \param role   DisplayRole, EditRole
     * \return  data stored under the given role for this item
     */
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    /*!
     * \brief sets the role data for the item at index to value.
     *
     * For EditRole, set the value of fields in the item of index.
     *
     * \param index  model index of time
     * \param value  value to set
     * \param role   role, EditRole or CheckStateRole
     * \return true if successful; otherwise returns false.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    /*!
     * \brief returns the data for the given role and section in the header
     * with the specified orientation.
     *
     * Horizontal header returns const_viewheader_list.
     *
     * \param section      row number or column number
     * \param orientation  orientation, vertical or horizational
     * \param role         role, DisplayRole
     * \return  data for the given role and section in the specified header
     */
    QVariant headerData(
            int section, Qt::Orientation orientation, int role ) const;
    /*!
     * \brief returns the item flags for the given index.
     *
     * If index is child of root or in the column of name, item is enabled,
     * and selectable.
     * Otherwise, item is enabled, selectable and editable.
     *
     * \param index  model index of item
     * \return  flags for the given index
     */
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    ///@}

    /*!
     * \brief add new item in the model.
     * \param item item to add
     */
    void appendItem( GraphicsConfigModelItem * item );
    /*!
     * \brief find the item with specified name and style.
     * \param item   name of item
     * \param style  style of item
     * \return  the pointer to the item. If not found, return item with default
     * configuration.
     */
    GraphicsConfigModelItem * configItem(
            const QString & item, const QString & style );
    /*!
     * \brief copy another model to this model.
     * \param model  model to copy from
     */
    void copy( GraphicsConfigModel * model );

    /** @name functions to access graphics configuration file
     * read and write the argument list file, XML format
     * The path of file is fixed: ./configxml/graphics_config.xml
     */
    ///@{
    /*!
     * \brief write graphics configuration file in XML format
     * \return  error structure
     */
    ErrorHandler writeFile();
    /*!
     * \brief read graphics configuration file in XML format
     * \return  error structure
     */
    ErrorHandler readFile();
    ///@}

public slots:
    /*!
     * \brief set default value of the model.
     */
    void setDefaultValue();
};


/*!
 * \brief delegate for graphics configuration list.
 */
class GraphicsConfigDelegate : public QStyledItemDelegate
{
public:
    static QStringList const_penstyle_options;    /**<  const string list for
        optional style of pen */
    static QStringList const_brushstyle_options;  /**<  const string list for
        optional style of brush */

public:
    /** @name construct functions
     * Construct an new entity.
     */
    ///@{
    /*!
     * \brief constructs a new entity for delegate
     * \param parent  pointer to the parent widget
     */
    GraphicsConfigDelegate( QObject* parent = 0 ) :
        QStyledItemDelegate( parent )
    {}
    ///@}

    /** @name functions for MVC
     * reimplementation functions for MVC.
     */
    ///@{
    /*!
     * \brief Renders the delegate using the given painter and style option for
     * the item specified by index.
     *
     * draw a line of pen in the column of pen color based on color, width and
     * style.
     * draw a block of brush in columns of brush color based on color, style.
     *
     * \param painter  the pointer to painter
     * \param option   option of item
     * \param index    index of model
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    /*!
     * \brief create the editor to be used for editing the data item with the
     * given index.
     *
     * For item in pen/brush color column, a color dialog is generated.
     * For item in pen width, generates NumLineEdit.
     * For item in pen/brush style column, a combo box is generated.
     *
     * \param parent  pointer to the parent widget of editor
     * \param option  option of ediget
     * \param index   index of model item
     * \return  if editer created, return the pointer to the editor, otherwise, 0
     */
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    /*!
     * \brief sets the contents of the given editor to the data for the item
     * at the given index.
     *
     * Set value to file dialog as current file.
     * Set value to line edit, NumLineEdit or Spinbox as default value.
     *
     * \param editor  pointer to the editor
     * \param index   index of model item
     */
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    /*!
     * \brief sets the data for the item at the given index in the model to the
     * contents of the given editor.
     *
     * Set the color, width, and style code to value.
     *
     * \param editor  pointer to the editor
     * \param model   pointer to the model
     * \param index   index of model item
     */
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    /*!
     * \brief updates the geometry of the editor for the item with the given
     * index, according to the rectangle specified in the option. If the item
     * has an internal layout, the editor will be laid out accordingly.
     * \param editor  pointer to the editor
     * \param option  option of ediget
     * \param index   index of model item
     */
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
    ///@}

    /*!
     * \brief Returns the size hint set for the item, or an invalid QSize if no
     * size hint has been set.
     *
     * Return widget with fixed height of 30px.
     *
     * \param option  specified option
     * \param index   specified index
     * \return  return the size of item
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

/*!
 * \brief tree view for graphics configuration list.
 */
class GraphicsConfigTreeView : public QTreeView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new tree for graphics configuration list
     * \param parent  pointer to the parent widget
     */
    GraphicsConfigTreeView( QWidget* parent = 0 );
};

#endif

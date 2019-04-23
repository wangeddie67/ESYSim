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

#ifndef LINK_TOOL_TABLE_H
#define LINK_TOOL_TABLE_H

#include "../define/qt_include.h"

/*!
 * \brief item in link tools list model.
 *
 * Contains the information and value of one tool in the link tool list.
 * Each item has following fields:
 * - name
 * - father, category of tool: Configuration, Simulation, Analyse, Debug
 * - version
 * - execution file direction
 * - execution command
 * - config file
 * - compile command
 */
class LinkToolModelItem : public QStandardItem
{
public:
	/*!
	 * \brief category for link tool
	 */
	enum LinkToolCategory {
		LINKTOOL_CONFIGURE,   /**< configuration tools */
		LINKTOOL_SIMULATION,  /**< simulation tools */
		LINKTOOL_DEBUG,	   /**< debug tools */
		LINKTOOL_ANALYSIS	 /**< analysis tools */
	};

protected:
	static QStringList const_category_options;  /**< const string list for
		category */

	QString m_name;	   /**< name */
	QString m_category;   /**< category of tool */
	QString m_version;	/**< version */
	QString m_direction;  /**< related path direction */
	QString m_exec;	   /**< execution command */
	QString m_config;	 /**< path of configuration file */
	QString m_compile;	/**< compile command */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief constructs an empty item.
	 */
	LinkToolModelItem() :
		QStandardItem()
	{}
	///@}

	/** @name Functions to access variables.
	 * Set and return variables.
	 */
	///@{
	/*!
	* \brief return the name of tool.
	* \return  name of tool
	*/
	const QString & name() const { return m_name; }
	/*!
	 * \brief return the category list in string.
	 * \return   category string list
	 */
	static const QStringList & categoryStringList()
	{ return const_category_options; }
	/*!
	* \brief return the category of tool.
	* \return  name of category
	*/
	const QString & category() const { return m_category; }
	/*!
	 * \brief return the category of tools in number in the option list.
	 * \return   the number of category list. If invalid category, return -1.
	 */
	int categoryNum() const;
	/*!
	 * \brief set the category of tool.
	 * \param category  name of category
	 */
	void setCategory( const QString & category ) { m_category = category; }
	/*!
	 * \brief return the version of tool, in string.
	 * \return  version of tool
	 */
	const QString & version() const { return m_version; }
	/*!
	 * \brief return the direction of tool, in string.
	 * \return  direction of tool
	 */
	const QString & direction() const { return m_direction; }
	/*!
	 * \brief return the execution command of tool, in string.
	 * \return  execution command of tool
	 */
	const QString & exec() const { return m_exec; }
	/*!
	 * \brief return the path of configuration file of tool, in string.
	 * \return  the path of configuration file of tool
	 */
	const QString & config() const { return m_config; }
	/*!
	 * \brief return the compile command of tool, in string.
	 * \return  the compile command of tool
	 */
	const QString & compile() const { return m_compile; }
	///@}

	/** @name functions to access argument list file
	 * read and write the argument list file, XML format
	 */
	/*!
	 * \brief read argument file in XML format
	 *
	 * Call ArgumentListModel to read the XML file and get the information
	 * about the tool
	 *
	 * \param file  path of xml file
	 * \return  error structure
	 */
	ErrorHandler readArgumentFile( const QString & file );
	///@}
};

/*!
 * \brief model of link tool list
 */
class LinkToolModel : public QStandardItemModel
{
	Q_OBJECT

public:
	/*!
	 * \brief column of link tool view
	 */
	enum LinkToolListViewColumn
	{
		LINKTOOL_NAME = 0,  /**< name of tool */
		LINKTOOL_CATEGORY,  /**< category of tool */
		LINKTOOL_VERSION,   /**< version */
		LINKTOOL_DIR,	   /**< direction */
		LINKTOOL_EXEC,	  /**< execution command */
		LINKTOOL_CONFIG,	/**< config file */
		LINKTOOL_COMPILE,   /**< compile command */
		LINKTOOL_COLUMN_COUNT, /**< total count of column */
	};

protected:
	static QStringList const_viewheader_list;  /**< const string list for
		argument list view header */

public:
	/** @name construct functions
	 * Construct an new model.
	 */
	///@{
	/*!
	 * \brief constructs a empty model
	 * \param parent  pointer to the parent object
	 */
	LinkToolModel( QObject *parent ) :
		QStandardItemModel( parent )
	{}
	///@}

	/** @name functions for MVC
	 * reimplementation functions for MVC.
	 */
	///@{
	/*!
	 * \brief Returns the number of rows under the given parent.
	 *
	 * If the parent is invalid, return the children of root item. Otherwise,
	 * return 0.
	 *
	 * \param parent  the parent index of model
	 * \return  the total number of items or 0.
	 */
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	/*!
	 * \brief returns the number of columns for the children of the given parent.
	 * \param parent  the parent index of model
	 * \return  LINKTOOL_COLUMN_COUNT
	 */
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	/*!
	 * \brief returns the data stored under the given role for the item
	 * referred to by the index.
	 *
	 * For display role and edit role, it returns the string for different
	 * fields.
	 * For check state role, return the check status of item
	 * Otherwise, empty QVariant.
	 *
	 * \param index  model index of item
	 * \param role   DisplayRole, EditRole or CheckStateRole
	 * \return  data stored under the given role for this item
	 */
	QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
	/*!
	 * \brief sets the role data for the item at index to value.
	 *
	 * For EditRole, set the value of fields in the item of index.
	 * For CheckStateRole, set the check state of item at index
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
	 * \param section	  row number or column number
	 * \param orientation  orientation, vertical or horizational
	 * \param role		 role, DisplayRole or TextAlignmentRole
	 * \return  data for the given role and section in the specified header
	 */
	QVariant headerData( int section, Qt::Orientation orientation,
						 int role) const;
	/*!
	 * \brief returns the item flags for the given index.
	 *
	 * If index is in name column, item is enabled, selectable, and checkable.
	 * Else if index is in category column, item is enabled, selectable, and
	 * editable.
	 * Otherwise, item is enabled, selectable.
	 *
	 * \param index  model index of item
	 * \return  flags for the given index
	 */
	Qt::ItemFlags flags( const QModelIndex &index ) const;
	///@}

	/** @name functions to access link tool list
	 * add and access the link tool item
	 */
	///@{
	/*!
	 * \brief return the link tool item at specified row in table.
	 * \param row  row of link tool item
	 * \return   link tool item at specified row
	 */
	LinkToolModelItem * linkToolItem( int row )
	{ return (LinkToolModelItem *)item( row, 0 ); }
	/*!
	 * \brief append item in link tool list.
	 *
	 * Add item at column 0 followed by LINKTOOL_COLUMN_COUNT - 1 empty item.
	 *
	 * \param item  pointer to item to append
	 */
	void appendItem( LinkToolModelItem * item );
	///@}

	/** @name functions to access link tool file
	 * read and write the link tool file, XML format
	 * The path of file is fixed: ./configxml/linktools.xml
	 */
	///@{
	/*!
	 * \brief write link tool file in XML format
	 * \return  error structure
	 */
	ErrorHandler writeLinkToolFile();
	/*!
	 * \brief read link tool file in XML format
	 * \return  error structure
	 */
	ErrorHandler readLinkToolFile();
	///@}

	/*!
	 * \brief generate renew script.
	 *
	 * The checked tools can be compiled again by running this script.
	 * If window platform, generate .bat; If unix platform, generate shell.
	 *
	 * \return  string of renew shell
	 */
	QString renewScript();
};

/*!
 * \brief delegate for argument list.
 */
class LinkToolDelegate : public QStyledItemDelegate
{
public:
	/*!
	 * \brief column of link tool view
	 */
	enum LinkToolViewType
	{
		LINKTOOLVIEW_EDIT,	/**< selectable, addable and removeable */
		LINKTOOLVIEW_SELECT,  /**< selectable, checkable */
	};

protected:
	LinkToolViewType m_type;  /**< type of model, EDIT or SELECT */

public:
	/** @name construct functions
	 * Construct an new entity.
	 */
	///@{
	/*!
	 * \brief constructs a new entity for delegate
	 * \param type	type of model view, EDIT or SELECT
	 * \param parent  pointer to the parent widget
	 */
	LinkToolDelegate( LinkToolViewType type, QObject* parent = 0
	) :
		QStyledItemDelegate( parent ), m_type( type )
	{}

	/** @name functions for MVC
	 * reimplementation functions for MVC.
	 */
	///@{
	/*!
	 * \brief create the editor to be used for editing the data item with the
	 * given index.
	 *
	 * generate combobox for category column, if the model is under edit type.
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
	 * Set value to combobox as current item.
	 *
	 * \param editor  pointer to the editor
	 * \param index   index of model item
	 */
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	/*!
	 * \brief sets the data for the item at the given index in the model to the
	 * contents of the given editor.
	 *
	 * Set the string of current selected to the category
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
};

/*!
 * \brief table view for link tool list.
 */
class LinkToolTableView : public QTableView
{
	Q_OBJECT

public:
	/*!
	 * \brief constructs a new table for link tool list
	 * \param parent  pointer to the parent widget
	 */
	LinkToolTableView( QWidget* parent = 0 );
};


#endif // LINK_TOOL_DIALOG_H

#ifndef ARGUMENT_LIST_TABLE_H
#define ARGUMENT_LIST_TABLE_H

#include "../define/qt_include.h"
#include "esy_argument.h"

/*!
 * \brief item in argument list model.
 *
 * Contains the information and value of one option in the argument list.
 * Each item has following fields:
 * - option (start with '-' usually),
 * - type to guide the behaviors,
 * - note to describe the brief of this option,
 * - value,
 * - loop level. 0 means no loop. higher value means higher priority in loop.
 */
class ArgumentListModelItem : public QStandardItem
{
protected:
	QString m_opt;  /**< option string */
	EsyArgumentItem::ArgumentType m_opt_type; /**< option type */
	QString m_note;  /**< option note, describe the brief of this option. */
	QString m_value;  /**< string of value */
	int m_loop;  /**< loop level */
	QString m_darg;  /**< string of depended argument */
	long m_tab;  /**< bool of depended argument */
	QString m_extension;  /**< extension name */
	bool m_is_shown;

public:
	/** @name construct functions
	 * Construct an new item.
	 */
	///@{
	/*!
	 * \brief constructs an empty item.
	 */
	ArgumentListModelItem() :
		QStandardItem()
	{}
	/*!
	 * \brief constructs a item with specified of each field.
	 * \param opt	option string
	 * \param type   option type
	 * \param note   string of option note
	 * \param value  string of value
	 * \param loop   loop level
	 */
	ArgumentListModelItem(
		const QString & opt, EsyArgumentItem::ArgumentType type,
		const QString & note, const QString & value, int loop,
		const QString & darg = QString(), long tab = 0,
		const QString & extension = ""
	) :
		QStandardItem(), m_opt( opt ), m_opt_type( type ), m_note( note ),
		m_value( value ), m_loop( loop ), m_darg( darg ), m_tab( tab ),
		m_extension( extension ), m_is_shown( true )
	{}
	///@}

	/** @name functions to access variables.
	 * Set and return variables.
	 */
	///@{
	/*!
	 * \brief returns option string.
	 * \return  option string
	 */
	const QString & opt() const { return m_opt; }
	/*!
	 * \brief sets option string.
	 * \param opt option string
	 */
	void setOpt( const QString & opt ) { m_opt = opt; }
	/*!
	 * \brief returns type of option.
	 * \return  option type
	 */
	EsyArgumentItem::ArgumentType optType() const { return m_opt_type; }
	/*!
	 * \brief sets type of option.
	 * \param type  option type
	 */
	void setOptType( EsyArgumentItem::ArgumentType type ) { m_opt_type = type; }
	/*!
	 * \brief returns note of option.
	 * \return  option note
	 */
	const QString & note() const { return m_note; }
	/*!
	 * \brief set note of option/
	 * \param note  option node
	 */
	void setNote( const QString & note ) { m_note = note; }
	/*!
	 * \brief return string of value of option.
	 * \return  option value
	 */
	const QString & value() const { return m_value; }
	/*!
	 * \brief sets string of value of option.
	 * \param value  option value
	 */
	void setValue( const QString & value ) { m_value = value; }
	/*!
	 * \brief returns loop level of option.
	 * \return  loop level
	 */
	int loop() const { return m_loop; }
	/*!
	 * \brief set loop level of option.
	 * \param loop loop level
	 */
	void setLoop( int loop ) { m_loop = loop; }
	/*!
	 * \brief returns name of depended argument.
	 * \return  depended argument
	 */
	const QString & dependArg() const { return m_darg; }
	/*!
	 * \brief set name of depended argument.
	 * \param arg  depended argument
	 */
	void setDependArg( const QString & arg ) { m_darg = arg; }
	/*!
	 * \brief returns value of depended argument.
	 * \return  value of depended argument
	 */
	long dependTab() const { return m_tab; }
	/*!
	 * \brief set value of depended argument.
	 * \param value value of depended argument
	 */
	void setDependTab( long value ) { m_tab = value; }
	/*!
	 * \brief returns extension name.
	 * \return  extension name
	 */
	const QString & extension() const { return m_extension; }
	/*!
	 * \brief set the extension name.
	 * \param value extension name
	 */
	void setExtension( const QString & extension ) { m_extension = extension; }
	/*!
	 * \brief returns the state of argument.
	 * \return  the state of argument, if shown, it is TRUE.
	 */
	bool isShown() const { return m_is_shown; }
	/*!
	 * \brief set the state of argument.
	 * \param value the state of argument
	 */
	void setShown( bool value ) { m_is_shown = value; }
	///@}
};

/*!
 * \brief model of argument list
 *
 * The model of argument list for one tool.
 * It hold a list of argument items and the information of the tool.
 * - name,
 * - version,
 * - direction,
 * - execution command,
 * - path of configuration file,
 * - compile command.
 */
class ArgumentListModel : public QStandardItemModel
{
	Q_OBJECT

public:
	/*!
	 * \brief column of argument view
	 */
	enum ArgumentListViewColumn
	{
		ARGULIST_OPT = 0, /**< option */
		ARGULIST_LOOP,	/**< loop level */
		ARGULIST_VALUE,   /**< value */
		ARGULIST_NOTE,	/**< option note */
		ARGULIST_COLUMN_COUNT /**< total count of column */
	};

	#define LINKCFG_EXTENSION "lkcfg"

protected:
	static QStringList const_viewheader_list;  /**< const string list for
		argument list view header */

    bool m_onlyone_model;
	QString m_name;	   /**< name */
	QString m_version;	/**< version */
	QString m_direction;  /**< related path direction */
	QString m_exec;	   /**< execution command */
	QString m_config;	 /**< path of configuration file */
	QString m_compile;	/**< compile command */

public:
	/** @name construct functions
	 * Construct an new model.
	 */
	///@{
	/*!
	 * \brief constructs a empty model
	 * \param parent  pointer to the parent object
	 */
    ArgumentListModel( bool onlyone, QObject *parent ) :
        QStandardItemModel( parent ), m_onlyone_model( onlyone )
	{}
	///@}

	/** @name functions to access variables.
	 * Set and return variables.
	 */
	///@{
	/*!
	* \brief return the name of tool.
	* \return  name of tool
	*/
	const QString & name() const { return m_name; }
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
	 * \return  ARGULIST_COLUMN_COUNT
	 */
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	/*!
	 * \brief returns the data stored under the given role for the item
	 * referred to by the index.
	 *
	 * For display role and edit role, it returns the string for different
	 * fields. In loop column, return the loop level for STRING_TYPE,
	 * DOUBLE_TYPE and LONG_TYPE and empty string for others. In value column,
	 * return value string except BOOL_TYPE, which is empty string.
	 * For check state role, return true/false for BOOL_TYPE which presented by
	 * 1/0. Otherwise, empty QVariant.
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
	 * Vertical header returns section.
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
	 * If index is in value column and item is bool type argument, item is
	 * enabled, selectable, and checkable.
	 * Else if index is in loop column and value column, item is enabled,
	 * selectable, and editable.
	 * Otherwise, item is enabled, selectable.
	 *
	 * \param index  model index of item
	 * \return  flags for the given index, ItemIsEnabled | ItemIsSelectable
	 */
	Qt::ItemFlags flags( const QModelIndex &index ) const;
	///@}

	/** @name functions to access argument list
	 * add and access the item in argument list
	 */
	///@{
	/*!
	 * \brief return the argument item at specified row in table.
	 * \param row  row of argument item
	 * \return   argument item at specified row
	 */
	ArgumentListModelItem * arguItem( int row )
		{ return (ArgumentListModelItem *)item( row ); }
	/*!
	 * \brief append item in argument list.
	 *
	 * Add item at column 0 followed by ARGULIST_COLUMN_COUNT - 1 empty item.
	 *
	 * \param item  pointer to item to append
	 */
	void appendItem( ArgumentListModelItem * item );
	///@}

	/** @name functions to access argument list file
	 * read and write the argument list file, XML format
	 */
	///@{
	/*!
	 * \brief write argument file in XML format
	 * \param file  path of xml file
	 * \return  error structure
	 */
	ErrorHandler writeArgumentFile( const QString & file );
	/*!
	 * \brief read argument file in XML format
	 * \param file  path of xml file
	 * \return  error structure
	 */
	ErrorHandler readArgumentFile( const QString & file );
	///@}

	void updateRow();

signals:
	void showRow( int row );
	void hideRow( int row );
};

/*!
 * \brief delegate for argument list.
 */
class ArgumentListDelegate : public QStyledItemDelegate
{
public:
	/** @name construct functions
	 * Construct an new entity.
	 */
	///@{
	/*!
	 * \brief constructs a new entity for delegate
	 * \param parent  pointer to the parent widget
	 */
	ArgumentListDelegate( QObject* parent = 0 ) :
		QStyledItemDelegate( parent )
	{}
	///@}


	/** @name functions for MVC
	 * reimplementation functions for MVC.
	 */
	///@{
	/*!
	 * \brief create the editor to be used for editing the data item with the
	 * given index.
	 *
	 * For data item in loop column, a spinbox is generated for STRING_TYPE,
	 * DOUBLE_TYPE, LONG_TYPE.
	 * For data item in value column, generates open file dialog for file typs
	 * (NEW_FILE_TYPE, OPENFILE_TYPE, NEW_FILE_IF_TYPE, OPEN_FILE_IF_TYPE) or
	 * generates NumLineEdit for number type (LONG_TYPE, DOUBLE_TYPE,
	 * LONG_VECTOR_TYPE, DOUBLE_VECTOR_TYPE) or line edit for other types.
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
	 * Set the first selected file to value.
	 * Set the text from edit to value.
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
 * \brief table view for argument list.
 */
class ArgumentListTableView : public QTableView
{
	Q_OBJECT

private:
    bool m_onlyone_model;

public:
	/*!
	 * \brief constructs a new table for argument list
	 * \param parent  pointer to the parent widget
	 */
    ArgumentListTableView( bool onlyone, QWidget* parent = 0 );
	/*!
	 * \brief Sets the model for the view to present.
	 * \param model  model to present
	 */
	void setModel( QAbstractItemModel * model );

public slots:
	/*!
	 * \brief update the column width, to fit the width of this widget.
	 */
	void updateColumnWidth();
};

#endif // LINK_TOOL_DIALOG_H

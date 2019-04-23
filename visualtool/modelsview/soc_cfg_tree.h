#ifndef SOC_CFG_TREE_H
#define SOC_CFG_TREE_H

#include "../define/qt_include.h"

#include "esy_soccfg.h"
#include "esy_networkcfg.h"

class SoCCfgModelItem : public QStandardItem
{
protected:
	long m_item_code;

public:
	SoCCfgModelItem( long item_code, const QString & text ) :
		QStandardItem( text ), m_item_code( item_code )
	{}

	long itemCode() { return m_item_code; }
};

/*!
 * \brief model of SoC configuration
 *
 * Show different options for SoC configuration.
 * Directly connect with SoC configuration interface.
 * Show or set the value in structure.
 */
class SoCCfgModel : public QStandardItemModel
{
	Q_OBJECT

public:
	/*!
	 * \brief type of configuration model
	 */
	enum SoCCfgType
	{
		SOCCFG_TILE,    /**< tile configuration */
		SOCCFG_ROUTER,  /**< router configuration */
		SOCCFG_GLOBAL,  /**< global configuration */
	};

	/*!
	 * \brief column of SoC configuration view
	 */
	enum SoCCfgColumn
	{
		SOCCFG_ARGUMENT = 0,  /**< argument name */
		SOCCFG_VALUE,         /**< argument value */
		SOCCFG_COLUMN_COUNT,  /**< total count of column */
	};

	/** @name row sequence of items
	 */
	///@{
	/*!
	 * \brief row sequence of level 1 item in tile configuration
	 */
	enum TileCfgViewRowL1
	{
		TILECFG_ID = 0,
		TILECFG_LOCATION,
		TILECFG_FEATURE,
		TILECFG_PREDICT,
		TILECFG_RUU,
		TILECFG_CACHE,
		TILECFG_MEMORY,
		TILECFG_EXECUTE,
        TILECFG_ASIC,
        TILECFG_L1_COUNT,
    };
	enum RouterCfgViewRowL1
	{
		ROUTERCFG_ID = 0,
		ROUTERCFG_LOCATION,    /**< location of the router */
		ROUTERCFG_FEATURE,     /**< pipeline cycle of router, default is 1 */
		ROUTERCFG_PORT_START,  /**< start point of port configuration item */
	};
	enum GlobalCfgViewRowL1
	{
		GLOBALCFG_TOPOLOGY = 0,
		GLOBALCFG_TILE_FEATURE,
		GLOBALCFG_PREDICT,
		GLOBALCFG_RUU,
		GLOBALCFG_CACHE,
		GLOBALCFG_MEMORY,
		GLOBALCFG_EXECUTE,
        GLOBALCFG_ASIC,
        GLOBALCFG_ROUTER,
		GLOBALCFG_PORT_START,
	};

	enum SoCCfgViewRowCount
	{
		TILELOC_COUNT = 5,
		TILEFEA_COUNT = 13,
		PREDCFG_COUNT = 7,
		MEMCFG_COUNT = 6,
		RUUCFG_COUNT = 9,
		CACHECFG_COUNT = 9,
		EXECFG_COUNT = 5,
		ROUTERFEA_COUNT = 2,
		PORTCFG_ROW_COUNT = 7,
		ROUTERLOC_COUNT = 2,
        TOPOLOGY_COUNT = 5,
        ASICTILE_COUNT = 3,
	};

	enum SoCCfgViewRowItem
	{
		TILELOC_AX_X = 0,
		TILELOC_AX_Y,
		TILELOC_NI_ID,
		TILELOC_NI_PHY,

		TILEFEA_TYPE,
		TILEFEA_ENABLE,
		TILEFEA_FREQ,
		TILEFEA_ISA_TYPE,
		TILEFEA_APP_DIR,
		TILEFEA_MAX_INST,
		TILEFEA_FETCH_SPEED,
		TILEFEA_MPI_ID,
		TILEFEA_META_SIZE,
		TILEFEA_RETURN_STACK_SIZE,
		TILEFEA_BTB_SET,
		TILEFEA_BTB_COMBINATION,
		TILEFEA_NI_POS,

		PREDCFG_UPDATE_TYPE,
		PREDCFG_PREDICT_TYPE,
		PREDCFG_BIMOD_SIZE,
		PREDCFG_L1_SIZE,
		PREDCFG_L2_SIZE,
		PREDCFG_L2_HIST_SIZE,
		PREDCFG_L2_XOR_ENABLE,

		MEMCFG_FIRST_CHUNK_DELAY,
		MEMCFG_NEIBER_CHUNK_DELAY,
		MEMCFG_TLB_ITLB_CFG,
		MEMCFG_TLB_DTLB_CFG,
		MEMCFG_TLB_MISS_DELAY,
		MEMCFG_BUS_WIDTH,

		RUUCFG_IFQ_SIZE,
		RUUCFG_BRANCH_PENALT,
		RUUCFG_DECODER_WIDTH,
		RUUCFG_ISSUE_WIDTH,
		RUUCFG_INORDER_ISSU,
		RUUCFG_INCLUDE_SPEC,
		RUUCFG_COMMIT_WIDTH,
		RUUCFG_SIZE,
		RUUCFG_LSQ_SIZE,

		CACHECFG_DL1_CFG,
		CACHECFG_DL1_DELAY,
		CACHECFG_DL2_CFG,
		CACHECFG_DL2_DELAY,
		CACHECFG_IL1_CFG,
		CACHECFG_IL1_DELAY,
		CACHECFG_IL2_CFG,
		CACHECFG_IL2_DELAY,
		CACHECFG_I_COMPRESS_ENABLE,

		EXECFG_INTEGAR_ADDER_UNIT,
		EXECFG_INTEGAR_MULTI_UNIT,
		EXECFG_FLOAT_ADDR_UNIT,
		EXECFG_FLOAT_MULTI_UNIT,
		EXECFG_MEMORY_PORT,

		ROUTERFEA_PIPELINE, /**< pipeline cycle of router, default is 1 */
		ROUTERFEA_PORTNUM,      /**< port number of router */

		PORTCFG_IN_VCNUM,     /**< number of virtual channel */
		PORTCFG_OUT_VCNUM,
		PORTCFG_DIRECTION,     /**< direction of port, N, S, W, E, NW, NE, SW, SE */
		PORTCFG_NI,            /**< network interface, bool */
		PORTCFG_INPUTBUFFER,   /**< size of input buffer */
		PORTCFG_OUTPUTBUFFER,  /**< size of output buffer */
		PORTCFG_NEIGHBORID,    /**< router id of output neighbor router */
		PORTCFG_NEIGHBORPORT,  /**< port id of output neighbor router */

		ROUTERLOC_AXIS_X, /**< X axis, column number */
		ROUTERLOC_AXIS_Y,     /**< Y axis, row number */

		TOPOLOGY_TOPOLOGY,
		TOPOLOGY_SIZE_START,
		TOPOLOGY_SIZE_END,
		TOPOLOGY_FREQ,
		TOPOLOGY_DATA_WIDTH,

        ASICTILE_NAME,
        ASICTILE_TYPE,
        ASICTILE_DELAY,
	};
	///@}

protected:
	/** @name const string list for items
	 */
	///@{
	static QStringList const_tilecfg_l1_view_list;
	static QStringList const_routercfg_l1_view_list;
	static QStringList const_globalcfg_l1_view_list;

	static QStringList const_soccfg_view_list;
	static QString const_routercfg_port_title;

	static QStringList const_viewheader_list;

	SoCCfgType m_type;  /**< type of configuration, network or router */
	EsySoCCfgTile * mp_tile_cfg;  /**< pointer to tile configuration
		interface */
	EsySoCCfgNetwork * mp_network_cfg;
	EsyNetworkCfgRouter * mp_router_cfg;  /**< pointer to router configuration
		interface */

public:
	/** @name construct functions
	 * Construct an new model.
	 */
	///@{
	/*!
	 * \brief constructs a empty model
	 * \param parent  pointer to the parent object
	 */
	SoCCfgModel( QObject *parent ) :
		QStandardItemModel( parent ), mp_tile_cfg( 0 ),
		mp_network_cfg( 0 ), mp_router_cfg( 0 )
	{}
	///@}

	QStandardItem * appendItem(
		long code, const QString & value, QStandardItem * parent = 0);

	void setTileCfg( EsySoCCfgTile * cfg );
	void setRouterCfg( EsyNetworkCfgRouter * routercfg );
	void setGlobalCfg( EsySoCCfg *cfg );
	void setConfigItems( SoCCfgType type );

    /** @name functions for MVC
     * reimplementation functions for MVC.
     */
    ///@{
    /*!
     * \brief Returns the number of rows under the given parent.
     *
     * If the parent is invalid, return
     * NETWORKCFG_PORT_START/ROUTERCFG_PORT_START + number of port;
     * Size item returns 0 or dim;
     * Position item returns 2;
     * port configuration item, returns PORTCFG_ROW_COUNT or PORTCFG_ROW_COUNT
     * - 2.
     * Otherwise, return 0
     *
     * \param parent  the parent index of model
     * \return  the total number of children items.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the number of columns for the children of the given parent.
     * \param parent  the parent index of model
     * \return  NETWORKCFG_COLUMN
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
     *
     * For display role and edit role, it returns the string for items
     * For check state role, return true/false for network interface in port
     * configuration item.
     *
     * \param index  model index of item
     * \param role   DisplayRole, EditRole or CheckStateRole
     * \return  data stored under the given role for this item
     */
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    /*!
     * \brief sets the role data for the item at index to value.
     *
     * For EditRole, set the value of fields in the item of index. Moreover,
     * if the topology and port number has changed, call setSizeCfg() and
     * setPortCfg() to update the model.
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
     * \param section      row number or column number
     * \param orientation  orientation, vertical or horizational
     * \param role         role, DisplayRole or TextAlignmentRole
     * \return  data for the given role and section in the specified header
     */
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role) const;
    /*!
     * \brief returns the item flags for the given index.
     *
     * If index is in argument column or has children, item is enabled and
     * selectable.
     * If index is in value column and item is network interface item in port
     * configuration, item is enabled, selectable, and checkable.
     * Otherwise, item is enabled, selectable, editable.
     *
     * \param index  model index of item
     * \return  flags for the given index, ItemIsEnabled | ItemIsSelectable
     */
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    ///@}

protected:
	/*!
	 * \brief check the match of model and pointer to interface
	 *
	 * If the model is NETWORKCFG_NETWORK and the pointer to router configuration
	 * has seted, return true;
	 * If the model is NETWORKCFG_ROUTER and the pointer to router configuration
	 * has seted, return true;
	 * Otherwise, return false;
	 *
	 * \return TRUE/FALSE
	 */
	bool isValid() const;
	/*!
	 * \brief add port configuration starting at start_row
	 *
	 * \param start_row  row of the first port configuration item
	 */
	void setPortCfg( int start_row );
	void setSizeCfg( int l1_row, int start_row, int end_row );
signals:
    /*!
     * \brief emit to update the scene.
     */
    void updateScene();
};

/*!
 * \brief delegate for network configuration list.
 */
class SoCCfgDelegate : public QStyledItemDelegate
{
protected:
	SoCCfgModel::SoCCfgType m_type;  /**< type of configuration,
        network or router */

public:
    /** @name construct functions
     * Construct an new entity.
     */
    ///@{
    /*!
     * \brief constructs a new entity for delegate
     * \param type    type of configuration, network or router
     * \param parent  pointer to the parent widget
     */
	SoCCfgDelegate(
		SoCCfgModel::SoCCfgType type, QObject* parent = 0
    ) :
        QStyledItemDelegate( parent ), m_type( type )
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
     * For value item for topology item, port direction, generates combobox
     * with topology options.
     * For value item for item with double value, generate number line edit.
     * For value item for item with integer value, generate spin box.
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
     * Set value to combobox as current selected.
     * Set value to NumLineEdit or Spinbox as default value.
     *
     * \param editor  pointer to the editor
     * \param index   index of model item
     */
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    /*!
     * \brief sets the data for the item at the given index in the model to the
     * contents of the given editor.
     *
     * Set the current selected index to value.
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
 * \brief table view for network configuration list.
 */
class SoCCfgTreeView : public QTreeView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new tree for network configuration list
     * \param parent  pointer to the parent widget
     */
	SoCCfgTreeView( QWidget * parent );
};

#endif // NETWORK_CFG_TREE_H

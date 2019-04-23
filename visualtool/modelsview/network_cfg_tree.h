#ifndef NETWORK_CFG_TREE_H
#define NETWORK_CFG_TREE_H

#include "../define/qt_include.h"

#include "esy_networkcfg.h"

/*!
 * \brief model of network configuration
 *
 * Show different options for network configuration or router configuration.
 * Directly connect with network configuration interface.
 * Show or set the value in structure.
 */
class NetworkCfgModel : public QStandardItemModel
{
    Q_OBJECT

public:
    /*!
     * \brief type of configuration model
     */
    enum NetworkCfgType
    {
        NETWORKCFG_NETWORK, /**< network configuration, for entire network */
        NETWORKCFG_ROUTER   /**< router configuration, for specified router */
    };

    /*!
     * \brief column of network configuration view
     */
    enum NetworkCfgColumn
    {
        NETWORKCFG_ARGUMENT = 0,  /**< argument name */
        NETWORKCFG_VALUE,         /**< argument value */
        NETWORKCFG_COLUMN_COUNT /**< total count of column */
    };

    /** @name row sequence of items
     */
    ///@{
    /*!
     * \brief row sequence of item in network configuration
     */
    enum NetworkCfgViewRow
    {
        NETWORKCFG_TOPOLOGY = 0,  /**< topology of network */
        NETWORKCFG_SIZE,          /**< size of network */
        NETWORKCFG_DATA_WIDTH,    /**< width of data path */
        NETWORKCFG_PORTNUM,       /**< port number of router */
        NETWORKCFG_PORT_START  /**< start point of port configuration item */
    };
    /*!
     * \brief row sequence of item in router configuration
     */
    enum RouterCfgViewRow
    {
        ROUTERCFG_POSITION = 0,  /**< position of router, in graphic, unit: px */
        ROUTERCFG_PIPELINE,      /**< pipeline cycle of router, default is 1 */
        ROUTERCFG_PORTNUM,       /**< port number of router */
        ROUTERCFG_PORT_START  /**< start point of port configuration item */
    };
    /*!
     * \brief row sequence of item in port configuration
     *
     * As the children of port configuration in network configuration and router
     * configuration.
     */
    enum PortCfgViewRow
    {
		PORTCFG_INPUT_VCNUM = 0,     /**< number of input virtual channel */
		PORTCFG_OUTPUT_VCNUM,     /**< number of output virtual channel */
        PORTCFG_DIRECTION,     /**< direction of port, N, S, W, E, NW, NE, SW, SE */
        PORTCFG_NI,            /**< network interface, bool */
        PORTCFG_INPUTBUFFER,   /**< size of input buffer */
        PORTCFG_OUTPUTBUFFER,  /**< size of output buffer */
        PORTCFG_NEIGHBORID,    /**< router id of output neighbor router */
        PORTCFG_NEIGHBORPORT,  /**< port id of output neighbor router */
        PORTCFG_ROW_COUNT  /**< total count of item */
        };
    /*!
     * \brief row sequence of item in axis configuration
     *
     * As the child of position item in router configuration and size item in
     * network configuration.
     */
    enum AxisCfgViewRow
    {
        AXISCFG_X = 0, /**< X axis, column number */
        AXISCFG_Y,     /**< Y axis, row number */
        AXISCFG_ROW_COUNT  /**< total count of item */
    };
    ///@}

protected:
    /** @name const string list for items
     */
    ///@{
    static QStringList const_networkcfg_view_list;  /**< const string list for
        network configuration item */
    static QStringList const_routercfg_view_list;   /**< const string list for
        router configuration item */
    static QStringList const_portcfg_view_list;     /**< const string list for
        port configuration item */
    static QString const_portcfg_title;             /**< const string for
        port configuration title */
    static QStringList const_axiscfg_view_list;     /**< const string list for
        axis configuration item */
    ///@}
    static QStringList const_viewheader_list;       /**< const string list for
        network configuration view header */

    NetworkCfgType m_type;  /**< type of configuration, network or router */
    EsyNetworkCfg * mp_network_cfg;  /**< pointer to network configuration
        interface */
    EsyNetworkCfgRouter * mp_router_cfg;    /**< pointer to router configuration
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
    NetworkCfgModel( QObject *parent ) :
        QStandardItemModel( parent ), mp_network_cfg( 0 ), mp_router_cfg( 0 )
    {}
    ///@}

    /*!
     * \brief set network configuration items
     *
     * Clear exist items. Then set the model to network configuration and set
     * the pointer to interface. Add items for network configuration.
     * Call setSizeCfg() add size item.
     * Call setPortCfg() add port configuration item.
     *
     * \param cfg  pointer to the network configuration items
     */
    void setNetworkCfg( EsyNetworkCfg * cfg );
    /*!
     * \brief set router configuration items
     *
     * Clear exist items. Then set the model to router configuration and set
     * the pointer to interface. Add items for router configuration.
     * Call setSizeCfg() add position item.
     * Call setPortCfg() add port configuration item.
     *
     * \param cfg  pointer to the router configuration items
     */
    void setRouterCfg( EsyNetworkCfgRouter * cfg );

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
     * \brief add size configuration as the child of item at row
     * \param row  row of parent item of size configuration items.
     */
    void setSizeCfg( int row );
    /*!
     * \brief add port configuration starting at start_row
     *
     * \param start_row  row of the first port configuration item
     */
    void setPortCfg( int start_row );

signals:
    /*!
     * \brief emit to update the scene.
     */
    void updateScene();
};

/*!
 * \brief delegate for network configuration list.
 */
class NetworkCfgDelegate : public QStyledItemDelegate
{
protected:
    NetworkCfgModel::NetworkCfgType m_type;  /**< type of configuration,
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
    NetworkCfgDelegate(
        NetworkCfgModel::NetworkCfgType type, QObject* parent = 0
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
class NetworkCfgTreeView : public QTreeView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new tree for network configuration list
     * \param parent  pointer to the parent widget
     */
    NetworkCfgTreeView( QWidget * parent );
};

#endif // NETWORK_CFG_TREE_H

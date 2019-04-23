#ifndef FAULTS_LIST_TABLE_H
#define FAULTS_LIST_TABLE_H

#include "../define/qt_include.h"

#include "esy_faultcfg.h"

/*!
 * \brief item in fault configuration list mode.
 *
 * Contains the information and value of one item in the fault configuration
 * list. Item is a combination of QStandardItem and EsyFaultConfigItem.
 * Each item has following fields:
 * - fault type,
 * - target router,
 * - target port,
 * - target virtual channel,
 * - target bit,
 * - states for state machine,
 * - transform probability matrix.
 */
class FaultListModelItem : public QStandardItem, public EsyFaultConfigItem
{
public:
    /** @name construct functions
     * Construct an new item.
     */
    ///@{
    /*!
     * \brief constructs an empty item.
     */
    FaultListModelItem() :
        QStandardItem(), EsyFaultConfigItem()
    {}
    /*!
     * \brief constructs an item with specified fields.
     * \param type      fault type
     * \param id        router id of target
     * \param port      port id of target
     * \param vc        vc id of target
     * \param bit       bit of target
     * \param state     state list for state machine
     * \param transfer  state transfer matrix for state machine
     */
    FaultListModelItem( FaultType type, long id, long port, long vc, long bit,
        vector< long > state, vector< vector< double > > transfer
    ) :
        QStandardItem(),
        EsyFaultConfigItem( type, id, port, vc, bit, state, transfer )
    {}
    /*!
     * \brief constructs an item from item in standard interface.
     * \param item  fault configuration item in standard interface
     */
    FaultListModelItem( const EsyFaultConfigItem & item ) :
        QStandardItem(), EsyFaultConfigItem( item )
    {}
    ///@}

    /*!
     * \brief get fault configuration item in standard interface.
     * \return  fault configuration item in standard interface
     */
    EsyFaultConfigItem getEsyFaultConfigItem() const
        { return EsyFaultConfigItem( (*this) ); }
};

/*!
 * \brief model of fault configuration list
 *
 * The model of fault configuration list in fault configuration file.
 * Each item has following fields:
 * - type,
 * - target router,
 * - target port,
 * - target virtual channel,
 * - target bit,
 * - states for state machine,
 * - transform probability matrix.
 */
class FaultListModel : public QStandardItemModel
{
    Q_OBJECT
public:
    /*!
     * \brief column of fault configuration list view
     */
    enum FaultCfgColumn
    {
        FAULTCFG_TYPE = 0,  /**< fault type */
        FAULTCFG_ROUTER,    /**< target router */
        FAULTCFG_PORT,      /**< target port */
        FAULTCFG_VC,        /**< target virtual channel */
        FAULTCFG_BIT,       /**< target bit */
        FAULTCFG_STATE,     /**< state list of state machine */
        FAULTCFG_TRASFER,   /**< transform probability matrix */
        FAULTCFG_COLUMN   /**< total count of column */
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
     * \param parent  pointer to the parent widget
     */
    FaultListModel( QObject *parent ) :
        QStandardItemModel( parent )
    {}
    ///@}

    /** @name functions to access standard interface
     * convert between table model and standard interface
     */
    ///@{
    /*!
     * \brief set the standard interface to fault configuration list
     * \param fault_cfg_list  fault configuration list of standard interface
     */
    void setFaultCfgList( const EsyFaultConfigList & fault_cfg_list );
    /*!
     * \brief get the standard interface from fault configuration list
     * \return fault configuration list of standard interface
     */
    EsyFaultConfigList getFaultCfgList();
    ///@}

    /** @name functions for MVC
     * reimplementation functions for MVC.
     */
    ///@{
    /*!
     * \brief Returns the number of rows under the given parent.
     * \param parent  the parent index of model
     * \return  When the parent is valid, return the number of children of parent.
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const ;
    /*!
     * \brief returns the number of columns for the children of the given parent.
     * \param parent  the parent index of model
     * \return  FAULTCFG_COLUMN
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
     *
     * Display each field in the table. Note: "-1" changes to "All".
     *
     * \param index  model index of item
     * \param role   role, DisplayRole, ToolTipRole
     * \return  data stored under the given role for this item
     */
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    /*!
     * \brief returns the data for the given role and section in the header
     * with the specified orientation.
     * \param section      row number or column number
     * \param orientation  orientation, vertical or horizational
     * \param role         role, DisplayRole
     * \return  data for the given role and section in the specified header
     */
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role) const;
    /*!
     * \brief returns the item flags for the given index.
     * \param index  model index of item
     * \return  flags for the given index, ItemIsEnabled | ItemIsSelectable
     */
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    ///@}

    /** @name functions to access standard interface
     * convert between table model and standard interface
     */
    ///@{
    /*!
     * \brief add one item in model from item in standard interface.
     *
     * Contruct a new item based on standard interface item and add it to the
     * end of model.
     *
     * \param item  fault configuration item in standard interface
     */
    void appendItem( const EsyFaultConfigItem & item );
    /*!
     * \brief update item at row in model from item in standard interface.
     *
     * Change the value of item at row to the value in standard interface item.
     *
     * \param row       row of item
     * \param new_item  fault configuration item in standard interface
     */
    void updateItem( int row, const EsyFaultConfigItem & new_item );
};

/*!
 * \brief table view for fault list.
 */
class FaultListTableView : public QTableView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new table for fault list
     * \param parent  pointer to the parent widget
     */
    FaultListTableView( QWidget* parent = 0 );
};

#endif // NETWORK_CFG_TREE_H

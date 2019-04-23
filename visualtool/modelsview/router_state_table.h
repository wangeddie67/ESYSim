#ifndef ROUTER_STATE_TABLE_H
#define ROUTER_STATE_TABLE_H

#include "../define/qt_include.h"

#include "../subwidgets/router_graphics_item.h"

/*!
 * \brief model of router status
 *
 * Show the status of input/output ports, including the first flit in the buffer.
 */
class RouterStateModel : public QStandardItemModel
{
    Q_OBJECT

public:
    /*!
     * \brief type of router state table
     */
    enum RouterStateType
    {
        ROUTERSTATE_INPUT,  /**< state of input buffers */
        ROUTERSTATE_OUTPUT  /**< state of output buffers */
    };

    /*!
     * \brief column of router state view
     */
    enum RouterStateColumn
    {
        ROUTERSTATE_PORT = 0,  /**< port name */
        ROUTERSTATE_VC,        /**< virtual channel */
        ROUTERSTATE_STATE,     /**< state of finite state machine */
        ROUTERSTATE_SIZE,      /**< buffer utilization and size */
        ROUTERSTATE_FLIT,      /**< the sequence of first flit in buffer */
        ROUTERSTATE_SRC,       /**< source of first flit in buffer */
        ROUTERSTATE_DST,       /**< destination of first flit in buffer */
        ROUTERSTATE_COLUMN_COUNT /**< total count of column */
    };

protected:
    static QStringList const_viewheader_list;       /**< const string list for
        network configuration view header */

    RouterStateType m_type;  /**< type of router state, input or output */
    RouterGraphicsItem * mp_router_item;  /**< pointer to router item in graphic */

public:
    /** @name construct functions
     * Construct an new model.
     */
    ///@{
    /*!
     * \brief constructs a empty model
     * \param type    type of router state, input or output
     * \param parent  pointer to the parent object
     */
    RouterStateModel( RouterStateType type, QObject *parent ) :
        QStandardItemModel( parent ), m_type( type ), mp_router_item( 0 )
    {
        QList< QStandardItem * > itemlist;
        for ( int col = 0; col < ROUTERSTATE_COLUMN_COUNT; col ++ )
        {
            itemlist << new QStandardItem();
        }
        appendRow( itemlist );
    }
    ///@}

    /*!
     * \brief set pointer to router item in graphic
     * \param item  pointer to router item in graphic
     */
    void setRouterGraphicsItem( RouterGraphicsItem * item );

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
     * \return  ROUTERSTATE_COLUMN_COUNT
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
     *
     * For item in input/output physical channel column, show the name of port;
     * Otherwise, show the text in item directly.
     *
     * \param index  model index of item
     * \param role   DisplayRole, EditRole or CheckStateRole
     * \return  data stored under the given role for this item
     */
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
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
    ///@}

    /*!
     * \brief update the router state table.
     */
    void update();

protected:
    /*!
     * \brief check the pointer to router item
     * \return  if the pointer is 0, return FALSE; otherwise, return TRUE.
     */
    bool isValid() const { return (mp_router_item != 0); }
};

/*!
 * \brief table view for router status.
 */
class RouterStateTableView : public QTableView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new table for router status
     * \param parent  pointer to the parent widget
     */
    RouterStateTableView( QWidget * parent );
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

#endif // NETWORK_CFG_TREE_H

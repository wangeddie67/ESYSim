#ifndef PATH_TRACE_TABLE_H
#define PATH_TRACE_TABLE_H

#include "../define/qt_include.h"

#include "esy_interdata.h"
#include "esy_netcfg.h"

/*!
 * \brief sorting item based on standard item
 *
 * Sorting item based on the digital value in the item.
 */
class StandardSortingItem : public QStandardItem
{
public:
    /** @name construct functions
     * Construct an new item.
     */
    ///@{
    /*!
     * \brief constructs an empty item.
     */
    StandardSortingItem() :
        QStandardItem()
    {}
    /*!
     * \brief constructs a item with specified text.
     * \param text  specified text in the item
     */
    StandardSortingItem( const QString & text ) :
        QStandardItem( text )
    {}
    ///@}

    /*!
     * \brief reimplement the function of comparation
     * \param other  item to compare
     * \return  if this item is lower than other, return TRUE. Otherwise, return
     * false
     */
    bool operator<( const QStandardItem & other ) const
    {
        return ( text().toInt() < other.text().toInt() );
    }
};

/*!
 * \brief model of path trace list
 *
 * The model of path trace list for one path.
 * It hold a list of path trace item to present a path from source to
 * destination. Or a series of turns with specified condition.
 */
class PathTraceModel : public QStandardItemModel
{
    Q_OBJECT

protected:
    /*!
     * \brief column of path trace view
     */
    enum PathTraceColumn
    {
        PATHTRACE_ID = 0,   /**< router id */
        PATHTRACE_IN_PC,    /**< input physical channel */
        PATHTRACE_IN_VC,    /**< input virtual channel */
        PATHTRACE_OUT_PC,   /**< output physical channel */
        PATHTRACE_OUT_VC,   /**< output virtual channel */
        PATHTRACE_COUNTER,  /**< counter of frequency of this turn */
        PATHTRACE_COLUMN_COUNT /**< total count of column */
    };

    static QStringList const_viewheader_list;  /**< const string list for
        argument list view header */
    EsyNetworkCfg * mp_network_cfg;  /**< pointer to network configuration
        structure */

public:
    /** @name construct functions
     * Construct an new model.
     */
    ///@{
    /*!
     * \brief constructs a empty model
     * \param parent       pointer to the parent object
     * \param network_cfg  pointer to the network configuration
     */
    PathTraceModel( QObject *parent, EsyNetworkCfg * network_cfg ) :
        QStandardItemModel( parent ), mp_network_cfg( network_cfg )
    {}
    ///@}

    /*!
     * \brief set path trace in model
     * \param pathhop  path pair trace to add
     */
    void setPathTrace( const vector< EsyDataItemHop > & pathhop );
    /*!
     * \brief set path trace in model
     * \param pathpair  path pair trace to add
     */
    void setPathPair( const QVector< EsyDataItemTraffic > & pathpair );

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
     * \return  PATHTRACE_COLUMN
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
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
};

/*!
 * \brief table view for path trace list.
 */
class PathTraceTableView : public QTableView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new table for path trace list
     * \param parent  pointer to the parent widget
     */
    PathTraceTableView( QWidget * parent );
};

#endif // NETWORK_CFG_TREE_H

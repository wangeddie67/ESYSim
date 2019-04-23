#ifndef ANALYSE_RESULT_TABLE_H
#define ANALYSE_RESULT_TABLE_H

#include "../define/qt_include.h"
#include "path_trace_table.h"

/*!
 * \brief model of result table in analyse dialog.
 *
 * Add items in one row or one column.
 */
class ResultTableModel : public QStandardItemModel
{
    Q_OBJECT

private:
    QStringList m_header_list;  /**< header string list */
    int m_row_count;  /**< row count, default 0 */
    int m_column_count;  /**< column count, default 0 */

public:
    /** @name construct functions
     * Construct an new model.
     */
    ///@{
    /*!
     * \brief constructs a new model.
     * \param parent  pointer to the parent widget
     */
    ResultTableModel( QObject *parent ) :
        QStandardItemModel( parent ), m_header_list(),
        m_row_count( 0 ), m_column_count( 0 )
    {}
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
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the number of columns for the children of the given parent.
     * \param parent  the parent index of model
     * \return  the number of column
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
     * \param index  model index of item
     * \param role   role, DisplayRole or TextAlignmentRole
     * \return  data stored under the given role for this item
     */
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    /*!
     * \brief returns the data for the given role and section in the header
     * with the specified orientation.
     * \param section      row number or column number
     * \param orientation  orientation, vertical or horizational
     * \param role         role, DisplayRole or TextAlignmentRole
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

    /*!
     * \brief clear all data
     */
    void clearData();
    /*!
     * \brief append data of one column
     * \param header  header string
     * \param value   data list in one column
     */
    void appendDataVector( const QString & header,
        const QVector< double > & value );
    /*!
     * \brief append data of one row
     * \param list  data list in one row
     */
    void appendDataItem( const QStringList & list );
    /*!
     * \brief append data of one row, for statistic task.
     * \param argument  argument string
     * \param value     argument value
     * \param tooltip   tooltip value
     */
    void appendDataItem(
        const QString & argument, double value, const QString & tooltip );
    /*!
     * \brief set header string list
     * \param header  header string list
     */
    void setHeaderList( const QStringList & header );

    /*!
     * \brief write data to file.
     * \param file  path of data file
     * \return  error structure
     */
    ErrorHandler writeDataFile( const QString & file );
};

/*!
 * \brief table view for result table in analyse dialog.
 *
 * Results are shown in read-only table without vertical header.
 * Sorting enable, no edit trigger, multi selection
 */
class ResultTableView : public QTableView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new view.
     * \param parent
     */
    ResultTableView( QWidget * parent );
};

#endif // NETWORK_CFG_TREE_H

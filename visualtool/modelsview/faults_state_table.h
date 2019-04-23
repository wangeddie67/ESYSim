#ifndef FAULTS_STATE_TABLE_H
#define FAULTS_STATE_TABLE_H

#include "../define/qt_include.h"

#include "path_trace_table.h"

/*!
 * \brief model of status in state machine to control one fault.
 *
 * Only one column of data. Each item has two states, Living and Faulty
 */
class FaultStateModel : public QStandardItemModel
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a empty model
     * \param parent  pointer to the parent object
     */
    FaultStateModel( QObject *parent ) :
        QStandardItemModel( parent )
    { setColumnCount( 1 ); }

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
     * \return  1
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
     * \param index  model index of item
     * \param role   role, DisplayRole, TextAlignmentRole or TextColorRole
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
     * \brief resize the total number of states to size.
     *
     * If the new size is smaller than previous, keep the items in front.
     * Otherwise, add item with Living state at the end.
     *
     * \param size
     */
    void resizeStates( int size );

    /*!
     * \brief set the state of one item.
     * \param item    row of item
     * \param faulty  state of item
     */
    void setStateFaulty( int item, bool faulty );

    /** @name functions to access fault state vector
     */
    ///@{
    /*!
     * \brief set states vector to model.
     * \param value  states vector
     */
    void setStatesVector( const vector< long > & value );
    /*!
     * \brief get states vector from model.
     * \return  states vector
     */
    vector< long > getStatesVector();
    ///@}

public slots:
    /*!
     * \brief change item states from Living to Faulty or observe.
     *
     * When triggered, the states of item at index switch between Living (0) and
     * Faulty (1).
     *
     * \param index
     */
    void changeItemStates( const QModelIndex & index );
};

/*!
 * \brief model of transfer probability matrix in state machine to control one
 * fault.
 *
 * One square matrix, the size equals to the number of states.
 */
class FaultMatrixModel : public QStandardItemModel
{
    Q_OBJECT

protected:
    int m_matrix_size;  /**< size of matrix */

public:
    /*!
     * \brief constructs a empty model
     * \param parent  pointer to the parent object
     */
    FaultMatrixModel( QObject *parent ) :
        QStandardItemModel( parent ), m_matrix_size( 0 )
    {}

    /** @name functions for MVC
     * reimplementation functions for MVC.
     */
    ///@{
    /*!
     * \brief Returns the number of rows under the given parent.
     * \param parent  the parent index of model
     * \return  m_matrix_size
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the number of columns for the children of the given parent.
     * \param parent  the parent index of model
     * \return  m_matrix_size
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    /*!
     * \brief returns the data stored under the given role for the item
     * referred to by the index.
     * \param index  model index of item
     * \param role   role, DisplayRole, TextAlignmentRole or TextColorRole
     * \return  data stored under the given role for this item
     */
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value,
                  int role = Qt::EditRole );
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
     * \brief resize the total number of states to size.
     *
     * If the new size is smaller than previous, keep the items in front.
     * Otherwise, add item with {1, 0, ...} at the end.
     *
     * \param size
     */
    void resizeStates( int size );
    /*!
     * \brief set the probability from current state to next state.
     * \param current  current state
     * \param next     next state
     * \param prob     probability
     */
    void setTransferProb( int current, int next, double prob );

    /** @name functions to transfer probability matrix
     */
    ///@{
    /*!
     * \brief set probability matrix to model.
     * \param value  probability matrix
     */
    void setStatesMatrix( const vector< vector< double > > & value );
    /*!
     * \brief get probability matrix from model.
     * \return  probability matrix
     */
    vector< vector< double > > getStatesMatrix();
    ///@}
};

/*!
 * \brief table view for fault states.
 */
class FaultStateTableView : public QTableView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new table for status in state machine
     * \param parent  pointer to the parent widget
     */
    FaultStateTableView( QWidget* parent = 0 );
};

/*!
 * \brief table view for transform probability matrix.
 */
class FaultMatrixTableView : public QTableView
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a new table for transform probability matrix in state
     * machine
     * \param parent  pointer to the parent widget
     */
    FaultMatrixTableView( QWidget* parent = 0 );
};

#endif

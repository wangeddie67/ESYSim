#ifndef APPLICATION_GANTT_CHAR_H
#define APPLICATION_GANTT_CHAR_H

#include "../define/qt_include.h"

/*!
 * \brief structure to store the information of processor and task.
 * Including router id, application id, task id and
 * the request time, start time, exit time of task
 */
class ProcessorItem
{
public:
    /*!
     * \brief status of processor
     */
    enum ProcessorStatus{
        PE_WAIT = 1,   /**< Waiting the first packets */
        PE_RECEIVE,    /**< Receiving packets */
        PE_COMPUTING,  /**< Computing */
        PE_SEND,       /**< Sending packets out */
        PE_FINISH,     /**< Task has finished */
        PE_RELEASE     /**< Release, no task assigned */
    };

protected:
    static QStringList const_pe_status_list;  /**< string for task status */

    long m_router_id;  /**< router id */
    long m_app_id;  /**< application id */
    long m_task_id;  /**< task id */
    ProcessorStatus m_task_status;  /**< task status, one of ProcessorStatus */
    double m_request_time;  /**< request time */
    double m_start_time;  /**< start time */
    double m_exit_time;  /**< exit time */

    QColor m_color;  /**< color of application */

public:
    /*!
     * \brief constructs an empty entity of processor item.
     */
    ProcessorItem() :
        m_router_id( -1 ), m_app_id( -1 ), m_task_id( -1 ),
        m_task_status( ProcessorItem::PE_RELEASE ),
        m_start_time( 0.0 ), m_exit_time( 0.0 )
    {}
    /*!
     * \brief constructs an entity of processor item for specified router.
     * \param id  router id
     */
    ProcessorItem( long id ) :
        m_router_id( id ), m_app_id( -1 ), m_task_id( -1 ),
        m_task_status( ProcessorItem::PE_RELEASE ),
        m_request_time( 0.0 ), m_start_time( 0.0 ), m_exit_time( 0.0 )
    {}
    /*!
     * \brief constructs an entity of processor item for specified task.
     * \param app_id      application id
     * \param task_id     task id
     * \param task_state  task status
     * \param start_time  start time of this states
     * \param end_time    end time of this states
     */
    ProcessorItem( long app_id, long task_id, ProcessorItem::ProcessorStatus task_state,
                   double start_time, double end_time ) :
        m_app_id( app_id ), m_task_id( task_id ), m_task_status( task_state ),
        m_request_time( -1 ), m_start_time( start_time ), m_exit_time( end_time )
    {}
    /*!
     * \brief constructs an entity of processor by copying from another entity.
     * \param t  entity to copy
     */
    ProcessorItem( const ProcessorItem & t ) :
        m_app_id( t.appId() ), m_task_id( t.taskId() ), m_task_status( t.taskStatus() ),
        m_request_time( t.requestTime() ), m_start_time( t.startTime() ), m_exit_time( t.exitTime() )
    {}

    /*!
     * \brief returns application id.
     * \return  application id
     */
    long appId() const { return m_app_id; }
    /*!
     * \brief returns task id.
     * \return  task id
     */
    long taskId() const { return m_task_id; }
    /*!
     * \brief set task id and generate the color for this task
     * \param id  task id
     */
    void setTaskId( long id );
    /*!
     * \brief return task status.
     * \return task status
     */
    ProcessorStatus taskStatus() const { return m_task_status; }
    /*!
     * \brief set task status.
     * \param state  task state
     */
    void setTaskState( ProcessorStatus state ) { m_task_status = state; }
    /*!
     * \brief set request time.
     * \param t  request time
     */
    void setRequestTime( double t ) { m_request_time = t; }
    /*!
     * \brief returns request time.
     * \return  request time
     */
    double requestTime() const { return m_request_time; }
    /*!
     * \brief set start time.
     * \param t  start time
     */
    void setStartTime( double t ) { m_start_time = t; }
    /*!
     * \brief returns start time.
     * \return start time
     */
    double startTime() const { return m_start_time; }
    /*!
     * \brief set exit time.
     * \param t  exit time
     */
    void setExitTime( double t ) { m_exit_time = t; }
    /*!
     * \brief returns exit time.
     * \return  exit time
     */
    double exitTime() const { return m_exit_time; }

    /*!
     * \brief set color for application
     * \param r  red change
     * \param g  green change
     * \param b  blue change
     */
    void setAppColor( int r, int g, int b ) { m_color = QColor(r, g, b); }
    /*!
     * \brief return application color
     * \return  color for application
     */
    QColor appColor() { return m_color; }

    /*!
     * \brief conver process status to str
     * \param state  process status
     * \return  string of this process status
     */
    static QString processorStatus2Str( ProcessorStatus state )
        { return const_pe_status_list[ state ]; }
};

/*!
 * \brief structure of data for gantt chart.
 * The first dimension if routers or tasks
 */
class GanttDate: public QVector< QVector< ProcessorItem > >
{
public:
    /*!
     * \brief constructs a new entity with specified routers.
     * \param size  number of items in first dimension,
     */
    GanttDate( long size ) :
        QVector< QVector< ProcessorItem > >( size )
    {}

    /*!
     * \brief add item belongs to specified router.
     * \param router_id  router id
     * \param item       imte to add
     */
    void addItem( long router_id, const ProcessorItem & item )
        { operator []( router_id ).append( item ); }
    /*!
     * \brief generate a new processor item and added to specifed router.
     * \param router_id   router id
     * \param app_id      application id
     * \param task_id     task id
     * \param task_state  task status
     * \param start_time  start time of state
     * \param end_time    end time of state
     */
    void addItem( long router_id, long app_id, long task_id,
         ProcessorItem::ProcessorStatus task_state,
         double start_time, double end_time
    )
    { operator []( router_id ).append( ProcessorItem(
        app_id, task_id, task_state, start_time, end_time) ); }
};

#endif // APPLICATION_GANTT_CHAR_H

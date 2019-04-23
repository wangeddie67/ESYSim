#include "analyse_plot.h"

#define GANTT_HEIGHT 0.8

CanvasPlotPicker::CanvasPlotPicker( AnalysePlot *parent ) :
    QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        parent->canvas() ),
    mp_window( parent )
{}

QwtText CanvasPlotPicker::trackerTextF( const QPointF & pos ) const
{
    QwtText t_text( mp_window->plotPickerText( pos ) );
    t_text.setRenderFlags( Qt::AlignLeft );
    return t_text;
}


AnalysePlot::AnalysePlot( int x, int y, QWidget * parent ) :
    QwtPlot( parent ), mp_spectrogram( 0 ), m_gantt_data( x * y )
{
    for ( int i = 0; i < axisCnt; i ++ )
    {
        enableAxis( i, false );
    }

    mp_result_plot_picker = new CanvasPlotPicker( this );
    mp_result_plot_picker->setStateMachine( new QwtPickerClickPointMachine );

    QwtLegend *legend = new QwtLegend( this );
    legend->setDefaultItemMode( QwtLegendData::Checkable );
    insertLegend( legend, QwtPlot::BottomLegend );
    connect(legend, SIGNAL( checked( const QVariant &, bool, int ) ),
            this, SLOT( plotShowCurve( const QVariant &, bool ) ) );

    QwtLinearColorMap * mp_area_colormap =
            new QwtLinearColorMap( Qt::darkCyan, Qt::red );
    mp_area_colormap->addColorStop( 0.1, Qt::cyan );
    mp_area_colormap->addColorStop( 0.6, Qt::green );
    mp_area_colormap->addColorStop( 0.95, Qt::yellow );

    mp_spectrogram = new QwtPlotSpectrogram();
    mp_spectrogram->setColorMap( mp_area_colormap );

    mp_data_traffic_area = new 	QwtMatrixRasterData();
    mp_data_traffic_area->setResampleMode( QwtMatrixRasterData::NearestNeighbour );
    mp_data_traffic_area->setInterval( Qt::XAxis, QwtInterval( -0.5, x - 0.5 ) );
    mp_data_traffic_area->setInterval( Qt::YAxis, QwtInterval( -0.5, y - 0.5 ) );
    mp_data_traffic_area->setInterval( Qt::ZAxis, QwtInterval( 0.0, 10.0 ) );
    mp_spectrogram->setData( mp_data_traffic_area );

    mp_axis_colormap =
            new QwtLinearColorMap( Qt::darkCyan, Qt::red );
    mp_axis_colormap->addColorStop( 0.1, Qt::cyan );
    mp_axis_colormap->addColorStop( 0.6, Qt::green );
    mp_axis_colormap->addColorStop( 0.95, Qt::yellow );

    mp_barchart = new QwtPlotBarChart();

    m_axis_unit.resize( axisCnt );
    m_axis_title.resize( axisCnt );
}

void AnalysePlot::attachCurve( const QString & title, int axis,
    const QVector< double > & xData, const QVector< double > & yData )
{
    QwtPlotCurve * t_curve = new QwtPlotCurve( title );
    switch ( m_curve_list.size() )
    {
    case 0: t_curve->setPen( QPen( Qt::blue ) ); break;
    case 1: t_curve->setPen( QPen( Qt::red ) ); break;
    case 2: t_curve->setPen( QPen( Qt::black ) ); break;
    case 3: t_curve->setPen( QPen( Qt::green ) ); break;
    case 4: t_curve->setPen( QPen( Qt::magenta ) ); break;
    }
    t_curve->setSamples( xData, yData );
    t_curve->setZ( - m_curve_list.size() );
    t_curve->setYAxis( axis );
    t_curve->attach( this );
    m_curve_list.append( t_curve );
}

void AnalysePlot::attachSpectrogram(
        const QVector< double > &values, int numColumns )
{
    mp_data_traffic_area->setValueMatrix( values, numColumns );
    mp_spectrogram->attach( this );
    axisWidget( QwtPlot::yRight )->setColorBarEnabled( true );
}

void AnalysePlot::attachGantt(const GanttDate &data, int appid)
{
    m_gantt_data = data;

    for (int router = 0; router < m_gantt_data.size(); router++)
    {
        for (int i = 0; i < m_gantt_data[router].size(); i++)
        {
            ProcessorItem data_item = m_gantt_data[router][i];
            if ((appid != -1) && (appid != data_item.appId()))
            {
                continue;
            }

            QwtPlotShapeItem *t_item = new QwtPlotShapeItem("");
            t_item->setRenderHint(QwtPlotItem::RenderAntialiased, true);

            QPainterPath path;
            path.addRect(data_item.startTime(), router - GANTT_HEIGHT / 2,
                         data_item.exitTime() - data_item.startTime(), GANTT_HEIGHT);
            t_item->setShape(path);

            qsrand(100 * data_item.appId());
            int r = qrand() % 150 + 100;
            qsrand(100 * data_item.appId() + 5550);
            int g = qrand() % 150 + 100;
            qsrand(100 * data_item.appId() + 25120);
            int b = qrand() % 150 + 100;
            QColor pencolor(r, g, b);

            QColor fillColor;
            switch (data_item.taskStatus())
            {
            case ProcessorItem::PE_WAIT:
                fillColor = QColor(r, g, b, 100);
                break; // Qt::gray; fillColor.lighter(300); break;
            case ProcessorItem::PE_RECEIVE:
                fillColor = Qt::magenta;
                break;
            case ProcessorItem::PE_COMPUTING:
                fillColor = Qt::green;
                break;
            case ProcessorItem::PE_SEND:
                fillColor = Qt::red;
                break;
            case ProcessorItem::PE_FINISH:
                fillColor = QColor(r, g, b, 30);
                break; // Qt::gray; fillColor.lighter(400);break;
            case ProcessorItem::PE_RELEASE:
                fillColor = Qt::white;
                break;
            default:
                break;
            }
            t_item->setBrush(fillColor);

            QPen pen(pencolor);
            pen.setJoinStyle(Qt::MiterJoin);
            pen.setWidth(2);
            t_item->setPen(pen);

            t_item->attach(this);
            m_gantt_shape_list.append(t_item);
        }
    }
}

void AnalysePlot::attachBarChart( const QString & title, const QVector< double > & yData )
{
    mp_barchart->setTitle( title );
    mp_barchart->setSamples( yData );
    mp_barchart->attach( this );
}

void AnalysePlot::clearCurve()
{
    for ( int i = 0; i < m_curve_list.size(); i ++ )
    {
        m_curve_list[ i ]->detach();
        delete m_curve_list[ i ];
    }
    m_curve_list.clear();
    mp_spectrogram->detach();
    axisWidget( QwtPlot::yRight )->setColorBarEnabled( false );
    for ( int i = 0; i < m_gantt_shape_list.size(); i ++ )
    {
        m_gantt_shape_list[ i ]->detach();
        delete m_gantt_shape_list[ i ];
    }
    m_gantt_shape_list.clear();
    mp_barchart->detach();
}

void AnalysePlot::setAxisTitleUnit( int axisId,
    const QString & title, const QString & unit )
{
    m_axis_title[ axisId ] = title;
    m_axis_unit[ axisId ] = unit;

    QString t_str;
    if ( !title.isEmpty() )
    {
        if ( unit.isEmpty() )
        {
            t_str = title;
        }
        else
        {
            t_str = title + " (" + unit + ")";
        }
    }

    setAxisTitle( axisId, t_str );
}

QString AnalysePlot::plotPickerText( const QPointF & pos )
{
    int t_index = qRound( pos.x() );
    if ( t_index < 0 )
    {
        t_index = 0;
    }

    QString t_str;
    if ( m_matrix_picker_enable )
    {
        QwtMatrixRasterData * data =
            (QwtMatrixRasterData *)(mp_spectrogram->data());
        if ( data )
        {
            t_str += "Router:" + QString::number( qRound( pos.x() ) +
                data->numColumns() * qRound( pos.y() ) );
            t_str += "\n" + m_axis_title[ yRight ] + ":" +
                QString::number( data->value( pos.x(), pos.y() ) ) + " " +
                m_axis_unit[ yRight ];
        }
    }
    else if ( m_gantt_enable )
    {
        for ( int i = 0; i < m_gantt_data.size(); i ++ )
        {
            if ( ( pos.y() > (i - GANTT_HEIGHT / 2) ) &&
                 ( pos.y() < (i + GANTT_HEIGHT / 2) ) )
            {
                for ( int j = 0; j < m_gantt_data[ i ].size(); j ++ )
                {
                    if ( ( pos.x() > m_gantt_data[ i ][ j ].startTime() ) &&
                         ( pos.x() < m_gantt_data[ i ][ j ].exitTime() ) )
                    {
                        ProcessorItem t_item = m_gantt_data[ i ][ j ];
                        t_str = "Router id: " + QString::number(i) + "\n" + "Task: " +
                                QString::number(t_item.appId()) + " - " +
                                QString::number(t_item.taskId()) + "\n" +
                                "Status: " + ProcessorItem::processorStatus2Str( t_item.taskStatus() ) + "\n" +
                                "Time: " + QString::number((int)(pos.x()));
                    }
                }
            }
        }
    }
    else if ( m_barchart_enable )
    {
        t_str += m_axis_title[ xBottom ] + ":" + QString::number( t_index ) +
            " " + m_axis_unit[ xBottom ] + "\n";
        t_str += mp_barchart->title().text() + ":" +
            QString::number( mp_barchart->sample( t_index ).y() ) +
                " " +  m_axis_unit[ yLeft ];
    }
    else
    {
        if ( m_curve_list.size() > 0 )
        {
            t_str += m_axis_title[ xBottom ] + ":" + QString::number( t_index ) +
                " " + m_axis_unit[ xBottom ];
            long factor = (long)(m_curve_list[ 0 ]->sample( 1 ).x());
            long t_x_index = t_index / factor;
            for ( int i = 0; i < m_curve_list.size(); i ++ )
            {
                if ( t_x_index >= (int)m_curve_list[ i ]->dataSize() )
                {
                    t_x_index = (int)m_curve_list[ i ]->dataSize() - 1;
                }
                t_str += "\n" + m_curve_list[ i ]->title().text() + ":" +
                    QString::number( m_curve_list[ i ]->sample( t_x_index ).y() ) +
                    " " +  m_axis_unit[ m_curve_list[ i ]->yAxis() ];
            }
        }
    }

    m_result_plot_picker_position = pos;
    m_result_plot_picker_position.setX( t_index );

    return t_str;
}

void AnalysePlot::wheelEvent( QWheelEvent * e )
{
    if ( !m_wheel_enable )
    {
        return;
    }

    int canvasleft = canvas()->pos().x();
    int canvasright = canvas()->pos().x() + canvas()->size().width();
    int canvasup = canvas()->pos().y();
    int canvasbottom = canvas()->pos().y() + canvas()->size().height();

    double plot_pos_x = e->posF().x();
    double plot_pos_y = e->posF().y();

    int axisid = axisCnt;
    double plot_pos = 0.0;
    if ( (plot_pos_x < canvasleft) && (plot_pos_y > canvasup) && (plot_pos_y < canvasbottom) )
    {
        axisid = yLeft;
        plot_pos = invTransform( yLeft, plot_pos_y - canvasup );
    }
    else if ( (plot_pos_x > canvasright) && (plot_pos_y > canvasup) && (plot_pos_y < canvasbottom) )
    {
        axisid = yRight;
        plot_pos = invTransform( yRight, plot_pos_y - canvasup );
    }
    else if ( (plot_pos_x > canvasleft) && (plot_pos_x < canvasright) &&  (plot_pos_y < canvasup) )
    {
        axisid = xTop;
        plot_pos = invTransform( xTop, plot_pos_x - canvasleft );
    }
    else if ( (plot_pos_x > canvasleft) && (plot_pos_x < canvasright) &&  (plot_pos_y > canvasbottom) )
    {
        axisid = xBottom;
        plot_pos = invTransform( xBottom, plot_pos_x - canvasleft );
    }
    else
    {
        return;
    }

    double axismax = m_axis_max[ axisid ];
    double window_width = axisInterval( axisid ).maxValue() -
        axisInterval( axisid ).minValue();
    double t_min = 0;
    double t_max = axismax;

    // no key
    if ( e->modifiers() == Qt::NoModifier )
    {
        if ( e->delta() < 0 )   // left
        {
            t_min = axisScaleDiv( axisid ).lowerBound() -
                ( window_width / 10 );
            t_max = axisScaleDiv( axisid ).upperBound() -
                ( window_width / 10 );
        }
        else    // right
        {
            t_min = axisScaleDiv( axisid ).lowerBound() +
                ( window_width / 10 );
            t_max = axisScaleDiv( axisid ).upperBound() +
                ( window_width / 10 );
        }
    }
    // ctrl
    else if ( ( e->modifiers() & Qt::ControlModifier ) > 0 )
    {
        double leftrate = ( plot_pos - axisInterval( axisid ).minValue() ) /
                window_width;
        if ( e->delta() > 0 )   // bigger
        {
            window_width = qRound( window_width / 2 );
        }
        else    // small
        {
            window_width = qRound( window_width * 2 );
        }
        t_min = plot_pos - ( window_width * leftrate );
        t_max = plot_pos + ( window_width * (1 - leftrate) );
    }

    if ( t_max > axismax )
    {
        t_max = axismax;
        t_min = axismax - window_width;
    }
    if ( t_min < 0 )
    {
        t_min = 0;
        t_max = window_width ;
    }

    setAxisScale( axisid, t_min, t_max );
    replot();
    axisScaleHandler( axisid );
}

void AnalysePlot::resizeEvent( QResizeEvent *e )
{
    QwtPlot::resizeEvent( e );

    emit sizeChanged();
}

void AnalysePlot::plotShowCurve( const QVariant &itemInfo, bool on )
{
    QwtPlotItem *item = infoToItem( itemInfo );
    if ( item )
    {
        item->setVisible( on );
        replot();
    }
}

QwtInterval AnalysePlot::curvesYInterval( int axis )
{
    int t_count = 0;
    double t_min = 0;
    double t_max = 0;
    for ( int i = 0; i < m_curve_list.size(); i ++ )
    {
        if ( m_curve_list[ i ]->yAxis() == axis )
        {
            if ( t_count == 0 )
            {
                t_min = m_curve_list[ i ]->minYValue();
                t_max = m_curve_list[ i ]->maxYValue();
                t_count ++;
            }
            else
            {
                if ( m_curve_list[ i ]->minYValue() < t_min )
                {
                    t_min = m_curve_list[ i ]->minYValue();
                }
                if ( m_curve_list[ i ]->maxYValue() > t_max )
                {
                    t_max = m_curve_list[ i ]->maxYValue();
                }
            }
        }
    }

    return QwtInterval( t_min, t_max );
}

QwtInterval AnalysePlot::curvesXInterval()
{
    double t_min = 0;
    double t_max = 0;
    for ( int i = 0; i < m_curve_list.size(); i ++ )
    {
        if ( i == 0 )
        {
            t_min = m_curve_list[ i ]->minXValue();
            t_max = m_curve_list[ i ]->maxXValue();
        }
        else
        {
            if ( m_curve_list[ i ]->minXValue() < t_min )
            {
                t_min = m_curve_list[ i ]->minXValue();
            }
            if ( m_curve_list[ i ]->maxXValue() > t_max )
            {
                t_max = m_curve_list[ i ]->maxXValue();
            }
        }
    }

    return QwtInterval( t_min, t_max );
}

QwtInterval AnalysePlot::barChartYInterval()
{
    return QwtInterval( mp_barchart->boundingRect().top(), mp_barchart->boundingRect().bottom() );
}

QwtInterval AnalysePlot::barChartXInterval()
{
    return QwtInterval( 0, mp_barchart->data()->size() );
}

QwtInterval AnalysePlot::matrixZInterval()
{
    QVector< double > t_matrix = mp_data_traffic_area->valueMatrix();
    if ( t_matrix.size() == 0 )
    {
        return QwtInterval( 0, 0 );
    }
    double t_min = t_matrix[ 0 ];
    double t_max = t_matrix[ 1 ];
    for ( int i = 1; i < t_matrix.size(); i ++ )
    {
        if ( t_matrix[ i ] < t_min )
        {
            t_min = t_matrix[ i ];
        }
        if ( t_matrix[ i ] > t_max )
        {
            t_max = t_matrix[ i ];
        }
    }

    return QwtInterval( t_min, t_max );
}

void AnalysePlot::setZAxisScale( double min, double max )
{
    mp_data_traffic_area->setInterval( Qt::ZAxis, QwtInterval( min, max ) );
    axisWidget( QwtPlot::yRight )->setColorMap(
                QwtInterval( min, max ), mp_axis_colormap );
    setAxisScale( yRight, min, max );
}

QwtInterval AnalysePlot::ganttTimeInterval()
{
    double t_min = 0;
    double t_max = 0;
    bool init = true;
    for ( int i = 0; i < m_gantt_data.size(); i ++ )
    {
        for ( int j = 0; j < m_gantt_data[ i ].size(); j ++ )
        {
            if ( init )
            {
                t_min = m_gantt_data[ i ][ j ].startTime();
                t_max = m_gantt_data[ i ][ j ].exitTime();
                init = false;
            }
            else
            {
                if ( m_gantt_data[ i ][ j ].startTime() < t_min )
                {
                    t_min = m_gantt_data[ i ][ j ].startTime();
                }
                if ( m_gantt_data[ i ][ j ].exitTime() > t_max )
                {
                    t_max = m_gantt_data[ i ][ j ].exitTime();
                }
            }
        }
    }

    return QwtInterval( t_min, t_max );
}

void AnalysePlot::contextMenuEvent( QContextMenuEvent * e )
{
    QMenu * menu = new QMenu();
    QAction * t_menu = menu->addAction( "Fit Window" );
    connect( t_menu, SIGNAL(triggered()), this, SLOT(fitWindow()) );
    menu->exec( e->globalPos() );
    delete menu;
}

void AnalysePlot::fitWindow()
{
    if ( m_matrix_picker_enable )
    {
        setAxisScale( xTop, -0.5, mp_data_traffic_area->numColumns() -0.5 );
        setAxisScale( xBottom, -0.5, mp_data_traffic_area->numColumns() -0.5 );
        setAxisScale( yLeft, mp_data_traffic_area->numRows() - 0.5, -0.5 );
        if ( m_fixed_z )
        {
            setZAxisScale( m_fixed_z_axis.minValue(), m_fixed_z_axis.maxValue() );
        }
        else
        {
            setZAxisScale( matrixZInterval().minValue(),
                           matrixZInterval().maxValue() );
        }
    }
    else if ( m_gantt_enable )
    {
        setAxisScale( yLeft, - GANTT_HEIGHT / 2, m_gantt_data.size() + GANTT_HEIGHT / 2 );
        setAxisScale( yRight, 0, 0 );
        setAxisScale( xTop, 0, 0 );
        setAxisScale( xBottom, ganttTimeInterval().minValue(),
                               ganttTimeInterval().maxValue() );
    }
    else if ( m_barchart_enable )
    {
        if ( m_fixed_yleft )
        {
            setAxisScale( yLeft, m_fixed_yleft_axis.minValue(),
                                 m_fixed_yleft_axis.maxValue() );
        }
        else
        {
            setAxisScale( yLeft, barChartYInterval().minValue(),
                                 barChartYInterval().maxValue() );
        }
        setAxisScale( yRight, 0, 0 );
        setAxisScale( xTop, 0, 0 );
        setAxisScale( xBottom, barChartXInterval().minValue(),
                               barChartXInterval().maxValue() );
    }
    else
    {
        setAxisScale( yLeft, curvesYInterval( yLeft ).minValue(),
                             curvesYInterval( yLeft ).maxValue() );
        setAxisScale( yRight, curvesYInterval( yRight ).minValue(),
                              curvesYInterval( yRight ).maxValue() );
        setAxisScale( xTop, 0, 0 );
        setAxisScale( xBottom, curvesXInterval().minValue(),
                               curvesXInterval().maxValue() );
    }
    replot();

    for ( int i = 0; i < axisCnt; i ++ )
    {
        axisScaleHandler( i );
        m_axis_max[ i ] = axisInterval( i ).maxValue();
    }
}

void AnalysePlot::updateAxesHandler( int axis, double min, double max )
{
    switch( axis )
    {
    case PlotAxisConfigPanel::xEdit:
        if ( m_matrix_picker_enable )
        {
            setAxisScale( xTop, min, max );
        }
        else
        {
            setAxisScale( xBottom, min, max );
        }
        break;
    case PlotAxisConfigPanel::yLeftEdit:
        setAxisScale( yLeft, min, max );
        break;
    case PlotAxisConfigPanel::yRightEdit:
        if ( !m_matrix_picker_enable )
        {
            setAxisScale( yRight, min, max );
        }
        break;
    case PlotAxisConfigPanel::zEdit:
        if ( m_matrix_picker_enable )
        {
            setAxisScale( yRight, min, max );
        }
        break;
    }
    replot();
}

void AnalysePlot::axisScaleHandler( int axis )
{
    switch( axis )
    {
    case xTop:
        if ( m_matrix_picker_enable )
        {
            emit axisScaleChanged( PlotAxisConfigPanel::xEdit,
                                   false, axisInterval( xTop ) );
        }
        break;
    case xBottom:
        if ( !m_matrix_picker_enable )
        {
            emit axisScaleChanged( PlotAxisConfigPanel::xEdit,
                axisEnabled( axis ), axisInterval( xBottom ) );
        }
        break;
    case yLeft:
        emit axisScaleChanged( PlotAxisConfigPanel::yLeftEdit,
            axisEnabled( axis ) && (!m_matrix_picker_enable), axisInterval( yLeft ) );
        break;
    case yRight:
        if ( m_matrix_picker_enable )
        {
            emit axisScaleChanged( PlotAxisConfigPanel::zEdit,
                axisEnabled( axis ), axisInterval( yRight ) );
            emit axisScaleChanged( PlotAxisConfigPanel::yRightEdit,
                false, QwtInterval( 0.0, 0.0 ) );
        }
        else
        {
            emit axisScaleChanged( PlotAxisConfigPanel::zEdit,
                false, QwtInterval( 0.0, 0.0 ) );
            emit axisScaleChanged( PlotAxisConfigPanel::yRightEdit,
                axisEnabled( axis ), axisInterval( yRight ) );
        }
        break;
    }
}

PlotAxisConfigPanel::PlotAxisConfigPanel( QWidget * parent ) :
    ToolBarBox( "Plot Parameters", -1, parent )
{
    mp_x_label = new QLabel( "X" );
    mp_min_x_lineedit = new QLineEdit( "0" );
    connect( mp_min_x_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );
    mp_max_x_lineedit = new QLineEdit( "0" );;
    connect( mp_max_x_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );

    mp_yleft_label = new QLabel( "Y.L" );
    mp_min_yleft_lineedit = new QLineEdit( "0" );;
    connect( mp_min_yleft_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );
    mp_max_yleft_lineedit = new QLineEdit( "0" );;
    connect( mp_max_yleft_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );

    mp_yright_label = new QLabel( "Y.R" );
    mp_min_yright_lineedit = new QLineEdit( "0" );;
    connect( mp_min_yright_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );
    mp_max_yright_lineedit = new QLineEdit( "0" );;
    connect( mp_max_yright_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );

    mp_z_label = new QLabel( "Z" );
    mp_min_z_lineedit = new QLineEdit( "0" );;
    connect( mp_min_z_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );
    mp_max_z_lineedit = new QLineEdit( "0" );;
    connect( mp_max_z_lineedit, SIGNAL( editingFinished() ),
             this, SLOT( editingFinishedHandler() ) );

    QGridLayout * axis_para_layout = new QGridLayout();
    axis_para_layout->addWidget( new QLabel("Min"), 1, 0, 1, 1 );
    axis_para_layout->addWidget( new QLabel("Max"), 2, 0, 1, 1 );
    axis_para_layout->addWidget( mp_x_label,        0, 1, 1, 1, Qt::AlignCenter );
    axis_para_layout->addWidget( mp_min_x_lineedit, 1, 1, 1, 1 );
    axis_para_layout->addWidget( mp_max_x_lineedit, 2, 1, 1, 1 );
    axis_para_layout->addWidget( mp_yleft_label,        0, 2, 1, 1, Qt::AlignCenter);
    axis_para_layout->addWidget( mp_min_yleft_lineedit, 1, 2, 1, 1 );
    axis_para_layout->addWidget( mp_max_yleft_lineedit, 2, 2, 1, 1 );
    axis_para_layout->addWidget( mp_yright_label,        0, 3, 1, 1, Qt::AlignCenter );
    axis_para_layout->addWidget( mp_min_yright_lineedit, 1, 3, 1, 1 );
    axis_para_layout->addWidget( mp_max_yright_lineedit, 2, 3, 1, 1 );
    axis_para_layout->addWidget( mp_z_label,        0, 4, 1, 1, Qt::AlignCenter );
    axis_para_layout->addWidget( mp_min_z_lineedit, 1, 4, 1, 1 );
    axis_para_layout->addWidget( mp_max_z_lineedit, 2, 4, 1, 1 );

    axis_para_layout->setMargin( 0 );
    axis_para_layout->setSpacing( 5 );

    mp_box_layout->addLayout( axis_para_layout );
}

void PlotAxisConfigPanel::editingFinishedHandler()
{
    if (mp_x_label->isVisible())
    {
        emit updateAxis( xEdit, mp_min_x_lineedit->text().toDouble(),
                                mp_max_x_lineedit->text().toDouble() );
    }
    if (mp_yleft_label->isVisible())
    {
        emit updateAxis( yLeftEdit, mp_min_yleft_lineedit->text().toDouble(),
                                    mp_max_yleft_lineedit->text().toDouble() );
    }
    if (mp_yright_label->isVisible())
    {
        emit updateAxis( yRightEdit, mp_min_yright_lineedit->text().toDouble(),
                                     mp_max_yright_lineedit->text().toDouble() );
    }
    if (mp_z_label->isVisible())
    {
        emit updateAxis( zEdit, mp_min_z_lineedit->text().toDouble(),
                                mp_max_z_lineedit->text().toDouble() );
    }
}

void PlotAxisConfigPanel::axisScaleHandler( int axis, bool enable, const QwtInterval &interval )
{
    switch ( axis )
    {
    case xEdit:
        mp_x_label->setVisible( enable );
        mp_min_x_lineedit->setVisible( enable ); mp_max_x_lineedit->setVisible( enable );
        mp_max_x_lineedit->setText( QString::number( interval.maxValue() ) );
        mp_min_x_lineedit->setText( QString::number( interval.minValue() ) );
        mp_max_x_lineedit->setText( QString::number( interval.maxValue() ) );
        break;
    case yLeftEdit:
        mp_yleft_label->setVisible( enable );
        mp_min_yleft_lineedit->setVisible( enable ); mp_max_yleft_lineedit->setVisible( enable );
        mp_min_yleft_lineedit->setText( QString::number( interval.minValue() ) );
        mp_max_yleft_lineedit->setText( QString::number( interval.maxValue() ) );
        break;
    case zEdit:
        mp_z_label->setVisible( enable );
        mp_min_z_lineedit->setVisible( enable ); mp_max_z_lineedit->setVisible( enable );
        mp_min_z_lineedit->setText( QString::number( interval.minValue() ) );
        mp_max_z_lineedit->setText( QString::number( interval.maxValue() ) );
        break;
    case yRightEdit:
        mp_yright_label->setVisible( enable );
        mp_min_yright_lineedit->setVisible( enable ); mp_max_yright_lineedit->setVisible( enable );
        mp_min_yright_lineedit->setText( QString::number( interval.minValue() ) );
        mp_max_yright_lineedit->setText( QString::number( interval.maxValue() ) );
        break;
    }
}

PlotSizeConfigPanel::PlotSizeConfigPanel( QWidget * parent ) :
    ToolBarBox( "Plot Size", -1, parent )
{
    mp_plot_x_label = new QLabel( "width" );
    mp_plot_x_lineedit = new QLineEdit( "0" );
    connect( mp_plot_x_lineedit, SIGNAL( editingFinished() ),
             this, SIGNAL( editingFinished() ) );
    mp_plot_x_unit_label = new QLabel( "dpi" );

    mp_plot_y_label = new QLabel( "height" );
    mp_plot_y_lineedit = new QLineEdit( "0" );
    connect( mp_plot_y_lineedit, SIGNAL( editingFinished() ),
             this, SIGNAL( editingFinished() ) );
    mp_plot_y_unit_label = new QLabel( "dpi" );

    mp_plot_fix_checkbox = new QCheckBox( "Fix size", this );
    connect( mp_plot_fix_checkbox, SIGNAL(clicked()),
            this, SIGNAL(editingFinished()) );

    QGridLayout * axis_para_layout = new QGridLayout();
    axis_para_layout->addWidget( mp_plot_x_label,      0, 0, 1, 1 );
    axis_para_layout->addWidget( mp_plot_x_lineedit,   0, 1, 1, 1 );
    axis_para_layout->addWidget( mp_plot_x_unit_label, 0, 2, 1, 1 );
    axis_para_layout->addWidget( mp_plot_y_label,      1, 0, 1, 1 );
    axis_para_layout->addWidget( mp_plot_y_lineedit,   1, 1, 1, 1 );
    axis_para_layout->addWidget( mp_plot_y_unit_label, 1, 2, 1, 1 );
    axis_para_layout->addWidget( mp_plot_fix_checkbox, 2, 0, 1, 3 );

    axis_para_layout->setMargin( 0 );
    axis_para_layout->setSpacing( 5 );

    mp_box_layout->addLayout( axis_para_layout );
}


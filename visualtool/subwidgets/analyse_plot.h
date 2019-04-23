#ifndef ANALYSE_PLOT_H
#define ANALYSE_PLOT_H

#include "../define/qt_include.h"
#include "../modelsview/application_gantt_chart.h"

class AnalysePlot;

class CanvasPlotPicker : public QwtPlotPicker
{
private:
    AnalysePlot * mp_window;
public:
    CanvasPlotPicker( AnalysePlot *parent );
    QwtText trackerTextF( const QPointF & pos ) const;
};

class AnalysePlot : public QwtPlot
{
    Q_OBJECT
public:
    AnalysePlot( int x, int y, QWidget * parent = NULL );

private:
    CanvasPlotPicker * mp_result_plot_picker;
    double m_axis_max[ axisCnt ];
    QPointF m_result_plot_picker_position;

    bool m_matrix_picker_enable;
    bool m_wheel_enable;
    bool m_gantt_enable;
    bool m_barchart_enable;

    QVector< QwtPlotCurve * > m_curve_list;
    QwtPlotSpectrogram * mp_spectrogram;
    QVector< QwtPlotShapeItem * > m_gantt_shape_list;
    QwtPlotBarChart * mp_barchart;

    QwtMatrixRasterData * mp_data_traffic_area;
    QwtLinearColorMap * mp_axis_colormap;
    GanttDate m_gantt_data;

    QVector< QString > m_axis_title;
    QVector< QString > m_axis_unit;

    bool m_fixed_z;
    bool m_fixed_yleft;

    QwtInterval m_fixed_yleft_axis;
    QwtInterval m_fixed_z_axis;

public:
    void setMatrixPickerEnable( bool enable = true )
        { m_matrix_picker_enable = enable; }
    void setGanttEnable( bool enable = true ) { m_gantt_enable = enable; }
    void setWheelEnable( bool enable = true ) { m_wheel_enable = enable; }
    void setBarChartEnable( bool enable = true ) { m_barchart_enable = enable; }

    void setAxisTitleUnit( int axisId,
        const QString & title, const QString & unit = QString() );
    void setZAxisScale( double min, double max );
    void setMatrixSize( int x, int y );

    void setFixedZAxis( bool fixed, QwtInterval interval )
        { m_fixed_z = fixed; m_fixed_z_axis = interval; }
    void setFixedYLeftAxis( bool fixed, QwtInterval interval )
        { m_fixed_yleft = fixed; m_fixed_yleft_axis = interval; }

    QwtInterval curvesXInterval();
    QwtInterval curvesYInterval( int axis );
    QwtInterval matrixZInterval();
    QwtInterval ganttTimeInterval();
    QwtInterval barChartXInterval();
    QwtInterval barChartYInterval();

    QString plotPickerText( const QPointF & pos );

    void attachCurve( const QString & title, int axis,
        const QVector< double > & xData, const QVector< double > & yData );
    void attachSpectrogram( const QVector< double > &values, int numColumns );
    void attachGantt( const GanttDate & data, int appid );
    void attachBarChart( const QString & title, const QVector< double > & yData );

    void clearCurve();

    void wheelEvent( QWheelEvent * e );
    void resizeEvent( QResizeEvent * e );

    void contextMenuEvent( QContextMenuEvent * e );

signals:
    void axisScaleChanged( int axisId, bool enable, const QwtInterval & interval );
    void sizeChanged();

public slots:
    void plotShowCurve ( const QVariant &itemInfo, bool on );
    void updateAxesHandler( int axis, double min, double max );
    void axisScaleHandler( int axis );
    void fitWindow();
};

class PlotAxisConfigPanel : public ToolBarBox
{
    Q_OBJECT
public:
    enum axidEdit {
        xEdit,
        yLeftEdit,
        yRightEdit,
        zEdit,
        editCount
    };

    PlotAxisConfigPanel( QWidget * parent );

private:
    QLabel    * mp_x_label;
    QLineEdit * mp_min_x_lineedit;
    QLineEdit * mp_max_x_lineedit;
    QLabel    * mp_yleft_label;
    QLineEdit * mp_min_yleft_lineedit;
    QLineEdit * mp_max_yleft_lineedit;
    QLabel    * mp_yright_label;
    QLineEdit * mp_min_yright_lineedit;
    QLineEdit * mp_max_yright_lineedit;
    QLabel    * mp_z_label;
    QLineEdit * mp_min_z_lineedit;
    QLineEdit * mp_max_z_lineedit;

signals:
    void updateAxis( int axis, double min, double max );

public slots:
    void editingFinishedHandler();
    void axisScaleHandler( int axis, bool enable, const QwtInterval &interval );
};

class PlotSizeConfigPanel : public ToolBarBox
{
    Q_OBJECT
public:
    PlotSizeConfigPanel( QWidget * parent );

private:
    QLabel    * mp_plot_x_label;
    QLineEdit * mp_plot_x_lineedit;
    QLabel    * mp_plot_x_unit_label;
    QLabel    * mp_plot_y_label;
    QLineEdit * mp_plot_y_lineedit;
    QLabel    * mp_plot_y_unit_label;
    QCheckBox * mp_plot_fix_checkbox;

public:
    bool isFixed() { return mp_plot_fix_checkbox->isChecked(); }
    QSize plotSize() { return QSize(
        mp_plot_x_lineedit->text().toDouble(),
        mp_plot_y_lineedit->text().toDouble() ); }

public slots:
    void setPlotSize( const QSize & size ) {
        mp_plot_x_lineedit->setText( QString::number( size.width() ) );
        mp_plot_y_lineedit->setText( QString::number( size.height() ) ); }

signals:
    void editingFinished();
};

#endif // ANALYSEDIALOG_H

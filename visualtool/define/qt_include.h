#ifndef QTINCLUDE_H
#define QTINCLUDE_H

#include <QApplication>

#include <QtGui>

#include <QDialog>
#include <QTreeWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTableWidget>
#include <QSplitter>
#include <QMainWindow>
#include <QScrollBar>
#include <QDesktopWidget>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QToolBox>
#include <QLabel>
#include <QHeaderView>
#include <QFileDialog>
#include <QProgressBar>
#include <QProgressDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QButtonGroup>
#include <QStyledItemDelegate>
#include <QToolTip>
#include <QGraphicsSceneMouseEvent>
#include <QColorDialog>
#include <QFlags>
#include <QScrollArea>

#include <QDomDocument>

#include <QtGlobal>

#include <QtAlgorithms>

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_plot.h>
#include <qwt_global.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_plot_renderer.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_shapeitem.h>
#include <qwt_plot_barchart.h>

#include "gui_component.h"
#include "esy_xmlerror.h"

/*!
 * \brief describes the work direction of this program.
 */
extern QString global_work_direction;

/*!
 * \brief Error information interface, same to EsyXmlError.
 */
class ErrorHandler
{
public:
    /*!
     * \brief error code
     */
    enum ErrorCode{
        NONEERROR,      /**< no error */
        FILEOPENERROR,  /**< fail to not open file */
        DOMERROR,       /**< fail to parse XML file */
        XMLTAGERROR     /**< xml tag is not correct */
    };

protected:
    ErrorCode m_error_type;   /**< error code */
    QStringList m_error_msg;  /**< error messages */
    QString m_error_class;    /**< error class */
    QString m_error_fun;      /**< error function */

public:
    /*! @name Constructor */
    ///@{
    /*!
     * \brief constructs a structure for no error.
     */
    ErrorHandler() :
        m_error_type( NONEERROR ), m_error_msg(), m_error_class(), m_error_fun()
    {}
    /*!
     * \brief constructs a structure for one error.
     * \param err         error code
     * \param msg         error messages
     * \param pare_class  class where error occurs
     * \param pare_fun    function where error occurs
     */
    ErrorHandler( ErrorCode err, QStringList msg, QString pare_class, QString pare_fun ) :
        m_error_type( err ), m_error_msg( msg ),
        m_error_class( pare_class ), m_error_fun( pare_fun )
    {}
    /*!
     * \brief constructs a structure from EsyXmlError entity.
     * \param e  EsyXmlError entity
     */
    ErrorHandler( const EsyXmlError & e ) :
        m_error_type(), m_error_msg(), m_error_class(), m_error_fun()
    {
        m_error_type = (ErrorCode)e.errorType();
        for ( size_t i = 0; i < e.errorMsg().size(); i ++ )
        {
            m_error_msg.append( QString::fromStdString( e.errorMsg()[ i ] ) );
        }
        m_error_class.fromStdString( e.errorClass() );
        m_error_fun.fromStdString( e.errorFun() );
    }
    ///@}

    /*!
     * \brief check the error code.
     * \return If error code is NoError, return false; otherwise, return true.
     */
    bool hasError() { return !(m_error_type == NONEERROR);}
    /*!
     * \brief generate the title for QMessageBox.
     * \return title for QMessageBox
     */
    QString title();
    /*!
     * \brief generate the text for QMessageBox.
     * \return text for QMessageBox
     */
    QString text();
};

#endif // QTINCLUDE_H

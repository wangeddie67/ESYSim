#ifndef AUTOMATION_CONTROL_PANEL_H
#define AUTOMATION_CONTROL_PANEL_H

#include "../define/qt_include.h"

class AutomationControlPanel : public ToolBarBox
{
    Q_OBJECT
public:
    enum ControlType {
        ReplayControl,
        AnalyseControl,
        OnlineControl,
    };

public:
    AutomationControlPanel(
        ControlType type, QString stepunit, QString jumpunit, QWidget *parent = 0);

private:
    QProgressBar * mp_load_pd;
    QLabel       * mp_cycle_label;	// cycle label

    QLabel      * mp_step_label;
    QLineEdit   * mp_step_edit;	// step length
    QLabel      * mp_step_unit_label;

    QPushButton * mp_prevrun_button;
    QPushButton * mp_previous_button;
    QPushButton * mp_stop_button;
    QPushButton * mp_next_button;
    QPushButton * mp_nextrun_button;

    QLabel      * mp_jump_label;
    QLineEdit   * mp_jump_edit;
    QLabel      * mp_jump_unit_label;
    QPushButton * mp_jump_button;

    QPushButton * mp_analyse_button;

    QTimer * mp_prevrun_timer;
    QTimer * mp_nextrun_timer;

    bool m_jump_flag;

private:
    void setComponentEnable( bool enable, bool stop_en );

public:
    void setControlType( ControlType type );

    int stepValue() { return mp_step_edit->text().toInt(); }
    long long jumpValue() { return mp_jump_edit->text().toLongLong(); }

    void setLoadProgressBarRange( int min, int max )
        { mp_load_pd->setMinimum( min ); mp_load_pd->setMaximum( max ); }

    void setCycleLabel( const QString & label )
        { mp_cycle_label->setText( label ); }

    void enableAllComponent( bool enable );

    void initTimer();
    void updatePrevTimer();
    void updateNextTimer();
    void stopTimer();

signals:
    void prevRunButtonClicked();
    void previousButtonClicked();
    void stopButtonClicked();
    void nextButtonClicked();
    void nextrunButtonClicked();
    void jumpButtonClicked();
    void analyseButtonClicked();
    void anyButtonClicked();

    void prevRunTimeOut();
    void nextRunTimeOut();

public slots:
    void prevRunButtonClickedHandler();
    void previousButtonClickedHandler();
    void stopButtonClickedHandler();
    void nextButtonClickedHandler();
    void nextrunButtonClickedHandler();
    void jumpButtonClickedHandler();
    void analyseButtonClickedHandler();

    void setLoadProgressBarValue( int value ) { mp_load_pd->setValue( value ); }
};

#endif // AUTOMATION_CONTROL_PANEL_H

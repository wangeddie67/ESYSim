#ifndef TOOLS_CONFIG_DIALOG_H
#define TOOLS_CONFIG_DIALOG_H

#include "../define/qt_include.h"

#include "../modelsview/argument_list_table.h"

class LoopLevelClass
{
public:
    int m_level;
    int m_total;
    int m_seq;
    QStringList m_value;

public:
    LoopLevelClass( int level, int total, const QStringList & value ) :
        m_level( level ), m_total( total ), m_seq( 0 ), m_value( value )
    {}
    LoopLevelClass( const LoopLevelClass & t ) :
        m_level( t.m_level ), m_total( t.m_total ), m_seq( t.m_seq ), m_value( t.m_value )
    {}
    bool increase()
    {
        m_seq ++;
        if ( m_seq == m_total )
        {
            m_seq = 0;
            return true;
        }
        return false;
    }
    int level() { return m_level; }
    QString currentString() { return m_value[ m_seq ]; }
};

bool loopLevelLessThan( const QPair<int, LoopLevelClass> &s1, const QPair<int, LoopLevelClass> &s2 );

// configuration dialog for traffic generator
class ToolsConfigDialog : public QDialog
{
	Q_OBJECT

	// constructor and destructor
public:
    ToolsConfigDialog( QString fname, bool onlyone = false );
    ~ToolsConfigDialog( void );
	
	// components variables
private:
    bool m_onlyone_model;

    QPushButton  * mp_ok_pushbutton;	// "ok" button

    ArgumentListTableView * mp_argu_table;	// parameter view
    ArgumentListModel * mp_argu_model;

    QPushButton  * mp_insert_loop_pushbutton;

    QLabel       * mp_command_label;
    QTableWidget * mp_command_tablewidget;	// show the final command

    QLabel       * mp_process_label;
    QSpinBox     * mp_process_spinbox; // parallel count
    FileLineEdit * mp_result_file_lineedit;

    QHBoxLayout  * mp_button_layout;	// horizontal layout, shows the buttons
    QVBoxLayout  * mp_parallel_layout;// vertical layout, shows the parallel arguments
    QVBoxLayout  * mp_table_layout;	// vertical layout, shows the table
	
    // variable of environment
private:
    QString m_cfg_file_dir;

    // components initialization functions
private:
  	// add components
	void addComponents();

	// slots functions
private slots:
	// push ok button, check command
    void okButtonHandler();
	// update command when items changed
    void changeItemHandler();
	// insert loop
    void insertLoopVarHandler();
	
	// I/O interface
public:
    QString getOutputFileName() { return mp_result_file_lineedit->text(); }
    int processCount() { return mp_command_tablewidget ->rowCount(); }
    QString processCommand( int i )
        { return mp_command_tablewidget ->item( i, 0 ) ->text(); }
    int availableNum()  { return mp_process_spinbox ->value(); }
    QString processDirection() { return mp_argu_model->direction(); }

private:
    void resizeEvent(QResizeEvent *);
};

#endif

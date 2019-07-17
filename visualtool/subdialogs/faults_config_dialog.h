#ifndef FAULTS_CONFIG_DIALOG_H
#define FAULTS_CONFIG_DIALOG_H

#include "../define/qt_include.h"

#include "result_base_dialog.h"

#include "esy_netcfg.h"

#include "../modelsview/faults_state_table.h"
#include "../modelsview/faults_list_table.h"

// configuration dialog for traffic generator
class FaultsConfigDialog : public ResultBaseDialog
{
	Q_OBJECT
	// constructor and destructor
public:
    /*!
     * \brief The TypicalFaultType enum
     */
    enum TypicalFaultType
    {
        TYPICALFAULT_CUSTOM,
        TYPICALFAULT_CONST_LIVING,
        TYPICALFAULT_CONST_FAULTY,
        TYPICALFAULT_TWO_STATES,
        TYPICALFAULT_TYPE_NUM
    };
    static QStringList const_typical_fault_type_str;

    FaultsConfigDialog( QString fname,
                        const EsyNetworkCfg & network_cfg );
	
	// components variables
private:
    QLabel    * mp_type_label;
    QComboBox * mp_type_combobox;
    QLabel    * mp_router_label;
    QComboBox * mp_router_combobox;
    QLabel    * mp_port_label;
    QComboBox * mp_port_combobox;
    QLabel    * mp_vc_label;
    QComboBox * mp_vc_combobox;
    QLabel    * mp_bit_label;
    QComboBox * mp_bit_combobox;

    QComboBox * mp_typical_combobox;

    QSpinBox * mp_state_spinbox;

    FaultStateTableView * mp_state_table;
    FaultStateModel     * mp_state_model;
    FaultMatrixTableView * mp_matrix_table;
    FaultMatrixModel     * mp_matrix_model;

    FaultListTableView * mp_faults_table;
    FaultListModel     * mp_faults_model;

    QPushButton * mp_insert_pushbutton;
    QPushButton * mp_delete_pushbutton;
    QPushButton * mp_edit_pushbutton;
    QPushButton * mp_save_pushbutton;

    QPushButton * mp_ok_pushbutton;
    QPushButton * mp_cancel_pushbutton;

    // variable of environment
private:
    EsyNetworkCfg m_network_cfg;
    QString m_cfg_file_dir;
//    FaultConfigList m_fault_cfg_list;

    int m_current_index;
    bool m_edit_enable;

    // components initialization functions
private:
  	// add components
	void addComponents();
    void setEditComponentEnable( bool enable );

	// slots functions
private slots:
    void typeComboBoxChangedHandler( int index );
    void routerComboBoxChangedHandler( int index );
    void portComboBoxChangedHandler( int index );
    void vcComboBoxChangedHandler( int index );

    void stateSpinboxChangedHandler( int value );

    void typicalComboBoxChangedHandler( int index );

    void insertButtonClickHandler();
    void deleteButtonClickHandler();
    void editButtonClickHandler();
    void saveButtonClickHandler();

    void okButtonClickHandler();
    void cancelButtonClickHandler();
};

#endif

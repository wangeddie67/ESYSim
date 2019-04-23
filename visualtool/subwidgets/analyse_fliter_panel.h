#ifndef ANALYSE_FLITER_PANEL_H
#define ANALYSE_FLITER_PANEL_H

#include "../define/qt_include.h"

#include "esy_networkcfg.h"

class AnalyseFilterPanel : public ToolBarBox
{
    Q_OBJECT
public:
    enum FliterType {
        FLITER_SOUR   = 0x01,
        FLITER_DEST   = 0x02,
        FLITER_ROUTER = 0x04,
        FLITER_INTER  = 0x08,
        FLITER_APP    = 0x10,
    };

    Q_DECLARE_FLAGS( FliterFlags, FliterType )

public:
    AnalyseFilterPanel(EsyNetworkCfg * cfg, QWidget *parent);

private:
    EsyNetworkCfg * mp_network_cfg;

    QLabel    * mp_src_id_label;
    QComboBox * mp_src_id_combobox;
    QLabel    * mp_dst_id_label;
    QComboBox * mp_dst_id_combobox;
    QLabel    * mp_addr_label;
    QComboBox * mp_addr_combobox;
    QLabel    * mp_input_phy_label;
    QComboBox * mp_input_phy_combobox;
    QLabel    * mp_input_vc_label;
    QComboBox * mp_input_vc_combobox;
    QLabel    * mp_output_phy_label;
    QComboBox * mp_output_phy_combobox;
    QLabel    * mp_output_vc_label;
    QComboBox * mp_output_vc_combobox;
    QLabel    * mp_app_label;
    QComboBox * mp_app_combobox;

public:
    void setVisibleFlag( const FliterFlags & flag );

    int srcId() { return mp_src_id_combobox->currentIndex() - 1; }
    int dstId() { return mp_dst_id_combobox->currentIndex() - 1; }
    int routerId() { return mp_addr_combobox->currentIndex() - 1; }
    int inputPhy() { return mp_input_phy_combobox->currentIndex() - 1; }
    int inputVc() { return mp_input_vc_combobox->currentIndex() - 1; }
    int outputPhy() { return mp_output_phy_combobox->currentIndex() - 1; }
    int outputVc() { return mp_output_vc_combobox->currentIndex() - 1; }
    int appId() { return mp_app_combobox->currentIndex() - 1; }

    void setRouterCount( long count );
    void setPhyCount( long count );
    void setVcCount( long count );
    void setAppCount( long count );

signals:

public slots:
    void routerIdChangedHandler( int index );
    void inputPhyChangedHandler( int index );
    void outputPhyChangedHandler( int index );

};

Q_DECLARE_OPERATORS_FOR_FLAGS( AnalyseFilterPanel::FliterFlags )

#endif // ANALYSE_FLITER_PANEL_H

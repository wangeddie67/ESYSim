#ifndef GRAPHICSCONFIGDIALOG_H
#define GRAPHICSCONFIGDIALOG_H

#include "../define/qt_include.h"
#include "../modelsview/graphics_config_tree.h"

class GraphicsConfigDialog : public QDialog
{
    Q_OBJECT
private:
    GraphicsConfigTreeView * mp_config_table_tree;
    GraphicsConfigModel    * mp_config_table_model;

    QPushButton * mp_default_button;
    QPushButton * mp_ok_button;
    QPushButton * mp_cancel_button;
public:
    GraphicsConfigDialog(QWidget *parent = 0);
    GraphicsConfigModel * graphicsConfigModel() { return mp_config_table_model; }
private:
    void addComponent();
signals:

public slots:
    void okButtonHandler();
};

#endif // GRAPHICSCONFIGDIALOG_H

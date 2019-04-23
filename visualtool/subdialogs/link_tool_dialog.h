/*
 T his program is free s*oftware; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA  02110-1301, USA.

 ---
 Copyright (C) 2015, Junshi Wang <>
 */

/*************************************************
  File Name :
    link_tool_dialog.h
  Description :
    Link tool interface by Qt
  Namespace :
    linktool
  Class List :
    LinkToolModelItem
    LinkToolModel
    FatherCombox
    LinkToolDialog
*************************************************/

/* file macro define */
#ifndef LINK_TOOL_H
#define LINK_TOOL_H

/* including head file */
#include "../define/qt_include.h"
#include "../modelsview/link_tool_table.h"

/*************************************************
  Name space :
    linktool

  Description :
    Link tool interface by Qt

  Public Type :
    LinkToolListViewColumn enum
    LinkToolTypeEnum enum

  Macro Define :
    string used by xml file
    string used for father combox

  Global define :
    const QStringList const_linktool_father_options
    const QStringList const_linktool_viewheader_list

  Class list :
    LinkToolModelItem
    LinkToolModel
    FatherCombox
    LinkToolDialog
*************************************************/

/*************************************************
  Class Name :
    LinkToolDialog

  Description :
    dialog of link tool configuration and renew

  Inherted :
    QDialog

  Properties :
    m_type            : LinkToolDialogType
    mp_layout         : QVBoxLayout *
    mp_edit_layout    : QHBoxLayout *
    mp_button_layout  : QHBoxLayout *
    mp_linktool_view  : QTreeView *
    mp_linktool_model : LinkToolModel *
    mp_add_button     : QPushButton *
    mp_remove_button  : QPushButton *
    mp_ok_button      : QPushButton *
    mp_cancel_button  : QPushButton *

  Public Functions :
    LinkToolDialog( LinkToolModel * model, LinkToolDialogType type );

  Private Functions :
    void drawComponent();

  Private slots functions :
    void okButtonClickedHandler();
    void addButtonClickedHandler();
    void removeButtonClickedHandler();
*************************************************/
class LinkToolDialog : public QDialog
{
    Q_OBJECT
/* Properties */
private:
    /* dialog type */
    LinkToolDelegate::LinkToolViewType m_type;

    /* layout widget */
    QVBoxLayout * mp_layout;
    QHBoxLayout * mp_button_layout;

    /* link tool model and view */
    LinkToolTableView * mp_linktool_view;
    LinkToolModel     * mp_linktool_model;
    LinkToolDelegate  * mp_linktool_delegate;

    /* push buttons */
    QPushButton * mp_add_button;
    QPushButton * mp_remove_button;
    QPushButton * mp_ok_button;
    QPushButton * mp_cancel_button;

/* Public functions */
public:
    /* constructor */
    LinkToolDialog( LinkToolModel * model,
                    LinkToolDelegate::LinkToolViewType type );
    /* function to access variables */
    LinkToolModel * linkToolModel() { return mp_linktool_model; }

/* Private slot functions */
private slots:
    /* ok button click handler */
    void okButtonClickedHandler();
    /* add button click handler */
    void addButtonClickedHandler();
    /* remove button click handler */
    void removeButtonClickedHandler();
};

#endif // LINK_TOOL_DIALOG_H

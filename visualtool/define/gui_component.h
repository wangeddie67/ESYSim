#ifndef RIBBONCOMPONENT_H
#define RIBBONCOMPONENT_H

#include <QWidget>
#include <QSplitter>
#include <QTabWidget>
#include <QToolButton>
#include <QTableWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDockWidget>
#include <QMainWindow>
#include <QFlags>
#include <QScrollArea>

/*!
 * \brief Splitter widget used in toolbar tab, between boxes.
 */
class ToolBarSplitter : public QWidget
{
    Q_OBJECT

public:
    /*!
     * \brief Constructer function
     *
     * Set fixed width is 3 px.
     *
     * \param parent pointer to parent widget
     */
    explicit ToolBarSplitter( QWidget * parent = 0 );

protected slots:
    /*!
     * \brief repaint the widget.
     *
     * Draw a vertical dark grey line in the middle of widget.
     *
     * \param event pointer to point event, unused.
     */
    void paintEvent( QPaintEvent * event );
};

/*!
 * \brief Button widget used in toolbar tab.
 *
 * The widget has two buttons, main button and menu button.
 * If the button does not provide a menu, only the main button is created,
 * and shows the icon, text.
 * If the button provides a menu, main button only shows the icon and menu
 * button shows the text and the menu. The action of main event is the latest
 * triggered action in menu.
 */
class ToolBarButton : public QWidget
{
    Q_OBJECT

public:
    /*!
     * \brief Button size.
     */
    enum SizeEnum{
        LargeButton,  /**< large button. large icon, vertical architecture,
            * 3 rows */
        SmallButton   /**< small button. small icon, horizational architecture,
            * 1 row */
    };

protected:  
    QToolButton * mp_main_button;  /**< pointer to main button */
    QToolButton * mp_menu_button;  /**< pointer to menu button */
    bool m_with_menu;  /**< button has menu */

public:
    /*!
     * \brief constructs a button without menu.
     * \param icon   icon of button
     * \param text   text of button
     * \param code   event code triggered by this button
     * \param size   button size, LargeButton or SmallButton
     * \param parent pointer to the parent widget
     */
    explicit ToolBarButton(
        const QIcon & icon, const QString & text, int code = -1,
        SizeEnum size = SmallButton, QWidget * parent = 0 );
    /*!
     * \brief constructs a button with menu
     * \param menu   menu list of actions
     * \param size   button size, LargeButton or SmallButton
     * \param parent pointer to the parent widget
     */
    explicit ToolBarButton(
        QMenu * menu, SizeEnum size = SmallButton, QWidget * parent = 0 );
    /*!
     * \brief returns the default action of button.
     * \return the default action
     */
    QAction * defaultAction() const { return mp_main_button->defaultAction(); }

signals:
    /*!
     * \brief triggered event.
     * \param action trigged action
     */
    void triggered( QAction * action );
    /*!
     * \brief clicked event.
     * \param checked checked state
     */
    void clicked( bool checked = false );

public slots:
    /*!
     * \brief sets default action of this button
     * \param action default action
     */
    void setDefaultAction( QAction * action );
    /*!
     * \brief handles the change on default action, for example, enable/disable.
     * when any changes happen on default action, the text will be updated by
     * add '\n' at the end.
     */
    void actionChangedHandler();
};

/*!
 * \brief Scroll area used in toolbar tab to show the buttons in scrollable list.
 */
class ToolBarButtonScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    /*!
     * \brief constructs a list for buttons
     * \param buttonlist list of buttons in the area
     * \param parent pointer to parent widget
     */
    explicit ToolBarButtonScrollArea(
        QList< ToolBarButton * > buttonlist, QWidget * parent = 0 );

signals:
    /*!
     * \brief triggered event.
     * \param action trigged action
     */
    void triggered( QAction* action );
};

/*!
 * \brief Toolbar box widget of a group components with a title.
 */
class ToolBarBox : public QWidget
{
    Q_OBJECT

protected:
    int m_code;  /**< code of this box */
    QHBoxLayout * mp_box_layout;  /**< pointer to layout */
    QList< QWidget * > m_removeable_widget;  /**< list of component can be
        * removed */
    QMap< int, QLineEdit * > m_lineedit_list; /**< list of pointer to line edit
        key is the special code of line edit. */

public:
    /*!
     * \brief constructs a new box.
     * \param title  title of box
     * \param code   code of box
     * \param parent pointer to the parent widget
     */
    explicit ToolBarBox(const QString & title, int code, QWidget * parent = 0);
    /*!
     * \brief adds a button list component
     * \param area  scroll area to added
     */
    void addButtonScrollArea(ToolBarButtonScrollArea * area);
    /*!
     * \brief adds a large button, with two stretches on the top and bottom.
     * \param button  the button to add
     */
    void addLargeButton(ToolBarButton * button);
    /*!
     * \brief adds small buttons, with three in one column and two stretches on
     * on the top and bottom
     * \param list  buttons to add
     */
    void addSmallButton(QList<ToolBarButton *> list);
    /*!
     * \brief remove widgets in m_removeable_widget.
     */
    void removeWidgets();
    /*!
     * \brief regist line edit with specified code.
     * \param key     code of this editer
     * \param editer  pointer to line edit
     */
    void addLineEdit(int key, QLineEdit * editer)
    { m_lineedit_list.insert( key, editer ); }
    /*!
     * \brief return the text of specified line edit
     * \param key  code of the editer to access
     * \return  current text of this editor. If not found, return 0.
     */
    QString lineEditText(int key) const
    { return m_lineedit_list.value( key )->text(); }
    /*!
     * \brief remove the specified editor
     * \param key  code of the editer to remove
     * \return  the number of removed item.
     */
    int remove(int key) { return m_lineedit_list.remove( key ); }
    /*!
     * \brief returns code of box.
     * \return  code of this box
     */
    int boxCode() { return m_code; }
    /*!
     * \brief returns layout of box.
     * \return  layout of this box
     */
    QHBoxLayout * hBoxLayout() { return mp_box_layout; }

signals:
    /*!
     * \brief triggered event.
     * \param action trigged action
     */
    void triggered(QAction * action);
};

/*!
 * \brief Toolbar tab widget used in Toolbar
 */
class ToolBarTab : public QWidget
{
    Q_OBJECT

protected:
    QHBoxLayout * mp_tab_layout;  /**< layout of tab */
    QList< ToolBarBox* > m_box_list;  /**< list of boxes in tab */
    int m_code;  /**< code of tab */
    QString m_tab_name;  /**< name of tab */

public:
    /*!
     * \brief constructs a new tab.
     * \param name    name of tab
     * \param code    code of tab
     * \param parent  pointer to the parent widget
     */
    explicit ToolBarTab(const QString & name, int code, QWidget * parent = 0);
    /*!
     * \brief add a box to the tab.
     * \param box  box to added
     */
    void addToolBarBox(ToolBarBox * box);
    /*!
     * \brief returns the code of tab.
     * \return  code of this tab
     */
    int tabCode() const { return m_code; }
    /*!
     * \brief finds to the box with specified code.
     * If the box is found, return the pointer to the box;
     * otherwise, return 0.
     * \param code  code of box to find.
     * \return  the pointer to the box, otherwise 0.
     */
    ToolBarBox * boxOfCode(int code);
    /*!
     * \brief returns layout of tab.
     * \return  layout of this box
     */
    QHBoxLayout * hBoxLayout() { return mp_tab_layout; }
    /*!
     * \brief returns name of tab.
     * \return  name of this box
     */
    const QString & tabName() { return m_tab_name; }

signals:
    /*!
     * \brief triggered event.
     * \param action trigged action
     */
    void triggered(QAction * action);
};

/*!
 * \brief Toolbar widget in ribbon style. The toolbar is a tab widget.
 * Each tab contains some boxes with components. Toolbar provide actions to
 * control the functions.
 */
class ToolBarWidget: public QTabWidget
{
    Q_OBJECT

protected:    
    QList< ToolBarTab* > m_tab_list;  /**< list of tabs in this toolbar */

public:
    /*!
     * \brief constructs a new toolbar widget.
     * \param parent  pointer to the parent widget
     */
    explicit ToolBarWidget(QWidget * parent = 0);
    /*!
     * \brief add a tab to the toolbar.
     * \param tab  tab to added
     */
    void addToolBarTab( ToolBarTab *tab );
    /*!
     * \brief remove tab at index in the toolbar
     * \param index  position of tab to remove
     */
    void removeToolBarTab(int index);
    /*!
     * \brief set tab with specified code visible or not
     * \param code     code of specified tab
     * \param visible  flag of visible, true or false.
     */
    void setTabVisible(int code, bool visible);
    /*!
     * \brief finds to the tab with specified code.
     * If the tab is found, return the pointer to the tab;
     * otherwise, return 0.
     * \param code  code of tab to find.
     * \return  the pointer to the tab, otherwise 0.
     */
    ToolBarTab * tabOfCode(int code);
signals:
    /*!
     * \brief triggered event.
     * \param action trigged action
     */
    void triggered(QAction * action);
};

/*!
 * \brief Main window in ribbon style
 */
class RibbonMainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    ToolBarWidget * mp_toolbar_widget;  /**< pointer to toolbar widget */
    QWidget * mp_central_widget;  /**< pointer to the central widget
        * in the window */
    QVBoxLayout * mp_main_layout;  /**< pointer of the main layout */

public:
    /*!
     * \brief constructs a new window.
     * \param parent  the pointer to the parent widget
     * \param flags   flag of window
     */
    explicit RibbonMainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    /*!
     * \brief set central widget
     * \param widget  pointer to central widget
     */
    void setRibbonCentralWidget(QWidget * widget);
    /*!
     * \brief set toolbar widget
     * \param widget  pointer to toolbar widget
     */
    void setRibbonToolbarWidget(ToolBarWidget * widget);
};

/*!
 * \brief Line edit widget for file direction.
 * It provides an extra file button which will start the file dialog directly.
 * The line edit can be edited directly as well.
 */
class FileLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /*!
     * \brief describes the style of file dialog
     */
    enum FileType{
        NEW_FILE,     /**< create new file. file dialog can create new file.
            * file filter is "*.*". */
        OPEN_FILE,    /**< open exist file. file dialog can choose from exist
            * file only. file filter is "*.*". */
        NEW_FILE_IF,  /**< create new file with data file interface.
            * file dialog can create new file.
            * file filter including "*.*", ".txt", ".bin". */
        OPEN_FILE_IF  /**< open exist file with data file interface.
            * file dialog can choose from exist file only.
            * file filter including "*.*", ".txt", ".bin". */
    };

public:
    /*!
     * \brief constructe a new line edit for file direction.
     * \param type      type of file dialog,
     *                  including NEW_FILE, OPEN_FILE, NEW_FILE_IF, OPEN_FILE_IF
     * \param contents  default contents in the line edit
     * \param path      current path of file dialog
     * \param fliter    file filter of file dialog
     * \param parent    pointer to the parent widget
     */
    FileLineEdit(
        FileType type, const QString & contents, const QString & path,
        const QString & fliter, QWidget * parent = 0
    );

protected:
    QPushButton * mp_file_button;  /**< pointer to the push button to
        * start file dialog */
    FileType m_type;  /**< tye of file dialog */
    QString m_path;  /**< current path of file dialog */
    QString m_fliter;  /**< file filter of file dialog */

protected:
    /*!
     * \brief resize event to adjust the size of file push button, which is a
     * square at any time
     * \param event  pointer to event strucut
     */
    void resizeEvent( QResizeEvent * event );

protected slots:
    /*!
     * \brief when the file button is clicked, start the file dialog.
     */
    void fileButtonClickHandler();
};

/*!
 * \brief Line edit widget for number.
 * It only accept digital input, integer or double format.
 */
class NumLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /*!
     * \brief Type of number
     */
    enum NumType{
        INT_NUM,  /**< integer format, postive and negative, but not dot "." */
        DOUBLE_NUM  /**< double format, postive and negative*/
    };

public:
    /*!
     * \brief construct a new line edit for number.
     * \param type      number type, INT_NUM or DOUBLE_NUM
     * \param contents  default contents in line edit
     * \param parent    pointer to the parent widget
     */
    NumLineEdit( NumType type, const QString & contents, QWidget * parent = 0 );

protected:
    NumType m_type;  /**< type of number */
};

#endif // RIBBONCOMPONENT_H

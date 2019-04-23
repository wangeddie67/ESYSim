#include "result_tab_widget.h"

// ---- constructor and destructor ---- //
// constructor
ResultTabWidget::ResultTabWidget()
{
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequestedHandler(int)));
}

// destructor
ResultTabWidget::~ResultTabWidget()
{
}
// ----- constructor and destructor ---- //

// add new tab
int ResultTabWidget::addTab( ResultBaseDialog* page, const QString & label )
{
	// add tab
	int index = QTabWidget::addTab(page, label);
	setCurrentWidget(page);
    page->setFocus();
 	return index;
}

int ResultTabWidget::indexOfType ( ResultBaseDialog::ResultTypeEnum type )
{
    for (int i = 0; i < count(); i ++)
    {
        ResultBaseDialog * t_page = (ResultBaseDialog *)widget( i );
        if (t_page ->resultType() == type)
            return i;
    }
    return -1;
}

// close one tab
void ResultTabWidget::tabCloseRequestedHandler(int index)
{
    ResultBaseDialog * currentpage = (ResultBaseDialog *)widget(index);
	// remove tab
	removeTab(index);
	delete currentpage;
}

void ResultTabWidget::loadEventtraceFinishedHandler(bool success)
{
    if (success)
        emit currentChanged(currentIndex());
}

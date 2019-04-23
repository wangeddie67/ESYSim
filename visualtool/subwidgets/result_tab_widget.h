#ifndef RESULT_TAB_WIDGET_H
#define RESULT_TAB_WIDGET_H

#include "../define/qt_include.h"

#include "../subdialogs/result_base_dialog.h"

// log dialog class
class ResultTabWidget : public QTabWidget
{
	Q_OBJECT
	
	// constructor and destructor
public:
	ResultTabWidget(void);
	~ResultTabWidget(void);

	// addTab function
public:
    int addTab ( ResultBaseDialog * page, const QString & label );
    int indexOfType ( ResultBaseDialog::ResultTypeEnum type );

	// slots functions
public slots:
	// close one tab
	void tabCloseRequestedHandler(int index);
	// update once progress
private slots:
//	void updateProgressHandler(int index, int progress);
    void loadEventtraceFinishedHandler(bool success);
};
#endif

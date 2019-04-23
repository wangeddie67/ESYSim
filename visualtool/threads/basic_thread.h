#ifndef BASIC_THREAD_H
#define BASIC_THREAD_H

#include "../define/qt_include.h"

// Reader benchmark and analyse specified task
class BasicThread : public QThread
{
	Q_OBJECT
	
	// constructor and deconstructor
public:
    BasicThread( QObject * parent = 0 ) :
        QThread( parent ),
        m_good( true ), m_quit_flag( false ), m_last_progress( -1 )
    {}
	
private:
	// state variables
    bool m_good;	// thread is good
    bool m_quit_flag;	// thread should quit
    int m_last_progress;
	
public:
    bool isGood() { return m_good; }
    void setGood( bool good ) { m_good = good;}

    bool isQuit() { return m_quit_flag; }
    void setQuit( bool quit ) { m_quit_flag = quit;}

    void updateProgress( long long prog, long long total )
    {
        if ( ( ( prog * 100 ) / total ) > m_last_progress )
        {
            m_last_progress ++;
            emit progress( prog );
        }
    }
	
protected:
    void run() { QThread::run(); }

public slots:
    void quit() { m_quit_flag = true; }
    
signals:
    void progress( int value );
};

#endif

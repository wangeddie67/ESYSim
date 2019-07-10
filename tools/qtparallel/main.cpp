#include <QCoreApplication>

#include <iostream>
#include <QTime>

#include "parallelclass.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ParallelClass p( argc, argv );

    p .run();

    return a.exec();
}


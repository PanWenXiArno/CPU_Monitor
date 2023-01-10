#include "processMonitor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc,argv);

    processMonitor monitor;
    if(argc < 2){
        printf("Usage:test -<option> {}\n");
        return 1;
    }

    if(argc > 2)
    {
        if (QString(argv[1]) == "-p")
        {
            monitor.get_pid(argv[2]);
        }
        else if(QString(argv[1]) == "-n")
        {
            monitor.get_pname(argv[2]);
        }
    }
    monitor.startWork();
    return a.exec();
}

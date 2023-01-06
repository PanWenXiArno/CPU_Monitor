#include "processMonitor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc,argv);

    processMonitor monitor;
    if(argc < 2){
        printf("Usage:test <process_name> [user]\n");
        return 1;
    }

    if(argc > 2){
        monitor.get_pid(argv[1],argv[2]);
    }
    else{
        monitor.get_pid(argv[1]);
    }  
    monitor.startWork();
    return a.exec();
}

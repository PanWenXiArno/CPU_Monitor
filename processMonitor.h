#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

#endif // PROCESS_MONITOR_H

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <QString>
#include <stdlib.h>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <qdebug.h>

#define VMRSS_LINE 17
#define VMSIZE_LINE 18
#define PROCESS_ITEM 14

typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
}Total_Cpu_Occupy_t;

typedef struct {
    unsigned int pid;
    unsigned long utime;  //user time
    unsigned long stime;  //kernel time
    unsigned long cutime; //all user time
    unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;

typedef struct {
    unsigned int size;
    unsigned int resident;
    unsigned int shared;
    unsigned int Trs;
    unsigned int Lrs;
    unsigned int Drs;
    unsigned int dt;
}statm_Paras;

class processMonitor : public QObject
{
    Q_OBJECT
public:
    processMonitor();

    const char* get_items(const char*buffer ,unsigned int item);

    unsigned long get_cpu_total_occupy();

    unsigned long get_cpu_proc_occupy(unsigned int pid);

    float get_proc_cpu(unsigned int pid);

    unsigned int get_proc_mem(unsigned int pid);

    unsigned int get_total_mem();

    float get_mem_rate(unsigned int pid);

    unsigned int get_proc_virtualmem(unsigned int pid);

    int get_pname(const char* process_name, const char* user = nullptr);

    int get_pid(const char* pid);

    void outputCPUInfo();

    void startWork();

private:
    QTimer outputLoop;

    unsigned int pid;

    char file_name[20];
};

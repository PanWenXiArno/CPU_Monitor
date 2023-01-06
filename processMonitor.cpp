#include "processMonitor.h"
#include <iostream>
processMonitor::processMonitor()
{

}

//获取第N项开始的指针
const char* processMonitor::get_items(const char*buffer ,unsigned int item){

    const char *p =buffer;

    int len = strlen(buffer);
    int count = 0;

    for (int i=0; i<len;i++){
        if (' ' == *p){
            count ++;
            if(count == item -1){
                p++;
                break;
            }
        }
        p++;
    }

    return p;
}


//获取总的CPU时间
unsigned long processMonitor::get_cpu_total_occupy(){

    FILE *fd;
    char buff[1024]={0};
    Total_Cpu_Occupy_t t;

    fd =fopen("/proc/stat","r");
    if (nullptr == fd){
        return 0;
    }

    fgets(buff,sizeof(buff),fd);
    char name[64]={0};
    sscanf(buff,"%s %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle);
    fclose(fd);

    return (t.user + t.nice + t.system + t.idle);
}


//获取进程的CPU时间
unsigned long processMonitor::get_cpu_proc_occupy(unsigned int pid){

    char file_name[64]={0};
    Proc_Cpu_Occupy_t t;
    FILE *fd;
    char line_buff[1024]={0};
    sprintf(file_name,"/proc/%d/stat",pid);

    fd = fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    fgets(line_buff,sizeof(line_buff),fd);

    sscanf(line_buff,"%u",&t.pid);
    const char *q =get_items(line_buff,PROCESS_ITEM);
    sscanf(q,"%ld %ld %ld %ld",&t.utime,&t.stime,&t.cutime,&t.cstime);
    fclose(fd);

    return (t.utime + t.stime + t.cutime + t.cstime);
}


//获取CPU占用率
float processMonitor::get_proc_cpu(unsigned int pid){

    unsigned long totalcputime1,totalcputime2;
    unsigned long procputime1,procputime2;

    totalcputime1=get_cpu_total_occupy();
    procputime1=get_cpu_proc_occupy(pid);

    usleep(200000);

    totalcputime2=get_cpu_total_occupy();
    procputime2=get_cpu_proc_occupy(pid);

    float pcpu = 0.0;
    if(0 != totalcputime2-totalcputime1){
        pcpu=100.0 * (procputime2-procputime1)/(totalcputime2-totalcputime1);
    }

    return pcpu;
}

//获取进程占用内存
unsigned int processMonitor::get_proc_mem(unsigned int pid){

    char file_name[64]={0};
    FILE *fd;
    char line_buff[512]={0};
    statm_Paras m;
    sprintf(file_name,"/proc/%d/statm",pid);

    fd =fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    fgets(line_buff,sizeof(line_buff),fd);
    sscanf(line_buff,"%d %d %d %d %d %d %d",&m.size,&m.resident,&m.shared,&m.Trs,&m.Lrs,&m.Drs,&m.dt);
    fclose(fd);

//    printf("m.resident = %d\n",m.resident);
    return m.size;
//    return (m.resident+m.shared);
}

//获取total内存
unsigned int processMonitor::get_total_mem(){

    char file_name[64]={0};
    FILE *fd;
    char line_buff[512]={0};
    sprintf(file_name,"/proc/meminfo");

    fd =fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    char name[20];
    unsigned int MemTotal;

    fgets(line_buff,sizeof(line_buff),fd);
    sscanf(line_buff,"%s %d",name,&MemTotal);
    fclose(fd);

//    printf("MemTotal = %d\n",MemTotal);
    return MemTotal;
}

//获取进程内存占用率
float processMonitor::get_mem_rate(unsigned int pid){
//    printf("Process Mem = %d\n",get_proc_mem(pid));
    printf("Total Mem = %d\n",get_total_mem());
    float pmem = 100.0 * (get_proc_mem(pid))/(get_total_mem());

    return pmem;
}

//获取进程占用虚拟内存
unsigned int processMonitor::get_proc_virtualmem(unsigned int pid){

    char file_name[64]={0};
    FILE *fd;
    char line_buff[512]={0};
    sprintf(file_name,"/proc/%d/status",pid);

    fd =fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    char name[64];
    int vmsize;
    for (int i=0; i<VMSIZE_LINE-1;i++){
        fgets(line_buff,sizeof(line_buff),fd);
    }

    fgets(line_buff,sizeof(line_buff),fd);
    sscanf(line_buff,"%s %d",name,&vmsize);
    fclose(fd);

    return vmsize;
}


//进程本身
int processMonitor::get_pid(const char* process_name, const char* user)
{
    if(user == nullptr){
        user = getlogin();
    }

    char cmd[512];
    if (user){
        sprintf(cmd, "pgrep %s -xu %s", process_name, user);
    }

    FILE *pstr = popen(cmd,"r");

    if(pstr == nullptr){
        return 0;
    }

    char buff[512];
    ::memset(buff, 0, sizeof(buff));
    if(nullptr == fgets(buff, 512, pstr)){
        return 0;
    }

    pid = atoi(buff);

    return 0;
}

void processMonitor::outputCPUInfo()
{
    QFile output("CPU_results.csv");
    QDateTime DateTime = QDateTime::currentDateTime();
    QString time = DateTime.time().toString().toLocal8Bit().data();
    if(output.open(QFile::WriteOnly | QIODevice:: Append))
    {
        QTextStream out(&output);
        out<<pid<<","<<time<<","<<get_proc_cpu(pid)<<","<<get_mem_rate(pid)<<","<<get_proc_virtualmem(pid)<<endl;
    }
    qDebug()<<"Time="<<time;
    printf("pid=%d\n",pid);
    printf("pcpu=%f\n",get_proc_cpu(pid));
    printf("pmem=%f\n",get_mem_rate(pid));
    printf("virtualmem=%d\n\n",get_proc_virtualmem(pid));

}

void processMonitor::startWork()

{
    QObject::connect(&outputLoop, &QTimer::timeout, this, &processMonitor::outputCPUInfo);
    outputLoop.start(2000);
    QFile output("CPU_results.csv");
    if(output.open(QFile::WriteOnly))
    {
        QTextStream out(&output);
        out<<"pid,Time,CPU Utility,RAM Comsumption,Virtual Memory Consumption"<<endl;
    }

}

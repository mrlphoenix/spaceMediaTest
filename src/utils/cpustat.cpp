#include <QStringList>
#include <QProcess>
#include <QDebug>
#include "cpustat.h"
#include "platformdefines.h"

#ifdef PLATFORM_DEFINE_ANDROID
#include "sys/sysinfo.h"
#endif

CPUStat::CPUStat(QObject *parent) : QObject(parent)
{
    thread = new CPUStatThread(parent);
    connect(thread,SIGNAL(infoReady(CPUStatWorker::DeviceInfo)),this,SIGNAL(infoReady(CPUStatWorker::DeviceInfo)));
    thread->start();
}

CPUStat::~CPUStat()
{
    if (thread)
        thread->deleteLater();
}

void CPUStat::getInfo()
{
    thread->loadInfo();
}


CPUStatWorker::CPUStatWorker(QObject *parent) : QObject(parent)
{
    state = GET_UID;
}

void CPUStatWorker::getInfo()
{
#ifdef PLATFORM_DEFINE_LINUX
    process = new QProcess();
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readyRead()));
    process->start("bash data/cpu_usage.sh");
#endif
#ifdef PLATFORM_DEFINE_RPI
    process = new QProcess();
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readyRead()));
    process->start("bash data/cpu_usage.sh");
#endif
#ifdef PLATFORM_DEFINE_ANDROID
    qDebug() << "ANDROID_CPU_INFO CALLED";
    if (state == GET_UID)
    {
        process = new QProcess();
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int, QProcess::ExitStatus)
            {
               uids = QString(process->readAll()).split("\n");
               currentUID = 0;
               trafficIn = trafficOut = 0;
               state = UID_TRAFFIC_IN;
               process->deleteLater();
               getInfo();
            });
        process->start("ls /proc/uid_stat");
    }
    else if (state == UID_TRAFFIC_IN)
    {
        process = new QProcess();
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int, QProcess::ExitStatus)
            {
                trafficIn += QString(process->readAll()).toLongLong();
                state = UID_TRAFFIC_OUT;
                process->deleteLater();
                getInfo();
            });
        process->start("cat /proc/uid_stat/" + uids[currentUID] + "/tcp_rcv");
    }
    else if (state == UID_TRAFFIC_OUT)
    {
        process = new QProcess();
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int, QProcess::ExitStatus)
            {
                trafficOut += QString(process->readAll()).toLongLong();
                currentUID++;
                if (currentUID < uids.count())
                    state = UID_TRAFFIC_IN;
                else
                    state = CPU;
                process->deleteLater();
                getInfo();
            });
        process->start("cat /proc/uid_stat/" + uids[currentUID] + "/tcp_snd");
    }
    else if (state == CPU)
    {
        struct sysinfo info;
        qDebug() << "Traffic: " << trafficIn + trafficOut;
        sysinfo(&info);
        memory = 100 - double(info.freeram) / double(info.totalram) * 100.;

        process = new QProcess();
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int, QProcess::ExitStatus)
            {
                QStringList items = QString(process->readAll()).split(" ");
                if (items.count())
                    cpu = items[0].toDouble();
                else cpu = 0.0;
                state = GET_UID;
                DeviceInfo info;
                info.cpu = cpu;
                info.memory = memory;
                info.trafficIn = trafficIn;
                info.trafficOut = trafficOut;
                emit InfoReady(info);
                process->deleteLater();
            });
        process->start("cat /proc/loadavg");
    }
#endif
}

void CPUStatWorker::readyRead()
{
    result.append(process->readAllStandardOutput());

    QString s(result);
    if (s.indexOf("END") > 0)
    {
        QStringList items = QString(result).split(" ");
        DeviceInfo stat;
        stat.cpu = items[0].toInt();
        stat.memory = items[1].toInt();
        stat.trafficIn = items[2].toInt();
        stat.trafficOut = items[3].toInt();
        emit InfoReady(stat);
        process->kill();
        process->deleteLater();
        result.clear();
    }
}

CPUStatThread::CPUStatThread(QObject *parent) : QThread(parent)
{
    worker = new CPUStatWorker(parent);
}

CPUStatThread::~CPUStatThread()
{
    if (worker)
        worker->deleteLater();
}

void CPUStatThread::loadInfo()
{
    worker->getInfo();
}

void CPUStatThread::run()
{
    qDebug() << "Initialization CPU Stat Thread";
    connect(worker,SIGNAL(InfoReady(CPUStatWorker::DeviceInfo)),this,SLOT(getInfo(CPUStatWorker::DeviceInfo)));
    exec();
}

void CPUStatThread::getInfo(CPUStatWorker::DeviceInfo info)
{
    emit infoReady(info);
}

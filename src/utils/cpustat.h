#ifndef CPUSTAT_H
#define CPUSTAT_H

#include <QObject>
#include <QThread>
#include <QProcess>
#include "singleton.h"

#define CPUStatInstance Singleton<CPUStat>::instance()

class CPUStatWorker : public QObject
{
    Q_OBJECT
public:
    CPUStatWorker(QObject * parent = 0);
    struct DeviceInfo
    {
        int cpu;
        int memory;
        int trafficIn;
        int trafficOut;
    };
    void getInfo();
signals:
    void InfoReady(CPUStatWorker::DeviceInfo info);
private slots:
    void readyRead();
private:
    QProcess * process;
    QByteArray result;
};

class CPUStatThread : public QThread
{
    Q_OBJECT
public:
    CPUStatThread(QObject * parent = 0);
    void loadInfo();
signals:
    void infoReady(CPUStatWorker::DeviceInfo info);
protected:
    void run();
private slots:
    void getInfo(CPUStatWorker::DeviceInfo info);
private:
    CPUStatWorker * worker;
};

class CPUStat : public QObject
{
    Q_OBJECT
public:
    explicit CPUStat(QObject *parent = 0);
    void getInfo();
signals:
    void infoReady(CPUStatWorker::DeviceInfo info);

public slots:
private:
    CPUStatThread * thread;
};

#endif // CPUSTAT_H

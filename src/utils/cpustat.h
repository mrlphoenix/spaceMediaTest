#ifndef CPUSTAT_H
#define CPUSTAT_H

#include <QObject>
#include <QThread>
#include <QProcess>

#include "singleton.h"

#define CPUStatInstance Singleton<CPUStat>::instance()

class QStringList;

class CPUStatWorker : public QObject
{
    Q_OBJECT
public:
    CPUStatWorker(QObject * parent = 0);
    struct DeviceInfo
    {
        double cpu;
        double memory;
        qlonglong trafficIn;
        qlonglong trafficOut;
    };
    enum AndroidInfoState{GET_UID, UID_TRAFFIC_IN, UID_TRAFFIC_OUT, CPU};

    void getInfo();
signals:
    void InfoReady(CPUStatWorker::DeviceInfo info);
private slots:
    void readyRead();
private:
    QProcess * process;
    QByteArray result;
    qlonglong trafficIn, trafficOut;
    double cpu, memory;
    QStringList uids;
    int currentUID;
    AndroidInfoState state;
};

class CPUStatThread : public QThread
{
    Q_OBJECT
public:
    CPUStatThread(QObject * parent = 0);
    ~CPUStatThread();
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
    ~CPUStat();
    void getInfo();
signals:
    void infoReady(CPUStatWorker::DeviceInfo info);

public slots:
private:
    CPUStatThread * thread;
};

#endif // CPUSTAT_H

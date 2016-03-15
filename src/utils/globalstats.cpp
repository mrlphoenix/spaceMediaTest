#include <QDebug>
#include "globalstats.h"

GlobalStats::GlobalStats(QObject *parent) : QObject(parent)
{
    downloadCount = 0;
    contentPlayCount = 0;
    contentTotalCount = 0;
    connectionErrorCount = 0;
    playlistErrorCount = 0;
    cpu = 0;
    memory = 0;
    trafficIn = trafficTotalIn = 0.0;
    trafficOut = trafficTotalOut = 0.0;

    monitorActive = true; //until checking implemented
    connectionActive = true;
    balance = 0.0f;

    connectionDropped = false;
    latitude = longitude = 0.0;
}

void GlobalStats::registryDownload()
{
    downloadCount++;
}

void GlobalStats::registryConnectionError()
{
    connectionErrorCount++;
}

void GlobalStats::registryPlaylistError()
{
    playlistErrorCount++;
}

void GlobalStats::setContentPlay(int count)
{
    contentPlayCount = count;
}

void GlobalStats::setContentTotal(int count)
{
    contentTotalCount = count;
}

void GlobalStats::setCpu(double cpu)
{
    qDebug() << "CPU INFO: " << cpu;
    this->cpu = cpu;
}

void GlobalStats::setMemory(double memory)
{
    qDebug() << "MEMORY INFO: " << memory;
    this->memory = memory;
}

void GlobalStats::setTraffic(qlonglong in, qlonglong out)
{
    if (connectionDropped)
    {
        qDebug() <<"dropped";

        trafficIn = 0;
        trafficOut = 0;
        trafficTotalIn = in;
        trafficTotalOut = out;
        connectionDropped = false;
    }
    else
    {
        qDebug() <<"not dropped";

        if (trafficTotalIn != 0.0 && trafficTotalOut != 0.0)
        {
            trafficIn = in - trafficTotalIn;
            trafficOut = out - trafficTotalOut;
        }
        trafficTotalIn = in;
        trafficTotalOut = out;
    }
}

void GlobalStats::setMonitorActive(bool isActive)
{
    monitorActive = isActive;
}

void GlobalStats::setConnectionState(bool isActive)
{
    if (connectionActive && !isActive)
        connectionDropped = true;
    connectionActive = isActive;
}

void GlobalStats::setBalance(double balance)
{
    this->balance = balance;
}

GlobalStats::Report GlobalStats::generateReport()
{
    qDebug() << "report generator called";
    Report result;
    result.connectionErrorCount = connectionErrorCount;
    result.contentPlayCount = contentPlayCount;
    result.contentTotalCount = contentTotalCount;
    result.downloadCount = downloadCount;
    result.playlistErrorCount = playlistErrorCount;

    downloadCount = 0;
    connectionErrorCount = 0;
    playlistErrorCount = 0;
    return result;
}

GlobalStats::SystemInfo GlobalStats::generateSystemInfo()
{
    SystemInfo result;
    result.balance = balance;
    result.trafficIn = trafficIn;
    result.trafficOut = trafficOut;
    result.cpu = cpu;
    result.memory = memory;
    result.monitorActive = monitorActive;
    result.connectionActive = connectionActive;
    return result;
}

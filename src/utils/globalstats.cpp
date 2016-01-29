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
    trafficIn = 0.0;
    trafficOut = 0.0;

    monitorActive = true; //until checking implemented
    connectionActive = true;
    balance = 0.0f;

    connectionDropped = false;
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

void GlobalStats::setCpu(int cpu)
{
    qDebug() << "CPU INFO: " << cpu;
    this->cpu = cpu;
}

void GlobalStats::setMemory(int memory)
{
    qDebug() << "MEMORY INFO: " << memory;
    this->memory = memory;
}

void GlobalStats::setTraffic(double in, double out)
{
    if (connectionDropped)
    {
        trafficIn = in;
        trafficOut = out;
        connectionDropped = false;
    }
    else
    {
        trafficIn = in - trafficIn;
        trafficOut = out - trafficOut;
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


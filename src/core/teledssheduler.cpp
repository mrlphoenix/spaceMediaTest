#include "teledssheduler.h"
#include "singleton.h"
#include "globalconfig.h"

TeleDSSheduler::TeleDSSheduler(QObject *parent) : QObject(parent)
{
    getPlaylistTimer = new QTimer();
    cpuInfoTimer = new QTimer();
    reportTimer = new QTimer();
    sysInfoTimer = new QTimer();
    resourceCounterTimer = new QTimer();
    gpsTimer = new QTimer();

    connect(getPlaylistTimer, SIGNAL(timeout()),this,SIGNAL(getPlaylist()));
    connect(cpuInfoTimer, SIGNAL(timeout()),this, SIGNAL(cpuInfo()));
    connect(reportTimer,SIGNAL(timeout()), this, SIGNAL(report()));
    connect(sysInfoTimer,SIGNAL(timeout()), this, SIGNAL(sysInfo()));
    connect(resourceCounterTimer, SIGNAL(timeout()), this, SIGNAL(resourceCounter()));
    connect (gpsTimer, SIGNAL(timeout()), this, SIGNAL(gps()));

    getPlaylistTimer->start(GlobalConfigInstance.getGetPlaylistTimerTime());
    cpuInfoTimer->start(GlobalConfigInstance.getCpuInfoTimerTime());
    reportTimer->start(GlobalConfigInstance.getReportTimerTime());
    sysInfoTimer->start(GlobalConfigInstance.getSysInfoTimerTime());
    resourceCounterTimer->start(GlobalConfigInstance.getResourceCounterTimerTime());
    gpsTimer->start(GlobalConfigInstance.getGpsTimerTime());
}

TeleDSSheduler::~TeleDSSheduler()
{
    getPlaylistTimer->deleteLater();
    cpuInfoTimer->deleteLater();
    reportTimer->deleteLater();
    sysInfoTimer->deleteLater();
    resourceCounterTimer->deleteLater();
}

void TeleDSSheduler::start(TeleDSSheduler::Task t)
{
    switch(t)
    {
    case GET_PLAYLIST:
        getPlaylistTimer->start(GlobalConfigInstance.getGetPlaylistTimerTime());
        break;
    case CPU_INFO:
        cpuInfoTimer->start(GlobalConfigInstance.getCpuInfoTimerTime());
        break;
    case REPORT:
        reportTimer->start(GlobalConfigInstance.getReportTimerTime());
        break;
    case SYS_INFO:
        sysInfoTimer->start(GlobalConfigInstance.getSysInfoTimerTime());
        break;
    case RESOURCE_COUNT:
        resourceCounterTimer->start(GlobalConfigInstance.getResourceCounterTimerTime());
        break;
    case GPS:
        gpsTimer->start(GlobalConfigInstance.getGpsTimerTime());
        break;
    default:
        getPlaylistTimer->start(GlobalConfigInstance.getGetPlaylistTimerTime());
        cpuInfoTimer->start(GlobalConfigInstance.getCpuInfoTimerTime());
        reportTimer->start(GlobalConfigInstance.getReportTimerTime());
        sysInfoTimer->start(GlobalConfigInstance.getSysInfoTimerTime());
        resourceCounterTimer->start(GlobalConfigInstance.getResourceCounterTimerTime());
        gpsTimer->start(GlobalConfigInstance.getGpsTimerTime());
        break;
    }
}

void TeleDSSheduler::stop(TeleDSSheduler::Task t)
{
    switch(t)
    {
    case GET_PLAYLIST:
        getPlaylistTimer->stop();
        break;
    case CPU_INFO:
        cpuInfoTimer->stop();
        break;
    case REPORT:
        reportTimer->stop();
        break;
    case SYS_INFO:
        sysInfoTimer->stop();
        break;
    case RESOURCE_COUNT:
        resourceCounterTimer->stop();
        break;
    case GPS:
        gpsTimer->stop();
        break;
    default:
        getPlaylistTimer->stop();
        cpuInfoTimer->stop();
        reportTimer->stop();
        sysInfoTimer->stop();
        resourceCounterTimer->stop();
        gpsTimer->stop();
        break;
    }
}

void TeleDSSheduler::restart(TeleDSSheduler::Task t)
{
    stop(t);
    start(t);
}


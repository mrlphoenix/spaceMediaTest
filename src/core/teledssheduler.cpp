#include "teledssheduler.h"
#include "singleton.h"
#include "globalconfig.h"
#include <QDebug>

TeleDSSheduler::TeleDSSheduler(QObject *parent) : QObject(parent)
{
    getPlaylistTimer = new QTimer();
    resourceCounterTimer = new QTimer();

    connect(getPlaylistTimer, SIGNAL(timeout()),this,SIGNAL(getPlaylist()));
    connect(resourceCounterTimer, SIGNAL(timeout()), this, SIGNAL(resourceCounter()));

    getPlaylistTimer->start(GlobalConfigInstance.getGetPlaylistTimerTime());
}

TeleDSSheduler::~TeleDSSheduler()
{
    getPlaylistTimer->deleteLater();
    resourceCounterTimer->deleteLater();
}

void TeleDSSheduler::start(TeleDSSheduler::Task t)
{
    qDebug() << "TeleDSSheduler::start " << t;
    switch(t)
    {
    case GET_PLAYLIST:
        getPlaylistTimer->start(GlobalConfigInstance.getGetPlaylistTimerTime());
        break;
    default:
        getPlaylistTimer->start(GlobalConfigInstance.getGetPlaylistTimerTime());

        break;
    }
}

void TeleDSSheduler::stop(TeleDSSheduler::Task t)
{
    qDebug() << "TeleDSSheduler::stop " << t;
    switch(t)
    {
    case GET_PLAYLIST:
        getPlaylistTimer->stop();
        break;
    default:
        getPlaylistTimer->stop();
        break;
    }
}

void TeleDSSheduler::restart(TeleDSSheduler::Task t)
{
    stop(t);
    start(t);
}


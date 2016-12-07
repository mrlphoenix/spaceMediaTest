#include <QDebug>
#include <QFile>
#include <QTimeZone>
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
    trafficIn = trafficTotalIn = 0;
    trafficOut = trafficTotalOut = 0;

    monitorActive = true; //until checking implemented
    connectionActive = true;
    balance = 0.0f;

    connectionDropped = false;
    latitude = longitude = 0.0;

    QFile f("/usr/share/zoneinfo/zone.tab");
    f.open(QFile::ReadOnly);
    QStringList lines = QString(f.readAll()).split("\n");
    foreach (const QString &s, lines)
    {
        if (s.simplified() == "" || s[0] == QChar('#'))
            continue;
        QStringList tokens = s.split("\t");
        if (tokens.count() > 2)
        {
            TimeZoneEntry newItem;
            newItem.shortName = tokens[0];
            QString coordsString = tokens[1];
            coordsString = coordsString.replace("+", " +").replace("-", " -");
            coordsString = coordsString.mid(1, coordsString.length()-1);
            QString latStr = coordsString.split(" ")[0];
            QString lonStr = coordsString.split(" ")[1];
            newItem.lat = latStr.toDouble()/(latStr.length() > 6 ? 10000. : 100.);
            newItem.lon = lonStr.toDouble()/(lonStr.length() > 6 ? 10000. : 100.);
            newItem.longName = tokens[2];
            tzDatabase.append(newItem);
        }
    }
    f.close();
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
        if (trafficTotalIn != 0 || trafficTotalOut != 0)
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

int GlobalStats::getUTCOffset()
{
    int foundIndex = 0;
    double distance = 10000000.;
    double lat = latitude;
    double lon = longitude;
    if (lat == 0.0 && lon == 0.0)
        return 0;

    int currentIndex = 0;
    foreach (const TimeZoneEntry &tz, tzDatabase)
    {
        double theta = lon - tz.lon;
        double cdistance = sin(lat/180.*M_PI) * sin(tz.lat/180.*M_PI) + cos(lat/180.*M_PI)*cos(tz.lat/180.*M_PI) * cos(theta/180.*M_PI);
        cdistance = acos(cdistance);
        cdistance = fabs(cdistance/M_PI*180.0);
        if (distance > cdistance)
        {
            foundIndex = currentIndex;
            distance = cdistance;
        }
        currentIndex++;
    }
    QTimeZone tz(tzDatabase[foundIndex].longName.toLocal8Bit());
    if (tz.isValid())
        return tz.offsetFromUtc(QDateTime::currentDateTime());
    else
        return 0;
}

void GlobalStats::setSunset(QTime sunset)
{
    this->sunset = sunset;
    measureSunset = QTime::currentTime();
}

void GlobalStats::setSunrise(QTime sunrise)
{
    this->sunrise = sunrise;
    measureSunrise = QTime::currentTime();
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

bool GlobalStats::shouldUpdateSunset()
{
    bool updateValue = !measureSunset.isValid();
    if (!updateValue)
    {
        if (measureSunset < QTime::currentTime())
        {
            if (measureSunset.secsTo(QTime::currentTime()) > 3600)
                return true;
            else
                return false;
        }
        else
            return true;
    }
    return updateValue;
}

bool GlobalStats::shouldUpdateSunrise()
{
    bool updateValue = !measureSunrise.isValid();
    if (!updateValue)
    {
        if (measureSunrise < QTime::currentTime())
        {
            if (measureSunrise.secsTo(QTime::currentTime()) > 3600)
                return true;
            else
                return false;
        }
        else
            return true;
    }
    return updateValue;
}

void GlobalStats::itemPlayed(QString areaId, QString contentId, QDateTime date)
{
    qDebug() << "GlobalStats::itemPlayed" << contentId << " " << date;
    if (!lastTimePlayed.contains(areaId))
        lastTimePlayed[areaId] = QHash<QString, QDateTime>();
    lastTimePlayed[areaId][contentId] = date;
}

bool GlobalStats::checkDelayPass(QString areaId, QString contentId)
{
    qDebug() << "GlobalStats::checkDelayPass";
    if (!lastTimePlayed.contains(areaId))
    {
        qDebug() << "no such area -> return true";
        return true;
    }
    if (!lastTimePlayed[areaId].contains(contentId))
    {
        qDebug() << "item was never played -> return true";
        return true;
    }
    QDateTime currentTime = QDateTime::currentDateTimeUtc().addSecs(getUTCOffset());
    qDebug() << currentTime.time() << "prevPL" << lastTimePlayed[areaId][contentId].time();
    return currentTime > lastTimePlayed[areaId][contentId];
}

bool GlobalStats::itemWasPlayed(QString areaId, QString contentId)
{
    if (!lastTimePlayed.contains(areaId))
        return false;
    return lastTimePlayed[areaId].contains(contentId);
}

void GlobalStats::setSystemData(QString tag, QByteArray data)
{
    systemData[tag] = data;
}

QByteArray GlobalStats::getSystemData(QString tag)
{
    return systemData[tag];
}

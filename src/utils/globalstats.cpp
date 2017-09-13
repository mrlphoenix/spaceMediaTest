#include <QDebug>
#include <QFile>
#include <QTimeZone>
#include <QProcess>
#include "platformdefines.h"
#ifdef PLATFORM_DEFINE_WINDOWS
#define _USE_MATH_DEFINES
#include "math.h"
#endif
#include "globalstats.h"
#include "platformspecific.h"

GlobalStats::GlobalStats(QObject *parent) : QObject(parent)
{
    downloadCount = 0;
    contentPlayCount = 0;
    contentTotalCount = 0;
    connectionErrorCount = 0;
    playlistErrorCount = 0;
    cpu = 0;
    memory = 0;
    trafficIn = prevTrafficIn = 0;
    trafficOut = prevTrafficOut = 0;

    monitorActive = true; //until checking implemented
    connectionActive = true;
    balance = 0.0f;

    connectionDropped = false;
    latitude = longitude = 0.0;
    hdmiGPIO = true;

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
        prevTrafficIn = in;
        prevTrafficOut = out;
        connectionDropped = false;
    }
    else
    {
        prevTrafficIn = trafficIn;
        prevTrafficOut = trafficOut;
        trafficIn = in;
        trafficOut = out;
    }
}

double GlobalStats::getTrafficIn()
{
    if (trafficIn > prevTrafficIn && prevTrafficIn != 0)
        return trafficIn - prevTrafficIn;
    return 0.0;
}

double GlobalStats::getTrafficOut()
{
    if (trafficOut > prevTrafficOut && prevTrafficOut != 0)
        return trafficOut - prevTrafficOut;
    return 0.0;
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
    if (PlatformSpecificService.isAndroid())
        return 0;
    if (PlatformSpecificService.isWindows())
        return 0;
    bool shouldUpdateUTC = false;
    QDateTime currentTimeUTC = QDateTime::currentDateTimeUtc();
    if (!lastTzCheck.isValid())
    {
        lastTzCheck = currentTimeUTC;
        shouldUpdateUTC = true;
    }
    if (lastTzCheck.secsTo(currentTimeUTC) > 300)
    {
        shouldUpdateUTC = true;
        lastTzCheck = currentTimeUTC;
    }
    if (shouldUpdateUTC)
    {
        //qDebug() << "GlobalStats::getUTCOffset";
        int foundIndex = 0;
        double distance = 10000000.;
        double lat = latitude;
        double lon = longitude;
        if (lat == 0.0 && lon == 0.0)
        {
            cachedTzValue = 0;
            return 0;
        }
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
        {
            auto newTz = tz.offsetFromUtc(QDateTime::currentDateTime());
            if (newTz != 0)
                cachedTzValue = tz.offsetFromUtc(QDateTime::currentDateTime());
            return cachedTzValue;
        }
        else
        {
            return cachedTzValue;
         //   cachedTzValue = 0;
          //  return 0;
        }
    }
    else return cachedTzValue;
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

bool GlobalStats::checkDelayPass(const QString &areaId, const QString &contentId, const QDateTime &realCurrentTime)
{
    //qDebug() << "GlobalStats::checkDelayPass " << QDateTime::currentDateTime();
   /* if (!lastTimePlayed.contains(areaId))
    {
        qDebug() << "no such area -> return true";
        return true;
    }
    if (!lastTimePlayed[areaId].contains(contentId))
    {
        qDebug() << "item was never played -> return true";
        return true;
    }*/
    //int getutcoffset = getUTCOffset();
    //qDebug() << "GET UTC OFFSET" << getutcoffset;
    //QDateTime currentTime = QDateTime::currentDateTimeUtc().addSecs(getutcoffset);
    //qDebug() << currentTime.time() << "prevPL" << lastTimePlayed[areaId][contentId].addSecs(getItemPlayTimeout(contentId)).time();
    //qDebug() << "GlobalStats::checkDelayPass before return" << QDateTime::currentDateTime();
    return realCurrentTime > lastTimePlayed[areaId][contentId].addSecs(getItemPlayTimeout(contentId));
}

QDateTime GlobalStats::getItemLastPlayDate(QString areaId, QString contentId)
{
    //qDebug() << "GlobalStats::itemLastPlayDate";
    if (!lastTimePlayed.contains(areaId))
        return QDateTime::currentDateTimeUtc().addSecs(getUTCOffset());
    if (!lastTimePlayed[areaId].contains(contentId))
        return QDateTime::currentDateTimeUtc().addSecs(getUTCOffset());
    return lastTimePlayed[areaId][contentId].addSecs(getItemPlayTimeout(contentId));
}

bool GlobalStats::itemWasPlayed(QString areaId, QString contentId)
{
    if (!lastTimePlayed.contains(areaId))
        return false;
    return lastTimePlayed[areaId].contains(contentId);
}

void GlobalStats::itemWasSkipped(int duration)
{
    foreach (const QString &areaId, lastTimePlayed.keys())
        foreach (const QString &contentId, lastTimePlayed[areaId].keys())
        {
            auto time = lastTimePlayed[areaId][contentId];
            time = time.addSecs(-duration);
            lastTimePlayed[areaId][contentId] = time;
        }
}

void GlobalStats::setItemActivated(const QString &item, bool isActive)
{
    itemActivated[item] = isActive;
}

bool GlobalStats::isItemActivated(const QString &item)
{
    if (itemActivated.contains(item))
        return itemActivated[item];
    return false;
}

void GlobalStats::addPriorityItem(const QString &contentId)
{
    if (!priorityItems.contains(contentId))
        priorityItems.append(contentId);
}

bool GlobalStats::isItemHighPriority(const QString &contentId)
{
    return priorityItems.contains(contentId);
}

void GlobalStats::setItemPlayTimeout(QString contentId, int timeout)
{
    itemTimeout[contentId] = timeout;
}

int GlobalStats::getItemPlayTimeout(const QString &contentId)
{
    if (!itemTimeout.contains(contentId))
    {
        qDebug() << "Error: Trying to find item timeout where contentId wasnt found " << contentId;
        return -1;
    }
    return itemTimeout[contentId];
}

bool GlobalStats::wasAnyItemPlayed(QString areaId)
{
    if (!lastTimePlayed.contains(areaId))
        return false;
    return (lastTimePlayed[areaId].count() > 0);
}

void GlobalStats::setSystemData(QString tag, QByteArray data)
{
    systemData[tag] = data;
}

QByteArray GlobalStats::getSystemData(QString tag)
{
    return systemData[tag];
}

void GlobalStats::setCRC32Hex(QString v)
{
    if (v != crc32Hex)
    {
        crc32Hex = v;
        PlatformSpecificService.writeToFile(v.toLocal8Bit(),"/home/pi/teleds/crc32.txt");
        QProcess p;
        p.start("/usr/bin/php",QStringList() << "/home/pi/teleds_git/git/splash/initialize.php");
        p.waitForStarted();
        p.waitForFinished();
    }
}

bool GlobalStats::cacheItemData(QString id)
{
    if (cachedSentData.contains(id))
        return false;
    else
    {
        cachedSentData.append(id);
        return true;
    }
}

void GlobalStats::clearCachedItemData()
{
    cachedSentData.clear();
}

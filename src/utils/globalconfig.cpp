#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include "globalconfig.h"
#include "globalstats.h"
#include "platformdefines.h"
#include "platformspecific.h"

GlobalConfig::GlobalConfig(QObject *parent) : QObject(parent)
{
    qDebug() << "global config init";

    statsInterval = 60000;
    autobright = false;
    min_bright = max_bright = 0;
    firstReleyEnabled = secondReleyEnabled = false;

    if (!QFile::exists(CONFIG_FOLDER + "config.dat"))
    {
        qDebug() << "config file does not exists; creating new one";
        QFile out(CONFIG_FOLDER + "config.dat");
        if (out.open(QFile::WriteOnly))
        {
            out.write(QString("{}").toLocal8Bit());
            out.flush();
            out.close();
            loadFromJson();
        }
        else
        {
            save();
            configured = false;
        }
    }
    else
        loadFromJson();
}


void GlobalConfig::setToken(QString token)
{
    this->token = token;
    save();
}

void GlobalConfig::setActivationCode(QString code)
{
    this->activationCode = code;
}

void GlobalConfig::setVideoQuality(QString quality)
{
    this->quality = quality;
}

void GlobalConfig::setGetPlaylistTimerTime(int msecs)
{
    getPlaylistTimerTime = msecs;
    save();
}

void GlobalConfig::setStatsInverval(int secs)
{
    statsInterval = secs * 1000;
}

void GlobalConfig::setAutoBrightness(bool active)
{
    autobright = active;
}

void GlobalConfig::setMinBrightness(int minBrightness)
{
    min_bright = minBrightness;
}

void GlobalConfig::setMaxBrightness(int maxBrightness)
{
    max_bright = maxBrightness;
}

void GlobalConfig::setReleyConfig(QHash<int, QList<int> > &reley_1, QHash<int, QList<int> > &reley_2)
{
    time_targeting_relay_1 = reley_1;
    time_targeting_relay_2 = reley_2;
}

bool GlobalConfig::getFirstReleyStatus()
{
    QDateTime currentTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset());
    //qDebug() << currentTime;
    int dayOfWeek = currentTime.date().dayOfWeek() - 1;
    int hour = currentTime.time().hour();
    //qDebug() << "dayOfWeek: " << dayOfWeek << " hour: " << hour << "rl: " << time_targeting_relay_1;
    return time_targeting_relay_1[dayOfWeek].contains(hour);
}

bool GlobalConfig::getSecondReleyStatus()
{
    QDateTime currentTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset());
    int dayOfWeek = currentTime.date().dayOfWeek()-1;
    int hour = currentTime.time().hour();
    return time_targeting_relay_2[dayOfWeek].contains(hour);
}

void GlobalConfig::setReleyEnabled(bool first, bool second)
{
    firstReleyEnabled = first;
    secondReleyEnabled = second;
}

void GlobalConfig::setSettings(QJsonObject json)
{
    settings = json;
    settingsObject = SettingsRequestResult::fromJson(json, false);
    save();
}

QJsonObject GlobalConfig::getSettings()
{
    return settings;
}

SettingsRequestResult &GlobalConfig::getSettingsObject()
{
    return settingsObject;
}

void GlobalConfig::setPlaylist(QJsonObject json)
{
    playlist = json;
    save();
}

QJsonObject GlobalConfig::getPlaylist()
{
    return playlist;
}

void GlobalConfig::setAreas(QJsonArray json)
{
    areas = json;
    save();
}

QJsonArray GlobalConfig::getAreas()
{
    return areas;
}

void GlobalConfig::setPlaylistNetworkError(int error_id)
{
    playlistNetworkErrorId = error_id;
    save();
}

void GlobalConfig::setPlayerConfig(QJsonObject result)
{
    playerConfigAPI = result;
    save();
}

QJsonObject GlobalConfig::getPlayerConfig()
{
    return playerConfigAPI;
}

void GlobalConfig::addContentPlaying(QString contentId)
{
    if (!contentInPlay.contains(contentId))
        contentInPlay.append(contentId);
}

void GlobalConfig::removeContentPlaying(QString contentId)
{
    if (contentInPlay.contains(contentId))
        contentInPlay.removeOne(contentId);
}

bool GlobalConfig::isContentInPlay(QString contentId)
{
    return contentInPlay.contains(contentId);
}

void GlobalConfig::setAreaToVirtualScreen(QString areaId, QString virtualScreenId)
{
    areaToVirtualScreen[areaId] = virtualScreenId;
}

QString GlobalConfig::getVirtualScreenId(QString areaId)
{
    if (areaToVirtualScreen.contains(areaId))
        return areaToVirtualScreen[areaId];
    else
        return "";
}

void GlobalConfig::setVolume(int value)
{
    this->volume = value;
    save();
}

void GlobalConfig::setMetaProperty(QString key, QString value)
{
    metaProperties[key] = value;
}

QString GlobalConfig::getMetaProperty(QString key)
{
    if (metaProperties.contains(key))
        return metaProperties[key];
    return QString();
}

void GlobalConfig::loadFromJson()
{
    qDebug() << "loading from config.dat";
    QFile configFile(CONFIG_FOLDER + "config.dat");
    configFile.open(QFile::ReadOnly);
    QJsonDocument doc;
    QByteArray data = configFile.readAll();
#ifdef PLATFORM_ENCODE_CONFIG
    data =  qUncompress(Platform::lfsrEncode(data, CONFIG_FOLDER));
#endif
    doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();
    this->device = root["device"].toString();
    this->token = root["token"].toString();
    this->settings = root["settings"].toObject();
    this->playerConfigAPI = root["playerConfigAPI"].toObject();
    this->playlist = root["playlist"].toObject();
    this->areas = root["areas"].toArray();
    this->playlistNetworkErrorId = root["playlistNetworkErrorId"].toInt();
    this->volume = root["volume"].toInt();

    qDebug() << "currentConfig: " << token;
    configFile.close();
    checkConfiguration();
}

void GlobalConfig::save()
{
    QJsonObject root;
    root["device"] = device;
    root["token"] = token;
    root["settings"] = settings;
    root["playerConfigAPI"] = playerConfigAPI;
    root["playlist"] = playlist;
    root["areas"] = areas;
    root["playlistNetworkErrorId"] = playlistNetworkErrorId;
    root["volume"] = volume;

    QJsonDocument doc(root);
    QFile file (CONFIG_FOLDER + "config.dat");
    file.open(QFile::WriteOnly);

    QByteArray data = doc.toJson();
#ifdef PLATFORM_ENCODE_CONFIG
    data = Platform::lfsrEncode(qCompress(data), CONFIG_FOLDER);
#endif
    file.write(data);
    file.flush();
    file.close();
}

void GlobalConfig::checkConfiguration()
{
    configured = (token != "");
}


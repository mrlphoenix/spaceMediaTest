#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include "globalconfig.h"
#include "platformdefines.h"

GlobalConfig::GlobalConfig(QObject *parent) : QObject(parent)
{
    qDebug() << "global config init";
    if (!QFile::exists(CONFIG_FOLDER + "config.dat"))
    {
        qDebug() << "config file does not exists; creating new one";

        playerId = "rpi";
        getPlaylistTimerTime = getPlaylistTimerDefaultTime;
        cpuInfoTimerTime = cpuInfoTimerDefaultTime;
        reportTimerTime = reportTimerDefaultTime;
        sysInfoTimerTime = sysInfoTimerDefaultTime;
        resourceCounterTimerTime = resourceCounterTimerDefaultTime;
        gpsTimerTime = gpsTimerDefaulTime;

        save();
        configured = false;
    }
    else
        loadFromJson();
}

void GlobalConfig::setPlayerId(QString playerId)
{
    this->playerId = playerId;
    save();
    checkConfiguration();
}

void GlobalConfig::setPublicKey(QString publicKey)
{
    this->publicKey = publicKey;
    save();
    checkConfiguration();
}

void GlobalConfig::setPlayerConfig(QString playerConfig)
{
    this->playerConfig = playerConfig;
    save();
}

void GlobalConfig::setSessionKey(QString key)
{
    sessionKey = key;
}

void GlobalConfig::setEncryptedSessionKey(QString key)
{
    this->encryptedSessionKey = key;
}

void GlobalConfig::setGetPlaylistTimerTime(int msecs)
{
    getPlaylistTimerTime = msecs;
    save();
}

void GlobalConfig::setCpuInfoTimerTime(int msecs)
{
    cpuInfoTimerTime = msecs;
    save();
}

void GlobalConfig::setReportTimerTime(int msecs)
{
    reportTimerTime = msecs;
    save();
}

void GlobalConfig::setSysInfoTimerTime(int msecs)
{
    sysInfoTimerTime = msecs;
    save();
}

void GlobalConfig::setResourceCounterTimerTime(int msecs)
{
    resourceCounterTimerTime = msecs;
    save();
}

void GlobalConfig::setGpsTimerTime(int msecs)
{
    gpsTimerTime = msecs;
    save();
}

void GlobalConfig::loadFromJson()
{
    qDebug() << "loading from config.dat";
    QFile configFile(CONFIG_FOLDER + "config.dat");
    configFile.open(QFile::ReadOnly);
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(configFile.readAll());
    QJsonObject root = doc.object();
    this->device = root["device"].toString();
    this->playerId = root["playerId"].toString();
    this->publicKey = root["publicKey"].toString();
    this->playerConfig = root["playerConfig"].toString();

    getPlaylistTimerTime = root["getPlaylistTimerTime"].toInt() ? root["getPlaylistTimerTime"].toInt() : getPlaylistTimerDefaultTime;
    cpuInfoTimerTime = root["cpuInfoTimerTime"].toInt() ? root["cpuInfoTimerTime"].toInt() : cpuInfoTimerDefaultTime;
    reportTimerTime = root["reportTimerTime"].toInt() ? root["reportTimerTime"].toInt() : reportTimerDefaultTime;
    sysInfoTimerTime = root["sysInfoTimerTime"].toInt() ? root["sysInfoTimerTime"].toInt() : sysInfoTimerDefaultTime;
    resourceCounterTimerTime = root["resourceCounterTimerTime"].toInt() ? root["resourceCounterTimerTime"].toInt() : resourceCounterTimerDefaultTime;
    gpsTimerTime = root["gpsTimerTime"].toInt() ? root["gpsTimerTime"].toInt()  : gpsTimerDefaulTime;


    qDebug() << "currentConfig: " << device << " " << playerId << " " << publicKey;
    configFile.close();
    checkConfiguration();
}

void GlobalConfig::save()
{
    QJsonObject root;
    root["device"] = device;
    root["playerId"] = playerId;
    root["publicKey"] = publicKey;
    root["playerConfig"] = playerConfig;

    root["getPlaylistTimerTime"] = getPlaylistTimerTime;
    root["cpuInfoTimerTime"] = cpuInfoTimerTime;
    root["reportTimerTime"] = reportTimerTime;
    root["sysInfoTimerTime"] = sysInfoTimerTime;
    root["resourceCounterTimerTime"] = resourceCounterTimerTime;
    root["gpsTimerTime"] = gpsTimerTime;

    QJsonDocument doc(root);
    QFile file (CONFIG_FOLDER + "config.dat");
    file.open(QFile::WriteOnly);
    file.write(doc.toJson());
    file.flush();
    file.close();
}


void GlobalConfig::checkConfiguration()
{
    if (playerId == "" || publicKey == "")
        configured = false;
    else
        configured = true;
}


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

        save();
        configured = false;
    }
    else
        loadFromJson();
}

void GlobalConfig::setPlayerConfig(QString playerConfig)
{
    this->playerConfig = playerConfig;
    save();
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

void GlobalConfig::loadFromJson()
{
    qDebug() << "loading from config.dat";
    QFile configFile(CONFIG_FOLDER + "config.dat");
    configFile.open(QFile::ReadOnly);
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(configFile.readAll());
    QJsonObject root = doc.object();
    this->device = root["device"].toString();
    this->token = root["token"].toString();



    qDebug() << "currentConfig: " << token;
    configFile.close();
    checkConfiguration();
}

void GlobalConfig::save()
{
    QJsonObject root;
    root["device"] = device;
    root["token"] = token;

    QJsonDocument doc(root);
    QFile file (CONFIG_FOLDER + "config.dat");
    file.open(QFile::WriteOnly);
    file.write(doc.toJson());
    file.flush();
    file.close();
}


void GlobalConfig::checkConfiguration()
{
    configured = (token != "");
}


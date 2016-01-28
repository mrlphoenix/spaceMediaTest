#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include "globalconfig.h"

GlobalConfig::GlobalConfig(QObject *parent) : QObject(parent)
{
    qDebug() << "global config init";
    if (!QFile::exists("data/config.dat"))
    {
        qDebug() << "config file does not exists";
        save("rpi","","");
        configured = false;
    }
    else
        loadFromJson();
}

void GlobalConfig::setPlayerId(QString playerId)
{
    this->playerId = playerId;
    save(device,playerId,publicKey);
    checkConfiguration();
}

void GlobalConfig::setPublicKey(QString publicKey)
{
    this->publicKey = publicKey;
    save(device, playerId, publicKey);
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

void GlobalConfig::loadFromJson()
{
    qDebug() << "loading from config.dat";
    QFile configFile("data/config.dat");
    configFile.open(QFile::ReadOnly);
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(configFile.readAll());
    QJsonObject root = doc.object();
    this->device = root["device"].toString();
    this->playerId = root["playerId"].toString();
    this->publicKey = root["publicKey"].toString();
    this->playerConfig = root["playerConfig"].toString();
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
    QJsonDocument doc(root);
    QFile file ("data/config.dat");
    file.open(QFile::WriteOnly);
    file.write(doc.toJson());
    file.flush();
    file.close();
}

void GlobalConfig::save(QString device, QString playerId, QString publicKey)
{
    QJsonObject root;
    root["device"] = device;
    root["playerId"] = playerId;
    root["publicKey"] = publicKey;
    root["playerConfig"] = playerConfig;
    QJsonDocument doc(root);
    QFile file ("data/config.dat");
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


#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#define GlobalConfigInstance Singleton<GlobalConfig>::instance()
#include <QObject>
#include <singleton.h>

class GlobalConfig : public QObject
{
    Q_OBJECT
public:
    explicit GlobalConfig(QObject *parent = 0);
    void setPlayerId(QString playerId);
    void setPublicKey(QString publicKey);
    void setPlayerConfig(QString playerConfig);
    void setSessionKey(QString key);

    bool isConfigured(){return configured;}
    bool isPlaylistActivated(){return playerConfig != "";}

    QString getPlayerId(){return playerId;}
    QString getPublicKey(){return publicKey;}
    QString getDevice(){return device;}
    QString getPlayerConfig(){return playerConfig;}
    QString getSessionKey(){return sessionKey;}
signals:

public slots:
private:
    void loadFromJson();
    void save();
    void save(QString device, QString playerId, QString publicKey);
    void checkConfiguration();
    QString device;
    QString playerId;
    QString publicKey;
    QString playerConfig;
    QString sessionKey;
    bool configured;
};

#endif // GLOBALCONFIG_H

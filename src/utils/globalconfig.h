#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#define GlobalConfigInstance Singleton<GlobalConfig>::instance()
#include <QObject>
#include <QHash>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <singleton.h>

#include "videoserviceresult.h"


//class contains current player config
//as its signleton you can access it by GlobalConfigInstance
//every time you call set*() method - it saves result into config file
class GlobalConfig : public QObject
{
    Q_OBJECT
public:
    explicit GlobalConfig(QObject *parent = 0);
    void setToken(QString token);
    void setActivationCode(QString code);
    void setVideoQuality(QString quality);
    void setVirtualScreenId(QString id){virtualScreenId = id;}

    void setGetPlaylistTimerTime(int msecs);
    void setStatsInverval(int secs);

    void setAutoBrightness(bool active);
    void setMinBrightness(int minBrightness);
    void setMaxBrightness(int maxBrightness);

    void setReleyConfig(QHash<int, QList<int> > &reley_1, QHash<int, QList<int> > &reley_2);
    bool getFirstReleyStatus();
    bool getSecondReleyStatus();
    void setReleyEnabled(bool first, bool second);
    bool getReleyEnabled(bool isFirst){return isFirst? firstReleyEnabled : secondReleyEnabled;}

    void setSettings(QJsonObject json);
    QJsonObject getSettings();
    void setPlaylist(QJsonObject json);
    QJsonObject getPlaylist();
    void setAreas(QJsonArray json);
    QJsonArray getAreas();
    void setPlaylistNetworkError(int error_id);
    int getPlaylistNetworkError(){return playlistNetworkErrorId;}
    
    void setPlayerConfig(QJsonObject result);
    QJsonObject getPlayerConfig();

    void addContentPlaying(QString contentId);
    void removeContentPlaying(QString contentId);
    bool isContentInPlay(QString contentId);

    void setAreaToVirtualScreen(QString areaId, QString virtualScreenId);
    QString getVirtualScreenId(QString areaId);

    void setVolume(int value);
    int getVolume(){return volume;}


    int getStatsInterval(){return statsInterval;}
    bool isAutoBrightnessActive(){return autobright;}
    int getMinBrightness(){return min_bright;}
    int getMaxBrightness(){return max_bright;}

    bool isConfigured(){return configured;}

    QString getDevice(){return device;}
    QString getToken(){return token;}
    QString getVideoQuality(){return quality;}
    QString getActivationCode(){return activationCode;}
    QString getVirtualScreenId(){return virtualScreenId;}

    int getGetPlaylistTimerTime() {return getPlaylistTimerTime;}

signals:

public slots:
private:
    void loadFromJson();
    void save();
    void checkConfiguration();
    QString device;
    bool configured;
    QString token;
    QString quality;
    QString activationCode;
    QString virtualScreenId;

    int statsInterval;
    bool autobright;
    int min_bright;
    int max_bright;


    QHash<int, QList<int> > time_targeting_relay_1;
    QHash<int, QList<int> > time_targeting_relay_2;
    QHash<QString, QString> areaToVirtualScreen;
    bool firstReleyEnabled, secondReleyEnabled;

    int getPlaylistTimerTime;

    QJsonObject settings;
    SettingsRequestResult settingsObject;

    QJsonArray areas;
    QJsonObject playlist, playerConfigAPI;
    int playlistNetworkErrorId;
    int volume;
    QList<QString> contentInPlay;
};

#endif // GLOBALCONFIG_H


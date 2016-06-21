#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#define GlobalConfigInstance Singleton<GlobalConfig>::instance()
#include <QObject>
#include <QHash>
#include <QVector>
#include <singleton.h>


//class contains current player config
//as its signleton you can access it by GlobalConfigInstance
//every time you call set*() method - it saves result into config file
class GlobalConfig : public QObject
{
    Q_OBJECT
public:
    explicit GlobalConfig(QObject *parent = 0);
    void setPlayerConfig(QString playerConfig);
    void setToken(QString token);
    void setActivationCode(QString code);
    void setVideoQuality(QString quality);
    void setVirtualScreenId(QString id){virtualScreenId = id;}

    void setGetPlaylistTimerTime(int msecs);
    void setStatsInverval(int secs);

    void setAutoBrightness(bool active);
    void setMinBrightness(int minBrightness);
    void setMaxBrightness(int maxBrightness);


    int getStatsInterval(){return statsInterval;}
    bool isAutoBrightnessActive(){return autobright;}
    int getMinBrightness(){return min_bright;}
    int getMaxBrightness(){return max_bright;}

    bool isConfigured(){return configured;}
    bool isPlaylistActivated(){return playerConfig != "";}

    QString getDevice(){return device;}
    QString getPlayerConfig(){return playerConfig;}
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
    QString playerConfig;
    bool configured;
    QString token;
    QString quality;
    QString activationCode;
    QString virtualScreenId;

    int statsInterval;
    bool autobright;
    int min_bright;
    int max_bright;
    QHash<QString, QVector<int> > time_targeting_relay_1;
    QHash<QString, QVector<int> > time_targeting_relay_2;


    int getPlaylistTimerTime;

};

#endif // GLOBALCONFIG_H


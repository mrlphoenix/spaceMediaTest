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
    void setPlayerConfig(QString playerConfig);
    void setToken(QString token);
    void setVideoQuality(QString quality);

    void setGetPlaylistTimerTime(int msecs);
    void setCpuInfoTimerTime(int msecs);
    void setReportTimerTime(int msecs);
    void setSysInfoTimerTime(int msecs);
    void setResourceCounterTimerTime(int msecs);
    void setGpsTimerTime(int msecs);

    bool isConfigured(){return configured;}
    bool isPlaylistActivated(){return playerConfig != "";}

    QString getDevice(){return device;}
    QString getPlayerConfig(){return playerConfig;}
    QString getToken(){return token;}
    QString getVideoQuality(){return quality;}

    int getGetPlaylistTimerTime() {return getPlaylistTimerTime;}
    int getCpuInfoTimerTime() {return cpuInfoTimerTime;}
    int getReportTimerTime() {return reportTimerTime;}
    int getSysInfoTimerTime() {return sysInfoTimerTime;}
    int getResourceCounterTimerTime() {return resourceCounterTimerTime;}
    int getGpsTimerTime() {return gpsTimerTime;}
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

    int getPlaylistTimerTime, cpuInfoTimerTime, reportTimerTime, sysInfoTimerTime, resourceCounterTimerTime, gpsTimerTime;
    const int getPlaylistTimerDefaultTime = 30000, cpuInfoTimerDefaultTime = 10000,
              reportTimerDefaultTime = 60000, sysInfoTimerDefaultTime = 10000, resourceCounterTimerDefaultTime = 10000, gpsTimerDefaulTime = 10000;
};

#endif // GLOBALCONFIG_H

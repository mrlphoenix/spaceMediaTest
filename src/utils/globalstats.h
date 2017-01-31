#ifndef GLOBALSTATS_H
#define GLOBALSTATS_H

#include <QObject>
#include <QTime>
#include <QList>
#include <QDebug>
#include <QDateTime>

#define GlobalStatsInstance Singleton<GlobalStats>::instance()

struct TimeZoneEntry{
    QString shortName;
    double lat;
    double lon;
    QString longName;
};

//this class is for storing current device stats
//unlike globalconfig it does not store info in file
//its singleton and can be aceessed by GlobalStatsInstance helper
class GlobalStats : public QObject
{
    Q_OBJECT
public:
    explicit GlobalStats(QObject *parent = 0);
    void registryDownload();
    void registryConnectionError();
    void registryPlaylistError();
    void setContentPlay(int count);
    void setContentTotal(int count);

    void setCpu(double cpu);
    void setMemory(double memory);
    void setTraffic(qlonglong in, qlonglong out);
    double getTrafficIn();
    double getTrafficOut();
    double getMemory(){return memory;}
    double getCpu(){return cpu;}
    void setMonitorActive(bool isActive);
    void setConnectionState(bool isActive);
    void setBalance(double balance);
    void setCurrentItem(QString item){currentItem = item;}
    QString getCurrentItem() {return currentItem;}

    void setGps(double lat, double lgt) {latitude = lat; longitude = lgt;}
    double getLatitude(){return latitude;}
    double getLongitude(){return longitude;}
    int getUTCOffset();




    void setSunset(QTime sunset);
    void setSunrise(QTime sunrise);
    QTime getSunset(){return sunset;}
    QTime getSunrise(){return sunrise;}
    bool shouldUpdateSunset();
    bool shouldUpdateSunrise();


    void itemPlayed(QString areaId, QString contentId, QDateTime date);
    bool checkDelayPass(QString areaId, QString contentId);
    QDateTime getItemLastPlayDate(QString areaId, QString contentId);
    bool itemWasPlayed(QString areaId, QString contentId);

    void setItemPlayTimeout(QString contentId, int timeout);
    int getItemPlayTimeout(QString contentId);
    bool wasAnyItemPlayed(QString areaId);

    void setSystemData(QString tag, QByteArray data);
    QByteArray getSystemData(QString tag);

    void setCampaignEndDate(QDateTime date) {campaignEndDate = date;}
    QDateTime getCampaignEndDate() {return campaignEndDate;}

    void setCRC32Hex(QString v) { crc32Hex = v; }
    QString getCRC32Hex() { return crc32Hex; }

    void setHDMI_CEC(QString value) { hdmiCEC = value; }
    QString getHDMI_CEC() {qDebug() << "HDMICEC = " << hdmiCEC; return hdmiCEC; }

    void setHDMI_GPIO(bool value) { hdmiGPIO = value; }
    bool getHDMI_GPIO() {qDebug() << "HDMIGPIO = " << hdmiGPIO; return hdmiGPIO; }

    struct Report
    {
        int downloadCount;
        int contentPlayCount;
        int contentTotalCount;
        int connectionErrorCount;
        int playlistErrorCount;
    };
    struct SystemInfo
    {
        double cpu, memory;
        qlonglong trafficIn, trafficOut;
        bool monitorActive, connectionActive;
        double balance;
    };

    Report generateReport();
    SystemInfo generateSystemInfo();

signals:

public slots:
private:
    //report vars
    int downloadCount;
    int contentPlayCount;
    int contentTotalCount;
    int connectionErrorCount;
    int playlistErrorCount;

    //system info vars
    double cpu, memory;
    qlonglong trafficIn, trafficOut, prevTrafficIn, prevTrafficOut;
    bool monitorActive, connectionActive;
    double balance;
    QString currentItem;
    QString crc32Hex;

    //additional
    bool connectionDropped;
    double latitude, longitude;

    QTime sunrise, sunset;
    QTime measureSunrise, measureSunset;
    QHash<QString, QByteArray> systemData;

    QList<TimeZoneEntry> tzDatabase;
    QDateTime lastTzCheck;
    int cachedTzValue;
    QHash<QString, QHash<QString, QDateTime> > lastTimePlayed;
    QHash<QString, int> itemTimeout;

    QDateTime campaignEndDate;
    QString hdmiCEC;
    bool hdmiGPIO;

};

#endif // GLOBALSTATS_H

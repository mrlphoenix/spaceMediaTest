#ifndef GLOBALSTATS_H
#define GLOBALSTATS_H

#include <QObject>

#define GlobalStatsInstance Singleton<GlobalStats>::instance()

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
    double getTrafficIn(){return trafficIn;}
    double getTrafficOut(){return trafficOut;}
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
    qlonglong trafficIn, trafficOut, trafficTotalIn, trafficTotalOut;
    bool monitorActive, connectionActive;
    double balance;
    QString currentItem;

    //additional
    bool connectionDropped;
    double latitude, longitude;
};

#endif // GLOBALSTATS_H

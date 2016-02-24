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

    void setCpu(int cpu);
    void setMemory(int memory);
    void setTraffic(double in, double out);
    void setMonitorActive(bool isActive);
    void setConnectionState(bool isActive);
    void setBalance(double balance);
    void setCurrentItem(QString item){currentItem = item;}
    QString getCurrentItem() {return currentItem;}

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
        int cpu, memory;
        int trafficIn, trafficOut;
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
    int cpu, memory;
    double trafficIn, trafficOut, trafficTotalIn, trafficTotalOut;
    bool monitorActive, connectionActive;
    double balance;
    QString currentItem;

    //additional
    bool connectionDropped;
};

#endif // GLOBALSTATS_H

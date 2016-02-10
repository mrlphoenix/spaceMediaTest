#ifndef TELEDSSHEDULER_H
#define TELEDSSHEDULER_H

#include <QObject>
#include <QTimer>

class TeleDSSheduler : public QObject
{
    Q_OBJECT
public:
    explicit TeleDSSheduler(QObject *parent = 0);
    ~TeleDSSheduler();

    enum Task{GET_PLAYLIST, CPU_INFO, REPORT, SYS_INFO, RESOURCE_COUNT, GPS, ALL};

    void start(Task t);
    void stop(Task t);
    void restart(Task t);

signals:
    void getPlaylist();
    void cpuInfo();
    void report();
    void sysInfo();
    void resourceCounter();
    void gps();
public slots:

private:
    QTimer * getPlaylistTimer;
    QTimer * cpuInfoTimer;
    QTimer * reportTimer;
    QTimer * sysInfoTimer;
    QTimer * resourceCounterTimer;
    QTimer * gpsTimer;
};

#endif // TELEDSSHEDULER_H

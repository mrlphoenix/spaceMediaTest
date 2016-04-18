#ifndef ANDROIDSPECS_H
#define ANDROIDSPECS_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>
#include <QSqlRecord>
#include "platformdefines.h"

class PlatformSpecific : public QObject
{
    Q_OBJECT
public:

    struct HardwareInfo
    {
        QString vendor;
        QString deviceName;
        QString deviceModel;
        QString osVersion;
        QString osName;
        QString cpuName;
    };

    struct SystemInfo
    {
        static SystemInfo get();
        static SystemInfo fromRecord(const QSqlRecord &record);
        QJsonObject serialize() const;
        QDateTime time;
        double cpu;
        double latitude;
        double longitude;
        double battery;
        int traffic;
        int free_memory;
        QString wifi_mac;
        int hdmi_cec;
        int hdmi_gpio;
        int free_space;
    };
    explicit PlatformSpecific(QObject *parent = 0);
    static QString getUniqueId();
    static int64_t getTrafficIn();
    static int64_t getTrafficOut();
    static int getMemoryUsage();
    static int getFreeMemory();
    static bool getHdmiCEC();
    static bool getHdmiGPIO();
    static double getBattery();
    static double getAvgUsage();
    static QString getWifiMac();
    static int getFreeSpace();
    static HardwareInfo getHardwareInfo();

signals:

public slots:
private:
    static QString getRpiDeviceNameById(QString id);
};

#endif // ANDROIDSPECS_H

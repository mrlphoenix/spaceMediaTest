#ifndef ANDROIDSPECS_H
#define ANDROIDSPECS_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>
#include "platformdefines.h"

class PlatformSpecs : public QObject
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
        QJsonObject serialize();
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
        //time TEXT
        //cpu REAL
        //latitude REAL
        //longitude REAL
        //battery REAL
        //traffic_in INTEGER
        //traffic_out INTEGER
        //free_memory INTEGER
        //wifi_mac TEXT
        //hdmi_cec INTEGER
        //hdmi_gpio INTEGER
        //free_space INTEGER
    };

    explicit PlatformSpecs(QObject *parent = 0);
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

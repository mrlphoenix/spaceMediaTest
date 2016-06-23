#ifndef ANDROIDSPECS_H
#define ANDROIDSPECS_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>
#include <QSqlRecord>
#include "platformdefines.h"


//class contains platform specific method for generating some data
class PlatformSpecific : public QObject
{
    Q_OBJECT
public:

    //struct used to send while initialization of player
    struct HardwareInfo
    {
        QString vendor;
        QString deviceName;
        QString deviceModel;
        QString osVersion;
        QString osName;
        QString cpuName;
    };

    //struct contains current device state
    //used to send as part of statistics
    struct SystemInfo
    {
        //method get read system data from device
        //use PlayformSpecific::SystemInfo::get()
        static SystemInfo get();

        //generates SystemInfo from SQL Record
        //used when we read events from database
        static SystemInfo fromRecord(const QSqlRecord &record);

        //serializes SystemInfo to JSON object
        //used when we send statistic
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

    //following methods are used by SystemInfo::get
    //provide platform independent interface for system data
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

    //GPIO
    static void init();
    static void writeGPIO(int n, int value);
    static void turnOnFirstReley();
    static void turnOffFirstReley();
    static void turnOnSecondReley();
    static void turnOffSecondReley();

    //helper method for generating HardwareInfo used for initialization
    static HardwareInfo getHardwareInfo();

    //method for extracting zip-file content using zip file name and content-id
    static void extractFile(QString file, QString id);

    //helper method used to flush data to file
    static void writeToFile(QByteArray data, QString filename);

signals:

public slots:
private:
    static QString getRpiDeviceNameById(QString id);
};


#endif // ANDROIDSPECS_H

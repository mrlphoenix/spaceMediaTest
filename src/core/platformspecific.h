#ifndef PLATFORMSPECIFIC_H
#define PLATFORMSPECIFIC_H
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QSqlRecord>
#include <QThread>

#include "platformdefines.h"
#include "singleton.h"

#define PlatformSpecificService Singleton<Platform::PlatformSpecific>::instance()


namespace Platform {

struct HardwareInfo
{
    QString vendor;
    QString deviceName;
    QString deviceModel;
    QString osVersion;
    QString osName;
    QString cpuName;
    QString uid;
};

struct SystemInfo
{
    //method get read system data from device

    //generates SystemInfo from SQL Record
    //used when we read events from database
    static SystemInfo fromRecord(const QSqlRecord &record);

    //serializes SystemInfo to JSON object
    //used when we send statistics
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


class PlatformSpecificWorker : public QObject
{
    Q_OBJECT
public:
    PlatformSpecificWorker();
    PlatformSpecificWorker(QObject * parent);
    ~PlatformSpecificWorker();

signals:
    void systemInfoReady(Platform::SystemInfo info);
    void hardwareInfoReady(Platform::HardwareInfo info);
public slots:

    //provide platform independent interface for system stats
    void generateSystemInfo();

    //helper methods for generateSystemInfo
    QString getUniqueId();
    int64_t getTrafficIn();
    int64_t getTrafficOut();
    int getMemoryUsage();
    int getFreeMemory();
    bool getHdmiCEC();
    bool getHdmiGPIO();
    double getBattery();
    double getAvgUsage();
    QString getWifiMac();
    int getFreeSpace();


    //following method are used to operate with GPIO
    //implemented only for RPI, on other platforms do nothing (right now)
    void writeGPIO(int n, int value);
    void turnOnFirstReley();
    void turnOffFirstReley();
    void turnOnSecondReley();
    void turnOffSecondReley();

    //helper method for generating HardwareInfo used for initialization request
    void getHardwareInfo();

    //method for extracting zip-file content using zip file name and content-id
    void extractFile(QString file, QString id);

    //helper method used to flush data to file
    void writeToFile(QByteArray data, QString filename);
protected:
    static QString getRpiDeviceNameById(QString id);

};

class PlatformSpecificThread : public QThread
{
    Q_OBJECT
public:
    PlatformSpecificThread(QObject * parent = 0);
    ~PlatformSpecificThread();

    void generateSystemInfo();
    void generateHardwareInfo();
    void turnOnFirstReley();
    void turnOffFirstReley();
    void turnOnSecondReley();
    void turnOffSecondReley();
    void extractFile(QString file, QString id);
    void writeToFile(QByteArray data, QString filename);

signals:
    //signals from PlatformSpecificWorker
    void systemInfoReady(Platform::SystemInfo info);
    void hardwareInfoReady(Platform::HardwareInfo info);

    //signals to PlatformSpecificWorker
    void generateSystemInfoSignal();
    void generateHardwareInfoSignal();
    void turnOnFirstReleySignal();
    void turnOffFirstReleySignal();
    void turnOnSecondReleySignal();
    void turnOffSecondReleySignal();

    void extractFileSignal(QString file, QString id);
    void writeToFileSignal(QByteArray data, QString filename);

public slots:

protected:
    void run();
private:
    PlatformSpecificWorker * worker;
};

class PlatformSpecific : public QObject
{
    Q_OBJECT
public:
    PlatformSpecific();
    ~PlatformSpecific();

    static QString getFileHash(QString filename);

public slots:
    void generateSystemInfo();
    void generateHardwareInfo();
    void turnOnFirstReley();
    void turnOffFirstReley();
    void turnOnSecondReley();
    void turnOffSecondReley();

    void extractFile(QString file, QString id);
    void writeToFile(QByteArray data, QString filename);

signals:
    void systemInfoReady(Platform::SystemInfo info);
    void hardwareInfoReady(Platform::HardwareInfo info);

private:
    PlatformSpecificThread * thread;
};

}

#endif // PLATFORMSPECIFIC_H

#ifndef ANDROIDSPECS_H
#define ANDROIDSPECS_H

#include <QObject>
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

    explicit PlatformSpecs(QObject *parent = 0);
    static QString getUniqueId();
    static int64_t getTrafficIn();
    static int64_t getTrafficOut();
    static int getMemoryUsage();
    static double getAvgUsage();
    static HardwareInfo getHardwareInfo();

signals:

public slots:
private:
    static QString getRpiDeviceNameById(QString id);
};

#endif // ANDROIDSPECS_H

#ifndef ANDROIDSPECS_H
#define ANDROIDSPECS_H

#include <QObject>
#include "platformdefines.h"

class PlatformSpecs : public QObject
{
    Q_OBJECT
public:
    explicit PlatformSpecs(QObject *parent = 0);
    static QString getUniqueId();
    static int64_t getTrafficIn();
    static int64_t getTrafficOut();
    static int getMemoryUsage();
    static double getAvgUsage();

signals:

public slots:
};

#endif // ANDROIDSPECS_H

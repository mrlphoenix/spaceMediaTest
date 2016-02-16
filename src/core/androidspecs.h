#ifndef ANDROIDSPECS_H
#define ANDROIDSPECS_H
#include "platformdefines.h"

#ifdef PLATFORM_DEFINE_ANDROID

#include <QObject>
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>

class AndroidSpecs : public QObject
{
    Q_OBJECT
public:
    explicit AndroidSpecs(QObject *parent = 0);
    QString getImei();

signals:

public slots:
};
#endif

#endif // ANDROIDSPECS_H

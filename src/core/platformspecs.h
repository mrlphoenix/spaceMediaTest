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

signals:

public slots:
};

#endif // ANDROIDSPECS_H

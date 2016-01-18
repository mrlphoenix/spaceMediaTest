#ifndef RPIVIDEOPLAYER_H
#define RPIVIDEOPLAYER_H

#include <QObject>
#include <QWidget>

class rpiVideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit rpiVideoPlayer(QObject *parent = 0);

signals:

public slots:
};

#endif // RPIVIDEOPLAYER_H

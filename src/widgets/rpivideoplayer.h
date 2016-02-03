#ifndef RPIVIDEOPLAYER_H
#define RPIVIDEOPLAYER_H

#include <QGuiApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QUrl>
#include <QTimer>
#include <QList>
#include <QDebug>
#include "randomplaylist.h"

class RpiVideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit RpiVideoPlayer(PlayerConfig::Area config, QObject * parent);
    ~RpiVideoPlayer();
    QString getFullPath(QString fileName);
    void update(PlayerConfig config);
    void setConfig(PlayerConfig::Area area);
public slots:
    void invokeNextVideoMethod(QString name);
    void next();
    void bindObjects();
protected:
    QQuickView view;
    AbstractPlaylist * playlist;
    bool isPlaylistRandom;
    PlayerConfig::Area config;
    QObject * viewRootObject;

};

#endif // RPIVIDEOPLAYER_H

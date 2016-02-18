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
    explicit RpiVideoPlayer(QObject * parent);
    ~RpiVideoPlayer();
    QString getFullPath(QString fileName);
    void update(PlayerConfig config);
    void setConfig(PlayerConfig::Area area);
    void play();
    void stop();

    struct CurrentItemStatus
    {
        QString item;
        bool isPlaying;
    };

    bool isPlaying() {return status.isPlaying;}
    QString getCurrentItem() {return status.item;}

public slots:
    void invokeNextVideoMethod(QString name);
    void invokeFileProgress(double p, QString name);
    void invokeProgress(double p);
    void invokeSimpleProgress(double p, QString);
    void invokeDownloadDone();

    void invokePlayerActivationRequiredView(QString url, QString playerId);
    void invokeNoItemsView(QString url);
    void invokeDownloadingView();

    void next();
    void playNext();
    void bindObjects();

    void showVideo();
    void hideVideo();
    void setBrightness(double value);//[0; 1] - lower brightness [1; 2] - higher brightness
protected:
    void invokeShowVideo(bool isVisible);

    QQuickView view;
    AbstractPlaylist * playlist;
    bool isPlaylistRandom;
    PlayerConfig::Area config;
    QObject * viewRootObject;

    CurrentItemStatus status;
    int delay;
};

#endif // RPIVIDEOPLAYER_H

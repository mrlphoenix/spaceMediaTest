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
#include <QQueue>
#include "playlist.h"
#include "platformspecific.h"


class TeleDSPlayer : public QObject
{
    Q_OBJECT
public:
    explicit TeleDSPlayer(QObject * parent);
    ~TeleDSPlayer();
    QString getFullPath(QString fileName);
    QString getFullPathZip(QString path);

    void show();
    void update(PlayerConfig config);
    void setConfig(PlayerConfig::VirtualScreen area);
    void play();
    void stop();

    struct CurrentItemStatus
    {
        QString item;
        bool isPlaying;
    };

    bool isPlaying() {return status.isPlaying;}
    QString getCurrentItem() {return status.item;}
    bool isFileCurrentlyPlaying(QString name);

signals:
    void refreshNeeded();
public slots:
    void invokeNextVideoMethod(QString name);
    void invokeNextVideoMethodAdvanced(QString name);
    void invokeFileProgress(double p, QString name);
    void invokeProgress(double p);
    void invokeSimpleProgress(double p, QString);
    void invokeDownloadDone();
    void invokeVersionText();

    void invokePlayerActivationRequiredView(QString url, QString playerId);
    void invokeNoItemsView(QString url);
    void invokeDownloadingView();
    void invokeEnablePreloading();
    void invokeStop();
    /*
     *  "brand_background": "",
  "brand_logo": "",
  "brand_color_1": "",
  "brand_color_2": "",
  "brand_teleds_copyright": 0
     * */
    void invokeSetTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, QString color3);
    void invokeRestoreDefaultTheme();

    void next();
    void playNext();
    void bindObjects();
    void stopPlaying();

    void showVideo();
    void hideVideo();
    void setBrightness(double value);//[0; 1] - lower brightness [1; 2] - higher brightness
    void gpsUpdate(double lat, double lgt);

    void systemInfoReady(Platform::SystemInfo info);
protected:
    void invokeShowVideo(bool isVisible);

    QQuickView view;
    AbstractPlaylist * playlist;
    bool isPlaylistRandom;
    PlaylistAPIResult config;
    QObject * viewRootObject;
    QQueue<QString> playedIds;

    CurrentItemStatus status;
    int delay;
    bool isActive;
};



#endif // RPIVIDEOPLAYER_H


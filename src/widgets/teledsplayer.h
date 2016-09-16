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
    void setConfig(PlayerConfig::VirtualScreen contentArea, PlayerConfig::VirtualScreen widgetArea);
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
    void invokeNextVideoMethodAdvanced(QString name, bool isWidget = false);
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
    void invokeStopMainPlayer();
    void invokeSetTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, QString color3, bool tileMode, bool showTeleDSLogo);
    void invokeSetMenuTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, bool tileMode, bool showTeleDSLogo);
    void invokeRestoreDefaultTheme();

    void invokeSetPlayerVolume(int value);

    void invokeSetLicenseData();
    void invokeSetDeviceInfo();

    ///mode = ["fullscreen", "split"]
    void invokeSetDisplayMode(QString mode);
    void invokeSetContentPosition(float contentLeft = 0.f, float contentTop = 0.f, float contentWidth = 100.f, float contentHeight = 100.f,
                                  float widgetLeft = 0.f, float widgetTop = 0.f, float widgetWidth = 0.f, float widgetHeight = 0.f);

    void runAfterStop();
    void next();
    void nextWidget();
    void playNext() {return playNextGeneric(false);}
    void playNextWidget() {return playNextGeneric(true);}
    void playNextGeneric(bool isWidget);
    void bindObjects();
    void stopPlaying();

    void setRestoreModeTrue();
    void setRestoreModeFalse();

    void showVideo();
    void hideVideo();
    void setBrightness(double value);//[0; 1] - lower brightness [1; 2] - higher brightness
    void gpsUpdate(double lat, double lgt);

    void systemInfoReady(Platform::SystemInfo info);
protected:
    void invokeShowVideo(bool isVisible);

    QQuickView view;
    AbstractPlaylist * playlist;
    AbstractPlaylist * widgetPlaylist;
    bool isPlaylistRandom;
    PlaylistAPIResult config, widgetConfig;
    QObject * viewRootObject;
    QQueue<QString> playedIds;

    CurrentItemStatus status;
    int delay;
    bool isActive;
    bool isSplitScreen;
};
#endif // RPIVIDEOPLAYER_H


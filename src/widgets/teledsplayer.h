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
#include <QEvent>
#include "playlist.h"
#include "platformspecific.h"


class TeleDSPlayer : public QObject
{
    Q_OBJECT
public:
    explicit TeleDSPlayer(QObject * parent);
    ~TeleDSPlayer();
    QString getFullPath(QString fileName, AbstractPlaylist *playlist);
    QString getFullPathZip(QString path);

    void show();
    void updateConfig(PlayerConfigAPI &playerConfig);
    void play(int delay = 1000);
    void stop();
    int nextCampaign() { return config.nextCampaign(); }
    int getCurrentCampaignIndex() { return config.currentCampaignId; }
    PlayerConfigAPI::Campaign::Area getAreaById(QString id);

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
    bool eventFilter(QObject *target, QEvent *event);
    //
    void invokeNextVideoMethodAdvanced(QString name, QString area_id);
    void invokeFileProgress(double p, QString name);
    void invokeProgress(double p);
    void invokeSimpleProgress(double p, QString);
    void invokeDownloadDone();
    void invokeVersionText();
    void invokePlayerCRC();
    void invokeToggleVisibility(int status);

    void invokePlayerActivationRequiredView(QString url, QString playerId);
    void invokeNoItemsView(QString url);
    void invokeDownloadingView();
    void invokeStop();
    void invokePrepareStop();
    void invokeStopMainPlayer();
    void invokeSetTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, QString color3, bool tileMode, bool showTeleDSLogo);
    void invokeSetMenuTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, bool tileMode, bool showTeleDSLogo);
    void invokeRestoreDefaultTheme();

    void invokeUpdateState();

    //new methods
    void invokeSetAreaCount(int areaCount);
    void invokePlayCampaign(int campaignIndex);
    void invokeInitArea(QString name, double campaignWidth, double campaignHeight, double x, double y, double w, double h, int rotation);

    void invokeSetPlayerVolume(int value);

    void invokeSetLicenseData();
    void invokeSetDeviceInfo();
    void invokeSetDisplayMode(QString mode);
    void invokeSetContentPosition(float contentLeft = 0.f, float contentTop = 0.f, float contentWidth = 100.f, float contentHeight = 100.f,
                                  float widgetLeft = 0.f, float widgetTop = 0.f, float widgetWidth = 0.f, float widgetHeight = 0.f);

    void runAfterStop();
    void next(QString area_id);
    void playNextGeneric(QString area_id);
    void bindObjects();
    void stopPlaying();

    void setRestoreModeTrue();
    void setRestoreModeFalse();

    void showVideo();
    void hideVideo();
    void setBrightness(double value);//[0; 1] - lower brightness [1; 2] - higher brightness
    void gpsUpdate(double lat, double lgt);

    void systemInfoReady(Platform::SystemInfo info);

    void nextCampaignEvent();
    void nextItemEvent();
protected:
    void invokeShowVideo(bool isVisible);

    QQuickView view;
    QHash<QString, AbstractPlaylist*> playlists;
    bool isPlaylistRandom;

    PlayerConfigAPI config;
    QObject * viewRootObject;
    QQueue<QString> playedIds;

    CurrentItemStatus status;
    int delay;
    bool isActive;
    bool isSplitScreen;
    QTimer * nextCampaignTimer;
    QTimer * checkNextVideoAfterStopTimer;
};

#endif // RPIVIDEOPLAYER_H


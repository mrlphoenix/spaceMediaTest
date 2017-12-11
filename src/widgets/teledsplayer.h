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
#include "spacemediamenu.h"

struct PlaylistContentId
{
    QString areaId;
    QString id;
};

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
    void play();
    void stop();
    int nextCampaign() { return config.nextCampaign(); }
    int getCurrentCampaignIndex() { return config.currentCampaignId; }
    PlayerConfigAPI::Campaign::Area getAreaById(QString id);

    struct CurrentItemStatus
    {
        QString item;
        bool isPlaying;
    };

    bool isPlaying();
    QString getCurrentItem() { return status.item; }
    bool isFileCurrentlyPlaying(QString name);
    
    QVector<QString> getAreas(QString areaString);
    QString packAreas(QString oldAreas, QString area);

signals:
    void refreshNeeded();
    void readyToUpdate();
    void keyDown(int code);
    void keyUp(int code);
public slots:
    bool eventFilter(QObject *target, QEvent *event);
    //
    void prepareStop();

    void onKeyDown(int code);
    void onKeyUp(int code);
    //QML bindings
    void invokeSetSpaceMediaTheme();
    void invokeSetSpaceMediaThemeBlack();
    void invokePlayerisReady();

    void invokeNextVideoMethodAdvanced(QString name, QString area_id);
    void invokeFileProgress(double p, QString name);
    void invokeProgress(double p);
    void invokeSimpleProgress(double p, QString);
    void invokeBackDownloadProgressBarVisible(bool isVisible);
    void invokeDownloadDone();
    void invokeVersionText();
    void invokeShowPassword();
    void invokeShowInternetConnectionInfo();
    void invokePlayerCRC();
    void invokeToggleVisibility(int status);
    void invokeSetDelay(int delay);

    void invokePlayerActivationRequiredView(QString url, QString playerId, bool showCode);
    void invokeNoItemsView(QString url);
    void invokeDownloadingView();
    void invokeStop();
    void invokePrepareStop();
    void invokeStopMainPlayer();
    void invokeSetTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, QString color3, bool tileMode, bool showTeleDSLogo);
    void invokeSetMenuTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, bool tileMode, bool showTeleDSLogo);
    void invokeRestoreDefaultTheme();
    void invokeToggleMenu();
    void invokeUpdateState();
    void invokeSetAreaCount(int areaCount);
    void invokePlayCampaign(int campaignIndex);
    void invokeInitArea(QString name, double campaignWidth, double campaignHeight, double x, double y, double w, double h, int rotation, double opacity, int index);
    void invokeSetPlayerVolume(int value);
    void invokeSetLicenseData();
    void invokeSetDeviceInfo();
    void invokeSetDisplayMode(QString mode);
    void invokeSetContentPosition(float contentLeft = 0.f, float contentTop = 0.f, float contentWidth = 100.f, float contentHeight = 100.f,
                                  float widgetLeft = 0.f, float widgetTop = 0.f, float widgetWidth = 0.f, float widgetHeight = 0.f);
    void invokeSkipCurrentItem();

    //SpaceMediaMenu QML methods
    void invokeMenuDisplayRotationSelected();
    void invokeMenuDisplayRotationChanged(int value, QString text);
    void invokeMenuHDMIGroupSelected();
    void invokeMenuHDMIGroupChanged(int value, QString text);
    void invokeMenuHDMIModeSelected();
    void invokeMenuHDMIModeChanged(int value, QString text);
    void invokeMenuHDMIDriveSelected();
    void invokeMenuHDMIDriveChanged(int value, QString text);
    void invokeMenuHDMIBoostSelected();
    void invokeMenuHDMIBoostChanged(int value);
    //
    void invokeMenuWifiNetworkSelected();
    void invokeMenuWifiNetworkChanged(QString text);
    void invokeMenuWifiNameSelected();
    void invokeMenuWifiNameChanged(QString text);
    void invokeMenuWifiPassSelected();
    void invokeMenuWifiPassChanged(QString text);
    void invokeMenuSaveSelected();
    void invokeMenuSavePressed();
    void invokeMenuCancelSelected();
    void invokeMenuCancelPressed();
    void invokeTogglePlayerIDVisible();

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
    void invokeSetAreaMuted(int index, bool isMuted);
protected:
    void invokeShowVideo(bool isVisible);

    QQuickView view;
    SpaceMediaMenuBackend menu;

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
    bool shouldStop;
};

#endif // RPIVIDEOPLAYER_H


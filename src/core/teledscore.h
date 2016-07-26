#ifndef TELEDSCORE_H
#define TELEDSCORE_H

#include <QObject>
#include <QVector>
#include <QProcess>
#include <QHash>
#include "soundwidgetinfo.h"
#include "videoservice.h"
#include "videodownloader.h"
#include "teledsplayer.h"
#include "cpustat.h"
#include "teledssheduler.h"
#include "statisticuploader.h"
#include "platformspecific.h"
#include "skinmanager.h"
#include "qhttpserver.h"


class BatteryStatus
{
public:
    BatteryStatus(){minCapacityLevel = 0; maxTimeWithoutPower = -1; inactiveTime = 0; isActive = false;}
    ~BatteryStatus(){;}
    void setConfig(int minCapacityLevel, int maxTimeWithoutPower);
    void setActive(bool isActive);
    bool checkIfNeedToShutDown(Platform::BatteryInfo status);

private:
    Platform::BatteryInfo status;
    int minCapacityLevel, maxTimeWithoutPower;
    int inactiveTime;
    QDateTime lastTimeChecked;
    bool isActive;
};

class TeleDSCore : public QObject
{
    Q_OBJECT
public:
    explicit TeleDSCore(QObject *parent = 0);

signals:
    void playerIdUpdate(QString playerId);
public slots:
    //slot is called when we should reinit player
    void initPlayer();

    //slot is called after player init backend response
    void initResult(InitRequestResult result);
    //slot is called when hardware info is ready
    void hardwareInfoReady(Platform::HardwareInfo info);


    //slots for automatic shutdown
    void checkForAutomaticShutdown();
    void automaticShutdownBatteryInfoReady(Platform::BatteryInfo info);

    //slot is called when backed returned player settings
    void playerSettingsResult(SettingsRequestResult result);
    //slot is called after we load virtual screens
    void virtualScreensResult(PlayerConfig result);
    //slot is called after we get response from loading virtual screens playlists
    void virtualScreenPlaylistResult(QHash<QString, PlaylistAPIResult> result);

    void onThemeReady(ThemeDesc desc);

    //slot is called when we need to update playlist
    void getPlaylistTimerSlot();

    //slot is called when every item got downloaded and we need to show items
    void downloaded();

    //method is outdated
    //used for loading systemInfo
    //now we do it when we play item
    void checkCPUStatus();

    //method is outdated
    //used for getting resource count from db
    //now we ignore this
    void getResourceCount();

    //method is outdated
    //used for update cpu
    //now instaed we updating it @ player play
    void updateCPUStatus(CPUStatWorker::DeviceInfo info);

    //method is outdated
    //we dont count resources
    void resourceCountUpdate(int count);

    //
    void needToDownloadResult(int count);

    void checkReleyTime();

    void showPlayer();


protected:
    void setupDownloader(PlayerConfig& newConfig);
    QVector<QObject*> widgets;
    TeleDSPlayer * teledsPlayer;

    VideoService * videoService;
    StatisticUploader * uploader;
    VideoDownloader * downloader;
    QTimer * statsTimer;

    InitRequestResult playerInitParams;
    QString encryptedSessionKey;
    TeleDSSheduler sheduler;

    PlayerConfig currentConfig;
    BatteryStatus batteryStatus;
    SkinManager * skinManager;

    bool shouldShowPlayer;

};

#endif // TELEDSCORE_H

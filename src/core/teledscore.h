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
    //slot is called after playlist got loaded
    void playlistResult(PlayerConfig result);
    //slot is called when backed returned player settings
    void playerSettingsResult(SettingsRequestResult result);
    //slot is called after we load virtual screens
    void virtualScreensResult(PlayerConfigNew result);
    //slot is called after we get response from loading virtual screens playlists
    void virtualScreenPlaylistResult(QHash<QString, PlaylistAPIResult> result);

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

protected:
    void setupDownloader(PlayerConfig& config);
    void setupDownloader(PlayerConfigNew& newConfig);
    QVector<QObject*> widgets;
    TeleDSPlayer * teledsPlayer;

    VideoService * videoService;
    StatisticUploader * uploader;
    VideoDownloader * downloader;
    QTimer * statsTimer;

    InitRequestResult playerInitParams;
    QString encryptedSessionKey;
    PlayerConfig currentConfig;
    TeleDSSheduler sheduler;

    PlayerConfigNew currentConfigNew;
};

#endif // TELEDSCORE_H

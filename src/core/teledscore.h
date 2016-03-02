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
    void initPlayer();

    void initResult(InitRequestResult result);
    void playlistResult(PlayerConfig result);
    void getPlaylistTimerSlot();

    void fakeInit();
    void downloaded();

    void checkCPUStatus();
    void generateReport();
    void generateSysInfo();
    void getResourceCount();
    void getGps();

    void updateCPUStatus(CPUStatWorker::DeviceInfo info);
    void resourceCountUpdate(int count);

    void needToDownloadResult(int count);

protected:
    void setupDownloader(PlayerConfig& config);
    QVector<QObject*> widgets;
    TeleDSPlayer * rpiPlayer;

    VideoService * videoService;
    StatisticUploader * uploader;
    VideoDownloader * downloader;
    QTimer * statsTimer;

    InitRequestResult playerInitParams;
    QString encryptedSessionKey;
    PlayerConfig currentConfig;
    TeleDSSheduler sheduler;
};


#endif // TELEDSCORE_H

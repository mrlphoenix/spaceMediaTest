#ifndef TELEDSCORE_H
#define TELEDSCORE_H

#include <QObject>
#include <QVector>
#include <QProcess>
#include <QHash>
#include "soundwidgetinfo.h"
#include "videoservice.h"
#include "videodownloader.h"
#include "rpivideoplayer.h"
#include "cpustat.h"

class TeleDSCore : public QObject
{
    Q_OBJECT
public:
    explicit TeleDSCore(QObject *parent = 0);
    QString encryptSessionKey();

signals:
    void playerIdUpdate(QString playerId);
public slots:
    void initPlayer();
    void processError(QProcess::ProcessError error);

    void initResult(InitRequestResult result);
    void playlistResult(PlayerConfig result);
    void getPlaylistTimerSlot();

    void fakeInit();
    void downloaded();

    void checkCPUStatus();
    void generateReport();
    void generateSysInfo();
    void updateCPUStatus(CPUStatWorker::DeviceInfo info);

protected:
    void setupDownloader(PlayerConfig& config);
    QVector<QObject*> widgets;
    RpiVideoPlayer * rpiPlayer;

    VideoService * videoService;
    QTimer * getPlaylistTimer;
    QTimer * cpuInfoTimer;
    QTimer * reportTimer;
    QTimer * generateSysInfoTimer;
    VideoDownloader * downloader;

    InitRequestResult playerInitParams;
    QString encryptedSessionKey;
    PlayerConfig currentConfig;
};


#endif // TELEDSCORE_H

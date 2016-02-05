#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QUrl>

#include <widgetfabric.h>
#include "teledscore.h"
#include "globalconfig.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "androidplayertest.h"


TeleDSCore::TeleDSCore(QObject *parent) : QObject(parent)
{
    AndroidPlayerTest * player = new AndroidPlayerTest(this);

  /*  DatabaseInstance;
    CPUStatInstance;
    sheduler = new TeleDSSheduler(this);
    videoService = new VideoService("http://api.teleds.com");
    uploader = new StatisticUploader(videoService,this);
    statsTimer = new QTimer();
    connect(statsTimer,SIGNAL(timeout()),uploader,SLOT(start()));
    statsTimer->start(5 * 60000);

    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfig)),this,SLOT(playlistResult(PlayerConfig)));

    connect (sheduler,SIGNAL(cpuInfo()), this, SLOT(checkCPUStatus()));
    connect (sheduler,SIGNAL(getPlaylist()), this, SLOT(getPlaylistTimerSlot()));
    connect (&CPUStatInstance,SIGNAL(infoReady(CPUStatWorker::DeviceInfo)),this,SLOT(updateCPUStatus(CPUStatWorker::DeviceInfo)));
    connect (sheduler, SIGNAL(report()), this, SLOT(generateReport()));
    connect (sheduler, SIGNAL(sysInfo()), this, SLOT(generateSysInfo()));
    connect (sheduler, SIGNAL(resourceCounter()), this, SLOT(getResourceCount()));
    connect (&DatabaseInstance,SIGNAL(resourceCount(int)),this,SLOT(resourceCountUpdate(int)));
    connect (sheduler, SIGNAL(gps()),this,SLOT(getGps()));
    QTimer::singleShot(70000,uploader,SLOT(start()));

    if (GlobalConfigInstance.isConfigured())
    {
        InitRequestResult result;
        result.player_id = GlobalConfigInstance.getPlayerId();
        result.public_key = GlobalConfigInstance.getPublicKey();
        result.status = "success";
        result.session_key = InitRequestResult::generateSessionKey();
        qDebug()<< "loading: plid = " << result.player_id << " public key = " << result.public_key;
        qDebug() << "session key = " << result.session_key;
        playerInitParams = result;
        encryptedSessionKey = encryptSessionKey();
        qDebug() << "encryptedSessionKey = " << encryptedSessionKey;
        GlobalConfigInstance.setSessionKey(result.session_key);
        GlobalConfigInstance.setEncryptedSessionKey(encryptedSessionKey);
        GlobalConfigInstance.setGetPlaylistTimerTime(10000);
        sheduler->restart(TeleDSSheduler::GET_PLAYLIST);
    } else
    {
        qDebug() << "player is not configurated";
        QTimer::singleShot(1000,this,SLOT(initPlayer()));
    }

    downloader = 0;
    rpiPlayer = 0;
    qDebug() << "TELEDS initialization done";
    QTimer::singleShot(120000,this,SLOT(ex()));*/
}

QString TeleDSCore::encryptSessionKey()
{
    qDebug() << "encrypting session key";
    QFile pubKey("pubkey.key");
    pubKey.open(QFile::WriteOnly);
    pubKey.write(playerInitParams.public_key.toLocal8Bit());
    pubKey.close();
    qDebug() << "writing pubkey.key";

    qDebug() << "encrypting...";
    QProcess echoProcess, rsaEncodeProcess, base64Process;
    echoProcess.setStandardOutputProcess(&rsaEncodeProcess);
    //rsaEncodeProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    rsaEncodeProcess.setStandardOutputProcess(&base64Process);
    base64Process.setStandardOutputFile("base64result.txt");
    //base64Process.setProcessChannelMode(QProcess::ForwardedChannels);
    connect (&echoProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));
    //replace with echo command instead of cmd
    echoProcess.start("echo " + playerInitParams.session_key);
    //echoProcess.start("cmd.exe",QStringList("/c echo " + playerInitParams.session_key));
    rsaEncodeProcess.start("openssl rsautl -encrypt -inkey pubkey.key -pubin");
    base64Process.start("openssl base64");

    if(!echoProcess.waitForStarted())
        return "";
    bool retval = false;
    qDebug() << "encrypting successful ";

    while ((retval = base64Process.waitForFinished()));
    QFile base64Encoded("base64result.txt");
    qDebug() << "reading result ";
    base64Encoded.open(QFile::ReadOnly);
    QString result = base64Encoded.readAll();
    result = result.mid(0,result.length()-1).replace("\n","").replace("\r","");

    base64Encoded.close();
    result = QUrl::toPercentEncoding(result);
    qDebug() << result;
    return result;
}

void TeleDSCore::initPlayer()
{
    qDebug() << "Video service initialization";
    videoService->init();
}

void TeleDSCore::processError(QProcess::ProcessError error)
{
    qDebug() << error;
}

void TeleDSCore::initResult(InitRequestResult result)
{
    emit playerIdUpdate(result.player_id);
    qDebug() << "got player id: " + result.player_id;
    playerInitParams = result;
    encryptedSessionKey = encryptSessionKey();
    GlobalConfigInstance.setEncryptedSessionKey(encryptedSessionKey);
    GlobalConfigInstance.setSessionKey(result.session_key);
    qDebug() << "ENC KEY: " << encryptedSessionKey;

    GlobalConfigInstance.setGetPlaylistTimerTime(10000);
    sheduler->restart(TeleDSSheduler::GET_PLAYLIST);

    GlobalConfigInstance.setPlayerId(result.player_id);
    GlobalConfigInstance.setPublicKey(result.public_key);

    fakeInit();
}

void TeleDSCore::playlistResult(PlayerConfig result)
{
    if (result.error == 300)
    {
        qDebug() << "player is not activated \n running fake init";
        fakeInit();
        return;
    }
    if (result.error != 0)
        GlobalStatsInstance.registryPlaylistError();
    QString info = playerInitParams.player_id + " : " + QString::number(result.error) + " [" + result.error_text + "]";
    qDebug() << info;
    emit playerIdUpdate(info);
    if (result.areas.count() > 0)
    {
        qDebug() << "areas found!";
        sheduler->stop(TeleDSSheduler::GET_PLAYLIST);
        setupDownloader(result);
        currentConfig = result;
    }
    else
        GlobalStatsInstance.registryPlaylistError();
    GlobalConfigInstance.setPlayerConfig(result.data);
}

void TeleDSCore::getPlaylistTimerSlot()
{
    qDebug() << "grabbing playlist";
    videoService->getPlaylist(playerInitParams.player_id,encryptedSessionKey);
}

void TeleDSCore::fakeInit()
{
    qDebug() << "fake init called";
    videoService->enablePlayer(playerInitParams.player_id);
    videoService->assignPlaylist(playerInitParams.player_id,10);
}

void TeleDSCore::downloaded()
{
    qDebug() << "downloaded";
    GlobalConfigInstance.setGetPlaylistTimerTime(60000);
    sheduler->restart(TeleDSSheduler::GET_PLAYLIST);
    if (rpiPlayer == NULL)
    {
        qDebug() << "creating RPI Video Player";
        rpiPlayer = new RpiVideoPlayer(currentConfig.areas[0],this);
    }
    else
    {
        rpiPlayer->update(currentConfig);
    }
}

void TeleDSCore::checkCPUStatus()
{
    CPUStatInstance.getInfo();
}

void TeleDSCore::generateReport()
{
    GlobalStats::Report report = GlobalStatsInstance.generateReport();
    DatabaseInstance.createReport(report.downloadCount,report.contentPlayCount,report.contentTotalCount,report.connectionErrorCount,report.playlistErrorCount);
}

void TeleDSCore::generateSysInfo()
{
    GlobalStats::SystemInfo sysInfo = GlobalStatsInstance.generateSystemInfo();
    DatabaseInstance.createSystemInfo(sysInfo.cpu,sysInfo.memory,sysInfo.trafficIn,sysInfo.trafficOut,sysInfo.monitorActive,sysInfo.connectionActive,sysInfo.balance);
}

void TeleDSCore::getResourceCount()
{
    DatabaseInstance.resourceCount();
}

void TeleDSCore::getGps()
{
    DatabaseInstance.createGPS(0.,0.);
}

void TeleDSCore::ex()
{
    qApp->exit();
}

void TeleDSCore::updateCPUStatus(CPUStatWorker::DeviceInfo info)
{
    GlobalStatsInstance.setCpu(info.cpu);
    GlobalStatsInstance.setMemory(info.memory);
    GlobalStatsInstance.setTraffic(info.trafficIn, info.trafficOut);
}

void TeleDSCore::resourceCountUpdate(int count)
{
    qDebug() << "total ITEMs count :" << count;
    GlobalStatsInstance.setContentTotal(count);
}

void TeleDSCore::setupDownloader(PlayerConfig &config)
{
    qDebug() << "starting downloader...";
    if (downloader)
    {
        downloader->disconnect();
        delete downloader;
    }
    downloader = new VideoDownloader(config,this);
    currentConfig = config;
    connect(downloader,SIGNAL(done()),this,SLOT(downloaded()));

    downloader->checkDownload();
    downloader->start();
}


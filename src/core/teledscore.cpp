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
#include "sslencoder.h"
#include "platformdefines.h"
#include "platformspecs.h"


TeleDSCore::TeleDSCore(QObject *parent) : QObject(parent)
{
    qDebug() << "Unique Id: " << PlatformSpecs::getUniqueId();

    DatabaseInstance;
    CPUStatInstance;
    videoService = new VideoService("http://api.teleds.com");


    uploader = new StatisticUploader(videoService,this);
    rpiPlayer = new RpiVideoPlayer(this);
    statsTimer = new QTimer();
    connect(statsTimer,SIGNAL(timeout()),uploader,SLOT(start()));
    statsTimer->start(90000);

    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfig)),this,SLOT(playlistResult(PlayerConfig)));

    connect (&sheduler,SIGNAL(cpuInfo()), this, SLOT(checkCPUStatus()));
    connect (&sheduler,SIGNAL(getPlaylist()), this, SLOT(getPlaylistTimerSlot()));
    connect (&CPUStatInstance,SIGNAL(infoReady(CPUStatWorker::DeviceInfo)),this,SLOT(updateCPUStatus(CPUStatWorker::DeviceInfo)));
    connect (&sheduler, SIGNAL(report()), this, SLOT(generateReport()));
    connect (&sheduler, SIGNAL(sysInfo()), this, SLOT(generateSysInfo()));
    connect (&sheduler, SIGNAL(resourceCounter()), this, SLOT(getResourceCount()));
    connect (&DatabaseInstance,SIGNAL(resourceCount(int)),this,SLOT(resourceCountUpdate(int)));
    connect (&sheduler, SIGNAL(gps()),this,SLOT(getGps()));
    connect (rpiPlayer,SIGNAL(refreshNeeded()),this, SLOT(initPlayer()));


    QTimer::singleShot(70000,uploader,SLOT(start()));

    qDebug() << CONFIG_FOLDER;

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
       // encryptedSessionKey = encryptSessionKey();
        encryptedSessionKey = SSLEncoder::encryptRSA(result.session_key.toLocal8Bit(), result.public_key.toLocal8Bit());
        qDebug() << "encryptedSessionKey = " << encryptedSessionKey;

        GlobalConfigInstance.setSessionKey(result.session_key);
        GlobalConfigInstance.setEncryptedSessionKey(encryptedSessionKey);
        GlobalConfigInstance.setGetPlaylistTimerTime(10000);
        sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
   //     sheduler.stop(TeleDSSheduler::ALL);

      /*  QFile decPlaylist("playlist.dec");
        decPlaylist.open(QFile::ReadOnly);
        QByteArray data = decPlaylist.readAll();
        QByteArray encr = SSLEncoder::encodeAES256(data,false,false);
        QByteArray decoded = SSLEncoder::decodeAES256(encr,false);
        QFile outPlaylist("playlist.dec.enc");
        outPlaylist.open(QFile::WriteOnly);
        outPlaylist.write(encr);
        outPlaylist.flush();
        outPlaylist.close();*/
    } else
    {
        qDebug() << "player is not configurated";
        QTimer::singleShot(1000,this,SLOT(initPlayer()));
    }

    downloader = 0;
    //rpiPlayer = 0;
    qDebug() << "TELEDS initialization done";
}


void TeleDSCore::initPlayer()
{
    qDebug() << "!!! Video service initialization";
    videoService->init();
}


void TeleDSCore::initResult(InitRequestResult result)
{
    emit playerIdUpdate(result.player_id);
    qDebug() << "got player id: " + result.player_id;
    playerInitParams = result;
    encryptedSessionKey = SSLEncoder::encryptRSA(result.session_key.toLocal8Bit(), result.public_key.toLocal8Bit());
    GlobalConfigInstance.setEncryptedSessionKey(encryptedSessionKey);
    GlobalConfigInstance.setSessionKey(result.session_key);
    qDebug() << "ENC KEY: " << encryptedSessionKey;

    GlobalConfigInstance.setGetPlaylistTimerTime(10000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);

    GlobalConfigInstance.setPlayerId(result.player_id);
    GlobalConfigInstance.setPublicKey(result.public_key);

   // fakeInit();
}

void TeleDSCore::playlistResult(PlayerConfig result)
{

    if (result.error == 300 || result.error == 106 || result.error == 301)
    {
        rpiPlayer->invokePlayerActivationRequiredView("http://teleds.tv",GlobalConfigInstance.getPlayerId());
        return;
    }
    if (result.error == 201)
    {
        rpiPlayer->invokeNoItemsView("http://teleds.tv");
        return;
    }

    if (result.error == 300)
    {
        qDebug() << "player is not activated" << endl << "running fake init";
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
        sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
        setupDownloader(result);
        currentConfig = result;
        if (!rpiPlayer->isPlaying() && downloader->itemsToDownloadCount() > 0)
            rpiPlayer->invokeDownloadingView();
    }
    else
    {
        rpiPlayer->invokeNoItemsView("http://teleds.tv");
        GlobalStatsInstance.registryPlaylistError();
    }
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
    qDebug() << "!!!!!!!!downloaded!";
    GlobalConfigInstance.setGetPlaylistTimerTime(30000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
    if (rpiPlayer == NULL)
    {
        qDebug() << "creating RPI Video Player";
        rpiPlayer = new RpiVideoPlayer(currentConfig.areas[0],this);
    }
    else
    {
        rpiPlayer->setConfig(currentConfig.areas.first());
        if (!rpiPlayer->isPlaying())
            rpiPlayer->play();
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
    if (downloader)
        downloader->updateConfig(config);
    else
    {
        downloader = new VideoDownloader(config, this);
        connect(downloader,SIGNAL(done()),this,SLOT(downloaded()));
        connect(downloader,SIGNAL(downloadProgress(double)),rpiPlayer,SLOT(invokeProgress(double)));
        connect(downloader,SIGNAL(totalDownloadProgress(double,QString)),rpiPlayer,SLOT(invokeFileProgress(double,QString)));
        connect(downloader,SIGNAL(done()),rpiPlayer,SLOT(invokeDownloadDone()));
        connect(downloader,SIGNAL(downloadProgressSingle(double,QString)),rpiPlayer,SLOT(invokeSimpleProgress(double,QString)));
    }

    currentConfig = config;
    downloader->checkDownload();
    downloader->start();
}


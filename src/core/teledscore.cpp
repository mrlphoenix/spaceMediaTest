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
    qDebug() << "Unique Id: " << PlatformSpecific::getUniqueId();

    DatabaseInstance;
    CPUStatInstance;
    videoService = new VideoService("http://api.teleds.com");


    uploader = new StatisticUploader(videoService,this);
    rpiPlayer = new TeleDSPlayer(this);
    statsTimer = new QTimer();
    connect(statsTimer,SIGNAL(timeout()),uploader,SLOT(start()));
    statsTimer->start(60000);

    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfig)),this,SLOT(playlistResult(PlayerConfig)));
    connect(videoService,SIGNAL(getPlayerSettings(SettingsRequestResult)),this,SLOT(playerSettingsResult(SettingsRequestResult)));
    connect(videoService,SIGNAL(getVirtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)),this,SLOT(virtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)));
    connect(videoService,SIGNAL(getPlayerAreasResult(PlayerConfigNew)),this,SLOT(virtualScreensResult(PlayerConfigNew)));

    connect (&sheduler,SIGNAL(getPlaylist()), this, SLOT(getPlaylistTimerSlot()));
    connect (rpiPlayer, SIGNAL(refreshNeeded()), this, SLOT(getPlaylistTimerSlot()));


    QTimer::singleShot(70000,uploader,SLOT(start()));

    qDebug() << CONFIG_FOLDER;

    if (GlobalConfigInstance.isConfigured())
    {
        InitRequestResult result;
        result.token = GlobalConfigInstance.getToken();
        result.status = "success";
        qDebug()<< "loading: token = " << result.token;
        playerInitParams = result;

        GlobalConfigInstance.setGetPlaylistTimerTime(10000);
        sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
        QTimer::singleShot(1000, this, SLOT(getPlaylistTimerSlot()));
     //   sheduler.stop(TeleDSSheduler::ALL);
    } else
    {
        qDebug() << "player is not configurated";
        QTimer::singleShot(1000,this,SLOT(initPlayer()));
    }

    downloader = 0;
   // rpiPlayer->setBrightness(0.5);
    qDebug() << "TELEDS initialization done";
}


void TeleDSCore::initPlayer()
{
    qDebug() << "!!! Video service initialization";
   // videoService->init();
    videoService->advancedInit();
}


void TeleDSCore::initResult(InitRequestResult result)
{
    emit playerIdUpdate(result.code);
    playerInitParams = result;

    GlobalConfigInstance.setGetPlaylistTimerTime(10000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);

    GlobalConfigInstance.setToken(result.token);
    GlobalConfigInstance.setActivationCode(result.code);
    rpiPlayer->invokePlayerActivationRequiredView("http://teleds.com",result.code);
    QTimer::singleShot(1000,this,SLOT(getPlaylistTimerSlot()));

   // fakeInit();
}

void TeleDSCore::playlistResult(PlayerConfig result)
{

    if (result.error == 300 || result.error == 106 || result.error == 301)
    {
        rpiPlayer->invokePlayerActivationRequiredView("http://teleds.tv",GlobalConfigInstance.getActivationCode());
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
    if (result.error == -1)
    {
        GlobalStatsInstance.setConnectionState(false);
        qDebug() << "No connection to the server";
        QJsonParseError error;
        QJsonDocument configJsonDoc = QJsonDocument::fromJson(GlobalConfigInstance.getPlayerConfig().toLocal8Bit(), &error);
        if (error.error)
        {
            qDebug() << "unparsable config playlist";
            return;
        }
        QJsonObject root = configJsonDoc.object();
        PlayerConfig config = PlayerConfig::fromJson(root);
        result = config;
    }
    else
        GlobalStatsInstance.setConnectionState(true);
    if (result.error != 0)
        GlobalStatsInstance.registryPlaylistError();

    if (result.areas.count() > 0)
    {
        qDebug() << "areas found!";
        sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
        setupDownloader(result);
        currentConfig = result;
    }
    else
    {
        rpiPlayer->invokeNoItemsView("http://teleds.tv");
        GlobalStatsInstance.registryPlaylistError();
    }
    GlobalConfigInstance.setPlayerConfig(result.data);
}

void TeleDSCore::playerSettingsResult(SettingsRequestResult result)
{
    qDebug() <<"Core: player settings result error content: " << result.error;
    GlobalConfigInstance.setVideoQuality(result.video_quality);
    if (result.error_id == 401)
    {
        if (result.error != "")
            GlobalConfigInstance.setActivationCode(result.error);
        rpiPlayer->invokePlayerActivationRequiredView("http://teleds.tv",GlobalConfigInstance.getActivationCode());
    }
    if (result.error_id == 403)
    {
        initPlayer();
    }
    else
    {
        if (result.error_id == 0)
        {
            videoService->getPlayerAreas();
          //  GlobalConfigInstance.setAutoBrightness(result.autobright);
            GlobalConfigInstance.setAutoBrightness(true);
            GlobalConfigInstance.setMinBrightness(result.min_bright);
            GlobalConfigInstance.setMaxBrightness(result.max_bright);
            GlobalConfigInstance.setStatsInverval(result.stats_interval);
            if (result.gps_lat != 0.0 && result.gps_long != 0.0)
                GlobalStatsInstance.setGps(result.gps_lat, result.gps_long);
            qDebug() << "STATS INTERVAL: " << result.stats_interval;
            //statsTimer->start(result.stats_interval*1000);
        }
    }
}

void TeleDSCore::virtualScreensResult(PlayerConfigNew result)
{
    qDebug() <<"Core: virtual screens " << result.screens.count();
    currentConfigNew = result;
    videoService->getPlaylist();
}

void TeleDSCore::virtualScreenPlaylistResult(QHash<QString, PlaylistAPIResult> result)
{

    int count = 0;
    if (result.count() == 0)
    {
        rpiPlayer->invokeNoItemsView("http://teleds.tv");
        return;
    }
    foreach (const QString & k, result.keys())
    {
        count+= result[k].items.count();
    }
     qDebug() << "Core: Playlist " << count;

    foreach (const QString &s, result.keys())
    {
        if (currentConfigNew.screens.contains(s))
            currentConfigNew.screens[s].playlist = result[s];
    }

    setupDownloader(this->currentConfigNew);
    //setup downloader
    //

}

void TeleDSCore::getPlaylistTimerSlot()
{
    qDebug() << "grabbing areas";
  //  videoService->getPlaylist(playerInitParams.token,encryptedSessionKey);
    videoService->getPlayerSettings();
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
}

void TeleDSCore::fakeInit()
{
    qDebug() << "fake init called";
  //  videoService->enablePlayer(playerInitParams.);
  //  videoService->assignPlaylist(playerInitParams.player_id,10);
}

void TeleDSCore::downloaded()
{
    qDebug() << "!!!!!!!!downloaded!";
    GlobalConfigInstance.setGetPlaylistTimerTime(30000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
    if (rpiPlayer == NULL)
    {
        qDebug() << "creating RPI Video Player";
        rpiPlayer = new TeleDSPlayer(currentConfig.areas[0],this);
    }
    else
    {
        if (currentConfig.areas.count())
        {
            rpiPlayer->setConfig(currentConfig.areas.first());
            if (!rpiPlayer->isPlaying())
            {
                rpiPlayer->play();
                QTimer::singleShot(3000,rpiPlayer, SLOT(invokeEnablePreloading()));
                //rpiPlayer->invokeEnablePreloading();
            }
        }
        else if (currentConfigNew.screens.count())
        {
            foreach (const PlayerConfigNew::VirtualScreen &v, currentConfigNew.screens)
                if (v.type != "audio")
                {
                    if (v.playlist.items.count() == 0)
                        rpiPlayer->invokeNoItemsView("http://teleds.tv");
                    else
                    {
                        rpiPlayer->invokeDownloadDone();
                        rpiPlayer->setConfig(v);
                        if (!rpiPlayer->isPlaying())
                        {
                            rpiPlayer->play();
                            QTimer::singleShot(1000, rpiPlayer, SLOT(invokeEnablePreloading()));
                        }
                        break;
                    }
                }
           /* rpiPlayer->setConfig(currentConfigNew.screens[currentConfigNew.screens.keys().at(0)]);
            if (!rpiPlayer->isPlaying())
            {
                rpiPlayer->play();
                QTimer::singleShot(12000, rpiPlayer, SLOT(invokeEnablePreloading()));
            }*/
        }
    }
}



void TeleDSCore::checkCPUStatus()
{
    CPUStatInstance.getInfo();
}

void TeleDSCore::getResourceCount()
{
    DatabaseInstance.resourceCount();
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

void TeleDSCore::needToDownloadResult(int count)
{
    if (!rpiPlayer->isPlaying() && count > 0)
        rpiPlayer->invokeDownloadingView();
}

void TeleDSCore::setupDownloader(PlayerConfig &config)
{
    if (downloader)
        downloader->updateConfig(config);
    else
    {
        downloader = new VideoDownloader(config, this);
        downloader->start();
        connect(downloader,SIGNAL(done()),this,SLOT(downloaded()));
        //connect(downloader,SIGNAL(downloadProgress(double)),rpiPlayer,SLOT(invokeProgress(double)));
        //connect(downloader,SIGNAL(totalDownloadProgress(double,QString)),rpiPlayer,SLOT(invokeFileProgress(double,QString)));
        connect(downloader,SIGNAL(done()),rpiPlayer,SLOT(invokeDownloadDone()));
        connect(downloader,SIGNAL(downloadProgressSingle(double,QString)),rpiPlayer,SLOT(invokeSimpleProgress(double,QString)));
        connect(downloader, SIGNAL(donwloadConfigResult(int)),this, SLOT(needToDownloadResult(int)));
    }
    currentConfig = config;
    downloader->runDownload();
    sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
    downloader->checkDownload();
    downloader->startDownload();
}

void TeleDSCore::setupDownloader(PlayerConfigNew &newConfig)
{
    qDebug() <<"Core: setup Downloader";
    if (downloader)
        downloader->updateConfig(newConfig);
    else
    {
        downloader = new VideoDownloader(newConfig, this);
        downloader->start();
        connect(downloader,SIGNAL(done()),this,SLOT(downloaded()));
        //connect(downloader,SIGNAL(downloadProgress(double)),rpiPlayer,SLOT(invokeProgress(double)));
        //connect(downloader,SIGNAL(totalDownloadProgress(double,QString)),rpiPlayer,SLOT(invokeFileProgress(double,QString)));
        connect(downloader,SIGNAL(done()),rpiPlayer,SLOT(invokeDownloadDone()));
        connect(downloader,SIGNAL(downloadProgressSingle(double,QString)),rpiPlayer,SLOT(invokeSimpleProgress(double,QString)));
        connect(downloader, SIGNAL(donwloadConfigResult(int)),this, SLOT(needToDownloadResult(int)));
    }
    currentConfigNew = newConfig;
    downloader->runDownloadNew();
}




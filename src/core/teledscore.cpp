#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QUrl>
#include <QDir>


#include "teledscore.h"
#include "globalconfig.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "sslencoder.h"
#include "platformdefines.h"
#include "platformspecs.h"

TeleDSCore::TeleDSCore(QObject *parent) : QObject(parent)
{
    PlatformSpecific::init();
    qDebug() << "Unique Id: " << PlatformSpecific::getUniqueId();

    //blinking GPIO
    QTimer * blinkTimer = new QTimer();
    connect(blinkTimer, SIGNAL(timeout()),this,SLOT(blinkGPIO()));
    blinkTimer->start(1000);


    DatabaseInstance;
    CPUStatInstance;
    videoService = new VideoService("http://api.teleds.com");
    QDir().mkpath("/sdcard/download/teleds");


    uploader = new StatisticUploader(videoService,this);
    teledsPlayer = new TeleDSPlayer(this);
    statsTimer = new QTimer();
    connect(statsTimer,SIGNAL(timeout()),uploader,SLOT(start()));
    statsTimer->start(60000);

    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfig)),this,SLOT(playlistResult(PlayerConfig)));
    connect(videoService,SIGNAL(getPlayerSettings(SettingsRequestResult)),this,SLOT(playerSettingsResult(SettingsRequestResult)));
    connect(videoService,SIGNAL(getVirtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)),this,SLOT(virtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)));
    connect(videoService,SIGNAL(getPlayerAreasResult(PlayerConfigNew)),this,SLOT(virtualScreensResult(PlayerConfigNew)));

    connect (&sheduler,SIGNAL(getPlaylist()), this, SLOT(getPlaylistTimerSlot()));
    connect (teledsPlayer, SIGNAL(refreshNeeded()), this, SLOT(getPlaylistTimerSlot()));
    GlobalConfigInstance.setGetPlaylistTimerTime(10000);

    qDebug() << CONFIG_FOLDER;

    //if player is already configurated - we load config from file
    //else  - make request for initialization
    if (GlobalConfigInstance.isConfigured())
    {
        InitRequestResult result;
        result.token = GlobalConfigInstance.getToken();
        result.status = "success";
        qDebug()<< "loading: token = " << result.token;
        playerInitParams = result;

        //until we load actual playlist - update timer every 10 sec
        GlobalConfigInstance.setGetPlaylistTimerTime(10000);
        sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
        //load playlist
        QTimer::singleShot(1000, this, SLOT(getPlaylistTimerSlot()));
    } else
    {
        qDebug() << "player is not configurated";
        QTimer::singleShot(1000,this,SLOT(initPlayer()));
    }

    downloader = 0;
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
    //save init params
    playerInitParams = result;

    //run getplaylist timer
    //player will try to load player every 10 sec
    GlobalConfigInstance.setGetPlaylistTimerTime(10000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);

    //save token and code in config file
    GlobalConfigInstance.setToken(result.token);
    GlobalConfigInstance.setActivationCode(result.code);

    //after initialization we show actitivation required action with given code
    teledsPlayer->invokePlayerActivationRequiredView("http://teleds.com",result.code);

    //start get playlist
    QTimer::singleShot(1000,this,SLOT(getPlaylistTimerSlot()));
}

void TeleDSCore::playlistResult(PlayerConfig result)
{
    //this slot is called when we get playlist result from backend

    //300/106/301 errors - player device need to be activated
    if (result.error == 300 || result.error == 106 || result.error == 301)
    {
        teledsPlayer->invokePlayerActivationRequiredView("http://teleds.com",GlobalConfigInstance.getActivationCode());
        return;
    }
    //201 error - playlist is empty; nothing to play
    if (result.error == 201)
    {
        teledsPlayer->invokeNoItemsView("http://teleds.com");
        return;
    }

    //if error is unknown - seems like we have problems with internet connection
    //load config from file
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

    //for some other errors - registry playlist error
    if (result.error != 0)
        GlobalStatsInstance.registryPlaylistError();

    //if config contains at least one area - prepare download and turn off get playlist timer
    //else we show "nothing to play" activity
    if (result.areas.count() > 0)
    {
        qDebug() << "areas found!";
        sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
        setupDownloader(result);
        currentConfig = result;
    }
    else
    {
        teledsPlayer->invokeNoItemsView("http://teleds.com");
        GlobalStatsInstance.registryPlaylistError();
    }
    //saving config to file
    GlobalConfigInstance.setPlayerConfig(result.data);
}

void TeleDSCore::playerSettingsResult(SettingsRequestResult result)
{
    //this method is called when we try to get player settings
    GlobalConfigInstance.setVideoQuality(result.video_quality);

    //if backend responsed with 401 - it means player need to be reactivated
    if (result.error_id == 401)
    {
        if (result.error != "")
            GlobalConfigInstance.setActivationCode(result.error);
        teledsPlayer->invokePlayerActivationRequiredView("http://teleds.com",GlobalConfigInstance.getActivationCode());
    }
    //403: player is not configurated - requesting initialization
    if (result.error_id == 403)
    {
        initPlayer();
    }
    else
    {
        //if no errors
        if (result.error_id == 0)
        {
            //load all player areas information
            videoService->getPlayerAreas();

            //setting up autobrightness setup
            GlobalConfigInstance.setAutoBrightness(result.autobright);
          //  GlobalConfigInstance.setAutoBrightness(true);
            GlobalConfigInstance.setMinBrightness(result.min_bright);
            GlobalConfigInstance.setMaxBrightness(result.max_bright);
            GlobalConfigInstance.setStatsInverval(result.stats_interval);

            //if static gps is given - saving it in stats
            if (result.gps_lat != 0.0 && result.gps_long != 0.0)
                GlobalStatsInstance.setGps(result.gps_lat, result.gps_long);
            qDebug() << "STATS INTERVAL: " << result.stats_interval;
            //statsTimer->start(result.stats_interval*1000);
        }
    }
}

void TeleDSCore::virtualScreensResult(PlayerConfigNew result)
{
    //after we load virtual screens list - load playlists for every screen
    qDebug() <<"Core: virtual screens " << result.screens.count();
    currentConfigNew = result;
    videoService->getPlaylist();
}

void TeleDSCore::virtualScreenPlaylistResult(QHash<QString, PlaylistAPIResult> result)
{
    //this method is called after we get playlists for virtual screens
    int count = 0;
    //if no items in playlist - show "nothing to play activity"
    if (result.count() == 0)
    {
        teledsPlayer->invokeNoItemsView("http://teleds.com");
        return;
    }
    //counting items in every playlist
    foreach (const QString & k, result.keys())
    {
        count+= result[k].items.count();
    }
     qDebug() << "Core: Playlist " << count;

     //setting up playlist for every virtual screen
    foreach (const QString &s, result.keys())
    {
        if (currentConfigNew.screens.contains(s))
            currentConfigNew.screens[s].playlist = result[s];
    }

    //prepare downloader
    setupDownloader(this->currentConfigNew);
}

void TeleDSCore::getPlaylistTimerSlot()
{
    qDebug() << "grabbing areas";
    //first - we load settings and restart playlist timer
    videoService->getPlayerSettings();
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
}

void TeleDSCore::downloaded()
{
    //this slot is called when all items got downloaded

    qDebug() << "!!!!!!!!downloaded!";

    //after we download items - update playlists every 30 secs
    GlobalConfigInstance.setGetPlaylistTimerTime(30000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
    //initialization of teledsPlayer
    if (teledsPlayer == NULL)
    {
        qDebug() << "creating RPI Video Player";
        teledsPlayer = new TeleDSPlayer(currentConfig.areas[0],this);
    }
    else
    {
        //currently only one area is supported, so we display first one
        if (currentConfig.areas.count())
        {
            teledsPlayer->setConfig(currentConfig.areas.first());
            //if player is inactive - start player and preload next item
            if (!teledsPlayer->isPlaying())
            {
                teledsPlayer->play();
                QTimer::singleShot(3000,teledsPlayer, SLOT(invokeEnablePreloading()));
            }
        }
        else if (currentConfigNew.screens.count())
        {
            //skip "audio" area - not supported yet
            foreach (const PlayerConfigNew::VirtualScreen &v, currentConfigNew.screens)
                if (v.type != "audio")
                {
                    if (v.playlist.items.count() == 0)
                        teledsPlayer->invokeNoItemsView("http://teleds.com");
                    else
                    {
                        teledsPlayer->invokeDownloadDone();
                        teledsPlayer->setConfig(v);
                        if (!teledsPlayer->isPlaying())
                        {
                            teledsPlayer->play();
                            QTimer::singleShot(1000, teledsPlayer, SLOT(invokeEnablePreloading()));
                        }
                        break;
                    }
                }
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
    if (!teledsPlayer->isPlaying() && count > 0)
        teledsPlayer->invokeDownloadingView();
}

void TeleDSCore::blinkGPIO()
{
    static int value = 0;
    if (value)
    {
        qDebug() << "RIGHT BLINK";
        PlatformSpecific::turnOnFirst();
        PlatformSpecific::turnOffSecond();
    }
    else
    {
        qDebug() << "LEFT BLINK";
        PlatformSpecific::turnOffFirst();
        PlatformSpecific::turnOnSecond();
    }
    value = 1 - value;
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
        connect(downloader,SIGNAL(done()),teledsPlayer,SLOT(invokeDownloadDone()));
        connect(downloader,SIGNAL(downloadProgressSingle(double,QString)),teledsPlayer,SLOT(invokeSimpleProgress(double,QString)));
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
    //if downloader already initializated - update items
    //else resetup it
    if (downloader)
        downloader->updateConfig(newConfig);
    else
    {
        downloader = new VideoDownloader(newConfig, this);

        //this start() method is from QThread - must be called once to prepare Downloader Worker.
        downloader->start();
        connect(downloader,SIGNAL(done()),this,SLOT(downloaded()));
        connect(downloader,SIGNAL(done()),teledsPlayer,SLOT(invokeDownloadDone()));
        connect(downloader,SIGNAL(downloadProgressSingle(double,QString)),teledsPlayer,SLOT(invokeSimpleProgress(double,QString)));
        connect(downloader, SIGNAL(donwloadConfigResult(int)),this, SLOT(needToDownloadResult(int)));
    }
    currentConfigNew = newConfig;
    //starting downloading and stop getPlaylist timer
    downloader->runDownloadNew();
    sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
}

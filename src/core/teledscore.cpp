#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QUrl>
#include <QDir>
#include <QTime>
#include <QTimeZone>
#include <QScreen>

#include "teledscore.h"
#include "globalconfig.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "sslencoder.h"
#include "platformdefines.h"
#include "platformspecific.h"

TeleDSCore::TeleDSCore(QObject *parent) : QObject(parent)
{
    DatabaseInstance;
    CPUStatInstance;
    PlatformSpecificService;

    //GPIO Releys
    QTimer * releyTimer = new QTimer();
    connect(releyTimer, SIGNAL(timeout()),this,SLOT(checkReleyTime()));
    releyTimer->start(60000);

    //Android battery shutdown-conditions checker
    QTimer * shutdownTimer = new QTimer();
    connect(shutdownTimer, SIGNAL(timeout()), this, SLOT(checkForAutomaticShutdown()));
    shutdownTimer->start(60000);

   // batteryStatus.setActive(true);
   // batteryStatus.setConfig(98, -1);


    videoService = new VideoService("http://api.teleds.com");
    QDir().mkpath("/sdcard/download/teleds");

    uploader = new StatisticUploader(videoService,this);
    teledsPlayer = new TeleDSPlayer(this);
    teledsPlayer->show();
    statsTimer = new QTimer();
    connect(statsTimer,SIGNAL(timeout()),uploader,SLOT(start()));
    statsTimer->start(60000);

    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfig)),this,SLOT(playlistResult(PlayerConfig)));
    connect(videoService,SIGNAL(getPlayerSettings(SettingsRequestResult)),this,SLOT(playerSettingsResult(SettingsRequestResult)));
    connect(videoService,SIGNAL(getVirtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)),this,SLOT(virtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)));
    connect(videoService,SIGNAL(getPlayerAreasResult(PlayerConfig)),this,SLOT(virtualScreensResult(PlayerConfig)));

    connect (&sheduler,SIGNAL(getPlaylist()), this, SLOT(getPlaylistTimerSlot()));
    connect (teledsPlayer, SIGNAL(refreshNeeded()), this, SLOT(getPlaylistTimerSlot()));

    connect (&PlatformSpecificService,SIGNAL(hardwareInfoReady(Platform::HardwareInfo)),this,SLOT(hardwareInfoReady(Platform::HardwareInfo)));
    connect (&PlatformSpecificService,SIGNAL(batteryInfoReady(Platform::BatteryInfo)),this,SLOT(automaticShutdownBatteryInfoReady(Platform::BatteryInfo)));
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

        //if settings is stored in config and if brand is active
        SettingsRequestResult settings = SettingsRequestResult::fromJson(GlobalConfigInstance.getSettings());
        if (settings.brand_active)
        {
            auto tdsPlayer = this->teledsPlayer;
            QTimer::singleShot(1000, [settings, tdsPlayer]() mutable {
                tdsPlayer->invokeSetTheme(settings.brand_background, settings.brand_logo, settings.brand_color_1, settings.brand_color_2, "#d7d7d7");
            });
        }
        else
        {
            teledsPlayer->show();
        }

        //until we load actual playlist - update timer every 10 sec
        GlobalConfigInstance.setGetPlaylistTimerTime(10000);
        sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
        //load playlist
        QTimer::singleShot(1000, this, SLOT(getPlaylistTimerSlot()));
    } else
    {
        qDebug() << "player is not configurated";
        QTimer::singleShot(1000,this,SLOT(initPlayer()));
        teledsPlayer->show();
    }

    downloader = 0;
    qDebug() << "TELEDS initialization done";
}


void TeleDSCore::initPlayer()
{
    qDebug() << "!!! Video service initialization";
    PlatformSpecificService.generateHardwareInfo();
    //videoService->advancedInit();
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

void TeleDSCore::hardwareInfoReady(Platform::HardwareInfo info)
{
    QJsonObject jsonBody;
    jsonBody["timestamp"] = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
    jsonBody["timezone"] = QString(QTimeZone::systemTimeZoneId());
    jsonBody["uniqid"] = info.uid;
    jsonBody["screen_width"] = qApp->screens().first()->geometry().width();
    jsonBody["screen_height"] = qApp->screens().first()->geometry().height();
    jsonBody["device_vendor"] = info.vendor;
    jsonBody["device_model"] = info.deviceModel;
    jsonBody["cpumodel"] = info.cpuName;
    jsonBody["os"] = info.osName;
    jsonBody["os_version"] = info.osVersion;

    jsonBody["gps_lat"] = GlobalStatsInstance.getLatitude();
    jsonBody["gps_long"] = GlobalStatsInstance.getLongitude();

    QJsonDocument doc(jsonBody);
    QByteArray jsonData = doc.toJson();
    videoService->advancedInit(jsonData);
}

void TeleDSCore::checkForAutomaticShutdown()
{
    qDebug() << "TeleDSCore::checkForAutomaticShutdown";
    PlatformSpecificService.generateBatteryInfo();
}

void TeleDSCore::automaticShutdownBatteryInfoReady(Platform::BatteryInfo info)
{
    if (batteryStatus.checkIfNeedToShutDown(info))
    {
        QProcess shutdownProcess;
        shutdownProcess.startDetached("reboot -p");
    }
}

void TeleDSCore::playlistResult(PlayerConfig result)
{
    //this slot is called when we get playlist result from backend

    //300/106/301 errors - player device need to be activated
    if (result.error_id == 300 || result.error_id == 106 || result.error_id == 301)
    {
        teledsPlayer->invokePlayerActivationRequiredView("http://teleds.com",GlobalConfigInstance.getActivationCode());
        return;
    }
    //201 error - playlist is empty; nothing to play
    if (result.error_id == 201)
    {
        teledsPlayer->invokeNoItemsView("http://teleds.com");
        return;
    }

    //if error is unknown - seems like we have problems with internet connection
    //load config from file
    if (result.error_id == -1)
    {
        GlobalStatsInstance.setConnectionState(false);
        qDebug() << "No connection to the server";
        //TODO: reworked with new type
       /* QJsonParseError error;
        QJsonDocument configJsonDoc = QJsonDocument::fromJson(GlobalConfigInstance.getPlayerConfig().toLocal8Bit(), &error);
        if (error.error)
        {
            qDebug() << "unparsable config playlist";
            return;
        }
        QJsonObject root = configJsonDoc.object();
        PlayerConfig config = PlayerConfig::fromJson(root);
        result = config;*/
    }
    else
        GlobalStatsInstance.setConnectionState(true);

    //for some other errors - registry playlist error
    if (result.error != 0)
        GlobalStatsInstance.registryPlaylistError();

    //if config contains at least one area - prepare download and turn off get playlist timer
    //else we show "nothing to play" activity
    if (result.screens.count() > 0)
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
   // GlobalConfigInstance.setPlayerConfig(result.data);
}

void TeleDSCore::playerSettingsResult(SettingsRequestResult result)
{
    qDebug() << "playerSettingsResult";
    //this method is called when we try to get player settings

    GlobalConfigInstance.setVideoQuality(result.video_quality);

    //saving time targeting reley config
    GlobalConfigInstance.setReleyEnabled(result.reley_1_enabled, result.reley_2_enabled);
    GlobalConfigInstance.setReleyConfig(result.time_targeting_relay_1, result.time_targeting_relay_2);

    //if backend responsed with 401 - it means player need to be reactivated
    if (result.error_id == 401)
    {
        qDebug() << "backend responsed with 401 - it means player need to be reactivated";
        if (result.error != "")
            GlobalConfigInstance.setActivationCode(result.error);
        if (teledsPlayer->isPlaying())
            teledsPlayer->stopPlaying();
        teledsPlayer->invokePlayerActivationRequiredView("http://teleds.com",GlobalConfigInstance.getActivationCode());
    }
    //403: player is not configurated - requesting initialization
    if (result.error_id == 403)
    {
        if (teledsPlayer->isPlaying())
            teledsPlayer->stopPlaying();
        qDebug() << "403: player is not configurated - requesting initialization";
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
            if (result.stats_interval >= 60000)
                statsTimer->start(result.stats_interval);

            if (result.brand_active)
            {
                auto tdsPlayer = this->teledsPlayer;
                QTimer::singleShot(1000, [result, tdsPlayer]() mutable {
                    tdsPlayer->invokeSetTheme(result.brand_background, result.brand_logo, result.brand_color_1, result.brand_color_2, "#d7d7d7");
                });
            }

            //should check in settings insead
            batteryStatus.setActive(true);
            batteryStatus.setConfig(30, 0);
        }
    }
}

void TeleDSCore::virtualScreensResult(PlayerConfig result)
{
    //after we load virtual screens list - load playlists for every screen
    qDebug() <<"Core: virtual screens " << result.screens.count();
    currentConfig = result;
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

     //setting up playlist for every virtual screen0
    foreach (const QString &s, result.keys())
    {
        if (currentConfig.screens.contains(s))
            currentConfig.screens[s].playlist = result[s];
    }
    //prepare downloader
    setupDownloader(this->currentConfig);
}

void TeleDSCore::getPlaylistTimerSlot()
{
    qDebug() << "getPlaylistTimerSlot";
    qDebug() << "grabbing areas";
    //first - we load settings and restart playlist timer
    videoService->getPlayerSettings();
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
}

void TeleDSCore::downloaded()
{
    //this slot is called when all items got downloaded

  /*  QProcess quitProcess(this);
    quitProcess.start("su", QString("-c|reboot -p").split("|"));
    if (quitProcess.waitForStarted(-1)){
    qDebug() << "quit started";

    quitProcess.waitForFinished();
    PlatformSpecificService.writeToFile(quitProcess.readAll(),"/sdcard/download/teleds/quitproc.txt");
    qDebug() << quitProcess.error();
    qDebug() << quitProcess.errorString();
    }
    else
    {
        qDebug() << "not started";
        qDebug() << quitProcess.error();
        qDebug() << quitProcess.errorString();
    }*/
    qDebug() << "TeleDSCore::downloaded";

    //after we download items - update playlists every 30 secs
    GlobalConfigInstance.setGetPlaylistTimerTime(30000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);

    //initialization of teledsPlayer
    //currently only one area is supported, so we display first one
    if (currentConfig.screens.count())
    {
        //skip "audio" area - not supported yet
        foreach (const PlayerConfig::VirtualScreen &v, currentConfig.screens)
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
                        QTimer::singleShot(2000, teledsPlayer, SLOT(playNext()));
                        //QTimer::singleShot(1000, teledsPlayer, SLOT(invokeEnablePreloading()));
                    }
                    break;
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

void TeleDSCore::checkReleyTime()
{
    qDebug() << "TeleDSCore::checkReleyTime";

    if (GlobalConfigInstance.getReleyEnabled(true))
    {
        qDebug() << "TeleDSCore::checkReleyTime -> first reley is enabled";
        if (GlobalConfigInstance.getFirstReleyStatus())
        {
            qDebug() << "TeleDSCore::checkReleyTime -> turn on first reley";
            PlatformSpecificService.turnOnFirstReley();
        }
        else
        {
            qDebug() << "TeleDSCore::checkReleyTime -> turn off first reley";
            PlatformSpecificService.turnOffFirstReley();
        }
    }
    if (GlobalConfigInstance.getReleyEnabled(false))
    {
        qDebug() << "TeleDSCore::checkReleyTime -> second reley is enabled";
        if (GlobalConfigInstance.getSecondReleyStatus())
        {
            qDebug() << "TeleDSCore::checkReleyTime -> turn on second reley";
            PlatformSpecificService.turnOnSecondReley();
        }
        else
        {
            qDebug() << "TeleDSCore::checkReleyTime -> turn off second reley";
            PlatformSpecificService.turnOffSecondReley();
        }
    }
}

void TeleDSCore::setupDownloader(PlayerConfig &newConfig)
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
    currentConfig = newConfig;
    //starting downloading and stop getPlaylist timer
    sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
    downloader->runDownloadNew();
}

void BatteryStatus::setConfig(int minCapacityLevel, int maxTimeWithoutPower)
{
    this->minCapacityLevel = minCapacityLevel;
    this->maxTimeWithoutPower = maxTimeWithoutPower;
}

void BatteryStatus::setActive(bool isActive)
{
    this->isActive = isActive;
    if (!isActive)
    {
        QDateTime invalidDate;
        lastTimeChecked = invalidDate;
        inactiveTime = 0;
    }
}

bool BatteryStatus::checkIfNeedToShutDown(Platform::BatteryInfo status)
{
    if (!isActive)
        return false;
    if (!lastTimeChecked.isValid())
    {
        lastTimeChecked = QDateTime::currentDateTime();
        this->status = status;
        return false;
    }
    else
    {
        this->status = status;
        QDateTime currentTime = QDateTime::currentDateTime();
        if (status.isCharging)
            inactiveTime = 0;
        else
            inactiveTime+= lastTimeChecked.secsTo(currentTime);
        lastTimeChecked = currentTime;
        if (status.value < minCapacityLevel && !status.isCharging)
            return true;
        if (maxTimeWithoutPower != -1 && inactiveTime >= maxTimeWithoutPower)
            return true;
    }
    return false;
}

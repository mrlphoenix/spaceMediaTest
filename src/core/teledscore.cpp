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
#include <QSslSocket>

#include "teledscore.h"
#include "globalconfig.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "sslencoder.h"
#include "platformdefines.h"
#include "platformspecific.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "statictext.h"

TeleDSCore::TeleDSCore(QObject *parent) : QObject(parent)
{
    DatabaseInstance;
    PlatformSpecificService;
    StaticTextService;
    StaticTextService.init();
    qRegisterMetaType< ThemeDesc >("ThemeDesc");
    qDebug() << "Working in " << QDir().currentPath();

    qDebug() << "SSL" << QSslSocket::supportsSsl();
    //GPIO Releys
    QTimer * releyTimer = new QTimer();
    connect(releyTimer, SIGNAL(timeout()),this,SLOT(checkReleyTime()));
    releyTimer->start(60000);

    //Android battery shutdown-conditions checker
    QTimer * shutdownTimer = new QTimer();
    connect(shutdownTimer, SIGNAL(timeout()), this, SLOT(checkForAutomaticShutdown()));
    shutdownTimer->start(60000);

    videoService = new VideoService("http://api.teleds.com");

    PlatformSpecificService.install();

    uploader = new StatisticUploader(videoService,this);
    teledsPlayer = new TeleDSPlayer(this);
    skinManager = new SkinManager(this);
    //teledsPlayer->show();               //
    shouldShowPlayer = true;
    statsTimer = new QTimer();
    connect(statsTimer,SIGNAL(timeout()),uploader,SLOT(start()));
    statsTimer->start(120000);

    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlayerSettings(SettingsRequestResult)),this,SLOT(playerSettingsResult(SettingsRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfigAPI)),this,SLOT(playlistResult(PlayerConfigAPI)));

    connect (&sheduler,SIGNAL(getPlaylist()), this, SLOT(getPlaylistTimerSlot()));
    connect (teledsPlayer, SIGNAL(refreshNeeded()), this, SLOT(getPlaylistTimerSlot()));

    connect (&PlatformSpecificService,SIGNAL(hardwareInfoReady(Platform::HardwareInfo)),this,SLOT(hardwareInfoReady(Platform::HardwareInfo)));
    connect (&PlatformSpecificService,SIGNAL(batteryInfoReady(Platform::BatteryInfo)),this,SLOT(automaticShutdownBatteryInfoReady(Platform::BatteryInfo)));

    //connect (skinManager, SIGNAL(skinReady(ThemeDesc)), this, SLOT(onThemeReady(ThemeDesc)));
    GlobalConfigInstance.setGetPlaylistTimerTime(10000);

    qDebug() << "Loading config from " << CONFIG_FOLDER;

    //if player is already configurated - we load config from file
    //else  - make request for initialization
    if (GlobalConfigInstance.isConfigured())
    {
        qDebug() << "TeleDSCore::loading config";
        InitRequestResult result;
        result.token = GlobalConfigInstance.getToken();
        result.status = "success";
        qDebug()<< "loading: token = " << result.token;
        playerInitParams = result;

        //if settings is stored in config and if brand is active
        SettingsRequestResult settings = SettingsRequestResult::fromJson(GlobalConfigInstance.getSettings());

      /*  if (settings.brand_active)
        {
            qDebug() << "TeleDSCore::init brand is active and stored in config file. loading";
            if (skinManager->isSkinReady(settings.brand_background, settings.brand_logo,
                                         settings.brand_menu_background, settings.brand_menu_logo,
                                         settings.brand_background_hash, settings.brand_logo_hash,
                                         settings.brand_menu_background_hash, settings.brand_menu_logo_hash))
            {
                ThemeDesc skin;
                skin.backgroundURL = settings.brand_background;
                skin.logoURL = settings.brand_logo;
                skin.menuBackgroundURL = settings.brand_menu_background;
                skin.menuLogoURL = settings.brand_menu_logo;
                skin.color1 = settings.brand_color_1;
                skin.color2 = settings.brand_color_2;
                skin.menuColor1 = settings.brand_menu_color_1;
                skin.menuColor2 = settings.brand_menu_color_2;
            }
            skinManager->updateSkin(settings.brand_background,settings.brand_logo,
                                    settings.brand_menu_background, settings.brand_menu_logo,
                                    settings.brand_background_hash, settings.brand_logo_hash,
                                    settings.brand_menu_background_hash, settings.brand_menu_logo_hash,
                                    settings.brand_color_1,settings.brand_color_2,
                                    settings.brand_menu_color_1, settings.brand_menu_color_2,
                                    settings.brand_repeat,settings.brand_teleds_copyright,
                                    settings.brand_menu_repeat, settings.brand_menu_teleds_copyright);

            QTimer::singleShot(1000,[this](){this->showPlayer();});
        }
        else
        {*/
            qDebug() << "TeleDSCore::init brand is NOT active";
            teledsPlayer->show();
       // }

        //until we load actual playlist - update timer every 10 sec
        GlobalConfigInstance.setGetPlaylistTimerTime(10000);
        sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
        //load playlist
        QTimer::singleShot(1000, this, SLOT(getPlaylistTimerSlot()));
    } else
    {
        qDebug() << "TeleDSCore::player is not configurated";
        QTimer::singleShot(1000,this,SLOT(initPlayer()));
        teledsPlayer->show();
    }


    downloader = 0;
    setupHttpServer();
    qDebug() << "TELEDS initialization done";
}

void TeleDSCore::setupHttpServer()
{
    httpserver = new QHttpServer(this);
    httpserver->listen(16080);
    connect(httpserver,SIGNAL(newRequest(QHttpRequest*,QHttpResponse*)),this,SLOT(handleNewRequest(QHttpRequest*,QHttpResponse*)));
}

void TeleDSCore::initPlayer()
{
    qDebug() << "!!! Video service initialization";
    PlatformSpecificService.generateHardwareInfo();
    //videoService->advancedInit();
}

void TeleDSCore::initResult(InitRequestResult result)
{
    qDebug() << "TeleDSCore::initResult" << result.code << result.token;
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
    qDebug() << "HARDWARE INFO Ready";
    QJsonObject jsonBody;
    qDebug() << "Before grabbing Info";
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


    qDebug() << "After grabbing info";

    jsonBody["gps_lat"] = GlobalStatsInstance.getLatitude();
    jsonBody["gps_long"] = GlobalStatsInstance.getLongitude();

    QJsonDocument doc(jsonBody);
    QByteArray jsonData = doc.toJson();
    videoService->advancedInit(jsonData);
}

void TeleDSCore::handleNewRequest(QHttpRequest *request, QHttpResponse *response)
{
    QString path = request->path();
    if (request->method() == QHttpRequest::HTTP_GET)
    {
        QStringList tokens = path.split("/");
        if (tokens.count() == 3)
        {
            QString widgetId = tokens[1];
            QString contentId = tokens[2];
            if (widgetId == "system" && contentId == "list")
            {
                qDebug() << "SYSTEM::LIST";
                QString dataStr;
                foreach (const QString &key, storedData.keys())
                {
                    dataStr+=key + ":";
                    foreach (const QString &cKey, storedData[key].keys())
                    {
                        dataStr+=cKey + ",";
                    }
                    dataStr += "\n";
                }
                QByteArray data = dataStr.toLocal8Bit();
                response->writeHead(200);
                response->setHeader("Content-Type","text/plain");
                response->setHeader("Content-Length", QString::number(data.size()));
                response->end(data);
                request->deleteLater();
            }
            if (storedData.contains(widgetId)){
                if (storedData[widgetId].contains(contentId))
                {
                    QByteArray data = storedData[widgetId][contentId];

                    response->writeHead(200);
                    response->setHeader("Content-Type","text/plain");
                    response->setHeader("Content-Length", QString::number(data.size()));
                    response->end(data);
                    request->deleteLater();
                }
            }
            else
            {
                QString notFound = "Not Found";
                response->writeHead(404);
                response->setHeader("Content-Type","text/plain");
                response->setHeader("Content-Length", QString::number(notFound.toLocal8Bit().size()));
                response->end(notFound.toLocal8Bit());
            }
        }
        else
        {
            QString wrongRequest = "Bad Request: cant recognize tokens";
            response->writeHead(400);
            response->setHeader("Content-Type","text/plain");
            response->setHeader("Content-Length", QString::number(wrongRequest.toLocal8Bit().size()));
            response->end(wrongRequest.toLocal8Bit());
        }
    }
    else if (request->method() == QHttpRequest::HTTP_PUT)
    {
        QStringList tokens = path.split("/");
        if (tokens.count() == 3)
        {
            QString widgetId = tokens[1];
            QString contentId = tokens[2];
            new HTTPServerDataReceiver(this,request,response,widgetId, contentId);
        }
        else
        {
            QString wrongRequest = "Bad Request: cant recognize tokens";
            response->writeHead(400);
            response->setHeader("Content-Type","text/plain");
            response->setHeader("Content-Length", QString::number(wrongRequest.toLocal8Bit().size()));
            response->end(wrongRequest.toLocal8Bit());
        }
    }
    else
    {
        QByteArray unsupported = QString("Unsupported method").toLocal8Bit();
        response->writeHead(405);
        response->setHeader("Content-Type","text/plain");
        response->setHeader("Content-Length", QString::number(unsupported.size()));
        response->end(unsupported);
    }
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

void TeleDSCore::playerSettingsResult(SettingsRequestResult result)
{
    qDebug() << "playerSettingsResult";
    //this method is called when we try to get player settings

    if (result.error_id == -1)
    {
        SettingsRequestResult storedSettings = SettingsRequestResult::fromJson(GlobalConfigInstance.getSettings());
        if (storedSettings.error_id == 0)
        {
            playerSettingsResult(storedSettings);
            return;
        }
    }
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
    else if (result.error_id == 402)
    {
        if (teledsPlayer->isPlaying())
            teledsPlayer->stopPlaying();
        qDebug() << "403: player is not configurated - requesting initialization";
        //invoke played is not activated
    }
    else
    {
        //if no errors
        if (result.error_id == 0)
        {
            //load all player areas information
            videoService->getPlaylist();
            GlobalConfigInstance.setVideoQuality(result.video_quality);

            //saving time targeting reley config
            GlobalConfigInstance.setReleyEnabled(result.reley_1_enabled, result.reley_2_enabled);
            GlobalConfigInstance.setReleyConfig(result.time_targeting_relay_1, result.time_targeting_relay_2);

            //setting up autobrightness setup
            GlobalConfigInstance.setAutoBrightness(result.autobright);
          //  GlobalConfigInstance.setAutoBrightness(true);
            GlobalConfigInstance.setMinBrightness(result.bright_night);
            GlobalConfigInstance.setMaxBrightness(result.bright_day);
            GlobalConfigInstance.setStatsInverval(result.stats_interval);
            GlobalConfigInstance.setVolume(result.volume);

            //if static gps is given - saving it in stats
            if (result.gps_lat != 0.0 && result.gps_long != 0.0)
                GlobalStatsInstance.setGps(result.gps_lat, result.gps_long);
            qDebug() << "STATS INTERVAL: " << result.stats_interval;
          //  if (result.stats_interval >= 60000)
           //     statsTimer->start(result.stats_interval);

           /* if (result.brand_active)
            {
                qDebug() << "TeleDSCore::settings brand is active!";
                skinManager->updateSkin(result.brand_background,result.brand_logo,
                                        result.brand_menu_background, result.brand_menu_logo,
                                        result.brand_background_hash, result.brand_logo_hash,
                                        result.brand_menu_background_hash, result.brand_menu_logo_hash,
                                        result.brand_color_1,result.brand_color_2,
                                        result.brand_menu_color_1, result.brand_menu_color_2,
                                        result.brand_repeat,result.brand_teleds_copyright,
                                        result.brand_menu_repeat, result.brand_menu_teleds_copyright);
            }
            else
            {
                qDebug() << "TeleDSCore::settings brand is NOT!!! active!";
                teledsPlayer->invokeRestoreDefaultTheme();
            //}*/

            batteryStatus.setActive(result.autooff_by_battery_level_active, result.autooff_by_discharging_time_active);
            if (result.autooff_by_battery_level_active || result.autooff_by_discharging_time_active)
                batteryStatus.setConfig(result.off_charge_percent, result.off_power_loss);
        }
    }
}

void TeleDSCore::playlistResult(PlayerConfigAPI result)
{
    //this method is called when we got playlist
    //!a | a*b = !a +b
    //when we should update playlist

    qDebug() << "TeleDSCore::playlistResult";
    if (!currentConfig.last_modified.isValid() || (result.last_modified > currentConfig.last_modified && result.last_modified.isValid()))
    {
        qDebug() << "TeleDSCore::playlistResult <> need to update";
        if (result.error_id == -1)
        {
            qDebug() << "TeleDSCore::playlistResult <> loading from config";
            qDebug() << "seems like server is offline so we load from config";
            PlayerConfigAPI storedResult = PlayerConfigAPI::fromJson(GlobalConfigInstance.getPlayerConfig());
            if (storedResult.last_modified.isValid())
            {
                qDebug() << "config is Valid";
                currentConfig = storedResult;
            }
        }
        else
        {
            currentConfig = result;
        }
        setupDownloader();
    }
    else
    {
        qDebug() << "TeleDSCore::no updates";
    }
    if (currentConfig.count() == 0)
    {
        teledsPlayer->invokeNoItemsView("http://teleds.com");
        return;
    }

}

void TeleDSCore::onThemeReady(ThemeDesc desc)
{
    qDebug() << "ThemeReady!";
    if (!desc.isDefault)
    {
        auto tdsPlayer = teledsPlayer;
        QTimer::singleShot(1000, [desc, tdsPlayer]() mutable {
            tdsPlayer->invokeSetTheme(desc.relocatedBackgroundURL.toString(),
                                      desc.relocatedLogoURL.toString(),
                                      desc.color1, desc.color2, QString("#d7d7d7"),
                                      desc.tileMode, desc.showTeleDSPlayer);
            tdsPlayer->invokeSetMenuTheme(desc.relocatedMenuBackgroundURL.toString(),
                                          desc.relocatedMenuLogoURL.toString(),
                                          desc.menuColor1, desc.menuColor2,
                                          desc.menuTileMode, desc.menuShowTeleDSPlayer);

        });
    }

    else
    {
        auto tdsPlayer = teledsPlayer;
        QTimer::singleShot(1000, [desc, tdsPlayer]() mutable {
            tdsPlayer->invokeRestoreDefaultTheme();
        });
    }
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
    qDebug() << "TeleDSCore::downloaded";

    //after we download items - update playlists every 30 secs
    GlobalConfigInstance.setGetPlaylistTimerTime(60000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);

    int currentCampaignIndex = teledsPlayer->getCurrentCampaignIndex();
    if (currentCampaignIndex >= currentConfig.campaigns.count())
        currentCampaignIndex = 0;

    qDebug() << "choosing campaign";
    currentConfig.currentCampaignId = currentCampaignIndex;
    PlayerConfigAPI::Campaign campaign = currentConfig.campaigns[0];

    qDebug() << "stopping player";
    teledsPlayer->invokeStop();
    teledsPlayer->updateConfig(currentConfig);
    teledsPlayer->invokeDownloadDone();
    foreach (const PlayerConfigAPI::Campaign::Area &area, campaign.areas)
        teledsPlayer->invokeInitArea(area.area_id, campaign.screen_width, campaign.screen_height,
                                     area.x, area.y, area.width, area.height);
    teledsPlayer->play();


    //initialization of teledsPlayer
  //  qDebug() << "TeleDSCore::downloaded | screenCount = " << currentConfig.screens.count();
   /* if (currentConfig.campaigns.count())
    {
        PlayerConfig::AreaCompositionType configType = currentConfig.getType();

        switch (configType)
        {
        default:
        case PlayerConfig::AREA_MULTI:
        case PlayerConfig::AREA_BROKEN:
            teledsPlayer->invokeNoItemsView("http://teleds.com");
            break;
        case PlayerConfig::AREA_FULLSCREEN:
            teledsPlayer->invokeSetDisplayMode("fullscreen");
            teledsPlayer->invokeDownloadDone();
            teledsPlayer->setConfig(currentConfig.getScreenByType("fullscreen"));
            if (!teledsPlayer->isPlaying())
            {
                qDebug() << "TeleDSCore::downloaded -> invoke first time play!";
                teledsPlayer->play();
                QTimer::singleShot(2000, teledsPlayer, SLOT(playNext()));
            }
            break;
        case PlayerConfig::AREA_SPLIT:
            teledsPlayer->invokeSetDisplayMode("split");
            teledsPlayer->invokeDownloadDone();

            PlayerConfig::VirtualScreen contentScreen = currentConfig.getScreenByType("content");
            PlayerConfig::VirtualScreen widgetScreen = currentConfig.getScreenByType("widget");

            teledsPlayer->invokeSetContentPosition(contentScreen.position.left(), contentScreen.position.top(),
                                                   contentScreen.position.width(), contentScreen.position.height(),
                                                   widgetScreen.position.left(), widgetScreen.position.top(),
                                                   widgetScreen.position.width(), widgetScreen.position.height());
            teledsPlayer->setConfig(contentScreen, widgetScreen);
            if (!teledsPlayer->isPlaying())
            {
                qDebug() << "TeleDSCore::downloaded -> invoke first time play with widget";
                teledsPlayer->play();
                QTimer::singleShot(1100, teledsPlayer, SLOT(playNextWidget()));
                QTimer::singleShot(2000, teledsPlayer, SLOT(playNext()));
            }
            break;
        }
    }
    else
        teledsPlayer->invokeNoItemsView("http://teleds.com");*/
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

void TeleDSCore::showPlayer()
{
    if (shouldShowPlayer)
    {
        teledsPlayer->show();
        shouldShowPlayer = false;
    }
}

void TeleDSCore::setupDownloader()
{
    qDebug() << "Core::setupDownloader";

    if (downloader)
        downloader->updateConfig(currentConfig);
    else
    {
        downloader = new VideoDownloader(currentConfig,this);
        downloader->start();
        connect(downloader, SIGNAL(done()), this, SLOT(downloaded()));
        connect(downloader,SIGNAL(downloadProgressSingle(double,QString)), teledsPlayer, SLOT(invokeSimpleProgress(double,QString)));
        connect(downloader, SIGNAL(donwloadConfigResult(int)),this, SLOT(needToDownloadResult(int)));
    }
    sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
    downloader->runDownloadNew();

    /*void TeleDSCore::setupDownloader(PlayerConfig &newConfig)
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
    }*/
}

void TeleDSCore::setupCampaignAreas(const PlayerConfigAPI::Campaign &c)
{
    foreach (const PlayerConfigAPI::Campaign::Area &area, c.areas)
        teledsPlayer->invokeInitArea(area.area_id,
                                     c.screen_width, c.screen_height,
                                     area.x, area.y, area.width, area.height);
}

void BatteryStatus::setConfig(int minCapacityLevel, int maxTimeWithoutPower)
{
    this->minCapacityLevel = minCapacityLevel;
    this->maxTimeWithoutPower = maxTimeWithoutPower;
}

void BatteryStatus::setActive(bool autooff_by_battery_level_active, bool autooff_by_discharging_time_active)
{
    this->autooff_by_battery_level_active = autooff_by_battery_level_active;
    this->autooff_by_discharging_time_active = autooff_by_discharging_time_active;
    if (!autooff_by_battery_level_active && !autooff_by_discharging_time_active)
    {
        QDateTime invalidDate;
        lastTimeChecked = invalidDate;
        inactiveTime = 0;
    }
}

bool BatteryStatus::checkIfNeedToShutDown(Platform::BatteryInfo status)
{
    if (!autooff_by_battery_level_active && !autooff_by_discharging_time_active)
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
        if (autooff_by_battery_level_active && status.value < minCapacityLevel && !status.isCharging)
            return true;
        if (autooff_by_discharging_time_active && maxTimeWithoutPower != -1 && inactiveTime >= maxTimeWithoutPower)
            return true;
    }
    return false;
}

HTTPServerDataReceiver::HTTPServerDataReceiver(TeleDSCore *core, QHttpRequest *request, QHttpResponse *response, QString widgetId, QString contentId) :
    req(request),
    res(response)
{
    connect(request, SIGNAL(data(const QByteArray&)), this, SLOT(accumulate(const QByteArray&)));
    connect(request, SIGNAL(end()), this, SLOT(reply()));
    connect(res, SIGNAL(done()), this, SLOT(deleteLater()));
    this->core = core;
    this->widgetId = widgetId;
    this->contentId = contentId;
}


void HTTPServerDataReceiver::accumulate(const QByteArray &data)
{
    this->data.append(data);
}

void HTTPServerDataReceiver::reply()
{
    qDebug() << "TeleDSCore::HTTPServerDataReceiver::reply() << " << data;
    if (!core->storedData.contains(widgetId))
        core->storedData[widgetId] = QHash<QString, QByteArray>();
    core->storedData[widgetId][contentId] = data;
    if (widgetId == "system")
        GlobalStatsInstance.setSystemData(contentId, data);
    qDebug() << "TeleDSCore::SERVER-> " + widgetId + " " + contentId << data;
    QByteArray successResponse = QString("Success").toLocal8Bit();
    res->writeHead(201);
    res->setHeader("Content-Type","text/plain");
    res->setHeader("Content-Length", QString::number(successResponse.size()));
    res->end(successResponse);
}

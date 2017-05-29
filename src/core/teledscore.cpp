#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QUrl>
#include <QDir>
#include <QTime>
#include <QTimeZone>
#include <QScreen>
#include <QSslSocket>
#include <QTcpSocket>
#include <QThread>
#include <QHttpMultiPart>
#include <QHttpPart>

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
#include "notherfilesystem.h"
#include "version.h"

TeleDSCore::TeleDSCore(QObject *parent) : QObject(parent)
{
    qDebug() << "TELEDS v" <<TeleDSVersion::getVersion();
    qsrand((uint)QTime::currentTime().msec());
    DatabaseInstance;
    PlatformSpecificService;
    StaticTextService;
    StaticTextService.init();
    qRegisterMetaType< ThemeDesc >("ThemeDesc");
    qDebug() << "Working in " << QDir().currentPath();

    //GPIO Releys
    qDebug() << "RELAYTM";
    QTimer * releyTimer = new QTimer();
    connect(releyTimer, SIGNAL(timeout()),this,SLOT(checkReleyTime()));
    releyTimer->start(60000);

    gpsInitializated = 0;

    keyboardServiceThread = 0;
    initSystemServices();

    //Android battery shutdown-conditions checker
    QTimer * shutdownTimer = new QTimer();
    connect(shutdownTimer, SIGNAL(timeout()), this, SLOT(checkForAutomaticShutdown()));
    shutdownTimer->start(60000);

    videoService = new VideoService("https://api.teleds.com");

    PlatformSpecificService.install();

    uploader = new StatisticUploader(videoService,this);
    teledsPlayer = new TeleDSPlayer(this);
    skinManager = new SkinManager(this);
    //teledsPlayer->show();
    shouldShowPlayer = true;

    statsTimer = new QTimer();
    connect(statsTimer,SIGNAL(timeout()),uploader,SLOT(start()));
    statsTimer->start(180000);


    updateTimer = new QTimer();
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(checkUpdate()));
    updateTimer->start(60000);

    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlayerSettings(SettingsRequestResult)),this,SLOT(playerSettingsResult(SettingsRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfigAPI)),this,SLOT(playlistResult(PlayerConfigAPI)));
    connect(videoService,SIGNAL(getUpdatesResult(UpdateInfoResult)),this,SLOT(updateInfoReady(UpdateInfoResult)));

    connect (&sheduler,SIGNAL(getPlaylist()), this, SLOT(getPlaylistTimerSlot()));
    connect (teledsPlayer, SIGNAL(refreshNeeded()), this, SLOT(getPlaylistTimerSlot()));
    connect (teledsPlayer, SIGNAL(readyToUpdate()), this, SLOT(playlistUpdateReady()));

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
        qDebug() << "loading: token = " << result.token;
        playerInitParams = result;
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

        //until we load playlist - load playlist every 10 sec
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

    qDebug() << "Init gps Receiver";
    gpsSocket = new QTcpSocket(this);
    connect(gpsSocket,SIGNAL(readyRead()), this, SLOT(gpsRead()));
    connect(gpsSocket, &QTcpSocket::disconnected, [this]() {
        qDebug() << "GPS CLOSED::init!!!!";
        reconnectToGpsServer();
    });
    gpsSocket->connectToHost("127.0.0.1", 2947);
    updateGps = true;
    buttonBlocked = false;

    QTimer::singleShot(300000, [](){
        GlobalStatsInstance.setSystemData("force_request", QString("true").toLocal8Bit());
    });

    QTimer *checkConfigFileTimer = new QTimer();
    connect(checkConfigFileTimer, &QTimer::timeout, [this](){
        qDebug() << "Config Backup Service started";
        QFileInfo configInfo("/home/pi/teleds/data/config.dat");
        if (configInfo.size() > 5000)
        {
            QFileInfo configBackupInfo("/home/pi/teleds/data/config_backup.dat");
            if (configBackupInfo.size() != configInfo.size())
            {
                qDebug() << "Backing up config File!";
                QFile configFile("/home/pi/teleds/data/config.dat");
                if (configFile.open(QFile::ReadOnly))
                {
                    QByteArray configData = configFile.readAll();
                    QFile configBackupFile("/home/pi/teleds/data/config_backup.dat");
                    if (configBackupFile.open(QFile::WriteOnly))
                    {
                        configBackupFile.write(configData);
                        configBackupFile.flush();
                        configBackupFile.close();
                    }
                    else
                        qDebug() << "Error: cant open backup file";
                }
                else
                    qDebug() << "Error: cant open config file";
            }
        }
        else
        {
            qDebug() << "Suspicious config File! Checking content";
            if (QFile::exists("/home/pi/teleds/data/config_backup.dat"))
            {
                QFile configFile("/home/pi/teleds/data/config.dat");
                if (configFile.open(QFile::ReadOnly))
                {
                    QJsonDocument configDoc = QJsonDocument::fromJson(configFile.readAll());
                    QJsonObject configRoot = configDoc.object();
                    if (configRoot["device"].toString() == "" &&
                        configRoot["areas"].toArray().count() == 0)
                    {
                        qDebug() << "Seems like playlist is empty, trying to replace with backuped one";
                        configFile.close();
                        if (configFile.open(QFile::WriteOnly))
                        {
                            QFile configBackupFile("/home/pi/teleds/data/config_backup.dat");
                            if (configBackupFile.open(QFile::ReadOnly))
                            {
                                QByteArray data = configBackupFile.readAll();
                                configFile.write(data);
                                configFile.flush();
                                configFile.close();
                                configBackupFile.close();
                                qApp->exit();
                            }
                            else
                                qDebug() << "Error: cant open backup file";
                        }
                        else
                            qDebug() << "Error: cant open config file for writing";
                    }
                    else
                    {
                        configFile.close();
                        qDebug() << "Config File seems configurated. Doing nothing";
                    }
                }
                else
                    qDebug() << "Error: cant open config file for reading";
            }
            else
                qDebug() << "Backup config file does not exist, do nothing";
        }
    });
    checkConfigFileTimer->start(120000);
}

void TeleDSCore::initSystemServices()
{
    qDebug() << "TeleDSCore::initSystemServices";
    qDebug() << "Initialization of Combos";

    resetCombo = QList<int>()           << 29 << 42 << 32;       //ctrl-shift-D
    menuCombo = QList<int>()            << 29 << 42 << 50;       //ctrl-shift-M
    settingsCombo = QList<int>()        << 29 << 42 << 31;       //ctrl-shift-S
    passCombo = QList<int>()            << 29 << 42 << 25;       //ctrl-shift-P
    ifconfigCombo = QList<int>()        << 29 << 42 << 23;       //ctrl-shift-I
    hidePlayerCodeCombo = QList<int>()  << 29 << 42 << 35;       //ctrl-shift-H
    skipItemCombo = QList<int>()        << 29 << 42 << 49;       //ctrl-shift-N
    rebootCombo = QList<int>()          << 29 << 42 << 19;       //ctrl-shift-R

    qDebug() << "Initialization of InputDeviceControlService";
    inputDeviceControlServiceThread = new QThread();
    inputDeviceControlService = new InputDeviceControlService();
    inputDeviceControlService->moveToThread(inputDeviceControlServiceThread);
    inputDeviceControlServiceThread->start();
    QTimer::singleShot(3000, [this](){
        connect(inputDeviceControlService, SIGNAL(deviceConnected()), this, SLOT(onInputDeviceConnected()));
        connect(inputDeviceControlService, SIGNAL(deviceDisconnected()), this, SLOT(onInputDeviceDisconnected()));
        connect(this, SIGNAL(runInputDeviceControlService()), inputDeviceControlService, SLOT(run()));
        emit runInputDeviceControlService();
    });

    qDebug() << "Initialization of GPIOButtonService";
    gpioButtonServiceThread = new QThread();
    QTimer *gpioButtonTimer = new QTimer();
    QObject::connect(gpioButtonTimer, SIGNAL(timeout()), &gpioButtonService, SLOT(checkPinSlot()));
    gpioButtonService.moveToThread(gpioButtonServiceThread);
    gpioButtonServiceThread->start();
    gpioButtonTimer->start(80);
    QObject::connect(&gpioButtonService, SIGNAL(buttonPressed()), this, SLOT(onButtonPressed()));

    qDebug() << "Initialization of Reset Player Button";
    QTimer *resetTimer = new QTimer(this);
    connect(resetTimer, SIGNAL(timeout()), this, SLOT(checkReset()));
    resetTimer->start(2500);

    //
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

void TeleDSCore::reconnectToGpsServer()
{
    QTimer::singleShot(30000, [this]()
    {
        gpsInitializated = false;
        qDebug() << "Reinitialization GPS";
        if (gpsSocket)
        {
            gpsSocket->disconnect();
            gpsSocket->deleteLater();
        }
        gpsSocket = new QTcpSocket(this);
        connect(gpsSocket,SIGNAL(readyRead()), this, SLOT(gpsRead()));
        connect(gpsSocket, &QTcpSocket::disconnected, [this]() {
            qDebug() << "GPS CLOSED!!!!";
            reconnectToGpsServer();
        });
        gpsSocket->connectToHost("127.0.0.1", 2947);
    });
}

QByteArray TeleDSCore::createZip()
{
    QProcess p;
    p.start("bash build_log.sh");
    p.waitForStarted();
    p.waitForFinished();
    qDebug() << "zip result";
    qDebug() << p.readAll();
    QFile f("/home/pi/teleds/ziplg.zip");
    if (f.open(QFile::ReadOnly))
    {
        QByteArray data = f.readAll();
        qDebug() << "ZIP SIZE" << data.count();
        f.close();
        return data;
    }
    return QByteArray();
}

void TeleDSCore::sendLogs()
{
    qDebug() << "TeleDSCore::sendLogs";
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    QByteArray data = createZip();
    if (data.count())
    {
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"LOGFILE\""));
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        filePart.setBody(data);

        multiPart->append(filePart);
        QUrl url("https://api.teleds.com/player/logs");
        QNetworkRequest request(url);
        request.setRawHeader("Authorization",GlobalConfigInstance.getToken().toLocal8Bit());

        QNetworkReply *reply = mgr->post(request, multiPart);
        multiPart->setParent(reply);
        connect(reply, &QNetworkReply::finished, [reply, mgr](){
            qDebug() << "LOGS SENT!";
            reply->deleteLater();
            mgr->deleteLater();
            qDebug() << reply->readAll();
            qDebug() << reply->errorString();
        });
    }
    else
    {
        qDebug() << "Error: no logs, cant send nothing";
        mgr->deleteLater();
    }


}

void TeleDSCore::initResult(InitRequestResult result)
{
    qDebug() << "TeleDSCore::initResult" << result.code << result.token << result.error_id;
    if (result.error_id == 0)
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
        teledsPlayer->invokePlayerActivationRequiredView("http://teleds.com",result.code, PlatformSpecificService.isAndroid());

        //start get playlist
        QTimer::singleShot(1000,this,SLOT(getPlaylistTimerSlot()));
    }
    else if (result.error_id == 422)
    {
        qDebug() << "Backend Returned 422 error on init" << QDateTime::currentDateTimeUtc();
        QTimer::singleShot(5000, this, [this](){
            initPlayer();
        });
    }
    else if (result.error_id == -1)
    {
        QTimer::singleShot(5000, this, [this](){
            initPlayer();
        });
    }
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

    qDebug() << "Hardware result timestamp = " << jsonBody["timestamp"].toString();
    qDebug() << "timezone" << jsonBody["timezone"].toString();
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
    qDebug() << "playerSettingsResult" << result.error_id;
    //this method is called when we try to get player settings

    if (result.error_id == -1)
    {
        SettingsRequestResult storedSettings = SettingsRequestResult::fromJson(GlobalConfigInstance.getSettings(), false);

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
        teledsPlayer->invokePlayerActivationRequiredView("http://teleds.com",GlobalConfigInstance.getActivationCode(),
                                                         PlatformSpecificService.isAndroid());
    }
    //403: player is not configurated - requesting initialization
    if (result.error_id == 403 || result.error_id == 404)
    {
        if (teledsPlayer->isPlaying())
            teledsPlayer->stopPlaying();
        qDebug() << "403/404: player is not configurated - requesting initialization";
        initPlayer();
    }
    else if (result.error_id == 402)
    {
        if (teledsPlayer->isPlaying())
            teledsPlayer->stopPlaying();
        qDebug() << "403: player is not configurated - requesting initialization";
        //invoke played is not activated
    }
    else if (result.error_id == 304) //settings was not updated
    {
        qDebug() << "304: settings wasnt updated - no need to update";
        videoService->getPlaylist();
    }
    else
    {
        //if no errors
        if (result.error_id == 0)
        {
            //load all player areas information
            videoService->getPlaylist();

            //saving time targeting reley config
            GlobalConfigInstance.setReleyEnabled(result.reley_1_enabled, result.reley_2_enabled);
            GlobalConfigInstance.setReleyConfig(result.time_targeting_relay_1, result.time_targeting_relay_2);

            //setting up autobrightness setup
            GlobalConfigInstance.setAutoBrightness(result.autobright);
            //GlobalConfigInstance.setAutoBrightness(true);
            GlobalConfigInstance.setMinBrightness(result.bright_night);
            GlobalConfigInstance.setMaxBrightness(result.bright_day);
            GlobalConfigInstance.setStatsInverval(result.stats_interval);
            GlobalConfigInstance.setVolume(result.volume);
            GlobalConfigInstance.setMetaProperty("settings_hash", result.hash);

            quint32 crc32id = SSLEncoder::CRC32(result.player_id.toLocal8Bit());
            QString crcHex = QString("%1").arg(crc32id, 8, 16, QLatin1Char( '0' )).toUpper();
            qDebug() << "CRC HEX = " << crcHex;
            GlobalStatsInstance.setCRC32Hex(crcHex);
            teledsPlayer->invokePlayerCRC();

            teledsPlayer->invokeSetPlayerVolume(result.volume);

            //if static gps is given - saving it in stats
            if (result.gps_lat != 0.0 && result.gps_long != 0.0 && !result.updateGps)
            {
                GlobalStatsInstance.setGps(result.gps_lat, result.gps_long);
            }
            updateGps = result.updateGps;
            if (updateGps && GlobalStatsInstance.getLatitude() == result.gps_lat && GlobalStatsInstance.getLongitude() == result.gps_long)
                GlobalStatsInstance.setGps(0.0, 0.0);
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
            if (result.send_logs == 1)
                sendLogs();
        }
    }
}

void TeleDSCore::playlistResult(PlayerConfigAPI result)
{
    //this method is called when we got playlist
    //when we should update playlist
    static int prevScreenRotation = 0;
    qDebug() << "TeleDSCore::playlistResult";

    if ((!currentConfig.last_modified.isValid()
       || ((result.last_modified > currentConfig.last_modified ||
            result.hash != currentConfig.hash) && result.last_modified.isValid())
       || prevScreenRotation != GlobalConfigInstance.getSettingsObject().base_rotation) &&
        result.error_id != 304)
    {
        qDebug() << "TeleDSCore::playlistResult <> need to update";
        if (result.error_id == -1)
        {
            qDebug() << "TeleDSCore::seems like server is offline so we load from config";
            PlayerConfigAPI storedResult = PlayerConfigAPI::fromJson(GlobalConfigInstance.getPlaylist());
            if (storedResult.last_modified.isValid())
            {
                qDebug() << "config is Valid";
                currentConfig = storedResult;
            }
        }
        else
        {
            currentConfig = result;
            GlobalConfigInstance.setMetaProperty("playlist_hash", result.hash);
        }
        if (currentConfig.count() == 0)
        {
            qDebug() << "NO ITEMS!";
            teledsPlayer->invokeStop();
            teledsPlayer->invokeNoItemsView("https://teleds.com");
            prevScreenRotation = GlobalConfigInstance.getSettingsObject().base_rotation;
            return;
        }
        prevScreenRotation = GlobalConfigInstance.getSettingsObject().base_rotation;
        setupDownloader();
    }
    else
    {
        qDebug() << "TeleDSCore::no updates (" << result.error_id << ")";
        prevScreenRotation = GlobalConfigInstance.getSettingsObject().base_rotation;
        teledsPlayer->invokeBackDownloadProgressBarVisible(false);
    }
    if (currentConfig.count() == 0)
    {
        teledsPlayer->invokeNoItemsView("https://teleds.com");
        prevScreenRotation = GlobalConfigInstance.getSettingsObject().base_rotation;
        return;
    }

}

void TeleDSCore::checkUpdate()
{
    updateTimer->stop();
    QTimer::singleShot(125000, [this](){
        updateTimer->stop();
        updateTimer->start(60000);
    });
    videoService->getUpdates("raspberry");
}


void TeleDSCore::checkKeys()
{
    auto keys = qApp->queryKeyboardModifiers();
    qDebug() << keys;
    if (keys.testFlag(Qt::ShiftModifier))
        qApp->quit();
}

void TeleDSCore::updateInfoReady(UpdateInfoResult result)
{
    qDebug() << "TeleDSCore::updateInfoReady " + QString::number(result.error_id) + "/" + result.error_text;
    if (result.error_id == 0)
    {
        if (TeleDSVersion::compareVersion(result.version_major, result.version_minor, result.version_release, result.version_build) == 1) //upcoming version is newer
        {
            qDebug() << "NEW Version Found! Trying to download!";
            QString genName =   "TDSU_" +
                                QString::number(result.version_major) + "." +
                                QString::number(result.version_minor) + "." +
                                QString::number(result.version_release) + "." +
                                QString::number(result.version_build) + ".dat";
            if (PlatformSpecificService.getFileHash(genName) != result.file_hash)
            {
                qDebug() << "need to download update";
                if (downloader)
                    downloader->startUpdateTask(result.file_url, result.file_hash, genName);
            }
            else
            {
                qDebug() << "update is already downloaded. Updating.";
                updateReady(genName);
            }
        }
        else{
            qDebug() << "You have the newest version of player";
        }
    }
    updateTimer->start(30000);
}

void TeleDSCore::updateReady(QString filename)
{
    qDebug() << "TeleDSCore::updateReady";
    NotherFileSystem nfs;
    nfs.load(filename);
    if (nfs.fileExists("system::updater"))
    {
        QByteArray updaterData = nfs.getFile("system::updater");
        QFile f("updater");
        if (f.open(QFile::WriteOnly))
        {
            f.write(updaterData);
            f.flush();
            f.setPermissions(QFile::ExeGroup | QFile::ExeOwner | QFile::ExeOther | QFile::ExeUser |
                             QFile::ReadOwner| QFile::ReadUser | QFile::ReadOther | QFile::ReadGroup |
                             QFile::WriteGroup | QFile::WriteOwner | QFile::WriteOther | QFile::WriteUser);
            f.close();
        }
    }
    teledsPlayer->invokeUpdateState();
    QFile infoFile("update_info");
    if (infoFile.open(QFile::WriteOnly))
    {
        infoFile.write(filename.toLocal8Bit());
        infoFile.flush();
        infoFile.close();
    }

    QTimer::singleShot(1000, [this, filename]() mutable{
            qApp->exit();
        });
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

void TeleDSCore::downloaded(int index)
{
    if (index != 0)
        return;
    //this slot is called when all items got downloaded
    qDebug() << "TeleDSCore::downloaded";

    if (teledsPlayer->isPlaying())
    {
        qDebug() << "TeleDSPlayer is busy playing: waiting until player will be ready!";
        teledsPlayer->prepareStop();
        return;
    }
    //after we download items - update playlists every 60 secs
    //GlobalConfigInstance.setGetPlaylistTimerTime(60000);
    //sheduler.restart(TeleDSSheduler::GET_PLAYLIST);

    int currentCampaignIndex = teledsPlayer->getCurrentCampaignIndex();
    if (currentCampaignIndex >= currentConfig.campaigns.count())
        currentCampaignIndex = 0;

    qDebug() << "choosing campaign" << currentCampaignIndex;
    currentConfig.currentCampaignId = currentCampaignIndex;
    PlayerConfigAPI::Campaign campaign = currentConfig.campaigns[0];
    qDebug() << campaign.campaign_id;
    qDebug() << "stopping player";
    teledsPlayer->invokeStop();
    teledsPlayer->updateConfig(currentConfig);
    teledsPlayer->invokeDownloadDone();
    foreach (const PlayerConfigAPI::Campaign::Area &area, campaign.areas)
        teledsPlayer->invokeInitArea(area.area_id, campaign.screen_width, campaign.screen_height,
                                     area.x, area.y, area.width, area.height, campaign.rotation);
    QTimer::singleShot(1000, [this](){
        teledsPlayer->play();
    });
}

void TeleDSCore::playWithoutDownload(int count)
{
    qDebug() << "Download Finished";
    if (!teledsPlayer->isPlaying() || count == 0)
    {
        downloaded(0);
    }
    GlobalConfigInstance.setGetPlaylistTimerTime(60000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);

    QTimer::singleShot(2000, [this](){
        teledsPlayer->invokeBackDownloadProgressBarVisible(false);
    });
}

void TeleDSCore::playlistUpdateReady()
{
    qDebug() << "TeleDSPlayer is ready to update -> updating playlist";
    GlobalConfigInstance.setGetPlaylistTimerTime(60000);
    sheduler.restart(TeleDSSheduler::GET_PLAYLIST);
    int currentCampaignIndex = teledsPlayer->getCurrentCampaignIndex();
    if (currentCampaignIndex >= currentConfig.campaigns.count())
        currentCampaignIndex = 0;
    qDebug() << "choosing campaign" << currentCampaignIndex;
    currentConfig.currentCampaignId = currentCampaignIndex;
    PlayerConfigAPI::Campaign campaign = currentConfig.campaigns[0];
    qDebug() << campaign.campaign_id;
    qDebug() << "stopping player";
    teledsPlayer->invokeStop();
    teledsPlayer->updateConfig(currentConfig);
    teledsPlayer->invokeDownloadDone();
    foreach (const PlayerConfigAPI::Campaign::Area &area, campaign.areas)
        teledsPlayer->invokeInitArea(area.area_id, campaign.screen_width, campaign.screen_height,
                                     area.x, area.y, area.width, area.height, campaign.rotation);
    QTimer::singleShot(1000, [this](){
        teledsPlayer->play();
    });
    currentConfig.nextCampaign();
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

void TeleDSCore::gpsRead()
{
    if (gpsInitializated == 0){
        //gpsSocket->write(QString("?POLL;").toLocal8Bit());
        //i = 1;
        gpsSocket->write(QString("?WATCH={\"enable\":true,\"json\":true}").toLocal8Bit());
        gpsInitializated = 1;
        return;
    }
    if (updateGps)
    {
        QByteArray jsonData = gpsSocket->readAll();
        //qDebug() << jsonData;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isObject())
        {
            QJsonObject root = doc.object();
           // qDebug() << root;
            if (root["class"].toString() == "TPV")
            {
                double lat = root["lat"].toDouble();
                double lon = root["lon"].toDouble();
                if (lat != 0.0 && lon != 0.0)
                {
                    //qDebug() << "Got GPS: " << lat << lon;
                    GlobalStatsInstance.setGps(lat, lon);
                }
            }
        }
    }
}

void TeleDSCore::nextCampaign()
{
    teledsPlayer->invokeStop();
    int duration = teledsPlayer->nextCampaign();

    int currentCampaignIndex = teledsPlayer->getCurrentCampaignIndex();
    if (currentCampaignIndex >= currentConfig.campaigns.count())
        currentCampaignIndex = 0;

    qDebug() << "choosing campaign";
    currentConfig.currentCampaignId = currentCampaignIndex;
    PlayerConfigAPI::Campaign campaign = currentConfig.campaigns[currentCampaignIndex];
    foreach (const PlayerConfigAPI::Campaign::Area &area, campaign.areas)
        teledsPlayer->invokeInitArea(area.area_id, campaign.screen_width, campaign.screen_height,
                                     area.x, area.y, area.width, area.height, campaign.rotation);
    teledsPlayer->play();

    QTimer::singleShot(duration, this, [this](){
        nextCampaign();
    });
}

void TeleDSCore::onButtonPressed(bool skipBlocked)
{
    static int status = 2;
    if (buttonBlocked && !skipBlocked)
    {
        qDebug() << "Button is blocked!";
        return;
    }
    if (status == 2)
    {
        status = 1;
        return;
    }
    status = 1 - status;
    teledsPlayer->invokeToggleVisibility(status);
    QProcess p;
    if (status)
    {
        p.startDetached("bash", QString("data/display_control.sh 1").split(" "));
        buttonBlocked = true;

        if (!skipBlocked){
            QTimer::singleShot(4000, [this](){
                buttonBlocked = false;
            });
        }
        QTimer::singleShot(750, [this]() {
            QFile f("display_control.log");
            if (f.open(QFile::ReadOnly))
            {
                QString result = f.readAll();
                if (result == "")
                {
                    qDebug() << "Warning: vcgen deamon is sleeping. Trying to wake up";
                    status = 1 - status;
                    this->onButtonPressed(true);
                }
                else
                {
                    GlobalStatsInstance.setHDMI_GPIO(true);
                    //buttonBlocked = false;
                }
                f.close();
            }
        });
    }
    else
    {
        p.startDetached("bash", QString("data/display_control.sh 0").split(" "));
        buttonBlocked = true;
        if (!skipBlocked){
            QTimer::singleShot(4000, [this](){
                buttonBlocked = false;
            });
        }
        QTimer::singleShot(750, [this]() {
            QFile f("display_control.log");
            if (f.open(QFile::ReadOnly))
            {
                QString result = f.readAll();
                if (result == "")
                {
                    qDebug() << "Warning: vcgen deamon is sleeping. Trying to wake up";
                    status = 1 - status;
                    this->onButtonPressed(true);
                }
                else
                {
                    GlobalStatsInstance.setHDMI_GPIO(false);
                    //buttonBlocked = false;
                }
                f.close();
            }
        });
    }
}

void TeleDSCore::onKeyDown(int code)
{
    teledsPlayer->onKeyDown(code);
    int prevCodeState = storedKeys[code];
    if (!checkCombo(settingsCombo))
    {
        storedKeys[code] = 1;
        if (checkCombo(settingsCombo))
        {
            teledsPlayer->invokeToggleMenu();
        }
    }

    storedKeys[code] = prevCodeState;
    if (!checkCombo(passCombo))
    {
        storedKeys[code] = 1;
        if (checkCombo(passCombo))
            teledsPlayer->invokeShowPassword();
    }

    storedKeys[code] = prevCodeState;
    if (!checkCombo(ifconfigCombo))
    {
        storedKeys[code] = 1;
        if (checkCombo(ifconfigCombo))
            teledsPlayer->invokeShowInternetConnectionInfo();
    }

    storedKeys[code] = prevCodeState;
    if (!checkCombo(hidePlayerCodeCombo))
    {
        storedKeys[code] = 1;
        if (checkCombo(hidePlayerCodeCombo))
            teledsPlayer->invokeTogglePlayerIDVisible();
    }

    storedKeys[code] = prevCodeState;
    if (!checkCombo(skipItemCombo))
    {
        storedKeys[code] = 1;
        if (checkCombo(skipItemCombo))
        {
            GlobalStatsInstance.itemWasSkipped(15);
            teledsPlayer->invokeSkipCurrentItem();
        }
    }

    storedKeys[code] = 1;

    qDebug() << "onKeyDown " << code;
}

void TeleDSCore::onKeyUp(int code)
{
    teledsPlayer->onKeyUp(code);
    storedKeys[code] = 0;
    qDebug() << "onKeyUp " << code;
}

void TeleDSCore::onInputDeviceConnected()
{
    qDebug() << "Input Device connected";
    isInputDeviceConnected = true;
    if (keyboardServiceThread)
        keyboardServiceThread->deleteLater();
    keyboardServiceThread = new QThread();
    inputService = new InputService();
    inputService->moveToThread(keyboardServiceThread);
    keyboardServiceThread->start();
    QTimer::singleShot(3000, [this](){
        connect(inputService, SIGNAL(keyDown(int)), this, SLOT(onKeyDown(int)));
        connect(inputService, SIGNAL(keyUp(int)), this, SLOT(onKeyUp(int)));
        connect(this, SIGNAL(runInputService()), inputService, SLOT(run()));
        emit runInputService();
    });
}

void TeleDSCore::onInputDeviceDisconnected()
{
    qDebug() << "Input Device Disconnected";
    isInputDeviceConnected = false;
    inputService->disconnect();
    keyboardServiceThread->disconnect();
    inputService->deleteLater();
    keyboardServiceThread->terminate();
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
        connect(downloader, SIGNAL(done(int)), this, SLOT(playWithoutDownload(int)));
        connect(downloader,SIGNAL(fileDownloaded(int)), this, SLOT(downloaded(int)));
        //connect(downloader,SIGNAL(fileDownloaded()), this, SLOT(downloaded()));
        connect(downloader,SIGNAL(downloadProgressSingle(double,QString)), teledsPlayer, SLOT(invokeSimpleProgress(double,QString)));
        connect(downloader, SIGNAL(donwloadConfigResult(int)),this, SLOT(needToDownloadResult(int)));
        connect(downloader, SIGNAL(updateReady(QString)), this, SLOT(updateReady(QString)));
    }
    sheduler.stop(TeleDSSheduler::GET_PLAYLIST);
    downloader->runDownloadNew();
    teledsPlayer->invokeBackDownloadProgressBarVisible(true);
}

void TeleDSCore::setupCampaignAreas(const PlayerConfigAPI::Campaign &c)
{
    foreach (const PlayerConfigAPI::Campaign::Area &area, c.areas)
        teledsPlayer->invokeInitArea(area.area_id,
                                     c.screen_width, c.screen_height,
                                     area.x, area.y, area.width, area.height, c.rotation);

}

bool TeleDSCore::checkCombo(const QList<int> &keys)
{
    if (!isInputDeviceConnected)
        return false;
    bool result = true;
    foreach (const int&k, keys)
    {
        if (storedKeys.contains(k))
            result = storedKeys[k];
        else
            return false;
        if (!result)
            return false;
    }
    return true;
}

void TeleDSCore::resetPlayer()
{
    teledsPlayer->invokeStop();
    QDir dir(qApp->applicationDirPath() + "/data/video");
    dir.removeRecursively();
    QFile configFile("data/config.dat");
    if (configFile.open(QFile::WriteOnly))
    {
        configFile.write(QString("{}").toLocal8Bit());
        configFile.flush();
        configFile.close();
    }
    QFile::remove(DATABASE_FOLDER + "stat.db");
    qApp->quit();
}

void TeleDSCore::rebootPlayer()
{
    QProcess p;
    p.start("reboot");
    p.waitForStarted();
    p.waitForFinished();
}

void TeleDSCore::checkReset()
{
    static int count = 0;
    static int rebootCount = 0;
    if (checkCombo(resetCombo))
        count++;
    else
        count = 0;
    if (count >= 3)
        resetPlayer();

    if (checkCombo(rebootCombo))
        rebootCount++;
    else
        rebootCount = 0;
    if (rebootCount >= 3)
    {
        rebootPlayer();
    }
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

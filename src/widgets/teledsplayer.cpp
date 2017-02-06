#include <QFileInfo>
#include <QProcess>
#include "teledsplayer.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "sunposition.h"
#include "version.h"
#include "statictext.h"

#include "linux/input.h"

TeleDSPlayer::TeleDSPlayer(QObject *parent) : QObject(parent)
{
    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);

    view.setSource(QUrl(QStringLiteral("qrc:/main_player.qml")));
    viewRootObject = dynamic_cast<QObject*>(view.rootObject());
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QTimer::singleShot(500,this,SLOT(bindObjects()));
    QTimer::singleShot(500,this,SLOT(invokeVersionText()));
    QTimer::singleShot(500,this,SLOT(invokeSetLicenseData()));

   // show();

    delay = 0000;
    status.isPlaying = false;
    status.item = "";
    isActive = true;
    isSplitScreen = false;
    nextCampaignTimer = new QTimer(this);
    connect(nextCampaignTimer,SIGNAL(timeout()),this, SLOT(nextCampaignEvent()));
    checkNextVideoAfterStopTimer = new QTimer(this);
    connect(checkNextVideoAfterStopTimer, SIGNAL(timeout()), this, SLOT(nextItemEvent()));
    checkNextVideoAfterStopTimer->setProperty("activated", false);
    checkNextVideoAfterStopTimer->start(5000);

    //view.installEventFilter(this);
    shouldStop = false;
}

TeleDSPlayer::~TeleDSPlayer()
{

}

QString TeleDSPlayer::getFullPath(QString fileName, AbstractPlaylist * playlist)
{
    QString nextFile =  VIDEO_FOLDER + fileName +
                        playlist->findItemById(fileName).file_hash +
                        playlist->findItemById(fileName).file_extension;
    QFileInfo fileInfo(nextFile);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}

QString TeleDSPlayer::getFullPathZip(QString path)
{
    QFileInfo fileInfo(path);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}

void TeleDSPlayer::show()
{
    qDebug() << "TeleDSPlayer::show";
#ifdef PLAYER_MODE_WINDOWED
    view.show();
    view.setMinimumHeight(520);
    view.setMinimumWidth(920);
#else
    view.showFullScreen();
#endif
}

void TeleDSPlayer::updateConfig(PlayerConfigAPI &playerConfig)
{
    config = playerConfig;
    foreach (const QString &key, playlists.keys())
        playlists[key]->deleteLater();
    playlists.clear();
    foreach (const PlayerConfigAPI::Campaign &campaign, playerConfig.campaigns)
    {
        foreach (const PlayerConfigAPI::Campaign::Area area, campaign.areas)
        {
            AbstractPlaylist* playlist = new SuperPlaylist(this);
            playlist->updatePlaylist(area);
            playlists[area.area_id] = playlist;
        }
    }
}

void TeleDSPlayer::play(int delay)
{
    qDebug() << "TeleDSPlayer::play!";

    //
    status.isPlaying = true;
    isActive = true;
    int currentCampaignId = config.currentCampaignId;
    qDebug() << "currentCampaignId = " << currentCampaignId;

    int duration = config.nextCampaign();
    duration -= config.campaigns[currentCampaignId].areas.first().content.count() * 50;
    if (config.campaigns.count() > 1)
        GlobalStatsInstance.setCampaignEndDate(QDateTime::currentDateTimeUtc().addMSecs(duration));
    else
    {
        QDateTime d;
        GlobalStatsInstance.setCampaignEndDate(d);
    }
    foreach (const PlayerConfigAPI::Campaign::Area &a,config.campaigns[currentCampaignId].areas)
    {
        invokeInitArea(a.area_id,
                       config.campaigns[currentCampaignId].screen_width,
                       config.campaigns[currentCampaignId].screen_height,
                       a.x, a.y, a.width, a.height, config.campaigns[currentCampaignId].rotation);
        QTimer::singleShot(delay, [this, a]() {
            next(a.area_id);
        });
    }

    foreach (const PlayerConfigAPI::Campaign::Area &a,config.campaigns[config.currentCampaignId].areas)
    {
        qDebug() << "TeleDSPlayer::reset current index";
        ((SuperPlaylist*)playlists[a.area_id])->resetCurrentItemIndex();
    }

    if (config.campaigns.count() > 1) {
        nextCampaignTimer->setInterval(duration);
        nextCampaignTimer->start();
        /*
        QTimer::singleShot(duration, [this](){
            this->invokeStop();
            this->play(100);
        });*/
    }
}

PlayerConfigAPI::Campaign::Area TeleDSPlayer::getAreaById(QString id)
{
    PlayerConfigAPI::Campaign::Area result;
    foreach (const PlayerConfigAPI::Campaign &cmp, config.campaigns)
        foreach (const PlayerConfigAPI::Campaign::Area &area, cmp.areas)
            if (area.area_id == id)
                result = area;
    return result;
}

bool TeleDSPlayer::isFileCurrentlyPlaying(QString name)
{
    return status.item == name;
}

bool TeleDSPlayer::eventFilter(QObject *target, QEvent *event)
{
    qDebug() << "TELEDSEVENT!" << event->type();
    if (event->type() == QEvent::KeyPress) {
          QKeyEvent *keyEvent = (QKeyEvent *)event;
          if (keyEvent->key() == Qt::Key_Q) {
            qApp->quit();
            return true;
          }
        }
    return QObject::eventFilter(target,event);
}

void TeleDSPlayer::prepareStop()
{
    shouldStop = true;
    invokePrepareStop();
}

void TeleDSPlayer::invokeNextVideoMethodAdvanced(QString name, QString area_id)
{
    if (name == "" || area_id == "" || !playlists.contains(area_id))
    {
        invokeStopMainPlayer();
        return;
    }
    PlayerConfigAPI::Campaign::Area::Content item = playlists[area_id]->findItemById(name);

    QVariant source;
    if (item.type == "video" || item.type == "audio" || item.type == "image")
    {
        source = QUrl(getFullPath(name,playlists[area_id]));
        invokeSetPlayerVolume(GlobalConfigInstance.getVolume());
    }
    else if (item.type == "html5_online")
        source = item.file_url;
    else if (item.type == "html5_zip")
        source = getFullPathZip(VIDEO_FOLDER + item.content_id + "/index.html");

    QVariant type;
    if (item.type == "html5_zip")
        type = "html5_online";
    else
        type = item.type;
    QVariant duration = item.duration;
    QVariant skip = item.play_start;
    QVariant fillMode = item.fill_mode;

    QMetaObject::invokeMethod(viewRootObject, "playNextItem",
                              Q_ARG(QVariant, area_id),
                              Q_ARG(QVariant, source),
                              Q_ARG(QVariant, type),
                              Q_ARG(QVariant, duration),
                              Q_ARG(QVariant, skip),
                              Q_ARG(QVariant, fillMode));
}

void TeleDSPlayer::invokeFileProgress(double p, QString name)
{
    qDebug() << "invoking file progress";
    QVariant percent(p);
    QVariant fileName(name);
    QMetaObject::invokeMethod(viewRootObject,"setTotalProgress",Q_ARG(QVariant, percent), Q_ARG(QVariant, fileName));
}

void TeleDSPlayer::invokeProgress(double p)
{
    qDebug() << "invoking download progress";
    QVariant percent(p);
    QMetaObject::invokeMethod(viewRootObject,"setProgress",Q_ARG(QVariant, percent));
}

void TeleDSPlayer::invokeSimpleProgress(double p, QString)
{
    QVariant percent(p);
    QMetaObject::invokeMethod(viewRootObject,"setDownloadProgressSimple",Q_ARG(QVariant, percent));
}

void TeleDSPlayer::invokeDownloadDone()
{
    qDebug() << "invoking download completed";
    QMetaObject::invokeMethod(viewRootObject,"downloadComplete");
    invokeSetDeviceInfo();
}

void TeleDSPlayer::invokeVersionText()
{
    qDebug() << "invoking version text";
    QVariant versionText(TeleDSVersion::getVersion());
    QMetaObject::invokeMethod(viewRootObject,"setVersion",Q_ARG(QVariant, versionText));
}

void TeleDSPlayer::invokePlayerCRC()
{
    qDebug() << "invoking version text";
    QVariant crc(GlobalStatsInstance.getCRC32Hex());
    QMetaObject::invokeMethod(viewRootObject,"setCRC",Q_ARG(QVariant, crc));
}

void TeleDSPlayer::invokeToggleVisibility(int status)
{
    qDebug() << "invokeToggleVisibility";
    QMetaObject::invokeMethod(viewRootObject, "toggleVisibility", Q_ARG(QVariant, QVariant(status)));
}

void TeleDSPlayer::invokePlayerActivationRequiredView(QString url, QString playerId)
{
    qDebug() << "invokePlayerActivationRequiredView";
    QVariant urlParam(url);
    QVariant playerIdParam("  " + playerId.toUpper() + "  ");
    QVariant updateDelayParam(GlobalConfigInstance.getGetPlaylistTimerTime()/1000);
    QMetaObject::invokeMethod(viewRootObject,"setNeedActivationLogo",
                              Q_ARG(QVariant, urlParam),
                              Q_ARG(QVariant, playerIdParam),
                              Q_ARG(QVariant, updateDelayParam));
    invokeSetDeviceInfo();
}

void TeleDSPlayer::invokeNoItemsView(QString url)
{
    qDebug() << "invokeNoItemsView";
    QVariant urlParam(url);
    QMetaObject::invokeMethod(viewRootObject,"setNoItemsLogo", Q_ARG(QVariant, urlParam));
    invokeSetDeviceInfo();
}

void TeleDSPlayer::invokeDownloadingView()
{
    qDebug() << "invokeDownloading View";
    QMetaObject::invokeMethod(viewRootObject,"setDownloadLogo");
}

void TeleDSPlayer::invokeStop()
{
    qDebug() << "TeleDSPlayer::invokeStop";
    nextCampaignTimer->stop();
    QMetaObject::invokeMethod(viewRootObject, "stopPlayer");
}

void TeleDSPlayer::invokePrepareStop()
{
    qDebug() << "TeleDSPlayer::invokePrepareStop";
    QMetaObject::invokeMethod(viewRootObject, "prepareStop");
}

void TeleDSPlayer::invokeStopMainPlayer()
{
    qDebug() << "TeleDSPlayer::invokeStopMainPlayer";
    QMetaObject::invokeMethod(viewRootObject, "stopMainPlayer");
    QTimer::singleShot(10000,this,SLOT(runAfterStop()));
}

void TeleDSPlayer::invokeSetTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, QString color3, bool tileMode, bool showTeleDSLogo)
{
    qDebug() << "TeleDSPlayer::invokeSetTheme";
    QVariant backgroundURLParam = QUrl(backgroundURL);
    QVariant logoURLParam = QUrl(logoURL);
    QVariant color1Param = QColor(color1);
    QVariant color2Param = QColor(color2);
    QVariant color3Param = QColor(color3);
    QVariant tileModeParam = tileMode;
    QVariant showTeleDSLogoParam = showTeleDSLogo;
    QMetaObject::invokeMethod(viewRootObject, "setTheme",
                              Q_ARG(QVariant, backgroundURLParam),
                              Q_ARG(QVariant, logoURLParam),
                              Q_ARG(QVariant, color1Param),
                              Q_ARG(QVariant, color2Param),
                              Q_ARG(QVariant, color3Param),
                              Q_ARG(QVariant, tileModeParam),
                              Q_ARG(QVariant, showTeleDSLogoParam));
    this->show();
}

void TeleDSPlayer::invokeSetMenuTheme(QString backgroundURL, QString logoURL, QString color1, QString color2, bool tileMode, bool showTeleDSLogo)
{
    //setMenuTheme(brandBGLogo, brandLogo, brandBGColor, brandFGColor, tileMode, showTeleDSLogo)
    qDebug() << "TeleDSPlayer::invokeSetMenuTheme";
    QVariant backgroundURLParam = QUrl(backgroundURL);
    QVariant logoUrlParam = QUrl(logoURL);
    QVariant color1Param = QColor(color1);
    QVariant color2Param = QColor(color2);
    QVariant tileModeParam = tileMode;
    QVariant showTeleDSLogoParam = showTeleDSLogo;

    QMetaObject::invokeMethod(viewRootObject, "setMenuTheme",
                              Q_ARG(QVariant, backgroundURLParam),
                              Q_ARG(QVariant, logoUrlParam),
                              Q_ARG(QVariant, color1Param),
                              Q_ARG(QVariant, color2Param),
                              Q_ARG(QVariant, tileModeParam),
                              Q_ARG(QVariant, showTeleDSLogoParam));
}

void TeleDSPlayer::invokeRestoreDefaultTheme()
{
    qDebug() << "TeleDSPlayer::invokeRestoreDefaultTheme";
    QMetaObject::invokeMethod(viewRootObject, "restoreDefaultTheme");
    this->show();
}

void TeleDSPlayer::invokeUpdateState()
{
    qDebug() << "TeleDSPlayer::invokeUpdateState";
    QMetaObject::invokeMethod(viewRootObject, "setUpdateState", Q_ARG(QVariant, QVariant(TeleDSVersion::getVersion())));
}

void TeleDSPlayer::invokeSetAreaCount(int areaCount)
{
    qDebug() << "TeleDSPlayer::invokeSetAreaCount";
    QMetaObject::invokeMethod(viewRootObject, "setAreaCount", Q_ARG(QVariant, QVariant(areaCount)));
}

void TeleDSPlayer::invokePlayCampaign(int campaignIndex)
{
    //for each of area in campaign - ask next Item and play it via next(areaID);
    config.currentCampaignId = campaignIndex;
    PlayerConfigAPI::Campaign currentCampaign = config.campaigns[campaignIndex];
    foreach (const PlayerConfigAPI::Campaign::Area &area, currentCampaign.areas)
        next(area.area_id);
}

void TeleDSPlayer::invokeInitArea(QString name, double campaignWidth, double campaignHeight, double x, double y, double w, double h, int rotation)
{
    qDebug() << "TeleDSPlayer::invokeInitArea";
    QMetaObject::invokeMethod(viewRootObject, "prepareArea",
                              Q_ARG(QVariant, QVariant(name)),
                              Q_ARG(QVariant, QVariant(campaignWidth)),
                              Q_ARG(QVariant, QVariant(campaignHeight)),
                              Q_ARG(QVariant, QVariant(x)),
                              Q_ARG(QVariant, QVariant(y)),
                              Q_ARG(QVariant, QVariant(w)),
                              Q_ARG(QVariant, QVariant(h)),
                              Q_ARG(QVariant, QVariant(rotation)));

}

void TeleDSPlayer::invokeSetPlayerVolume(int value)
{
    QMetaObject::invokeMethod(viewRootObject, "setPlayerVolume", Q_ARG(QVariant,QVariant(value/100.0)));
}

void TeleDSPlayer::invokeSetLicenseData()
{
    qDebug() << "TeleDSPlayer::invokeSetLicenseData";
    QVariant eulaParam = StaticTextService.getEula();
    QString eula = StaticTextService.getEula();
    qDebug() << "TeleDSPlayer::invoke eula(" + QString::number(eula.length()) + ")";
    QVariant privacyPolicyParam = StaticTextService.getPrivacyPolicy();
    QVariant opensourceParam = StaticTextService.getOpenSource();
    QVariant legalParam = StaticTextService.getLegal();
    QMetaObject::invokeMethod(viewRootObject, "setLicenseText",
                              Q_ARG(QVariant, eulaParam),
                              Q_ARG(QVariant, privacyPolicyParam),
                              Q_ARG(QVariant, opensourceParam),
                              Q_ARG(QVariant, legalParam));
}

void TeleDSPlayer::invokeSetDeviceInfo()
{

    qDebug() << "TeleDSPlayer::invokeSetDeviceInfo";
    SettingsRequestResult settings = SettingsRequestResult::fromJson(GlobalConfigInstance.getSettings());
    QVariant nameParam = settings.name;
    QVariant connectionName = PlatformSpecificService.getConnectionName();
    QMetaObject::invokeMethod(viewRootObject, "setDeviceInfo",
                              Q_ARG(QVariant, nameParam),
                              Q_ARG(QVariant, connectionName));
}

void TeleDSPlayer::invokeSetDisplayMode(QString mode)
{
    qDebug() << "TeleDSPlayer::invokeSetDisplayMode -> " << mode;
    QVariant modeParam = mode;
    if (mode == "fullscreen")
        invokeSetContentPosition();

    QMetaObject::invokeMethod(viewRootObject, "setDisplayMode",
                              Q_ARG(QVariant, modeParam));
}

void TeleDSPlayer::invokeSetContentPosition(float contentLeft, float contentTop, float contentWidth, float contentHeight,
                                            float widgetLeft, float widgetTop, float widgetWidth, float widgetHeight)
{
    qDebug() << "TeleDSPlayer::invokeSetContentPosition";
    QMetaObject::invokeMethod(viewRootObject, "setContentPosition",
                              Q_ARG(QVariant, QVariant(contentLeft)),
                              Q_ARG(QVariant, QVariant(contentTop)),
                              Q_ARG(QVariant, QVariant(contentWidth)),
                              Q_ARG(QVariant, QVariant(contentHeight)),
                              Q_ARG(QVariant, QVariant(widgetLeft)),
                              Q_ARG(QVariant, QVariant(widgetTop)),
                              Q_ARG(QVariant, QVariant(widgetWidth)),
                              Q_ARG(QVariant, QVariant(widgetHeight)));
}

void TeleDSPlayer::runAfterStop()
{
    /*
    qDebug() << "TeleDSPlayer::runAfterStop";
    bool haveNext = playlist->haveNext();
    next();
    if (haveNext)
    {
        qDebug() << "TeleDSPlayer::runAfterStop -> we have next item";
        QTimer::singleShot(1000,this,SLOT(next()));
    }*/
}

void TeleDSPlayer::next(QString area_id)
{
    qDebug() << "TeleDSPlayer::next(" << area_id;

    if (shouldStop)
    {
        shouldStop = false;
        emit readyToUpdate();
        return;
    }
    if (isActive)
    {
        qDebug() << "next method is called";
        if (delay == 0)
            playNextGeneric(area_id);
        else
        {
            QTimer::singleShot(1000, [this, area_id]() { playNextGeneric(area_id); } );
            hideVideo();
            status.isPlaying = false;
            status.item = "";
        }
    }
    else
    {
        qDebug() << "Player is not active, so no next item";
    }
}

void TeleDSPlayer::playNextGeneric(QString area_id)
{
    qDebug() <<QDateTime::currentDateTime().time();
    qDebug() << "playNextGeneric!";
    if (!playlists.contains(area_id))
    {
        qDebug() << "playlist " << area_id << " not found";
        return;
    }
    //call haveNext instead
    //if haveNext has none then dont play anything
    if (playlists[area_id]->haveNext())
    {
        QString nextItem = playlists[area_id]->getStoredItem();
        invokeNextVideoMethodAdvanced(nextItem,area_id);
    /*    if (GlobalConfigInstance.isAutoBrightnessActive())
        {
            SunsetSystem sunSystem;

            double minBrightness = std::min(GlobalConfigInstance.getMinBrightness(), GlobalConfigInstance.getMaxBrightness());
            double maxBrightness = std::max(GlobalConfigInstance.getMinBrightness(), GlobalConfigInstance.getMaxBrightness());
            double brightnessValue = sunSystem.getSinPercent() * (maxBrightness - minBrightness) + minBrightness;
            brightnessValue = std::max(std::min(brightnessValue, maxBrightness), minBrightness);
            qDebug() << "BRIGHTNESS = " << brightnessValue;
            if (brightnessValue/100. < 0)
                setBrightness(1.0);
            else
                setBrightness(brightnessValue/100.);
        }
        else
            setBrightness(1.0);*/
        playedIds.enqueue(nextItem);
        PlatformSpecificService.generateSystemInfo();

        status.isPlaying = true;
        status.item = nextItem;
        GlobalStatsInstance.setCurrentItem(nextItem);

        showVideo();
    }
    else
    {
        qDebug() << "Playlist doesnt have next item: stopping!";
        invokePrepareStop();

        //this->playNextGeneric(area_id);
         checkNextVideoAfterStopTimer->setProperty("area_id", area_id);
         checkNextVideoAfterStopTimer->setProperty("activated", true);
        //we need to stop playback after last item end
        //invoke prepare stop
    }

    qDebug() <<QDateTime::currentDateTime().time();
}

void TeleDSPlayer::bindObjects()
{
    qDebug() << "binding QML and C++";
    connect(&PlatformSpecificService,SIGNAL(systemInfoReady(Platform::SystemInfo)),this,SLOT(systemInfoReady(Platform::SystemInfo)));
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
    QObject::connect(viewRootObject,SIGNAL(refreshId()), this, SIGNAL(refreshNeeded()));
    QObject::connect(viewRootObject,SIGNAL(gpsChanged(double,double)),this,SLOT(gpsUpdate(double,double)));
    QObject::connect(viewRootObject,SIGNAL(setRestoreModeTrue()),this,SLOT(setRestoreModeTrue()));
    QObject::connect(viewRootObject,SIGNAL(setRestoreModeFalse()),this, SLOT(setRestoreModeFalse()));
    QObject::connect(viewRootObject,SIGNAL(nextItem(QString)), this, SLOT(next(QString)));
}

void TeleDSPlayer::stopPlaying()
{
    isActive = false;
    status.isPlaying = false;
    invokeStop();
    invokeNoItemsView("http://teleds.com");
}

void TeleDSPlayer::setRestoreModeTrue()
{
    Platform::PlatformSpecific::setResetWindow(true);
}

void TeleDSPlayer::setRestoreModeFalse()
{
    Platform::PlatformSpecific::setResetWindow(false);
}

void TeleDSPlayer::showVideo()
{
    invokeShowVideo(true);
}

void TeleDSPlayer::hideVideo()
{
    invokeShowVideo(false);
}

void TeleDSPlayer::setBrightness(double value)
{
    QVariant brightness(value);
    QMetaObject::invokeMethod(viewRootObject,"setBrightness",Q_ARG(QVariant, brightness));
}

void TeleDSPlayer::gpsUpdate(double lat, double lgt)
{
    GlobalStatsInstance.setGps(lat, lgt);
}

void TeleDSPlayer::systemInfoReady(Platform::SystemInfo info)
{
    foreach (const QString &areaId, playlists.keys())
    {
        auto playlist = playlists[areaId];
        if (playlist){
            if (!playedIds.isEmpty())
            {
                auto item = playlists[areaId]->findItemById(playedIds.head());
                if (item.content_id != "")
                {
                    DatabaseInstance.createPlayEvent(item, info);
                    playedIds.dequeue();
                    break;
                }
            }
        }
    }
}

void TeleDSPlayer::nextCampaignEvent()
{
    qDebug() << "nextCampaignEvent";
    this->invokeStop();
    checkNextVideoAfterStopTimer->stop();
    this->play(100);
}

void TeleDSPlayer::nextItemEvent()
{
    if (checkNextVideoAfterStopTimer->property("activated").toBool())
    {
        checkNextVideoAfterStopTimer->setProperty("activated", false);
        this->playNextGeneric(checkNextVideoAfterStopTimer->property("area_id").toString());
    }
}

void TeleDSPlayer::invokeShowVideo(bool isVisible)
{
    //qDebug() << "invoking video visibility change -> " + (isVisible ? QString("true") : QString("false"));
    QVariant isVisibleArg(isVisible);
    QMetaObject::invokeMethod(viewRootObject,"showVideo",Q_ARG(QVariant, isVisibleArg));
}

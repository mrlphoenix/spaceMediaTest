#include <QFileInfo>
#include <QProcess>
#include "teledsplayer.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "sunposition.h"
#include "version.h"
#include "statictext.h"

TeleDSPlayer::TeleDSPlayer(QObject *parent) : QObject(parent)
{
    playlist = 0;
    widgetPlaylist = 0;

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
}

TeleDSPlayer::~TeleDSPlayer()
{
    if (playlist)
        delete playlist;
}

QString TeleDSPlayer::getFullPath(QString fileName)
{
    QString nextFile = VIDEO_FOLDER + fileName + playlist->findItemById(fileName).getExtension();
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


void TeleDSPlayer::update(PlayerConfig config)
{
    foreach (const PlayerConfig::VirtualScreen &screen, config.screens)
        if (screen.id == this->config.id)
            setConfig(screen);
}

void TeleDSPlayer::setConfig(PlayerConfig::VirtualScreen area)
{
    qDebug() << "TeleDSPlayer::set config: ";
    config = area.playlist;
    if (playlist == NULL)
        playlist = new SuperPlaylist(this);
    isPlaylistRandom = true;

    playlist->updatePlaylist(area.playlist);
}

void TeleDSPlayer::setConfig(PlayerConfig::VirtualScreen contentArea, PlayerConfig::VirtualScreen widgetArea)
{
    qDebug() << "TeleDS::setConfig split-screen";
    config = contentArea.playlist;
    widgetConfig = widgetArea.playlist;
    if (playlist == NULL)
        playlist = new SuperPlaylist(this);
    if (widgetPlaylist == NULL)
        widgetPlaylist = new SuperPlaylist(this);
    isPlaylistRandom = true;
    playlist->updatePlaylist(contentArea.playlist);
    widgetPlaylist->updatePlaylist(widgetArea.playlist);
    qDebug() << "SET CONFIG-> " << contentArea.playlist.items.count() << widgetArea.playlist.items.count();
}

void TeleDSPlayer::play()
{
    isActive = true;
    QTimer::singleShot(1000,this,SLOT(next()));
}

bool TeleDSPlayer::isFileCurrentlyPlaying(QString name)
{
    return status.item == name;
}

void TeleDSPlayer::invokeNextVideoMethod(QString name)
{
    qDebug() << "invoking next";
    QVariant source = QUrl(getFullPath(name));
    qDebug() << source;
    QMetaObject::invokeMethod(viewRootObject,"playFile",Q_ARG(QVariant,source));
}

void TeleDSPlayer::invokeNextVideoMethodAdvanced(QString name, bool isWidget)
{
    PlaylistAPIResult::PlaylistItem item;
    if (isWidget)
        item = widgetPlaylist->findItemById(name);
    else
        item = playlist->findItemById(name);
    qDebug() << "invoking next method::advanced -> " << item.name;
    QVariant source;
    if (item.type == "video" || item.type == "audio")
        source = QUrl(getFullPath(name));
    else if (item.type == "html5_online")
        source = item.fileUrl;
    else if (item.type == "html5_zip")
        source = getFullPathZip(VIDEO_FOLDER + item.id + "/index.html");

    QVariant type;
    if (item.type == "html5_zip")
        type = "html5_online";
    else
        type = item.type;

    QVariant build = CONFIG_BUILD_NAME;
    QVariant duration = item.duration;
    QVariant skip = item.skipTime;
    QString methodName = isWidget ? "playWidget" : "playFileAdvanced";
    QMetaObject::invokeMethod(viewRootObject, methodName.toStdString().data(),
                              Q_ARG(QVariant,source),
                              Q_ARG(QVariant, type),
                              Q_ARG(QVariant, build),
                              Q_ARG(QVariant, duration),
                              Q_ARG(QVariant, skip));
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

void TeleDSPlayer::invokeEnablePreloading()
{
    static bool invokedOnce = false;
    if (invokedOnce)
        return;
    QString nextItem = playlist->next();
//    DatabaseInstance.playResource(playlist->findItemById(nextItem));

    QVariant nextItemParam = QUrl(getFullPath(nextItem));
    QMetaObject::invokeMethod(viewRootObject, "enablePreloading", Q_ARG(QVariant, nextItemParam));
    invokedOnce = true;
}

void TeleDSPlayer::invokeStop()
{
    qDebug() << "TeleDSPlayer::invokeStop";
    QMetaObject::invokeMethod(viewRootObject, "stopPlayer");
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
                              Q_ARG(QVariant, legalParam))  ;
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

void TeleDSPlayer::next()
{
    if (isActive)
    {
        qDebug() << "next method is called";
        if (delay == 0)
            playNext();
        else
        {
            QTimer::singleShot(delay,this,SLOT(playNext()));
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

void TeleDSPlayer::nextWidget()
{
    if (isActive)
    {
        qDebug() << " next widget method is called";
        if (delay == 0)
            playNextWidget();
        else
        {
            QTimer::singleShot(delay,this,SLOT(playNextWidget()));
            hideVideo();
            status.isPlaying = false;
            status.item = "";
        }
    }
    else
    {
        qDebug() << "Player is not active, so no next widget";
    }
}

void TeleDSPlayer::playNextGeneric(bool isWidget)
{
    if (!playlist)
    {
        qDebug() << "playlist empty";
        return;
    }
    QString nextItem;
    if (isWidget && widgetPlaylist)
        nextItem = widgetPlaylist->next();
    else
        nextItem = playlist->next();
    invokeNextVideoMethodAdvanced(nextItem, isWidget);
    if (GlobalConfigInstance.isAutoBrightnessActive())
    {
        SunsetSystem sunSystem;
        int minBrightness = std::min(GlobalConfigInstance.getMinBrightness(), GlobalConfigInstance.getMaxBrightness());
        int maxBrightness = std::max(GlobalConfigInstance.getMinBrightness(), GlobalConfigInstance.getMaxBrightness());
        double originalValue = sunSystem.getLinPercent();
        double brightnessValue = sunSystem.getSinPercent() * (maxBrightness - minBrightness) + minBrightness;
        qDebug() <<"Autobrightness is active with value: LINEAR= " + QString::number(originalValue) + " , SIN= " + QString::number(brightnessValue);
        if (brightnessValue/100. < 0.05)
            setBrightness(1.0);
        else
            setBrightness(brightnessValue/100.);
    }
    qDebug() << "inserting into database PLAY";
    playedIds.enqueue(nextItem);
    PlatformSpecificService.generateSystemInfo();

    status.isPlaying = true;
    status.item = nextItem;
    GlobalStatsInstance.setCurrentItem(nextItem);

    showVideo();
}

void TeleDSPlayer::bindObjects()
{
    qDebug() << "binding QML and C++";
    connect(&PlatformSpecificService,SIGNAL(systemInfoReady(Platform::SystemInfo)),this,SLOT(systemInfoReady(Platform::SystemInfo)));
    QObject::connect(viewRootObject,SIGNAL(nextItem()),this, SLOT(next()));
    QObject::connect(viewRootObject,SIGNAL(nextWidget()),this,SLOT(nextWidget()));
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
    QObject::connect(viewRootObject,SIGNAL(refreshId()), this, SIGNAL(refreshNeeded()));
    QObject::connect(viewRootObject,SIGNAL(gpsChanged(double,double)),this,SLOT(gpsUpdate(double,double)));
    QObject::connect(viewRootObject,SIGNAL(setRestoreModeTrue()),this,SLOT(setRestoreModeTrue()));
    QObject::connect(viewRootObject,SIGNAL(setRestoreModeFalse()),this, SLOT(setRestoreModeFalse()));
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
    qDebug() << "TeleDSPlayer::setRestoreModeTrue";
    Platform::PlatformSpecific::setResetWindow(true);
}

void TeleDSPlayer::setRestoreModeFalse()
{
    qDebug() << "TeleDSPlayer::setRestoreModeFalse";
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
    qDebug() << "invoking Brightness setup";
    QVariant brightness(value);
    QMetaObject::invokeMethod(viewRootObject,"setBrightness",Q_ARG(QVariant, brightness));
}

void TeleDSPlayer::gpsUpdate(double lat, double lgt)
{
    GlobalStatsInstance.setGps(lat, lgt);
}

void TeleDSPlayer::systemInfoReady(Platform::SystemInfo info)
{
    qDebug() << "systemInfoReady";
    DatabaseInstance.createPlayEvent(playlist->findItemById(playedIds.dequeue()), info);
}

void TeleDSPlayer::invokeShowVideo(bool isVisible)
{
    qDebug() << "invoking video visibility change -> " + (isVisible ? QString("true") : QString("false"));
    QVariant isVisibleArg(isVisible);
    QMetaObject::invokeMethod(viewRootObject,"showVideo",Q_ARG(QVariant, isVisibleArg));
}

#include <QFileInfo>
#include "teledsplayer.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "platformspecs.h"
#include "sunposition.h"
#include "version.h"

TeleDSPlayer::TeleDSPlayer(PlayerConfig::Area config, QObject *parent) : QObject(parent)
{
#ifdef PLATFORM_DEFINE_ANDROID
    QTimer * trafficDisplay = new QTimer(this);
    QObject::connect(trafficDisplay,SIGNAL(timeout()),this,SLOT(invokeDisplayTrafficUpdate()));
    trafficDisplay->start(10000);
#endif
    playlist = 0;
    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);
    this->config = config;
    setConfig(config);

    view.setSource(QUrl(QStringLiteral("qrc:/main_player.qml")));
    viewRootObject = dynamic_cast<QObject*>(view.rootObject());
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QTimer::singleShot(1000,this,SLOT(bindObjects()));
    QTimer::singleShot(1000,this,SLOT(next()));
    QTimer::singleShot(1000,this,SLOT(invokeVersionText()));

#ifdef PLAYER_MODE_WINDOWED
    view.show();
    view.setMinimumHeight(520);
    view.setMinimumWidth(920);
#else
    view.showFullScreen();
#endif
    delay = 0;
    status.isPlaying = false;
    status.item = "";
}

TeleDSPlayer::TeleDSPlayer(QObject *parent) : QObject(parent)
{
#ifdef PLATFORM_DEFINE_ANDROID
    QTimer * trafficDisplay = new QTimer(this);
    QObject::connect(trafficDisplay,SIGNAL(timeout()),this,SLOT(invokeDisplayTrafficUpdate()));
    trafficDisplay->start(10000);
#endif
    playlist = 0;
    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);

    view.setSource(QUrl(QStringLiteral("qrc:/main_player.qml")));
    viewRootObject = dynamic_cast<QObject*>(view.rootObject());
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QTimer::singleShot(1000,this,SLOT(bindObjects()));
    QTimer::singleShot(500,this,SLOT(invokeVersionText()));

#ifdef PLAYER_MODE_WINDOWED
    view.show();
    view.setMinimumHeight(520);
    view.setMinimumWidth(920);
#else
    view.showFullScreen();
#endif

    delay = 0000;
    status.isPlaying = false;
    status.item = "";
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

void TeleDSPlayer::update(PlayerConfig config)
{
    foreach (const PlayerConfig::Area& area, config.areas)
        if (area.id == this->config.id)
            setConfig(area);
}

void TeleDSPlayer::setConfig(PlayerConfig::Area area)
{
    area.id = 0;
  /*  qDebug() << "RPI PLAYER: given playlist = " << area.playlist.type;
    config = area;
    if (playlist)
    {
        if (area.playlist.type == "random" && playlist->getType() != "random")
        {
            qDebug() << "RPI Player:: playlist STANDART -> RANDOM";
            playlist->deleteLater();
            playlist = new RandomPlaylist(this);
            isPlaylistRandom = true;
        }
        else if (area.playlist.type != "random" && playlist->getType() == "random")
        {
            qDebug() << "RPI Player:: playlist RANDOM -> STANDART";
            playlist->deleteLater();
            playlist = new StandartPlaylist(this);
            isPlaylistRandom = false;
        }
    }
    else if (area.playlist.type == "random")
    {
        qDebug() << "RPI Player:: playlist RANDOM";
        playlist = new RandomPlaylist(this);
        isPlaylistRandom = true;
    }
    else
    {
        qDebug() << "RPI Player:: playlist STANDART";
        playlist = new StandartPlaylist(this);
        isPlaylistRandom = false;
    }
    playlist->updatePlaylist(area.playlist);*/

}

void TeleDSPlayer::setConfig(PlayerConfigNew::VirtualScreen area)
{
    qDebug() << "TeleDSPlayer::set config: ";
    configNew = area.playlist;
    if (playlist)
    {
        if (area.type == "random" && playlist->getType() != "random")
        {
            qDebug() << "RPI Player::playlist Standart -> RANDOM";
            playlist->deleteLater();
            playlist = new MagicRandomPlaylist(this);
            isPlaylistRandom = true;
        }
        else if (area.type != "random" && playlist->getType() == "random")
        {
            qDebug() << "RPI Player::playlist RANDOM -> STANDART";
            playlist->deleteLater();
            playlist = new StandartPlaylist(this);
            isPlaylistRandom = false;
        }
    }
    else if (area.type == "random")
    {
        qDebug() << "RPI Player::playlist RANDOM";
        playlist = new MagicRandomPlaylist(this);
        isPlaylistRandom = true;
    }
    else
    {
        qDebug() << "RPI Player::playlist STANDART";
        playlist = new StandartPlaylist(this);
        isPlaylistRandom = false;
    }
    playlist->updatePlaylist(area.playlist);
}

void TeleDSPlayer::play()
{
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

void TeleDSPlayer::invokeNextVideoMethodAdvanced(QString name)
{
    qDebug() << "invoking next method::advanced";
    PlaylistAPIResult::PlaylistItem item = playlist->findItemById(name);

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
    QVariant duration = item.duration * 1000;
    QVariant skip = item.skipTime * 1000;
    QMetaObject::invokeMethod(viewRootObject,"playFileAdvanced",
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
    QMetaObject::invokeMethod(viewRootObject,"setNeedActivationLogo",Q_ARG(QVariant, urlParam), Q_ARG(QVariant, playerIdParam), Q_ARG(QVariant, updateDelayParam));
}

void TeleDSPlayer::invokeNoItemsView(QString url)
{
    qDebug() << "invokeNoItemsView";
    QVariant urlParam(url);
    QMetaObject::invokeMethod(viewRootObject,"setNoItemsLogo", Q_ARG(QVariant, urlParam));
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

void TeleDSPlayer::next()
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

void TeleDSPlayer::playNext()
{
    if (!playlist)
    {
        qDebug() << "playlist empty";
        return;
    }
    QString nextItem = playlist->next();
    invokeNextVideoMethodAdvanced(nextItem);
    //invokeNextVideoMethod(nextItem);
    if (GlobalConfigInstance.isAutoBrightnessActive())
    {
        SunsetSystem sunSystem;
        double originalValue = sunSystem.getLinPercent();
        double brightnessValue = sunSystem.getSinPercent() * (GlobalConfigInstance.getMaxBrightness() - GlobalConfigInstance.getMinBrightness()) + GlobalConfigInstance.getMinBrightness();
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
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
    QObject::connect(viewRootObject,SIGNAL(refreshId()), this, SIGNAL(refreshNeeded()));
    QObject::connect(viewRootObject,SIGNAL(gpsChanged(double,double)),this,SLOT(gpsUpdate(double,double)));
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

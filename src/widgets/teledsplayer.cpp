#include <QFileInfo>
#include "teledsplayer.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "platformspecs.h"

TeleDSPlayer::TeleDSPlayer(PlayerConfig::Area config, QObject *parent) : QObject(parent)
{
#ifdef PLATFORM_DEFINE_ANDROID
    PlatformSpecs specs;
    qDebug() << "Unique id: " << specs.getUniqueId();
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
    view.showFullScreen();
    delay = 0;
    status.isPlaying = false;
    status.item = "";
}

TeleDSPlayer::TeleDSPlayer(QObject *parent) : QObject(parent)
{
#ifdef PLATFORM_DEFINE_ANDROID
    PlatformSpecs specs;
    qDebug() << "Unique id: " << specs.getUniqueId();
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
    view.showFullScreen();


    delay = 5000;
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
    QString nextFile = VIDEO_FOLDER + fileName + ".mp4";
    QFileInfo fileInfo(nextFile);
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
    qDebug() << "RPI PLAYER: given playlist = " << area.playlist.type;
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

void TeleDSPlayer::invokeDisplayTrafficUpdate()
{
#ifdef PLATFORM_DEFINE_ANDROID
    qlonglong in = PlatformSpecs::getTrafficIn();
    qlonglong out = PlatformSpecs::getTrafficOut();
    int memory = PlatformSpecs::getMemoryUsage();
    double cpuLoad = PlatformSpecs::getAvgUsage();
    QVariant inParam(in);
    QVariant outParam(out);
    QVariant memoryParam(memory);
    QVariant cpuLoadParam(cpuLoad);
    QMetaObject::invokeMethod(viewRootObject, "displayTrafficInfo",
                              Q_ARG(QVariant, inParam),
                              Q_ARG(QVariant, outParam),
                              Q_ARG(QVariant, memoryParam),
                              Q_ARG(QVariant, cpuLoadParam));
#endif
}

void TeleDSPlayer::next()
{
    qDebug() << "next method is called";
    QTimer::singleShot(delay,this,SLOT(playNext()));
    hideVideo();
    status.isPlaying = false;
    status.item = "";
}

void TeleDSPlayer::playNext()
{
    if (!playlist)
    {
        qDebug() << "playlist empty";
        return;
    }
    QString nextItem = playlist->next();
    invokeNextVideoMethod(nextItem);

    qDebug() << "inserting into database PLAY";
    DatabaseInstance.playResource(config.id,config.playlist.id,nextItem,0.0,0.0);
    status.isPlaying = true;
    status.item = nextItem;
    GlobalStatsInstance.setCurrentItem(nextItem);

    showVideo();
}

void TeleDSPlayer::bindObjects()
{
    qDebug() << "binding QML and C++";
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

void TeleDSPlayer::invokeShowVideo(bool isVisible)
{
    qDebug() << "invoking video visibility change";
    QVariant isVisibleArg(isVisible);
    QMetaObject::invokeMethod(viewRootObject,"showVideo",Q_ARG(QVariant, isVisibleArg));
}

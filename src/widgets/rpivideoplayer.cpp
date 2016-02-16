#include <QFileInfo>
#include "rpivideoplayer.h"
#include "statisticdatabase.h"
#include "androidspecs.h"

RpiVideoPlayer::RpiVideoPlayer(PlayerConfig::Area config, QObject *parent) : QObject(parent)
{
#ifdef PLATFORM_DEFINE_ANDROID
    AndroidSpecs specs;
    qDebug() << "IMEEEEI!!!!!!: " << specs.getImei();
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

RpiVideoPlayer::RpiVideoPlayer(QObject *parent) : QObject(parent)
{
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
    setBrightness(0.6);
}

RpiVideoPlayer::~RpiVideoPlayer()
{
    if (playlist)
        delete playlist;
}

QString RpiVideoPlayer::getFullPath(QString fileName)
{
    QString nextFile = VIDEO_FOLDER + fileName + ".mp4";
    QFileInfo fileInfo(nextFile);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}

void RpiVideoPlayer::update(PlayerConfig config)
{
    foreach (const PlayerConfig::Area& area, config.areas)
        if (area.id == this->config.id)
            setConfig(area);
}

void RpiVideoPlayer::setConfig(PlayerConfig::Area area)
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

void RpiVideoPlayer::play()
{
    QTimer::singleShot(1000,this,SLOT(next()));
}

void RpiVideoPlayer::invokeNextVideoMethod(QString name)
{
    qDebug() << "invoking next";
    QVariant source = QUrl(getFullPath(name));
    qDebug() << source;
    QMetaObject::invokeMethod(viewRootObject,"playFile",Q_ARG(QVariant,source));
}

void RpiVideoPlayer::invokeFileProgress(double p, QString name)
{
    qDebug() << "invoking file progress";
    QVariant percent(p);
    QVariant fileName(name);
    QMetaObject::invokeMethod(viewRootObject,"setTotalProgress",Q_ARG(QVariant, percent), Q_ARG(QVariant, fileName));
}

void RpiVideoPlayer::invokeProgress(double p)
{
    qDebug() << "invoking download progress";
    QVariant percent(p);
    QMetaObject::invokeMethod(viewRootObject,"setProgress",Q_ARG(QVariant, percent));
}

void RpiVideoPlayer::invokeDownloadDone()
{
    qDebug() << "invoking download completed";
    QMetaObject::invokeMethod(viewRootObject,"downloadComplete");
}

void RpiVideoPlayer::next()
{
    qDebug() << "next method is called";
    QTimer::singleShot(delay,this,SLOT(playNext()));
    hideVideo();
    status.isPlaying = false;
    status.item = "";
}

void RpiVideoPlayer::playNext()
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
    showVideo();
}

void RpiVideoPlayer::bindObjects()
{
    qDebug() << "binding QML and C++";
    QObject::connect(viewRootObject,SIGNAL(nextItem()),this, SLOT(next()));
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
}

void RpiVideoPlayer::showVideo()
{
    invokeShowVideo(true);
}

void RpiVideoPlayer::hideVideo()
{
    invokeShowVideo(false);
}

void RpiVideoPlayer::setBrightness(double value)
{
    qDebug() << "invoking Brightness setup";
    QVariant brightness(value);
    QMetaObject::invokeMethod(viewRootObject,"setBrightness",Q_ARG(QVariant, brightness));
}

void RpiVideoPlayer::invokeShowVideo(bool isVisible)
{
    qDebug() << "invoking video visibility change";
    QVariant isVisibleArg(isVisible);
    QMetaObject::invokeMethod(viewRootObject,"showVideo",Q_ARG(QVariant, isVisibleArg));
}

#include <QFileInfo>
#include "rpivideoplayer.h"
#include "statisticdatabase.h"
#include "platformdefines.h"

RpiVideoPlayer::RpiVideoPlayer(PlayerConfig::Area config, QObject *parent) : QObject(parent)
{
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
    config = area;
    if (playlist)
    {
        if (area.playlist.type == "random" && playlist->getType() != "random")
        {
            playlist->deleteLater();
            playlist = new RandomPlaylist(this);
            isPlaylistRandom = true;
        }
        else if (area.playlist.type != "random" && playlist->getType() == "random")
        {
            playlist->deleteLater();
            playlist = new StandartPlaylist(this);
            isPlaylistRandom = false;
        }
    }
    else if (area.playlist.type == "random")
    {
        playlist = new RandomPlaylist(this);
        isPlaylistRandom = true;
    }
    else
    {
        playlist = new StandartPlaylist(this);
        isPlaylistRandom = false;
    }
    playlist->updatePlaylist(area.playlist);
}

void RpiVideoPlayer::invokeNextVideoMethod(QString name)
{
    qDebug() << "invoking next";
    QVariant source = QUrl(getFullPath(name));
    qDebug() << source;
    QMetaObject::invokeMethod(viewRootObject,"playFile",Q_ARG(QVariant,source));
}

void RpiVideoPlayer::next()
{
    qDebug() << "next method is called";
    if (!playlist)
    {
        qDebug() << "playlist empty";
        return;
    }
    QString nextItem = playlist->next();
    invokeNextVideoMethod(nextItem);

    qDebug() << "inserting into database PLAY";
    DatabaseInstance.playResource(config.id,config.playlist.id,nextItem,0.0,0.0);
}

void RpiVideoPlayer::bindObjects()
{
    qDebug() << "bind Next Item";
    QObject::connect(viewRootObject,SIGNAL(nextItem()),this, SLOT(next()));
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
}

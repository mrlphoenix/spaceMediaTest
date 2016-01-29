#include <QFileInfo>
#include "rpivideoplayer.h"
#include "statisticdatabase.h"

RpiVideoPlayer::RpiVideoPlayer(PlayerConfig::Area config, QObject *parent) : QObject(parent)
{
    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);
    this->config = config;
    setConfig(config);

    view.setSource(QUrl(QStringLiteral("qrc:///simple_player.qml")));
    viewRootObject = dynamic_cast<QObject*>(view.rootObject());
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QTimer::singleShot(1000,this,SLOT(bindObjects()));
    QTimer::singleShot(1000,this,SLOT(next()));
    view.showFullScreen();
}

RpiVideoPlayer::~RpiVideoPlayer()
{

}

QString RpiVideoPlayer::getFullPath(QString fileName)
{
    QString nextFile = "data/video/" + fileName + ".mp4";
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
    if (area.playlist.type == "random")
    {
        randomPlaylist.updatePlaylist(area.playlist);
        isPlaylistRandom = true;
    }
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
    QString nextItem;
    if (isPlaylistRandom)
    {
        qDebug() << "playlist is trandom";
        nextItem = randomPlaylist.next();
        invokeNextVideoMethod(nextItem);
    }
    qDebug() << "inserting into database PLAY";
    DatabaseInstance.playResource(config.id,config.playlist.id,nextItem,0.0,0.0);
}

void RpiVideoPlayer::bindObjects()
{
    qDebug() << "bind Next Item";
    QObject::connect(viewRootObject,SIGNAL(nextItem()),this, SLOT(next()));
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
}

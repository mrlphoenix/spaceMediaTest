#include <QFileInfo>
#include "rpivideoplayer.h"

rpiVideoPlayer::rpiVideoPlayer(QWidget *parent) : QVideoWidget(parent)
{
    player.setVideoOutput(this);
    connect(&player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(playerStateChanged(QMediaPlayer::State)));
}

void rpiVideoPlayer::setConfig(PlayerConfig::Area area)
{
    config = area;
    randomPlaylist.updatePlaylist(area.playlist);
}

void rpiVideoPlayer::play()
{
    if (config.playlist.type == "random")
        next();
}

void rpiVideoPlayer::next()
{
    QString nextFile = "data/video/" + randomPlaylist.next() + ".mp4";
    QFileInfo fileInfo(nextFile);
    player.setMedia(QMediaContent(QUrl::fromLocalFile(fileInfo.absoluteFilePath())));
    player.play();
}

void rpiVideoPlayer::update(PlayerConfig config)
{
    foreach (const PlayerConfig::Area& area, config.areas)
        if (area.id == area.id)
            setConfig(area);
}

void rpiVideoPlayer::playerStateChanged(QMediaPlayer::State state)
{
    if (config.playlist.type == "random")
    {
        if (state == QMediaPlayer::StoppedState)
        {
            next();
        }
    }
}


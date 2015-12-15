#include <QMediaContent>
#include <QFileInfo>
#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) : QVideoWidget(parent)
{

}

VideoPlayer::VideoPlayer(QStringList playlist, QWidget *parent) : QVideoWidget(parent)
{
    addToPlaylist(playlist);
    this->playlist.setCurrentIndex(1);
    this->playlist.setPlaybackMode(QMediaPlaylist::Loop);
    player.setPlaylist(&(this->playlist));
    player.setVideoOutput(this);
}

void VideoPlayer::addToPlaylist(QStringList fileNames)
{
    foreach (QString const &file, fileNames)
    {
        QFileInfo fileInfo(file);
        if (fileInfo.exists())
        {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            if (fileInfo.suffix().toLower() == QLatin1String("m3u"))
            {
                playlist.load(url);
            }
            else
                playlist.addMedia(url);
        }
        else
        {
            QUrl url(file);
            if (url.isValid())
                playlist.addMedia(url);
        }
    }
}

void VideoPlayer::play()
{
    player.play();
}

void VideoPlayer::stop()
{
    player.stop();
}


void VideoPlayer::setSound(int volume)
{
    player.setVolume(volume);
}

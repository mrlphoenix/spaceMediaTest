#ifndef RPIVIDEOPLAYER_H
#define RPIVIDEOPLAYER_H

#include <QObject>
#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaPlaylist>
#include "randomplaylist.h"

class rpiVideoPlayer : public QVideoWidget
{
    Q_OBJECT
public:
    explicit rpiVideoPlayer(QWidget *parent = 0);
    void setConfig(PlayerConfig::Area area);
    void play();

signals:

public slots:
    void next();
    void update(PlayerConfig config);
    void playerStateChanged(QMediaPlayer::State state);
private:
    QMediaPlayer player;
    QMediaPlaylist playlist;
    RandomPlaylist randomPlaylist;
    PlayerConfig::Area config;
};

#endif // RPIVIDEOPLAYER_H

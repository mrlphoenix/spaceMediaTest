#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QVideoWidget>
#include <QJsonObject>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <iwidgetinfo.h>

class VideoPlayer : public QVideoWidget,public IWidgetInfo
{
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = 0);
    VideoPlayer(QStringList playlist, QWidget *parent = 0);
    void addToPlaylist(QStringList fileNames);
    void play();
    void stop();

    virtual void setSound(int volume);

signals:

public slots:

private:
    QMediaPlaylist playlist;
    QMediaPlayer player;
};

#endif // VIDEOPLAYER_H

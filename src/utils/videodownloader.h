#ifndef VIDEODOWNLOADER_H
#define VIDEODOWNLOADER_H

#include <QObject>
#include <QWidget>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QLabel>
#include "videoserviceresult.h"

class VideoDownloader : public QObject
{
    Q_OBJECT
public:
    explicit VideoDownloader(PlayerConfig config, QObject *parent = 0);
    void setOutput(QProgressBar * total, QProgressBar * current, QLabel * textOut);
    void checkDownload();
    void start();

    static QString getFileHash(QString fileName);

signals:
    void done();
public slots:
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    void download();

    QProgressBar * total;
    QProgressBar * current;
    QLabel * textOut;

    QNetworkAccessManager manager;
    QNetworkReply * reply;
    PlayerConfig config;
    QFile * file;
    QVector<PlayerConfig::Area::Playlist::Item> itemsToDownload;
    int currentItemIndex;
};

#endif // VIDEODOWNLOADER_H

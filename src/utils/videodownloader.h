#ifndef VIDEODOWNLOADER_H
#define VIDEODOWNLOADER_H

#include <QObject>
#include <QWidget>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QLabel>
#include <QList>
#include <QTimer>
#include "videoserviceresult.h"
#include "statisticdatabase.h"



class FileSwapper : public QObject
{
    Q_OBJECT
public:
    explicit FileSwapper();
    void add(QString mainFile, QString tempFile);
    void start();
    void stop();
    struct SwapDefines
    {
        QString mainFile;
        QString tempFile;
    };
signals:
    void done();
private slots:
    void runSwapCycle();
private:
    QTimer swapTimer;
    QList<SwapDefines> needToSwap;
};

class VideoDownloader : public QObject
{
    Q_OBJECT
public:
    explicit VideoDownloader(PlayerConfig config, QObject *parent = 0);
    ~VideoDownloader();
    void prepareDownload();
    void checkDownload();
    void start();

    static QString getFileHash(QString fileName);
    void updateConfig(PlayerConfig config);
    int itemsToDownloadCount(){return itemsToDownload.count();}
    void getDatabaseInfo();

signals:
    void done();
    void downloadProgress(double p);
    void totalDownloadProgress(double p, QString name);
    void downloadProgressSingle(double p, QString name);
public slots:
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void getResources(QList<StatisticDatabase::Resource> resources);
private slots:
    void download();
private:
    bool isFileUpdated(PlayerConfig::Area::Playlist::Item item);

    QNetworkAccessManager manager;
    QNetworkReply * reply;
    PlayerConfig config;
    QFile * file;
    QVector<PlayerConfig::Area::Playlist::Item> itemsToDownload;
    QList<StatisticDatabase::Resource> resources;
    FileSwapper swapper;
    int currentItemIndex;
};

#endif // VIDEODOWNLOADER_H

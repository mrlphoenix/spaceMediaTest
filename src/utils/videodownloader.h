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

class VideoDownloaderWorker : public QObject
{
    Q_OBJECT
public:
    explicit VideoDownloaderWorker(PlayerConfig config, QObject *parent = 0);
    ~VideoDownloaderWorker();


    static QString getFileHash(QString fileName);

signals:
    void done();
    void downloadProgress(double p);
    void totalDownloadProgress(double p, QString name);
    void downloadProgressSingle(double p, QString name);
    void checkDownloadItemsTodownloadResult(int c);
public slots:
    void updateConfig(PlayerConfig config);
    int itemsToDownloadCount(){return itemsToDownload.count();}
    void prepareDownload();
    void start();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void getResources(QList<StatisticDatabase::Resource> resources);
    void getDatabaseInfo();
    void checkDownload();
private slots:
    void download();
    void connectError(QNetworkReply::NetworkError err);
    void runDonwload();
    static void writeToFileJob(QFile* f, QNetworkReply * r);
private:
    bool isFileUpdated(PlayerConfig::Area::Playlist::Item item);

    QNetworkAccessManager * manager;
    QNetworkReply * reply;
    PlayerConfig config;
    QFile * file;
    QVector<PlayerConfig::Area::Playlist::Item> itemsToDownload;
    QList<StatisticDatabase::Resource> resources;
    FileSwapper swapper;
    QTimer * restarter;
    int currentItemIndex;
};


class VideoDownloader : public QThread
{
    Q_OBJECT
public:
    VideoDownloader(PlayerConfig config, QObject *parent);
    ~VideoDownloader();


public slots:
    void runDownload();
    void prepareDownload(){worker->prepareDownload();}
    void checkDownload(){worker->checkDownload();}
    void startDownload(){worker->start();}
    int itemsToDownloadCount() {return worker->itemsToDownloadCount();}
    void updateConfig(PlayerConfig config){worker->updateConfig(config);}
signals:
    void done();
    void downloadProgressSingle(double p, QString name);
    void donwloadConfigResult(int c);
    void runDownloadSignal();

protected:
    void run();
private:
    VideoDownloaderWorker * worker;
};

#endif // VIDEODOWNLOADER_H

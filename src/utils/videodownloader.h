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
#include <QHash>

#include "videoserviceresult.h"
#include "statisticdatabase.h"

class QDateTime;

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

    QString getCacheFileHash(QString fileName);

    struct HashMeasure
    {
        QString hash;
        QDateTime date;
    };

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
    void runDownloadNew();
    static void writeToFileJob(QFile* f, QNetworkReply * r);
private:
    bool isFileUpdated(PlaylistAPIResult::PlaylistItem item);

    QNetworkAccessManager * manager;
    QNetworkReply * reply;
    PlayerConfig config;
    QFile * file;
    QVector<PlaylistAPIResult::PlaylistItem> itemsToDownload;
    QList<StatisticDatabase::Resource> resources;
    FileSwapper swapper;
    QTimer * restarter;
    int currentItemIndex;
    QHash<QString, HashMeasure> hashCache;
};


class VideoDownloader : public QThread
{
    Q_OBJECT
public:
    VideoDownloader(PlayerConfig config, QObject *parent);
    ~VideoDownloader();


public slots:
    void runDownload();
    void runDownloadNew();
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
    void runDownloadSignalNew();

protected:
    void run();
private:
    VideoDownloaderWorker * worker;
};



#endif // VIDEODOWNLOADER_H

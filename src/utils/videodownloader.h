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
    explicit VideoDownloaderWorker(PlayerConfigAPI config, QObject *parent = 0);
    ~VideoDownloaderWorker();


    static QString getFileHash(QString fileName);

    QString getCacheFileHash(QString fileName);

    struct HashMeasure
    {
        QString hash;
        QDateTime date;
    };

signals:
    void done(int count);
    void downloadProgress(double p);
    void totalDownloadProgress(double p, QString name);
    void downloadProgressSingle(double p, QString name);
    void checkDownloadItemsTodownloadResult(int c);
    void fileDownloaded(int index);
public slots:
    void updateConfig(PlayerConfigAPI config);
    int itemsToDownloadCount(){return itemsToDownload.count();}
    void prepareDownload();
    void start();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void getResources(QList<StatisticDatabase::Resource> resources);
    void getDatabaseInfo();
    void checkDownload();
    void onSslError(QNetworkReply* reply, QList<QSslError>);
    void onSslError(QList<QSslError> data);
private slots:
    void download();
    void connectError(QNetworkReply::NetworkError err);
    void runDonwload();
    void runDownloadNew();
    static void writeToFileJob(QFile* f, QNetworkReply * r);
private:
    QString updateHash(QString fileName);
    QNetworkAccessManager * manager;
    QNetworkReply * reply;
    PlayerConfigAPI config;
    QFile * file;
    QVector<PlayerConfigAPI::Campaign::Area::Content> itemsToDownload;
    QList<StatisticDatabase::Resource> resources;
    FileSwapper swapper;
    QTimer * restarter;
    int currentItemIndex;
    QHash<QString, HashMeasure> hashCache;
};

class UpdateDownloaderWorker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateDownloaderWorker(QObject * parent);
    ~UpdateDownloaderWorker();

signals:
    void ready(QString filename);

public slots:
    void setTask(QString url, QString hash, QString filename);
    void httpReadyRead();
    void httpFinished();

private:
    QFile * file;
    QNetworkAccessManager * manager;
    QNetworkReply * reply;
    QString fileName;
    QString hash;
    QString url;
};


class VideoDownloader : public QThread
{
    Q_OBJECT
public:
    VideoDownloader(PlayerConfigAPI config, QObject *parent);
    ~VideoDownloader();


public slots:
    void runDownload();
    void runDownloadNew();
    void prepareDownload(){worker->prepareDownload();}
    void checkDownload(){worker->checkDownload();}
    void startDownload(){worker->start();}
    int itemsToDownloadCount() {return worker->itemsToDownloadCount();}
    void updateConfig(PlayerConfigAPI config){worker->updateConfig(config);}

    void startUpdateTask(QString url, QString hash, QString filename);
signals:
    void done(int count);
    void downloadProgressSingle(double p, QString name);
    void donwloadConfigResult(int c);
    void runDownloadSignal();
    void runDownloadSignalNew();
    void fileDownloaded(int index);

    void startTask(QString url, QString hash, QString filename);
    void updateReady(QString filename);

protected:
    void run();
private:
    VideoDownloaderWorker * worker;
    UpdateDownloaderWorker * updateWorker;
};



#endif // VIDEODOWNLOADER_H

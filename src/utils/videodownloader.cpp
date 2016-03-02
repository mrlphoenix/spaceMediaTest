#include <QVector>
#include <QDebug>
#include <QFileInfo>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include "videodownloader.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "platformdefines.h"

VideoDownloaderWorker::VideoDownloaderWorker(PlayerConfig config, QObject *parent) : QObject(parent)
{
    this->config = config;
    file = 0;
    currentItemIndex = 0;
    connect (&DatabaseInstance,SIGNAL(resourceFound(QList<StatisticDatabase::Resource>)),this,SLOT(getResources(QList<StatisticDatabase::Resource>)));
    connect (&swapper,SIGNAL(done()),this,SLOT(download()));
    restarter = 0;
    manager = new QNetworkAccessManager(this);
}

VideoDownloaderWorker::~VideoDownloaderWorker()
{
    if (file)
        file->deleteLater();
    if (reply)
        reply->deleteLater();
}

void VideoDownloaderWorker::prepareDownload()
{
    getDatabaseInfo();
}

void VideoDownloaderWorker::checkDownload()
{
    int itemCount = 0;
    itemsToDownload.clear();

    foreach (const PlayerConfig::Area& area, config.areas)
        foreach(const PlayerConfig::Area::Playlist::Item& item, area.playlist.items)
        {
            if (isFileUpdated(item))
            {
                QString filename = VIDEO_FOLDER + item.iid + ".mp4";
                QString filehash;
                if (!QFile::exists(filename))
                {
                    qDebug() << "FILE DOES NOT EXISTS. " << item.iid <<" need to be downloaded";
                    itemsToDownload.append(item);
                }
                else if ((filehash = getFileHash(filename)) != item.sha1)
                {
                    qDebug() << "different hashes: " << filehash << " vs " << item.sha1;
                    itemsToDownload.append(item);
                }
                itemCount++;
            }

        }
    GlobalStatsInstance.setContentPlay(itemCount);
    qDebug() << "FILES NEED TO BE DOWNLOADED: " << itemsToDownload.count();
    emit checkDownloadItemsTodownloadResult(itemsToDownload.count());
}

void VideoDownloaderWorker::start()
{
    currentItemIndex = 0;
    download();
}

void VideoDownloaderWorker::download()
{
    if (itemsToDownload.count() > currentItemIndex)
    {
        qDebug() << "Downloading " + itemsToDownload[currentItemIndex].name;
        emit totalDownloadProgress(double(currentItemIndex+1)/double(itemsToDownload.count()),itemsToDownload[currentItemIndex].name);

        QString tempFileName = VIDEO_FOLDER + itemsToDownload[currentItemIndex].iid + ".mp4_";

        if (QFile::exists(tempFileName))
        {
            qDebug() << "temp file found. Resuming downloading";
            QFileInfo info(tempFileName);
            file = new QFile(tempFileName);
            file->open(QFile::Append);
            QNetworkRequest request(QUrl(itemsToDownload[currentItemIndex].path));

            QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(info.size()) + "-";
            request.setRawHeader("Range",rangeHeaderValue);
            reply = manager->get(request);
        }
        else
        {
            file = new QFile(tempFileName);
            file->open(QFile::WriteOnly);

            reply = manager->get(QNetworkRequest(QUrl(itemsToDownload[currentItemIndex].path)));
        }
        connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(connectError(QNetworkReply::NetworkError)));
        GlobalStatsInstance.registryDownload();

        DatabaseInstance.registryResource(itemsToDownload[currentItemIndex].iid,
                                          itemsToDownload[currentItemIndex].name,
                                          itemsToDownload[currentItemIndex].lastupdate,
                                          itemsToDownload[currentItemIndex].size);
    }
    else
    {
        qDebug() << "downloading completed";
        emit done();
    }
}

void VideoDownloaderWorker::connectError(QNetworkReply::NetworkError err)
{
    qDebug() << "Error! Connection lost!" << err;
  //  currentItemIndex--;
 //   if (restarter)
 //       delete restarter;
 //   reply->disconnect();
 //   restarter = new QTimer(this);
 //   restarter->start(10000);
 //   connect(restarter,SIGNAL(timeout()),this,SLOT(runDonwload()));
 //   file->flush();
 //   file->close();

 //   reply->deleteLater();
 //   file->deleteLater();
}

void VideoDownloaderWorker::runDonwload()
{
    qDebug() << "VDW: run Download";
    checkDownload();
    start();
}

void VideoDownloaderWorker::writeToFileJob(QFile *f, QNetworkReply *r)
{
    f->write(r->readAll());
    f->flush();
}

bool VideoDownloaderWorker::isFileUpdated(PlayerConfig::Area::Playlist::Item item)
{
    foreach (const StatisticDatabase::Resource &resource, resources)
        if (item.iid == resource.iid)
        {
            if (item.lastupdate > resource.lastupdated)
                return true;
            else
                return false;
        }
    return true;
}

QString VideoDownloaderWorker::getFileHash(QString fileName)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if (hash.addData(&f)) {
            return QString(hash.result().toHex()).toLower();
        }
    }
    return QByteArray();
}

void VideoDownloaderWorker::updateConfig(PlayerConfig config)
{
    this->config = config;
    currentItemIndex = 0;
    if (file)
    {
        delete file;
        file = 0;
    }
}

void VideoDownloaderWorker::getDatabaseInfo()
{
    DatabaseInstance.getResources();
}

void VideoDownloaderWorker::httpFinished()
{
    if (reply->error())
    {
        QTimer::singleShot(10000,this,SLOT(download()));
    //    delete reply;
    //    reply = 0;
    //    delete file;
    //    file = 0;
        if (manager)
            manager->deleteLater();
        manager = new QNetworkAccessManager(this);
        return;
    }
    qDebug()<<"File downloading Finished. Registering in database.";
    QString currentItemId = itemsToDownload[currentItemIndex].iid;
    currentItemIndex++;
    file->flush();
    file->close();

    delete reply;
    reply = 0;
    delete file;
    file = 0;
    swapper.add(VIDEO_FOLDER + currentItemId + ".mp4", VIDEO_FOLDER + currentItemId + ".mp4_");
    swapper.start();

  //  download();
}

void VideoDownloaderWorker::httpReadyRead()
{
    if (restarter)
        restarter->stop();
    if (file)
    {
      //  QtConcurrent::run(writeToFileJob, file, reply);
        file->write(reply->readAll());
        file->flush();
        qDebug() << "updating file status: " << itemsToDownload[currentItemIndex].iid << " [ " << file->size() << " ] bytes";
       // DatabaseInstance.updateResourceDownloadStatus(itemsToDownload[currentItemIndex].iid,file->size());
    }
}

void VideoDownloaderWorker::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    emit downloadProgress(double(bytesRead)/double(totalBytes));

    double fileProgress = 1.0 / double(itemsToDownload.count());
    double totalProgress = double(currentItemIndex) * fileProgress;

    emit downloadProgressSingle(totalProgress + double(bytesRead)/double(totalBytes)*fileProgress, itemsToDownload[currentItemIndex].name);
}

void VideoDownloaderWorker::getResources(QList<StatisticDatabase::Resource> resources)
{
    this->resources = resources;
    checkDownload();
}


FileSwapper::FileSwapper()
{
    connect(&swapTimer,SIGNAL(timeout()),this, SLOT(runSwapCycle()));
}

void FileSwapper::add(QString mainFile, QString tempFile)
{
    SwapDefines defines;
    defines.mainFile = mainFile;
    defines.tempFile = tempFile;
    needToSwap.append(defines);
}

void FileSwapper::start()
{
    swapTimer.start(1000);
}

void FileSwapper::stop()
{
    swapTimer.stop();
}

void FileSwapper::runSwapCycle()
{
    QList<SwapDefines> rest;
    foreach (const SwapDefines &defs, needToSwap)
    {
        QFileInfo info(defs.mainFile);

        if (GlobalStatsInstance.getCurrentItem() != info.baseName())
        {
            QFile::remove(defs.mainFile);
            QFile::rename(defs.tempFile,defs.mainFile);
            qDebug() << "Swapped " << defs.mainFile << " with " << defs.tempFile;
        }
        else
            rest.append(defs);
    }
    needToSwap = rest;
    if (needToSwap.isEmpty())
    {
        swapTimer.stop();
        emit done();
    }
}


VideoDownloader::VideoDownloader(PlayerConfig config, QObject *parent) : QThread(parent)
{
    worker = new VideoDownloaderWorker(config,this);
    connect(worker,SIGNAL(done()),this, SIGNAL(done()));
    connect(worker,SIGNAL(downloadProgressSingle(double,QString)), this, SIGNAL(downloadProgressSingle(double,QString)));
    connect(worker, SIGNAL(checkDownloadItemsTodownloadResult(int)),this,SIGNAL(donwloadConfigResult(int)));
    connect(this, SIGNAL(runDownloadSignal()),worker,SLOT(runDonwload()));
}

VideoDownloader::~VideoDownloader()
{

}

void VideoDownloader::runDownload()
{
    qDebug() << "VIDEO DOWNLOADER::runDownload";
    emit runDownloadSignal();
}

void VideoDownloader::run()
{

}

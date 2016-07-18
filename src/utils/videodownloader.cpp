#include <QVector>
#include <QDebug>
#include <QFileInfo>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include "videodownloader.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "platformdefines.h"


#ifdef PLATFORM_DEFINE_ANDROID
#include "JlCompress.h"
#endif

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
}

void VideoDownloaderWorker::prepareDownload()
{
    getDatabaseInfo();
}

void VideoDownloaderWorker::checkDownload()
{
    int itemCount = 0;
    itemsToDownload.clear();
    foreach (const QString &area, config.screens.keys())
    {
        PlaylistAPIResult playlist = config.screens[area].playlist;
        foreach (const PlaylistAPIResult::CampaignItem &campaign, playlist.items)
            foreach (const PlaylistAPIResult::PlaylistItem &item, campaign.content)
            {
                //no need to download online resource
                if (item.type == "html5_online")
                    continue;
                QString filename = VIDEO_FOLDER + item.id + item.getExtension();
                QString filehash;
                if (!QFile::exists(filename))
                {
                    qDebug() << "checkDownload:: file does not exists " << item.id << " and need to be downloaded";
                    itemsToDownload.append(item);
                }
                else if ((filehash = getCacheFileHash(filename)) != item.fileHash)
                {
                    qDebug() << "different hashes " << filehash << " vs " << item.fileHash;
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

        QStringList currentItemUrlTokens = itemsToDownload[currentItemIndex].fileUrl.split(".");
        QString extension;
        if (currentItemUrlTokens.count() < 2)
            extension = "";
        else
            extension = "." + currentItemUrlTokens.last();
        QString tempFileName = VIDEO_FOLDER + itemsToDownload[currentItemIndex].id + extension + "_";

        if (QFile::exists(tempFileName))
        {
            qDebug() << "temp file found. Resuming downloading";
            QFileInfo info(tempFileName);
            file = new QFile(tempFileName);
            file->open(QFile::Append);
            QNetworkRequest request(QUrl(itemsToDownload[currentItemIndex].fileUrl));

            QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(info.size()) + "-";
            request.setRawHeader("Range",rangeHeaderValue);
            reply = manager->get(request);
        }
        else
        {
            file = new QFile(tempFileName);
            file->open(QFile::WriteOnly);

            reply = manager->get(QNetworkRequest(QUrl(itemsToDownload[currentItemIndex].fileUrl)));
        }
        connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(connectError(QNetworkReply::NetworkError)));
        GlobalStatsInstance.registryDownload();

        DatabaseInstance.registryResource(itemsToDownload[currentItemIndex].id,
                                          itemsToDownload[currentItemIndex].name,
                                          itemsToDownload[currentItemIndex].updated_at,
                                          0);
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

void VideoDownloaderWorker::runDownloadNew()
{
    qDebug() << "VDW: run Donwload new";
    checkDownload();
    start();
}

void VideoDownloaderWorker::writeToFileJob(QFile *f, QNetworkReply *r)
{
    f->write(r->readAll());
    f->flush();
}

bool VideoDownloaderWorker::isFileUpdated(PlaylistAPIResult::PlaylistItem item)
{
    foreach (const StatisticDatabase::Resource &resource, resources)
        if (item.id == resource.iid)
        {
            if (item.updated_at > resource.lastupdated)
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
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f)) {
            f.close();
            return QString(hash.result().toHex()).toLower();
        }
    }
    return "";
}

QString VideoDownloaderWorker::getCacheFileHash(QString fileName)
{
    QFileInfo fileInfo(fileName);

    if (hashCache.contains(fileName)){
        qDebug() << "sha1 of " + fileName + " found in cache";
        if (fileInfo.lastModified() > hashCache[fileName].date)
        {
            qDebug() << "but file was updated: calculating hash";
            QFile f(fileName);
            if (f.open(QFile::ReadOnly)) {
                QCryptographicHash hash(QCryptographicHash::Sha1);
                if (hash.addData(&f)) {
                    QString hashHex = QString(hash.result().toHex()).toLower();
                    HashMeasure hashMeasure;
                    hashMeasure.date = QDateTime::currentDateTime();
                    hashMeasure.hash = hashHex;
                    hashCache[fileName] = hashMeasure;
                    return hashHex;
                }
            }
            return "";
        }
        else
            return hashCache[fileName].hash;
    }
    else
    {
        qDebug() << "filehash  of " + fileName + " was not found in cache - calculating!";
        QFile f(fileName);
        if (f.open(QFile::ReadOnly)) {
            QCryptographicHash hash(QCryptographicHash::Md5);
            if (hash.addData(&f)) {
                QString hashHex = QString(hash.result().toHex()).toLower();
                HashMeasure hashMeasure;
                hashMeasure.date = QDateTime::currentDateTime();
                hashMeasure.hash = hashHex;
                hashCache[fileName] = hashMeasure;
                return hashHex;
            }
        }
        return "";
    }

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
        qDebug() << "VDW::httpFinished -> Error No internet connection";
        reply->disconnect();
        QTimer::singleShot(10000,this,SLOT(download()));
    //    delete reply;
    //    reply = 0;
    //    delete file;
    //    file = 0;
        if (manager)
        {
            manager->disconnect();
            manager->deleteLater();
        }
        manager = new QNetworkAccessManager(this);
        return;
    }
    qDebug()<<"File downloading Finished. Registering in database.";
    qDebug()<<"C="<<itemsToDownload.count() << " I=" << currentItemIndex;
    PlaylistAPIResult::PlaylistItem currentItem = itemsToDownload[currentItemIndex];
    QString currentItemId = currentItem.id;
    currentItemIndex++;
    file->flush();
    file->close();

    delete reply;
    reply = 0;
    delete file;
    file = 0;
    if (currentItem.type == "html5_zip")
    {
        PlatformSpecificService.extractFile(currentItemId + currentItem.getExtension(), currentItemId);
     /*   QDir dir(VIDEO_FOLDER + currentItemId);
        dir.removeRecursively();
        QDir().mkdir(VIDEO_FOLDER + currentItemId);
        QFile::rename(VIDEO_FOLDER + currentItemId + currentItem.extension() + "_", VIDEO_FOLDER + currentItemId + currentItem.extension());
#ifdef PLATFORM_DEFINE_ANDROID
        QFile * zipContentFile = new QFile(VIDEO_FOLDER + currentItemId + currentItem.extension());
        zipContentFile->open(QFile::ReadOnly);
        JlCompress::extractDir(zipContentFile,VIDEO_FOLDER + currentItemId + "/");
        zipContentFile->close();
        delete zipContentFile;
#endif*/
    }
    else
        swapper.add(VIDEO_FOLDER + currentItemId + currentItem.getExtension(), VIDEO_FOLDER + currentItemId + currentItem.getExtension() + "_");
    swapper.start();

  //  download();
}

void VideoDownloaderWorker::httpReadyRead()
{
    static int v = 0;
    v++;
    if (restarter)
        restarter->stop();
    if (file)
    {
      //  QtConcurrent::run(writeToFileJob, file, reply);
        file->write(reply->readAll());
        file->flush();
        if (v % 10 == 0)
            qDebug() << QDateTime::currentDateTime().time().toString("HH:mm:ss ") << "updating file status: " << itemsToDownload[currentItemIndex].id << " [ " << file->size() << " ] bytes";
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
    connect(this, SIGNAL(runDownloadSignalNew()),worker,SLOT(runDownloadNew()));
}

VideoDownloader::~VideoDownloader()
{

}

void VideoDownloader::runDownload()
{
    qDebug() << "VIDEO DOWNLOADER::runDownload";
    emit runDownloadSignal();
}

void VideoDownloader::runDownloadNew()
{
    qDebug() << "VIDEO DOWNLOADER::runDownloadNew";
    emit runDownloadSignalNew();
}

void VideoDownloader::run()
{

}

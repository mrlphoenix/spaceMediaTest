#include <QVector>
#include <QDebug>
#include <QFileInfo>
#include "videodownloader.h"
#include "statisticdatabase.h"
#include "globalstats.h"
#include "platformdefines.h"

VideoDownloader::VideoDownloader(PlayerConfig config, QObject *parent) : QObject(parent)
{
    this->config = config;
    file = 0;
    currentItemIndex = 0;
}

VideoDownloader::~VideoDownloader()
{
    if (file)
        file->deleteLater();
    if (reply)
        reply->deleteLater();
}

void VideoDownloader::checkDownload()
{
    int itemCount = 0;
    itemsToDownload.clear();

    foreach (const PlayerConfig::Area& area, config.areas)
        foreach(const PlayerConfig::Area::Playlist::Item& item, area.playlist.items)
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
    GlobalStatsInstance.setContentPlay(itemCount);
    qDebug() << "FILES NEED TO BE DOWNLOADED: " << itemsToDownload.count();
}

void VideoDownloader::start()
{
    currentItemIndex = 0;
    download();
}

void VideoDownloader::download()
{
    if (itemsToDownload.count() > currentItemIndex)
    {
        qDebug() << "Downloading " + itemsToDownload[currentItemIndex].name;
        emit totalDownloadProgress(double(currentItemIndex+1)/double(itemsToDownload.count()),itemsToDownload[currentItemIndex].name);
        file = new QFile(VIDEO_FOLDER + itemsToDownload[currentItemIndex].iid + ".mp4");
        file->open(QFile::WriteOnly);

        reply = manager.get(QNetworkRequest(QUrl(itemsToDownload[currentItemIndex].path)));
        connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
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

QString VideoDownloader::getFileHash(QString fileName)
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

void VideoDownloader::updateConfig(PlayerConfig config)
{
    this->config = config;
    currentItemIndex = 0;
    if (file)
    {
        delete file;
        file = 0;
    }
}

void VideoDownloader::httpFinished()
{
    qDebug()<<"File downloading Finished. Registering in database.";
    currentItemIndex++;
    file->flush();
    file->close();

    delete reply;
    reply = 0;
    delete file;
    file = 0;

    download();
}

void VideoDownloader::httpReadyRead()
{
    if (file)
    {
        file->write(reply->readAll());
        file->flush();
        qDebug() << "updating file status: " << itemsToDownload[currentItemIndex].iid << " [ " << file->size() << " ] bytes";
        DatabaseInstance.updateResourceDownloadStatus(itemsToDownload[currentItemIndex].iid,file->size());
    }
}

void VideoDownloader::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    emit downloadProgress(double(bytesRead)/double(totalBytes));
}


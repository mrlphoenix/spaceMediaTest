#include <QVector>
#include <QDebug>
#include <QFileInfo>
#include "videodownloader.h"
#include "statisticdatabase.h"
#include "globalstats.h"

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
    foreach (const PlayerConfig::Area& area, config.areas)
        foreach(const PlayerConfig::Area::Playlist::Item& item, area.playlist.items)
        {
            QString filename = "data/video/" + item.iid + ".mp4";
            if (!QFile::exists(filename))
                itemsToDownload.append(item);
            else if (getFileHash(filename) != item.sha1)
                itemsToDownload.append(item);
            itemCount++;
        }
    GlobalStatsInstance.setContentPlay(itemCount);
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
        file = new QFile("data/video/" + itemsToDownload[currentItemIndex].iid + ".mp4");
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

void VideoDownloader::httpFinished()
{
    qDebug()<<"File downloading Finished. Registering in database.";
    currentItemIndex++;
    file->flush();
    file->close();

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;

    download();
    reply->deleteLater();
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
    ;
}


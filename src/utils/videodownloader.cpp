#include <QVector>
#include <QDebug>
#include <QFileInfo>
#include "videodownloader.h"
#include <statisticdatabase.h>

VideoDownloader::VideoDownloader(PlayerConfig config, QObject *parent) : QObject(parent)
{
    this->config = config;
    total = 0;
    current = 0;
    textOut = 0;
    file = 0;
    currentItemIndex = 0;
}

void VideoDownloader::setOutput(QProgressBar *total, QProgressBar *current, QLabel *textOut)
{
    if (total == NULL || current == NULL || textOut == NULL)
        return;

    this->total = total;
    this->current = current;
    this->textOut = textOut;
    total->setValue(0);
    current->setValue(0);
}

void VideoDownloader::checkDownload()
{
    if (textOut)
        textOut->setText("Preparing for download");

    foreach (const PlayerConfig::Area& area, config.areas)
        foreach(const PlayerConfig::Area::Playlist::Item& item, area.playlist.items)
        {
            QString filename = "data/video/" + item.iid + ".mp4";
            if (!QFile::exists(filename))
                itemsToDownload.append(item);
            else if (getFileHash(filename) != item.sha1)
                itemsToDownload.append(item);
        }
    if (total)
        total->setMaximum(itemsToDownload.count());
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
        if (textOut)
            textOut->setText("Downloading " + itemsToDownload[currentItemIndex].name);
        file = new QFile("data/video/" + itemsToDownload[currentItemIndex].iid + ".mp4");
        file->open(QFile::WriteOnly);

        reply = manager.get(QNetworkRequest(QUrl(itemsToDownload[currentItemIndex].path)));
        connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
    }
    else
    {
        if (textOut)
            textOut->setText("Completed");
        emit done();
    }
    if (total)
        total->setValue(currentItemIndex);
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
    DatabaseInstance.registryResource(itemsToDownload[currentItemIndex].iid,
                                      itemsToDownload[currentItemIndex].name,
                                      itemsToDownload[currentItemIndex].lastupdate,
                                      itemsToDownload[currentItemIndex].size);
    currentItemIndex++;
    file->flush();
    file->close();

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;

    download();
}

void VideoDownloader::httpReadyRead()
{
    if (file)
            file->write(reply->readAll());
}

void VideoDownloader::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (current)
    {
        current->setMaximum(totalBytes);
        current->setValue(bytesRead);
    }
}


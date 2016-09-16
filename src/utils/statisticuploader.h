#ifndef STATISTICUPLOADER_H
#define STATISTICUPLOADER_H

#include <QObject>
#include <QList>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "statisticdatabase.h"
#include "videoservice.h"

class StatisticUploader : public QObject
{
    Q_OBJECT
public:
    explicit StatisticUploader(VideoService* videoService, QObject *parent = 0);
signals:
    void finished(bool success);
public slots:
    bool start();
    void eventsReady(QList<StatisticDatabase::PlayEvent> events);
    void eventsUploadResult(NonQueryResult result);

private:
    QNetworkAccessManager manager;
    VideoService * videoService;
};


#endif // STATISTICUPLOADER_H

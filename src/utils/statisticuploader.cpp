#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QProcess>
#include <QTimer>
#include <QUrlQuery>

#include "statisticuploader.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "singleton.h"
#include "sslencoder.h"

StatisticUploader::StatisticUploader(VideoService *videoService, QObject *parent) : QObject(parent)
{
    qDebug() << "Statistic Uploader Initialization";
    this->videoService = videoService;

    connect(&DatabaseInstance,SIGNAL(eventsFound(QList<StatisticDatabase::PlayEvent>)),this,SLOT(eventsReady(QList<StatisticDatabase::PlayEvent>)));
    connect(videoService,SIGNAL(sendStatisticEventsResult(NonQueryResult)),this,SLOT(eventsUploadResult(NonQueryResult)));
}

bool StatisticUploader::start()
{
    qDebug() << "Uploader:start";
    DatabaseInstance.findEventsToSend();
    return false;
}

void StatisticUploader::eventsReady(QList<StatisticDatabase::PlayEvent> events)
{
    if (events.count() == 0)
    {
        return;
    }
    QJsonArray result;
    foreach (const StatisticDatabase::PlayEvent &event, events)
        result.append(event.serialize());
    QJsonDocument doc(result);
    QString strToSend = doc.toJson();

    //for debugging
    PlatformSpecificService.writeToFile(strToSend.toLocal8Bit(), VIDEO_FOLDER + "stats.txt");

    //send via videoService
    videoService->sendEvents(strToSend);
}

void StatisticUploader::eventsUploadResult(NonQueryResult result)
{
    if (result.status == "success")
    {
        qDebug() << "EventsUploadResult::Success";
        DatabaseInstance.eventsUploaded();
    }
    else
    {
        qDebug() << "EventsUploadResult::FAIL" << result.source;
    }
}

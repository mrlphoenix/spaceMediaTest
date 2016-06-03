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
    connect(&DatabaseInstance,SIGNAL(playsFound(QList<StatisticDatabase::Play>)),this,SLOT(playsReady(QList<StatisticDatabase::Play>)));
    connect(&DatabaseInstance, SIGNAL(systemInfoFound(QList<PlatformSpecific::SystemInfo>)),this,SLOT(systemInfoReady(QList<PlatformSpecific::SystemInfo>)));
    connect(videoService,SIGNAL(sendStatisticResult(NonQueryResult)),this,SLOT(systemInfoUploadResult(NonQueryResult)));
    connect(videoService,SIGNAL(sendStatisticPlaysResult(NonQueryResult)),this,SLOT(playsUploadResult(NonQueryResult)));
    connect(videoService,SIGNAL(sendStatisticEventsResult(NonQueryResult)),this,SLOT(eventsUploadResult(NonQueryResult)));
}

bool StatisticUploader::start()
{
    qDebug() << "Uploader:start";
    DatabaseInstance.findEventsToSend();
    return false;
}

void StatisticUploader::playsReady(QList<StatisticDatabase::Play> plays)
{
    if (plays.count() == 0)
        return;
    QJsonArray result;
    foreach (const StatisticDatabase::Play &play, plays)
        result.append(play.serialize());
    QJsonDocument doc(result);
    QString strToSend = doc.toJson();
    videoService->sendPlays(strToSend);
}
void StatisticUploader::systemInfoReady(QList<PlatformSpecific::SystemInfo> data)
{
    if (data.count() == 0)
    {
        return;
    }
    QJsonArray result;
    foreach (const PlatformSpecific::SystemInfo &info, data)
        result.append(info.serialize());
    QJsonDocument doc(result);
    QString strToSend = doc.toJson();

    videoService->sendStatistic(strToSend);
}

void StatisticUploader::eventsReady(QList<StatisticDatabase::PlayEvent> events)
{
    /*
     *   {
    "timestamp": 0,
    "virtual_screen_area_id": "string",
    "content_id": "string",
    "campaign_id": "string",
    "gps": {
      "latitude": 0,
      "longitude": 0
    },
    "cpu_load": 0,
    "battery": 0,
    "traffic": 0,
    "free_memory": 0,
    "wifi_mac": "string",
    "hdmi_cec": true,
    "hdmi_gpio": true,
    "free_space": 0
  }
     * */
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
    PlatformSpecific::writeToFile(strToSend.toLocal8Bit(), VIDEO_FOLDER + "stats.txt");

    //send via videoService
    videoService->sendEvents(strToSend);
}

void StatisticUploader::systemInfoUploadResult(NonQueryResult result)
{
    if (result.status == "success")
    {
        qDebug() << "SystemInfoUploadResult::Success";
        DatabaseInstance.systemInfoUploaded();
    }
    else
    {
        qDebug() << "SystemInfoUploadResult::FAIL" << result.source;
    }
}

void StatisticUploader::playsUploadResult(NonQueryResult result)
{
    if (result.status == "success")
    {
        qDebug() << "PlaysUploadResult::Success";
        DatabaseInstance.playsUploaded();
    }
    else
    {
        qDebug() << "PlaysUploadResult::FAIL" << result.source;
    }
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

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
    manager = 0;
}

bool StatisticUploader::start()
{
    qDebug() << "Uploader:start";
    DatabaseInstance.findEventsToSend();
    return false;
}

void StatisticUploader::eventsReady(QList<StatisticDatabase::PlayEvent> events)
{
    qDebug() << "Events ready to send: " << events.count();
    if (events.count() == 0)
    {
        return;
    }
    DatabaseInstance.prepareEventsToSend();
    QJsonArray result;
    foreach (const StatisticDatabase::PlayEvent &event, events)
        result.append(event.serialize());
    QJsonDocument doc(result);

    QFile f("/home/pi/teleds/stats.txt");
    f.open(QFile::WriteOnly);
    f.write(doc.toJson());
    f.flush();
    f.close();
    sendEvents(doc.toJson());

  //  QString strToSend = doc.toJson();

    //send via videoService
  //  videoService->sendEvents(strToSend);
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
        DatabaseInstance.resetPreparedEvents();
        GlobalStatsInstance.clearCachedItemData();
    }
}

void StatisticUploader::requestFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "EventsUploadResult::FAIL";
        DatabaseInstance.resetPreparedEvents();
        GlobalStatsInstance.clearCachedItemData();
        //internet connection error
    }
    else
    {
        qDebug() << "EventsUploadResult::SUCCESS";
        DatabaseInstance.eventsUploaded();
    }
}

void StatisticUploader::sendEvents(QByteArray data)
{
    if (manager)
    {
        manager->disconnect();
        manager->deleteLater();
    }
    manager = new QNetworkAccessManager(this);
    QByteArray compressedData = SSLEncoder::compressGZIP(data);
    QUrl url(videoService->getServerURL());
    url.setPath("/player/event");
    QNetworkRequest networkRequest(url);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkRequest.setRawHeader("Authorization", GlobalConfigInstance.getToken().toLocal8Bit());
    networkRequest.setRawHeader("Content-Encoding", QString("gzip").toLocal8Bit());

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));

    manager->post(networkRequest, compressedData);


    /*
     *  VideoServiceRequest result;
    //result.body = data.toLocal8Bit();
    qDebug() << "original size: " << data.toLocal8Bit().count();
    result.body = SSLEncoder::compressGZIP(data.toLocal8Bit());
    qDebug() << "upload body size = " << result.body.count();
    result.knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.headers["Content-Encoding"] = "gzip";
    result.methodAPI = "player/event";
    result.method = "POST";
    result.name = "statistics:events";
     * */

    /*
     *
    foreach (const VideoServiceRequest::VideoServiceRequestParam& param, request.params)
        query.addQueryItem(param.key,param.value);
    url.setPath("/" + request.methodAPI);
    if (request.method == "GET")
        url.setQuery(query);
    else
    {
        if (request.body.count())
            data = request.body;
        else
            data = query.toString(QUrl::FullyEncoded).toUtf8();
    }

    QNetworkRequest networkRequest(url);
    if (request.method == "POST")
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    manager->disconnect();

    if (request.name == "init")
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(initVideoRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "playlist")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlaylistRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "settings")
        connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlayerSettingsRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "statistics:events")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(sendStatisticEventsRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "update")
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getUpdatesRequestFinishedSlot(QNetworkReply*)));
    else
    {
        qDebug() << "ERROR: undefined method: " << request.name;
    };

     * */
}

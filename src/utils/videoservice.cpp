#include <QUrlQuery>
#include <QUrl>
#include <QNetworkRequest>
#include <QDebug>
#include <QTimeZone>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScreen>
#include "videoservice.h"
#include "singleton.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "platformspecific.h"

VideoService::VideoService(QString serverURL, QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->serverURL = serverURL;

    connect(this,SIGNAL(initVideoRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(initRequestResultReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlaylistRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(getPlaylistResultReply(QNetworkReply*)));
    connect(this,SIGNAL(sendStatisticEventsRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(sendStatisticEventsResultReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlayerSettingsRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(getPlayerSettingsReply(QNetworkReply*)));
    connect(this,SIGNAL(getUpdatesRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(getUpdatesReply(QNetworkReply*)));

    connect(&resultProcessor,SIGNAL(initResult(InitRequestResult)),this,SIGNAL(initResult(InitRequestResult)));
    connect(&resultProcessor,SIGNAL(getPlaylistResult(PlayerConfigAPI)),this,SIGNAL(getPlaylistResult(PlayerConfigAPI)));
    connect(&resultProcessor,SIGNAL(sendStatisticEventsResult(NonQueryResult)),this,SIGNAL(sendStatisticEventsResult(NonQueryResult)));
    connect(&resultProcessor,SIGNAL(getPlayerSettingsResult(SettingsRequestResult)),this,SIGNAL(getPlayerSettings(SettingsRequestResult)));
    connect(&resultProcessor,SIGNAL(getUpdatesResult(UpdateInfoResult)), this, SIGNAL(getUpdatesResult(UpdateInfoResult)));

    currentRequestExists = false;
}

void VideoService::getPlayerSettings()
{
    executeRequest(VideoServiceRequestFabric::getSettingsRequest());
}

void VideoService::getPlaylist()
{
    executeRequest(VideoServiceRequestFabric::getPlaylistRequest());
}

void VideoService::sendEvents(QString data)
{
    executeRequest(VideoServiceRequestFabric::sendEventsRequest(data));
}

void VideoService::getUpdates(QString platform)
{
    executeRequest(VideoServiceRequestFabric::getUpdateVersion(platform));
}

void VideoService::advancedInit(QByteArray data)
{
    qDebug() << "VideoService::advancedInit";
    executeRequest(VideoServiceRequestFabric::advancedInitRequest(data));
}

void VideoService::executeRequest(VideoServiceRequest request)
{
    if (currentRequestExists)
    {
        requests.enqueue(request);
    }
    else
    {
        currentRequest = request;
        performRequest(request);
        currentRequestExists = true;
    }
}

bool VideoService::processReplyError(const QNetworkReply *reply, QString method)
{
    if (reply->error())
    {
        qDebug() << method + "::reply_error: " + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
        return true;
    }
    return false;
}

void VideoService::initVideoRequestFinishedSlot(QNetworkReply *reply)
{
    qDebug() << "initVideoRequestFinishedSlot";
    processReplyError(reply,"init");
    emit initVideoRequestFinished(reply);
    nextRequest();
}

void VideoService::getPlaylistRequestFinishedSlot(QNetworkReply *reply)
{
    if (processReplyError(reply,"getPlaylist"))
        GlobalStatsInstance.registryPlaylistError();
    emit getPlaylistRequestFinished(reply);
    nextRequest();
}

void VideoService::sendStatisticEventsRequestFinishedSlot(QNetworkReply *reply)
{
    processReplyError(reply,"sendStatistic:events");
    emit sendStatisticEventsRequestFinished(reply);
    nextRequest();
}

void VideoService::getPlayerSettingsRequestFinishedSlot(QNetworkReply *reply)
{
    processReplyError(reply,"settings");
    emit getPlayerSettingsRequestFinished(reply);
    nextRequest();
}

void VideoService::getUpdatesRequestFinishedSlot(QNetworkReply *reply)
{
    processReplyError(reply, "update");
    emit getUpdatesRequestFinished(reply);
    nextRequest();
}

void VideoService::performRequest(VideoServiceRequest request)
{
    qDebug() << "VideoService::performRequest" << request.name;
    QUrl url(serverURL);
    QUrlQuery query;
    QByteArray data;
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

    foreach (const QNetworkRequest::KnownHeaders &key, request.knownHeaders.keys())
        networkRequest.setHeader(key,request.knownHeaders[key]);
    foreach (const QString &key, request.headers.keys())
        networkRequest.setRawHeader(key.toLocal8Bit(), request.headers[key].toLocal8Bit());

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
    }
    //qDebug() << data;
    if (request.method == "GET")
        manager->get(networkRequest);
    else
        manager->post(networkRequest, data);
}

void VideoService::nextRequest()
{
    if (requests.count() > 0)
    {
        currentRequest = requests.dequeue();
        performRequest(currentRequest);
        currentRequestExists = true;
    }
    else
        currentRequestExists = false;
}


VideoServiceRequest::VideoServiceRequestParam::VideoServiceRequestParam(QString key, QString value)
{
    this->key = key;
    this->value = value;
}

VideoServiceRequest VideoServiceRequestFabric::sendEventsRequest(QString data)
{
    VideoServiceRequest result;
    result.body = data.toLocal8Bit();
    result.knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "player/event";
    result.method = "POST";
    result.name = "statistics:events";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::advancedInitRequest(QByteArray bodyData)
{
    VideoServiceRequest result;

    result.body = bodyData;
    result.knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";

    result.methodAPI = "player";
    result.name = "init";
    result.method = "POST";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getPlaylistRequest()
{
    VideoServiceRequest result;
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "player/playlist";
    result.name = "playlist";
    result.method = "GET";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getSettingsRequest()
{
    VideoServiceRequest result;
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "player/settings";
    result.name = "settings";
    result.method = "GET";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getUpdateVersion(QString platform)
{
    VideoServiceRequest result;
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "app/" + platform;
    result.name = "update";
    result.method = "GET";
    return result;
}

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
#include "platformspecs.h"

VideoService::VideoService(QString serverURL, QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->serverURL = serverURL;

    connect(this,SIGNAL(initVideoRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(initRequestResultReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlaylistRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(getPlaylistResultReply(QNetworkReply*)));
    connect(this,SIGNAL(sendStatisticRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(sendStatisticResultReply(QNetworkReply*)));
    connect(this,SIGNAL(sendStatisticPlaysRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(sendStatisticPlaysResultReply(QNetworkReply*)));
    connect(this,SIGNAL(sendStatisticEventsRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(sendStatisticEventsResultReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlayerSettingsRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(getPlayerSettingsReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlayerAreasRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(getPlayerAreasReply(QNetworkReply*)));
    connect(this,SIGNAL(getVirtualScreenPlaylistRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(getVirtualScreenPlaylist(QNetworkReply*)));

    connect(&resultProcessor,SIGNAL(initResult(InitRequestResult)),this,SIGNAL(initResult(InitRequestResult)));
    connect(&resultProcessor,SIGNAL(getPlaylistResult(PlayerConfig)),this,SIGNAL(getPlaylistResult(PlayerConfig)));
    connect(&resultProcessor,SIGNAL(sendStatisticResult(NonQueryResult)),this,SIGNAL(sendStatisticResult(NonQueryResult)));
    connect(&resultProcessor,SIGNAL(sendStatisticPlaysResult(NonQueryResult)),this,SIGNAL(sendStatisticPlaysResult(NonQueryResult)));
    connect(&resultProcessor,SIGNAL(sendStatisticEventsResult(NonQueryResult)),this,SIGNAL(sendStatisticEventsResult(NonQueryResult)));
    connect(&resultProcessor,SIGNAL(getPlayerSettingsResult(SettingsRequestResult)),this,SIGNAL(getPlayerSettings(SettingsRequestResult)));
    connect(&resultProcessor,SIGNAL(getPlayerAreasResult(PlayerConfigNew)),this,SIGNAL(getPlayerAreasResult(PlayerConfigNew)));
    connect(&resultProcessor,SIGNAL(getVirtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)), this, SIGNAL(getVirtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)));

    currentRequestExists = false;
}

void VideoService::init()
{
    executeRequest(VideoServiceRequestFabric::initVideoPlayerRequest());
}

void VideoService::getPlaylist(QString playerId, QString cryptedSessionKey)
{
    executeRequest(VideoServiceRequestFabric::getPlaylistRequest(playerId,cryptedSessionKey));
}

void VideoService::getPlayerSettings()
{
    executeRequest(VideoServiceRequestFabric::getPlaylistSettingsRequest());
}

void VideoService::getPlayerAreas()
{
    executeRequest(VideoServiceRequestFabric::getPlayerAreasRequest());
}

void VideoService::getPlaylist(QString areaId)
{
    executeRequest(VideoServiceRequestFabric::getVirtualScreenPlaylistRequest(QStringList(areaId)));
}

void VideoService::getPlaylist()
{
    executeRequest(VideoServiceRequestFabric::getVirtualScreenPlaylistRequest());
}

void VideoService::sendStatistic(QString data)
{
    executeRequest(VideoServiceRequestFabric::sendStatisticRequest(data));
}

void VideoService::sendPlays(QString data)
{
    executeRequest(VideoServiceRequestFabric::sendPlaysRequest(data));
}

void VideoService::sendEvents(QString data)
{
    executeRequest(VideoServiceRequestFabric::sendEventsRequest(data));
}

void VideoService::advancedInit()
{
    executeRequest(VideoServiceRequestFabric::advancedInitRequest());
}

void VideoService::executeRequest(VideoServiceRequest request)
{
    if (currentRequestExists)
        requests.enqueue(request);
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

void VideoService::sendStatisticRequestFinishedSlot(QNetworkReply *reply)
{
    processReplyError(reply,"sendStatistic:system");
    emit sendStatisticRequestFinished(reply);
    nextRequest();
}

void VideoService::sendStatisticPlaysRequestFinishedSlot(QNetworkReply *reply)
{
    processReplyError(reply,"sendStatistic:plays");
    emit sendStatisticPlaysRequestFinished(reply);
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

void VideoService::getPlayerAreasRequestFinishedSlot(QNetworkReply *reply)
{
    processReplyError(reply,"playerAreas");
    emit getPlayerAreasRequestFinished(reply);
    nextRequest();
}

void VideoService::getVirtualScreenPlaylistRequestFinishedSlot(QNetworkReply *reply)
{
    processReplyError(reply,"playerVirtualScreenPlaylist");
    emit getVirtualScreenPlaylistRequestFinished(reply);
    nextRequest();
}

void VideoService::performRequest(VideoServiceRequest request)
{
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
    else if (request.name == "getPlaylist")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlaylistRequestFinishedSlot(QNetworkReply*)));

    else if (request.name == "settings")
        connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlayerSettingsRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "areas")
        connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlayerAreasRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "playlist")
        connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getVirtualScreenPlaylistRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "statistics:system")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(sendStatisticRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "statistics:plays")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(sendStatisticPlaysRequestFinishedSlot(QNetworkReply*)));
    else if (request.name == "statistics:events")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(sendStatisticEventsRequestFinishedSlot(QNetworkReply*)));
    else
    {
        qDebug() << "ERROR: undefined method: " << request.name;
    }

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


VideoServiceRequest VideoServiceRequestFabric::initVideoPlayerRequest()
{
    VideoServiceRequest result;
    result.methodAPI = "initialization";
    result.name = "init";
    result.method = "GET";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getPlaylistRequest(QString playerId, QString cryptedSessionKey)
{
    VideoServiceRequest result;
    VideoServiceRequest::VideoServiceRequestParam playerIdParam, cryptedSessionKeyParam;
    playerIdParam.key = "player_id";
    playerIdParam.value = playerId;

    cryptedSessionKeyParam.key = "ctypted_session_key";
    cryptedSessionKeyParam.value = cryptedSessionKey;
    result.params.append(playerIdParam);
    result.params.append(cryptedSessionKeyParam);

    result.methodAPI = "getplaylist";
    result.name = "getPlaylist";
    result.method = "GET";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::sendStatisticRequest(QString data)
{
    VideoServiceRequest result;
    result.body = data.toLocal8Bit();
    result.knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "event/state";
    result.method = "POST";
    result.name = "statistics:system";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::sendEventsRequest(QString data)
{
    VideoServiceRequest result;
    result.body = data.toLocal8Bit();
    result.knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "event";
    result.method = "POST";
    result.name = "statistics:events";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::sendPlaysRequest(QString data)
{
    VideoServiceRequest result;
    result.body = data.toLocal8Bit();
    result.knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "event/play";
    result.method = "POST";
    result.name = "statistics:plays";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::advancedInitRequest()
{
    VideoServiceRequest result;
    QJsonObject jsonBody;
    jsonBody["timestamp"] = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
    jsonBody["timezone"] = QString(QTimeZone::systemTimeZoneId());
    jsonBody["uniqid"] = PlatformSpecific::getUniqueId();
    jsonBody["screen_width"] = qApp->screens().first()->geometry().width();
    jsonBody["screen_height"] = qApp->screens().first()->geometry().height();
    PlatformSpecific::HardwareInfo hardwareInfo = PlatformSpecific::getHardwareInfo();
    jsonBody["device_vendor"] = hardwareInfo.vendor;
    jsonBody["device_model"] = hardwareInfo.deviceModel;
    jsonBody["cpumodel"] = hardwareInfo.cpuName;
    jsonBody["os"] = hardwareInfo.osName;
    jsonBody["os_version"] = hardwareInfo.osVersion;

    jsonBody["gps_lat"] = GlobalStatsInstance.getLatitude();
    jsonBody["gps_long"] = GlobalStatsInstance.getLongitude();

    QJsonDocument doc(jsonBody);
    QByteArray jsonData = doc.toJson();
    qDebug() << QString(jsonData);

    result.body = jsonData;
    result.knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";

    result.methodAPI = "player";
    result.name = "init";
    result.method = "POST";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getPlaylistSettingsRequest()
{
    VideoServiceRequest result;
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "player/settings";
    result.name = "settings";
    result.method = "GET";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getPlayerAreasRequest()
{
    VideoServiceRequest result;
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "player/virtual-screens";
    result.name = "areas";
    result.method = "GET";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getVirtualScreenPlaylistRequest()
{
    VideoServiceRequest result;
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "player/playlist";
    result.name = "playlist";
    result.method = "GET";
    return result;
}

VideoServiceRequest VideoServiceRequestFabric::getVirtualScreenPlaylistRequest(QStringList areas)
{
    VideoServiceRequest result;
    result.headers["Authorization"] = GlobalConfigInstance.getToken();
    result.methodAPI = "player/playlist";
    result.name = "playlist";
    result.method = "GET";

    result.params.append(VideoServiceRequest::VideoServiceRequestParam("areas", areas.join(",")));
    return result;
}

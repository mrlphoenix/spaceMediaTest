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
    currentRequest = NULL;

    connect(this,SIGNAL(initVideoRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(initRequestResultReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlaylistRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(getPlaylistResultReply(QNetworkReply*)));
    connect(this,SIGNAL(sendStatisticRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(sendStatisticResultReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlayerSettingsRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(getPlayerSettingsReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlayerAreasRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(getPlayerAreasReply(QNetworkReply*)));
    connect(this,SIGNAL(getVirtualScreenPlaylistRequestFinished(QNetworkReply*)), &resultProcessor, SLOT(getVirtualScreenPlaylist(QNetworkReply*)));

    connect(&resultProcessor,SIGNAL(initResult(InitRequestResult)),this,SIGNAL(initResult(InitRequestResult)));
    connect(&resultProcessor,SIGNAL(getPlaylistResult(PlayerConfig)),this,SIGNAL(getPlaylistResult(PlayerConfig)));
    connect(&resultProcessor,SIGNAL(sendStatisticResult(NonQueryResult)),this,SIGNAL(sendStatisticResult(NonQueryResult)));
    connect(&resultProcessor,SIGNAL(getPlayerSettingsResult(SettingsRequestResult)),this,SIGNAL(getPlayerSettings(SettingsRequestResult)));
    connect(&resultProcessor,SIGNAL(getPlayerAreasResult(PlayerConfigNew)),this,SIGNAL(getPlayerAreasResult(PlayerConfigNew)));
    connect(&resultProcessor,SIGNAL(getVirtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)), this, SIGNAL(getVirtualScreenPlaylistResult(QHash<QString,PlaylistAPIResult>)));

}

void VideoService::init()
{
    executeRequest(new InitVideoPlayerRequest());
}

void VideoService::getPlaylist(QString playerId, QString cryptedSessionKey)
{
    executeRequest(new GetPlaylistRequest(playerId, cryptedSessionKey));
}

void VideoService::getPlayerSettings()
{
    executeRequest(new GetPlaylistSettingsRequest());
}

void VideoService::getPlayerAreas()
{
    executeRequest(new GetPlayerAreasRequest());
}

void VideoService::getPlaylist(QString areaId)
{
    executeRequest(new GetVirtualScreenPlaylistRequest(QStringList(areaId)));
}

void VideoService::getPlaylist()
{
    executeRequest(new GetVirtualScreenPlaylistRequest());
}

void VideoService::sendStatistic(QString playerId, QString encodedSessionKey, QString data)
{
    executeRequest(new SendStatisticRequest(playerId, encodedSessionKey, data));
}

void VideoService::advancedInit()
{
    executeRequest(new AdvancedStatisticRequest());
}

void VideoService::executeRequest(VideoServiceRequest *request)
{
    if (currentRequest)
        requests.enqueue(request);
    else
    {
        currentRequest = request;
        performRequest(request);
    }
}


void VideoService::initVideoRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "init:error" + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
    }
    emit initVideoRequestFinished(reply);
    nextRequest();
}


void VideoService::getPlaylistRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "getPlaylist:error" + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
        GlobalStatsInstance.registryPlaylistError();
    }
    emit getPlaylistRequestFinished(reply);
    nextRequest();
}

void VideoService::sendStatisticRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "sendStatistic:error " + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
    }
    emit sendStatisticRequestFinished(reply);
    nextRequest();
}

void VideoService::getPlayerSettingsRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "get player settings error " + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
    }
    emit getPlayerSettingsRequestFinished(reply);
    nextRequest();
}

void VideoService::getPlayerAreasRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "get player areas error" + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
    }
    emit getPlayerAreasRequestFinished(reply);
    nextRequest();
}

void VideoService::getVirtualScreenPlaylistRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "get player areas error" + reply->errorString();
        GlobalStatsInstance.registryPlaylistError();
    }
    emit getVirtualScreenPlaylistRequestFinished(reply);
    nextRequest();
}

void VideoService::performRequest(VideoServiceRequest *request)
{
    QUrl url(serverURL);
    QUrlQuery query;
    QByteArray data;
    foreach (const VideoServiceRequest::VideoServiceRequestParam& param, request->params)
        query.addQueryItem(param.key,param.value);
    url.setPath("/" + request->methodAPI);
    if (request->method == "GET")
        url.setQuery(query);
    else
    {
        if (request->body.count())
            data = request->body;
        else
            data = query.toString(QUrl::FullyEncoded).toUtf8();
    }

    QNetworkRequest networkRequest(url);
    if (request->method == "POST")
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    foreach (const QNetworkRequest::KnownHeaders &key, request->knownHeaders.keys())
        networkRequest.setHeader(key,request->knownHeaders[key]);
    foreach (const QString &key, request->headers.keys())
        networkRequest.setRawHeader(key.toLocal8Bit(), request->headers[key].toLocal8Bit());


    manager->disconnect();

    if (request->name == "init")
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(initVideoRequestFinishedSlot(QNetworkReply*)));
    else if (request->name == "getPlaylist")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlaylistRequestFinishedSlot(QNetworkReply*)));
    else if (request->name == "statistics")
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(sendStatisticRequestFinishedSlot(QNetworkReply*)));
    else if (request->name == "settings")
        connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlayerSettingsRequestFinishedSlot(QNetworkReply*)));
    else if (request->name == "areas")
        connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlayerAreasRequestFinishedSlot(QNetworkReply*)));
    else if (request->name == "playlist")
        connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getVirtualScreenPlaylistRequestFinishedSlot(QNetworkReply*)));
    else
    {
        qDebug() << "ERROR: undefined method: " << request->name;
    }

    if (request->method == "GET")
        manager->get(networkRequest);
    else
        manager->post(networkRequest, data);
}

void VideoService::nextRequest()
{
    if (currentRequest)
        delete currentRequest;
    if (requests.count() > 0)
    {
        currentRequest = requests.dequeue();
        performRequest(currentRequest);
    }
    else
        currentRequest = NULL;
}

InitVideoPlayerRequest::InitVideoPlayerRequest()
{
    methodAPI = "initialization";
    name = "init";
    method = "GET";
}

InitVideoPlayerRequest::~InitVideoPlayerRequest()
{

}

EnablePlayerRequest::EnablePlayerRequest(QString playerId)
{
    VideoServiceRequestParam playerIdParam;
    playerIdParam.key = "player_id";
    playerIdParam.value = playerId;
    params.append(playerIdParam);

    methodAPI = "enabled";
    name = "enablePlaylist";
    method = "GET";
}

EnablePlayerRequest::~EnablePlayerRequest()
{

}

AssignPlaylistToPlayerRequest::AssignPlaylistToPlayerRequest(QString playerId, int playlistId)
{
    //http://api.teleds.com/setplaylist?player_id=3xsg-xuc5-ykdp&playlist=10
    VideoServiceRequestParam playerIdParam, playlistIdParam;
    playerIdParam.key = "player_id";
    playerIdParam.value = playerId;
    playlistIdParam.key = "playlist";
    playlistIdParam.value = QString::number(playlistId);
    params.append(playerIdParam);
    params.append(playlistIdParam);

    methodAPI = "setplaylist";
    name = "assignPlaylistToPlayer";
    method = "GET";
}

AssignPlaylistToPlayerRequest::~AssignPlaylistToPlayerRequest()
{

}

GetPlaylistRequest::GetPlaylistRequest(QString playerId, QString cryptedSessionKey)
{
    //http://api.teleds.com/getplaylist?player_id=r2s6-6fb9-5hdb&ctypted_session_key=XZH%2F57V...
    VideoServiceRequestParam playerIdParam, cryptedSessionKeyParam;
    playerIdParam.key = "player_id";
    playerIdParam.value = playerId;

    cryptedSessionKeyParam.key = "ctypted_session_key";
    cryptedSessionKeyParam.value = cryptedSessionKey;
    params.append(playerIdParam);
    params.append(cryptedSessionKeyParam);

    methodAPI = "getplaylist";
    name = "getPlaylist";
    method = "GET";
}

GetPlaylistRequest::~GetPlaylistRequest()
{

}

SendStatisticRequest::SendStatisticRequest(QString playerId, QString encryptedSessionKey, QString data)
{
    VideoServiceRequestParam playerIdParam, encrypedSessionKeyParam, dataParam;
    playerIdParam.key = "player_id";
    playerIdParam.value = playerId;

    encrypedSessionKeyParam.key = "ctypted_session_key";
    encrypedSessionKeyParam.value = encryptedSessionKey;

    dataParam.key = "statistics";
    dataParam.value = data;

    params.append(playerIdParam);
    params.append(encrypedSessionKeyParam);
    params.append(dataParam);

    methodAPI = "statistics";
    name = "statistics";
    method = "POST";
}

SendStatisticRequest::~SendStatisticRequest()
{

}

AdvancedStatisticRequest::AdvancedStatisticRequest()
{
    QJsonObject jsonBody;
    jsonBody["timestamp"] = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss");
    jsonBody["timezone"] = QString(QTimeZone::systemTimeZoneId());
    jsonBody["uniqid"] = PlatformSpecs::getUniqueId();
    jsonBody["screen_width"] = qApp->screens().first()->geometry().width();
    jsonBody["screen_height"] = qApp->screens().first()->geometry().height();
    PlatformSpecs::HardwareInfo hardwareInfo = PlatformSpecs::getHardwareInfo();
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

    body = jsonData;
    knownHeaders[QNetworkRequest::ContentTypeHeader] = "application/json";

    methodAPI = "player";
    name = "init";
    method = "POST";
}

AdvancedStatisticRequest::~AdvancedStatisticRequest()
{

}

GetPlaylistSettingsRequest::GetPlaylistSettingsRequest()
{
    headers["Authorization"] = GlobalConfigInstance.getToken();
    methodAPI = "player/settings";
    name = "settings";
    method = "GET";
}

GetPlaylistSettingsRequest::~GetPlaylistSettingsRequest()
{

}

GetPlayerAreasRequest::GetPlayerAreasRequest()
{
    headers["Authorization"] = GlobalConfigInstance.getToken();
    methodAPI = "player/virtual-screens";
    name = "areas";
    method = "GET";
}

GetPlayerAreasRequest::~GetPlayerAreasRequest()
{

}

GetVirtualScreenPlaylistRequest::GetVirtualScreenPlaylistRequest()
{
    headers["Authorization"] = GlobalConfigInstance.getToken();
    methodAPI = "player/playlist";
    name = "playlist";
    method = "GET";
}

GetVirtualScreenPlaylistRequest::GetVirtualScreenPlaylistRequest(QStringList areas)
{
    headers["Authorization"] = GlobalConfigInstance.getToken();
    methodAPI = "player/playlist";
    name = "playlist";
    method = "GET";

    params.append(VideoServiceRequestParam("areas", areas.join(",")));
}

VideoServiceRequest::VideoServiceRequestParam::VideoServiceRequestParam(QString key, QString value)
{
    this->key = key;
    this->value = value;
}

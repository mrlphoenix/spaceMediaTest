#include <QUrlQuery>
#include <QUrl>
#include <QNetworkRequest>
#include <QDebug>
#include "videoservice.h"
#include "singleton.h"
#include "globalstats.h"

VideoService::VideoService(QString serverURL, QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->serverURL = serverURL;
    currentRequest = NULL;
    connect(this,SIGNAL(initVideoRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(initRequestResultReply(QNetworkReply*)));
    connect(this,SIGNAL(enablePlayerRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(enablePlayerResultReply(QNetworkReply*)));
    connect(this,SIGNAL(assignPlaylistToPlayerRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(assignPlaylistResultReply(QNetworkReply*)));
    connect(this,SIGNAL(getPlaylistRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(getPlaylistResultReply(QNetworkReply*)));
    connect(this,SIGNAL(sendStatisticRequestFinished(QNetworkReply*)),&resultProcessor,SLOT(sendStatisticResultReply(QNetworkReply*)));

    connect(&resultProcessor,SIGNAL(initResult(InitRequestResult)),this,SIGNAL(initResult(InitRequestResult)));
    connect(&resultProcessor,SIGNAL(enablePlayerResult(QString)),this,SIGNAL(enablePlayerResult(QString)));
    connect(&resultProcessor,SIGNAL(assignPlaylistResult(QString)),this,SIGNAL(assignPlaylistResult(QString)));
    connect(&resultProcessor,SIGNAL(getPlaylistResult(PlayerConfig)),this,SIGNAL(getPlaylistResult(PlayerConfig)));
    connect(&resultProcessor,SIGNAL(sendStatisticResult(NonQueryResult)),this,SIGNAL(sendStatisticResult(NonQueryResult)));
}

void VideoService::init()
{
    executeRequest(new InitVideoPlayerRequest());
}

void VideoService::enablePlayer(QString playerId)
{
    executeRequest(new EnablePlayerRequest(playerId));
}

void VideoService::assignPlaylist(QString playerId, int playlistId)
{
    executeRequest(new AssignPlaylistToPlayerRequest(playerId,playlistId));
}

void VideoService::getPlaylist(QString playerId, QString cryptedSessionKey)
{
    executeRequest(new GetPlaylistRequest(playerId, cryptedSessionKey));
}

void VideoService::sendStatistic(QString playerId, QString encodedSessionKey, QString data)
{
    executeRequest(new SendStatisticRequest(playerId, encodedSessionKey, data));
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

void VideoService::enablePlayerRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "enable player:error" + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
    }

    emit enablePlayerRequestFinished(reply);
    nextRequest();
}

void VideoService::assignPlaylistToPlayerRequestFinishedSlot(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "Assign Player:error" + reply->errorString();
        GlobalStatsInstance.registryConnectionError();
    }

    emit assignPlaylistToPlayerRequestFinished(reply);
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
        data = query.toString(QUrl::FullyEncoded).toUtf8();

    QNetworkRequest networkRequest(url);
    manager->disconnect();

    if (request->name == "init")
    {
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(initVideoRequestFinishedSlot(QNetworkReply*)));
        manager->get(networkRequest);
    }
    else if (request->name == "enablePlaylist")
    {
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(enablePlayerRequestFinishedSlot(QNetworkReply*)));
        manager->get(networkRequest);
    }
    else if (request->name == "assignPlaylistToPlayer")
    {
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(assignPlaylistToPlayerRequestFinishedSlot(QNetworkReply*)));
        manager->get(networkRequest);
    }
    else if (request->name == "getPlaylist")
    {
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(getPlaylistRequestFinishedSlot(QNetworkReply*)));
        manager->get(networkRequest);
    }
    else if (request->name == "statistics")
    {
        connect (manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(sendStatisticRequestFinishedSlot(QNetworkReply*)));
        manager->post(networkRequest,data);
    }
    else
    {
        qDebug() << "ERROR: undefined method: " << request->name;
    }
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

    methodAPI = "statistics";
    name = "statistics";
    method = "POST";
}

SendStatisticRequest::~SendStatisticRequest()
{

}

#include <QUrlQuery>
#include <QUrl>
#include <QNetworkRequest>
#include <QDebug>
#include "videoservice.h"

VideoService::VideoService(QString serverURL, QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->serverURL = serverURL;
    currentRequest = NULL;
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
    emit initVideoRequestFinished(reply);
    nextRequest();
}

void VideoService::enablePlayerRequestFinishedSlot(QNetworkReply *reply)
{
    emit enablePlayerRequestFinished(reply);
    nextRequest();
}

void VideoService::assignPlaylistToPlayerRequestFinishedSlot(QNetworkReply *reply)
{
    emit assignPlaylistToPlayerRequestFinished(reply);
    nextRequest();
}

void VideoService::getPlaylistRequestFinishedSlot(QNetworkReply *reply)
{
    emit getPlaylistRequestFinished(reply);
    nextRequest();
}

void VideoService::performRequest(VideoServiceRequest *request)
{
    QUrl url(serverURL);
    QUrlQuery query;
    foreach (const VideoServiceRequest::VideoServiceRequestParam& param, request->params)
        query.addQueryItem(param.key,param.value);
    url.setPath("/" + request->methodAPI);
    url.setQuery(query);

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
    //http://api.teleds.com/enabled?player_id=3xsg-xuc5-ykdp
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

    cryptedSessionKeyParam.key = "playlist";
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

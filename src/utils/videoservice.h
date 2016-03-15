#ifndef VIDEOSERVICE_H
#define VIDEOSERVICE_H

#include <QObject>
#include <QQueue>
#include <QVector>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "videoserviceresult.h"

//http://api.teleds.com/initialization

class VideoServiceRequest
{
    friend class VideoService;
public:
    VideoServiceRequest(){;}
    virtual ~VideoServiceRequest(){;}

    struct VideoServiceRequestParam
    {
        QString key;
        QString value;
    };
protected:
    QVector<VideoServiceRequestParam> params;
    QString methodAPI;
    QString name;
    QString method;
};

class InitVideoPlayerRequest : public VideoServiceRequest
{
public:
    InitVideoPlayerRequest();
    virtual ~InitVideoPlayerRequest();
};

class EnablePlayerRequest : public VideoServiceRequest
{
public:
    EnablePlayerRequest(QString playerId);
    virtual ~EnablePlayerRequest();
};

class AssignPlaylistToPlayerRequest : public VideoServiceRequest
{
public:
    AssignPlaylistToPlayerRequest(QString playerId, int playlistId);
    virtual ~AssignPlaylistToPlayerRequest();
};

class GetPlaylistRequest : public VideoServiceRequest
{
public:
    GetPlaylistRequest(QString playerId, QString cryptedSessionKey);
    virtual ~GetPlaylistRequest();
};

class SendStatisticRequest : public VideoServiceRequest
{
public:
    SendStatisticRequest(QString playerId, QString encryptedSessionKey, QString data);
    virtual ~SendStatisticRequest();
};


//---------------------------------------------------------------------
class VideoService : public QObject
{
    Q_OBJECT
public:
    explicit VideoService(QString serverURL, QObject *parent = 0);

    void init();
    void enablePlayer(QString playerId);
    void assignPlaylist(QString playerId, int playlistId);
    void getPlaylist(QString playerId, QString cryptedSessionKey);
    void sendStatistic(QString playerId, QString encodedSessionKey, QString data);

    void executeRequest(VideoServiceRequest* request);

signals:
    void initResult(InitRequestResult result);
    void enablePlayerResult(QString result);
    void assignPlaylistResult(QString result);
    void getPlaylistResult(PlayerConfig result);
    void sendStatisticResult(NonQueryResult result);

    void initVideoRequestFinished(QNetworkReply * reply);
    void enablePlayerRequestFinished(QNetworkReply * reply);
    void assignPlaylistToPlayerRequestFinished(QNetworkReply * reply);
    void getPlaylistRequestFinished(QNetworkReply * reply);
    void sendStatisticRequestFinished(QNetworkReply * reply);

public slots:
    void initVideoRequestFinishedSlot(QNetworkReply * reply);
    void enablePlayerRequestFinishedSlot(QNetworkReply * reply);
    void assignPlaylistToPlayerRequestFinishedSlot(QNetworkReply * reply);
    void getPlaylistRequestFinishedSlot(QNetworkReply * reply);
    void sendStatisticRequestFinishedSlot(QNetworkReply * reply);

private:

    void performRequest(VideoServiceRequest* request);
    void nextRequest();


    QQueue<VideoServiceRequest*> requests;
    QNetworkAccessManager * manager;
    VideoServiceRequest * currentRequest;
    QString serverURL;
    VideoServiceResultProcessor resultProcessor;
};

//----------------------------------------------------------------
#endif // VIDEOSERVICE_H

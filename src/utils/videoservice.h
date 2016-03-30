#ifndef VIDEOSERVICE_H
#define VIDEOSERVICE_H

#include <QObject>
#include <QQueue>
#include <QVector>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
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
        VideoServiceRequestParam(){;}
        VideoServiceRequestParam(QString key, QString value);
        ~VideoServiceRequestParam(){;}
        QString key;
        QString value;
    };
protected:
    QVector<VideoServiceRequestParam> params;
    QString methodAPI;
    QString name;
    QString method;
    QByteArray body;
    QHash<QString, QString> headers;
    QHash<QNetworkRequest::KnownHeaders, QString> knownHeaders;

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

class AdvancedStatisticRequest : public VideoServiceRequest
{
public:
    AdvancedStatisticRequest();
    virtual ~AdvancedStatisticRequest();
};

class GetPlaylistSettingsRequest : public VideoServiceRequest
{
public:
    GetPlaylistSettingsRequest();
    virtual ~GetPlaylistSettingsRequest();
};

class GetPlayerAreasRequest : public VideoServiceRequest
{
public:
    GetPlayerAreasRequest();
    virtual ~GetPlayerAreasRequest();
};
class GetVirtualScreenPlaylistRequest : public VideoServiceRequest
{
public:
    GetVirtualScreenPlaylistRequest();
    GetVirtualScreenPlaylistRequest(QStringList areas);
    virtual ~GetVirtualScreenPlaylistRequest(){;}
};


//---------------------------------------------------------------------
class VideoService : public QObject
{
    Q_OBJECT
public:
    explicit VideoService(QString serverURL, QObject *parent = 0);

    void init();
    void getPlaylist(QString playerId, QString cryptedSessionKey);
    void getPlayerSettings();
    void getPlayerAreas();
    void getPlaylist(QString areaId);
    void getPlaylist();


    void sendStatistic(QString playerId, QString encodedSessionKey, QString data);
    void advancedInit();

    void executeRequest(VideoServiceRequest* request);



signals:
    void initResult(InitRequestResult result);
    void getPlaylistResult(PlayerConfig result);
    void sendStatisticResult(NonQueryResult result);
    void getPlayerSettings(SettingsRequestResult result);
    void getPlayerAreasResult(PlayerConfigNew result);

    void initVideoRequestFinished(QNetworkReply * reply);
    void getPlaylistRequestFinished(QNetworkReply * reply);
    void sendStatisticRequestFinished(QNetworkReply * reply);
    void getPlayerSettingsRequestFinished(QNetworkReply * reply);
    void getPlayerAreasRequestFinished(QNetworkReply * reply);
    void getVirtualScreenPlaylistRequestFinished(QNetworkReply* reply);

public slots:
    void initVideoRequestFinishedSlot(QNetworkReply * reply);
    void getPlaylistRequestFinishedSlot(QNetworkReply * reply);
    void sendStatisticRequestFinishedSlot(QNetworkReply * reply);
    void getPlayerSettingsRequestFinishedSlot(QNetworkReply * reply);
    void getPlayerAreasRequestFinishedSlot(QNetworkReply * reply);
    void getVirtualScreenPlaylistRequestFinishedSlot(QNetworkReply * reply);

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

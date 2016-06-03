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
    friend class VideoServiceRequestFabric;
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

class VideoServiceRequestFabric
{
public:
    static VideoServiceRequest initVideoPlayerRequest();
    static VideoServiceRequest getPlaylistRequest(QString playerId, QString cryptedSessionKey);
    static VideoServiceRequest sendStatisticRequest(QString data);
    static VideoServiceRequest sendEventsRequest(QString data);
    static VideoServiceRequest sendPlaysRequest(QString data);
    static VideoServiceRequest advancedInitRequest();
    static VideoServiceRequest getPlaylistSettingsRequest();
    static VideoServiceRequest getPlayerAreasRequest();
    static VideoServiceRequest getVirtualScreenPlaylistRequest();
    static VideoServiceRequest getVirtualScreenPlaylistRequest(QStringList areas);
};
//---------------------------------------------------------------------

/*class NetworkManager : public QObject
{
    Q_OBJECT
public:
    NetworkManager(QObject *parent);
    ~NetworkManager();

    void performRequest(VideoServiceRequest &request);
};*/


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


    void sendStatistic(QString data);
    void sendPlays(QString data);
    void sendEvents(QString data);
    void advancedInit();

    void executeRequest(VideoServiceRequest request);

    bool processReplyError(const QNetworkReply * reply, QString method);

signals:
    void initResult(InitRequestResult result);
    void getPlaylistResult(PlayerConfig result);
    void sendStatisticResult(NonQueryResult result);
    void sendStatisticEventsResult(NonQueryResult result);
    void sendStatisticPlaysResult(NonQueryResult result);
    void getPlayerSettings(SettingsRequestResult result);
    void getPlayerAreasResult(PlayerConfigNew result);
    void getVirtualScreenPlaylistResult(QHash<QString, PlaylistAPIResult> result);

    void initVideoRequestFinished(QNetworkReply * reply);
    void getPlaylistRequestFinished(QNetworkReply * reply);
    void sendStatisticRequestFinished(QNetworkReply * reply);
    void sendStatisticPlaysRequestFinished(QNetworkReply * reply);
    void sendStatisticEventsRequestFinished(QNetworkReply * reply);
    void getPlayerSettingsRequestFinished(QNetworkReply * reply);
    void getPlayerAreasRequestFinished(QNetworkReply * reply);
    void getVirtualScreenPlaylistRequestFinished(QNetworkReply* reply);

public slots:
    void initVideoRequestFinishedSlot(QNetworkReply * reply);
    void getPlaylistRequestFinishedSlot(QNetworkReply * reply);
    void sendStatisticRequestFinishedSlot(QNetworkReply * reply);
    void sendStatisticPlaysRequestFinishedSlot(QNetworkReply * reply);
    void sendStatisticEventsRequestFinishedSlot(QNetworkReply * reply);
    void getPlayerSettingsRequestFinishedSlot(QNetworkReply * reply);
    void getPlayerAreasRequestFinishedSlot(QNetworkReply * reply);
    void getVirtualScreenPlaylistRequestFinishedSlot(QNetworkReply * reply);

private:

    void performRequest(VideoServiceRequest request);
    void nextRequest();


    QQueue<VideoServiceRequest> requests;
    QNetworkAccessManager * manager;
    VideoServiceRequest currentRequest;
    bool currentRequestExists;
    QString serverURL;
    VideoServiceResponseHandler resultProcessor;
};



//----------------------------------------------------------------
#endif // VIDEOSERVICE_H

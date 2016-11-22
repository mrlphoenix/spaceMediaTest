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
    static VideoServiceRequest sendEventsRequest(QString data);
    static VideoServiceRequest advancedInitRequest(QByteArray bodyData);
    static VideoServiceRequest getPlaylistRequest();
    static VideoServiceRequest getSettingsRequest();
    static VideoServiceRequest getUpdateVersion(QString platform);
};
//---------------------------------------------------------------------
class VideoService : public QObject
{
    Q_OBJECT
public:
    explicit VideoService(QString serverURL, QObject *parent = 0);

    void advancedInit(QByteArray data);
    void getPlayerSettings();
    void getPlaylist();
    void sendEvents(QString data);
    void getUpdates(QString platform);

    void executeRequest(VideoServiceRequest request);
    bool processReplyError(const QNetworkReply * reply, QString method);

signals:
    void initResult(InitRequestResult result);
    void getPlaylistResult(PlayerConfigAPI result);
    void sendStatisticEventsResult(NonQueryResult result);
    void getPlayerSettings(SettingsRequestResult result);
    void getUpdatesResult(UpdateInfoResult result);

    void initVideoRequestFinished(QNetworkReply * reply);
    void getPlaylistRequestFinished(QNetworkReply * reply);
    void sendStatisticEventsRequestFinished(QNetworkReply * reply);
    void getPlayerSettingsRequestFinished(QNetworkReply * reply);
    void getUpdatesRequestFinished(QNetworkReply * reply);

public slots:
    void initVideoRequestFinishedSlot(QNetworkReply * reply);
    void getPlaylistRequestFinishedSlot(QNetworkReply * reply);
    void sendStatisticEventsRequestFinishedSlot(QNetworkReply * reply);
    void getPlayerSettingsRequestFinishedSlot(QNetworkReply * reply);
    void getUpdatesRequestFinishedSlot(QNetworkReply * reply);

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

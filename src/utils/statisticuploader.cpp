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

    connect(&DatabaseInstance,SIGNAL(playsFound(QList<StatisticDatabase::Play>)),this,SLOT(playsReady(QList<StatisticDatabase::Play>)));
    connect(&DatabaseInstance, SIGNAL(systemInfoFound(QList<PlatformSpecific::SystemInfo>)),this, SLOT(systemInfoReady(QList<PlatformSpecific::SystemInfo>)));
    connect(videoService,SIGNAL(sendStatisticResult(NonQueryResult)),this,SLOT(uploadResult(NonQueryResult)));
}

bool StatisticUploader::start()
{
    DatabaseInstance.findSystemInfoToSend();
    DatabaseInstance.findPlaysToSend();
    return false;
}

void StatisticUploader::playsReady(QList<StatisticDatabase::Play> plays)
{
    if (plays.count() == 0)
        return;
    QJsonArray result;
    foreach (const StatisticDatabase::Play &play, plays)
        result.append(play.serialize());
    QJsonDocument doc(result);
    QString strToSend = doc.toJson();
    videoService->sendPlays(strToSend);

  //  this->plays = plays;
  //  QTimer::singleShot(500,this,SLOT(nextState()));
}
void StatisticUploader::systemInfoReady(QList<PlatformSpecific::SystemInfo> data)
{
    qDebug() << "StatisticUploader::systemInfoReady";
    if (data.count() == 0)
        return;
    QJsonArray result;
    foreach (const PlatformSpecific::SystemInfo &info, data)
        result.append(info.serialize());
    QJsonDocument doc(result);
    QString strToSend = doc.toJson();

    videoService->sendStatistic(strToSend);
  //  this->monitoring = monitoring;
    //  QTimer::singleShot(500,this,SLOT(nextState()));
}

void StatisticUploader::uploadResult(NonQueryResult result)
{
    if (result.status == "success")
    {
        qDebug() << "UPLOAD SUCCESS!!!";
        //DatabaseInstance.uploadingSuccessfull();
    }
    else
    {
        qDebug() << "UPLOAD FAILED" << result.source;
       // DatabaseInstance.uploadingFailed();
    }
}

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

StatisticUploader::StatisticUploader(VideoService *videoService, QObject *parent) : QObject(parent)
{
    state = IDLE;

    this->videoService = videoService;

    connect(&DatabaseInstance,SIGNAL(playsFound(QList<StatisticDatabase::Play>)),this,SLOT(playsReady(QList<StatisticDatabase::Play>)));
    connect(&DatabaseInstance, SIGNAL(reportsFound(QList<StatisticDatabase::Report>)),this,SLOT(reportsReady(QList<StatisticDatabase::Report>)));
    connect(&DatabaseInstance, SIGNAL(systemInfoFound(QList<StatisticDatabase::SystemInfo>)),this, SLOT(systemInfoReady(QList<StatisticDatabase::SystemInfo>)));
    connect(&DatabaseInstance, SIGNAL(gpsFound(QList<StatisticDatabase::GPS>)),this, SLOT(gpsReady(QList<StatisticDatabase::GPS>)));
    connect(videoService,SIGNAL(sendStatisticResult(NonQueryResult)),this,SLOT(uploadResult(NonQueryResult)));
}

bool StatisticUploader::start()
{
    if (state == IDLE)
    {
        nextState();
        return true;
    }
    return false;
}

void StatisticUploader::playsReady(QList<StatisticDatabase::Play> plays)
{
    this->plays = plays;
    QTimer::singleShot(500,this,SLOT(nextState()));
}

void StatisticUploader::reportsReady(QList<StatisticDatabase::Report> reports)
{
    qDebug() << "UPLOADER: REPORTS READY!";
    this->reports = reports;
    QTimer::singleShot(500,this,SLOT(nextState()));
}

void StatisticUploader::systemInfoReady(QList<StatisticDatabase::SystemInfo> monitoring)
{
    this->monitoring = monitoring;
    QTimer::singleShot(500,this,SLOT(nextState()));
}

void StatisticUploader::gpsReady(QList<StatisticDatabase::GPS> gpses)
{
    this->gpses = gpses;
    QTimer::singleShot(500,this,SLOT(nextState()));
}

void StatisticUploader::nextState()
{
    if (state == IDLE)
    {
        state = GRABBING_PLAYS;
    }
    else if (state == GRABBING_PLAYS)
        state = GRABBING_REPORTS;
    else if (state == GRABBING_REPORTS)
        state = GRABBING_SYSTEM_INFO;
    else if (state == GRABBING_SYSTEM_INFO)
        state = GRABBING_GPS;
    else if (state == GRABBING_GPS)
        state = PEEKING;
    else if (state == PEEKING)
        state = UPLOADING;
    else
        state = IDLE;
    runStateStep();
}

void StatisticUploader::replyFinished(QNetworkReply *reply)
{
    qDebug() << reply->readAll();
}

void StatisticUploader::uploadResult(NonQueryResult result)
{
    if (result.status == "success")
    {
        qDebug() << "UPLOAD SUCCESS!!!";
        DatabaseInstance.uploadingSuccessfull();
    }
    else
    {
        qDebug() << "UPLOAD FAILED" << result.error_text;
        DatabaseInstance.uploadingFailed();
    }
    toIdleState();
}

void StatisticUploader::toIdleState()
{
    plays.clear();
    reports.clear();
    monitoring.clear();
    gpses.clear();
}

void StatisticUploader::runStateStep()
{
    if (state == GRABBING_PLAYS)
    {
        DatabaseInstance.findPlaysToSend();
        qDebug() << "UPLOADER: grabbing plays";
    }
    else if (state == GRABBING_REPORTS)
    {
        DatabaseInstance.findReportsToSend();
        qDebug() << "UPLOADER: grabbing reports";
    }
    else if (state == GRABBING_SYSTEM_INFO)
    {
        DatabaseInstance.findSystemInfoToSend();
        qDebug() << "UPLOADER: grabbing system info";
    }
    else if (state == GRABBING_GPS)
    {
        qDebug() << "UPLOADER: Grabbing gps";
        DatabaseInstance.findGPStoSend();
    }
    else if (state == PEEKING)
    {
        qDebug() << "UPLOADER: peeking items";
        DatabaseInstance.peekItems();
        nextState();
    }
    else if (state == UPLOADING)
    {
        QJsonDocument doc(generateStatisticModel());
        QByteArray data = doc.toJson();
        QFile statJson("s.txt");
        statJson.open(QFile::WriteOnly);
        statJson.write(data);
        statJson.flush();
        statJson.close();

        QProcess gzipProcess;
        QProcess encryptProcess;

        gzipProcess.setStandardOutputProcess(&encryptProcess);
        QString hexSessionKey = GlobalConfigInstance.getSessionKey().toLocal8Bit().toHex();
        gzipProcess.start("gzip -9 -k -c s.txt");
        encryptProcess.start("openssl enc -aes-256-cbc -base64 -K " + hexSessionKey + " -iv 30303030303030303030303030303030");

        bool retval = false;
        while ((retval = encryptProcess.waitForFinished()));

        QByteArray result = encryptProcess.readAll();
        QString base64Data = result;

        base64Data = QUrl::toPercentEncoding(base64Data.replace("\n","").replace("\r",""));

        qDebug() << QString(base64Data);

        //build request
    /*   QUrl url("http://api.teleds.com/statistics");
        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QUrlQuery params;
        params.addQueryItem("player_id", GlobalConfigInstance.getPlayerId());
        params.addQueryItem("ctypted_session_key", GlobalConfigInstance.getEncryptedSessionKey());
        params.addQueryItem("statistics", base64Data);

        qDebug() << "UPLOAD QUERY:::" + params.query();
        QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));

        manager.post(request, params.toString(QUrl::FullyEncoded).toUtf8());*/
        videoService->sendStatistic(GlobalConfigInstance.getPlayerId(), GlobalConfigInstance.getEncryptedSessionKey(), base64Data);
    }
}

QJsonObject StatisticUploader::generateStatisticModel()
{
    QJsonObject root;
    QJsonObject player;
    QJsonObject currentGps;
    player["id"] = GlobalConfigInstance.getPlayerId();
    player["time"] = StatisticDatabase::serializeDate(QDateTime::currentDateTime());

    currentGps["latitude"] = 0.0;
    currentGps["longitude"] = 0.0;

    player["gps"] = currentGps;
    player["version"] = 0;

    root["player"] = player;

    QJsonArray statistics;
    foreach (const StatisticDatabase::Play &play,plays)
    {
        QJsonObject playObject;
        playObject["area_id"] = play.areaId;
        playObject["playlist_id"] = play.playlistId;
        playObject["item_id"] = play.iid;
        playObject["time"] = StatisticDatabase::serializeDate(play.time);

        QJsonObject playGps;
        playGps["latitude"] = play.latitude;
        playGps["longitude"] = play.longitude;
        playObject["gps"] = playGps;

        playObject["version"] = 0;

        statistics.append(playObject);
    }
    root["statistics"] = statistics;

    QJsonArray reportArray;
    foreach (const StatisticDatabase::Report &report, reports)
    {
        QJsonObject reportObject;
        reportObject["time"] = StatisticDatabase::serializeDate(report.time);
        reportObject["downloads"] = report.downloads;
        reportObject["content_play"] = report.contentPlay;
        reportObject["content_total"] = report.contentTotal;
        reportObject["error_connect"] = report.errorConnect;
        reportObject["error_playlist"] = report.errorPlaylist;
        reportArray.append(reportObject);
    }
    root["reports"] = reportArray;

    QJsonArray monitoringArray;
    foreach (const StatisticDatabase::SystemInfo &sysInfo, monitoring)
    {
        QJsonObject monitoringObject;
        monitoringObject["time"] = StatisticDatabase::serializeDate(sysInfo.time);
        monitoringObject["cpu"] = sysInfo.cpu;
        monitoringObject["memory"] = sysInfo.memory;

        QJsonObject trafficObject;
        trafficObject["in"] = sysInfo.trafficIn;
        trafficObject["out"] = sysInfo.trafficOut;

        monitoringObject["traffic"] = trafficObject;
        monitoringObject["monitor"] = sysInfo.monitor ? true : false;
        monitoringObject["connect"] = sysInfo.connect ? true : false;
        monitoringObject["balance"] = sysInfo.balance;
        monitoringArray.append(monitoringObject);
    }
    root["monitoring"] = monitoringArray;

    QJsonArray gpsArray;
    foreach (const StatisticDatabase::GPS &gps, gpses)
    {
        QJsonObject gpsRootObject;
        gpsRootObject["time"] = StatisticDatabase::serializeDate(gps.time);
        QJsonObject gpsObject;
        gpsObject["latitude"] = gps.latitude;
        gpsObject["longitude"] = gps.longitude;
        gpsRootObject["gps"] = gpsObject;
        gpsArray.append(gpsRootObject);
    }
    root["gps"] = gpsArray;
    return root;
}


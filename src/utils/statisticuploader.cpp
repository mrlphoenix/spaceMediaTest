#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QProcess>

#include "statisticuploader.h"
#include "globalstats.h"
#include "globalconfig.h"
#include "singleton.h"

StatisticUploader::StatisticUploader(QObject *parent) : QObject(parent)
{
    state = IDLE;
    connect(&DatabaseInstance,SIGNAL(playsFound(QList<Play>)),this,SLOT(playsReady(QList<StatisticDatabase::Play>)));
    connect(&DatabaseInstance, SIGNAL(reportsFound(QList<Report>)),this,SLOT(reportsReady(QList<StatisticDatabase::Report>)));
    connect(&DatabaseInstance, SIGNAL(systemInfoFound(QList<SystemInfo>)),this, SLOT(systemInfoReady(QList<StatisticDatabase::SystemInfo>)));
    connect(&DatabaseInstance, SIGNAL(gpsFound(QList<GPS>)),this, SLOT(gpsReady(QList<StatisticDatabase::GPS>)));
}

bool StatisticUploader::start()
{

}

void StatisticUploader::playsReady(QList<StatisticDatabase::Play> plays)
{
    this->plays = plays;
    nextState();
}

void StatisticUploader::reportsReady(QList<StatisticDatabase::Report> reports)
{
    this->reports = reports;
    nextState();
}

void StatisticUploader::systemInfoReady(QList<StatisticDatabase::SystemInfo> monitoring)
{
    this->monitoring = monitoring;
    nextState();
}

void StatisticUploader::gpsReady(QList<StatisticDatabase::GPS> gpses)
{
    this->gpses = gpses;
    nextState();
}

void StatisticUploader::nextState()
{
    if (state == IDLE)
        state = GRABBING_PLAYS;
    else if (state == GRABBING_PLAYS)
        state = GRABBING_REPORTS;
    else if (state == GRABBING_SYSTEM_INFO)
        state = GRABBING_SYSTEM_INFO;
    else if (state == GRABBING_GPS)
        state = PEEKING;
    else if (state == PEEKING)
        state = UPLOADING;
    else
        state = IDLE;
    runStateStep();
}

void StatisticUploader::runStateStep()
{
    if (state == GRABBING_PLAYS)
        DatabaseInstance.findPlaysToSend();
    else if (state == GRABBING_REPORTS)
        DatabaseInstance.findReportsToSend();
    else if (state == GRABBING_SYSTEM_INFO)
        DatabaseInstance.findSystemInfoToSend();
    else if (state == GRABBING_GPS)
        DatabaseInstance.findGPStoSend();
    else if (state == PEEKING)
    {
        DatabaseInstance.peekItems();
        nextState();
    }
    else if (state == UPLOADING)
    {
        QJsonDocument doc(generateStatisticModel());
        QString docString = doc.toJson();
        docString = docString.replace("\r","").replace("\n","");

        //QByteArray data = docString.toLocal8Bit();
        //Encrypt and encode
        //

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


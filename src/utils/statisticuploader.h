#ifndef STATISTICUPLOADER_H
#define STATISTICUPLOADER_H

#include <QObject>
#include <QList>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "statisticdatabase.h"
#include "videoservice.h"

class StatisticUploader : public QObject
{
    Q_OBJECT
public:
    explicit StatisticUploader(VideoService* videoService, QObject *parent = 0);
    enum UploadState{IDLE, GRABBING_PLAYS, GRABBING_REPORTS, GRABBING_SYSTEM_INFO, GRABBING_GPS, PEEKING, UPLOADING};
signals:
    void finished(bool success);
public slots:
    bool start();


    void playsReady(QList<StatisticDatabase::Play> plays);
    void reportsReady(QList<StatisticDatabase::Report> reports);
    void systemInfoReady(QList<StatisticDatabase::SystemInfo> monitoring);
    void gpsReady(QList<StatisticDatabase::GPS> gpses);

    void nextState();
    void replyFinished(QNetworkReply * reply);
    void uploadResult(NonQueryResult result);

private:
    void toIdleState();
    void runStateStep();
    QJsonObject generateStatisticModel();
    QList<StatisticDatabase::Play> plays;
    QList<StatisticDatabase::Report> reports;
    QList<StatisticDatabase::SystemInfo> monitoring;
    QList<StatisticDatabase::GPS> gpses;

    UploadState state;
    QNetworkAccessManager manager;
    VideoService * videoService;
};

#endif // STATISTICUPLOADER_H

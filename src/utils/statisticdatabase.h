#ifndef STATISTICDATABASE_H
#define STATISTICDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QHash>
#include <QThread>
#include <QSqlError>
#include <QList>
#include <QSqlRecord>
#include <QDateTime>
#include "singleton.h"


#define DatabaseInstance Singleton<StatisticDatabase>::instance()


class DatabaseWorker : public QObject
{
    Q_OBJECT

public:
    DatabaseWorker(QString dbName, QObject* parent = 0);
    ~DatabaseWorker();

public slots:
    void slotExecute(const QString &queryId, const QString &sql);
    void slotExecutePrepared(const QString &queryId, const QString &resultId = QString());
    void slotPrepare(const QString &queryId, const QString &sql);
    void slotBindValue(const QString &queryId, const QString &placeholder, const QVariant &val);

signals:
    void executed(const QString &queryId, const QString &resultId);
    void executeFailed(const QString &queryId, const QSqlError &err, const QString &resultId);
    void prepared(const QString &queryId);
    void prepareFailed(const QString &queryId, const QSqlError &err);
    void results(const QString &queryId, const QList<QSqlRecord> &records, const QString &resultId);

private:
    QSqlDatabase m_database;
    QHash<QString, QSqlQuery*> m_queries;
    void executeOneTime(const QString &queryId, const QString &sql);
    void executePrepared(const QString &queryId, const QString &resultId = QString());
};


class QueryThread : public QThread
{
    Q_OBJECT

public:
    QueryThread(QString dbName, QObject *parent = 0);
    ~QueryThread();

    void execute(const QString &queryId, const QString &sql);
    void executePrepared(const QString &queryId, const QString &resultId = QString());
    void prepare(const QString &queryId, const QString &sql);
    void bindValue(const QString &queryId, const QString &placeholder, const QVariant &val);

signals:
    void progress( const QString &msg);
    void ready(bool);
    void executed(const QString &queryId, const QString &resultId);
    void executeFailed(const QString &queryId, const QSqlError &err, const QString &resultId);
    void prepared(const QString &queryId);
    void prepareFailed(const QString &queryId, const QSqlError &err);
    void results(const QString &queryId, const QList<QSqlRecord> &records, const QString &resultId);

protected:
    void run();

signals:
    void fwdExecute(const QString &queryId, const QString &sql);
    void fwdExecutePrepared(const QString &queryId, const QString &resultId = QString());
    void fwdPrepare(const QString &queryId, const QString &sql);
    void fwdBindValue(const QString &queryId, const QString &placeholder, const QVariant &val);

private:
    DatabaseWorker *m_worker;
    QString dbName;
};

class StatisticDatabase : public QObject
{
    Q_OBJECT
public:
    explicit StatisticDatabase(QObject *parent = 0);

    static QString serializeDate(QDateTime date);
    static QDateTime deserializeDate(QString date);

    //resource operations

    void registryResource(QString iid, QString name, QDateTime lastupdated, int size);
    void updateResourceDownloadStatus(QString iid, int filesize);
    void findResource(QString iid);
    void resourceCount();

    void playResource(int areaId, int playlistId, QString itemId, double latitude, double longitude);
    void findPlaysToSend();

    void createReport(int downloads, int contentPlay, int contentTotal, int error_connect, int error_playlist);
    void findReportsToSend();

    void createSystemInfo(int cpu, int memory, double trafficIn, double trafficOut, bool monitor, bool connection, double balance);
    void findSystemInfoToSend();

    void createGPS(double latitude, double longitude);
    void findGPStoSend();

    //statistic operation
    void peekItems(); //call this after you peeked all items to upload to the server
    void uploadingFailed(); //call this if uploading failed
    void uploadingSuccessfull(); //call this upon uploading success

    struct Play
    {
        static Play fromRecord(const QSqlRecord& record);
        int playId;
        int areaId;
        int playlistId;
        QString iid;
        QDateTime time;
        double latitude, longitude;
        QString version;
        int sent;
    };
    struct Report
    {
        static Report fromRecord(const QSqlRecord& record);
        int reportId;
        QDateTime time;
        int downloads;
        int contentPlay;
        int contentTotal;
        int errorConnect;
        int errorPlaylist;
        int sent;
    };
    struct SystemInfo
    {
        static SystemInfo fromRecord(const QSqlRecord& record);
        int systemInfoId;
        QDateTime time;
        int cpu;
        int memory;
        double trafficIn;
        double trafficOut;
        int monitor;
        int connect;
        double balance;
        int sent;
    };
    struct GPS
    {
        static GPS fromRecord(const QSqlRecord& record);
        int gpsId;
        QDateTime time;
        double latitude;
        double longitude;
    };

signals:
    void resourceFound(QList<QSqlRecord> records);
    void playsFound(QList<StatisticDatabase::Play> records);
    void reportsFound(QList<StatisticDatabase::Report> records);
    void systemInfoFound(QList<StatisticDatabase::SystemInfo> records);
    void gpsFound(QList<StatisticDatabase::GPS> records);
    void resourceCount(int count);
    void unknownResult(QString queryId, QList<QSqlRecord> records);

public slots:

private:
    const QString databaseName = "stat.db";
    QueryThread * queryThread;
private slots:
    void slotResults(const QString &queryId, const QList<QSqlRecord> &records, const QString);
};

#endif // STATISTICDATABASE_H

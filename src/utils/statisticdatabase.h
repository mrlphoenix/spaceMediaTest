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
#include "platformspecific.h"
#include "videoserviceresult.h"

#define DatabaseInstance Singleton<StatisticDatabase>::instance()


//Database Worker - is universal worker for database
//it can execute any query and will raise signal when it will be executed
//also it incapsulate query queue
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

//QueryThread class is needed
//to run DB Worker in separated thread so we wont get freezes during transanctions
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

//StatisticDatabase - class that resembles service-layer interface for our DB configuration
//its singleton so you can just call StatisticDatabase.method in any other part of code

class StatisticDatabase : public QObject
{
    Q_OBJECT
public:
    explicit StatisticDatabase(QObject *parent = 0);

    static QString serializeDate(QDateTime date);
    static QDateTime deserializeDate(QString date);

    //registies player in DB
    void registryResource(QString iid, QString name, QDateTime lastupdated, int size);
    //outdated method used to track how much we downloaded
    void updateResourceDownloadStatus(QString iid, int filesize);

    //oudated methods
    void getResources();
    void resourceCount();

    //store in DB information about item was played and current state
    void createPlayEvent(PlayerConfigAPI::Campaign::Area::Content item, Platform::SystemInfo info);

    void removeResource(QString itemId);

    //statistic operations
    //outdated method - use eventsUploaded instead
    void playsUploaded();
    void systemInfoUploaded();

    //call this when we successfully upload event to server
    //it removes all items from event table
    void eventsUploaded();

    //find resources
    void findResource(QString iid);

    //outdated methods - use findEventstoSend
    void findPlaysToSend();
    void findSystemInfoToSend();
    void findEventsToSend();

    struct Play
    {
        static Play fromRecord(const QSqlRecord& record);
        QJsonObject serialize() const;
        QString time, screen, area, content, campaign;
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
    struct Resource
    {
        static Resource fromRecord(const QSqlRecord& record);
        QString iid;
        QString name;
        QDateTime lastupdated;
        int size;
        int filesize;
        QDateTime lastTimePlayed;
    };

    struct PlayEvent
    {
        static PlayEvent fromRecord(const QSqlRecord& record);
        QJsonObject serialize() const;
        QString time, screen, area, content, campaign;
        int systemInfoId;

        double cpu;
        double latitude;
        double longitude;
        double battery;
        int traffic;
        int free_memory;
        QString wifi_mac;
        int hdmi_cec;
        int hdmi_gpio;
        int free_space;
    };

signals:
    void resourceFound(QList<StatisticDatabase::Resource> records);
    void playsFound(QList<StatisticDatabase::Play> records);
    void systemInfoFound(QList<Platform::SystemInfo> records);
    void eventsFound(QList<StatisticDatabase::PlayEvent> records);
    void resourceCount(int count);
    void unknownResult(QString queryId, QList<QSqlRecord> records);

public slots:

private:
    QString databaseName;
    QueryThread * queryThread;
private slots:
    void slotResults(const QString &queryId, const QList<QSqlRecord> &records, const QString);
};

/*
 * 1. On First Cloud start we grab user's ip location, trying to search for info
 * 2. We store info on the server and add to person his ipd and other stuff
 * */
/*
 * 1. Open thread in browser.
 * 2. Open start picture, search in pictures for headers
 * 3. In the header find opID
 * 4. if id is public - grab token, if private - ask
 * */
/*
 * Message->gzip->encode();
 * encode:: keyfound^byte
 * Message->
 * */
#endif // STATISTICDATABASE_H


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
#include "platformspecs.h"
#include "videoserviceresult.h"

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

    void registryResource(QString iid, QString name, QDateTime lastupdated, int size);
    void updateResourceDownloadStatus(QString iid, int filesize);

    void getResources();
    void resourceCount();

    void playResource(PlaylistAPIResult::PlaylistItem item);
    void removeResource(QString itemId);

    void createSystemInfo(PlatformSpecific::SystemInfo info);

    //statistic operation
    void playsUploaded();
    void systemInfoUploaded();

    void findResource(QString iid);
    void findPlaysToSend();
    void findSystemInfoToSend();

    struct Play
    {
        static Play fromRecord(const QSqlRecord& record);
        QJsonObject serialize() const;
        QString time, screen, area, content;
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


signals:
    void resourceFound(QList<StatisticDatabase::Resource> records);
    void playsFound(QList<StatisticDatabase::Play> records);
    void systemInfoFound(QList<PlatformSpecific::SystemInfo> records);
    void resourceCount(int count);
    void unknownResult(QString queryId, QList<QSqlRecord> records);

public slots:

private:
    QString databaseName;
    QueryThread * queryThread;
private slots:
    void slotResults(const QString &queryId, const QList<QSqlRecord> &records, const QString);
};

#endif // STATISTICDATABASE_H


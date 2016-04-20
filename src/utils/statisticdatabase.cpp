#include <QDebug>
#include <QLocale>
#include "statisticdatabase.h"
#include "globalconfig.h"

DatabaseWorker::DatabaseWorker(QString dbName, QObject* parent)
    : QObject( parent )
{
    // thread-specific connection, see db.h
    m_database = QSqlDatabase::addDatabase("QSQLITE", "WorkerDatabase");
    m_database.setDatabaseName(dbName);
    m_database.setHostName("");
    if ( !m_database.open() )
    {
        qWarning() << "Unable to connect to database, giving up:" << m_database.lastError().text();
        return;
    }
    qDebug() << "DB OPENED";

    if (m_database.tables().count() < 5)
    {
        m_database.exec("create table resource (iid TEXT PRIMARY KEY, name TEXT, lastupdated TEXT, size INTEGER, filesize INTEGER, lastTimePlayed TEXT)");
        m_database.transaction();
        m_database.exec("create table play (play_id INTEGER PRIMARY KEY AUTOINCREMENT, time TEXT, screen TEXT, area TEXT, content TEXT)");
        m_database.transaction();
        m_database.exec("create table systemInfo (report_id INTEGER PRIMARY KEY AUTOINCREMENT, time TEXT, cpu REAL, latitude REAL, longitude REAL, battery REAL, traffic_in INTEGER, traffic_out INTEGER, free_memory INTEGER, wifi_mac TEXT, hdmi_cec INTEGER, hdmi_gpio INTEGER, free_space INTEGER)");
        m_database.transaction();
        m_database.commit();
    }
}

DatabaseWorker::~DatabaseWorker()
{
    qDeleteAll(m_queries);
}

void DatabaseWorker::slotExecute( const QString& queryId, const QString& sql)
{
    // if sql is not valid, treat this as already prepared statment
    // that needs to be executed
    bool isPrepared = (sql.isEmpty() || sql.isNull());

    if (isPrepared)
        executePrepared(queryId);
    else
        executeOneTime(queryId, sql);
}

void DatabaseWorker::slotExecutePrepared(const QString &queryId, const QString &resultId)
{
    executePrepared(queryId, resultId);
}


void DatabaseWorker::executeOneTime(const QString& queryId, const QString& sql)
{
    qDebug()<<"execute one tgime method Entering";
    // use previously defined db connection
    QSqlQuery query(m_database);
    // execute query, get result
    bool ok = query.exec(sql);
    qDebug() << ok;
    // check for errors
    if (!ok) {
        qDebug() << QString("execute failed for one time query id [%1]").arg(queryId) << "error: " << query.lastError();
        // emit error signal
        emit executeFailed(queryId, query.lastError(), QString());
        return;
    }
    // send executed signal
    qDebug() << "seems like query is executed well";
    emit executed(queryId, QString());
    // accumulate data and emit result
    QList<QSqlRecord> recs;
    while (query.next())
    {
        recs.append(query.record());
    }
    emit results(queryId, recs, QString());
}

void DatabaseWorker::executePrepared(const QString& queryId, const QString &resultId)
{
    // constuct result_id, if not set
    QString newResultId;
    if (resultId.isEmpty() || resultId.isNull())
        newResultId = QString("queryresult_%1").arg(QDateTime::currentMSecsSinceEpoch());
    else
        newResultId = resultId;

    // check if there is query with specified id
    if (!m_queries.contains(queryId)) {
        qDebug() << QString("prepared query id [%1] not found").arg(queryId);
        return;
    }
    // get query from hash
    QSqlQuery *query;
    query = m_queries.value(queryId);
    // execute and check query status
    bool ok = query->exec();

    if (!ok) {
        qDebug() << QString("execute failed for prepared query id [%1]").arg(queryId) << "error " << query->lastError();
        emit executeFailed(queryId, query->lastError(), newResultId);
        return;
    }
    // send executed signal
    emit executed(queryId, newResultId);

    QList<QSqlRecord> recs;
    while (query->next())
    {
        recs.append(query->record());
    }
    // result saved - release resources
    query->finish();
    // send results signal
    emit results(queryId, recs, newResultId);
}

void DatabaseWorker::slotPrepare(const QString &queryId, const QString &sql)
{
    // check if there is query with specified id
    if (m_queries.contains(queryId)) {
        qDebug() << QString("already prepared query id [%1]").arg(queryId);
        return;
    }
    QSqlQuery *query;
    // use previously defined db connection
    query = new QSqlQuery(m_database);
    // prepare and check query status
    bool ok = query->prepare(sql);

    if (!ok) {
        qDebug() << QString("prepare failed for query id [%1]").arg(queryId);
        emit prepareFailed(queryId, query->lastError());
        return;
    }
    // save query to our list
    m_queries.insert(queryId, query);
    // send prepared signal
    emit prepared(queryId);
}

void DatabaseWorker::slotBindValue(const QString &queryId, const QString &placeholder, const QVariant &val)
{
    // check if there is query with specified id
    if (!m_queries.contains(queryId)) {
        qDebug() << QString("prepared query id [%1] not found").arg(queryId);
        return;
    }
    // get query from hash
    QSqlQuery *query;
    query = m_queries.value(queryId);
    // bind value
    query->bindValue(placeholder, val);
}


QueryThread::QueryThread(QString dbName, QObject *parent)
    : QThread(parent)
{
    this->dbName = dbName;
}

QueryThread::~QueryThread()
{
    delete m_worker;
}

void QueryThread::execute(const QString &queryId, const QString &sql)
{
    qDebug() << "EXECUTE " + queryId + " [" + sql + "]";
    emit fwdExecute(queryId, sql); // forwards to the worker
}

void QueryThread::executePrepared(const QString &queryId, const QString &resultId)
{
    emit fwdExecutePrepared(queryId, resultId);
}

void QueryThread::prepare(const QString &queryId, const QString &sql)
{
    emit fwdPrepare(queryId, sql); // forwards to the worker
}

void QueryThread::bindValue(const QString &queryId, const QString &placeholder, const QVariant &val)
{
    emit fwdBindValue(queryId, placeholder, val); // forwards to the worker
}

void QueryThread::run()
{
    emit ready(false);
    emit progress( "QueryThread starting, one moment please..." );

    // Create worker object within the context of the new thread
    m_worker = new DatabaseWorker(dbName);

    connect( this, SIGNAL(fwdExecute(QString,QString)),
             m_worker, SLOT(slotExecute(QString,QString)) );

    connect( this, SIGNAL(fwdExecutePrepared(QString,QString)),
             m_worker, SLOT(slotExecutePrepared(QString,QString)) );

    connect( this, SIGNAL(fwdPrepare(QString,QString)),
             m_worker, SLOT(slotPrepare(QString,QString)) );

    connect( this, SIGNAL(fwdBindValue(QString,QString,QVariant)),
             m_worker, SLOT(slotBindValue(QString,QString,QVariant)) );

    qRegisterMetaType< QList<QSqlRecord> >( "QList<QSqlRecord>" );

    // forward final signals
    connect( m_worker, SIGNAL(results(QString,QList<QSqlRecord>,QString)),
             this, SIGNAL(results(QString,QList<QSqlRecord>,QString)) );

    connect( m_worker, SIGNAL(executed(QString,QString)),
             this, SIGNAL(executed(QString,QString)) );

    connect( m_worker, SIGNAL(executeFailed(QString,QSqlError,QString)),
             this, SIGNAL(executeFailed(QString,QSqlError,QString)) );

    connect( m_worker, SIGNAL(prepared(QString)),
             this, SIGNAL(prepared(QString)) );

    connect( m_worker, SIGNAL(prepareFailed(QString,QSqlError)),
             this, SIGNAL(prepareFailed(QString,QSqlError)) );

    emit progress("query runned");
    emit ready(true);

    exec();  // our event loop
}

//--------------------------------------------------------------------------------------------------------

StatisticDatabase::StatisticDatabase(QObject *parent) : QObject(parent)
{
    queryThread = new QueryThread(databaseName,parent);
    queryThread->start();
    connect (queryThread, SIGNAL(results(QString,QList<QSqlRecord>,QString)),this,SLOT(slotResults(QString,QList<QSqlRecord>,QString)));
}

void StatisticDatabase::registryResource(QString iid, QString name, QDateTime lastupdated, int size)
{
    QString sql = QString("insert  or replace into resource (iid,name,lastupdated,size,filesize,lastTimePlayed) VALUES('%1', '%2', '%3', %4, 0, NULL);").arg(
                iid, name, serializeDate(lastupdated), QString::number(size));
    queryThread->execute("registryResource",sql);
}

void StatisticDatabase::updateResourceDownloadStatus(QString iid, int filesize)
{
    QString sql = QString("update resource set filesize = %1 where iid = '%2'").arg(QString::number(filesize),iid);
    queryThread->execute("updateResourceDownloadStatus",sql);
}

void StatisticDatabase::findResource(QString iid)
{
    QString sql = QString("select * from resource where iid = '%1'").arg(iid);
    queryThread->execute("findResource", sql);
}

void StatisticDatabase::getResources()
{
    QString sql = "select * from resource";
    queryThread->execute("findResource", sql);
}

void StatisticDatabase::resourceCount()
{
    QString sql = "select count(*) cnt from resource";
    queryThread->execute("resourceCount",sql);
}

void StatisticDatabase::playResource(PlaylistAPIResult::PlaylistItem item)
{
    //create table play (play_id INTEGER PRIMARY KEY AUTOINCREMENT, time TEXT, screen TEXT, area TEXT, content TEXT)
    QString sql = QString("insert into play(time, screen, area, content) VALUES ('%1', '%2', '%3', '%4'").arg(
                    QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd HH:mm:ss"),
                    GlobalConfigInstance.getVirtualScreenId(),
                    item.areaId,
                    item.id
                );
    queryThread->execute("playResource", sql);
    sql = QString("update Resource set lastTimePlayed = '%1' where iid = '%2'").arg(serializeDate(QDateTime::currentDateTime()), item.id);
    queryThread->execute("playResource",sql);
}

void StatisticDatabase::playResource(int areaId, int playlistId, QString itemId, double latitude, double longitude)
{
    QString sql = QString("insert into play(area_id, playlist_id, iid, time, latitude, longitude, version) VALUES (%1, %2, '%3', '%4', %5, %6, 0)").arg(
                QString::number(areaId),QString::number(playlistId), itemId, serializeDate(QDateTime::currentDateTime()),
                QString::number(latitude), QString::number(longitude));
    queryThread->execute("playResource",sql);

    sql = QString("update Resource set lastTimePlayed = '%1' where iid = '%2'").arg(serializeDate(QDateTime::currentDateTime()), itemId);
    queryThread->execute("playResource",sql);
}

void StatisticDatabase::removeResource(QString itemId)
{
    QString sql = QString("delete from Resource where iid = '%1'").arg(itemId);
    queryThread->execute("removeResource",sql);
}

void StatisticDatabase::findPlaysToSend()
{
    QString sql = "select * from play where sent IS NULL";
    queryThread->execute("findPlaysToSend",sql);
}

void StatisticDatabase::createReport(int downloads, int contentPlay, int contentTotal, int error_connect, int error_playlist)
{
    QString sql = QString("insert into Report(time, downloads, content_play, content_total, error_connect, error_playlist) VALUES ('%1', %2, %3, %4, %5, %6)").arg(
                serializeDate(QDateTime::currentDateTime()),QString::number(downloads),QString::number(contentPlay), QString::number(contentTotal),
                QString::number(error_connect), QString::number(error_playlist));
    queryThread->execute("createReport", sql);
}

void StatisticDatabase::findReportsToSend()
{
    QString sql = "select * from report where sent IS NULL";
    queryThread->execute("findReportsToSend", sql);
}

void StatisticDatabase::createSystemInfo(PlatformSpecific::SystemInfo info)
{
    QString sql = QString(QString("insert into SystemInfo (time, cpu, latitude, longitude, battery, traffic_in, traffic_out, free_memory, wifi_mac, hdmi_cec, hdmi_gpio, free_space) ") +
                  QString("VALUES ('%1', %2, %3, %4, %5, %6, %6, %7, '%8', %9, %10, %11)")).arg(
                    info.time.toString("yyyy-MM-dd HH:mm:ss"),
                    QString::number(info.cpu),
                    QString::number(info.latitude), QString::number(info.longitude),
                    QString::number(info.battery), QString::number(info.traffic),
                    QString::number(info.free_memory), info.wifi_mac,
                    QString::number(info.hdmi_cec)).arg(
                    QString::number(info.hdmi_gpio),
                    QString::number(info.free_space));

    qDebug() << "SQL>>" + sql;

    //insert into SystemInfo (time, cpu, latitude, longitude, battery, traffic_in, traffic_out, free_memory, wifi_mac, hdmi_cec, hdmi_gpio, free_space)
    //VALUES ('YYYY-04-20 09:42:00', 11.85, 0, 0, 99, 0, 0, 42188800, '00:00:00:00:00:00', 1, 1, 7950688, %12)"

    queryThread->execute("createSystemInfo", sql);
}


void StatisticDatabase::createSystemInfo(int cpu, int memory, double trafficIn, double trafficOut, bool monitor, bool connection, double balance)
{
    QString sql = QString("insert into SystemInfo (time, cpu, memory, traffic, out, monitor, connect, balance) VALUES ('%1', %2, %3, %4, %5, %6, %7, %8)").arg(
                serializeDate(QDateTime::currentDateTime()),QString::number(cpu),QString::number(memory),
                QString::number(trafficIn),QString::number(trafficOut),
                QString::number(monitor), QString::number(connection), QString::number(balance));
    queryThread->execute("createSystemInfo", sql);
}

void StatisticDatabase::findSystemInfoToSend()
{
    QString sql = "select * from SystemInfo";
    queryThread->execute("findSystemInfoToSend",sql);
}

void StatisticDatabase::createGPS(double latitude, double longitude)
{
    QString sql = QString("insert into gps (time, latitude, longitude) VALUES ('%1', %2, %3)").arg(
                serializeDate(QDateTime::currentDateTime()),QString::number(latitude), QString::number(longitude));
    queryThread->execute("createGPS", sql);
}

void StatisticDatabase::findGPStoSend()
{
    QString sql = "select * from gps where sent IS NULL";
    queryThread->execute("findGPStoSend",sql);
}

void StatisticDatabase::peekItems()
{
    queryThread->execute("peekItems:play", "update Play set sent = 0 where SENT IS NULL");
    queryThread->execute("peekItems:report", "update Report set sent = 0 where SENT IS NULL");
    queryThread->execute("peekItems:systemInfo", "update systemInfo set sent = 0 WHERE sent IS NULL");
    queryThread->execute("peekItems:gps", "update gps set sent = 0 where sent is NULL");
}

void StatisticDatabase::uploadingFailed()
{
    queryThread->execute("uploadingFailed:play","update Play set sent = NULL where sent = 0");
    queryThread->execute("uploadingFailed:report","update Report set sent = NULL where sent = 0");
    queryThread->execute("uploadingFailed:systemInfo","update systemInfo set sent = NULL WHERE sent = 0");
    queryThread->execute("uploadingFailed:GPS","update gps set sent = NULL where sent = 0");
}

void StatisticDatabase::uploadingSuccessfull()
{
    queryThread->execute("uploadingSuccess:","delete from Play where sent = 0");
    queryThread->execute("uploadingSuccess:","delete from Report where sent = 0");
    queryThread->execute("uploadingSuccess:","delete from systemInfo where sent = 0");
    queryThread->execute("uploadingSuccess:","delete from gps where sent = 0");
}

QString StatisticDatabase::serializeDate(QDateTime date)
{
    QLocale locale(QLocale::English);
    return locale.toString(date,"ddd, dd MMM yyyy HH:mm:ss +0000");
}

QDateTime StatisticDatabase::deserializeDate(QString date)
{
    QLocale locale(QLocale::English);
    return locale.toDateTime(date.replace(" +0000",""),"ddd, dd MMM yyyy HH:mm:ss");
}
void StatisticDatabase::slotResults(const QString &queryId, const QList<QSqlRecord> &records, const QString)
{
    qDebug() << "slot result is called " << queryId;
    if (queryId == "findResource")
    {
        QList<StatisticDatabase::Resource> result;
        foreach (const QSqlRecord& record, records)
            result.append(Resource::fromRecord(record));
        emit resourceFound(result);
    }
    else if (queryId == "resourceCount")
    {
        if (records.count() > 0)
            emit resourceCount(records.at(0).value(0).toInt());
        else
            emit resourceCount(0);
    }
    else if (queryId == "findPlaysToSend")
    {
        QList<Play> plays;
        foreach (const QSqlRecord& record, records)
            plays.append(Play::fromRecord(record));
        emit playsFound(plays);
    }
    else if (queryId == "findReportsToSend")
    {
        QList<Report> reports;
        foreach (const QSqlRecord& record, records)
            reports.append(Report::fromRecord(record));
        emit reportsFound(reports);
    }
    else if (queryId == "findSystemInfoToSend")
    {
        QList<PlatformSpecific::SystemInfo> systemInfos;
        foreach (const QSqlRecord& record, records)
            systemInfos.append(PlatformSpecific::SystemInfo::fromRecord(record));
        emit systemInfoFound(systemInfos);
    }
    else if (queryId == "findGPStoSend")
    {
        QList<GPS> gpses;
        foreach (const QSqlRecord& record, records)
            gpses.append(GPS::fromRecord(record));
        emit gpsFound(gpses);
    }
    else
        emit unknownResult(queryId, records);
}


StatisticDatabase::Play StatisticDatabase::Play::fromRecord(const QSqlRecord &record)
{
    Play result;
    result.playId = record.value("play_id").toInt();
    result.areaId = record.value("area_id").toInt();
    result.playlistId = record.value("playlist_id").toInt();
    result.iid = record.value("iid").toString();
    result.time = deserializeDate(record.value("time").toString());
    result.latitude = record.value("latitude").toDouble();
    result.longitude = record.value("longitude").toDouble();
    result.version = record.value("version").toString();
    result.sent = record.value("sent").toInt();
    return result;
}

StatisticDatabase::Report StatisticDatabase::Report::fromRecord(const QSqlRecord &record)
{
    Report result;
    result.reportId = record.value("report_id").toInt();
    result.time = deserializeDate(record.value("time").toString());
    result.downloads = record.value("downloads").toInt();
    result.contentPlay = record.value("content_play").toInt();
    result.contentTotal = record.value("content_total").toInt();
    result.errorConnect = record.value("error_connect").toInt();
    result.errorPlaylist = record.value("error_playlist").toInt();
    result.sent = record.value("sent").toInt();
    return result;
}

StatisticDatabase::SystemInfo StatisticDatabase::SystemInfo::fromRecord(const QSqlRecord &record)
{
    SystemInfo result;
    result.systemInfoId = record.value("systemInfo_id").toInt();
    result.time = deserializeDate(record.value("time").toString());
    result.cpu = record.value("cpu").toInt();
    result.memory = record.value("memory").toInt();
    result.trafficIn = record.value("traffic").toDouble();
    result.trafficOut = record.value("out").toDouble();
    result.monitor = record.value("monitor").toInt();
    result.connect = record.value("connect").toInt();
    result.balance = record.value("balance").toDouble();
    result.sent = record.value("sent").toInt();
    return result;
}

StatisticDatabase::GPS StatisticDatabase::GPS::fromRecord(const QSqlRecord &record)
{
    GPS result;
    result.gpsId = record.value("gps_id").toInt();
    result.time = deserializeDate(record.value("time").toString());
    result.latitude = record.value("latitude").toDouble();
    result.longitude = record.value("longitude").toDouble();
    return result;
}

StatisticDatabase::Resource StatisticDatabase::Resource::fromRecord(const QSqlRecord &record)
{
    Resource result;
    result.iid = record.value("iid").toString();
    result.name = record.value("name").toString();
    result.lastupdated = deserializeDate(record.value("lastupdated").toString());
    result.filesize = record.value("filesize").toInt();
    result.size = record.value("size").toInt();
    return result;
}

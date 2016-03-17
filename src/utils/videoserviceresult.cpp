#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDateTime>
#include <QCryptographicHash>
#include <QProcess>
#include <QByteArray>
#include <QFile>
#include <QRegExp>
#include <QLocale>
#include <QPointF>

#include "videoserviceresult.h"
#include "globalconfig.h"
#include "sslencoder.h"




VideoServiceResultProcessor::VideoServiceResultProcessor(QObject *parent) : QObject(parent)
{

}

void VideoServiceResultProcessor::initRequestResultReply(QNetworkReply *reply)
{
    if (reply->error())
    {
        InitRequestResult result;
        result.error_id = -1;
        result.error_text = reply->errorString();
        emit initResult(result);
    }
    else
    {
        QByteArray data = reply->readAll();
        qDebug() << data;
        QJsonDocument doc = QJsonDocument::fromJson(data);
        InitRequestResult result = InitRequestResult::fromJson(doc.object());
        emit initResult(result);
    }
}

void VideoServiceResultProcessor::getPlaylistResultReply(QNetworkReply *reply)
{
    if (reply->error())
    {
        PlayerConfig config;
        config.error = -1;
        config.error_text = "NETWORK ERROR";
        emit getPlaylistResult(config);
    }
    else
    {
        QJsonDocument doc;
        QJsonParseError error;
        QByteArray replyData = reply->readAll();
        QFile out("pl.txt");
        out.open(QFile::WriteOnly);
        out.write(replyData);
        out.flush();
        out.close();
        doc = QJsonDocument::fromJson(replyData,&error);
        if (error.error == QJsonParseError::NoError)
            emit getPlaylistResult(PlayerConfig::fromErrorJson(doc.object()));
        else
        {
            QByteArray decodedPlaylist = SSLEncoder::decodeAES256(replyData,true);
            QJsonDocument playlistDoc;

            QJsonParseError playlistError;
            playlistDoc = QJsonDocument::fromJson(decodedPlaylist,&playlistError);
            if (playlistError.error == QJsonParseError::NoError)
                emit getPlaylistResult(PlayerConfig::fromJson(playlistDoc.object()));
            else
            {
                qDebug() << "ERROR while parsing playlist: not json!";
                PlayerConfig config;
                config.error = -1;
                config.error_text = "UNPARSABLE Playlist";
                emit getPlaylistResult(config);
            }
        }
    }
}

void VideoServiceResultProcessor::sendStatisticResultReply(QNetworkReply *reply)
{
    NonQueryResult result;
    if (reply->error())
    {
        result.error_id = -1;
        result.error_text = "NETWORK ERROR";
        result.status = "NETWORK ERROR";
    }
    else
    {
        QByteArray replyData = reply->readAll();
        qDebug() << "STATS REPLY" << replyData;
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result.error_id = root["error_id"].toInt();
        result.error_text = root["error_text"].toString();
        result.status = root["status"].toString();
    }
    emit sendStatisticResult(result);
}

void VideoServiceResultProcessor::getPlayerSettingsReply(QNetworkReply *reply)
{
    SettingsRequestResult result;
    if (reply->error())
    {
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            result.error_id = httpStatus.toInt();
        else
            result.error_id = -1;
    }
    else
    {
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result = SettingsRequestResult::fromJson(root);
    }
    emit getPlayerSettingsResult(result);
}


InitRequestResult InitRequestResult::fromJson(QJsonObject data)
{
    InitRequestResult result;
    result.status = data["status"].toString();
    result.token = "Bearer " + data["token"].toString();
    result.code = data["code"].toString();
    return result;
}

PlayerConfig PlayerConfig::fromJson(QJsonObject json)
{
    PlayerConfig result;
    QJsonDocument dataDoc(json);
    result.data = dataDoc.toJson();
    QJsonArray areas = json["areas"].toArray();
    foreach (const QJsonValue& item, areas)
    {
        QJsonObject object = item.toObject();
        PlayerConfig::Area currentArea;
        currentArea.id = object["id"].toInt();
        currentArea.type = object["type"].toString();


       // PlayerConfig::Area::Playlist areaPlaylist;
        QJsonObject playlistObject = object["playlist"].toObject();

        currentArea.playlist.id = playlistObject["id"].toInt();
        currentArea.playlist.content = playlistObject["content"].toString();
        currentArea.playlist.type = playlistObject["type"].toString();

        QJsonArray playlistItems = playlistObject["items"].toArray();
        foreach (const QJsonValue& playlistItem, playlistItems)
        {
            QJsonObject itemObject = playlistItem.toObject();
            PlayerConfig::Area::Playlist::Item item;
            item.init();
            item.delay = itemObject["delay"].toInt();
            item.dtype = itemObject["dtype"].toString();
            item.duration = itemObject["duration"].toInt();
            item.height = itemObject["height"].toInt();
            item.iid = itemObject["iid"].toString();
            item.itype = itemObject["itype"].toString();

            //"Fri, 04 Dec 2015 15:56:02 +0000"
            QLocale locale(QLocale::English);
            item.lastupdate = locale.toDateTime(itemObject["lastupdate"].toString().replace(" +0000",""),"ddd, dd MMM yyyy HH:mm:ss");

            //\u041b\u0430\u0437\u0435\u0440
            item.name = fromUtfEscapes(itemObject["name"].toString());

            //http:\/\/u1501.storage3.spcdn.ru\/915\/6uwmp\/original.mp4
            item.path = itemObject["path"].toString().replace("\\","");
            item.position = itemObject["position"].toInt();

           // item.pend = itemObject["pend"].toBool();
           // item.pstart = itemObject["pstart"].toBool();
            if (itemObject["pstart"].isBool())
                item.pdate.pstart = false;
            else
            {
                item.pdate.pstart = true;
                item.pdate.pstartDate = locale.toDateTime(itemObject["pstart"].toString().replace(" +0000",""),"ddd, dd MMM yyyy HH:mm:ss");
            }
            if (itemObject["pend"].isBool())
                item.pdate.pend = false;
            else
            {
                item.pdate.pend = true;
                item.pdate.pendDate = locale.toDateTime(itemObject["pend"].toString().replace(" +0000",""),"ddd, dd MMM yyyy HH:mm:ss");
            }

            item.sha1 = itemObject["sha1"].toString();
            item.size = itemObject["size"].toInt();
            item.version = itemObject["version"].toInt();
            item.width = itemObject["width"].toInt();

            QJsonArray ttArray = itemObject["ttargeting"].toArray();
            foreach (const QJsonValue& ttArrayValue, ttArray)
            {
                QJsonArray ttArrayItemArray = ttArrayValue.toArray();
                QVector<QString> data;
                foreach (const QJsonValue& ttItem, ttArrayItemArray)
                    data.append(ttItem.toString());
                item.ttargeting.append(data);
            }
            QJsonArray gtArray = itemObject["gtargeting"].toArray();
            foreach (const QJsonValue& gtArrayValue, gtArray)
            {
                QJsonArray gtArrayItemArray = gtArrayValue.toArray();
                QVector<double> data;
                foreach (const QJsonValue& gtItem, gtArrayItemArray)
                    data.append(gtItem.toDouble());
                item.gtargeting.append(data);
            }
            if (item.gtargeting.size() > 2)
            {
                abort();
                item.buildGeo();
            }
            currentArea.playlist.items.append(item);
        }
        result.areas.append(currentArea);
    }
    return result;
}

PlayerConfig PlayerConfig::fromErrorJson(QJsonObject json)
{
    PlayerConfig result;
    result.error = json["error_id"].toInt();
    result.error_text = json["error_text"].toString();
    result.status = json["status"].toString();
    return result;
}

QString PlayerConfig::fromUtfEscapes(QString str)
{
    QRegExp rx("(\\\\u[0-9a-fA-F]{4})");
    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        str.replace(pos++, 6, QChar(rx.cap(1).right(4).toUShort(0, 16)));
    }
    return str;
}

bool PlayerConfig::Area::Playlist::Item::checkTimeTargeting() const
{
    //check for 7 days in week
    if (ttargeting.count() != 7)
    {
        qDebug() << "RANDOM PL: TTargeting is inactive. Allowing item>> " << iid << " [" << name << "]";
        return true;
    }
    QDateTime currentTime = QDateTime::currentDateTime();
    int hour = currentTime.time().hour();
    int dayOfWeek = currentTime.date().dayOfWeek();
    //check for 24 hour items
    if (ttargeting[dayOfWeek].count() == 24)
    {
        bool timeTargetResult = ttargeting[dayOfWeek][hour] == "1";
        if (timeTargetResult)
            qDebug() << "RANDOM PL: Time targeting Passed, Allowing item to play>> " << iid << " [" << name << "]";
        else
            qDebug() << "RANDOM PL: Time targeting was not passed. Disallowing item to play>> " << iid << " [" << name << "]";
        return timeTargetResult;
    }
    else
    {
        qDebug() << "RANDOM PL: some strange values in time targeting hours. Allowing item to play>> " << iid << " [" << name << "]";
        return true;
    }
}

bool PlayerConfig::Area::Playlist::Item::checkDateRange() const
{
    bool pstart = false, pend = false;
    QDateTime currentTime = QDateTime::currentDateTime();
    if (pdate.pstart)
    {
        if (pdate.pstartDate < currentTime)
            pstart = true;
    }
    else
        pstart = true;

    if (pdate.pend)
    {
        if (pdate.pendDate > currentTime)
            pend = true;
    }
    else
        pend = true;
    if (pstart && pend)
        qDebug() << "RANDOM PL: Time range Passed. Item is Allowed";
    else
        qDebug() << "RANDOM PL: Time range wasnt Passed. Skipping item";

    return pstart && pend;
}

bool PlayerConfig::Area::Playlist::Item::checkGeoTargeting(QPointF gps) const
{
    if (!isPolygonSet || gtargeting.count() < 3)
    {
        qDebug() << "RANDOM PL: GeoTargeting is inactive. Item is Allowed>> " << iid << " [" << name << "]";
        return true;
    }
    bool result = geoPolygon.containsPoint(gps,Qt::OddEvenFill);
    if (result)
    {
        qDebug() << "RANDOM PL: Geo Targeting is ok. Item is Allowed>> " << iid << " [" << name << "]";
        return true;
    }
    else
    {
        qDebug() << "RANDOM PL: Geo Targeting is not passed. Skipping item>> " << iid << " [" << name << "]";
        return false;
    }
}

NonQueryResult NonQueryResult::fromJson(QJsonObject data)
{
    NonQueryResult result;
    if (data.contains("error_id"))
    {
        result.error_id = data["error_id"].toString().toInt();
        result.error_text = data["error_text"].toString();
    }
    result.status = data["status"].toString();

    result.source = QJsonDocument(data).toJson();

    return result;
}

void PlayerConfig::Area::Playlist::Item::init()
{
    position = 0;
    version = 0;
    delay = 0;
    size = 0;
    width = 0;
    height = 0;
    duration = 15;
    isPolygonSet = false;
}

void PlayerConfig::Area::Playlist::Item::buildGeo()
{
    if (gtargeting.size() < 3)
        return;

    qDebug() << "PLAYLIST: Building Geo!";

    QList<QPointF> geoPoints;
    foreach (const QVector<double> &p, gtargeting)
        geoPoints.append(QPointF(p[0],p[1]));
    geoPolygon =  QPolygonF::fromList(geoPoints);
    isPolygonSet = true;
}

SettingsRequestResult SettingsRequestResult::fromJson(QJsonObject data)
{
    SettingsRequestResult result;
    result.created_at = QDateTime::fromString(data["created_at"].toString(), "yyyy-MM-dd HH:mm:ss");
    result.updated_at = QDateTime::fromString(data["updated_at"].toString(), "yyyy-MM-dd HH:mm:ss");
    result.error_id = 0;
    return result;
}

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




VideoServiceResponseHandler::VideoServiceResponseHandler(QObject *parent) : QObject(parent)
{

}

void VideoServiceResponseHandler::initRequestResultReply(QNetworkReply *reply)
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
        QJsonDocument doc = QJsonDocument::fromJson(data);
        InitRequestResult result = InitRequestResult::fromJson(doc.object());
        emit initResult(result);
    }
}

void VideoServiceResponseHandler::getPlaylistResultReply(QNetworkReply *reply)
{

}

void VideoServiceResponseHandler::sendStatisticResultReply(QNetworkReply *reply)
{
    NonQueryResult result;
    if (reply->error())
    {
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            result.error_id = httpStatus.toInt();
        else
            result.error_id = -1;
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result.status = root["error"].toString();
    }
    else
    {
        result.error_id = 0;
        result.status = "success";
    }
    emit sendStatisticResult(result);
}

void VideoServiceResponseHandler::sendStatisticEventsResultReply(QNetworkReply *reply)
{
    NonQueryResult result;
    if (reply->error())
    {
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            result.error_id = httpStatus.toInt();
        else
            result.error_id = -1;
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result.status = root["error"].toString();
    }
    else
    {
        result.error_id = 0;
        result.status = "success";
    }
    emit sendStatisticEventsResult(result);
}

void VideoServiceResponseHandler::sendStatisticPlaysResultReply(QNetworkReply *reply)
{
    NonQueryResult result;
    if (reply->error())
    {
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            result.error_id = httpStatus.toInt();
        else
            result.error_id = -1;
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result.status = root["error"].toString();
    }
    else
    {
        result.error_id = 0;
        result.status = "success";
    }
    emit sendStatisticPlaysResult(result);
}

void VideoServiceResponseHandler::getPlayerSettingsReply(QNetworkReply *reply)
{
    SettingsRequestResult result;
    if (reply->error())
    {
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            result.error_id = httpStatus.toInt();
        else
            result.error_id = -1;

        qDebug() << "VideoServiceResponseHandler::Settings -> network eror." << result.error_id;
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result.error = root["error"].toString();
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

void VideoServiceResponseHandler::getPlayerAreasReply(QNetworkReply *reply)
{
    PlayerConfig result;
    if (reply->error())
    {
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            result.error_id = httpStatus.toInt();
        else
            result.error_id = -1;
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result.error = root["error"].toString();
    }
    else
    {
        QByteArray  replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonArray root = doc.array();
        result = PlayerConfig::fromJson(root);
        GlobalConfigInstance.setAreas(root);
    }
    emit getPlayerAreasResult(result);
}

void VideoServiceResponseHandler::getVirtualScreenPlaylist(QNetworkReply *reply)
{
    QHash<QString, PlaylistAPIResult> result;
    if (reply->error())
    {
        int error_id = 0;
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            error_id = httpStatus.toInt();
        else
            error_id = -1;
        GlobalConfigInstance.setPlaylistNetworkError(error_id);
    }
    else
    {
        GlobalConfigInstance.setPlaylistNetworkError(0);
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonArray root = doc.array();
        result = PlaylistAPIResult::getAllItems(root);
        GlobalConfigInstance.setVirtualScreens(root);
    }
    emit getVirtualScreenPlaylistResult(result);
}


InitRequestResult InitRequestResult::fromJson(QJsonObject data)
{
    InitRequestResult result;
    result.status = data["status"].toString();
    result.token = "Bearer " + data["token"].toString();
    result.code = data["code"].toString();
    return result;
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

SettingsRequestResult SettingsRequestResult::fromJson(QJsonObject data)
{
    SettingsRequestResult result;
    result.created_at = QDateTime::fromString(data["created_at"].toString(), "yyyy-MM-dd HH:mm:ss");
    result.updated_at = QDateTime::fromString(data["updated_at"].toString(), "yyyy-MM-dd HH:mm:ss");
    result.error_id = 0;
    result.error = data["error"].toString();
    result.video_quality = data["video_quality"].toString();
    result.gps_lat = data["gps_lat"].toDouble();
    result.gps_long = data["gps_long"].toDouble();
    result.autobright = data["autobright"].toBool();
    result.min_bright = data["min_bright"].toInt();
    result.max_bright = data["max_bright"].toInt();
    result.name = data["name"].toString();

    result.reley_1_enabled = !data["time_targeting_relay_1"].isNull();
    result.reley_2_enabled = !data["time_targeting_relay_2"].isNull();

    if (result.reley_1_enabled)
        result.time_targeting_relay_1 = generateHashByString(data["time_targeting_relay_1"].toObject()["content"].toString());
    if (result.reley_2_enabled)
        result.time_targeting_relay_2 = generateHashByString(data["time_targeting_relay_2"].toObject()["content"].toString());

    result.brand_active = data["brand_active"].toBool();
    result.brand_background = data["brand_background"].toString();
    result.brand_logo = data["brand_logo"].toString();
    result.brand_background_hash = data["brand_background_hash"].toString();
    result.brand_logo_hash = data["brand_logo_hash"].toString();
    result.brand_color_1 = data["brand_color_1"].toString();
    result.brand_color_2 = data["brand_color_2"].toString();
    result.brand_teleds_copyright = data["brand_teleds_copyright"].toInt();
    result.stats_interval = data["stats_interval"].toInt();

    result.autooff_active = data["autooff_active"].toBool();
    result.off_power_loss = data["off_power_loss"].toInt();
    result.off_charge_percent = data["off_charge_percent"].toInt();

    GlobalConfigInstance.setSettings(data);

    return result;
}

QHash<int, QList<int> > SettingsRequestResult::generateHashByString(QString content)
{
    QHash <int, QList<int> > result;
    content = "{\"content\":" + content + "}";
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    QJsonObject root = doc.object();
    QJsonArray weekDayArray = root["content"].toArray();
    int currentDay = 0;
    foreach (const QJsonValue &weekDayValue, weekDayArray)
    {
        QList <int> hours;
        QJsonArray hoursArray = weekDayValue.toArray();
        foreach (const QJsonValue &v, hoursArray)
            hours.append(v.toInt());
        result[currentDay] = hours;
        currentDay++;
    }
    return result;
}

PlaylistAPIResult PlaylistAPIResult::fromJson(QJsonObject json)
{
    PlaylistAPIResult result;
    result.type = json["type"].toString();
    QJsonArray campaigns = json["campaigns"].toArray();
    foreach (const QJsonValue &jsonCampaign, campaigns)
    {
        QJsonObject campaignObject = jsonCampaign.toObject();
        PlaylistAPIResult::CampaignItem campaign;
        campaign.play_order = campaignObject["play_order"].toInt();
        campaign.play_timeout = campaignObject["play_timeout"].toInt();
        campaign.play_type = campaignObject["play_type"].toString();
        QJsonArray campaignContent = campaignObject["content"].toArray();
        foreach (const QJsonValue &jsonContent, campaignContent)
        {
            QJsonObject contentObject = jsonContent.toObject();
            PlaylistAPIResult::PlaylistItem item;
            item.id = contentObject["content_id"].toString();
            item.campaignId = contentObject["campaign_id"].toString();
            campaign.id = item.campaignId;
            item.name = contentObject["name"].toString();
            item.fileUrl = contentObject["fileUrl"].toString();
            item.fileHash = contentObject["fileHash"].toString();
            item.duration = contentObject["duration"].toInt();
            item.type = contentObject["type"].toString();

            QDateTime nullDateTime;
            if (contentObject["play_starts"].isNull())
                item.play_starts = nullDateTime;
            else
                item.play_starts = QDateTime::fromString(contentObject["play_starts"].toString(), "yyyy-MM-dd HH:mm:ss");

            if (contentObject["play_ends"].isNull())
                item.play_ends = nullDateTime;
            else
                item.play_ends = QDateTime::fromString(contentObject["play_ends"].toString(), "yyyy-MM-dd HH:mm:ss");

            QJsonObject timeTargeting = contentObject["time_targeting"].toObject();
            foreach (const QString &key, timeTargeting.keys())
            {
                QJsonArray timeTargetingItems = timeTargeting[key].toArray();
                QVector<int> timeTargetingVectorItems;
                foreach (const QJsonValue &v, timeTargetingItems)
                    timeTargetingVectorItems.append(v.toInt());
                item.time_targeting[key] = timeTargetingVectorItems;
            }

            QJsonArray geoTargeting = contentObject["geo_targeting"].toArray();
            QVector<QVector<PlaylistAPIResult::PlaylistItem::gps> > geoTargetingVector;
            foreach (const QJsonValue &v, geoTargeting)
            {
                QVector <PlaylistAPIResult::PlaylistItem::gps> geoTargetingAreaVector;
                QJsonArray geoTargetingArea = v.toArray();
                QPolygonF currentPolygon;
                foreach (const QJsonValue &areaValue, geoTargetingArea)
                {
                    QJsonObject gpsObject = areaValue.toObject();
                    PlaylistAPIResult::PlaylistItem::gps gps;
                    gps.latitude = gpsObject["latitude"].toDouble();
                    gps.longitude = gpsObject["longitude"].toDouble();
                    geoTargetingAreaVector.append(gps);
                    currentPolygon.append(QPointF(gps.latitude, gps.longitude));
                }
                geoTargetingVector.append(geoTargetingAreaVector);
                item.polygons.append(currentPolygon);
            }
            item.geo_targeting = geoTargetingVector;
            item.updated_at = QDateTime::fromString(contentObject["updated_at"].toString(), "yyyy-MM-dd HH:mm:ss");
            campaign.content.append(item);
        }
        result.items.append(campaign);
    }
    GlobalConfigInstance.setPlaylist(json);
    return result;
}

QHash<QString, PlaylistAPIResult> PlaylistAPIResult::getAllItems(QJsonArray json)
{
    QHash <QString, PlaylistAPIResult> result;
    foreach (const QJsonValue &v, json)
    {
        QJsonObject areaDescObject = v.toObject();
        QJsonObject playlistObject = areaDescObject["playlist"].toObject();
        PlaylistAPIResult item = PlaylistAPIResult::fromJson(playlistObject);
        item.id = areaDescObject["area_id"].toString();
        item.type = playlistObject["type"].toString();
        for (int i = 0; i< item.items.count(); i++)
            item.items[i].areaId = item.id;
        result[item.id] = item;
    }
    return result;
}

PlayerConfig PlayerConfig::fromJson(QJsonArray data)
{
    PlayerConfig result;
    result.error_id = 0;

    foreach (const QJsonValue &v, data)
    {
        QJsonObject virtualScreenObject = v.toObject();
        PlayerConfig::VirtualScreen screen;
        screen.virtual_screen_id = virtualScreenObject["virtual_screen_id"].toString();
        GlobalConfigInstance.setVirtualScreenId(screen.virtual_screen_id);
        screen.id = virtualScreenObject["id"].toString();
        screen.display_type = virtualScreenObject["display_type"].toString();
        screen.type = virtualScreenObject["type"].toString();
        screen.audio_priority = virtualScreenObject["audio_priority"].toInt();
        screen.screen_priority = virtualScreenObject["screen_priority"].toInt();
        screen.position.setTopLeft(QPoint(virtualScreenObject["start_x"].toInt(),
                                          virtualScreenObject["start_y"].toInt()));
        screen.position.setBottomRight(QPoint(virtualScreenObject["end_x"].toInt(),
                                              virtualScreenObject["end_y"].toInt()));
        screen.resfreshTime = virtualScreenObject["refresh_at"].toInt();
        result.screens[screen.id] = screen;
    }
    return result;
}

QString PlaylistAPIResult::PlaylistItem::getExtension() const
{
    QStringList tokens = fileUrl.split(".");
    if (tokens.count() < 2)
        return "";
    return "." + tokens.last();
}

bool PlaylistAPIResult::PlaylistItem::checkTimeTargeting() const
{
    QString dayInt = QString::number(QDateTime::currentDateTimeUtc().date().dayOfWeek() + 1);
    int hour = QDateTime::currentDateTimeUtc().time().hour();
    if (time_targeting.contains(dayInt))
        return time_targeting[dayInt].contains(hour);
    return time_targeting.count() == 0;
}

bool PlaylistAPIResult::PlaylistItem::checkDateRange() const
{
    bool sinceCheck = true, untilCheck = true;
    if (play_starts.isValid())
        sinceCheck = QDateTime::currentDateTimeUtc() > play_starts;
    if (play_ends.isValid())
        untilCheck = QDateTime::currentDateTimeUtc() < play_ends;
    return sinceCheck && untilCheck;
}

bool PlaylistAPIResult::PlaylistItem::checkGeoTargeting(QPointF gps) const
{
    if (polygons.count())
    {
        foreach (const QPolygonF &p, polygons)
            if (p.containsPoint(gps,Qt::OddEvenFill))
                return true;
        return false;
    }
    else return true;
}

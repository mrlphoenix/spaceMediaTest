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
    reply->deleteLater();
}

void VideoServiceResponseHandler::getPlaylistResultReply(QNetworkReply *reply)
{
    PlayerConfigAPI result;
    if (reply->error())
    {
        QVariant httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (httpStatus.isValid())
            result.error_id = httpStatus.toInt();
        else
            result.error_id = -1;

        qDebug() << "VideoServiceResponseHandler::getPlaylistResultReply -> network eror." << result.error_id;
        if (result.error_id != -1)
        {
            QByteArray replyData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(replyData);
            QJsonObject root = doc.object();
            result.error = root["error"].toString();
        }
    }
    else
    {
        QByteArray replyData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(replyData);
        QJsonObject root = doc.object();
        result = PlayerConfigAPI::fromJson(root);
    }
    emit getPlaylistResult(result);
    reply->deleteLater();
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
    reply->deleteLater();
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
    reply->deleteLater();
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

SettingsRequestResult SettingsRequestResult::fromJson(QJsonObject data, bool needSave)
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
    result.bright_night = data["bright_night"].toInt();
    result.bright_day = data["bright_day"].toInt();
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

    result.brand_menu_background = data["brand_background_second_screen"].toString();
    result.brand_menu_background_hash = data["brand_background_second_screen_hash"].toString();
    result.brand_menu_logo = data["brand_logo_second_screen"].toString();
    result.brand_menu_logo_hash = data["brand_logo_second_screen_hash"].toString();
    result.brand_menu_color_1 = data["brand_color_1_second_screen"].toString();
    result.brand_menu_color_2 = data["brand_color_2_second_screen"].toString();
    result.brand_repeat = data["repeat"].toInt();//toBool();
    result.brand_menu_repeat = data["repeat_second_screen"].toInt();//toBool();

    result.brand_teleds_copyright = data["brand_teleds_copyright"].toBool();
    result.brand_menu_teleds_copyright = data["brand_teleds_copyright_second_screen"].toBool();

    result.stats_interval = data["stats_interval"].toInt();

    result.autooff_by_battery_level_active = data["autooff_by_battery_level_active"].toBool();
    result.autooff_by_discharging_time_active = data["autooff_by_discharging_time_active"].toBool();
    result.off_power_loss = data["off_power_loss"].toInt();
    result.off_charge_percent = data["off_charge_percent"].toInt();

    result.is_paid = data["is_paid"].toBool();
    result.volume = data["volume"].toInt();

    if (needSave)
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

        for (int cmpIndex = 0; cmpIndex< item.items.count(); ++cmpIndex)
            for (int itemIndex = 0; itemIndex <  item.items[cmpIndex].content.count(); ++itemIndex)
                item.items[cmpIndex].content[itemIndex].virtualScreenId = GlobalConfigInstance.getVirtualScreenId(item.id);

        for (int i = 0; i< item.items.count(); ++i)
        {
            item.items[i].areaId = item.id;
            for (int j = 0; j < item.items[i].content.count(); ++j)
                item.items[i].content[j].areaId = item.id;
        }
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
        GlobalConfigInstance.setAreaToVirtualScreen(screen.id, screen.virtual_screen_id);
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

PlayerConfig::AreaCompositionType PlayerConfig::getType()
{
    bool contentFound = false, widgetFound = false, multipleContent = false;
    foreach (const VirtualScreen &v, screens)
    {
        if (v.type == "fullscreen" && v.playlist.items.count())
            return AREA_FULLSCREEN;
        else if (v.type == "content" && v.playlist.items.count())
        {
            if (contentFound)
                multipleContent = true;
            contentFound = true;
        }
        else if (v.type == "widget" && v.playlist.items.count())
            widgetFound = true;
    }
    if (widgetFound && contentFound)
        return AREA_SPLIT;
    if (multipleContent)
        return AREA_MULTI;
    else
        return AREA_BROKEN;
}

PlayerConfig::VirtualScreen PlayerConfig::getScreenByType(QString type)
{
    VirtualScreen result;
    foreach (const QString &k, screens.keys())
    {
        VirtualScreen currentScreen = screens[k];
        if (currentScreen.type == type && currentScreen.playlist.items.count())
            return currentScreen;
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
    {
        bool result = time_targeting[dayInt].contains(hour);
        qDebug() << "is Time Targeting Passed for " + this->id + "? :" << result;
        return result;
    }
    bool result = (time_targeting.count() == 0);
    qDebug() << "is Time Targeting Passed for " + this->id + "? :" << result;
    return result;
}

bool PlaylistAPIResult::PlaylistItem::checkDateRange() const
{
    bool sinceCheck = true, untilCheck = true;
    if (play_starts.isValid())
        sinceCheck = QDateTime::currentDateTimeUtc() > play_starts;
    if (play_ends.isValid())
        untilCheck = QDateTime::currentDateTimeUtc() < play_ends;
    qDebug() << "checkDateRange for " + id + "? :" << sinceCheck << untilCheck;
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

PlayerConfigAPI PlayerConfigAPI::fromJson(QJsonObject json)
{
    PlayerConfigAPI result;
    result.last_modified = timeFromJson(json["last_modified"]);
    QJsonArray campaigns = json["campaigns"].toArray();
    foreach (const QJsonValue &cValue, campaigns)
        result.campaigns.append(PlayerConfigAPI::Campaign::fromJson(cValue.toObject()));
    return result;
}

QDateTime PlayerConfigAPI::timeFromJson(QJsonValue v)
{
    QDateTime invalidDate;
    if (v.isNull())
        return invalidDate;
    else
        return QDateTime::fromString(v.toString(), "yyyy-MM-dd HH:mm:ss");
}

PlayerConfigAPI::Campaign PlayerConfigAPI::Campaign::fromJson(QJsonObject json)
{
    PlayerConfigAPI::Campaign result;
    result.campaign_id = json["campaign_id"].toString();
    result.duration = json["duration"].toInt();
    result.start_timestamp = timeFromJson(json["start_timestamp"]);
    result.end_timestamp = timeFromJson(json["end_timestamp"]);
    result.play_order = json["play_order"].toInt();
    QJsonArray areas = json["areas"].toArray();
    foreach (const QJsonValue &aValue, areas)
        result.areas.append(PlayerConfigAPI::Campaign::Area::fromJson(aValue.toObject()));
    return result;
}
/*
 *     foreach (const QJsonValue &v, geoTargeting)
{
    QVector <PlayerConfigAPI::Campaign::Area::Content::gps> geoTargetingAreaVector;
    QJsonArray geoTargetingArea = v.toArray();
    QPolygonF currentPolygon;
    foreach (const QJsonValue &areaValue, geoTargetingArea)
    {
        QJsonObject gpsObject = areaValue.toObject();
        PlayerConfigAPI::Campaign::Area::Content::gps gps;
        gps.latitude = gpsObject["latitude"].toDouble();
        gps.longitude = gpsObject["longitude"].toDouble();
        geoTargetingAreaVector.append(gps);
        currentPolygon.append(QPointF(gps.latitude, gps.longitude));
    }
    geoTargetingVector.append(geoTargetingAreaVector);
    result.polygons.append(currentPolygon);
}
item.geo_targeting = geoTargetingVector;
 * */

PlayerConfigAPI::Campaign::Area PlayerConfigAPI::Campaign::Area::fromJson(QJsonObject json)
{
    PlayerConfigAPI::Campaign::Area result;
    result.area_id = json["area_id"].toString();
    result.type = json["type"].toString();
    result.screen_height = json["campaign_height"].toInt();
    result.screen_width = json["campaign_width"].toInt();
    result.x = json["left"].toInt();
    result.y = json["top"].toInt();
    result.width = json["width"].toInt();
    result.height = json["height"].toInt();
    result.opacity = double(json["opacity"].toInt())/100.;
    result.z_index = json["z_index"].toInt();
    result.sound_enabled = json["sound_enabled"].toBool();
    QJsonArray content = json["content"].toArray();
    foreach (const QJsonValue &cValue, content)
        result.content.append(PlayerConfigAPI::Campaign::Area::Content::fromJson(cValue.toObject()));
}

PlayerConfigAPI::Campaign::Area::Content PlayerConfigAPI::Campaign::Area::Content::fromJson(QJsonObject json)
{
    PlayerConfigAPI::Campaign::Area::Content result;
    result.campaign_id = json["campaign_id"].toString();
    result.area_id = json["area_id"].toString();
    result.content_id = json["content_id"].toString();
    result.payment_type = json["payment_type"].toString();
    result.play_order = json["play_order"].toInt();
    result.play_type = json["play_type"].toString();
    result.play_timeout = json["play_timeout"].toInt();
    result.start_timestamp = timeFromJson(json["start_timestamp"]);
    result.end_timestamp = timeFromJson(json["end_timestamp"]);
    result.name = json["name"].toString();
    result.type = json["type"].toString();
    result.rotate = json["rotate"].toInt();
    result.duration = json["duration"].toInt();
    result.play_start = json["play_start"].toInt();
    result.file_url = json["file_url"].toString();
    result.file_hash = json["file_hash"].toString();

    QJsonObject timeTargeting = json["time_targeting"].toObject();
    foreach (const QString &key, timeTargeting.keys())
    {
        QJsonArray timeTargetingItems = timeTargeting[key].toArray();
        QVector<int> timeTargetingVectorItems;
        foreach (const QJsonValue &v, timeTargetingItems)
            timeTargetingVectorItems.append(v.toInt());
        result.time_targeting[key] = timeTargetingVectorItems;
    }

    QJsonArray geoTargeting = json["geo_targeting"].toArray();
    QVector<QVector<PlayerConfigAPI::Campaign::Area::Content::gps> > geoTargetingVector;
    foreach (const QJsonValue &v, geoTargeting)
    {
        QVector <PlayerConfigAPI::Campaign::Area::Content::gps> geoTargetingAreaVector;
        QJsonArray geoTargetingArea = v.toArray();
        QPolygonF currentPolygon;
        foreach (const QJsonValue &areaValue, geoTargetingArea)
        {
            QJsonObject gpsObject = areaValue.toObject();
            PlayerConfigAPI::Campaign::Area::Content::gps gps;
            gps.latitude = gpsObject["latitude"].toDouble();
            gps.longitude = gpsObject["longitude"].toDouble();
            geoTargetingAreaVector.append(gps);
            currentPolygon.append(QPointF(gps.latitude, gps.longitude));
        }
        geoTargetingVector.append(geoTargetingAreaVector);
        result.polygons.append(currentPolygon);
    }
    result.geo_targeting = geoTargetingVector;
}

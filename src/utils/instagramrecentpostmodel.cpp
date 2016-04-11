#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "instagramrecentpostmodel.h"

InstagramRecentPostModel::InstagramRecentPostModel()
{

}

InstagramRecentPostModel InstagramRecentPostModel::fromJson(QByteArray json)
{
    InstagramRecentPostModel result;
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject root = doc.object();

    QJsonObject pagination = root["pagination"].toObject();
    QJsonObject meta = root["meta"].toObject();

    result.min_tag_id = pagination["min_tag_id"].toString();
    result.code = meta["code"].toInt();

    QJsonArray data = root["data"].toArray();
    foreach (const QJsonValue &value, data)
    {
        InstagramRecentPostModel::Data dataItem;
        QJsonObject valueObject = value.toObject();
        QJsonArray tags = valueObject["tags"].toArray();
        foreach (const QJsonValue &tag, tags)
            dataItem.tags.append(tag.toString());
        dataItem.postLink = valueObject["link"].toString().replace("\\","");

        InstagramRecentPostModel::Data::Image thumb,lowres,standartRes;
        thumb.url = valueObject["images"].toObject()["thumbnail"].toObject()["url"].toString().replace("\\","");
        thumb.width = valueObject["images"].toObject()["thumbnail"].toObject()["width"].toInt();
        thumb.height = valueObject["images"].toObject()["thumbnail"].toObject()["height"].toInt();

        lowres.url = valueObject["images"].toObject()["low_resolution"].toObject()["url"].toString().replace("\\","");
        lowres.width = valueObject["images"].toObject()["low_resolution"].toObject()["width"].toInt();
        lowres.height = valueObject["images"].toObject()["low_resolution"].toObject()["height"].toInt();

        standartRes.url = valueObject["images"].toObject()["standard_resolution"].toObject()["url"].toString().replace("\\","");
        standartRes.width = valueObject["images"].toObject()["standard_resolution"].toObject()["width"].toInt();
        standartRes.height = valueObject["images"].toObject()["standard_resolution"].toObject()["height"].toInt();
        dataItem.thumbnail = thumb;
        dataItem.low_resolution = lowres;
        dataItem.standard_resolution = standartRes;

        dataItem.caption = valueObject["caption"].toObject()["text"].toString();
        dataItem.authorProfilePictureUrl = valueObject["user"].toObject()["profile_picture"].toString().replace("\\","");
        result.data.append(dataItem);
    }

    return result;
}



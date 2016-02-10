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

#include "videoserviceresult.h"
#include "globalconfig.h"


QString InitRequestResult::getRandomString(int length)
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString randomString;
    qsrand(QDateTime::currentDateTime().toTime_t());
    for(int i=0; i < length; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

QString InitRequestResult::generateSessionKey()
{
    return QCryptographicHash::hash(getRandomString(16).toLocal8Bit(),QCryptographicHash::Md5).toHex();
}
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
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        InitRequestResult result = InitRequestResult::fromJson(doc.object());
        qDebug() << "videoSErviceResult::" + result.public_key;
        publicKey = result.public_key;
        result.session_key = InitRequestResult::generateSessionKey();
        sessionKey = result.session_key;
        emit initResult(result);
    }
}

void VideoServiceResultProcessor::enablePlayerResultReply(QNetworkReply *reply)
{
    if (reply->error())
        emit enablePlayerResult("NETWORK ERROR: " + reply->errorString());
    else
        emit enablePlayerResult(reply->readAll());
}

void VideoServiceResultProcessor::assignPlaylistResultReply(QNetworkReply *reply)
{
    if (reply->error())
        emit assignPlaylistResult("NETWORK ERROR: " + reply->errorString());
    else
        emit assignPlaylistResult(reply->readAll());
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
        doc = QJsonDocument::fromJson(replyData,&error);
        if (error.error == QJsonParseError::NoError)
            emit getPlaylistResult(PlayerConfig::fromErrorJson(doc.object()));
        else
        {
            QString hexSessionKey = sessionKey;
            if (hexSessionKey == "")
                hexSessionKey = GlobalConfigInstance.getSessionKey();
            hexSessionKey = hexSessionKey.toLocal8Bit().toHex();
            QFile cryptedDataFile("playlist.bin");
            cryptedDataFile.open(QFile::WriteOnly);
            QByteArray binaryData = QByteArray::fromBase64(replyData);
            cryptedDataFile.write(binaryData);
            cryptedDataFile.close();

            QProcess decodeProcess;
            /*
             * openssl enc -aes-256-cbc -d -K 3530383833326435363964346637303661666161633435313163626565333038 -iv 30303030303030303030303030303030 -in ./DecodedBase64.bin
             * */
            decodeProcess.start("openssl enc -aes-256-cbc -d -K " + hexSessionKey + " -iv 30303030303030303030303030303030 -in playlist.bin");
            QByteArray decodedPlaylist;
            decodeProcess.waitForFinished();
            decodedPlaylist.append(decodeProcess.readAll());
            QFile playlistJson("playlist.json");
            playlistJson.open(QFile::WriteOnly);
            playlistJson.write(decodedPlaylist);
            playlistJson.close();

            QJsonDocument playlistDoc;
            QJsonParseError playlistError;
            playlistDoc = QJsonDocument::fromJson(decodedPlaylist,&playlistError);
            if (playlistError.error == QJsonParseError::NoError)
                emit getPlaylistResult(PlayerConfig::fromJson(playlistDoc.object()));
            else
            {
                qDebug() << "ERROR while parsing playlist";
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

QString VideoServiceResultProcessor::getRandomString(int length)
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString randomString;
    qsrand(QDateTime::currentDateTime().toTime_t());
    for(int i=0; i < length; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

InitRequestResult InitRequestResult::fromJson(QJsonObject data)
{
    InitRequestResult result;
    result.status = data["status"].toString();

    if (data.contains("error_id"))
    {
        result.error_id = data["error_id"].toString().toInt();
        result.error_text = data["error_text"].toString();
    }
    else
    {
        result.player_id = data["player_id"].toString();
        result.public_key = data["public_key"].toString();
    }
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
        return true;
    QDateTime currentTime = QDateTime::currentDateTime();
    int hour = currentTime.time().hour();
    int dayOfWeek = currentTime.date().dayOfWeek();
    //check for 24 hour items
    if (ttargeting[dayOfWeek].count() == 24)
        return ttargeting[dayOfWeek][hour] == "1";
    else
        return true;
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

    return pstart && pend;
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

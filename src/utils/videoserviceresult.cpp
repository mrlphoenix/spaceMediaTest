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
        QJsonDocument doc;
        doc.fromJson(reply->readAll());
        InitRequestResult result = InitRequestResult::fromJson(doc.object());

        publicKey = result.public_key;
        result.session_key = QCryptographicHash::hash(getRandomString(16).toLocal8Bit(),QCryptographicHash::Md5);
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
        doc.fromJson(reply->readAll(),&error);
        if (error.error != QJsonParseError::NoError)
            emit getPlaylistResult(PlayerConfig::fromErrorJson(doc.object()));
        else
        {
            QString hexSessionKey = sessionKey;
            hexSessionKey = hexSessionKey.toLocal8Bit().toHex();
            QFile cryptedDataFile("playlist.bin");
            cryptedDataFile.open(QFile::WriteOnly);
            QByteArray binaryData = QByteArray::fromBase64(reply->readAll());
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

            QJsonDocument playlistDoc;
            QJsonParseError playlistError;
            playlistDoc.fromJson(decodedPlaylist,&playlistError);
            if (playlistError.error == QJsonParseError::NoError)
                emit getPlaylistResult(PlayerConfig::fromJson(playlistDoc.object()));
            else
            {
                qDebug() << "ERROR while parsing playlist";
                PlayerConfig config;
                config.error = -1;
                config.error_text = "UNPARSABLE";
                emit getPlaylistResult(config);
            }
        }
    }
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
            item.iid = itemObject["iid"].toInt();
            item.itype = itemObject["itype"].toString();

            //"Fri, 04 Dec 2015 15:56:02 +0000"
            QLocale locale(QLocale::English);
            item.lastupdate = locale.toDateTime(itemObject["lastupdate"].toString().replace(" +0000",""),"ddd, dd MMM yyyy HH:mm:ss");

            //\u041b\u0430\u0437\u0435\u0440
            item.name = fromUtfEscapes(itemObject["name"].toString());

            //http:\/\/u1501.storage3.spcdn.ru\/915\/6uwmp\/original.mp4
            item.path = itemObject["path"].toString().replace("\\","");

            item.pend = itemObject["pend"].toBool();
            item.position = itemObject["position"].toInt();
            item.pstart = itemObject["pstart"].toBool();
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
    result.error = json["error"].toInt();
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











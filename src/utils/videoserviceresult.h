#ifndef VIDEOSERVICERESULT_H
#define VIDEOSERVICERESULT_H

#include <QObject>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDateTime>
#include <QPolygonF>

struct InitRequestResult
{
    static InitRequestResult fromJson(QJsonObject data);
    QString status;
    int error_id;
    QString error_text;

    //QString player_id;
    //QString public_key;
    //QString session_key;
    QString code;
    QString token;
};
struct NonQueryResult
{
    static NonQueryResult fromJson(QJsonObject data);

    QString status;
    int error_id;
    QString error_text;
    QString source;
};

struct SettingsRequestResult
{
    static SettingsRequestResult fromJson(QJsonObject data);

    int error_id;
    QString video_quality;
    QDateTime created_at;
    QDateTime updated_at;
    QString error;
};

struct PlaylistAPIResult
{
    static PlaylistAPIResult fromJson(QJsonObject json);
    QString id;
    struct PlaylistItem
    {
        QString id;
        QString name;
        QString fileUrl;
        QString fileHash;
        int videoWidth;
        int videoHeight;
        int duration;
        QString type;
        int play_priority;
        QDateTime play_starts;
        QDateTime play_ends;

        QHash<QString, QVector<int> > time_targeting;
        struct gps
        {
            double latitude, longitude;
        };

        QVector<QVector<gps> > geo_targeting;
    };
    QVector<PlaylistItem> items;

    /*
{
  "0cb7366e-69b7-4276-9fe6-4e4de31fd7b8": [
    {
      "id": "b5d6475d-f987-4b7b-be53-f978615d53e5",
      "name": "TeleDS Sample Content",
      "fileUrl": "http://google.com/",
      "fileHash": "2aae6c35c94fcfb415dbe95f408b9ce91ee846ed",
      "videoWidth": "1920",
      "videoHeight": "1080",
      "duration": "15",
      "type": "fullscreen",
      "play_priority": "0",
      "play_timeout": "180",
      "play_type": "normal",
      "play_starts": "2016-01-01 00:00:00",
      "play_ends": "2016-05-01 00:00:00",
      "time_targeting": {
        "1": [
          10
        ]
      },
      "geo_targeting": [
        [
          {
            "latitude": 53.5555,
            "longitude": 40.1111
          }
        ]
      ]
    }
  ]
}*/
};

struct PlayerConfig
{
    static PlayerConfig fromJson(QJsonObject json);
    static PlayerConfig fromErrorJson(QJsonObject json);

    static QString fromUtfEscapes(QString str);
    QString status;
    int error;
    QString error_text;
    QString data;

    struct Area
    {
        int id;
        QString type;

        struct Playlist
        {
            int id;
            QString type;
            QString content;
            struct Item
            {
                void init();
                void buildGeo();

                QString iid;
                QString itype;
                int position;
                int version;
                QDateTime lastupdate;
                int delay;
                QString dtype;
                struct PDate
                {
                    bool pstart, pend;
                    QDateTime pstartDate, pendDate;
                };
                PDate pdate;
                QVector<QVector<QString> > ttargeting;
                QVector<QVector<double> > gtargeting;
                QString name;
                QString path;
                QString sha1;
                int size;
                int width;
                int height;
                int duration;

                bool checkTimeTargeting() const;
                bool checkDateRange() const;
                bool checkGeoTargeting(QPointF gps) const;
            private:
                QPolygonF geoPolygon;
                bool isPolygonSet;
            };
            QVector<Item> items;
        };
        Playlist playlist;
    };
    QVector<Area> areas;
};

class VideoServiceResultProcessor : public QObject
{
    Q_OBJECT
public:
    explicit VideoServiceResultProcessor(QObject *parent = 0);

signals:
    void initResult(InitRequestResult result);
    void getPlaylistResult(PlayerConfig result);
    void sendStatisticResult(NonQueryResult result);
    void getPlayerSettingsResult(SettingsRequestResult result);

public slots:
    void initRequestResultReply(QNetworkReply * reply);
    void getPlaylistResultReply(QNetworkReply * reply);
    void sendStatisticResultReply(QNetworkReply * reply);
    void getPlayerSettingsReply(QNetworkReply * reply);
private:
};

#endif // VIDEOSERVICERESULT_H

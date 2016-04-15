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
    QString name;
    QString video_quality;
    QDateTime created_at;
    QDateTime updated_at;
    QString error;


    double gps_lat;
    double gps_long;
    bool autobright;
    int min_bright;
    int max_bright;
    int stats_interval;
};

struct PlaylistAPIResult
{
    static PlaylistAPIResult fromJson(QJsonArray json);
    static QHash<QString, PlaylistAPIResult> getAllItems(QJsonArray json);
    QString id;
    QString type;
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
        int play_timeout;
        QString play_type;
        QDateTime play_starts;
        QDateTime play_ends;
        QDateTime updated_at;

        QHash<QString, QVector<int> > time_targeting;
        struct gps
        {
            double latitude, longitude;
        };

        QVector<QVector<gps> > geo_targeting;
        QVector<QPolygonF> polygons;

        bool checkTimeTargeting() const;
        bool checkDateRange() const;
        bool checkGeoTargeting(QPointF gps) const;
    };
    QVector<PlaylistItem> items;
};


struct PlayerConfigNew
{
    static PlayerConfigNew fromJson(QJsonArray data);
    int error_id;
    QString error;
    struct VirtualScreen
    {
        QString virtual_screen_id;
        QString id;
        QString type;
        QRect position;
        QString display_type;
        QDateTime play_until;
        int resfreshTime;
        int audio_priority;
        int screen_priority;
        PlaylistAPIResult playlist;
    };
    QHash<QString, VirtualScreen> screens;
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
                static Item fromNewItem(PlaylistAPIResult::PlaylistItem item);

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
    void getPlayerAreasResult(PlayerConfigNew result);
    void getVirtualScreenPlaylistResult(QHash<QString, PlaylistAPIResult> result);

public slots:
    void initRequestResultReply(QNetworkReply * reply);
    void getPlaylistResultReply(QNetworkReply * reply);
    void sendStatisticResultReply(QNetworkReply * reply);
    void getPlayerSettingsReply(QNetworkReply * reply);
    void getPlayerAreasReply(QNetworkReply * reply);
    void getVirtualScreenPlaylist(QNetworkReply * reply);
private:
};

#endif // VIDEOSERVICERESULT_H

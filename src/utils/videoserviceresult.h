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

struct UpdateInfoResult
{
    static UpdateInfoResult fromJson(QJsonObject data);
    int version_major;
    int version_minor;
    int version_release;
    int version_build;
    QString file_url;
    QString file_hash;
    int file_size;

    int error_id;
    QString error_text;
};

struct SettingsRequestResult
{
    static SettingsRequestResult fromJson(QJsonObject data, bool needSave = true);

    int error_id;
    QString name;
    QString video_quality;
    QDateTime created_at;
    QDateTime updated_at;
    QString error;
    QHash<int, QList<int> > time_targeting_relay_1;
    QHash<int, QList<int> > time_targeting_relay_2;
    bool reley_1_enabled, reley_2_enabled;

    static QHash<int, QList<int> > generateHashByString(QString content);

    double gps_lat;
    double gps_long;
    bool autobright;
    int bright_night;
    int bright_day;

    int stats_interval;
    int base_rotation;

    //branding
    bool brand_active;
    QString brand_background;
    QString brand_logo;
    QString brand_background_hash;
    QString brand_logo_hash;
    QString brand_color_1;
    QString brand_color_2;
    QString brand_menu_color_1;
    QString brand_menu_color_2;
    QString brand_menu_background;
    QString brand_menu_background_hash;
    QString brand_menu_logo;
    QString brand_menu_logo_hash;

    bool brand_repeat;
    bool brand_menu_repeat;
    bool brand_teleds_copyright;
    bool brand_menu_teleds_copyright;

    //auto off
    int off_power_loss;
    int off_charge_percent;
    bool autooff_by_battery_level_active;
    bool autooff_by_discharging_time_active;
    bool is_paid;
    int volume;
};
struct PlayerConfigAPI
{
    static PlayerConfigAPI fromJson(QJsonObject json);
    static QDateTime timeFromJson(QJsonValue v);
    int count();
    int currentAreaCount();
    int nextCampaign();     //returns current Campaign total time

    
    QDateTime last_modified;
    int error_id;
    QString error_text;
    QString error;
    int currentCampaignId;
    struct Campaign
    {
        static PlayerConfigAPI::Campaign fromJson(QJsonObject json);
        int itemCount() const;
        int play_order;
        QString campaign_id;
        int duration;
        QDateTime start_timestamp;
        QDateTime end_timestamp;
        int screen_width;
        int screen_height;
        int rotation;

        struct Area {
            static PlayerConfigAPI::Campaign::Area fromJson(QJsonObject json);
            QString area_id;
            QString type;
            int x;
            int y;
            int width;
            int height;
            int screen_width;
            int screen_height;
            int z_index;
            double opacity;
            bool sound_enabled;

            struct Content{

                static PlayerConfigAPI::Campaign::Area::Content fromJson(QJsonObject json);
                QString getExtension() const;

                QString content_id;
                QString area_id;
                QString campaign_id;
                int play_order;
                QString payment_type;
                QString play_type;
                int play_timeout;
                QDateTime start_timestamp;
                QDateTime end_timestamp;
                QString name;
                QString type;
                int rotate;
                int duration;
                int play_start;
                QString file_url;
                QString file_hash;
                QString file_extension;
                QString fill_mode;

                struct gps
                {
                    double latitude, longitude;
                };
                QHash<QString, QVector<int> > time_targeting;
                QVector<QVector<gps> > geo_targeting;
                QVector<QPolygonF> polygons;

                bool checkTimeTargeting() const;
                bool checkDateRange() const;
                bool checkGeoTargeting(QPointF gps) const;
            };
            QVector<Content> content;
        };
        QVector<Area> areas;
    };
    QVector<Campaign> campaigns;

    //--------------
    QVector<PlayerConfigAPI::Campaign::Area::Content> items();
};

class VideoServiceResponseHandler : public QObject
{
    Q_OBJECT
public:
    explicit VideoServiceResponseHandler(QObject *parent = 0);

signals:
    void initResult(InitRequestResult result);
    void getPlaylistResult(PlayerConfigAPI result);
    void sendStatisticEventsResult(NonQueryResult result);
    void getPlayerSettingsResult(SettingsRequestResult result);
    void getUpdatesResult(UpdateInfoResult result);

public slots:
    void initRequestResultReply(QNetworkReply * reply);
    void getPlaylistResultReply(QNetworkReply * reply);
    void sendStatisticEventsResultReply(QNetworkReply * reply);
    void getPlayerSettingsReply(QNetworkReply * reply);
    void getUpdatesReply(QNetworkReply * reply);
private:
};

#endif // VIDEOSERVICERESULT_H

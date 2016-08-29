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
};

struct PlaylistAPIResult
{
    static PlaylistAPIResult fromJson(QJsonObject json);
    static QHash<QString, PlaylistAPIResult> getAllItems(QJsonArray json);
    QString id;
    QString type;

    struct PlaylistItem
    {
        QString getExtension() const;
        QString id;
        QString name;
        QString fileUrl;
        QString fileHash;
        QString campaignId;
        QString areaId;
        QString virtualScreenId;
        int videoWidth;
        int videoHeight;
        int duration;
        int skipTime;
        QString type;
        int play_order;
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

    struct CampaignItem
    {
        QString id;
        int play_order;
        int play_timeout;
        QString play_type;
        QString areaId;
        QVector<PlaylistItem> content;
    };

    QVector<CampaignItem> items;
};

struct PlayerConfig
{
    static PlayerConfig fromJson(QJsonArray data);
    enum AreaCompositionType {AREA_BROKEN, AREA_FULLSCREEN, AREA_SPLIT, AREA_MULTI};
    AreaCompositionType getType();
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
    VirtualScreen getScreenByType(QString type);
    QHash<QString, VirtualScreen> screens;
};

class VideoServiceResponseHandler : public QObject
{
    Q_OBJECT
public:
    explicit VideoServiceResponseHandler(QObject *parent = 0);

signals:
    void initResult(InitRequestResult result);
    void getPlaylistResult(PlayerConfig result);
    void sendStatisticResult(NonQueryResult result);
    void sendStatisticEventsResult(NonQueryResult result);
    void sendStatisticPlaysResult(NonQueryResult result);
    void getPlayerSettingsResult(SettingsRequestResult result);
    void getPlayerAreasResult(PlayerConfig result);
    void getVirtualScreenPlaylistResult(QHash<QString, PlaylistAPIResult> result);

public slots:
    void initRequestResultReply(QNetworkReply * reply);
    void getPlaylistResultReply(QNetworkReply * reply);
    void sendStatisticResultReply(QNetworkReply * reply);
    void sendStatisticEventsResultReply(QNetworkReply * reply);
    void sendStatisticPlaysResultReply(QNetworkReply * reply);
    void getPlayerSettingsReply(QNetworkReply * reply);
    void getPlayerAreasReply(QNetworkReply * reply);
    void getVirtualScreenPlaylist(QNetworkReply * reply);
private:
};

#endif // VIDEOSERVICERESULT_H

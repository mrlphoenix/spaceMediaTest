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

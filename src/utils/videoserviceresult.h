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
    static QString getRandomString(int length);
    static QString generateSessionKey();
    QString status;
    int error_id;
    QString error_text;

    QString player_id;
    QString public_key;
    QString session_key;
};
struct NonQueryResult
{
    static NonQueryResult fromJson(QJsonObject data);

    QString status;
    int error_id;
    QString error_text;
    QString source;
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
    void enablePlayerResult(QString result);
    void assignPlaylistResult(QString result);
    void getPlaylistResult(PlayerConfig result);
    void sendStatisticResult(NonQueryResult result);

public slots:
    void initRequestResultReply(QNetworkReply * reply);
    void enablePlayerResultReply(QNetworkReply * reply);
    void assignPlaylistResultReply(QNetworkReply * reply);
    void getPlaylistResultReply(QNetworkReply * reply);
    void sendStatisticResultReply(QNetworkReply * reply);
private:
    QString getRandomString(int length);

    QString publicKey;
    QString sessionKey;
};

#endif // VIDEOSERVICERESULT_H

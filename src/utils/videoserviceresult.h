#ifndef VIDEOSERVICERESULT_H
#define VIDEOSERVICERESULT_H

#include <QObject>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDateTime>

struct InitRequestResult
{
    static InitRequestResult fromJson(QJsonObject data);
    QString status;
    int error_id;
    QString error_text;

    QString player_id;
    QString public_key;
};

struct PlayerConfig
{
    static PlayerConfig fromJson(QJsonObject json);
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
                int iid;
                QString itype;
                int position;
                int version;
                QDateTime lastupdate;
                int delay;
                QString dtype;
                bool pstart;
                bool pend;
                QVector<QVector<QString> > ttargeting;
                QVector<QVector<double> > gtargeting;
                QString name;
                QString path;
                QString sha1;
                int size;
                int width;
                int height;
                int duration;
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

public slots:
    void initRequestResultReply(QNetworkReply * reply);
    void enablePlayerResultReply(QNetworkReply * reply);
    void assignPlaylistResultReply(QNetworkReply * reply);
    void getPlaylistResultReply(QNetworkReply * reply);
};

#endif // VIDEOSERVICERESULT_H

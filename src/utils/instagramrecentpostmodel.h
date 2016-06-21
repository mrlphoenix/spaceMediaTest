#ifndef INSTAGRAMRECENTPOSTMODEL_H
#define INSTAGRAMRECENTPOSTMODEL_H

#include <QString>
#include <QVector>
#include <QByteArray>


//outdated class used to display latest instagramm post by tag
//i keep it as i hear we have plan for our own inst
class InstagramRecentPostModel
{
public:
    InstagramRecentPostModel();
    static InstagramRecentPostModel fromJson(QByteArray json);
    QString min_tag_id;
    int code;
    struct Data
    {
        QVector<QString> tags;
        QString type;
        QString postLink;
        struct Image
        {
            QString url;
            int width, height;
        };
        Image low_resolution, thumbnail, standard_resolution;
        QString caption;
        QString authorProfilePictureUrl;
    };
    QVector<Data> data;
};
#endif // INSTAGRAMRECENTPOSTMODEL_H

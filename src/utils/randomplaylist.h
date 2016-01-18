#ifndef RANDOMPLAYLIST_H
#define RANDOMPLAYLIST_H

#include <QObject>
#include <QDateTime>
#include <QHash>
#include <QWidget>
#include <QVector>
#include "videoserviceresult.h"

class RandomPlaylist : public QObject
{
    Q_OBJECT
public:
    explicit RandomPlaylist(QObject *parent = 0);
    void updatePlaylist(PlayerConfig::Area::Playlist playlist);

signals:

public slots:
    QString next();
private:

    void splitItems();
    void shuffle();
    bool itemDelayPassed(const PlayerConfig::Area::Playlist::Item& item);

    PlayerConfig::Area::Playlist playlist;
    QVector<PlayerConfig::Area::Playlist::Item> fixedFloatingItems;
    QVector<PlayerConfig::Area::Playlist::Item> floatingNoneItems;
    QHash<QString,QDateTime> lastTimeShowed;
};

#endif // RANDOMPLAYLIST_H

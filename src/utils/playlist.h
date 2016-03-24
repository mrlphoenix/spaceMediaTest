#ifndef RANDOMPLAYLIST_H
#define RANDOMPLAYLIST_H

#include <QObject>
#include <QDateTime>
#include <QHash>
#include <QWidget>
#include <QVector>
#include "videoserviceresult.h"

#define MAGIC_PLAYLIST_VALUE 4.

class AbstractPlaylist : public QObject
{
    Q_OBJECT
public:
    explicit AbstractPlaylist(QObject * parent);
    virtual ~AbstractPlaylist(){;}
    virtual void updatePlaylist(PlayerConfig::Area::Playlist playlist)=0;
    virtual QString getType()=0;
    virtual PlayerConfig::Area::Playlist::Item findItemById(QString iid);
public slots:
    virtual QString next()=0;
protected:
    PlayerConfig::Area::Playlist playlist;
};

class StandartPlaylist : public AbstractPlaylist
{
    Q_OBJECT
public:
    explicit StandartPlaylist(QObject * parent);
    virtual ~StandartPlaylist(){;}
    virtual void updatePlaylist(PlayerConfig::Area::Playlist playlist);
    virtual QString getType() {return "list";}
public slots:
    virtual QString next();
private:
    int currentItemIndex;
};

class RandomPlaylist : public AbstractPlaylist
{
    Q_OBJECT
public:
    explicit RandomPlaylist(QObject *parent = 0);
    virtual ~RandomPlaylist(){;}
    virtual void updatePlaylist(PlayerConfig::Area::Playlist playlist);

    virtual QString getType() {return "random";}
signals:

public slots:
    virtual QString next();
protected:

    void splitItems();
    void shuffle();
    bool itemDelayPassed(const PlayerConfig::Area::Playlist::Item& item);

    QVector<PlayerConfig::Area::Playlist::Item> fixedFloatingItems;
    QVector<PlayerConfig::Area::Playlist::Item> floatingNoneItems;
    QHash<QString,QDateTime> lastTimeShowed;
};

class MagicRandomPlaylist : public RandomPlaylist
{
    Q_OBJECT
public:
    explicit MagicRandomPlaylist(QObject * parent = 0);
    virtual ~MagicRandomPlaylist(){;}
    virtual void updatePlaylist(PlayerConfig::Area::Playlist playlist);

    virtual QString next();
protected:
    int allLength;
    int magic;
    QDateTime minPlayTime;
};

#endif // RANDOMPLAYLIST_H

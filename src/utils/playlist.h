#ifndef RANDOMPLAYLIST_H
#define RANDOMPLAYLIST_H

#include <QObject>
#include <QDateTime>
#include <QHash>
#include <QWidget>
#include <QVector>
#include "videoserviceresult.h"

#define MAGIC_PLAYLIST_VALUE 4.


//this file is for virtual playlist implementations
//main difference in these playlist is in implementation of next() method
//also updatePlaylist can be vary as we need to prepare lists in different manner
class AbstractPlaylist : public QObject
{
    Q_OBJECT
public:
    explicit AbstractPlaylist(QObject * parent);
    virtual ~AbstractPlaylist(){;}
    virtual void updatePlaylist(PlaylistAPIResult playlist)=0;
    virtual QString getType()=0;
    virtual PlaylistAPIResult::PlaylistItem findItemById(QString iid);
public slots:
    virtual QString next()=0;
protected:
    PlaylistAPIResult playlist;
};

class StandartPlaylist : public AbstractPlaylist
{
    Q_OBJECT
public:
    explicit StandartPlaylist(QObject * parent);
    virtual ~StandartPlaylist(){;}
    virtual void updatePlaylist(PlaylistAPIResult playlist);
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
    virtual void updatePlaylist(PlaylistAPIResult playlist);

    virtual QString getType() {return "random";}
signals:

public slots:
    virtual QString next();
protected:

    void splitItems();
    void shuffle();
    bool itemDelayPassed(const PlaylistAPIResult::PlaylistItem &item);

    QList<PlaylistAPIResult::PlaylistItem> fixedFloatingItems;
    QList<PlaylistAPIResult::PlaylistItem> floatingNoneItems;
    QHash<QString,QDateTime> lastTimeShowed;
};

class MagicRandomPlaylist : public RandomPlaylist
{
    Q_OBJECT
public:
    explicit MagicRandomPlaylist(QObject * parent = 0);
    virtual ~MagicRandomPlaylist(){;}
    virtual void updatePlaylist(PlaylistAPIResult playlist);

    virtual QString next();
protected:
    int allLength;
    int magic;
    QDateTime minPlayTime;
};

#endif // RANDOMPLAYLIST_H

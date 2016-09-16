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
    virtual PlaylistAPIResult::PlaylistItem findItemById(QString iid)=0;
public slots:
    virtual QString next()=0;
    virtual bool haveNext()=0;
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
    virtual bool haveNext(){return true;}
private:
    int currentItemIndex;
};

class SuperPlaylist : public AbstractPlaylist
{
    Q_OBJECT
public:
    explicit SuperPlaylist(QObject * parent);
    virtual ~SuperPlaylist(){;}
    virtual void updatePlaylist(PlaylistAPIResult playlist);
    virtual QString next();
    virtual bool haveNext();
    virtual PlaylistAPIResult::PlaylistItem findItemById(QString iid);
    virtual QString getType() {return "random";}
protected:
    void splitItems();
    void shuffle(bool fixedFloating = true, bool floatingNone = true);
    bool itemDelayPassed(const PlaylistAPIResult::CampaignItem &item);


    int allLength;
    double magic;
    QDateTime minPlayTime;
    QString currentCampaignId;
    int currentItemIndex;
    QString storedNextItem;

    QList<PlaylistAPIResult::CampaignItem> fixedFloatingItems;
    QList<PlaylistAPIResult::CampaignItem> floatingNoneItems;
    QHash<QString,PlaylistAPIResult::CampaignItem> campaigns;
    QHash<QString,QDateTime> lastTimeShowed;
};

#endif // RANDOMPLAYLIST_H

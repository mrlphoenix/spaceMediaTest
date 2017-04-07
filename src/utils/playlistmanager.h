#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include "playlist.h"


struct PlaylistItem
{
    bool found;
    QString areaId;
    PlayerConfigAPI::Campaign::Area::Content content;
    QString getFullPath();
};

class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistManager(QObject *parent = 0);

signals:

public slots:
    PlaylistItem getItem(const QString &areaId, const QString name);
    PlaylistItem nextItem(const QString &areaId);
    bool playlistExists(const QString &areaId);
    void updateConfig(PlayerConfigAPI &playerConfig);
    void resetIndex(const QString &areaId);
    void addForceItem(const QString &areaId, const QString &item);
private:
    QHash<QString, AbstractPlaylist*> playlists;
};

#endif // PLAYLISTMANAGER_H

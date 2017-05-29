#include "playlistmanager.h"
#include "platformdefines.h"
#include <QFileInfo>

PlaylistManager::PlaylistManager(QObject *parent) : QObject(parent)
{

}

PlaylistItem PlaylistManager::getItem(const QString &areaId, const QString name)
{
    PlaylistItem result;
    result.areaId = areaId;
    if (playlists.contains(areaId))
    {
        auto p = static_cast<SuperPlaylist*>( playlists[areaId] );
        result.content = p->findItemById(name);
        result.found = (result.content.content_id != "");
    }
    else
        result.found = false;
    return result;
}

PlaylistItem PlaylistManager::nextItem(const QString &areaId)
{
    PlaylistItem result;
    if (playlists.contains(areaId))
    {
        if (playlists[areaId]->haveNext())
        {
            result.content = playlists[areaId]->findItemById(playlists[areaId]->getStoredItem());
            result.found = true;
        }
    }
    result.found = false;
    result.areaId = areaId;
    return result;
}

bool PlaylistManager::playlistExists(const QString &areaId)
{
    return playlists.contains(areaId);
}

void PlaylistManager::updateConfig(PlayerConfigAPI &playerConfig)
{
    foreach (const QString &key, playlists.keys())
        playlists[key]->deleteLater();
    playlists.clear();
    foreach (const PlayerConfigAPI::Campaign &campaign, playerConfig.campaigns)
    {
        foreach (const PlayerConfigAPI::Campaign::Area area, campaign.areas)
        {
            AbstractPlaylist* playlist = new SuperPlaylist(this);
            playlist->updatePlaylist(area);
            playlists[area.area_id] = playlist;
        }
    }
}

void PlaylistManager::resetIndex(const QString &areaId)
{
    if (playlists.contains(areaId))
        static_cast<SuperPlaylist*>(playlists[areaId])->resetCurrentItemIndex();
}

void PlaylistManager::addForceItem(const QString &areaId, const QString &item)
{
    if (playlists.contains(areaId))
        playlists[areaId]->forceItems.append(item);
}


QString PlaylistItem::getFullPath()
{
    if (content.type == "video" || content.type == "audio" || content.type == "image")
    {
        QString nextFile = VIDEO_FOLDER + content.content_id + content.file_hash + content.file_extension;
        QFileInfo info(nextFile);
        return QUrl::fromLocalFile(info.absoluteFilePath()).toString();
    }
    else if (content.type == "html5_online")
        return content.file_url;
    else if (content.type == "html5_zip")
    {
        QFileInfo info(VIDEO_FOLDER + content.content_id + "/index.html");
        return QUrl::fromLocalFile(info.absoluteFilePath()).toString();
    }
    //unknown content type
    return "";
    /*
     *
QString TeleDSPlayer::getFullPath(QString fileName, AbstractPlaylist * playlist)
{
    QString nextFile =  VIDEO_FOLDER + fileName +
                        playlist->findItemById(fileName).file_hash +
                        playlist->findItemById(fileName).file_extension;
    QFileInfo fileInfo(nextFile);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}

QString TeleDSPlayer::getFullPathZip(QString path)
{
    QFileInfo fileInfo(path);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}

if (item.type == "video" || item.type == "audio" || item.type == "image")
{
    source = QUrl(getFullPath(name,playlists[area_id]));
    invokeSetPlayerVolume(GlobalConfigInstance.getVolume());
}
else if (item.type == "html5_online")
    source = item.file_url;
else if (item.type == "html5_zip")
    source = getFullPathZip(VIDEO_FOLDER + item.content_id + "/index.html");

     * */
}

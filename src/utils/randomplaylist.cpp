#include "randomplaylist.h"

RandomPlaylist::RandomPlaylist(QObject *parent) : AbstractPlaylist(parent)
{

}

void RandomPlaylist::updatePlaylist(PlayerConfig::Area::Playlist playlist)
{
    this->playlist = playlist;
    splitItems();
}

QString RandomPlaylist::next()
{
    qDebug() << "RandomPlaylist:: next";
    shuffle();
    bool found = false;
    QString result;

    foreach(const PlayerConfig::Area::Playlist::Item& item,fixedFloatingItems)
        if (itemDelayPassed(item) && item.checkTimeTargeting() && item.checkDateRange() && item.checkGeoTargeting(QPointF(0.f,0.f)))
        {
            found = true;
            QDateTime delayPassTime = QDateTime::currentDateTime();
            delayPassTime = delayPassTime.addSecs(item.delay);
            lastTimeShowed[item.iid] = delayPassTime;
            result = item.iid;
            break;
        }

    if (!found)
        foreach(const PlayerConfig::Area::Playlist::Item& item,floatingNoneItems)
            if (item.checkTimeTargeting() && item.checkDateRange())
            {
                QDateTime delayPassTime = QDateTime::currentDateTime();
                delayPassTime.addSecs(item.delay);
                lastTimeShowed[item.iid] = delayPassTime;
                result = item.iid;
                break;
            }
    return result;
}

void RandomPlaylist::splitItems()
{
    fixedFloatingItems.clear();
    floatingNoneItems.clear();
    foreach (const PlayerConfig::Area::Playlist::Item& item, playlist.items)
        if (item.dtype == "fixed")
            fixedFloatingItems.append(item);
        else if (item.dtype == "none")
            floatingNoneItems.append(item);
        else
        {
            fixedFloatingItems.append(item);
            floatingNoneItems.append(item);
        }
}

void RandomPlaylist::shuffle()
{
    QVector<PlayerConfig::Area::Playlist::Item> newFixed, newFloating;
    while (fixedFloatingItems.count() > 0)
    {
        int currentItemIndex = qrand()%fixedFloatingItems.count();
        newFixed.append(fixedFloatingItems[currentItemIndex]);
        fixedFloatingItems.removeAt(currentItemIndex);
    }
    while (floatingNoneItems.count() > 0)
    {
        int currentItemIndex = qrand()%floatingNoneItems.count();
        newFloating.append(floatingNoneItems[currentItemIndex]);
        floatingNoneItems.removeAt(currentItemIndex);
    }
    fixedFloatingItems = newFixed;
    floatingNoneItems = newFloating;
}

bool RandomPlaylist::itemDelayPassed(const PlayerConfig::Area::Playlist::Item& item)
{
    if (lastTimeShowed.contains(item.iid))
    {
        if (QDateTime::currentDateTime() > lastTimeShowed[item.iid])
        {
            qDebug() << "RANDOM PL: Item Delay Passed!>>" << item.iid << " [" << item.name << "]";
            return true;
        }
        else
        {
            qDebug() << "RANDOM PL: Item Delay is Not Passed. Skipping item.>>" << item.iid << " [" << item.name << "]";
            return false;
        }
    }
    else
        return true;
}

AbstractPlaylist::AbstractPlaylist(QObject *parent) : QObject(parent)
{

}

StandartPlaylist::StandartPlaylist(QObject *parent) : AbstractPlaylist(parent)
{
    currentItemIndex = 0;
}

void StandartPlaylist::updatePlaylist(PlayerConfig::Area::Playlist playlist)
{
    this->playlist = playlist;
}

QString StandartPlaylist::next()
{
    int itemsCount = playlist.items.count();
    if (currentItemIndex >= itemsCount)
        currentItemIndex = itemsCount-1;
    QString item = playlist.items[currentItemIndex].iid;
    currentItemIndex = (currentItemIndex + 1)%itemsCount;
    return item;
}

#include "globalstats.h"
#include "singleton.h"
#include "playlist.h"

AbstractPlaylist::AbstractPlaylist(QObject *parent) : QObject(parent)
{
}

SuperPlaylist::SuperPlaylist(QObject *parent) : AbstractPlaylist(parent)
{
    allLength = 0;
    magic = 1;
    currentItemIndex = -1;
    lastFreeFloatingItemPlayedIndex = -1;
}

void SuperPlaylist::updatePlaylist(const PlayerConfigAPI::Campaign::Area &playlist)
{
    this->playlist = playlist;
    splitItems();
    allLength = 0;
    //pretend we just played all items
    minPlayTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset());
    //calculating total play time
    //setuping campaing list
    qDebug() << "SuperPlaylist::campaigns clear";
    campaigns.clear();
    foreach (const PlayerConfigAPI::Campaign::Area::Content &item, playlist.content)
    {
        allLength += item.duration;
        campaigns[item.content_id] = item;
    }
    minPlayTime = minPlayTime.addMSecs(-allLength);
    int tempTime = 0;
    auto playlistCopy = playlist.content;
    QList<PlayerConfigAPI::Campaign::Area::Content> shuffledPlaylist;
    while (!playlistCopy.isEmpty())
    {
        int index = qrand()%playlistCopy.count();
        shuffledPlaylist.append(playlistCopy[index]);
        playlistCopy.removeAt(index);
    }
    foreach (const PlayerConfigAPI::Campaign::Area::Content &item, shuffledPlaylist)
    {
        if (!GlobalStatsInstance.itemWasPlayed(item.area_id, item.content_id))
        {
            QDateTime itemFakePlayTime = minPlayTime;
            itemFakePlayTime = itemFakePlayTime.addMSecs(tempTime).addSecs(item.play_timeout);
            GlobalStatsInstance.itemPlayed(item.area_id,item.content_id,itemFakePlayTime);
            tempTime += item.duration;
        }
       /* if (!lastTimeShowed.contains(item.content_id))
        {
            QDateTime itemFakePlayTime = minPlayTime;
            itemFakePlayTime = itemFakePlayTime.addMSecs(tempTime);
            lastTimeShowed[item.content_id] = itemFakePlayTime;
            tempTime += item.duration;
        }*/
    }
    magic = qRound(double(allLength/1000) / double(playlist.content.count()) * MAGIC_PLAYLIST_VALUE);
}

QString SuperPlaylist::next()
{
    qDebug() << "SuperPlaylist::next";
    /*
     * 1. Перемешиваем массив
    2. Считаем общую продолжительность проигрывания всех роликов ($total_video_time)
    3. Эмулируем свойство lastplayed для каждого ролика (только если ролик не разу не проигрывался на устройстве)
        1. $last_play = $time(текущее время) - $total_video_time + $tmptime;
        2. $tmptime += $lenght;
    4. Высчитываем промежуток сортировки ($magic)
        1. $magic = round($total / $count * 4); // общее время проигрывание роликов / колво роликов * 4
    5. Сортируем массив кастомной сортировкой
        1. Сортировка по return (ceil($a_lastplayed/$magic) < ceil($b_lastplayed/$magic)) ? -1 : 1;
        2. Сортировка по return ($a['timeout'] < $b['timeout']) ? -1 : 1; // `Timeout (если ==)
    6. Проигрываем 1й элемент массива - если его играть нельзя, то переходим к проигрыванию бесплатных роликов
     * */
    currentItemIndex = 0;
    shuffle(true, false);
    std::sort(fixedFloatingItems.begin(), fixedFloatingItems.end(),
              [&, this](const PlayerConfigAPI::Campaign::Area::Content &a, const PlayerConfigAPI::Campaign::Area::Content &b)
              {
                  int aLastPlayed = std::ceil(minPlayTime.msecsTo(lastTimeShowed[a.content_id]) / magic);
                  int bLastPlayed = std::ceil(minPlayTime.msecsTo(lastTimeShowed[b.content_id]) / magic);
                  if (aLastPlayed == bLastPlayed)
                      return a.play_timeout > b.play_timeout;
                  else
                      return aLastPlayed > bLastPlayed;
              });
    for (int i = 0; i < fixedFloatingItems.count(); ++i)
    {
        PlayerConfigAPI::Campaign::Area::Content item = fixedFloatingItems.at(i);
        if (itemDelayPassed(item) && item.checkTimeTargeting() && item.checkDateRange() &&
            item.checkGeoTargeting(QPointF(GlobalStatsInstance.getLatitude(), GlobalStatsInstance.getLongitude())))
        {
            QDateTime delayPassTime = QDateTime::currentDateTimeUtc().addSecs(item.play_timeout + GlobalStatsInstance.getUTCOffset() - 7);
            GlobalStatsInstance.itemPlayed(playlist.area_id,item.content_id,delayPassTime);
            //lastTimeShowed[item.content_id] = delayPassTime;
            lastPlayed = item.content_id;
            return item.content_id;
        }
    }

    qDebug() << "SuperPlaylist::cant find proper item with fixed-floating type. Trying to search in floating-none list(" + QString::number(floatingNoneItems.count()) + ")";


    QString freeItemResult = nextFreeItem();
    if (freeItemResult.isEmpty())
    {
        lastPlayed = "";
        return "";
    }
    else
        return freeItemResult;
}

bool SuperPlaylist::haveNext()
{
    storedNextItem = next();
    return storedNextItem != "";
}

PlayerConfigAPI::Campaign::Area::Content SuperPlaylist::findItemById(QString id)
{
    foreach (const PlayerConfigAPI::Campaign::Area::Content &content, campaigns)
        if (content.content_id == id)
            return content;
    PlayerConfigAPI::Campaign::Area::Content emptyItem;
    return emptyItem;
}

void SuperPlaylist::splitItems()
{
    qDebug() << "SuperPlaylist::splitItems";
    fixedFloatingItems.clear();
    floatingNoneItems.clear();
    foreach (const PlayerConfigAPI::Campaign::Area::Content &item, playlist.content)
        if (item.play_type == "normal")
            fixedFloatingItems.append(item);
        else if (item.play_type == "free")
            floatingNoneItems.append(item);
        else
        {
            fixedFloatingItems.append(item);
            floatingNoneItems.append(item);
        }
    qDebug() << "FF: " << fixedFloatingItems.count() << " FN: " << floatingNoneItems.count();
}

void SuperPlaylist::shuffle(bool fixedFloating, bool floatingNone)
{
    //this method is called when we need to shuffle our playlist items
    QList<PlayerConfigAPI::Campaign::Area::Content> newFixed, newFloating;

    if (fixedFloating)
    {
        while (fixedFloatingItems.count() > 0)
        {
            int currentItemIndex = qrand()%fixedFloatingItems.count();
            newFixed.append(fixedFloatingItems[currentItemIndex]);
            fixedFloatingItems.removeAt(currentItemIndex);
        }
        fixedFloatingItems = newFixed;
    }

    if (floatingNone)
    {
        while (floatingNoneItems.count() > 0)
        {
            int currentItemIndex = qrand()%floatingNoneItems.count();
            newFloating.append(floatingNoneItems[currentItemIndex]);
            floatingNoneItems.removeAt(currentItemIndex);
        }
        floatingNoneItems = newFloating;
    }
}

bool SuperPlaylist::itemDelayPassed(const PlayerConfigAPI::Campaign::Area::Content &item)
{
    return GlobalStatsInstance.checkDelayPass(playlist.area_id, item.content_id);
}

QString SuperPlaylist::nextFreeItem()
{
    lastFreeFloatingItemPlayedIndex++;
    if (lastFreeFloatingItemPlayedIndex >= floatingNoneItems.count())
        lastFreeFloatingItemPlayedIndex = 0;

    qDebug() << "SuperPlaylist::nextFreeItem <> currentIndex = " << lastFreeFloatingItemPlayedIndex;
    bool indexReseted = false;
    for (int i = lastFreeFloatingItemPlayedIndex; i< floatingNoneItems.count(); i++)
    {
        auto item = floatingNoneItems[i];
        if (item.checkTimeTargeting() && item.checkDateRange() && (indexReseted ? true : lastPlayed != item.content_id) &&
            item.checkGeoTargeting(QPointF(GlobalStatsInstance.getLatitude(), GlobalStatsInstance.getLongitude())))
        {
            //QDateTime delayPassTime = QDateTime::currentDateTime();
            //delayPassTime = delayPassTime.addMSecs(item.play_timeout);
            //lastTimeShowed[item.content_id] = delayPassTime;
            QDateTime delayPassTime = QDateTime::currentDateTimeUtc().addSecs(item.play_timeout + GlobalStatsInstance.getUTCOffset() - 7);
            GlobalStatsInstance.itemPlayed(playlist.area_id,item.content_id,delayPassTime);

            lastPlayed = item.content_id;
            lastFreeFloatingItemPlayedIndex = i;
            return item.content_id;
        }
        if (i == floatingNoneItems.count() - 1 && !indexReseted)
        {
            i = -1;
            indexReseted = true;
        }
    }
    return "";
}

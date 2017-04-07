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
   // minPlayTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset());
    minPlayTime = QDateTime::currentDateTimeUtc();
    //calculating total play time
    items.clear();
    int maxTimeout = 0;
    foreach (const PlayerConfigAPI::Campaign::Area::Content &item, playlist.content)
    {
        allLength += item.duration;
        items[item.content_id] = item;
        if (item.play_timeout > maxTimeout)
            maxTimeout = item.play_timeout;
        GlobalStatsInstance.setItemPlayTimeout(item.content_id, item.play_timeout);
    }
    minPlayTime = minPlayTime.addMSecs(-allLength);
    minPlayTime = minPlayTime.addSecs(-maxTimeout);
    qDebug() << minPlayTime;
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
            QDateTime itemFakePlayTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset());
            qDebug() << "itemFakePlayTime" << itemFakePlayTime << "| "<< QDateTime::currentDateTimeUtc();
            itemFakePlayTime = itemFakePlayTime.addMSecs(-qrand()%(item.play_timeout*1000+1) - item.play_timeout*400);
            GlobalStatsInstance.itemPlayed(item.area_id,item.content_id,itemFakePlayTime);
            tempTime += item.duration;
        }
    }

    magic = qRound(double(allLength/1000) / double(playlist.content.count()) * MAGIC_PLAYLIST_VALUE);
}

QString SuperPlaylist::next()
{
    qDebug() << "SuperPlaylist::next";
    qDebug() << "forceItemscount = " <<forceItems.count();
    if (forceItems.count())
    {
        QString itemName = forceItems.first();
        forceItems.removeFirst();
        foreach (const QString &k, items.keys())
        {
            auto item = items[k];
            if (item.content_id == itemName)
            {
                if (GlobalStatsInstance.isItemActivated(item.content_id))
                    return item.content_id;
                else
                    return "";
            }
        }
    }
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
    auto realCurrentTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset());
    qDebug() << "start next: " << QDateTime::currentDateTimeUtc().time();
    //shuffle(true, false);
    std::sort(normalFloatingItems.begin(), normalFloatingItems.end(),
              [&, this](const PlayerConfigAPI::Campaign::Area::Content &a, const PlayerConfigAPI::Campaign::Area::Content &b)
              {
                  int aLastPlayed = std::ceil(minPlayTime.secsTo(GlobalStatsInstance.getItemLastPlayDate(a.area_id, a.content_id)) / magic);
                  int bLastPlayed = std::ceil(minPlayTime.secsTo(GlobalStatsInstance.getItemLastPlayDate(b.area_id, b.content_id)) / magic);
                  if (aLastPlayed == bLastPlayed)
                      return a.play_timeout > b.play_timeout;
                  else
                      return aLastPlayed > bLastPlayed;
              });

    //qDebug() << "after sort: " << QDateTime::currentDateTimeUtc().time();
    QPointF currentGps(GlobalStatsInstance.getLatitude(), GlobalStatsInstance.getLongitude());
    for (int i = 0; i < normalFloatingItems.count(); i++)
    {
        PlayerConfigAPI::Campaign::Area::Content item = normalFloatingItems[i];
        if (GlobalStatsInstance.checkDelayPass(playlist.area_id, item.content_id, realCurrentTime) && item.checkTimeTargeting() && item.checkDateRange() &&
            item.checkGeoTargeting(currentGps) && GlobalStatsInstance.isItemActivated(item.content_id))
        {
            qDebug() << "Next Item is " << item.name;
            QDateTime delayPassTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset() - 7);
            GlobalStatsInstance.itemPlayed(playlist.area_id,item.content_id,delayPassTime);

            bool dontPlayItem = false;
            QDateTime d = GlobalStatsInstance.getCampaignEndDate();
            if (d.isValid())
            {
                QDateTime cd = QDateTime::currentDateTimeUtc();
                if (lastPlayed != "")
                    cd = cd.addMSecs(findItemById(lastPlayed).duration);
                cd = cd.addMSecs(item.duration);
                if (cd > d)
                    dontPlayItem = true;

            }
            dontPlayItem = false;
            if (!dontPlayItem)
            {
                lastPlayed = item.content_id;
                return item.content_id;
            }
            else
            {
                qDebug() << "item cannot be played. Returning nothing! ";

                qDebug() << QDateTime::currentDateTime().time();
                return "";
            }
        }
    }
    qDebug() << "SuperPlaylist::cant find proper item with normal/floating type. Trying to search in floating-free list("
                + QString::number(floatingFreeItems.count()) + ")";
    QString freeItemResult = nextFreeItem();
    if (freeItemResult.isEmpty())
    {
        lastPlayed = "";

        qDebug() << QDateTime::currentDateTime().time();
        return "";
    }
    else
    {

        qDebug() << QDateTime::currentDateTime().time();
        return freeItemResult;
    }
}

bool SuperPlaylist::haveNext()
{
    storedNextItem = next();
    return storedNextItem != "";
}

PlayerConfigAPI::Campaign::Area::Content SuperPlaylist::findItemById(QString id)
{
    foreach (const PlayerConfigAPI::Campaign::Area::Content &content, items)
        if (content.content_id == id)
            return content;
    PlayerConfigAPI::Campaign::Area::Content emptyItem;
    return emptyItem;
}

void SuperPlaylist::resetCurrentItemIndex()
{
    qDebug() << this->playlist.area_id;
    currentItemIndex = -1;
    lastFreeFloatingItemPlayedIndex = -1;
    lastPlayed = "";
}

void SuperPlaylist::splitItems()
{
    qDebug() << "SuperPlaylist::splitItems";
    normalFloatingItems.clear();
    floatingFreeItems.clear();
    foreach (const PlayerConfigAPI::Campaign::Area::Content &item, playlist.content)
        if (item.play_type == "normal")
            normalFloatingItems.append(item);
        else if (item.play_type == "free")
            floatingFreeItems.append(item);
        else
        {
            normalFloatingItems.append(item);
            floatingFreeItems.append(item);
        }
    qDebug() << "Normal/Floating: " << normalFloatingItems.count() << " Floating/Free: " << floatingFreeItems.count();
}

void SuperPlaylist::shuffle(bool fixedFloating, bool floatingNone)
{
    //this method is called when we need to shuffle our playlist items
    if (fixedFloating)
        std::random_shuffle(normalFloatingItems.begin(), normalFloatingItems.end());

    if (floatingNone)
        std::random_shuffle(floatingFreeItems.begin(), floatingFreeItems.end());
}

QString SuperPlaylist::nextFreeItem()
{
    lastFreeFloatingItemPlayedIndex++;
    if (lastFreeFloatingItemPlayedIndex >= floatingFreeItems.count())
        lastFreeFloatingItemPlayedIndex = 0;

    qDebug() << "SuperPlaylist::nextFreeItem <> currentIndex = " << lastFreeFloatingItemPlayedIndex;
    bool indexReseted = false;
    QPointF currentGps(GlobalStatsInstance.getLatitude(), GlobalStatsInstance.getLongitude());
    for (int i = lastFreeFloatingItemPlayedIndex; i < floatingFreeItems.count(); i++)
    {
        auto item = floatingFreeItems[i];
        if (GlobalStatsInstance.isItemActivated(item.content_id) && item.checkTimeTargeting() && item.checkDateRange() && (indexReseted ? true : lastPlayed != item.content_id) &&
            item.checkGeoTargeting(currentGps))
        {
            qDebug() << "Next Item is " << item.name;
            QDateTime delayPassTime = QDateTime::currentDateTimeUtc().addSecs(GlobalStatsInstance.getUTCOffset() - 7);
            GlobalStatsInstance.itemPlayed(playlist.area_id,item.content_id,delayPassTime);

            lastFreeFloatingItemPlayedIndex = i;

            bool dontPlayItem = false;
            QDateTime d = GlobalStatsInstance.getCampaignEndDate();
            if (d.isValid())
            {
                QDateTime cd = QDateTime::currentDateTimeUtc();
                if (lastPlayed != "")
                    cd = cd.addMSecs(findItemById(lastPlayed).duration);
                cd = cd.addMSecs(item.duration);
                if (cd > d.addMSecs(5000))
                    dontPlayItem = true;
                qDebug() << d.time() << cd.time();
            }
            dontPlayItem = false;
            if (!dontPlayItem)
            {
                lastPlayed = item.content_id;
                return item.content_id;
            }
            else
            {
                qDebug() << "item cannot be played";
                return "";
            }
            //return item.content_id;
        }
        else
            qDebug() << "SKIPPING FREE ITEM " << item.name << "!!!!!!";
        if (i == floatingFreeItems.count() - 1 && !indexReseted)
        {
            i = -1;
            indexReseted = true;
        }
    }
    return "";
}

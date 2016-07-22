#include "playlist.h"

RandomPlaylist::RandomPlaylist(QObject *parent) : AbstractPlaylist(parent)
{

}

void RandomPlaylist::updatePlaylist(PlaylistAPIResult playlist)
{
    this->playlist = playlist;
    splitItems();
}

QString RandomPlaylist::next()
{
/*    qDebug() << "RandomPlaylist::next";
    shuffle();
    bool found = false;
    QString result;

    //first we need to try to find proper item in fixed | floating types
    //test item for delay, timeRange, DateRange.
    //In future: test for GeoTargeting.
    foreach (const PlaylistAPIResult::PlaylistItem &item, fixedFloatingItems)
        if (itemDelayPassed(item) && item.checkTimeTargeting() && item.checkDateRange())
        {
            found = true;
            QDateTime delayPassTime = QDateTime::currentDateTime();
            delayPassTime = delayPassTime.addSecs(item.play_timeout);
            lastTimeShowed[item.id] = delayPassTime;
            result = item.id;
            break;
        }
    //if we didnt found item in fixed | floating types - we need to find it in floating | none parts of playlist
    //note that we dont test item for delay
    if (!found)
        foreach (const PlaylistAPIResult::PlaylistItem &item, floatingNoneItems)
            if (item.checkTimeTargeting() && item.checkDateRange())
            {
                QDateTime delayPassTime = QDateTime::currentDateTime();
                delayPassTime.addSecs(item.play_timeout);
                lastTimeShowed[item.id] = delayPassTime;
                result = item.id;
                break;
            }
    return result;*/
    return "";
}

void RandomPlaylist::splitItems()
{
  /*  fixedFloatingItems.clear();
    floatingNoneItems.clear();
    foreach (const PlaylistAPIResult::PlaylistItem &item, playlist.items)
        if (item.play_type == "fixed")
            fixedFloatingItems.append(item);
        else if (item.play_type == "none")
            floatingNoneItems.append(item);
        else
        {
            fixedFloatingItems.append(item);
            floatingNoneItems.append(item);
        }*/
}

void RandomPlaylist::shuffle()
{
    //this method is called when we need to shuffle our playlist items
    QList<PlaylistAPIResult::PlaylistItem> newFixed, newFloating;
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

bool RandomPlaylist::itemDelayPassed(const PlaylistAPIResult::PlaylistItem &item)
{
    if (lastTimeShowed.contains(item.id))
    {
        if (QDateTime::currentDateTime() > lastTimeShowed[item.id])
        {
            qDebug() << "RANDOM PL: Item Delay Passed!>>" << item.id << " [" << item.name << "]" <<
                        QDateTime::currentDateTime().time().toString() << " /// " << lastTimeShowed[item.id].time().toString();
            return true;
        }
        else
        {
            qDebug() << "RANDOM PL: Item Delay is Not Passed. Skipping item.>>" << item.id << " [" << item.name << "]";
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

void StandartPlaylist::updatePlaylist(PlaylistAPIResult playlist)
{
    this->playlist = playlist;
}

QString StandartPlaylist::next()
{
    //in standart playlist next() method is simple - we just play items one by one
    int itemsCount = playlist.items.count();
    if (currentItemIndex >= itemsCount)
        currentItemIndex = itemsCount-1;
    qDebug() << currentItemIndex;
    qDebug() << playlist.items.count();
    QString item = playlist.items[currentItemIndex].content.at(0).id;
    currentItemIndex = (currentItemIndex + 1)%itemsCount;
    return item;
}

MagicRandomPlaylist::MagicRandomPlaylist(QObject *parent) : RandomPlaylist(parent)
{
    allLength = 0;
    magic = 1;
}

void MagicRandomPlaylist::updatePlaylist(PlaylistAPIResult playlist)
{
  /*  this->playlist = playlist;
    splitItems();
    allLength = 0;
    //pretend we just played all items
    minPlayTime = QDateTime::currentDateTimeUtc();
    //calculating total play time
    foreach (const PlaylistAPIResult::PlaylistItem &item, playlist.items)
        allLength += item.duration;
    minPlayTime.addSecs(-allLength);
    int tempTime = 0;
    foreach (const PlaylistAPIResult::PlaylistItem &item, playlist.items)
    {
        if (!lastTimeShowed.contains(item.id))
        {
            QDateTime itemFakePlayTime = minPlayTime;
            itemFakePlayTime.addSecs(tempTime);
            lastTimeShowed[item.id] = itemFakePlayTime;
            tempTime += item.duration;
        }
    }
    magic = qRound(double(allLength) / double(playlist.items.count()) * MAGIC_PLAYLIST_VALUE);*/
}

QString MagicRandomPlaylist::next()
{
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

  /*  qDebug() << "RandomPlaylist::next";
    shuffle();
    bool found = false;
    QString result;

    std::sort(fixedFloatingItems.begin(),fixedFloatingItems.end(),
              [&, this](const PlaylistAPIResult::PlaylistItem &a,
                const PlaylistAPIResult::PlaylistItem &b)
        {
            int aLastPlayed = std::ceil(minPlayTime.secsTo(lastTimeShowed[a.id]) / magic );
            int bLastPlayed = std::ceil(minPlayTime.secsTo(lastTimeShowed[b.id]) / magic );
            if (aLastPlayed == bLastPlayed)
                return a.play_timeout < b.play_timeout;
            else
                return aLastPlayed < bLastPlayed;
        });
    for (int i = 0; i < MAGIC_PLAYLIST_VALUE + 1 && i < fixedFloatingItems.count(); ++i)
    {
        PlaylistAPIResult::PlaylistItem item = fixedFloatingItems.at(i);
        if (itemDelayPassed(item) && item.checkTimeTargeting() && item.checkDateRange())
        {
            QDateTime delayPassTime = QDateTime::currentDateTime();
            delayPassTime = delayPassTime.addSecs(item.play_timeout);
            lastTimeShowed[item.id] = delayPassTime;
            result = item.id;
            found = true;
            break;
        }
    }
    qDebug() << "couldnt find proper item with fixed-floating type. Trying to search in floating-none list";
    if (!found)
        foreach(const PlaylistAPIResult::PlaylistItem &item,floatingNoneItems)
            if (item.checkTimeTargeting() && item.checkDateRange())
            {
                QDateTime delayPassTime = QDateTime::currentDateTime();
                delayPassTime = delayPassTime.addSecs(item.play_timeout);
                lastTimeShowed[item.id] = delayPassTime;
                result = item.id;
                break;
            }
    return result;*/
    return "";
}

SuperPlaylist::SuperPlaylist(QObject *parent) : AbstractPlaylist(parent)
{
    allLength = 0;
    magic = 1;
    currentItemIndex = -1;
}

void SuperPlaylist::updatePlaylist(PlaylistAPIResult playlist)
{
    this->playlist = playlist;
    splitItems();
    allLength = 0;
    //pretend we just played all items
    minPlayTime = QDateTime::currentDateTimeUtc();
    //calculating total play time
    //setuping campaing list
    qDebug() << "SuperPlaylist::campaigns clear";
    campaigns.clear();
    foreach (const PlaylistAPIResult::CampaignItem &campaign, playlist.items)
    {
        int campaignDuration = 0;
        foreach (const PlaylistAPIResult::PlaylistItem &item, campaign.content)
            campaignDuration += item.duration;
        allLength += campaignDuration;
        campaigns[campaign.id] = campaign;
    }
    qDebug() << "SuperPlaylist::campaigns restored";
    minPlayTime = minPlayTime.addSecs(-allLength);
    int tempTime = 0;
    foreach (const PlaylistAPIResult::CampaignItem &campaign, playlist.items)
    {
        int campaignDuration = 0;
        foreach (const PlaylistAPIResult::PlaylistItem &item, campaign.content)
            campaignDuration += item.duration;
        if (!lastTimeShowed.contains(campaign.id))
        {
            QDateTime campaignFakePlayTime = minPlayTime;
            campaignFakePlayTime = campaignFakePlayTime.addSecs(tempTime);
            lastTimeShowed[campaign.id] = campaignFakePlayTime;
            tempTime += campaignDuration;
        }
    }
    magic = qRound(double(allLength) / double(playlist.items.count()) * MAGIC_PLAYLIST_VALUE);
}

QString SuperPlaylist::next()
{
    qDebug() << "SuperPlaylist::next";

    currentItemIndex++;
    bool nextCampaign = false;
    bool resetCampaign = !campaigns.contains(currentCampaignId);
    if (!resetCampaign)
        nextCampaign = currentItemIndex >= campaigns[currentCampaignId].content.count();
    else
    {
        qDebug() << "SuperPlaylist::resetCampaignReason-> " << campaigns.count() << " " << currentCampaignId;
    }

    if (resetCampaign)
        currentCampaignId = "";
    qDebug() << "SuperPlaylist::should reset campaign = " << resetCampaign << " &should set next campaign = " << nextCampaign;

    QString itemResult;
    if (!resetCampaign && !nextCampaign)
    {
        qDebug() << "SuperPlaylist::no need to choose next campaign -> going next item with index = " << currentItemIndex;
        PlaylistAPIResult::CampaignItem currentCampaign = campaigns[currentCampaignId];
        while (currentItemIndex < currentCampaign.content.count())
        {
            PlaylistAPIResult::PlaylistItem currentItem = currentCampaign.content[currentItemIndex];
            if (currentItem.checkTimeTargeting() && currentItem.checkDateRange())
            {
                itemResult = currentItem.id;
                break;
            }
            currentItemIndex++;
        }
        if (currentItemIndex >= currentCampaign.content.count())
            nextCampaign = true;
    }

    if (resetCampaign || nextCampaign)
    {
        qDebug() << "SuperPlaylist::need to choose campaign";
        currentItemIndex = 0;
        shuffle();
        bool found = false;
        std::sort(fixedFloatingItems.begin(), fixedFloatingItems.end(),
                  [&, this](const PlaylistAPIResult::CampaignItem &a, const PlaylistAPIResult::CampaignItem &b)
                  {
                      int aLastPlayed = std::ceil(minPlayTime.secsTo(lastTimeShowed[a.id]) / magic);
                      int bLastPlayed = std::ceil(minPlayTime.secsTo(lastTimeShowed[b.id]) / magic);
                      if (aLastPlayed == bLastPlayed)
                          return a.play_timeout < b.play_timeout;
                      else
                          return aLastPlayed < bLastPlayed;
                  });
        for (int i = 0; i < MAGIC_PLAYLIST_VALUE + 1 && i < fixedFloatingItems.count(); ++i)
        {
            PlaylistAPIResult::CampaignItem campaign = fixedFloatingItems.at(i);
            if (itemDelayPassed(campaign))
            {
                int foundItemIndex = 0;
                foreach (const PlaylistAPIResult::PlaylistItem &item, campaign.content)
                {
                    if (item.checkTimeTargeting() && item.checkDateRange())
                    {
                        QDateTime delayPassTime = QDateTime::currentDateTime();
                        delayPassTime = delayPassTime.addSecs(campaign.play_timeout);
                        lastTimeShowed[campaign.id] = delayPassTime;
                        currentCampaignId = campaign.id;
                        itemResult = item.id;
                        currentItemIndex = foundItemIndex;
                        found = true;
                        break;
                    }
                    foundItemIndex++;
                }
                if (found)
                    break;
            }
        }
        if (!found)
        {
            qDebug() << "SuperPlaylist::cant find proper item with fixed-floating type. Trying to search in floating-none list";
            foreach (const PlaylistAPIResult::CampaignItem &campaign, floatingNoneItems)
            {
                bool freeItemFound = false;
                int foundItemIndex = 0;
                foreach (const PlaylistAPIResult::PlaylistItem &item, campaign.content)
                {
                    if (item.checkTimeTargeting() && item.checkDateRange())
                    {
                        QDateTime delayPassTime = QDateTime::currentDateTime();
                        delayPassTime = delayPassTime.addSecs(campaign.play_timeout);
                        lastTimeShowed[campaign.id] = delayPassTime;
                        currentCampaignId = campaign.id;
                        currentItemIndex = foundItemIndex;
                        itemResult = item.id;
                        freeItemFound = true;
                    }
                    if (freeItemFound)
                        break;
                    foundItemIndex++;
                }
            }
        }
    }
    return itemResult;
}

PlaylistAPIResult::PlaylistItem SuperPlaylist::findItemById(QString iid)
{
    foreach (const PlaylistAPIResult::CampaignItem &campaign, campaigns)
        foreach (const PlaylistAPIResult::PlaylistItem &item, campaign.content)
            if (item.id == iid)
                return item;
    PlaylistAPIResult::PlaylistItem emptyItem;
    return emptyItem;
}

void SuperPlaylist::splitItems()
{
    qDebug() << "SuperPlaylist::splitItems";
    fixedFloatingItems.clear();
    floatingNoneItems.clear();
    foreach (const PlaylistAPIResult::CampaignItem &item, playlist.items)
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

void SuperPlaylist::shuffle()
{
    //this method is called when we need to shuffle our playlist items
    QList<PlaylistAPIResult::CampaignItem> newFixed, newFloating;
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

bool SuperPlaylist::itemDelayPassed(const PlaylistAPIResult::CampaignItem &item)
{
    if (lastTimeShowed.contains(item.id))
    {
        if (QDateTime::currentDateTime() > lastTimeShowed[item.id])
        {
            qDebug() << "SUPER PL: Item Delay Passed!>>" << item.id
                     << QDateTime::currentDateTime().time().toString() << " /// " << lastTimeShowed[item.id].time().toString();
            return true;
        }
        else
        {
            qDebug() << "SUPER PL: Item Delay is Not Passed. Skipping item.>>" << item.id;
            return false;
        }
    }
    else
        return true;
}

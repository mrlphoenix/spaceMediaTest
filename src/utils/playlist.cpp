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
    qDebug() << "RandomPlaylist::next";
    shuffle();
    bool found = false;
    QString result;

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
    return result;
}




void RandomPlaylist::splitItems()
{
    fixedFloatingItems.clear();
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
        }
}

void RandomPlaylist::shuffle()
{
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
            qDebug() << "RANDOM PL: Item Delay Passed!>>" << item.id << " [" << item.name << "]";
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

PlaylistAPIResult::PlaylistItem AbstractPlaylist::findItemById(QString iid)
{
    foreach (const PlaylistAPIResult::PlaylistItem &item, playlist.items)
        if (item.id == iid)
            return item;
    PlaylistAPIResult::PlaylistItem emptyItem;
    return emptyItem;
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
    int itemsCount = playlist.items.count();
    if (currentItemIndex >= itemsCount)
        currentItemIndex = itemsCount-1;
    qDebug() << currentItemIndex;
    qDebug() << playlist.items.count();
    QString item = playlist.items[currentItemIndex].id;
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
    this->playlist = playlist;
    splitItems();
    allLength = 0;
    minPlayTime = QDateTime::currentDateTimeUtc();
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
    magic = qRound(double(allLength) / double(playlist.items.count()) * MAGIC_PLAYLIST_VALUE);
}

QString MagicRandomPlaylist::next()
{
    qDebug() << "RandomPlaylist::next";
    shuffle();
    bool found = false;
    QString result;

    std::sort(fixedFloatingItems.begin(),fixedFloatingItems.end(),
              [&, this](const PlaylistAPIResult::PlaylistItem &a,
                const PlaylistAPIResult::PlaylistItem &b)
        {
            int aLastPlayed = minPlayTime.secsTo(lastTimeShowed[a.id]) % magic;
            int bLastPlayed = minPlayTime.secsTo(lastTimeShowed[b.id]) % magic;
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
            delayPassTime.addSecs(item.play_timeout);
            lastTimeShowed[item.id] = delayPassTime;
            result = item.id;
            found = true;
        }
    }
    if (!found)
        foreach(const PlaylistAPIResult::PlaylistItem &item,floatingNoneItems)
            if (item.checkTimeTargeting() && item.checkDateRange())
            {
                QDateTime delayPassTime = QDateTime::currentDateTime();
                delayPassTime.addSecs(item.play_timeout);
                lastTimeShowed[item.id] = delayPassTime;
                result = item.id;
                break;
            }
    return result;

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
        2. Сортировка по return ($a['timeout'] < $b['timeout']) ? -1 : 1; // Timeout (если ==)
    6. Проигрываем 1й элемент массива - если его играть нельзя, то переходим к проигрыванию бесплатных роликов
     * */
}

#include <QFile>
#include <QStringList>
#include <QString>
#include <QElapsedTimer>
#include <QDebug>

#include "subsmanager.h"


SubManager::SubManager(QString filename, QObject * parent): QObject (parent)
{
    currentItemIndex = 0;
    zeroTime = QTime::fromString("00:00:00,000","hh:mm:ss,zzz");
    connect(&startTimer,SIGNAL(timeout()),this,SLOT(subStarted()));
    connect(&endTimer,SIGNAL(timeout()),this,SLOT(subStopped()));
    load(filename);
}

void SubManager::load(QString filename)
{
    currentItemIndex = 0;
    startTimer.stop();
    endTimer.stop();
    items.clear();

    QFile file (filename);
    if (file.exists())
    {
        file.open(QFile::ReadOnly);
        QString data = file.readAll();
        QStringList stringList = data.split("\n");
        for (int i = 0; i< stringList.count(); ++i)
        {
            bool isInt = false;
            stringList[i].toInt(&isInt);
            if (isInt) //found index element
            {
                i++;    //next string = times
                QStringList timeTokens = stringList[i].split(" --> ");
                SubItem item;
                item.timeStart = QTime::fromString(timeTokens[0],"hh:mm:ss,zzz");
                item.timeStart = item.timeStart.addMSecs(-200);
                item.timeEnd   = QTime::fromString(timeTokens[1],"hh:mm:ss,zzz");
                item.timeEnd   = item.timeEnd.addMSecs(-200);
                i++;
                //building string
                QStringList totalStrings;
                while (stringList[i] != "" && i<stringList.count())
                {
                    totalStrings.append(stringList[i]);
                    i++;
                }
                item.string = totalStrings.join("\n");
                items.append(item);
            }
        }
        file.close();
    }
}

SubManager::~SubManager()
{

}

void SubManager::start(int msecs)
{
    currentTime = zeroTime;
    elapsedTimer.start();
    currentTime = currentTime.addMSecs(msecs);
    currentItemIndex = -1;
    for (int i = 0; i<items.count(); ++i)
    {
        currentItemIndex = i;
        if (items.at(i).timeStart > currentTime)
            break;
    }
    if (currentItemIndex != -1)
    {
        startTimer.setInterval(currentTime.msecsTo(items.at(currentItemIndex).timeStart));
        startTimer.start();
        endTimer.setInterval(currentTime.msecsTo(items.at(currentItemIndex).timeStart) +
                             items.at(currentItemIndex).timeStart.msecsTo(items.at(currentItemIndex).timeEnd));
        endTimer.start();
    }
}

void SubManager::subStarted()
{
    emit showText(items[currentItemIndex].string);
    startTimer.stop();
}

void SubManager::subStopped()
{
    currentTime = zeroTime;
    currentTime = currentTime.addMSecs(elapsedTimer.elapsed());
    emit hideText();
    endTimer.stop();
    currentItemIndex++;
    if (currentItemIndex < items.count())
    {
        startTimer.setInterval(currentTime.msecsTo(items.at(currentItemIndex).timeStart));
        endTimer.setInterval(currentTime.msecsTo(items.at(currentItemIndex).timeEnd));
        startTimer.start();
        endTimer.start();
    }
    else
    {
        elapsedTimer.restart();
        startTimer.stop();
        endTimer.stop();
    }
}

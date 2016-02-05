#include <QJsonArray>
#include <soundwidgetinfo.h>
#include "widgetfabric.h"

WidgetFabric::WidgetFabric(QObject *parent) : QObject(parent)
{

}

QWidget *WidgetFabric::create(QJsonObject object, QWidget *parent)
{
    QString widgetType = object["type"].toString();
    QString widgetLeft = object["left"].toString();
    QString widgetTop = object["top"].toString();
    QString widgetWidth = object["width"].toString();
    QString widgetHeight = object["height"].toString();
    QString widgetId = object["id"].toString();

    if (widgetType == "" || widgetLeft == "" || widgetTop == "" || widgetWidth == "" || widgetHeight == "")
        return NULL;
    int widgetLeftValue = getRelativeSize(widgetLeft,parent->width());
    int widgetTopValue  = getRelativeSize(widgetTop, parent->height());
    int widgetWidthValue = getRelativeSize(widgetWidth, parent->width());
    int widgetHeightValue = getRelativeSize(widgetHeight, parent->height());

    if (widgetLeftValue == std::numeric_limits<int>::min() || widgetTopValue  == std::numeric_limits<int>::min() ||
        widgetWidthValue  == std::numeric_limits<int>::min() || widgetHeightValue  == std::numeric_limits<int>::min())
    {
        return NULL;
    }

 /*   if (widgetType.toLower() == "videoplayer")
    {
        VideoPlayer * videoplayer;
        QJsonArray playlist = object["playlist"].toArray();
        QStringList fileNames;
        for (int i=0; i<playlist.size(); ++i)
            fileNames.append(playlist[i].toString());
        videoplayer = new VideoPlayer(fileNames,parent);
        videoplayer->move(widgetLeftValue,widgetTopValue);
        videoplayer->resize(widgetWidthValue, widgetHeightValue);
        videoplayer->setId(widgetId);
        videoplayer->play();
        videoplayer->setSound(0);
        return videoplayer;
    }
    else if (widgetType.toLower() == "yandexweather")
    {
        YandexWeatherWidget * yandexWeather = new YandexWeatherWidget(parent);
        yandexWeather->setId(widgetId);
        yandexWeather->move(widgetLeftValue,widgetTopValue);
        yandexWeather->resize(widgetWidthValue, widgetHeightValue);
        yandexWeather->loadInfo(object["src"].toString());
        yandexWeather->setStyleSheet("YandexWeatherWidget{background-image: url(" + object["background-image"].toString() +");}");
        return yandexWeather;
    }
    else if (widgetType.toLower() == "instagram")
    {
        QString accessToken = object["access-token"].toString();
        QString tag = object["tag"].toString();
        int updateTime = object["update-time"].toInt();
        InstagramRecentPostViewer * instagramWidget = new InstagramRecentPostViewer(accessToken,tag,updateTime,parent);
        instagramWidget->setId(widgetId);
        instagramWidget->move(widgetLeftValue,widgetTopValue);
        instagramWidget->resize(widgetWidthValue, widgetHeightValue);
        return instagramWidget;
    }*/
    return NULL;
}

int WidgetFabric::getRelativeSize(QString value, int parentSize)
{
    bool relativeSize = true;
    if (value.indexOf("%") == -1)
    {
        if (value.indexOf("px") == -1)
            return std::numeric_limits<int>::min();
        else
            relativeSize = false;
    }

    bool parseOk = false;
    int intValue = value.replace("px","").replace("%","").toInt(&parseOk);

    if (!parseOk)
        return std::numeric_limits<int>::min();

    if (relativeSize && parentSize == 0)
        return 0;

    return relativeSize ? intValue * parentSize / 100 : intValue;
}

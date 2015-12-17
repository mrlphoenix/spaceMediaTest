#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QBrush>
#include <QUrl>
#include <QPalette>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <yandexweathermodel.h>
#include "yandexweatherwidget.h"
#include "yandexweatherdaywidget.h"
#include "ui_yandexweatherwidget.h"

YandexWeatherWidget::YandexWeatherWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YandexWeatherWidget)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    connect (manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
}


void YandexWeatherWidget::loadInfo(QString srcUrl)
{
    url = srcUrl;
    manager->get(QNetworkRequest(QUrl(url)));
}

void YandexWeatherWidget::updateInfo()
{
    ui->currentTimeLabel->setText("Данные на: " + data.fact.observation_time.toString("HH:mm"));
    ui->weatherTypeLabel->setText(data.fact.weather_type);
    ui->currentTempLabel->setText(QString::number(data.fact.temperature.value) + "°C");
    ui->SunLabel->setText("Восход: " + data.days[0].sunrise + "   Закат: " + data.days[0].sunset);
    ui->windLabel->setText("Ветер: " + QString::number(data.fact.wind_speed) + "м/c " + windDescription(data.fact.wind_direction));
    ui->HumidityLabel->setText("Влажность: " + QString::number(data.fact.humidity) + "%");
    ui->pressureLabel->setText("Давление: " + QString::number(data.fact.pressure) + "мм рт. ст.");

    if (!dayWidgets.isEmpty())
        foreach (const QWidget* widget, dayWidgets)
            delete widget;
    dayWidgets.clear();
    foreach (const YandexWeatherModel::Day &day, data.days)
    {
        YandexWeatherDayWidget *widget = new YandexWeatherDayWidget(day,this);
        ui->horizontalLayout->addWidget(widget);
        dayWidgets.append(widget);
    }

}

YandexWeatherWidget::~YandexWeatherWidget()
{
    delete ui;
}

void YandexWeatherWidget::on_pushButton_clicked()
{

}

void YandexWeatherWidget::replyFinished(QNetworkReply *reply)
{
    if (reply->error())
        qDebug()<<"ERROR"<<reply->readAll();
    else
    {
        data = YandexWeatherModel::fromXml(reply->readAll());
        updateInfo();
    }
}

QString YandexWeatherWidget::windDescription(QString wind)
{
    if (wind == "w")
        return "западный";
    if (wind == "nw")
        return "северозападный";
    if (wind == "n")
        return "северный";
    if (wind == "ne")
        return "северовосточный";
    if (wind == "e")
        return "восточный";
    if (wind == "se")
        return "юговосточный";
    if (wind == "s")
        return "южный";
    if (wind == "sw")
        return "югозападный";
    return "";
}

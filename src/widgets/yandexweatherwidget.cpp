#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <yandexweathermodel.h>
#include "yandexweatherwidget.h"
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
        data = YandexWeatherModel::fromXml(reply->readAll());
}

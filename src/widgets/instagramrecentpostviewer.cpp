#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QPixmap>

#include "instagramrecentpostviewer.h"
#include "ui_instagramrecentpostviewer.h"
#include <instagramrecentpostmodel.h>

InstagramRecentPostViewer::InstagramRecentPostViewer(QString accessToken, QString tag, int updateTime, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InstagramRecentPostViewer)
{
    ui->setupUi(this);
    this->accessToken = accessToken;
    this->tag = tag;
    manager = new QNetworkAccessManager(this);
    connect (manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
    reloadTimer = new QTimer();
    connect(reloadTimer,SIGNAL(timeout()),this,SLOT(reload()));
    reloadTimer->start(updateTime);
    reload();
}

InstagramRecentPostViewer::~InstagramRecentPostViewer()
{
    delete ui;
    delete manager;
    delete reloadTimer;
}

void InstagramRecentPostViewer::reload()
{
    QNetworkRequest request;
    QUrl url("https://api.instagram.com/v1/tags/" + tag + "/media/recent");
    QUrlQuery query;
    query.addQueryItem("access_token",accessToken);
    url.setQuery(query);
    request.setUrl(url);
    manager->get(request);
    downloadTarget = "update";
}

void InstagramRecentPostViewer::replyFinished(QNetworkReply *reply)
{
    if (reply->error())
        qDebug()<<"ERROR"<<reply->readAll();
    else
    {
        if (downloadTarget == "update")
        {
            InstagramRecentPostModel result = InstagramRecentPostModel::fromJson(reply->readAll());
            if (result.code == 200 && result.data.count() > 0)
            {
                ui->postTextLabel->setText(result.data[0].caption);
                QNetworkRequest request(QUrl(result.data[0].standard_resolution.url));
                manager->get(request);
                downloadTarget = "image";
            }
        }
        else if (downloadTarget == "image")
        {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->width(),ui->imageLabel->height(),Qt::KeepAspectRatio));
        }
    }
}

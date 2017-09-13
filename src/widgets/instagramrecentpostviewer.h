#ifndef INSTAGRAMRECENTPOSTVIEWER_H
#define INSTAGRAMRECENTPOSTVIEWER_H
/*
#include <QPainter>
#include <QStyleOption>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>
#include <QTimer>

namespace Ui {
class InstagramRecentPostViewer;
}

class InstagramRecentPostViewer : public QWidget, public SoundWidgetInfo
{
    Q_OBJECT

public:
    explicit InstagramRecentPostViewer(QString accessToken, QString tag, int updateTime, QWidget *parent = 0);
    ~InstagramRecentPostViewer();
public slots:
    void reload();
    void replyFinished(QNetworkReply* reply);
    virtual void setSound(int){;}

private:
    Ui::InstagramRecentPostViewer *ui;
    QString accessToken;
    QString tag;
    QNetworkAccessManager * manager;
    QTimer * reloadTimer;
    QString downloadTarget;
};
*/
#endif // INSTAGRAMRECENTPOSTVIEWER_H

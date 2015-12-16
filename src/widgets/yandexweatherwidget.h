#ifndef YANDEXWEATHERWIDGET_H
#define YANDEXWEATHERWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <abstractwidget.h>
#include <yandexweathermodel.h>

namespace Ui {
class YandexWeatherWidget;
}

class YandexWeatherWidget : public QWidget, public AbstractWidget
{
    Q_OBJECT

public:
    explicit YandexWeatherWidget(QWidget *parent = 0);
    virtual void setSound(int){;}
    void loadInfo(QString srcUrl);
    ~YandexWeatherWidget();

private slots:
    void on_pushButton_clicked();
    void replyFinished(QNetworkReply* reply);

private:
    Ui::YandexWeatherWidget *ui;
    QString url;
    QNetworkAccessManager * manager;
    YandexWeatherModel data;
};

#endif // YANDEXWEATHERWIDGET_H

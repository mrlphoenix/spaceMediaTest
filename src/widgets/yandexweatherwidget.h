#ifndef YANDEXWEATHERWIDGET_H
#define YANDEXWEATHERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QStyleOption>
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
    void updateInfo();
    ~YandexWeatherWidget();

private slots:
    void on_pushButton_clicked();
    void replyFinished(QNetworkReply* reply);


protected:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
private:
    QString windDescription(QString wind);
    Ui::YandexWeatherWidget *ui;
    QString url;
    QNetworkAccessManager * manager;
    YandexWeatherModel data;
    QVector<QWidget*> dayWidgets;
};

#endif // YANDEXWEATHERWIDGET_H

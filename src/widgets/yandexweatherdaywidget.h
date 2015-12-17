#ifndef YANDEXWEATHERDAYWIDGET_H
#define YANDEXWEATHERDAYWIDGET_H

#include <QWidget>
#include <yandexweathermodel.h>

namespace Ui {
class YandexWeatherDayWidget;
}

class YandexWeatherDayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit YandexWeatherDayWidget(YandexWeatherModel::Day day, QWidget *parent = 0);
    void updateData(YandexWeatherModel::Day day);
    ~YandexWeatherDayWidget();

private:
    Ui::YandexWeatherDayWidget *ui;
};

#endif // YANDEXWEATHERDAYWIDGET_H

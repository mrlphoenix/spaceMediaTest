#include "yandexweatherdaywidget.h"
#include "ui_yandexweatherdaywidget.h"

YandexWeatherDayWidget::YandexWeatherDayWidget(YandexWeatherModel::Day day, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YandexWeatherDayWidget)
{
    ui->setupUi(this);
    updateData(day);
}

void YandexWeatherDayWidget::updateData(YandexWeatherModel::Day day)
{
    ui->weekDayLabel->setText(QDate::shortDayName(day.date.dayOfWeek()));
    ui->dateLabel->setText(QString::number(day.date.day()) + " " + QDate::longMonthName(day.date.month()));

    QString dayTemp, nightTemp, weatherType;
    foreach (const YandexWeatherModel::Day::dayPart &dayPart, day.dayParts)
    {
        if (dayPart.type == "day_short")
        {
            dayTemp = QString::number(dayPart.temperature);
            weatherType = dayPart.weather_type;
        }
        else if (dayPart.type == "night_short")
            nightTemp = QString::number(dayPart.temperature);
    }
    ui->dayTemperatureLabel->setText(dayTemp);
    ui->nightTemperatureLabel->setText(nightTemp);
    ui->weatherTypeWidget->setText(weatherType);
}

YandexWeatherDayWidget::~YandexWeatherDayWidget()
{
    delete ui;
}



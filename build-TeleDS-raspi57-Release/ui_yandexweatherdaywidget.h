/********************************************************************************
** Form generated from reading UI file 'yandexweatherdaywidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_YANDEXWEATHERDAYWIDGET_H
#define UI_YANDEXWEATHERDAYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_YandexWeatherDayWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *weekDayLabel;
    QLabel *dateLabel;
    QLabel *weatherTypeWidget;
    QLabel *dayTemperatureLabel;
    QLabel *nightTemperatureLabel;

    void setupUi(QWidget *YandexWeatherDayWidget)
    {
        if (YandexWeatherDayWidget->objectName().isEmpty())
            YandexWeatherDayWidget->setObjectName(QStringLiteral("YandexWeatherDayWidget"));
        YandexWeatherDayWidget->resize(172, 170);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(YandexWeatherDayWidget->sizePolicy().hasHeightForWidth());
        YandexWeatherDayWidget->setSizePolicy(sizePolicy);
        YandexWeatherDayWidget->setStyleSheet(QStringLiteral("background:transparent;"));
        verticalLayout = new QVBoxLayout(YandexWeatherDayWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, -1, 0, -1);
        weekDayLabel = new QLabel(YandexWeatherDayWidget);
        weekDayLabel->setObjectName(QStringLiteral("weekDayLabel"));
        weekDayLabel->setStyleSheet(QStringLiteral("QLabel { background-color : rgba(230,230,240,180); color : black; }"));
        weekDayLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(weekDayLabel);

        dateLabel = new QLabel(YandexWeatherDayWidget);
        dateLabel->setObjectName(QStringLiteral("dateLabel"));
        dateLabel->setStyleSheet(QStringLiteral("QLabel { background-color : rgba(230,230,240,180); color : black; }"));
        dateLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(dateLabel);

        weatherTypeWidget = new QLabel(YandexWeatherDayWidget);
        weatherTypeWidget->setObjectName(QStringLiteral("weatherTypeWidget"));
        weatherTypeWidget->setStyleSheet(QStringLiteral("QLabel { background-color : rgba(200,200,170,180); color : black; }"));
        weatherTypeWidget->setScaledContents(false);
        weatherTypeWidget->setAlignment(Qt::AlignCenter);
        weatherTypeWidget->setWordWrap(true);

        verticalLayout->addWidget(weatherTypeWidget);

        dayTemperatureLabel = new QLabel(YandexWeatherDayWidget);
        dayTemperatureLabel->setObjectName(QStringLiteral("dayTemperatureLabel"));
        QFont font;
        font.setPointSize(12);
        dayTemperatureLabel->setFont(font);
        dayTemperatureLabel->setStyleSheet(QStringLiteral("QLabel { background-color : rgba(200,200,170,180); color : black; }"));
        dayTemperatureLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(dayTemperatureLabel);

        nightTemperatureLabel = new QLabel(YandexWeatherDayWidget);
        nightTemperatureLabel->setObjectName(QStringLiteral("nightTemperatureLabel"));
        nightTemperatureLabel->setFont(font);
        nightTemperatureLabel->setStyleSheet(QStringLiteral("QLabel { background-color : rgba(200,200,170,180); color : black; }"));
        nightTemperatureLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(nightTemperatureLabel);


        retranslateUi(YandexWeatherDayWidget);

        QMetaObject::connectSlotsByName(YandexWeatherDayWidget);
    } // setupUi

    void retranslateUi(QWidget *YandexWeatherDayWidget)
    {
        YandexWeatherDayWidget->setWindowTitle(QApplication::translate("YandexWeatherDayWidget", "Form", 0));
        weekDayLabel->setText(QApplication::translate("YandexWeatherDayWidget", "WeekDay", 0));
        dateLabel->setText(QApplication::translate("YandexWeatherDayWidget", "Date", 0));
        weatherTypeWidget->setText(QApplication::translate("YandexWeatherDayWidget", "WeatherType", 0));
        dayTemperatureLabel->setText(QApplication::translate("YandexWeatherDayWidget", "DayTemp", 0));
        nightTemperatureLabel->setText(QApplication::translate("YandexWeatherDayWidget", "NightTemp", 0));
    } // retranslateUi

};

namespace Ui {
    class YandexWeatherDayWidget: public Ui_YandexWeatherDayWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YANDEXWEATHERDAYWIDGET_H

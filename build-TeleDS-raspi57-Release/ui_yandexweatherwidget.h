/********************************************************************************
** Form generated from reading UI file 'yandexweatherwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_YANDEXWEATHERWIDGET_H
#define UI_YANDEXWEATHERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_YandexWeatherWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *windLabel;
    QLabel *weatherTypeLabel;
    QLabel *HumidityLabel;
    QLabel *pressureLabel;
    QLabel *currentTempLabel;
    QLabel *SunLabel;
    QHBoxLayout *horizontalLayout;
    QLabel *currentTimeLabel;

    void setupUi(QWidget *YandexWeatherWidget)
    {
        if (YandexWeatherWidget->objectName().isEmpty())
            YandexWeatherWidget->setObjectName(QStringLiteral("YandexWeatherWidget"));
        YandexWeatherWidget->resize(776, 385);
        YandexWeatherWidget->setStyleSheet(QStringLiteral(""));
        gridLayout = new QGridLayout(YandexWeatherWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        windLabel = new QLabel(YandexWeatherWidget);
        windLabel->setObjectName(QStringLiteral("windLabel"));

        gridLayout->addWidget(windLabel, 3, 1, 1, 1);

        weatherTypeLabel = new QLabel(YandexWeatherWidget);
        weatherTypeLabel->setObjectName(QStringLiteral("weatherTypeLabel"));
        QFont font;
        font.setPointSize(12);
        weatherTypeLabel->setFont(font);

        gridLayout->addWidget(weatherTypeLabel, 2, 0, 2, 1);

        HumidityLabel = new QLabel(YandexWeatherWidget);
        HumidityLabel->setObjectName(QStringLiteral("HumidityLabel"));

        gridLayout->addWidget(HumidityLabel, 4, 1, 1, 1);

        pressureLabel = new QLabel(YandexWeatherWidget);
        pressureLabel->setObjectName(QStringLiteral("pressureLabel"));

        gridLayout->addWidget(pressureLabel, 5, 1, 1, 1);

        currentTempLabel = new QLabel(YandexWeatherWidget);
        currentTempLabel->setObjectName(QStringLiteral("currentTempLabel"));
        QFont font1;
        font1.setPointSize(16);
        currentTempLabel->setFont(font1);

        gridLayout->addWidget(currentTempLabel, 4, 0, 2, 1);

        SunLabel = new QLabel(YandexWeatherWidget);
        SunLabel->setObjectName(QStringLiteral("SunLabel"));

        gridLayout->addWidget(SunLabel, 2, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMaximumSize);

        gridLayout->addLayout(horizontalLayout, 6, 0, 1, 2);

        currentTimeLabel = new QLabel(YandexWeatherWidget);
        currentTimeLabel->setObjectName(QStringLiteral("currentTimeLabel"));

        gridLayout->addWidget(currentTimeLabel, 1, 0, 1, 1);


        retranslateUi(YandexWeatherWidget);

        QMetaObject::connectSlotsByName(YandexWeatherWidget);
    } // setupUi

    void retranslateUi(QWidget *YandexWeatherWidget)
    {
        YandexWeatherWidget->setWindowTitle(QApplication::translate("YandexWeatherWidget", "Form", 0));
        windLabel->setText(QString());
        weatherTypeLabel->setText(QString());
        HumidityLabel->setText(QString());
        pressureLabel->setText(QString());
        currentTempLabel->setText(QString());
        SunLabel->setText(QString());
        currentTimeLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class YandexWeatherWidget: public Ui_YandexWeatherWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YANDEXWEATHERWIDGET_H

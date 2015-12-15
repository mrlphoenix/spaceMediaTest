#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <widgetfabric.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QFile jsonFile("data/config.txt");
    jsonFile.open(QFile::ReadOnly);
    QJsonDocument jDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject root = jDoc.object();
    QJsonArray widgets = root["widgets"].toArray();
    foreach (const QJsonValue &value, widgets)
    {
        QWidget * w = WidgetFabric::create(value.toObject(),this);
        w->show();
        this->widgets.append(dynamic_cast<AbstractWidget*>(w));
        widgetsTable[dynamic_cast<AbstractWidget*>(w)->getId()] = dynamic_cast<AbstractWidget*>(w);
    }
    QJsonArray soundProperties = root["sound"].toArray();
    foreach (const QJsonValue &value, soundProperties)
    {
        QJsonObject soundProperty = value.toObject();
        QString id = soundProperty["id"].toString();
        if (widgetsTable.contains(id))
            widgetsTable[id]->setSound(soundProperty["volume"].toInt());
    }

    jsonFile.close();
}

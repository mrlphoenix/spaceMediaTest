#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <widgetfabric.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer::singleShot(1000,this,SLOT(onLoad()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoad()
{
    QFile jsonFile("data/config.txt");
    jsonFile.open(QFile::ReadOnly);
    QJsonDocument jDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject root = jDoc.object();
    QJsonArray widgets = root["widgets"].toArray();
    foreach (const QJsonValue &value, widgets)
    {
        QWidget * w = WidgetFabric::create(value.toObject(),this);
        if (w)
        {
            w->show();
            this->widgets.append(dynamic_cast<SoundWidgetInfo*>(w));
            widgetsTable[dynamic_cast<SoundWidgetInfo*>(w)->getId()] = dynamic_cast<SoundWidgetInfo*>(w);
        }
        else
            qDebug() << "ERROR! Widget of type " + value.toObject()["type"].toString() + " [" +value.toObject()["id"].toString() + "] cant be created";
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

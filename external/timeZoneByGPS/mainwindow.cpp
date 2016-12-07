#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QStringList>
#include <QList>
#include <QMessageBox>
#include <QDebug>
#include <math.h>

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

void MainWindow::on_pushButton_clicked()
{
    QFile f("/usr/share/zoneinfo/zone.tab");
    f.open(QFile::ReadOnly);
    QList<TimeZoneEntry> timezones;
    int foundIndex = 0;
    QStringList lines = QString(f.readAll()).split("\n");
    foreach (const QString &s, lines)
    {
        qDebug() << s;
        if (s.simplified() == "" || s[0] == QChar('#'))
            continue;
        QStringList tokens = s.split("\t");
        TimeZoneEntry newItem;
        newItem.shortName = tokens[0];
        QString coordsString = tokens[1];
        coordsString = coordsString.replace("+", " +").replace("-", " -");
        coordsString = coordsString.mid(1, coordsString.length()-1);
        QString latStr = coordsString.split(" ")[0];
        QString lonStr = coordsString.split(" ")[1];
        newItem.lat = latStr.toDouble()/(latStr.length() > 6 ? 10000. : 100.);
        newItem.lon = lonStr.toDouble()/(lonStr.length() > 6 ? 10000. : 100.);
        newItem.longName = tokens[2];
        timezones.append(newItem);
    }
    double distance = 1000000000.0;
    double lat = ui->lineEdit->text().toDouble();
    double lon = ui->lineEdit_2->text().toDouble();

    int currentIndex = 0;
    foreach (const TimeZoneEntry &tz, timezones)
    {
        double theta = lon - tz.lon;
        double cdistance = sin(lat/180.*M_PI) * sin(tz.lat/180.*M_PI) + cos(lat/180.*M_PI)*cos(tz.lat/180.*M_PI) * cos(theta/180.*M_PI);
        cdistance = acos(cdistance);
        cdistance = fabs(cdistance/M_PI*180.0);
        if (distance > cdistance)
        {
            foundIndex = currentIndex;
            distance = cdistance;
        }
        currentIndex++;
    }
    QMessageBox::information(this, "found", timezones[foundIndex].longName);
    f.close();
}

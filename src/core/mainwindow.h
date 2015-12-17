#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QHash>
#include "iwidgetinfo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onLoad();

private:
    Ui::MainWindow *ui;
    QVector<IWidgetInfo*> widgets;
    QHash<QString,IWidgetInfo*> widgetsTable;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QProcess>
#include <QHash>
#include "soundwidgetinfo.h"
#include "videoservice.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    QString encryptSessionKey();

private slots:
    void onLoad();
    void initPlayer();
    void processError(QProcess::ProcessError error);

    void initResult(InitRequestResult result);
    void playlistResult(PlayerConfig result);
    void getPlaylistTimerSlot();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QVector<SoundWidgetInfo*> widgets;
    QHash<QString,SoundWidgetInfo*> widgetsTable;
    VideoService * videoService;
    QTimer * getPlaylistTimer;

    InitRequestResult playerInitParams;
    QString encryptedSessionKey;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QProcess>
#include <QHash>
#include "soundwidgetinfo.h"
#include "videoservice.h"
#include "videodownloader.h"
#include "rpivideoplayer.h"

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
    void downloaded();

private:
    void setupDownloader(PlayerConfig& config);
    Ui::MainWindow *ui;
    QVector<rpiVideoPlayer*> rpiPlayers;
    QVector<SoundWidgetInfo*> widgets;
    QHash<QString,SoundWidgetInfo*> widgetsTable;
    VideoService * videoService;
    QTimer * getPlaylistTimer;
    VideoDownloader * downloader;

    InitRequestResult playerInitParams;
    QString encryptedSessionKey;
    PlayerConfig currentConfig;
};

#endif // MAINWINDOW_H

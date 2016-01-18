#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QUrl>
#include <widgetfabric.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    videoService = new VideoService("http://api.teleds.com");
    connect(videoService,SIGNAL(initResult(InitRequestResult)),this,SLOT(initResult(InitRequestResult)));
    connect(videoService,SIGNAL(getPlaylistResult(PlayerConfig)),this,SLOT(playlistResult(PlayerConfig)));

    getPlaylistTimer = new QTimer();
    connect (getPlaylistTimer,SIGNAL(timeout()),this,SLOT(getPlaylistTimerSlot()));

    QTimer::singleShot(1000,this,SLOT(initPlayer()));

    downloader = 0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::encryptSessionKey()
{
    QFile pubKey("pubkey.key");
    pubKey.open(QFile::WriteOnly);
    pubKey.write(playerInitParams.public_key.toLocal8Bit());
    pubKey.close();

    QProcess echoProcess, rsaEncodeProcess, base64Process;
    echoProcess.setStandardOutputProcess(&rsaEncodeProcess);
    rsaEncodeProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    rsaEncodeProcess.setStandardOutputProcess(&base64Process);
    base64Process.setStandardOutputFile("base64result.txt");
    //base64Process.setProcessChannelMode(QProcess::ForwardedChannels);
    connect (&echoProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));

    echoProcess.start("cmd.exe",QStringList("/c echo " + playerInitParams.session_key));
    rsaEncodeProcess.start("openssl rsautl -encrypt -inkey pubkey.key -pubin");
    base64Process.start("openssl base64");

    if(!echoProcess.waitForStarted())
        return "";
    bool retval = false;

    while ((retval = base64Process.waitForFinished()));
    QFile base64Encoded("base64result.txt");
    base64Encoded.open(QFile::ReadOnly);
    QString result = base64Encoded.readAll();
    result = result.mid(0,result.length()-1).replace("\n","").replace("\r","");
    base64Encoded.close();
    result = QUrl::toPercentEncoding(result);
    return result;
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

void MainWindow::initPlayer()
{
    videoService->init();
}

void MainWindow::processError(QProcess::ProcessError error)
{
    qDebug() << error;
}

void MainWindow::initResult(InitRequestResult result)
{
    ui->label->setText(result.player_id);
    playerInitParams = result;
    encryptedSessionKey = encryptSessionKey();
    getPlaylistTimer->start(10000);
}

void MainWindow::playlistResult(PlayerConfig result)
{
    ui->label->setText(playerInitParams.player_id + " : " + QString::number(result.error) + " [" + result.error_text + "]");
    if (result.areas.count() > 0)
    {
        getPlaylistTimer->stop();
        setupDownloader(result);
        currentConfig = result;
    }
}

void MainWindow::getPlaylistTimerSlot()
{
    videoService->getPlaylist(playerInitParams.player_id,encryptedSessionKey);
}

void MainWindow::on_pushButton_clicked()
{
    videoService->enablePlayer(playerInitParams.player_id);
    videoService->assignPlaylist(playerInitParams.player_id,10);
}

void MainWindow::downloaded()
{
    ui->label->hide();
    ui->totalPB->hide();
    ui->currentPB->hide();
    ui->download_status->hide();

    getPlaylistTimer->start(30000);
    if (rpiPlayers.count() == 0)
    {
        rpiVideoPlayer *player = new rpiVideoPlayer(this);
        player->setConfig(currentConfig.areas[0]);
        player->resize(this->width(),this->height());
        player->play();
        player->show();
        rpiPlayers.append(player);
    }
    else
    {
        rpiPlayers[0]->update(currentConfig);
    }
}

void MainWindow::setupDownloader(PlayerConfig &config)
{
    if (downloader)
    {
        downloader->disconnect();
        delete downloader;
    }
    downloader = new VideoDownloader(config,this);
    currentConfig = config;
    connect(downloader,SIGNAL(done()),this,SLOT(downloaded()));
    ui->totalPB->show();
    ui->currentPB->show();
    ui->download_status->show();
    downloader->setOutput(ui->totalPB,ui->currentPB,ui->download_status);
    downloader->checkDownload();
    downloader->start();
}


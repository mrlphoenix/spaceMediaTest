#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Audio.h>

#include "iptvwidget.h"
#include "ui_iptvwidget.h"

IPTVWidget::IPTVWidget(QString src, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IPTVWidget),
    media(0)
{
    ui->setupUi(this);

    instance = new VlcInstance(VlcCommon::args(), this);
    player = new VlcMediaPlayer(instance);
    player->setVideoWidget(ui->video);
    player->audio()->setVolume(0);
    media = new VlcMedia(src, instance);
    player->open(media);
}

void IPTVWidget::setSound(int volume)
{
    player->audio()->setVolume(volume);
}

IPTVWidget::~IPTVWidget()
{
    delete ui;
}

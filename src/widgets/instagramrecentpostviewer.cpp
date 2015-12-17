#include "instagramrecentpostviewer.h"
#include "ui_instagramrecentpostviewer.h"

InstagramRecentPostViewer::InstagramRecentPostViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InstagramRecentPostViewer)
{
    ui->setupUi(this);
}

InstagramRecentPostViewer::~InstagramRecentPostViewer()
{
    delete ui;
}

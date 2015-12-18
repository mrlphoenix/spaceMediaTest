#ifndef IPTVWIDGET_H
#define IPTVWIDGET_H

#include <QWidget>
#include <soundwidgetinfo.h>

namespace Ui {
class IPTVWidget;
}
class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class IPTVWidget : public QWidget, public SoundWidgetInfo
{
    Q_OBJECT

public:
    explicit IPTVWidget(QString src, QWidget *parent = 0);
    virtual void setSound(int volume);
    ~IPTVWidget();

private:
    Ui::IPTVWidget *ui;

    VlcInstance *instance;
    VlcMedia *media;
    VlcMediaPlayer *player;
};

#endif // IPTVWIDGET_H

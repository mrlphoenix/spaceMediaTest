#ifndef INSTAGRAMRECENTPOSTVIEWER_H
#define INSTAGRAMRECENTPOSTVIEWER_H

#include <QWidget>

namespace Ui {
class InstagramRecentPostViewer;
}

class InstagramRecentPostViewer : public QWidget
{
    Q_OBJECT

public:
    explicit InstagramRecentPostViewer(QWidget *parent = 0);
    ~InstagramRecentPostViewer();

private:
    Ui::InstagramRecentPostViewer *ui;
};

#endif // INSTAGRAMRECENTPOSTVIEWER_H

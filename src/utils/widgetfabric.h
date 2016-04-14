#ifndef WIDGETFABRIC_H
#define WIDGETFABRIC_H

#include <QObject>
#include <QWidget>
#include <QJsonObject>



class WidgetFabric : public QObject
{
    Q_OBJECT
public:
    explicit WidgetFabric(QObject *parent = 0);
    static QWidget * create(QJsonObject object, QWidget *parent);

signals:

public slots:
private:
    static int getRelativeSize(QString value, int parentSize);
};




/*
 * sprites/
 * text/
 * config/
 * maps/
 * sound/
 * other/
 * */
//sprites.nfs //13764133
//text.nfs    //13755
//config.nfs  //3100
//maps.nfs    //178900
//sound.nfs   //75230140
//other.nfs   //240


#endif // WIDGETFABRIC_H

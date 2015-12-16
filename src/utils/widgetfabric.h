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

#endif // WIDGETFABRIC_H

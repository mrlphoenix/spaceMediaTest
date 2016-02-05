#ifndef ANDROIDPLAYERTEST_H
#define ANDROIDPLAYERTEST_H

#include <QObject>
#include <QGuiApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QUrl>
#include <QTimer>
#include <QList>
#include <QDebug>

class AndroidPlayerTest : public QObject
{
    Q_OBJECT
public:
    explicit AndroidPlayerTest(QObject * parent);
    ~AndroidPlayerTest();
    QString getFullPath(QString fileName);
public slots:
    void invokeNextVideoMethod(QString name);
    void next();
    void bindObjects();
protected:
    QQuickView view;
    QObject * viewRootObject;

};
#endif // ANDROIDPLAYERTEST_H

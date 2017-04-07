#include <QGuiApplication>
#include <QByteArray>
#include <QQmlApplicationEngine>
#include <QProcess>
#include <QFile>
#include <teledscore.h>
#include <QDir>
#include <QDebug>
#include <stdio.h>
#include <linux/input.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <QHash>
#include <QString>
#include "sslencoder.h"


int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication app(argc, argv);
    QDir().setCurrent(qApp->applicationDirPath());
    app.setOverrideCursor( QCursor( Qt::BlankCursor ) );
    //QApplication a(argc, argv);
    //MainWindow w;
    TeleDSCore core;

  /*  int ff = open("/dev/input/event0", O_RDONLY);
    input_event event;
    qDebug() << ff;
    while (1)
    {
        read(ff,&event,sizeof(input_event));
        if (event.value < 2 && event.type == 1)
            qDebug() << event.code << event.value;
    }*/



    //w.showFullScreen();

 /*   QQuickView view;
    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty(QStringLiteral("initialUrl"), "http://ya.ru");
    engine.load(QUrl(QStringLiteral("qrc:/browser.qml")));

    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);

   // context->setContextProperty(QStringLiteral("initialUrl"), "http://ya.ru");
    view.setSource(QUrl(QStringLiteral("qrc:/browser.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.showFullScreen();
    */


    return app.exec();
}

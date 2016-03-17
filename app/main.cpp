#include <QGuiApplication>
#include <QByteArray>
#include <QProcess>
#include <QFile>
#include <teledscore.h>
#include "sslencoder.h"


int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication app(argc, argv);
  //  app.setOverrideCursor( QCursor( Qt::BlankCursor ) );
    //QApplication a(argc, argv);
    //MainWindow w;
    TeleDSCore core;
    //w.showFullScreen();


    return app.exec();
}

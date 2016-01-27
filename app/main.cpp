#include <QApplication>
#include <QGuiApplication>
#include <mainwindow.h>
#include <teledscore.h>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication app(argc, argv);
    //QApplication a(argc, argv);
   // MainWindow w;
    TeleDSCore core;
    //w.showFullScreen();

    return app.exec();
}

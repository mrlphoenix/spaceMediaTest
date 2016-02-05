#include <QApplication>
#include <QGuiApplication>
#include <QQuickView>
#include <QSurfaceFormat>
#include <QQuickItem>
#include "teledscore.h"
int main(int argc, char *argv[])
{
  /*  QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication app(argc, argv);
    TeleDSCore core;
    //w.showFullScreen();
*/
  //  QApplication app(argc, argv);
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
        QGuiApplication app(argc, argv);

    TeleDSCore core;
 /*   QQuickView view;
    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);

//    QObject *viewRootObject = dynamic_cast<QObject*>(view.rootObject());
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    view.showFullScreen();*/

    return app.exec();
}

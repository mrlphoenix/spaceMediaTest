#include "androidplayertest.h"

#include <QFileInfo>
#include "androidplayertest.h"
#include "statisticdatabase.h"

AndroidPlayerTest::AndroidPlayerTest(QObject *parent) : QObject(parent)
{
    QSurfaceFormat curSurface = view.format();
    curSurface.setRedBufferSize(8);
    curSurface.setGreenBufferSize(8);
    curSurface.setBlueBufferSize(8);
    curSurface.setAlphaBufferSize(0);
    view.setFormat(curSurface);

    view.setSource(QUrl(QStringLiteral("qrc:/simple_player.qml")));
    viewRootObject = dynamic_cast<QObject*>(view.rootObject());
    if (viewRootObject)
        qDebug() <<"Root object is ok";
    else
        qDebug() <<"BULLSHIT";
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QTimer::singleShot(1000,this,SLOT(bindObjects()));
    QTimer::singleShot(1000,this,SLOT(next()));
    view.showFullScreen();
}

AndroidPlayerTest::~AndroidPlayerTest()
{

}

QString AndroidPlayerTest::getFullPath(QString fileName)
{
    QString nextFile = "data/video/" + fileName + ".mp4";
    QFileInfo fileInfo(nextFile);
    return QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
}


void AndroidPlayerTest::invokeNextVideoMethod(QString name)
{
    qDebug() << "invoking next";
    QVariant source = QUrl("file:///sdcard/download/teleds/1.mp4");
    qDebug() << source;
    QMetaObject::invokeMethod(viewRootObject,"playFile",Q_ARG(QVariant,source));
}

void AndroidPlayerTest::next()
{
    invokeNextVideoMethod("file:///sdcard/download/teleds/1.mp4");

    qDebug() << "inserting into database PLAY";
}

void AndroidPlayerTest::bindObjects()
{
    qDebug() << "bind Next Item";
    QObject::connect(viewRootObject,SIGNAL(nextItem()),this, SLOT(next()));
    qApp->connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
}


#ifndef SUBSMANAGER_H
#define SUBSMANAGER_H


#include <QObject>
#include <QString>
#include <QTime>
#include <QFile>
#include <QTimer>
#include <QElapsedTimer>

struct SubItem
{
    QTime timeStart, timeEnd;
    QString string;
};

class SubManager : public QObject
{
    Q_OBJECT
public:
    SubManager(QString filename, QObject * parent);
    void load(QString filename);
    ~SubManager();
    void start(int msecs = 0);


    QElapsedTimer elapsedTimer;
signals:
    void showText(QString text);
    void hideText();
public slots:
    void subStarted();
    void subStopped();
private:
    QList<SubItem> items;
    QTime currentTime;
    int currentItemIndex;
    QTimer startTimer;
    QTimer endTimer;
    QTime zeroTime;
};

#endif // SUBSMANAGER_H
